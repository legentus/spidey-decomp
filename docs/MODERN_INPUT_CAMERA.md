# Modern Input + Camera Architecture

**Project:** Spider-Man 2000 PC modernization  
**Status:** design / RE checkpoint  
**Date:** 2026-09-30

## Goals

Modernize the game's input and camera as first-class engine systems:

- full gamepad support;
- controller hot-plugging;
- left-stick movement;
- independent right-stick camera;
- mouse free-look;
- proper triggers/bumpers/stick buttons;
- configurable deadzones, sensitivity and inversion;
- persistent per-action remapping;
- dynamic keyboard/Xbox/PlayStation/generic button UI;
- rumble;
- modern camera feel that is not permanently constrained by the original 2000 camera design.

The original systems are useful compatibility anchors, not mandatory final architecture.

---

## Existing retail input path

Relevant retail anchors from the original symbol database:

- `DXINPUT_SetupController = 0x00501890`
- `DXINPUT_PollController = 0x00501E50`
- `DXINPUT_GetControllerButtonState = 0x00501FB0`
- `DXINPUT_GetNumControllerButtons = 0x00502210`
- `PCINPUT_GetMappedStates = 0x0050A190`
- `PCINPUT_GetControllerMappingForAction = 0x0050A450`
- `PCINPUT_SetControllerMappingForAction = 0x0050A4E0`
- `PCINPUT_GetControllerDirections = 0x0050A9D0`
- `Pad_Update = 0x00505720`

Current decomp evidence:

- `PCINPUT_GetMappedStates` combines keyboard/controller input into an action-bit mask.
- Controller mappings use the same action IDs as keyboard mappings.
- `Pad_Update` translates that bit mask into the game's `SControl` button state.
- Existing controller mapping storage already supports per-action get/set/default operations.
- Existing menu code has a controller configuration screen concept, but current labels are legacy strings such as `button N`.
- Existing controller direction handling uses coarse thresholds around `gControllerX/gControllerY`.
- `SControl` already contains raw/processed analogue movement and analogue aim fields, but the current decomp implementation does not yet provide a complete modern two-stick path.
- Current modernization patches do not replace the full controller/action pipeline yet.

This gives us two distinct layers:
1. compatibility action injection for existing gameplay;
2. new high-resolution analogue state for modern movement/camera systems.

---

## Recommended modern input architecture

### 1. Modern input backend

Keep modern device APIs out of the preserved VC6-era proxy headers.

Use a modern x86 helper DLL, analogous to `spidey_renderer11.dll`, behind a small versioned C ABI.

The helper owns:
- device enumeration/hot-plugging;
- controller family detection;
- normalized buttons;
- left/right stick axes;
- trigger axes;
- deadzones;
- rumble;
- device identity;
- last-active-device tracking.

Do not commit to a specific provider/library until implementation research chooses the best supported backend. The proxy should depend only on the ABI.

### 2. Normalized frame state

Expose one stable frame snapshot, conceptually:

- `move_x`, `move_y`;
- `camera_x`, `camera_y`;
- `left_trigger`, `right_trigger`;
- digital button bitset;
- connected/device-family flags;
- sequence/frame counter.

Raw provider button numbers must not leak into gameplay code.

### 3. Action binding layer

Map normalized physical controls to named game actions.

Initial compatibility route:
- hook/wrap `PCINPUT_GetMappedStates @ 0x0050A190`;
- merge modern controller digital actions into the action mask the retail game already consumes;
- preserve keyboard/mouse operation.

This minimizes initial risk because existing gameplay keeps receiving its normal action bits.

Longer term:
- migrate gameplay to explicit named actions where useful;
- keep legacy mask generation only as a compatibility adapter.

### 4. Analogue separation

Never encode right-stick camera into the legacy digital action mask.

Maintain separate modern analogue channels:
- left stick -> movement intent;
- right stick -> camera intent;
- mouse relative delta -> same camera-intent layer.

Movement and camera can therefore evolve independently of old DirectInput joystick assumptions.

### 5. UI/glyph layer

UI should request the glyph for a **bound action**, not hard-code a key or controller button.

Examples:
- action `Jump` -> keyboard key;
- action `Jump` -> Xbox A;
- action `Jump` -> PlayStation Cross.

Track last active input family and swap glyph sets automatically.

Controller configuration should eventually display semantic names/glyphs instead of `button N`.

---

## Existing camera anchors

Useful retail symbols:

- `CCamera::PushMode = 0x00416720`
- `CCamera::PopMode = 0x00416780`
- `CCamera::SetCamAngle = 0x004178E0`
- `CCamera::SetCamXZDistance = 0x004179F0`
- `CCamera::SetCamYDistance = 0x00417A70`
- `CPlayer::SetCamAngleLock = 0x004B9E10`
- `CPlayer::EnterLookaroundMode = 0x004C3580`
- `CPlayer::ExitLookaroundMode = 0x004C3810`
- `CPlayer::SetupLookaroundCamera = 0x004C38A0`
- `CPlayer::PutCameraBehind = 0x004C64A0`

Existing camera modes include:
- NORMAL;
- LOOSE;
- USER;
- LOOKAROUND;
- multiple scripted/boss/special modes.

`CPlayer::PutCameraBehind` is a concrete legacy recenter path that aligns camera heading to Spider-Man.

---

## Camera plan

### Stage A — compatibility free-look

Fastest low-risk proof:
- feed normalized mouse/right-stick camera intent;
- keep ordinary retail camera positioning/collision;
- suppress or gate forced `PutCameraBehind` recenter during normal player control;
- modify yaw/pitch through known camera state;
- yield to scripted/boss/cutscene modes.

This is a prototype, not a promise to keep the original camera feel.

### Stage B — dedicated modern gameplay camera

If Stage A still feels old or constrained, move ordinary gameplay to a dedicated camera controller.

The modern controller should own:
- yaw;
- pitch;
- orbit distance;
- pivot/shoulder offsets;
- smoothing;
- acceleration/deceleration;
- recenter policy;
- vertical angle limits;
- movement-relative/camera-relative steering policy;
- camera collision/obstruction response.

Retail camera code can then be used only for:
- scripted ownership signals;
- boss/cutscene/special cameras;
- selected collision/trace helpers if they remain useful.

### Camera ownership model

Use an explicit ownership state:

- `MODERN_GAMEPLAY`
- `RETAIL_SCRIPTED`
- `RETAIL_BOSS_SPECIAL`
- `MENU/CINEMATIC`

When retail enters a scripted camera:
- suspend modern camera updates;
- allow retail to own camera transform.

When scripted ownership ends:
- seed the modern yaw/pitch/position from the final retail camera;
- resume smoothly rather than snapping behind Spider-Man.

This avoids fighting the original camera every frame.

### Desired settings

Plan for:
- mouse sensitivity X/Y;
- controller camera sensitivity;
- controller deadzone;
- invert X/Y;
- camera recenter: off / soft / classic;
- recenter delay and strength;
- camera distance;
- optional FOV;
- optional camera-relative movement;
- optional shoulder/pivot tuning.

---

## Implementation order

1. Finish current display Apply + frontend mouse-return validation.
2. Complete Phase 3E D3D7 main-scene raster isolation proof.
3. Add modern input helper ABI and normalized state.
4. Inject controller actions at `PCINPUT_GetMappedStates`.
5. Add menu navigation + hot-plugging + rumble.
6. Add right-stick/mouse camera intent.
7. Prototype Stage A free-look.
8. Evaluate feel.
9. If still constrained, implement Stage B dedicated modern camera.
10. Add dynamic glyph/remapping UI and polish.

---

## Design rule

**Do not make the modern camera architecture depend on preserving the original camera behavior.**

Reuse retail systems only when they improve compatibility or feel. The project may replace ordinary gameplay camera ownership entirely while preserving scripted/cinematic behavior.


## Retail PC analogue path — confirmed limitation

Static disassembly of the original PC executable changes an important assumption.

### `Pad_Update @ 0x00505720`

Retail behavior:
- obtains two action masks from `PCINPUT_GetMappedStates`;
- updates the game's digital `SButton` entries;
- updates duplicate player-facing digital actions;
- expires vibration;
- does not populate `SControl::RawAnalogueMove*`, `RawAnalogueAim*`, `AnalogueMove*` or `AnalogueAim*`.

### `PCINPUT_GetMappedStates @ 0x0050A190`

Retail controller behavior:
- polls DirectInput controller X, Y and POV;
- uses approximately +/-250 X/Y thresholds;
- emits only digital up/down/left/right action bits;
- maps POV/hat direction to the same bits;
- evaluates configured controller buttons and ORs their action bits into the mask.

Therefore the old PC port does not preserve stick magnitude through the normal gameplay action path.

### Architectural consequence

