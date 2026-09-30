# Spider-Man 2000 PC Modernization — New Chat Handoff

**Date:** 2026-09-30  
**Active repository:** https://github.com/legentus/spidey-decomp  
**Active branch:** `dev`  
**Live source of truth:** `dev` + `docs/CURRENT_STATUS.md`  
**Current implementation frontier:** `502bd2864c5e6fd8a1e268f18656591a8b709d0c`  
**Current documentation frontier:** the live `dev` HEAD after the Phase 3E checkpoint commit.

> **CURRENT OVERRIDE:** Later Phase 3C sections in this file are historical context. The actual pending user-facing test is now Phase 3D live Apply + frontend mouse return. Phase 3E F9 D3D7-draw suppression is implemented but defaults OFF and should only be exercised after the Phase 3D checks pass.

### Current pending user-facing checks
1. At 2560x1440, change 16:9 -> 4:3 -> Apply and verify the running frontend changes immediately without restart.
2. Change back to 16:9 -> Apply and verify immediate full-width restoration.
3. Enter gameplay, return to main menu, and verify mouse hover/click works after the gameplay -> frontend transition.
4. Confirm logs contain `frontend_bounds_sync` and the new `logical_render_resolution ... selected=... content=... aspect=...` fields.

### Next renderer-isolation experiment after those checks
- F9 toggles guarded suppression of already-DX11-accepted main-scene D3D7 draws.
- Default is OFF.
- F10 remains the complete D3D7 reference path.
- Inspect `d3d7_suppressed` / `d3d7_fallback` telemetry and visual behavior before making suppression permanent.

### Modern input/camera design document
- `docs/MODERN_INPUT_CAMERA.md`
- includes retail input hook anchors, normalized input architecture, dynamic glyph/remapping plan, and a two-stage camera plan that can progress to a dedicated modern camera rather than being constrained by legacy camera behavior.

### New major modernization goals
- full modern controller support with remapping and dynamic glyph UI;
- modern mouse/right-stick camera;
- the final camera is **not required** to remain constrained by the original camera system. Existing camera functions are RE anchors and scripted-transition helpers, but a dedicated modern gameplay camera may replace ordinary legacy camera ownership if that produces the desired feel.\n\n## READ THIS FIRST

This project has frequent ChatGPT "error in input stream" interruptions. **Do not trust stale chat text over the repo.**

On entering a new chat:

1. Open/fetch the live `dev` HEAD.
2. Read the tail of `docs/CURRENT_STATUS.md`.
3. Read this file.
4. Read `docs/DX11_MIGRATION.md` for architecture.
5. Determine which commits actually landed before any interruption.
6. Continue from the repo frontier; do not redo already committed work.
7. **Live-update `docs/CURRENT_STATUS.md` while working**, not just at the end.
8. Push small commits to `dev` every few meaningful steps.
9. If a stream dies, the next chat must be able to recover from the repo alone.

The user explicitly requires this disconnect-safe workflow.

---

## Project links

- Active decomp/modernization repo: https://github.com/legentus/spidey-decomp
- Active `dev` branch: https://github.com/legentus/spidey-decomp/tree/dev
- Live status: https://github.com/legentus/spidey-decomp/blob/dev/docs/CURRENT_STATUS.md
- DX11 migration notes: https://github.com/legentus/spidey-decomp/blob/dev/docs/DX11_MIGRATION.md
- This handoff: https://github.com/legentus/spidey-decomp/blob/dev/docs/NEW_CHAT_HANDOFF.md
- Upstream decomp: https://github.com/krystalgamer/spidey-decomp
- Upstream tools: https://github.com/krystalgamer/spidey-tools
- User orchestration repo: https://github.com/legentus/Spiderman-2000
- Retail game Google Drive: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
- Retail/docs Google Drive: https://drive.google.com/drive/folders/1Py0hitNzvKJ5xFU3kSyFUAgpX3F7_uf-

User local paths:
- Project: `F:\Spider-Man 2000 Recomp\project main`
- Game: `C:\Program Files (x86)\Activision\Spider-Man`
- Matching VC6-era toolchain: `C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS`
- User test/update entrypoint: `UPDATE_AND_TEST_LATEST_BUILD.bat`

---

## Retail EXE fingerprint — do not patch other builds blindly

