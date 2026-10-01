# Spider-Man 2000 PC Modernization — New Chat Handoff

**Date:** 2026-10-01  
**Active repository:** https://github.com/legentus/spidey-decomp  
**Active branch:** `dev`  
**Google Drive game-files folder:** https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx  
**Live source of truth:** live `dev` + `docs/CURRENT_STATUS.md`

> **MANDATORY STARTUP RULE:** Do not trust this file over a newer live repo. First fetch `dev`, inspect recent commits, and read the tail of `docs/CURRENT_STATUS.md`. The project has suffered repeated stream interruptions, so GitHub is the checkpoint and chat text is not.

## Exact current frontier

The latest frontend/audio/display batch is implemented and statically audited. It is now ready for a combined runtime validation pass.

Important recent commits, newest implementation chain first:

- `85e45c41841093d4e489eb5dda12d4b107cb8ee9` — `audio: align stereo value with six-row layout`
- `c296af57c6af30c64f2db3f0941b0c68f485ea24` — `display: preserve selected window mode during startup`
- `e04f9b9e2d677152ebcb0b8448c4c4afde9a3a59` — `audio display: harden live fallback and mode transitions`
- `5c3b1340f3a1ce436fd4068f8680bd67cdc46234` — `display: stabilize mode transitions and menu sizing`
- `cb0d64ad3754bccec48817b9135b0b7d29bbdce8` — `renderer11: correct exclusive mode transition order`
- `e7d9500b270fba9218ce5ded408f52dc3fcfbb8f` — `display: connect window modes to DX11 swap chain`
- `c1c6c3a630677225dd13468a62fe0b8717f3eef7` — `renderer11: bump ABI for window mode control`
- `b8853c19d5ccf66d09f2352905719a4c110c98c4` — `display: add persistent fullscreen borderless windowed setting`
- `b0eb4f66df689fa75001e418766b9d12b6618de7` — `audio: align six-row text sliders and hit regions`
- `117b6b0fa6b03084943d1c827fd1dfd00bdf8b22` — `audio: apply output device live without invalidating Bink`
- `685b00ff9a95c034612951e96b88863a5c4b2a3d` — `frontend: unify modern canvas and remove audio row offset`

Documentation checkpoint before this refresh:
- `f5ff403c3ae4568ddd821353cb0d3f771bae5a72` — `docs: checkpoint frontend settings test batch`

## User requirements that are authoritative now

1. **Proper widescreen / Hor+**
   - Current widescreen work must be true widescreen, not stretched 4:3.
   - Earlier moving-background warp/distortion is confirmed fixed.
   - Hor+ side-plane culling synchronization is implemented.
   - Remaining visual validation / 2D layout work should continue after the current frontend/audio/display batch is validated.

2. **Mouse**
   - User explicitly confirmed the post-level menu mouse-location / hover bug is fixed.
   - Preserve that behavior. Do not regress the coordinate fix.

3. **High-FPS game speed**
   - Higher FPS still feels sped up.
   - Gameplay `Logic` vs present-rate telemetry is implemented.
   - The prior crash session never reached gameplay, so no gameplay timing conclusion is valid yet.
   - Preferred architecture remains fixed-step simulation + independent render/interpolation unless runtime evidence disproves it.

4. **Audio output**
   - Output selection must apply **live without restarting the game**.
   - `(System Default)` remains the default policy.
   - The old live restart could leave Bink with a stale DirectSound backend and caused a crash path inside `binkw32_.DLL`.
   - New implementation retains the old DirectSound object for Bink while the game SFX path switches, then safely rebinds Bink when no movie handle is active.
   - This live-safe implementation now needs runtime validation.

5. **Main-menu FPS**
   - Retail main menu is intentionally ~30 logical FPS.
   - User wants uncapped/high-refresh menu rendering.
   - Do not simply run old shell logic uncapped; preserve logical cadence and decouple rendering/interpolate visual state.

6. **Frontend/settings resolution**
   - User does not want a visible/logical 640x480 frontend underneath the selected modern resolution.
   - The selected modern logical resolution is now authoritative for frontend coordinates.
   - The hidden D3D7 producer may still use a compatibility backing where retail D3D7 cannot create the selected size (runtime-proven 2560x1440 D3D7 failure); that hidden backing must never define visible menu/text/slider/mouse coordinates.

