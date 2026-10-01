# DRIVE LOG WORKFLOW + SHELL-LIFECYCLE TEXT FIX — READ FIRST (2026-10-01)

Google Drive project root:
https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

Runtime Logs folder:
https://drive.google.com/drive/folders/1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s

**Permanent workflow:** read the newest runtime log directly from the connected Drive `Logs` folder. Do not ask the user to re-upload routine logs into chat. New sessions should produce only `spidey-decomp.log`.

Latest analyzed Drive session was revision `b0c4910d007d7c3562552cceaed45dfb196c10bf`.

Observed:
- changing/applying resolution correctly shrank text;
- backing out of Display Options made it large again;
- user did not test mouse.

Grounded trace:
- Apply restore reached correct 2560x1440 scale: requested 256 -> effective 85, frontend=1.
- backing out caused another display-options call to mark frontend=0 and return effective scale to 256.

Correction:
- `0x006B78F4` is DirectDraw/windowed state, **not** shell-active state.
- real frontend boundaries are `PShell_Initialise @ 0x0048D790` and `PShell_Cleanup @ 0x0048D880`.

Fix:
- `02077c1218251814dcecfa7f02881a12f0752178` — frontend text policy is driven by actual shell lifecycle; modern display rebuilds no longer revoke frontend ownership.

NEXT TEST:
1. change resolution + Apply;
2. back out of Display Options;
3. verify text stays small in parent and other frontend menus;
4. optionally test mouse;
5. put only the new `spidey-decomp.log` in the Drive `Logs` folder.

Expected lifecycle log entries:
- `frontend_lifecycle_install ... initialise_calls=>0 ... cleanup_calls=>0`
- frontend navigation retains `frontend_active=1`
- actual gameplay transition logs `pshell_cleanup_post active=0`.

---

# PERMANENT SINGLE-LOG POLICY — READ BEFORE ALL OLDER TEST INSTRUCTIONS

The user has explicitly required that routine test logging be consolidated so they do not hit the attachment limit.

**From now on, request exactly one runtime attachment:**
- `spidey-decomp.log`

Do **not** ask for separate compat/draw/present/texture/input/timing/renderer11/input11/audio/camera/crash/dxerror logs. Historical instructions below that name multiple files are superseded.

The test launcher now creates a single uploadable runtime artifact containing:
- `[SESSION]` revision/build/executable identity;
- proxy categories such as `[COMPAT]`, `[DRAW]`, `[PRESENT]`, `[TEXTURE]`, `[INPUT]`, `[TIMING]`, `[AUDIO]`, `[CAMERA]`, `[DXERROR]`, `[CRASH]`, `[RUNTIME]`;
- `[RENDERER11]` lines;
- `[INPUT11]` lines.

Logging implementation frontier:
- `66d39d8f5ceb24d60cf87954e4059c940311b151`
- `ae8b8332d3fa94ee8258e1d77a74099e6c2c00db`
- `1d49799222c71696ee57b222fc33e3d142574c11`
- `897c4f8b6e711405beaff7585802bc3590331b90`
- `ea9b5d679d300a8f749be8f806171974c5ffdde8`
- `c87954c7616d18321a22be8846aa0e1134078fbe`

Routine timestamped session folders are now intentionally single-file for diagnostics.

---

# FRONTEND APPLY-SCALE + MOUSE-BOX FIX FRONTIER — READ FIRST (2026-10-01)

Latest runtime tested `56374190919f6a12ff8abbe6602fee4dcba75f1b`:
- no reported crash;
- changing resolution made text large until level -> frontend roundtrip;
- mouse was confined to an invisible upper-left rectangle, not just a bottom floor.

Root causes from logs/source:
- Display Apply temporarily classified the live shell as gameplay, setting effective text scale back to 256.
- raw mouse bounds were successfully full-client, but coordinates were pre-scaled to modern logical and then scaled again by retail PCSHELL.

Fixes:
- `b04a78b492dfe41842d42cf6017328b48d4138e6` — restore live frontend ownership immediately after Display Apply; map client mouse once into the retail PC canvas.
- `19eb5bca0824e31d09e66ccce737e9ebca653d30` — validator tags / comment cleanup.

NEXT TEST:
- change resolution and confirm text remains small immediately;
- move cursor to every edge/corner before and after a level transition;
- verify hover/click alignment remains exact;
- inspect compat/input logs for `display_apply_frontend_restore` and `basis=client_to_retail_pc_canvas`.

Do not restore the prior client->modern-logical mouse mapping; that was the upper-left box bug.

---

# FRONTEND CURSOR + RESOLUTION-AWARE TEXT FRONTIER — READ FIRST (2026-10-01)

Runtime revision `78cfba2b22fa6a4dc079f4ddd14f9ca1793e5f8e` did **not crash**. The normal-level transition regression is fixed.

Remaining user-visible issues from that runtime:
- post-level shell cursor still hit a horizontal lower bound;
- Audio Output text/row was still cut off;
- Display Mode values Fullscreen Exclusive / Windowed / Borderless were too large/clipped;
- user explicitly wants frontend text to shrink appropriately at higher resolution.

