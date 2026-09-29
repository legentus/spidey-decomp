[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$ToolchainRoot = "C:\vs"
$NMakePath = Join-Path $ToolchainRoot "BIN\nmake.exe"
$ArchiveUrl = "https://github.com/krystalgamer/spidey-decomp-vs/releases/download/v1.0/spidey-vs.zip"
$ArchivePath = Join-Path $env:TEMP "spidey-vs-v1.0.zip"

if (Test-Path $NMakePath) {
    Write-Host "[OK] Preserved matching toolchain already present: $ToolchainRoot"
    exit 0
}

if (Test-Path $ToolchainRoot) {
    throw "C:\vs already exists but BIN\nmake.exe is missing. Refusing to overwrite an unknown toolchain directory."
}

Write-Host "[..] Downloading the preserved Spider-Man matching toolchain used by upstream CI..."
Write-Host "     $ArchiveUrl"
Invoke-WebRequest -Uri $ArchiveUrl -OutFile $ArchivePath -UseBasicParsing

Write-Host "[..] Extracting to $ToolchainRoot ..."
New-Item -ItemType Directory -Path $ToolchainRoot -Force | Out-Null
Expand-Archive -Path $ArchivePath -DestinationPath $ToolchainRoot -Force
Remove-Item $ArchivePath -Force

if (-not (Test-Path $NMakePath)) {
    throw "Toolchain extraction completed, but $NMakePath was not found."
}

Write-Host "[OK] Matching toolchain ready: $ToolchainRoot"
