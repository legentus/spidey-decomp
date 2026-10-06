# CHASE VENOM AUTHORED-CADENCE PLAYER-AI FRONTIER (2026-10-06)

> **Workflow update:** the authoritative working repository is now the local Git checkout at `F:\Spider-Man 2000 Recomp\project main`. Normal TEST/FAST BATs build that local checkout directly and do not refresh from GitHub. `origin` remains a fallback/backup remote. Read the top of `docs/CURRENT_STATUS.md` before changing this workflow.

## LIVE FRONTIER — PAIRED SCRIPTED SPIDEYAI0 + ACTIVE CAMERA AT AUTHORED 20-HZ CADENCE

This section supersedes the older Chase frontier notes immediately below it.

Latest proper instrumented test:
- runtime revision: `b1174ba0935aa512641d7e4fdafe42aa6ed6df25`;
- preserved archive: `logs/20261006-004842/spidey-decomp.log`;
- result: **STILL WRONG**.

Most important proof from that run:
- the Wait05->Wait06 recovery **did fire**;
- node `298` / checksum `0x6B450F4B` was invoked;
- Wait05 persistent pulse state was complete;
- Spider-Man still failed to traverse the building;
- the blocking model checksum was **`0x1AD2FBED` = `Inside01`**;
- collision was `0x0003`;
- active worker was type 3 / code 10.

Conclusion:
- trigger progression is downstream;
- the root issue is the physical/scripted player trajectory reaching `Inside01` incorrectly.

### FPS / update-order RE

Retail call chain:
- `Ob_AI @ 0x00460FC0` -> `CBody::EveryFrame()` -> virtual AI;
- CPlayer vtable: `0x0053C464`;
- CPlayer virtual AI: `CPlayer::AI @ 0x004C65C0`;
- `CPlayer::AI` performs ordinary per-frame housekeeping, then at `0x004C684F` loads `player+0x554` and calls it;
- CPlayer construction writes `SpideyAI0 @ 0x004B13F0` into `player+0x554` via immediate `0x004BA2B5`.

Turn-controller RE:
- `CheckForwards @ 0x004BF8A0` calls `SetTargetTorsoAngle @ 0x004C6970`;
- `SetTargetTorsoAngle` computes turn target/step/budget in `field_DF0/DF4/DF8`;
- later in `SpideyAI0`, retail performs:
  - `angle += field_DF4 * field_80`;
  - `field_DF8 -= field_80`;
- the integration is elapsed-time-aware, but the AI can reevaluate steering/collision feedback between calls.

Therefore:
- one 20-FPS player update with `field_80=3` is not generally equivalent to three 60-Hz updates with `field_80=1`;
- the native-60 half-step movement preserves approximate displacement but changes update order, turning feedback, friction/collision sequencing, surface transitions and trigger sweep inputs.

### Implemented candidate

The constructor's default `SpideyAI0` callback immediate is patched to a wrapper.

Policy:
- ordinary CPlayer housekeeping: 60 Hz;
- CSuper animation: 60 Hz;
- render/presentation: 60 Hz;
- other game bodies: unchanged 60 Hz;
- only default Spider-Man `SpideyAI0` callback is gated while:
  - retail level ID is `0x501`; and
  - synthesized input `field_1AC != 0`.

During synthesized Chase control:
- canonical 60-Hz ticks accumulate;
- two callback opportunities are held;
- on the third tick retail `SpideyAI0` runs once with accumulated `field_80` (normally 3);
- original `field_80` is restored after the callback.

The old Wait05->Wait06 forced recovery is disabled for this experiment so natural authored trigger hits determine success.

Existing synth/ramp compatibility naturally reduces to original 20-Hz behavior because those functions are now entered once every three ticks with `field_80=3`.

Telemetry:
- `chase_player_ai_20hz_install`;
- `chase_player_ai_20hz_stats` with total calls, actual retail calls, held calls, max elapsed.

Forced-clean matching VC6 build: **PASS**.

Installed candidate:
- behavior commit: `795367ce3ddabb05c2aaee843f838e54750f75cf`;
- proxy SHA-256: `05810A992991ACF4B47FF0FC6C4AE536251C53F0B17BCA9B02888F8ABAB35F4B`;
- renderer11 SHA-256: `818BB56194A6DE5A8E3C92BFF661B244412BFAC2D68AA023EFB51539C11446CB`;
- input11 SHA-256: `A512066ABEA6F2A4523ED78CC2ACCA48E5D888853E514B54296B36513FDC738E`;
- prepare/install + 32-bit modern-input preflight: **PASS**.

Next action:
- test via `TEST_LATEST_BUILD.bat`;
- replay Chase Venom;
- verify whether the approach angle/path to `Inside01` changes and whether Spider-Man naturally enters/exits the building;
- exit normally and inspect the AI20 stats plus Chase trace.

## Start here

Latest tested runtime:
- `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`
- Chase Venom in-engine cutscene is visually 60 FPS.
- Route bug remains in that tested build: Spider-Man fails the building-entry course and runs into the wall.
- User-supplied `spidey-decomp(20261005-233411).log` is internally the same `df0b1d62...` revision and is therefore not a newer test.

Current source frontier:
- `0ab2efb34c841814b2313aa74301e5eb3789a7ad` — generalized Chase synthesized world-heading sample/hold.
- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546` — startup-only E32/E34/camera-heading ownership xrefs.
- `2f6e0dcbb8dec436350b523bd6de59e87be25977` — detailed worker-timing documentation descendant.

Key new RE:
- type-2 route worker converts target world direction to **camera-relative axes** using `CCamera+0x23A`;
- `ReadAnalogueInput` converts axes to `field_E32` using `field_E34`;
- `CheckForwards` adds the current transform-derived `camera+0x23A` back before torso/movement turning;
- holding only old axes while the camera runs at 60 Hz changes the world direction, so the compatibility layer now preserves the sampled **world heading**;
- compensation applies to any active synthesized analogue movement, not only when type 2 happens to be the worker-list head;
- timed worker types 3/6/7/8/9 all use `field_80`; do not slow their countdowns again.

New telemetry:
- `heading_samples`
- `heading_corrections`
- `heading_max_pre_correction_drift`
- `worker_mask_before`
- `worker_mask_after`
- fresh sample camera heading / E34 / E32 / desired world heading
- startup xrefs labeled `SpideyAI0_E32`, `SpideyAI0_E34`, `SpideyAI0_CameraHeading23A`

Detailed RE:
- `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`
- `docs/CURRENT_STATUS.md`
- `docs/HIGH_FPS_TIMING_AUDIT.md`

Next action:
- build latest `dev` with `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
- test Level Select -> Chase Venom through the known building-entry failure;
- confirm 60-FPS cutscene is retained;
- report fixed / improved / unchanged / worse;
- exit cleanly and return one consolidated log.

Do not:
- reintroduce the CBody minimum-two-tick limiter;
- globally cap the cutscene/game to 20/30 FPS;
- inflate Venom FollowDirections thresholds;
- globally scale SynthesizeAnalogueInput timers that already consume `field_80`.

---

# RETAIL PLAYER-PHYSICS RESTORATION UPDATE (2026-10-04)

Latest runtime result:
- live cutscene now plays after the Doc Ock pre-render;
- full reconstructed player physics then causes Spider-Man to fall through the scripted spawn into the yellow void and die;
- current log exits normally; this is a grounding/physics regression, not a crash.

A/B:
- old retail-physics runtime at frame 900: body Y `15695872`, valid ground shadow;
- reconstructed-physics runtime at frame 900: body Y `25915794`, shadow still near the expected ground band;
- same scripted camera/focus handoff in both.

Fix:
- `c109e2e869b06575ec942170d4bb89b568650b51` disables global `patch_physics()` installation;
- retail DoPhysics/swinging/crawling own live runtime again;
- reconstructed/native-60 physics remains in source for RE only;
- RotY and Mysterio timing fixes remain active.

Next test:
- repeat New Game -> difficulty -> Doc Ock pre-render -> in-game cutscene;
- verify Spidey stays at the intended spawn and does not fall/die;
- if restored, proceed briefly into gameplay.

Future native-60 player work must use narrow hooks inside/around retail physics, not a whole-function replacement. Known seams are documented in CURRENT_STATUS.

---

# LIVE-CUTSCENE CRASH FIX UPDATE (2026-10-04)

The first runtime of the native-60 physics + RotY batch built successfully but crashed at:
New Game -> difficulty -> first Doc Ock pre-render -> transition into the next in-engine cutscene.

Crash evidence:
- movie surface released successfully first;
- then `0xC0000005` in the proxy;
- retail stack includes `M3dColij_InitLineInfo` and `Utils_GetGroundHeight`, proving live-world collision/placement had started;
- user clarified the next scene is an in-game cutscene, not another pre-render.

Root-cause candidate and fix:
- reconstructed `CPlayer::DoPhysics` left local `SLineInfo::pItem` uninitialized when movement length was zero;
- recompiled stack layout makes that retail defect unsafe for stationary live-cutscene startup;
- `9d05114c1d401daac541023c8b3776bdc0d196fb` explicitly sets `lineInfo.pItem = 0` before the sweep.

Next test:
- repeat only the exact New Game -> difficulty -> Doc Ock movie -> in-game cutscene transition first;
- if it passes, resume normal gameplay/native-60 checks;
- if it still crashes, use the fresh log plus `Release/spider.map` to symbolize the proxy fault and isolate the physics hook.

---

# BUILD-RECOVERY UPDATE — NATIVE-60 TEST BUILD FIXED AFTER VC6 COMPILE FAILURE (2026-10-04)

The first attempt to build the combined native-60 player-physics + RotY batch at `33c3016d...` failed before runtime.

Compile blockers and committed fixes:
- `8043e1ef...` — include `physics.h` so `patch_physics()` is declared;
- `a2c022f2...` — correct crawling side-probe C78/C7C/C80 component math;
- `8bce08e5...` — replace stale CPlayer collision aliases with the corrected `mLineInfo` / `mLineInfo2` layout;
- `84a0c0d4...` — CURRENT_STATUS checkpoint documenting the failed build and exact structural mappings.

No runtime test has happened yet for the native-60 physics/RotY changes.