Implementation:
- `8ed74f4a8a54b8b884434813969e099d8ff817f5` — raw mouse bounds use live client size; visible cursor, hotspot, and hit tests share client->logical mapping.
- `ef7df27e9b24349ef4a7413464d50e99ec69d498` — frontend Mess_SetScale becomes resolution-aware relative to the 640x480 PC baseline, including text measurement/menu width calculations; gameplay scale is unchanged.

Expected examples:
- requested scale 256 at logical 1920x1080 -> ~113;
- requested scale 256 at logical 2560x1440 -> ~85.

**NEXT ACTION:** updater/build; validate Audio Output visibility, all Display Mode strings, then enter/leave a level and verify full downward cursor movement plus preserved hover/click alignment. Collect input + compat logs if anything remains wrong.

Do not undo DX11-authoritative rendering or the now-working level-transition compatibility fixes.

---

# TRAINING-PASS / LEVEL-TRANSITION FIX FRONTIER — READ FIRST (2026-10-01)

Runtime revision `8afcffd25e907f4fc34f8f2aaa8154e9a924e60a` now boots and plays training successfully. Two transition bugs remain and have been fixed in source:

1. **Normal-level start crash**
   - main DX11 gameplay is healthy;
   - fatal path is legacy PCMovie/DXinit touching a DirectDraw scene surface that had been lost by an earlier Exclusive interval;
   - previous restore helper returned early whenever current mode was no longer Exclusive.
   - Fix: `9c8a2c30696d2fcbfaa4af435168ae1202590cc9` always repairs lost legacy primary/scene surfaces before compatibility producers, independent of current window mode.

2. **Cursor horizontal floor after returning from training**
   - user had applied 2560x1440 Windowed;
   - internal retail display transition later overwrote selected output to 1920x1440 while pending modern selection stayed 2560x1440;
   - 16:9 logical canvas therefore collapsed to 1920x1080 and the existing mouse-bound sync clamped there.
   - Fix: `d7ef76af697d4c7cf9ac64e1a24bbbecd4123892` prevents retail compatibility transitions from owning the modern selected DX11 output.

Cleanup:
- `d8edc7e24bfa341145f100fbb4d3a09f3d51a494` removes a false PCTex D3D diagnostic caused by treating COM `Release()` refcount 1 as an HRESULT.

**NEXT ACTION:** normal updater/build, then:
- boot;
- use desired 2560x1440 mode;
- enter training;
- return to menu and check full cursor movement + hover alignment;
- launch the same normal level that crashed;
- if it enters, play 30-60 seconds;
- collect compat/present/draw/dxerror/input/renderer11 logs.

Do not undo DX11-authoritative rendering. Training runtime proves the main renderer works. The current fixes are ownership/lifecycle fixes for remaining legacy compatibility producers and selected-output state.

---

# STARTUP-CRASH FIX FRONTIER — READ FIRST (2026-10-01)

The first DX11-authoritative runtime at revision `5411665738851098256555b12c3b3ac048b12cfa` crashed during the first splash movie.

Root cause is now grounded and fixed:
- renderer helper falsely reported an empty frame as ready;
- proxy immediately entered DXGI Exclusive on frame 1;
- no shadow SRV existed, so DX11 presentation was rejected;
- startup Bink still depended on DirectDraw movie/scene surfaces, which were lost by the premature Exclusive takeover.

Fix chain:
- `3f3d8fd6e6e8bf88a6cc596c0e8a20835277ba6b` — complete non-empty DX11 frame required for takeover.
- `a4e005885f0eb1780b3366a990e20ffc8db3fcc2` — Bink/movie surfaces block takeover; movie frame releases Exclusive before retail DirectDraw work.
- `ac275ef13e289e687eb3093c424d9d5ccfbd9e76` — restore retail primary/scene surfaces after Exclusive release.

**NEXT ACTION:** user should run the normal `UPDATE_AND_TEST_LATEST_BUILD.bat` and first verify only that the game boots through the splash movies to the menu. If it does, continue the wider Windowed/Borderless/Exclusive test. Request fresh present/renderer11/draw/compat/dxerror logs.

Expected first-frame evidence:
- empty frame -> `presentable=0`
- Exclusive remains deferred
- movie gate reports `movie_blocks=1` while Bink is active
- after movie ends, a complete non-empty DX11 frame becomes `presentable=1` and may acquire Exclusive.

Do not undo the DX11-authoritative pivot. PCMovie/Bink is explicitly recognized as a remaining legacy producer until its surface path is migrated to DX11.

---

# DX11-AUTHORITATIVE FRONTIER — READ THIS FIRST (2026-10-01)

**Current dev HEAD before this handoff refresh:** `2ab250f717003584cc1fed17a32225692e2c6db6`

The user explicitly chose to stop treating D3D7 as the main renderer after the first true Fullscreen Exclusive test exposed the DXGI/D3D7 ownership conflict. The project is now in a deliberate renderer-replacement phase.

