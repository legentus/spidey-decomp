# CURRENT STATUS

**Project:** Spider-Man 2000 PC decomp / developer build  
**Repository:** `legentus/spidey-decomp`  
**Working branch:** `dev`  
**Upstream-sync branch:** `master`  
**Upstream baseline:** `krystalgamer/spidey-decomp`  
**Baseline commit:** `4eb5635bf1e0aae9edff771d3b9b0db16971ff28` — `patch: CFT4Bit::SetTransparency`  
**Last live update:** 2026-09-29

## Immediate Goal

Establish **Spider-Man 2000 Developer Build 0.1**:

1. Reproduce the existing matching/decomp Windows build.
2. Install it against the user's exact retail PC game.
3. Prove rebuilt C++ executes reliably.
4. Preserve a clean recovery path to the stock game.
5. Select and fix the first real bug only after the baseline works.

## Repository Policy

- Keep `master` clean for upstream synchronization.
- Put our active development work on `dev` and feature branches.
- Do not commit original retail game binaries or PKR assets.
- Preserve original matching/reconstruction work where possible.
- Separate intentional modernization / developer functionality from matching work.
- No fabricated addresses, layouts, offsets, or runtime state; verify before patching.

## Live-Documentation Rule

This file is updated **during work**, not only at the end of a chat. After any meaningful discovery, failed path, code change, build frontier, or test result, update this file so work can resume after an interruption.

## Verified State

- User fork exists and is writable.
- `master` is the upstream-sync branch.
- `dev` was created from upstream head `4eb5635bf1e0aae9edff771d3b9b0db16971ff28`.
- First live-status commit on `dev`: `b2a2beb8f49ec89693ef556f031d5f26d5b92a30`.
- The Windows matching build is the preserved old Visual Studio/NMAKE project (`spider.mak` / `build.bat`), **not** the CMake target.
- CMake currently builds a portability executable; upstream CI uses the old Windows project to produce `Release\\spider.dll`.
- CI renames/copies `Release\\spider.dll` to `binkw32.dll` and validates it with Tobey Validator.

## Bootstrap Architecture — VERIFIED FROM SOURCE

The current reconstructed Windows build is an incremental **Bink proxy + live EXE patcher**:

1. Retail `SpideyPC.exe` normally loads `binkw32.dll`.
2. The reconstructed `spider.dll` is installed/renamed as `binkw32.dll`.
3. The original retail Bink DLL is expected to exist as `binkw32_.dll`.
4. `forwards.h` forwards the retail Bink export surface to `binkw32_.dll`.
5. Our DLL's `DllMain` runs on `DLL_PROCESS_ATTACH`.
6. It allocates a console, sets the title to `spidey-decomp - <commit>`, runs runtime structure assertions, makes the retail EXE text range writable, applies `game_patches()`, then restores protection.
7. Patch macros in `my_patch.h` redirect selected retail function entries/calls to reconstructed C++ functions in the proxy DLL.
8. This lets the project replace reconstructed functions incrementally while the rest of the original executable remains intact.

### Important implication

We **do not need a standalone fully rebuilt executable before fixing bugs**. We can validate one reconstructed subsystem/function at a time inside the retail game.

## Current Frontier

**ACTIVE:** perform the first baseline build/install/launch from `dev` before making gameplay-source changes.

CI workflow was updated on `dev` at commit `9748899a10ce5ac708f54e4f9c7d8c4de3845351` to:
- run on pushes to `dev`;
- allow manual `workflow_dispatch`;
- allow PR validation targeting `dev`.

**Observed after the push:** GitHub's Actions-runs API currently reports zero runs for branch `dev`, and the commit has no combined status entries. Therefore the CI build is **not yet verified**. Do not assume the DLL was built. This may require enabling Actions for the newly created fork or another workflow-side fix; exact cause not yet proven.

To avoid blocking progress on fork Actions initialization, a local one-command Windows matching-build path has now been added and documented.

### Added build/install helpers

- `scripts/setup_matching_toolchain.ps1` — commit `5b5ea2d37e94feef9dfc6a8d2681f339cc6960b1`
- `scripts/build_matching.ps1` — commit `a3a18cd8b5ea8af3793b0fd4bb5fa2db9ab509e1`
- `scripts/install_dev_proxy.ps1` — commit `903ad122a41eb5da1e6ae28602c30637451e83ab`
- `scripts/restore_stock_bink.ps1` — commit `74a08d8da003111d671934cc02f6a37e11453d14`
- `docs/BUILD_AND_INSTALL.md` — commit `384a592aa6c16e3f61308fd54b47fcdc9a122190`

These changes do **not** modify reconstructed gameplay/engine code.

## Baseline Test Plan

Once CI produces the DLL:

1. Back up the retail game's original `binkw32.dll`.
2. Rename the original to `binkw32_.dll`.
3. Install our generated artifact as `binkw32.dll`.
4. Launch `SpideyPC.exe`.
5. Verify the decomp console opens and shows the commit/version.
6. Capture any assertion output/crash before changing source behavior.

Do **not** move on to a gameplay bug until this baseline is confirmed.

## Next Action

**User-side baseline test is now the blocking step.**

From a Windows clone of this repository:

```powershell
git checkout dev
powershell -ExecutionPolicy Bypass -File .\scripts\build_matching.ps1
```

Then install the generated proxy against the user's retail game directory:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install_dev_proxy.ps1 -GameDir "<folder containing SpideyPC.exe>"
```

Launch `SpideyPC.exe` and capture:
- whether the `spidey-decomp` console appears;
- the full console output, especially any validation failures;
- whether the game reaches the menu / gameplay;
- the SHA-256 printed by the build/install helper.

If the fork's GitHub Actions page shows workflows disabled, enable Actions there as a secondary CI path. As of the latest check, the API still reports zero `dev` workflow runs.

**Do not start bug-fix source changes until this baseline result is recorded.**


## User Workflow Change — 2026-09-29

User requested a BAT-first local workflow matching the Destroy All Humans recomp style.

**ACTIVE NOW:** add top-level Windows BAT launchers so the user can keep one local clone, update it from `origin/dev`, build, install, run, and restore without typing PowerShell commands.

Planned BAT interface:
- `SETUP_FIRST_TIME.bat`
- `UPDATE_PROJECT.bat`
- `BUILD_DEV.bat`
- `INSTALL_DEV_BUILD.bat`
- `RUN_GAME.bat`
- `UPDATE_BUILD_INSTALL.bat`
- `RESTORE_STOCK_GAME.bat`

Local game path will be stored in an ignored local config file and never committed.


## BAT Workflow — READY

Top-level BAT workflow is now committed on `dev`:

- `SETUP_FIRST_TIME.bat` — configure / change retail game folder.
- `UPDATE_PROJECT.bat` — fetch + fast-forward local `dev` from the user's fork.
- `BUILD_DEV.bat` — build the matching proxy.
- `INSTALL_DEV_BUILD.bat` — install the built proxy into the configured game folder.
- `BUILD_AND_INSTALL.bat` — build + install in one step.
- `RUN_GAME.bat` — launch `SpideyPC.exe` with the configured working directory.
- `RESTORE_STOCK_GAME.bat` — restore the preserved retail Bink DLL.
- `SPIDEY_DEV_MENU.bat` — numbered menu for all of the above.

`spidey_local_config.bat` is ignored by Git and stores only the local retail game path.

Documentation was rewritten BAT-first at commit `7839787e9730eb2321636b51060a539d9762c88c`.

A standalone bootstrap package was also generated for the user as `Spider-Man-2000-Dev-Bootstrap.zip`; its `GET_SPIDEY_PROJECT.bat` clones `legentus/spidey-decomp` branch `dev` into a local folder (default: Documents\\Spider-Man-2000-Dev), or updates an existing clone, then runs first-time setup.

### Normal local loop

```text
UPDATE_PROJECT.bat
        ↓
BUILD_AND_INSTALL.bat
        ↓
