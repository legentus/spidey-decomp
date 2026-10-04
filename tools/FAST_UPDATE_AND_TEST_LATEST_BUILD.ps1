param()

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

function Read-LocalRevision {
    $path = Join-Path $RepoRoot "LOCAL_DEV_REVISION.txt"
    if (-not (Test-Path -LiteralPath $path)) {
        return ""
    }

    return (Get-Content -Raw -LiteralPath $path).Trim()
}

function Test-GitSha([string]$Value) {
    return [bool]($Value -match '^[0-9a-fA-F]{40}$')
}

function Touch-SourceFile([string]$RelativePath) {
    $full = Join-Path $RepoRoot ($RelativePath -replace '/', '\')
    if (Test-Path -LiteralPath $full) {
        (Get-Item -LiteralPath $full).LastWriteTimeUtc = [DateTime]::UtcNow
        Write-Host ("[FAST] Marked changed source for incremental rebuild: " + $RelativePath)
        return $true
    }

    return $false
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 Dev - FAST Update + Test"
Write-Host "============================================================"
Write-Host ""

$beforeRevision = Read-LocalRevision
Write-Host ("[INFO] Before update: " + $(if ($beforeRevision) { $beforeRevision } else { "<unknown>" }))

Write-Host ""
Write-Host "[1/2] Checking/updating dev..."
& (Join-Path $PSScriptRoot "UPDATE_SPIDEY_PROJECT.ps1") -NoPause
if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "[ERROR] Project update failed." -ForegroundColor Red
    Read-Host "Press Enter to close"
    exit $LASTEXITCODE
}

$afterRevision = Read-LocalRevision
Write-Host ("[INFO] After update:  " + $(if ($afterRevision) { $afterRevision } else { "<unknown>" }))

# Conservative defaults. If exact revisions/compare data are unavailable,
# the fast launcher deliberately falls back to the normal safe rebuild.
$forceCleanProxy = $true
$rebuildRenderer11 = $true
$rebuildInput11 = $true
$allowExistingProxy = $false
$compareSucceeded = $false

if ((Test-GitSha $beforeRevision) -and (Test-GitSha $afterRevision)) {
    if ($beforeRevision -eq $afterRevision) {
        Write-Host ""
        Write-Host "[FAST] Already on the same dev revision."
        Write-Host "[FAST] Reusing existing bridge artifacts and allowing an unchanged proxy."
        $forceCleanProxy = $false
        $rebuildRenderer11 = $false
        $rebuildInput11 = $false
        $allowExistingProxy = $true
        $compareSucceeded = $true
    } else {
        $compareUrl = "https://api.github.com/repos/legentus/spidey-decomp/compare/$beforeRevision...$afterRevision"
        $headers = @{ "User-Agent" = "Spider-Man-2000-Fast-Tester" }

        try {
            Write-Host ""
            Write-Host "[FAST] Resolving changed files between revisions..."
            $compare = Invoke-RestMethod -Uri $compareUrl -Headers $headers -UseBasicParsing -TimeoutSec 30
            $changedFiles = @($compare.files)

            # GitHub's compare file list can be capped for extremely large deltas.
            # Treat a 300-file response as potentially incomplete and use the safe path.
            if ($changedFiles.Count -ge 300) {
                throw "Compare returned 300 files; using conservative full rebuild."
            }

            $compareSucceeded = $true
            $forceCleanProxy = $false
            $rebuildRenderer11 = $false
            $rebuildInput11 = $false

            foreach ($file in $changedFiles) {
                $path = [string]$file.filename
                if (-not $path) {
                    continue
                }

                $lower = $path.ToLowerInvariant()
                $ext = [System.IO.Path]::GetExtension($lower)

                if ($lower.StartsWith("renderer11/") -or
                    $lower -eq "scripts/build_renderer11.ps1" -or
                    $lower -eq "renderer11_legacy_bridge.h") {
                    $rebuildRenderer11 = $true
                }

                if ($lower.StartsWith("input11/") -or
                    $lower -eq "scripts/build_input11.ps1" -or
                    $lower -eq "input11_legacy_bridge.h") {
                    $rebuildInput11 = $true
                }

                # Files that do not participate in the matching proxy build can be ignored.
                if ($lower.StartsWith("docs/") -or
                    $lower.StartsWith(".github/") -or
                    $lower.StartsWith("tests/") -or
                    $lower.StartsWith("tools/") -or
                    $lower.StartsWith("renderer11/") -or
                    $lower.StartsWith("input11/") -or
                    $lower.EndsWith(".md") -or
                    $lower.EndsWith(".txt") -or
                    $lower.EndsWith(".yml") -or
                    $lower.EndsWith(".yaml") -or
                    $lower.EndsWith(".json")) {
                    continue
                }

                if ($ext -eq ".cpp" -or $ext -eq ".c") {
                    [void](Touch-SourceFile $path)
                    continue
                }

                # Header/build-system/resource changes can affect many objects.
                # A clean build is safer than trying to guess dependency fan-out.
                if ($ext -eq ".h" -or
                    $ext -eq ".hpp" -or
                    $ext -eq ".inl" -or
                    $ext -eq ".mak" -or
                    $ext -eq ".rc" -or
                    $ext -eq ".def" -or
                    $lower -eq "build.bat" -or
                    $lower -eq "spider.mak") {
                    $forceCleanProxy = $true
                }
            }

            # The runtime revision string changes at every new dev commit.
            # Touching main.cpp guarantees the incremental matching build embeds
            # the new revision even when the source archive carries old timestamps.
            [void](Touch-SourceFile "main.cpp")

            Write-Host ""
            Write-Host ("[FAST] Changed files: " + $changedFiles.Count)
            Write-Host ("[FAST] Proxy clean build required: " + $forceCleanProxy)
            Write-Host ("[FAST] Rebuild Renderer11: " + $rebuildRenderer11)
            Write-Host ("[FAST] Rebuild Input11:    " + $rebuildInput11)
        } catch {
            Write-Host ""
            Write-Host ("[WARNING] Could not safely resolve incremental delta: " + $_.Exception.Message)
            Write-Host "[INFO] Falling back to the normal clean/bridge rebuild path for this run."
            $forceCleanProxy = $true
            $rebuildRenderer11 = $true
            $rebuildInput11 = $true
            $allowExistingProxy = $false
        }
    }
} else {
    Write-Host ""
    Write-Host "[INFO] Exact before/after Git SHAs are unavailable."
    Write-Host "[INFO] Using the normal safe rebuild path for this run."
}

$env:SPIDEY_FAST_FORCE_CLEAN = $(if ($forceCleanProxy) { "1" } else { "0" })
$env:SPIDEY_FAST_REBUILD_RENDERER11 = $(if ($rebuildRenderer11) { "1" } else { "0" })
$env:SPIDEY_FAST_REBUILD_INPUT11 = $(if ($rebuildInput11) { "1" } else { "0" })
$env:SPIDEY_FAST_ALLOW_EXISTING_PROXY = $(if ($allowExistingProxy) { "1" } else { "0" })

Write-Host ""
Write-Host "[2/2] Building/installing/launching latest test..."
Write-Host ""

& (Join-Path $PSScriptRoot "TEST_LATEST_BUILD.ps1") -PostUpdate -Fast
exit $LASTEXITCODE
