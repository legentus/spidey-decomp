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


## Start-menu highlight texture lookup corrected for hybrid retail/DLL state — 2026-09-29

Latest runtime result:
- previous texture-lookup stack overflow is gone;
- pressing Enter reaches `PShell_DrawHighlight`;
- missing checksum observed in crash stack: `0xE90B5F6E`;
- new failure is `0xC0000005` inside DLL `Spool_FindTextureEntry(u32)` at `0x1004DB78`;
- access target is `0x00000004`, consistent with dereferencing a null `SAnimFrame*` to read `pTexture`;
- linker map resolves function start:
  - `Spool_FindTextureEntry(u32) = 0x1004DB30`
  - crash = function + `0x48`;
- retail caller `0x0047A59E` maps to `PShell_DrawHighlight + 0xE`.

Root hybrid-state issue:
- reconstructed `TextureChecksumHashTable[512]` is DLL-owned state;
- retail menu/asset loading is expected to populate the retail game's live hash table, not necessarily the DLL copy;
- reconstructed `gAnimTable[13]` is also DLL-owned and is explicitly zeroed by `Bit_Init()`;
- live retail animation table is already defined as `G_ANIM_TABLE = 0x0056EA64`;
- therefore the old default fallback `gAnimTable[13]->pTexture` is unsafe in this hybrid runtime.

Live texture-table inference:
- `TextureChecksumHashTable` is 512 pointers = `0x800` bytes;
- reconstructed declaration order places it immediately before live `G_LOWGRAPHICS = 0x006B78F8`;
- inferred base is therefore `0x006B70F8`;
- this base is NOT trusted blindly.

Implementation commit:
`0d3a720777cb35a0785839604ed7f21a14224ba3`

New runtime behavior:
1. inspect untouched retail `Spool_FindTextureEntry` at `0x004C9460` (known size 132 bytes);
2. search those retail bytes for absolute address `0x006B70F8`;
3. only if the retail function itself embeds that exact address, accept it as the verified live texture hash-table base;
4. search the verified retail table first;
5. search the DLL-owned reconstructed table second;
6. on genuine miss, use live `G_ANIM_TABLE[13]` as default first, then DLL `gAnimTable[13]` only if populated;
7. log resolver state and genuine misses to `spidey-decomp-compat.log`;
8. if the inferred base is not corroborated by retail code, do not use it and dump the full 132-byte retail function to the compat log for exact follow-up analysis;
9. string texture lookup now reads live `G_TEXTUREENTRIES` instead of the DLL copy.

Secondary hardening commit:
`7a28adb2e2d7b2b9efe1ec07cee7424086a51885`
- `Spool_TextureAccess` no longer directly dereferences DLL `gAnimTable[13]`;
- it uses the same safe default helper;
- if no default exists, returns `-1` instead of dereferencing null.

Static verification passed:
- live hash resolver present;
- inferred base must be corroborated by retail machine code;
- retail table is searched before DLL table;
- local table remains as secondary path;
- live retail animation table is preferred for defaults;
- no remaining direct `gAnimTable[13]->pTexture` dereferences in spool.cpp;
- string lookup uses live retail texture entries;
- existing linker-map generation and collection remain enabled.

Next runtime test:
- update and run latest build;
- reach start screen and press Enter;
- provide all generated logs;
- especially inspect `spidey-decomp-compat.log` for either:
  - `texture_hash_table verified retail_base=0x006B70F8`, or
  - `texture_hash_table UNRESOLVED ... retail_code=...`.


## Runtime assertion logging added for texture/menu diagnostics — 2026-09-29

Additional diagnostics:
- `776f2a8c3d57b750f8a2b2ddb2db054c7f6d3e74`
  - `DoAssert` now preserves varargs formatting with `_vsnprintf`;
  - console output now shows actual values instead of raw format strings;
  - failed assertions are appended to `spidey-decomp-runtime.log`.
- `84c98daaa30c0c23282c02c1a8c3882b7f45b5d7`
  - latest-test launcher removes stale runtime log;
  - copies fresh runtime log into the timestamped session;
  - reports it as `[RUNTIME]`.

Current test frontier:
- DirectDraw startup compatibility remains solved by the built-in windowed path;
- prior texture-lookup stack overflow is solved;
- current focus is start-menu highlight texture lookup after Enter;
- live retail texture hash-table resolver + safe retail default texture handling are implemented;
- next run should reveal whether `0x006B70F8` is corroborated by retail code and whether the menu highlight texture is found in the live table.

Next user action:
- run `UPDATE_SPIDEY_PROJECT.bat`;
- run `TEST_LATEST_BUILD.bat`;
- reach the start screen and press Enter;
- provide all generated logs, including:
  - `spidey-decomp-compat.log`
  - `spidey-decomp-runtime.log`
  - `spidey-decomp-crash.log` if present
  - `proxy-link-map.txt`
  - launcher output/session/fingerprint files.


## PLAYABLE MILESTONE + new priorities — 2026-09-29

User runtime report:
- game now boots through splash/title;
- user can enter the first level and control Spider-Man;
- user successfully quit back to main menu;
- **no sound at all** during current runtime;
- entering/changing Options crashes reproducibly;
- user provided two independent options-menu crash sessions;
- full modern controller support is now an explicit project requirement:
  - Xbox-style controller support;
  - analog stick/trigger handling;
  - configurable button mappings;
  - Xbox button/UI prompts.

Both options-menu crash logs agree on:
- exception: `0xC0000005`;
- exact retail EIP: `0x0043EB29`;
- fault module: `SpideyPC.exe`;
- first session read target `0x8A14244A`;
- second session read target `0x8A14270A`;
- both have `ESI=0x0043EAF0`;
- crash is therefore reproducible in the same retail function/path rather than a random heap fault.

DirectDraw compatibility remains active and game is now playable.

Texture compatibility observation from both sessions:
- inferred `0x006B70F8` hash-table base was NOT corroborated;
- retail machine code itself reveals an indexed absolute base operand `0x006AB934` in the lookup sequence;
- default texture resolves non-null (`0x006AD3C8`);
- current texture compatibility logging is very noisy and should be cleaned up after crash triage.

ACTIVE WORKSTREAMS:
1. map and fix options crash at `0x0043EB29`;
2. diagnose missing DirectSound/audio path without regressing gameplay;
3. design/implement modern XInput/Xbox controller layer with remapping and Xbox prompts after stability hooks are in place.


## Options crash root-cause fixed + exact retail texture table decoded — 2026-09-29

Options crash:
- both independent sessions faulted at retail `0x0043EB29`;
- `tools/names.json` maps function start `0x0043EAF0` to `Font::height(char*)`;
- crash offset: `Font::height + 0x39`;
- both linker maps show DLL return address `0x100237EC` immediately after the reconstructed `Font::height` wrapper calls retail;
- reconstructed wrapper incorrectly invoked the retail C++ instance method as a free function:
  `typedef i32 (*func_ptr)(char*); return func(txt);`
- this failed to pass the `Font* this` pointer in ECX.

Fix commit:
`a0a8aee2f2fcbd931c3d9eb80c8270da38711563`
- removes invalid retail call-through;
- uses already reconstructed implementation:
  `heightAboveBaseline(txt) + heightBelowBaseline(txt)`.

Texture resolver correction:
- retail `Spool_FindTextureEntry` bytes explicitly decode:
  - `mov eax,[eax*4 + 0x006AB934]` => exact live retail texture hash table base `0x006AB934`;
  - default-texture flag at `0x006B2F08`;
  - retail animation-table slot 13 at `0x0056EA98`;
- previous inferred `0x006B70F8` was correctly rejected by runtime validation.

Correction commit:
`b3af5be5087b6cf7a2003aaf70daeb0c621c24d7`
- live hash resolver now verifies and uses exact `0x006AB934`;
- reads live retail default-texture flag;
- throttles repeated identical texture-miss logging.

Both commits are implemented but not yet runtime-tested.

Current priorities:
1. runtime-test the Options fix while preserving first-level playability;
2. diagnose/fix total absence of audio;
3. add XInput/Xbox controller support through the existing PCINPUT/Pad abstraction, preserving remapping support and adding Xbox button prompts/UI.


## Audio diagnostics + first XInput/Xbox backend implemented — 2026-09-29

### Audio diagnostic implementation

Retail `DXSOUND_Init` machine code was decoded from the preserved function artifact and gives exact retail globals:
- `g_pDS = 0x006B7920`;
- `gDxSoundBuffers[128] = 0x006BBAD4`;
- `gDxSoundHolder[32] = 0x006BBD50`;
- `g_pDSBuffer = 0x006BBF1C`.

The retail function:
- creates the primary buffer through `g_pDS`;
- calls `SetVolume(0)`;
- starts the primary buffer looping;
- already reports failed DirectSound HRESULTs through the hooked DS error reporter.

`SDDXSoundHolder` is verified 12 bytes and its first field is `LPDIRECTSOUNDBUFFER pDSB`, so active-voice diagnostics read the correct field.

Commit:
`7d42a06b1c70f8c706748926a8bfb3f5754d68ff`

Diagnostic behavior:
- the existing `DXINIT_DirectX8` call-site compatibility patch now redirects through a wrapper that calls untouched retail `0x004FDE90` and logs DirectSound state afterward;
- direct retail calls to:
  - `SFX_Init = 0x004718B0`
  - `SFX_SpoolInLevelSFX = 0x004719B0`
  are redirected through behavior-preserving diagnostic wrappers;
- wrappers call the original retail function, then log:
  - retail DirectSound device pointer;
  - primary buffer pointer;
  - number of non-null loaded sample buffers;
  - number of active voice buffers;
- output: `spidey-decomp-audio.log`.

Launcher collection:
`d32ef72818aecd7c4747ad43b3fe9f8bee79e922`

No reconstructed audio subsystem has been substituted yet; this remains diagnostic-only to preserve the playable retail path.

### XInput / Xbox controller backend phase 1

Retail controller conventions were verified from `DXINPUT_PollController = 0x00501E50` machine code:
- X/Y axes use range `-1000..+1000`;
- POV uses DirectInput hundredths-of-degrees;
- button state semantics:
  - `0xFF` newly pressed;
  - `0x7F` held;
  - `0x80` newly released;
  - `0x00` idle.

Retail controller entrypoints:
- setup: `0x00501890`;
- poll: `0x00501E50`;
- get button state: `0x00501FB0`;
- setup FF: `0x00501FC0`;
- start FF: `0x005021A0`;
- stop FF: `0x005021E0`;
- get button count: `0x00502210`.

Implementation commit:
`568c9c20148a382c77c34e6c246afa9e556222f0`

Backend:
- dynamically loads `xinput1_4.dll`, then `xinput1_3.dll`, then `xinput9_1_0.dll`;
- scans users 0..3 and uses first connected XInput controller;
- keyboard/mouse path remains untouched when no controller exists;
- left stick converted to retail -1000..+1000 range with XInput deadzone;
- D-pad converted to retail POV angles;
- LT/RT exposed as remappable digital buttons with threshold;
- press/held/release states match retail encoding;
- XInput rumble integrated with existing force-feedback start/stop interface.

Stable Xbox button index mapping deliberately preserves the retail default action table:
- 0 = X
- 1 = A
- 2 = View
- 3 = B
- 4 = Y
- 5 = LT
- 6 = LB
- 7 = RT
- 8 = LS
- 9 = RB
- 10 = RS
- 11 = Menu
- 12..15 = D-pad U/D/L/R

This makes existing defaults map naturally:
- Smart Bomb -> X
- Jump -> A
- Crouch -> B
- Select Weapon -> Y
- shoulder/trigger actions -> LB/RB/RT
- Start -> Menu

Xbox configuration UI:
- retail `initActionMaps = 0x0050D0F0`;
- exact `sprintf("button %i")` call is at `0x0050D28C`;
- only that call is redirected to Xbox-name formatter;
- Options controller mappings show `A/B/X/Y/LB/RB/LT/RT/View/Menu/LS/RS` rather than generic button numbers.

Controller log:
- `spidey-decomp-controller.log`;
- records selected XInput DLL, rumble availability and connected user index.

Launcher collection:
`deee34e11ad38c85c4f22980d32dd212c775a40e`

### Controller feature scope still remaining

Phase 1 covers:
- XInput detection;
- left-stick movement;
- D-pad;
- Xbox face/shoulder/trigger/Menu/View/stick-click buttons;
- remapping through the existing game mapping system;
- Xbox labels in the controller configuration UI;
- rumble.

Still to implement after runtime validation:
- right-stick integration where appropriate for Spider-Man's camera/UI semantics;
- broader in-game Xbox prompt/icon replacement outside the controller configuration screen;
- persistence/UX edge-case testing across disconnect/reconnect and restored defaults.

### Next runtime test

Run latest update/build and verify:
1. title -> Options no longer crashes;
2. changing several Options values works;
3. first level still loads and remains playable;
4. note whether any sound is heard;
5. with Xbox/XInput controller connected:
   - left-stick movement;
   - A/B/X/Y;
   - LB/RB/RT;
   - Menu/Start;
   - D-pad/menu navigation;
   - controller remapping screen and Xbox labels;
   - rumble if encountered.

Return all logs. New important logs:
- `spidey-decomp-audio.log`
- `spidey-decomp-controller.log`
plus usual compat/runtime/crash/map/session/fingerprint logs.


## REGRESSION: black screen + proxy DLL crash — 2026-09-29

Runtime test of revision:
`8c167ecf781029226ae8cd36a422cf597861e905`

Observed by user:
- game launched to a black screen;
- never reached normal playable/title state;
- eventually crashed.

Crash log:
- exception `0xC0000005`;
- write access violation;
- fault address `0x1002C8D1`;
- write target `0x0000001F`;
- fault module is rebuilt proxy `binkw32.dll`;
- proxy base `0x10000000`;
- proxy offset `0x0002C8D1`.
This is a NEW regression in our DLL, not the previous retail Options crash at `0x0043EB29`.

Other evidence from same failed run:
- retail EXE fingerprint is unchanged;
- windowed DirectDraw compatibility patch installed;
- verified texture hash table `0x006AB934` still accepted;
- XInput DLL `xinput1_4.dll` loaded with rumble support;
- audio diagnostics show retail DirectSound device + primary buffer are valid;
- `SFX_Init` loaded 42 buffers;
- level/menu SFX spool raised this to 44 buffers;
- no active voices were observed at those diagnostic checkpoints;
- a D3D diagnostic fired with error value `0x00000004` from retail caller `0x004FDDD4` / probable call site `0x004FDDCF`, before the windowed compatibility state had been marked seen.

IMMEDIATE NEXT ACTION:
1. map proxy crash `0x1002C8D1` exactly through this run's link map;
2. identify which new change owns that instruction;
3. revert/fix only the crashing regression before further feature work;
4. preserve the audio evidence, because it already proves DirectSound initialization and bank loading are succeeding.


## Black-screen regression mapped + startup-active experiments parked — 2026-09-29

Exact crash mapping from uploaded current-build linker map:
- proxy crash: `0x1002C8D1`;
- current `DCMem_New` start: `0x1002C880`;
- fault offset: `DCMem_New + 0x51`;
- stack return `0x10037B33`;
- current `PCTex_CreateTexture256` start: `0x100379A0`;
- caller offset: `PCTex_CreateTexture256 + 0x193`.

The source and original retail machine code establish the failure mechanism:
- `PCTex_CreateTexture256` allocates its temporary converted texture buffer with
  `DCMem_New(2 * rounded_width * rounded_height, 0, 1, 0, 1)`;
- `DCMem_New` calls `Mem_CoreNew`;
- `DCMem_New` has no null check before calculating its alignment result;
- if the underlying allocation returns null, the aligned result becomes `0x20` and it writes the alignment byte to `0x1F`;
- current crash log is exactly a write AV to `0x0000001F`.

This identifies the immediate fault but does NOT yet prove why the underlying texture allocation failed.

Important regression-scope facts:
- `PCTex_CreateTexture256` and `DCMem_New` were already active in the previously playable build;
- therefore the crash is most likely an exposed consequence of one of the newly activated startup paths rather than a newly introduced allocator implementation;
- audio diagnostics already established that DirectSound initialized correctly and sample banks loaded before the crash;
- XInput DLL loaded but no controller connection was logged.

Stability rollback / bisect commits:

`50e5ea75f25e20c7792b7ba320c00697b7b8aceb`
- keeps exact decoded retail texture table address `0x006AB934` and verification;
- disables runtime consumption of that table for now;
- restores prior DLL-owned default-texture gating behavior;
- reason: previous playable build had the retail table unresolved, while the failed build was first to actively consume the decoded retail table.

`7eef8ba810b23727a976dfe593849c7c3114834b`
- restores direct retail `DXINIT_DirectX8` call flow;
- keeps only the proven RealWinMain argument patch `2 -> 3`;
- disables active SFX diagnostic call redirections now that their evidence has been captured;
- parks XInput/Xbox runtime hooks and Options label hook;
- XInput/audio implementation remains compiled in source for later one-at-a-time reactivation.

The Options `Font::height` fix remains ACTIVE.

NEXT TEST PURPOSE:
- confirm known-good title/gameplay startup is restored;
- test Options crash fix independently of texture/audio/controller experiments.

If startup is restored:
1. test entering Options and changing settings;
2. confirm first level still plays;
3. audio is expected to remain unresolved for this isolation run;
4. controller phase 1 is intentionally inactive for this isolation run.

If the same `DCMem_New/PCTex_CreateTexture256` crash persists after this rollback, next step is to add narrowly scoped allocation telemetry around the exact PCTex buffer request and game-heap state.


## Black screen persists after startup-hook rollback — 2026-09-29

Runtime test of revision:
`4e4f2b1ddc706b49237512669abcc92ff9b92238`

Observed:
- game window appears;
- screen remains completely black;
- no sound;
- no automatic crash;
- process remains alive/hung until user force-closes it in Task Manager.

Uploaded evidence:
- retail EXE fingerprint unchanged;
- windowed DirectDraw arg patch installs at the expected retail RealWinMain call;
- retail texture table address `0x006AB934` verifies, but runtime consumption is disabled (`runtime_use=0`);
- one texture miss is logged for checksum `0xE90B5F6E`, falling back to non-null default texture `0x006AD3C8`;
- no crash log exists for this run because the process did not fault.

Conclusion:
- black-screen regression is NOT caused solely by the parked XInput hooks, parked audio wrappers, or active consumption of the retail texture hash table;
- regression predates those changes and must be isolated against the last user-confirmed playable revision rather than by further speculative patches.

Immediate next action:
1. identify exact last revision/run that reached title + first level;
2. diff startup-affecting code from that revision to current dev;
3. restore/bisect only those deltas;
4. do not add new feature work until title/gameplay baseline is recovered.


## Exact confirmed-playable runtime restored; Options fix reduced to one trampoline — 2026-09-29

Historical evidence lookup:
- both user-provided Options-crash sessions were built from revision
  `35e73ed3c4ca8c06b581df83f0f0913f0c915982`;
- that is therefore the exact last user-confirmed playable runtime:
  - splash/title visible;
  - first level entered;
  - Spider-Man controllable;
  - return to main menu worked;
  - only entering Options crashed.

Diff from that exact baseline to the black-screen tree showed only three runtime C++ files changed:
- `main.cpp`;
- `spool.cpp`;
- `FontTools.cpp`.
Other differences were docs and launcher logging only.

Recovery commits:
- `ae7bb3432ecfd03bfa6ebbd8e3122465755f64eb`
  - restores `main.cpp` byte-for-byte from confirmed playable `35e73ed...`;
  - removes all active/inactive audio/XInput experiment code from the current runtime file for this isolation build.
- `2dbb4d896280e82ad921f701213926669a483e9c`
  - restores `spool.cpp` byte-for-byte from confirmed playable `35e73ed...`;
  - returns texture lookup/default behavior exactly to the runtime that was known playable.
- `b8bc0957721c29499737741f1fd9c31740e7cf49`
  - starts from confirmed-playable `FontTools.cpp`;
  - changes ONLY the broken `Font::height` retail trampoline;
  - old broken declaration:
    `typedef i32 (*func_ptr)(char*);`
  - new declaration:
    `typedef i32 (FASTCALL *func_ptr)(Font*, void*, char*);`
  - call:
    `return func(this, 0, txt);`
  - rationale: x86 C++ instance method needs `this` in ECX; this mirrors the already-proven `Font::width` retail trampoline pattern and preserves retail behavior rather than substituting reconstructed height logic.

Mechanical verification:
- current `main.cpp` == exact contents at playable `35e73ed...`: PASS;
- current `spool.cpp` == exact contents at playable `35e73ed...`: PASS;
- old broken Font::height free-function trampoline absent: PASS;
- FASTCALL Font*/dummy-EDX trampoline present: PASS;
- compare against playable baseline now shows runtime-code difference ONLY in `FontTools.cpp`;
- remaining non-runtime differences are documentation and test-log collection.

NEXT TEST:
1. update/build;
2. verify splash/title returns;
3. if title returns, enter Options and change settings;
4. load first level once;
5. send all logs.

Interpretation:
- if black screen STILL occurs, then the single Font::height trampoline change itself is implicated and should be reverted for a pure baseline confirmation;
- if startup returns, the prior black-screen regression was in the post-playable main/spool experiment set and is now eliminated;
- audio/controller feature work remains paused until the playable baseline is reconfirmed.


## Black screen persists with only Font::height differing from confirmed playable runtime — 2026-09-29

Latest tested revision:
`d6c077ff95193cf033ea310169e10d24777a275f`

User result:
- black screen;
- no sound;
- no automatic crash reported.

Uploaded evidence:
- retail EXE fingerprint remains unchanged:
  SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`;
- DirectDraw windowed compatibility patch still installs at:
  - push `0x00515BA9`
  - call `0x00515BAD`
  - target `0x004FDE90`;
- texture behavior matches the confirmed-playable-era implementation:
  - inferred retail base `0x006B70F8` remains unresolved;
  - checksum `0xE90B5F6E` repeatedly misses;
  - non-null default texture `0x006AD3C8` is returned.

At this revision:
- `main.cpp` is byte-for-byte equal to confirmed-playable `35e73ed...`;
- `spool.cpp` is byte-for-byte equal to confirmed-playable `35e73ed...`;
- the only remaining runtime C++ difference is the isolated `Font::height` FASTCALL trampoline.

NEXT ACTION:
- restore `FontTools.cpp` exactly from confirmed-playable `35e73ed...`;
- also restore the test runner from that exact revision to remove non-runtime launcher/logging deltas from the control test;
- perform a PURE BASELINE test with no runtime code differences from the user-confirmed playable revision.

If that pure baseline still black-screens, source regression is ruled out and investigation must move to local/environment state (game config, preserved retail Bink DLL, generated state/files, registry/settings, or other installation differences).


## PURE confirmed-playable baseline prepared — 2026-09-29

Control revision source:
`35e73ed3c4ca8c06b581df83f0f0913f0c915982`

This is the exact revision from both user sessions that:
- reached splash/title;
- entered first level;
- allowed Spider-Man control;
- returned to main menu;
- crashed only when entering Options.

Pure-baseline restoration:
- `a3acab00d81c22d63732a190846bb8aa03b4fce3`
  - restores `FontTools.cpp` exactly from `35e73ed...`;
  - removes the isolated Font::height experiment entirely.
- `00b2165e8fc792c900d29416fa165a078e01f8b6`
  - restores `tools/TEST_LATEST_BUILD.ps1` exactly from `35e73ed...`.

Mechanical GitHub comparison against `35e73ed...` now reports:
- NO runtime-source differences;
- NO test-runner differences;
- ONLY `docs/CURRENT_STATUS.md` differs.

Therefore the next test is a true source/runtime control test.

Parallel external-state investigation:
- retail `SPIDEYDX_LoadSettings = 0x00515680` is real and reads persistent settings before startup;
- if pure baseline still black-screens, likely causes move outside current source tree:
  - persistent game/settings state;
  - preserved retail `binkw32_.dll` contents;
  - local installation/generated data state;
  - graphics/runtime/driver state;
  - other external environment changes.
- do NOT resume Options/audio/controller feature work until this control test result is known.

NEXT TEST:
- run updater;
- run latest test;
- no feature validation needed;
- report only whether splash/title returns or remains black, plus all logs as usual.


## Pure baseline runs with audio/input but renders black — 2026-09-30

Latest tested revision:
`f24a5d1b734a33e60d476ddcc43c34b8cb7d7643`

This revision is mechanically identical to the last user-confirmed playable runtime `35e73ed3c4ca8c06b581df83f0f0913f0c915982` except for documentation.

User result:
- game boots;
- audio is audible again;
- Start input works;
- user can enter the main menu after pressing Start;
- screen remains completely black throughout;
- no crash reported.

Uploaded evidence:
- retail EXE fingerprint remains unchanged:
  SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`;
- windowed DirectDraw compatibility patch installs at the expected retail caller:
  - push `0x00515BA9`
  - call `0x00515BAD`
  - target `DXINIT_DirectX8 = 0x004FDE90`
  - argument `2 -> 3`;
- no crash evidence.

Conclusion:
- source regression is ruled out by the pure-baseline control;
- game logic, input, audio, and shell progression are functioning;
- active failure is specifically visible presentation/render output;
- investigate the existing retail windowed DirectDraw path:
  1. primary/front surface creation;
  2. offscreen/back/scene surface creation;
  3. clipper/window association;
  4. final Blt/Flip/present call and its HRESULT;
  5. source/destination rectangles and surface-loss state.

Do not resume Options/audio/controller feature work until visible rendering is restored.


## Windowed presentation probe implemented — 2026-09-30

Current observed runtime:
- pure confirmed-playable source runs;
- audio is audible;
- Start input works;
- main-menu state advances;
- visible output remains black.

Retail render/present path decoded:
- `DXPOLY_EndScene = 0x00502A40`;
- when presentation is requested, retail calls:
  `DXPOLY_Flip = 0x00502990`;
- exact call site:
  `0x00502D41`
  with original bytes:
  `E8 4A FC FF FF`;
- windowed `DXPOLY_Flip` checks `gDxOptionRelated = 0x006B78F4`;
- windowed path performs:
  primary surface `0x006B7904`
  `Blt(gRect, scene surface 0x006B7908, ... DDBLT_WAIT ...)`;
- stored destination rectangle is `gRect = 0x006B5958`;
- retail game HWND is `0x006B58D0`;
- retail resolution globals used for diagnostics:
  - width `0x02E096F8`
  - height `0x02E0970C`
  - bpp `0x02E098E4`;
- low-graphics flag: `0x006B78F8`.

Implementation commit:
`6db8ea90d2e6ebe27aa61a5eeaccdc890674915f`

Probe behavior:
1. exact-byte guard verifies retail call site and target;
2. replaces ONLY the direct call at `0x00502D41`;
3. wrapper calls untouched retail `DXPOLY_Flip(0x00502990)`;
4. before present:
   - reads current client rect;
   - converts it to screen coordinates;
   - compares against stored retail `gRect`;
   - if stale/different, refreshes `gRect` before the retail Blt;
   - logs whether correction occurred;
   - records scene-surface pointer, dimensions, pitch, bpp, caps, loss state;
   - samples a 3x3 pixel grid from the scene surface and records hash/non-black count;
