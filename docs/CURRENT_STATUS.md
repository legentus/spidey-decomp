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
