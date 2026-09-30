# Spider-Man 2000 PC Modernization / Decomp — NEW CHAT HANDOFF

Date: 2026-09-30
Active repository: https://github.com/legentus/spidey-decomp
Active branch: `dev`
Upstream baseline: https://github.com/krystalgamer/spidey-decomp
Tools upstream: https://github.com/krystalgamer/spidey-tools
User orchestration repo: https://github.com/legentus/Spiderman-2000
Retail Google Drive source: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

## READ THIS FIRST

Do **not** restart the investigation from old chat history.

The live GitHub `dev` branch and `docs/CURRENT_STATUS.md` are authoritative.

Before changing code:
1. Read this file.
2. Read `docs/CURRENT_STATUS.md` from the newest entries at the bottom upward.
3. Read `docs/DX11_MIGRATION.md`.
4. Confirm the current `dev` HEAD.
5. Preserve the disconnect-safe workflow: update `docs/CURRENT_STATUS.md` during work, not only at the end.
6. Do not ask the user to repeat logs/tests that are already documented or included in the handoff.

The project has now pivoted from long-term DirectDraw7/Direct3D7 patching to an incremental **Direct3D 11 renderer migration**. The old D3D7 renderer remains a temporary reference/fallback only.

## USER WORK STYLE — MANDATORY

- Make concrete progress without repeatedly asking for clarification when the repo/logs already answer the question.
- Never fabricate addresses, calling conventions, machine-code behavior, or runtime state.
- Verify retail behavior before patching.
- Use exact-byte guards or bounded verified call-site scans for retail patches when possible.
- Static-check changes before asking for a runtime test.
- Runtime tests should answer a real uncertainty.
- Live-update `docs/CURRENT_STATUS.md` after important findings, failed hypotheses, or implementation steps.
- Commit/push `dev` every few meaningful changes so an input-stream failure cannot erase work.
- Keep `master` clean/upstream-oriented.
- BAT-first user workflow; do not require manual PowerShell as the normal path.
- Do not commit retail game binaries/assets.
- When a chat disconnects, inspect live `dev` + `CURRENT_STATUS.md` first and continue from the real frontier.

## USER LOCAL PATHS

Project:
`F:\Spider-Man 2000 Recomp\project main`

Retail game:
`C:\Program Files (x86)\Activision\Spider-Man`

Preserved matching toolchain:
`C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS`

Normal one-click workflow:
`UPDATE_AND_TEST_LATEST_BUILD.bat`

That BAT:
1. updates the project from GitHub `dev`;
2. force-cleans/builds the legacy matching proxy;
3. builds the modern x86 DX11 helper;
4. installs both DLLs into the game folder;
5. launches the game;
6. captures the full test-session logs.

## EXACT RETAIL EXE FINGERPRINT

Repeatedly verified:

```text
sha256=D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C
file_size=1507328
machine=0x014C
timestamp=0x3B7A3167
entrypoint_rva=0x0012B46F
image_base=0x00400000
size_of_image=0x02A0D000
characteristics=0x010F
.text va=0x00001000 vsize=0x00139CE5 raw=0x00001000 rawsize=0x0013A000
.rdata va=0x0013B000 vsize=0x0000AFE8 raw=0x0013B000 rawsize=0x0000B000
.data va=0x00146000 vsize=0x028C5538 raw=0x00146000 rawsize=0x00029000
.rsrc va=0x02A0C000 vsize=0x00000DE8 raw=0x0016F000 rawsize=0x00001000
```

## CURRENT ARCHITECTURE

The game is still retail-hosted, not a standalone replacement EXE.

Legacy path:
1. retail `SpideyPC.exe` loads `binkw32.dll`;
2. rebuilt `spider.dll` is installed as proxy `binkw32.dll`;
3. original RAD Bink remains as `binkw32_.dll`;
4. proxy forwards Bink exports;
5. proxy patches verified retail functions/call sites into reconstructed code.

New DX11 path:
- `spidey_renderer11.dll` is a separate modern x86 DLL;
- built with VS 2022 / current Windows SDK;
- legacy proxy loads it dynamically with `LoadLibraryA/GetProcAddress`;
- communication is a versioned C ABI;
- modern D3D11/DXGI headers do **not** enter the VC6 matching build.

## CURRENT SOURCE FRONTIER

Source frontier immediately before this handoff-document refresh:
`0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0`
— `docs: checkpoint Direct3D 11 Phase 0 scaffold`

The handoff-document update itself may advance HEAD without changing runtime behavior.

### DX11 Phase 0 is IMPLEMENTED, awaiting first runtime validation

Implemented files:
- `renderer11/include/spidey_renderer11_api.h`
- `renderer11/src/spidey_renderer11.cpp`
- `renderer11/CMakeLists.txt`
- `renderer11/spidey_renderer11.def`
- `scripts/build_renderer11.ps1`
- `docs/DX11_MIGRATION.md`