Next action:
- run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` again from the newest `dev`;
- if compilation succeeds, perform the existing traversal/crawl/AI/game-speed test;
- if another VC6 compile error appears, fix that first and do not treat it as a gameplay result.

The timing architecture and source conversions themselves are unchanged by this build-repair pass.

---

# NATIVE 60-HZ HANDOFF UPDATE — FIRST PHYSICS + AI CONVERSION BATCH READY (2026-10-04)

Latest tested runtime:
- `21ced52bc9c8e5903fe66cc939e4114804c248fb`;
- log `spidey-decomp(20261004-220426).log`.

That runtime proved:
- Mysterio elapsed-time laser patch installs successfully;
- master timer cadence is essentially 60 Hz, so remaining speed-up is downstream legacy per-update logic;
- all four requested retail captures completed cleanly;
- retail `CAIProc_RotY::Execute @ 0x00401110` is a raw authored-frame primitive;
- player normal/crawling physics use a two-vblank/30-Hz base quantum in important paths;
- `CVenom::SynthesizeAnalogueInput` already advances its obvious timers/path indices with `field_80`, so no blind Venom 0.5 scaler is justified.

Current implemented source:
- `6b56677c383331da12035109df59b165ae69a066` — native-60 player physics half-step integration;
- `48b1a9d503f1646f508720eae0174a57617e9ed5` — native-60 RotY half-step Execute;
- `e3109c1ee2e1f252ade18e72279c2d14b7e75ba8` — install RotY and retire completed startup dumps;
- `bb1cfe03abc61d7445dd0cf6f9eb5edfa7001b64` — CURRENT_STATUS test-frontier checkpoint.

Architecture remains:
- native gameplay simulation must be correct at 60 Hz;
- continuous legacy two-vblank physics/rotation is split across two 1/60 ticks;
- already elapsed-aware `field_80` paths stay elapsed-aware;
- event/state counters are not scaled;
- >60 FPS later comes from decoupled rendering/interpolation while simulation stays capped to canonical 60-Hz advancement.

RotY exact scheme:
- preserve retail constructor/division;
- use otherwise-unused zeroed `CAIProc::field_C` as half-step phase;
- two 60-Hz angular halves sum exactly to one retail `field_24` step;
- decrement retail `field_20` only after both halves;
- `field_80==2` reproduces one old retail step in one call;
- `field_80==1` produces smooth native-60 turning over two calls.

Do not patch yet:
- generic `CAIProc::Wait` — reconstructed source is suspicious, but retail implementation is inlined and exact countdown setup still needs grounding;
- Venom synthesized input — capture shows its obvious timing is already `field_80`-based.

Next test:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. test normal traversal, jump/fall, and crawl if convenient;
3. watch enemy/AI rotations/state transitions;
4. compare subjective overall game speed to the previous 60-FPS runtime;
5. quick manual-aim sanity only;
6. ordinary scripted/cutscene progression if convenient;
7. no hitch reproduction work;
8. return consolidated `spidey-decomp.log` plus subjective pace notes.

If build fails, fix compiler errors before runtime testing.
Do not claim CI-green: GitHub returned no visible workflow runs/statuses for this branch.

---

# NATIVE 60-HZ HANDOFF UPDATE — FULL AI/VENOM/PLAYER-PHYSICS CAPTURE READY (2026-10-04)

Latest source:
- `e4df3b156aeec6d81519697f7d9f158603dfc99e` — adds the true normal + crawling player physics retail blocks to the existing native-60 startup capture.
- `54ef05c5e8595ff48989b7c786a95374c90cf316` — CURRENT_STATUS checkpoint for that source frontier.

The next runtime should capture, in one startup-only batch:
- `CAIProc_RotY_Block @ 0x00401060`, size `0x120`;
- `CVenom_SynthesizeAnalogueInput_Block @ 0x004E9B00`, size `0x19A0`;
- `CPlayer_DoPhysics_Real @ 0x00466CE0`, size `0x1040`;
- `CPlayer_DoCrawlingPhysics @ 0x00467FD0`, size `0xD70`.

Purpose:
- prove the shared raw AI timing semantics before touching `CAIProc::Wait` / rotation;
- reconstruct the known Venom automated-input/cutscene path;
- identify any raw per-Logic player physics integration contributing to the user's slight 60-FPS speed-up;
- compare normal vs crawling physics before patching either;
- implement the first substantial native-60 correction batch after this one log.

User requirement remains absolute:
- native gameplay simulation must be correct at 60 Hz;
- no global 20/30-Hz gameplay cap under a 60-Hz renderer;
- >60 FPS later comes from decoupled presentation/interpolation over a maximum 60-Hz simulation.

No hitch testing is needed; logging cleanup is validated and the hitch branch is closed.

Expected next startup lines:
- `high_fps_compat mysterio_laser=1 ...`;
- four `high_fps_re_bytes_done ... valid=1` lines for the blocks above.

---

# NATIVE 60-HZ MASTER HANDOFF — TWO EXACT TIMING CAPTURES READY (2026-10-04)

This supersedes the older high-FPS checkpoint below.

Authoritative project:
- Repo: https://github.com/legentus/spidey-decomp
- Branch: `dev`
- Google Drive root: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
- Handoff folder ID: `1l-4gLh-jftGT1aNrP73wD8n3IScqQcvO`
- Logs folder ID: `1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s`

Latest tested runtime:
- `a4e1105d1f9568a24ca8817847573392a7f8324a`;
- log `spidey-decomp(20261004-213030).log`;
- logging cleanup eliminated the recurring hitching;
- gameplay still feels slightly sped up at 60 FPS.

User requirement:
- **60 Hz is the native/minimum simulation target**;
- do not preserve 20/30-Hz gameplay under a 60-Hz renderer;
- make game speed, physics, AI, cutscenes and boss fights correct at 60;
- later obtain >60 FPS by decoupled rendering/presentation + interpolation over a fixed maximum 60-Hz gameplay simulation.

Current source frontier:
- `6cf830b974316134a8a7813ac1eda42279eacd60` — runtime-proven Mysterio vtable guard;
- `026322a195ecb16ef41d3803ff477356504e8713` — prune resolved timing probes; retain only unresolved exact captures;
- `f835bbdb433af3edc722b3743f20e47bb05a3321` — CURRENT_STATUS native-60 frontier checkpoint.

Current startup captures:
- `CAIProc_RotY_Block @ 0x00401060`, size `0x120`;
- `CVenom_SynthesizeAnalogueInput_Block @ 0x004E9B00`, size `0x19A0`.

Critical RE already complete:
- `Ob_AI` runs `EveryFrame`, optional `UpdateFrame`, then virtual `AI` every dispatch; there is no hidden 30-Hz AI interleave.
- Therefore raw one-per-call countdowns really accelerate at higher Logic cadence.
- `CBody::EveryFrame`, `CSuper::UpdateFrame`, `CBaddy::RunTimer`, `CAIProc_MoveTo`, camera and many movement paths already use canonical elapsed ticks.
- `CAIProc::Wait` is a raw `field_C--` candidate, but do not patch until exact `CAIProc_RotY` retail semantics are captured.
- Full `SpideyAI0` review found its direct increments are event/state counters, not generic timers; do not globally scale that giant state machine.
- `CVenom_FollowDirections` is a small script/direction dispatcher and calls `CVenom_SynthesizeAnalogueInput`; the latter is the direct next target for the known chase/cutscene timing failure.

Mysterio:
- next runtime should now install the elapsed-time laser liveness fix;
- expected guard: slot0/destructor `0x0045B540`, slot1/Move `0x0045BAC0`.

Exact next test:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. expected source is `026322a...` or newer docs-only descendant;
3. boot into ordinary gameplay; no hitch reproduction is needed;
4. return the one consolidated `spidey-decomp.log`.

Required lines:
- `high_fps_compat mysterio_laser=1 ...`;
- `high_fps_re_bytes_done label=CAIProc_RotY_Block ... valid=1`;
- `high_fps_re_bytes_done label=CVenom_SynthesizeAnalogueInput_Block ... valid=1`.

After the log:
1. reconstruct the two exact binaries;
2. disassemble AI rotation/wait semantics;
3. disassemble Venom synthesized input;
4. implement the first broad native-60 timing correction batch;
5. keep state/event counters unchanged;
6. validate 60-Hz game speed before moving to >60 presentation interpolation.

Hitch branch is closed unless a new independent stall appears.
Manual aim is frozen unless regression.
Real-shadow work remains paused until native-60 timing reaches a stable checkpoint.

---

# HIGH-FPS MASTER HANDOFF UPDATE — HITCHES FIXED; NATIVE 60-HZ GAMEPLAY FRONTIER (2026-10-04)

Latest tested runtime:
- `a4e1105d1f9568a24ca8817847573392a7f8324a`
- log `spidey-decomp(20261004-213030).log`

User-confirmed:
- manual aim remains frozen/accepted;
- **logging cleanup got rid of the recurring hitches**;
- gameplay still feels slightly sped up at 60 FPS;
- 60 Hz must be the minimum/native simulation rate;
- higher refresh is to come from render interpolation/presentation above a correct 60-Hz simulation.

Current source:
- `6cf830b974316134a8a7813ac1eda42279eacd60` — correct Mysterio vtable guard to runtime-proven slot 0 `0x0045B540`; remove completed first RE byte dumps;
- `62f9500c4085a0841f5afbe9f85ee7d68b021e1f` — startup-only capture of `AIProcBlock @ 0x00401000..0x00402100` and `CPlayer_DoPhysics @ 0x004BFEC0..0x004C00B0`.

Critical new RE:
- complete `Ob_AI` capture proves no hidden 30-Hz object-AI interleave;
- each active object gets `EveryFrame()`, optional `UpdateFrame()`, then virtual `AI()` each dispatch;
- therefore raw per-call AI timers really advance at the full Logic cadence;
- `CAIProc::Wait` is the clearest reusable raw-counter candidate;
- do not patch it yet: the next build captures the exact retail AI-proc block so constructor/count semantics can be proven first;
- player physics is also captured next because user-visible speed-up could include player integration and that retail function is still missing from the decompiled tree.

Hitch branch:
- CLOSED unless a new reproducible stall appears.
- Do not re-enable high-frequency diagnostic success logging.

Timing architecture:
1. make gameplay genuinely correct at native 60 Hz;
2. preserve existing `field_80` / `gTimerRelated` elapsed-time paths;
3. convert only proven raw frame-count timing primitives;
4. keep player input/camera at 60 Hz;
5. later separate render/present from simulation;
6. interpolate for 120/144/165/240+ Hz without running gameplay faster than 60 Hz.

Exact next runtime:
- build `62f9500c...` or newer;
- verify startup line `high_fps_compat mysterio_laser=1`;
- verify `high_fps_re_bytes_done label=AIProcBlock ... valid=1`;
- verify `high_fps_re_bytes_done label=CPlayer_DoPhysics ... valid=1`;
- normal gameplay is enough; no hitch reproduction work is needed;
- return the consolidated log.

After that:
- reconstruct AI-proc block;
- classify Wait/RotY/LookAt timing semantics;
- reconstruct player DoPhysics;
- implement the first broad native-60 timing conversion batch.

---

# HIGH-FPS MASTER HANDOFF — 60-HZ CORRECTNESS + STARTUP RE CAPTURE READY (2026-10-04)

This supersedes the older reticle/hitch handoff below. Do **not** redo the camera/reticle RE.

Authoritative project:
- Repo: https://github.com/legentus/spidey-decomp
- Branch: `dev`
- Google Drive root: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
- Handoff folder ID: `1l-4gLh-jftGT1aNrP73wD8n3IScqQcvO`
- Logs folder ID: `1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s`

Latest tested runtime:
- `091c2345ef1d4c927878d6ec47c2c54efaadf9ce`
- log `spidey-decomp(20261004-203738).log`
- user result: manual aiming is perfect for now; freeze it unless regression.

Latest high-FPS source:
- `98d52ec80b5876db8e347460be307555b905de4b` — quiet timing hot-path success I/O;
- `31f80818ab5ee73cacae4b3d952205add448fc86` — quiet renderer hot-path success logs;
- `6cfcd74aaecc72a2e1ac37885a03dc4aad0f52ae` — elapsed-time Mysterio laser liveness;
- `9f8a62f46d00e437861cd55facf0a6ef91a76b4e` — startup-only missing-retail capture;
- `62c7e71dc32f6cadec9c077dde13ec66d1645a0c` — full chunked `SpideyAI0` capture;
- `52c9a5cf9c22740839a2fc03d28a013605a5f240` — detailed high-FPS architecture audit;
- `58c00e48cca88de09aba5ed5915a1904f612d5bb` — CURRENT_STATUS frontier checkpoint.

Source state: **committed, NOT runtime-tested yet**.

## What is already proven

The engine is a mixed timing model, not a simple fixed-30-FPS simulation:
- `PlayAway @ 0x004559D0` calls `Logic @ 0x00455400` and `Display @ 0x004555A0`, then waits if `Vblanks` did not advance;
- `gTimerRelated @ 0x006B4CA8` is a canonical ~60-unit/sec elapsed-time clock;
- `CBody::EveryFrame @ 0x00460ED0` derives `field_80 = gTimerRelated - previous`, capped at 6;
- `CSuper::UpdateFrame @ 0x00460DA0` advances animation by `field_80 * mAnimSpeed / 2`;
- many movement/camera/boss paths already scale by `field_80` or use `CBaddy::RunTimer`;
- other AI/effect/cutscene paths still advance raw counters once per update.

Therefore:
- do not globally multiply gameplay by a new float delta;
- do not simply remove the PlayAway wait and run Logic >60 Hz;
- do not globally force all Logic to 30 Hz and sacrifice the validated 60-Hz camera/input path.

Target architecture:
1. repair raw timing assumptions until 60-Hz Logic is correct;
2. keep simulation advancement at a canonical maximum of 60 Hz;
3. later decouple Display/presentation for 120/144/240+ Hz;
4. interpolate visible state on render-only frames where needed.

Full reasoning:
- `docs/HIGH_FPS_TIMING_AUDIT.md`.

## Mysterio high-FPS repair already implemented

Retail phase-2 laser uses a one-update liveness handshake:
- constructor `0x0045B3E0` installs vtable `0x0053BB34`;
- derived Move slot is `0x0045BAC0`;
- `SetPos @ 0x0045B5E0` writes byte `this+0x44 = 1`;
- Move kills the bit if the marker is zero, then clears it each update.

`6cfcd74...` converts only that liveness primitive to elapsed `gTimerRelated` time:
- vtable patch is guarded against exact expected slot values;
- existing marker byte is reused as a compact timestamp;
- liveness grace is three 60-Hz ticks / 50 ms, allowing the documented 20-Hz authored producer cadence;
- stale beam still dies through retail `CBit::Die`.

No global clock change.

## Hitch result

The `091c...` probe proved:
- several huge stalls were caused by our synchronous diagnostic writes;
- some genuine stalls remain in DX11 shadow replay/end-frame;
- some genuine stalls remain inside untouched retail `Logic`;
- web firing follows hitches and is not the trigger.

The two quiet-logging commits above are intended to remove the self-inflicted part and need the next runtime to validate them.

## Missing retail RE prepared for next runtime

The decompiled tree and upstream both still lack the critical routines.

Startup-only byte capture now emits:
- `Ob_AI @ 0x00460FC0`, size `0x1A0`;
- `CVenom_FollowDirections @ 0x004EB530`, size `0x160`;
- `SpideyAI0 @ 0x004B13F0`, size `0x73A0`.

Why:
- `Ob_AI` will prove actual object/AI interleave cadence before changing `CAIProc::Wait`;
- `CVenom_FollowDirections` is a direct target for the known 20-FPS Catch/Chase Venom failure;
- `SpideyAI0` contains the otherwise-stubbed player AI/cutscene state machine and is needed to locate automated player movement.

Capture is startup-only and chunked; it adds no per-frame logging cost.

## Exact next action

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

The test build must contain `62c7e71d...` or newer source.

Test:
1. boot and enter normal gameplay;
2. move through the level long enough for several of the previously recurring hitches;
3. note whether hitching is substantially reduced after logging cleanup;
4. optionally fire once immediately after a perceived hitch as a marker;
5. quick aim sanity only — manual aim is frozen/perfect;
6. return the single consolidated `spidey-decomp.log`.

The log must contain:
- `high_fps_compat mysterio_laser=1 ...`;
- `high_fps_re_bytes_done label=Ob_AI ... valid=1`;
- `high_fps_re_bytes_done label=CVenom_FollowDirections ... valid=1`;
- `high_fps_re_bytes_done label=SpideyAI0 ... valid=1`.

After receiving it:
1. reconstruct the hex chunks into exact binary functions;
2. disassemble `Ob_AI`;
3. determine object-AI interleave cadence;
4. disassemble `CVenom_FollowDirections`;
5. inspect `SpideyAI0` for automated/cutscene movement;
6. implement the next elapsed-tick repairs in small commits;
7. continue toward fixed-60 simulation + uncapped/interpolated rendering.

Do not return to real-shadow work until this timing phase is sufficiently complete.

---

# CHAT-LIMIT MASTER HANDOFF — RETICLE NO-DRAG + HITCH PHASE TEST READY (2026-10-04)

This chat ended at the exact point where the next runtime test is ready.

Authoritative project:
- Repo: https://github.com/legentus/spidey-decomp
- Branch: `dev`
- Google Drive root: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
- Handoff folder ID: `1l-4gLh-jftGT1aNrP73wD8n3IScqQcvO`
- Logs folder ID: `1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s`

Latest tested runtime:
- revision `2ec405d96253df7332d5fe6609729fb4f310b720`
- log `spidey-decomp(20261004-200625).log`

Latest untested source:
- `f3f25d9f3b134f4b7bd8d6a15f5d98ca8f9f3bf1` — same-frame post-camera reticle update
- `ca2af74d4238b3fe4255a2d8c45cff3766c91bc0` — slow-frame presenter phase partition
- `c286d708b6f6d4a8f2fefef35808d44a6179cdad` — split outside-present stalls into retail logic vs logic telemetry vs other work

Current source is **implemented and committed but not runtime-tested**.

The next chat must:
1. fetch live `dev` first;
2. read the top of `docs/CURRENT_STATUS.md` and this file;
3. run no duplicate RE before checking the latest source;
4. ask the user to run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
5. test fast manual-aim sweeps for zero reticle drag;
6. collect several hitch events;
7. use the new phase fields to decide whether stalls are inside PresentShadow/replay/transient/draw-probe/presenter remainder or outside presenter;
8. document the runtime result before changing source.

Freeze unless regression:
- hip-fire targeting;
- aimed locomotion/re-entry suppression;
- unified TPS mode-3 camera;
- 96-unit vertical manual-aim framing;
- elapsed-time 60 Hz pacing.

Do not resume real-shadow work until this camera/hitch test is evaluated.

---

# LIVE FRONTIER — RETICLE DRAG FIX + HITCH PHASE PROBE READY (2026-10-04)

## LATEST TESTED RUNTIME

Revision:
- `2ec405d96253df7332d5fe6609729fb4f310b720`

Log:
- `spidey-decomp(20261004-200625).log`

User result:
- 96-unit manual-aim vertical framing is much better;
- aimed locomotion remains fixed;
- actual manual-aim camera movement remains fixed;
- remaining manual-aim refinement: reticle/cursor visibly lags behind during fast look input;
- toward end user fired a web after each perceived hitch.

Validated hitch correlation:
- FireWeb timestamps trail the major stalls instead of preceding them;
- examples include stall frame 3961 -> shot frame 3970 and stall frame 4484 -> shot frame 4499;
- FireWeb is therefore not the hitch trigger.

## NEW UNTESTED SOURCE

- `f3f25d9f3b134f4b7bd8d6a15f5d98ca8f9f3bf1` — same-frame post-camera manual reticle update
- `ca2af74d4238b3fe4255a2d8c45cff3766c91bc0` — in-memory slow-frame presenter phase partition
- `c286d708b6f6d4a8f2fefef35808d44a6179cdad` — outside-present retail-logic / telemetry discriminator
- `f3c22660941980a0be645c6fb984e92d288d00e3` — CURRENT_STATUS checkpoint for the added discriminator

### Reticle fix

Root cause:
- SpideyAI0/SetupLookaroundCamera calculates field_DC0 before the current frame's CCamera::AI orbit update;
- camera then moves later in the same frame;
- fast look input therefore renders a reticle ray derived from stale camera state.

Fix:
- existing 0x00418458 -> 0x00416B10 framing wrapper still applies 96-unit elevated focus and calls retail;
- after retail returns, it recomputes field_DC0 from the final current-frame camera position -> framed focus;
- field_DE4 remains active;
- early SetupLookaroundCamera point remains only as a fallback.

Expected:
- `modern_manual_camera event=framing ... post_camera_reticle=1 reticle_point=...`
- no cursor drag/chase on fast mouse sweeps.

Do not tune sensitivity or the 96-unit framing unless user explicitly asks after this test.

### Hitch phase probe

Existing slow-event timing only says the interval between presenter entries was late.

New in-memory fields:
- `present_work_us`
- `outside_present_us`
- `record_timing_us`
- `transient_us`
- `shadow_end_us`
- `draw_probe_us`
- `present_shadow_us`
- `other_present_us`

All phase measurements use QPC in memory. No new per-frame file I/O.

Interpretation:
- large present_shadow_us = actual DX11 present/GPU wait;
- large shadow_end_us = replay/end-frame;
- large transient_us = transient surface processing;
- large draw_probe_us = draw capture flush;
- large other_present_us = another presenter-side operation;
- large outside_present_us with small presenter values = hitch is in game/update/render code before presenter.

Additional `c286d708...` outside-present discriminator:
- `logic_retail_us` = time inside untouched retail gameplay logic `0x00455400`;
- `logic_telemetry_us` = time spent writing the once-per-second logic timing line;
- `logic_calls` = wrapped retail logic calls in that inter-present interval;
- `outside_nonlogic_us` = outside-present remainder after subtracting the two logic buckets.

Why this was added before testing:
- the old runtime has a strong correlation between several 300–630 ms stalls and the gameplay-logic timing hook immediately before the delayed present;
- that ordering alone cannot distinguish a retail logic stall from synchronous telemetry I/O;
- the next log now resolves that ambiguity without another runtime cycle.

## EXACT NEXT ACTION

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`c286d708...` or newer**.

Test:
1. manual aim and perform fast mouse sweeps;
2. check whether reticle remains snappy/no-drag;
3. verify framing and aimed locomotion still work;
4. fire while aiming/moving/turning;
5. wait for several hitches;
6. optional: shoot once after each hitch again;
7. quick hip-fire sanity;
8. return one consolidated log.

Do not resume real-shadow work until this camera/hitch test is evaluated.

---

# LIVE FRONTIER — TPS CAMERA MOVES; VERTICAL AIM FRAMING READY (2026-10-04)

## LATEST TESTED RUNTIME

Revision:
- `0ed86176ca66461e96693cba29a2a32c2fc8bde8`

Log:
- `spidey-decomp(20261004-195535).log`

User result:
- aimed locomotion still works;
- unified manual-aim camera now really moves/orbits;
- reticle/cursor is again effectively centered through Spider-Man, so aiming feels obstructed.

Runtime confirms real manual-aim orbit movement:
- yaw changes substantially during aim (for example `3804 -> 17 -> 494 -> 724 -> 1380 ...`);
- camera position changes accordingly;
- aimed movement still changes Spider-Man world position.

Freeze:
- locomotion/re-entry suppression;
- unified mode-3 look input;
- timer pacing;
- hip-fire targeting.

## NEW UNTESTED SOURCE

Source:
- `e68c8de3021c20119c47b3d15bc0372bd907883f` — vertical TPS aim framing
- `b37788fe417a26d39e00389ac87853e9cc9a08c2` — VC6-safe telemetry scope fix

Docs:
- `45301e82bf22a6ee18047e6bf6ca4336c0ec27f5` — CURRENT_STATUS checkpoint

### New manual-aim framing

Goal:
- keep actual TPS camera freely movable;
- put Spider-Man below the reticle instead of underneath it;
- reticle should appear slightly above Spider-Man;
- web ray should follow that same visible aim line.

Implementation:
- framed focus = Spider-Man body position with Y shifted 96 game units upward;
- exact fixed-point shift = `96 * 4096 = 393216`;
- patch direct call `0x00418458 -> 0x00416B10`;
- CM_Normal still owns orbit position/collision;
- wrapper changes only `field_144` before retail shared postprocess builds final orientation;
- LoadIntoMikeCamera remains untouched;
- reticle field_DC0 ray is now camera -> same framed focus, extended x8.

Expected startup:
- `modern_manual_camera_framing installed=1 ...`
- `modern_camera_install ... manual_focus=framed_above_body manual_framing=1 framing_up_units=96 ...`
- `modern_manual_aim_install ... reticle_source=framed_tps_camera_ray ...`

Expected runtime:
- `modern_manual_camera event=framing ... framing_up_units=96`
- reticle logs include `framed_focus` and `framing_up_units=96`.

Success:
- Spider-Man visibly sits below reticle;
- camera still orbits/tilts while aim held;
- aimed movement remains functional;
- webs land along reticle/view direction.

If the vertical offset feels wrong, tune 96 up/down. If vertical framing is good but Spider-Man still blocks aim horizontally, add a small shoulder offset next. Do not restore the old independent free-view cursor.

## EXACT NEXT ACTION

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`b37788fe...` or newer**.

Test:
1. hold manual aim and judge reticle vertical placement relative to Spider-Man;
2. sweep camera left/right/up/down;
3. aim/fire at several world points;
4. move W/A/S/D + diagonals while aiming and rotating camera;
5. release aim and verify clean normal-camera return;
6. quick hip-fire sanity.

Return one consolidated log.

---

# LIVE FRONTIER — AIMED MOVEMENT FIXED; UNIFIED TPS MANUAL-AIM CAMERA READY (2026-10-04)

## LATEST TESTED RUNTIME

Revision:
- `759e58dc4e27f06cb7678a1a06e4ed3743463328`

Log:
- `spidey-decomp(20261004-194202).log`

Validated user result:
- manual-aim locomotion now works;
- Spider-Man no longer vibrates/resets in place;
- camera still did not behave like a modern TPS while manual aim was held.

Runtime proof:
- locomotion mask reaches `actual_aim_state=0`;
- `enter_suppressed` climbs into the hundreds;
- body position/velocity change significantly during aimed movement;
- movement release restores normal aim state.

Freeze the movement/re-entry solution unless a regression is observed.

## TIMER STATUS

The elapsed-time periodic source is now behaving much better:
- repeated settled windows at exactly `60.000 Hz`;
- `vblank_one=60`, `vblank_multi=0`;
- timer callbacks and virtual ticks stay matched;
- source callbacks and elapsed source milliseconds stay matched.

The prior steady 58–59 Hz one-shot drift is gone. There are still isolated stalls/transitions; do not touch timing again unless the user still perceives a recurring hitch.

## NEW UNTESTED SOURCE

Gameplay source:
- `44dbfec52838fdadbd5b82556b15ced208297220` — `gameplay: unify manual aim with TPS orbit camera`

Docs:
- `b506a0d0a640c5bac80a7bf1d257b91849bc302d` — CURRENT_STATUS implementation checkpoint

### New camera model

Old manual-aim design used two competing camera spaces:
- real mode-3 orbit yaw/pitch;
- independent manual free-view yaw/pitch + final publish override.

That is now removed from the active path.

While manual aim is held:
- mouse/right stick updates the real `gSpideyModernCameraYaw`;
- pitch updates the real `gSpideyModernCameraYDistance`;
- retail CM_Normal owns orbit position/collision/focus;
- no post-CM `field_144` free-view rewrite;
- no final LoadIntoMikeCamera quaternion override;
- reticle/web aim follows the resulting camera ray;
- locomotion re-entry guard remains untouched.

This is meant to behave like a modern third-person shooter:
- camera moves with look input;
- movement continues underneath it;
- reticle/web direction is camera-relative.

No shoulder offset is implemented yet. If the camera works but reticle placement overlaps Spider-Man or feels too centered, next step is an over-the-shoulder offset—not reintroducing the old free-view cursor camera.

Expected startup:
- `modern_camera_install ... manual_aim_free_view=0 manual_tps_unified=1 ... manual_publish=0 ...`

Expected runtime:
- `modern_manual_camera event=tps_orbit ... reticle_policy=camera_ray`

## EXACT NEXT ACTION

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`44dbfec5...` or newer**.

Test:
1. hold manual aim and move mouse/right stick left/right/up/down;
2. actual camera should now orbit/tilt;
3. move W/A/S/D + diagonals while aiming and rotating camera;
4. fire webs while aim + move + camera rotation are simultaneous;
5. report reticle placement/accuracy, especially whether it overlaps Spider-Man;
6. release aim and confirm normal camera resumes cleanly;
7. quick hip-fire sanity;
8. report whether any periodic hitch remains perceptible.

Return one consolidated log.

Do not resume real-shadow work until this camera test is evaluated.

---

# LIVE FRONTIER — FINAL MANUAL-CAMERA PUBLISH + AIM RE-ENTRY GUARD + ELAPSED 60 HZ TIMER READY (2026-10-04)

## LATEST TESTED RUNTIME

Revision:
- `93d63347505da81a769c1d58ba62361c4650f4b5`

Logs:
- `spidey-decomp(20261004-190816).log`
- `spidey-decomp(20261004-191012).log`

User result:
- manual aim cursor now moves and web direction is mostly accurate;
- visible camera itself still did not rotate during manual aim;
- movement while aiming still failed, with Spider-Man visibly vibrating/resetting;
- user still saw a small hitch and attempted web-fire timestamps near the end.

Decisive log evidence:
- manual free-view yaw/pitch/focus changes correctly;
- movement mask engages;
- CheckForwards reaches run state;
- raw `field_8EA` is then reasserted to 1 while the sidecar remains active;
- body position remains unchanged;
- old exact 24-frame 31–33 ms timer beat is gone in settled gameplay;
- first phased timer source nevertheless settles around 58–59 Hz due one-shot re-arm drift.

## NEW UNTESTED SOURCE

- `71d6bf609bca7ba9cacc860aa008db03c302da74` — final manual-camera publish + redundant aim-entry guard
- `b4d961a4a349bf397a45b5025aaeba353a3f479f` — periodic timer source + direct FireWeb timestamp
- `26a03d05afe21e82517bee43400129c1190176b5` — anchor timer dispatch to absolute `timeGetTime` elapsed milliseconds
- `879ee5ffbb3a4696312d9682c55ec56adcbdba1f` — CURRENT_STATUS checkpoint

### Camera

Canonical retained CCamera::AI call order:
- `0x00418414 -> CM_Normal 0x00418E00`
- `0x00418458 -> shared postprocess 0x00416B10`
- `0x0041865F -> LoadIntoMikeCamera 0x00416A20`

The previous build changed field_144 after CM_Normal, so reticle/web moved but visible quaternion did not.

New code patches the final LoadIntoMikeCamera call. During effective manual aim only:
- derive orientation from camera position -> manual focus;
- build it with retail Utils_CalcAim / RotMatrixYXZ / MToQ;
- temporarily publish through retail LoadIntoMikeCamera;
- restore internal retail quaternion immediately afterward.

Expected:
- `modern_manual_camera event=publish ...`

### Movement

EnterLookaroundMode @ 0x004C3580 explicitly writes `field_8EA=1`.

New code scans the player-AI range for exact direct calls to EnterLookaroundMode and routes them through a guard:
- initial entry still calls retail;
- redundant re-entry is suppressed while the modern locomotion sidecar is active;
- reasserted raw field_8EA is cleared while movement owns the sidecar.

Expected:
- startup `enter_reentry_calls=<nonzero>`;
- movement lines add `enter_retail`, `enter_suppressed`, `raw_reclear`;
- successful runtime should keep `actual_aim_state=0` while mask is active and finally show nonzero body delta.

### Timing

Old chained one-shot 16/17 ms source removed the original 24-frame ~32 ms beat but accumulated re-arm latency.

New source:
- one real 1 ms periodic WinMM heartbeat;
- absolute delivery deadlines use `floor(n*1000/60)+1`;
- heartbeat phase uses actual `timeGetTime` elapsed milliseconds, not callback count;
- untouched retail TimerCallback/MyVSync receives only the intended 16/17 ms deliveries.

Expected:
- source callbacks ~1000/sec;
- dispatched timer callbacks / gameplay logic near 60/sec;
- no old 24-frame double-frame cadence.

### Hitch timestamps

All direct main-EXE calls to FireWeb @ 0x004C5DD0 are wrapped only to record:
- `fire_web_calls`
- `last_fire_frame`

Slow gameplay-event threshold is now 18 ms and records those fire timestamps in the existing buffered timing line.

## EXACT NEXT ACTION

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`26a03d05...` or newer**.

Test:
1. manual aim + mouse/right-stick: actual visible camera should rotate and cursor/web should remain aligned;
2. aim + W/A/S/D/diagonals: Spider-Man should translate rather than vibrate;
3. aim + move + camera + fire together;
4. release aim while moving; quick pause/unpause/camera-transition sanity;
5. play long enough to observe several small hitches and fire near them when practical;
6. quick hip-fire sanity;
7. return one consolidated log.

If compile fails under VC6, capture the compiler output before runtime testing.

Do not resume shadow-map work until this test is evaluated.

---

# LIVE FRONTIER — VC6 COMPILE FAILURE RECOVERED; RETRY SAME RUNTIME TEST (2026-10-04)

The user attempted the free-manual-aim / aimed-locomotion / phased-60-Hz-timer test from revision:
- `9e3679f7b5dc79d78f3acb2b4f55fc8f6645c75d`

It **did not compile under the project's matching VC6 toolchain**, so no new gameplay result exists.

VC6 errors were isolated to the new timer IAT hook:
- pointer-typed `IMAGE_THUNK_DATA::AddressOfData` broke RVA arithmetic;
- pointer-typed `IMAGE_THUNK_DATA::Function` broke raw DWORD assignment;
- VC6's `InterlockedExchange` prototype rejected `volatile LONG*`.

Corrections:
- `9b7e6b32767adc4f941c0ef1a31331ae2c4885ca` — raw thunk values now copied through VC6/header-neutral storage and first Interlocked cast fixed;
- `54185f2192320881a3c37d59d1e2b39b8c4ec18c` — remaining two Interlocked casts fixed;
- `0657180eff56795297cbeb8f35e26ddf2394a8fe` — CURRENT_STATUS build-failure checkpoint.

No intended gameplay/timing behavior changed from the prior test frontier.

## NEXT ACTION

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` again.