Modern controller work should have two outputs:

1. **Compatibility digital actions**
   - semantic modern buttons / d-pad can be translated into the existing retail action mask for current gameplay and menu code.

2. **Native modern analogue channels**
   - left-stick magnitude goes to a new movement-intent path;
   - right-stick magnitude goes to the modern camera-intent path;
   - mouse relative delta joins the same camera-intent layer.

Do not quantize modern stick axes to the old +/-250 direction bits except when intentionally generating legacy compatibility actions.

The runtime bridge now logs the inherited `SControl` analogue fields beside modern helper state to verify their actual live behavior before any analogue injection.

## Broad controller provider direction

Phase 0 deliberately uses dynamically loaded XInput because it adds no new runtime dependency and cleanly validates the ABI.

For the later broad-provider phase, SDL3 is the leading candidate because its gamepad layer provides:
- standardized semantic gamepad positions instead of arbitrary button numbers;
- device mappings and user-provided mapping support;
- hot-plug handling;
- face-button label queries suitable for Xbox vs PlayStation prompt UI;
- rumble and optional sensors;
- current official Windows x86 builds, which fit this 32-bit game process.

Keep this as a provider choice behind the existing `spidey_input11` ABI. Gameplay, camera and UI code must not depend directly on SDL types.


## Camera ownership RE — baseline gameplay mode identified

The legacy enum name is misleading in this PC build:

**`CAMERAMODE_DEMO (3)` is the baseline mode used by the ordinary Spider-Man gameplay camera presets.**

Grounding from original retail machine code:

- `CPlayer::SetFallingCamera @ 0x004BF5D0`
- `CPlayer::SetSwingCamera @ 0x004BF690`
- `CPlayer::SetFloorCamera @ 0x004BF720`
- `CPlayer::SetWallCamera @ 0x004BF7A0`
- `CPlayer::SetCeilingCamera @ 0x004BF820`

Each routine applies its camera offset/distance preset only when the active camera's `mCameraMode` at offset `0x2A0` equals 3.

This changes the initial ownership strategy:

### Initial modern-camera ownership candidate

- mode 3 / `DEMO`: modern ordinary-gameplay candidate;
- all other modes: retail-owned by default until classified by runtime telemetry/RE.

Do not infer ownership from enum names alone.

### Forced recenter

`CPlayer::PutCameraBehind @ 0x004C64A0` is confirmed as a real forced-recenter path:
- normal player flow computes player heading then calls `CCamera::SetCamAngle`;
- oriented/crawl flow derives heading from surface orientation;
- mode-3-specific logic also adjusts vertical distance in certain player states.

The modern camera prototype should gate this path only while modern mode-3 gameplay ownership is active, not globally.

### Passive ownership telemetry

The proxy now creates `spidey-decomp-camera.log`, containing:
- camera pointer;
- current and pushed mode;
- heading values;
- camera/focus positions;
- distance/zoom/collision values;
- modern right-stick intent beside retail camera state.

This is the runtime evidence source for classifying every other camera mode before Stage A or Stage B camera control is enabled.


## Passive semantic/action discovery

Retail controller configuration uses 11 action records at `0x00568690`, stride `0x1C`:

- `+0x00` action bit;
- `+0x04` inline 16-byte action label;
- `+0x14` keyboard binding;
- `+0x18` controller binding.

The first four joystick rows are movement directions and are not button-remappable in the retail joystick-config UI.

The modernization proxy now logs all 11 records once at runtime. This is the preferred source for assigning semantic action names to the initial compatibility binding layer. Do not invent names from bit values if the retail table supplies a label.

## Mouse camera-intent source

No new mouse API is required for the first free-look prototype.

The existing Alt+Tab compatibility wrapper already intercepts all calls to retail `DXINPUT_PollMouse @ 0x00501CC0`.

Original retail behavior:
1. buffered DirectInput events yield relative X/Y deltas;
2. `DXINPUT_PollMouse` accumulates them;
3. `PCINPUT_UpdateMouse @ 0x0050A8A0` later scales and integrates them into the absolute shell cursor.

The wrapper now passively mirrors the pre-integration relative deltas while preserving the retail return values exactly.

Use this source for mouse `CameraX/CameraY` in Stage A:
- resolution independent;
- no screen-edge limitation;
- already integrated with existing focus/reacquire handling;
- menu cursor behavior remains separate.

Camera telemetry logs mouse and controller intent side by side and emits throttled `input_intent` samples.

