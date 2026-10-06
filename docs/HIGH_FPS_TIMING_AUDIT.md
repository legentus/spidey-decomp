# HIGH-FPS TIMING AUDIT

Date: 2026-10-04  
Branch: `dev`

## Goal

Make Spider-Man's PC simulation behave correctly at 60 Hz first, then allow presentation above 60 Hz without allowing gameplay simulation to run faster than the engine's canonical clock.

This document is deliberately conservative: do not globally multiply the game by a floating delta. The retail executable already contains elapsed-tick-aware movement and animation, mixed with raw per-update counters and one-update handshakes. A global multiplier would double-correct the first group while still mishandling the latter groups.

## External symptom baseline

PCGamingWiki reports that the full PC release is capped at 60 FPS but behaves incorrectly above its lower authored cadences:
- numerous gameplay/cutscene problems above 30 FPS;
- the Catch/Chase Venom automated sequence needs 20 FPS as a workaround;
- Mysterio's phase-2 lasers malfunction at 30 FPS and above and need 20 FPS as a workaround;
- several cutscenes can lead to a game-over loop over 30 FPS;
- the main menu is capped at 30 FPS;
- the Kellogg demo reportedly has a native 30-FPS cap absent from the full release.

Reference:
- https://www.pcgamingwiki.com/wiki/Spider-Man_(2001)
- https://community.pcgamingwiki.com/files/file/923-spider-man-2001-kelloggs-demo/

These are symptom references, not proof of internal mechanisms. Internal conclusions below come from the retail binary and decompiled source.

## Retail timing architecture

### PlayAway / Logic / Display

Known addresses:
- `Logic = 0x00455400`
- `Display = 0x004555A0`
- `PlayAway = 0x004559D0`
- `MyVSync = 0x004E5CF0`
- `TimerCallback = 0x00511130`

Retail `PlayAway`:
1. snapshots `Vblanks @ 0x006B4CA0`;
2. calls `Logic`;
3. calls `Display`;
4. compares the current `Vblanks` with the snapshot;
5. if the clock has not advanced, calls `Pause(1)`;
6. then begins the next iteration.

Therefore the full release deliberately prevents a normal Logic+Display iteration from immediately running again before at least one canonical timer tick.

### TimerCallback / MyVSync

`TimerCallback` advances `gTimerVblankRelated` using the multimedia timer and calls `MyVSync` until integer timer time catches `Vblanks`.

`MyVSync`:
- increments `Vblanks @ 0x006B4CA0`;
- conditionally increments `gTimerRelated @ 0x006B4CA8`.

`gTimerRelated` is the useful gameplay elapsed-time clock. Its unit is nominally 1/60 second.

## Body / animation elapsed-tick model

### CBody::EveryFrame — 0x00460ED0

Retail behavior:
- first/update-reset path seeds `field_80 = 2`;
- otherwise:
  - `field_80 = gTimerRelated - field_7C`;
  - `field_7C = gTimerRelated`;
  - `field_80` is clamped to 6;
- `field_84 += field_80`.

At a normal 60-Hz Logic rate, `field_80` is usually 1.
At a 30-Hz Logic rate, it is usually 2.

### CSuper::UpdateFrame — 0x00460DA0

Animation increment is:

`field_80 * mAnimSpeed / 2`

This is strong evidence that the engine already expects body/animation time to vary with elapsed canonical ticks. A 60-Hz call with `field_80=1` advances half the animation amount of a 30-Hz call with `field_80=2`, preserving real-time animation speed.

Danger:
- if `field_80 == 0`, retail forcibly substitutes 2.
- therefore simply removing the PlayAway clock wait and running Logic above 60 Hz would make supers advance as though a full 30-Hz step occurred on zero-time updates.

## Existing canonical timer helper

`CBaddy::RunTimer` already implements the preferred elapsed-tick countdown model:

```cpp
*a2 -= this->field_80;
if (*a2 < 0)
    *a2 = 0;
return *a2;
```