Required source revision: **`54185f21...` or newer**.

If it compiles and launches, perform the same test:
1. manual aim camera can look away from Spider-Man;
2. W/A/S/D + diagonals while aiming actually translate;
3. aim + move + camera + fire;
4. clean release of aim while moving, pause/unpause, camera transition;
5. judge the regular ~0.4 s hitch;
6. quick hip-fire sanity;
7. upload the single consolidated log.

If the compiler reports another error, capture the build output first; do not treat it as a gameplay result.

---

# LIVE FRONTIER — FREE MANUAL AIM + AIMED LOCOMOTION + 60 HZ TIMER PHASE BUILD READY (2026-10-04)

## READ THIS FIRST

Latest **tested** runtime:
- `spidey-decomp(20261004-182002).log`
- tested revision: `8958de0676dda897b5c8dc346493276d4c5ffffd`

Latest source is newer and **UNTESTED**:
- `edf6dcc169efce90f9655f8038eeda9337278902` — phased 60 Hz timer source
- `af820d29ebf2344bf087860a4287df0c6307c85d` — free manual-aim view + locomotion aim-state sidecar
- `a50f64b3afb22b55c30d508619e2d09691542f38` — timer hook fail-closed/atomic cleanup hardening
- `c7c20392e10c354c4b292810af4feb8dd229e54a` — manual-aim ownership/mode-transition hardening
- `d47b2000b0554df5d09c8272863e1f5c6106164a` — CURRENT_STATUS test-frontier checkpoint

