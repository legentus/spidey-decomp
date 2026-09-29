[CmdletBinding()]
param(
    [switch]$SkipToolchainSetup
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$ToolchainRoot = "C:\vs"
$NMakePath = Join-Path $ToolchainRoot "BIN\nmake.exe"
$RuntimeHeader = Join-Path $RepoRoot "runtime_version.h"
$ReleaseDll = Join-Path $RepoRoot "Release\spider.dll"
$ReleasePdb = Join-Path $RepoRoot "Release\spider.pdb"
$ArtifactDir = Join-Path $RepoRoot "out\matching"
$ArtifactDll = Join-Path $ArtifactDir "binkw32.dll"

if (-not (Test-Path $NMakePath)) {
    if ($SkipToolchainSetup) {
        throw "Matching toolchain not found at $ToolchainRoot and -SkipToolchainSetup was specified."
    }

    & (Join-Path $PSScriptRoot "setup_matching_toolchain.ps1")
    if ($LASTEXITCODE -ne 0) {
        throw "Matching toolchain setup failed with exit code $LASTEXITCODE."
    }
}

$Version = "LOCAL"
try {
    $GitVersion = (& git -C $RepoRoot rev-parse HEAD 2>$null).Trim()
    if ($LASTEXITCODE -eq 0 -and $GitVersion) {
        $Version = $GitVersion
    }
} catch {
    # A source snapshot without Git is still buildable; it identifies itself as LOCAL.
}

$HadRuntimeHeader = Test-Path $RuntimeHeader
$PreviousRuntimeHeader = $null
if ($HadRuntimeHeader) {
    $PreviousRuntimeHeader = Get-Content -Raw -Path $RuntimeHeader
}

try {
    Set-Content -Path $RuntimeHeader -Encoding Ascii -Value ('#define RUNTIME_VERSION "' + $Version + '"')

    Write-Host "============================================================"
    Write-Host "  Spider-Man 2000 Matching Windows Build"
    Write-Host "============================================================"
    Write-Host "Repository: $RepoRoot"
    Write-Host "Version:    $Version"
    Write-Host "Toolchain:  $ToolchainRoot"
    Write-Host ""

    Push-Location $RepoRoot
    try {
        & cmd.exe /d /c build.bat
        if ($LASTEXITCODE -ne 0) {
            throw "build.bat failed with exit code $LASTEXITCODE."
        }
    } finally {
        Pop-Location
    }

    if (-not (Test-Path $ReleaseDll)) {
        throw "Build returned success but Release\spider.dll was not produced."
    }

    New-Item -ItemType Directory -Path $ArtifactDir -Force | Out-Null
    Copy-Item -Path $ReleaseDll -Destination $ArtifactDll -Force

    if (Test-Path $ReleasePdb) {
        Copy-Item -Path $ReleasePdb -Destination (Join-Path $ArtifactDir "spider.pdb") -Force
    }

    $Hash = Get-FileHash -Algorithm SHA256 -Path $ArtifactDll

    Write-Host ""
    Write-Host "[OK] Matching proxy build complete."
    Write-Host "Artifact:   $ArtifactDll"
    Write-Host "SHA-256:    $($Hash.Hash)"
    if (Test-Path (Join-Path $ArtifactDir "spider.pdb")) {
        Write-Host ("Symbols:    " + (Join-Path $ArtifactDir "spider.pdb"))
    }
} finally {
    if ($HadRuntimeHeader) {
        [System.IO.File]::WriteAllText($RuntimeHeader, $PreviousRuntimeHeader)
    } elseif (Test-Path $RuntimeHeader) {
        Remove-Item $RuntimeHeader -Force
    }
}
