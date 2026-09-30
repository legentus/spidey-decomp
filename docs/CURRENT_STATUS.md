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