Verified retail `SpideyPC.exe`:
- SHA-256: `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- size: `1507328`
- machine: `0x014C`
- timestamp: `0x3B7A3167`
- entrypoint RVA: `0x0012B46F`
- image base: `0x00400000`
- size of image: `0x02A0D000`

---

## Current architecture

This is still a **retail-host proxy**, not yet a standalone native EXE.

1. `SpideyPC.exe` loads `binkw32.dll`.
2. Rebuilt `spider.dll` is installed as proxy `binkw32.dll`.
3. Retail Bink remains `binkw32_.dll`.
4. The proxy forwards Bink exports and patches selected retail functions/calls.
5. Modern renderer is a separate `spidey_renderer11.dll` built with VS2022 x86/Win32.
6. Legacy proxy talks to renderer11 through a stable C ABI.
7. Renderer ABI is currently **6**.
8. D3D7 still exists for compatibility/state/resource plumbing and still executes original draws in the background.
9. **DX11 geometry is the default visible renderer.**
10. F10 remains an A/B switch to the old D3D7-rendered reference image.

Do not describe the project as fully standalone or fully free of D3D7 yet.

---

## Major verified milestones

### Alt+Tab/input — fixed
- Retail DirectInput polling is wrapped.
- Devices reacquire correctly after foreground return.
- User confirmed Alt+Tab no longer permanently kills controls.
- Important commit: `92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`.

### Double-present/menu flashing — fixed
- Only one visible presenter owns the HWND in compatibility mode.
- Rapid stale building/city imagery in menus is gone.

### Whole-screen black flashing — fixed
- Presenter clears only bars, not the whole HWND before each blit.

### Texture/caps issues — fixed
- Correct retail D3DDEVICEDESC7 base is `0x006B5780`.
- Texture hash table base is `0x006AB934`.

### DX11 Phase 0 — passed
- Modern DLL builds/loads.
- Hardware D3D11 probe succeeds.
- User GPU log showed feature level 11.1 on RTX 4070 SUPER.

### DX11 Phase 1 — passed
- DX11/DXGI owns final presentation.
- Legacy D3D7 scene was transferred into DX11.
- User booted, started a new game, and played normally.

### DX11 Phase 2A — passed
- Normal frame transfer changed from HDC/GDI to:
  D3D7 scene lock -> BGRA8 D3D11 upload texture -> shader draw -> DXGI Present.
- HDC and direct-HWND fallback paths remain for safety.

### DX11 Phase 2B — passed
- Game textures are mirrored to per-ID DX11 sidecar resources.
- Existing 0..1023 texture IDs remain canonical.

### DX11 Phase 2C1 — passed
Retail DrawPrimitive stream was intercepted and replayed offscreen in DX11.

Observed stream over a real gameplay run:
- 100% triangle fans;
- 100% FVF `0x144`;
- XYZRHW + diffuse + UV;
- sampled frame replay had `queued == submitted == rendered`;
- zero sampled state-cache mismatches;
- zero sampled shadow render skips;
- transient surfaces were recovered into synthetic IDs 1024+;
- DX11 shadow target contained real non-black scene pixels.

### DX11 Phase 2C2 — passed visually
F10 switched the visible image to the independently reconstructed DX11 geometry renderer.
User reported:
- everything looked correct;
- DX11 may have looked slightly better.

Representative runtime frame:
- retail calls: 5,007;
- DX11 shadow submit: 5,007;
- renderer queued/submitted/rendered: 5,007/5,007/5,007;
- zero skipped renders;
- zero missing textures.

D3D7/DX11 exact sampled pixels are close but not bit-identical:
- median absolute per-channel difference about 1 level;
- mean about 2.78 levels in the measured sample set;
- no visual problem reported.

### DX11 Phase 2C3 — passed
DX11 geometry was promoted to **default visible**.
User tested:
- normal boot;
- frontend/gameplay;
- F10 reference/DX11 switching;
- everything looked clean.

Small frametime hitches roughly every 0.5–1 second were noticed on a frametime graph, but user reports they existed before this DX11 work and occur regardless of renderer. Treat as a separate later profiling task unless evidence ties it to current changes.

---

## Why 2560x1440 needs a compatibility split

D3D7 physical 2560x1440 is a verified dead end:
- scene surface creation can succeed;
- `IDirect3D7::CreateDevice` rejects a 2560x1440 scene surface;
- HRESULT: `0x88760082 = DDERR_INVALIDOBJECT`;
- failure occurs before the first splash when 2560x1440 is restored directly through old D3D7.

Therefore modern output and legacy backing must be separate concepts.

Current target for selected 2560x1440:
- **user-selected/output:** 2560x1440x32
- **DX11 target/swap output:** 2560x1440
- **hidden safe D3D7 backing:** 1920x1440x32
- **frontend canvas:** still intentionally 640x480 / 4:3 for now

Do not re-enable physical D3D7 2560x1440.

---

## Phase 3A — native logical gameplay resolution

Implemented architecture:
1. physical D3D7 compatibility surface;
2. logical game resolution;
3. actual DX11 render/output resolution.

The project can now make the game's logical gameplay dimensions different from the D3D7 physical backing.

Intended 2560x1440 configuration:
- D3D7 physical: 1920x1440;
- logical gameplay: 2560x1440;
- DX11 color/depth target: 2560x1440;
- DXGI output: 2560x1440.

Per-frame geometry telemetry was added to prove whether the retail software projection naturally produces true Hor+ vertices beyond the old 1920-wide physical viewport. If not, a narrow projection/FOV hook is the next step after settings persistence works.

---

## Phase 3B test result — settings visible, but persistence/apply was broken

Tested revision:
`91ffd2884f2591eab48374386c342fbcdb1c30e6`

User result:
- Screen Size and Aspect Ratio rows appeared;
- aspect values could be cycled;
- 2560x1440 appeared while scrolling Screen Size;
- settings did not actually commit;
- reopening the menu reverted Screen Size;
- pressing Enter on 2560x1440 could immediately change it back to an odd/older resolution;
- user requested an explicit **Apply** row so display settings can be applied without restarting.

Runtime evidence from that test:
- all Phase 3B aspect call patches installed successfully;
- aspect log reached `16:9 scalar=0.750000`;
- committed output stayed `1920x1440`;
- modern mode table had 24 entries and explicitly exposed 2560x1440;
- renderer11 already initialized at 2560x1440;
- therefore the remaining bug was **retail Display Options transaction semantics**, not DX11 capability.

Important discovered retail behavior:
- Screen Size uses saved width/height globals as its temporary working variables.
- Pressing Enter anywhere in the original menu calls `DXINIT_SetDisplayOptions`.
- Original Color Depth changes also call next/previous-resolution searches to pick a resolution compatible with the new bpp.
- After Color Depth was repurposed as Aspect Ratio, those compatibility searches were still mutating Screen Size.

This explains the user's exact symptom.

---

## Phase 3C — CURRENT UNTESTED FRONTIER

**This is the next runtime test. Do not assume it passed.**

The Display Options screen has been converted to a transaction model.

### New rows
1. Screen Size
2. Aspect Ratio
3. Brightness
4. **Apply**

### Pending vs committed settings

Screen Size and Aspect Ratio now edit **pending** state only.

Committed:
- `gSpideySelectedOutputWidth/Height`
- `gSpideyAspectMode`

Pending:
- separate width/height
- separate aspect mode

Screen Size text reads pending resolution.
Aspect Ratio text reads pending aspect.

Retail resolution stepping algorithms are still reused, but they receive temporary local values rather than the committed saved globals.

### Aspect modes
- AUTO
- 4:3
- 5:4
- 16:9
- 16:10
- 21:9
- 32:9

Aspect/projection scalar runtime VA:
`0x00550064`

Explicit scalars:
- 4:3 = 1.0
- 5:4 = 1.06667
- 16:9 = 0.75
- 16:10 = 0.83333
- 21:9 = 0.57143
- 32:9 = 0.375

AUTO:
`(4 * height) / (3 * width)`

Aspect selection is persisted in:
`spidey-modern-video.ini`
beside `SpideyPC.exe`.

### Obsolete Color Depth coupling disabled

The old post-color-depth resolution searches are now no-ops:
- `0x0050DDFB -> 0x00500E20`
- `0x0050DE1F -> 0x00500F40`

Changing Aspect Ratio must no longer change Screen Size.

### Apply semantics

Pressing Enter on rows 0–2 no longer commits/rebuilds display state.

Pressing Enter on **Apply**:
1. commits pending resolution/aspect;
2. writes saved width/height/bpp globals;
3. applies the aspect scalar;
4. preserves the frontend's known-good 640x480 canvas when currently in frontend;
5. immediately calls retail `SPIDEYDX_SaveSettings @ 0x00515850`;
6. resets pending state to the committed selection.

Back/Escape without Apply should discard pending resolution/aspect changes.

### Retail save routine verified

Retained retail bytes for `SPIDEYDX_SaveSettings` were disassembled.
It serializes the exact relevant globals:
- width: `0x02E096F8`
- height: `0x02E0970C`
- bpp: `0x02E098E4`
- brightness: `0x00562D60`

### Exact byte-verified menu patch sites

Retail function:
- `PCSHELL_DoDisplayOptions = 0x0050D9B0`
- size: 1476 bytes

Relevant patches:
- `0x0050DA72 -> 0x0043FFF0`: intercept third AddEntry and append Apply
- `0x0050DB56 -> 0x00529F90`: Screen Size formatter -> pending formatter
- `0x0050DBBB -> 0x00529F90`: Aspect Ratio formatter
- `0x0050DCF8 -> 0x00500250`: Enter/confirm -> Apply-only handler
- `0x0050DDAB -> 0x005010C0`: Aspect previous
- `0x0050DDCE -> 0x00501060`: Aspect next
- `0x0050DDFB -> 0x00500E20`: disable old aspect/color-depth compatibility resolution step
- `0x0050DE1F -> 0x00500F40`: disable fallback compatibility resolution step
- `0x0050DE71 -> 0x00500F40`: previous Screen Size operates on pending state
- `0x0050DE88 -> 0x00500E20`: next Screen Size operates on pending state

Row-1 label pointer:
- `0x0054BBD4` -> "Aspect Ratio"

Every direct-call patch verifies:
- opcode is `E8`;
- original target matches expected address;
- otherwise patch is refused and logged.

### Critical install-order fix

The Apply-specific `0x0050DCF8` hook must install **before** the generic display-options compatibility scan, because the generic installer rewrites remaining direct calls to `0x00500250`.

Correct install order:
1. modern-mode reinit compatibility;
2. transactional Display Options / Apply hooks;
3. generic display-options compatibility for remaining retail callers.

Expected:
- generic `display_options_compat patched_calls=3` now, not 4.

Source correction commit:
`016b7438fd7008d8e6075105c0b4f2e9201d5703`

Documentation commits immediately afterward:
- `10b4ec705ba631a7abb8a0a1d8e95af97a1999b4`
- `2f14244f05ad2c1946a6668b89fe60064ca9b409`

---

## EXACT NEXT USER TEST

The next chat should ask the user to run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Then:

1. Open Options -> Display Options.
2. Verify four rows:
   - Screen Size
   - Aspect Ratio
   - Brightness
   - Apply
3. Set Screen Size = **2560x1440**.
4. Set Aspect Ratio = **16:9**.
5. Move to **Apply** and press Enter.
6. Confirm the menu remains usable and Screen Size still displays 2560x1440.
7. Back out.
8. Reopen Display Options.
9. Verify 2560x1440 + 16:9 are still selected.
10. Start gameplay **without restarting the process**.
11. Inspect:
    - whether gameplay fills 16:9;
    - whether it is Hor+ rather than stretched;
    - HUD placement;
    - frontend stability;
    - level transitions.
12. Exit normally and upload the full generated log set.

Expected new log markers:
- `display_menu_mod ... rows=4 ... applyentry=1 applyconfirm=1`
- `display_pending_reset reason=menu_open ...`
- while cycling:
  `display_pending_resolution ... value=2560x1440 committed=1920x1440`
- aspect:
  `display_pending_aspect ... value=16:9 ...`
- Apply:
  `display_aspect reason=display_menu_apply_commit ...`
  `display_apply committed=1 selected=2560x1440x32 aspect=16:9 ... saved_now=1`
- reopen:
  `display_pending_reset reason=menu_open selected=2560x1440 aspect=16:9`
- gameplay:
  `display_options selected=2560x1440x32 physical=1920x1440x32 ... legacy_backing_remap=1`
  `logical_render_resolution ... logical=2560x1440 physical=1920x1440 selected=2560x1440`
- renderer11:
  native 2560x1440 shadow target / presentation.

If this test fails:
- inspect the logs first;
- confirm install markers for every Display Options patch;
- confirm the Apply hook owned `0x0050DCF8`;
- compare pending vs committed logs;
- do not reopen D3D7 2560x1440 experiments.

---

## Important retail addresses

Rendering:
- `DXINIT_DirectX8 = 0x004FDE90`
- RealWinMain DX init call = `0x00515BAD`
- argument push = `0x00515BA9`
- `DXPOLY_Flip = 0x00502990`
- `DXPOLY_EndScene = 0x00502A40`
- EndScene -> Flip call = `0x00502D41`

Globals:
- windowed flag = `0x006B78F4`
- HWND = `0x006B58D0`
- primary surface = `0x006B7904`
- scene surface = `0x006B7908`
- gRect = `0x006B5958`
- live DX width = `0x006B78E4`
- live DX height = `0x006B78E8`
- live bpp = `0x006B78EC`
- logical game width = `0x00568154`
- logical game height = `0x00568158`
- saved width = `0x02E096F8`
- saved height = `0x02E0970C`
- saved bpp = `0x02E098E4`
- brightness = `0x00562D60`
- projection/aspect scalar = `0x00550064`
- D3DDEVICEDESC7 base = `0x006B5780`
- texture hash table = `0x006AB934`
- D3D7 device pointer slot = `0x006B791C`

Display menu:
- `PCSHELL_DoDisplayOptions = 0x0050D9B0`
- row-1 label slot = `0x0054BBD4`
- retail save routine = `SPIDEYDX_SaveSettings = 0x00515850`

DirectInput:
- `DXINPUT_Initialize = 0x005013D0`
- `DXINPUT_Release = 0x00501440`
- `DXINPUT_SetupKeyboard = 0x00501590`
- `DXINPUT_SetupMouse = 0x00501710`
- `DXINPUT_PollKeyboard = 0x00501B80`
- `DXINPUT_PollMouse = 0x00501CC0`
- `DXINPUT_PollController = 0x00501E50`

---

## Known-good / do not regress

- Alt+Tab input recovery.
- Startup movies/audio.
- No rapid building/city menu flashes.
- No whole-screen black presenter flashes.
- White/missing menu textures remain fixed.
- D3D7 2560x1440 remains physically quarantined.
- DX11 geometry remains default visible renderer.
- F10 remains a working D3D7-reference A/B switch.
- Texture mirroring/transient recovery remains intact.
- Frontend compatibility canvas stays 640x480 until intentionally modernized.

---

## Future roadmap after Phase 3C passes

Immediate:
1. Prove Apply/persistence.
2. Prove selected 2560x1440 drives logical 2560x1440 DX11 gameplay.
3. Check geometry-range telemetry for true Hor+.
4. If projection is still 4:3, hook only the upstream projection/FOV constant/math.
5. Modernize HUD safe-area/layout.
6. Decide how to modernize frontend while preserving original menu behavior.

Renderer:
- controlled suppression of original main-scene D3D7 `DrawPrimitive` once DX11 remains authoritative;
- keep state/resource plumbing initially;
- eventually remove D3D7 scene/device dependency.

Later:
- frame-time hitch profiling;
- modern XInput/controller backend;
- mod/plugin APIs;
- editor/dev tooling ("Spidey Studio" concept);
- progressively more standalone/native ownership.

---

## REQUIRED WORKING STYLE FOR THE NEXT CHAT

The user expects active engineering, not just advice.

When asked to continue:
- inspect live repo state;
- implement the next bounded batch;
- update `docs/CURRENT_STATUS.md` **during the work**;
- push small commits every few changes;
- include exact commit hashes in progress notes;
- only ask the user to test at a meaningful runtime boundary;
- do not make them retest tiny changes one at a time if a larger safe batch can be statically grounded;
- do not fabricate reverse-engineering facts;
- preserve a known-good fallback when crossing risky renderer/device boundaries.

### Disconnect protocol

If the chat errors:
1. inspect live `dev` HEAD;
2. inspect the latest commits;
3. read the tail of `docs/CURRENT_STATUS.md`;
4. compare it with this handoff;
5. identify what landed and what did not;
6. immediately document recovery;
7. continue from the committed frontier.

**The repo is the checkpoint. Chat text is not the checkpoint.**
