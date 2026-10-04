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