### What the latest tested runtime proved

- Hip-fire remains effectively fixed. Do not change its selector without a demonstrated regression.
- Manual web direction is now correct: where the user could aim, webs consistently traveled in that direction.
- The remaining camera issue was that retail mode 3 still published Spider-Man's body as `camera.field_144`, so the view remained centered on him.
- Manual locomotion still failed even though CheckForwards entered run state; a later manual-aim reset returned the player to stand with zero position/velocity.
- The small hitch is phase-locked to roughly every 24 presented frames / 0.4 s, matching the retail 16 ms (62.5 Hz) multimedia timer beating against the 60 Hz virtual-vblank clock.
- Runtime shadow probe confirms `G_MECHLIST @ 0x006A9038` head is the Spider-Man actor (`region=spidey`), so it is valid as the current-player identity for the camera wrapper.

### New untested behavior

**Manual aim camera**
- seeds independent yaw/pitch from the current visible camera ray;
- retail mode 3 still owns camera position/collision;
- mouse/right stick changes the independent manual view;
- after retail camera position generation, `camera.field_144` becomes a forward free-aim focus rather than Spider-Man's body;
- field_DC0/web direction continues to consume that same camera ray.

**Manual aim movement**
- while aim + movement are both held, retail `field_8EA` is masked across locomotion;
- a sidecar preserves effective manual-aim state for modern camera/reticle logic;
- release/mode/player ownership transitions restore the real aim flag;
- present-time validation is the fail-safe.

**Timer**
- keeps retail TimerCallback/MyVSync/pause/accumulator;
- replaces only the fixed 16 ms periodic source with chained one-shot 16/17 ms 60 Hz phase timing;
- timeKillEvent is hooked before timeSetEvent so synthetic IDs are impossible without cleanup ownership;
- hook failure leaves retail timing untouched.

## EXACT NEXT USER TEST

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Verify the loaded session is **`c7c20392...` or newer** (documentation commits after it are expected).

Test:
1. manual aim left/right/up/down — can the view now aim away from Spider-Man, and does the web/reticle follow?
2. while aiming, W/A/S/D + diagonals — does Spider-Man actually translate?
3. aim + move + camera + fire simultaneously;
4. release aim while moving — verify clean exit/no stuck state;
5. quick pause/camera-mode regression;
6. play long enough to judge whether the regular ~0.4 s micro-hitch is gone/reduced/unchanged/worse;
7. quick hip-fire sanity only;
8. return the single consolidated `spidey-decomp.log`.

Expected markers:
- `timer_pacing_install ... atomic_cleanup=1 ...`
- `timer_pacing event=intercept ...`
- `timing_present ... timer_active=1 ... timer_unexpected_delta=0 ...`
- `modern_manual_camera event=acquire/update ...`
- `modern_manual_aim event=movement ... aim_state=1 actual_aim_state=0 locomotion_mask=1 ...`

Do not resume real-shadow work until this runtime is evaluated.

---

# LIVE FRONTIER — HIP-FIRE VALIDATED; CAMERA-RAY MANUAL-AIM DIAGNOSTIC BUILD READY (2026-10-04)

## READ THIS FIRST

Latest user runtime log:
- `spidey-decomp(20261004-175613).log`
- runtime-reported revision: `b779d80f11cd94704c4acccb95cdd8ac1dcd70ec`

Important recovery distinction:
- the above `b779d80f...` runtime identity was not present on recoverable GitHub `dev`;
- the user-visible result from that runtime is still authoritative test evidence;
- current recoverable gameplay source is now committed on GitHub.

Latest gameplay source:
- `303232155bf7bc61235aa18a883d6a9b89f4cc7a` — `gameplay: align manual aim ray and add hitch diagnostics`
- this source is **implemented but NOT runtime-tested yet**.

Documentation checkpoints after the runtime result/source change:
- `530f73b1031551ce04dfbd5530f17c5f3b4f18ab` — record third-pass runtime result;
- `3d146a3f73b54fc512f45075efcdd9f92be64a47` — document the new camera-ray/hitch diagnostic implementation.

## Latest proven runtime result

**Hip-fire:** now effectively fixed according to the user.
- Direct `source=modern_camera_scan` acquisitions appear in the runtime log.
- The selector uses the unmodified visible ray `camera.field_144 - camera.mPos`.
- Do not rewrite or retune hip-fire unless a future test shows a regression.

**Manual aim:** still broken in the last runtime.
- cursor direction remained inverted;
- cursor could leave the screen while camera moved;
- Spider-Man could not translate while aiming;
- CheckForwards nevertheless received real WASD axes and returned success;
- later samples still showed unchanged body position and zero body velocity.

**Hitching:** the small periodic frametime blip returned.
- the user intentionally attempted to fire a web at each observed hitch near the end of the session;
- existing timing telemetry shows recurring ~31–33 ms present intervals during otherwise ~60 Hz gameplay;
- avoid high-frequency disk logging while diagnosing this.

## New untested source behavior at `30323215...`

1. Manual aim no longer reflects X/Y around camera origin.
   - `field_DC0` now lies on the exact same unmodified visible camera ray that already works for hip-fire.
   - This fixes the geometrically invalid third-pass point where X/Y were reversed but Z remained forward.

2. Manual movement telemetry is throttled.
   - no longer opens/closes the log for every successful CheckForwards call;
   - logs transitions + periodic samples;
   - adds body position/velocity, animation, collision, aim/wall/ceiling flags, ignore-input and ground-grace state.

3. Reticle telemetry adds `body_delta` so actual translation can be distinguished from pose/animation changes.

4. Slow presents are buffered in memory.
   - intervals >25 ms record frame, interval_us, web-target-call count, and `check_web_shot` count;
   - records are printed only during the existing once-per-second timing flush;
   - expected marker: `[TIMING] slow_present_events ...`.

5. No locomotion bypass beyond the existing CheckForwards wrapper was guessed in this pass.
   - movement remains intentionally diagnostic until the new state fields identify the post-CheckForwards blocker.