RUN_GAME.bat
```

### Next blocking step

User should bootstrap/clone locally, run the BAT workflow, and report the first build + launch result. Capture the complete build console output and any `spidey-decomp` runtime console output. Do not begin gameplay-source bug fixes until the baseline proxy has been tested against the user's exact retail installation.


## Bootstrap Git Dependency Fix — 2026-09-29

First user run of the standalone bootstrap stopped because `git.exe` was not in PATH.

**ACTIVE FIX:** replace the bootstrap with a self-healing version that:
1. checks PATH for Git;
2. checks normal Git for Windows install locations even if PATH is stale/missing;
3. installs Git automatically with `winget` when available;
4. falls back to downloading the current 64-bit Git for Windows installer from the official `git-for-windows/git` GitHub release and runs it silently;
5. continues cloning/updating `legentus/spidey-decomp` branch `dev` in the same run.

User should not need to manually install Git or edit PATH.


### Bootstrap Git dependency fix completed

Committed fixes:
- `GET_SPIDEY_PROJECT.bat` now auto-detects Git, auto-installs Git for Windows when absent, and continues the clone/update in the same run.
- `UPDATE_PROJECT.bat` now finds Git in standard install locations even when PATH is stale.
- `SETUP_FIRST_TIME.bat` no longer tells the user to install Git manually.
- Git path quoting was hardened for installs under `C:\Program Files`.

Latest bootstrap commit: `2309c4613a9e97d25206ae3211ed46ca794802e7`.

**Next user action:** discard the old bootstrap ZIP/BAT, run the newly generated `GET_SPIDEY_PROJECT.bat`, and report the complete output if it stops again.


## Bootstrap Fix #2 — Portable Git — 2026-09-29

Second bootstrap attempt reached the official Git for Windows installer download (`Git-2.56.0-64-bit.exe`) but the installer path returned failure before the project clone.

**Decision:** stop relying on a system-wide Git installation entirely.

New design:
- Prefer an existing system Git when available.
- Otherwise download the official **MinGit 64-bit portable ZIP** from the latest `git-for-windows/git` GitHub release.
- Extract it under the local Spider-Man development folder (no admin/UAC, no installer, no PATH persistence required).
- Use that portable `git.exe` for clone/update operations.
- Keep future `UPDATE_PROJECT.bat` able to use the bundled portable Git when system Git is unavailable.

This should make the project self-contained on a clean Windows machine.


### Portable MinGit bootstrap implemented

Verified official package used by the bootstrap:
- release: `git-for-windows/git v2.56.0.windows.1`
- asset: `MinGit-2.56.0-64-bit.zip`
- SHA-256: `064b440ff870ed5198527e8f3a92cdf5bd2fd0fedf5e718af95e3fdaddeff718`

Committed:
- `GET_SPIDEY_PROJECT.bat` switched from installer/winget flow to verified portable MinGit: `c9d9210e66102e10d550dcf203c07c7fe710455e`
- `UPDATE_PROJECT.bat` now also detects the private portable Git location: `18784b2d615b4e2bb393a3a4e5bb37e192070be2`

Portable Git location:
`%LOCALAPPDATA%\Spidey2000Dev\MinGit`

No administrator rights, system-wide Git install, or persistent PATH modification should be required.

**Next user action:** run the new portable-MinGit bootstrap. If it fails, capture all output; the expected progression is download -> SHA-256 verify -> extract -> clone `dev` -> first-time game path setup.


## Bootstrap Fix #3 — Remove PowerShell Dependency — 2026-09-29

User's Windows environment does not expose `powershell.exe`, so the portable-MinGit bootstrap stopped before download.

**New bootstrap rule:** no PowerShell dependency.

Next implementation will use only Windows command-line tools expected on current Windows builds:
- `curl.exe` for download
- `certutil.exe -hashfile ... SHA256` for checksum verification
- `tar.exe -xf` for ZIP extraction

If any of those are missing, the bootstrap will print exactly which tool is unavailable rather than failing generically.

Goal remains: zero manual prerequisites and no admin/system-wide Git install.


### PowerShell-free user workflow implemented

Changes committed:
- `build.bat` now supports a private matching compiler location through `SPIDEY_MSVC_ROOT`: `6c7b9be6e18dace8e986c41be7da4de854b6cc11`
- `BUILD_DEV.bat` no longer calls PowerShell and downloads/extracts the preserved compiler with `curl.exe` + `tar.exe`: `09d91dd5375dfeb1528241555b0c4d325cdbb7cd`
- `INSTALL_DEV_BUILD.bat` no longer calls PowerShell: `6ad38bea3b5af1a6a2c9107f40cdf65bd40184a4`
- `RESTORE_STOCK_GAME.bat` no longer calls PowerShell: `a69f146f6d83ecb4e80b1a22361be9649ae32b20`
- `GET_SPIDEY_PROJECT.bat` no longer calls PowerShell; portable MinGit setup now uses only `curl.exe`, `certutil.exe`, and `tar.exe`: `79667480f51ba0a130e5df8cd1e172a98a4eb86e`

Private local tools:
- MinGit: `%LOCALAPPDATA%\Spidey2000Dev\MinGit`
- preserved matching compiler: `%LOCALAPPDATA%\Spidey2000Dev\MatchingVS`

No administrator rights, PowerShell, system-wide Git installation, or writes to `C:\vs` are required by the new user workflow.

**Next user action:** discard all older bootstrap BATs and run the bootstrap generated from commit `79667480f51ba0a130e5df8cd1e172a98a4eb86e`. Expected path: portable MinGit download -> SHA-256 verification -> tar extraction -> clone `dev` -> configure game folder.


## Bootstrap Fix #4 — Eliminate Git/curl/PowerShell dependencies — 2026-09-29

User's environment also does not expose `curl.exe`. Continuing to add prerequisite probes is the wrong design.

**New final local-update design:**
- Do not require Git on the user's PC at all.
- Do not require PowerShell.
- Do not require curl.
- Bootstrap/update downloads the GitHub `dev` branch archive directly:
  `https://github.com/legentus/spidey-decomp/archive/refs/heads/dev.zip`
- Download is performed through Windows Script Host (`cscript.exe`) using built-in Windows HTTP/COM components.
- ZIP extraction is performed through Windows Shell COM, with `tar.exe` only as an optional fast path when present.
- Local refresh preserves `spidey_local_config.bat`.
- The user's normal update workflow remains a single `UPDATE_PROJECT.bat`.

This is now preferred over maintaining a local Git clone because the user's goal is a self-updating working copy, not local source-control operations.


## DAH Workflow Review / Root Cause — 2026-09-29

Reviewed the actual working DAH port workflow in `legentus/DAH-Port`:
- `UPDATE_DAH_PORT.bat` is intentionally tiny and delegates to `tools/UPDATE_DAH_PORT.ps1`.
- `TEST_LATEST_BUILD.bat` is intentionally tiny and delegates to `tools/TEST_LATEST_BUILD.ps1`.
- The real logic lives in the tools scripts; the user-facing BAT layer stays stable.

Important correction for Spider-Man:
- Earlier failures of `where powershell.exe` and `where curl.exe` do **not** prove those Windows components are absent.
- On normal Windows they live under explicit system paths such as:
  - `%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\System32\curl.exe`
  - `%SystemRoot%\System32\tar.exe`
  - `%SystemRoot%\System32\certutil.exe`
  - `%SystemRoot%\System32\cscript.exe`
- A damaged/minimal PATH can therefore make `where` fail even though the tools exist.

**New direction:** mirror the proven DAH pattern. User-facing BATs will explicitly repair/discover Windows system paths first, then delegate to stable tool scripts. Stop adding layers of Git installers/portable prerequisites.


## DAH-style workflow conversion completed

The Spider-Man local workflow now mirrors the proven DAH port structure:

User-facing BATs:
- `GET_SPIDEY_PROJECT.bat` — first-time bootstrap wrapper
- `UPDATE_SPIDEY_PROJECT.bat` — update-only wrapper
- `TEST_LATEST_BUILD.bat` — update -> restart if workflow changed -> build -> install -> launch
- legacy names `UPDATE_PROJECT.bat` and `BUILD_AND_INSTALL.bat` now forward to the new workflow