5. after retail present:
   - records primary-surface state;
   - samples a 3x3 pixel grid over the current destination rectangle;
6. logs frames 1..5, every 120th frame, and every frame where the destination rectangle is corrected.

Output:
`spidey-decomp-present.log`

Launcher collection commit:
`938229983c79c8d91436f54c9d07a13b90bd3ef8`

Static verification passed:
- exact call-site guard present;
- retail flip entry itself is not patched;
- wrapper always returns through untouched retail flip;
- destination rectangle refresh is bounded to a real current client rectangle;
- scene/primary samples enabled;
- log cadence is sparse;
- launcher clears and captures fresh presentation log.

Interpretation of next run:
- scene non-black + primary non-black => DirectDraw rendering and Blt work; investigate desktop/window composition/visibility;
- scene non-black + primary black => final Blt/presentation failure despite no reported HRESULT;
- scene black => rendering into offscreen scene surface is failing/being cleared;
- `corrected=1` followed by visible output => stale window destination rectangle was the compatibility bug.

Next user action:
- update/build;
- launch normally;
- if still black, let it run through splash/start/menu for at least ~10 seconds;
- send all logs, especially `spidey-decomp-present.log`.


## PRESENTATION ROOT CAUSE ISOLATED — scene renders, primary never updates — 2026-09-30

Latest test revision:
`527aa0ba2fea79865d4b06e683defd2e59a169cc`

User result:
- game remains visually a black box;
- audio/game state continue to run underneath.

Presentation probe evidence:
- probe installed successfully at retail `DXPOLY_EndScene -> DXPOLY_Flip` call site `0x00502D41`;
- retail windowed state:
  - option=1
  - lowgfx=0;
- stored destination rect and live client rect agree exactly:
  `0,0,640,480`;
- therefore stale rectangle is ruled out;
- scene surface:
  - ptr non-null;
  - not lost;
  - 640x480;
  - 32bpp;
  - GetDC succeeds;
  - 3x3 sample reports 9/9 non-black pixels;
- primary surface:
  - ptr non-null;
  - not lost;
  - 1920x1080;
  - 32bpp;
  - GetDC succeeds;
  - 3x3 sample reports 9/9 non-black pixels.

CRITICAL TEMPORAL EVIDENCE:
- scene sample hash changes from
  `0x7E0B5BDA`
  to
  `0x3B302417`
  by frame 360, proving the game is continuing to render changing frames;
- primary sample hash remains
  `0x3565BD06`
  on every sampled frame through frame 480;
- therefore the retail windowed presentation path is not propagating the rendered scene to the visible primary/display output.

Conclusion:
- game rendering itself is working;
- game loop/audio/input are working;
- stale gRect is ruled out;
- active compatibility defect is specifically the retail DirectDraw primary-surface windowed Blt/presentation behavior on this system.

NEXT IMPLEMENTATION:
- preserve retail renderer and offscreen scene surface;
- preserve retail DXPOLY_Flip call for state/error behavior;
- add a windowed compatibility presenter that copies the already-rendered scene surface directly into the HWND client DC after retail flip;
- use scene-surface GetDC + window GetDC + StretchBlt/BitBlt;
- only activate when gDxOptionRelated indicates windowed mode;
- log copy dimensions and Win32 result;
- leave fullscreen retail path untouched.


## Direct-window compatibility presenter implemented — 2026-09-30

New runtime evidence from `spidey-decomp-present.log` proves:
- scene surface contains non-black pixels from frame 1 onward;
- scene sample hash changes during runtime, so rendered content is updating;
- primary surface sample hash remains constant across all sampled frames;
- stored and live client rects both remain `0,0,640,480`;
- therefore stale rectangle is ruled out;
- failure is specifically the retail windowed DirectDraw primary-surface presentation step.

Implementation commit:
`cf3d827a11958464c73a2ac8a1ee1d1532a03d6a`

Compatibility behavior:
1. existing exact-call-site wrapper still invokes untouched retail `DXPOLY_Flip(0x00502990)` first;
2. only when retail `gDxOptionRelated` indicates windowed mode:
   - gets the already-rendered scene surface at retail `0x006B7908`;
   - reads actual HWND client dimensions;
   - gets a GDI DC from the scene surface;
   - gets the real HWND client DC;
   - uses `BitBlt` when scene/client sizes match;
   - uses `StretchBlt(COLORONCOLOR)` when sizes differ;
   - calls `GdiFlush`;
   - releases both DCs;
3. fullscreen path is untouched;
4. original renderer, D3D device, scene surface and retail flip still run normally.

Current observed dimensions make the common path:
- scene = 640x480;
- HWND client = 640x480;
- therefore direct `BitBlt`.

Presentation log now also records:
`compat_present frame=<n> result=<0/1> error=<win32> src=<w>x<h> dst=<w>x<h> stretch=<0/1>`

Static verification passed:
- retail flip occurs before compatibility copy;
- windowed guard present;
- source is scene surface, not primary;
- BitBlt + StretchBlt fallback present;
- fullscreen untouched;
- result logging present.

NEXT TEST:
- update/build;
- launch normally;
- if image appears, verify splash/title/menu visibility;
- if still black, let it run at least 10 seconds and provide all logs;
- key line will be `compat_present ... result=...` in `spidey-decomp-present.log`.


## Presentation probe result: scene renders, visible presentation path is broken — 2026-09-30

Latest tested revision:
`527aa0ba2fea79865d4b06e683defd2e59a169cc`

User result:
- game remains a black visible window/box;
- game audio is audible;
- game continues accepting input and advancing state underneath the black output.

Presentation probe evidence:
- probe installed successfully at retail `DXPOLY_EndScene -> DXPOLY_Flip` call site `0x00502D41`;
- retail flip target remains `0x00502990`;
- windowed mode flag is active;
- HWND is valid;
- stored and live client rectangles both remain `0,0,640,480`;
- no stale-rectangle correction was needed;
- offscreen scene surface:
  - valid pointer;
  - `640x480`;
  - 32 bpp;
  - not lost;
  - GetDC succeeds;
  - all sampled points are non-black;
- primary DirectDraw surface:
  - valid pointer;
  - desktop-sized `1920x1080`;
  - 32 bpp;
  - not lost;
  - GetDC succeeds;
  - all sampled points are non-black.

Critical temporal result:
- scene sample hash is initially `0x7E0B5BDA`;
- by frames 360/480 it changes to `0x3B302417`, proving rendered scene content changes over time;
- primary sample hash remains frozen at `0x3565BD06` across frames 1..480;
- therefore the game renderer is generating changing visible pixels in the offscreen scene surface, but the changing scene is not reaching the user-visible window through the legacy DirectDraw primary-surface path.

Conclusion:
- rendering generation is working;
- game logic/input/audio are working;
- the black-window bug is isolated to presentation/composition after the offscreen scene surface;
- stale `gRect` is ruled out;
- surface-loss is ruled out;
- source scene being black is ruled out;
- do NOT modify gameplay, texture generation, D3D scene rendering, audio, or input while fixing this.

**NEXT FRONTIER / RECOMMENDED NEXT ACTION:**
Implement a narrow compatibility presenter that bypasses the legacy DirectDraw primary-surface/DWM path:
1. keep retail rendering into `g_pDDS_Scene` unchanged;
2. after retail `DXPOLY_Flip` (or instead of its windowed primary Blt), acquire the scene surface DC or lock/read the 32-bpp scene surface;
3. present those already-rendered pixels directly to the actual game HWND using a controlled GDI path (`BitBlt`/compatible DC or `StretchDIBits`);
4. first implement as a diagnostic compatibility probe, not a renderer rewrite;
5. if the image appears, classify the bug as modern-Windows/DWM incompatibility with legacy DirectDraw primary-surface presentation and retain the direct HWND presenter as the compatibility solution;
6. once visible rendering is restored, resume the parked Options fix, audio follow-up, and XInput/Xbox controller work one at a time.

Do not spend another cycle on DirectDraw SetDisplayMode, bpp, texture lookup, or scene rendering before trying the direct HWND presentation probe.


## New-chat recovery checkpoint — 2026-09-29

Recovery sources checked:
- full disconnect-safe handoff ZIP manifest: PASS;
- prior Spider-Man 2000 project conversation context recovered;
- live GitHub branch `dev` confirmed;
- current branch head before this checkpoint: `974fbf073a9134de63f9d0102508bfcdb19a8805`;
- latest archived presentation test remains revision `527aa0ba2fea79865d4b06e683defd2e59a169cc`.

Important correction to the handoff wording:
- the direct-to-HWND compatibility presenter is **already implemented** in ancestor commit
  `cf3d827a11958464c73a2ac8a1ee1d1532a03d6a`;
- it is present in the current `dev` history through documentation checkpoint
  `6969ecf9c5290cee586cf0db4c6c4c922df4b0dd`;
- the archived latest runtime logs predate that implementation, so there is no runtime result for the direct-window presenter yet.

Therefore the exact current frontier is **runtime validation**, not reimplementation.

NEXT TEST:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. observe whether splash/title/menu become visible;
4. if still black, leave the game running at least ~10 seconds;
5. return all generated logs, especially `spidey-decomp-present.log`;
6. key evidence is the new line:
   `compat_present frame=<n> result=<0/1> error=<win32> src=<w>x<h> dst=<w>x<h> stretch=<0/1>`.

Do not redo SetDisplayMode/bpp, texture-table, stale-rectangle, scene-black, or surface-loss investigation before this test.


### Authoritative retail Google Drive source

Retail PC game source folder:
https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

Verified direct folder inventory includes:
- `SpideyPC.exe` — 1,507,328 bytes;
- `binkw32.dll` — original retail Bink DLL;
- `data.pkr`;
- `media.pkr`;
- `texture.dat`;
- setup/support binaries;
- `Docs` and `Uninstall` folders.

This Drive folder is reference/input material only. Do not commit retail binaries or PKR assets to Git.


## Runtime result: direct presenter works for menu; mouse crash + resolution/splash follow-up — 2026-09-29

Tested revision:
`a74074b77fe18adb254cd1a5b67c448a81b4d3cd`

User-visible result:
- splash screens remain black;
- once the game reaches the start menu, the image becomes visible;
- moving the mouse causes an immediate crash;
- resolution cannot currently be changed as desired;
- user explicitly requires native 2560x1440 (1440p) support.

Presentation evidence:
- direct HWND presenter is executing successfully:
  `compat_present ... result=1 error=0 src=640x480 dst=640x480 stretch=0`;
- scene surface remains 640x480 / 32 bpp;
- window client remains 640x480;
- retail resolution globals report 1280x1024 / 32 bpp during this run;
- therefore the direct GDI compatibility presenter is sufficient to expose normal shell/menu scene rendering, but splash/movie presentation uses a different path and is not yet handled by this presenter.

Crash evidence:
- exception: `0xC0000005`;
- retail EIP: `0x0043EB29`;
- this is the exact previously identified `Font::height(char*) + 0x39` failure site;
- current build's DLL stack return `0x100236FC` maps to reconstructed `Font::height(char*)` at `0x100236F0 + 0xC`;
- another return `0x1002CC0F` maps to `Mess_TextHeight + 0xF`;
- conclusion: mouse movement is driving a shell/UI text-height path and reproducing the same broken retail C++ instance-method trampoline that previously crashed Options.

Immediate implementation order:
1. restore the isolated, previously prepared FASTCALL `Font::height` trampoline fix that passes `this` in ECX;
2. inspect resolution enumeration/storage/surface creation and add native 2560x1440 support without hardcoding only the presenter;
3. trace the Bink/splash presentation path separately from the normal scene presenter so movies become visible too.

Do not regress the now-working direct HWND menu presentation path.


## Compatibility fix batch ready: mouse crash + splash movies + native modern resolutions — 2026-09-29

Implementation commits:
- `0addc001023f61886c34c33c50a7ae314f09a11f`
  - fixes reconstructed `Font::height(char*)` trampoline;
  - retail method is now invoked with FASTCALL-compatible `this` in ECX;
  - directly addresses the latest mouse-move crash at retail `0x0043EB29`.
- `23a5e77e51313edf52e06d9aa3904cc6c86ade78`
  - adds movie/splash presentation compatibility;
  - scans retail `PCMOVIE_NextFrame 0x0050B5A0..0x0050B790`;
  - verified retail machine code contains exactly one direct `DXPOLY_Flip` call at `0x0050B71A`;
  - redirects that movie-specific call through the already working direct-HWND presenter;
  - does NOT globally replace `DXPOLY_Flip`.
  - adds modern-resolution restoration/injection:
    - saved settings globals: `0x02E096F8/0x02E0970C/0x02E098E4`;
    - live DX globals: `0x006B78E4/0x006B78E8/0x006B78EC`;
    - retail game-resolution mirrors: `0x00568154/0x00568158`;
    - retail display-mode context:
      - count `0x006B5998`;
      - surfaces `0x006B599C`;
      - flags `0x006B789C`.
  - startup wrapper restores saved resolution into the actual live render globals before retail DX initialization.
  - imports modern 32-bit Windows display modes and explicitly guarantees a `2560x1440x32` entry.
- `562b5a27ba7f21a740a431ea0f73c55a99afdea3`
  - mode augmentation is now applied after every retail `initDirectDraw7` call, not only first startup;
  - this is necessary because retail `DXINIT_SetDisplayOptions` rebuilds DirectDraw and would otherwise wipe the augmented modern mode table.
- `778b60ff68a63be4be1736e04cefb365689d937a`
  - presentation diagnostics now distinguish:
    - `saved_res=<w>x<h>x<bpp>`;
    - `live_res=<w>x<h>x<bpp>`;
  - scene-surface dimensions remain independently logged.

Static/reverse-engineering checks completed:
- uploaded crash DLL return `0x100236FC` maps to `Font::height + 0xC`;
- uploaded secondary return `0x1002CC0F` maps to `Mess_TextHeight + 0xF`;
- retail `PCMOVIE_NextFrame` contains one direct flip call:
  `0x0050B71A -> 0x00502990`;
- retail `DXINIT_SetDisplayOptions` contains a direct reinit call:
  `0x005006B0 -> initDirectDraw7 0x004FEDD0`;
- retail `DXINIT_DirectX8` contains two direct `initDirectDraw7` calls:
  `0x004FDED4` and `0x004FDEFE`;
- all modern-mode entries use the retail-valid flag plus accelerated-resolution flag (`1 | 4`);
- 2560x1440 is an actual render-mode entry, not only presenter scaling.

NEXT TEST:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. verify splash/legal/logo movies are now visible;
4. at start menu, move the mouse around repeatedly and confirm no crash;
5. enter Options -> Display/Video;
6. verify modern resolutions appear, including `2560x1440`;
7. select/apply 2560x1440;
8. return to menu/game and verify the image is still visible;
9. provide all generated logs.

Most important next-log evidence:
- `spidey-decomp-compat.log`
  - `restore_saved_resolution ...`
  - `modern_mode_reinit patched_calls=...`
  - `modern_modes before=... after=... added=... windows_1440=...`
- `spidey-decomp-present.log`
  - `movie_present installed ...`
  - `saved_res=...`
  - `live_res=...`
  - `scene_pre ... width=2560 height=1440 ...` after 1440p is applied.

If 2560x1440 appears in the menu but applying it fails, the next target is the retail `DXINIT_SetDisplayOptions` transition itself; do not regress mode enumeration or fall back to scaled 640x480.


## Runtime result: native 1280x1024 works; borderless collapse + game-heap exhaustion before menu — 2026-09-29

Tested revision:
`07c84285d214aad27470b6460dda1a6fabb0b908`

User-visible result:
- startup begins at the larger monitor/window aspect;
- DirectDraw initialization then collapses the game into the classic smaller legacy-sized box;
- game crashes before reaching the start menu.

Confirmed resolution behavior from logs:
- saved mode = `1280x1024x32`;
- live DX mode = `1280x1024x32`;
- actual scene surface = `1280x1024x32`;
- therefore the native-render-resolution restoration is working;
- current desktop/primary surface = `1920x1080x32`;
- the size/aspect switch is the retail windowed `initDirectDraw7` path resizing the HWND client to the selected render resolution, not the renderer falling back to 640x480.

Modern-mode injection also worked:
- initial retail mode context: 4 entries;
- augmented context: 24 entries;
- Windows enumeration reports 2560x1440;
- explicit 2560x1440 entry installed;
- all three verified retail `initDirectDraw7` call sites are wrapped.

Crash diagnosis:
- exception target: write to `0x0000001F`;
- apparent module name is `binkw32.dll`, but this is the project's proxy DLL loaded at preferred base `0x10000000`, not proof of a RAD Bink decoder fault;
- crash EIP `0x1002C6D1` maps via the uploaded linker map to reconstructed `DCMem_New` at `0x1002C680 + 0x51`;
- stack return `0x10037993` maps into reconstructed `PCTex_CreateTexture256` (`0x10037800..`);
- `DCMem_New` currently does not handle `Mem_CoreNew` returning NULL:
  - NULL base produces alignment offset 32;
  - calculated result becomes `0x20`;
  - writing the alignment byte at `result - 1` writes to exactly `0x1F`, matching the crash.
- conclusion: the fixed game heap is exhausted during texture creation/reload and the reconstructed allocator turns the OOM into an access violation.

Next implementation:
1. add a compatibility fallback allocation path for `DCMem_New` when the original game heap is exhausted, while retaining the exact 32-byte alignment contract;
2. mark fallback blocks with a sentinel heap id and teach delete/shrink/handle paths to recognize them safely;
3. keep the borderless HWND at monitor dimensions after every DirectDraw reinit instead of allowing retail `MoveWindow` to shrink it to the internal render resolution;
4. present the internal scene aspect-fit/centered in that monitor-sized window so legacy 4:3/5:4 modes do not become either a tiny window or a horizontally distorted fullscreen image;
5. retain native 2560x1440 scene support and the movie-specific presenter.

Do not revert native scene-resolution restoration: this test proves it is now functioning.


## Fix batch ready: borderless monitor window + allocator OOM fallback — 2026-09-29

Implementation commits:
- `d0adc9dbb511f071d553843b19fbc1069c6c1765`
  - `DCMem_New` now detects original fixed-heap allocation failure before alignment;
  - allocates an ABI-compatible fallback block from the process C heap;
  - preserves the existing 32-byte aligned returned-pointer contract;
  - marks fallback blocks with signed 4-bit `ParentHeap = -1`;
  - `Mem_DeleteX` frees fallback blocks with `free()`;
  - `Mem_ShrinkX` treats fallback blocks safely;
  - `Mem_MakeHandle` recognizes fallback blocks;
  - logs each fallback as `mem_fallback alloc=... requested=... block=...`.
- `bb38a72588bb868aab8a82a829f90e54171b111c`
  - hardens `Mem_NewTop` against a completely empty free list;
  - returns NULL instead of dereferencing a null free-block pointer, allowing the DCMem fallback to engage.
- `5374ca47c1b35d571d5fc1bac72f845b650fdae2`
  - keeps the game HWND borderless and monitor-sized after every wrapped `initDirectDraw7` and after full `DXINIT_DirectX8`;
  - stops retail windowed DirectDraw initialization from shrinking the desktop-sized window to the selected internal render resolution;
  - presenter now aspect-fits the internal scene into the monitor-sized client area;
  - legacy 4:3/5:4 modes are centered with black bars instead of becoming a small window or being horizontally stretched;
  - matching-aspect modern modes (for example 2560x1440 on a 16:9 target) fill the window naturally;
  - presentation log now records `present=x,y,WxH` and `aspect_fit=`.
- `6a94e9c3bb337e2c9e30bb329410b48931a24c57`
  - replaces `unsigned long long` aspect math with 32-bit `unsigned long` products because the project explicitly supports pre-MSVC-1300 toolchains;
  - all supported resolution products are safely below 32-bit overflow.

Static checks:
- current crash `0x1002C6D1` is `DCMem_New + 0x51` in the proxy linker map;
- caller return `0x10037993` is inside `PCTex_CreateTexture256`;
- write target `0x1F` exactly matches NULL base + 32-byte alignment math;
- native scene rendering was proven at 1280x1024 before this fix;
- 2560x1440 remains in the augmented mode table;
- movie-specific presentation remains enabled.

Build validation note:
- connector-side source checks passed;
- this environment cannot network-clone the private/current repo into the local compiler container, so no independent local binary compile was possible here;
- changes were kept compatible with the project's legacy MSVC constraints visible in `my_types.h`.

NEXT TEST:
1. `UPDATE_SPIDEY_PROJECT.bat`
2. `TEST_LATEST_BUILD.bat`
3. Observe startup window:
   - it should remain monitor-sized instead of collapsing into the legacy small box;
   - a 1280x1024 internal mode on a 16:9 desktop should be centered/aspect-fit with bars.
4. Let all splash/movie screens run through.
5. Confirm whether the game reaches the start menu without crashing.
6. Move the mouse repeatedly at the start menu.
7. Open display options and check whether 2560x1440 remains listed.
8. If possible select/apply 2560x1440 and report the visual result.
9. Upload all generated logs.

Most useful new evidence:
- `spidey-decomp-compat.log`
  - `borderless_monitor_window ...`
  - any `mem_fallback ... requested=...` lines;
- `spidey-decomp-present.log`
  - `dst=<monitor size>`;
  - `present=x,y,WxH aspect_fit=1` for legacy aspect modes;
  - `aspect_fit=0` for matching 16:9 modes;
- crash log if any.

Do not revert the native-resolution or modern-mode work unless a later test demonstrates a renderer-level incompatibility; this test already proved the real scene can render at 1280x1024.


## Build-only regression fixed: legacy SDK monitor API incompatibility — 2026-09-29

Tested revision:
`ed7db84793b6c03867baf0ab475919ea7cd120dc`

Result:
- runtime test did not start because the matching MSVC 6-era toolchain failed compiling `main.cpp`;
- allocator changes compiled;
- failure was isolated to the new borderless-window helper using APIs absent from the project's old Windows headers:
  - `MonitorFromWindow`;
  - `MONITOR_DEFAULTTONEAREST`;
  - `MONITORINFO`;
  - `GetMonitorInfoA`.

Build log errors begin at `main.cpp(927)` and cascade from those missing declarations.

Fix:
- commit `00e09b932a55f7dea60386b462f1c7f4ced54f36`;
- replaced multi-monitor API usage with old-SDK-safe `GetSystemMetrics(0)` / `GetSystemMetrics(1)`;
- this matches the retail game's existing primary-display sizing approach in `RealWinMain`;
- borderless behavior remains:
  - popup/no caption frame;
  - window forced to primary display origin `0,0`;
  - width/height set to primary desktop dimensions;
- log now reports:
  `borderless_monitor_window ... source=GetSystemMetrics`.

NEXT ACTION:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. first confirm matching build succeeds;
4. only if it launches, continue the existing runtime checks for borderless sizing, splash playback, allocator fallback, menu reachability, mouse movement, and 2560x1440.


## Workflow convenience: one-click update + build/test BAT — 2026-09-29

Added:
- `UPDATE_AND_TEST_LATEST_BUILD.bat`
- commit `d31834725336edbb629948d48a9b3a2baec988a8`

Behavior:
1. runs `UPDATE_SPIDEY_PROJECT.bat`;
2. stops immediately if update fails;
3. re-enters the project directory;
4. runs `TEST_LATEST_BUILD.bat` (forced clean matching build, install, launch, log capture);
5. propagates failure exit codes.

This is now the preferred normal test workflow after the file has been pulled once:
`UPDATE_AND_TEST_LATEST_BUILD.bat`

For the first use on a checkout that predates this file, run `UPDATE_SPIDEY_PROJECT.bat` once to obtain it.


## Runtime result: movies visible, borderless correct, crash moved into PCTex_CreateTexture256 — 2026-09-29

Tested revision:
`fde5f00bd2306d5a877c8ba5b3c15d1196942923`

User-visible:
- startup/splash movies are now visible;
- movies could not be skipped with input;
- game crashes during transition into the start menu.

Confirmed presentation:
- primary desktop/window target = 2560x1440;
- saved/live internal render = 1280x1024x32;
- presenter aspect-fits that 5:4 scene to 1800x1440 at x=380;
- borderless HWND remains 2560x1440;
- movie-specific presenter is installed and visibly working.

New crash:
- EIP = `0x10037CB1`;
- access = write to NULL (`0x00000000`);
- current linker map places `PCTex_CreateTexture256` at `0x10037AF0`;
- fault offset inside that function = `+0x1C1`;
- registers at failure include EDI=0 and 64x64-looking dimension values (EBX/EBP=0x40);
- this is no longer the previous `DCMem_New + 0x51 -> write 0x1F` crash;
- compat log contains no `mem_fallback` entry before failure.

Retail/reference disassembly note:
- repository retail blob `tools/functions/5300640.bin` is the original `PCTex_CreateTexture256` body;
- current reconstructed function remains tagged `@AlmostMatching`;
- the early body allocates a temporary 16-bit conversion buffer, optionally clears it, resolves a palette, then converts indexed source bytes into that buffer before D3D texture creation.
- next patch should guard and independently backstop this transient conversion buffer, then log exact call arguments/stage.

Movie skip:
- `GameFMV_PlayMovie` calls `Pad_Update()` every movie frame and only checks skip triggers after 60 frames;
- borderless helper currently uses `SWP_NOACTIVATE`, which can leave the launch console as the active window and DirectInput foreground devices unable to report movie-skip input;
- `PCINPUT_GetMappedStates` also does not initialize its output masks before polling, so failed/unfocused polls can leave undefined values.

NEXT PATCH:
1. remove `SWP_NOACTIVATE` from borderless resize so the game is activated when brought to the top;
2. initialize mapped-state masks to zero in `Pad_Update`;
3. harden `PCTex_CreateTexture256` transient conversion-buffer allocation:
   - retain original DCMem path first;
   - if it still returns NULL, use a process-heap temporary buffer;
   - never continue conversion with a null destination;
   - free with the matching allocator;
   - log entry dimensions/source/palette/buffer ownership and failure stage;
4. add guards for null source/palette and failed PVR creation before indexing the global texture table.


## Fix batch ready: movie input activation + guarded CreateTexture256 — 2026-09-29

Implementation:
- `0d02ad0bd067093de2e9453fb70fa59a604e28bc`
  - removed `SWP_NOACTIVATE` from the borderless `SetWindowPos` call;
  - bringing the HWND to `HWND_TOP` can now activate it, which is required by foreground DirectInput devices used by movie skipping.
- `c27acf0ce7735ecbd1da63b2d9d75ed7b67c6108`
  - initializes `Pad_Update` mapped-state masks to zero before `PCINPUT_GetMappedStates`;
  - prevents failed/unfocused polls from leaving undefined stack input state.
- `fe6711fe9d4cfece53463707cb479db5055f77f7`
  - first texture hardening draft; superseded immediately by scoped correction below.
