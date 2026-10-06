# Chase Venom 60-Hz Frontier — Good 20-FPS Reference -> BaddyList Cadence Candidate
Date: 2026-10-06

## Ground-truth result

The temporary full-engine 20-FPS reference run succeeded visually:

- archive: `logs/20261006-020549/spidey-decomp.log`
- runtime/source revision: `67ef1b72dc91191368dd3c3aa23da360b6cc0e1a`
- the user confirmed Spider-Man physically chased Venom through the building correctly
- the user also observed that the 60-Hz sequence had seemed to happen faster, while the full-20 reference took noticeably longer

This is the first known-good modern trace for the building traversal.

## Direct good20 vs fail60 trace diff

Failing paired player+camera run:
- `logs/20261006-012109/spidey-decomp.log`

Working full-20 run:
- `logs/20261006-020549/spidey-decomp.log`

Key findings:

1. The runs are identical through Chase synth sample 21.
2. The first hard divergence is sample 22:
   - failing 60-Hz camera transform heading: 183
   - working 20-FPS camera transform heading: 168
   - player position/state/worker are still otherwise aligned at that point.
3. The first physical position divergence occurs at sample 37, where the failing run receives an accumulated 6-tick player update while the working run receives the expected 3-tick update.
4. This supports the user's perception that the 60-Hz scripted sequence is not phase-equivalent to the authored cadence.

## Final through-building worker is now explained

Both runs eventually execute the same type-3 code-9 synthesized worker (`axes=0,127`).

Working 20-FPS behavior:
- camera heading stays near 1029 through the worker
- desired world heading stays near 2053
- Spider-Man moves almost purely +Z
- he passes through the building correctly

Failing 60-Hz behavior:
- the same worker starts with camera heading around 2080
- the camera keeps rotating through roughly 2300 -> 3000 -> 3700
- the same camera-relative scripted forward input therefore becomes a different world-space direction
- Spider-Man moves strongly +X, hits the side wall, and remains stuck fighting collision

This is the first direct trace explanation that matches the visible bug.

## L5A1 camera/trigger graph

Recovered from `L5A1_T.trg`:

- `TRGP_Wait05` = node 70
- node 70 fans out to nodes `71, 73, 76, 206, 279, 336`
- node 71 contains the synthesized player script
- node 73 executes trigger opcode 186 with `frames=32`
- retail opcode 186 calls `CCamera::SetFixedPosAnglesMode`
- node 73 uses camera target node 72
- node 76 is a spatial/type-1 trigger and links to node 74
- node 74 also executes opcode 186, with `frames=128`, using camera target node 75

The 32/128 camera interpolation counters themselves are elapsed-tick aware: retail `CCamera::CM_FixedPosAngles` subtracts `field_80`. The failure is therefore not simply the interpolation countdown running 3x fast.

The stronger model is that the later spatial trigger/camera transition is being reached/activated too early relative to Spider-Man's scripted phase.

## Logic ordering and Venom discovery

Retail `Logic @ 0x00455400` ordering:

- `0x0045549C`: reset command-point collision flags
- `0x004554A8`: player `Ob_AI`
- additional world/body `Ob_AI` lists
- `0x004554F5`: `Ob_AI(&BaddyList, 0)`
- `0x0045551D`: `Trig_DoPendingCommandLists`
- later: camera `Ob_AI`

Therefore a baddy can move/cross a trigger, queue a camera command, have that command consumed, and let the camera use the new state in the same Logic pass.

Retail `FindBaddyOfType @ 0x00403F90` proves:
- `BaddyList = 0x0056E990`

The decompiled Venom class also removes itself from `BaddyList` in `CVenom::~CVenom`, confirming Venom lives on that list.

This makes Venom/BaddyList cadence the strongest remaining coupled-system hypothesis.

## Current 60-Hz candidate

The full-engine timer has been restored to native 60-Hz delivery:
- target: 60 Hz
- expected canonical vblanks per callback: 1
- first delivery target: ~17 ms

Existing Chase authored-cadence compatibility remains:
- scripted player AI: 20-Hz equivalent
- active scripted camera AI: 20-Hz equivalent
- synthesized-input worker producer: 20-Hz sample/hold

New candidate:
- patch only retail call site `0x004554F5 -> Ob_AI(&BaddyList,0)`
- during level `0x501` while player `field_1AC` synthesized control is active:
  - skip two BaddyList dispatches
  - call untouched retail `Ob_AI` on the third canonical tick
  - the bodies' own `EveryFrame()` then naturally see ~3 elapsed ticks
- outside that narrow condition, BaddyList runs normally every Logic update
- no Venom velocity, animation, timer, trigger, or movement field is manually scaled

New telemetry:
- `chase_baddy_ai_20hz_install`
- `chase_baddy_ai_20hz_stats`
- Chase trace adds:
  - `camera_mode`
  - `camera_interp`
  - `camera74_pulses_set`
  - `camera74_pulses`

The next instrumented log should tell us directly whether node 74 still activates before the final code-9 run.

## Build state before checkpoint

A forced-clean matching VC6 build of the current uncommitted candidate completed successfully with exit code 0 before switching PC access from Desktop Commander to Local Commander.

The source still needs to be committed, pushed, and installed for the next runtime test.

## PC access workflow

As of this frontier, use **Local Commander** for local PC/repo work instead of Desktop Commander. Local Commander is the preferred primary interface because the user wants the unlimited-usage path.

Authoritative repo remains:
`F:\Spider-Man 2000 Recomp\project main`
branch: `dev`
