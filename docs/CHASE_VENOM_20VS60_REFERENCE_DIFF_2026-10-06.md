# Chase Venom 20 FPS vs 60 Hz Reference Diff — 2026-10-06

## Purpose

This checkpoint preserves the first successful instrumented **full-engine 20 FPS** Chase Venom run and the direct comparison against the immediately preceding failing 60 Hz run.

Do not treat the current source frontier as fully validated. The successful 20 FPS reference is authoritative runtime evidence. The newest 60 Hz BaddyList/camera-shot changes in `main.cpp` are **WIP / untested** and exist to continue the isolation work without losing progress.

## Authoritative reference logs

Working full-engine 20 FPS:
- `logs/20261006-020549/spidey-decomp.log`
- behavior source installed for the reference: `85ae7108eaaf19b826a1fe924904c89cd62ed380`
- user result: **WORKS** — Spider-Man visibly follows Venom through the building and the scripted passage completes.

Failing 60 Hz paired player+camera cadence run:
- `logs/20261006-012109/spidey-decomp.log`
- result: **FAILS** — Spider-Man enters the building hole at the wrong trajectory, collides with `Inside01`, continues scripted movement/jumping while control is locked, then eventually returns control.

Tracked comparison tools/results:
- `tools/research/compare_good20_fail60.py`
- `tools/research/compare_good20_fail60.txt`
- `tools/research/compare_final_building_window.py`
- `tools/research/compare_final_building_window.txt`

## User timing observation

At 60 Hz the Chase script appeared to progress faster. In the successful 20 FPS run the whole scripted sequence visibly took longer.

This matches the architecture seen in the failing targeted-cadence builds: player/synth/camera had been gated toward 20 Hz, but the rest of Logic and other world actors still received 60 Hz update opportunities. This means related level/camera/baddy state could advance out of phase even when individual elapsed-time counters were nominally correct.

## Direct trace comparison — earliest divergence

The working 20 FPS and failing 60 Hz traces are identical through **trace sample 21**.

First divergence:
- sample 22
- failing 60 Hz camera transform heading: **183**
- working 20 FPS camera transform heading: **168**
- working 20 FPS reaches the corresponding later camera state about two samples afterward.

This is the first hard state divergence found in the trace.

A second important divergence occurs around sample 37:
- failing 60 Hz receives an anomalous **6-tick** player update;
- working 20 FPS receives the expected **3-tick** update;
- this is the first point where physical player position begins to diverge materially.

Interpretation:
- the targeted 20 Hz wrappers were not perfectly phase-locked to the same Logic pass;
- accumulated ticks could occasionally bunch into 6-tick updates;
- the 60 Hz world continued advancing around those held consumers.

## Final through-building worker — root physical failure

The decisive difference is the final through-building scripted worker.

Both runs execute the same synthesized worker:
- type 3
- **code 9**
- analogue axes: `0,127`

This command is camera-relative.

### Working 20 FPS

During the final code-9 worker:
- camera transform heading remains locked around **1029**;
- desired world heading remains around **2053**;
- Spider-Man runs almost straight along **+Z**;
- he physically follows Venom through the building and out the intended side.

### Failing 60 Hz

During the same code-9 worker:
- camera heading begins around **2080**;
- it continues rotating through roughly **2300 → 3000 → 3700**;
- the same `axes=0,127` therefore maps to a completely different world-space direction;
- Spider-Man initially moves strongly along **+X** instead of the working +Z course;
- he collides with the side/interior geometry (`Inside01`) and becomes trapped in the hole;
- the synthesized program continues driving later movement/jump states until the worker chain expires and manual control finally returns.

This directly matches the user's in-game observation.

## Important conclusion

The remaining failure is not:
- missing Wait04/Wait06 trigger activation;
- generic noclip/collision;
- modern free-look camera ownership by itself;
- SpideyAI0 cadence by itself;
- active camera cadence by itself.

The working reference proves that the **camera shot/state used by the final code-9 worker is still active and locked at heading ~1029 at 20 FPS**, while the corresponding camera state has already advanced/expired in the 60 Hz simulation.

Therefore the primary RE target is now:

> Find the separate L5A1 level/camera command that owns the camera shot active during the final code-9 worker, determine why its state/lifetime advances too early at 60 Hz, and make that authored camera-state lifetime elapsed-time correct / phase-correct without lowering the whole engine to 20 FPS.

## L5A1 script relationship already established

Node 71 primarily contains the synthesized player program. Trigger opcode 199 hands its following words to `CPlayer::SwitchToSynthesizedInput`.

Therefore the camera shot controlling code-9 world direction is **not embedded in the same synthesized player stream**. It is driven by a separate sibling/linked level command.

The latest WIP instrumentation began tracing:
- active camera mode;
- camera interpolation ticks;
- command point node 74 pulse state (`NumPulsesSet` / `NumPulses`) as a suspected nearby camera-shot transition;
- BaddyList/Venom cadence, because Venom and other baddies remained at 60 Hz in previous targeted-cadence tests and may be crossing/activating the sibling camera trigger too early.

Node-74 ownership is **not yet proven**; treat it as instrumentation, not a conclusion.

