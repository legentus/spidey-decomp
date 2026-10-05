# Chase Venom scripted-input pipeline reverse engineering

Date: 2026-10-05  
Branch: `dev`

## Purpose

This document captures the retail x86 -> C reconstruction that now drives the Chase Venom native-60 compatibility work.

The important distinction is that the Chase failure is no longer being treated as a generic "cutscene FPS" problem. Runtime already proves the in-engine cutscene and ordinary Logic are executing at the intended 60-Hz cadence. The remaining bug is inside the scripted player-control pipeline.

All addresses below refer to the retail PC executable currently fingerprinted by the project.

## Relevant retail functions

- `CPlayer::SwitchToSynthesizedInput @ 0x004BC1A0`
- `CPlayer::SynthesizeAnalogueInput @ 0x004BC300`
- `CPlayer::ReadAnalogueInput @ 0x004BD510`
- direct synth call inside ReadAnalogueInput: `0x004BD572 -> 0x004BC300`
- `CPlayer::CheckForwards @ 0x004BF8A0`
- `CPlayer::SetTargetTorsoAngle @ 0x004C6970`
- `CPlayer::GetEffectiveHeading @ 0x004C6AA0`
- camera pointer global: `0x0056F3B8`
- `CCamera::field_23A @ +0x23A`
- Chase Venom retail level ID: `0x501`

## 1. SwitchToSynthesizedInput

The retail routine is now statically reconstructed far enough to establish the input-script lifecycle.

Equivalent high-level behavior:

```cpp
void CPlayer::SwitchToSynthesizedInput(short* script)
{
    field_60 = 0;
    field_64 = 0;
    field_68 = 0;

    field_1AC = 1;       // synthesized-input mode active
    field_1B4 = 1;       // parser/script-active flag
    field_1B8 = script;  // script cursor/source

    field_AE5 = 0;
    field_1A4 = 0;
    field_1A8 = 0;

    // Existing trail/effect objects at +0x584/+0x588 are retired.
    // Existing held object is released and an appropriate animation is entered.
    // The per-script auxiliary slots beginning at +0x1C0 are cleared.

    // The routine advances over the initial encoded script header and returns
    // a pointer immediately after the 0 / 0xFF terminator sequence.
}
```

The important FPS point is that this function only establishes scripted-control state. It is not itself the route integrator.

## 2. SynthesizeAnalogueInput top-level clock

The retail prologue proves the main script clock is already elapsed-tick-aware:

```cpp
field_1B0 += field_80;
```

`field_80` is the body elapsed tick count derived from the canonical `gTimerRelated` clock.

Therefore:

- do not globally divide or scale the script clock for 60 Hz;
- do not globally multiply the entire synthesized-input interpreter by a floating delta;
- worker-specific behavior must be classified separately.

The worker list head is stored at `player + 0x1BC`.

## 3. Worker dispatch

The active worker type is read from `worker[0]`.

The retail dispatch around `0x004BCB85` subtracts 2 and indexes a jump table. The worker relevant to Chase route steering is type 2.

Known type-2 layout:

```cpp
struct SynthWorkerType2
{
    int type;       // 2
    int size;       // normally 5 dwords
    int targetX;    // +0x08
    int targetZ;    // +0x0C
    int next;       // +0x10
};
```

## 3A. Worker dispatch classification

The retail worker jump table at `0x004BD44C` maps worker types 2..15 as follows:

| Type | Retail body | Classification |
| --- | --- | --- |
| 2 | `0x004BCFB8` | X/Z route target; produces camera-relative analogue axes |
| 3 | `0x004BCC2D` | timed synthesized action/direction worker |
| 4 | `0x004BD207` | invalid/unhandled in this dispatcher |
| 5 | `0x004BD0FD` | spatial/target-vector worker; no raw per-call timer found |
| 6 | `0x004BCE5B` | timed animation/action worker |
| 7 | `0x004BCF37` | timed player-state worker |
| 8 | `0x004BCDDC` | timed `field_E00` worker |
| 9 | `0x004BCD68` | timed wait/parser-resume worker |
| 10..14 | `0x004BD207` | invalid/unhandled in this dispatcher |
| 15 | `0x004BCB9A` | conditional state/animation worker |

The important timing result is that every timed worker inspected uses the body elapsed tick count `field_80`, for example:

```cpp
worker->remaining -= player->field_80;
```

This is true for types 3, 6, 7, 8 and 9.

