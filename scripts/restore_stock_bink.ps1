[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string]$GameDir
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$GameDir = (Resolve-Path $GameDir).Path
$GameExe = Join-Path $GameDir "SpideyPC.exe"
$LiveBink = Join-Path $GameDir "binkw32.dll"
$OriginalBink = Join-Path $GameDir "binkw32_.dll"

if (-not (Test-Path $GameExe)) {
    throw "SpideyPC.exe was not found in: $GameDir"
}

if (-not (Test-Path $OriginalBink)) {
    throw "binkw32_.dll was not found. There is no preserved retail Bink DLL to restore."
}

if (Test-Path $LiveBink) {
    Remove-Item -Path $LiveBink -Force
}

Move-Item -Path $OriginalBink -Destination $LiveBink

Write-Host "[OK] Restored the preserved retail binkw32.dll."
Write-Host "Game directory: $GameDir"
