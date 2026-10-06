# SPIDER-MAN 2000 — NEW CHAT HANDOFF — CHASE VENOM CAMERA-RELATIVE STEERING FRONTIER

Date: 2026-10-05  
Authoritative repo: https://github.com/legentus/spidey-decomp  
Branch: `dev`

## Read this first

The current task is the native-60 conversion of Spider-Man (2000), specifically the Chase Venom scripted sequence.

The latest actually tested runtime is still:

- `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`
- user-confirmed in-engine Chase Venom cutscene runs at 60 FPS;
- route remains broken in that tested runtime: Spider-Man fails the intended building-entry course and runs into the wall instead of following Venom.

The user's latest supplied log:

- `spidey-decomp(20261005-233411).log`

is internally the same `df0b1d62...` runtime. It is therefore a baseline/pre-fix log and does **not** test the newer Chase compatibility code below.

## Current source frontier

Latest behavior source:

- `0ab2efb34c841814b2313aa74301e5eb3789a7ad` — `timing: preserve all synthesized Chase steering samples`

Important predecessor:

- `f2f46b7ff37332cbcb2179ac5f8d0b2991dc9a0e` — first world-heading hold across held steering frames.
- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546` — startup-only E32/E34/camera-heading ownership xrefs.

Documentation descendants before this dedicated handoff:

- `a29986c32249e259a9327ee2c418f6e19345bcd9` — detailed Chase scripted-input RE document.
- `122f927aae76a2d1f393ccfc823144e68e592831` — HIGH_FPS_TIMING_AUDIT update.
- `2f6e0dcbb8dec436350b523bd6de59e87be25977` — synthesized-worker timing classification.
- `c63fa11a946d48f4164fb02e8e657bfb1c764cd9` — CURRENT_STATUS generalized steering checkpoint.
- `d8114bad0c71799b5220cd9f235a762ede28f72b` — NEW_CHAT_HANDOFF checkpoint.

## What was newly reverse engineered

The relevant retail pipeline is now:

`CPlayer::SynthesizeAnalogueInput -> CPlayer::ReadAnalogueInput -> CPlayer::CheckForwards -> CPlayer::SetTargetTorsoAngle`

Retail addresses:

- `CPlayer::SwitchToSynthesizedInput @ 0x004BC1A0`
- `CPlayer::SynthesizeAnalogueInput @ 0x004BC300`
- `CPlayer::ReadAnalogueInput @ 0x004BD510`
- direct synth call: `0x004BD572 -> 0x004BC300`
- `CPlayer::CheckForwards @ 0x004BF8A0`
- `CPlayer::SetTargetTorsoAngle @ 0x004C6970`
- `CPlayer::GetEffectiveHeading @ 0x004C6AA0`
- camera pointer global: `0x0056F3B8`
- camera transform heading: `CCamera + 0x23A`
- Chase Venom retail level ID: `0x501`

### Type-2 route worker

The Chase route worker reads target X/Z from its script worker block.

It does **not** output a world-space desired heading directly.

Instead it:

1. computes world direction from Spider-Man to the target;
2. reads the actual camera transform heading from `CCamera+0x23A`;
3. transforms the desired direction into camera-relative signed analogue axes;
4. writes those axes to `player+0xE2D/+0xE2E`.

### ReadAnalogueInput

The relevant retail analogue conversion is equivalent to:

```cpp
if (field_E2D || field_E2E)
{
    field_8F0 += 0x20;
    if (field_8F0 > 0x100)
        field_8F0 = 0x100;

    int inputAngle = ratan2(
        -(signed char)field_E2D,
         (signed char)field_E2E);

    field_E32 =
        (field_E34 - inputAngle + 0x400) &
        0x0FFF;
}
else
{
    field_8F0 = 0;
    field_E32 = field_E34;
}
```

The raw `field_8F0 += 0x20` ramp is confirmed per-call behavior. A prior Chase-only correction of this ramp executed correctly but did not fix the route by itself.

### CheckForwards

For normal ground locomotion:

```cpp
desiredWorldHeading =
    (field_E32 + camera->field_23A) &
    0x0FFF;