Real logic:
- `tools/BOOTSTRAP_SPIDEY_PROJECT.ps1`
- `tools/UPDATE_SPIDEY_PROJECT.ps1`
- `tools/TEST_LATEST_BUILD.ps1`

Key fix from DAH review:
- BAT wrappers no longer depend on PATH to find PowerShell.
- They explicitly check standard Windows locations:
  - `%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe`
- They also prepend standard Windows system directories to PATH before launching the tool script.

Local source updates do not require Git:
- updater downloads the `dev` branch ZIP from GitHub;
- records the exact remote commit in `LOCAL_DEV_REVISION.txt`;
- preserves `spidey_local_config.bat`, logs, build outputs, and local revision state;
- `TEST_LATEST_BUILD.ps1` restarts itself after an update exactly like the DAH workflow.

Latest-test behavior:
1. update local project;
2. restart if the workflow changed;
3. read/configure retail Spider-Man folder;
4. download preserved matching compiler if needed;
5. build `Release\spider.dll`;
6. stage/install it as `binkw32.dll`;
7. preserve retail Bink as `binkw32_.dll`;
8. record revision + proxy SHA-256 under `logs\<timestamp>`;
9. launch `SpideyPC.exe`.

Latest safety cleanup:
- bootstrap refuses to mirror into a non-empty unrelated folder;
- `LOCAL_DEV_REVISION.txt` and `logs/` are ignored.

**Next user action:** use the newly packaged DAH-style bootstrap containing only `GET_SPIDEY_PROJECT.bat` and `tools\BOOTSTRAP_SPIDEY_PROJECT.ps1`. Discard all earlier bootstrap ZIPs.


## Local Bootstrap Success — 2026-09-29

User confirmed the DAH-style bootstrap completed successfully.

Local state now present:
- full Spider-Man decomp/dev project downloaded locally;
- `UPDATE_SPIDEY_PROJECT.bat` present and working copy established;
- `TEST_LATEST_BUILD.bat` present;
- local game path configured;
- local project is ready for the first baseline build/install/launch.

**Immediate next step:** run `TEST_LATEST_BUILD.bat` with the current unmodified gameplay/decomp source.

Baseline success criteria:
1. updater reports local project current;
2. matching compiler toolchain is downloaded/extracted if not already present;
3. `Release\spider.dll` builds successfully;
4. proxy is staged/installed as `binkw32.dll`;
5. original retail Bink is preserved as `binkw32_.dll`;
6. `SpideyPC.exe` launches;
7. `spidey-decomp` console appears and prints revision/validation output;
8. game reaches menu/gameplay without immediate failure.

Do not make gameplay/source changes until this exact baseline is captured.


## First TEST_LATEST_BUILD run — updater false failure — 2026-09-29

User ran `TEST_LATEST_BUILD.bat` from:
`F:\Spider-Man 2000 Recomp\project main`

Observed:
- local revision before update: `94a77b0b3271c57d5eb40bb835969bf1fa9cfd8e`
- remote revision: `3c13230edf34ae2241c43bcc0b2059a387169f4d`
- dev archive downloaded successfully;
- archive extracted successfully;
- local project refresh completed successfully;
- updater printed `[OK] Local project is current.`;
- immediately afterward the parent latest-test script printed `[ERROR] Update failed.`;
- process exit code was 3.

Root cause identified:
- `robocopy` uses exit codes 0-7 for successful/acceptable outcomes.
- the updater correctly treated codes below 8 as success, but left `$LASTEXITCODE` equal to robocopy's code (3 in this run).
- `TEST_LATEST_BUILD.ps1` then checked that stale `$LASTEXITCODE` and misclassified the successful update as a failure.
- the updater also has a success-path `exit 0` when already current; because the updater is invoked inside the latest-test PowerShell process, that should be replaced with a normal return so it cannot terminate the parent test workflow.

**ACTIVE FIX:** normalize `$global:LASTEXITCODE = 0` on all successful updater returns and avoid `exit` on successful updater paths.


### Updater exit-code fix committed

Fix commit:
`3b90e5d8b39081bb1ca1e3dc053b9d62b813cdb1`

Changes:
- successful "already current" path now uses `return` instead of `exit 0`;
- all successful updater completions explicitly set `$global:LASTEXITCODE = 0`;
- this prevents successful robocopy codes 1-7 (observed code 3) from being misread by `TEST_LATEST_BUILD.ps1` as an update failure.

Recovery from the user's current local state:
1. run `UPDATE_SPIDEY_PROJECT.bat` once by itself so the fixed updater is pulled into the local project;
2. then run `TEST_LATEST_BUILD.bat` again.


## First successful full build; install blocked by Program Files permissions — 2026-09-29

User's latest TEST_LATEST_BUILD run reached the actual build and produced a proxy successfully.

Observed:
- updater reported local/remote revision `16c61b783962d845d9c0056db463b4109fb1585b` and `[OK] Already current`;
- configured game path: `C:\Program Files (x86)\Activision\Spider-Man`;
- preserved matching toolchain downloaded/extracted successfully;
- full NMAKE/MSVC6-era build completed successfully;
- linker produced `Release\spider.dll`;
- staged proxy SHA-256:
  `5C444AE81948E054B835C8E0DD2D31C2098EEA063B7BF9B896B5BDE3873F1B72`;
- first install attempt failed at renaming retail `binkw32.dll` to `binkw32_.dll` with AccessDenied because the game is installed under Program Files (x86).

**ACTIVE FIX:** TEST_LATEST_BUILD should detect that the configured game directory is not writable and automatically relaunch itself elevated once, then continue the update/build/install/launch workflow. The user should not have to manually right-click Run as administrator.


### Automatic elevation fix committed

Fix commit:
`1339465e6da0741c4b1204712a3101c105ea1c0f`