Stable ABI version:
`SPIDEY_RENDERER11_ABI_VERSION = 1`

Exports:
- `SpideyRenderer11_GetAbiVersion`
- `SpideyRenderer11_GetBackendName`
- `SpideyRenderer11_Probe`
- `SpideyRenderer11_Initialize`
- `SpideyRenderer11_Resize`
- `SpideyRenderer11_BeginFrame`
- `SpideyRenderer11_Present`
- `SpideyRenderer11_Shutdown`

Modern backend already contains:
- D3D11 hardware device probe;
- D3D11 device/immediate context;
- DXGI swap chain;
- RGBA8 RTV;
- D24S8 depth buffer;
- viewport setup;
- resize;
- clear/present;
- adapter + feature-level logging to `spidey-renderer11.log`.

Legacy bridge currently:
- loads `spidey_renderer11.dll`;
- verifies ABI 1;
- calls the DX11 hardware probe;
- logs backend/probe state;
- **does not switch visible rendering yet**.

## EXACT NEXT ACTION

The next user action is a Phase 0 plumbing/probe test:

```bat
UPDATE_AND_TEST_LATEST_BUILD.bat
```

This is **not** expected to visually render through DX11 yet.

Expected successful behavior:
- game still boots/renders using the known-good D3D7 compatibility path;
- the working Alt+Tab input recovery remains working;
- old building/city flashing remains gone;
- whole-screen black flash remains gone;
- `spidey-decomp-compat.log` contains something equivalent to:
  `renderer11_bridge loaded ... abi=1 expected=1 backend=Direct3D 11 probe=1`
- `spidey-renderer11.log` exists and records a successful hardware probe / D3D feature level;
- test-session metadata contains `renderer11_sha256=...`.

User should upload the full test session after running it.

### If Phase 0 passes

Proceed to **Phase 1: DX11 owns final presentation**.

Phase 1 goal:
- keep legacy D3D7 scene rendering initially;
- initialize DX11 swap chain at physical client/output dimensions;
- transfer/copy the completed D3D7 scene into a DX11 texture/staging/upload path;
- present exclusively with DXGI;
- remove the GDI `BitBlt/StretchBlt` compatibility presenter.

Do not jump directly to full fixed-function 3D emulation before Phase 1 presentation parity works.

## WHY THE PROJECT PIVOTED TO DX11

The old renderer became the dominant modernization blocker.

Proven D3D7 limitation:
- a 2560x1440 DirectDraw scene surface can be created;
- `IDirect3D7::CreateDevice` then rejects that surface with
  `0x88760082 = DDERR_INVALIDOBJECT`;
- this happens before the first splash frame if 2560x1440 is persisted and restored at boot.

Two-session 1440p evidence:
1. Boot below 1440p, enter frontend, select/save 2560x1440:
   - game stays alive;
   - frontend remains internally 640x480;
   - saved setting changes to 2560x1440;
   - **this did not prove live 2560x1440 rendering**.
2. Next launch with 2560x1440 persisted:
   - startup restores 2560x1440;
   - D3D7 `CreateDevice` fails before any splash;
   - cleanup guard prevents the old secondary null dereference.

Therefore:
- 2560x1440 has never actually rendered live through the D3D7 path;
- continuing to fight D3D7 for modern resolution support was judged worse than migrating the renderer.

## CURRENT D3D7 SAFETY DURING DX11 MIGRATION

While DX11 is only Phase 0/probe:
- exact 2560x1440 is quarantined from the legacy D3D7 mode table;
- persisted 2560x1440 is recovered to the last verified safe `1440x1080x32`;
- this prevents another pre-splash D3D7 crash;
- native 2560x1440 is intended to return through DXGI once DX11 owns presentation/rendering.

Do not remove that safety until DX11 presentation owns the output.

## RECENT PROVEN FIXES — DO NOT REGRESS

### 1. Alt+Tab input loss — FIXED

Retail input routines were mapped exactly:
- `DXINPUT_Initialize = 0x005013D0`
- `DXINPUT_Release = 0x00501440`
- `DXINPUT_SetupKeyboard = 0x00501590`
- `DXINPUT_SetupMouse = 0x00501710`
- `DXINPUT_PollKeyboard = 0x00501B80`
- `DXINPUT_PollMouse = 0x00501CC0`
- `DXINPUT_PollController = 0x00501E50`

Retail globals:
- DirectInput object `0x006B7A30`
- input HWND `0x006B7A60`
- keyboard device `0x006B7A5C`
- mouse device `0x006B7A64`
- controller device `0x006B7A2C`
- keyboard state `0x006B792C`
- mouse state `0x006B7A54`
- controller state `0x006B7A34`

