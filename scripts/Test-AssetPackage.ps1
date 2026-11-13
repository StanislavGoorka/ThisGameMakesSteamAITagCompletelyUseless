$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$localRoot = Join-Path $repoRoot '.local'
$scratch = Join-Path $localRoot ('asset-gate-' + [Guid]::NewGuid().ToString('N'))
$gitSafe = $scratch.Replace('\', '/')
$utf8 = [Text.UTF8Encoding]::new($false)
try {
    New-Item -ItemType Directory -Path $scratch -Force | Out-Null
    # Isolated checkout; no assets are changed in the real repository.
    foreach ($relative in @('AGENTS.md', 'README.md', '.gitattributes', 'docs', 'content', '.agents', '.github', 'src', 'tests', 'tools', 'assets', 'CMakeLists.txt')) {
        Copy-Item -LiteralPath (Join-Path $repoRoot $relative) -Destination $scratch -Recurse
    }
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch init --quiet
    if ($LASTEXITCODE -ne 0) { throw 'Cannot initialize fixture checkout' }
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch add -- assets content
    if ($LASTEXITCODE -ne 0) { throw 'Cannot track fixture assets' }
    $gate = Join-Path $PSScriptRoot 'Validate-AssetPackage.ps1'
    function Check-Gate([string]$Name, [bool]$Pass, [string]$Expected) {
        $output = & powershell -NoProfile -ExecutionPolicy Bypass -File $gate -Root $scratch
        $actual = $LASTEXITCODE -eq 0
        if ($actual -ne $Pass -or -not (($output -join "`n").Contains($Expected))) { throw "$Name failed: $output" }
        Write-Output "PASS: $Name"
    }
    Check-Gate 'valid tracked payload' $true 'Git-tracked asset payload'
    $orphan = Join-Path $scratch 'assets/approved/orphan.dat'
    [IO.File]::WriteAllText($orphan, 'isolated non-player data', $utf8)
    Check-Gate 'unregistered file' $false 'Unregistered asset'
    Remove-Item -LiteralPath $orphan
    [IO.File]::WriteAllText($orphan, 'hidden isolated test data', $utf8)
    [IO.File]::SetAttributes($orphan, [IO.FileAttributes]::Hidden)
    Check-Gate 'hidden unregistered file' $false 'Unregistered asset'
    Remove-Item -LiteralPath $orphan -Force
    $registry = Get-Content -Raw -Encoding UTF8 (Join-Path $scratch 'content/assets.json') | ConvertFrom-Json
    $relative = $registry.assets[0].path
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch rm --cached --quiet -- $relative
    if ($LASTEXITCODE -ne 0) { throw 'Cannot untrack fixture asset' }
    Check-Gate 'registered but untracked' $false 'Untracked asset'
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch add -- $relative
    $path = Join-Path $scratch $relative
    $original = [IO.File]::ReadAllBytes($path)
    $corrupt = [byte[]]$original.Clone()
    $corrupt[0] = $corrupt[0] -bxor 1
    [IO.File]::WriteAllBytes($path, $corrupt)
    Check-Gate 'changed hash' $false 'Hash mismatch'
    [IO.File]::WriteAllBytes($path, $original)
    $approvalsPath = Join-Path $scratch 'content/approvals.json'
    $originalApprovals = [IO.File]::ReadAllBytes($approvalsPath)
    $approvals = Get-Content -Raw -Encoding UTF8 $approvalsPath | ConvertFrom-Json
    $approvals.approvals[0].status = 'denied'
    [IO.File]::WriteAllText($approvalsPath, ($approvals | ConvertTo-Json -Depth 20), $utf8)
    Check-Gate 'denied permission' $false 'no granted permission'
    [IO.File]::WriteAllBytes($approvalsPath, $originalApprovals)
    Check-Gate 'restored payload' $true 'Git-tracked asset payload'
    Write-Output 'PASS: all 7 asset gate checks.'
} finally {
    $resolved = [IO.Path]::GetFullPath($scratch)
    $boundary = [IO.Path]::GetFullPath($localRoot).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    if (-not $resolved.StartsWith($boundary, [StringComparison]::OrdinalIgnoreCase) -or
        -not ([IO.Path]::GetFileName($resolved)).StartsWith('asset-gate-')) { throw 'Unsafe fixture cleanup path' }
    if (Test-Path -LiteralPath $resolved) { Remove-Item -LiteralPath $resolved -Recurse -Force }
}