Many reconstructed boss routines already use this helper correctly.

## Mixed timing classes found in source

### Tick-aware / generally safe at 60 Hz

Examples:
- Carnage position/health/attack timers frequently multiply or accumulate by `field_80`.
- Chopper motion has explicit `for (... field_80 ...)` substeps in some physics paths.
- Camera code consumes `field_80`.
- Switch timing uses `field_80`.
- `CBody::EveryFrame` and `CSuper::UpdateFrame` are elapsed-tick based.
- many boss state timers call `CBaddy::RunTimer`.
- some bit/effect code (for example reconstructed `bit2.cpp`) stores `gTimerRelated` and subtracts elapsed time directly.

### Definitely update-count based

These must not be globally scaled blindly.

#### AI process wait

`CAIProc::Wait` decrements `field_C` by exactly one per Execute call.

Before changing it, reconstruct `Ob_AI`: if object AI is intentionally interleaved at a lower cadence, raw decrement semantics may be part of that scheduler.

#### Bit/effect layer

Multiple reconstructed Move routines perform one step per Move invocation:
- raw `mPos += mVel`;
- raw gravity/friction application;
- `++mAge`;
- `--mLifetime`;
- raw animation-frame increments.

Examples include sparks, frags, motion blur, glass bits and several combat effects.

A generic "multiply everything by delta" is unsafe because some effects in the same subsystem already use absolute `gTimerRelated` time.

### Candidate raw gameplay timers requiring classification

Static scan found likely timing counters that do not use `field_80`/RunTimer, including examples in:
- `hostage.cpp`: `field_328++`, `field_230--`;
- `rhino.cpp`: `field_1F0--`;
- `scorpion.cpp`: raw `field_BD8 -= 1` / `field_1F8--` in some states while other states use RunTimer/field_80;
- `simby.cpp`: raw `field_1A4++` and `field_328--`;
- `thug.cpp`: raw `field_230--` / `field_1F8--`;
- `chopper.cpp`: several raw counters and effect ages;
- `cop.cpp`: raw per-call counters;
- `spidey.cpp`: `CheckGroundGone` has raw `field_EA4--`.

Do not automatically convert every `++/--`: many `dumbAssPad++` operations are state transitions, and other fields are event/dirty counters rather than time.

## First concrete high-FPS gameplay repair: Mysterio laser

Source commit:
- `6cfcd74aaecc72a2e1ac37885a03dc4aad0f52ae`

Binary proof:
- `CMysterioLaser` constructor `0x0045B3E0` installs vtable `0x0053BB34`;
- destructor path resets to base `CNonRenderedBit` vtable `0x0053BB2C`;
- those vtables are exactly 8 bytes apart, consistent with two 32-bit virtual slots;
- `CBit` declares virtual destructor then virtual Move;
- therefore derived vtable slot 1 is the Move override;
- runtime patch additionally requires slot 0 == `0x0045B300` and slot 1 == `0x0045BAC0` before installing.

Producer:
- `CMysterioLaser::SetPos @ 0x0045B5E0` ends by writing `byte[this+0x44] = 1`.

Retail consumer `0x0045BAC0`:
- if `byte[this+0x44] == 0`, calls `CBit::Die @ 0x00408930`;
- always clears `byte[this+0x44] = 0` afterward.

That is a one-consumer-update liveness handshake. It depends on relative producer/consumer call cadence rather than elapsed time.

New compatibility wrapper:
- reuses byte +0x44 without changing the object size;
- retail SetPos still writes fresh marker 1;
- next Move converts 1 into an encoded `gTimerRelated` timestamp;
- liveness is accepted for <= 3 canonical ticks (50 ms);
- if no producer refresh arrives by the fourth tick, it calls retail CBit::Die;
- no sidecar map, allocation, destructor hook or global timer change.

This directly converts the known broken synchronization primitive from update-count semantics to real engine-time semantics.