## Current architecture

After the first complete replayable DX11 frame:

- DX11/DXGI own the visible main frame in Windowed, Borderless and Fullscreen Exclusive.
- main-scene D3D7 `BeginScene/EndScene`, clear, fixed-function state forwarding, accepted main draws, main-surface Blt and retail Flip are no longer required for the visible frame.
- D3D7 remains temporarily only for still-unported offscreen/resource compatibility paths.
- a real D3D7 fallback scene opens lazily only if an unsupported/offscreen path genuinely needs it.
- true Exclusive is deferred until the first authoritative DX11 frame, preventing startup from invalidating DirectDraw surfaces before the bypass is active.
- old F9/F10 D3D7 reference/debug toggles are retired.

## Commits that define this frontier

- `df5cf4bbbfc00ca340b750325a747780202866ac` — renderer: make DX11 authoritative for main scene
- `566aec873acf194c21e1ca609b52ae3b51732c8c` — renderer: defer exclusive until DX11 frame is authoritative
- `f194ca2a924644dd94225235de6988f5cf96e4bb` — renderer: stop forwarding main-scene state to D3D7
- `0b3c6cce0b9e1715296557a47172740b9cd8881e` — renderer: retire D3D7 reference toggles
- `2ab250f717003584cc1fed17a32225692e2c6db6` — docs: checkpoint DX11-authoritative renderer pivot

The triggering failure and exact evidence are documented in `docs/CURRENT_STATUS.md`.

## Why this was safe enough to promote from preview

The last runtime immediately before the Exclusive crash showed complete main-frame capture at the sampled frontier:

- frame 2400: 121/121 draws submitted to DX11, `shadow_skip=0`, `missing=0`
- frame 2640: 104/104 submitted, including 2D + 3D, no missing/offscreen fallback
- frame 2760: 134/134 submitted, `shadow_skip=0`, `missing=0`

DXGI itself successfully entered exclusive. The crash came afterward because legacy DirectDraw/D3D7 surfaces became `DDERR_SURFACELOST`. The migration therefore removes the visible frame's dependency on those surfaces rather than trying to keep two display owners synchronized.

## Final hardening after the initial DX11-authoritative checkpoint

- `f4385217703d0a4e5669e17cd0ea95dbd337d76a` — metadata tags for new migration helpers.
- `4c726ebd6f4ecf0c1996bc64512596935228fc18` — release DXGI exclusive before any legacy compatibility rebuild, install hooks on replacement D3D7 objects, then reacquire Exclusive.
- `0de70eaf8983a14c94c1b67138fd4406ad52f927` — CURRENT_STATUS checkpoint for this hardened runtime frontier.

Static audit at `4c726eb` passed structural delimiter checks, verified the 51-field draw telemetry argument count, verified no F9/F10 handlers remain, and verified replacement-object hooks are installed before Exclusive reacquisition.

## NEXT ACTION — do not blindly extend before this runtime validation

Have the user run the normal:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Test one focused renderer pass:

1. boot normally and confirm frontend appears;
2. Apply Borderless;
3. Apply Windowed;
4. Apply Fullscreen Exclusive and remain there while navigating menus;
5. enter gameplay for at least roughly one minute;
6. relaunch while Exclusive is persisted to test deferred-exclusive startup;
7. switch Exclusive -> Windowed -> Exclusive once;
8. if practical, return from gameplay and verify the already-fixed post-level mouse alignment.

Do **not** use F9/F10; those reference toggles are intentionally retired.

Request the fresh session logs, especially:

- `spidey-decomp-draw.log`
- `spidey-decomp-present.log`
- `spidey-decomp-compat.log`
- `spidey-renderer11.log`
- any `spidey-decomp-dxerror*.log` / crash log

Success signals:

- `dx11_authoritative=1`
- main `d3d7_suppressed > 0`
- `missing=0`
- `shadow_skip=0`
- `d3d7_fallback=0`
- `fallback_scene_begins=0`
- DX11 shadow presentation active and retail Flip inactive after warmup
- Exclusive log entered after `dx11_authoritative_ready`
- no `DDERR_SURFACELOST`

## After this passes

Continue renderer removal in this order:

1. direct DX11 texture ownership/source upload instead of D3D7-surface mirroring as source of truth;
2. DX11-native transient/offscreen render targets and copies;
3. drive all legacy fallback counters to zero;
4. remove DirectDraw display ownership;
5. finally remove the real D3D7 device/init once resource/object identity dependencies are gone.

## Mandatory workflow

- Live-update `docs/CURRENT_STATUS.md` while working.
- Make small meaningful `dev` commits every few minutes / at each grounded milestone.
- Repo/status outrank chat transcript on recovery.
- On an interruption, inspect live `dev` + `CURRENT_STATUS.md`, identify surviving commits, reconstruct only the unsaved tail, checkpoint immediately.
- Preserve confirmed regression guards: moving-background warp fix and post-level mouse alignment fix.
- Prefer a few meaningful runtime tests, not speculative build spam.

---

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
