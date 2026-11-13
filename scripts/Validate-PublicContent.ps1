param([string]$Root = (Join-Path $PSScriptRoot '..'))

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$repoRoot = [IO.Path]::GetFullPath($Root)
& powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'Validate-AssetPackage.ps1') -Root $repoRoot
if ($LASTEXITCODE -ne 0) { exit 1 }
try {
    $catalog = Get-Content -Raw -Encoding UTF8 (Join-Path $repoRoot 'content/texts.json') | ConvertFrom-Json
    foreach ($record in $catalog.texts) {
        if ($record.status -ne 'approved' -or $record.value.Contains('[AI PLACEHOLDER]')) {
            throw "Public content rejects placeholder/unapproved text: $($record.id)"
        }
    }
} catch {
    Write-Output "ERROR: $($_.Exception.Message)"
    exit 1
}
Write-Output 'PASS: public text registry contains only approved records.'
Write-Output 'Registry checks do not scan executable strings or certify a depot.'
exit 0