7. **Display modes**
   - Display Settings must expose:
     - Fullscreen Exclusive
     - Borderless
     - Windowed
   - This is implemented as a fifth-row Display menu with Display Mode on row 3 and Apply on row 4.
   - Fullscreen Exclusive uses real DXGI `SetFullscreenState(TRUE)`, not fake borderless fullscreen.
   - WindowMode persists in `spidey-modern-video.ini`.
   - Needs runtime validation.

## Audio settings screenshot regression and fix

The user's screenshot showed:
- menu text shifted upward;
- sliders/arrows still at their old hard-coded Y positions;
- Stereo value independently offset;
- Output row still too low/off-screen.

Root cause:
- the first six-row workaround moved only `CMenu::mY`;
- `DrawSlider`, slider mouse logic, and the separately drawn Stereo/Mono value use independent hard-coded positions.

Current correction moves the entire authored Audio layout consistently by one 20-unit row:
- CMenu rows: -20
- all three slider draw calls: -20
- slider mouse hit logic: -20
- standalone Stereo/Mono value: -20

Relevant commits:
- `b0eb4f66...`
- `85e45c4...`

Do not reintroduce a text-only offset.

## Frontend logical-canvas correction

Commit:
- `685b00ff9a95c034612951e96b88863a5c4b2a3d`

Behavior:
- shell's old 640x480x16 request no longer becomes the active visible/logical frontend canvas when a modern mode is selected;
- frontend mouse bounds/canvas helpers prefer the modern logical width/height;
- selected modern dimensions remain authoritative;
- retail D3D7 2560x1440 remains a verified dead end and may be remapped internally to 1920x1440 as a hidden producer only.

## Live-safe audio switching

Key retail facts:
- `G_PDS @ 0x006B7920`
- `PCMOVIE_Init @ 0x0050B0F0`
- Bink initialized flag `0x00AC0BA0`
- active Bink handle `0x00AC0BA4`
- `shutdownDirectSound8 @ 0x005000F0`
- `DXSOUND_Init @ 0x005039F0`
- `Shell_SFXMusic @ 0x004977D0`

New behavior:
- selected endpoint is applied in-session;
- old DirectSound receives a temporary retained reference before retail shutdown if Bink is initialized;
- game DirectSound/SFX is rebuilt on the selected endpoint;
- if Bink is active, old backend remains valid;
- at a safe no-active-Bink point, retail `PCMOVIE_Init` rebinds Bink to current `G_PDS`, then retained old DirectSound is released;
- if endpoint creation + System Default fallback both fail, prior working DirectSound and prior selected-device index are restored.

## Display-mode implementation

Display menu is now:
1. Resolution
2. Aspect Ratio
3. Brightness
4. Display Mode
5. Apply

Modes:
- Fullscreen Exclusive
- Borderless
- Windowed

Renderer:
- ABI is now **8**.
- `SpideyRenderer11_SetFullscreenState` is declared, implemented, resolved, and mandatory.
- Leaving exclusive occurs before retail compatibility-producer rebuild.
- Renderer shutdown exits exclusive before releasing the swap chain.
- Persisted WindowMode is loaded before early renderer startup.
- Old unconditional startup force-to-borderless behavior is removed.

## Widescreen and renderer facts that must not be lost

- Moving/background warp bug is confirmed fixed. Do not reopen old UV/RHW investigation unless it regresses.
- `M3d_RenderSetup @ 0x00472DC0` is the upstream 3D projection path.
- aspect scalar global: `0x00550064`
- 16:9 scalar: 0.75
- Hor+ side-plane culling sync commit:
  - `e59539d4a501cc1269cf0b23988c15ede041bd80`
- exact 2D provenance:
  - `PCGfx_DrawQuad2D @ 0x00507470`
  - `PCGfx_DrawQPoly2D @ 0x00507910`
  - `PCGfx_DrawQPoly3D @ 0x00508550`
  - `DXPOLY_DrawPoly @ 0x00503100`
