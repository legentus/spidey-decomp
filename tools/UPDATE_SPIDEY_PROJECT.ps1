param(
    [switch]$NoPause
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

function Fail([string]$Message) {
    Write-Host ""
    Write-Host "[ERROR] $Message" -ForegroundColor Red
    if (-not $NoPause) {
        Write-Host ""
        Read-Host "Press Enter to close"
    }
    exit 1
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 Dev - Update"
Write-Host "============================================================"
Write-Host "Project: $RepoRoot"
Write-Host ""

[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$archiveUrl = "https://github.com/legentus/spidey-decomp/archive/refs/heads/dev.zip"
$commitApi = "https://api.github.com/repos/legentus/spidey-decomp/commits/dev"
$headers = @{ "User-Agent" = "Spider-Man-2000-Dev-Updater" }

try {
    $remote = Invoke-RestMethod -Uri $commitApi -Headers $headers -UseBasicParsing
    $remoteRevision = [string]$remote.sha
} catch {
    Fail "Could not query the current dev revision from GitHub: $($_.Exception.Message)"
}

if (-not $remoteRevision -or $remoteRevision.Length -lt 7) {
    Fail "GitHub returned an invalid dev revision."
}

$revisionFile = Join-Path $RepoRoot "LOCAL_DEV_REVISION.txt"
$currentRevision = ""
if (Test-Path $revisionFile) {
    $currentRevision = (Get-Content -Raw $revisionFile).Trim()
}

Write-Host "[INFO] Local revision:  $(if ($currentRevision) { $currentRevision } else { '<unknown>' })"
Write-Host "[INFO] Remote revision: $remoteRevision"

if ($currentRevision -eq $remoteRevision) {
    Write-Host ""
    Write-Host "[OK] Already current." -ForegroundColor Green
    if (-not $NoPause) {
        Write-Host ""
        Read-Host "Press Enter to close"
    }
    exit 0
}

$tempRoot = Join-Path $env:TEMP ("Spidey2000Update-" + [Guid]::NewGuid().ToString("N"))
$zipPath = Join-Path $tempRoot "dev.zip"
$extractRoot = Join-Path $tempRoot "extract"

try {
    New-Item -ItemType Directory -Force -Path $extractRoot | Out-Null

    Write-Host "[..] Downloading dev branch archive..."
    Invoke-WebRequest -Uri $archiveUrl -OutFile $zipPath -Headers $headers -UseBasicParsing

    Write-Host "[..] Extracting..."
    Expand-Archive -LiteralPath $zipPath -DestinationPath $extractRoot -Force

    $sourceRoot = Join-Path $extractRoot "spidey-decomp-dev"
    if (-not (Test-Path (Join-Path $sourceRoot "README.md"))) {
        Fail "Downloaded archive did not contain the expected spidey-decomp-dev root."
    }

    $robocopyCandidates = @(
        (Join-Path $env:SystemRoot "System32\robocopy.exe"),
        (Join-Path $env:SystemRoot "Sysnative\robocopy.exe")
    )
    $robocopy = $robocopyCandidates | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $robocopy) {
        Fail "Windows robocopy.exe could not be located."
    }

    Write-Host "[..] Refreshing local project files..."
    $args = @(
        $sourceRoot,
        $RepoRoot,
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
        Fail "Project refresh failed with robocopy exit code $rc."
    }

    Set-Content -Path $revisionFile -Value $remoteRevision -Encoding ASCII

    Write-Host ""
    Write-Host "[OK] Local project is current." -ForegroundColor Green
    Write-Host "Revision: $remoteRevision"
} finally {
    if (Test-Path $tempRoot) {
        Remove-Item -Recurse -Force $tempRoot -ErrorAction SilentlyContinue
    }
}

if (-not $NoPause) {
    Write-Host ""
    Read-Host "Press Enter to close"
}