- `da9cfd83e6eeead93fd93a20b3611839b3eed714`
  - cleanly reapplies texture hardening only to `PCTex_CreateTexture256`;
  - restores `PCTex_CreateTexture16` exactly to its pre-diagnostic state after catching an over-broad edit during static review;
  - logs each CreateTexture256 call and stage to `spidey-decomp-texture.log`;
  - retains DCMem as the first allocation path;
  - if DCMem still returns NULL, uses a temporary process-heap conversion buffer;
  - never enters indexed-color conversion with a null destination;
  - guards null source and null palette;
  - returns cleanly if PVR creation/recreation fails instead of continuing with an invalid texture handle;
  - frees the temporary conversion buffer with its matching allocator.
- `141a4e479265848dd2bd8f881f0e5c484f57a8f5`
  - `TEST_LATEST_BUILD.ps1` now clears/captures `spidey-decomp-texture.log` into the normal timestamped test-session folder automatically.

Static verification after correction:
- `compatConversionBuffer` and `textureCall` now occur only inside `PCTex_CreateTexture256`;
- `PCTex_CreateTexture16` no longer contains any of the Create256 diagnostics;
- borderless presenter and 2560x1440 mode injection remain unchanged;
- movie presenter remains installed.

NEXT TEST:
- use the standard one-click `UPDATE_AND_TEST_LATEST_BUILD.bat`;
- check whether a key/button can skip a splash after the 60-frame skip delay;
- if not skipped, let movies finish;
- confirm whether the game reaches the start menu;
- move the mouse at the start menu if it reaches it;
- upload the whole new test-session output, including the automatically captured `spidey-decomp-texture.log`.


## Runtime result: menu reached; frontend textures/caps wrong; Alt+Tab loses input — 2026-09-29

Tested revision:
`8783b22198653ca0b310f55e1ed4f84efeef2916`

User-visible:
- game now boots through the splash movies and reaches the main menu;
- main menu renders with a white/missing background and visibly broken composition;
- frontend/start menu is unstable visually;
- Alt+Tab out and back causes controls to stop responding.

Confirmed presentation:
- borderless HWND stays 2560x1440;
- startup scene is 1280x1024x32 and aspect-fit to 1800x1440;
- at frontend takeover (present frame ~195), retail live mode changes to 640x480x16 while the actual offscreen scene is 640x480x32;
- presenter correctly keeps the 2560x1440 window and aspect-fits the 640x480 scene to 1920x1440 at x=320.

Texture diagnostics:
- CreateTexture256 calls 1-25 create normally.
- During frontend texture reload, ordinary 64x64 / 128x128 / 512x512 assets begin calculating impossible conversion buffer sizes:
  - 64x64 -> -679215104 bytes;
  - 128x128 -> 1578106880 bytes;
  - 512x512 -> -520093696 bytes.
- those conversions are rejected by the new guard rather than crashing, which explains why the game now survives but menu art is missing.

Verified root cause:
- retail initDirect3D7 function blob `tools/functions/5235120.bin` contains:
  - load device from `0x006B791C`;
  - push `0x006B5780`;
  - call the device vtable GetCaps method.
- therefore the real retail `D3DDEVICEDESC7` base is `0x006B5780`.
- reconstructed `PCTex.cpp` currently defines `G_D3DDEV_CAPS` at `0x006B5788`, eight bytes too far into the structure.
- `PCTex_UpdateForSoftwareRenderer` copies `dwMaxTextureWidth`, `dwMaxTextureHeight`, and `dwMaxTextureAspectRatio` from this misbased struct during frontend renderer reload.
- bad high-bit cap values make the signed aspect-ratio comparison succeed and then multiply normal texture dimensions by garbage, producing the huge/negative conversion sizes above.

Input diagnostics:
- `DXINPUT_PollKeyboard` only reacquires on `DIERR_INPUTLOST`; after Alt+Tab DirectInput may instead return `DIERR_NOTACQUIRED`, leaving keyboard input permanently unacquired.
- `DXINPUT_PollMouse` is still a MEDIUMTODO stub returning a magic nonzero value without writing either output delta; `PCINPUT_UpdateMouse` then consumes uninitialized deltas. This can directly explain frontend mouse/control instability.

Next implementation:
1. correct `G_D3DDEV_CAPS` from `0x006B5788` to verified retail `0x006B5780`;
2. add a narrow caps sanity log around frontend texture reload;
3. make keyboard polling reacquire on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
4. implement buffered DirectInput mouse polling with the same press/held/release state semantics as keyboard;
5. instrument and preserve saved resolution when the retail frontend issues its hardcoded 640x480x16 display reset, without blocking genuine non-640x480 display-option changes.


## Recovered after input-stream interruption: frontend corruption root cause proven — 2026-09-29

Recovered from the interrupted investigation and re-verified against live `dev`:
- current HEAD before recovery: `8783b22198653ca0b310f55e1ed4f84efeef2916`;
- prior movie/input/texture hardening commits are present;
- the newly discovered D3D caps correction had NOT yet been committed when the stream failed.

Last verified runtime result:
- game now reaches and runs the main menu;
- splash movies are visible;
- frontend presentation remains borderless at 2560x1440;
- at the movie -> frontend transition, live internal mode changes from saved 1280x1024x32 to 640x480x16;
- menu screenshot shows white/missing background composition and broken frontend rendering;
- Alt+Tab out/in causes controls to stop responding.

Texture evidence:
- the same ordinary 64x64 and 128x128 textures create successfully before the frontend renderer reinit;
- after the renderer reinit, those calls begin calculating impossible conversion sizes such as -679215104 and 1578106880 bytes;
- failed texture creation explains the missing/white frontend art rather than bad retail assets.

Retail disassembly breakthrough:
- original retail `initDirect3D7` performs `IDirect3DDevice7::GetCaps` using destination address `0x006B5780`;
- reconstructed `PCTex.cpp` currently defines `G_D3DDEV_CAPS` at `0x006B5788`;
- this is an 8-byte offset error;
- therefore PCTex reads shifted/wrong `D3DDEVICEDESC7` fields after renderer reinit, including `dwMaxTextureWidth`, `dwMaxTextureHeight`, `dwMaxTextureAspectRatio`, and texture-cap flags;
- this directly explains the impossible rounded texture dimensions/conversion byte counts after frontend reinit.

Immediate next actions:
1. correct `G_D3DDEV_CAPS` to retail-proven `0x006B5780`;
2. retain diagnostic logging for one test to prove post-reinit texture sizes normalize;
3. instrument/redirect direct callers of retail `DXINIT_SetDisplayOptions(0x00500250)` so the exact source/arguments of the 640x480x16 frontend switch are known before changing semantics;
4. inspect DirectInput foreground-device poll/reacquire behavior on focus loss and restore.


## Post-interruption recovery audit: more source work survived — 2026-09-29

Live `dev` audit after the user's pasted interruption transcript:
- current HEAD at audit: `8675a76b7109c18e77f0d56295f060fefc2ddb26`;
- no important source progress was lost;
- two implementation commits survived beyond what was visible in the interrupted transcript:
  - `99bcb6fc62be5398ea177c986975d408d2e390c0` — fixes `G_D3DDEV_CAPS` from `0x006B5788` to retail-proven `0x006B5780` and logs post-reinit caps;
  - `8675a76b7109c18e77f0d56295f060fefc2ddb26` — DirectInput focus recovery and buffered mouse polling.

Verified input work in `8675a76b...`:
- keyboard reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
- keyboard state is cleared before reacquire to avoid stuck transitions;
- mouse polling is no longer the old magic-value stub;
- mouse now uses buffered `GetDeviceData`, zeroes deltas, reacquires on focus restoration, and tracks button press/held/release transitions.

True remaining frontier:
1. test the D3D-cap fix against the broken white frontend art;
2. identify the exact caller/arguments responsible for the movie->frontend `640x480x16` reset;
3. preserve the saved/native render resolution for that automatic frontend reset while still allowing genuine user-selected resolution changes;
4. runtime-test Alt+Tab input recovery from `8675a76b...`.


## Current true frontier after recovery audit — 2026-09-29

Additional surviving commits discovered during live branch audit:
- `d88c160afe1474645a5d69ce0f89e76d578b1738`
  - adds `SpideyCompatSetDisplayOptions`;
  - intercepts the retail frontend's exact `640x480x16` compatibility reset only when the windowed compatibility path is active;
  - substitutes the validated saved width/height/bpp for that legacy reset;
  - leaves non-640x480 display-option requests unchanged;
  - logs requested vs applied mode and whether the saved mode was preserved;
  - re-injects modern modes and restores the borderless desktop-sized window after the retail mode change.
- `88b5d1775c04d3386ef99336e7f54402579207bb`
  - installs the display-options compatibility wrapper from `game_patches()`.

Static verification at HEAD after recovery:
- D3D caps base is retail-proven `0x006B5780`;
- caps reload diagnostic is present;
- keyboard reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
- mouse now has buffered DirectInput polling and reacquire logic;
- frontend 640x480x16 reset wrapper exists and is installed;
- only exact legacy 640x480x16 requests are substituted, so normal user resolution choices remain available;
- modern mode reinjection and borderless restore still run after the retail display-options call;
- texture diagnostic capture remains enabled.

No further source change is needed before the next runtime test. The next test is specifically intended to validate all three recovered fixes together:
1. white/missing frontend art should be corrected by the caps-base fix;
2. movie->frontend should remain at saved 1280x1024x32 rather than dropping to 640x480x16;
3. Alt+Tab out/in should reacquire keyboard and mouse input.

Expected diagnostic evidence:
- `spidey-decomp-texture.log`: sane `caps_reload` values and no absurd negative/GB-scale conversion sizes for ordinary 64x64/128x128 assets;
- `spidey-decomp-compat.log`: `display_options request=640x480x16 apply=1280x1024x32 ... preserve_saved=1`;
- `spidey-decomp-present.log`: live scene remains at saved resolution through frontend takeover instead of switching to 640x480x16.


## Fix batch ready: frontend textures + saved mode + Alt-Tab input — 2026-09-29

Implementation commits:
- `99bcb6fc62be5398ea177c986975d408d2e390c0`
  - corrected retail D3D caps base in `PCTex.cpp` from incorrect `0x006B5788` to verified `0x006B5780`;
  - verification source is untouched retail `initDirect3D7` blob `tools/functions/5235120.bin`: device GetCaps is called with `0x006B5780`;
  - added `caps_reload` diagnostics with max texture width/height/aspect/caps.
- `8675a76b7109c18e77f0d56295f060fefc2ddb26`
  - keyboard polling now reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
  - clears stale key state before reacquiring;
  - replaced `DXINPUT_PollMouse` MEDIUMTODO magic-value stub with real buffered DirectInput mouse polling;
  - mouse polling now reacquires after focus loss and emits the existing 0xFF/new, 0x7F/held, 0x80/released state semantics.
- `d88c160afe1474645a5d69ce0f89e76d578b1738`
  - added same-signature wrapper for retail `DXINIT_SetDisplayOptions` at `0x00500250`;
  - logs requested/applied display modes;
  - only intercepts the legacy windowed frontend reset `640x480x16`;
  - when saved render settings are valid and non-640x480, applies saved width/height/bpp instead;
  - leaves genuine non-640x480 option changes untouched.
- `88b5d1775c04d3386ef99336e7f54402579207bb`
  - installs the display-options wrapper during game patch startup.
- `a4abe97e848a223f5dd85a4dd724a10a92f5506e`
  - corrected retail texture checksum hash table resolution;
  - untouched retail `Spool_FindTextureEntry` blob `tools/functions/5018720.bin` disassembles to:
    - `and eax, 0x1FF`
    - `mov eax, [eax*4 + 0x006AB934]`
  - therefore verified hash table base is `0x006AB934`;
  - removed the old inferred `0x006B70F8` runtime check, which could never pass because `PATCH_PUSH_RET` had already overwritten retail entry `0x004C9460`.
- `472898d0d110d59aaecd621b9677983c249ca6cc`
  - `WM_ACTIVATE` now checks `LOWORD(wParam)` rather than the entire WPARAM, so the minimized flag cannot make an inactive window appear active.

Why the white frontend occurred:
- after frontend renderer reset, the misbased D3D caps struct made `dwMaxTextureAspectRatio` read unrelated high-bit data;
- CreateTexture256 then multiplied normal dimensions by that bogus cap:
  - 64x64 paths attempted -679215104-byte conversions;
  - 128x128 paths attempted 1578106880-byte conversions;
  - 512x512 paths attempted -520093696-byte conversions;
- the new allocation guard prevented the old crash but correctly skipped those impossible textures, exposing missing/white menu art;
- separately, the texture checksum resolver was returning the default texture for repeated misses because it used the wrong hash table base.

Static verification:
- no remaining `0x006B5788` D3D-cap macro in current PCTex.cpp;
- CreateTexture256 diagnostics remain scoped only to CreateTexture256;
- DXINPUT_PollMouse no longer contains the stub printf/magic return;
- keyboard and mouse both handle `DIERR_NOTACQUIRED`;
- display-options wrapper is installed in `game_patches`;
- retail texture table resolver now uses `0x006AB934` with SEH-protected traversal;
- GitHub Actions currently reports no workflow runs for this branch, so matching-MSVC compile validation still occurs through the user's one-click BAT.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. let or skip splash movies;
3. verify main-menu background/art is restored;
4. verify the frontend does not switch the internal live render mode back to 640x480x16;
5. move/click the mouse and navigate menus;
6. Alt+Tab out and back, then verify keyboard and mouse controls recover;
7. if stable, open Display Options and check/select 2560x1440;
8. upload the full session including `spidey-decomp-texture.log`.

Expected useful log changes:
- `caps_reload base=0x006B5780 ...` with sane max texture values;
- formerly failing 64/128/512 CreateTexture256 calls should reach `stage=done` rather than `conversion_alloc_failed`;
- `texture_hash_table retail_blob_verified base=0x006AB934`;
- repeated checksum misses should drop sharply or disappear;
- `display_options_compat patched_calls=...`;
- legacy reset should log `request=640x480x16 apply=<saved mode> preserve_saved=1`;
- present log should remain at the saved live render resolution through frontend takeover.


## Runtime result: texture/resolution fixes work; frontend flicker + focus input remain — 2026-09-29

Tested revision:
`ba5cff2c4e5a6271900923534eb13f7771d7162a`

User-visible:
- game boots through startup and reaches the start/main menus;
- the proper main-menu background is restored;
- unrelated-looking building/city images rapidly appear/disappear over both the start and main menus;
- Alt+Tab out and back still causes controls to stop responding.

Confirmed fixes from logs:
- display-options wrapper installed across 4 retail call sites;
- frontend request `640x480x16 option4=0 option5=4` was intercepted and changed to `1280x1024x32` with `preserve_saved=1`;
- live scene remains 1280x1024x32 through frontend presentation;
- borderless target remains 2560x1440;
- texture hash table resolves to retail-proven `0x006AB934`;
- all previously failing CreateTexture256 assets now create at sane sizes:
  - 64x64 -> 64x64;
  - 128x128 -> 128x128;
  - 512x512 -> 512x512;
- no absurd negative/GB-scale conversion sizes remain.

Interpretation:
- white/missing frontend art was fixed by the D3D caps correction;
- the new flicker is not an allocation/texture-creation failure;
- strongest current regression candidate is forcing the retail frontend's intentional 640x480x16 internal canvas to 1280x1024x32;
- prior test at the retail 640x480 frontend mode did not report these building/city flashes, while this artifact appeared immediately after saved-mode preservation was introduced.

Next implementation:
1. stop substituting the exact frontend `640x480x16 option4=0 option5=4` request;
2. keep the outer HWND borderless/desktop-sized and continue aspect-fit presentation, so the window will not shrink;
3. retain the fixed D3D caps address and texture hash table;
4. later handle native frontend/widescreen as a separate UI/rendering project instead of forcing legacy frontend assumptions into a larger canvas;
5. add explicit DirectInput focus-transition handling:
   - unacquire/clear on deactivation;
   - reacquire keyboard/mouse/controller on activation;
   - log WM_ACTIVATE state plus Acquire/GetDeviceData results to a dedicated input log;
6. capture that input log in the one-click test workflow.


## Fix batch ready: legacy frontend canvas + explicit focus reacquire — 2026-09-29

Based on runtime revision:
`ba5cff2c4e5a6271900923534eb13f7771d7162a`

Implementation:
- `9b90226fdd517136a80170ef56a468285eb44973`
  - stops replacing the frontend's exact `640x480x16 option4=0 option5=4` request with the saved gameplay resolution;
  - keeps the retail frontend's intended internal canvas;
  - retains the borderless desktop-sized HWND and aspect-fit presenter;
  - keeps modern resolution enumeration, D3D caps correction, texture hash fix, and startup saved-resolution restore;
  - logs `frontend_legacy=1` for this exact automatic frontend request.
- `61d0ab1813d04baf8a2f71bd7de6f0beaa8aa53b`
  - adds explicit DirectInput application activation handling;
  - clears keyboard/mouse/controller transition state on every focus transition;
  - unacquires foreground devices when the app deactivates;
  - explicitly reacquires keyboard, mouse, and controller when the app becomes active;
  - logs activation state, Acquire HRESULTs, foreground/active/focus HWNDs;
  - adds poll-path reacquire diagnostics for keyboard and mouse.
- `a4d653a123412d43454ee7a3d132417b5fdf086b`
  - exposes `DXINPUT_HandleActivation`.
- `3cd51d4d81d85ff04b06533f99d11b53f7f294a3`
  - handles `WM_ACTIVATEAPP` in `SpideyWndProc` and routes app focus transitions to DirectInput.
- `e502f3b177a03a06864127350c8b9c4a3f561774`
  - one-click test workflow now clears/captures `spidey-decomp-input.log`.

Reason for frontend-canvas change:
- the prior white/missing background problem is conclusively fixed: all formerly failing 64x64, 128x128, and 512x512 CreateTexture256 calls now complete normally;
- the flickering unrelated building/city imagery first appeared in the build that forced the retail frontend canvas from 640x480x16 to 1280x1024x32;
- the renderer already clears the scene each BeginScene, so this is not simply uncleared desktop memory;
- allowing the legacy frontend canvas while scaling only at presentation is the narrowest regression test.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. verify startup movies still render;
3. check start menu and main menu for the rapidly flashing building/city imagery;
4. the inner frontend should now be 640x480, but the outer window must remain borderless 2560x1440 and aspect-fit;
5. Alt+Tab out, wait briefly, Alt+Tab back;
6. test keyboard and mouse input after return;
7. upload full session, especially:
   - `spidey-decomp-compat.log`;
   - `spidey-decomp-present.log`;
   - `spidey-decomp-texture.log`;
   - NEW `spidey-decomp-input.log`.

Expected diagnostics:
- compat: `display_options request=640x480x16 apply=640x480x16 ... frontend_legacy=1`;
- present: frontend scene returns to 640x480 while outer destination remains 2560x1440;
- input: deactivate/activate pairs plus explicit DirectInput Acquire results.


## Runtime result: legacy frontend canvas restored; visual flicker + Alt+Tab input still remain — 2026-09-29

Tested revision:
`42fe9d5b3eda8cd376eb585f986a4bb5b9638f9f`

User-visible:
- start/main menu behavior is improved compared with the forced-1280 frontend build;
- proper menu/background assets remain present;
- rapid transient images (described as building/city imagery) still flash in and out over both start and main menus;
- Alt+Tab out/back still kills controls completely.

Confirmed from logs:
- exact retail frontend request now passes through unchanged:
  `640x480x16 option4=0 option5=4 preserve_saved=0 frontend_legacy=1`;
- borderless HWND remains 2560x1440;
- presenter sees a real 640x480x32 scene and aspect-fits it to 1920x1440 at x=320;
- corrected D3D caps remain sane:
  `max_w=16384 max_h=16384 max_aspect=16384 tex_caps=0x00000CCD`;
- all CreateTexture256 calls continue to complete at sane sizes, including 512x512 and 512x240 frontend assets;
- therefore the remaining flashes are not the prior bad-caps/failed-texture bug.

New DirectDraw teardown clue:
- dxerror log reports `D3D error=0x00000004` at retail call site `0x004FDDCF`;
- retail disassembly proves this site is not a D3D draw failure:
  - it loads movie DirectDraw object `0x006B7900`;
  - calls COM vtable +8 = `Release()`;
  - return value 4 is the remaining COM reference count;
- retail movie NextFrame uses movie surface `0x00AC0A3C`;
- retail PCMOVIE_Stop closes Bink/file state but does not release `0x00AC0A3C`;
- multiple startup movies can therefore leave movie surfaces alive across the frontend DirectDraw rebuild.
- this is now the strongest renderer-state lead for old/foreign imagery flashing through after startup movies.

Input evidence:
- no `spidey-decomp-input.log` was supplied with this test session, despite the workflow being configured to capture it if created;
- regardless of whether it was omitted manually or never created, WM_ACTIVATEAPP-only recovery did not solve runtime behavior.

Next implementation:
1. release the retail movie surface at `0x00AC0A3C` on final movie frame and on direct PCMOVIE_Stop call paths;
2. log movie-surface Release() refcounts before frontend DirectDraw teardown;
3. make keyboard/mouse focus recovery poll-driven as well as message-driven:
   - compare `GetForegroundWindow()` with the DirectInput HWND;
   - unacquire/clear while background;
   - explicitly Acquire again on foreground transition before GetDeviceData;
   - log transition/result even if WM_ACTIVATEAPP is missed;
4. ensure input log is created at DirectInput initialization so absence itself is diagnostic;
5. retain legacy frontend canvas, D3D caps fix, texture hash fix, borderless presentation, and modern resolution support.


## Fix batch ready: release leaked movie surface + foreground-polled DirectInput recovery — 2026-09-29

Implementation:
- `f84ee7f885ed6f36faf6ec5656d1c131e563c9c9`
  - changes the movie-specific flip hook to a dedicated wrapper;
  - reads the real retail Bink pointer at `0x00AC0BA4`;
  - when the final movie frame has just been presented, releases the real retail movie surface at `0x00AC0A3C` and clears the slot;
  - logs surface size and Release() remaining-ref count;
  - also scans direct retail callers of `PCMOVIE_Stop 0x0050B790` and redirects them through a wrapper that performs the same movie-surface cleanup after the untouched retail stop logic;
  - this covers both normal movie completion and direct stop/skip paths where available.
- `52c9f7f7204412668cbfadefd449da998e125383`
  - adds foreground-window polling as a second DirectInput recovery mechanism;
  - keyboard/mouse polling compares `GetForegroundWindow()` against `gDxInputHwnd` every poll;
  - foreground transitions invoke the existing activation handler even if `WM_ACTIVATEAPP` is missed;
  - polling is suppressed while the game is not foreground;
  - DirectInput initialization now always creates `spidey-decomp-input.log` with HWND/foreground/focus state.
- `12986a56b9365ef1c8ce8a549fd273898e9098dd`
  - old-MSVC compile correction: moves the foreground-sync helper below the `gDxInputHwnd` definition.

Retail evidence behind movie cleanup:
- retail `PCMOVIE_NextFrame 0x0050B5A0` uses movie surface pointer `0x00AC0A3C`;
- the function blits that surface into scene `0x006B7908` and calls `DXPOLY_Flip` at `0x0050B71A`;
- retail `PCMOVIE_Stop 0x0050B790` closes Bink/file state but does not Release the movie surface;
- DirectDraw teardown later calls Release() on movie DirectDraw object `0x006B7900` and reports remaining refcount 4;
- multiple startup movies leaking one surface reference each is consistent with that count and with stale movie/display state surviving into the frontend rebuild.

Static verification:
- movie cleanup references the retail surface/Bink globals, not reconstructed DLL-owned placeholders;
- final-frame release occurs only after the movie frame has been presented;
- direct Stop callers use untouched retail Stop first, then surface cleanup;
- foreground polling compiles against globals declared before use;
- legacy 640x480 frontend canvas remains enabled;
- fixed D3D caps, texture hash, borderless presentation, and modern resolution support remain intact.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. observe all startup movies;
3. check start + main menus for rapidly flashing building/city imagery;
4. Alt+Tab out for a second, then return and test keyboard + mouse;
5. upload the full session.

Most useful evidence:
- `spidey-decomp-present.log`
  - `movie_surface_release reason=final_frame ... remaining_refs=...`;
  - `movie_stop_compat patched_calls=...`;
- `spidey-decomp-dxerror.log`
  - compare the old movie DirectDraw remaining refcount 4 after cleanup;
- `spidey-decomp-input.log`
  - now guaranteed to be created at input initialization;
  - should show foreground deactivate/reactivate and Acquire results.


## Runtime result: movie leak ruled out; retail input path identified; double-present race targeted — 2026-09-30

Tested revision:
`e16babda924aa82eb1910577e848317bc7c51f20`

User-visible:
- rapid building/city imagery still flashes over start and main menus;
- Alt+Tab out/back still removes all menu control.

What this run conclusively ruled out:
- movie-surface leak is NOT the source of menu flashing;
- `spidey-decomp-present.log` shows all four startup movie surfaces released with `remaining_refs=0`;
- flashing remained unchanged after that cleanup.

Input root cause discovered:
- the reconstructed `DXINPUT_*` changes in `DXsound.cpp` were never on the retail host's execution path;
- `game_patches()` did not patch retail `DXINPUT_*` entries/callers;
- this explains why no `spidey-decomp-input.log` was ever created and why several iterations of reconstructed reacquire logic had no runtime effect.

Retail input mapping recovered from untouched retail function blobs:
- `DXINPUT_Initialize = 0x005013D0`;
- `DXINPUT_Release = 0x00501440`;
- `DXINPUT_SetKeyState = 0x00501510`;
- `DXINPUT_SetMouseButtonState = 0x00501530`;
- `DXINPUT_GetKeyName = 0x00501550`;
- `DXINPUT_SetupKeyboard = 0x00501590`;
- `DXINPUT_SetupMouse = 0x00501710`;
- `DXINPUT_SetupController = 0x00501890`;
- `DXINPUT_PollKeyboard = 0x00501B80`;
- `DXINPUT_GetKeyState = 0x00501CB0`;
- `DXINPUT_PollMouse = 0x00501CC0`;
- `DXINPUT_GetMouseButtonState = 0x00501E40`;
- `DXINPUT_PollController = 0x00501E50`;
- `DXINPUT_GetControllerButtonState = 0x00501FB0`;
- `DXINPUT_StartForceFeedbackEffect = 0x005021A0`;
- `DXINPUT_StopForceFeedbackEffect = 0x005021E0`;
- `DXINPUT_GetNumControllerButtons = 0x00502210`.

Retail input globals verified from those same functions:
- DirectInput object `0x006B7A30`;
- input HWND `0x006B7A60`;
- keyboard device `0x006B7A5C`;
- mouse device `0x006B7A64`;
- controller device `0x006B7A2C`;
- keyboard transition state `0x006B792C`;
- mouse-button state `0x006B7A54`;
- controller-button state `0x006B7A34`.

Presentation race diagnosis:
- compatibility HWND is 2560x1440;
- retail DirectDraw primary remains 1920x1080;
- in windowed mode retail `DXPOLY_Flip` first Blts scene -> legacy primary, then `SpideyDiagDXPOLYFlip` immediately GDI-stretches scene -> HWND;
- this means two independent presentation paths paint the same visible window every frame through different-sized targets;
- after texture/caps/movie fixes all succeeded, this double-present path is now the strongest explanation for rapid transient foreign imagery.

