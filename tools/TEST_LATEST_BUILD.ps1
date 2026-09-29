param(
    [switch]$PostUpdate
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

function Stop-WithPause([string]$Message, [int]$Code = 1) {
    Write-Host ""
    if ($Code -eq 0) {
        Write-Host $Message -ForegroundColor Green
    } else {
        Write-Host "[ERROR] $Message" -ForegroundColor Red
    }
    Write-Host ""
    Read-Host "Press Enter to close"
    exit $Code
}

function Read-LocalGameDir {
    $configPath = Join-Path $RepoRoot "spidey_local_config.bat"

    if (Test-Path $configPath) {
        foreach ($line in Get-Content $configPath) {
            if ($line -match '^\s*set\s+"?SPIDEY_GAME_DIR=(.+?)"?\s*$') {
                return $Matches[1].Trim('"')
            }
        }
    }

    Write-Host ""
    Write-Host "First-time game setup"
    Write-Host "Enter the folder that contains SpideyPC.exe."
    $gameDir = (Read-Host "Game folder").Trim().Trim('"')

    if (-not $gameDir) {
        Stop-WithPause "No game folder was entered."
    }

    $full = [System.IO.Path]::GetFullPath($gameDir)
    if (-not (Test-Path (Join-Path $full "SpideyPC.exe"))) {
        Stop-WithPause "SpideyPC.exe was not found in '$full'."
    }

    @(
        "@echo off",
        ('set "SPIDEY_GAME_DIR=' + $full + '"')
    ) | Set-Content -Path $configPath -Encoding ASCII

    return $full
}

function Ensure-MatchingToolchain {
    $root = Join-Path $env:LOCALAPPDATA "Spidey2000Dev\MatchingVS"
    $nmake = Join-Path $root "BIN\nmake.exe"

    if (Test-Path $nmake) {
        return $root
    }

    $url = "https://github.com/krystalgamer/spidey-decomp-vs/releases/download/v1.0/spidey-vs.zip"
    $tempZip = Join-Path $env:TEMP "spidey-vs-v1.0.zip"

    Write-Host "[..] Downloading preserved matching compiler toolchain..."
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $url -OutFile $tempZip -UseBasicParsing

    if (Test-Path $root) {
        Remove-Item -Recurse -Force $root
    }
    New-Item -ItemType Directory -Force -Path $root | Out-Null

    Write-Host "[..] Extracting matching compiler toolchain..."
    Expand-Archive -LiteralPath $tempZip -DestinationPath $root -Force
    Remove-Item $tempZip -Force -ErrorAction SilentlyContinue

    if (-not (Test-Path $nmake)) {
        Stop-WithPause "Matching toolchain extraction completed, but BIN\nmake.exe was not found."
    }

    return $root
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 Dev - Latest Test"
Write-Host "============================================================"
Write-Host ""

$revisionFile = Join-Path $RepoRoot "LOCAL_DEV_REVISION.txt"
$beforeRevision = ""
if (Test-Path $revisionFile) {
    $beforeRevision = (Get-Content -Raw $revisionFile).Trim()
}

if (-not $PostUpdate) {
    & (Join-Path $PSScriptRoot "UPDATE_SPIDEY_PROJECT.ps1") -NoPause
    if ($LASTEXITCODE -ne 0) {
        Stop-WithPause "Update failed." $LASTEXITCODE
    }

    $afterRevision = ""
    if (Test-Path $revisionFile) {
        $afterRevision = (Get-Content -Raw $revisionFile).Trim()
    }

    if ($afterRevision -and $afterRevision -ne $beforeRevision) {
        Write-Host ""
        Write-Host "[INFO] Project changed during update."
        Write-Host "[..] Restarting with the newly updated test workflow..."

        $psExe = (Get-Process -Id $PID).Path
        & $psExe -NoProfile -ExecutionPolicy Bypass -File $PSCommandPath -PostUpdate
        exit $LASTEXITCODE
    }
} else {
    Write-Host "[INFO] Continuing with the newly updated workflow."
}

$revision = "dev"
if (Test-Path $revisionFile) {
    $revision = (Get-Content -Raw $revisionFile).Trim()
}
Write-Host "[TEST] Revision $revision"

$gameDir = Read-LocalGameDir
Write-Host "[INFO] Game: $gameDir"

$gameExe = Join-Path $gameDir "SpideyPC.exe"
$liveBink = Join-Path $gameDir "binkw32.dll"
$originalBink = Join-Path $gameDir "binkw32_.dll"

if (-not (Test-Path $gameExe)) {
    Stop-WithPause "SpideyPC.exe was not found in '$gameDir'."
}

$toolchainRoot = Ensure-MatchingToolchain
Write-Host "[OK] Matching toolchain: $toolchainRoot"

$runtimeHeader = Join-Path $RepoRoot "runtime_version.h"
$runtimeBackup = $null
$hadRuntimeHeader = Test-Path $runtimeHeader
if ($hadRuntimeHeader) {
    $runtimeBackup = Get-Content -Raw $runtimeHeader
}

try {
    Set-Content -Path $runtimeHeader -Value ('#define RUNTIME_VERSION "' + $revision + '"') -Encoding ASCII

    $env:SPIDEY_MSVC_ROOT = $toolchainRoot

    Write-Host ""
    Write-Host "[..] Building matching proxy..."
    & $env:ComSpec /d /c ('"' + (Join-Path $RepoRoot "build.bat") + '"')
    if ($LASTEXITCODE -ne 0) {
        Stop-WithPause "Matching build failed." $LASTEXITCODE
    }
} finally {
    if ($hadRuntimeHeader) {
        [System.IO.File]::WriteAllText($runtimeHeader, $runtimeBackup)
    } elseif (Test-Path $runtimeHeader) {
        Remove-Item $runtimeHeader -Force
    }
}

$builtDll = Join-Path $RepoRoot "Release\spider.dll"
if (-not (Test-Path $builtDll)) {
    Stop-WithPause "Build completed but Release\spider.dll was not produced."
}

$outDir = Join-Path $RepoRoot "out\matching"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$proxyDll = Join-Path $outDir "binkw32.dll"
Copy-Item $builtDll $proxyDll -Force

$pdb = Join-Path $RepoRoot "Release\spider.pdb"
if (Test-Path $pdb) {
    Copy-Item $pdb (Join-Path $outDir "spider.pdb") -Force
}

$hash = (Get-FileHash -Algorithm SHA256 $proxyDll).Hash
Write-Host "[OK] Proxy SHA-256: $hash"

if (-not (Test-Path $originalBink)) {
    if (-not (Test-Path $liveBink)) {
        Stop-WithPause "Neither binkw32.dll nor binkw32_.dll exists in the game folder."
    }

    Write-Host "[..] Preserving retail Bink DLL as binkw32_.dll..."
    Move-Item $liveBink $originalBink
} else {
    Write-Host "[OK] Preserved retail binkw32_.dll already exists."
}

Copy-Item $proxyDll $liveBink -Force
Write-Host "[OK] Installed rebuilt proxy as binkw32.dll."

$logRoot = Join-Path $RepoRoot "logs"
New-Item -ItemType Directory -Force -Path $logRoot | Out-Null
$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$sessionDir = Join-Path $logRoot $stamp
New-Item -ItemType Directory -Force -Path $sessionDir | Out-Null

@(
    "revision=$revision",
    "proxy_sha256=$hash",
    "game=$gameExe",
    "started=$(Get-Date -Format o)"
) | Set-Content -Path (Join-Path $sessionDir "test-session.txt") -Encoding UTF8

Write-Host ""
Write-Host "[RUN] $gameExe"
Write-Host "[LOG] $sessionDir"
Start-Process -FilePath $gameExe -WorkingDirectory $gameDir

Write-Host ""
Write-Host "[OK] Latest dev build installed and launched." -ForegroundColor Green
Write-Host "If it crashes or validation fails, send the complete spidey-decomp console output."
Write-Host ""
Read-Host "Press Enter to close this launcher"