Therefore the synthesized-input subsystem does **not** contain a second obvious family of raw `--timer` counters that would run three times too fast at native 60. Its time durations are mostly already expressed in canonical elapsed ticks.

The remaining native-60 problem is the frequency of the controller feedback loop: spatial steering/output is recalculated more often against changing player/camera state.

This also means a 20-Hz-equivalent synth sample must preserve the whole sampled controller output, not assume route type 2 is always the linked-list head. Multiple workers can coexist.

## 4. Exact type-2 target steering

Retail type-2 worker body begins at `0x004BCFB8`.

Equivalent C-like reconstruction:

```cpp
void ExecuteType2RouteWorker(CPlayer* p, Worker* w)
{
    int targetX = w->targetX;
    int targetZ = w->targetZ;

    CVector target = { targetX, 0, targetZ };

    // Retail helper at 0x004E61A0.
    if (DistanceToTargetXZ(target, p->mPos) < 0x40)
    {
        RemoveWorkerFromList(w);
        return;
    }

    int dx = (targetX - p->mPos.vx) >> 12;
    int dz = (targetZ - p->mPos.vz) >> 12;

    int targetWorldAngle = ratan2(dz, dx);

    CCamera* camera = *(CCamera**)0x0056F3B8;
    int cameraTransformHeading =
        *(unsigned short*)((unsigned char*)camera + 0x23A);

    int stickAngle =
        (0x400 -
         targetWorldAngle -
         cameraTransformHeading) &
        0x0FFF;

    int axisX = SinTable[stickAngle] / 32;
    int axisY = -CosTable[stickAngle] / 32;

    p->field_E2D = clamp(axisX, -127, 127);
    p->field_E2E = clamp(axisY, -127, 127);
}
```

The exact retail sine/cosine tables used by this path are based at:

- `0x00610C4A`
- `0x00610C48`

### Critical conclusion

The scripted route worker does **not** output a world heading.

It outputs **camera-relative analogue stick axes**.

That makes camera update order part of the steering controller.

## 5. CCamera +0x23A is transform heading, not merely requested camera angle

The reconstructed `CCamera::LoadIntoMikeCamera` path computes `field_23A` from the actual camera transform:

```cpp
if (Transform.m[0][2] || Transform.m[2][2])
{
    field_23A =
        (-1024 -
         ratan2(
             Transform.m[2][2],
             Transform.m[0][2])) &
        0x0FFF;
}
```

This is materially different from simply reading `CCamera::field_236`, which is the angle controlled by `SetCamAngle`.

For Chase steering, the type-2 worker consumes the actual transform-derived heading at `+0x23A`.

## 6. ReadAnalogueInput — relevant reconstructed path

Retail `CPlayer::ReadAnalogueInput @ 0x004BD510` first preserves the previous axes and clears the current axes.

When `field_1AC != 0`, it directly calls:

```text
0x004BD572 -> CPlayer::SynthesizeAnalogueInput @ 0x004BC300
```

After inversion/dead-zone processing, the active-axis path is equivalent to:

```cpp
if (field_E2D || field_E2E)
{
    field_8F0 += 0x20;
    if (field_8F0 > 0x100)
        field_8F0 = 0x100;

    int inputAngle =
        ratan2(
            -(signed char)field_E2D,
             (signed char)field_E2E);

    field_E32 =
        (field_E34 -
         inputAngle +
         0x400) &
        0x0FFF;
}
else
{
    field_8F0 = 0;
    field_E32 = field_E34;
}
```

### Timing classification

`field_8F0 += 0x20` is definitely raw per-call behavior.

A Chase-only elapsed correction for this ramp was previously runtime-proven to execute but did not fix the route by itself.

The more important new fact is that the analogue axes generated by type 2 are transformed again into `field_E32`, using `field_E34` as another heading basis.

The exact owner/update semantics of `field_E34` are not yet proven.

## 7. CheckForwards — desired-heading consumption

Retail `CPlayer::CheckForwards @ 0x004BF8A0` rejects movement for several state/aim/animation cases, then requires non-zero `field_E2D/E2E`.

The heading path is:

```cpp
int desiredHeading;

if (!field_8E8)
{
    CCamera* camera = *(CCamera**)0x0056F3B8;
    desiredHeading =
        (field_E32 + camera->field_23A) &
        0x0FFF;
}
else
{
    desiredHeading =
        (short)field_E32;
}

int effectiveHeading = GetEffectiveHeading();
int delta =
    (desiredHeading - effectiveHeading) &
    0x0FFF;

// The rest of CheckForwards selects run/turn behavior and, in the
// forward-turn path, calls:
SetTargetTorsoAngle(desiredHeading, true);
```

