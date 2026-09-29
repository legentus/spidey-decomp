[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$GameDir,

    [string]$Artifact
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
if (-not $Artifact) {
    $Artifact = Join-Path $RepoRoot "out\matching\binkw32.dll"
}

$GameDir = (Resolve-Path $GameDir).Path
$GameExe = Join-Path $GameDir "SpideyPC.exe"
$LiveBink = Join-Path $GameDir "binkw32.dll"
$OriginalBink = Join-Path $GameDir "binkw32_.dll"

if (-not (Test-Path $GameExe)) {
    throw "SpideyPC.exe was not found in: $GameDir"
}

if (-not (Test-Path $Artifact)) {
    throw ("Developer proxy artifact was not found: " + $Artifact + [Environment]::NewLine + "Run scripts\build_matching.ps1 first.")
}

if (-not (Test-Path $OriginalBink)) {
    if (-not (Test-Path $LiveBink)) {
        throw "Neither binkw32.dll nor binkw32_.dll exists in the game directory."
    }

    Write-Host "[..] Preserving the retail Bink DLL as binkw32_.dll ..."
    Move-Item -Path $LiveBink -Destination $OriginalBink
} else {
    Write-Host "[OK] binkw32_.dll already exists; leaving it untouched."
}

Write-Host "[..] Installing reconstructed proxy as binkw32.dll ..."
Copy-Item -Path $Artifact -Destination $LiveBink -Force

$Hash = Get-FileHash -Algorithm SHA256 -Path $LiveBink
Write-Host ""
Write-Host "[OK] Developer proxy installed."
Write-Host "Game:       $GameExe"
Write-Host "Proxy:      $LiveBink"
Write-Host "Original:   $OriginalBink"
Write-Host "SHA-256:    $($Hash.Hash)"
Write-Host ""
Write-Host "Launch SpideyPC.exe normally. A spidey-decomp console should appear."
Write-Host "Use scripts\restore_stock_bink.ps1 -GameDir <path> to restore the retail DLL."