Fix commit:
- `92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`
  - windowed compatibility mode no longer calls retail `DXPOLY_Flip`; direct scene -> HWND presentation is the sole windowed presenter;
  - original retail Flip remains untouched for non-windowed mode;
  - present log now records `present_path ... retail_flip=0 direct_hwnd=1`;
  - installs retail-call-site wrappers for `DXINPUT_PollKeyboard 0x00501B80` and `DXINPUT_PollMouse 0x00501CC0`;
  - wrappers operate on the actual retail keyboard/mouse/controller DirectInput objects and state arrays;
  - foreground transition explicitly Unacquires on background and Acquires on return;
  - keyboard failures get one explicit Acquire + retail retry;
  - input log is created by the installer itself, proving the hook installed even before first input poll.

Static verification:
- retail input addresses above were recovered from exact original function blobs, not inferred from reconstructed DLL layout;
- wrappers call untouched retail poll functions by absolute address, so original key/mouse transition semantics remain in charge;
- installer rewrites only direct E8 call sites targeting the two retail poll functions;
- the DLL wrapper calls retail by function pointer, so it cannot be recursively repatched;
- legacy 640x480 frontend canvas, fixed D3D caps, correct texture hash table, modern mode list, movie cleanup, and borderless presentation remain enabled.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. verify splash movies and frontend boot;
3. watch start/main menu for flashing building/city imagery;
4. Alt+Tab out for a second and back;
5. test keyboard, mouse, and controller if available;
6. upload full session.

Key expected logs:
- `spidey-decomp-present.log`: `present_path ... windowed=1 retail_flip=0 direct_hwnd=1`;
- `spidey-decomp-input.log`: installer line with nonzero keyboard/mouse call counts, then `foreground_acquire` / `background_unacquire` transitions around Alt+Tab.


## Runtime result: 2560x1440 saved mode fails D3D7 CreateDevice before splash — 2026-09-30

Tested revision:
`e7f678156efbcca850cca86d94c38d52b50dddec`

Observed:
- clean matching build succeeded;
- game crashed before the first splash/movie frame;
- retail-input compatibility installer DID run:
  - keyboard direct-call sites patched = 2;
  - mouse direct-call sites patched = 1;
- no retail input poll/foreground-transition entry was logged before the crash;
- no presentation frame was reached.

Critical difference from the preceding successful menu run:
- previous successful runtime restored `1440x1080x32`;
- this failed runtime restored saved `2560x1440x32`.

D3D failure:
- retail call site `0x004FEA30` invokes `IDirect3D7::CreateDevice(pGUID, g_pDDS_Scene, &g_D3DDevice7)`;
- it returned `0x88760082 = DDERR_INVALIDOBJECT`;
- the scene DirectDraw surface had already been created, but Direct3D7 rejected it as a valid render target/device surface at this mode;
- source windowed scene creation uses `DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN`.

Secondary crash:
- after CreateDevice failure, retail cleanup calls function `0x00503AF0`;
- at `0x00503AF7` it dereferences global `0x006BBF1C`;
- that global is NULL in this failure state, producing the observed C0000005 read from address 0;
- this cleanup AV is secondary; the root failure is the 2560x1440 CreateDevice rejection.

Interpretation:
- the new retail input hooks did not cause this startup crash;
- the single-presenter path also did not execute before the crash;
- native 2560x1440 is NOT runtime-safe on the current DirectDraw7/D3D7 render-target path and must not remain selectable/persisted as if verified;
- 1440x1080x32 is the latest verified working saved internal mode on the same machine/runtime.

Immediate recovery plan:
1. quarantine exact `2560x1440x32` from the selectable modern-mode list until a valid D3D7 render-target path is implemented;
2. if the persisted saved mode is exactly 2560x1440, recover to the last verified `1440x1080x32` mode before retail DX initialization and update the in-memory saved setting so restart is not bricked;
3. add a guarded wrapper for retail cleanup function `0x00503AF0` so a future DirectX init failure cannot turn into a null-deref crash;
4. retain the new retail input polling hooks and sole windowed direct-HWND presenter for the next test;
5. continue 2560x1440 support as a separate renderer-compatibility task rather than claiming it is already supported.


## Recovery patch ready: quarantine 2560x1440 and guard failed-D3D cleanup — 2026-09-30

Implementation:
- `17a75488bc16a865e8b1b8f787544ea06b1228ae`
  - removes exact 2560x1440 from modern mode enumeration on the current DirectDraw7/D3D7 path;
  - removes the old unconditional explicit 2560x1440 mode injection;
  - logs `explicit_2560x1440=0 quarantined_2560x1440=1`;
  - if persisted saved settings are 2560x1440, startup recovers to the last runtime-verified working `1440x1080x32`;
  - writes that recovered mode back into the in-memory saved-setting fields so subsequent display-option paths do not immediately reapply the broken mode;
  - direct `DXINIT_SetDisplayOptions(2560,1440,...)` requests are also recovered to 1440x1080 until native 1440p render-target support is fixed;
  - installs a direct-call-site wrapper around retail cleanup function `0x00503AF0`;
  - if retail global `0x006BBF1C` is NULL, the cleanup call is skipped and logged instead of dereferencing NULL at `0x00503AF7`;
  - if the object exists, untouched retail cleanup runs normally.

The 2560x1440 mode is quarantined, not abandoned:
- current failure is specifically `IDirect3D7::CreateDevice` rejecting the 2560x1440 windowed scene surface with `DDERR_INVALIDOBJECT`;
- windowed scene creation currently uses `DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN`;
- native 1440p support remains an open renderer-compatibility task, likely requiring a render-target allocation/path change rather than simple mode enumeration.

Retained for the next runtime test:
- retail keyboard/mouse poll call-site wrappers from `92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`;
- single windowed scene->HWND presentation path (retail Flip skipped only in compatibility/windowed mode);
- fixed D3D caps base;
- fixed retail texture hash table;
- legacy 640x480 frontend canvas;
- movie-surface cleanup.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. expected startup compat log:
   - `restore_saved_resolution request=2560x1440x32 apply=1440x1080x32 quarantined_2560x1440=1`;
   - modern mode log shows `explicit_2560x1440=0 quarantined_2560x1440=1`;
3. verify splash movies return;
4. verify whether start/main menu building/city flashes are gone under the single-presenter path;
5. Alt+Tab out/back and test menu keyboard/mouse;
6. upload the full session, especially input/present/compat logs.

If this boots, the test finally isolates the intended two fixes because the unrelated persisted-2560 startup failure is removed.


## Runtime success: Alt+Tab fixed and building flashes gone; DPI/native 1440p + widescreen frontier — 2026-09-30

Tested revision:
`8892e08060938d5a9f0e0ff028fb9dfaa7175f4e`

User-visible success:
- Alt+Tab out/back now preserves menu controls;
- the previous rapidly flashing building/city imagery is gone;
- remaining visual issue is an intermittent whole-screen flash;
- screen is still pillarboxed because current internal render/frontend modes are 4:3;
- 2560x1440 is absent from Display Settings because the preceding crash-recovery patch deliberately quarantined it.

Runtime proof for input fix:
- retail input hook installed with keyboard call sites=2 and mouse call sites=1;
- initial foreground Acquire returned 1 (already acquired / harmless legacy state);
- background transition explicitly Unacquired devices;
- foreground return explicitly Acquired keyboard/mouse/controller and all three returned `0x00000000`;
- user confirmed controls continue working after tabbing back in.
This closes the Alt+Tab input-loss bug.

Runtime proof for single-presenter fix:
- every logged compatibility frame uses `retail_flip=0 direct_hwnd=1`;
- user confirmed the old flashing building/city imagery disappeared.
This closes the old double-present/foreign-primary-content artifact.

Remaining whole-screen flash diagnosis:
- aspect-fit presenter still clears the ENTIRE client to black before every StretchBlt;
- current 1440x1080 -> 2560x1440 presentation uses `present=320,0,1920x1440 aspect_fit=1` every frame;
- frontend 640x480 -> 2560x1440 uses the same pillarboxed 1920x1440 destination;
- a GDI-visible FillRect between frames can therefore expose a full black frame before StretchBlt.

Physical-resolution/DPI breakthrough:
- compatibility HWND/client reports 2560x1440;
- DirectDraw primary still reports 1920x1080;
- exact ratio is 4/3 in both dimensions (2560/1920 and 1440/1080), strongly indicating process DPI virtualization/scaling;
- this explains why a 2560x1440 offscreen scene surface could be created but D3D7 CreateDevice rejected it while DirectDraw considered the primary only 1920x1080.

Widescreen evidence:
- current presenter is correctly preserving source aspect, not stretching:
  - 1440x1080 (4:3) -> 1920x1440 with 320px side bars;
  - frontend 640x480 (4:3) -> 1920x1440 with the same side bars.
- do NOT remove bars by stretching; true widescreen requires a 16:9 internal render target and then validation/correction of camera projection and UI mapping.
- source `PCSHELL_CoordsDCtoPC` maps virtual 512x240 shell coordinates independently to live X/Y resolution, so UI behavior at 16:9 must be checked separately.
- source `M3d_RenderSetup` remains retail (not replaced by patch_ps2m3d), so camera/projection behavior must be runtime-validated once a real 16:9 render target boots before changing FOV math.

Implementation:
- `543b456f90495cdb8123b5437d8c1e039f832bde`
  - dynamically resolves `SetProcessDPIAware` / `IsProcessDPIAware` from already-loaded user32 using old-SDK-safe GetProcAddress;
  - enables process DPI awareness during DLL_PROCESS_ATTACH before retail creates its window/DirectDraw objects;
  - logs DPI set result, actual awareness state, and physical screen metrics;
  - re-enables 2560x1440 mode only when process DPI awareness is active;
  - filters DPI-aware enumerated render modes above the physical screen dimensions;
  - startup/set-display only quarantine 2560x1440 if DPI awareness could not be established;
  - removes the full-client black FillRect from aspect-fit presentation and clears only actual side/top/bottom bars;
  - retains the retail-input fix and sole direct-HWND windowed presenter.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. first inspect `spidey-decomp-compat.log` for:
   - `dpi_awareness ... process_aware=1 metrics=2560x1440`;
   - `modern_modes ... native_2560x1440=1 dpi_aware=1`;
3. verify the intermittent whole-screen black flash is gone/reduced;
4. verify Alt+Tab remains fixed;
5. open Display Settings and confirm 2560x1440 has returned;
6. select/apply 2560x1440:
   - if it boots/renders, capture screenshot + full logs so widescreen/FOV/UI behavior can be classified;
   - if D3D7 still rejects it, cleanup guard should prevent the old null-deref and logs will show the remaining renderer limitation cleanly.

Do not implement projection/FOV stretching before this test: first determine whether DPI-aware DirectDraw now exposes a true 2560x1440 primary and whether retail M3d projection naturally handles the 16:9 render target.


## Two-session 1440p comparison: selectable != renderable — 2026-09-30

Compared user ZIPs:
- `initialloadNOTat1440.zip`, session 20260930-021227;
- `second attempt to load at 1440.zip`, session 20260930-021422.
Both tested revision:
`423cbf0ed7f9c6e72bb41890d6c40b1c4779cae4`.

First session (booted below 2560x1440):
- early DPI call reported `process_aware=1 metrics=2560x1440`;
- saved startup mode was `1920x1440x32`;
- modern mode injection exposed native `2560x1440`;
- game booted successfully;
- full-screen black flashing was gone after bar-only clear change;
- sole direct-HWND presenter remained active;
- DirectDraw primary STILL reported 1920x1080 even though HWND/client and Win32 metrics were 2560x1440;
- frontend stayed internally 640x480 after startup;
- selecting/scrolling to 2560x1440 only changed the saved-resolution fields (`saved_res=2560x1440x32`) while `live_res` remained 640x480;
- therefore this session did NOT prove a live 2560x1440 D3D render target.

Second session (boot with persisted 2560x1440):
- early DPI call again reported `process_aware=1 metrics=2560x1440`;
- startup restored `2560x1440x32` live;
- mode list exposed 2560x1440;
- before any splash/present frame, retail `IDirect3D7::CreateDevice` failed with `0x88760082 = DDERR_INVALIDOBJECT` at call site `0x004FEA4A`;
- cleanup guard prevented the old secondary null-deref and logged `cleanup_503AF0 skipped null_global=0x006BBF1C`;
- retail input hook installed but no poll occurred before D3D failure.

Conclusion:
- DPI awareness is useful for physical Win32 metrics and mode enumeration, but it does not by itself make DirectDraw expose a 2560x1440 primary; DirectDraw still reports 1920x1080;
- 2560x1440 has never actually rendered on the current D3D7 device path;
- the renderer must be made tolerant of a render target larger than the legacy DirectDraw primary/device bootstrap target.

Next experiment:
1. move DPI-awareness setup to the very first DLL_PROCESS_ATTACH work and request per-monitor-v2 dynamically before AllocConsole/other UI work;
2. retain 2560x1440 in mode selection;
3. wrap retail `initDirect3D7 0x004FE1B0` only for the 2560x1440 path;
4. hook IDirect3D7::CreateDevice during that call:
   - first try untouched CreateDevice on the real 2560x1440 scene;
   - on DDERR_INVALIDOBJECT, create a known-good 1920x1080 video-memory 3D bootstrap surface;
   - copy/create a matching Z-buffer attachment when possible;
   - create the retail-selected D3D device on the bootstrap;
   - try `SetRenderTarget(real_2560x1440_scene)`;
   - if SetRenderTarget succeeds, continue retail init at true 2560x1440;
   - if it fails, switch the retail scene/global live resolution to the 1920x1080 bootstrap so startup remains safe instead of exiting/crashing.
5. log every HRESULT/caps transition so the next test distinguishes true 1440p from safe 1080p fallback.

Widescreen note:
- menus remaining 640x480/4:3 are separate from gameplay render resolution;
- do not stretch the 4:3 frontend;
- once a live 16:9 gameplay target is working, validate retail projection/FOV and then patch UI safe-area mapping independently.


## DX11 migration started — Phase 0 bridge scaffold — 2026-09-30

Decision:
- stop investing heavily in making DirectDraw7/Direct3D7 the long-term modern renderer;
- migrate to Direct3D 11 incrementally;
- keep the current D3D7 path as a temporary reference/fallback until DX11 reaches parity;
- do NOT pursue DX12 for this project: it adds explicit synchronization/descriptor/command-list complexity without meaningful benefit for Spider-Man 2000.

Architecture:
- legacy retail-compatible proxy stays `binkw32.dll` / rebuilt `spider.dll`, compiled by the preserved VC6-era matching toolchain;
- new modern x86 renderer is `spidey_renderer11.dll`, compiled with VS 2022 / current Windows SDK;
- the two communicate through a versioned C ABI resolved dynamically with `LoadLibraryA` / `GetProcAddress`;
- modern D3D11/DXGI headers never enter the matching proxy build.

Implemented Phase 0 files:
- `renderer11/include/spidey_renderer11_api.h`
  - ABI version 1;
  - GetAbiVersion / GetBackendName / Probe;
  - Initialize / Resize / BeginFrame / Present / Shutdown.
- `renderer11/src/spidey_renderer11.cpp`
  - hardware D3D11 device probe;
  - D3D11 device + immediate context;
  - DXGI swap chain;
  - RGBA8 backbuffer RTV;
  - D24S8 depth buffer;
  - viewport setup;
  - resize;
  - clear + present;
  - adapter/feature-level logging to `spidey-renderer11.log`.
- `renderer11/CMakeLists.txt`
  - modern x86 DLL target linked to d3d11 + dxgi.
- `renderer11/spidey_renderer11.def`
  - stable undecorated export names for the x86 C ABI.
- `scripts/build_renderer11.ps1`
  - configures/builds with Visual Studio 17 2022, Win32;
  - copies output to `out/renderer11/spidey_renderer11.dll`.
- `docs/DX11_MIGRATION.md`
  - full staged migration plan.

Legacy bridge:
- proxy loads `spidey_renderer11.dll` during the existing DX initialization wrapper;
- verifies ABI=1;
- calls `SpideyRenderer11_Probe`;
- records backend/probe state in `spidey-decomp-compat.log`;
- does NOT switch visible rendering yet.

One-click workflow:
- still builds the matching proxy first;
- now builds the DX11 helper second;
- installs `spidey_renderer11.dll` next to `SpideyPC.exe`;
- records its SHA-256 in the session metadata;
- clears/captures `spidey-renderer11.log`.

D3D7 safety during migration:
- exact 2560x1440 is again quarantined from the legacy D3D7 mode table regardless of DPI awareness;
- a persisted 2560x1440 D3D7 setting is recovered to the last verified safe 1440x1080x32 mode;
- this avoids another pre-splash D3D7 CreateDevice crash while DX11 is only in probe/scaffold mode;
- 2560x1440 will return through DXGI once DX11 owns rendering/presentation.

Relevant commits:
- `09db45a1bd6cd09ed429078b3bbcaa641573c1ed` — stable C bridge API;
- `ab4c7f5461c8814ab06d233e2199c8d4febc3a72` — CMake target;
- `343de8e6f50b9448444b461f69a08c6e2a0f3f03` — D3D11 device/swap-chain implementation;
- `b1fd7e3d3e93268db95c68dfd7015dbb72be66ce` — modern build script;
- `e754aca6117048555db0fb2bf2a43bfd7bed812a` — legacy probe + D3D7 safety;
- `168d4d3f215dcd8c51e60b47b81f6b691e834ee7` — one-click build/install/log integration;
- `1496e2c29f6e1c35a49be9e93bd55bc3b51decbd` — migration documentation;
- `453146284b3c6bedfe06277da93d45d0fef5c44f` / `b745a89c5170edef9008289044b4b4ad42273e63` — undecorated x86 exports;
- `550f0af1f897201430ac94770bd35b455d774fc6` — CMake-path fallback correction.

Phase 0 next test:
- run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
- this is a plumbing/probe test, NOT a visible DX11-rendering test yet;
- expected:
  - old game still renders through the known-good D3D7 path;
  - Alt+Tab fix remains working;
  - `spidey-decomp-compat.log` contains:
    `renderer11_bridge loaded ... abi=1 expected=1 backend=Direct3D 11 probe=1`;
  - new `spidey-renderer11.log` contains a successful hardware probe and D3D feature level.

After Phase 0 passes:
1. Phase 1: DX11 takes over final presentation while D3D7 still renders the scene;
2. Phase 2: migrate texture ownership;
3. Phase 3: migrate 2D/frontend primitives;
4. Phase 4: emulate D3D7 fixed-function 3D states/shaders and triangle fans;
5. Phase 5: true 16:9 projection/FOV + UI safe-area work;
6. Phase 6: DX11 becomes the default renderer and D3D7 becomes diagnostic/reference only.


## Full new-chat handoff refreshed for DX11 frontier — 2026-09-30

- Rewrote `docs/NEW_CHAT_HANDOFF.md` so it no longer points at the obsolete black-screen/DirectDraw frontier.
- New handoff is centered on the current Direct3D 11 migration.
- Source frontier before handoff refresh: `0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0`.
- Handoff document commit: `2cbc00900e27f3ce42e357fdc7c9576641250385`.
- Exact next action remains the first DX11 Phase 0 plumbing/probe runtime test using `UPDATE_AND_TEST_LATEST_BUILD.bat`.
- The external full handoff ZIP should include:
  - current documentation;
  - DX11 migration/bridge source snapshots;
  - repo/Drive/upstream links;
  - latest 1440p two-session evidence;
  - previous Sep-30 full handoff as historical baseline;
  - disconnect/live-documentation protocol.


## First DX11 Phase 0 test: renderer11 export linker failure — 2026-09-30

User ran `UPDATE_AND_TEST_LATEST_BUILD.bat` against runtime revision
`0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0`.

Observed:
- legacy matching proxy force-cleaned, compiled, and linked successfully;
- CMake configured `renderer11` as VS 2022 Win32/x86 successfully;
- `spidey_renderer11.cpp` compiled successfully;
- final renderer11 DLL link failed before installation/launch;
- all eight exports named by `renderer11/spidey_renderer11.def` were reported unresolved;
- therefore no DX11 runtime probe occurred and this is NOT a game/runtime-rendering failure.

Root cause:
- the .def currently aliases each public export to an explicitly underscore-prefixed x86 C symbol, e.g.
  `SpideyRenderer11_Probe=_SpideyRenderer11_Probe`;
- module-definition export resolution already performs the x86 C-name decoration lookup for an undecorated export entry;
- explicitly supplying the underscore-prefixed alias causes an additional decoration lookup / wrong internal name on the modern linker path;
- a local MSVC-ABI-compatible i686 COFF reproduction with clang-cl + lld-link confirms the behavior:
  explicit `Foo=_Foo` fails looking for `__Foo`, while plain `Foo` resolves the object symbol `_Foo` and links.

ACTIVE FIX:
- change the .def EXPORTS list to plain undecorated public names with no `=_Name` aliases;
- keep the C ABI and `extern "C" __cdecl` source definitions unchanged;
- rerun the same Phase 0 one-click test after this build-only correction.

Do not advance to DX11 Phase 1 until the helper builds, installs, loads, ABI-checks, and probes successfully.


### DX11 export linker fix committed

Fix commit:
`c89c6034a0bfb70cf99b38b1fec7745bd8ddc166` — `renderer11: fix x86 DEF export decoration`

Change:
- `renderer11/spidey_renderer11.def` now lists only the eight undecorated public export names;
- removed explicit `=_Spidey...` aliases;
- source declarations/definitions remain `extern "C" __cdecl` and ABI version remains 1;
- proxy lookup strings in `main.cpp` already request the same undecorated names, so no bridge-side change is required.

Static validation:
- independent i686 MSVC-ABI COFF reproduction links successfully with plain .def names and its PE export table contains exactly the undecorated names;
- the previous explicit underscore alias form reproduces the unresolved-name failure.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. confirm the updater pulls commit `c89c6034a0bfb70cf99b38b1fec7745bd8ddc166` or later;
3. renderer11 should now pass the DLL link stage;
4. if build/install succeeds, let the game launch normally and upload the complete session output;
5. Phase 0 success still requires:
   - `renderer11_bridge loaded ... abi=1 expected=1 backend=Direct3D 11 probe=1`;
   - `spidey-renderer11.log` with successful D3D11 probe / feature level;
   - known-good D3D7 visible rendering and Alt+Tab behavior unchanged.

If a new failure appears, stop at that exact Phase 0 layer and fix it before Phase 1.


## DX11 Phase 0 PASSED — 2026-09-30

Tested revision:
`da13c63f51a25f6492c8d32a8f0165d28648228a`

User result:
- game booted normally through splash/start flow and reached the main menu;
- existing D3D7 visible rendering remained intact;
- expected 4:3 side bars remain;
- 2560x1440 remains intentionally absent because the legacy D3D7 2560x1440 mode is still quarantined.

Runtime proof:
- renderer11 bridge loaded successfully;
- ABI matched;
- backend reported `Direct3D 11`;
- hardware probe returned success;
- renderer log reported `hr=0x00000000 feature_level=0xB100` (D3D feature level 11.1);
- saved 2560x1440 was safely recovered to 1440x1080 for the still-active D3D7 path;
- current presenter remains 1440x1080 -> aspect-fit 1920x1440 inside the 2560x1440 client, hence 320-pixel bars on each side.

Phase 0 is CLOSED.

NEXT FRONTIER — PHASE 1:
- make DX11 own final presentation while D3D7 temporarily continues rendering the scene;
- retain a fail-safe fallback to the current direct-HWND GDI presenter;
- do not re-enable legacy D3D7 2560x1440 yet;
- only after DX11 presentation is verified should native 16:9 scene ownership / 2560x1440 rendering advance.


## DX11 Phase 1 implementation ready for runtime test — 2026-09-30

Goal:
DX11 owns the final HWND presentation while the legacy D3D7 renderer continues producing the scene surface.

Implementation commits:
- `3d333706da0788b012892cca6f68f32c242d9d23` — bridge ABI bumped to 2 and HDC presenter entry added;
- `a392d3665f1617e1add6e83491840ea8ac109232` — exported `SpideyRenderer11_PresentHdc`;
- `72bdb8f5051b3bfa6e4f36073e0cc28cf925e9db` — link GDI32;
- `a799860a24cf739c92effc2211897c083fe34bee` — GDI-compatible B8G8R8A8 DX11 swap chain + HDC-to-backbuffer presenter;
- `7eecc345ebd26f964fe2e0413dcd32834831c46e` — legacy proxy routes the windowed presentation path through DX11;
- `4de672aa0b26e73f42d403adea40733d951688a1` — preserve GDI-compatible swap-chain flag across resize;
- `cec2b0723bda00f2b8b41b17bd2b351e525ba7f2` — portable HDC bridge typedef.

Design:
- Phase 1 ABI is now version 2.
- The modern helper creates a GDI-compatible DX11/DXGI swap-chain backbuffer.
- The legacy D3D7 scene remains the render source.
- The proxy obtains the scene surface HDC and passes it to `SpideyRenderer11_PresentHdc`.
- Renderer11 aspect-fits/copies that source into the hidden DX11 backbuffer, then calls DXGI `Present`.
- Because the backbuffer is not visible until DXGI Present, bar clearing/copying cannot expose the old GDI intermediate-frame flicker.
- On DX11 initialize/resize/present failure, Phase 1 is disabled for that session and the known-good direct scene->HWND GDI presenter is used automatically.
- No D3D7 native-2560x1440 change is included here. The 2560x1440 option remains intentionally quarantined until a later scene-rendering phase.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. build must succeed with renderer ABI 2;
3. game should reach the main menu;
4. expected compat log:
   - `renderer11_bridge loaded ... abi=2 expected=2 ... phase1_exports=1`;
   - `renderer11_phase1 initialize ... size=2560x1440 result=1`;
5. expected renderer log:
   - `initialize device_ready ... width=2560 height=1440 ...`;
   - `targets ready width=2560 height=1440`;
   - repeating `present_hdc frame=... src=... dst=2560x1440 ...`;
6. expected present log:
   - `compat_present_dx11 ... result=1`;
   - `present_path ... dx11=1 direct_hwnd=0 compat_result=2`.
7. If DX11 presentation fails, upload logs; expected fallback marker:
   - `renderer11_phase1 present_failed disabling_dx11_present_fallback=gdi_hwnd`;
   - game should still boot using `dx11=0 direct_hwnd=1 compat_result=1`.

Visual expectation for this test:
- image should look broadly the same as the current successful build;
- black side bars are still expected when the source is 4:3;
- 2560x1440 is still intentionally absent from the D3D7 mode list.


## Phase 1 first test stopped before launch: stale renderer11 object cache — 2026-09-30

Tested revision:
`25f2c51a52c6788950aa86fc6b025e9e234a29ee`

Observed:
- matching proxy force-cleaned, compiled, and linked successfully;
- renderer11 CMake configure/generate succeeded;
- renderer11 link then failed with exactly one unresolved export:
  `SpideyRenderer11_PresentHdc`;
- the build output did NOT show `spidey_renderer11.cpp` recompiling before the link;
- therefore the game never launched and this was not a runtime crash.