`tools/TEST_LATEST_BUILD.ps1` now:
- probes write access to the configured game directory before build/install;
- if the game directory is protected (observed under `C:\Program Files (x86)\Activision\Spider-Man`), automatically relaunches itself with UAC elevation;
- resumes with `-PostUpdate -Elevated` to avoid re-running the updater unnecessarily;
- confirms elevated write access before continuing;
- preserves the same automatic build/install/launch workflow.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` once to pull this fix, then run `TEST_LATEST_BUILD.bat`. Accept the Windows UAC prompt when it appears. Expected next frontier is actual proxy install + game launch.


## First successful proxy install + game launch — 2026-09-29

User ran the fixed elevated `TEST_LATEST_BUILD.bat`.

Observed successful baseline:
- revision tested: `8a7a069ed2650f8c390b10922b47737cb0bd8c0d`;
- game: `C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe`;
- elevated game-folder access confirmed;
- matching toolchain found at `C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS`;
- incremental rebuild completed successfully;
- proxy SHA-256 remained:
  `5C444AE81948E054B835C8E0DD2D31C2098EEA063B7BF9B896B5BDE3873F1B72`;
- retail `binkw32.dll` was successfully preserved as `binkw32_.dll`;
- rebuilt proxy installed as live `binkw32.dll`;
- game launched successfully;
- session log directory:
  `F:\Spider-Man 2000 Recomp\project main\logs\20260929-024052`.

This is the first confirmed build/install/launch baseline for the local Spider-Man dev workflow.

**Next frontier:** capture/verify the runtime console assertions and whether the game reaches menu/gameplay cleanly under the proxy. Once confirmed, perform one low-risk deliberate source-level proof change before choosing the first real gameplay bug.


## Disc-image auto-mount workflow — 2026-09-29

User accepted the non-crack compatibility path: automatically mount a backup image of their own Spider-Man disc before launch.

Planned TEST_LATEST_BUILD behavior:
1. read `SPIDEY_DISC_IMAGE` from the local config;
2. if missing, prompt once for an ISO path and save it locally;
3. mount the ISO with Windows' built-in disk-image support;
4. build/install the current proxy as usual;
5. launch `SpideyPC.exe`;
6. wait for the game process to exit;
7. unmount the ISO only if this script mounted it.

The disc image path remains local-only and is not committed.


### ISO auto-mount implementation completed

Final clean implementation commit:
`6d5405a419fea28e4e2bf011d099dc1c6be079b5`

`tools/TEST_LATEST_BUILD.ps1` now:
- reads `SPIDEY_DISC_IMAGE` from `spidey_local_config.bat`;
- prompts once for the user's Spider-Man ISO if not configured;
- validates that the file exists and is an `.iso`;
- saves the ISO path locally;
- detects whether that ISO is already mounted;
- mounts it with Windows `Mount-DiskImage` if needed;
- reports the assigned drive letter when available;
- launches `SpideyPC.exe`;
- keeps the image mounted for the full lifetime of the game process;
- automatically dismounts the ISO after the game exits only when the script mounted it;
- leaves an already-mounted ISO alone;
- records the disc-image path in the per-run test-session log.

During implementation a malformed intermediate script commit was detected during verification and immediately replaced before user testing. Commit `6d5405a419fea28e4e2bf011d099dc1c6be079b5` is the clean replacement.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. On the first run only, enter the path to the Spider-Man ISO. Accept the existing UAC prompt for the Program Files game install. Verify that the CD-ROM dialog no longer appears and that the game reaches the menu/gameplay.


## ISO auto-mount test — mounted successfully but CD check still fails — 2026-09-29

User tested revision `4d315296553698cd972253e35153cf2a6ff5b239` with configured disc image:
`F:\Spider-Man.iso`

Observed:
- matching proxy built successfully;
- proxy SHA-256: `9568710A82A82C781F4233AFA34086A5AAD5C1AB2DEBD34AB42D8C919220D144`;
- proxy installed successfully;
- ISO mounted successfully as drive `I:`;
- `SpideyPC.exe` launched;
- game still displayed the original "Please insert the Spider-Man CD-ROM" error;
- game exited with code 1;
- launcher then unmounted the ISO successfully.

Conclusion:
- automatic mounting works;
- a plain Windows-mounted ISO does not satisfy the game's original CD validation;
- next investigation is to determine what disc characteristics the retail check expects (for example data layout, volume identity, mixed-mode/audio TOC, or another property) and whether the current ISO representation preserves them.


## ISO mount compatibility test result — 2026-09-29

User confirmed the configured ISO mounted successfully as drive I:, the dev proxy built/installed, and the game launched, but the game still displayed its original disc-required dialog and exited with code 1.

Current conclusion:
- mounting works;
- Windows built-in ISO presentation is not sufficient for this legacy game check on the tested system.

Next step:
- add a pre-launch compatibility diagnostic for the mounted optical drive;
- support a fuller virtual optical-drive backend when the built-in Windows mount is not compatible;
- preserve the current automatic mount/unmount workflow.


Latest test-runner update: commit 9bcdaacc9fccf59384cc6813fb91cd31cf3443ad adds pre-launch disc compatibility diagnostics and an alternate virtual-drive backend. Next action: update locally and rerun TEST_LATEST_BUILD.bat.


## Cracked EXE boot succeeds; first runtime crash — 2026-09-29

User is now testing with a cracked Spider-Man PC executable, so the physical-disc/ISO workaround is no longer part of the active workflow.

Latest run:
- tested revision: `9bcdaacc9fccf59384cc6813fb91cd31cf3443ad`;
- proxy SHA-256: `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`;
- proxy installed successfully;
- game passed the previous disc gate and actually booted;
- process later exited with code `-1073741819` = `0xC0000005` (access violation).

Important risk:
- the decomp proxy applies many fixed-address runtime patches into `SpideyPC.exe`;
- the cracked executable may differ from the original retail executable at those addresses even if it otherwise boots;
- before treating the crash as a decomp bug, we must fingerprint the exact running EXE and validate that its PE layout and patched bytes are compatible with the hardcoded addresses.

**ACTIVE NEXT STEP:** remove the disc-image requirement from TEST_LATEST_BUILD and add automatic executable fingerprint + patch-site compatibility logging before launch.


## Crash-diagnostic frontier — 2026-09-29

The ISO/MCI/virtual-drive testing path is superseded and no longer active. User is testing with a cracked EXE that boots without the disc gate.

Latest known runtime result:
- game boots under rebuilt proxy;
- process later exits with `0xC0000005` access violation.

Diagnostics now added:
- `tools/TEST_LATEST_BUILD.ps1` reset to the pre-ISO workflow;
- exact running `SpideyPC.exe` is fingerprinted every test:
  - SHA-256
  - file size
  - PE machine
  - timestamp
  - entry point RVA
  - image base
  - image size
  - section layout
- fingerprint saved under the timestamped test-session directory as `game-exe-fingerprint.txt`;
- proxy installs an unhandled-exception filter and writes `spidey-decomp-crash.log` containing:
  - exception code
  - fault address
  - fault module/path
  - module base + module-relative offset
  - x86 register state;
- launcher waits for the game to exit and copies the native crash log into the same session directory when present;
- launcher labels `-1073741819` explicitly as `0xC0000005`.

Relevant commits:
- `295313a4b71b74f0b79cdac05142bf76beec737c` — remove ISO workflow; add EXE fingerprint/crash collection;
- `ab73cf5e3ecb3312390390906acb95f98a6b111f` — native unhandled-exception crash logger;
- `a21dc7cb3743291ff9c14c8f747423bcb8bba2f3` — fix session-log/fingerprint ordering.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. After the crash, provide the new console output plus the timestamped session's `game-exe-fingerprint.txt` and `spidey-decomp-crash.log` if generated.


## Critical build-system finding — stale DLL was executed — 2026-09-29

Latest user run at revision `bfaee84279eab635aaefadce36bb412683751de2`:
- `main.cpp` was recompiled;
- NMAKE output did **not** show a subsequent `link.exe` step;
- staged proxy SHA-256 remained `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`, identical to the previous pre-crash-logger DLL;
- therefore the newly added crash-logger code was not present in the DLL actually launched;
- this explains why no `spidey-decomp-crash.log` was generated.

The exact tested EXE fingerprint is:
- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- file size 1507328
- PE timestamp `0x3B7A3167`
- image base `0x00400000`
- image size `0x02A0D000`
- .text RVA `0x1000`, raw size `0x13A000`

The cracked EXE preserves the expected fixed-address layout at a coarse PE level.

**ACTIVE FIX:** make TEST_LATEST_BUILD force a clean relink/rebuild whenever source revision changes and verify that the output DLL timestamp/hash changed when compilation occurred. Also replace the overwriteable unhandled-exception filter with a first-priority vectored exception handler filtered to access violations.


## Stale-DLL root cause fixed; vectored crash capture ready — 2026-09-29

Analysis of the previous test showed the diagnostic code had not actually been linked into the DLL that ran:
- NMAKE recompiled `main.cpp`;
- no `link.exe` step followed;
- proxy SHA-256 remained exactly `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`;
- therefore `Release\spider.dll` was stale and the missing crash log was expected.

Fixes now active:
- `build.bat` supports `SPIDEY_FORCE_CLEAN`;
- `TEST_LATEST_BUILD.ps1` forces a clean matching build for every test;
- test runner verifies `Release\spider.dll` was freshly regenerated after build start;
- stale DLLs are rejected instead of staged;
- native crash logger now uses a first-priority vectored exception handler when available;
- vectored API is resolved dynamically for compatibility with the preserved Visual C++ 6 headers;
- handler only logs access violations;
- log records:
  - exception address/code;
  - read/write/execute operation;
  - invalid target address;
  - fault module/base/offset;
  - x86 registers;
  - 16 stack DWORDs;
- fallback top-level exception filter remains if vectored handlers are unavailable.

Relevant commits:
- `68a8192f1bf858152bbacf458f72280cd44d5d77` — forced-clean support;
- `4e805ffce4f42cc66213c820e7e60d35f80c53d4` — clean test build + fresh-DLL validation;
- `dcf07ccc811aff34939eaa3eabc8b162baff8add` — repaired VS6-compatible vectored crash handler;
- `b817c98657be1a477996cb2c145e47b08c0c9cfe` — reliable CLEAN failure propagation.

Static verification passed:
- no stale old handler symbol;
- no duplicated crash-handler tail;
- forced-clean path present once;
- fresh-DLL check present;
- fingerprint and crash-log collection paths present.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. The build should now visibly perform a full CLEAN, compile, and `link.exe`. The proxy SHA should differ from the stale `5508A0BE...` build. After the crash, return the launcher output and `spidey-decomp-crash.log` from the timestamped session folder.


## First actionable crash mapped — 2026-09-29

Clean rebuild/runtime diagnostics succeeded:
- fresh proxy SHA-256: `8279675802F6E838B9041BA66F9DE82B8FA678698BB26DE5BF318240E755BEE8`;
- exact EXE fingerprint unchanged;
- native crash log captured.

Crash:
- exception `0xC0000005`;
- fault address `0x00503AF7`;
- null read target `0x00000000`;
- mapped through `tools/names.json` to `DXSOUND_ShutDown()+0x7`;
- retail/reconstructed shutdown immediately dereferences `g_pDSBuffer->Stop()`, so this is a secondary cleanup crash caused by a null primary sound buffer.

Important control-flow finding:
- the game's DirectX error macros call `DXINIT_ShutDown()` on any failed HRESULT;
- that shutdown path reaches `DXSOUND_ShutDown()`;
- if failure occurs before `DXSOUND_Init()` creates `g_pDSBuffer`, cleanup itself crashes;
- observed register `EDI=0x80004001` is consistent with a preceding `E_NOTIMPL` HRESULT, but the exact first failing DirectX call is not yet proven.

**ACTIVE DIAGNOSTIC:** hook the retail error reporters at their mapped addresses:
- `displayDIError` 0x004FC240
- `displayDSError` 0x004FC630
- `displayD3DError` 0x004FC820

The wrappers will record HRESULT + original source file + original source line to `spidey-decomp-dxerror.log` before normal error handling continues.


## DirectX first-failure logger ready — 2026-09-29

Latest crash analysis:
- clean rebuild confirmed by visible full compile + link;
- fresh proxy SHA-256 `8279675802F6E838B9041BA66F9DE82B8FA678698BB26DE5BF318240E755BEE8`;
- crash captured at `0x00503AF7`;
- mapped to `DXSOUND_ShutDown()+0x7`;
- access type: read;
- target address: `0x00000000`;
- this is a secondary cleanup crash caused by null `g_pDSBuffer`.

DirectX error macros call global shutdown on failed HRESULTs, so the real bug is an earlier DirectX failure. The observed register value `0x80004001` suggests E_NOTIMPL but is not sufficient to identify the exact API call.

New diagnostics:
- retail `displayDIError` at `0x004FC240` hooked;
- retail `displayDSError` at `0x004FC630` hooked;
- retail `displayD3DError` at `0x004FC820` hooked;
- wrappers append kind/HRESULT/original source file/original source line to `spidey-decomp-dxerror.log`;
- normal reconstructed error display still runs afterward;
- latest-test launcher removes stale DirectX logs before launch and copies the new log into the timestamped session folder after exit.

Commits:
- `4bc1e3039f44032038db849e62cb1ea07dbe3d75` — DirectX first-failure hooks;
- `42a1f715ccc1222fb6118a7a20f5e4756e8543ec` — collect DirectX error log.

Static verification:
- each retail error address patched exactly once;
- logger path present;
- launcher cleanup/copy path present.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. Return the new `spidey-decomp-dxerror.log` plus the crash log and launcher output.


## Primary DirectX failure confirmed — caller-address diagnostic next — 2026-09-29

Latest test at revision `9cf5b139661f11bd613ef646d2e0f3e2a477ba52`:
- clean forced rebuild/link succeeded;
- proxy SHA-256 `3929CF3E440064D1846030A368D5409F2BD3A48B7CA493167F88A1FB5CC875EE`;
- EXE fingerprint unchanged;
- DirectX diagnostic captured:
  `D3D error=0x80004001 file=C:\backup\SpideyPC\SpideyPC\D3d\DXinit.cpp line=1005`;
- `0x80004001` is E_NOTIMPL;
- subsequent cleanup still faults at `DXSOUND_ShutDown()+0x7` reading address 0.

The embedded original source line cannot be mapped directly to the current reconstructed source because line numbering has diverged.

**ACTIVE NEXT DIAGNOSTIC:** replace the three DirectX error wrappers with x86 naked trampolines that preserve normal calling semantics while recording the retail return/call-site address. This will identify the exact instruction/API call that produced E_NOTIMPL.


## Exact DirectX call-site capture ready — 2026-09-29

The latest DirectX error log proves the primary failure:
- kind: D3D
- HRESULT: `0x80004001` (E_NOTIMPL)
- original source: `C:\backup\SpideyPC\SpideyPC\D3d\DXinit.cpp`
- original source line: `1005`
- secondary crash remains `DXSOUND_ShutDown()+0x7` null-reading `g_pDSBuffer`.

Because the reconstructed `DXinit.cpp` line numbering no longer matches the original Neversoft source, line 1005 cannot by itself identify the exact API call.

New diagnostic commit:
- `dcf0e7ce21468230de9b8c07b4d5d02c5e74d229`

Changes:
- DI/DS/D3D error wrappers are now x86 naked trampolines;
- trampolines capture the untouched retail return address directly from the entry stack;
- logger records:
  - HRESULT
  - original file/line
  - `caller_return`
  - probable direct-call site `caller_return - 5`;
- each trampoline restores the original stack exactly and tail-jumps to the normal reconstructed error display function;
- normal error/cleanup behavior is otherwise preserved.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. Return `spidey-decomp-dxerror.log`; its new caller/call-site fields should identify the exact failing DirectDraw/Direct3D instruction.


## Updater transport failure — 2026-09-29

Latest user attempt failed before build/test:
- UPDATE_SPIDEY_PROJECT.ps1 could not query the GitHub dev revision;
- PowerShell reported: "The underlying connection was closed: An unexpected error occurred on a send.";
- no source/build/runtime failure occurred.

**ACTIVE FIX:** make GitHub SHA lookup best-effort rather than mandatory:
1. retry GitHub API several times;
2. fall back to local Git `ls-remote` when available;
3. fall back to explicit Windows curl when available;
4. if exact remote SHA still cannot be resolved, continue by downloading/refeshing the dev branch archive and identify the local source state with the archive SHA-256 instead of aborting;
5. add retries/fallback for archive download too.

Because the currently installed updater cannot fetch its own fix when the API path fails, provide a minimal replacement ZIP containing only `tools\UPDATE_SPIDEY_PROJECT.ps1`.


### Hardened updater committed

Commit:
`af7c0e4cf915b0e7187ad6046cc42e1865c7a28a`

New updater behavior:
- retries GitHub commit API up to 3 times;
- falls back to `git ls-remote` when Git is available;
- falls back to Windows `curl.exe` when available;
- if exact commit SHA still cannot be resolved, does not abort;
- instead downloads the dev branch archive, refreshes the local tree, and records an `archive-<sha256-prefix>` source identity;
- archive download itself retries PowerShell transport and falls back to curl;
- normal exact-SHA tracking remains when GitHub revision lookup succeeds.

Because the installed updater can fail before fetching this change, a minimal replacement ZIP is being provided containing only:
`tools\UPDATE_SPIDEY_PROJECT.ps1`

**Next user action:** extract that ZIP into the local project root and overwrite the existing updater script, then run `UPDATE_SPIDEY_PROJECT.bat` followed by `TEST_LATEST_BUILD.bat`.


## Exact DirectX error call site captured — 2026-09-29

Latest test at revision `13b50f409ab2afccbb4846a9d5a1b34c082c6ea0`:
- forced clean build/link succeeded;
- proxy SHA-256 `F07384C326D25AA2E6551B5C9F8E21A2F65DEAFA18B6A03305643CB8843B90F7`;
- EXE fingerprint unchanged;
- primary DirectX error:
  - kind: D3D
  - HRESULT: `0x80004001` (E_NOTIMPL)
  - original source: `DXinit.cpp`
  - original source line: 1005
  - caller return: `0x004FFBB1`
  - probable direct call site: `0x004FFBAC`;
- secondary cleanup crash remains:
  - `DXSOUND_ShutDown()+0x7`
  - read from `0x00000000`.

**ACTIVE NEXT STEP:** map `0x004FFBAC` inside retail `initDirectDraw7()` to the exact DirectDraw/Direct3D method, then patch/tolerate that specific modern-Windows compatibility failure while preserving diagnostics.


## Runtime instruction-window dump added — 2026-09-29

Exact error reporter call site from latest run:
- caller return: `0x004FFBB1`
- displayD3DError call site: `0x004FFBAC`
- HRESULT: `0x80004001` (E_NOTIMPL)

The direct error-report call is not itself the failing COM API call; the failing DirectDraw/Direct3D vtable call occurs earlier in the same retail block.

Commit:
`e683078040b6e8e96a93aac5d91f1e8abb2f8a67`

The DX error logger now also records:
- `code_window_base = call_site - 0x60`
- 160 raw instruction bytes spanning 96 bytes before and 64 bytes after the error-report call.

This will allow offline disassembly of the exact retail code around the failure and identification of the failing COM method/vtable slot without requiring the user to upload the executable.

Static verification passed:
- code-window base field present;
- 160-byte dump present;
- structured exception guard present.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`, then return the new `spidey-decomp-dxerror.log`. No additional files should be necessary unless the fault changes.


