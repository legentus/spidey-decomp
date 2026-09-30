param(
    [switch]$PostUpdate,
    [switch]$Elevated
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

function Test-DirectoryWritable([string]$Path) {
    $probe = Join-Path $Path (".spidey-write-test-" + [Guid]::NewGuid().ToString("N") + ".tmp")
    try {
        [System.IO.File]::WriteAllBytes($probe, [byte[]]@())
        Remove-Item -LiteralPath $probe -Force -ErrorAction SilentlyContinue
        return $true
    } catch {
        Remove-Item -LiteralPath $probe -Force -ErrorAction SilentlyContinue
        return $false
    }
}

function Get-PeFingerprint([string]$Path) {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $Path).Hash
    $length = (Get-Item -LiteralPath $Path).Length

    $fs = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read, [System.IO.FileShare]::ReadWrite)
    $br = New-Object System.IO.BinaryReader($fs)

    try {
        if ($br.ReadUInt16() -ne 0x5A4D) {
            throw "Missing MZ header."
        }

        $fs.Position = 0x3C
        $peOffset = $br.ReadInt32()
        $fs.Position = $peOffset

        if ($br.ReadUInt32() -ne 0x00004550) {
            throw "Missing PE signature."
        }

        $machine = $br.ReadUInt16()
        $numberOfSections = $br.ReadUInt16()
        $timeDateStamp = $br.ReadUInt32()
        [void]$br.ReadUInt32()
        [void]$br.ReadUInt32()
        $sizeOfOptionalHeader = $br.ReadUInt16()
        $characteristics = $br.ReadUInt16()

        $optionalStart = $fs.Position
        $magic = $br.ReadUInt16()
        if ($magic -ne 0x10B) {
            throw ("Expected PE32 executable, optional-header magic was 0x{0:X4}." -f $magic)
        }

        $fs.Position = $optionalStart + 16
        $entryPoint = $br.ReadUInt32()

        $fs.Position = $optionalStart + 28
        $imageBase = $br.ReadUInt32()

        $fs.Position = $optionalStart + 56
        $sizeOfImage = $br.ReadUInt32()

        $sectionTable = $optionalStart + $sizeOfOptionalHeader
        $sections = @()

        for ($i = 0; $i -lt $numberOfSections; $i++) {
            $fs.Position = $sectionTable + ($i * 40)
            $nameBytes = $br.ReadBytes(8)
            $name = ([System.Text.Encoding]::ASCII.GetString($nameBytes)).Trim([char]0)
            $virtualSize = $br.ReadUInt32()
            $virtualAddress = $br.ReadUInt32()
            $rawSize = $br.ReadUInt32()
            $rawPointer = $br.ReadUInt32()
            $sections += [PSCustomObject]@{
                Name = $name
                VirtualSize = $virtualSize
                VirtualAddress = $virtualAddress
                RawSize = $rawSize
                RawPointer = $rawPointer
            }
        }

        return [PSCustomObject]@{
            Sha256 = $hash
            Length = $length
            Machine = $machine
            TimeDateStamp = $timeDateStamp
            EntryPointRva = $entryPoint
            ImageBase = $imageBase
            SizeOfImage = $sizeOfImage
            Characteristics = $characteristics
            Sections = $sections
        }
    } finally {
        $br.Close()
        $fs.Close()
    }
}

function Write-PeFingerprint([string]$Path, [string]$OutputPath) {
    $pe = Get-PeFingerprint $Path

    $lines = @(
        ("path=" + $Path),
        ("sha256=" + $pe.Sha256),
        ("file_size=" + $pe.Length),
        ("machine=0x{0:X4}" -f $pe.Machine),
        ("timestamp=0x{0:X8}" -f $pe.TimeDateStamp),
        ("entrypoint_rva=0x{0:X8}" -f $pe.EntryPointRva),
        ("image_base=0x{0:X8}" -f $pe.ImageBase),
        ("size_of_image=0x{0:X8}" -f $pe.SizeOfImage),
        ("characteristics=0x{0:X4}" -f $pe.Characteristics),
        ""
    )

    foreach ($s in $pe.Sections) {
        $lines += ("section={0} va=0x{1:X8} vsize=0x{2:X8} raw=0x{3:X8} rawsize=0x{4:X8}" -f
            $s.Name, $s.VirtualAddress, $s.VirtualSize, $s.RawPointer, $s.RawSize)
    }

    $lines | Set-Content -Path $OutputPath -Encoding ASCII
    return $pe
}