Root cause:
- the ZIP updater intentionally preserves `out/`;
- the Phase 1 source/API changed, but the preserved CMake/MSBuild tree contained an older renderer11 object file from Phase 0;
- archive extraction/source-refresh timestamps can be older than preserved object timestamps, so MSBuild considered the stale object current;
- the new .def file requested `SpideyRenderer11_PresentHdc`, while the reused old object did not contain it.

Fix:
- commit `422f8dd97e56a2d1a2016642637f3ce57e8a148e` — `renderer11: force clean modern builds after source refresh`;
- `scripts/build_renderer11.ps1` now deletes `out/renderer11/build` before every modern renderer configure/build;
- stale renderer11 DLL/PDB artifacts are also removed before rebuilding;
- this makes every test compile the modern DLL from the exact current source regardless of archive timestamps.

NEXT TEST:
1. rerun `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. expected output now includes:
   - `[..] Removing cached Direct3D 11 build tree...`;
   - a fresh CMake configure;
   - `spidey_renderer11.cpp` compiling;
   - successful renderer11 link;
3. only after that does the actual Phase 1 runtime presentation test begin.


## DX11 Phase 1 PASSED — 2026-09-30

Tested revision:
`a16d29d7fc6c4b60a817ca25ddcd5ef4ef599ad3`

User-visible result:
- game booted successfully;
- reached the main menu;
- started a new game;
- entered live gameplay and ran around for roughly a minute;
- user exited normally after the gameplay test.

Runtime proof:
- renderer bridge loaded with ABI 2 and `phase1_exports=1`;
- DX11 Phase 1 initialized successfully at a 2560x1440 client size;
- renderer11 created the hardware D3D11 device/swap chain and 2560x1440 targets;
- present log shows the visible windowed path consistently using `dx11=1 direct_hwnd=0 compat_result=2`;
- renderer11 `present_hdc` continued through at least frame 5160 with no logged fallback;
- frontend transitions continued to use the legacy 640x480 source while gameplay returned to the 1920x1440 D3D7 scene source;
- legacy DirectDraw primary remains 1920x1080 but is no longer the visible presentation owner;
- 2560x1440 remains intentionally quarantined from the legacy D3D7 render-mode path;
- black side bars therefore remain expected because the active gameplay source is still 1920x1440 (4:3), aspect-fitted to the DX11 2560x1440 swap chain at x=320.

Architecture now:
1. Retail/reconstructed D3D7 still creates and renders the scene.
2. The scene surface HDC is handed across the ABI-2 bridge.
3. `spidey_renderer11.dll` owns the real 2560x1440 DXGI swap chain.
4. DX11 copies/aspect-fits the legacy scene into its backbuffer.
5. DXGI Present is now the sole visible presentation path in compatibility/windowed mode.
6. Direct-HWND GDI presentation remains only as automatic fallback.

Phase 1 is CLOSED.

NEXT FRONTIER:
- remove the transitional GDI/HDC copy from the normal DX11 path and establish DX11-owned frame/texture upload;
- then migrate render resources/2D/fixed-function responsibilities until D3D7 no longer constrains scene dimensions;
- only once DX11 owns the scene render target should 2560x1440 be re-enabled as a true internal render resolution and 16:9 projection/FOV/UI work proceed.

Do not interpret the current 2560x1440 swap-chain size as native 2560x1440 game rendering yet: the current gameplay source remains 1920x1440 and the frontend remains 640x480.


## DX11 Phase 2A implementation ready for runtime test — 2026-09-30

Objective:
Remove GDI/HDC from the normal DX11 presentation path and establish a real DX11 texture/shader frame path while legacy D3D7 still renders the source scene.

Implementation:
- renderer ABI bumped to 3;
- new export: `SpideyRenderer11_PresentPixels`;
- modern helper now links `d3dcompiler`;
- D3D7 scene surface is locked after rendering completes;
- 32-bit X8R8G8B8/BGRA-compatible scene pixels are uploaded row-by-row into a dynamic `DXGI_FORMAT_B8G8R8A8_UNORM` D3D11 texture;
- a shader-model-4 fullscreen triangle samples that texture;
- point filtering preserves legacy pixel/UI sharpness;
- culling is explicitly disabled for deterministic fullscreen rendering;
- the DX11 render target is cleared to black, an aspect-fit viewport is selected, the textured triangle is drawn, and DXGI Present displays it;
- normal success returns presenter path code 3.

Fallback chain:
1. preferred: D3D7 surface Lock -> DX11 dynamic texture -> shader draw -> DXGI Present;
2. fallback: proven DX11 GDI-compatible HDC presenter;
3. final fallback: proven direct scene->HWND GDI presenter.

Legacy surface lock behavior:
- first attempt uses `DDLOCK_WAIT | DDLOCK_READONLY`;
- if that fails, retry with `DDLOCK_WAIT` only;
- unsupported/failing lock or unexpected pixel masks do not disable DX11; they fall back to the ABI-2 HDC bridge;
- only an actual `PresentPixels` failure disables the pixel path for the remainder of that process, again falling back to DX11/HDC.

Implementation commits:
- `1cf7b67e63ea14b084791203dd8b08a460e17964` — ABI 3 / pixel presenter declaration;
- `81d5262c786ed356385ebb60a26eecef87f11ee7` — pixel presenter export;
- `37da7baf56d80827628e546bc368290fd3b6a23f` — link d3dcompiler;
- `1cbc08494845077ce6a22e673db468247109f6ed` — shader/upload presentation pipeline;
- `f79703467838744641f94a1984b2861112ebe5dd` — proxy prefers locked-surface pixel upload;
- `57986686d1aa0fdc2bda6af6a0cebdbdf5d8e8fb` — DirectDraw lock retry;
- `5d79869e929dbde1292cccf10b44915b3d50f1b6` — point-filtered parity;
- `6574be387bafa9e13f77a11d0eb63df605e5c853` — deterministic no-cull fullscreen pass.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected build/runtime markers:
- clean modern renderer build;
- `renderer11_bridge loaded ... abi=3 expected=3 ... phase2_exports=1`;
- `renderer11_phase2 initialize ... result=1`;
- renderer log:
  - `blit_pipeline ready shader_model=4_0 filter=point cull=none`;
  - `upload_texture ready width=...`;
  - `present_pixels frame=...`;
- present log:
  - `compat_present_dx11_pixels ... result=1`;
  - `present_path ... dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

If the legacy surface cannot be locked or its pixel layout differs, expected safe fallback:
- `compat_present_dx11_pixels ... skipped ... fallback=dx11_hdc`;
- `present_path ... dx11=1 dx11_pixels=0 dx11_hdc=1 ... compat_result=2`.

Visual expectation:
- no intended visual change yet;
- 4:3 pillarboxing remains expected;
- 2560x1440 remains quarantined as a legacy D3D7 scene mode;
- this test proves the final image passes through a D3D11 texture and shader rather than GDI.

Texture/render migration map discovered while implementing Phase 2A:
- `PCTex_CreateTexturePVRInId` is the central legacy DirectDraw texture creation/upload path;
- `PCTex_ReleaseSysTexture` and `PCTex_ReleaseAllTextures` centralize legacy texture destruction;
- `PCTex_GetDirect3DTexture` exposes each texture's DirectDraw surface;
- `PCGfx_ProcessTexture` chooses the current texture and calls `DXPOLY_SetTexture`;
- queued textured quads place the DirectDraw texture in `DXPOLY::field_4`;
- `PCGfx_BeginScene` / `PCGfx_EndScene` bracket `DXPOLY_BeginScene` / `DXPOLY_EndScene`.

That map is the basis for Phase 2B: mirror legacy texture handles into DX11 SRVs and migrate 2D/textured draw submission away from D3D7 incrementally.


## DX11 Phase 2A PASSED — 2026-09-30

Tested revision:
`93c667546ee25ca567e01e5a0bc36daf0c4428cb`

User-visible result:
- game loaded normally with the Phase 2A shader-upload path active.

Runtime proof:
- ABI 3 bridge loaded with `phase2_exports=1`;
- DX11 initialized at 2560x1440;
- the fullscreen shader pipeline compiled/created successfully;
- a 1920x1440 BGRA8 upload texture was created for the gameplay/boot source;
- the D3D7 scene surface locked successfully on the first attempt (`lock_retry=0`);
- visible presentation used `dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`;
- frontend transition rebuilt the DX11 upload texture at 640x480 and continued on the same shader path;
- full uploaded logs contain no `failed`, `fallback`, `dx11_hdc=1`, or `direct_hwnd=1` marker;
- shader presentation continued through at least frame 3240.

Phase 2A is CLOSED.

NEXT FRONTIER — Phase 2B:
Introduce DX11 sidecar texture ownership keyed by the game's existing PCTex IDs. Mirror legacy texture creation/destruction into the renderer11 DLL while leaving D3D7 draws intact. This creates a verified DX11 texture inventory before any primitive class is switched over.


## DX11 Phase 2B implementation ready for runtime test — 2026-09-30

Objective:
Establish DX11 ownership of the game's individual texture resources before migrating primitive submission.

Architecture:
- renderer ABI is now version 4;
- renderer11 owns a 1024-entry sidecar texture table keyed by the existing PCTex texture ID;
- each mirrored texture is normalized to `DXGI_FORMAT_B8G8R8A8_UNORM` and receives a DX11 SRV;
- 16-bit, 24-bit, and 32-bit legacy RGB mask formats are accepted and converted using the actual DirectDraw surface masks;
- each sidecar stores the final legacy DirectDraw texture-surface handle;
- renderer11 maintains a fast legacy-handle -> texture-ID index;
- PCTex creation mirrors from the final converted system-memory staging surface, so DX11 receives the exact pixel result D3D7 receives after legacy palette/PVR conversion;
- PCTex release/release-all destroys the matching DX11 sidecar resource;
- DX11 is explicitly initialized after retail D3D7 init, with a lazy texture-init fallback if boot order ever creates a texture earlier.

DXPOLY coverage instrumentation:
- the existing D3D7 render path remains unchanged;
- `renderScene()` counts total polygons, textured polygons, mirrored texture hits, missing texture hits, and resident DX11 texture count;
- coverage is logged for the first five scenes and every 120th scene;
- no DX11 primitive draw is active yet.

Key implementation commits:
- `e7ca821bfff4745401b85d746a0e6d1d0f0f4e00` — DX11 game texture sidecar;
- `3452fa305e263df85c7ba616690ed7edc4f5c350` — correct resident counting on replacement;
- `779cdc4d1e58eb7b1b025997a71759b1efc020b3` — legacy bridge declarations;
- `22195d7063fd9547102ad1976dcf0cb46481fa87` — legacy->DX11 texture bridge and early initialization;
- `69ed6607724e88804dfdd884bf44de5f10a00b42` — PCTex creation/release lifetime mirroring;
- `522c81a3f0eac472647795b18dc5c92462f8dc20` — lazy texture initialization fallback;
- `d2c5566fbd906de64f041e14026d84714df576bb` / `5126e963741ca6fa37999d0f106d700a181d35eb` / `830c56dd3c89818941ce2db893bf82009d03f77c` — legacy surface-handle association;
- `e875b685f470a424465f32dfeb310da13bb659f7` / `a95f2c3f241d3d0665fc341454292216a0018962` — fast handle resolver;
- `cf9074a3e506c18bcd329d8e301518b376c9a173` — DXPOLY migration-coverage instrumentation.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected:
- renderer bridge log: `abi=4 expected=4 ... phase2b_exports=1`;
- compat log: `renderer11_phase2b early_initialize ... result=1` (or, if boot ordering differs, a successful `lazy_texture_initialize`);
- renderer log should contain many `texture_update id=... resident=...` and `texture_handle id=... handle=...` entries;
- legacy texture log should contain matching `dx11_mirror ... result=1` and `dx11_associate ... result=1`;
- `dx11_draw_coverage` lines should show how many real textured polygons resolve to mirrored DX11 textures;
- Phase 2A presentation should remain `dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

Important:
A successful 2B test still renders polygons with D3D7. It proves resource parity and handle coverage. Phase 2C will use these SRVs and the already-decoded `SDXPolyField` layout to replace the centralized D3D7 `DrawPrimitive(D3DPT_TRIANGLEFAN,...)` path incrementally.


## DX11 Phase 2B resource mirroring PASSED; retail draw hook required — 2026-09-30

Tested revision:
`ba3e494f52edf269d22f7cde82e78527978a2cdd`

User result:
- game booted normally;
- user entered gameplay and played successfully.

Resource-mirroring proof from the uploaded logs:
- renderer bridge loaded ABI 4 with `phase2b_exports=1`;
- early DX11 initialization succeeded at 2560x1440;
- 2,002 PCTex mirror operations succeeded;
- 2,002 legacy-handle associations succeeded;
- zero `dx11_mirror ... result=0` records were emitted;
- all observed mirrored source textures in this run were 16-bit A1R5G5B5-style surfaces (R=0x7C00 G=0x03E0 B=0x001F A=0x8000);
- resident DX11 texture count reached 568;
- Phase 2A presentation remained `dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3` through gameplay.

Important discovery:
- no `dx11_draw_coverage` lines were emitted at all;
- therefore the retail EXE is still executing its own `renderScene()` / DXPOLY loop;
- the reconstructed `DXsound.cpp::renderScene()` implementation is not a live patched path;
- the PCTex hooks are live because `patch_pctex()` redirects those functions, but DXPOLY has not yet been redirected.

Retail D3D7 device address recovered from existing runtime disassembly evidence:
- prior DX error diagnostics around the verified retail CreateDevice call at `0x004FEA4A` include:
  `A1 18 79 6B 00` -> retail IDirect3D7* at `0x006B7918`;
  `8B 15 08 79 6B 00` -> scene surface at `0x006B7908`;
  `68 1C 79 6B 00` -> address of the CreateDevice output slot;
- therefore the live retail `IDirect3DDevice7*` is stored at `0x006B791C`.

NEXT FRONTIER:
- hook the live retail IDirect3DDevice7 vtable rather than the reconstructed DXPOLY queue;
- first hook is diagnostic/pass-through only:
  - validate the device pointer with GetCaps;
  - wrap BeginScene/EndScene/DrawPrimitive (and optionally Clear) while always calling the original D3D7 methods;
  - resolve each current texture surface against the already-proven DX11 sidecar table;
  - log real retail draw coverage and FVF/primitive usage;
- do not suppress or alter D3D7 rendering until runtime coverage is proven.


## Phase 2C0 live retail DrawPrimitive probe ready — 2026-09-30

Purpose:
Observe the actual retail D3D7 primitive stream before suppressing or replacing any D3D7 draw.

Why this is needed:
- Phase 2B proved PCTex resource mirroring, but reconstructed `DXsound.cpp::renderScene()` is not on the live retail execution path;
- therefore coverage must be measured at the live COM device boundary rather than the proxy-owned reconstructed scene queue.

Implementation:
- recovered retail `IDirect3DDevice7*` slot: `0x006B791C`;
- validates the live pointer with `GetCaps`;
- resolves and validates the device vtable;
- hooks only `IDirect3DDevice7::DrawPrimitive` at vtable index 25;
- original method is preserved and ALWAYS called; this build does not suppress, duplicate, or replace a primitive;
- first/unusual draws log:
  - primitive type;
  - FVF;
  - vertex count/flags;
  - stage-0 legacy texture pointer;
  - resolved DX11 sidecar texture ID;
  - first TL vertex when FVF is 0x144;
  - viewport;
  - depth, alpha blend/test, fog, texture color/alpha ops, addressing, and filtering state;
- per presented frame logs aggregate calls/textured/mirrored/missing/primitive/FVF counts;
- probe is revalidated after display-option changes because retail can recreate the D3D7 device;
- probe is also revalidated at the verified Flip boundary;
- `tools/TEST_LATEST_BUILD.ps1` now captures `spidey-decomp-draw.log`.

Code commits:
- `cfa1db3759beba806a1e5a45fc92088e5d756c83` — live pass-through D3D7 DrawPrimitive probe;
- `524d17a3f7823cf54b5df586e8ee05ccfa0df986` — test harness captures draw log;
- `071e69d34a6a3903094e4b4e038cf7804ae767d5` — fixed-function/viewport state snapshot on sampled draws.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat` and boot through menus into gameplay.

Expected new log:
`spidey-decomp-draw.log`

Expected installation marker:
`draw_probe installed device_slot=0x006B791C ... index=25 ... getcaps_hr=0x00000000`

Expected live draw samples:
`draw_sample ... primitive=... fvf=... count=... texture=... mirrored_id=... v0=... state=... viewport=...`

Expected frame coverage:
`draw_frame frame=... calls=... textured=... mirrored=... missing=... triangle_fan=... fvf_0x144=... other_primitive=... other_fvf=... resident=...`

Safety expectation:
- visuals should be unchanged;
- every probed draw is still executed by the original D3D7 DrawPrimitive;
- Phase 2A framebuffer upload/presentation remains the visible path.

GO/NO-GO for actual Phase 2C rendering:
- GO when live retail draws are overwhelmingly/fully TRIANGLEFAN + FVF 0x144 and textured draws resolve to DX11 mirrored IDs with negligible missing coverage;
- any other primitive/FVF/state pattern will be implemented explicitly before D3D7 suppression.


## Phase 2C0 retail DrawPrimitive probe PASSED — gameplay run 2026-09-30

Tested revision:
`88c37819fb3935ed7d8a8dd649044a111e4b5752`

User result:
- booted successfully;
- entered gameplay and played for a while;
- no new visible regression reported.

Probe validation:
- retail EXE fingerprint still matches the known target;
- live D3D7 device slot `0x006B791C` validated with `GetCaps = S_OK`;
- DrawPrimitive vtable hook installed successfully across D3D7 device recreation;
- Phase 2A DX11 presentation remained active and stable.

Full draw-log aggregate:
- logged active draws: **2,423,890**;
- triangle fans: **2,423,890 / 2,423,890 (100%)**;
- FVF 0x144: **2,423,890 / 2,423,890 (100%)**;
- other primitive types: **0**;
- other FVFs: **0**;
- textured draws: **2,384,258**;
- DX11-mirrored textured draws: **2,383,026**;
- unresolved textured draws: **1,232**;
- mirrored coverage: **99.9483%** of textured draws;
- unresolved draws involve only **9 distinct DirectDraw surface pointers**;
- max observed calls in a logged frame: **9,316**;
- resident DX11 texture set reached 568.

Observed baseline fixed-function state:
- primitive = D3DPT_TRIANGLEFAN (6);
- FVF = 324 / 0x144;
- transformed/lit vertex shape matches XYZRHW + diffuse + UV;
- common depth: Z enabled, ZWRITE enabled for opaque and disabled for translucent, ZFUNC=4 (LESSEQUAL);
- common opaque blend: ALPHABLEND=0, SRC=2, DST=1;
- common translucent blend observed on unresolved surfaces: ALPHABLEND=1, SRC=5, DST=6;
- texture color/alpha op commonly 4 (MODULATE), ARG1=2 (TEXTURE), ARG2=0 (DIFFUSE);
- addressing observed WRAP(1) and CLAMP(3);
- MAG/MIN filter observed value 2 (linear);
- viewport tracks the retail internal target (e.g. 640x480 frontend).

Important conclusion:
The live retail renderer is dramatically narrower than a general D3D7 backend. The complete observed primitive stream is one primitive family + one FVF. This makes a direct DX11 TL-vertex compatibility pipeline practical.

NEXT FRONTIER:
1. add on-demand mirroring for the 9 transient/unresolved DirectDraw texture surfaces at the draw boundary;
2. extend renderer11 ABI for parallel retail primitive submission;
3. render triangle-fan/FVF-0x144 draws into a separate DX11 offscreen scene target while D3D7 remains the visible reference;
4. initially emulate the reconstructed fixed-function subset:
   - depth enable/write/compare;
   - blend modes used by DXPOLY_SetBlendMode;
   - texture modulate/select behavior;
   - texture alpha enable;
   - wrap/clamp;
   - point/linear filter;
   - viewport;
5. compare/log DX11 shadow frame coverage before suppressing any D3D7 draw.


## CHECKPOINT — live retail draw stream validated; ready for parallel DX11 geometry — 2026-09-30

Reason for checkpoint:
User explicitly requested a checkpoint before further renderer changes.

Last tested runtime revision:
`88c37819fb3935ed7d8a8dd649044a111e4b5752`

Observed runtime status:
- game booted cleanly;
- user entered gameplay and played without a reported regression;
- retail D3D7 DrawPrimitive probe installed successfully on the live device;
- retail device slot `0x006B791C` validated with `GetCaps=S_OK`;
- Phase 2A DX11 shader presentation remained active;
- Phase 2B DX11 texture sidecar remained stable.

Retail primitive stream — full-run aggregate:
- total observed active DrawPrimitive calls: **2,423,890**;
- **100%** primitive type = `D3DPT_TRIANGLEFAN`;
- **100%** FVF = `0x144` / decimal 324;
- other primitive types = **0**;
- other FVFs = **0**;
- textured draws = **2,384,258**;
- textured draws resolving to mirrored DX11 textures = **2,383,026**;
- unresolved textured draws = **1,232**;
- DX11 texture-handle coverage = **99.9483%**;
- unresolved draws are concentrated in only **9 distinct DirectDraw surface pointers**;
- maximum observed DrawPrimitive calls in a logged frame = **9,316**;
- resident DX11 texture set reached **568**.

Verified live vertex/state shape:
- transformed/lit vertex data is XYZRHW + diffuse + UV;
- triangle-fan stream matches the reconstructed `SDXPolyField` layout;
- common depth state: Z enabled, opaque Z-write enabled, translucent Z-write disabled, ZFUNC=LESSEQUAL;
- opaque: alpha blend disabled, SRC=ONE, DEST=ZERO-equivalent D3D7 values observed as SRC=2 DST=1;
- translucent path observed SRC=5 DST=6;
- texture color/alpha op commonly MODULATE;
- ADDRESSU/V observed WRAP and CLAMP;
- MAG/MIN filtering observed LINEAR;
- viewport follows the retail internal target, e.g. 640x480 in frontend.

Critical architecture conclusion:
The live renderer is not a broad arbitrary D3D7 workload. The complete observed primitive stream is one primitive family and one FVF, which makes a focused DX11 compatibility renderer practical.

Important dead-end avoided:
- reconstructed `DXsound.cpp::renderScene()` is NOT the live runtime scene loop;
- do not base the migration on reconstructed `gSceneBuffer`;
- the correct migration seam is the live retail `IDirect3DDevice7::DrawPrimitive` COM boundary.

NEXT SAFE IMPLEMENTATION STEP:
1. add on-demand mirroring at the live draw boundary for the 9 unresolved DirectDraw surfaces;
2. extend renderer11 ABI with a retail-TL-vertex draw submission entry point;
3. create a separate DX11 offscreen scene color/depth target;
4. shadow every eligible retail triangle-fan draw into that DX11 target while ALWAYS still calling original D3D7 DrawPrimitive;
5. emulate only the observed fixed-function subset first:
   - viewport;
   - depth enable/write/compare;
   - blend;
   - texture modulate/alpha;
   - wrap/clamp;
   - point/linear filtering;
6. log shadowed/skipped draw coverage and never make the DX11 shadow scene visible until coverage/state parity is demonstrated;
7. after parity, compare the offscreen DX11 scene against the D3D7 reference before suppressing any D3D7 draw.

Do NOT yet:
- expose 2560x1440 as a D3D7 scene mode;
- remove D3D7 DrawPrimitive;
- replace the visible framebuffer with the new geometry shadow target;
- discard the Phase 2A framebuffer-upload fallback.


## DX11 Phase 2C1 shadow geometry implementation ready for runtime test — 2026-09-30

Goal:
Render the real retail D3D7 primitive stream into a completely offscreen DX11 scene target while preserving the original D3D7 renderer as the visible/reference path.

Safety model:
- retail D3D7 DrawPrimitive is ALWAYS called;
- no D3D7 primitive is suppressed;
- the DX11 shadow color/depth targets are never copied to the swap chain;
- the existing Phase 2A D3D7-scene -> DX11 PresentPixels path remains the only visible presentation path;
- shadow replay is sampled on frames 1-5 and every 120th frame to avoid doubling ~9k draw calls every frame during parity bring-up.

ABI / resource changes:
- renderer11 ABI bumped to 5;
- persistent PCTex IDs remain 0..1023;
- synthetic transient DX11 texture slots use 1024..2047;
- unresolved DirectDraw texture surfaces are AddRef'd/queued during live draws and locked/mirrored only after retail EndScene at the verified Flip boundary;
- shadow commands retain unresolved legacy handles, so transient textures mirrored at Flip can still resolve before the same frame is replayed.

Live D3D7 state hooks:
- IDirect3DDevice7::SetRenderTarget (vtable 8);
- Clear (10);
- SetViewport (13);
- SetRenderState (20);
- DrawPrimitive (25);
- SetTexture (35);
- SetTextureStageState (37).

State-cache behavior:
- current viewport/render/depth/blend/texture-stage/texture/render-target state is initialized from the live D3D7 device after hook install;
- setter hooks update the cache only after the original retail call succeeds;
- sampled/unusual draws still query the live D3D7 state and log `cache_mismatch=0/1` so state-block/bypass behavior cannot silently invalidate the shadow renderer;
- shadow clears/draws are accepted only when the current retail render target equals the main scene surface at `0x006B7908`.

DX11 shadow renderer:
- TL vertex input matches retail FVF 0x144: XYZRHW + diffuse + UV;
- triangle fans are expanded to ordered triangle lists;
- XYZRHW is converted to clip space while preserving reciprocal-W for perspective interpolation;
- vertex colors are decoded from packed D3DCOLOR;
- texture MODULATE + diffuse is implemented;
- alpha operation MODULATE and SELECTARG2(diffuse alpha) are implemented;
- untextured draws use a 1x1 white texture;
- depth enable/write/compare states are cached;
- D3D7 blend factors are mapped to DX11, including safe alpha-slot normalization for color-derived blend factors;
- wrap/clamp and point/linear sampler states are cached;
- offscreen scene target is BGRA8 with D24S8 depth;
- all expanded vertices for a sampled frame are uploaded in one dynamic-buffer Map, then original draw ordering/state changes are replayed.

Parity diagnostics:
- renderer11 samples the same 3x3 quarter/center coordinates used by `SpideyLogSurfaceState`;
- DX11 `shadow_frame` logs include `sample_hash` and `nonblack`;
- this allows direct frame-number comparison against `scene_pre sample_hash` in `spidey-decomp-present.log`;
- exact hash equality is not required for the first test, but `nonblack > 0` and stable render counts are mandatory before making the shadow scene visible.

