[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$SourceDir = Join-Path $RepoRoot "renderer11"
$BuildDir = Join-Path $RepoRoot "out\renderer11\build"
$ArtifactDir = Join-Path $RepoRoot "out\renderer11"
$Artifact = Join-Path $ArtifactDir "spidey_renderer11.dll"

$cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
$cmakePath = $null

if ($cmakeCommand) {
    $cmakePath = $cmakeCommand.Source
} else {
    $known = "C:\Program Files\CMake\bin\cmake.exe"
    if (Test-Path $known) {
        $cmakePath = $known
    }
}

if (-not $cmakePath) {
    throw "CMake was not found. Install CMake or add cmake.exe to PATH."
}

# The user's ZIP-based updater intentionally preserves out\ so normal build
# artifacts/logs survive a source refresh. GitHub archive extraction can give
# refreshed source files timestamps that are older than an existing CMake
# object file, however, which can make MSBuild incorrectly reuse stale renderer
# objects after an API/export change. Always regenerate the modern renderer
# build tree from scratch so the DLL is guaranteed to match the checked-out
# source revision.
if (Test-Path -LiteralPath $BuildDir) {
    Write-Host "[..] Removing cached Direct3D 11 build tree..."
    Remove-Item -LiteralPath $BuildDir -Recurse -Force
}

New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null
New-Item -ItemType Directory -Path $ArtifactDir -Force | Out-Null

if (Test-Path -LiteralPath $Artifact) {
    Remove-Item -LiteralPath $Artifact -Force
}

$artifactPdb = Join-Path $ArtifactDir "spidey_renderer11.pdb"
if (Test-Path -LiteralPath $artifactPdb) {
    Remove-Item -LiteralPath $artifactPdb -Force
}

Write-Host "============================================================"
Write-Host "  Spider-Man 2000 - Direct3D 11 Renderer Build"
Write-Host "============================================================"
Write-Host "Source: $SourceDir"
Write-Host "Build:  $BuildDir"
Write-Host "CMake:  $cmakePath"
Write-Host ""

$configureArgs = @(
    "-S", $SourceDir,
    "-B", $BuildDir,
    "-G", "Visual Studio 17 2022",
    "-A", "Win32"
)

& $cmakePath @configureArgs
if ($LASTEXITCODE -ne 0) {
    throw "DX11 renderer CMake configure failed with exit code $LASTEXITCODE."
}

$buildArgs = @(
    "--build", $BuildDir,
    "--config", "Release",
    "--parallel"
)

& $cmakePath @buildArgs
if ($LASTEXITCODE -ne 0) {
    throw "DX11 renderer build failed with exit code $LASTEXITCODE."
}

$built = Join-Path $BuildDir "Release\spidey_renderer11.dll"
if (-not (Test-Path $built)) {
    throw "DX11 renderer build returned success but '$built' was not produced."
}

Copy-Item -LiteralPath $built -Destination $Artifact -Force

$pdb = Join-Path $BuildDir "Release\spidey_renderer11.pdb"
if (Test-Path $pdb) {
    Copy-Item -LiteralPath $pdb -Destination (Join-Path $ArtifactDir "spidey_renderer11.pdb") -Force
}

$hash = Get-FileHash -Algorithm SHA256 -LiteralPath $Artifact

Write-Host ""
Write-Host "[OK] Direct3D 11 renderer build complete."
Write-Host "Artifact: $Artifact"
Write-Host "SHA-256: $($hash.Hash)"