## EXACT NEXT USER TEST

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` and confirm the session revision contains `30323215...` or newer.

Then in one gameplay session:
1. enter manual aim and sweep camera left/right/up/down;
2. report whether reticle direction is natural, whether it stays on-screen, and whether it is actually centered on camera look direction;
3. while still aiming, hold W/A/S/D individually for about two seconds and try a diagonal;
4. try aim + move + camera rotation + fire;
5. quick hip-fire regression only — do not spend time retesting already-good targeting;
6. play long enough to catch several recurring frametime blips;
7. if practical, keep firing a web on/near each observed hitch as a manual timestamp;
8. return the single consolidated `spidey-decomp.log`.

Expected diagnostic markers:
- `modern_manual_aim event=movement ... collision=... aim_state=... wall=... ceiling=... ignore_input=... ground_grace=...`
- `modern_manual_aim event=reticle ... body_delta=...`
- `timing_present ... slow_event_count=... slow_event_stored=...`
- `[TIMING] slow_present_events ... check_web_shot_calls=...`

Do not resume real-shadow work before evaluating this test. The prior world-space caster probe already passed.

---

# LIVE FRONTIER — VC6 COMPILE BLOCKER FIXED; CURRENT BUILD MUST BE RERUN (2026-10-04)


## MANDATORY RECOVERY / LIVE-UPDATE RULE FOR THE NEXT CHAT

**Do not keep important work only in the conversation.** This project must survive an `error in input stream` at any point.

Required operating procedure:
- fetch live `dev` before working;
- read the newest `docs/CURRENT_STATUS.md`;
- continuously append confirmed RE findings / runtime results / exact next steps to `CURRENT_STATUS.md`;
- commit + push every coherent source fix promptly;
- also commit doc-only RE checkpoints after meaningful discoveries, even if no source changed yet;
- update `NEW_CHAT_HANDOFF.md` whenever the recovery frontier materially advances;
- before asking the user to test, make sure source + docs are already committed and pushed;
- after the test result arrives, checkpoint the conclusion before starting the next fix;
- if interrupted, recover from live GitHub first rather than redoing work from the transcript.

The user explicitly requested frequent live Git updates so interruptions do not lose progress. Treat this as mandatory, not optional housekeeping.


## 2026-10-04 LATEST — THIRD-PASS MANUAL AIM / HIP-FIRE IMPLEMENTED

Latest tested build before this source change: `7a6af671...`.
User observed:
- Spider-Man tried to move but remained stuck/twisted;
- WASD still moved the old reticle;
- reticle X and Y were both inverted;
- hip-fire targeting remained inconsistent.

Log proof:
- CheckForwards received full axes and returned success while aim was held;
- direct modern hip-fire selector never produced `source=modern_camera_scan`.

New source commit:
- `b074592eb6bd8d5b0b6f323165de71e8d97eb248` — `gameplay: isolate modern aim from legacy lookaround`

Changes:
- skip retail `SetupLookaroundCamera @ 0x004C38A0` entirely during modern mode-3 manual aim;
- preserve `field_8EA` aim state but own `field_DC0/field_DE4` directly;
- flip field_DC0 X/Y screen-plane convention while preserving forward Z;
- this removes legacy WASD lookaround/pose ownership that was fighting locomotion;
- hip-fire modern scan now walks the exact retail `SelectTargetBaddy` list at `0x0056E990` instead of `G_MECHLIST @ 0x006A9038`;
- added periodic scan diagnostics and manual-aim body position/velocity/state/animation telemetry.

NEXT:
Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, confirm `b074592e...` or newer, then test manual aim movement, natural reticle axes, simultaneous move+aim+fire, and camera-centered hip fire. Return consolidated log.



## 2026-10-04 CHECKPOINT — DO NOT LOSE THIS MANUAL-AIM FRONTIER

Latest tested runtime: `7a6af671ec671d7f61a1003b296f59d655b39dcd` with `spidey-decomp(20261004-093356).log`.

User result:
- Spider-Man **tries** to move while manual aim is held (body twists/leans) but translation is still blocked;
- WASD still moves the legacy aiming cursor;
- mouse moves the modern camera, but the resulting reticle response is inverted on BOTH X and Y;
- hip-fire auto-targeting is still not fully reliable.

Interpretation:
- the earlier CheckForwards gates are no longer the whole movement problem; a later locomotion/state gate still blocks translation;
- SetupLookaroundCamera still consumes E2D/E2E for legacy reticle motion, so its axis reads must be neutralized/overridden during modern aim while preserving normal movement axes;
- current camera-ray -> field_DC0 sign convention is reversed for reticle projection and needs both axes corrected;
- hip-fire must continue as a separate camera-ray-selection problem.

Exact resume order:
1. inspect latest `modern_manual_aim event=movement/reticle` telemetry;
2. trace the post-CheckForwards movement commit/state gate that leaves Spider-Man twisting in place;
3. stop SetupLookaroundCamera from consuming WASD/E2D/E2E for reticle motion while modern aim is active;
4. correct both reticle axes so left/right/up/down are natural;
5. retest hip-fire camera scan independently;
6. only then resume Renderer11 real-shadow implementation (caster probe already passed).

Do not revert or recreate the existing commits:
- `aabf69a90786b639c4e32e1d74e64cae88e3430e`
- `b44b3bdca3337c0cfe4a57dc8a042feaa0a946ea`



## 2026-10-04 LATEST FRONTIER — SECOND MANUAL-AIM PASS + DIRECT CAMERA TARGET SCAN

Supersedes the prior first-pass modern-manual-aim test instructions.

User tested `bf1fb237...`:
- patches installed, but WASD still aimed the legacy reticle and Spider-Man remained locked;
- mouse moved the modern camera, not the reticle;
- hip-fire camera targeting remained inconsistent.

Latest source:
- `aabf69a90786b639c4e32e1d74e64cae88e3430e` — movement/reticle input decoupling
- `b44b3bdca3337c0cfe4a57dc8a042feaa0a946ea` — direct visible-camera hip-fire target scan

Manual aim now wraps the actual SpideyAI0 call sites:
- `0x004B231A -> CheckForwards`: temporarily clear held input byte `player->field_E0C + 0x40` only while movement is evaluated, then restore; E2D/E2E movement axes remain intact.
- `0x004B8673 -> SetupLookaroundCamera`: keep retail state logic but force `field_DC0` to the mode-3 camera center ray after retail lookaround processing; the reticle is rendered from field_DC0.

Hip fire now scans targettable, non-zombie, valid-radius bodies directly against `camera.field_144-camera.mPos`, applies real player range + untouched retail LOS, and chooses the most centered candidate. The prior retail camera-transform selector remains fallback only.

Next test must update to `b44b3bdc...` or newer and verify movement during manual aim, camera-centered reticle, firing while moving, and stable close/medium hip-fire acquisition. Return the consolidated log.



## 2026-10-04 LIVE UPDATE — PAUSE VALIDATED; MODERN MANUAL AIM + CAMERA-ORIGIN WEB TARGETING

Fetch live `dev`; this section supersedes the earlier “rerun VC6 blocker” instructions below.

Current gameplay commits:
- `58742eb419fa755d2c044425ae2cbc44963ac731` — `gameplay: modernize manual aim and camera target origin`
- `f34aa5ddc3efd3c2571dd0de8e9483276108b1fd` — `gameplay: validate camera targets with retail LOS`
- `3f5b574cc5ca120902d119b042d32f3ebacaf3b4` — status checkpoint

Latest runtime result:
- pause/unpause is fixed; custom Pause Options box uses the in-place lifecycle path and no longer crashes;
- world-space shadow probe passed for Spider-Man and NPC supers;
- web targeting is still inconsistent in the old orientation-only camera patch;
- user also wants manual aim modernized so mouse/right stick aim while Spider-Man can still move.

Manual-aim RE:
- `CPlayer::EnterLookaroundMode @ 0x004C3580` sets `field_8EA=1`, pushes the camera mode, then explicitly switches to retail mode 7 / FRONT;
- `CPlayer::CheckForwards @ 0x004BF8A0` has the exact movement lock `jne` at `0x004BF8C5` when `field_8EA != 0`.

New first-pass modern manual aim:
- validated bytes `0x004C370B: 6A 07 -> 6A 03`, retaining the aim/reticle state but keeping normal mode-3 camera ownership;
- validated bytes `0x004BF8C5: 0F 85 3F 01 00 00 -> NOP x6`, allowing ordinary CheckForwards locomotion while aiming;
- all other retail aim-state restrictions remain untouched;
- existing relative mouse and Input11 right-stick camera channels now remain active in aim mode;
- patches fail closed through exact-byte validation.

Web-targeting root cause:
- retail `SelectTargetBaddy` builds the angular vector from Spider-Man's body position even when we replace its orientation matrix;
- a third-person camera behind/above the player therefore creates parallax: screen-centered close targets can still fail the body's forward cone.

New mode-3 targeting path:
- use the visible render camera position + orientation for the retail centeredness scorer;
- preserve retail candidate eligibility and cached player-distance weighting;
- restore Spider-Man position/matrix immediately;
- revalidate accepted target with untouched retail `Utils_LineOfSight @ 0x004E67A0` from Spider-Man's real position;
- retain the previous orientation-only retail call as a fallback when necessary.

Expected runtime markers:
- `modern_manual_aim_install camera_mode=1 ... movement=1 ...`
- `camera_web_target ... source=render_camera_origin ...`
- fallback marker: `source=render_camera_orientation_fallback`

Shadow status:
- probe is complete/passed, not merely pending;
- do not keep extending probe telemetry;
- next shadow implementation is a dedicated Renderer11 world-space caster submission ABI, using local `SModel` geometry plus live per-part pose and `CSuper::mTransform`, then a directional depth-map pass and receiver sampling;
- do not attempt to reconstruct world geometry from the current XYZRHW replay;
- do not remove legacy blob shadows until the real shadow path is visually stable.

Exact next runtime:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. verify the loaded revision contains the two gameplay commits above;
3. manual aim: enter aim, move view with mouse/right stick, move Spider-Man simultaneously, fire while moving;
4. target close + medium enemies centered by camera while Spider-Man faces elsewhere;
5. report any exit/recenter snap;
6. quick pause regression;
7. return the single consolidated `spidey-decomp.log`.



## 2026-10-04 LIVE UPDATE — PAUSE HARDENING, BOTH WEB PATHS, SHADOW CASTER PROBE

The original ZIP handoff froze `dev` at `ea5f5f676d60ceb36a3c78df51aaffa73509b0dd`. **Continue from live `dev`, not that frozen commit.**

New source frontier:
- `f7c2532a956610768609e6732ec64e83b89bd463` — `pause: resize expanding box in place`
  - removes retail `CMenu::Zoom` / delete+reallocate from the hooked pause frame;
  - preserves the live `CExpandingBox` and updates its target rectangle in place under SEH.
- `23260d11fb8a0b0533b9d0588cb96b927c8def44` — `gameplay: align CheckWebShot targeting to camera`
  - wraps both retail `SelectTargetBaddy` call sites: `0x004C5B2F` (SelectAutoAimTarget) and `0x004C09E2` (CheckWebShot);
  - both use the same camera-forward temporary scoring matrix and preserve the retail scorer.
- `62d8711b09eb633a3ddc8a5271aeff0f55a697c6` + `0b72b0a340a3676f4678b8d0fc6888d24af242b7`
  - guarded world-space shadow-caster telemetry for the retail mech-list head and current web target.

Shadow RE breakthrough:
- Renderer11's normal replay still sees projected `FVF 0x144 / XYZRHW` vertices, so do not attempt to invert that path back to world space;
- historical `thps2-stuff/m3d.mik::RenderSuperItem` proves the pre-projection engine has the exact inputs required for real animated shadows: `CSuper::mTransform`, per-part `SMatrix` animation transforms, `SModel` local vertices, and face lists;
- the new runtime probe logs region/model/pose/ground-contact evidence as `[SHADOW] world_space_probe ...`;
- once validated, next implementation is a Renderer11 world-space caster submission API + directional depth-map pass, then removal of legacy character blobs only after the replacement is stable.

Exact next combined runtime test:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. confirm the session revision is newer than the frozen handoff revision;
3. pause/unpause repeatedly and enter/exit custom Pause -> Options several times;
4. test web attacks with the camera and Spider-Man deliberately facing different directions;
5. remain near a targetable NPC for more than five seconds;
6. return the single consolidated `spidey-decomp*.log`.

Do not request a separate shadow-only test before examining that combined log.

**Fetch live `dev` first. GitHub outranks this file if it has advanced.**

Live frontier immediately before this handoff refresh:
- code fix: `71290b003e895f179c2ab1921fb59bdb39b1753e` — `compat: scope process attach locals for VC6`
- status checkpoint: `5bdcdb2eaf3461889549cdaad3928e08adca4007` — `docs: record VC6 switch-scope build fix`

The user's most recent FAST build attempt was revision:
`5709bdef9269f6d6e9e02c2c0ced6a74f3fbaf56`

It failed in VC6 with C2360 because `FILE* runtimeVersionLog` was initialized directly under the `DLL_PROCESS_ATTACH` switch case and later case labels could jump across that initialization.

The compile blocker is now fixed:
- the entire `DLL_PROCESS_ATTACH` case body is enclosed in its own braces;
- the local `runtimeVersionLog` lifetime cannot cross `DLL_THREAD_ATTACH`, `DLL_THREAD_DETACH`, or `DLL_PROCESS_DETACH`;
- no runtime behavior was intentionally changed;
- static structure after the edit: braces 911/911, parentheses 4507/4507, brackets 323/323.

**This fixed revision has not yet been locally compiled/runtime-tested by the user.**

## Exact next action

Run:
`FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`

If the matching VC6 build succeeds, perform the already-pending current-build validation in one session:

1. Pause/unpause repeatedly during gameplay, including around active/aggro enemies.
2. Test camera-centered web targeting against both non-aggro and aggro enemies while Spider-Man's body faces elsewhere.
3. Orbit the camera around Spider-Man and NPCs and verify floor/blob shadows remain anchored underneath their owners.
4. Open Pause -> Options and verify the purple container encloses all six custom rows through Back, then Back returns to a correctly-sized parent pause box.
5. Brief move/swing/orbit regression.

Expected current-build startup/runtime evidence:
- `[RUNTIME] runtime_revision=<current revision>`
- `camera_web_target_install ... forward_axis=negative_local_z`
- `quadbit_camera_anchor installed=1 ... reason=ok`
- `pause_menu_box_refresh reason=add_options_parent refreshed=1 ...`
- `pause_menu_box_refresh reason=enter_options refreshed=1 ...`

If pause still crashes on the current build, upload the fresh consolidated `spidey-decomp.log`; also preserve any crash log if one is generated.

Do **not** treat the previous `ef78...` runtime log as evidence that the new web-axis, QuadBit-anchor, or dynamic pause-box fixes failed. That log predates all three.

---

# LIVE CONTINUATION — STALE PLAY-CURRENT TEST IDENTIFIED; FRESH CURRENT-BUILD LOGGING ADDED (2026-10-04)

The latest user feedback was produced by an **older installed build**, not the current source frontier.

Uploaded log identity:
- revision `ef78ea5ff959f4518a96567aed340bd935c251e5`
- clean `exit_code=0`

That run predates all three fixes waiting for validation:
- `5e3dd2e...` web-target forward-axis correction
- `a24b4d27...` world QuadBit/blob-shadow camera-basis restoration
- `dd35f977...` dynamic Pause Options expanding-box resize

Therefore:
- do not conclude those fixes failed from that log;
- do not patch the separate aggro/CheckWebShot targeting path until the current axis-corrected build is tested;
- the reported pause crash is not captured in the uploaded file.

Workflow hardening now on dev:
- `f892eaa163459fdca5e7504e6e0a83cfcdff6d9a` — runtime writes `runtime_revision=<RUNTIME_VERSION>` into direct-launch logs.
- `5709bdef9269f6d6e9e02c2c0ced6a74f3fbaf56` — `RUN_GAME.bat` clears stale logs, launches the installed build only, waits for exit, records exit code, and leaves a fresh `spidey-decomp.log`.

NEXT:
1. Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` once to install current dev.
2. Then use `RUN_GAME.bat` for no-update/no-build playtests.
3. Re-test:
   - pause during gameplay;
   - camera web target vs non-aggro and aggro enemies;
   - NPC + Spider-Man blob shadows while orbiting camera;
   - Pause -> Options container sizing.
4. If anything fails, upload the fresh `spidey-decomp.log`.

---

# LIVE CONTINUATION — LAST UPLOADED RUNTIME WAS OLD ef78 BUILD (2026-10-04)

The user's latest reported runtime problems (pause crash, inconsistent web targeting especially after enemy aggro, blob shadows still camera-relative) came from `spidey-decomp(20261004-071556).log`, whose session revision is:

`ef78ea5ff959f4518a96567aed340bd935c251e5`

That run predates the current fixes:
- `5e3dd2e...` web forward-axis correction;
- `a24b4d2...` QuadBit camera-anchor correction;
- `dd35f97...` dynamic pause expanding-box rebuild.

The log does not contain `quadbit_camera_anchor` or `pause_menu_box_refresh`, and it ends with `[SESSION] exit_code=0`; therefore it is not a valid runtime test of current `dev` and does not itself capture the reported crash.