Implementation commits after the user checkpoint:
- `4524794d0a5da2745f2fffc2108d34d892b18b77` — ABI 5 shadow/transient API;
- `23c6bfef4f9eac04f8c1cab407d53cd0ee3bf21f` — exports;
- `2883b8fd0d68d4e20b34b591ae6cadf0e61ce807` — batched shadow storage;
- `b2d41be98188a229dfd28a7bd5afb38797062924` — offscreen TL-vertex pipeline/state caches;
- `ad0679b1a0e68f63dcc0ec683d9fa033f4689c83` — transient textures + shadow submission/replay;
- `da49f46328f6950c7821d562495076dfd4f70196` — late transient resolve before replay;
- `be2ebc7fb700ba3c434b795bf64fc70cc4bed684` — proxy loads ABI 5;
- `e38017cd7c06a4df905a55fce374b47c0301e428` / `aff06e2351c1260d8a2d733d9cd8251acff57d15` — legacy bridge structures/functions;
- `4c713592b8124faafb30b4469bedbaddb5a37d2d` / `927395f5eda8cbacb54f4e809a2bd8fee937af6b` — D3D7 state cache and setter wrappers;
- `db064b0d3cfb5222510d775db03c37ea9dec4e3a` — live draws enqueue into shadow queue;
- `e46d6ba656238e81e1b9811307b920f1275e7285` — per-frame shadow/transient coverage;
- `86a335e337cdcfb3dd747f494217f3f74723e78d` — live vtable state-hook installation;
- `755bfc8a17b3ba0d8f2e73baa2be78ad4a77105c` — EndScene/Flip shadow replay;
- `47bf8226116ca134bae8a6ffe1c0bd2b99addaf3` / `b1d49b8afa1db0eda6365ba572a6a41c39114402` / `e8a23ac33e9b0b3bf4faa359943c5707fcb6d38b` — sampled replay/capture and clean accounting;
- `dca790dedb85049a367f2f5cd3e44763b8cfe058` — cached-vs-live state validation;
- `21895ce6559193850657cd3e3ac783249aea86e2` / `c6451e4c57eec7936712c5e1c7afba82d7d8e819` / `247dd1de95de04b48f2c36738766a6257538d4e7` — main-scene render-target isolation;
- `6e94d5649a327ac452e7ac5619be1bb23def5f30` — matching 3x3 DX11 sample hash;
- `d4de1a0d6f49dc7eee973b26cbd4a089d607c8e1` — D3D7 blend-factor normalization for DX11 alpha slots.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`, boot through the frontend, start/enter gameplay, move around for at least a short interval, then exit normally.

Expected build/runtime markers:
- compat: `renderer11_bridge loaded ... abi=5 expected=5 ... phase2c_exports=1`;
- draw log: seven `state_hook installed` markers and
  `draw_probe installed ... state_hooks=7 shadow_state_valid=1`;
- sampled draw lines should report `cache_mismatch=0`;
- first unresolved legacy surfaces should produce `transient_mirror ... result=10xx` after EndScene;
- sampled `draw_frame` lines should show nonzero `shadow_submit`, ideally zero `shadow_skip`, and classify any non-main-target draws under `shadow_offscreen_skip`;
- renderer log should show:
  - `shadow pipeline ready shader_model=4_0 tl_vertex=1`;
  - `shadow targets ready width=...`;
  - `shadow vertex_buffer ready bytes=...`;
  - `shadow_frame ... replay=1 ... rendered=... skipped_render=... sampled=1 sample_hash=... nonblack=...`;
- visible present path must remain:
  `dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

GO criteria for Phase 2C2:
- normal boot/gameplay remains stable;
- no state-hook cache mismatches;
- transient surface mirroring succeeds or remaining misses are specifically characterized;
- sampled-frame shadow_submit/queued/rendered counts agree for main-scene draws;
- skipped_render is zero or fully explained;
- DX11 shadow sample is nonblack on actual rendered gameplay frames.

If those pass, next step is Phase 2C2: make a diagnostic copy/view of the DX11 shadow scene for visual parity inspection, correct any half-pixel/blend/depth differences, then prepare the first controlled switch where DX11 geometry becomes visible while D3D7 remains a fallback.


## DX11 Phase 2C1 PASSED — offscreen retail geometry shadow validated — 2026-09-30

Tested revision:
`ba57b5c7adaf49878b23f8a5cdc09bca33eb747a`

User-visible result:
- normal boot succeeded;
- no visible regression reported;
- existing D3D7-reference -> DX11 PresentPixels path remained the visible output.

Runtime validation:
- ABI 5 bridge loaded successfully with `phase2c_exports=1`;
- all seven retail D3D7 state/draw hooks installed successfully;
- live device validated with GetCaps=S_OK;
- sampled draw-state diagnostics reported **zero cache mismatches**;
- every sampled active retail frame had:
  - `shadow_submit == calls`;
  - `shadow_skip == 0`;
  - `shadow_offscreen_skip == 0`;
- renderer shadow replay had:
  - `queued == submitted == rendered`;
  - `skipped_submit == 0`;
  - `skipped_render == 0`;
- large gameplay samples successfully replayed thousands of real draws, including 8,271 draws at frame 2760 and 8,139 at frame 3240;
- shadow target sampled non-black on every compared active frame;
- the unusual frame 3120 had matching non-black occupancy on both paths: 8/9 samples;
- the remaining transient texture gap was successfully closed in this run:
  - 8 previously-unmapped DirectDraw surfaces were mirrored into synthetic IDs 1024..1031;
  - subsequent sampled frames reported `missing=0`;
  - one recovered surface was RGB565, demonstrating the transient conversion path is not limited to A1R5G5B5;
- resident DX11 texture count reached 576;
- renderer emitted no shadow setup/map/state creation failures.

Quantitative parity:
- the DX11 3x3 shadow sample hashes do not yet equal the retail D3D7 scene hashes;
- hash byte ordering was verified equivalent: both compute FNV-style accumulation over COLORREF-compatible 0x00BBGGRR values;
- therefore the mismatch represents actual pixel-value/rasterization/state differences, not merely diagnostic byte ordering;
- non-black occupancy nevertheless matches on every compared logged frame, strongly indicating the scene is structurally present and positioned plausibly.

Phase 2C1 conclusion:
The live retail primitive stream can now be reconstructed into an independent DX11 color/depth scene with complete sampled draw coverage and no observed state-cache divergence. D3D7 remains authoritative/visible.

NEXT FRONTIER — Phase 2C2:
- add exact nine-pixel sample-value logging to both D3D7 and DX11 diagnostics;
- add an opt-in runtime preview toggle that presents the DX11 shadow color target while D3D7 continues rendering in the background;
- run shadow capture continuously only while preview is enabled;
- preserve instant switch back to the known-good D3D7-reference presentation path;
- use direct visual comparison + per-pixel samples to correct half-pixel, blend, depth, texture/color, or viewport differences before suppressing any D3D7 draw.


## DX11 Phase 2C2 live shadow preview ready for runtime test — 2026-09-30

Phase 2C1 validation source run:
`ba57b5c7adaf49878b23f8a5cdc09bca33eb747a`

Validated before implementing 2C2:
- normal boot succeeded;
- all 7 live retail D3D7 hooks installed;
- ABI 5 bridge loaded;
- zero sampled cache mismatches;
- all sampled active frames had shadow_submit == retail calls;
- shadow_skip == 0;
- shadow_offscreen_skip == 0;
- renderer replay had queued == submitted == rendered and skipped_render == 0;
- 8 transient surfaces were recovered into synthetic IDs 1024..1031;
- subsequent sampled frames reported missing=0;
- resident DX11 texture count reached 576;
- shadow target was nonblack on every compared active sample;
- D3D7 and DX11 nonblack occupancy matched on every compared sampled frame, including 8/9 at frame 3120;
- renderer emitted no shadow setup/map/state creation failure.

Observed parity limitation:
- the 3x3 DX11 shadow hash does not yet equal the D3D7 scene hash;
- diagnostic byte ordering was verified equivalent, so the mismatch represents actual pixel/raster/state differences rather than hash encoding.

Phase 2C2 implementation:
- renderer ABI bumped to 6;
- new exports:
  - `SpideyRenderer11_ShadowSetContinuous(int)`;
  - `SpideyRenderer11_PresentShadow(int preserveAspect, int vsync)`;
- F10 toggles live DX11 shadow preview;
- enabling preview has a deliberate one-frame warmup so the first visible shadow frame is fully captured;
- while preview is enabled:
  - every main-scene retail triangle fan is shadow-captured;
  - shadow replay runs every frame;
  - the DX11 shadow color target is presented directly to the existing DXGI swap chain;
  - D3D7 still executes every original DrawPrimitive in the background;
- pressing F10 again immediately returns to the known-good D3D7 scene -> PresentPixels path;
- if PresentShadow fails, the proxy automatically falls back to the D3D7-reference presenter;
- preview remains aspect-fitted, so current 4:3 pillarboxing is expected;
- exact nine COLORREF sample values are now logged on both:
  - D3D7 `scene_pre ... samples=...`;
  - DX11 `shadow_frame ... samples=...`;
- expensive DX11 pixel readback remains sampled (first frames/every 120th), even while shadow rendering/presentation runs continuously;
- `present_path` now identifies `dx11_shadow=1`, preview enabled/ready state, and path code 4.

Implementation commits:
- `2b4b69b7e7de94ee73280e78e19d325193d88003` — ABI 6 declarations;
- `64e2b6d11e5b4f1a9e2fd3a3101fd54a7906acec` — ABI 6 exports;
- `2b36515e2598a0a16644ecb3c7e0ae9d19ee5fc1` — continuous shadow replay, direct shadow presentation, exact DX11 sample colors;
- `b99323d9a13f440f701b6a97840dfdee048e2fbe` — legacy preview-control declarations;
- `13c382f8dd1216a8c866652b7435e34b28618d92` — proxy loads ABI 6 preview exports;
- `2d6d5b8a962b93126f68b8af1aa0184b96e6bc30` — proxy preview bridge wrappers;
- `2fd8bddda9d05496571b87775dcdd289e6d802ea` — exact D3D7 sample colors;
- `ecb3eb6245c949a8a92a0eeba22050956e177d6e` — continuous capture while preview is active;
- `b279fcf9edc5beeff5159f0d5fd5565f48a73b15` — F10 preview toggle and path-4 presentation;
- `72f202b65d9eff538f2995dbf53119730a574bf0` — sampled-only parity readback during continuous preview.

NEXT TEST:
1. Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.
2. Boot normally into gameplay.
3. First verify the game still looks normal before touching F10.
4. Press **F10 once** to switch visible output to the DX11 shadow renderer.
5. Move around/look around for several seconds and inspect:
   - geometry placement;
   - textures;
   - HUD/menu;
   - transparency/blending;
   - depth/occlusion;
   - any half-pixel or shimmering offset;
   - missing/black/flickering elements.
6. Press **F10 again** and confirm the normal D3D7-reference image returns instantly.
7. If useful, toggle back and forth several times.
8. Exit normally and provide screenshots plus the captured logs.

Expected ABI/runtime markers:
- compat: `abi=6 expected=6 ... phase2c2_exports=1`;
- normal presentation before F10:
  `dx11_shadow=0 dx11_pixels=1 ... compat_result=3`;
- toggle:
  `shadow_preview_toggle ... enabled=1 key=F10`;
  renderer: `shadow_continuous enabled=1`;
- after one warmup frame:
  `present_path ... dx11_shadow=1 ... shadow_preview=1 shadow_ready=1 compat_result=4`;
  renderer: `present_shadow ...`;
- F10 off:
  `shadow_preview_toggle ... enabled=0`;
  renderer: `shadow_continuous enabled=0`;
  presentation returns to compat_result=3.

GOAL OF THIS TEST:
Visually characterize the first independently-rendered DX11 Spider-Man scene. Do not suppress D3D7 yet. Use the new exact sample colors + screenshots to correct parity before making DX11 authoritative.


## DX11 Phase 2C2 PASSED — live shadow renderer visually validated — 2026-09-30

Tested revision:
`27022600ea3d7328abe3f5cebd718ea73581134a`

User visual result:
- F10 DX11 shadow preview looked correct;
- user reported it may have looked slightly better than the D3D7-reference image;
- repeated toggling remained stable.

Runtime validation:
- ABI 6 loaded with `phase2c2_exports=1`;
- all seven retail state/draw hooks installed;
- repeated F10 on/off transitions were observed throughout the run;
- while DX11 shadow was visible, `present_path` reported:
  - `dx11_shadow=1`;
  - `dx11_pixels=0`;
  - `shadow_preview=1`;
  - `shadow_ready=1`;
  - `compat_result=4`;
- renderer11 continuously presented the shadow target at 1920x1440 aspect-fitted into the 2560x1440 swap chain;
- no renderer11 `failed`, `rejected`, `setup_failed`, `map_failed`, `present_shadow present_failed`, or no-free-slot diagnostics occurred;
- no `cache_mismatch=1` occurred;
- all logged sampled DX11 frames had `skipped_submit=0` and `skipped_render=0`;
- representative gameplay frame 4080:
  - retail calls = 5,007;
  - shadow_submit = 5,007;
  - missing = 0;
  - renderer queued/submitted/rendered = 5,007/5,007/5,007;
  - skipped_render = 0;
- transient texture recovery continued to work, including RGB565 surfaces;
- later frontend transition at frame 4200 still replayed all 3,587 draws successfully after transient recovery.

Exact pixel parity:
- matched D3D7/DX11 sample sets remain close but not bit-identical;
- across 35 matched nine-pixel sample frames, median absolute per-channel difference was 1 level;
- mean absolute per-channel difference was about 2.78 levels;
- this is consistent with small legacy-vs-DX11 raster/filter/color-math differences rather than missing scene content;
- visual inspection found no objectionable discrepancy.

Phase 2C2 conclusion:
The independent DX11 geometry renderer is visually viable and stable enough to become the default visible renderer.

NEXT FRONTIER — Phase 2C3:
- make DX11 geometry/shadow presentation enabled by default;
- retain F10 as an immediate A/B fallback to the D3D7-reference image;
- keep D3D7 DrawPrimitive executing in the background for one more validation stage;
- after default-DX11 runtime validation, begin a controlled mode that suppresses original main-scene D3D7 DrawPrimitive while leaving all state/texture/device plumbing intact.


## DX11 Phase 2C3 — DX11 geometry is now the default visible renderer — 2026-09-30

Implementation:
- `gSpideyShadowPreviewEnabled` now defaults to 1;
- the first Flip synchronizes renderer11 continuous shadow replay before the first shadow EndFrame;
- the default windowed/compat presentation path is therefore:
  retail state + primitive interception -> renderer11 DX11 scene -> DXGI swap chain;
- the old D3D7 scene -> PresentPixels path remains intact as the built-in reference fallback;
- **F10 now acts as the A/B reference toggle**:
  - default/on = DX11 geometry visible;
  - off = D3D7-rendered reference image visible;
  - toggling DX11 back on keeps the deliberate one-frame warmup to avoid presenting a partially captured frame;
- original D3D7 DrawPrimitive is still executed in the background in both modes for this validation stage.

Source commit:
- `4fc226ceb3e11003e6872feef9c1ad7a042a985e` — make DX11 geometry the default visible renderer.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. do **not press F10 initially**;
3. confirm startup, frontend, level load, HUD, and gameplay all look correct with DX11 now active by default;
4. play for several minutes;
5. press F10 once and confirm the old D3D7-reference image appears;
6. press F10 again and confirm DX11 returns after one warmup frame;
7. return to frontend / change level if convenient, to validate device/mode transitions while DX11 remains default;
8. exit normally and provide logs.

Expected markers:
- `shadow_default frame=... enabled=1 mode=dx11_geometry key=F10_reference_toggle`;
- renderer: `shadow_continuous enabled=1`;
- normal default presentation after startup:
  `dx11_shadow=1 dx11_pixels=0 shadow_preview=1 shadow_ready=1 compat_result=4`;
- F10 reference mode:
  `enabled=0` followed by `dx11_shadow=0 dx11_pixels=1 ... compat_result=3`.

If Phase 2C3 passes:
- begin Phase 2D: controlled suppression of original **main-scene** D3D7 DrawPrimitive while DX11 is authoritative;
- keep D3D7 state setters, texture/resource creation, and any offscreen passes intact initially;
- provide a reference-mode switch that re-enables D3D7 draws with a warmup frame;
- prove the visible game no longer depends on D3D7 geometry rendering before moving scene/depth/resource ownership further into DX11.


## DX11 Phase 2C3 PASSED; Phase 3A native desktop gameplay implemented — 2026-09-30

Latest user validation:
- DX11 geometry default-visible run completed cleanly;
- user reported no visual issues;
- F10 reference/DX11 toggling remained clean;
- the user noted tiny frametime hitches roughly every 0.5–1 second, but reports they were already present before the DX11 conversion and occur regardless of renderer;
- treat the hitch as a separate profiling item after native-resolution/aspect work unless new evidence ties it to renderer diagnostics.

Runtime evidence:
- ABI 6 / seven retail D3D7 hooks remained healthy;
- default mode starts with DX11 geometry enabled;
- once scene geometry becomes active, presentation runs through `dx11_shadow=1`, `dx11_pixels=0`, `compat_result=4`;
- sampled gameplay draws continue with zero shadow skips and zero missing textures after transient capture.

Phase 3A implementation:
- preserve a safe physical D3D7 compatibility surface;
- separate that from the game's **logical gameplay resolution**;
- after each retail display-mode transition:
  - frontend remains its original physical/logical 640x480 path;
  - gameplay logical resolution is set to the borderless client/desktop dimensions;
- on the current 2560x1440 desktop this creates:
  - D3D7 compatibility backing: 1920x1440 (current known-good mode);
  - game logical gameplay viewport: 2560x1440;
  - DX11 shadow color/depth target: 2560x1440;
- shadow DrawPrimitive state uses the modern logical viewport while the retail D3D7 device keeps its physical viewport;
- renderer11 therefore presents the gameplay shadow target 1:1 to the 2560x1440 swap chain instead of aspect-fitting a 1920x1440 target;
- frontend remains 4:3/pillarboxed for this first native-gameplay phase;
- F10 fallback now restores legacy logical dimensions before exposing the D3D7 reference, and keeps the completed DX11 frame visible during that one-frame handoff;
- F10 back to DX11 restores the desktop-native logical dimensions with the existing warmup.

Aspect handling:
- this is desktop/client driven rather than hard-coded 2560x1440;
- 16:9, 16:10, ultrawide, and other client aspects feed the same logical-resolution path;
- the first required runtime proof is 2560x1440/16:9 on the user's current display.

True Hor+ verification:
- the DrawPrimitive probe now logs per-frame TL-vertex X/Y ranges;
- it also reports how many vertices extend outside the physical 4:3 D3D7 width/height;
- if logical 2560x1440 causes vertices to populate X > 1920 while the vertical range remains appropriate, the original engine projection is naturally responding to the new logical width and true Hor+ is active;
- if geometry remains confined to the physical 4:3 range, the next step is a narrowly scoped upstream projection/FOV hook rather than stretching the image.

Implementation commits:
- `dee4d8165afd880cf2961d084cccd2ecd3585425` — desktop-native logical-resolution controls;
- `e3ce9f6bc8ebe4595a4c548291e97559b8eb609f` — apply modern logical gameplay dimensions after retail mode changes;
- `b4e57dc4e5e6309e8dc3c209eb2df437e4c36e70` — bind captured draws to modern logical viewport + geometry-range telemetry;
- `2884cde0e82513d3e413dbf75b7f3e10976b4a21` — native-sized DX11 target + clean F10 legacy handoff.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected gameplay on the current system:
- compat log:
  `logical_render_resolution ... modern=1 frontend=0 logical=2560x1440 physical=1920x1440 client=2560x1440`;
- renderer log:
  `shadow targets ready width=2560 height=1440`;
  `present_shadow ... src=2560x1440 dst=2560x1440 rect=0,0,2560x1440`;
- present log should show DX11 shadow default without the old 320-pixel pillarbox;
- draw log should show:
  `modern=1 logical=2560x1440 physical=1920x1440 xrange=... outside_physical_x=...`.

Visual checks:
- gameplay fills 16:9 without horizontal stretch;
- compare vertical framing against the old 4:3 view: desired behavior is same vertical FOV with more world visible left/right;
- inspect HUD placement and cutscene/gameplay transitions;
- frontend is intentionally still 4:3;
- F10 should fall back to a clean 4:3 D3D7 reference, then restore native DX11 on the next toggle.


## Phase 3A runtime result — native settings were not exposed — 2026-09-30

Tested revision:
- `c8f41c98065585bac837fa3218896981dd98f44c`.

User result:
- no visible widescreen difference;
- 2560x1440 was not selectable in Display Options;
- there was no user-facing 16:9/aspect-ratio setting.

Runtime evidence:
- every observed Display Options apply remained the retail frontend request `640x480x16`;
- modern mode injection grew the mode table from 4 to 23 entries and detected the Windows 2560x1440 desktop, but the injector was still explicitly omitting 2560x1440 because of the earlier D3D7 CreateDevice failure;
- DX11 itself was already initialized with a 2560x1440 swap target, so the missing setting is a menu/configuration-layer problem, not a DX11 capability problem.

Root cause / revised design:
- the old 2560x1440 quarantine belongs only at the **legacy D3D7 physical backing** layer now;
- 2560x1440 must be exposed to the retail Screen Size selector and preserved as the user's selected/output resolution;
- selecting 2560x1440 will use a known-good 1920x1440 D3D7 compatibility backing while DX11 owns the requested 2560x1440 output;
- the original `PCSHELL_DoDisplayOptions` is at `0x0050D9B0` and is 1476 bytes;
- its three rows are Screen Size, Color Depth, Brightness;
- row 0 uses `DXINIT_GetPrevResolution`/`DXINIT_GetNextResolution`;
- row 1 uses `DXINIT_GetPrevColorDepth`/`DXINIT_GetNextColorDepth`;
- row 2 is brightness.

Aspect-ratio implementation plan:
- because DX11 output is always 32-bit, repurpose the obsolete Color Depth row as **Aspect Ratio** without changing CMenu row count/layout;
- patch the exact row-1 formatter and left/right helper calls in retail `PCSHELL_DoDisplayOptions`;
- modes: AUTO, 4:3, 5:4, 16:9, 16:10, 21:9, 32:9;
- apply the known retail projection/aspect scalar at runtime VA `0x00550064`;
- AUTO scalar = `(4 * height) / (3 * width)`; known explicit values include 4:3=1.0 and 16:9=0.75;
- persist the aspect selection in a small modern-video INI next to the game.

Current action:
- expose 2560x1440 in the retail resolution list;
- preserve requested modern resolution separately from the safe D3D7 backing;
- install the in-game Aspect Ratio row patch;
- then provide one new runtime frontier for the user to test.


## Phase 3B — in-game modern video settings implemented — 2026-09-30

Goal:
Make modern output resolution and aspect ratio explicit, selectable settings in the retail Display Options screen.

Retail UI reverse engineering:
- exact function: `PCSHELL_DoDisplayOptions = 0x0050D9B0`, size 1476 bytes;
- retained historical retail function bytes were used to verify all patch sites;
- byte-verified direct calls:
  - `0x0050DBBB -> 0x00529F90` (row-1 value formatter / sprintf);
  - `0x0050DDAB -> 0x005010C0` (previous color depth);
  - `0x0050DDCE -> 0x00501060` (next color depth);
- row-1 label pointer slot: `0x0054BBD4`;
- retail aspect/projection scalar: runtime VA `0x00550064`.

Resolution changes:
- 2560x1440 is no longer omitted from the injected retail resolution table;
- the existing Screen Size row can therefore enumerate it through the original `DXINIT_GetPrevResolution` / `DXINIT_GetNextResolution` logic;
- selected/output resolution is now separate from the legacy D3D7 physical backing;
- selecting 2560x1440 preserves/saves **2560x1440x32**;
- only the hidden legacy D3D7 backing is remapped to known-good **1920x1440x32**;
- startup restoration also preserves 2560x1440 instead of overwriting the saved config with a fallback;
- logical DX11 render dimensions now prefer the explicitly selected Screen Size rather than always following the desktop client.

Aspect-ratio menu:
- the obsolete Color Depth row is repurposed in-place as **Aspect Ratio**;
- this preserves the retail three-row CMenu layout and all existing input/drawing behavior;
- selectable values:
  - AUTO
  - 4:3
  - 5:4
  - 16:9
  - 16:10
  - 21:9
  - 32:9
- DX11 output is fixed at 32-bit, so removing user-facing color-depth selection does not remove a meaningful modern renderer option;
- explicit projection scalars:
  - 4:3 = 1.0
  - 5:4 = 1.06667
  - 16:9 = 0.75
  - 16:10 = 0.83333
  - 21:9 = 0.57143
  - 32:9 = 0.375
- AUTO computes `(4 * height) / (3 * width)` from the selected output resolution;
- the chosen scalar is written live to `0x00550064`.

Persistence:
- aspect selection is stored in `spidey-modern-video.ini` beside `SpideyPC.exe`;
- menu left/right changes save immediately;
- startup reloads the aspect mode;
- final display-mode application re-applies the scalar after Screen Size changes.

Safety:
- each retail call patch verifies opcode `E8` and the exact expected original target before writing;
- a mismatch is logged and the patch is skipped rather than writing to an unknown executable;
- the existing frontend 640x480 compatibility canvas remains intact;
- F10 DX11/reference fallback remains intact.

Implementation commits:
- `a1b685189f9e614b8fda21968bd4d824cb05877c` — expose 2560x1440 in Screen Size;
- `15de90532e319e5c161602b837437bc73b8492e7` — preserve selected output across legacy backing remap;
- `b2cad1aa4e1b833fe1b7751cdac60eb797d7e912` — separate selected output from D3D7 physical resolution at apply time;
- `99a459e5f3b163011faf17ba00f12c5aec30e76a` — repurpose Color Depth as Aspect Ratio using exact retail call patches;
- `f9f23c6dc59dfc460c9b17d12eb28d1d6746cc87` — install/persist/apply aspect selection;
- `6235292cdba5abeff5103d6a8bce9d6ba014fef4` — pin modern-video INI to the game directory.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. open Options -> Display Options;
3. verify the second row says **Aspect Ratio**;
4. cycle it and confirm AUTO / 4:3 / 5:4 / 16:9 / 16:10 / 21:9 / 32:9 are visible;
5. on Screen Size, cycle until **2560x1440** is visible;
6. select **2560x1440 + 16:9**;
7. leave/apply the menu, start gameplay, and inspect framing/HUD;
8. optionally re-open Display Options and confirm the selected resolution remains 2560x1440;
9. exit and provide the generated logs.

Expected log markers:
- `modern_modes ... windows_1440=1 ui_2560x1440_exposed=1 ...`;
- three `display_menu_patch ... installed=1` lines;
- `display_menu_mod ... label=1 format=1 prev=1 next=1 modes=7`;
- `display_aspect ... label=16:9 scalar=0.750000 selected=2560x1440`;
- `display_options selected=2560x1440x32 physical=1920x1440x32 ... legacy_backing_remap=1 preserve_selected=1`;
- gameplay: `logical_render_resolution ... logical=2560x1440 physical=1920x1440 selected=2560x1440`;
- renderer11 shadow target/presentation at 2560x1440.

Pass criteria:
- settings are actually visible/selectable in the original Display Options screen;
- 2560x1440 survives menu exit/re-entry and restart;
- 16:9 visibly affects gameplay projection without stretching;
- frontend remains stable.