## Legacy recenter rejection in user camera modes

Original `CCamera::SetCamAngle @ 0x004178E0` refuses to apply requested heading changes in:

- LOOSE (15);
- USER (16);
- LOOKAROUND (17).

This is relevant because `CPlayer::PutCameraBehind` performs ordinary forced recentering through `SetCamAngle`.

Possible Stage-A experiment after passive runtime classification:
- keep ordinary mode-3 camera behavior as the baseline;
- temporarily enter a suitable user-controlled mode, or gate the recenter call, while applying modern yaw/pitch intent;
- compare feel/collision/script behavior.

This is only a compatibility experiment. Do not make the final modern camera dependent on USER/LOOSE/LOOKAROUND if those modes inherit undesirable legacy constraints.


---

## Stage A mode-3 orbit prototype — implemented 2026-10-04

The pause/settings UI milestone is complete and camera work is now active.

### New exact retail RE

Canonical retail SpideyPC.exe disassembly grounds the ordinary gameplay seam inside:

- CCamera::AI @ 0x00417CB0.

The mode dispatch reads mCameraMode @ +0x2A0. For mode 3:

- dispatch entry: 0x00418412;
- ECX carries CCamera*;
- direct call at 0x00418414;
- retail target: 0x00418E00;
- execution then rejoins at 0x00418456.

The target at 0x00418E00 is the retail mode-3 normal gameplay camera generator used by the already-confirmed floor/wall/ceiling/swing/falling presets.

Crucially, the rest of CCamera::AI still runs after that call:

- 0x00416B10 camera post-processing/collision/orientation stage;
- shake/orientation work;
- final CCamera::LoadIntoMikeCamera @ 0x00416A20.

This provides a narrow Stage-A hook: replace only the single mode-3 generator call, while keeping the retail camera pipeline before and after it.

### Mode-3 orbit inputs

Immediately before mode dispatch, retail copies the current camera distance values into:

- XZ distance: 0x00548860;
- Y distance: 0x00548864.

For mode 3 it derives:

- radial 3D distance: 0x0054885C = sqrt(xz^2 + y^2);
- vertical orbit angle: 0x00548858 = ratan2(-y, xz).

CM_Normal @ 0x00418E00 consumes:

- 0x00548858 vertical angle;
- 0x0054885C radial distance;
- camera angles around +0x234/+0x236/+0x238;
- focus/pivot data already produced by retail.

It then builds the desired camera position at +0x24C and preserves the normal focus path.

Therefore the first modern orbit prototype can change yaw and pitch without replacing the retail position/collision system:

- horizontal orbit: own camera->field_236 @ +0x236;
- vertical orbit: own a modern Y-distance, then recompute 0x548858/0x54885C;
- preserve retail XZ distance;
- preserve the retail post-mode collision/orientation stage.

### Implemented source boundary

Implementation commits:

- e15892a8c76eef180a932b25d6bfb13e62795adb — feat: add mode-3 modern orbit camera prototype
- d2d546551255be6d0b83919236d0aed7098dcbd3 — guard: reset modern camera ownership cleanly

Hook:

- call site: 0x00418414;
- expected retail target: 0x00418E00;
- replacement: SpideyModernMode3Camera.

The wrapper uses an ABI-compatible __fastcall(CCamera*, void*) signature because the preserved VC6 build does not support an explicit __thiscall function-pointer typedef.

### Ownership policy

The prototype is intentionally conservative:

1. Only mode 3 can be modern-owned.
2. Until the player actually moves mouse/right stick, mode 3 runs completely retail.
3. On first camera intent:
   - seed modern yaw from live field_236;
   - seed modern vertical distance from live 0x00548864;
   - begin modern gameplay ownership.
4. Other retail camera modes remain untouched.
5. Ownership is dropped on:
   - active camera pointer change;
   - mode changing away from 3;
   - camera detach.
6. Returning to mode 3 does not restore stale modern state; a new input intent reseeds from the then-current retail camera.

This means cinematics, bosses, special cameras, lookaround and other non-mode-3 ownership remain retail-controlled by default.

### Input policy

Mouse:
- existing relative DirectInput deltas from the DXINPUT_PollMouse compatibility wrapper;
- no screen-position or Win32 mouse API;
- current initial yaw scale: 3 angle units per raw mouse X count;
- current initial pitch scale: 2 Y-distance units per raw mouse Y count;
- mouse-up is mapped to look-up.