function Relaunch-Elevated {
    $psExe = (Get-Process -Id $PID).Path
    $argLine = '-NoProfile -ExecutionPolicy Bypass -File "' + $PSCommandPath + '" -PostUpdate -Elevated'

    Write-Host ""
    Write-Host "[INFO] The Spider-Man game folder requires Administrator access."
    Write-Host "[..] Requesting elevation so the retail Bink DLL can be preserved and the dev proxy installed..."

    try {
        $proc = Start-Process -FilePath $psExe -Verb RunAs -ArgumentList $argLine -Wait -PassThru
        exit $proc.ExitCode
    } catch {
        Stop-WithPause "Administrator elevation was cancelled or failed: $($_.Exception.Message)"
    }
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

if (-not (Test-DirectoryWritable $gameDir)) {
    if ($Elevated) {
        Stop-WithPause "The game folder is still not writable even after elevation: '$gameDir'."
    }
    Relaunch-Elevated
}

if ($Elevated) {
    Write-Host "[OK] Elevated access confirmed for the game folder."
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
    $env:SPIDEY_FORCE_CLEAN = "1"
    $buildStartedUtc = [DateTime]::UtcNow

    Write-Host ""
    Write-Host "[..] Building matching proxy (forced clean build)..."
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

Write-Host ""
Write-Host "[..] Building Direct3D 11 renderer bridge..."
try {
    & (Join-Path $RepoRoot "scripts\build_renderer11.ps1")
    if ($LASTEXITCODE -ne 0) {
        Stop-WithPause "Direct3D 11 renderer build failed." $LASTEXITCODE
    }
} catch {
    Stop-WithPause ("Direct3D 11 renderer build failed: " + $_.Exception.Message)
}

$renderer11Dll = Join-Path $RepoRoot "out\renderer11\spidey_renderer11.dll"
if (-not (Test-Path $renderer11Dll)) {
    Stop-WithPause "Direct3D 11 renderer build completed but spidey_renderer11.dll was not produced."
}
$renderer11Hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $renderer11Dll).Hash
Write-Host "[OK] Renderer11 SHA-256: $renderer11Hash"

$builtDll = Join-Path $RepoRoot "Release\spider.dll"
if (-not (Test-Path $builtDll)) {
    Stop-WithPause "Build completed but Release\spider.dll was not produced."
}

$builtInfo = Get-Item -LiteralPath $builtDll
if ($builtInfo.LastWriteTimeUtc -lt $buildStartedUtc.AddSeconds(-2)) {
    Stop-WithPause ("Build returned success, but Release\spider.dll was not freshly regenerated. " +
        "DLL timestamp: " + $builtInfo.LastWriteTimeUtc.ToString("o") +
        "; build started: " + $buildStartedUtc.ToString("o"))
}
Write-Host ("[OK] Fresh DLL timestamp: " + $builtInfo.LastWriteTimeUtc.ToString("o"))

$outDir = Join-Path $RepoRoot "out\matching"
New-Item -ItemType Directory -Force -Path $outDir | Out-Null
$proxyDll = Join-Path $outDir "binkw32.dll"
Copy-Item $builtDll $proxyDll -Force

$pdb = Join-Path $RepoRoot "Release\spider.pdb"
if (Test-Path $pdb) {
    Copy-Item $pdb (Join-Path $outDir "spider.pdb") -Force
}

$linkMap = Join-Path $RepoRoot "Release\spider.map"
if (Test-Path $linkMap) {
    Copy-Item $linkMap (Join-Path $outDir "spider-link-map.txt") -Force
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

$liveRenderer11 = Join-Path $gameDir "spidey_renderer11.dll"
Copy-Item -LiteralPath $renderer11Dll -Destination $liveRenderer11 -Force
Write-Host "[OK] Installed Direct3D 11 renderer bridge as spidey_renderer11.dll."

$logRoot = Join-Path $RepoRoot "logs"
New-Item -ItemType Directory -Force -Path $logRoot | Out-Null
$stamp = Get-Date -Format "yyyyMMdd-HHmmss"
$sessionDir = Join-Path $logRoot $stamp
New-Item -ItemType Directory -Force -Path $sessionDir | Out-Null

Write-Host "[..] Fingerprinting SpideyPC.exe..."
try {
    $peInfo = Write-PeFingerprint $gameExe (Join-Path $sessionDir "game-exe-fingerprint.txt")
    Write-Host ("[INFO] EXE SHA-256: " + $peInfo.Sha256)
    Write-Host ("[INFO] PE timestamp: 0x{0:X8}" -f $peInfo.TimeDateStamp)
    Write-Host ("[INFO] Image size: 0x{0:X8}" -f $peInfo.SizeOfImage)
} catch {
    Stop-WithPause ("Failed to fingerprint SpideyPC.exe: " + $_.Exception.Message)
}

@(
    "revision=$revision",
    "proxy_sha256=$hash",
    "renderer11_sha256=$renderer11Hash",
    "game=$gameExe",
    "started=$(Get-Date -Format o)"
) | Set-Content -Path (Join-Path $sessionDir "test-session.txt") -Encoding UTF8

if (Test-Path $linkMap) {
    Copy-Item $linkMap (Join-Path $sessionDir "proxy-link-map.txt") -Force
    Write-Host "[MAP] Proxy linker map captured:"
    Write-Host ("  " + (Join-Path $sessionDir "proxy-link-map.txt"))
}

$crashLog = Join-Path $gameDir "spidey-decomp-crash.log"
$dxErrorLog = Join-Path $gameDir "spidey-decomp-dxerror.log"
$compatLog = Join-Path $gameDir "spidey-decomp-compat.log"
$presentLog = Join-Path $gameDir "spidey-decomp-present.log"
$textureLog = Join-Path $gameDir "spidey-decomp-texture.log"
$drawLog = Join-Path $gameDir "spidey-decomp-draw.log"
$inputLog = Join-Path $gameDir "spidey-decomp-input.log"
$renderer11Log = Join-Path $gameDir "spidey-renderer11.log"
$runtimeLog = Join-Path $gameDir "spidey-decomp-runtime.log"

if (Test-Path $crashLog) {
    Remove-Item -LiteralPath $crashLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $dxErrorLog) {
    Remove-Item -LiteralPath $dxErrorLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $compatLog) {
    Remove-Item -LiteralPath $compatLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $presentLog) {
    Remove-Item -LiteralPath $presentLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $textureLog) {
    Remove-Item -LiteralPath $textureLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $drawLog) {
    Remove-Item -LiteralPath $drawLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $inputLog) {
    Remove-Item -LiteralPath $inputLog -Force -ErrorAction SilentlyContinue
}
if (Test-Path $renderer11Log) {
    Remove-Item -LiteralPath $renderer11Log -Force -ErrorAction SilentlyContinue
}
if (Test-Path $runtimeLog) {
    Remove-Item -LiteralPath $runtimeLog -Force -ErrorAction SilentlyContinue
}

Write-Host ""
Write-Host "[RUN] $gameExe"
Write-Host "[LOG] $sessionDir"
$gameProcess = Start-Process -FilePath $gameExe -WorkingDirectory $gameDir -PassThru

Write-Host ""
Write-Host "[OK] Latest dev build installed and launched." -ForegroundColor Green
Write-Host "[INFO] Waiting for Spider-Man to exit so crash diagnostics can be collected."
Write-Host ""

$gameProcess.WaitForExit()
$exitCode = $gameProcess.ExitCode
Write-Host ("[INFO] Spider-Man exited with code " + $exitCode + ".")

if (Test-Path $crashLog) {
    Copy-Item -LiteralPath $crashLog -Destination (Join-Path $sessionDir "spidey-decomp-crash.log") -Force
    Write-Host "[CRASH] Native crash log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-crash.log"))
}

