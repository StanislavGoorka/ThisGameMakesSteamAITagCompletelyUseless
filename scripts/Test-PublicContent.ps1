$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..'))
$localRoot = Join-Path $repoRoot '.local'
$scratch = Join-Path $localRoot ('public-content-' + [Guid]::NewGuid().ToString('N'))
$gitSafe = $scratch.Replace('\', '/')
$utf8 = [Text.UTF8Encoding]::new($false)
try {
    New-Item -ItemType Directory -Path $scratch -Force | Out-Null
    foreach ($relative in @('AGENTS.md', 'README.md', '.gitattributes', 'docs', 'content', '.agents', '.github', 'src', 'tests', 'tools', 'assets', 'scripts', 'CMakeLists.txt', 'CMakePresets.json')) {
        Copy-Item -LiteralPath (Join-Path $repoRoot $relative) -Destination $scratch -Recurse
    }
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch init --quiet
    if ($LASTEXITCODE -ne 0) { throw 'Cannot initialize fixture checkout' }
    & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $scratch add -- assets content
    if ($LASTEXITCODE -ne 0) { throw 'Cannot track fixture assets' }
    function Check-Process([string]$Name, [string]$Script, [bool]$Pass, [string]$Expected) {
        $output = & powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot $Script) -Root $scratch
        if (($LASTEXITCODE -eq 0) -ne $Pass -or -not (($output -join "`n").Contains($Expected))) {
            throw "$Name failed: $output"
        }
        Write-Output "PASS: $Name"
    }
    function Write-Texts($Records) {
        $data = @{schema_version = 1; texts = @($Records)}
        [IO.File]::WriteAllText((Join-Path $scratch 'content/texts.json'), ($data | ConvertTo-Json -Depth 20), $utf8)
    }
    Write-Texts @()
    Check-Process 'empty public catalog' 'Validate-PublicContent.ps1' $true 'only approved records'
    $placeholder = @{id = 'test'; value = '[AI PLACEHOLDER] Test'; status = 'ai_placeholder'; source_asset_id = $null}
    Write-Texts @($placeholder)
    Check-Process 'developer accepts marked text' 'Validate-AssetPackage.ps1' $true 'Git-tracked asset payload'
    Check-Process 'public rejects marked text' 'Validate-PublicContent.ps1' $false 'Public content rejects'
    # Synthetic, non-player fixture. No authorship claim and no product text.
    $approved = @{id = 'fixture'; value = 'synthetic test value'; status = 'approved'; source_asset_id = 'kenney1bit12-license'}
    Write-Texts @($approved)
    Check-Process 'approved-only fixture' 'Validate-PublicContent.ps1' $true 'only approved records'
    Write-Texts @($approved, $placeholder)
    Check-Process 'public rejects mixed list' 'Validate-PublicContent.ps1' $false 'Public content rejects'
    $approved.source_asset_id = 'unknown'
    Write-Texts @($approved)
    Check-Process 'public rejects missing source' 'Validate-PublicContent.ps1' $false 'Text missing approved source'
    $approved.source_asset_id = 'kenney1bit12-license'
    $approved.status = 'unknown'
    Write-Texts @($approved)
    Check-Process 'public rejects unknown status' 'Validate-PublicContent.ps1' $false 'Unknown text status'
    $approved.status = 'approved'
    $approved.value = '[AI PLACEHOLDER] Test'
    Write-Texts @($approved)
    Check-Process 'public rejects relabeled marker' 'Validate-PublicContent.ps1' $false 'Approved text contains placeholder'
    Write-Texts @()
    Check-Process 'restored public catalog' 'Validate-PublicContent.ps1' $true 'only approved records'
    Write-Output 'PASS: all 9 public content checks.'
} finally {
    $resolved = [IO.Path]::GetFullPath($scratch)
    $boundary = [IO.Path]::GetFullPath($localRoot).TrimEnd('\', '/') + [IO.Path]::DirectorySeparatorChar
    if (-not $resolved.StartsWith($boundary, [StringComparison]::OrdinalIgnoreCase) -or
        -not ([IO.Path]::GetFileName($resolved)).StartsWith('public-content-')) { throw 'Unsafe fixture cleanup path' }
    if (Test-Path -LiteralPath $resolved) { Remove-Item -LiteralPath $resolved -Recurse -Force }
}