Controller:
- existing normalized Input11 right stick;
- XInput Y remains positive-up;
- current initial full-stick yaw speed: 32 / 4096-angle units per completed frame;
- current initial full-stick pitch speed: 7 Y-distance units per completed frame.

Input is consumed at most once per completed-frame snapshot even if camera AI is evaluated more than once.

### Vertical limits

Initial Stage-A Y-distance clamp:

- minimum: -480;
- maximum: +260.

Retail XZ distance is preserved. Because the effective pitch is derived from both XZ and Y distance, the exact angular limits naturally vary somewhat with the active retail distance preset.

This is intentional for the first compatibility prototype. A dedicated Stage-B camera can own explicit pitch angles later.

### Telemetry

Install marker:

modern_camera_install installed=1 call=0x00418414 retail_mode3=0x00418E00 ownership=mode3_only ... collision=retail_after_mode3

Ownership:

- modern_camera event=acquire ...
- modern_camera event=release ...

Updates:

modern_camera event=update ... yaw=... retail_yaw=... y_dist=... xz_dist=... vertical_angle=... mouse=... stick=... retail_overrode_yaw=...

retail_overrode_yaw=1 is especially important: CM_Normal has a conditional late yaw recomputation when its auxiliary angle fields are nonzero. This telemetry tells us whether that legacy behavior is fighting the modern yaw on floor/wall/ceiling states.

### First runtime success criteria

The first runtime is a proof of the seam, not final feel tuning.

Success means:

- moving the mouse left/right or right stick horizontally genuinely orbits the camera;
- yaw can continue through a full 360-degree range in ordinary mode-3 gameplay;
- mouse/right-stick vertical input pitches the camera up/down within the clamp;
- stopping input preserves the chosen orbit instead of immediately snapping behind Spider-Man;
- basic movement/swinging remains playable;
- the existing retail collision stage prevents catastrophic wall penetration in ordinary cases;
- entering a non-mode-3/scripted camera releases modern ownership instead of fighting it.

Sensitivity, inversion, recenter behavior, wall/ceiling special handling, shoulder offset and smoothing are explicitly follow-up tuning/Stage-B work.


---

## Camera sensitivity, frame-pacing cleanup, and camera-forward web targeting — 2026-10-04

The first Stage-A orbit-camera runtime test was a strong success.

User report:
- horizontal orbit works;
- vertical orbit works;
- no notable camera-control problems were reported.

Runtime telemetry supports that result:
- modern mode-3 ownership acquired normally;
- sampled camera updates kept the requested yaw;
- retail_overrode_yaw remained 0 in the examined orbit samples.

### Persistent camera sensitivity

A single Camera Sensitivity setting now scales both:
- relative mouse camera X/Y;
- Input11 right-stick camera X/Y.

Configuration:
- default: 100 percent, exactly matching the first successful prototype feel;
- range: 25..200 percent;
- step: 5 percent;
- persisted in spidey-modern-video.ini:
  - section: Controls
  - key: CameraSensitivityPercent.

Pause -> Options is now six rows:

0. Options (disabled heading)
1. UI Scale
2. Text Scale
3. Camera Sensitivity
4. Apply Settings
5. Back

Apply Settings commits and persists camera sensitivity alongside UI/Text scale. The next camera input uses the new value immediately; no renderer/device rebuild is needed.

Implementation commits:
- 2ca650606bf1c851859c699be7e019b131f69e5b — input: add persistent camera sensitivity state
- fdfe39ec6000856c655dbf137bf90c66d9c59aa8 — input: persist camera sensitivity setting
- f6fcaa31edb7ae5cc8ea8cb8df2091ecfd302413 — pause: add camera sensitivity option
- fa0f7e230c210019d8c6066076585fb84144b8ee — pause: wire camera sensitivity controls
- fde069d43a284c27ca385ef427a6751f3b5bd06f — camera: apply configurable sensitivity

### Periodic frame-hitch root cause and mitigation

The successful camera runtime also exposed visible frametime hitching plus a larger freeze every several seconds.

Two old renderer-validation diagnostics were still forcing synchronous readbacks on the normal gameplay path approximately every 120 frames:

1. Proxy / DirectDraw surface sampling:
   - SpideyLogSurfaceState
   - GetDC
   - nine GetPixel calls
   - ReleaseDC.

2. Renderer11 shadow-target sampling:
   - copy nine pixels into a D3D11_USAGE_STAGING texture;
   - blocking D3D11_MAP_READ.