Current action:
1. user must run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` once to install current dev;
2. only then use `PLAY_CURRENT_BUILD.bat` for repeated tests without downloading/rebuilding;
3. verify the session header is current and startup contains the new web/shadow markers;
4. retest pause, aggro/non-aggro web targeting, and blob shadows;
5. if pause still crashes, collect the new consolidated log plus any crash log.

Do not rewrite current fixes based on the stale ef78 runtime.

---

# LIVE CONTINUATION — WEB AIM + BLOB SHADOW + DYNAMIC PAUSE BOX READY FOR RUNTIME TEST (2026-10-04)

This is the current live frontier. It supersedes older test instructions below.

Source fixes now on `dev`:

- `5e3dd2e066de9bd89d94dd675f152baaf85d22e3` — camera web targeting: align active-camera forward with retail SelectTargetBaddy's negative-local-Z scoring convention.
- `a24b4d27f586c97af175e5202bb9fb7db2268cbe` — blob/world QuadBits: restore the active camera GTE rotation before retail DisplayQuadBitList so floor blobs stay world-anchored.
- `dd35f977e54e963d0deaede43c895cd7a8d2a95e` — Pause Options: rebuild the existing retail expanding box from the current row list via `CMenu::Zoom @ 0x0043FC60`.

The Options container fix is dynamic, not a six-row pixel hack:
- entering custom Options rebuilds the box from current submenu rows;
- Back restores the parent CMenu state then rebuilds the parent box;
- initial insertion of Options above Quit also rebuilds the parent box;
- future row-count changes should therefore follow retail `GetMenuHeight()` automatically.

## Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, then test in one session:

1. **Camera web targeting**
   - turn Spider-Man's body away from a baddy;
   - center the baddy with the camera;
   - fire the normal enemy-targeting web;
   - expected: selection/hit follows camera center;
   - verify ordinary straight-ahead targeting too.

2. **Blob shadow anchoring**
   - keep a thug/cop/NPC stationary;
   - orbit the camera around them;
   - expected: floor blob remains under the NPC rather than sliding with camera rotation;
   - briefly watch other QuadBit effects for regression.

3. **Pause Options container**
   - open Pause -> Options;
   - expected: purple container encloses Options, UI Scale, Text Scale, Camera Sensitivity, Apply Settings, and Back;
   - press Back;
   - expected: parent pause container encloses Options and Quit normally.

4. Brief move/swing/orbit regression.

If anything is wrong, upload only the single consolidated `spidey-decomp.log` and describe the visible result.

Useful lines:
- `camera_web_target_install ... forward_axis=negative_local_z`
- `quadbit_camera_anchor installed=1 ... reason=ok`
- `pause_menu_box_refresh reason=enter_options refreshed=1 ...`
- `pause_menu_box_refresh reason=add_options_parent refreshed=1 ...`

Do not patch the separate `CheckWebShot @ 0x004C09E2 -> SelectTargetBaddy` path unless runtime evidence demonstrates the normal enemy-targeting action still needs it. That call stores a separate handle state and is not the grounded `SelectAutoAimTarget -> player+0xDCC -> FireWeb` path fixed here.

---

# LIVE CONTINUATION — WEB TARGET AXIS + QUADBIT CAMERA ANCHOR READY FOR RUNTIME TEST (2026-10-04)

This continuation supersedes older web-target/shadow hypotheses below.

## New source fixes

### Camera-forward web targeting
Commit:
- `5e3dd2e066de9bd89d94dd675f152baaf85d22e3` — `gameplay: align camera web aim with retail forward axis`

Canonical retained-function RE established:
- `SelectAutoAimTarget @ 0x004C5AA0` stores the result of `SelectTargetBaddy @ 0x004C8410` into `player+0xDCC`.
- `FireWeb @ 0x004C5DD0` directly consumes `player+0xDCC` when a target exists; there is no separate body-facing hit gate that needs replacing first.
- `SelectTargetBaddy` transforms player-to-candidate through `player+0x89C`, normalizes it, and scores **negative local Z** for forward/centeredness.
- the active camera QToM transform uses **positive local Z** as visible camera-forward.

Fix:
- continue temporarily substituting only the scoring matrix at the existing `0x004C5B2F` hook;
- after QToM(camera->field_214), negate row 2 of the temporary camera matrix;
- preserve untouched retail eligibility/range/LOS/scoring and restore `player+0x89C` immediately after selection.

### Character blob-shadow anchoring
Commit:
- `a24b4d27f586c97af175e5202bb9fb7db2268cbe` — `render: restore camera transform for world quad bits`

Canonical RE established:
- standard thug/cop floor shadows are the CBody -> CQuadBit path.
- `CBody::UpdateShadow @ 0x004605A0` and `CQuadBit::OrientUsing @ 0x00409400` produce camera-independent world-space geometry.
- `Bit_Init @ 0x00407FC0` registers `QuadBitList` with `DisplayQuadBitList @ 0x004097E0`.
- registration PUSH is at `0x004081D4`, dword target operand `0x004081D5`.
- `DisplayQuadBitList` subtracts `gMikeCamera[0].Position` and projects with `gte_rtps`, but does not reload the camera rotation matrix itself.
- `M3d_RenderSetup @ 0x00472DC0` loads `SCamera::Transform` through `gte_SetRotMatrix @ 0x0046D7B0`; later model rendering can overwrite that shared GTE rotation state.
- active retail camera transform is `gMikeCamera[0].Transform @ 0x0056F1E4`.

Fix:
- replace only the QuadBitList display callback registered by retail Bit_Init;
- wrapper reloads `0x0056F1E4` via untouched `gte_SetRotMatrix`;
- wrapper then calls untouched retail `DisplayQuadBitList`;
- no shadow world position, floor/collision data, quad corner generation, or Renderer11 replay coordinates are altered.
- `patch_CBit()` was checked and does not replace Bit_Init, so this registration hook remains live.

## Exact next combined runtime test

Run:
`FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`

Then in one gameplay session:

1. **Web targeting**
   - put Spider-Man's body facing away from a baddy;
   - center the baddy with the camera;
   - fire the normal enemy-targeting web;
   - it should acquire/hit based on the camera center;
   - repeat with baddy somewhat left/right and above/below if practical;
   - also verify normal straight-ahead targeting still works.

2. **Blob shadow**
   - find a thug/cop/NPC with the normal circular/soft floor blob;
   - keep the NPC stationary;
   - orbit the camera around them;
   - blob should remain under the NPC instead of sliding as camera angle changes;
   - briefly watch other world-space QuadBit effects for regressions.

3. **Quick regression**
   - move/swing/orbit for a minute;
   - no need to re-test the already-confirmed large-hitch removal or sensitivity unless something looks wrong.

If either new fix is wrong, upload **only** the single consolidated `spidey-decomp.log` and describe the visible result. Useful startup lines are:
- `camera_web_target_install ... forward_axis=negative_local_z`
- `quadbit_camera_anchor installed=1 ... reason=ok`

Do not re-enable the old blocking readbacks.

---

# LIVE CONTINUATION — BAT WORKFLOW CLEANUP (2026-10-04)

The repository launcher surface has been simplified. Old historical sections may mention deleted BAT names; those references are historical only.

Current supported BAT entry points:
- `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` — normal authoritative update/build/install/test workflow
- `TEST_LATEST_BUILD.bat` — full non-fast recovery/test workflow
- `UPDATE_SPIDEY_PROJECT.bat` — standalone updater
- `GET_SPIDEY_PROJECT.bat` — first-machine bootstrap
- `RUN_GAME.bat` — run the currently installed build only; no update/download/rebuild
- `RESTORE_STOCK_GAME.bat` — restore retail Bink
- `build.bat` / `clean.bat` — low-level build helpers

Deleted obsolete wrappers:
`BUILD_AND_INSTALL.bat`, `BUILD_DEV.bat`, `INSTALL_DEV_BUILD.bat`, `SETUP_FIRST_TIME.bat`, `SPIDEY_DEV_MENU.bat`, `UPDATE_AND_TEST_LATEST_BUILD.bat`, `UPDATE_PROJECT.bat`, and `scripts/update_project_worker.bat`.

Do not recreate those wrappers unless a concrete workflow requires them.

---

# LIVE CONTINUATION — FRAME-PACING CLASSIFICATION ADDED TO THE COMBINED TEST (2026-10-04)

The original full handoff frontier was verified exactly before continuing:
- handoff/live dev at recovery: `df1adf4f2664a5183146efd639dac2a65ae5ecf9`;
- no hidden newer source work existed and no RE was repeated.

New continuation work is telemetry-only and does **not** change camera, web targeting, renderer output, or retail timing behavior:
- `08e4acdafaa38b267fcb57862c8e2a5ed8539703` — low-overhead frame cadence telemetry;
- `f69abaf545193bb1441d3421661e3f752daf896d` — VC6-safe cadence arithmetic.

The exact combined runtime test is still the next authoritative user action. It now also produces enough evidence to classify any smaller hitch that survives the already-disabled 120-frame blocking readbacks.

New `timing_present` fields:
- `cadence_intervals`;
- `over20ms`, `over25ms`, `over30ms`, `over50ms`;
- `max_interval_us`;
- `vblank_same`, `vblank_one`, `vblank_multi`, `vblank_max_delta`.

Why this matters:
- the active gameplay presenter is DX11 shadow, not the HDC fallback;
- old log samples average ~5,357 tiny DX11 commands / ~16,299 vertices per frame, which is a later CPU-efficiency target but did not positively correlate with the old low-Hz windows;
- reconstructed retail `PCTIMER_Init` uses a 16 ms multimedia timer and accumulates 0.96 of a 60-Hz vblank per callback;
- if retail `MyVSync` advances `Vblanks` as expected, that arithmetic produces a 32 ms cadence gap about every 25 callbacks (~0.4 s), making the timer/vblank model a strong residual micro-stutter hypothesis;
- do **not** replace the timer yet: current `MyVSync` source is still an incomplete decomp stub, so validate with the new runtime cadence data first.

NEXT:
Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` once, test Camera Sensitivity + major-freeze behavior + smaller hitch behavior + camera-forward enemy web targeting + brief movement/swing regression, then upload only the consolidated `spidey-decomp.log`.

---

# HANDOFF PACKAGE CHECKPOINT — CAMERA FOLLOW-UP BATCH AWAITING ONE COMBINED RUNTIME (2026-10-04)

This file was refreshed specifically for a new-chat handoff after an interrupted engineering turn.

**Verified live dev before this handoff refresh:** `0c0f22614ca9e6b835e2b524d896659e47f8aec5`

Nothing from the interrupted work was lost. The repository already contains:

- the runtime-validated Stage-A 360-degree mode-3 orbit camera;
- persistent Camera Sensitivity in Pause -> Options;
- both periodic synchronous renderer readbacks disabled by default;
- camera-forward web enemy auto-targeting at `SelectAutoAimTarget -> SelectTargetBaddy`;
- reduced camera telemetry/logging traffic after the first camera success;
- complete RE notes and the exact combined runtime test below.

**Do not redo the RE or re-implement these features.** The next chat should fetch live `dev`, read the first/current section of this file plus the tail of `docs/CURRENT_STATUS.md`, and continue from the runtime test frontier.

The next authoritative user action is still one combined run of `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` validating:

1. Camera Sensitivity at an obvious low/high value and persistence after Apply.
2. Whether the old repeating half-second-class freeze is gone; separately note smaller hitching.
3. Camera-centered enemy web targeting while Spider-Man's body faces elsewhere.
4. A brief normal movement/swing/camera regression pass.
5. Upload only the single consolidated `spidey-decomp.log`.

If web targeting is mirrored, behind, or vertically wrong, use `camera_web_target` telemetry and the documented transform convention before changing axes. If the large periodic freeze is gone but smaller hitching remains, keep profiling from the new no-readback baseline instead of re-enabling old readback diagnostics.

---

# LATEST FRONTIER — CAMERA FOLLOW-UP BATCH READY FOR COMBINED TEST (2026-10-04)

The first modern mode-3 orbit camera is runtime-validated by the user and considered a strong success.

Current dev now includes three requested follow-ups:

1. Persistent Camera Sensitivity in Pause -> Options
   - 25..200 percent, 5 percent steps, default 100 percent;
   - scales both relative mouse and Input11 right stick;
   - saved under [Controls] CameraSensitivityPercent in spidey-modern-video.ini;
   - custom pause submenu is now six rows: heading, UI Scale, Text Scale, Camera Sensitivity, Apply Settings, Back.

2. Periodic hitch mitigation
   - old DirectDraw GetDC/GetPixel diagnostic readback disabled by default;
   - old Renderer11 staging texture + blocking D3D11_MAP_READ sample disabled by default;
   - both had 120-frame cadence and are high-confidence causes of the user's large repeating freeze;
   - opt-in env flags preserve diagnostics if needed:
     SPIDEY_DIAG_SURFACE_READBACK=1
     SPIDEY_RENDERER11_DIAG_READBACK=1
   - do not declare all smaller hitching solved until runtime validation.

3. Camera-forward web enemy targeting
   - exact retail chain grounded:
     CheckWebShot 0x004C0510
     SelectAutoAimTarget 0x004C5AA0
     FireWeb 0x004C5DD0
     SelectTargetBaddy 0x004C8410
     SelectTargetSwitch 0x004C8570
   - SelectAutoAimTarget call 0x004C5B2F is the only patched targeting call;
   - retail SelectTargetBaddy normally scores candidate centeredness through player + 0x89C;
   - wrapper temporarily supplies the active mode-3 camera orientation, generated from camera field_214 through retail QToM @ 0x0047C7F0;
   - retail candidate filtering, range weighting, LOS and result handling remain untouched;
   - Spider-Man's original field_89C matrix is restored immediately after selection;
   - other SelectTargetBaddy callers remain unchanged.

Implementation commits in this batch:
- 61419a368c7a7e962673a74f7052fe55465254ff — perf: disable periodic DX11 diagnostic readback
- 6cb305796f272043654ed3819bb92e7c6c53249a — perf: disable periodic DirectDraw pixel sampling
- 2ca650606bf1c851859c699be7e019b131f69e5b — input: add persistent camera sensitivity state
- fdfe39ec6000856c655dbf137bf90c66d9c59aa8 — input: persist camera sensitivity setting
- f6fcaa31edb7ae5cc8ea8cb8df2091ecfd302413 — pause: add camera sensitivity option
- fa0f7e230c210019d8c6066076585fb84144b8ee — pause: wire camera sensitivity controls
- fde069d43a284c27ca385ef427a6751f3b5bd06f — camera: apply configurable sensitivity
- 954882bb63a86c39011bbc3996104f905b8559aa — gameplay: aim web auto-targeting from camera
- c71240e223884de59665e63c1b8984b606301845 — perf: throttle validated camera telemetry
- 1313b3840b8aaa0696783d6fe0d6baadee9b20b2 — docs: checkpoint camera sensitivity and web targeting
- 9ad45941f8c772933733973f00356195a8fe862f — docs: checkpoint camera follow-up batch
- 743b02c937ccb92130f781dfab454e4e0144c110 — docs: record camera telemetry throttle

Runtime evidence from prior revision 5ec06e...:
- modern camera installed and acquired successfully;
- user reported no camera-control complaints;
- sampled modern updates retained requested yaw with retail_overrode_yaw=0;
- timing windows are often normal ~59-61 Hz but intermittently fall into the mid-40 Hz range.

Canonical targeting RE confidence:
- retained Git function blobs for SelectAutoAimTarget, SelectTargetBaddy, SelectTargetSwitch, CheckWebShot and FireWeb were independently matched byte-for-byte against the materialized same-build executable by Git blob SHA.

