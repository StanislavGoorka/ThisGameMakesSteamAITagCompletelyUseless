$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$localRoot = [IO.Path]::GetFullPath((Join-Path $repoRoot '.local'))
$scratch = Join-Path $localRoot ('plan-tests-' + [Guid]::NewGuid().ToString('N'))
$validator = Join-Path $PSScriptRoot 'Validate-Project.ps1'
$utf8 = [Text.UTF8Encoding]::new($false)

function Write-Json($Value, [string]$Path) {
    [IO.File]::WriteAllText($Path, ($Value | ConvertTo-Json -Depth 30), $utf8)
}

function Seed-TestAsset([string]$Fixture) {
    $relative = 'assets/approved/fixture.dat'
    $path = Join-Path $Fixture $relative
    New-Item -ItemType Directory -Path (Split-Path $path -Parent) -Force | Out-Null
    [IO.File]::WriteAllText($path, 'synthetic non-player test data', $utf8)
    $record = @{id = 'fixture'; path = $relative; sha256 = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash;
        bytes = (Get-Item -LiteralPath $path).Length; author = 'synthetic fixture'; source_url = 'https://example.invalid/fixture';
        source_version = 'test'; license = 'test-only'; license_evidence = 'synthetic test, not a real permission';
        non_ai_evidence = 'synthetic test, not an authorship claim'; approval_id = 'fixture-permission'; status = 'approved'}
    $permission = @{id = 'fixture-permission'; source_url = $record.source_url; source_version = 'test';
        status = 'approved'; approved_at = '2026-11-10'; evidence = 'synthetic test only, not a user approval'; scope = 'isolated validator fixture'}
    $assets = Get-Content -Raw -Encoding UTF8 (Join-Path $Fixture 'content/assets.json') | ConvertFrom-Json
    $approvals = Get-Content -Raw -Encoding UTF8 (Join-Path $Fixture 'content/approvals.json') | ConvertFrom-Json
    $assets.assets = @($assets.assets) + @($record)
    $approvals.approvals = @($approvals.approvals) + @($permission)
    Write-Json $assets (Join-Path $Fixture 'content/assets.json')
    Write-Json $approvals (Join-Path $Fixture 'content/approvals.json')
}

function Run-Case([string]$Name, [scriptblock]$Change, [bool]$ShouldPass, [string]$Expected = '') {
    $fixture = Join-Path $scratch $Name
    New-Item -ItemType Directory -Path $fixture -Force | Out-Null
    foreach ($relative in @('AGENTS.md', 'README.md', 'docs', 'content', '.agents', '.github', 'src', 'tests', 'tools', 'CMakeLists.txt')) {
        Copy-Item -LiteralPath (Join-Path $repoRoot $relative) -Destination $fixture -Recurse
    }
    if (Test-Path -LiteralPath (Join-Path $repoRoot 'assets')) {
        Copy-Item -LiteralPath (Join-Path $repoRoot 'assets') -Destination $fixture -Recurse
    }
    & $Change $fixture
    $result = & powershell -NoProfile -ExecutionPolicy Bypass -File $validator -Root $fixture
    $passed = $LASTEXITCODE -eq 0
    $output = $result -join "`n"
    if ($passed -ne $ShouldPass -or ($Expected -and -not $output.Contains($Expected))) {
        throw "Unexpected result for $Name : $output"
    }
    Write-Output "PASS: $Name"
}

try {
    New-Item -ItemType Directory -Path $scratch -Force | Out-Null
    Run-Case 'valid-records' { param($dir) } $true
    Run-Case 'future-dependency' {
        param($dir)
        $path = Join-Path $dir 'docs/plan/tasks.json'
        $data = Get-Content -LiteralPath $path -Raw -Encoding UTF8 | ConvertFrom-Json
        $data.tasks[1].depends_on = @('P002')
        Write-Json $data $path
    } $false 'non-topological dependency'
    Run-Case 'handoff-mismatch' {
        param($dir)
        $path = Join-Path $dir 'docs/workflow/NEXT.md'
        $data = Get-Content -LiteralPath $path -Raw -Encoding UTF8
        [IO.File]::WriteAllText($path, ($data -replace 'Task: [A-Z0-9-]+', 'Task: P000'), $utf8)
    } $false 'NEXT Task differs'
    Run-Case 'unfinished-predecessor' {
        param($dir)
        $path = Join-Path $dir 'docs/workflow/STATE.json'
        $data = Get-Content -LiteralPath $path -Raw -Encoding UTF8 | ConvertFrom-Json
        $data.current_task = 'P003'
        $data.completed = @('P000', 'P001')
        Write-Json $data $path
    } $false 'unfinished dependency'
    Run-Case 'orphan-file' {
        param($dir)
        $path = Join-Path $dir 'assets/approved'
        New-Item -ItemType Directory -Path $path -Force | Out-Null
        [IO.File]::WriteAllText((Join-Path $path 'fixture.dat'), 'not a game asset', $utf8)
    } $false 'Unregistered asset'
    Run-Case 'unmarked-text' {
        param($dir)
        Write-Json @{schema_version = 1; texts = @(@{id = 'fixture'; value = 'Unmarked'; status = 'ai_placeholder'; source_asset_id = $null})} (Join-Path $dir 'content/texts.json')
    } $false 'Missing placeholder prefix'
    Run-Case 'approved-text-without-source' {
        param($dir)
        Write-Json @{schema_version = 1; texts = @(@{id = 'fixture'; value = 'Unverified'; status = 'approved'; source_asset_id = 'missing'})} (Join-Path $dir 'content/texts.json')
    } $false 'Text missing approved source'
    Run-Case 'broken-local-link' {
        param($dir)
        Add-Content -LiteralPath (Join-Path $dir 'README.md') -Value '[missing](not-present.md)' -Encoding UTF8
    } $false 'Broken link'
    Run-Case 'registered-fixture' {
        param($dir)
        Seed-TestAsset $dir
    } $true
    Run-Case 'tampered-file' {
        param($dir)
        Seed-TestAsset $dir
        [IO.File]::WriteAllText((Join-Path $dir 'assets/approved/fixture.dat'), 'tampered test data', $utf8)
    } $false 'Hash mismatch'
    Run-Case 'missing-permission' {
        param($dir)
        Seed-TestAsset $dir
        Write-Json @{schema_version = 1; approvals = @()} (Join-Path $dir 'content/approvals.json')
    } $false 'Missing approval'
    Write-Output 'PASS: all 11 validator behavior checks.'
} finally {
    $resolved = [IO.Path]::GetFullPath($scratch)
    $boundary = $localRoot.TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    if (-not $resolved.StartsWith($boundary, [StringComparison]::OrdinalIgnoreCase) -or
        -not ([IO.Path]::GetFileName($resolved)).StartsWith('plan-tests-')) {
        throw "Refusing cleanup outside the verified scratch directory: $resolved"
    }
    if (Test-Path -LiteralPath $resolved) { Remove-Item -LiteralPath $resolved -Recurse -Force }
}