- 2D sidecar/provenance commits:
  - `a3421ff...`
  - `fe6f93a...`
  - `63f7e9b...`
- renderer shadow state carries exact `drawClass`.
- separate 2D/3D coordinate-range telemetry exists.

## Timing facts

Retail timing:
- `Vblanks @ 0x006B4CA0`
- `Pause @ 0x004E5D60`
- `PlayAway @ 0x004559D0`
- `Logic @ 0x00455400`
- gameplay Logic call site: `0x00455A8B -> 0x00455400`
- `Shell_MainMenu @ 0x00493990`

Telemetry:
- `spidey-decomp-timing.log`
- logs `timing_logic ... hz=`
- logs `timing_present ... hz=`

Previous crash session was frontend-only. Do not infer gameplay simulation rate from it.

## Next runtime test — exact requested validation

Run the normal update/build/test workflow and verify all of the following in one session:

1. Audio screen:
   - text/sliders/Stereo value/Output row are vertically aligned;
   - Output row is fully visible.

2. Audio live switching:
   - changing Output Device moves audio to the selected endpoint **without restarting the game**;
   - try more than one endpoint if practical;
   - return to System Default;
   - no Bink crash.

3. Stereo/Mono:
   - Stereo -> Mono -> Stereo remains stable.

4. Frontend/settings:
   - menu/settings use the selected modern logical resolution;
   - no visible/logical 640x480 frontend fallback;
   - settings and mouse hit regions line up.

5. Display Mode:
   - cycle and Apply Fullscreen Exclusive / Borderless / Windowed;
   - each mode visibly behaves as expected;
   - return to preferred mode;
   - relaunch and verify persistence.

6. Mouse:
   - post-level mouse alignment remains fixed.

7. Gameplay timing:
   - enter gameplay long enough for `timing_logic` and `timing_present` samples;
   - report whether gameplay still feels sped up.

Do not use F9. F10 is unnecessary for this test.

## After this batch validates

Resume the still-open priorities:
1. true Hor+ visual validation / remaining 2D layout work;
2. gameplay Logic Hz vs Present Hz analysis;
3. fixed-step/interpolation game-speed correction;
4. uncapped/high-refresh main-menu rendering while preserving shell logical cadence.

## Mandatory live-document / disconnect-safe protocol

This is a user requirement, not optional process advice.

During substantial work:
1. fetch live `dev` before editing;
2. read the tail of `docs/CURRENT_STATUS.md`;
3. update `docs/CURRENT_STATUS.md` **while working**, not only at the end;
4. commit meaningful source changes in small, descriptive commits;
5. add documentation checkpoints after major RE findings / before long risky work;
6. do not hold 20–30 minutes of irreplaceable RE only in chat context;
7. document rejected hypotheses and dead ends so they are not repeated;
8. prefer fewer, larger meaningful runtime tests rather than asking the user to test every small patch;
9. if a stream/input error occurs:
   - fetch live `dev`;
   - inspect recent commits;
   - read `CURRENT_STATUS.md`;
   - identify exactly what survived;
   - reconstruct only the unsaved tail;
   - checkpoint it immediately;
   - then continue.

**Rule:** repo is the checkpoint; chat is not.

## User workflow

Normal user-side flow is BAT-first:
- `UPDATE_AND_TEST_LATEST_BUILD.bat` for current combined update/build/install/test workflow.
- Supporting build documentation: `docs/BUILD_AND_INSTALL.md`.

## Project links

- Active repo: https://github.com/legentus/spidey-decomp
- Dev branch: https://github.com/legentus/spidey-decomp/tree/dev
- Current status: https://github.com/legentus/spidey-decomp/blob/dev/docs/CURRENT_STATUS.md
- This live handoff: https://github.com/legentus/spidey-decomp/blob/dev/docs/NEW_CHAT_HANDOFF.md
- DX11 migration: https://github.com/legentus/spidey-decomp/blob/dev/docs/DX11_MIGRATION.md
- Modern input/camera design: https://github.com/legentus/spidey-decomp/blob/dev/docs/MODERN_INPUT_CAMERA.md
- Upstream decomp: https://github.com/krystalgamer/spidey-decomp
- Google Drive full game/files: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
