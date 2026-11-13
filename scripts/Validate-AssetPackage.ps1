param([string]$Root = (Join-Path $PSScriptRoot '..'))

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath($Root).TrimEnd('\', '/')
$gitSafe = $repoRoot.Replace('\', '/')

# This is a checkout asset gate, not the final executable/depot allowlist.
& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'Validate-Project.ps1') -Root $repoRoot
if ($LASTEXITCODE -ne 0) { exit 1 }
try {
    $gitRoot = & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -C $repoRoot rev-parse --show-toplevel 2>&1
    if ($LASTEXITCODE -ne 0 -or [IO.Path]::GetFullPath([string]$gitRoot) -ne $repoRoot) {
        throw 'Asset gate requires the exact Git checkout root'
    }
    $records = Get-Content -Raw -Encoding UTF8 (Join-Path $repoRoot 'content/assets.json') | ConvertFrom-Json
    $registered = @{}
    foreach ($asset in $records.assets) { $registered[$asset.path.Replace('\', '/')] = $true }
    $pending = [Collections.Generic.Stack[string]]::new()
    $approved = Join-Path $repoRoot 'assets/approved'
    if (Test-Path -LiteralPath $approved) { $pending.Push($approved) }
    while ($pending.Count -gt 0) {
        $item = Get-Item -Force -LiteralPath $pending.Pop()
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Reparse asset path: $($item.FullName)" }
        if ($item.PSIsContainer) {
            foreach ($child in Get-ChildItem -Force -LiteralPath $item.FullName) { $pending.Push($child.FullName) }
        } else {
            $relative = $item.FullName.Substring($repoRoot.Length + 1).Replace('\', '/')
            if (-not $registered.ContainsKey($relative)) { throw "Unregistered asset: $relative" }
            $tracked = & git -c core.fsmonitor=false -c "safe.directory=$gitSafe" -c core.quotepath=false -C $repoRoot ls-files -- $relative
            if ($LASTEXITCODE -ne 0) { throw 'Cannot query tracked asset paths' }
            if ($tracked -cne $relative) { throw "Untracked asset: $relative" }
        }
    }
} catch {
    Write-Output "ERROR: $($_.Exception.Message)"
    exit 1
}
Write-Output 'PASS: registered, approved, hash-verified and Git-tracked asset payload.'
Write-Output 'This gate does not audit human authorship or authorize a release.'
exit 0