Fix:
- direct retail callers of PollKeyboard/PollMouse are wrapped;
- real retail devices are explicitly Unacquired in background and Acquired on foreground return;
- user verified Alt+Tab out/back no longer kills controls.

Important commit:
`92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`

### 2. Rapid flashing building/city images in menus — FIXED

Root cause:
- two presentation paths were painting the same HWND:
  1. retail DirectDraw Flip/Blt into legacy primary;
  2. compatibility GDI scene->HWND presenter.

Legacy primary and physical client were different sizes, creating visible stale/foreign frames.

Fix:
- windowed compatibility mode uses one presenter only;
- retail Flip remains untouched only for original non-windowed behavior;
- user confirmed the flashing building/city imagery disappeared.

Same major commit:
`92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`

### 3. Whole-screen black flashing — FIXED

Root cause:
- aspect-fit presenter cleared the entire HWND black before every StretchBlt;
- DWM/GDI could expose the clear as a transient full-black frame.

Fix:
- clear only the actual pillarbox/letterbox bars;
- user reported the whole-screen flash was gone.

Commit:
`543b456f90495cdb8123b5437d8c1e039f832bde`

### 4. White/missing menu art / absurd texture dimensions — FIXED

Root cause:
- reconstructed PCTex D3D device caps base was eight bytes wrong;
- retail-proven D3DDEVICEDESC7 base is `0x006B5780`, not `0x006B5788`;
- after frontend renderer reinit the shifted fields produced absurd negative/GB-scale texture buffer dimensions.

Fix:
- corrected caps base;
- texture creation logs returned to sane 32/64/128/512 dimensions.

Important commit:
`99bcb6fc62be5398ea177c986975d408d2e390c0`

### 5. Texture hash lookup — KEEP

Retail texture hash table base:
`0x006AB934`

Verified from retail blob; do not revert to old guessed base.

### 6. Movie surfaces

Movie surface cleanup was implemented and runtime proved each startup movie surface released to refcount 0.
Flashing buildings persisted before the double-present fix, so movie-surface leak was **ruled out** as that visual artifact.

Keep cleanup; do not reopen that theory without new evidence.

## PRESENTATION STATE BEFORE DX11 TAKES OVER

Compatibility HWND/client:
- physical 2560x1440 on user's monitor.

Legacy D3D7 frontend:
- 640x480 internal canvas.

Known safe gameplay-ish internal modes include 1440x1080x32.

Presenter:
- preserves source aspect ratio;
- therefore 4:3 scene becomes pillarboxed in 16:9;
- do not “fix” this by stretching.

True widescreen requires:
1. a real 16:9 render target;
2. projection/FOV validation;
3. UI/HUD safe-area handling.

## WIDESCREEN / UI FINDINGS

The current side bars are expected while the internal scene is 4:3.

Relevant source:
- `PCSHELL_CoordsDCtoPC` maps virtual 512x240 shell coordinates independently into live X/Y resolution;
- frontend/HUD therefore needs its own 16:9 policy rather than arbitrary stretching.
- `M3d_RenderSetup` remains retail, so projection/FOV behavior should be measured after DX11 creates a real 16:9 gameplay target.

DX11 migration Phase 5 is explicitly reserved for:
- preserve vertical FOV / expand horizontal FOV as needed;
- HUD/menu safe area;
- cutscene/model-preview/special-camera validation.

## DX11 MIGRATION PHASES

Phase 0 — bridge/device probe
- IMPLEMENTED, awaiting first runtime validation.

Phase 1 — DX11 final presentation
- legacy D3D7 still renders scene;
- DX11/DXGI owns presentation;
- remove GDI presenter.

Phase 2 — texture ownership
- migrate PCTex resources to ID3D11Texture2D + SRV;
- preserve retail texture lookup/spooling semantics.

Phase 3 — 2D/frontend
- sprites/quads/fonts/HUD;
- orthographic shader path;
- proper 16:9 safe area.

Phase 4 — fixed-function 3D emulation
Known centralized legacy state families:
- depth enable/write/function;
- alpha blend enable/factors;
- fog color/start/end;
- texture address U/V;
- min/mag filter;
- texture-stage color/alpha operations;
- texture binding;
- triangle-fan submission.

D3D11 has no triangle-fan topology:
- convert fans to triangle-list indices.

Phase 5 — true widescreen/FOV/UI.

Phase 6 — DX11 default; D3D7 reference/diagnostic only.

DX12 is intentionally **not** planned. Its explicit barriers, descriptor heaps, fences, command lists, etc. add complexity without meaningful benefit for this game.

## IMPORTANT DX11 COMMITS