## Phase 3C — transactional Display Options + Apply row — 2026-09-30

Tested revision that exposed the bug:
- `91ffd2884f2591eab48374386c342fbcdb1c30e6`.

User result:
- Screen Size and Aspect Ratio rows were visible;
- changing them did not reliably affect rendering;
- 2560x1440 would appear while cycling but reverted to 1920x1440 when Enter was pressed or the menu was reopened;
- user requested a dedicated **Apply** row so display changes can be committed without restarting the game.

Runtime evidence:
- all three original Phase 3B aspect call patches installed successfully;
- aspect cycling itself worked: log reached `16:9 scalar=0.750000`;
- committed output remained `1920x1440` throughout all `display_options` wrapper calls;
- presentation telemetry briefly observed saved globals at `2560x1440x32` while the user was cycling Screen Size, then later back at `1920x1440x32`;
- therefore retail Screen Size navigation was directly mutating the saved globals as a temporary menu variable, while the frontend display-options call restored our still-committed 1920x1440 selection.

Additional retail bug exposed by the Color Depth -> Aspect Ratio repurpose:
- after changing original Color Depth, retail calls `DXINIT_GetNextResolution` / `DXINIT_GetPrevResolution` to find a resolution compatible with the new bpp;
- those calls remained active after row 1 became Aspect Ratio, so aspect changes could silently mutate Screen Size;
- exact obsolete compatibility call sites:
  - `0x0050DDFB -> 0x00500E20`;
  - `0x0050DE1F -> 0x00500F40`.

Transactional menu design:
- add a fourth original CMenu entry: **Apply**;
- Screen Size and Aspect Ratio now edit separate pending values;
- pending Screen Size formatting no longer reads the committed/saved globals;
- pending resolution stepping still uses the original retail mode-table algorithms, but on local temporary width/height values;
- Aspect Ratio stepping modifies only pending aspect state;
- the old color-depth compatibility-resolution searches are disabled;
- pressing Enter on Screen Size / Aspect Ratio / Brightness no longer calls `DXINIT_SetDisplayOptions`;
- pressing Enter on **Apply** atomically:
  1. copies pending resolution/aspect to committed modern state;
  2. writes selected output to the retail saved width/height/bpp globals;
  3. writes the aspect projection scalar;
  4. applies the current frontend/device state safely;
  5. calls retail `SPIDEYDX_SaveSettings @ 0x00515850` immediately;
  6. resets pending state to the newly committed selection.
- Back/Escape without Apply leaves committed resolution/aspect unchanged.

Retail save verification:
- retained retail `SPIDEYDX_SaveSettings` bytes were disassembled;
- it serializes:
  - width from `0x02E096F8`;
  - height from `0x02E0970C`;
  - bpp from `0x02E098E4`;
  - brightness from `0x00562D60`;
- Apply therefore persists exactly the fields the original game saves to `Spidey.cfg`.

Exact new byte-verified call patches:
- `0x0050DA72 -> 0x0043FFF0`: intercept third AddEntry and append Apply;
- `0x0050DB56 -> 0x00529F90`: format pending Screen Size;
- `0x0050DCF8 -> 0x00500250`: Enter/confirm becomes Apply-only commit;
- `0x0050DDFB -> 0x00500E20`: disable aspect->resolution compatibility step;
- `0x0050DE1F -> 0x00500F40`: disable fallback aspect->resolution compatibility step;
- `0x0050DE71 -> 0x00500F40`: previous Screen Size operates on pending state;
- `0x0050DE88 -> 0x00500E20`: next Screen Size operates on pending state;
- existing row-1 formatter/prev/next patches remain byte-verified.

Additional fix:
- frontend-mode recognition no longer incorrectly requires brightness option value 4; changing brightness can no longer cause the 640x480 frontend request to be mistaken for gameplay resolution.

Implementation commits:
- `cdc6b5206e9aaed1c67a502b24f7cc0a657b82e2` — stage resolution/aspect and add Apply row;
- `080152a3d206063038fbe18f26d54f8b8d1edfe7` — Apply-only atomic commit, immediate retail save, and frontend-brightness classification fix.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. open Display Options and verify four rows:
   - Screen Size
   - Aspect Ratio
   - Brightness
   - Apply
3. set Screen Size to **2560x1440**;
4. set Aspect Ratio to **16:9**;
5. move to Apply and press Enter;
6. verify the menu remains usable and Screen Size still reads 2560x1440;
7. back out and reopen Display Options; it must still read 2560x1440 + 16:9;
8. start gameplay without restarting the process;
9. verify gameplay uses the selected modern resolution/aspect;
10. exit and provide logs.

Expected new log markers:
- `display_menu_mod ... rows=4 ... applyentry=1 applyconfirm=1`;
- `display_pending_reset reason=menu_open ...`;
- Screen Size cycling:
  `display_pending_resolution direction=... value=2560x1440 committed=1920x1440`;
- Aspect cycling:
  `display_pending_aspect ... value=16:9 committed=...`;
- Apply:
  `display_aspect reason=display_menu_apply_commit ...`;
  `display_apply committed=1 selected=2560x1440x32 aspect=16:9 ... saved_now=1`;
- reopening:
  `display_pending_reset reason=menu_open selected=2560x1440 aspect=16:9`;
- gameplay transition:
  `display_options selected=2560x1440x32 physical=1920x1440x32 ... legacy_backing_remap=1`;
  `logical_render_resolution ... logical=2560x1440 physical=1920x1440 selected=2560x1440`.


Phase 3C install-order correction:
- the Apply-specific `0x0050DCF8` hook must install before `SpideyInstallDisplayOptionsCompat`, because the generic installer scans and rewrites all remaining direct calls to `0x00500250`;
- install order is now:
  1. modern mode reinit compat;
  2. transactional Display Options / Apply hooks;
  3. generic display-options compat for all remaining retail call sites;
- expected `display_options_compat patched_calls` changes from 4 to **3** by design;
- correction commit: `016b7438fd7008d8e6075105c0b4f2e9201d5703`.


## Phase 3C pre-runtime build blocker — MSVC6 __thiscall typedef — 2026-09-30

User test of revision `2f14244f05ad2c1946a6668b89fe60064ca9b409` did not reach runtime.

Build result:
- forced clean matching build started normally;
- compile stopped at `main.cpp(1573)`;
- MSVC6 error: `C4234: nonstandard extension used : '__thiscall' keyword reserved for future use`;
- therefore no Phase 3C runtime conclusions can be drawn from this attempt.

Root cause:
- the new Apply-row code declared retail `CMenu::AddEntry @ 0x0043FFF0` through an explicit `__thiscall` function-pointer typedef;
- this matching compiler does not support spelling `__thiscall` there.

Compatibility fix:
- declare the raw retail function pointer as `__fastcall(CMenu*, void*, const char*)`;
- pass an unused dummy second parameter;
- this is ABI-compatible with the x86 retail member call for this target:
  - `CMenu* this` remains in ECX;
  - unused dummy occupies EDX;
  - the actual label argument remains on the stack;
  - both conventions use callee stack cleanup for the stacked argument.
- no retail address or Phase 3C menu logic changed.

Next action:
1. update/build again with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. if matching build succeeds, perform the existing four-row Display Options / 2560x1440 + 16:9 + Apply runtime test;
3. only then evaluate pending/committed settings behavior.


## Phase 3C runtime result — PASS; Phase 3D begins — 2026-09-30

Tested revision:
- `9fa0e6ab1027e259052fe0b8486f9e6ade57b0f4`.

User result:
- selected resolution and aspect ratio now apply in gameplay;
- requested 2560x1440 + 16:9 survives Apply and menu re-entry;
- gameplay visibly uses the requested modern output/aspect;
- new issues observed:
  1. level backgrounds/backdrops are strongly distorted, especially while the camera moves;
  2. main menu/frontend remains low-resolution 4:3 instead of following the selected output/aspect.

Runtime proof from `spidey-decomp-compat(20260930-195855).log`:
- every transactional Display Options patch installed, including `apply_entry=1` and `apply_confirm=1`;
- `display_apply committed=1 selected=2560x1440x32 aspect=16:9 ... saved_now=1`;
- reopen resets pending state from committed `2560x1440 + 16:9`;
- gameplay transition reports:
  `logical_render_resolution reason=display_options_gameplay modern=1 frontend=0 logical=2560x1440 physical=1920x1440 selected=2560x1440`.
- frontend transition still reports:
  `logical_render_resolution reason=display_options_frontend modern=0 frontend=1 logical=640x480 physical=640x480 selected=2560x1440`.

Phase 3C conclusion:
- transactional Screen Size / Aspect Ratio / Apply behavior is validated;
- 2560x1440 remains physically quarantined from the legacy D3D7 device as intended;
- do not reopen the old Apply/persistence issue unless new evidence regresses it.

Phase 3D current goals:
1. remove the intentional 640x480 frontend logical-resolution lock while preserving retail UI semantics and stability;
2. diagnose/fix moving gameplay backdrop/background distortion separately from general widescreen output.

Initial backdrop evidence:
- gameplay DX11 shadow draws are active at logical 2560x1440 with physical D3D7 backing 1920x1440;
- frame telemetry includes transformed vertices with extreme screen-coordinate ranges (millions), while normal frontend pre-transformed geometry stays around the original 640x480 coordinate space;
- investigate special transformed/backdrop geometry and RHW/viewport conversion before changing projection globally.


### Phase 3D implementation frontier — modern frontend + TL clipping correction — 2026-09-30

Source commits:
- `653680d8bb6e4661964cfdb83a90bdc4f85737a6` — preserve D3D7 transformed-vertex screen-space clipping in renderer11;
- `f74f2ee31e85cfe291bd8dbaa95f9c687c340335` — drive frontend logical resolution / DX11 replay target from selected modern output.

#### Background/backdrop distortion correction

Observed geometry is D3D7 FVF 0x144 / XYZRHW: X/Y/Z are already transformed screen-space values and RHW is carried for perspective interpolation.

Previous renderer11 conversion:
- computed NDC from X/Y;
- reconstructed clip W as `1 / RHW`;
- multiplied X/Y/Z by that clip W;
- let DX11 clip in varying homogeneous W.

That is not faithful to already-transformed TL geometry when very large off-screen triangles cross viewport boundaries. Gameplay telemetry contains exactly those extreme transformed coordinates, and the visual symptom is moving/warping backgrounds.

New conversion:
- keep output clip position in screen-space-derived NDC with fixed shader `W = 1`;
- store source RHW in the input position.w payload only;
- perform screen-space clipping with constant W;
- preserve RHW texture perspective explicitly by interpolating `uv * rhw` and `rhw`, then dividing in the pixel shader;
- depth remains source post-transform Z;
- F10 D3D7 reference path remains available.

Expected renderer marker:
`shadow pipeline ready shader_model=4_0 tl_vertex=screen_space manual_uv_perspective=1`.

#### Modern frontend/menu resolution

The frontend still requests a safe legacy 640x480x16 D3D7 device. That physical compatibility backing is intentionally retained.

What changes:
- selected modern output is now allowed in frontend as the game's logical render resolution;
- the shell/global logical width/height can therefore be 2560x1440 while the hidden legacy frontend device remains 640x480;
- DX11 shadow capture/replay viewport and target use the selected modern output in frontend as well as gameplay;
- F10 reference mode still calls the physical-resolution logical path.

This deliberately separates:
- **legacy physical backing**: 640x480 frontend / 1920x1440 backing for selected 2560x1440 gameplay;
- **modern logical + visible DX11 output**: selected resolution/aspect in both frontend and gameplay.

Expected frontend markers after selecting 2560x1440:
- `logical_render_resolution reason=display_options_frontend modern=1 frontend=1 logical=2560x1440 physical=640x480 selected=2560x1440`;
- draw frames in frontend: `modern=1 logical=2560x1440 physical=640x480`;
- renderer11 shadow target remains `2560x1440` across gameplay -> frontend transitions.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. confirm build succeeds and renderer log contains `tl_vertex=screen_space manual_uv_perspective=1`;
3. confirm Display Options still show/persist 2560x1440 + 16:9;
4. inspect the main menu before gameplay and after returning from gameplay:
   - it should now use the selected modern resolution/aspect rather than a 640x480 4:3 render;
   - check menu artwork, text, cursor/mouse hit-testing, and Options navigation;
5. enter a level and specifically watch distant level backgrounds/backdrops while moving and rotating the camera;
6. verify foreground geometry, HUD, transparency, and texture perspective remain stable;
7. press F10 only if an A/B reference is useful; DX11 should be default and D3D7 should still provide the old reference;
8. exit and provide full logs plus screenshots of main menu and a gameplay scene if distortion remains.

Do not remove the 640x480 frontend D3D7 backing yet. This test is about modern logical/DX11 frontend ownership while retaining the safe compatibility device.


## Phase 3D runtime result — visual pass; live-apply/input follow-ups — 2026-09-30

Tested revision:
- `e836711981744db3916b6dad636f88879ead0bfb`.

User result:
- modern frontend/main-menu rendering is a major visual improvement;
- the transformed-vertex clipping correction makes gameplay backgrounds/backdrops look substantially better;
- two remaining UX issues:
  1. changing Display Options and pressing Apply commits/saves the selection, but the visible running scene does not always reflect the new setting until restart;
  2. after leaving gameplay and returning to the main menu, the mouse cursor still moves but mouse clicking/hover selection no longer works correctly.

Runtime proof:
- renderer11 initializes at 2560x1440 and the new pipeline marker is active:
  `tl_vertex=screen_space manual_uv_perspective=1`;
- frontend is now modern logical 2560x1440 while retaining the safe hidden 640x480 D3D7 backing;
- Apply commits are recorded for both 4:3 and 16:9 in the same run, including `saved_now=1`;
- gameplay is modern logical 2560x1440 over the 1920x1440 legacy compatibility backing;
- steady-state observed main-scene frames are entirely the expected triangle-fan / FVF 0x144 subset and submit all observed draws to renderer11 with `shadow_skip=0`;
- transient texture misses occur during resource/mode transitions but settle to zero after mirroring.

Interpretation:
- persistence/transaction logic is working;
- remaining Apply issue is **live presentation/layout activation**, not failure to save the setting;
- mouse device is still live (cursor movement continues and input log shows successful foreground acquisition); likely failure is stale frontend mouse coordinate/bounds state across gameplay -> frontend logical-resolution transition.

### DX11 migration status after Phase 3D

Current validated position:
- DX11 owns the visible modern output path;
- DX11 mirrors retail textures and fixed-function state;
- DX11 replays effectively 100% of the observed current main-scene primitive stream in sampled frontend/gameplay frames;
- modern 2560x1440 logical rendering and selected aspect behavior are operational;
- transformed TL-vertex clipping/perspective semantics are now much closer to retail D3D7 and visually validated.

Still legacy-dependent:
- retail D3D7 device/surfaces still exist as compatibility/source infrastructure;
- retail D3D7 still produces the transformed primitive/state stream that renderer11 mirrors/replays;
- some offscreen/resource/movie/device-lifecycle responsibilities still pass through legacy DirectDraw/D3D7 infrastructure;
- F10 intentionally preserves a D3D7 reference path for A/B diagnosis.

Next renderer milestone is therefore not basic draw coverage; it is **removing D3D7 as the producer/dependency beneath the already-working DX11 visible path**.

### New major modernization goals accepted

#### Modern controller layer
Target:
- complete modern gamepad support;
- left-stick movement;
- right-stick camera;
- triggers/bumpers/start/back/stick buttons;
- deadzones, sensitivity, inversion;
- rumble;
- controller menu navigation;
- persistent per-action remapping;
- dynamic button-prompt UI/glyphs based on last active input device;
- retain keyboard/mouse interoperability.

Existing game already has useful foundations:
- controller action-mapping tables and setters/getters;
- controller polling and button-state paths;
- legacy controller configuration/menu concepts;
- force-feedback entry points.

Plan:
- add a normalized modern controller state layer rather than exposing raw legacy DirectInput joystick assumptions directly to gameplay;
- feed that state into the existing action mapping where appropriate;
- keep a dedicated right-stick axis pair available for modern camera control.

#### Modern mouse/right-stick camera
Target:
- user-controlled third-person camera during normal gameplay;
- mouse and right-stick yaw/pitch;
- Spider-Man movement no longer forcibly dictates camera heading;
- preserve scripted, boss, cutscene, fixed, special traversal, and camera-collision behavior;
- optional configurable recenter behavior rather than mandatory continuous recentering.

Existing game camera has strong reusable foundations:
- camera modes include NORMAL, LOOSE, USER, LOOKAROUND and others;
- camera angle/distance/offset setters already exist;
- player `PutCameraBehind` is a concrete current recenter path that follows Spider-Man heading;
- lookaround setup/exit machinery and camera-angle locks already exist in retail.

Recommended architecture:
1. finish current live Apply + frontend mouse-state fixes;
2. stabilize the DX11/D3D7 lifecycle boundary;
3. create normalized modern input/controller layer;
4. add mouse/right-stick free-look as an overlay on existing camera state, suppressing automatic `PutCameraBehind` recenter only in normal user-controlled gameplay;
5. add button glyphs/remapping UI after normalized action state is stable.

Immediate next work:
- make Apply activate selected output/aspect in the current running frontend/gameplay without restart;
- synchronize frontend mouse coordinate/bounds state whenever legacy physical backing and modern logical output diverge or transition.


### Phase 3D follow-up implementation — live Apply + frontend mouse transition — 2026-09-30

Implementation commits:
- `e6797491c77a83842dee5c70d2e77b8d2ed5653e` — resynchronize retail frontend mouse bounds after display/mode transitions;
- `70efef29479e6fc66cb42e54ec6001992bca425f` — make selected Aspect Ratio define the live non-stretched DX11 content canvas inside the selected Screen Size.

#### Retail mouse RE / fix

Original-symbol database and retained retail function bytes identify:
- `PCINPUT_SetMouseBounds @ 0x0050A6B0`;
- `PCINPUT_SetMousePosition @ 0x0050A700`;
- `PCINPUT_GetMousePosition @ 0x0050A750`;
- `PCINPUT_IsMouseOver @ 0x0050A820`;
- `PCINPUT_UpdateMouse @ 0x0050A8A0`;
- `PCSHELL_Initialize @ 0x0050C010`;
- `PCSHELL_IsMouseOver @ 0x0050C5F0`.

Retained retail disassembly proves:
- `PCSHELL_Initialize` establishes mouse bounds from live legacy dimensions `0x006B78E4/0x006B78E8`, minus 32 pixels;
- that bounds initialization occurs only when the shell cursor sprite is first created;
- `PCINPUT_SetMouseBounds` stores the four clamp limits at retail globals `0x00AC0924/2C/28/30`;
- `PCINPUT_UpdateMouse` clamps mouse X/Y against those stored bounds;
- `PCINPUT_IsMouseOver` uses current mouse X/Y and the current resolution-dependent hotspot calculation.

Therefore gameplay -> frontend can leave gameplay-sized mouse clamp bounds alive after the frontend physical D3D7 canvas returns to 640x480.

New behavior:
- every frontend display transition explicitly calls the retail mouse-bounds API using the current frontend physical canvas;
- current mouse position is preserved when already valid;
- if the old gameplay position lies outside the new frontend domain it is safely recentered;
- input log marker:
  `retail_input event=frontend_bounds_sync ...`.

#### Live Aspect / Apply behavior

Apply was already committing and persisting state correctly. The latest test proved both 4:3 and 16:9 commits in one run, but the frontend had no immediate visible aspect treatment because Screen Size remained the full DX11 canvas.

New model:
- **Screen Size** = selected output/swap-chain extent;
- **Aspect Ratio** = largest non-stretched logical content canvas that fits inside that output;
- AUTO = use the full selected output aspect;
- explicit aspect modes aspect-fit inside the selected output.

Examples at 2560x1440:
- 16:9 -> 2560x1440 content;
- 4:3 -> 1920x1440 content, centered/pillarboxed by the existing aspect-preserving DX11 presenter;
- 5:4 -> 1800x1440 content;
- 16:10 -> 2304x1440 content;
- 21:9 -> 2560x1097 content;
- 32:9 -> 2560x720 content.

The retail projection scalar remains applied as before. The logical content canvas is refreshed immediately inside the existing Apply path, so no restart should be required.

Expected compat marker now includes both output and live content:
`logical_render_resolution ... selected=<output> content=<aspect-fit-content> aspect=<mode>`.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. in Display Options at 2560x1440:
   - switch 16:9 -> 4:3 and press Apply;
   - verify the running frontend changes immediately to centered 4:3/pillarboxed content without restart;
   - switch back to 16:9 and Apply;
   - verify it immediately returns to full 2560x1440;
3. verify the Display Options values remain committed after closing/reopening;
4. enter gameplay, then back out to main menu;
5. move and click the mouse across several main-menu items;
6. verify input log contains `frontend_bounds_sync` after the mode transition;
7. enter gameplay once more to ensure the mouse fix did not disturb keyboard/gameplay input;
8. provide the full new log set; screenshots are useful if any aspect mode is stretched or miscentered.


### Phase 3E prepared — guarded removal of D3D7 main-scene DrawPrimitive — 2026-09-30

Implementation:
- `502bd2864c5e6fd8a1e268f18656591a8b709d0c` — opt-in F9 D3D7 main-scene draw suppression trial.

Purpose:
- distinguish "DX11 can replay everything visibly" from "the retail D3D7 color/depth main-scene draw is still required for hidden side effects";
- prove the next renderer boundary without deleting the fallback.

Behavior:
- default OFF, so the existing Apply/mouse regression test is unchanged;
- F9 toggles only the diagnostic suppression flag;
- a retail main-scene `DrawPrimitive` is suppressed only when ALL are true:
  - DX11 geometry mode is enabled;
  - DX11 has completed its warmup and is the ready visible path;
  - the draw targets the retail main scene, not an offscreen surface;
  - the exact draw was accepted by the DX11 triangle-fan replay path;
  - the draw has no texture or its texture is already mirrored/resident;
- unsupported state, unresolved transient texture, offscreen draw, warmup frame, and F10 reference mode all fall back to the original D3D7 call;
- F10 therefore remains a complete D3D7 reference even if F9 suppression is armed.

New telemetry:
- present log:
  `d3d7_main_draw_suppression frame=... enabled=... effective=... dx11=... ready=... key=F9`;
- draw-frame log adds:
  - `d3d7_suppress=<0|1>`;
  - `d3d7_suppressed=<count>`;
  - `d3d7_fallback=<count>`.

Interpretation for a future F9 test:
- if image/gameplay remains unchanged while `d3d7_suppressed` approaches main-scene draw count and fallback stays near zero in steady state, D3D7 main-scene rasterization is no longer functionally required for the visible path;
- any missing effects/readback-dependent behavior means those specific dependencies must be identified before permanent suppression;
- do NOT suppress offscreen D3D7 work yet because transient textures/resource generation may depend on it.

This diagnostic is intentionally layered behind the current Phase 3D user-facing fixes. First validate live Apply + mouse return; then use F9 as the next renderer-isolation experiment.

### Camera modernization note — do not over-commit to legacy camera internals

User explicitly wants the option to modernize the camera beyond the feel/limitations of the original system after trying an initial implementation.

Therefore:
- existing camera modes/helpers are useful RE anchors and may provide collision/script transition knowledge;
- they are **not** an architectural requirement for the final modern camera;
- design the modern input/camera boundary so a later dedicated camera controller can own yaw, pitch, distance, smoothing, collision and recenter policy directly while only yielding to explicit scripted/cinematic camera ownership.

Useful retail camera hook anchors from the original symbol database:
- `CCamera::PushMode = 0x00416720`;
- `CCamera::PopMode = 0x00416780`;
- `CCamera::SetCamAngle = 0x004178E0`;
- `CCamera::SetCamXZDistance = 0x004179F0`;
- `CCamera::SetCamYDistance = 0x00417A70`;
- `CPlayer::SetCamAngleLock = 0x004B9E10`;
- `CPlayer::EnterLookaroundMode = 0x004C3580`;
- `CPlayer::ExitLookaroundMode = 0x004C3810`;
- `CPlayer::SetupLookaroundCamera = 0x004C38A0`;
- `CPlayer::PutCameraBehind = 0x004C64A0`.

Recommended long-term camera split:
1. normalized mouse/right-stick input produces camera intent;
2. modern camera controller owns ordinary gameplay camera transform;
3. retail scripted/boss/cutscene state can temporarily claim camera ownership;
4. on return to ordinary gameplay, modern camera resumes without being forced behind Spider-Man;
5. legacy collision/lookaround code may be reused selectively or replaced entirely based on feel/testing.


#### Phase 3E static scene-dependency audit

Additional static checks before runtime F9 use:
- `PCMovie.cpp` copies decoded movie frames into `g_pDDS_Scene` with DirectDraw `Blt`; F9 only intercepts D3D7 `DrawPrimitive`, so the movie copy is outside the suppression scope;
- retained retail `DXPOLY_SaveScreen @ 0x005033E0` was disassembled from the original function bytes and references the primary surface `0x006B7904`, not the scene surface `0x006B7908`;
- D3D7 offscreen render-target draws are never suppressed.

This does not prove there are no obscure same-frame scene-surface consumers, but it removes two obvious risks and keeps the trial reversible/fail-closed. Runtime F9 testing remains the authority before making D3D7 main-scene suppression permanent.


### Modern input/camera architecture documented

New design document:
- `docs/MODERN_INPUT_CAMERA.md`
- commit `811e5713b4afd8c5d5f62f3324ad75202e29ffec`.

Key compatibility seam:
- `PCINPUT_GetMappedStates @ 0x0050A190` is the preferred initial digital-action injection point;
- `Pad_Update @ 0x00505720` remains the downstream legacy action consumer;
- right-stick/mouse camera intent must stay separate from the legacy digital mask.

The document explicitly preserves the option to replace ordinary legacy camera ownership with a dedicated modern gameplay camera after evaluating the first free-look prototype.


## Modern Input Phase 0 implementation begins — passive helper foundation — 2026-09-30

New source:
- `input11/CMakeLists.txt`;
- `input11/include/spidey_input11_api.h`;
- `input11/spidey_input11.def`;
- `input11/src/spidey_input11.cpp`;
- `scripts/build_input11.ps1`.

Initial commits:
- `22bc97d441a12cf4fd9a7d1f51e7142fc6e410d7`;
- `21cffdd9262d176ca8e7eef493d4d7a2f9553554`;
- `254349907ce351ed05b1cf4e886d9aee34f52065`;
- `9686366d3fd65369918d84c8d422b60ab45e7806`;
- `e50cb442fdda4a006fe79a6e28c9dafcd76867f3`.

Architecture:
- modern input lives in a separate VS2022 Win32 helper DLL, matching the successful renderer11 split;
- proxy compatibility layer will communicate through a versioned C ABI;
- ABI v1 exposes one normalized controller snapshot:
  - connected/device family/user index/packet number;
  - semantic button bitset;
  - normalized `moveX/moveY`;
  - normalized `cameraX/cameraY`;
  - normalized left/right triggers;
- rumble API is included from the start.