## Root DirectDraw failure decoded — compatibility patch selected — 2026-09-29

The 160-byte runtime instruction window conclusively maps the primary failure:

Retail code:
- `0x004FFB72: call [vtable+0x50]`
  - `IDirectDraw7::SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)`
- `0x004FFB94: call [vtable+0x54]`
  - `IDirectDraw7::SetDisplayMode(width, height, bpp, refresh, flags)`
- return stored in EDI at `0x004FFB97`;
- failed HRESULT then reaches `displayD3DError` at `0x004FFBAC`.

Argument globals visible directly in the retail instruction stream:
- `0x006B78E4` = requested width;
- `0x006B78E8` = requested height;
- `0x006B78EC` = requested color depth;
- `0x006B7900` = retail `IDirectDraw7*`.

Thus the primary startup blocker is specifically `IDirectDraw7::SetDisplayMode` returning `DDERR_UNSUPPORTED / E_NOTIMPL`.

**ACTIVE FIX:** patch only the five-byte sequence at `0x004FFB94` (`FF 51 54 8B F8`) with a direct call to a compatibility thunk that:
1. calls the original `IDirectDraw7::SetDisplayMode` with the untouched requested parameters;
2. if it succeeds, preserves original behavior;
3. if it returns `DDERR_UNSUPPORTED` and requested bpp is 16, retries the same mode at 32 bpp;
4. on successful 32-bpp retry, updates retail `gColorCount` at `0x006B78EC` to 32;
5. returns the final HRESULT in EAX and mirrors the overwritten `mov edi,eax` behavior before resuming at `0x004FFB99`;
6. refuses to install if the expected original five bytes are not present;
7. logs both attempts/results for the test session.