- `09db45a1bd6cd09ed429078b3bbcaa641573c1ed` — stable C bridge API
- `ab4c7f5461c8814ab06d233e2199c8d4febc3a72` — CMake target
- `343de8e6f50b9448444b461f69a08c6e2a0f3f03` — D3D11 device/swap-chain backend
- `b1fd7e3d3e93268db95c68dfd7015dbb72be66ce` — modern build script
- `e754aca6117048555db0fb2bf2a43bfd7bed812a` — legacy bridge probe + D3D7 safety
- `168d4d3f215dcd8c51e60b47b81f6b691e834ee7` — one-click build/install/log integration
- `1496e2c29f6e1c35a49be9e93bd55bc3b51decbd` — migration docs
- `453146284b3c6bedfe06277da93d45d0fef5c44f` / `b745a89c5170edef9008289044b4b4ad42273e63` — undecorated x86 exports
- `550f0af1f897201430ac94770bd35b455d774fc6` — CMake path fallback
- `0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0` — Phase 0 status checkpoint

## OLD D3D7 REVERSE-ENGINEERING FACTS STILL USEFUL

Windowed DX init:
- retail `DXINIT_DirectX8 = 0x004FDE90`
- RealWinMain call site `0x00515BAD`
- argument push site `0x00515BA9`
- compatibility changed third arg 2 -> 3 to activate retail windowed branch.

Presentation:
- `DXPOLY_Flip = 0x00502990`
- `DXPOLY_EndScene = 0x00502A40`
- EndScene->Flip call site `0x00502D41`

Scene/presentation globals:
- HWND `0x006B58D0`
- windowed flag `0x006B78F4`
- primary surface `0x006B7904`
- scene surface `0x006B7908`
- rect `0x006B5958`
- live DX width `0x006B78E4`
- live DX height `0x006B78E8`
- color count/bpp `0x006B78EC`
- game width `0x00568154`
- game height `0x00568158`

Saved settings:
- width `0x02E096F8`
- height `0x02E0970C`
- bpp `0x02E098E4`

D3D caps:
- retail base `0x006B5780`.

D3D7 2560x1440 failure:
- `IDirect3D7::CreateDevice` call site around `0x004FEA4A`
- HRESULT `0x88760082 = DDERR_INVALIDOBJECT`
- old cleanup function `0x00503AF0`
- null dereference was `0x00503AF7` via global `0x006BBF1C`
- cleanup is guarded now.

## HISTORICAL PLAYABLE BASELINE

Historical visible/playable commit:
`35e73ed3c4ca8c06b581df83f0f0913f0c915982`

At that revision user previously:
- reached first level;
- controlled Spider-Man;
- returned to menu;
- Options crashed.

The old Options crash was retail `Font::height(char*)` calling convention:
- retail function `0x0043EAF0`;
- reconstructed wrapper originally failed to pass `this` in ECX;
- correct FASTCALL trampoline was later restored and tested in newer work.

Do not reset the project to this historical baseline unless specifically diagnosing a regression.

## AUDIO

Current audio is working.
Do not reactivate old audio diagnostics unless a new audio bug is reported.

## CONTROLLER / XINPUT FUTURE WORK

Still desired after renderer stabilization:
- modern Xbox/XInput backend;
- analog sticks/triggers;
- configurable mappings;
- Xbox button prompts;
- rumble.

Historical retail button states:
- `0xFF` new press
- `0x7F` held
- `0x80` release
- `0x00` idle

Do not distract Phase 0/1 DX11 work with controller feature expansion unless the user changes priority.

## LOGS TO EXPECT FROM THE NEXT TEST

Normal session should include:
- `test-session.txt`
- `game-exe-fingerprint.txt`
- `proxy-link-map.txt`
- `spidey-decomp-compat.log`
- `spidey-decomp-present.log`
- `spidey-decomp-texture.log`
- `spidey-decomp-input.log`
- `spidey-decomp-dxerror.log` if DirectX errors occur
- `spidey-decomp-crash.log` if a native crash occurs
- NEW: `spidey-renderer11.log`

Treat all logs the user uploads as a set.

## HANDOFF / INTERRUPTION RECOVERY

If an input/message stream fails:
1. inspect current `dev` HEAD;
2. read newest `docs/CURRENT_STATUS.md`;
3. read this document;
4. determine exactly which commits landed before the interruption;
5. continue from the actual repo frontier;
6. document recovery immediately;
7. keep committing in small checkpoints.

Do not reconstruct work from stale chat text if the repo already answers it.

## LONG-TERM USER GOAL

The target is not merely “make the old game run.”

The user wants a maintainable modern PC development environment that can:
- fix original bugs;
- add features;
- support modern resolutions/widescreen;
- add modern controller support;
- expose mod/plugin APIs;
- eventually support editor/dev tooling (“Spidey Studio” concept);
- progressively replace/decompile/reconstruct retail systems without throwing away working gameplay.

The DX11 migration is now the rendering foundation for that long-term direction.