## Runtime-only RE probes ready

Commits:
- `9f8a62f46d00e437861cd55facf0a6ef91a76b4e`
- `62c7e71dc32f6cadec9c077dde13ec66d1645a0c`

The next normal runtime logs startup-only, chunked machine-code bytes for:
- `Ob_AI @ 0x00460FC0`, size `0x1A0` (next symbol `0x00461160`);
- `CVenom_FollowDirections @ 0x004EB530`, size `0x160` (next symbol `0x004EB690`);
- `SpideyAI0 @ 0x004B13F0`, size `0x73A0` (next known function `0x004B8790`).

This is startup-only I/O. It adds no per-frame instrumentation.

Purpose:
- reconstruct whether `Ob_AI` interleaves object AI and at what cadence;
- reconstruct Venom direction/path stepping;
- reconstruct the giant player AI/cutscene function to locate automated movement branches involved in Catch Venom and other in-engine cutscene failures.

## Renderer / hitch result relevant to timing work

The latest tested `091c2345...` runtime showed that the visible hitch problem and high-FPS gameplay correctness are related only partially.

Proven hitch sources included:
- synchronous diagnostic file writes;
- DX11 replay/end-frame work;
- untouched retail Logic stalls;
- some other presenter work.

Hot-path success logging was therefore disabled by default in:
- `98d52ec80b5876db8e347460be307555b905de4b`
- `31f80818ab5ee73cacae4b3d952205add448fc86`

Those cleanup commits still need runtime validation.

## Target architecture

### Phase 1 — make 60-Hz simulation correct

Keep the current validated 60-Hz camera/input path.

For each gameplay subsystem:
- preserve code already using `field_80`, RunTimer or absolute `gTimerRelated`;
- convert truly time-based raw update counters to elapsed canonical ticks;
- convert one-update handshakes to elapsed-time windows/timestamps;
- keep event counters and state-machine stage increments untouched;
- repair known 20-Hz-authored sequences individually.

### Phase 2 — prevent simulation >60 Hz

When lifting the frame cap:
- do not let Logic execute multiple times at the same `gTimerRelated` tick;
- avoid the `CSuper::UpdateFrame(field_80==0 -> 2)` zero-time hazard;
- run simulation at a fixed canonical maximum of 60 Hz.

### Phase 3 — render above simulation rate

Decouple Display/presentation from Logic.

For 120/144/240+ output:
- run rendering/presentation at output cadence;
- retain simulation at 60 Hz;
- interpolate camera and relevant render transforms between previous/current simulation states;
- do not advance AI, collision, timers, animation events, damage or physics on render-only frames.

This is the long-term uncapped/high-refresh design.

## Runtime validation update — 2026-10-04

Tested runtime:
- `a4e1105d1f9568a24ca8817847573392a7f8324a`
- log `spidey-decomp(20261004-213030).log`

Validated:
- user confirms the prior recurring hitches are gone after the hot-path logging cleanup;
- hitch investigation is therefore closed unless a new independent stall appears;
- all three startup retail captures from the first RE batch completed successfully;
- Mysterio compatibility correctly refused to install because runtime slot 0 is `0x0045B540`, not the initial `0x0045B300` guard; slot 1 matched `0x0045BAC0`.

Source follow-up:
- `6cf830b974316134a8a7813ac1eda42279eacd60` corrects the Mysterio guard and retires the completed first RE capture;
- `62f9500c4085a0841f5afbe9f85ee7d68b021e1f` adds startup-only capture for:
  - `AIProcBlock @ 0x00401000`, size `0x1100`;
  - `CPlayer_DoPhysics @ 0x004BFEC0`, size `0x1F0`.

The runtime result also changes the priority:
- 60 Hz is a mandatory native simulation baseline;
- user reports gameplay still feels slightly sped up at 60;
- the canonical timer itself is approximately 60 Hz, so the remaining acceleration is expected to come from per-Logic-call subsystems.