## Current uncommitted WIP source frontier being checkpointed

The WIP source restores global timer delivery back to native 60 Hz after the successful full-engine 20 FPS reference and adds:

1. **BaddyList authored-cadence experiment**
   - retail Logic call site: `0x004554F5 -> Ob_AI(&BaddyList,0)`
   - BaddyList global: `0x0056E990`
   - Venom lives on this list.
   - proposed wrapper holds two dispatches and runs untouched `Ob_AI` on the third canonical tick while level `0x501` synthesized player control is active.
   - purpose: test whether Venom / baddy-trigger progression is what advances the separate camera shot too early.
   - **UNTESTED at this checkpoint.**

2. **Additional Chase camera telemetry**
   - `camera_mode`
   - `camera_interp`
   - node 74 pulse state
   - intended to identify exactly when the separate camera shot transitions in 60 Hz vs the working 20 FPS reference.

3. **Global timer restored to 60 Hz**
   - diagnostic 20 FPS timer constants returned to 60 Hz / one vblank per callback.
   - full-engine 20 FPS remains only the reference result, not the intended final policy.

## Exact next RE steps

1. Build the WIP 60 Hz + BaddyList cadence + camera-shot telemetry source.
2. Run Chase Venom with `TEST_LATEST_BUILD.bat`.
3. Compare the new trace against the working 20 FPS reference, focusing on:
   - the exact sample where camera heading leaves the working ~1029 shot;
   - camera mode/interpolation fields;
   - node 74 pulse state;
   - BaddyList cadence counters;
   - Venom/world-trigger timing.
4. If BaddyList cadence keeps the final camera shot locked through code 9 and the route works, reduce the fix further to the exact Venom/trigger/camera transition.
5. If it still fails, decompile the sibling camera command directly from the Wait05/Wait06 linked trigger graph and patch that timer/state transition rather than further broad cadence throttling.

## Workflow / safety

- Local Commander is now the preferred local-PC interface for this project.
- Local repo remains authoritative.
- Logs are intentionally ignored by Git but remain preserved locally.
- Comparison scripts/results are now tracked under `tools/research`.
- Any WIP commit created from this frontier must be labeled **untested** until an actual runtime Chase test is performed.

## 2026-10-06 — Node 76 camera controller proven / phase-locked world-list candidate

Deeper L5A1 + retail object-factory RE proves the separate camera transition around the final through-building worker:

- node 73 links to camera target node 72 and executes camera opcode 186 (`CCamera::SetFixedPosAnglesMode`) with 32 frames;
- node 76 is a type-1 object node at approximately fixed-point position `(69111808, 61440, 19329024)`;
- node 76 links to node 74 and node 87;
- node 74 links to camera target node 75 and executes opcode 186 with **128 frames**;
- node 72 target is approximately `(74461184, 0, 24510464)`;
- node 75 target is approximately `(66482176, 0, 20025344)`.

Retail `Trig_CreateObject @ 0x004DEE70` maps node-76 object type **203 / 0xCB** to construction of `CScriptOnlyBaddy @ 0x004075B0`.

The constructor installs vtable `0x0053B2E8`; virtual AI slot +8 resolves to `CScriptOnlyBaddy::AI @ 0x00407840`.

Crucially, the constructor attaches this script-only controller to the list head at **0x0056E994**, while Venom is on **BaddyList @ 0x0056E990**. Retail Logic confirms:

- `0x004554F5 -> Ob_AI(&BaddyList @ 0x0056E990, 0)`;
- `0x00455501 -> Ob_AI(&ControlBaddyList @ 0x0056E994, 0)`;
- pending trigger commands are consumed afterward at `0x0045551D`.

Therefore the prior BaddyList-only cadence candidate could slow Venom while leaving the actual camera-controller object at 60 Hz.

Node 76's apparent script tail decodes as `0x4280, 32, 0x4100`. Retail `CBaddy::ExecuteCommand` maps opcode `0x4280` to a handler that writes the parameter to `field_230`. `CScriptOnlyBaddy::AI` updates `field_230` using `field_80`, so this particular 32-tick delay is already elapsed-time correct. A separate `field_238` raw per-call decrement exists in the class, but it is **not currently proven to own node 76's camera transition** and is not being patched.

New 60-Hz candidate:
- global timer remains native 60 Hz;
- scripted player/camera/synth compatibility remains;
- Venom's BaddyList and node-76's ControlBaddyList now share **one phase gate**;
- both retail `Ob_AI` list dispatches are held for two Logic passes and released on the exact same third canonical tick;
- because BaddyList executes immediately before ControlBaddyList in retail Logic, a shared due-tick marker guarantees both lists advance in the same authored phase;
- outside L5A1 synthesized player control, both lists run retail every Logic update.

New telemetry:
- separate BaddyList calls/retail/held counts;
- separate ControlBaddyList calls/retail/held counts;
- shared gate-event count and max elapsed;
- existing camera mode/interpolation and node-74 pulse state trace remains active.

Forced-clean VC6 matching build of this phase-locked two-list source: **PASS**.

This candidate is implemented and built but **not yet runtime-tested**.
