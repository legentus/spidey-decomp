[CmdletBinding()]
param(
    [switch]$NoPause
)

$ErrorActionPreference = "Stop"
$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
Set-Location $RepoRoot

function Pause-IfNeeded {
    if (-not $NoPause) {
        Write-Host ""
        Read-Host "Press Enter to close"
    }
}

function Fail([string]$Message, [int]$Code = 1) {
    Write-Host ""
    Write-Host "[ERROR] $Message" -ForegroundColor Red
    Pause-IfNeeded
    exit $Code
}

function Git([Parameter(ValueFromRemainingArguments = $true)][string[]]$Args) {
    $output = & git @Args 2>&1
    $code = $LASTEXITCODE
    if ($code -ne 0) {
        if ($output) { $output | ForEach-Object { Write-Host $_ } }
        throw "git $($Args -join ' ') failed with exit code $code."
    }
    return $output
}

function Git-Text([string[]]$Args) {
    return ((Git @Args) -join [Environment]::NewLine).Trim()
}

function Test-Ancestor([string]$Older, [string]$Newer) {
    & git merge-base --is-ancestor $Older $Newer 2>$null
    return ($LASTEXITCODE -eq 0)
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 - Claude Decomp Safe Sync"
Write-Host "============================================================"
Write-Host ""
Write-Host "[POLICY] Local dev is authoritative."
Write-Host "[POLICY] Claude works only on origin/claude-decomp."
Write-Host "[POLICY] No force pushes, no automatic conflict resolution."
Write-Host ""

if (-not (Get-Command git -ErrorAction SilentlyContinue)) {
    Fail "git was not found on PATH."
}

try {
    $inside = Git-Text @("rev-parse", "--is-inside-work-tree")
} catch {
    Fail "This folder is not a Git working tree."
}
if ($inside -ne "true") {
    Fail "This folder is not a Git working tree."
}

$branch = Git-Text @("branch", "--show-current")
if ($branch -ne "dev") {
    Fail "Expected local branch 'dev', but current branch is '$branch'. Switch back to dev before syncing."
}

$dirtyParts = @()

& git diff --quiet
if ($LASTEXITCODE -ne 0) {
    $dirtyParts += (& git diff --name-only)
}

& git diff --cached --quiet
if ($LASTEXITCODE -ne 0) {
    $dirtyParts += (& git diff --cached --name-only)
}

$untracked = @(& git ls-files --others --exclude-standard)
if ($LASTEXITCODE -ne 0) {
    Fail "Could not inspect untracked files."
}
$dirtyParts += $untracked

$dirtyParts = @($dirtyParts | Where-Object { $_ } | Sort-Object -Unique)
if ($dirtyParts.Count -gt 0) {
    Write-Host "[BLOCKED] Local working tree is not clean:" -ForegroundColor Yellow
    $dirtyParts | ForEach-Object { Write-Host ("  " + $_) }
    Fail "Commit, stash, or intentionally remove local changes before syncing. Nothing was fetched/merged into the working tree."
}

Write-Host "[1/7] Fetching remote refs..."
try {
    Git @("fetch", "origin", "--prune") | Out-Null
} catch {
    Fail $_.Exception.Message
}

try {
    $localHead = Git-Text @("rev-parse", "HEAD")
    $remoteDev = Git-Text @("rev-parse", "origin/dev")
    $claudeTip = Git-Text @("rev-parse", "origin/claude-decomp")
} catch {
    Fail "Required refs origin/dev and origin/claude-decomp must both exist."
}

Write-Host ("[INFO] local dev:      " + $localHead)
Write-Host ("[INFO] origin/dev:     " + $remoteDev)
Write-Host ("[INFO] Claude snapshot: " + $claudeTip)

$remoteDevInLocal = Test-Ancestor $remoteDev $localHead
$localInRemoteDev = Test-Ancestor $localHead $remoteDev

if (-not $remoteDevInLocal) {
    if ($localInRemoteDev) {
        Fail "origin/dev is ahead of local dev. This script will not silently pull unexpected dev changes. Ask ChatGPT to reconcile it first."
    }
    Fail "local dev and origin/dev have diverged. Automatic sync is intentionally blocked."
}

if ($localHead -ne $remoteDev) {
    Write-Host "[2/7] Publishing our local dev commits so Claude can see the current frontier..."
    try {
        Git @("push", "origin", "dev") | Out-Null
    } catch {
        Fail "Could not fast-forward origin/dev. No force push was attempted. Re-fetch and reconcile manually."
    }
    Git @("fetch", "origin", "--prune") | Out-Null
    $remoteDev = Git-Text @("rev-parse", "origin/dev")
} else {
    Write-Host "[2/7] origin/dev already contains our local frontier."
}

if (Test-Ancestor $claudeTip $localHead) {
    Write-Host "[3/7] Claude snapshot is already contained in local dev; nothing new to merge."
    Write-Host "[4/7] No backup branch needed."
    Write-Host "[5/7] No merge needed."
} else {
    $stamp = Get-Date -Format "yyyyMMdd-HHmmss"
    $backupBranch = "backup/pre-claude-sync-$stamp"

    Write-Host ("[3/7] Creating rollback branch " + $backupBranch + " ...")
    try {
        Git @("branch", $backupBranch, $localHead) | Out-Null
    } catch {
        Fail "Could not create the local rollback branch."
    }

    Write-Host ("[4/7] Merging pinned Claude snapshot " + $claudeTip.Substring(0,8) + " ...")
    & git merge --no-ff --no-edit $claudeTip
    $mergeCode = $LASTEXITCODE

    if ($mergeCode -ne 0) {
        $conflicts = (& git diff --name-only --diff-filter=U 2>$null) -join [Environment]::NewLine
        Write-Host ""
        Write-Host "[CONFLICT] Claude changes overlap our local work." -ForegroundColor Yellow
        if ($conflicts) {
            Write-Host "Conflicting files:"
            Write-Host $conflicts
        }
        Write-Host ("Rollback branch preserved: " + $backupBranch)
        & git merge --abort 2>$null
        if ($LASTEXITCODE -ne 0) {
            Fail "Merge conflicted and git merge --abort also failed. Do not continue editing; ask ChatGPT to inspect the repo." 3
        }
        Fail "Merge was safely aborted. Local dev is back where it started; no conflict was auto-resolved." 2
    }

    Write-Host "[5/7] Merge completed successfully."
    $diffCheck = & git diff --check HEAD^1..HEAD 2>&1
    if ($LASTEXITCODE -ne 0) {
        Write-Host "[WARNING] git diff --check reported whitespace/errors in the merged change:" -ForegroundColor Yellow
        $diffCheck | ForEach-Object { Write-Host $_ }
        Write-Host "[WARNING] Merge is retained, but review these lines before runtime testing."
    }
}

Write-Host "[6/7] Re-fetching before publishing integrated dev..."
try {
    Git @("fetch", "origin", "--prune") | Out-Null
} catch {
    Fail $_.Exception.Message
}

$currentHead = Git-Text @("rev-parse", "HEAD")
$latestRemoteDev = Git-Text @("rev-parse", "origin/dev")

if (-not (Test-Ancestor $latestRemoteDev $currentHead)) {
    Fail "origin/dev changed while the sync was running. Integrated work is safe locally, but this script will not overwrite the newer remote. Ask ChatGPT to reconcile."
}

try {
    Git @("push", "origin", "dev") | Out-Null
} catch {
    Fail "Integrated local dev is safe, but push to origin/dev failed. No force push was attempted."
}

Git @("fetch", "origin", "--prune") | Out-Null
$latestClaude = Git-Text @("rev-parse", "origin/claude-decomp")
$finalHead = Git-Text @("rev-parse", "HEAD")

Write-Host "[7/7] Sync complete." -ForegroundColor Green
Write-Host ("[OK] local/origin dev: " + $finalHead)

if ($latestClaude -ne $claudeTip -and -not (Test-Ancestor $latestClaude $finalHead)) {
    Write-Host ""
    Write-Host "[NOTE] Claude pushed additional commits while this sync was running." -ForegroundColor Yellow
    Write-Host ("       New Claude tip: " + $latestClaude)
    Write-Host "       Those commits were NOT mixed into this snapshot. Run the BAT again after Claude finishes."
}

Write-Host ""
Write-Host "For Claude's next batch, tell it to:"
Write-Host "  1. git fetch origin"
Write-Host "  2. git checkout claude-decomp"
Write-Host "  3. git merge --ff-only origin/dev"
Write-Host "  4. decompile/commit/push only to claude-decomp"
Write-Host ""
Write-Host "Do NOT let Claude push directly to dev or master."
Pause-IfNeeded
