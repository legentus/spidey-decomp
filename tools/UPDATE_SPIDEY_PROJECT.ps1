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

function Find-GitExecutable {
    $cmd = Get-Command git.exe -ErrorAction SilentlyContinue
    if ($cmd -and $cmd.Source) {
        return $cmd.Source
    }

    $pf86 = [Environment]::GetFolderPath("ProgramFilesX86")
    $pf = [Environment]::GetFolderPath("ProgramFiles")
    $candidates = @()

    if ($pf) {
        $candidates += (Join-Path $pf "Git\cmd\git.exe")
    }
    if ($pf86) {
        $candidates += (Join-Path $pf86 "Git\cmd\git.exe")
    }
    if ($env:LOCALAPPDATA) {
        $candidates += (Join-Path $env:LOCALAPPDATA "Programs\Git\cmd\git.exe")
        $candidates += (Join-Path $env:LOCALAPPDATA "Spidey2000Dev\MinGit\cmd\git.exe")
    }

    foreach ($candidate in $candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }

    return $null
}

function Find-CurlExecutable {
    $candidates = @()

    if ($env:SystemRoot) {
        $candidates += (Join-Path $env:SystemRoot "System32\curl.exe")
        $candidates += (Join-Path $env:SystemRoot "Sysnative\curl.exe")
    }

    $cmd = Get-Command curl.exe -ErrorAction SilentlyContinue
    if ($cmd -and $cmd.Source) {
        $candidates += $cmd.Source
    }

    foreach ($candidate in ($candidates | Select-Object -Unique)) {
        if ($candidate -and (Test-Path -LiteralPath $candidate)) {
            return $candidate
        }
    }

    return $null
}

function Get-RemoteDevRevision {
    param(
        [string]$CommitApi,
        [hashtable]$Headers
    )

    for ($attempt = 1; $attempt -le 3; $attempt++) {
        try {
            if ($attempt -gt 1) {
                Write-Host ("[..] Retrying GitHub revision query (" + $attempt + "/3)...")
            }

            $remote = Invoke-RestMethod -Uri $CommitApi -Headers $Headers -UseBasicParsing -TimeoutSec 30
            $sha = [string]$remote.sha

            if ($sha -match '^[0-9a-fA-F]{40}$') {
                Write-Host "[OK] GitHub revision resolved through API."
                return $sha.ToLowerInvariant()
            }
        } catch {
            Write-Host ("[WARNING] GitHub API attempt " + $attempt + " failed: " + $_.Exception.Message)
            if ($attempt -lt 3) {
                Start-Sleep -Seconds (2 * $attempt)
            }
        }
    }

    $gitExe = Find-GitExecutable
    if ($gitExe) {
        try {
            Write-Host "[..] GitHub API unavailable; trying git ls-remote..."
            $result = & $gitExe ls-remote "https://github.com/legentus/spidey-decomp.git" "refs/heads/dev" 2>$null

            if ($LASTEXITCODE -eq 0 -and $result) {
                $sha = (($result | Select-Object -First 1) -split '\s+')[0]

                if ($sha -match '^[0-9a-fA-F]{40}$') {
                    Write-Host "[OK] GitHub revision resolved through Git."
                    return $sha.ToLowerInvariant()
                }
            }
        } catch {
            Write-Host ("[WARNING] git ls-remote failed: " + $_.Exception.Message)
        }
    }

    $curlExe = Find-CurlExecutable
    if ($curlExe) {
        $tempJson = Join-Path $env:TEMP ("spidey-dev-revision-" + [Guid]::NewGuid().ToString("N") + ".json")

        try {
            Write-Host "[..] Git fallback unavailable; trying Windows curl..."
            & $curlExe -L --fail --silent --show-error --retry 3 --retry-delay 2 --connect-timeout 20 -H "User-Agent: Spider-Man-2000-Dev-Updater" -o $tempJson $CommitApi

            if ($LASTEXITCODE -eq 0 -and (Test-Path -LiteralPath $tempJson)) {
                $json = Get-Content -Raw -LiteralPath $tempJson | ConvertFrom-Json
                $sha = [string]$json.sha

                if ($sha -match '^[0-9a-fA-F]{40}$') {
                    Write-Host "[OK] GitHub revision resolved through curl."
                    return $sha.ToLowerInvariant()
                }
            }
        } catch {
            Write-Host ("[WARNING] curl revision query failed: " + $_.Exception.Message)
        } finally {
            Remove-Item -LiteralPath $tempJson -Force -ErrorAction SilentlyContinue
        }
    }

    return $null
}