At roughly 60 FPS, the 120-frame cadence is approximately two seconds and is a strong match for the reported repeating freeze.

These readbacks are now default OFF. They remain available only as explicit diagnostic opt-ins:

- SPIDEY_DIAG_SURFACE_READBACK=1
- SPIDEY_RENDERER11_DIAG_READBACK=1

Ordinary metadata/timing telemetry remains enabled.

Implementation:
- 61419a368c7a7e962673a74f7052fe55465254ff — perf: disable periodic DX11 diagnostic readback
- 6cb305796f272043654ed3819bb92e7c6c53249a — perf: disable periodic DirectDraw pixel sampling

This is a high-confidence fix for the large periodic stall, but runtime validation is still required before declaring all frametime hitching solved.

### Camera-forward web auto-aim RE

User requirement:
webbing an enemy should be selected by the direction the camera is facing rather than Spider-Man's body heading.

Exact retained-function RE:

- CPlayer::CheckWebShot = 0x004C0510
- CPlayer::SelectAutoAimTarget = 0x004C5AA0
- CPlayer::FireWeb = 0x004C5DD0
- CPlayer::SelectTargetBaddy = 0x004C8410
- CPlayer::SelectTargetSwitch = 0x004C8570.

Inside SelectAutoAimTarget:
- call site 0x004C5B2F calls SelectTargetBaddy @ 0x004C8410.

Inside SelectTargetBaddy:
- retail keeps its candidate eligibility checks;
- range weighting remains retail;
- LOS/final acceptance remains retail;
- candidate-relative world vectors are transformed through player->field_89C;
- that player orientation matrix is what makes centeredness scoring body-facing rather than camera-facing.

The exact retained function blobs were independently verified against the materialized same-build executable using Git blob SHA:
- SelectAutoAimTarget retained blob matched exactly;
- SelectTargetBaddy retained blob matched exactly;
- SelectTargetSwitch retained blob matched exactly;
- CheckWebShot retained blob matched exactly;
- FireWeb retained blob matched exactly.

Therefore this targeting RE is grounded in the repository's retained canonical function bytes rather than inferred from nearby code.

### Camera-forward targeting implementation

Commit:
- 954882bb63a86c39011bbc3996104f905b8559aa — gameplay: aim web auto-targeting from camera

Patch scope:
- only SelectAutoAimTarget's direct call at 0x004C5B2F is replaced;
- expected target remains 0x004C8410;
- other callers of SelectTargetBaddy are unchanged.

Wrapper:
- preserves retail enemy enumeration, filtering, distance weighting, LOS, and result handling;
- when the active camera is ordinary mode 3:
  - copies Spider-Man's field_89C target-scoring matrix;
  - converts the active camera's final orientation quaternion field_214 using retail QToM @ 0x0047C7F0;
  - temporarily presents that exact camera rotation to the retail SelectTargetBaddy scorer;
  - restores Spider-Man's original matrix immediately after the retail call;
- when the camera is not ordinary mode 3, selection falls back to untouched retail body-facing behavior.

This uses the same final camera quaternion that CCamera::LoadIntoMikeCamera converts for the visible render camera, so pitch and camera collision/orientation adjustment are included rather than rebuilding aim direction from a guessed yaw.

Telemetry:
- camera_web_target_install ...
- camera_web_target event=select source=render_camera_transform ... target=...
- target changes are logged without logging every call.

### Next combined runtime test

Run FAST_UPDATE_AND_TEST_LATEST_BUILD.bat.

Validate in one session:

1. Pause -> Options:
   - Camera Sensitivity row is present;
   - change to an obvious lower value such as 50 percent;
   - Apply Settings;
   - camera becomes slower immediately;
   - optionally try 150 percent;
   - reopen Options and verify the applied value persists.

2. Frametime:
   - play for long enough to observe the old repeating freeze interval;
   - specifically report whether the every-several-seconds half-second-class freeze is gone;
   - report whether smaller constant hitching remains.

3. Web targeting:
   - rotate Spider-Man so his body is not facing an enemy;
   - rotate the camera until that enemy is near screen center;
   - fire the enemy-targeting web action;
   - expected target selection follows the camera;
   - test targets to camera-left/right and above/below if practical;
   - moving Spider-Man's body away while keeping the camera on the enemy should not steal target priority.

4. Brief movement/swing/camera regression check.

Provide the single consolidated spidey-decomp.log after the run.

