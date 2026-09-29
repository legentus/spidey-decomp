param()

$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Host ""
    Write-Host "[ERROR] $Message" -ForegroundColor Red
    Write-Host ""
    Read-Host "Press Enter to close"
    exit 1
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 Developer Project - First Setup"
Write-Host "============================================================"
Write-Host ""

$defaultTarget = Join-Path $env:USERPROFILE "Documents\Spider-Man-2000-Dev"
$entered = Read-Host "Install folder [$defaultTarget]"
$target = if ([string]::IsNullOrWhiteSpace($entered)) { $defaultTarget } else { $entered.Trim().Trim('"') }
$target = [System.IO.Path]::GetFullPath($target)

Write-Host ""
Write-Host "Target:"
Write-Host "  $target"
Write-Host ""

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$archiveUrl = "https://github.com/legentus/spidey-decomp/archive/refs/heads/dev.zip"
$commitApi = "https://api.github.com/repos/legentus/spidey-decomp/commits/dev"
$headers = @{ "User-Agent" = "Spider-Man-2000-Dev-Bootstrap" }

try {
    $remote = Invoke-RestMethod -Uri $commitApi -Headers $headers -UseBasicParsing
    $remoteRevision = [string]$remote.sha
} catch {
    Fail "Could not query GitHub dev revision: $($_.Exception.Message)"
}

$tempRoot = Join-Path $env:TEMP ("Spidey2000Bootstrap-" + [Guid]::NewGuid().ToString("N"))
$zipPath = Join-Path $tempRoot "dev.zip"
$extractRoot = Join-Path $tempRoot "extract"

try {
    New-Item -ItemType Directory -Force -Path $extractRoot | Out-Null

    Write-Host "[..] Downloading Spider-Man dev project..."
    Invoke-WebRequest -Uri $archiveUrl -OutFile $zipPath -Headers $headers -UseBasicParsing

    Write-Host "[..] Extracting..."
    Expand-Archive -LiteralPath $zipPath -DestinationPath $extractRoot -Force

    $sourceRoot = Join-Path $extractRoot "spidey-decomp-dev"
    if (-not (Test-Path (Join-Path $sourceRoot "README.md"))) {
        Fail "Downloaded archive did not contain the expected project root."
    }

    New-Item -ItemType Directory -Force -Path $target | Out-Null

    $robocopyCandidates = @(
        (Join-Path $env:SystemRoot "System32\robocopy.exe"),
        (Join-Path $env:SystemRoot "Sysnative\robocopy.exe")
    )
    $robocopy = $robocopyCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $robocopy) {
        Fail "Windows robocopy.exe could not be located."
    }

    Write-Host "[..] Installing/updating local working copy..."
    $args = @(
        $sourceRoot,
        $target,
        "/MIR",
        "/R:2",
        "/W:1",
        "/NFL",
        "/NDL",
        "/NJH",
        "/NJS",
        "/NP",
        "/XD", ".git", "out", "Release", "Debug", "logs",
        "/XF", "spidey_local_config.bat", "LOCAL_DEV_REVISION.txt"
    )

    & $robocopy @args
    $rc = $LASTEXITCODE
    if ($rc -ge 8) {
        Fail "Project copy failed with robocopy exit code $rc."
    }

    Set-Content -Path (Join-Path $target "LOCAL_DEV_REVISION.txt") -Value $remoteRevision -Encoding ASCII
} finally {
    if (Test-Path $tempRoot) {
        Remove-Item -Recurse -Force $tempRoot -ErrorAction SilentlyContinue
    }
}

$configPath = Join-Path $target "spidey_local_config.bat"
if (-not (Test-Path $configPath)) {
    Write-Host ""
    Write-Host "Game setup"
    Write-Host "Enter the folder containing SpideyPC.exe."
    $gameInput = (Read-Host "Game folder").Trim().Trim('"')
    if (-not $gameInput) {
        Fail "No game folder was entered."
    }

    $gameDir = [System.IO.Path]::GetFullPath($gameInput)
    if (-not (Test-Path (Join-Path $gameDir "SpideyPC.exe"))) {
        Fail "SpideyPC.exe was not found in '$gameDir'."
    }

    @(
        "@echo off",
        ('set "SPIDEY_GAME_DIR=' + $gameDir + '"')
    ) | Set-Content -Path $configPath -Encoding ASCII
}

Write-Host ""
Write-Host "============================================================"
Write-Host "  PROJECT READY"
Write-Host "============================================================"
Write-Host ""
Write-Host "Local project:"
Write-Host "  $target"
Write-Host ""
Write-Host "Normal workflow:"
Write-Host "  UPDATE_SPIDEY_PROJECT.bat"
Write-Host "  TEST_LATEST_BUILD.bat"
Write-Host ""
Write-Host "TEST_LATEST_BUILD.bat updates, rebuilds, installs, and launches."
Write-Host ""
Read-Host "Press Enter to close"