This is deliberately narrower than forcing windowed mode or globally ignoring DirectDraw failures.


## First DirectDraw compatibility fix implemented — 2026-09-29

Root failure proven from retail runtime bytes:
- `IDirectDraw7::SetDisplayMode` call at `0x004FFB94`;
- requested mode is passed from retail globals:
  - width `0x006B78E4`
  - height `0x006B78E8`
  - bpp `0x006B78EC`;
- returned HRESULT `DDERR_UNSUPPORTED / E_NOTIMPL`.

Compatibility implementation commit:
`edab1663977c21afff7e2a051d0ee740e410097c`

Behavior:
- validates exact retail bytes at `0x004FFB94` are `FF 51 54 8B F8`;
- if bytes differ, refuses to install and logs the mismatch;
- replaces only those five bytes with a call to an x86 compatibility thunk;
- thunk calls the original `IDirectDraw7::SetDisplayMode` through the live retail COM object;
- original requested mode remains the first attempt;
- only when result is `DDERR_UNSUPPORTED` and requested bpp is 16:
  - retries same width/height/refresh/flags at 32 bpp;
  - if retry succeeds, writes 32 to retail `gColorCount` at `0x006B78EC`;
- thunk reproduces overwritten `mov edi,eax`;
- thunk performs original six-argument stdcall cleanup and resumes at `0x004FFB99`;
- instruction cache is flushed after patching;
- no global DirectDraw errors are ignored;
- no windowed-mode forcing is applied.

Compatibility log:
`spidey-decomp-compat.log`

Example expected successful line:
`SetDisplayMode 640x480x16 first=0x80004001 retry_bpp=32 retry=0x00000000`

Launcher collection commit:
`63298fb64d6281bd31681f99f041791cb82014b4`

Static verification passed:
- exact byte guard present;
- exact retail call site present;
- retry condition limited to unsupported 16-bpp mode;
- retail bpp global updated only after successful 32-bpp retry;
- EDI/result semantics restored;
- original 24-byte stdcall argument cleanup preserved;
- launcher removes stale compat logs and copies the new one into the timestamped test folder.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` then `TEST_LATEST_BUILD.bat`. If the game progresses farther, return the launcher output and all generated diagnostic logs. If it still exits at startup, `spidey-decomp-compat.log` is the primary file needed.


## 16->32 compatibility attempt did not clear startup failure — 2026-09-29

Latest test at revision `d86523d538a7b97ac32fac0d5b6748378a2be4a2`:
- forced clean build/link succeeded;
- proxy SHA-256 `7FC5396C91CA478E11A3DB95C565DC3F33C3B7BF2A16262C312A74F004CDDF16`;
- EXE fingerprint unchanged;
- live instruction window proves the compatibility patch installed at `0x004FFB94`:
  original `FF 51 54 8B F8` is now a direct `E8 rel32` call;
- nevertheless the same D3D `0x80004001` error reaches `displayD3DError`;
- secondary crash remains `DXSOUND_ShutDown()+0x7` null read;
- no `spidey-decomp-compat.log` was collected.

Interpretation:
- the compatibility thunk executed far enough to return an HRESULT into retail EDI;
- the returned HRESULT is still `0x80004001`;
- we do not yet know whether:
  1. requested bpp was not 16, so retry condition did not run; or
  2. 16->32 retry ran and also returned E_NOTIMPL.

**ACTIVE FIX/DIAGNOSTIC:** record SetDisplayMode arguments and first/retry HRESULTs in static runtime state inside the proxy, then append that state through the already-proven DirectX error logger. This avoids relying on a separate compatibility log file and will conclusively distinguish those two cases on the next run.


## SetDisplayMode attempt state now embedded in proven DX logger — 2026-09-29

Commit:
`cf319fed1a753cc16d91802b211b5681ae51b4fe`

Reason:
- the SetDisplayMode compatibility thunk is definitely installed in live retail code;
- the game still returns `0x80004001`;
- the separate `spidey-decomp-compat.log` did not appear, so it cannot be trusted as the sole diagnostic channel.

New runtime state captured by the proxy:
- whether compatibility helper executed;
- requested width;
- requested height;
- requested bpp;
- requested refresh;
- requested flags;
- first SetDisplayMode HRESULT;
- whether 32-bpp retry was attempted;
- retry HRESULT.

The already-working `spidey-decomp-dxerror.log` now prints that state as:
`compat_state width=... height=... bpp=... first=... retry_attempted=... retry=...`

It also independently reads the retail mode globals:
- `0x006B78E4` width
- `0x006B78E8` height
- `0x006B78EC` bpp

This makes the next test conclusive even if the standalone compat log is still absent.

Static verification:
- seen: PASS
- first: PASS
- retryFlag: PASS
- retryResult: PASS
- compatState: PASS
- retailGlobals: PASS

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`, then return only the new `spidey-decomp-dxerror.log` unless the crash behavior changes.