NEXT AUTHORITATIVE STEP:
Run FAST_UPDATE_AND_TEST_LATEST_BUILD.bat once and perform one combined test:
- Camera Sensitivity at 50 percent then optionally 150 percent; Apply and verify persistence.
- Observe whether the old every-several-seconds major freeze is gone; separately note smaller hitching.
- Face Spider-Man away from an enemy, center enemy with camera, fire enemy-targeting web; camera should choose the target.
- Brief movement/swing/camera regression check.
- Upload the single consolidated spidey-decomp.log.

If web targeting is mirrored/behind/vertically wrong, do not guess: use camera_web_target telemetry plus the exact transform convention RE before changing axes.

---

# LATEST FRONTIER — MODERN MODE-3 ORBIT CAMERA PROTOTYPE UNDER TEST (2026-10-04)

Pause Options/UI scale milestone is closed. User confirmed keyboard Enter now works for Options, Apply Settings and Back, with mouse behavior preserved.

The active task is now the requested modern 3D gameplay camera: 360-degree horizontal orbit where possible plus limited vertical pitch, using mouse/right stick.

Implemented on dev:

- e15892a8c76eef180a932b25d6bfb13e62795adb — mode-3 modern orbit prototype.
- d2d546551255be6d0b83919236d0aed7098dcbd3 — camera ownership transition guards.
- 09f0cd9cb65a65868d97a70f1bce8910118d75ba — detailed MODERN_INPUT_CAMERA.md documentation.
- 9055c9a980ce5c89428b69a5e222050f8d6f0dfd — CURRENT_STATUS checkpoint.

Exact retail seam:
- CCamera::AI @ 0x00417CB0
- mode-3 call site 0x00418414
- retail CM_Normal target 0x00418E00
- retail post-mode collision/orientation still runs at 0x00416B10
- final LoadIntoMikeCamera @ 0x00416A20

Prototype behavior:
- only camera mode 3 can be modern-owned;
- no camera mutation until explicit mouse/right-stick intent;
- yaw seeds from live field_236 and wraps freely 0..4095;
- vertical orbit seeds from live Y distance and is clamped -480..+260;
- retail XZ distance remains intact;
- derived retail mode-3 radius/vertical-angle inputs are recomputed before CM_Normal;
- retail CM_Normal and the downstream collision/orientation pipeline still execute;
- ownership drops on non-mode-3 camera, pointer change or detach, so scripted/boss/special cameras remain retail-controlled.

Input:
- relative DirectInput mouse X/Y already captured by existing wrapper;
- Input11 normalized right stick already available;
- mouse yaw scale 3; pitch scale 2;
- full right-stick yaw 32 angle units/frame; pitch 7 Y-distance units/frame;
- one input snapshot consumed at most once per completed frame.

Key telemetry:
- modern_camera_install ...
- modern_camera event=acquire ...
- modern_camera event=update ... retail_overrode_yaw=...
- modern_camera event=release ...

Next authoritative step:
Run FAST_UPDATE_AND_TEST_LATEST_BUILD.bat and test ordinary gameplay orbit/pitch, movement/swinging, basic wall/corner collision, and right stick if available. Report direction/sensitivity and upload the single consolidated spidey-decomp.log. Use telemetry before making speculative architectural changes.

If Stage A proves too constrained by wall/ceiling/orientation behavior, proceed toward the dedicated Stage-B modern gameplay camera described in docs/MODERN_INPUT_CAMERA.md rather than overfitting legacy camera modes.

---

# LATEST FRONTIER — RAW DIRECTINPUT ENTER LATCH UNDER TEST; CAMERA NEXT (2026-10-04)

Runtime on `081479c4741962a808bb7e326c3edb3e67685d9a` still showed the custom pause Options UI working only by mouse for Open / Apply / Back. Options ordering remains correct immediately above Quit.

Important correction: the previous `0x10` Enter code was in the tested source; stale startup telemetry made it look otherwise. Exact retail disassembly shows `PCSHELL_CheckTriggers` owns a separate Enter one-shot latch at `0x00AC1238`, so a late second query can be suppressed.

Current `dev` implementation:

- `c55f7dbc1a6bf38b76912d8ee44e17fb667e01a6` — reads the raw DirectInput state byte for DIK `0x1C` through retail `DXINPUT_GetKeyState @ 0x00501CB0`, treats both `0xFF` fresh press and `0x7F` held as physically down, and creates our own one-press latch for custom pause rows.
- `8fd5f5309facc7faf88fd53c4d3c05abb7609dbc` — startup marker now says `pause_keyboard_source=raw_directinput_dik_0x1c`.
- `5af38bd169f96962511f858d9ccb62ac704e7e7c` — live status checkpoint.