```

The function then compares against `GetEffectiveHeading()` and feeds the desired heading into the retail turn/run path.

### Why this matters at 60 FPS

`CCamera+0x23A` is derived from the actual camera transform matrix, not merely the requested camera angle.

The first 20-Hz sample/hold experiment held camera-relative analogue axes for two additional 60-Hz frames while the camera kept updating at 60 Hz.

That was not equivalent to the original authored controller:
- same held stick axes;
- changed camera transform basis;
- changed effective world steering direction.

This is a concrete FPS-dependent feedback-loop mismatch.

## Current compatibility behavior

The current behavior source keeps:

- rendering: 60 Hz;
- camera: 60 Hz;
- physics: 60 Hz;
- collision: 60 Hz;
- animation: 60 Hz;
- ordinary Logic: 60 Hz.

Only the Chase synthesized controller is sampled at a 20-Hz-equivalent cadence.

On a fresh synthesized analogue sample:
- retail synth runs;
- retail ReadAnalogueInput runs;
- wrapper reconstructs the effective desired world heading;
- that world heading becomes the held controller sample.

On the two intervening 60-Hz frames:
- analogue magnitude/axes remain available to retail movement;
- after retail ReadAnalogueInput, the wrapper rewrites `field_E32` against the current `camera+0x23A`;
- CheckForwards therefore sees the same sampled world heading even if the camera transform moved.

This is intentionally narrow:
- only level `0x501`;
- only synthesized input;
- manual controls remain untouched;
- no global 20/30-FPS cap is reintroduced.

## Synth worker timing classification

The worker dispatcher has now been mapped enough for timing:

- type 2 — X/Z route steering / camera-relative analogue axes;
- type 3 — timed synthesized action/direction;
- type 5 — spatial/target-vector worker;
- type 6 — timed animation/action worker;
- type 7 — timed player-state worker;
- type 8 — timed `field_E00` worker;
- type 9 — timed wait/parser-resume worker;
- type 15 — conditional state/animation worker;
- type 4 and 10..14 — invalid/unhandled path in this dispatcher.

Every timed worker inspected (3, 6, 7, 8, 9) subtracts `player->field_80`.

Therefore their durations are already canonical elapsed-time-aware. Do **not** apply another generic 1/3 timing factor to them.

The current problem is feedback frequency and update-basis order, not their countdown speed.

## Other suspects already ruled out

Do not blindly slow these:

### Target torso/body turn interpolation

Retail turn interpolation already scales with `field_80`.

### Run-into-wall persistence

The wall persistence path also increments with `field_80`.

Patching these as though they were raw frame counters would double-correct them.

## Current telemetry

Shutdown Chase stats include:

- `synth_calls`
- `active_calls`
- `retail_updates`
- `held_calls`
- `max_elapsed`
- `heading_samples`
- `heading_corrections`
- `heading_max_pre_correction_drift`
- ramp stats
- trace sample counts.

Each stored route sample includes:

- canonical tick;
- elapsed synth ticks;
- script clock/mode;
- analogue axes;
- input ramp;
- player state;
- player XYZ;
- player angle;
- wall/ceiling flags;
- sampled camera transform heading;
- `field_E34`;
- `field_E32`;
- reconstructed desired world heading;
- `worker_mask_before`;
- `worker_mask_after`;
- worker-head type/size/data before and after synth.

The worker mask is important because multiple synthesized workers can coexist and route type 2 does not have to be the linked-list head.

## Remaining unknown: field_E34 ownership/order

`ReadAnalogueInput` uses `field_E34` as a heading basis, but its exact writer/update ordering is still unresolved.

The current source includes startup-only displacement scans over:

- `SpideyAI0 @ 0x004B13F0 + 0x73A0`

for:

- `+0xE32`
- `+0xE34`
- `+0x23A`

Expected log labels:

- `SpideyAI0_E32`
- `SpideyAI0_E34`
- `SpideyAI0_CameraHeading23A`

These are startup-only static scans and do not add per-frame file I/O.

## Exact next test

Run:

`FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`

Then use Level Select -> Chase Venom.

Test only through the known failure region first:

1. confirm the in-engine cutscene remains visually 60 FPS;
2. watch the exact building-entry route;
3. report `fixed`, `improved`, `unchanged`, or `worse`;
4. continue far enough to establish whether later chase movement remains playable;
5. exit cleanly so the in-memory trace is dumped;
6. return the one consolidated `spidey-decomp.log`.

The next runtime should simultaneously answer:
- whether the world-heading hold fixes the course;
- how much held-axis world-heading drift existed;
- which synthesized workers coexist at the doorway;
- who owns/updates `field_E34`.

## Do not do

- Do not reintroduce the historical Chase CBody minimum-two-tick limiter.
- Do not solve the sequence by capping the real-time cutscene or game to 20/30 FPS.
- Do not globally slow `SynthesizeAnalogueInput`.
- Do not blindly scale workers already using `field_80`.
- Do not inflate route thresholds as a substitute for fixing the steering semantics.
- Do not re-enable hot-path success logging that previously caused frame-time hitches.

## Important project rules

- Native simulation target remains 60 Hz.
- Later >60-FPS support should decouple rendering/presentation from a maximum 60-Hz simulation.
- Preserve retail code where possible and use narrow hooks at proven ownership seams.
- Keep `docs/CURRENT_STATUS.md`, handoff docs, and RE documentation current.
- Commit meaningful changes frequently.
- Prefer fewer, higher-value runtime tests with in-memory telemetry over repeated probe-only builds.

## Files to read in the new chat

Start with:

1. this file;
2. `docs/CURRENT_STATUS.md`;
3. `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`;
4. `docs/HIGH_FPS_TIMING_AUDIT.md`;
5. `docs/NEW_CHAT_HANDOFF.md`;
6. the included latest baseline log;
7. relevant Chase section of `main.cpp`.