## Naked SetDisplayMode thunk proven unreliable — replacing with full-block helper — 2026-09-29

Latest uploaded logs:
- primary D3D error remains `0x80004001`;
- retail mode globals at failure are `640x480x16`;
- live retail code contains the installed direct `E8 rel32` compatibility call at the old SetDisplayMode site;
- however DX logger reports `compat_state not_seen`;
- secondary crash remains unchanged at `DXSOUND_ShutDown()+0x7`.

Conclusion:
- patch installation is proven;
- the current naked thunk/stack-forwarding path is not reaching the C++ compatibility helper correctly;
- do not infer that the 32-bpp retry itself failed, because the helper never recorded execution.

**ACTIVE FIX:** remove the naked forwarding thunk and replace the complete original 36-byte retail SetDisplayMode argument-setup/call/result block at `0x004FFB75..0x004FFB98` with:
1. a direct call to a normal zero-argument C++ helper;
2. helper reads retail globals directly:
   - lpDD `0x006B7900`
   - width `0x006B78E4`
   - height `0x006B78E8`
   - bpp `0x006B78EC`;
3. helper performs original SetDisplayMode call and conditional 16->32 retry;
4. patched retail block executes `mov edi,eax` after the helper call;
5. remaining bytes are NOP-filled through `0x004FFB98`;
6. exact original 36-byte sequence is validated before patching.

This removes all custom stack argument forwarding from the compatibility path.


## Full-block SetDisplayMode compatibility helper implemented — 2026-09-29

Commit:
`41f9a8b296b85ff77dfdac861ba640efcc5c1e47`

The previous naked forwarding thunk has been removed entirely.

New patch strategy:
- validates the exact original 36-byte retail sequence at `0x004FFB75..0x004FFB98`;
- that sequence covers:
  - loading retail bpp/width/height/lpDD globals;
  - pushing SetDisplayMode arguments;
  - indirect COM call through vtable slot +0x54;
  - `mov edi,eax`;
- replaces the whole sequence with:
  - `call SpideyCompatSetDisplayModeFromGlobals`;
  - `mov edi,eax`;
  - NOP padding through `0x004FFB98`;
- the normal C++ helper reads exact retail globals itself:
  - lpDD `0x006B7900`
  - width `0x006B78E4`
  - height `0x006B78E8`
  - bpp `0x006B78EC`;
- helper performs original SetDisplayMode call;
- if and only if first result is DDERR_UNSUPPORTED and bpp is 16, retries same mode at 32 bpp;
- successful 32-bpp retry updates retail bpp global to 32;
- helper attempt state remains embedded in the proven DX error logger.

Static verification passed:
- old naked thunk removed;
- zero-argument retail-global helper present;
- 36-byte exact signature guard present;
- direct helper call begins at 0x004FFB75;
- `mov edi,eax` restored immediately after helper call;
- remaining 29 bytes are NOP-filled;
- instruction cache flush covers all 36 bytes;
- DX logger still prints compatibility state.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. Return the new `spidey-decomp-dxerror.log`; if behavior changes or a new crash appears, return all generated logs.


## Full-block helper still not observed; switching to direct retail 32-bpp probe — 2026-09-29

User will provide all generated logs for every test going forward; treat the full log set as the standard test handoff.

Latest run at revision `f04d01c5ee21951abe2a10714fb9035c60f5c682`:
- clean build/link succeeded;
- proxy SHA-256 `57231C2CD56FDCA55E1673BEB0ADC6285D20640F52E9D54798273BBDC3AF4BAF`;
- EXE fingerprint unchanged;
- live instruction window proves the full 36-byte replacement is installed:
  - retail block now begins with direct `E8 rel32`;
  - followed by `mov edi,eax`;
  - remaining bytes are NOP-filled;
- DX logger nevertheless still reports `compat_state not_seen`;
- retail mode globals remain `640x480x16`;
- primary HRESULT remains `0x80004001`;
- secondary cleanup crash remains unchanged at `DXSOUND_ShutDown()+0x7`.

Conclusion:
- stop spending test cycles on the DLL helper/detour path;
- test the actual compatibility hypothesis using an in-place retail instruction edit with no cross-module call.

**ACTIVE FIX:** restore the original SetDisplayMode argument/call block and patch only its first six bytes:
- original: `8B 15 EC 78 6B 00` = `mov edx,[0x006B78EC]` (load requested bpp);
- replacement: `BA 20 00 00 00 90` = `mov edx,32; nop`.

All remaining retail instructions, argument pushes, COM vtable call, EDI assignment, and error handling stay untouched.

Purpose of this probe:
- conclusively determine whether 32-bpp SetDisplayMode works on the current Windows/DirectDraw stack;
- if the line-1005 error disappears or moves, 16-bpp mode switching is the compatibility blocker;
- if E_NOTIMPL remains at the same site, the blocker is SetDisplayMode/exclusive mode itself rather than color depth.


## Direct retail 32-bpp SetDisplayMode probe ready — 2026-09-29

Implementation commit:
`77253607b2c3fc3f3b058699d983a0d66e0bcd09`

The previous cross-module helper/detour path has been removed from this compatibility test.

Current probe:
- exact patch site: `0x004FFB75`;
- validates original bytes:
  `8B 15 EC 78 6B 00`
  = `mov edx,[0x006B78EC]`;
- replaces only those six bytes with:
  `BA 20 00 00 00 90`
  = `mov edx,32; nop`;
- original retail SetDisplayMode argument pushes remain intact;
- original retail `IDirectDraw7::SetDisplayMode` vtable call remains intact;
- original `mov edi,eax` remains intact;
- original DirectX error handling remains intact;
- no DLL helper is called by this probe.

Static verification passed:
- exact six-byte guard present;
- only bpp-load instruction is replaced;
- old full-block helper removed;
- old 36-byte patch removed;
- no direct helper call remains at 0x004FFB75;
- instruction cache flush covers six bytes;
- existing DX/crash diagnostics remain enabled.

Interpretation for next run:
- if `DXinit.cpp:1005 / 0x80004001` disappears or moves, 16-bpp SetDisplayMode is the compatibility problem;
- if the exact same error remains, exclusive SetDisplayMode itself is unsupported and the next fix should move to windowed/borderless DirectDraw initialization instead of color-depth retrying.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. User will provide all generated logs by default.


## 32-bpp retail probe still E_NOTIMPL — exclusive fullscreen path is the blocker — 2026-09-29

Latest test at revision `377991e26604e6fdaa23dda955a38a732ba05882`:
- clean forced rebuild/link succeeded;
- proxy SHA-256 `36434A263AD9921061F9DC4435A1DDF6F653D43215706A83C94C84F577258480`;
- EXE fingerprint unchanged;
- live retail code confirms the direct bpp patch installed:
  `BA 20 00 00 00 90` = `mov edx,32; nop`;
- the original retail SetDisplayMode vtable call remains intact;
- despite forcing 32 bpp, the exact same D3D error remains:
  - HRESULT `0x80004001` / DDERR_UNSUPPORTED / E_NOTIMPL
  - original `DXinit.cpp` line 1005
  - same error-report call site;
- secondary cleanup crash remains `DXSOUND_ShutDown()+0x7` null read.

Conclusion:
- 16-bit color depth is NOT the compatibility blocker;
- exclusive fullscreen `IDirectDraw7::SetDisplayMode` itself is unsupported/failing on this environment;
- stop testing alternate bpp values.

**ACTIVE NEXT STEP:** identify and patch the retail branch that selects the game's existing windowed DirectDraw path (`gDxOptionRelated`) instead of the exclusive fullscreen path. Reuse the game's own windowed surface/clipper code rather than suppressing SetDisplayMode errors or inventing a new renderer path.


## Built-in windowed DirectDraw route implemented — 2026-09-29

Implementation commit:
`36570947fa515636c9f3326282295c0e5fd2a37a`

Latest evidence:
- forcing the retail SetDisplayMode call to 32 bpp still produced the same `0x80004001` at the same original error site;
- therefore color depth is not the blocker;
- exclusive fullscreen SetDisplayMode is the compatibility failure.