Recovered `Ob_AI` proves there is no hidden 30-Hz throttle inside object dispatch:
- active bodies receive `EveryFrame`;
- supers receive `UpdateFrame`;
- virtual `AI()` runs every dispatch.

Therefore raw AI counters such as `CAIProc::Wait(field_C--)` are genuine 60-Hz call-count candidates, while `CAIProc_MoveTo`, `CBaddy::RunTimer`, camera, animation and many movement paths already consume elapsed `field_80` ticks.

The next capture exists specifically to avoid changing `CAIProc::Wait` or player physics from reconstructed source guesses. Reconstruct the retail AI-proc constructors/Execute functions and retail player physics first, then convert only semantics proven to be frame-count based.

## Next steps

1. Runtime-test current `dev` to:
   - verify hot-path logging cleanup materially reduces hitches;
   - verify startup Mysterio vtable guard reports install=1;
   - collect startup byte chunks for Ob_AI, CVenom_FollowDirections and SpideyAI0.
2. Reassemble those byte chunks and disassemble the missing retail routines.
3. Locate Catch Venom's automated player movement branch and convert its path timing to canonical elapsed ticks.
4. Revisit generic AI waits only after Ob_AI cadence is proven.
5. Continue source classification of raw gameplay counters.
6. Only after 60-Hz behavior is robust, patch PlayAway into separate fixed-simulation and high-refresh render scheduling.


## Chase Venom scripted steering — camera-relative controller RE (2026-10-05)

The Chase route failure has now been pushed below the generic scheduler layer.

Detailed reconstruction:
- `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`

### Proven retail chain

`CPlayer::SynthesizeAnalogueInput @ 0x004BC300`:
- main script clock `field_1B0 += field_80` is already elapsed-tick-aware;
- active worker list head is at `+0x1BC`;
- route worker type 2 is the relevant automated movement producer.

Type-2 worker:
- reads target X/Z from `worker+0x08/+0x0C`;
- checks target distance against `0x40`;
- computes target direction from current Spider-Man X/Z;
- subtracts `CCamera+0x23A`;
- emits camera-relative signed analogue axes into `field_E2D/E2E`.

`CPlayer::ReadAnalogueInput @ 0x004BD510`:
- calls retail synth at `0x004BD572` while synthesized mode is active;
- contains the confirmed raw per-call `field_8F0 += 0x20` ramp;
- derives `field_E32 = (field_E34 - ratan2(-axisX, axisY) + 0x400) & 0xFFF`.

`CPlayer::CheckForwards @ 0x004BF8A0`:
- when `field_8E8 == 0`, consumes `(field_E32 + camera->field_23A) & 0xFFF` as the desired movement heading;
- compares it with `GetEffectiveHeading`;
- calls `SetTargetTorsoAngle(desiredHeading, true)` on the forward-turn path.

`CCamera+0x23A` is transform-derived:
- `CCamera::LoadIntoMikeCamera` calculates it from the actual camera matrix;
- it is not simply the requested `field_236` camera angle.

### Consequence for the first 20-Hz sample/hold experiment

Holding the type-2 analogue axes for two native-60 frames is not equivalent to a true 20-Hz control update:
- the held axes are camera-relative;
- the camera transform keeps updating at 60 Hz;
- `CheckForwards` uses the current camera transform heading;
- therefore the same held axes can resolve to a changing world desired heading.

This is a real cross-cadence feedback bug, not a cutscene frame cap.

### Current fix

Source:
- `f2f46b7ff37332cbcb2179ac5f8d0b2991dc9a0e`

Policy:
- keep render/camera/physics/collision/animation at 60 Hz;
- keep Chase synthesized controller sampling at a 20-Hz equivalent;
- on a fresh type-2 sample, capture the effective world desired heading after retail ReadAnalogueInput;
- on held frames, compensate `field_E32` against the current `camera+0x23A`;
- CheckForwards then sees the same sampled world heading across the three 60-Hz simulation frames.

