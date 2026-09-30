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