This proves that the movement consumer adds the **current** transform-derived camera heading back into the steering path on ordinary ground movement.

## 8. Why holding camera-relative axes is not a faithful 20-Hz emulation

The first untested sample/hold candidate at `6c40ef8...` did this:

1. execute retail synth once per 3 canonical ticks;
2. hold `field_E2D/E2E` on the two intervening 60-Hz calls;
3. keep physics/render/camera at 60 Hz.

The new assembly reconstruction shows a flaw in that design.

The held axes are camera-relative.

While those axes are held:

- the camera continues updating at 60 Hz;
- `CCamera+0x23A` can change;
- ReadAnalogueInput and CheckForwards continue running;
- therefore the same held stick vector can resolve to a different world steering direction before the next 20-Hz synth sample.

That behavior did not exist in a true 20-Hz simulation step, where the controller sample and its camera basis belonged to the same authored update.

## 9. New compatibility model

Implemented in:

- `f2f46b7ff37332cbcb2179ac5f8d0b2991dc9a0e` — preserve Chase world heading across held steering frames.

The 20-Hz-equivalent synth sample remains narrow to:

- retail level `0x501`;
- synthesized input only.

On a fresh synthesized analogue steering sample:

1. retail synth produces the camera-relative axes;
2. retail ReadAnalogueInput produces `field_E32`;
3. the wrapper reconstructs the effective world desired heading exactly the way CheckForwards will consume it;
4. that world heading becomes the held controller sample.

On the two intervening 60-Hz calls:

1. the old axes can remain held for normal movement magnitude behavior;
2. after retail ReadAnalogueInput, the wrapper rewrites `field_E32`;
3. the rewrite compensates for the **current** `camera+0x23A`;
4. CheckForwards therefore sees the same sampled world heading even if the camera transform moved.

The first implementation gated the compensation on "worker-list head is type 2." Deeper dispatch reconstruction showed that was too narrow because route steering can coexist with other worker types.

Current source:
- `0ab2efb34c841814b2313aa74301e5eb3789a7ad` — preserve the sampled world heading for **any active synthesized analogue movement**, not only a type-2 head worker.
- The trace now records `worker_mask_before` and `worker_mask_after`, a bitmask of every worker type present in the linked list.

This preserves:

- 60-Hz render;
- 60-Hz camera;
- 60-Hz physics;
- 60-Hz collision;
- 60-Hz animation;
- a 20-Hz-equivalent scripted steering controller.

It does **not** lower the cutscene FPS.

## 10. New telemetry

The Chase stats now include:

- `heading_samples`
- `heading_corrections`
- `heading_max_pre_correction_drift`

The route trace now records, on fresh synth samples:

- camera transform heading `camera+0x23A`;
- input basis `field_E34`;
- relative desired heading `field_E32`;
- reconstructed world desired heading.

This will show how large the previously uncorrected held-frame heading drift would have been.

## 11. Remaining unknown: field_E34 ownership

The exact writer/update order for `field_E34` is the next static/runtime RE target.

Commit:

- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546` — trace Chase steering-basis ownership.

The next build performs startup-only machine-code displacement scans over:

`SpideyAI0 @ 0x004B13F0 + 0x73A0`

for:

- `+0xE32`
- `+0xE34`
- `+0x23A`

Expected log labels:

- `SpideyAI0_E32`
- `SpideyAI0_E34`
- `SpideyAI0_CameraHeading23A`

This is static startup scanning only. It adds no synchronous per-frame logging.

## 12. Exact next validation

Use `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

The tested source should be `e1c8a369...` or a documentation-only descendant.

In Chase Venom:

1. confirm the in-engine cutscene remains visually 60 FPS;
2. watch the building-entry turn where Spider-Man previously ran into the wall;
3. report fixed / improved / unchanged / worse;
4. continue long enough to know whether the chase remains playable;
5. exit cleanly so the in-memory trace dumps;
6. return the one consolidated `spidey-decomp.log`.

If the route still fails, the same log should now answer two questions at once:

1. whether camera-basis drift was materially present and corrected;
2. who writes `field_E34` and whether its update ordering exposes another authored-cadence assumption.