if (Test-Path $dxErrorLog) {
    Copy-Item -LiteralPath $dxErrorLog -Destination (Join-Path $sessionDir "spidey-decomp-dxerror.log") -Force
    Write-Host "[DXERR] DirectX error log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-dxerror.log"))
}

if (Test-Path $compatLog) {
    Copy-Item -LiteralPath $compatLog -Destination (Join-Path $sessionDir "spidey-decomp-compat.log") -Force
    Write-Host "[COMPAT] DirectDraw compatibility log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-compat.log"))
}

if (Test-Path $presentLog) {
    Copy-Item -LiteralPath $presentLog -Destination (Join-Path $sessionDir "spidey-decomp-present.log") -Force
    Write-Host "[PRESENT] DirectDraw presentation log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-present.log"))
}

if (Test-Path $textureLog) {
    Copy-Item -LiteralPath $textureLog -Destination (Join-Path $sessionDir "spidey-decomp-texture.log") -Force
    Write-Host "[TEXTURE] Texture conversion log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-texture.log"))
}

if (Test-Path $drawLog) {
    Copy-Item -LiteralPath $drawLog -Destination (Join-Path $sessionDir "spidey-decomp-draw.log") -Force
    Write-Host "[DRAW] Retail D3D7 draw probe log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-draw.log"))
}

if (Test-Path $inputLog) {
    Copy-Item -LiteralPath $inputLog -Destination (Join-Path $sessionDir "spidey-decomp-input.log") -Force
    Write-Host "[INPUT] DirectInput focus/reacquire log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-input.log"))
}

if (Test-Path $renderer11Log) {
    Copy-Item -LiteralPath $renderer11Log -Destination (Join-Path $sessionDir "spidey-renderer11.log") -Force
    Write-Host "[DX11] Direct3D 11 renderer log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-renderer11.log"))
}

if (Test-Path $runtimeLog) {
    Copy-Item -LiteralPath $runtimeLog -Destination (Join-Path $sessionDir "spidey-decomp-runtime.log") -Force
    Write-Host "[RUNTIME] Runtime assertion log captured:"
    Write-Host ("  " + (Join-Path $sessionDir "spidey-decomp-runtime.log"))
}

if ($exitCode -eq -1073741819) {
    Write-Host "[CRASH] Exit code is 0xC0000005 (access violation)."
}
elseif ($exitCode -eq -1073741571) {
    Write-Host "[CRASH] Exit code is 0xC00000FD (stack overflow)."
}

Write-Host ""
Read-Host "Press Enter to close this launcher"