First provider:
- dynamic XInput loader;
- probes `xinput1_4.dll`, `xinput1_3.dll`, `xinput9_1_0.dll`, `xinput1_2.dll`, `xinput1_1.dll` in order;
- no XInput import library dependency is added to the matching proxy;
- standard radial left/right-stick deadzones and trigger threshold are normalized in the helper;
- up to four XInput users are scanned, preserving the current active user while connected.

Important scope:
- this first provider proves the modern input ABI with Xbox/XInput-compatible pads;
- PlayStation/native HID/generic-pad support is intentionally **not** encoded as XInput assumptions in the ABI and can be added as later providers;
- modern input is still passive and does not replace retail gameplay controls yet.

Next implementation steps:
1. add helper build/install/log collection to `TEST_LATEST_BUILD.ps1`;
2. load/probe/poll `spidey_input11.dll` from the proxy without injecting actions yet;
3. validate helper telemetry with and without a controller connected;
4. only after passive validation, merge semantic controller actions into `PCINPUT_GetMappedStates @ 0x0050A190`.


### Modern Input Phase 0 — passive bridge integrated — 2026-09-30

Implementation now includes:
- legacy-safe `input11_legacy_bridge.h`;
- proxy-side dynamic loading/probing of `spidey_input11.dll`;
- ABI v1 validation;
- per-frame passive polling from the completed-frame path;
- no gameplay action injection yet;
- connection/state telemetry in `spidey-decomp-input.log`;
- helper-provider lifecycle log in `spidey-input11.log`;
- standard updater/test workflow builds, installs and collects the helper automatically;
- 32-bit `spidey_input11_probe.exe` preflight runs before the game launches.

Important commits:
- `44cbb396a1951a995d23e134012d6c0b339a1415` — legacy-safe bridge header;
- `0dcc44185aa4dacb23246efe6a8e86b0f11cdbb8` — proxy passive load/poll integration;
- `9c46347e3b5db7f981f59f4015dfc475a0c2ed75` — build/install/log collection;
- `9bb457f7ba3ac53a07991e73a709a84a3cdc3b91` — 32-bit preflight source;
- `67eb23e911e2e864ac8e99e255704a4bf01b4ed8` — build probe target;
- `04f0110c820896974173b26b02ab9b429f27d4b6` — export probe artifact;
- `84d18970a2d01999682a41308763080fc2d6d057` — run preflight before game;
- `af0beffbd61db96eb44e726882a97da80f2ec00e` — isolate preflight working directory.

Expected passive runtime markers:
- updater console:
  `abi=1 expected=1 backend=spidey_input11/xinput-dynamic probe=1`;
- proxy input log:
  `input11_bridge loaded ... abi=1 expected=1 ... passive=1`;
- no controller:
  `input11_state ... connected=0 ... passive=1`;
- connected XInput pad:
  `input11_state ... connected=1 family=1 user=<n> ... move=... camera=... triggers=... passive=1`;
- helper log:
  `backend loaded provider=xinput dll=<chosen xinput dll> abi=1`;
  plus controller connect/disconnect transitions.

Safety:
- this phase cannot alter player movement/buttons/camera because the normalized state is observation-only;
- retail DirectInput remains authoritative until passive runtime proof is received.

Provider roadmap note:
- current XInput provider is intentionally dependency-free and limited to the Phase 0 proof;
- broad PlayStation/generic gamepad support remains provider-pluggable behind the same ABI.


### Retail PC analogue-input RE result — digitalized joystick confirmed

Original retail `Pad_Update @ 0x00505720` and `PCINPUT_GetMappedStates @ 0x0050A190` were disassembled from retained retail function bytes.

Confirmed retail PC behavior:
- `Pad_Update` calls `PCINPUT_GetMappedStates`, updates digital `SButton` fields, and expires vibration;
- it does **not** populate the `SControl` raw/processed analogue movement or aim fields;
- `PCINPUT_GetMappedStates` calls `DXINPUT_PollController(&gControllerX, &gControllerY, &gControllerAxesRelatedTwo)`;
- controller X/Y magnitude is reduced to four digital direction bits:
  - X < -250 -> left bit 0x4;
  - X > +250 -> right bit 0x8;
  - Y < -250 -> up bit 0x1;
  - Y > +250 -> down bit 0x2;
- POV/hat data is likewise reduced to those same directional bits;
- controller button mappings are then OR'd into the action masks.

Conclusion:
- the retail PC controller path is fundamentally **digitalized**, despite the inherited console-era `SControl` analogue fields;
- modern left-stick magnitude and right-stick camera should not be forced through the old PC joystick-direction path;
- the new normalized helper should feed true analogue channels directly into a modern movement/camera layer while optionally producing the legacy action mask for compatibility.

Passive telemetry commit:
- `8de9028d3ba6cba8db164956c813948396a365bb` logs the legacy raw/processed analogue fields beside modern helper state so the next runtime can verify whether they remain zero/unused in actual PC play.

This materially reduces risk for the modern camera plan: right-stick camera is not replacing a hidden working PC right-stick system; it is adding one that the retail PC path does not provide.


### Passive camera ownership telemetry + gameplay-camera RE — 2026-09-30

Implementation:
- `2d5dc86b59e4a738e831fe88e8efdf1dba9937e4` — passive active-camera/mode/transform telemetry;
- `4cd7bf549696c76d8ad7176c6e3682c4ce9f4516` — automatic camera-log collection in the standard test session.

Telemetry is observation-only:
- retail active-camera pointer is read from `0x0056F3B8`, the same pointer used by retail `CPlayer::PutCameraBehind`;
- no camera fields/functions are modified or called by the telemetry path;
- `spidey-decomp-camera.log` records:
  - active camera pointer;
  - camera mode number + enum name;
  - pushed/previous mode;
  - `field_236` heading and `field_23A` transform-derived heading;
  - camera position;
  - focus/tripod target;
  - XZ/Y camera distances;
  - zoom;
  - collision-ray IDs;
  - modern-input connection state and normalized right-stick `cameraX/cameraY`;
- logs on camera-pointer changes, camera-mode changes, and periodically;
- with a modern controller connected periodic sampling increases to every 60 completed frames so right-stick intent can be correlated with unchanged retail camera behavior.

#### Important retail camera naming correction

Static disassembly proves `CAMERAMODE_DEMO == 3` is not merely a demo/cutscene camera in this PC build. It is the baseline mode used by ordinary player gameplay camera presets.

Evidence:
- `CPlayer::SetFallingCamera @ 0x004BF5D0`;
- `CPlayer::SetSwingCamera @ 0x004BF690`;
- `CPlayer::SetFloorCamera @ 0x004BF720`;
- `CPlayer::SetWallCamera @ 0x004BF7A0`;
- `CPlayer::SetCeilingCamera @ 0x004BF820`.

Every one:
1. loads active camera from `0x0056F3B8`;
2. only applies its normal movement-state camera preset when `camera->mCameraMode == 3`;
3. drives existing camera offset/distance setters;
4. writes a player camera-preset/state ID at player + `0x540`:
   - floor = 0;
   - wall = 1;
   - ceiling = 2;
   - swing = 4;
   - falling = 5.

Therefore mode 3 should be treated as the leading **ordinary gameplay ownership** candidate for a future modern-camera controller, despite the legacy enum name `DEMO`.

#### PutCameraBehind — confirmed recenter mechanism

Retail `CPlayer::PutCameraBehind @ 0x004C64A0`:
- reads the same active camera pointer at `0x0056F3B8`;
- in ordinary non-crawl flow computes the player's effective heading and calls `CCamera::SetCamAngle`;
- in crawl/oriented-surface flow derives an angle from the current surface/orientation and calls `SetCamAngle`;
- when the camera is mode 3 it also adjusts Y-distance/angle behavior for certain crawl/surface states.

This is the concrete forced-recenter path the first modern free-look prototype will need to gate/suppress during modern ordinary-gameplay ownership.

#### CCamera::AI / CM_Normal static notes

Original `CCamera::AI @ 0x00417CB0` bytes were independently disassembled:
- updates global angle/distance/offset interpolation;
- updates tripod/focus state;
- dispatches mode-specific camera behavior;
- applies collision/orientation/shake processing;
- ends by loading the result into the engine camera through `CCamera::LoadIntoMikeCamera`.

The dispatch for modes 3..17 uses a compact lookup table immediately after the function body; the retained per-function binary archive does not include those adjacent table bytes. The exact lookup table has **not** been guessed. Runtime mode telemetry plus known explicit mode functions will be used to ground ownership before modifying dispatch.

Current safe architectural conclusion:
- modern ordinary gameplay camera can initially claim mode 3;
- other modes remain retail-owned until runtime telemetry or specific RE proves they are safe to absorb;
- this keeps scripted/boss/special cameras intact while allowing a later full modern camera for normal play.


### Passive action-map + raw mouse intent telemetry — 2026-09-30

Implementation:
- `69829c9ea9639ef9792b2c2a07b2f3f797d11d08` — one-time read-only dump of the retail controller/action descriptor table;
- `e0f1993de6d7db714c5a2a091bae07788232b7bc` — mirror raw relative mouse deltas from the existing DirectInput compatibility wrapper;
- `dfbb8bec595a54d25d496b7cb2190d17397f2393` — static-audit fix: move shared raw-mouse telemetry declarations before first use in the VC6-era translation unit;
- `201fc404605746f2fe5811f692d1c39b7fa6bcc6` — throttle activity-driven camera telemetry so short mouse/right-stick input is captured without logging every frame.

#### Retail action descriptor table

Original controller-menu code/disassembly establishes:
- table base: `0x00568690`;
- count: 11;
- stride: `0x1C`;
- +0x00: action bit/mask;
- +0x04: inline 16-byte retail action label;
- +0x14: keyboard mapping;
- +0x18: controller mapping.

The first four controller-menu rows correspond to movement/direction and are disabled for joystick-button remapping; later rows are button-remappable.

The passive bridge now dumps the table once:
`retail_action_map index=<n> action=0x.... label=<retail text> keyboard=... controller=... passive=1`.

This deliberately uses the game's own runtime labels rather than guessing semantic names for action bits.

#### Raw relative mouse path

Retail machine-code RE:
- `DXINPUT_PollMouse @ 0x00501CC0` consumes buffered DirectInput mouse events and accumulates relative X/Y deltas before any absolute cursor integration;
- `PCINPUT_UpdateMouse @ 0x0050A8A0` later scales/integrates those deltas into the shell cursor and clamps them to mouse bounds.

The project already owns every direct call to `DXINPUT_PollMouse` through `SpideyCompatRetailPollMouse` for Alt+Tab recovery.

New passive behavior:
- call retail PollMouse unchanged;
- on successful polls, mirror/accumulate the returned relative X/Y deltas;
- once per completed frame, camera telemetry snapshots and clears those accumulators;
- `spidey-decomp-camera.log` now includes:
  - `input_mouse=<dx>,<dy>`;
  - `mouse_polls=<count>`;
  - existing `input_camera=<right-stick-x>,<right-stick-y>`.

Activity sampling:
- mode/camera changes still log immediately;
- periodic samples remain;
- non-zero mouse or right-stick intent adds a throttled `event=input_intent` sample at most every 15 frames.

This gives Stage-A camera work one unified evidence stream for mouse and right-stick intent without adding another mouse-capture subsystem.

#### SetCamAngle ownership clue

Original `CCamera::SetCamAngle @ 0x004178E0` bytes explicitly compare `mCameraMode` and skip angle changes when mode is:
- 15 = LOOSE;
- 16 = USER;
- 17 = LOOKAROUND.

Because `CPlayer::PutCameraBehind` recenters through `SetCamAngle`, this suggests a potentially useful Stage-A compatibility mechanism: an appropriate user-controlled camera mode can naturally reject legacy recenter requests.

Do **not** switch modes based on this fact alone. The behavior of the mode-specific AI/dispatch must still be validated at runtime. The final modern camera remains free to replace ordinary legacy camera ownership entirely.

#### Static audit result

Before runtime handoff:
- raw mouse telemetry declarations now occur before their camera use;
- exactly one action-map logger, one passive camera sampler and one raw-mouse state set are present;
- new proxy code contains no C++11-only `auto`, `nullptr`, or lambda syntax;
- telemetry varargs were mechanically checked:
  - camera state: 25 specifiers / 25 data arguments;
  - input11 state: 21 / 21;
  - retail action map: 5 / 5;
- input helper remains C++17 only inside the separate VS2022 Win32 DLL;
- XInput is dynamically loaded; the old proxy does not link against XInput;
- the 32-bit preflight, helper install, helper log capture and camera log capture are all wired into the standard test harness.

No gameplay input or camera ownership is changed by any of these passive additions.


### Combined passive runtime boundary prepared

Static work is intentionally stopping before action injection or camera mutation.

Reason:
- modern input helper/build/preflight is wired;
- retail semantic action table will self-report at runtime;
- legacy analogue fields will be observed beside true modern stick magnitude;
- raw relative mouse and right-stick intent are now correlated with passive camera mode/transform state;
- live Apply and frontend mouse-return fixes are still awaiting runtime confirmation;
- Phase 3E F9 suppression is ready but remains opt-in/default-off.

The retained `CCamera::AI` binary was checked for the mode-dispatch tables referenced at `0x0041866C` and `0x0041868C`. The archived function blob ends exactly at `0x0041866C`; the table data is not present elsewhere found in the repo. No table contents were inferred or fabricated.

Next authoritative step is the combined runtime test documented at the top of `docs/NEW_CHAT_HANDOFF.md`. Do not enable modern controller action injection or camera ownership until the passive logs validate these boundaries.


## Priority shift from user runtime observations — 2026-09-30

User explicitly reprioritized current work ahead of the optional F9 renderer-isolation test.

Current priority order:

1. **Proper widescreen**
   - current implementation visibly stretches a 4:3 presentation to 16:9;
   - this is not acceptable as the final widescreen implementation;
   - target is true widescreen/Hor+ gameplay geometry with correct projection and non-stretched 2D/HUD behavior.

2. **Frontend/menu mouse hit-testing**
   - cursor motion itself is responsive;
   - hover/selection regions do not line up reliably with the visible menu after returning from a level;
   - user often has to place the pointer slightly above the visible option to select/click it;
   - treat this as a coordinate-transform / bounds-sync issue, not a mouse-acquisition issue unless new evidence proves otherwise.

3. **High-frame-rate simulation speed**
   - at higher render frame rates the game feels sped up;
   - renderer isolation/F9 is deprioritized because DX11 alone is not expected to fix simulation speed;
   - must identify the simulation/game-timer cadence and decouple gameplay/physics from render rate;
   - compare two design families:
     a. fixed 30 Hz simulation with interpolation for rendering;
     b. delta-time / fixed-step accumulator conversion with rate-independent gameplay;
   - do not choose until original timer/update semantics are grounded.

4. **Audio output modernization**
   - current game sound is effectively stuck on one headset/output device;
   - default behavior must become Windows **system default** output;
   - add an Audio menu that lists output devices and includes `System Default` as the default selection;
   - manual device selection should persist.

5. **Uncap main-menu framerate**
   - current main menu is 30 FPS;
   - user wants it uncapped;
   - this must not reintroduce sped-up menu animation/input timing, so frontend pacing and simulation timing need to be separated.

Renderer isolation:
- F9 D3D7-main-draw suppression test is postponed until these priorities are addressed or until renderer-isolation evidence becomes directly useful to one of them.

Immediate investigation plan:
- trace widescreen projection/FOV and 2D scaling separately;
- trace shell mouse coordinate conversion/hit-test bounds, especially gameplay -> frontend transition;
- trace frame timer/update loop and identify where 30 Hz assumptions enter simulation;
- trace DirectSound device creation and existing sound options/menu;
- trace frontend 30 FPS limiter and determine whether it shares the same timer path as gameplay.


### Frontend mouse hover/click coordinate bug — root cause fixed — 2026-09-30

User symptom:
- cursor motion itself is responsive;
- after returning from gameplay, visible menu items often do not highlight/click at the visible cursor position;
- moving the cursor slightly **above** the intended option makes selection work.

Static retail/source RE found an exact coordinate-space mismatch:

`PCSHELL_IsMouseOver` converts shell/DC hit rectangles into PC pixels using:
- live DX width `gDxResolutionX @ 0x006B78E4`;
- live DX height `gDxResolutionY @ 0x006B78E8`.

But retail:
- `PCINPUT_IsMouseOver @ 0x0050A820`;
- `PCINPUT_GetMouseHotspotPosition @ 0x0050A770`

scale the cursor hotspot using:
- gameplay logical width `0x00568154`;
- gameplay logical height `0x00568158`.

After gameplay at 2560x1440, the frontend can be back on its proven 640x480 canvas while the gameplay logical dimensions remain 2560x1440. A nominal 15-pixel Y hotspot therefore becomes:

`15 * 1440 / 480 = 45`

instead of 15 pixels.

That moves the effective hit-test point about 30 pixels below the visible cursor, which directly explains why hovering above an item can select it.

Fix commit:
- `4381062061b7a625c036dff7d851a3fac9430818` — `input: unify menu mouse hotspot coordinate space`.

Implementation:
- replaces retail `PCINPUT_IsMouseOver @ 0x0050A820`;
- replaces retail `PCINPUT_GetMouseHotspotPosition @ 0x0050A770`;
- hotspot scaling now uses the same live DX canvas dimensions as `PCSHELL_CoordsDCtoPC`;
- preserves retail strict `>` / `<` hitbox semantics;
- preserves cursor movement/acquisition behavior;
- byte-verifies the expected retail entry bytes before installing either replacement;
- logs:
  `mouse_coordinate_compat mouse_over=1 hotspot=1 ... basis=live_dx_canvas`.

This is intentionally separate from the existing frontend bounds/reacquire fix: bounds control where the cursor may move; this patch fixes where the shell believes the cursor's clickable hotspot actually is.


### Timing / main-menu cap RE — fixed-step evidence — 2026-09-30

Retail timing is now grounded enough to reject a blind global delta-time conversion.

#### Engine vblank clock

`PCTIMER_Init` creates a multimedia timer (nominal 16 ms) and converts real milliseconds into a 60 Hz virtual-vblank clock:
`gTimerMsInterval = timer_ms * 60 / 1000`.

The timer callback advances `gTimerVblankRelated` and calls `MyVSync` until retail `Vblanks @ 0x006B4CA0` catches up.

Retail `Pause @ 0x004E5D60` is confirmed machine-code busy-wait:
- target = `Vblanks + Time`;
- spin until `Vblanks >= target`.

#### Gameplay cadence

`PlayAway @ 0x004559D0` contains the normal gameplay loop.

Per loop:
1. snapshot `Vblanks`;
2. run `Logic @ 0x00455400`;
3. run display/render work;
4. if the current `Vblanks` still equals the snapshot, call `Pause(1)`;
5. begin the next gameplay loop.

Therefore normal retail gameplay is explicitly constrained to **at most one simulation/update pass per 60 Hz engine vblank**.

`Logic` also increments multiple gameplay/frame counters once per call, reinforcing that large parts of the game are authored as fixed-step/per-update code.

Current implication:
- do not retrofit global delta-time multipliers through gameplay/physics as the first modernization;
- first measure whether modern runtime is actually executing Logic faster than the intended 60 Hz cadence;
- preferred architecture is fixed-step simulation at the retail-authored cadence plus independent rendering/interpolation, rather than changing thousands of fixed-point/per-frame constants.

#### Main menu 30 FPS cap — exact mechanism

`Shell_MainMenu @ 0x00493990` has a distinct pacing sequence near `0x00494150`:

- compare current `Vblanks` to the loop's saved starting vblank;
- if no vblank elapsed during update/render, call `Pause(1)`;
- then unconditionally call a second `Pause(1)`.

On the 60 Hz engine clock this deliberately produces approximately **30 menu loops/frames per second**.

This confirms the user's observed 30 FPS menu cap.

Important:
- simply NOPing both waits would make the shell loop run as fast as possible and can accelerate loop-count-based menu animation;
- the correct uncapping design should separate shell simulation/update cadence from render/presentation cadence, just as gameplay modernization should separate fixed simulation from rendering.

Current timing direction:
- gameplay: preserve 60 Hz fixed simulation unless runtime telemetry proves another intended cadence;
- shell/menu: preserve 30 Hz logical update semantics initially;
- render/present: allow independent high/uncapped cadence;
- interpolate visual state between fixed logical updates where useful/feasible.

This approach is safer than global delta-time conversion for this 2000 fixed-point engine and directly addresses the user's request to avoid physics/gameplay speed changes at high FPS.


## Recovery after input-stream interruption — 2026-10-01

The user supplied the interrupted-session transcript and asked whether any work was lost.

Recovery audit:
- live `dev` was inspected directly;
- the last pre-audio timing/documentation checkpoint was:
  - `e497ed897cfb6aeca55a08d5a5b39a4ad0e973b7` — `docs: ground gameplay and shell fixed-step timing`;
- live `dev` is three implementation commits ahead of that checkpoint:
  - `ea594c7ce6e8349a46ca29b4d4e5d834b5eb6b24` — `audio: add persisted output-device selection backend`;
  - `4aa5d59a4000c03ab5a9c3c80ae775058dfe4379` — `audio: collect device-selection log in test sessions`;
  - `7826aec4b1fb6adb2d2afcf14284ed807b4ae188` — `audio: add output-device row with safe shell restart`.

### Audio work that definitely survived

Backend:
- dynamically resolves `DirectSoundCreate8` and `DirectSoundEnumerateA`;
- enumerates playback devices;
- creates an authored row 0 named `(System Default)`;
- persists manual device selection by DirectSound GUID in `spidey-modern-audio.ini`;
- default/fallback policy is row 0 / system default;
- hooks the retail DirectSoundCreate8 import thunk at `0x00517A70`;
- logs enumeration/selection/create results to `spidey-decomp-audio.log`.

Audio menu:
- retail `Shell_SFXMusic @ 0x004977D0` is extended from five rows to six;
- row 5 is `Output: <device name>`;
- uses the retail left/right trigger masks for selection;
- refreshes devices when the Audio menu opens;
- switching output is shell-only and performs a controlled DirectSound shutdown/recreate;
- after recreation it runs retail `DXSOUND_Init @ 0x005039F0` and re-spools the `menu` SFX bank;
- if a manually selected device fails to initialize, it falls back to `(System Default)`;
- the resulting selection is persisted;
- install marker:
  `audio_menu_mod retail=0x004977D0 rows=6 output_row=5 ... live_restart=shell_only`.

### Other work confirmed safe

Mouse:
- `4381062061b7a625c036dff7d851a3fac9430818` survives on `dev`;
- frontend cursor hotspot/hit-test scaling now uses the same live DX canvas as shell hit rectangles.

Timing:
- retail gameplay is grounded as a maximum 60 Hz fixed-step/update loop;
- retail main menu's ~30 FPS cap is grounded as two one-vblank waits per normal loop;
- no global delta-time conversion was committed;
- no menu-FPS uncapping patch was committed yet.

Widescreen:
- substantial static RE survived in the interrupted transcript;
- `M3d_RenderSetup @ 0x00472DC0` was identified as the upstream 3D projection path;
- the aspect scalar `0x00550064` is consumed inside that projection math;
- the engine's separate PSX-style pixel-aspect state was also identified;
- no final Hor+ implementation was committed before the interruption.

### What was actually lost

No substantive implementation commit was lost.

The only missing durable state from the interrupted tail was:
- final written documentation summarizing the new audio implementation;
- any analysis performed after commit `7826aec4...` but before the stream terminated.

That analysis was short and can be reconstructed from the supplied transcript plus the surviving source. The authoritative implementation frontier is the live `dev` branch, not the interrupted chat text.


## Widescreen recovery RE — projection patch vs render-domain separation — 2026-10-01

Recovery package and live branch were re-verified before further engineering:
- handoff SHA-256: `8bee9904a6ea22854f87c0301425c2b5ba0d771bccc319f7d73e5c0f9b0432a6`;
- ZIP integrity: PASS;
- live `dev` HEAD still exactly `ce20a6eb1caf00df0816551f54a5c88a6fcd0c95`;
- no branch/package divergence exists.

### Established aspect scalar is valid

The current aspect values are not speculative.

The public `r57zone/Spider-Man-Settings` utility writes the aspect-ratio float at executable file offset `0x150064`, which maps to retail VA `0x00550064`, and uses the same values already present in this project:
- 4:3 = 1.0
- 5:4 = 1.06667
- 16:9 = 0.75
- 16:10 = 0.83333
- 21:9 = 0.57143
- 32:9 = 0.375

That independent implementation credits the original address discovery and documents widescreen support, while also documenting two limitations relevant to our current work:
- HUD remains stretched;
- edge polygons can appear late because culling/optimization still assumes the old view.

Therefore do **not** replace `0x00550064` with a guessed FOV constant. The remaining work is to separate world projection, source-screen coordinates, 2D/HUD layout, frontend layout, and culling.

### Runtime evidence proves the render domains are already decoupled

Latest 2560x1440/16:9 evidence shows:
- selected visible output = `2560x1440`;
- aspect scalar = `0.750000`;
- gameplay logical dimensions at `0x00568154/58` = `2560x1440`;
- compatibility D3D7 backing can remain `1920x1440`;
- early pixel-present path preserves that 4:3 backing as `rect=320,0,1920x1440` on the 2560x1440 DX11 target;
- once geometry replay becomes active, the DX11 shadow target is `2560x1440`;
- captured transformed vertices span `x=0..2560`, `y=0..1440`.

The captured D3D7 viewport state can still report `640x480` while the transformed XYZRHW stream spans the 2560x1440 logical domain. For shadow preview the proxy currently compensates by overriding the replay viewport to the modern logical dimensions.

Implication:
- D3D7 `SetViewport` state alone is **not** a trustworthy normalization basis after compatibility remapping;
- transformed XYZRHW coordinates, the logical projection canvas, the hidden backing surface, and the visible DX11 target must be modeled as separate domains;
- a blanket “modern canvas” policy for both 3D and 2D is the wrong final architecture.

### M3d_RenderSetup confirms the upstream split

Retail `M3d_RenderSetup @ 0x00472DC0`:
- consumes `PixelAspectX @ 0x00654F58` and `PixelAspectY @ 0x00654F5C`;
- computes viewport center/scale fields from the supplied `SViewport`;
- consumes logical output width/height at `0x00568154/58`;
- consumes aspect scalar `0x00550064` in the projection coefficient divided by viewport Zoom.

This confirms that `0x00550064` is an upstream 3D projection control, while logical width/height also participate in screen-space conversion.

### Current engineering direction

Priority 1 remains true Hor+ widescreen, but the implementation should now be split deliberately:

1. retain the proven aspect scalar for 3D projection;
2. give geometry replay an explicit logical/source-screen normalization basis instead of inheriting arbitrary retail D3D7 viewport state;
3. classify/separate 2D/HUD/frontend layout from 3D world projection before applying any widescreen transform;
4. preserve the original frontend coordinate semantics until its modern layout pass is handled explicitly;
5. trace the old-view culling/frustum boundary separately so widened projection does not reveal late-appearing edge geometry.

Do not request a runtime test yet. Continue static classification and implementation until the 3D-vs-2D boundary is explicit enough for a meaningful widescreen build.