function Download-DevArchive {
    param(
        [string]$Url,
        [string]$OutputPath,
        [hashtable]$Headers
    )

    for ($attempt = 1; $attempt -le 3; $attempt++) {
        Remove-Item -LiteralPath $OutputPath -Force -ErrorAction SilentlyContinue

        try {
            if ($attempt -gt 1) {
                Write-Host ("[..] Retrying archive download (" + $attempt + "/3)...")
            }

            Invoke-WebRequest -Uri $Url -OutFile $OutputPath -Headers $Headers -UseBasicParsing -TimeoutSec 120

            if ((Test-Path -LiteralPath $OutputPath) -and ((Get-Item -LiteralPath $OutputPath).Length -gt 1024)) {
                return $true
            }
        } catch {
            Write-Host ("[WARNING] PowerShell archive download attempt " + $attempt + " failed: " + $_.Exception.Message)
            if ($attempt -lt 3) {
                Start-Sleep -Seconds (2 * $attempt)
            }
        }
    }

    $curlExe = Find-CurlExecutable
    if ($curlExe) {
        try {
            Remove-Item -LiteralPath $OutputPath -Force -ErrorAction SilentlyContinue
            Write-Host "[..] PowerShell download failed; trying Windows curl..."
            & $curlExe -L --fail --show-error --retry 4 --retry-delay 2 --connect-timeout 20 -o $OutputPath $Url

            if ($LASTEXITCODE -eq 0 -and (Test-Path -LiteralPath $OutputPath) -and ((Get-Item -LiteralPath $OutputPath).Length -gt 1024)) {
                return $true
            }
        } catch {
            Write-Host ("[WARNING] curl archive download failed: " + $_.Exception.Message)
        }
    }

    return $false
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

$revisionFile = Join-Path $RepoRoot "LOCAL_DEV_REVISION.txt"
$currentRevision = ""

if (Test-Path -LiteralPath $revisionFile) {
    $currentRevision = (Get-Content -Raw -LiteralPath $revisionFile).Trim()
}

$remoteRevision = Get-RemoteDevRevision -CommitApi $commitApi -Headers $headers

Write-Host "[INFO] Local revision:  $(if ($currentRevision) { $currentRevision } else { '<unknown>' })"

if ($remoteRevision) {
    Write-Host "[INFO] Remote revision: $remoteRevision"

    if ($currentRevision -eq $remoteRevision) {
        Write-Host ""
        Write-Host "[OK] Already current." -ForegroundColor Green

        if (-not $NoPause) {
            Write-Host ""
            Read-Host "Press Enter to close"
        }

        $global:LASTEXITCODE = 0
        return
    }
} else {
    Write-Host "[WARNING] Could not resolve the exact dev commit SHA."
    Write-Host "[INFO] Continuing with a full dev-archive refresh instead of aborting."
}

$tempRoot = Join-Path $env:TEMP ("Spidey2000Update-" + [Guid]::NewGuid().ToString("N"))
$zipPath = Join-Path $tempRoot "dev.zip"
$extractRoot = Join-Path $tempRoot "extract"

try {
    New-Item -ItemType Directory -Force -Path $extractRoot | Out-Null

    Write-Host "[..] Downloading dev branch archive..."

    if (-not (Download-DevArchive -Url $archiveUrl -OutputPath $zipPath -Headers $headers)) {
        Fail "Could not download the dev branch archive after all retry/fallback methods."
    }

    $archiveHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $zipPath).Hash.ToLowerInvariant()
    $resolvedRevision = $remoteRevision

    if (-not $resolvedRevision) {
        $resolvedRevision = "archive-" + $archiveHash.Substring(0, 16)
        Write-Host ("[INFO] Archive identity: " + $resolvedRevision)
    }

    Write-Host "[..] Extracting..."

    try {
        Expand-Archive -LiteralPath $zipPath -DestinationPath $extractRoot -Force
    } catch {
        Fail ("Could not extract the downloaded dev archive: " + $_.Exception.Message)
    }

    $sourceRoot = Join-Path $extractRoot "spidey-decomp-dev"

    if (-not (Test-Path (Join-Path $sourceRoot "README.md"))) {
        Fail "Downloaded archive did not contain the expected spidey-decomp-dev root."
    }

    $robocopyCandidates = @(
        (Join-Path $env:SystemRoot "System32\robocopy.exe"),
        (Join-Path $env:SystemRoot "Sysnative\robocopy.exe")
    )

    $robocopy = $robocopyCandidates | Where-Object { Test-Path -LiteralPath $_ } | Select-Object -First 1

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

    Set-Content -Path $revisionFile -Value $resolvedRevision -Encoding ASCII

    Write-Host ""
    Write-Host "[OK] Local project is current." -ForegroundColor Green

    if ($remoteRevision) {
        Write-Host ("Revision: " + $remoteRevision)
    } else {
        Write-Host ("Revision identity: " + $resolvedRevision)
        Write-Host "[WARNING] Exact Git commit SHA was unavailable during this update."
    }
} finally {
    if (Test-Path $tempRoot) {
        Remove-Item -Recurse -Force $tempRoot -ErrorAction SilentlyContinue
    }
}

if (-not $NoPause) {
    Write-Host ""
    Read-Host "Press Enter to close"
}

$global:LASTEXITCODE = 0
return