Telemetry:
- `heading_samples`;
- `heading_corrections`;
- `heading_max_pre_correction_drift`;
- per-sample camera heading, E34 basis, E32 relative heading and reconstructed world heading.

### Remaining unknown: E34

`field_E34` is read by ReadAnalogueInput as a heading basis but its writer/update order is not yet identified.

Source:
- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546`

The next runtime performs startup-only xref scans over `SpideyAI0 @ 0x004B13F0 + 0x73A0` for:
- `+0xE32`;
- `+0xE34`;
- `+0x23A`.

This remains consistent with the project rule: do not globally scale the engine. Convert only proven cross-cadence behavior at the narrowest ownership seam.

## Chase Venom authored-cadence player-AI finding — 2026-10-06

Latest proper harness run proved the forced Wait05->Wait06 recovery fired at the black wall and still did not restore traversal. The blocker was `Inside01` (`0x1AD2FBED`), so trigger progression is downstream of the trajectory failure.

Deeper retail RE identifies a cleaner cadence seam:
- `Ob_AI @ 0x00460FC0` runs `CBody::EveryFrame()` then virtual AI;
- CPlayer virtual AI is `CPlayer::AI @ 0x004C65C0`;
- `CPlayer::AI` performs ordinary per-frame bookkeeping and then calls the function pointer at `player+0x554`;
- `CPlayer_CPlayer` initializes that pointer to `SpideyAI0 @ 0x004B13F0` using the immediate at `0x004BA2B5`.

Turn-controller RE explains why elapsed-time scaling alone is insufficient:
- `SetTargetTorsoAngle` computes `field_DF4` and `field_DF8` from current heading error;
- later `SpideyAI0` integrates `angle += field_DF4 * field_80` and `field_DF8 -= field_80`;
- `CheckForwards` can retarget on later AI passes, so three one-tick updates can recompute nonlinear steering between substeps whereas one three-tick update does not;
- the same ordering issue applies to friction, collision, surface transition and trigger feedback.

Implemented compatibility experiment:
- ordinary CPlayer housekeeping, animation, camera/global systems and all other objects remain 60 Hz;
- only the default `SpideyAI0` callback is gated to one execution per three canonical 60-Hz ticks while level `0x501` and synthesized control are active;
- the retail callback runs with accumulated `field_80` (normally 3);
- downstream forced Wait recovery is disabled so original trigger faces must be hit naturally.

Forced-clean matching VC6 build: PASS.

## 2026-10-06 01:09 — synthesized-control tail + camera cadence

Latest harness: `logs/20261006-010753/spidey-decomp.log`, revision `f4841e241602bcdf294fe01d333aea3bb055dbe0`. The player-AI 20-Hz wrapper was active (`retail_calls=775`, `held_calls=1549`, `max_elapsed=3`) and Chase still failed.

The user's longstanding observation that Spider-Man remains uncontrollable and continues moving for ~3–5 seconds after the visible cutscene is correlated with the script state: `field_1AC` remains active and type-3 workers continue to run until the final worker expires, at which point `synth=0` and control returns.

Type-3 code 10 is camera-relative (`E2D=-127`). The building-approach trace shows its desired world heading following `CCamera+0x23A`. Retail camera AI is `0x00417CB0` via camera vtable `0x0053B4BC`, slot `0x0053B4C4`; it calls `CM_Normal @ 0x00418E00`.

Simple `field_236` camera interpolation scales with `field_80`, but `CM_Normal` performs nonlinear camera solve/collision/focus work per AI invocation. Therefore three 60-Hz camera solves can produce a different transform heading path from one 20-FPS solve with `field_80=3`.

Next candidate pairs scripted `SpideyAI0` and the active retail camera at 20 Hz during L5A1 synthesized control while leaving rendering, other bodies, inactive cameras, and ordinary gameplay at 60 Hz. Forced-clean VC6 build: PASS.
