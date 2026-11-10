param([string]$Root = (Join-Path $PSScriptRoot '..'))

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath($Root).TrimEnd('\', '/')
$problems = [Collections.Generic.List[string]]::new()

function Check([bool]$Condition, [string]$Message) {
    if (-not $Condition) { $problems.Add($Message) }
}

function Read-Json([string]$Relative) {
    $path = Join-Path $repoRoot $Relative
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing file: $Relative" }
    return Get-Content -LiteralPath $path -Raw -Encoding UTF8 | ConvertFrom-Json
}

function Unique-Ids($Items, [string]$Label) {
    $seen = @{}
    foreach ($item in $Items) {
        Check (-not $seen.ContainsKey($item.id)) "$Label duplicate id: $($item.id)"
        $seen[$item.id] = $true
    }
}

try {
    $plan = Read-Json 'docs/plan/tasks.json'
    $state = Read-Json 'docs/workflow/STATE.json'
    $assets = Read-Json 'content/assets.json'
    $approvals = Read-Json 'content/approvals.json'
    $texts = Read-Json 'content/texts.json'
    foreach ($record in @($plan, $state, $assets, $approvals, $texts)) {
        Check ($record.schema_version -eq 1) 'Unsupported schema_version'
    }
    Unique-Ids $plan.tasks 'Task'
    Unique-Ids $assets.assets 'Asset'
    Unique-Ids $approvals.approvals 'Approval'
    Unique-Ids $texts.texts 'Text'

    $known = @{}
    foreach ($task in $plan.tasks) {
        Check ($task.id -cmatch '^P[0-9]{3}(?:-[0-9]{2})?$') "Invalid task id: $($task.id)"
        foreach ($field in @('title', 'scope', 'acceptance', 'phase')) {
            Check (-not [string]::IsNullOrWhiteSpace($task.$field)) "Empty task $field on $($task.id)"
        }
        foreach ($dependency in $task.depends_on) {
            Check ($known.ContainsKey($dependency)) "Task $($task.id) has unknown or non-topological dependency $dependency"
        }
        $known[$task.id] = $task
    }
    $done = @{}
    foreach ($id in $state.completed) {
        Check ($known.ContainsKey($id)) "Unknown completed task: $id"
        Check (-not $done.ContainsKey($id)) "Duplicate completed task: $id"
        $done[$id] = $true
        Check (Test-Path -LiteralPath (Join-Path $repoRoot "docs/workflow/history/$id.md")) "Missing report for completed task: $id"
    }
    foreach ($id in $state.completed) {
        if ($known.ContainsKey($id)) {
            foreach ($dependency in $known[$id].depends_on) {
                Check ($done.ContainsKey($dependency)) "Completed task $id has unfinished dependency $dependency"
            }
        }
    }
    Check ($known.ContainsKey($state.current_task)) 'Unknown current_task'
    Check (-not $done.ContainsKey($state.current_task)) 'current_task is already completed'
    if ($known.ContainsKey($state.current_task)) {
        foreach ($dependency in $known[$state.current_task].depends_on) {
            Check ($done.ContainsKey($dependency)) "Current task has unfinished dependency $dependency"
        }
    }
    foreach ($block in $state.blocked) {
        Check ($known.ContainsKey($block.task_id)) 'Unknown blocked task'
        Check (-not $done.ContainsKey($block.task_id)) 'Blocked task is completed'
        Check (-not [string]::IsNullOrWhiteSpace($block.reason)) 'Blocked task requires reason'
        Check (-not [string]::IsNullOrWhiteSpace($block.unblock_when)) 'Blocked task requires unblock_when'
    }
    Check (Test-Path -LiteralPath (Join-Path $repoRoot $state.last_report)) 'Missing last_report'
    $next = Get-Content -LiteralPath (Join-Path $repoRoot 'docs/workflow/NEXT.md') -Raw -Encoding UTF8
    $match = [regex]::Match($next, '(?m)^Task: (P[0-9]{3}(?:-[0-9]{2})?)\s*$')
    Check ($match.Success -and $match.Groups[1].Value -eq $state.current_task) 'NEXT Task differs from STATE current_task'

    $approvalById = @{}
    foreach ($approval in $approvals.approvals) {
        Check ($approval.status -in @('approved', 'pending', 'denied')) 'Invalid approval status'
        if ($approval.status -eq 'approved') {
            foreach ($field in @('approved_at', 'evidence', 'scope', 'source_url', 'source_version')) {
                Check (-not [string]::IsNullOrWhiteSpace($approval.$field)) "Approved permission missing $field"
            }
        }
        $approvalById[$approval.id] = $approval
    }
    $assetById = @{}
    $assetPaths = @{}
    foreach ($asset in $assets.assets) {
        $assetById[$asset.id] = $asset
        foreach ($field in @('author', 'source_url', 'source_version', 'license', 'license_evidence', 'non_ai_evidence')) {
            Check (-not [string]::IsNullOrWhiteSpace($asset.$field)) "Asset $($asset.id) missing $field"
        }
        Check ($asset.status -eq 'approved') "Asset not approved: $($asset.id)"
        Check ($asset.sha256 -match '^[0-9a-fA-F]{64}$') "Invalid hash: $($asset.id)"
        $path = [IO.Path]::GetFullPath((Join-Path $repoRoot $asset.path))
        $approvedRoot = [IO.Path]::GetFullPath((Join-Path $repoRoot 'assets/approved')) + [IO.Path]::DirectorySeparatorChar
        $inside = $path.StartsWith($approvedRoot, [StringComparison]::OrdinalIgnoreCase)
        Check $inside "Asset outside assets/approved: $($asset.id)"
        Check (-not $assetPaths.ContainsKey($path)) "Duplicate asset path: $path"
        $assetPaths[$path] = $true
        if ($inside -and (Test-Path -LiteralPath $path -PathType Leaf)) {
            Check ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -eq $asset.sha256) "Hash mismatch: $($asset.id)"
            Check ((Get-Item -LiteralPath $path).Length -eq $asset.bytes) "Size mismatch: $($asset.id)"
        } else { Check $false "Missing asset: $($asset.id)" }
        if ($approvalById.ContainsKey($asset.approval_id)) {
            $permission = $approvalById[$asset.approval_id]
            Check ($permission.status -eq 'approved') "Asset has no granted permission: $($asset.id)"
            Check ($permission.source_url -eq $asset.source_url -and $permission.source_version -eq $asset.source_version) "Permission scope mismatch: $($asset.id)"
        } else { Check $false "Missing approval: $($asset.id)" }
    }
    $approvedDir = Join-Path $repoRoot 'assets/approved'
    if (Test-Path -LiteralPath $approvedDir) {
        foreach ($file in Get-ChildItem -LiteralPath $approvedDir -Recurse -File) {
            Check ($assetPaths.ContainsKey($file.FullName)) "Unregistered asset: $($file.FullName)"
        }
    }
    foreach ($entry in $texts.texts) {
        Check (-not [string]::IsNullOrWhiteSpace($entry.value)) "Empty text: $($entry.id)"
        if ($entry.status -eq 'ai_placeholder') {
            Check ($entry.value.StartsWith('[AI PLACEHOLDER]', [StringComparison]::Ordinal)) "Missing placeholder prefix: $($entry.id)"
            Check ($null -eq $entry.source_asset_id) "Placeholder must not claim approved source: $($entry.id)"
        } elseif ($entry.status -eq 'approved') {
            Check (-not $entry.value.Contains('[AI PLACEHOLDER]')) "Approved text contains placeholder: $($entry.id)"
            Check ($null -ne $entry.source_asset_id -and $assetById.ContainsKey($entry.source_asset_id)) "Text missing approved source: $($entry.id)"
        } else { Check $false "Unknown text status: $($entry.id)" }
    }

    $markdown = @(Get-ChildItem -LiteralPath $repoRoot -File -Filter '*.md')
    foreach ($dir in @('docs', '.agents', 'content')) {
        $dirPath = Join-Path $repoRoot $dir
        if (Test-Path -LiteralPath $dirPath) { $markdown += @(Get-ChildItem -LiteralPath $dirPath -Recurse -File -Filter '*.md') }
    }
    foreach ($file in $markdown) {
        $body = Get-Content -LiteralPath $file.FullName -Raw -Encoding UTF8
        foreach ($link in [regex]::Matches($body, '\[[^\]\r\n]+\]\(([^)\r\n]+)\)')) {
            $target = $link.Groups[1].Value
            if ($target -match '^(https?://|mailto:|#)') { continue }
            $target = ($target -split '#', 2)[0]
            $targetPath = Join-Path $file.DirectoryName $target
            Check (Test-Path -LiteralPath $targetPath) "Broken link in $($file.Name): $target"
        }
    }
    foreach ($skill in Get-ChildItem -LiteralPath (Join-Path $repoRoot '.agents/skills') -Directory) {
        $body = Get-Content -LiteralPath (Join-Path $skill.FullName 'SKILL.md') -Raw -Encoding UTF8
        Check ($body -match '(?s)^---\r?\nname: [a-z0-9-]+\r?\ndescription: [^\r\n]+\r?\n---') "Invalid skill frontmatter: $($skill.Name)"
    }
} catch { $problems.Add($_.Exception.Message) }

if ($problems.Count -gt 0) {
    foreach ($problem in $problems) { Write-Output "ERROR: $problem" }
    exit 1
}
Write-Output "PASS: task graph, handoff, local links, skills and content registries ($($plan.tasks.Count) tasks)."
Write-Output 'This validates records, not human authorship, runtime content or Steam acceptance.'
exit 0