Next test: run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`; verify startup marker first, then Enter on Options, Apply Settings, and Back. New diagnostics `pause_enter_state raw=...` will distinguish raw keyboard delivery from routing failure if anything remains wrong.

If this passes, checkpoint immediately and move to camera work.

---

# LATEST FRONTIER — VERIFIED RETAIL ENTER MASK PATCH UNDER TEST; CAMERA NEXT (2026-10-04)

The previous pause interaction patch correctly moved Options immediately above Quit, but Enter still did not activate Options, Apply Settings, or Back. Runtime on `b7acebd821d7bbefa95c7e69b2071be68045055e` showed only the mouse confirmation path firing.

Exact retail disassembly of `SpideyPC.exe` resolved the mistake:

- `PCSHELL_CheckTriggers @ 0x0050C180`
- mask `0x10` directly checks DIK `0x1C` = Enter
- mask `0x100` is mouse left click
- mask `0x1000` is not Enter

Implemented on `dev`:

- `85df9590dc42d03145756e05e67f59b44a69f45d` — custom pause Enter fallback corrected to verified retail mask `0x10`; telemetry now uses `source=keyboard_enter_mask_0x10`.
- `2a0e575db32d0a26d77172592a2fc930b771ff3d` — live status checkpoint with disassembly evidence and exact next test.

Next test: `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`. Verify Options remains above Quit, Enter opens Options, Enter activates Apply Settings, Enter activates Back, and mouse clicks still work. If this passes, immediately checkpoint the pass and move to camera work.

---

# LATEST FRONTIER — PAUSE OPTIONS KEYBOARD + ORDER POLISH UNDER TEST; CAMERA NEXT (2026-10-03)

Runtime on `e5ca5a25fc6b0d6ccdbb85b446b18b03c0a825c0` proved the custom pause Options submenu works end-to-end for visibility, UI Scale, Text Scale, Apply and persistence. Remaining user-reported polish:

- Options could only be opened by mouse click, not Enter.
- Apply Settings could only be activated by mouse click, not Enter.
- Options was appended after Quit; user wants Options immediately above Quit so Quit stays last.

Implemented on `dev`:

- `9581ef731b21568da55a99cd49eb7de4cf593e7c` — native retail confirm action `0x1000` now services custom Options rows; parent Options insertion moves before the previous final retail row and preserves final-row selection.
- `648b293e3590c1346decfab8028a6b64c2e4b479` — refreshed startup telemetry for five-row submenu, before-last insertion and confirm action.
- `fbb660f865a324fdeee6d91f38818f35fa9d493c` — live status checkpoint.

Next test is short and final for this milestone: run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, verify Options is immediately above Quit, Enter opens Options, Enter applies settings, Enter activates Back, and mouse behavior still works. Expected confirmation telemetry includes `source=confirm_action_0x1000`.

If this passes, checkpoint the pass and move directly to camera work. Do not reopen already-proven UI scale/compass/HUD work unless the user reports a regression.

---

# LATEST FRONTIER — UI SCALE ROW-0 FIX; CAMERA NEXT AFTER ONE SHORT TEST (2026-10-03)

**THIS SECTION OVERRIDES THE OLDER PAUSE-OPTIONS TEST STATE BELOW.**

The user's runtime test of session revision `bfdaf30d9c6eb75983862e168d753b9903fbf8ae` succeeded overall:

- custom in-level **Options** submenu opens and works;
- **Text Scale** was visible, adjustable, applied live, and persisted correctly;
- the **Gameplay UI Scale** row was not exposed/usable;
- the user explicitly wants to move to camera work as soon as this final UI-scale issue is closed.

Runtime proof:
- submenu entered as four rows;
- all observed scale-adjust events were `kind=menu_text line=1`;
- Text Scale applied from 100% to 115%;
- Gameplay UI remained 125%;
- Apply and Back both completed cleanly.

The fix is already on live `dev`:

- `ba5691d4fb84feeb6334269bb1edce1778c23627` — **fix: expose gameplay UI scale in pause Options**
- `4159190bcaa7341896561f913504cc34f437c987` — **chore: tighten pause Options row fix**
- `b9dc5127a4cbcd99abbc0fda5a2fe245ee9ca7de` — **docs: record pause Options runtime and UI-scale row fix**

New submenu layout:

0. `Options` — disabled heading / sacrificial retail first row
1. `UI Scale: N%`
2. `Text Scale: N%`
3. `Apply Settings`
4. `Back`

The submenu now starts selection on row 1. Shape validation requires all five entries. New enter telemetry reports row count, selected line, cursor line, and heading-disabled state.

## Exact next action

Run only this short test:

1. `UPDATE_AND_TEST_LATEST_BUILD.bat`
2. gameplay -> Pause -> Options;
3. confirm `UI Scale: N%` is now visible and initially selected;
4. change UI Scale to something obvious such as 150%;
5. Apply Settings;
6. return to gameplay and verify the HUD geometry changes immediately;
7. reopen Options and confirm the applied value persisted;
8. optionally nudge Text Scale once to ensure it still works.

Expected log:
- `pause_options_state action=enter ... rows=5 line=1 ... heading_disabled=1`
- `pause_options_adjust kind=gameplay_ui line=1 ...`
- `pause_ui_apply old_gameplay=125 new_gameplay=...`
- `pause_options_confirm action=apply line=3 rows=5 ...`

If that passes:
- checkpoint the success in `docs/CURRENT_STATUS.md` immediately;
- **move directly to the requested camera implementation**;
- do not spend another runtime cycle on already-proven UI work unless a regression appears.

The same runtime also continued to show the single-hook compass arrow geometry inside the established compact-holder region. Do not reopen health/web-cartridge/compass transforms unless the user reports a visual problem.

Fetch live `dev` first in every new chat; it outranks this document if newer.

---

# CURRENT NEW-CHAT FRONTIER — PAUSE OPTIONS SUBMENU + COMPASS ISOLATION (2026-10-03/04)

**READ THIS SECTION BEFORE ALL OLDER MATERIAL BELOW.**

The user is actively runtime-testing the current build while this handoff is being prepared.

## Source of truth / fetch-first rule

1. Fetch live `dev` first.
2. Read the tail of `docs/CURRENT_STATUS.md`.
3. Let live GitHub outrank this ZIP if `dev` has advanced.
4. Do not redo already-grounded RE unless new runtime evidence contradicts it.
5. Repo/status is the checkpoint; chat is not.

Active repository:
https://github.com/legentus/spidey-decomp

Dev branch:
https://github.com/legentus/spidey-decomp/tree/dev

Google Drive project root:
https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

Runtime Logs folder:
https://drive.google.com/drive/folders/1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s

Handoff folder:
https://drive.google.com/drive/folders/1l-4gLh-jftGT1aNrP73wD8n3IScqQcvO

## Exact checkpoint at handoff creation

Live `dev` HEAD when this handoff was prepared:

`bfdaf30d9c6eb75983862e168d753b9903fbf8ae`

That HEAD is documentation-only:
- `bfdaf30` — **docs: checkpoint pause Options submenu and compass isolation**

Latest code checkpoint immediately beneath it:
- `47129a8ef6d9de995206b61e9ea04f3dcef79b98` — **guard: lock pause Options snapshot to CMenu layout**

Current code frontier also includes:
- `d45947c0241e9f226f0a8dd326f2a23fa113194c` — standalone custom pause **Options** submenu
- `018b16c3ecdd1f8ad8746a0d75e4013a43152f8a` — atomic pause-options handler repair
- `5f309f4709902995a1648882a1b5b8ffa2a30f8b` — transform only the dynamic compass-arrow geometry

The user is currently testing this frontier. **Wait for their test result before changing these paths again.**

## Current behavior under test

Parent pause menu:
- original retail rows remain;
- exactly one new row is added: **Options**.

Custom in-level Options submenu:
1. `UI Scale: N%`
2. `Text Scale: N%`
3. `Apply Settings`
4. `Back`

Important architecture:
- this is our own gameplay-safe submenu;
- it does **not** invoke retail `Shell_Options` or `PCSHELL_DoDisplayOptions`;
- the same live pause `CMenu` object is temporarily repurposed;
- bytes from CMenu offset `+0x8` onward are snapshotted/restored;
- vtable and expanding-box pointer are never overwritten;
- compile-time guard requires `sizeof(CMenu) == 0x53C`.

Expected controls:
- left/right adjusts UI Scale or Text Scale by 5%;
- Apply Settings commits both and saves `spidey-modern-video.ini`;
- UI scale changes on subsequent HUD draws;
- text scale is re-applied immediately;
- Back without Apply discards pending changes;
- Apply, then later edit, then Back preserves applied values and discards only later edits.

## Latest runtime facts that MUST NOT be lost

### Health/web-cartridge paths

The user has confirmed:
- health bar/fill is now inside its holder;
- web-cartridge HUD pieces are in the correct location.

The broad-panel QPoly passthrough fix is proven. **Do not rescale those six already-live QPolys again.**

### Compass

Prior build transformed all three compass QPolys. Runtime logging proved:
- only `0x00463D19` is the dynamic arrow polygon that needs compact bottom-right anchoring;
- `0x00464035` and `0x00464257` are already in live HUD space and were being double-transformed.

Current code transforms only:
- `0x00463D19`

and leaves:
- `0x00464035`
- `0x00464257`

on retail live-space behavior.

Current telemetry:
`gameplay_ui_alignment source=compass_arrow_qpoly ... call=0x00463D19 ...`

### Why the old in-level Display Options approach is forbidden

Calling retail `PCSHELL_DoDisplayOptions @ 0x0050D9B0` from gameplay crashed in `Shell_DrawBackground @ 0x0048DA90` because the frontend background object at `0x006A7780` is null during gameplay.

Do not reintroduce that architecture.

### Why graphical sliders are forbidden in the gameplay pause menu

Calling retail slider draw `0x00498060` while paused crashed because frontend slider animation resources are not loaded:
- `Spool_FindAnim` returned null;
- retail advanced that to a bogus `0x18` frame pointer;
- `Panel_DrawTexturedPoly_1 @ 0x00462B30` faulted at `0x00462B3B` reading `0x1C`.

The custom pause Options submenu must stay resource-free unless gameplay-safe art is explicitly implemented later.

## Current edge-case simulation already performed

The submenu state machine was statically/simulated through:
- normal open -> adjust -> Apply -> Back;
- Back without Apply;
- 50% lower bound;
- 200% upper bound;
- Apply with no changes;
- pause menu pointer change during submenu;
- same-object retail menu rebuild during submenu;
- 40-row capacity guard;
- Apply, then further edits, then Back;
- CMenu layout drift guarded by compile-time size check.

Do not claim runtime validation until the user's current test returns.

## Exact next action for the next chat

**First response after ingesting this handoff:**
- fetch live `dev`;
- read `docs/CURRENT_STATUS.md`;
- inspect whether anything advanced past `bfdaf30`;
- then wait for / process the user's in-progress runtime result.

When the user returns with a result:
- pull the single newest `spidey-decomp.log` from the connected Drive `Logs` folder unless they attached it directly;
- correlate any screenshot with the same session revision;
- check:
  - `pause_options_entry`
  - `pause_options_state action=enter`
  - `pause_options_adjust`
  - `pause_options_confirm action=apply`
  - `pause_ui_apply`
  - `ui_text_scale reason=pause_options_apply`
  - `pause_options_state action=restore`
  - `compass_arrow_qpoly`
  - any `[CRASH]` lines.

If the test passes, checkpoint it immediately in `CURRENT_STATUS.md` and commit before moving to new work.

## Mandatory live-update / disconnect-safe protocol

This is a user requirement.

During substantial work:
1. fetch live `dev` before editing;
2. read `CURRENT_STATUS.md` before reconstructing anything;
3. live-update `docs/CURRENT_STATUS.md` throughout the work;
4. commit small grounded milestones to `dev`;
5. document rejected hypotheses and failed call paths;
6. do not hold important RE only in chat;
7. prefer fewer, larger runtime tests;
8. keep the single-log policy;
9. on stream/input interruption:
   - fetch live `dev`;
   - inspect newest commits;
   - read status docs;
   - identify exactly what survived;
   - reconstruct only the missing tail;
   - checkpoint immediately;
   - continue from repo, not memory.

## Single-log policy

Routine runtime evidence is one file:
- `spidey-decomp.log`

Do not ask for the historical collection of separate compat/draw/present/texture/input/etc. logs.

---

# GAMEPLAY / PAUSE UI SCALING FRONTIER — READ FIRST (2026-10-01)

Latest Drive session tested revision:
`0b2d306b48beadfb1d0de6dc45a8dad4f4fb3faf`

Result:
- frontend/main-menu resolution-aware text is now correct and persists;
- gameplay/pause text remained retail density;
- gameplay HUD widgets remained original relative size instead of using compact high-resolution density.

Fixes now implemented:
- `563fdd671d89904a4f59bb910ab798a7c8615776` — **ui: scale gameplay and pause text with resolution**
- `d680f800c244ab13dfcc805ceb9225b08a657115` — **ui: scale gameplay HUD widgets with resolution**

Grounding:
- retail gameplay panel renders in a 512x240 reference space and already expands panel quads to live logical resolution;
- new wrapper applies the inverse high-resolution density before that existing retail transform;
- at 2560x1440 density is X=0.25 / Y=0.333333;
- rectangles retain nearest edge/center anchoring;
- frontend and non-panel full-screen effects are excluded.

NEXT TEST:
1. updater/build;
2. confirm main menu text still correct;
3. gameplay: inspect Spider-Man HUD widgets and text;
4. pause: inspect pause text and menu graphics;
5. optionally compare 1920x1080 and 2560x1440;
6. put only new `spidey-decomp.log` into Drive Logs folder.

Drive Logs:
https://drive.google.com/drive/folders/1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s

Expected:
- gameplay `ui_text_scale requested=256 effective=85` at 2560x1440;
- `gameplay_ui_scale_install` has nonzero patched call counts;
- sampled `gameplay_ui_scale density=0.250000,0.333333`.

---

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

## FULL-ENGINE 20-FPS GROUND-TRUTH DIAGNOSTIC FRONTIER

The paired scripted-player + active-camera 20-Hz experiment still failed. The next diagnostic changes the modern timer dispatcher itself from 60-Hz retail callback delivery to 20-Hz delivery (~50 ms). Retail `TimerCallback` then naturally advances ~3 canonical vblanks per callback, so the entire engine observes the authored 20-FPS update quantum.

Purpose: capture one instrumented WORKING Chase run from the same executable/logging stack. Then diff its player position, camera heading, synth workers, collision state, level-script state and trigger timing against the failing 60-Hz trace to find the first true divergence.

This build is deliberately temporary and should not be mistaken for the final native-60 policy.

Installed reference build identity:
- behavior commit `85ae7108eaaf19b826a1fe924904c89cd62ed380`
- proxy `3543F54E9E79CF70603AE5ED2397A243ED1F383B309AD07A42D56E85B2EE55E5`
- renderer `A1F6E5C21892DAA82555C8620B2FA9FFE822B7F917E7197289BB927688DFD458`
- input `44680FE5EBE1CD63CDDD14B3423745A4DBE6707BE2506EDC37E5C27671D31F50`

Use `TEST_LATEST_BUILD.bat` for the reference run. The whole game running around 20 FPS is expected and intentional.


## 2026-10-06 Chase good20 -> BaddyList frontier

See `docs/NEW_CHAT_HANDOFF_2026-10-06_CHASE_GOOD20_BADDYLIST_FRONTIER.md` for the complete current frontier.

Key state:
- working ground-truth: full-engine 20-FPS run `logs/20261006-020549/spidey-decomp.log`;
- failing comparison: `logs/20261006-012109/spidey-decomp.log`;
- failure proven to be camera/actor phase during the same code-9 scripted worker;
- Venom confirmed on `BaddyList @ 0x0056E990`;
- current candidate restores global 60 Hz and cadence-gates BaddyList only during L5A1 synthesized player control;
- forced-clean matching build passed before checkpoint;
- use Local Commander for local PC/repo interaction going forward.


## 2026-10-06 02:34 — WORKING 20 FPS REFERENCE DIFF FRONTIER

Successful reference:
- `logs/20261006-020549/spidey-decomp.log`
- full-engine 20 FPS
- user confirmed Spider-Man actually chases Venom through the building.

Failing comparison:
- `logs/20261006-012109/spidey-decomp.log`
- targeted player+camera 20-Hz compatibility inside a 60-Hz world
- still fails.

Critical final-worker difference:
- both execute type-3/code-9 with `axes=0,127`;
- working camera heading stays ~1029;
- failing 60-Hz camera starts ~2080 and rotates onward;
- camera-relative forward therefore maps to the wrong world direction and Spider-Man hits `Inside01`.

Durable RE document:
`docs/CHASE_VENOM_20VS60_REFERENCE_DIFF_2026-10-06.md`

Tracked comparison tooling/results:
`tools/research/compare_good20_fail60.py`
`tools/research/compare_good20_fail60.txt`
`tools/research/compare_final_building_window.py`
`tools/research/compare_final_building_window.txt`

Current source includes WIP/UNTESTED BaddyList 20-Hz cadence plus camera-shot telemetry and restores the global timer to 60 Hz. Next runtime should test that WIP before further changes.

## 2026-10-06 — node76 camera-controller frontier

The earlier BaddyList-only candidate was incomplete. Node 76 is now proven to be a type-203 `CScriptOnlyBaddy` on **ControlBaddyList @ 0x0056E994**, and it links to node 74's 128-frame fixed-camera transition. Venom is separately on BaddyList @ `0x0056E990`.

Retail Logic updates the two lists back-to-back before pending trigger commands. Current source therefore cadence-gates both lists with a **single shared phase**, so Venom and the node-76 camera controller advance together every third canonical tick while the rest of the engine remains at native 60 Hz.

Forced-clean matching build: PASS. Not runtime-tested yet.


## 2026-10-06 03:00 — FIRST KNOWN-GOOD 60-FPS CHASE BUILD

The phase-locked world-list candidate at behavior commit `3ec28e4b2b7abb51f6166256df750690721fa475` is runtime-proven **WORKING**.

Successful archive:
`logs/20261006-030007/spidey-decomp.log`

The game timer is still 60 Hz. The fix keeps the authored Chase subsystems in phase:
- player/synth authored cadence;
- active scripted camera cadence;
- Venom's `BaddyList`;
- script-only camera controller `ControlBaddyList`.

Critical final code-9 proof:
- camera heading = 1029 throughout;
- desired world heading = 2053;
- Spider-Man traverses the building on +Z and exits naturally.

Do not regress or remove `3ec28e4b` behavior without preserving this known-good checkpoint first.


### Chase fix promoted to production policy

Direct good20-vs-good60 comparison confirms the final code-9 worker is phase-equivalent:
- camera heading 1029 in both;
- desired world heading 2053 in both;
- identical Z traversal;
- max aligned camera/world-heading difference = 0.

Keep behavior commit `3ec28e4b` / tag `chase60-known-good-20261006` as the authoritative known-good Chase 60-FPS implementation.


## Venom Chase bar UI frontier — 2026-10-06

The Chase gameplay fix remains known-good and untouched.

New L5A1 HUD finding:
- `Venom_DisplayProgressBar @ 0x004E7E10` is the top Chase meter;
- it is one composite made from many independent panel quads;
- our generic high-resolution HUD scaler was assigning different left/center/right anchors to adjacent meter pieces, causing the visible fragmentation.

Candidate now forces all five progress-bar coordinate calls to one shared top-center anchor in level `0x501` only.

See:
`docs/VENOM_CHASE_BAR_RE_2026-10-06.md`

Next test: run `TEST_LATEST_BUILD.bat`, enter Venom Chase, inspect the top chase bar, then exit normally for log inspection.


## Mysterio frontier — 2026-10-06

User requires:
1. boss health fill aligned with holder;
2. retail/default game camera only during Mysterio;
3. verify suspected over-frequent lasers with a 20-FPS ground-truth comparison.

60-FPS source candidate now contains the UI + camera fixes and telemetry-only wrapper at `0x0045F489 -> FireBoobies`.

See `docs/MYSTERIO_60FPS_FRONTIER_2026-10-06.md`.

Do not change Venom Chase phase-lock behavior while working on Mysterio.


### Mysterio 20-FPS reference now prepared

60-FPS restore point: `0b3586fb`.

Current diagnostic changes only timer delivery to full-engine 20 FPS (20 Hz callback / ~3 canonical ticks). Mysterio UI fix, retail-camera guard, and FireBoobies telemetry remain identical.

After one Mysterio fight run, compare attack starts/tick spacing and restore timer to 60 before making the production laser fix.


### Mysterio reference compile-order fix

The first `f807ad2f` test build failed because `SpideyMysterioFireBoobiesTelemetry` called `SpideyIsMysterioBossActive` before the helper was declared. Fixed with a forward declaration only. No behavior changes.


## Mysterio laser production candidate — native 60 FPS

The full-engine 20-FPS reference `logs/20261006-040238/spidey-decomp.log` produced visibly different/correct laser behavior.

Current source restores global timer to 60 Hz and cadence-gates only `CMysterio::AI` via vtable slot `0x0053BABC` to one retail call per three canonical ticks while boss type 311 is active. This directly reduces state-6 FireBoobies/beam-refresh service cadence to the authored rate without lowering rendering or the rest of the world.

Health bar remains unresolved despite the first Mysterio common-boss-fill scaling candidate. Do not mark it fixed.

Next test: Mysterio fight at 60 FPS, compare lasers to 20-FPS reference, then inspect `mysterio_ai_20hz_stats` and corrected `mysterio_laser_attack` timing.


## Mysterio crash recovery / narrow laser frontier

Retire whole-`CMysterio::AI` 20-Hz experiment. Crash run:
`logs/20261006-041611/spidey-decomp.log`

Current candidate:
- global timer 60 Hz;
- CMysterio AI 60 Hz;
- only `FireBoobies` call at `0x0045F489` cadence-gated to 3 canonical ticks;
- retail Mysterio camera policy unchanged;
- Venom Chase fix untouched.

New CSoftSpot telemetry:
- retail CSoftSpot::Hit requires `SHitInfo.field_0 & 0x04`;
- telemetry-only wrapper at vtable slot `0x0053BB94` records hit flags, damage, part, HP and player web mode, then calls retail unchanged.

Next run should test Mysterio long enough for several laser attacks and deliberately try the ordinary web attack on one soft spot. Exit normally if stable.


## Mysterio laser current safe frontier — SetPos-only cadence

Do NOT restore whole-Mysterio-AI or whole-FireBoobies throttling; both caused runtime instability.

Current candidate keeps all boss/state logic at 60 Hz and samples only the two `CMysterioLaser::SetPos` calls at the authored 3-canonical-tick boundary:
- 0x0045D3AB
- 0x0045D44E

Existing laser liveness compatibility remains active.

Soft-spot telemetry is pass-through only and now reads player web mode from raw retail offset +0x8F8.

Forced-clean matching VC6 build + link PASS. Next action is one Mysterio runtime test.


## QuadBit world-effect anchoring frontier

Mysterio helmet ring is CQuadBit. Retail DisplayQuadBitList uses both GTE camera rotation and a separate DCX 4x4 transform.

Old fix restored only GTE rotation. New candidate also rebuilds exact retail DCX camera/projection matrix:
`matrix4x4_ml(dst, 0x0056E778, 0x0056E570)` -> `0x0056E6F8`
before untouched DisplayQuadBitList.

Forced-clean VC6 build/link PASS.

CVenomWrap is CNonRenderedBit, so its tentacle/wrap drift is a separate renderer path.