Relevant reconstructed source:
- retail caller uses `DXINIT_DirectX8(hwnd, hInstance, 2)`;
- `DXINIT_DirectX8` computes `gDxOptionRelated = a3 & 1`;
- `initDirectDraw7` uses `gDxOptionRelated != 0` to select the game's own windowed DirectDraw path using `DDSCL_NORMAL`, primary/offscreen surfaces, and a clipper;
- changing the third DXINIT argument from 2 to 3 preserves bit 1 and enables bit 0.

Current patch strategy:
- no SetDisplayMode detour;
- no bpp forcing;
- scans retail .text `0x00401000..0x0053B000` for direct calls targeting retail `DXINIT_DirectX8` at `0x004FDE90`;
- within a bounded 20-byte window before each matching call, looks for exactly one `push 2` (`6A 02`);
- requires exactly one unambiguous call-site match in the whole text range;
- verifies the call still resolves to `0x004FDE90`;
- patches only the immediate byte from `2` to `3`;
- flushes instruction cache;
- logs push site/call site/target to `spidey-decomp-compat.log`;
- refuses to patch on ambiguity or verification failure.

Static verification passed:
- new windowed initializer installer present;
- previous SetDisplayMode installer removed;
- previous direct 32-bpp probe removed;
- target address correct;
- bounded scan + uniqueness check present;
- immediate changes only `02 -> 03`;
- launcher still collects compat, DX, crash, session, and fingerprint logs.

User preference:
- user will send all logs after every test; treat the complete log set as the standard test input.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` then `TEST_LATEST_BUILD.bat`, and provide all generated logs.


## MAJOR MILESTONE: reaches start screen; new crash is stack overflow — 2026-09-29

Latest test at revision `172735331305286580fc1b76e1859c680f4fa77b`:
- clean forced build/link succeeded;
- proxy SHA-256 `3858F003D643B50DECAE2BA985AFF1FA4100AB2B44FEF1B20E65E81F1824C81E`;
- EXE fingerprint unchanged;
- compatibility patch installed successfully:
  - push site `0x00515BA9`
  - call site `0x00515BAD`
  - target `DXINIT_DirectX8 = 0x004FDE90`
  - argument changed `2 -> 3`;
- game successfully passed DirectDraw initialization;
- all splash screens played;
- game reached the start/title screen;
- pressing Enter/Start then crashed.

New process exit code:
- decimal `-1073741571`
- NTSTATUS `0xC00000FD`
- **STATUS_STACK_OVERFLOW**

This is a new failure class and confirms the previous DirectDraw startup blocker is fixed/worked around.

No `spidey-decomp-crash.log` was produced because the current vectored handler only logs `EXCEPTION_ACCESS_VIOLATION`.

**ACTIVE NEXT STEP:**
1. extend native crash diagnostics to handle `STATUS_STACK_OVERFLOW`;
2. reserve emergency exception stack space early using `SetThreadStackGuarantee` when available;
3. make the stack-overflow logging path minimal/safe;
4. capture EIP/registers plus a bounded stack window at the overflow;
5. trace the title/start-screen transition in source to identify likely recursion/re-entry caused by an active reconstructed patch.


## Start-screen stack-overflow diagnostic frontier — 2026-09-29

Latest runtime milestone:
- game reaches the title/start screen successfully;
- pressing Enter/Start causes process exit `0xC00000FD` (stack overflow);
- previous DirectDraw startup failure is no longer the active blocker.

Diagnostics added:
- `53f5b503cedd7f07e80e924ab798eb57579eac52`: stack-overflow-aware native crash capture with reserved exception stack, extended stack dump, and EBP return-chain logging;
- `3750f34e62c2be7c161665194402af28b05150f3`: launcher labels `0xC00000FD` explicitly.

A source audit of the currently active reconstructed modules did not find the simplest direct self-recursion pattern.

Next test:
- update and rebuild;
- reach the title screen;
- press Enter once;
- provide all generated logs, especially `spidey-decomp-crash.log`.


## Stack overflow pinpointed at retail texture lookup — 2026-09-29

Latest crash log:
- exception: `0xC00000FD` (stack overflow);
- EIP / exception address: `0x004C9460`;
- ESP: `0x000C2000`;
- stack contains an extremely repetitive alternating pattern:
  - `0x1004D9DC`
  - `0xE90B5F6E`
  repeated throughout the captured window;
- EBP chain is unreadable because the stack is exhausted.

Important source correlation:
- reconstructed `Spool_FindTextureEntry(u32 checksum)` is currently `@SMALLTODO`;
- it explicitly calls retail address `0x004C9460` as a temporary fallback;
- `patch_spool()` patches nearby spool functions including `0x004C9430` and `0x004C95C0`, but not `0x004C9460` itself.

This strongly suggests the title/menu transition is entering a recursion/re-entry loop involving retail texture lookup and one of the reconstructed spool/texture functions.

ACTIVE NEXT STEP:
1. map retail `0x004C9460` in names/symbol data;
2. locate all calls/references to `0x004C9460`;
3. map proxy address/offset `0x1004D9DC` to a reconstructed function;
4. remove the recursion at its source rather than increasing stack size.


## Texture-lookup recursion fix implemented — 2026-09-29

Crash evidence:
- stack overflow occurs at retail `Spool_FindTextureEntry(u32)` = `0x004C9460`;
- captured stack repeats the same DLL return address `0x1004D9DC` and checksum `0xE90B5F6E`, consistent with recursive re-entry through the temporary retail fallback.

Source finding:
- reconstructed `Spool_FindTextureEntry(u32 checksum)` contained:
  `func_ptr func = (func_ptr)0x004C9460; return func(checksum);`
- immediately after that unreachable return, the full hash-table lookup implementation was already present;
- upstream currently contains the same temporary fallback, so no upstream fix exists to merge.

Fix commit:
- `d33111220ab81d2f6ad4b1597cac6f72f1597799`
- removes the retail `0x004C9460` call-through;
- activates the existing `TextureChecksumHashTable[checksum & 511]` traversal;
- preserves the existing default-texture behavior.

Future address-resolution tooling:
- `d5d70161b5cc4d5a99bcc8724f0cc947e01ace02`: Release linker now emits `Release\spider.map`;
- `9b407fb170283f3dc2c1daa9a9de266d2a3f62ba`: test launcher copies it into each session as `proxy-link-map.txt`.

Static verification passed:
- `0x004C9460` fallback is absent from the reconstructed checksum lookup;
- hash-table traversal is reachable;
- default texture fallback remains;
- Release linker has /MAP enabled;
- clean target removes stale map;
- session logger captures the linker map.

Next runtime test:
- update and run the latest build;
- let the game reach the title/menu normally;
- do not assume Enter is required; simply note whether it crashes on its own or after input;
- provide all generated logs, including the new `proxy-link-map.txt`.


## Stack overflow fixed; missing-texture path now exposes access violation — 2026-09-29

Latest test at revision `446608a2b34fea0a5153ee9f41e670134d8c6af5`:
- clean forced build/link succeeded;
- proxy SHA-256 `0D022AAAA2CF37FC96E2B19A1EBF1DA89446CEE49148403C4A04D1CE136ED513`;
- EXE fingerprint unchanged;
- windowed DirectDraw compatibility patch still installs correctly;
- title screen remains reachable;
- user reports the game says it cannot find a texture after Enter;
- previous stack overflow is gone;
- new crash is `0xC0000005` at DLL address `0x1004DB78`;
- access is a read from `0x00000004`;
- crash stack contains checksum `0xE90B5F6E`.

This is strong evidence that removing the retail `0x004C9460` call-through broke the recursion successfully and exposed the next real issue in the reconstructed texture-miss fallback.

ACTIVE NEXT STEP:
1. resolve `0x1004DB78` against the captured `proxy-link-map.txt`;
2. inspect the exact source operation at that symbol/offset;
3. harden the missing-texture fallback so an absent checksum does not dereference an unavailable default texture;
4. preserve logging of the missing checksum for later asset-table correctness work.
