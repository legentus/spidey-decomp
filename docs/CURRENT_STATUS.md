# CURRENT STATUS

## LOCAL-AUTHORITATIVE CHECKOUT WORKFLOW (2026-10-05)

The active authoritative development copy is now the real Git checkout at:
- `F:\Spider-Man 2000 Recomp\project main`
- branch: `dev`
- remote `origin` is retained for backup/fallback, but normal development does **not** refresh from GitHub before building.

Workflow rules:
- ChatGPT edits, documents, and checkpoints the local checkout directly.
- `TEST_LATEST_BUILD.bat` derives `LOCAL_DEV_REVISION.txt` from the local Git `HEAD`, skips the GitHub updater, then runs the existing build/install/launch pipeline.
- `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` is now a local-only convenience wrapper around the same test path.
- `UPDATE_SPIDEY_PROJECT.bat` is intentionally disabled in local-authoritative mode so an archive mirror cannot overwrite unpushed local work.
- The original `tools\UPDATE_SPIDEY_PROJECT.ps1` remains untouched for fallback/recovery.
- The pre-conversion tested source/doc versions are preserved outside the repo at `F:\Spider-Man 2000 Recomp\project main pre-local-git-backup`.
- Returning to the former GitHub-authoritative workflow is reversible from Git history plus the retained updater; do not discard local commits before switching modes.

At conversion time, the real checkout was reconciled to `dev` HEAD `3686276c7c868685791fd48813732868b2b04082`, with latest tested runtime still `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`. The Chase camera-relative/world-heading steering source remains implemented but runtime-untested.

### Local conversion/build checkpoint

Local-only workflow commits:
- `de858d4eadfa4439dce290860d35d3f2dc6313f9` — make the real local checkout authoritative and replace the normal BAT entry points with updater-bypassing local wrappers.
- `12e7e750445a34337d9db50796a2e9684f20aee5` — add prepare/build/install-without-launch support.
- `34c6f816a876e086b3e60cf99ee731bb709baf8f` — make the existing test pipeline robust when invoked through Remote Desktop Commander, where `ComSpec` may be unset.

A forced-clean prepare/install completed successfully from `34c6f816...`:
- matching VC6 proxy: PASS;
- renderer11 rebuild: PASS;
- input11 rebuild + 32-bit preflight: PASS;
- proxy installed into the real game folder: PASS;
- renderer/input bridges installed: PASS;
- game intentionally **not launched**.

Installed untested runtime identity:
- revision: `34c6f816a876e086b3e60cf99ee731bb709baf8f`;
- proxy SHA-256: `6225AFAE8F5C5BC72DBE2320F0DC02848150B2CFADB66BDB3F1ED30EAA2AEAD0`;
- renderer11 SHA-256: `50237510B0F1B3F45B1A18FCF563CEFF951AAAE44A767662F93CE756C734CBE6`;
- input11 SHA-256: `12DD9983D27BB20426D632A558BB93C1BEA3A0621F18AE959E4D8E210A884DB3`;
- consolidated log path: `C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`.

### Direct-launch Chase observation (2026-10-05)

The user manually launched the already-installed `34c6f816...` candidate by starting `SpideyPC.exe` directly. This bypassed the test-session harness, so the consolidated log remained at its 9-line seeded header and no per-run repo archive was created.

Gameplay observation is still valid evidence:
- Chase behavior remains **non-vanilla**;
- overall route behavior was effectively the same;
- the user could jump back out from the side of the building where Spider-Man entered;
- based on retail memory/expected Venom route, Spider-Man should emerge on the other side of the building;
- this suggests the current compatibility layer is still preserving the wrong route/path sequence rather than reproducing retail Chase traversal.

This direct-launch observation was later confirmed by the proper instrumented harness run below. Preserve it only as the earlier observational test; the authoritative diagnostic evidence is now `logs/20261005-211946/spidey-decomp.log`.

### Instrumented Chase run 20261005-211946 — route still wrong; type-3 held-latch bug identified

The proper `TEST_LATEST_BUILD.bat` run reproduced the same non-vanilla behavior: Spider-Man still does not chase Venom through the building and does not emerge from the far side.

Authoritative tested runtime behavior is therefore **TESTED — STILL WRONG** for the installed `34c6f816...` candidate.

High-signal evidence from the completed trace:
- `synth_calls=2271`, `retail_updates=759`, `held_calls=1512`, `trace_samples=759`;
- scheduler Logic remained essentially one canonical tick per call, so the global native-60 scheduler is not the remaining cause;
- `input_basis_e34=0` for all 759 synth samples and the startup SpideyAI0 scan found zero `E34` references in the scanned range;
- only two type-2 X/Z route targets appeared, and both reached the retail completion radius and removed themselves;
- the building sequence contains a long type-3 worker with `code=10`, initial countdown `490` canonical ticks, and sampled axes `E2D=-127,E2E=0`.

Direct retail disassembly of `CPlayer::SynthesizeAnalogueInput @ 0x004BC300`, type-3 body `0x004BCC2D`, proved that code 10 does **two** per-synth-call side effects:
- `player[0x260] = 1`;
- `E2D = -127`.

The prior 20-Hz sample/hold wrapper preserved the analogue axes on the two held native-60 frames but did **not** reassert the type-3 latch bytes. The rest of `CPlayer` still runs at 60 Hz, so the scripted action latch could disappear on two out of every three frames even while the held analogue direction remained present.

New behavior candidate:
- commit `ee6022150a0911d5503495a8f1d04ded55097bc7` — `timing: preserve Chase type3 held latches`;
- reassert all active type-3 latch side effects on held synth frames without advancing worker timers;
- preserve the already-sampled final `E2D/E2E` axes so worker-order precedence is unchanged;
- add `type3_latch_calls`, `type3_latch_writes`, `type3_latch_dynamic`, and `type3_latch_directional` telemetry;
- forced-clean VC6 matching build: **PASS**;
- prepare/install pipeline: **PASS**, game intentionally not launched.

Installed untested candidate identity:
- behavior revision: `ee6022150a0911d5503495a8f1d04ded55097bc7`;
- proxy SHA-256: `F6F493370BE87F73F1FCBC9C920ADAB8A3EA078DFEFB3A323123616F350EE072`;
- renderer11 SHA-256: `4083E26D3086FDBB1F95ADBEF9DBF621DD250B1D4BB41034BA5388C0E85924FD`;
- input11 SHA-256: `E6225CD5C67DDF3621B5B92554D4929D4D52697E777331E58E766657B398F256`.

Detailed RE and opcode map: `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`.

### 2026-10-05 22:54 instrumented result — type-3 latch fix active, route still wrong; one-off ground loss captured

Authoritative session:
- runtime revision: `b77c529723c070933c13d67b0d49c02d48b1c8cb` (documentation descendant of behavior commit `ee602215...`);
- preserved log: `logs/20261005-225403/spidey-decomp.log`;
- live log size: 6,813,018 bytes;
- user result: first level start fell into the yellow haze immediately after the in-engine cutscene; subsequent restarts behaved normally, but the original Chase building route remained wrong.

Type-3 held-latch correction definitely executed:
- `synth_calls=2739`;
- `retail_updates=916`;
- `held_calls=1823`;
- `type3_latch_calls=604`;
- `type3_latch_writes=732`;
- `type3_latch_dynamic=174`;
- `type3_latch_directional=558`;
- `ramp_unexpected=0`.

The correction materially changed the long type-3 path but did not restore retail:
- prior long code-10 segment ended near `(74195197, 53248, 15734966)`;
- new long code-10 segment ended near `(75041216, 53248, 15735343)`;
- therefore held type-3 latch preservation is real behavior, but is not sufficient by itself.

#### Yellow-haze fall: camera ruled out as first cause

Three starts from the same scripted spawn were present in the trace.

Bad first attempt vs the first normal restart:
- samples 0..16 are byte-for-byte equivalent across the traced control/camera fields and position;
- at the first divergence (relative sample 17):
  - camera heading remains exactly `168` in both;
  - synthesized axes remain `124,29` in both;
  - desired relative/world headings remain `1897/2065` in both;
  - route worker remains the same type-2 target;
  - **player state diverges from `0x400` to `0x4` on the bad run**;
  - the body trajectory then falls away and reaches Y ~21 million before the level resets.

Retail/decomp source confirms `CPlayer::CheckGroundGone()` sets `field_E1C = 4` when `mCollision & 2` (ground collision) is absent after the short `field_EA4` grace period.

Conclusion:
- the one-off yellow-haze death was a **grounding/collision loss**, not a camera steering failure;
- modern camera heading had not diverged when the fall state was entered;
- because it happened only on the first load, treat first-load collision/readiness or another transition-state race as the likely class until reproduced.

The next trace now records:
- `collision=0x...`;
- `ground_grace=...`;
so any recurrence can be attributed exactly.

#### 3D camera is now a serious suspect for the remaining Chase route

Separate from the fall, runtime camera telemetry proves modern mode-3 ownership was acquired during Chase from tiny mouse deltas (examples include `-2,-1`, `1,0`, etc.).

This matters because retail type-3 synth codes 8..11 are **literal camera-relative analogue directions**:
- code 10 = hold left (`E2D=-127`);
- code 9 = hold forward/back axis counterpart, etc.

The existing world-heading hold only preserves the sampled world direction between 20-Hz synth updates. If the **sample itself** was generated against our modern free-look camera rather than the authored retail camera, the stable world direction can still be the wrong one.

New generic ownership rule in the working candidate:
- while active player `field_1AC != 0` (synthesized/scripted control), modern mode-3 camera releases with reason `scripted_player_input`;
- retail `CM_Normal` owns the camera completely for that call;
- ordinary manual gameplay regains modern camera normally when scripted control ends;
- this is not Chase-specific and matches the documented camera-ownership architecture.

Forced-clean VC6 build with this change: **PASS**.

Installed untested scripted-camera-yield candidate:
- behavior commit: `bbacc44edc799d896e6223832b5e9b5e0c56f500`;
- proxy SHA-256: `145429C1340A604AD4DFB0EFAB6C9B6E111E7E6E9EB9AB9BDDF9B13B794BFF8B`;
- renderer11 SHA-256: `3F674EEE169CEC7CEB3536C97CA12AAC133764EA0FF2A618FE0AABB7E1291C07`;
- input11 SHA-256: `84419F651993C23ADC265E44C5E0C12769AA988CD5A480503F3B7DA6C3E5529D`;
- modern-input preflight: **PASS**;
- prepare/install pipeline: **PASS**;
- game was intentionally not launched.

Git note:
- local commit is authoritative and safe;
- the immediate `git push origin dev` attempt failed with remote access/authentication, so do **not** assume GitHub contains `bbacc44e...` yet.

### 2026-10-05 23:09 instrumented result — camera yield did not fix route; black-wall trigger transition identified

Authoritative failing session:
- runtime revision: `b1a72d62b2ed2a2690e68478e9e441fff6318633`;
- behavior under test: `bbacc44edc799d896e6223832b5e9b5e0c56f500`;
- preserved log: `logs/20261005-230941/spidey-decomp.log`;
- result: **STILL WRONG** — scripted-camera ownership yield did not restore the building traversal;
- user observation: Spider-Man reaches the black wall inside the building but does not pass through it to continue after Venom.

#### Runtime collision proof

The new `collision` trace changes the diagnosis from steering-only to an actual environment transition failure.

During the long active type-3 `code=10` worker:
- repeated samples have `mCollision & 1`, proving the normal forward/world collision sweep is hitting geometry;
- repeated samples have `mCollision=0x3`, i.e. wall + ground;
- the player enters `field_E1C=0x80000`, exactly the state set by retail `CPlayer::CheckRunIntoWall()`;
- later samples enter `0x1000`/surface-transition behavior while the script continues to command code 10;
- representative blocked position is approximately `(74.6M, 0, 15.9M)` in fixed-point coordinates.

Retail `CPlayer::DoPhysics @ 0x00466CE0` has no generic `field_1AC` scripted noclip branch. Therefore the correct retail behavior is not “scripted movement ignores every wall.” The building environment state/trigger is supposed to change.

#### L5A1 retail level-data reconstruction

The packed retail archive was parsed and the exact L5A1 assets were extracted locally for RE:
- `L5A1_G.psx`;
- `L5A1_L.psx`;
- `L5A1_O.psx`;
- `L5A1_T.trg`;
- `l5a1.kat`.

The TRG file is valid `_TRG`, version 2/1, with 344 nodes.

High-signal embedded names include:
- `Inside`;
- `Outside`;
- `CutBldg_Win`;
- `TRGP_Wait`;
- `Chase Venom to his Hideout`;
- `You Lost Venom`.

Relevant decoded nodes:
- node 45: type 6, visibility command that turns `Inside` **off**;
- node 338: type 6, visibility command that turns `Inside` **on**;
- node 312: `Outside`;
- nodes 293/295/296: `CutBldg_Win`;
- **node 44**: type 6 physical command point, no incoming links, links to `21,48,53,196,292,300,338,305`.

Node 44:
- checksum: **`0x854B6E67`**;
- command list: `[134, 1, 3, FFFF]`;
- opcode 134 initializes the one-pulse requirement;
- opcode 3 pulses its linked nodes;
- therefore node 44 is the authored physical building-state transition that fans out to the `Inside` and window/building nodes.

The checksum appears exactly once in `L5A1_G.psx`.
Retail PSX trigger-table reconstruction proves:
- checksum table begins at file offset `0x3D76C`;
- **trigger-table index 2 = `0x854B6E67` = node 44**.

#### Retail trigger-collision path

Retail `SpideyAI0 @ 0x004B13F0`:
1. snapshots `mPos` into `CBody::field_E8` near `0x004B1AE2`;
2. after player movement, computes `mPos - field_E8`;
3. normalizes the movement direction;
4. creates a trigger-only swept line approximately from `previous_position - 16 units along movement direction` to `current_position`;
5. sets `SLineInfo::RecordTriggerZoneHits = 1`;
6. calls retail `M3dZone_LineToItem`.

Retail `M3dColij_LineToThisItem @ 0x004529C0`:
- when that sweep hits a trigger-marked face, it reads the face's trigger index;
- resolves the index through the PSX checksum table;
- calls `Trig_TriggerCommandPoint(checksum, true)`.

Thus the intended building sequence is:
**Spider-Man crosses trigger face index 2 -> checksum 0x854B6E67 -> node 44 -> pulse linked building-state nodes -> Inside transition.**

The current native-60 Chase reaches normal solid wall collision while the real node-44 command point is still unexecuted.

#### Implemented recovery candidate

A narrow compatibility recovery is now implemented in `main.cpp`.

It does **not**:
- noclip Spider-Man;
- remove arbitrary collision;
- teleport/reposition him;
- directly force `Inside01` visibility.

It only acts when all of these are simultaneously true:
- retail level ID is `0x501`;
- synthesized player control `field_1AC` is active;
- the active worker is type 3 / code 10;
- `mCollision & 1` proves Spider-Man is physically blocked;
- position is inside the observed building-entry cluster;
- the real retail `GetCommandPoint(44)` exists;
- its checksum is exactly `0x854B6E67`;
- node 44 is still unexecuted;
- the blocked state is observed on two consecutive samples.

Then it invokes:
`Trig_TriggerCommandPoint(0x854B6E67, true)`

That schedules the exact authored node-44 command list and its original linked `Inside`/window/building state fanout.

New telemetry:
- `building_entry_checks`;
- `building_entry_blocked_matches`;
- `building_entry_natural_seen`;
- `building_entry_recovery_attempts`;
- `building_entry_recovery_fires`;
- one-time `chase_building_entry_recovery` event with player state, collision bits, worker, blocking item checksum/region/model/flags, and face flags.

Forced-clean VC6 matching build of this source: **PASS**.

Installed untested node-44 recovery candidate:
- behavior commit: `335dd7e51eb1baba785a96da7d81782fac683c28`;
- proxy SHA-256: `DF2FBDDD7DA53DEFA8CB114D3FB2E26298185479093BE9E930ACC06C73E63A28`;
- renderer11 SHA-256: `4D78B3B931E9DD80142A4C342832008199EB127EDE9A33E9313E24353D69C057`;
- input11 SHA-256: `3C4A9974B8D02B15A938528CFD22BC97EB245A37C3B2522A1AA7BB4E7601862F`;
- full forced-clean proxy build: **PASS**;
- renderer11 rebuild: **PASS**;
- input11 rebuild + 32-bit ABI preflight: **PASS**;
- prepare/install into the real game folder: **PASS**;
- game intentionally not launched.

### Corrected recovery frontier — Wait05 -> Wait06

The direct-launch test of the prior Wait04/node-44 recovery still showed the same black-wall failure, but that direct launch produced only a 617-byte header log, so no recovery counters survived.

Deeper L5A1 reconstruction proved the Wait chain:
- Wait02 node 34 / `0x6C28CB52`;
- Wait03 node 39 / `0x1B2FFBC4`;
- Wait04 node 44 / `0x854B6E67`;
- Wait05 node 70 / `0xF24C5EF1`;
- Wait06 node 298 / `0x6B450F4B`.

Wait04 turns `Inside` on and enables Wait05. Wait05 launches the long scripted building sequence and enables Wait06. Therefore the active long type-3/code-10 worker at the black wall belongs to the **Wait05 -> Wait06** leg.

Also, `SCommandPoint::Executed` is per-frame and is cleared every Logic update. The previous recovery incorrectly treated it as persistent state.

Persistent stage evidence is instead:
`NumPulsesSet != 0 && NumPulses == 0`

because opcode 134 initializes the pulse count and opcode 3 performs the fanout then decrements it.

Corrected behavior candidate:
- require Wait05 persistent state = completed;
- require Wait06 persistent state = not completed;
- require scripted type-3/code-10 movement;
- require real forward collision in the captured building cluster for two consecutive samples;
- invoke the exact authored Wait06 command point `Trig_TriggerCommandPoint(0x6B450F4B,true)`;
- no noclip, teleport, or manual wall deletion.

Forced-clean matching VC6 build: **PASS**.

Installed untested corrected Wait05->Wait06 candidate:
- behavior commit: `9905039dddaa58981bd225c1b8cfdeebb119ed20`;
- proxy SHA-256: `532190CAEAEB3BA41EEFDFA1CAF7C7F96A7BE8D1C0CAB5105AE034D780F68D72`;
- renderer11 SHA-256: `56EC90E75E65069372A8D3EC81F7CF530EA7049F3CB6754A6949F074A8A7DFDA`;
- input11 SHA-256: `1460892CBD6E41783FB74D20F28123DB83C7CA2125FE1B421CBF9F9C62E06141`;
- prepare/install pipeline: **PASS**;
- modern-input 32-bit ABI preflight: **PASS**;
- game intentionally not launched.

### 2026-10-06 00:48 test — Wait06 recovery fired and still did not fix route

Proper harness log:
- runtime revision: `b1174ba0935aa512641d7e4fdafe42aa6ed6df25`;
- preserved log: `logs/20261006-004842/spidey-decomp.log`;
- result: **STILL WRONG**.

Critical proof:
- `chase_building_entry_recovery fired=1`;
- recovered stage: `wait05_to_wait06`;
- node: `298`;
- checksum: `0x6B450F4B`;
- Wait05 persistent state: completed (`NumPulsesSet=1, NumPulses=0`);
- Wait06 had not completed before recovery;
- recovery fired at approximately `(74563756, 53248, 15833957)`;
- blocking model checksum: **`0x1AD2FBED` = `Inside01`**;
- collision: `0x0003`;
- active head worker: type 3 / code 10.

Therefore the trigger-stage hypothesis is downstream, not causal: even forcing the exact authored Wait06 command point while blocked does not make Spider-Man traverse the building.

### Root cause pivot — reproduce 20-FPS player update order, not just elapsed time

Deeper retail RE identified the exact player-AI seam:
- `Ob_AI @ 0x00460FC0` calls `CBody::EveryFrame()`, then the object's virtual AI;
- CPlayer vtable `0x0053C464` routes virtual AI to `CPlayer::AI @ 0x004C65C0`;
- `CPlayer::AI` performs ordinary per-frame housekeeping, then loads the callback from `player+0x554` at `0x004C684F`;
- `CPlayer_CPlayer` writes retail `SpideyAI0 @ 0x004B13F0` to `player+0x554` using the immediate at `0x004BA2B5`.

Turn-controller RE also proves why three 60-Hz updates are not generally equivalent to one 20-Hz update:
- `SetTargetTorsoAngle` computes a target step and remaining-call budget in `field_DF4/field_DF8`;
- retail `SpideyAI0` later integrates with `angle += field_DF4 * field_80` and `field_DF8 -= field_80`;
- however `CheckForwards` can reevaluate/retarget the turn controller on subsequent AI passes;
- movement, friction, collision, surface-state and trigger feedback are likewise re-entered between the native-60 half-steps.

So preserving total elapsed ticks is insufficient: update **order and feedback cadence** differ.

### Implemented authored-cadence player-AI candidate

New policy:
- ordinary CPlayer housekeeping remains 60 Hz;
- CSuper animation remains 60 Hz;
- renderer remains 60 Hz;
- camera/global engine and every other object remain on the normal native-60 path;
- only the default Spider-Man callback `SpideyAI0` is cadence-gated while:
  - level is `0x501`; and
  - synthesized control `field_1AC` is active.

During that state:
- canonical 60-Hz ticks are accumulated;
- two intermediate callback invocations are held;
- on the third tick, retail `SpideyAI0` runs once with `player->field_80 = 3`;
- the original `field_80` is restored afterward.

This makes turning, movement integration, friction, collision, trigger sweeps and scripted-player state transitions see the same single three-tick quantum as the known-good 20-FPS case.

The existing synth/ramp wrappers naturally collapse to their retail 20-Hz behavior under this owner:
- synth sees one call every three ticks with `field_80=3`;
- the raw analogue ramp correction becomes one `0x20` increment per 50 ms;
- there are no held player-AI frames inside SpideyAI0 itself.

The downstream Wait05->Wait06 forced recovery is **disabled** for this candidate so the runtime tests whether the natural authored path crosses the original trigger faces.

New telemetry:
- `chase_player_ai_20hz_install`;
- `chase_player_ai_20hz_stats` with total wrapper calls, actual retail calls, held calls, and max elapsed ticks.

Forced-clean matching VC6 build of this source: **PASS**.

Installed untested authored-cadence player-AI candidate:
- behavior commit: `795367ce3ddabb05c2aaee843f838e54750f75cf`;
- proxy SHA-256: `05810A992991ACF4B47FF0FC6C4AE536251C53F0B17BCA9B02888F8ABAB35F4B`;
- renderer11 SHA-256: `818BB56194A6DE5A8E3C92BFF661B244412BFAC2D68AA023EFB51539C11446CB`;
- input11 SHA-256: `A512066ABEA6F2A4523ED78CC2ACCA48E5D888853E514B54296B36513FDC738E`;
- prepare/install pipeline: **PASS**;
- modern-input 32-bit ABI preflight: **PASS**;
- game intentionally not launched.

### 2026-10-06 01:09 test — player-AI 20 Hz alone still fails

Proper harness archive:
- `logs/20261006-010753/spidey-decomp.log`;
- runtime revision: `f4841e241602bcdf294fe01d333aea3bb055dbe0`;
- result: **STILL WRONG** — Spider-Man does not physically follow Venom through the building.

The player-AI cadence hook definitely worked:
- `chase_player_ai_20hz_install installed=1`;
- total wrapper calls: `4851`;
- actual retail `SpideyAI0` calls while gated: `775`;
- held callback opportunities: `1549`;
- maximum elapsed passed to retail: `3`;
- synth retail updates: `775`;
- synth held calls inside `SpideyAI0`: **0**;
- all recorded Chase synth samples use `field_80=3`.

Therefore throttling only `SpideyAI0` to the real 20-FPS quantum is not sufficient.

Important user observation now corroborated by telemetry:
- after the visible cutscene ends, Spider-Man continues moving under script control for roughly 3–5 seconds and manual input is unavailable;
- this is not a separate bug: `field_1AC` / synthesized control and the remaining worker chain stay active after the visible camera/cutscene presentation ends;
- the script continues consuming type-3 workers until the final type-3/code-10 worker reaches zero;
- only then does the trace change to `synth=0` and manual control returns.

So the extra autonomous movement is the still-running Chase program, and the player is spending that period trying to finish the scripted route while physically displaced/stuck.

### Remaining FPS dependency — active camera solve cadence

Type-3 direction semantics remain critical:
- code 10 writes `E2D=-127`;
- this is a literal camera-relative direction;
- `CheckForwards` converts that through the current transform-derived `CCamera+0x23A`;
- the latest building approach shows the code-10 world heading tracking the active camera heading while Spider-Man approaches/collides with the building.

Retail camera RE:
- active camera vtable base: `0x0053B4BC`;
- virtual AI slot `+8`: `0x0053B4C4`;
- retail target: `CCamera_AI @ 0x00417CB0`;
- `CCamera_AI` owns heading interpolation and calls `CM_Normal @ 0x00418E00`;
- the simple `field_236` interpolation is elapsed-tick aware, but `CM_Normal` performs nonlinear camera position/focus/collision solving every camera AI invocation.

Thus one 20-FPS camera solve with `field_80=3` is not guaranteed equivalent to three 60-Hz camera solves with `field_80=1`. Because type-3 code 10 uses the resulting camera transform as the player's world direction, this can directly change the building-entry angle.

Implemented next candidate:
- keep scripted `SpideyAI0` at the authored 20-Hz cadence;
- additionally cadence-gate **only the active retail camera AI** during L5A1 synthesized player control;
- camera wrapper accumulates canonical ticks and calls retail `CCamera_AI` once every three ticks with `camera->field_80=3`;
- inactive cameras and all ordinary gameplay remain unchanged;
- rendering/presentation and all other bodies remain 60 Hz;
- forced Wait trigger recovery remains disabled.

New telemetry:
- `chase_camera_ai_20hz_install`;
- `chase_camera_ai_20hz_stats`.

Forced-clean matching VC6 build of the paired player+camera source: **PASS**.

Installed untested paired player+camera authored-cadence candidate:
- behavior commit: `bf3bc6430ea86ddd0da077d70c9f66abb44bac1c`;
- proxy SHA-256: `8C0A262F709BC1A3B6A66DD188DCF6AFC0C17EA32A69BF6FEA3664FEC663F9DE`;
- renderer11 SHA-256: `52C32CE38F78A2161C9A9CF1361DD25EA58D2A701629D16763699B28F6E1F18A`;
- input11 SHA-256: `B566BCE7B07FB9727121766A781ABE106A71BBE7588A0FB4B7DDCD4E24D51BE2`;
- prepare/install pipeline: **PASS**;
- modern-input 32-bit ABI preflight: **PASS**;
- game intentionally not launched.

Next runtime action:
1. run `TEST_LATEST_BUILD.bat`;
2. replay Chase Venom;
3. watch specifically whether the code-10 approach angle/path through the building changes;
4. note whether the 3–5 second post-cutscene autonomous movement changes;
5. exit normally so player/camera cadence stats and the route trace can be compared.

## NEW-CHAT HANDOFF CHECKPOINT (2026-10-05)

Dedicated recovery document:
- `docs/NEW_CHAT_HANDOFF_2026-10-05_CHASE_VENOM_CAMERA_STEERING_FRONTIER.md`
- created at commit `79cabf84f85c7cb6bf1790ddde7aa7c1b0e33fdc`.

This checkpoint preserves the current generalized synthesized-steering behavior source `0ab2efb34...`, the E32/E34/camera-heading ownership probe, worker timing classification, last tested `df0b1d62...` baseline, and the exact next Chase Venom runtime test.


## CHASE VENOM — SYNTH WORKER TIMING CLASSIFIED; GENERALIZED WORLD-HEADING HOLD READY (2026-10-05)

Latest tested runtime is still `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`:
- cutscene itself is confirmed 60 FPS;
- Spider-Man still misses the intended building-entry route;
- the supplied `spidey-decomp(20261005-233411).log` is the same runtime revision, not a test of the new Chase compatibility code.

### Additional assembly -> C result

Retail `CPlayer::SynthesizeAnalogueInput @ 0x004BC300` worker jump table is now classified far enough for timing:

- type 2: X/Z route steering, camera-relative axes;
- type 3: timed synthesized action/direction;
- type 5: spatial/target-vector worker;
- type 6: timed animation/action;
- type 7: timed player state;
- type 8: timed `field_E00`;
- type 9: timed wait/parser resume;
- type 15: conditional state/animation;
- 4 and 10..14 route to the invalid/unhandled body.

Every timed worker inspected (3, 6, 7, 8, 9) subtracts `player->field_80`, so their durations are already canonical elapsed-time based. No new raw one-call timer family was found.

This further isolates the Chase bug to **controller feedback cadence / heading-basis update order**, not worker countdown speed.

Detailed reconstruction:
- `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`.

### Correction to world-heading compatibility scope

The prior camera compensation assumed the active route worker would be the linked-list head. That is not guaranteed because multiple synthesized workers can coexist.

Current behavior source:
- `0ab2efb34c841814b2313aa74301e5eb3789a7ad` — `timing: preserve all synthesized Chase steering samples`.

It now:
- captures/preserves the effective sampled world heading for **any active synthesized analogue movement** during Chase;
- no longer depends on worker type 2 being the list head;
- keeps render/camera/physics/collision/animation at 60 Hz;
- keeps the synthesized controller at the narrow 20-Hz-equivalent sample cadence;
- records `worker_mask_before` / `worker_mask_after` so the next log identifies every concurrent worker type at the doorway.

The startup-only `E32/E34/camera+0x23A` ownership scan from `e1c8a369...` remains installed.

Documentation source:
- `2f6e0dcbb8dec436350b523bd6de59e87be25977`.

### Next runtime validation

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, then Level Select -> Chase Venom.

Observe:
1. cutscene remains 60 FPS;
2. whether Spider-Man now takes the doorway/building-entry turn correctly;
3. if not, whether the miss is improved, unchanged, or worse;
4. continue long enough to establish whether later chase movement remains valid;
5. exit cleanly and return the single consolidated `spidey-decomp.log`.

The next log is intentionally high-value: it should reveal synthesized cadence, world-heading compensation/drift, the full active worker mix, and all E32/E34/+0x23A static xrefs in one run.


## CHASE VENOM — CAMERA-RELATIVE STEERING PIPELINE RECONSTRUCTED; WORLD-HEADING HOLD READY (2026-10-05)

Latest tested runtime remains:
- revision `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`;
- in-engine Chase cutscene is confirmed 60 FPS;
- route failure remains: Spider-Man takes the wrong course at the building-entry turn and runs into the wall instead of following Venom.

The newly supplied `spidey-decomp(20261005-233411).log` is not a newer behavior test:
- its internal session/runtime revision is also `df0b1d62...`;
- therefore it predates all `chase_synth_20hz_*` code and cannot validate the new compatibility path.

### New retail x86 -> C reconstruction

Detailed document:
- `docs/CHASE_VENOM_INPUT_PIPELINE_RE.md`

Newly proven retail control chain:

1. `CPlayer::SynthesizeAnalogueInput @ 0x004BC300` already advances its main script clock with `field_80`.
2. Chase route worker type 2 reads target X/Z from its worker block.
3. It computes direction from current Spider-Man position to that target.
4. Critically, it converts that direction into **camera-relative analogue axes** using `CCamera+0x23A`.
5. `CPlayer::ReadAnalogueInput @ 0x004BD510` converts those axes into `field_E32` using another heading basis, `field_E34`.
6. `CPlayer::CheckForwards @ 0x004BF8A0` then adds the **current** `CCamera+0x23A` to `field_E32` for ordinary ground locomotion before calling `SetTargetTorsoAngle`.

`CCamera+0x23A` is not just the requested camera angle. `CCamera::LoadIntoMikeCamera` derives it from the actual camera transform matrix.

### Important correction to the first sample/hold design

The earlier untested source `6c40ef8...` held type-2 output axes on the two intervening 60-Hz frames.

That is not a faithful 20-Hz emulation because those axes are camera-relative while the camera itself keeps updating at 60 Hz. The same held stick vector can therefore map to a different world direction before the next scripted steering sample.

New source:
- `f2f46b7ff37332cbcb2179ac5f8d0b2991dc9a0e` — `timing: preserve Chase world heading across held steering frames`.

Behavior:
- synth producer still samples at a 20-Hz equivalent only for level `0x501` + synthesized input;
- render/camera/physics/collision/animation remain 60 Hz;
- on a fresh type-2 sample, the wrapper captures the effective world desired heading after retail `ReadAnalogueInput`;
- on intervening held frames, it rewrites `field_E32` against the current `camera+0x23A`;
- `CheckForwards` therefore sees the same sampled world heading even while the camera transform moves.

New stats:
- `heading_samples`;
- `heading_corrections`;
- `heading_max_pre_correction_drift`.

New trace fields:
- camera transform heading;
- `field_E34` input basis;
- `field_E32` relative desired heading;
- reconstructed desired world heading.

### One remaining steering-basis unknown

The exact owner/update order for `field_E34` is not yet proven.

New source:
- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546` — `timing: trace Chase steering-basis ownership`.

It adds startup-only displacement scans across:
- `SpideyAI0 @ 0x004B13F0 + 0x73A0`

for:
- `+0xE32`;
- `+0xE34`;
- `+0x23A`.

Expected labels:
- `SpideyAI0_E32`;
- `SpideyAI0_E34`;
- `SpideyAI0_CameraHeading23A`.

No per-frame file logging was added.

### Current authoritative source frontier

Behavior source:
- `e1c8a369a80b8dfd14ed79655c00110ac1b2c546`.

Documentation descendant:
- `a29986c32249e259a9327ee2c418f6e19345bcd9` — detailed scripted-input RE document.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Test only Chase Venom through the known failure point:
1. confirm the real-time cutscene is still 60 FPS;
2. watch the building-entry route;
3. report fixed / improved / unchanged / worse;
4. continue far enough to establish whether the chase stays playable;
5. exit cleanly so the in-memory trace is dumped;
6. return the single consolidated log.

The next log should simultaneously validate:
- 20-Hz-equivalent script sampling;
- camera-compensated world-heading hold;
- magnitude of the pre-correction held-axis heading drift;
- `field_E34` ownership/update sites.

Do not reintroduce a global 20/30-FPS cap or the historical CBody minimum-two-tick limiter.


## CHASE VENOM — CUTSCENE CONFIRMED 60 FPS; SCRIPTED STEERING SAMPLE/HOLD FIX READY (2026-10-05)

Latest tested runtime:
- revision `df0b1d62b8c1987c7a14dfa7e0f190ecbbb46306`;
- consolidated log: `spidey-decomp(20261005-232014).log`;
- entered through Level Select as in prior tests;
- user confirms the in-engine cutscene now remains at 60 FPS;
- Chase Venom route failure is unchanged: Spider-Man still fails to follow Venom through/into the building and takes the wrong course into the wall.

### Scheduler result

The current run proves the Chase level is overwhelmingly executing at one canonical 60-Hz tick per Logic/Present call:

`chase_scheduler_stats logic_calls=5254 logic_delta0=0 logic_delta1=5242 logic_delta2=0 logic_delta3plus=11 logic_updater_active=125 present_calls=5288 present_delta0=0 present_delta1=5242 present_delta2=30 present_delta3plus=15 present_updater_active=125 present_delta2_max_run=30 retail_level=0x501 ...`

Interpretation:
- the current broken route is **not** caused by the cutscene still being globally capped to 30 FPS;
- the prior observed ~30-FPS cutscene occurred while the experimental level-wide `CBody::EveryFrame` minimum-two-tick gate was installed;
- that body gate was removed in `117dae2359cbdf6c3db0dccf20c0b387b3f79c98`;
- after removing it, the cutscene returned to 60 FPS while the route bug remained;
- therefore do not add another global/cutscene FPS limiter.

User requirement remains:
- real-time/in-engine cutscenes must stay at 60 FPS, consistent with gameplay.

### New root hypothesis

The remaining high-value mismatch is the **feedback cadence of the synthesized steering producer**, not global simulation cadence.

Retail path:
- `CPlayer::ReadAnalogueInput @ 0x004BD510` clears the live analogue axes each update;
- when `field_1AC != 0`, it calls `CPlayer::SynthesizeAnalogueInput @ 0x004BC300` at call site `0x004BD572`;
- the Chase route's type-2 synthesized worker computes analogue steering from Spider-Man's current position to a target every time synth runs;
- at native 60 Hz that steering feedback is recomputed three times as often as a 20-Hz-authored sequence;
- even when elapsed timers are correct, a higher-frequency closed-loop steering controller can follow a materially different trajectory around collision geometry.

This fits the observed failure better than the earlier timer hypotheses:
- target/script timing can be correct;
- player physics can be real-time correct;
- but recalculating steering every 16.7 ms instead of every 50 ms can curve Spider-Man differently enough to hit the wall/opening boundary.

### New implementation — 60-Hz cutscene + 20-Hz scripted steering producer

Source commit:
- `6c40ef8ff321bc9efd3a4862a6b7e09a70ffc20f` — `timing: sample Chase Venom scripted steering at 20hz`.

Scope:
- only retail level ID `0x501`;
- only while Spider-Man synthesized input is active;
- manual controls untouched;
- rendering remains 60 Hz;
- physics remains 60 Hz;
- animation remains 60 Hz;
- camera remains 60 Hz;
- ordinary game Logic remains 60 Hz.

Synth wrapper:
- patches the direct retail synth call at `0x004BD572 -> 0x004BC300`;
- first scripted sample executes immediately;
- thereafter accumulates canonical `gTimerRelated @ 0x006B4CA8` ticks;
- executes retail synth when 3 ticks have accumulated;
- temporarily gives retail synth the full accumulated elapsed tick count through player `field_80`, then restores the normal one-tick body value before downstream physics;
- on the intervening two 60-Hz calls, restores/holds the most recent synthesized analogue axes instead of recomputing steering.

This is sample-and-hold control:
- script/control producer: 20-Hz-equivalent;
- movement/physics/render consumer: 60 Hz.

### Chase-only input-ramp correction restored as part of the producer model

The previously proven raw `field_8F0 += 0x20` input ramp was not sufficient by itself to fix Chase Venom, so the old broad experiment remained retired.

For this candidate it is reintroduced **only as part of the Chase-Venom synthesized-control model**:
- retail `ReadAnalogueInput` still runs every 60-Hz frame;
- when level `0x501` + synthesized input are active, the wrapper converts the raw ramp to `+0x20 per 3 canonical ticks`;
- remainder accumulation preserves exact real-time rate;
- manual/non-synthesized input remains untouched.

Reason:
- at stock 20-Hz authored cadence, the steering sample and input-strength ramp advanced together;
- holding steering at 20 Hz while leaving the ramp at 60 Hz would not faithfully reproduce that controller.

### Built-in route trace for this validation

This build also records the route state **in memory only** on actual 20-Hz synth updates.

Capacity:
- 2048 samples (over 100 seconds at 20 Hz);
- no synchronous gameplay file I/O;
- dumps only at clean shutdown.

Each sample records:
- canonical tick;
- elapsed synth ticks;
- normal body `field_80`;
- script clock / script-active / synth mode;
- output axes;
- input ramp;
- player state;
- player XYZ;
- heading;
- wall / ceiling flags;
- worker head before and after synth:
  - type;
  - block size;
  - data2;
  - data3.

For type-2 target steering specifically:
- head type `2`;
- block size is normally `5`;
- data2/data3 are the target X/Z values recovered from the retail worker layout.

This means the next runtime is both a fix test and a useful diagnostic if the fix fails.

Expected startup:
`chase_synth_20hz_install synth=1 synth_call=0x004BD572 synth_retail=0x004BC300 ramp_calls=>0 ramp_retail=0x004BD510 level=0x501 policy=60hz_render_physics_20hz_scripted_control_sample_hold cadence_ticks=3 manual_input=untouched`

Expected shutdown:
`chase_synth_20hz_stats synth_installed=1 ... active_calls>0 retail_updates>0 held_calls>0 ... ramp_corrections>0 ramp_unexpected=0 ...`

Then:
`chase_synth_trace ...`

### Completed scheduler capture retired

The large startup byte/xref dump from `117dae...` served its purpose and is no longer called in the new source.

The small in-memory `chase_scheduler_stats` counter remains for one more validation so the next log can simultaneously confirm that the cutscene stayed 60 Hz while the scripted producer ran at the intended sample/hold cadence.

### Static sanity

At source `6c40ef8...`:
- `main.cpp` braces: 1171/1171;
- parentheses: 5383/5383;
- one synth-install log marker;
- one synth-stats marker;
- one synth-trace format;
- completed scheduler startup capture call count: 0;
- synth call-site address `0x004BD572` appears in implementation/logging;
- one shutdown route-trace dump call.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `6c40ef8ff321bc9efd3a4862a6b7e09a70ffc20f` or a newer documentation-only descendant.

Use Level Select as before.

Test only Chase Venom:
1. verify the in-engine cutscene still visually remains at 60 FPS;
2. watch the exact end-of-building route where Spider-Man previously ran into the wall instead of following Venom;
3. report whether the route is fixed, improved, unchanged, or worse;
4. continue far enough to know whether the chase remains playable;
5. exit cleanly so the in-memory route trace is dumped;
6. return the single consolidated log.

If fixed:
- retain sample/hold as a narrow compatibility primitive for this 20-Hz-authored scripted controller;
- consider generalizing only to other proven scripted sequences, never manual gameplay.

If unchanged:
- inspect the emitted type-2 target/position trajectory and wall-state transition;
- determine whether the target data itself is wrong under Level Select or whether a downstream collision/surface transition consumes the held route incorrectly;
- do not lower the cutscene FPS.

## CUTSCENE TARGET RESET — ALL IN-ENGINE CUTSCENES MUST RUN AT 60 FPS (2026-10-05)

User requirement:
- all real-time / in-engine cutscenes should run at 60 FPS, consistent with gameplay;
- do **not** solve Chase Venom by retaining a 20- or 30-FPS cutscene cap;
- prerecorded FMV playback is a separate subsystem and is not what this requirement refers to.

Latest tested runtime:
- revision `358423b58434c46898827083d2354b16e27e251c`;
- log `spidey-decomp(20261005-230438).log`;
- Chase Venom still failed at the end-of-building sequence;
- user observed the in-engine cutscene itself visibly locks to ~30 FPS.

### Historical CBody limiter is now definitively insufficient

The corrected retail-level gate **did activate** in this runtime:

`chase_venom_body_cadence_stats installed=5 calls=54638 active_calls=54638 waits=2867 special_bypass=112 updater_bypass=132 max_delta=6391 last_retail_level=0x501 level_0x501_checks=54638`

Therefore:
- retail level ID `0x501` is correct even when entered through Level Select;
- Level Select does not prevent this detection path from activating;
- the historical minimum-two-tick `CBody::EveryFrame` limiter by itself does not repair Chase Venom under the current native-60 architecture.

Do not keep stacking fixes underneath that limiter.

### New interpretation

The user's observed 30-FPS drop during the in-engine cutscene is now a first-class suspect.

Current project target:
1. remove the retail/inherited 30-FPS real-time cutscene presentation/scheduler restriction;
2. run in-engine cutscenes at the same 60-Hz cadence as gameplay;
3. separately convert any remaining Chase Venom authored logic that assumes 20/30 update cadence to elapsed canonical ticks;
4. never depend on lowering presentation/simulation FPS to make a scripted sequence work.

PCGamingWiki corroborates the legacy behavior:
- the PC port is generally 30-FPS-oriented;
- some in-game cutscenes still require a 20-FPS cap in stock/compatibility setups;
- Chase Venom specifically breaks above 20 FPS.
This is treated as evidence about old authored timing, not as the desired solution.

### Failed body-level compatibility removed

Source commit:
- `117dae2359cbdf6c3db0dccf20c0b387b3f79c98` — `timing: capture 60hz cutscene scheduler path`.

Changes:
- removes the entire Chase-Venom-only `CBody::EveryFrame` cadence wrapper;
- removes all five compatibility call-site hooks;
- removes its shutdown stats path;
- normal native-60 body scheduling is restored for the next run.

### Scheduler capture added

Startup-only byte captures:
- `Logic @ 0x00455400`, size `0x1A0`;
- `Display @ 0x004555A0`, size `0x430`;
- `PlayAway @ 0x004559D0`, size `0x2C0`;
- `SpideyMain @ 0x00455C90`, size `0x610`.

Startup xref scans are also added for each range against:
- `0x005FAE98` — first historical frame-updater global;
- `0x0060CFB0` — second historical frame-updater global;
- `0x006B4CA0` — `Vblanks`;
- `0x006B4CA8` — `gTimerRelated` / canonical body timer.

Purpose:
- reconstruct the exact retail scheduler branch that causes real-time cinematics to present at ~30 FPS;
- determine whether Logic, Display or both are deliberately interleaved/skipped;
- identify the meaning of the two updater globals the historical frame limiter exempts.

### In-memory-only Chase scheduler telemetry

No synchronous per-frame file writes are added.

While retail level ID is `0x501`, the runtime accumulates:
- Logic-call count;
- Logic vblank delta histogram: <=0 / 1 / 2 / 3+;
- Logic calls while either frame-updater global is nonzero;
- Present-call count;
- Present vblank delta histogram: <=0 / 1 / 2 / 3+;
- Present calls while either frame-updater global is nonzero;
- longest consecutive run of `present delta == 2`.

Shutdown emits one summary line:

`chase_scheduler_stats ...`

This is designed to distinguish:
- genuine 60-Hz logic + 30-Hz presentation;
- 30-Hz logic + 30-Hz presentation;
- a mixed scheduler state controlled by the updater globals.

### Static sanity

At source `117dae235...`:
- `main.cpp` braces 1141/1141;
- parentheses 5247/5247;
- old Chase body cadence install marker count: 0;
- old Chase body wrapper count: 0;
- one PlayAway byte capture exists;
- scheduler Logic/Present record hooks are each installed once.

### Exact next test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `117dae2359cbdf6c3db0dccf20c0b387b3f79c98` or a newer documentation-only descendant.

Use Level Select as before.

Test:
1. enter Chase Venom;
2. observe normal gameplay FPS before the in-engine chase cutscene;
3. observe whether the in-game cutscene still visibly drops to ~30 FPS;
4. let the broken end-of-building route reproduce once;
5. exit cleanly;
6. return the one consolidated log.

Expected startup evidence:
- `Logic_Block`;
- `Display_Block`;
- `PlayAway_Block`;
- `SpideyMain_Block`;
- xref summaries for the two updater globals and Vblank/timer globals.

Expected shutdown:
- `chase_scheduler_stats ...`.

After that log:
1. reconstruct `PlayAway` / scheduler state exactly;
2. remove the real-time cutscene 30-FPS restriction globally so in-engine cutscenes run at 60;
3. re-test Chase Venom at true 60;
4. if the path still fails, convert the specific remaining Chase scripted primitive rather than lowering cutscene FPS.

## TEST CONTEXT NOTE — CHASE VENOM IS BEING ENTERED THROUGH LEVEL SELECT (2026-10-05)

User clarified that current Chase Venom reproductions are launched through the game's Level Select rather than reached naturally through story progression.

Interpretation:
- the corrected compatibility gate at source `88934f3a183c36a2e7ed65a3111a596b034f7570` calls the retail `Trig_GetLevelId @ 0x004DE770` after the level is loaded;
- therefore Level Select should **not** prevent cadence logging/activation if the loaded Chase Venom level really reports retail ID `0x501`;
- the next log's `level_0x501_checks` and `active_calls` counters are the authoritative proof;
- however, Level Select may bypass story/save/progression setup that normal campaign entry would establish;
- if the cadence gate is proven active and Chase Venom still behaves differently, keep Level Select as a possible setup-variable difference rather than assuming the timing fix itself is disproven.

For current testing, continuing through Level Select is acceptable and desirable for fast iteration. Do not require a full story playthrough unless later evidence specifically points to missing progression state.

## CHASE VENOM — INTERRUPTED SESSION RECOVERED; RETAIL LEVEL-ID GATE FIXED (2026-10-05)

Latest runtime tested before the disconnect:
- revision `e3875c8e1cce218c4e2b2ba86874830ef0a8a7ed`;
- consolidated log: `spidey-decomp(20261005-090809).log`;
- user confirmed Spider-Man still ran into the wall at the end of the Chase Venom cutscene;
- user also reported Venom's tentacle effect appearing detached/camera-relative, but explicitly asked to defer that and focus on the chase path.

### Critical runtime result

All five historical `CBody::EveryFrame` cadence hooks installed:

- `0x00460F99`;
- `0x00460FAE`;
- `0x00461111`;
- `0x00461126`;
- `0x004F6C19`.

However shutdown telemetry was:

`chase_venom_body_cadence_stats installed=5 calls=92968 active_calls=0 waits=0 special_bypass=0 updater_bypass=0 max_delta=0`

Therefore the previous run **did not test the historical cadence mechanism at all**. No body update was ever gated.

### Root cause of zero activation

The compatibility code used:

`Trig_GetLevelID() == 0x501`

But the reconstructed `trig.cpp` implementation is still a placeholder:

`int Trig_GetLevelID(void) { return 0x686868; }`

Thus any DLL-side call resolving to that reconstructed function can never report the real retail level ID.

`tools/names.json` identifies the original executable function as:

- `?Trig_GetLevelId@@YAHXZ`;
- address `0x004DE770`.

The Venom reconstruction's existing level check uses decimal `1281`, which is `0x501`, so the Chase Venom level constant itself remains consistent; the broken piece was the function implementation used by our gate.

### Source correction

Source commit:
- `88934f3a183c36a2e7ed65a3111a596b034f7570` — `timing: call retail level id for Chase Venom gate`.

Changes:
- added an explicit `__cdecl` retail function pointer to `0x004DE770`;
- `SpideyChaseVenomCadenceActive()` now calls the retail EXE directly;
- it no longer calls the unfinished decomp `Trig_GetLevelID()` stub;
- tracks the last retail level ID seen;
- counts successful `0x501` checks in memory;
- startup logging now records `retail_get_level_id=0x004DE770`;
- shutdown cadence stats now include:
  - `last_retail_level=0x...`;
  - `level_0x501_checks=...`.

Static sanity at `88934f3...`:
- `main.cpp` braces: 1131/1131;
- parentheses: 5244/5244;
- two textual occurrences of `0x004DE770` (pointer + startup description);
- the only `Trig_GetLevelID()` textual occurrence in this compatibility area is the explanatory comment; no active call remains;
- cadence install/stats markers each occur once.

### Interpretation

The failed `e3875c8...` test does **not** falsify Kellog's body-cadence fix.

It only proves:
1. the five call-site hooks themselves installed correctly;
2. our level activation predicate was permanently false because it used a placeholder decomp function;
3. the actual minimum-two-tick wait path never ran.

The next runtime is therefore the **first real validation** of the adapted historical fix.

### Exact next test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `88934f3a183c36a2e7ed65a3111a596b034f7570` or a newer documentation-only descendant.

Test:
1. replay Chase Venom;
2. reach the end-of-building automated chase;
3. observe whether Spider-Man now follows Venom through/out of the building instead of turning into the wall;
4. continue far enough to verify the chase no longer fails from separation;
5. exit cleanly and return the single consolidated log.

Expected startup:
`chase_venom_body_cadence_install ... retail_get_level_id=0x004DE770 ...`

Expected shutdown if activation is finally correct:
- `active_calls > 0`;
- `level_0x501_checks > 0`;
- normally `waits > 0`;
- `last_retail_level` should reflect the final level/menu state at shutdown and is not itself required to remain `0x501`.

If `active_calls > 0` and the chase is still broken, then the historical fix truly fails under this project's 60-Hz architecture and the next step is to compare its timer/updater assumptions against our synthetic 60-Hz callback rather than continue patching unrelated locomotion counters.

## CHASE VENOM — FAILED RAMP PATCH RETIRED; PROVEN CBODY CADENCE FIX ADAPTED (2026-10-05)

Latest failed runtime:
- revision `66427a9a84dd82a8ddf2a6dad5a9d46277785983`;
- consolidated log: `spidey-decomp(20261005-084226).log`;
- user confirmed the Chase Venom path bug remained unchanged;
- ignore the later/new-game experimentation in that session for diagnosis.

### Scripted input ramp result

The `field_8F0` ramp experiment was definitely active:
- install succeeded at the only direct retail `CPlayer::ReadAnalogueInput @ 0x004BD510` call;
- shutdown stats:
  - total calls: 9508;
  - scripted calls: 2284;
  - corrected active-axis updates: 1117;
  - unexpected retail behavior: 0;
  - remainder: 0.

Conclusion:
- the fixed `+0x20` per-call ramp is a real raw-cadence quirk;
- changing it to a 20-Hz-equivalent rate is **not sufficient** to fix Chase Venom;
- remove the experiment rather than retaining an unvalidated behavioral change.

The source no longer installs or contains the scripted-input ramp patch.

### External proven fix located

A public reverse-engineering patch in `krystalgamer/spidey-tools` provides an existing fix for this exact high-FPS Chase Venom failure:

- commit: `cac3b525c7d7cddd3650c4cbb6a7af3ab6bfc97d` — `Added Kellog's Frame Limiter`;
- later retained in current `spidey-tools` releases with `FPS_DIVIDER = 2`.

Its mechanism is **not** a path-script, turn, wall, or Venom-specific patch.

It hooks the five retail `CBody::EveryFrame @ 0x00460ED0` call sites:
- `0x00460F99` — first/radial path in `CBody::InterleaveAI`;
- `0x00460FAE` — second/normal path in `CBody::InterleaveAI`;
- `0x00461111` — first/radial path in `Ob_AI`;
- `0x00461126` — second/normal path in `Ob_AI`;
- `0x004F6C19` — `CWeb_AI`.

For ordinary bodies (mCBodyFlags bit 2 clear), before calling retail `EveryFrame` it waits until:

`gTimerRelated/current_frame - body->field_7C >= 2`

unless either retail frame-updater global at `0x005FAE98` or `0x0060CFB0` is active.

This proves the important missing invariant:
- large parts of retail body AI were authored expecting a **minimum two-vblank body update quantum**;
- our native-60 conversion made `field_80 == 1` common;
- player physics was explicitly converted for that one-tick path, but not every scripted body-AI state machine was.

This directly explains why local elapsed-time fixes could leave Chase Venom broken even when the player physics, top-level synthesized-input scheduler, heading interpolation, and wall timer are individually correct.

### New compatibility implementation

Source commits:
- `8997afe0a35ced32ac3bd8496cc4008ba83cffad` — `timing: adapt proven Chase Venom body cadence fix`;
- `ad4bcf5ef05da35758b965f14754d14edcfa3ef4` — `timing: validate Chase Venom body cadence level-wide`.

Implementation:
- reproduces the historical five-callsite `CBody::EveryFrame` interception;
- keeps retail `CBody::EveryFrame` itself authoritative;
- respects the historical `mCBodyFlags & 4` bypass, because retail itself forces those bodies to `field_80=2`;
- respects the two historical frame-updater globals;
- waits only until the canonical 60-Hz vblank clock supplies the second elapsed tick;
- records counters only; no per-frame disk I/O.

Important scope difference from the old global limiter:
- the compatibility gate is active **only while `Trig_GetLevelID() == 0x501` (Chase Venom)**;
- every other level remains on the project's validated native-60 path;
- this level-wide activation is intentional for the first validation because the prior post-synth trace did not reliably identify the exact bad transition window;
- if this fixes the sequence, narrow the activation window afterward rather than guessing beforehand.

Startup expected:
`chase_venom_body_cadence_install calls=5 expected=5 retail_every_frame=0x00460ED0 level=0x501 activation=level_0x501 minimum_delta=2 scope=chase_venom_level_only source=kellog_frame_limiter_adapted`

Shutdown expected:
`chase_venom_body_cadence_stats installed=5 calls=... active_calls=... waits=... special_bypass=... updater_bypass=... max_delta=...`

### What this test means

If Chase Venom now works:
- the root layer is confirmed: one or more level/script body-AI consumers require the retail minimum-two-tick cadence;
- do **not** conclude that the entire game should be returned to 30 Hz;
- next work is to narrow which body/state transition actually needs the gate, then decide between:
  1. a small scripted-sequence cadence compatibility island, or
  2. converting that remaining body-AI primitive to true one-tick/native-60 semantics.

If Chase Venom is still broken:
- verify the five hooks installed and `active_calls/waits` were nonzero;
- if they were, compare our wrapper semantics directly against the external patch's updater globals / call ordering before pursuing another unrelated timer.

### Static sanity

At source commit `ad4bcf5e...`:
- `main.cpp` braces: 1130/1130;
- parentheses: 5238/5238;
- one cadence install marker;
- one cadence shutdown-stats marker;
- failed `scripted_input_ramp_*` code count: 0;
- all five historical callsite addresses present exactly once in the new installer.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `ad4bcf5ef05da35758b965f14754d14edcfa3ef4` or a newer documentation-only descendant.

Test only the relevant path:
1. enter/replay Chase Venom;
2. reach the automated building chase;
3. watch the end where Venom jumps out and Spider-Man previously turned right / jumped into the wall;
4. confirm whether Spider-Man now follows the authored route;
5. continue until the chase no longer fails from excess Venom distance;
6. exit cleanly and return the one consolidated log.

No need to do unrelated new-game testing for this build.

## CHASE VENOM — SHARED SCRIPTED-INPUT RAMP FIX CANDIDATE IMPLEMENTED (2026-10-05)

Latest startup/xref runtime:
- revision `4c95746a345b285ed6659c548420698fbbe6fe13`;
- consolidated log: `spidey-decomp(20261005-082904).log`;
- exact input-dispatch bytes and DF0/DF4/DF8 xrefs captured successfully.

### Heading interpolation cleared

The DF0/DF4/DF8 xref pass disproved the remaining “turn interpolation itself runs once per raw frame” hypothesis.

Retail `SpideyAI0` consumer around `0x004B7181`:
- loads `field_DF8` (remaining turn/interpolation count);
- loads player `field_80`;
- when `DF8 > field_80`:
  - multiplies signed `field_DF4` by `field_80`;
  - adds that elapsed-scaled turn step to the current heading;
  - subtracts `field_80` from `field_DF8`;
- otherwise:
  - snaps heading to `field_DF0`;
  - clears `field_DF8`.

Therefore `CPlayer::SetTargetTorsoAngle` + its main consumer are already elapsed-tick aware. Do not scale DF4 or inflate DF8.

The other DF4 xref around `0x004B23A4` only uses the sign of DF4 to choose left/right turn animation variants; it is not the interpolation integrator.

### Proven raw-cadence seam: CPlayer::ReadAnalogueInput field_8F0 ramp

The captured `CPlayer::ReadAnalogueInput @ 0x004BD510` contains:

- when analogue axes are non-zero:
  - `field_8F0 += 0x20`;
  - clamp `field_8F0` to `0x100`;
- when axes are zero:
  - `field_8F0 = 0`.

This ramp uses **no `field_80` at all**.

The same routine calls `CPlayer::SynthesizeAnalogueInput @ 0x004BC300` whenever player `field_1AC` is active, then runs the same raw `+0x20` ramp on the synthesized axes. Thus scripted Spider-Man movement definitely passes through a per-update ramp whose real-time behavior changes with Logic cadence.

At:
- 20 updates/s: 8 ramp updates = ~400 ms to reach `0x100`;
- 30 updates/s: ~267 ms;
- 60 updates/s: ~133 ms.

That is a strong fit for the known Chase Venom symptom: the script can request the correct route while Spider-Man's scripted movement/steering strength reaches full response much earlier in real time, pushing him into the wall before the authored route transition.

### Implemented fix candidate

Source commit:
- `e602bc64e36cf872e2060cfee8ac38ad2742c537` — `timing: preserve scripted input ramp at 20hz`.

Implementation:
- hook direct retail calls to `CPlayer::ReadAnalogueInput @ 0x004BD510`;
- call the full retail routine first;
- only when `field_1AC != 0` (scripted/synthesized player input) and analogue axes remain active:
  - verify retail performed the exact expected fixed `+0x20` / clamp behavior;
  - replace only that ramp result with a canonical-clock equivalent of `+0x20 per 3 elapsed 60-Hz ticks`;
- use a remainder sidecar so the 60-Hz sequence is exact over time:
  - field_80=1: +10, +11, +11 = +32 per 3 ticks;
  - field_80=2: +21, +21, +22 over three updates = +64 per 6 ticks;
  - field_80=3: +32, reproducing retail 20-Hz behavior exactly;
- field_80 is clamped to the same 0..6 elapsed range used elsewhere in retail player timing;
- manual/non-synthesized player input is left completely untouched;
- if retail changed field_8F0 in any unexpected way, the wrapper leaves it alone and increments an in-memory unexpected counter rather than forcing a guess.

Validation telemetry:
- startup:
  - `scripted_input_ramp_install calls=...`;
- shutdown:
  - `scripted_input_ramp_stats installed=... calls=... scripted_calls=... corrections=... unexpected=... remainder=...`.

No per-frame disk I/O is added.

### Why 20-Hz basis is intentionally scoped to scripted input

The PC port is generally designed around ~30-FPS gameplay, but the Chase Venom in-game cutscene is specifically known to require a temporary 20-FPS cap. The raw field_8F0 ramp sits after the synthesized-input producer and before ordinary locomotion consumes the axes. Applying the 20-Hz rate only while `field_1AC` is active targets that cutscene/script class without making manual controls sluggish or undoing native-60 player physics.

If this validates, it is a reusable engine fix for other Spider-Man scripted-movement/cutscene sequences that use the same synthesized-input path.

### Static sanity

At source commit `e602bc64...`:
- `main.cpp` braces: 1127/1127;
- parentheses: 5234/5234;
- one install log marker;
- one shutdown stats marker;
- xref-only DF capture calls removed from startup.

GitHub currently reports no Actions workflow runs for branch `dev`, so runtime build remains the authoritative compile check.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `e602bc64e36cf872e2060cfee8ac38ad2742c537` or a newer documentation-only descendant.

Test:
1. replay the same end-of-building Chase Venom sequence at native 60 Hz;
2. watch specifically whether Spider-Man now follows Venom out of the building instead of turning right/jumping into the wall;
3. continue far enough to confirm the chase does not game-over from excess separation;
4. exit cleanly;
5. return the single consolidated `spidey-decomp.log` and report whether the bad path was fixed, improved, unchanged, or made worse.

Expected log evidence:
- `scripted_input_ramp_install calls=>0`;
- shutdown `scripted_input_ramp_stats` with `scripted_calls>0`, `corrections>0`, and ideally `unexpected=0`.

If successful:
- keep this shared synthesized-input fix;
- remove any remaining Chase-Venom-only RE scaffolding;
- then audit other raw-per-update player/cutscene primitives using the same methodology.

If unchanged:
- the field_8F0 ramp is still a proven raw-cadence bug, but it is not sufficient by itself to explain Chase Venom; retain the evidence and continue to the next consumer rather than reverting validated native-60 systems.

## CHASE VENOM RE — WALL DETECTOR ELAPSED-AWARE; TURN INTERPOLATION XREF PASS READY (2026-10-05)

Latest runtime:
- revision `335ea9443b7faf79108cdfd34e168afb8fa21fe5`;
- consolidated log: `spidey-decomp(20261005-081314).log`;
- user replayed the Chase Venom failure sequence successfully.

### Runtime trace result

The attempted post-`CPlayer::SynthesizeAnalogueInput` in-memory trace installed at call site `0x004BD572`, but shutdown reported:
- `samples=0`;
- `dropped=0`.

Treat that literally: the sampled path/condition was not active in the way assumed during the reproduced sequence. Do not infer runtime values from a trace that did not fire.

### Static retail findings from this build

All seven startup retail blocks captured successfully.

Important timing result from `CPlayer::CheckRunIntoWall @ 0x004BFBC0`:
- its persistent run-into-wall counter at player offset `+0xAD7` advances by the low byte of player `field_80`;
- therefore the basic wall-hit delay is already elapsed-tick aware and is **not** simply running 3x fast at native 60 Hz.

Important result from `CPlayer::SetTargetTorsoAngle @ 0x004C6970`:
- computes target heading in `field_DF0`;
- computes per-update angular step in `field_DF4`;
- computes the remaining interpolation/update count in `field_DF8`;
- this routine itself contains no `field_80` scaling;
- default turn interpolation uses a nominal count of 10 updates, with state-dependent alternatives and clamping;
- this is now the strongest raw-cadence candidate, but do **not** patch it until its consumer is grounded. If the consumer already multiplies/decrements by elapsed ticks, changing the setter would be wrong.

The observed Chase Venom symptom still fits this seam well:
- the script can request a correct target direction;
- if heading interpolation completes too quickly in real time at 60 Hz, Spider-Man can curve into the wall earlier than authored and transition into wall crawl;
- this is a hypothesis pending the consumer xref.

### Source change

Source commit:
- `8da02a0745416e3d5544b8208421a0747dfc090b` — `timing: trace player turn interpolation consumers`.

Changes:
- removed the zero-sample Chase Venom in-memory sampler and its shutdown dump;
- added a startup-only capture of the retail input-dispatch candidate `0x004BD510..0x004BD74F`;
- added a startup xref scanner over `SpideyAI0 @ 0x004B13F0..0x004B878F`;
- scanner finds exact little-endian references to player offsets:
  - `0x0DF0` — target heading;
  - `0x0DF4` — per-update heading step;
  - `0x0DF8` — heading interpolation/count state;
- for each reference it logs the exact address plus a compact surrounding byte window;
- no per-frame/gameplay disk logging is added.

Static sanity:
- `main.cpp` braces: 1118/1118;
- parentheses: 5200/5200;
- old chase trace summary code removed;
- exactly one ReadAnalogueInput-candidate capture and one DF0/DF4/DF8 scan request are present.

### Exact next run

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `8da02a0745416e3d5544b8208421a0747dfc090b` or a newer documentation-only descendant.

No gameplay test is needed for this capture:
1. launch normally;
2. reach the menu / allow startup patches to finish;
3. exit cleanly;
4. return the single consolidated `spidey-decomp.log`.

Expected evidence:
- `high_fps_re_bytes_done label=CPlayer_ReadAnalogueInput_Candidate ... valid=1`;
- `high_fps_field_xref ... field=0x0DF0 ...`;
- `high_fps_field_xref ... field=0x0DF4 ...`;
- `high_fps_field_xref ... field=0x0DF8 ...`;
- corresponding `high_fps_field_xref_done` summaries.

Then:
1. disassemble the exact DF4/DF8 consumers inside `SpideyAI0`;
2. determine whether count/step application is raw-per-call or elapsed-tick aware;
3. patch the smallest shared turn primitive if proven raw-cadence;
4. only then replay Chase Venom for validation.

Do not change the Chase Venom distance threshold and do not alter the validated player native-60 physics path.

## CHASE VENOM RE — SCRIPT INTERPRETER CLEARED; LOCOMOTION CONSUMER TRACE READY (2026-10-05)

Latest capture runtime:
- revision `d7a5ca0c2af706fbc59d1fd902a7df907c0905b9`;
- consolidated log: `spidey-decomp(20261005-074910).log`;
- all requested retail blocks captured successfully:
  - `CPlayer::SwitchToSynthesizedInput @ 0x004BC1A0`;
  - `CPlayer::SynthesizeAnalogueInput @ 0x004BC300`;
  - `CVenom::FollowDirections @ 0x004EB530`.
- the validated native-60 player physics hooks still install successfully in the same runtime.

### Player synthesized-input timing result

The simple hypothesis that the Chase Venom script interpreter advances 3x too fast at 60 Hz is now disproven.

Retail `CPlayer::SynthesizeAnalogueInput`:
- reads player `field_80` on entry;
- advances the top-level script clock at player offset `+0x1B0` by `field_80`;
- schedules/dispatches script entries against that elapsed-tick clock rather than one unit per Logic call;
- maintains active command blocks through the linked list at player offset `+0x1BC`.

Recovered worker timing:
- command types 3, 6, 7, 8 and 9 decrement their active timers by `field_80`, not by one raw update;
- command type 2 is a geometric target-steering worker: it repeatedly computes direction from Spider-Man's current position to its target and synthesizes analogue input instead of directly integrating a fixed per-frame position;
- no justification exists for globally scaling `CPlayer::SynthesizeAnalogueInput` by 1/2 or 1/3.

This is encouraging for other scripted sequences: the shared script scheduler is already canonical elapsed-time aware and should remain untouched.

### Chase Venom coordinator result

Retail `CVenom::FollowDirections @ 0x004EB530` contains the level-0x501 chase-distance/failure coordination:
- it measures the Venom-to-Spider-Man separation;
- when Spider-Man becomes too far behind, it sets the chase failure state and pulses the relevant level node;
- this matches the observed eventual game-over after Spider-Man takes the bad path.

Therefore:
- do not increase the chase-distance threshold as the primary fix;
- that would hide the symptom instead of fixing why Spider-Man turns into/climbs the wall.

### New primary suspect

The remaining high-value seam is downstream of synthesized input:
- `CPlayer::CheckForwards @ 0x004BF8A0`;
- `CPlayer::CheckRunIntoWall @ 0x004BFBC0`;
- `CPlayer::CheckStickToCeiling @ 0x004BFCE0`;
- `CPlayer::CheckStickToWall @ 0x004BFEC0`;
- `CPlayer::AI @ 0x004C65C0`;
- `CPlayer::SetTargetTorsoAngle @ 0x004C6970`;
- `CPlayer::GetEffectiveHeading @ 0x004C6AA0`.

These routines consume the synthesized axes and decide locomotion, heading and wall/ceiling transitions. A legacy cadence assumption here naturally explains the actual symptom: the scripted target direction can be correct while Spider-Man turns into the building and enters wall-crawl state.

### New diagnostic source

Source commit:
- `3d54949ff8fbe636aeca6501a8fe06ab38a4ea11` — `timing: trace Chase Venom locomotion consumer`.

Startup-only retail captures now gather:
- `CPlayer_AI_Block @ 0x004C65C0`, size `0x2E0`;
- `CPlayer_CheckForwards_Block @ 0x004BF8A0`, size `0x320`;
- `CPlayer_CheckRunIntoWall_Block @ 0x004BFBC0`, size `0x120`;
- `CPlayer_CheckStickToCeiling_Block @ 0x004BFCE0`, size `0x1E0`;
- `CPlayer_CheckStickToWall_Block @ 0x004BFEC0`, size `0x1F0`;
- `CPlayer_SetTargetTorsoAngle_Block @ 0x004C6970`, size `0x130`;
- `CPlayer_GetEffectiveHeading_Block @ 0x004C6AA0`, size `0x130`.

A Chase-Venom-only post-synth trace is also installed:
- active only when level ID is `0x501` and Spider-Man is in synthesized-input mode;
- records entirely in memory during gameplay; **no per-frame disk I/O**;
- records canonical tick / `field_80`, script clock and next command, active worker block types, synthesized axes, player state, position/velocity, Venom delta, heading, collision, animation, wall/ceiling flags and ignore-input state;
- writes the accumulated trace only during normal timer/session shutdown;
- capacity is 4096 samples; dropped-sample count is reported.

Static source sanity at `3d54949...`:
- `main.cpp` braces: 1126 open / 1126 close;
- parentheses: 5236 open / 5236 close;
- completed first-pass `CPlayer_SynthesizeAnalogueInput_Block` capture label removed;
- exactly one new trace installer marker and one shutdown dump call are present.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `3d54949ff8fbe636aeca6501a8fe06ab38a4ea11` or a newer documentation-only descendant.

This time **reproduce Chase Venom once**:
1. play the end-of-building scripted chase where Spider-Man takes the bad route;
2. let the wrong turn/wall-climb happen far enough to make the failure obvious;
3. then exit the game cleanly so `timeKillEvent` can dump the in-memory trace;
4. return the single consolidated `spidey-decomp.log` and note approximately where the visible wrong turn occurred.

Expected startup evidence:
- seven `high_fps_re_bytes_done ... valid=1` lines for the player locomotion/heading blocks;
- `chase_venom_trace_install calls=...`.

Expected shutdown evidence:
- `chase_venom_trace_summary ... samples=... dropped=...`;
- followed by `chase_venom_trace ...` samples.

After that log:
1. reconstruct the seven consumer routines;
2. correlate the exact sample where heading/state/wall flags diverge with the active synthesized command type;
3. patch the smallest shared timing/locomotion primitive that causes the bad turn;
4. prefer a reusable native-60 fix over any level-specific 20-FPS throttle.

Do not alter the validated player physics hooks, master 60-Hz timer, or Chase Venom distance threshold while this consumer path is being isolated.

## CHASE VENOM HIGH-FPS FAILURE CONFIRMED; PLAYER SYNTHESIZED-INPUT RETAIL CAPTURE ADDED (2026-10-05)

Latest tested runtime:
- revision `82d2f966f2691010f5e60af1c1d6cdd6cebdbc0a`;
- consolidated log: `spidey-decomp(20261005-073554).log`;
- user reproduced the known Chase Venom cutscene/path failure at the current native-60 cadence;
- the already-validated player native-60 physics hooks all install successfully in this run, so do not roll them back as a workaround.

External symptom match:
- PCGamingWiki documents the end-of-building Chase Venom cutscene bug: after Venom exits the building, Spider-Man follows an improper path / turns into the wall, falls too far behind, and can trigger game over;
- the community workaround is a temporary 20-FPS cap for this sequence.

Current RE conclusion:
- previous exact retail capture of `CVenom_SynthesizeAnalogueInput @ 0x004E9B00` showed its main command timers/path progression already consume `field_80`; do not apply a blind Venom-wide 0.5/0.333 scaler;
- the visible failing actor is Spider-Man while the game owns his controls during the scripted chase;
- retail has an unreconstructed `CPlayer::SwitchToSynthesizedInput @ 0x004BC1A0` and `CPlayer::SynthesizeAnalogueInput @ 0x004BC300` path, making that the highest-value unresolved seam;
- `CVenom_FollowDirections @ 0x004EB530` remains the compact dispatcher coordinating the chase/script and is captured alongside the player routines.

New source:
- `78ac212e09baa7e3090d4945b156f99c4d97eaeb` — one-shot startup capture for the Chase Venom synthesized-input path.

Startup-only captures added:
- `CPlayer_SwitchToSynthesizedInput_Block @ 0x004BC1A0`, size `0x140`;
- `CPlayer_SynthesizeAnalogueInput_Block @ 0x004BC300`, size `0x11B0`;
- `CVenom_FollowDirections_Block @ 0x004EB530`, size `0x160`.

Logging policy:
- these are startup byte dumps only;
- no synchronous per-frame Chase Venom telemetry was added, specifically to avoid reintroducing the logging-induced hitches previously eliminated;
- the three blocks are immutable retail code, so the next run does **not** need to reproduce the chase bug to collect them.

Exact next step:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. merely reach the game/menu and exit once the startup log is written;
3. return the single consolidated `spidey-decomp.log`;
4. reconstruct the player synthesized-input command parser and compare it with the already-recovered Venom command semantics;
5. if a raw 20/30-FPS-authored step/countdown is proven, patch only that seam to canonical 60-Hz elapsed-time / substep semantics;
6. only add level-specific in-memory runtime tracing if the static retail reconstruction still leaves multiple plausible active branches.

Do not solve this by globally reducing Logic to 20 FPS or by changing the validated native-60 player physics path.

## RUNTIME VALIDATED — NARROW PLAYER NATIVE-60 BATCH FIXES PERCEIVED SPEED WITHOUT REGRESSING GROUNDING (2026-10-05)

Latest tested runtime:
- revision `35d7a3e385c8ba16a521018bad9a781cef15321b`;
- consolidated log: `spidey-decomp(20261005-065935).log`;
- user report: timing felt materially better and no longer felt sped up;
- session ran from 01:08:35.9836581 to 02:08:20.4822684 local time (~3584.499 s / 59m44.5s) and exited normally.

Install validation:
- normal friction hook = 1;
- crawl friction hook = 1;
- special-move half-step threshold = 1;
- normal-move half-step threshold = 1;
- post-collision velocity restore threshold = 1;
- fall half-step threshold = 1;
- crawl-move half-step threshold = 1;
- no expected-byte failures or partial-install evidence.

Master timing:
- source callbacks = 3,583,686;
- dispatched callbacks = 215,021;
- virtual ticks = 215,021;
- paused callbacks = 297;
- unexpected vblank deltas = 1 across the full session;
- last interval = 17 ms;
- 215,021 dispatched ticks across ~3584.499 seconds = ~59.986 Hz.
- This strongly confirms the canonical game timer is running at the intended ~60 Hz rather than globally fast.

Grounding / scripted-cutscene regression guard:
- at frame 28200 the live-cutscene player body is exactly `-78729216,15695872,-155586560`;
- shadow is `-78729216,16089088,-155586560`;
- shadow normal is `0,-4096,0`;
- these are the same known-good grounded values previously observed with fully retail player physics.
- Frames 28500 and 28800 retain the same body/shadow ground band, so the new narrow hooks do not reproduce the old reconstructed-physics fall-through regression.

Other observations:
- no crash/access-violation/assert evidence in the session;
- session exits with `exit_code=0`;
- one isolated `[DXERROR] D3D error=0x00000001` from `PCTex.cpp:1740` occurs mid-session, immediately followed by continued renderer/input activity and is not correlated with the timing work or a process failure. Track separately only if a visible texture/rendering symptom appears.
- consolidated timing output has `file_telemetry=0`, so no periodic `timing_logic hz=` / `timing_present hz=` samples were emitted in this run; the long-run canonical timer calculation above is therefore the strongest numeric cadence evidence.

Conclusion:
- mark the first narrow retail-preserving player native-60 batch as **runtime validated**;
- keep full reconstructed `patch_physics()` disabled;
- retain retail ownership of collision/grounding/landing/platform/cutscene physics;
- preserve the current two friction hooks + five threshold-byte edits;
- next native-60 work should move outward to other proven raw-frame/countdown primitives (AI/state timers) rather than altering the validated player-physics path.

## IMPLEMENTED — FIRST NARROW RETAIL-PRESERVING PLAYER NATIVE-60 BATCH READY (2026-10-04)

Source chain:
- `091ff54415df063c2b545cad6a81061b7130b855` — declare the narrow player friction hook;
- `7c31e955b362a76e23798e4b442b9e93bd4b6ee4` — implement half-step force/damping inside the retail friction seam;
- `5d0c2585c9f1437fb4677cf99c1d013c1e754383` — install the two retail friction call hooks plus displacement/fall threshold edits;
- `9235eb1db0a64ed479f3bcde6356ea42b3b86f96` — preserve the retail zero-tick path by changing comparison thresholds instead of branch opcodes.

Runtime ownership:
- retail `CPlayer::DoPhysics @ 0x00466CE0` still owns the complete normal-physics function;
- retail `CPlayer::DoCrawlingPhysics @ 0x00467FD0` still owns the complete crawl-physics function;
- retail `CPlayer::DoSwingingPhysics @ 0x00467D20` remains untouched;
- global `patch_physics()` remains disabled;
- custom code owns only two friction CALL sites plus five one-byte elapsed-vblank thresholds.

Force/damping hooks:
- `0x00466D84 -> CVector::operator%=(mFric) @ 0x004E76B0`;
- `0x0046801D -> CVector::operator%=(mFric) @ 0x004E76B0`.
- Retail has already executed `mVel += mAcc` when the wrapper is entered.
- For `field_80 == 1`, the wrapper subtracts `mAcc` to recover start-of-tick velocity and applies the previously derived fixed-point half-step acceleration/damping factors.
- Retail `KillSmall()` executes immediately afterward as before.
- All other `field_80` values fall through to the retail friction operator.

Displacement/fall threshold edits:
- special no-collision movement threshold immediate: `0x00466DC9: 02 -> 00`;
- normal movement threshold immediate: `0x00466E22: 02 -> 00`;
- post-collision velocity reconstruction threshold immediate: `0x00467592: 02 -> 00`;
- fall displacement threshold immediate: `0x004677ED: 02 -> 00`;
- crawling movement threshold immediate: `0x00468056: 02 -> 00`.

Why the threshold is changed from 2 to 0:
- retail `CBody::EveryFrame` derives `field_80 = gTimerRelated - field_7C`, asserts it is non-negative, and caps it at 6;
- keeping the existing `JLE` while changing `cmp ...,2` to `cmp ...,0` preserves the original direct shortcut for a zero-tick update;
- `field_80 == 1` now uses retail's already-existing general elapsed-vblank expression, yielding approximately half displacement/fall for a native 60-Hz tick;
- `field_80 == 2` also uses the general path but its extra term is zero, reproducing the original full displacement;
- `field_80 > 2` continues to use the same retail catch-up path it always used.
- For odd fixed-point movement values, the retail general expression can differ from a plain arithmetic half by at most one fixed-point unit.

Retail-byte grounding:
- captured normal-physics bytes confirm both friction CALL sites target `0x004E76B0`;
- all five threshold immediate bytes are exactly `0x02` in the known retail capture;
- patch installation validates expected CALL targets/bytes before writing anything.

Static validation at `9235eb1...`:
- `physics.cpp` braces/parentheses balanced;
- `main.cpp` braces/parentheses balanced;
- one friction-hook declaration and one definition;
- one installer and one installer call;
- zero live `patch_physics();` calls;
- all five threshold sites present exactly once in the installer.
- This batch has **not yet been compiled or runtime-tested** under the local matching VC6 toolchain.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected revision:
- `9235eb1db0a64ed479f3bcde6356ea42b3b86f96` or a newer documentation-only descendant.

First confirm startup telemetry contains:
- `player_physics_60_install normal_friction=1 crawl_friction=1 special_move=1 normal_move=1 velocity_restore=1 fall=1 crawl_move=1`.

Then do one meaningful pass:
1. New Game -> difficulty -> first Doc Ock pre-render -> following live/in-engine cutscene;
2. confirm Spider-Man is still correctly grounded/alive at the scripted spawn;
3. continue into ordinary gameplay and judge walking/running/jumping/falling pace at 60;
4. test wall/ceiling crawling if convenient;
5. fight/move around enemies briefly so the already-active native-60 RotY path is exercised too;
6. quick manual-aim/camera sanity only; do not retune those frozen systems;
7. return the single consolidated `spidey-decomp.log` and note whether game/player pace now feels materially closer to correct 60-Hz timing.

If this gate passes, continue the native-60 audit outward from player physics into the next proven raw-frame/countdown primitives. Do not broaden timing patches from guesswork.

## RUNTIME RESULT — RETAIL PHYSICS RESTORATION VALIDATED; SCRIPTED SPAWN FIXED (2026-10-04)

Latest tested runtime:
- revision `0b4d314341a516ed694f8ba15ada53389adb4dcb`;
- user confirmed the first pre-render -> in-engine cutscene transition now succeeds;
- Spider-Man remains alive at the scripted spawn instead of falling into the yellow void/death trigger;
- user continued into normal gameplay successfully;
- session exits normally with `exit_code=0`.

Runtime evidence from `spidey-decomp(20261004-225452).log`:
- frame 900 player body = `-78886624,15695872,-158920544`;
- frame 900 shadow = `-78886624,16089088,-158920544`;
- shadow normal = `0,-4096,0`, confirming the retail ground path is again keeping the player on the intended floor band;
- frame 1200 remains on the same valid ground band and the session continues into ordinary gameplay;
- timer pacing shutdown reports `dispatched_callbacks=4902`, `virtual_ticks=4902`, `unexpected_vblank_delta=1`, `last_interval_ms=17`;
- this continues to support an essentially 60-Hz canonical master tick rather than an accidentally over-fast timer source.

Conclusion:
- the retail-physics restoration is validated and the scripted-spawn regression is closed;
- do not reinstall the whole reconstructed `CPlayer::DoPhysics` replacement;
- native-60 work can now proceed through narrow retail-preserving hooks around authored 30-Hz integration/countdown primitives.

Immediate next implementation target:
- preserve retail collision/landing/spawn/platform behavior;
- hook only the normal/crawling force + damping integration seams around `0x00466D79/0x00466D84/0x00466D8B` and `0x00468012/0x0046801D/0x00468024`;
- reuse the already-derived half-step fixed-point math so two 60-Hz updates reproduce one retail two-vblank/30-Hz velocity quantum;
- keep displacement/collision ownership in retail until a narrower displacement seam is independently grounded.

## IMPLEMENTED — RETAIL PLAYER PHYSICS RESTORED; NATIVE-60 REWORK MOVES TO NARROW HOOKS (2026-10-04)

Source:
- `c109e2e869b06575ec942170d4bb89b568650b51` — stop installing `patch_physics()` globally.

Runtime ownership now:
- retail `CPlayer::DoPhysics @ 0x00466CE0` active;
- retail `CPlayer::DoSwingingPhysics @ 0x00467D20` active;
- retail `CPlayer::DoCrawlingPhysics @ 0x00467FD0` active;
- reconstructed/native-60 physics source remains in `physics.cpp` as RE/reference but is not patched into the live executable;
- native-60 `CAIProc_RotY` remains active;
- Mysterio elapsed-time laser fix remains active.

Why this is required:
- previous retail-physics runtime `21ced52bc9c8e5903fe66cc939e4114804c248fb` handles the exact first live-cutscene spawn correctly;
- reconstructed-physics runtime reaches the same scripted camera/focus handoff but loses player grounding and falls through the level;
- at frame 900:
  - retail body Y = `15695872`, valid ground shadow;
  - reconstructed body Y = `25915794`, while shadow position remains near the expected ground band (~`15817598`);
- therefore spawn/camera/level data is valid and the full-function physics replacement is the regression.

Narrow retail-preserving 60-Hz seam identified for follow-up:
- `SpideyAI0` directly calls retail `CPlayer::DoPhysics` at `0x004B1B18`;
- retail normal-physics integration begins with:
  - `0x00466D79` -> `CVector::operator+=(mAcc)`;
  - `0x00466D84` -> `CVector::operator%=(mFric)`;
  - `0x00466D8B` -> `CVector::KillSmall()`;
- crawling has the same compact sequence around:
  - `0x00468012`;
  - `0x0046801D`;
  - `0x00468024`.
- This gives a path to implement native-60 force/damping/displacement corrections without replacing retail collision/grounding/cutscene behavior.

Exact next validation:
1. build newest `dev`;
2. New Game -> difficulty -> first Doc Ock pre-render;
3. verify the following in-game cutscene plays;
4. verify Spider-Man remains at the scripted spawn and does **not** fall into the yellow void;
5. if spawn is restored, continue briefly into gameplay and note whether pace is back to the pre-physics-patch behavior.

Do not evaluate the removed full-function player-physics conversion as active; it is intentionally offline pending narrow-hook redesign.

## RUNTIME RESULT — CRASH FIXED, BUT FULL RECOMPILED PLAYER PHYSICS BREAKS SCRIPTED SPAWN (2026-10-04)

Latest tested runtime:
- revision `15969b7ab692189c416fa3d6919b6b387e55e903`;
- the previous pre-render -> live-cutscene access violation is fixed;
- process exits normally (`exit_code=0`);
- however Spider-Man falls through the intended live-cutscene spawn into the yellow void, triggers the normal falling/death sequence, and reaches Retry/Quit.

Exact A/B evidence against the full `CPlayer::DoPhysics` replacement:

Shared live-cutscene handoff:
- current reconstructed-physics build, frame 675:
  - camera pos `-100413440,-5496832,-28516352`;
  - focus `-78921728,19869696,-166903808`.
- previous retail-physics runtime `21ced52bc9c8e5903fe66cc939e4114804c248fb`, frame 723:
  - the **same** camera pos and focus values.

Frame-900 player comparison:
- retail-physics runtime:
  - body `-78729216,15695872,-155586560`;
  - shadow `-78729216,16089088,-155586560`;
  - shadow normal `0,-4096,0`;
  - player remains correctly grounded and gameplay proceeds.
- reconstructed-physics runtime:
  - body `-79482342,25915794,-164378260`;
  - shadow position remains near the expected ground band at Y `15817598`;
  - body has fallen roughly 10.2 million fixed-point Y units below the known-good grounded height before death.

Conclusion:
- scripted camera/spawn data is correct;
- the level/floor still exists and the shadow path can still find its vicinity;
- the regression is specifically the globally installed reconstructed `CPlayer::DoPhysics`/player-physics runtime ownership;
- two separate live-transition failures (uninitialized local crash, then incorrect grounding after that fix) show the full reconstructed function is not faithful enough to replace retail globally yet.

Strategy change:
- keep the reconstructed physics source and retail byte capture as RE/reference;
- **stop installing `patch_physics()` at runtime**;
- restore retail player physics/collision/spawn behavior immediately;
- keep independently grounded native-60 work such as RotY and Mysterio active;
- reintroduce player 60-Hz corrections later as narrow hooks around proven integration primitives inside retail physics instead of replacing the complete collision/grounding function.

This is not a retreat to a 30-Hz final architecture. It is an isolation step that restores correctness while we move the 60-Hz conversion to smaller retail-preserving seams.

## FIX IMPLEMENTED — ZERO-MOVE LIVE-CUTSCENE COLLISION STATE (2026-10-04)

Source fix:
- `9d05114c1d401daac541023c8b3776bdc0d196fb` — initialize `CPlayer::DoPhysics` local `lineInfo.pItem` to zero before the movement sweep.

Why:
- the failing transition is from a pre-rendered movie into an in-engine cutscene;
- crash occurs after successful movie-surface release, during retail collision/ground-height work;
- reconstructed `DoPhysics` preserved a retail uninitialized-local defect that is unsafe after recompilation;
- a stationary player/live-cutscene spawn can skip the sweep and branch on random stack data;
- observed invalid read target `0xFB4BC000` is consistent with a bogus uninitialized object pointer.

Next validation is intentionally narrow:
1. New Game;
2. select difficulty;
3. let the first Doc Ock pre-render play;
4. verify the following **in-game cutscene actually begins and proceeds**;
5. only if that transition succeeds, continue into ordinary gameplay and resume native-60 pace/physics checks.

If it still crashes at the same transition, preserve the new log; next step is symbolizing the proxy fault address against `Release/spider.map` and then testing the physics hook in isolation, not changing movie playback.

## RUNTIME REGRESSION — PRE-RENDER -> LIVE CUTSCENE HANDOFF CRASH (2026-10-04)

Latest tested runtime:
- revision `f7bd7cbe5075554f61043cd3a6dfc8ed0bda0261`;
- build succeeds under the matching VC6 toolchain;
- process exits with `0xC0000005` access violation.

Exact user repro:
1. New Game;
2. select difficulty;
3. first pre-rendered Doc Ock/Octavius cutscene plays;
4. that movie ends successfully;
5. the next sequence should be an **in-game/live-engine cutscene**;
6. that live sequence never appears: transition freezes, then crashes.

Crash-tail facts:
- `movie_surface_release reason=stop_call ... remaining_refs=0` is logged immediately before the fault, so the movie surface itself shuts down;
- first access violation:
  - proxy address `0x1003E338`;
  - read target `0xFB4BC000`;
  - `EDI=0xFB4BC000`;
- stack retains retail return addresses including:
  - `0x004526EC`, inside retail `M3dColij_InitLineInfo @ 0x004524C0`;
  - `0x004E68EB`, inside retail `Utils_GetGroundHeight @ 0x004E6840`;
- this places the failure after movie shutdown, during live-world collision/ground-placement work.

Primary root-cause candidate found in the newly reconstructed `CPlayer::DoPhysics`:
- local `SLineInfo lineInfo` intentionally preserved a retail uninitialized-`pItem` defect;
- if the player's computed movement length is zero, the movement sweep loop is skipped;
- `lineInfo.pItem` is then tested without ever passing through `M3dColij_InitLineInfo`;
- in the original executable this depended on accidental stack contents;
- in the recompiled proxy the stack layout is different, so a stationary player at live-cutscene startup can turn that garbage pointer into an invalid collision object dereference;
- this exactly matches the user clarification that the failing next sequence is an in-engine cutscene where Spider-Man can begin stationary.

Immediate fix:
- explicitly initialize the local `lineInfo.pItem = 0` before the zero-movement branch can inspect it;
- this removes undefined behavior in the recompiled replacement without changing any successful collision path;
- keep the native-60 physics and RotY conversions otherwise unchanged.

## BUILD FIX — FIRST NATIVE-60 BATCH NOW COMPILES PAST REPORTED SOURCE ERRORS (2026-10-04)

User build attempt:
- requested revision: `33c3016d0a68202cdd0204d661da3bbe964b55a7`;
- build stopped in VC6 before runtime, so **no gameplay result exists yet for the native-60 physics + RotY batch**.

Reported compile blockers:
1. `main.cpp`: `patch_physics` undeclared;
2. `physics.cpp`: invalid `CVector +/- int` expressions in the crawling side-probe reconstruction;
3. `spidey.cpp`: stale player collision aliases (`field_B74/B84/B8C`, `field_C18/C1C/C28/C30`) removed when the corrected `CPlayer` line-info layout was installed.

Fixes committed:
- `8043e1efb2929a4a1d2e93b523e6c0a841674262` — include `physics.h` in `main.cpp`, exposing `patch_physics(void)`;
- `a2c022f2a67f8ec513ab095a1d10420ecbf5560b` — rewrite crawling left/right probe endpoints component-wise using `field_C78/C7C/C80`;
- `8bce08e50fb5679fd12db2f54b44421c1a6041d7` — migrate stale collision aliases to the corrected structures:
  - old `field_B74` -> `mLineInfo.pItem`;
  - old `field_B84` -> `mLineInfo.Normal`;
  - old `field_B8C[3]` -> `mLineInfo.pFace[3]`;
  - old `field_C18` -> `mLineInfo2.pItem`;
  - old `field_C1C` -> `mLineInfo2.Position`;
  - old `field_C28` -> `mLineInfo2.Normal`;
  - old `field_C30[3]` -> `mLineInfo2.pFace[3]`.

Why those mappings are exact:
- `mLineInfo` starts at `CPlayer+0xB0C`; the old aliases correspond to offsets `+0x68/+0x78/+0x80`, exactly `SLineInfo::pItem/Normal/pFace`;
- `mLineInfo2` starts at `CPlayer+0xBB0`; old `C18/C1C/C28/C30` likewise map to `+0x68/+0x6C/+0x78/+0x80`, exactly `pItem/Position/Normal/pFace`;
- the three contiguous `C78/C7C/C80` fields are used as the x/y/z components of the crawling side direction elsewhere in retail/decompiled player code.

Static post-fix audit:
- `physics.h` declares `patch_physics(void)`;
- `main.cpp` includes `physics.h` and calls `patch_physics()`;
- no invalid `hookPos +/- scalar` expressions remain;
- all six crawl side endpoint components now use the proper C78/C7C/C80 axis component;
- none of the seven stale player collision aliases remain in `spidey.cpp`;
- the corrected `mLineInfo` / `mLineInfo2` members are present.

The warnings shown by VC6 are unchanged pre-existing signed/unsigned and long-to-short warnings and are not the failure cause.

### Exact next action

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` again.

Expected revision:
- `8bce08e50fb5679fd12db2f54b44421c1a6041d7` or a newer documentation-only descendant.

If compilation succeeds, proceed with the already-defined combined native-60 runtime test:
- normal traversal/jump/fall;
- crawling if convenient;
- enemy turning/AI behavior;
- subjective overall game pace at 60;
- quick manual-aim regression check;
- ordinary scripted transition if convenient.

If VC6 exposes another compile error, return the build output and fix that before runtime testing.

## IMPLEMENTED — FIRST COMBINED NATIVE-60 PHYSICS + AI BATCH READY (2026-10-04)

Current `dev` source frontier:
- `6b56677c383331da12035109df59b165ae69a066` — player physics: split the retail two-vblank movement/force quantum for native 60 Hz;
- `0741d1c82c81edbe0a94edcc180e93c7c87d7ca5` — declare native-60 RotY replacement;
- `48b1a9d503f1646f508720eae0174a57617e9ed5` — implement native-60 `CAIProc_RotY::Execute`;
- `e3109c1ee2e1f252ade18e72279c2d14b7e75ba8` — install RotY patch and retire the completed startup capture calls.

RotY implementation details:
- retail constructor remains untouched, preserving `field_20 = numFrames` and `field_24 = requestedAngle / numFrames`;
- retail Execute at `0x00401110` is replaced through the normal `PATCH_PUSH_RET` patch path;
- inherited `field_C`, unused by retail RotY and zero-initialized by `CClass::operator new`, stores one-bit half-step phase;
- `field_80 == 1`: consume one 1/60 half of the authored angular step;
- `field_80 == 2`: consume both halves and reproduce exactly one retail step/countdown;
- odd/negative integer angular steps are split so both halves sum exactly to retail `field_24`;
- `field_20` decrements only after the second half, preserving authored real-time duration;
- completion flags and turn-direction flags preserve retail semantics;
- elapsed `field_80 > 2` consumes additional half-ticks for real-time catch-up.

Player physics in the same test build:
- normal physics and crawling physics use the committed half-step acceleration/friction integrator when `field_80 == 1`;
- displacement and fall motion are split for a 1/60 tick;
- two 60-Hz velocity updates mathematically reproduce one original 30-Hz damping/acceleration update apart from fixed-point integer rounding;
- swinging physics is reconstructed but is not force-scaled by this batch because its movement comes from the swinger object's transform/path rather than the same raw `mVel += mAcc; mVel %= mFric` path.

Venom:
- no Venom-specific 0.5 multiplier was added;
- its captured synthesized-input timers/path progression already use `field_80`;
- patch shared primitives first, then retest the real chase later.

Generic `CAIProc::Wait`:
- still a likely raw-frame candidate in reconstructed source;
- retail symbol map shows no standalone named Wait routine because it is inlined;
- leave it unchanged until exact constructor/countdown semantics can be grounded from a relevant retail call site/capture.

Capture cleanup:
- active startup dumps for RotY, Venom synthesized input, player physics and crawling physics are removed;
- `SpideyLogHighFpsRetailBytes` helper remains available for future exact probes but has no current calls.

Static source validation after the batch:
- `ai.h`: balanced braces/parentheses;
- `ai.cpp`: balanced braces/parentheses;
- `main.cpp`: balanced braces/parentheses;
- RotY declaration, implementation, patch site and `patch_ai()` install are all present;
- completed startup probe labels have zero active occurrences in `main.cpp`.
- GitHub Actions returned no visible workflow runs/statuses for this branch, so **do not claim CI-green**; the next local VC6 build is the compile/runtime validation.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` from the local project.

Expected source revision:
- `e3109c1ee2e1f252ade18e72279c2d14b7e75ba8` or a newer documentation-only descendant.

Test one meaningful gameplay pass:
1. ordinary walking/running/jumping and general traversal;
2. compare subjective game speed against the previous 60-FPS build — movement should no longer feel globally accelerated if player physics was the dominant cause;
3. crawl on walls/ceilings if convenient, because crawling has the same new half-step integration;
4. normal enemy/AI behavior and turning for a few encounters, watching for rotations that are too slow/fast or state hangs;
5. quick manual-aim regression sanity only; do not retune it;
6. progress through an ordinary scripted/cutscene transition if convenient;
7. no hitch reproduction testing is needed.

Return the one consolidated `spidey-decomp.log` plus subjective notes on whether the game's pace now feels correct at 60.

If the local build fails, return the compiler output and patch immediately before any runtime testing.

## RUNTIME RESULT — FOUR NATIVE-60 CAPTURES COMPLETE; MASTER CLOCK CORRECT (2026-10-04)

Latest tested runtime:
- revision `21ced52bc9c8e5903fe66cc939e4114804c248fb`;
- log `spidey-decomp(20261004-220426).log`.

Validated in this runtime:
- Mysterio elapsed-time laser compatibility now installs successfully:
  - vtable slot 0/delete destructor = `0x0045B540`;
  - vtable slot 1/Move = `0x0045BAC0`;
  - `high_fps_compat mysterio_laser=1`.
- all four startup RE captures completed with `valid=1`:
  - `CAIProc_RotY_Block @ 0x00401060`, size `0x120`;
  - `CVenom_SynthesizeAnalogueInput_Block @ 0x004E9B00`, size `0x19A0`;
  - `CPlayer_DoPhysics_Real @ 0x00466CE0`, size `0x1040`;
  - `CPlayer_DoCrawlingPhysics @ 0x00467FD0`, size `0xD70`.

Master-clock result:
- timer source callbacks: 94,352;
- canonical dispatched callbacks / virtual ticks: 5,661;
- source elapsed time: approximately 94.35 seconds;
- dispatched cadence is therefore essentially 60 Hz;
- the user-visible slight speed-up at 60 is **not** the master timer running fast;
- the remaining problem is downstream code consuming a correct 60-Hz update cadence with legacy per-frame assumptions.

### Exact CAIProc_RotY retail finding

The captured block resolves the previously incomplete reconstructed source.

Constructor `CAIProc_RotY::CAIProc_RotY @ 0x00401060`:
- attaches AI type `0x104`;
- stores `field_20 = numFrames`;
- stores `field_24 = requestedAngle / numFrames`.

Execute `CAIProc_RotY::Execute @ 0x00401110`:
- adds `field_24` to the owning baddy's Y angle once per Execute;
- sets the retail turn-direction flag;
- decrements `field_20` by exactly one per Execute;
- completes when the raw frame countdown reaches zero;
- does **not** consume `field_80`.

This is a proven legacy frame-counted AI primitive. At 60 Logic calls/sec it rotates and completes roughly twice as fast as the original two-vblank/30-Hz authored cadence.

Safe native-60 strategy:
- preserve the retail constructor and its integer division exactly;
- use inherited `CAIProc::field_C` as a half-step phase accumulator for RotY only;
- `CClass::operator new` zero-fills allocations, and retail RotY does not otherwise use `field_C`;
- split each original `field_24` angular step into two integer-exact halves whose sum equals the original step, including odd and negative steps;
- decrement `field_20` only after both 1/60 halves have been consumed;
- if `field_80 == 2`, consume both halves in the same call and reproduce the original 30-Hz step exactly;
- if `field_80 == 1`, consume one half for smooth native-60 rotation;
- larger elapsed values consume the corresponding number of canonical half-ticks.

### Player physics result

The captured retail normal/crawling physics proves the same broad two-vblank assumption:
- retail treats `field_80 == 1` and `field_80 == 2` as the same base velocity/displacement quantum in important paths;
- this explains a large part of the subjective 60-FPS speed-up.

The repo has already advanced beyond the tested runtime with a source reconstruction and first native-60 conversion:
- `248dae028e58f48e4c77e5ac7a095364ae6093b4` — reconstruct retail player physics;
- `6b56677c383331da12035109df59b165ae69a066` — split retail player physics step for native 60 Hz;
- `e19e1918ec13abc21523d3c650f6add099ddc255` — correct player physics symbol map.

The current player-physics conversion:
- half-steps acceleration/friction at `field_80 == 1`;
- uses a fixed-point square-root damping split so two 60-Hz velocity updates reproduce one original 30-Hz velocity update apart from integer rounding;
- halves displacement/fall movement for a single 1/60 tick;
- covers normal and crawling physics;
- is implemented/committed but **not runtime-tested yet** by this log.

### Venom synthesized-input result

The capture disproves the simple hypothesis that the known Venom chase failure is caused by raw one-per-call timers inside `CVenom::SynthesizeAnalogueInput`.

Observed timing behavior:
- `field_350 += field_80`;
- command timers subtract `field_80`;
- path/index progression such as `field_394` advances by `field_80`;
- multiple command handlers are already canonical elapsed-tick aware.

Therefore:
- do **not** add a blind 0.5 scaler to Venom's synthesized-input dispatcher;
- the high-FPS chase failure is more likely downstream/upstream of that dispatcher, including shared player physics or other raw AI timing primitives;
- keep the exact Venom capture as reference, but no Venom-specific timing patch is justified yet.

### Immediate implementation frontier

1. land native-60 `CAIProc_RotY::Execute` using the proven half-step scheme;
2. retain the already-committed player-physics 60-Hz conversion;
3. retire the four completed startup byte captures so normal logs stay compact;
4. runtime-test the combined physics + RotY batch;
5. only after that result, continue into additional proven raw frame-counted AI/effect/cutscene primitives.

The 60-Hz requirement remains:
- gameplay simulation must be semantically correct at native 60 Hz;
- state/event counters are not delta-scaled;
- after native 60 is stable, >60 presentation will be decoupled/interpolated without running gameplay faster than 60.

## SOURCE ADVANCE — TRUE PLAYER PHYSICS ADDED TO NEXT NATIVE-60 CAPTURE (2026-10-04)

Latest source:
- `e4df3b156aeec6d81519697f7d9f158603dfc99e` — `timing: capture true player physics for 60hz patching`.

Why this matters:
- the earlier `0x004BFEC0` probe was not the complete authoritative player physics routine;
- the symbol map / call-path audit identified the real lower-level player physics block at `0x00466CE0`;
- crawling has a separate retail path at `CPlayer_DoCrawlingPhysics @ 0x00467FD0`.

The next single startup capture therefore now gathers all unresolved high-value native-60 seams in one run:
- `CAIProc_RotY_Block @ 0x00401060`, size `0x120`;
- `CVenom_SynthesizeAnalogueInput_Block @ 0x004E9B00`, size `0x19A0`;
- `CPlayer_DoPhysics_Real @ 0x00466CE0`, size `0x1040`;
- `CPlayer_DoCrawlingPhysics @ 0x00467FD0`, size `0xD70`.

This is still startup-only logging and does not reintroduce the hitch-causing hot-path diagnostics.

The resulting batch should be sufficient to:
1. prove shared AI rotation/wait frame-count semantics;
2. inspect the known Venom automated-input path;
3. determine whether the user-visible 60-FPS speed-up includes raw per-call player velocity/acceleration/friction integration;
4. compare normal and crawling physics semantics before modifying either;
5. implement a coherent first native-60 timing batch instead of requesting multiple small tests.

Expected runtime revision:
- `e4df3b156aeec6d81519697f7d9f158603dfc99e` or newer documentation-only descendant.

Expected startup completion lines:
- `high_fps_compat mysterio_laser=1 ...`;
- `high_fps_re_bytes_done label=CAIProc_RotY_Block ... valid=1`;
- `high_fps_re_bytes_done label=CVenom_SynthesizeAnalogueInput_Block ... valid=1`;
- `high_fps_re_bytes_done label=CPlayer_DoPhysics_Real ... valid=1`;
- `high_fps_re_bytes_done label=CPlayer_DoCrawlingPhysics ... valid=1`.

## NATIVE 60-HZ RE FRONTIER — PLAYER STATE MACHINE CLEARED; TWO TIMING SEAMS REMAIN (2026-10-04)

Authoritative `dev` source before this documentation checkpoint:
- `026322a195ecb16ef41d3803ff477356504e8713` — `timing: prune resolved startup timing probes`.

Latest tested runtime remains:
- `a4e1105d1f9568a24ca8817847573392a7f8324a`;
- log `spidey-decomp(20261004-213030).log`;
- user confirms the diagnostic/logging cleanup eliminated the recurring hitching;
- user-visible remaining timing defect: gameplay still feels slightly sped up at 60 FPS.

### Locked requirement

The target is **native, semantically correct 60-Hz gameplay simulation**:
- 60 Hz is the minimum gameplay simulation rate;
- game speed, AI, physics, cutscenes and boss fights must match intended real-time behavior at 60;
- do not hide legacy timing assumptions by running gameplay at 20/30 Hz under a 60-Hz renderer;
- after 60-Hz simulation is correct, 120/144/165/240+ presentation will be implemented by render/presentation decoupling and interpolation while gameplay remains capped to canonical 60-Hz advancement.

### Recovered source work after the tested runtime

The repo advanced safely through:
- `6cf830b974316134a8a7813ac1eda42279eacd60` — correct Mysterio laser vtable guard to runtime-proven deleting destructor `0x0045B540`;
- `62f9500c4085a0841f5afbe9f85ee7d68b021e1f` — temporary central AI/player-physics startup capture;
- `54a6812e8fa3c4a793275f65d921de7e6a1f660b` — temporary full player/synthesized-input timing capture;
- `026322a195ecb16ef41d3803ff477356504e8713` — prune already-resolved captures and retain only unresolved timing seams.

Current startup-only capture now contains exactly:
- `CAIProc_RotY_Block @ 0x00401060`, size `0x120`;
- `CVenom_SynthesizeAnalogueInput_Block @ 0x004E9B00`, size `0x19A0`.

No capture runs in a gameplay/render hot path.

### Static conclusions from the completed SpideyAI0 capture

The full `SpideyAI0 @ 0x004B13F0` capture was inspected beyond the initial dispatch mapping.

Important negative result:
- the small number of direct `++` operations found inside the giant player state machine are event/state counters, not continuously advancing gameplay timers;
- examples occur only on camera/state setup, input transitions, switches, or state changes;
- therefore **do not halve, skip, or delta-scale those counters**.

Important positive result:
- player AI already consults `field_80`, `gTimerRelated`, animation state and other elapsed-time-aware state in many branches;
- the missing `CPlayer::AI`, `CPlayer::DoPhysics`, and synthesized-input functions are more valuable timing seams than globally modifying `SpideyAI0`.

This further narrows native-60 work toward shared timing primitives rather than giant state-machine surgery.

### Ob_AI result remains decisive

Recovered `Ob_AI @ 0x00460FC0` proves:
- each active body receives `EveryFrame()`;
- supers receive `UpdateFrame()`;
- virtual `AI()` runs every object dispatch;
- there is no hidden 30-Hz object-AI interleave.

Therefore a raw per-AI-call countdown really does expire faster when Logic frequency increases, whereas code using `field_80` preserves real time.

### Current unresolved reusable AI seam

`CAIProc::Wait` in reconstructed source performs a raw one-per-Execute decrement:
- `field_C = field_C - 1`.

However, do not patch it from the reconstructed source alone yet:
- constructor and rotation semantics around `CAIProc_RotY` must be proven from the exact retail bytes;
- `CAIProc_RotY` currently has incomplete/decompiler-placeholder behavior in the source tree;
- the retained `0x00401060..0x00401180` startup capture exists specifically to settle this before implementing the first shared native-60 AI conversion.

### Current unresolved Venom/cutscene seam

Recovered `CVenom_FollowDirections @ 0x004EB530` is a compact scripted direction/state dispatcher, not a generic physics loop.

It calls into:
- `CVenom_SynthesizeAnalogueInput @ 0x004E9B00`;
- player focus-lock/state helpers;
- trigger/switch helpers;
- level/script state checks.

Because the known Venom chase failure is an automated movement/cutscene problem, the remaining `CVenom_SynthesizeAnalogueInput_Block` capture is the highest-value exact target for identifying the 20/30-Hz-authored path without slowing the whole game.

### Next runtime — capture only, no hitch testing

Run the latest `dev` with `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected source:
- `026322a195ecb16ef41d3803ff477356504e8713` or newer documentation-only descendant.

Expected startup lines:
- `high_fps_compat mysterio_laser=1 ... destructor_found=0x0045B540 ... move_found=0x0045BAC0 ...`;
- `high_fps_re_bytes_done label=CAIProc_RotY_Block ... valid=1`;
- `high_fps_re_bytes_done label=CVenom_SynthesizeAnalogueInput_Block ... valid=1`.

Normal gameplay is sufficient. No hitch reproduction is needed.

After that log:
1. reconstruct/disassemble the two remaining exact retail blocks;
2. implement the first shared native-60 AI timing repair only where semantics are proven;
3. repair the Venom automated-input timing path;
4. continue classifying raw boss/cutscene timers;
5. validate native 60 before beginning >60 render interpolation.

## RUNTIME RESULT — HITCH CLEANUP VALIDATED; 60-HZ TIMING NOW SOLE FRONTIER (2026-10-04)

Tested runtime:
- revision `a4e1105d1f9568a24ca8817847573392a7f8324a`;
- log `spidey-decomp(20261004-213030).log`;
- user confirms the logging cleanup **got rid of the hitches**.

Hitch conclusion:
- the recurring hitch problem was dominated by our own synchronous diagnostic/success logging;
- `98d52ec80b5876db8e347460be307555b905de4b` and `31f80818ab5ee73cacae4b3d952205add448fc86` are now runtime-validated as the practical hitch fix;
- remove hitch diagnosis from the active work queue unless a new, reproducible stall appears;
- web firing was only a marker and requires no hitch-related changes.

60-Hz user-visible result:
- gameplay is still **slightly sped up at 60 FPS**;
- user explicitly requires **60 Hz as the minimum/native simulation target**;
- higher refresh must be achieved by rendering/interpolation above a correct 60-Hz simulation, not by lowering the whole game to 30.

Timer result from this runtime:
- the master virtual-vblank clock remains approximately 60 Hz rather than running fast;
- therefore remaining speed-up comes from subsystems that advance once per Logic/AI call instead of consuming canonical elapsed ticks.

Retail RE capture completed successfully:
- `Ob_AI @ 0x00460FC0` captured completely;
- `CVenom_FollowDirections @ 0x004EB530` captured completely;
- `SpideyAI0 @ 0x004B13F0` captured completely.

New `Ob_AI` conclusion:
- every active object receives `EveryFrame()`;
- supers receive `UpdateFrame()`;
- virtual `AI()` then runs every `Ob_AI` dispatch;
- there is no hidden 30-Hz AI interleave inside `Ob_AI`.

This makes raw per-call timers real 60-Hz acceleration candidates. Confirmed example:
- `CAIProc::Wait` decrements `field_C` by exactly one per Execute call.

Counterexample already correct:
- `CAIProc_MoveTo` uses the owning baddy's `field_80`;
- `CBaddy::RunTimer` subtracts `field_80`;
- body/camera/animation systems widely use `field_80` or absolute `gTimerRelated`.

Mysterio runtime result:
- first elapsed-time laser patch safely declined installation because runtime vtable slot 0 was `0x0045B540`, not the guessed `0x0045B300`;
- slot 1 matched `0x0045BAC0`;
- source correction `6cf830b974316134a8a7813ac1eda42279eacd60` updates the guard to the runtime-proven destructor and removes the no-longer-needed startup byte-dump instrumentation.

Active timing architecture:
1. make gameplay **natively correct at 60 Hz**;
2. preserve every subsystem already consuming canonical elapsed ticks;
3. convert reusable raw per-update timing primitives and proven raw gameplay timers to 60-Hz elapsed-time semantics;
4. keep player input/camera responsiveness at full 60 Hz;
5. once simulation is correct, never advance gameplay more than once per canonical 60-Hz tick;
6. decouple Display/presentation and interpolate for 120/144/165/240+ Hz.

Do not resume shadow work until the 60-Hz timing conversion reaches a stable runtime checkpoint.

## AUTHORITATIVE HIGH-FPS FRONTIER — SOURCE + RE CAPTURE READY (2026-10-04)

Latest tested runtime:
- `091c2345ef1d4c927878d6ec47c2c54efaadf9ce`
- log: `spidey-decomp(20261004-203738).log`
- manual aim validated as perfect-for-now; freeze camera/aim unless regression.

Latest source frontier:
- `98d52ec80b5876db8e347460be307555b905de4b` — disable proven hot-path timing success I/O by default;
- `31f80818ab5ee73cacae4b3d952205add448fc86` — remove renderer hot-path success logging;
- `6cfcd74aaecc72a2e1ac37885a03dc4aad0f52ae` — convert Mysterio laser one-update liveness handshake to elapsed `gTimerRelated` ticks;
- `9f8a62f46d00e437861cd55facf0a6ef91a76b4e` — add startup-only missing-retail byte capture;
- `62c7e71dc32f6cadec9c077dde13ec66d1645a0c` — extend startup capture to the full `SpideyAI0` body in 128-byte chunks;
- `52c9a5cf9c22740839a2fc03d28a013605a5f240` — add `docs/HIGH_FPS_TIMING_AUDIT.md`.

Current source is **implemented/committed but NOT runtime-tested**.

The next runtime has two purposes:
1. validate that removing synchronous success logging materially reduces the recurring hitches;
2. collect the missing retail machine code needed to continue timing RE without guessing.

Expected startup capture:
- `Ob_AI @ 0x00460FC0`, size `0x1A0`;
- `CVenom_FollowDirections @ 0x004EB530`, size `0x160`;
- `SpideyAI0 @ 0x004B13F0`, size `0x73A0`;
- each emitted as `high_fps_re_bytes ... offset=... hex=...`, followed by `high_fps_re_bytes_done ... valid=1`.

Expected Mysterio install line:
- `high_fps_compat mysterio_laser=1 ... move_found=0x0045BAC0 ... grace_ticks=3 ...`.

Static source audit after the capture work:
- braces: 1106 / 1106;
- parentheses: 5146 / 5146;
- brackets: 372 / 372;
- no C++11 `auto` or `nullptr` introduced.

Do **not** globally apply floating-point delta time. Current evidence proves a mixed timing model:
- body/camera/animation/boss paths already consume canonical elapsed `field_80` ticks;
- other AI/effect/cutscene paths still use raw per-update counters;
- event/state counters such as most `dumbAssPad++` transitions are not timers and must remain unscaled.

Current intended architecture:
- first make 60-Hz simulation semantically correct;
- then cap simulation advancement to canonical 60-Hz ticks;
- finally decouple `Display`/presentation for 120/144/240+ Hz and interpolate render state where necessary.

Exact audit and rationale:
- `docs/HIGH_FPS_TIMING_AUDIT.md`.

Exact next action:
- run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
- move through the current level long enough to provoke several of the old recurring hitches;
- web shots may still be used as user-visible hitch markers;
- no extended manual-aim test is needed beyond a quick regression sanity;
- return the single consolidated `spidey-decomp.log`.
- After that log arrives, reconstruct/disassemble `Ob_AI` first, then `CVenom_FollowDirections`, then `SpideyAI0`, and continue directly into Catch Venom/cutscene timing repair.

Do not resume real-shadow work until this timing phase is complete enough to support correct high-refresh gameplay.

## HIGH-FPS IMPLEMENTATION CHECKPOINT — MYSTERIO LIVENESS FIX + 60-HZ SIMULATION POLICY (2026-10-04)

**Status: implemented/committed; NOT runtime-tested yet.**

Authoritative source:
- `6cfcd74aaecc72a2e1ac37885a03dc4aad0f52ae` — `timing: make Mysterio laser liveness elapsed-time based`
- prior hot-path logging cleanup remains:
  - `98d52ec80b5876db8e347460be307555b905de4b`
  - `31f80818ab5ee73cacae4b3d952205add448fc86`

### Mysterio laser repair

Retail RE is now sufficiently exact to patch the known phase-2 high-FPS failure without touching global pacing:

- `CMysterioLaser` constructor at `0x0045B3E0` installs vtable `0x0053BB34`.
- vtable slot 0 is expected deleting destructor `0x0045B300`.
- vtable slot 1 is expected virtual liveness/Move routine `0x0045BAC0`.
- `CMysterioLaser::SetPos @ 0x0045B5E0` writes byte `this+0x44 = 1` after refreshing beam geometry.
- retail `0x0045BAC0` kills the bit if that marker is zero, then clears it every consumer update.

This is a producer/consumer **one-update handshake**, so consumer cadence can invalidate a still-live beam.

The new wrapper patches only vtable slot 1 and is runtime guarded:
- install only when slot 0 is exactly `0x0045B300` and slot 1 is exactly `0x0045BAC0`;
- otherwise leave retail untouched and log the mismatch.

Object layout remains unchanged. The existing marker byte is reused:
- `0` = stale/unrefreshed;
- `1` = fresh marker written by retail SetPos;
- `2..255` = encoded `gTimerRelated` timestamp modulo 254.

On Move:
- fresh `1` becomes a timestamp;
- an already timestamped laser remains alive while elapsed canonical time is <= 3 ticks;
- 3 ticks at the engine's 60-unit/sec clock = 50 ms, which permits the documented 20-Hz authored producer cadence;
- if no new SetPos refresh occurs by the fourth tick, retail `CBit::Die @ 0x00408930` is invoked.

No sidecar allocation, destructor hook, global delta multiplier, or scheduler change is involved.

Expected startup telemetry:
- `high_fps_compat mysterio_laser=1 ... move_found=0x0045BAC0 clock=gTimerRelated_60hz grace_ticks=3 ...`

### Refined high-FPS architecture

The static audit now supports a stronger policy than simply forcing a 30-Hz gameplay loop.

At the retail full-release loop:
- `PlayAway` waits until at least one canonical timer tick has elapsed before the next Logic+Display iteration;
- therefore retail Logic normally cannot run twice with the same `gTimerRelated` value.

At 60 Logic calls/sec:
- `CBody::EveryFrame` naturally produces `field_80=1`;
- movement/camera/boss code that multiplies or loops by `field_80` therefore advances one canonical 1/60-second tick;
- `CSuper::UpdateFrame` advances by `field_80 * mAnimSpeed / 2`, so `field_80=1` already gives a half-sized 60-Hz animation step equivalent in real time to the nominal 30-Hz `field_80=2` step.

The real incompatibilities are the systems that bypass canonical elapsed ticks:
- `CAIProc::Wait` decrements by exactly one per Execute call;
- several effect/bit movers increment `mAge`, decrement lifetime, integrate velocity/gravity, or advance animation exactly once per Move;
- individual player/boss/cutscene counters also contain raw per-call `++/--`;
- frame-to-frame producer/consumer handshakes such as Mysterio laser assume a specific relative call cadence.

**Current target architecture:**
1. keep `Vblanks/gTimerRelated` as the canonical 60-unit real-time clock;
2. make 60-Hz Logic fully correct by converting raw per-call gameplay timers/state/lifetimes to elapsed-tick semantics where needed;
3. preserve the validated 60-Hz camera/input path rather than dropping whole Logic to 30 Hz;
4. once Logic is safe, prevent simulation from running above the canonical 60-Hz tick rate;
5. decouple `Display`/presentation from Logic for 120/144/240+ Hz output;
6. interpolate render transforms between 60-Hz simulation states where visible smoothness requires it;
7. convert known authored 20-Hz sequences individually (Mysterio laser is the first concrete example) instead of applying a global 20/30-FPS compatibility cap.

This avoids the unsafe `CSuper::UpdateFrame` zero-tick fallback (`field_80==0 -> 2`) that would occur if Logic were simply allowed to run >60 Hz.

### Next RE / implementation targets

Continue before asking for a general runtime test:
- audit `Ob_AI @ 0x00460FC0` / bit-list update ordering to identify which raw Move systems execute once per Logic call;
- classify active gameplay `++/--` counters into:
  - intentionally event/count based;
  - should use `field_80`;
  - should use absolute `gTimerRelated`;
- inspect the Venom-chase authored sequence for the known 20-FPS path/cutscene failure;
- compare the Kellogg demo's native 30-FPS pacing mechanism if a clean executable/dump becomes available;
- keep all fixes isolated and runtime guarded rather than globally multiplying velocities by a floating delta.

Do not resume real-shadow work until this timing phase is complete enough for a dedicated runtime test.

## RUNTIME CHECKPOINT — MANUAL AIM VALIDATED; HITCH SOURCES RESOLVED; HIGH-FPS TIMING FRONTIER (2026-10-04)

Tested runtime:
- revision `091c2345ef1d4c927878d6ec47c2c54efaadf9ce`
- log `spidey-decomp(20261004-203738).log`

User-visible result:
- manual aiming is now **perfect for now**;
- same-frame post-camera reticle update is validated;
- 96-unit vertical framing is accepted;
- aimed movement, camera orbit, and firing remain working;
- **freeze manual-aim work unless a future regression appears**.

Hitch result:
- the phase probe successfully separated multiple causes;
- several very large stalls are self-inflicted synchronous diagnostic/logging work, not gameplay or DX11 presentation:
  - frame 3347: 132.884 ms interval, 130.390 ms in `logic_telemetry_us`;
  - frame 3580: 234.599 ms interval, 232.152 ms in `logic_telemetry_us`;
  - frame 3807: 327.935 ms interval, 325.286 ms in `logic_telemetry_us`;
  - frame 4437: 177.980 ms interval, 175.496 ms in `logic_telemetry_us`;
  - additional stalls at frames 1982/2630/2864/3956/4592/8357/8586 are dominated by `record_timing_us`, which includes the synchronous timing-window write.
- genuine non-logging stalls also remain:
  - frame 961: 466.948 ms interval, 462.087 ms in `shadow_end_us`;
  - frame 1681: 384.864 ms interval, 381.378 ms in `shadow_end_us`;
  - frame 7184 and frame 7801 are dominated by unpartitioned presenter remainder (`other_present_us`);
  - frame 7417: 209.395 ms interval, 205.362 ms inside untouched retail gameplay logic `0x00455400`;
  - frame 7657: 168.557 ms interval, 165.241 ms in retail gameplay logic;
  - frame 8977: 327.539 ms interval, 324.503 ms in retail gameplay logic.
- web shots still trail the hitch events and remain a user marker, not the trigger.

New priority requested by user:
- pause real-shadow work;
- fix the engine's high-FPS timing architecture before returning to shadows;
- research and implement a proper render/simulation decoupling rather than merely forcing the old frame loop to 60 Hz.

Current static timing evidence:
- `PCTimer.cpp` maintains an engine clock in nominal **60 vblank units per second**: `field_4 * 60 / 1000`, accumulated into `gTimerVblankRelated`, then advanced through `MyVSync()`.
- `CBody::EveryFrame` derives per-object elapsed ticks in `field_80 = gTimerRelated - field_7C`, clamps it to 6, and seeds first update with `field_80=2`.
- `CSuper::UpdateFrame` advances animation by `field_80 * mAnimSpeed / 2`, with a fallback `field_80=2`.
- those two values strongly support a native nominal gameplay update quantum of **2 vblank ticks = 1/30 second**, while real-time clocks remain 60-unit based.
- some systems are already elapsed-tick aware (movement paths use `field_80`), while others are still update-count based (for example `CAIProc::Wait` decrements by one per Execute call).

Working architecture hypothesis:
1. keep the canonical engine clock in real 60-Hz vblank units;
2. run gameplay simulation on a fixed 30-Hz step (2 vblank ticks);
3. allow rendering/input/presentation at 60 Hz or higher;
4. later add render interpolation between simulation states for truly smooth uncapped output;
5. audit and migrate frame-count timers/state machines that bypass `field_80`;
6. retain a compatibility path for known sequences that historically require 20 FPS until their timing is individually repaired.

Do not implement a blind global variable-delta multiplier. The existing engine already has a partial elapsed-tick model; the correct first task is to identify the authoritative gameplay-loop gate and `gTimerRelated` writer, then decouple simulation from presentation at that boundary.

Exact next RE:
- map/decompile retail gameplay function `0x00455400` and caller at `0x00455A8B`;
- locate every writer of `gTimerRelated`, `Vblanks`, and `TTime`;
- distinguish whole-simulation dispatch from per-subsystem logic before adding a fixed-step gate;
- remove/quiet synchronous timing diagnostics once the required evidence is safely documented.

### Exact retail timing / loop RE completed

Original-function binary dumps and `tools/names.json` now prove:

- `0x00455400 = Logic`.
  - It is the whole gameplay update dispatcher, not a small helper.
  - It increments frame/call counters at `0x005FCCF4` and `0x0060CFA8` once per Logic call.
  - It runs `Pad_Update`, calls `Ob_AI` repeatedly for the player and global object lists, and runs additional gameplay/camera/trigger systems.
- `0x004555A0 = Display`.
- `0x004559D0 = PlayAway`.
  - At `0x00455A66` it snapshots `Vblanks`.
  - At `0x00455A8B` it calls `Logic`.
  - It later calls `Display`.
  - At `0x00455B4E` it compares current `Vblanks` with the snapshot; if unchanged, `0x00455B59` calls `Pause(1)`.
  - Therefore the retail full-release loop deliberately waits for at least one nominal 60-Hz timer tick before starting another Logic+Display iteration.
- `0x00511130 = TimerCallback`.
  - It derives `gTimerMsInterval = configured_ms * 60 / 1000`.
  - It accumulates this into `gTimerVblankRelated`.
  - It repeatedly calls `MyVSync` until integer timer time catches `Vblanks`.
- `0x004E5CF0 = MyVSync`.
  - increments `Vblanks @ 0x006B4CA0`;
  - conditionally increments `gTimerRelated @ 0x006B4CA8`;
  - thus `gTimerRelated` is a canonical real-time 60-unit/sec engine clock, not a render-frame counter.
- `CBody::EveryFrame @ 0x00460ED0` computes object elapsed time as `gTimerRelated - field_7C`, clamps it to 6, and seeds first update with `field_80=2`.
- `CSuper::UpdateFrame @ 0x00460DA0` advances animation using `field_80 * mAnimSpeed / 2`, but if `field_80 == 0` it forcibly substitutes `2`.

Consequences:
- blindly removing `PlayAway`'s wait and calling `Logic` faster than 60 Hz is unsafe: repeated Logic calls can observe zero elapsed timer ticks, while `CSuper::UpdateFrame` converts zero to two ticks and advances animation as if 1/30 second elapsed;
- blindly multiplying movement/physics by a new floating-point delta is also wrong because a significant part of the engine already consumes `field_80`;
- the port is a mixed timing model: elapsed-tick-aware physics/animation/camera code coexists with raw per-Logic-call counters and state machines.

External corroboration:
- PCGamingWiki documents that the full release is capped at 60 but was designed around 30 and breaks above it;
- the Kellogg's demo reportedly contains a native 30-FPS cap absent from the full release;
- specific authored sequences require 20 FPS, including Chase Venom and Mysterio phase 2.

### Mysterio laser high-FPS RE

The known Mysterio >30-FPS failure has a concrete frame-coupled mechanism in retail machine code:

- `CMysterioLaser::SetPos` is the large routine beginning at `0x0045B5E0`.
- Near its end it sets byte `this+0x44 = 1`, marking the laser as refreshed/alive for the current producer update.
- The tiny virtual routine at `0x0045BAC0`:
  - tests `this+0x44`;
  - if zero, calls `CBit::Die @ 0x00408930`;
  - then always clears `this+0x44 = 0`.
- This is a one-update producer/consumer liveness handshake, not elapsed-time logic.
- If laser positioning and bit movement run at different effective cadences, an extra consumer update can kill/recreate or otherwise destabilize the beam even though essentially no real time elapsed.
- This is exactly the class of bug that requires cadence-independent state/timers, not a global velocity multiplier.

High-FPS modernization policy now:
1. preserve `Vblanks/gTimerRelated` as the canonical 60-unit real-time clock;
2. preserve the now-good high-rate camera/input path;
3. convert raw per-call lifetime/wait/state counters to elapsed-tick or fixed-point time semantics subsystem by subsystem;
4. repair frame-to-frame handshakes such as Mysterio laser liveness using real elapsed ticks / grace windows;
5. once gameplay state no longer assumes one Logic call == one authored frame, decouple `Display` from `Logic` and allow refresh-rate/uncapped presentation;
6. add transform interpolation for render rates above simulation cadence where needed;
7. keep temporary compatibility handling for authored 20-FPS sequences until those paths are individually converted.

Do not patch `Pause(1)` out yet. Render uncapping without a corresponding Logic scheduler/interpolator would either speed up frame-coupled systems or only repeat identical simulation states.

## CHAT-LIMIT HANDOFF CHECKPOINT — RETICLE NO-DRAG + HITCH PHASE TEST READY (2026-10-04)

The chat reached its maximum length immediately after the reticle-lag/hitch investigation.

Authoritative live frontier:
- latest **tested** runtime revision: `2ec405d96253df7332d5fe6609729fb4f310b720`
- latest tested log: `spidey-decomp(20261004-200625).log`
- latest source-changing commit: `c286d708b6f6d4a8f2fefef35808d44a6179cdad`
- source immediately before it: `ca2af74d4238b3fe4255a2d8c45cff3766c91bc0`
- reticle source immediately before that: `f3f25d9f3b134f4b7bd8d6a15f5d98ca8f9f3bf1`
- current source status: **implemented, committed, NOT runtime-tested**
- exact next action: run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, test fast manual-aim mouse sweeps plus several hitches, and return one consolidated `spidey-decomp.log`

What is already proven:
- manual-aim movement works;
- actual TPS camera movement works;
- 96-unit vertical manual-aim framing is accepted as much better;
- remaining visible manual-aim issue in the tested build is one-frame reticle drag on fast camera movement;
- direct FireWeb timestamps show the user fires **after** the major hitches, so FireWeb is not the trigger;
- regular 60 Hz cadence is healthy between isolated large stalls.

What is ready for the next runtime:
- post-camera same-frame reticle rebuild after `0x00418458 -> 0x00416B10`, so rendered reticle uses the final current-frame camera state;
- in-memory presenter phase partition for every captured slow frame:
  - `present_work_us`
  - `outside_present_us`
  - `record_timing_us`
  - `transient_us`
  - `shadow_end_us`
  - `draw_probe_us`
  - `present_shadow_us`
  - `other_present_us`

Do **not** resume real-shadow work until the reticle/hitch test is evaluated. Do not tune camera sensitivity or the 96-unit framing unless the next runtime demonstrates a need.

### Additional hitch discriminator added before runtime — `c286d708...`

Static correlation in the latest tested log exposed one important ambiguity worth removing before asking for another test:

- many 300–630 ms slow-present events occur on the frame immediately after the once-per-second gameplay-logic timing wrapper runs;
- this does **not** prove diagnostic logging is the cause, because a stall inside untouched retail logic would produce the same ordering;
- therefore the next runtime now accumulates the work between presenter entries and splits it into:
  - `logic_retail_us` — time inside untouched retail gameplay logic `0x00455400`;
  - `logic_telemetry_us` — time spent writing the once-per-second logic timing telemetry;
  - `logic_calls` — number of wrapped retail logic calls in the interval;
  - `outside_nonlogic_us` — remaining outside-present time after subtracting retail logic + logic telemetry.

Together with the existing presenter phase fields, one hitch can now distinguish:
- DX11 present / replay / transient / draw-probe / presenter remainder;
- retail gameplay logic;
- timing-log I/O;
- or other game/update/render work outside both presenter and the wrapped retail logic.

This is diagnostics only: no camera, reticle, movement, targeting, timer cadence, renderer behavior, or 96-unit framing policy changed.

- Repo: https://github.com/legentus/spidey-decomp
- Branch: `dev`
- Google Drive project root: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx
- Google Drive Handoff folder ID: `1l-4gLh-jftGT1aNrP73wD8n3IScqQcvO`
- Google Drive Logs folder ID: `1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s`


## MANDATORY INTERRUPTION-SAFE GIT / DOCUMENTATION PROTOCOL

This project is being developed through long interactive ChatGPT sessions where an input/output stream can fail without warning. **Do not allow meaningful work to exist only in chat text or an uncommitted working tree.**

During every continuation session:

1. **Fetch and verify live `dev` before doing RE or code work.**
   - Treat GitHub `dev` as the authoritative recoverable state.
   - Compare the live HEAD with the revision named in this file and `docs/NEW_CHAT_HANDOFF.md`.
   - If chat text and the repository disagree, inspect the newest commit/doc checkpoint before redoing work.

2. **Live-update `docs/CURRENT_STATUS.md` throughout the session.**
   - Record each confirmed RE finding, implementation decision, runtime result, failure, and exact next step as soon as it becomes useful.
   - Do not wait until the end of the session.
   - Mark hypotheses separately from runtime-validated facts.

3. **Commit and push meaningful source changes immediately.**
   - Commit after each coherent fix/RE implementation, not after a giant batch.
   - Prefer small descriptive commits that can be independently recovered.
   - Never leave a useful source change uncommitted while moving on to another subsystem.

4. **Checkpoint documentation frequently even when source has not changed.**
   - If several minutes of RE produce an important address, call graph, data-structure conclusion, or ruled-out hypothesis, write it to `CURRENT_STATUS.md` and commit/push the documentation.
   - The goal is that a stream failure loses at most a small amount of exploratory reasoning, never the project frontier.

5. **Before asking the user for a runtime test, checkpoint everything first.**
   - Source changes committed/pushed.
   - `CURRENT_STATUS.md` updated with expected telemetry and exact test steps.
   - `NEW_CHAT_HANDOFF.md` updated when the recovery frontier materially changes.
   - State the exact `dev` HEAD / source commit that the test must contain.

6. **After a runtime result, document it before starting the next implementation.**
   - Record the tested session revision, filename, visible user result, decisive log evidence, what was proven/disproven, and the next code target.
   - Then commit/push that checkpoint.

7. **If a stream interruption occurs:**
   - do not reconstruct from memory first;
   - fetch live `dev`;
   - read the top of `CURRENT_STATUS.md` and `NEW_CHAT_HANDOFF.md`;
   - inspect the newest commits;
   - determine what survived;
   - continue from the newest committed frontier without repeating completed RE.

This protocol is a project requirement. The user explicitly wants the repo and documentation updated continually so interruptions do not erase progress.


## IMPLEMENTATION CHECKPOINT — SAME-FRAME RETICLE + SLOW-FRAME PHASE PARTITION READY (2026-10-04)

**Status: source implemented; NOT runtime-tested yet.**

Source commits:
- `f3f25d9f3b134f4b7bd8d6a15f5d98ca8f9f3bf1` — `gameplay: update manual reticle after camera orbit`
- `ca2af74d4238b3fe4255a2d8c45cff3766c91bc0` — `timing: partition slow frames by presenter phase`

### Reticle responsiveness fix

The validated 96-unit vertical framing remains unchanged.

The early SpideyAI0 / SetupLookaroundCamera path still writes a compatibility reticle point, but it is no longer authoritative for the rendered frame.

At the existing manual camera framing hook:
- `0x00418458 -> 0x00416B10`;
- framed focus is applied;
- untouched retail postprocess runs;
- **after retail returns**, `SpideyModernAimApplyCameraPoint` runs again;
- `field_DC0` is rebuilt from the **final current-frame camera position -> framed focus**;
- `field_DE4=1`.

This removes the one-frame stale camera sample that made the cursor trail behind fast look input.

Expected framing telemetry now includes:
- `post_camera_reticle=1`
- `reticle_point=x,y,z`

Success criterion:
- fast mouse flicks keep reticle locked/snappy with camera motion;
- no visible chase/drag;
- framing remains 96 units above Spider-Man;
- aimed locomotion and web direction remain intact.

### Hitch phase partition

The direct FireWeb timestamps proved web firing happens **after** the large stalls, so FireWeb is not treated as a cause.

The existing slow-event probe is extended without adding per-frame file I/O.

For each completed DXPOLY flip wrapper, the following phase durations are retained in memory:
- `record_timing_us` — frame-safe timing bookkeeping;
- `transient_us` — pending transient-surface processing;
- `shadow_end_us` — DX11 shadow replay/end-frame;
- `draw_probe_us` — retail draw-probe flush/install;
- `present_shadow_us` — actual DX11 PresentShadow call;
- `other_present_us` — remainder of wrapper work;
- `present_work_us` — total time spent inside previous DXPOLY flip wrapper;
- `outside_present_us` — slow inter-present interval minus previous wrapper work.

These are copied into the existing in-memory slow-event record and only emitted on the normal once-per-second timing flush.

New slow-event format:
- `present_work_us=...`
- `outside_present_us=...`
- `record_timing_us=...`
- `transient_us=...`
- `shadow_end_us=...`
- `draw_probe_us=...`
- `present_shadow_us=...`
- `other_present_us=...`

Interpretation for the next hitch:
- huge `present_shadow_us` => actual visible DX11 present/GPU wait;
- huge `shadow_end_us` => DX11 replay/end-frame path;
- huge `transient_us` => transient surface mirror/lock path;
- huge `draw_probe_us` => retail capture/probe flush;
- huge `other_present_us` => another presenter-side operation;
- huge `outside_present_us` with small `present_work_us` => stall occurs in game/update/render work before entering presenter, not in presentation itself.

### Static audit

After `ca2af74d...`:
- braces: 1093 / 1093;
- parentheses: 5103 / 5103;
- brackets: 363 / 363;
- no C++11 `auto`, lambdas, or `nullptr`;
- timing phase probe uses only QPC and in-memory counters on the hot path;
- slow-event file output remains once per timing window;
- no existing camera sensitivity, framing constant, locomotion mask, hip-fire or web-target code changed;
- no GitHub CI status is currently attached.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`c286d708...` or newer**.

Test:
1. manual aim and make several very fast left/right/up/down mouse sweeps;
2. judge specifically whether the reticle still trails or now stays glued to the current camera direction;
3. verify 96-unit placement still feels right;
4. move while aiming + turning + firing;
5. play until several hitches occur;
6. optional: continue firing once after each hitch as a sanity marker, though FireWeb is already ruled out as the trigger;
7. quick hip-fire sanity.

Return one consolidated log.


## RUNTIME CHECKPOINT — VERTICAL AIM FRAMING GOOD; RETICLE HAS ONE-FRAME DRAG; WEB FIRE NOT HITCH CAUSE (2026-10-04)

Tested revision:
- `2ec405d96253df7332d5fe6609729fb4f310b720`

Runtime log:
- `spidey-decomp(20261004-200625).log`

### User-visible result

Manual aim is now substantially better:
- 96-unit vertical framing is accepted as much better;
- aimed movement remains working;
- actual camera orbit remains working;
- remaining manual-aim issue is **cursor/reticle drag when looking quickly**.

User wants the reticle to remain snappy and have no perceptible lag/drag behind camera motion.

### Reticle lag root cause

Current frame order proves the reticle point is generated too early.

Observed ordering:
1. `SpideyAI0` calls modern SetupLookaroundCamera wrapper;
2. `modern_manual_aim event=reticle` computes `field_DC0` from the camera position/focus currently stored;
3. later in the frame `CCamera::AI` runs CM_Normal and changes the actual orbit position/yaw;
4. `modern_manual_camera event=framing` then applies the 96-unit elevated focus;
5. rendering uses the updated current-frame camera with a `field_DC0` that was derived from the earlier camera state.

This is effectively a one-frame stale aim ray. At slow movement it is hard to notice; on a fast flick it looks like the cursor trails/has drag.

Do not change camera sensitivity or the 96-unit framing to solve this.

### Exact reticle fix target

Keep the early SetupLookaroundCamera wrapper as a compatibility fallback, but make the post-camera framing seam authoritative.

At `0x00418458 -> 0x00416B10` wrapper:
- apply elevated framed focus;
- call untouched retail postprocess;
- **after retail returns, recompute `player->field_DC0` from the final current-frame camera position -> framed focus**;
- set `field_DE4=1`;
- this overwrites the stale pre-camera ray before RenderLookaroundReticle consumes it.

Expected result:
- reticle direction and current rendered camera are from the same frame;
- no one-frame chase/drag on fast mouse movement;
- movement/framing/web logic unchanged.

### Hitch correlation — FireWeb follows the stalls

Direct FireWeb timestamps finally make the user test meaningful.

Representative late-run examples:
- stall frame `3170`: ~316 ms; next FireWeb frame `3182`;
- stall frame `3290`: ~600 ms; next FireWeb frame `3299`;
- stall frame `3961`: ~484 ms; next FireWeb frame `3970`;
- stall frame `4093`: ~513 ms; next FireWeb frame `4125`;
- stall frame `4484`: ~361 ms; next FireWeb frame `4499`;
- final gameplay stall frame `4704`: ~541 ms; no later gameplay shot before frontend transition.

Therefore:
- FireWeb is **not the trigger** for these hitches;
- user is successfully firing in reaction to them.

The regular 60 Hz cadence is otherwise healthy between stalls.

### Next hitch diagnostic

Current `slow_present_events` measures time between calls into the DXPOLY flip wrapper. It proves a frame was late but cannot locate where the time was spent.

Add in-memory phase telemetry only:
- total time inside `SpideyDiagDXPOLYFlip`;
- transient surface processing;
- DX11 shadow replay/end-frame;
- retail draw-probe flush;
- DX11 PresentShadow;
- implied outside-present/game time = interval between wrapper entries minus prior wrapper duration.

Store phase data alongside existing slow events and only flush on the once-per-second timing line.

Do not add per-frame file writes.


## RUNTIME CHECKPOINT — TPS CAMERA MOVES; RETICLE/CAMERA FRAMING STILL THROUGH SPIDER-MAN (2026-10-04)

Tested revision:
- `0ed86176ca66461e96693cba29a2a32c2fc8bde8`

Runtime log:
- `spidey-decomp(20261004-195535).log`

### User-visible result

The unified TPS camera now responds to manual-aim look input, but the reticle/cursor is again effectively stuck through Spider-Man / near screen center.

User requirement is now precise:
- Spider-Man should sit below the manual-aim reticle;
- reticle should be slightly above Spider-Man;
- mouse/right-stick should still rotate the actual camera freely;
- aimed locomotion must remain working;
- web direction must continue following the reticle/view.

### Runtime proof that the unified orbit itself works

During manual aim, `modern_manual_camera event=tps_orbit` and `modern_camera event=update` show the real orbit changing substantially.

Representative manual-aim yaw sequence:
- `3804 -> 17 -> 494 -> 724 -> 1380 -> 1828 -> 1427 -> 3106 ...`

The corresponding camera positions change around Spider-Man as expected.

Therefore:
- do **not** revert the unified orbit-camera work;
- the remaining problem is framing / aim anchor placement.

Aimed locomotion remains successful in the same run:
- movement mask engages;
- `enter_suppressed` continues climbing;
- body world position changes by large amounts;
- no recurrence of the old stand/run vibration loop.

Freeze locomotion.

### Root cause of reticle overlap

The unified-TPS source intentionally removed the separate manual free-view and made the reticle use the raw mode-3 camera center ray.

Retail CM_Normal focuses the camera essentially on Spider-Man's body position.

Therefore the reticle now projects through the same body-centered focus:
- camera movement works;
- aim ray changes with camera;
- but Spider-Man occupies the same central screen region as the reticle.

This is a framing issue, not an input or targeting failure.

---

## IMPLEMENTATION CHECKPOINT — VERTICAL TPS AIM FRAMING READY (2026-10-04)

**Status: source implemented; NOT runtime-tested yet.**

Source commits:
- `e68c8de3021c20119c47b3d15bc0372bd907883f` — `gameplay: frame manual aim above Spider-Man`
- `b37788fe417a26d39e00389ac87853e9cc9a08c2` — `compat: fix framed reticle telemetry scope`

### New framing model

Manual aim keeps the validated unified mode-3 orbit camera.

A single framed focus is now defined as:
- Spider-Man body position;
- Y shifted **96 game units upward**;
- world +Y is downward, so implementation subtracts `96 * 4096` from body Y.

No horizontal shoulder bias is added yet.

The new camera seam is:
- `0x00418458 -> shared camera postprocess/orientation @ 0x00416B10`

Order:
1. untouched `CM_Normal` calculates the real orbit camera position/collision;
2. manual-aim wrapper changes only `camera->field_144` to the framed point above Spider-Man;
3. untouched retail shared postprocess builds the final camera orientation toward that framed target;
4. untouched `LoadIntoMikeCamera` publishes it.

This means:
- actual camera remains freely movable;
- Spider-Man should render below the camera-center reticle;
- reticle should appear slightly above Spider-Man;
- camera orientation and reticle/web ray use the **same framed target**.

### Reticle/web ray

`SpideyModernAimApplyCameraPoint` no longer extends the body-centered raw focus.

It directly constructs the ray:
- camera position -> framed focus above Spider-Man;
- extends that ray x8 into `player->field_DC0`;
- `RenderLookaroundReticle` therefore projects the same direction the visible camera is looking.

This avoids both previous bad designs:
- independent free cursor with mostly fixed camera;
- unified camera with reticle directly through Spider-Man.

### Expected telemetry

Startup:
- `modern_manual_camera_framing installed=1 address=0x00418458 retail=0x00416B10 ...`
- `modern_camera_install ... manual_focus=framed_above_body manual_framing=1 ... framing_up_units=96 ...`
- manual aim install should report `reticle_source=framed_tps_camera_ray`.

Runtime:
- `modern_manual_camera event=framing ... focus=... body=... framing_up_units=96 ...`
- reticle lines now include:
  - `camera_focus=...`
  - `framed_focus=...`
  - `framing_up_units=96`

Expected relation:
- framed-focus Y should equal body Y minus `393216`;
- camera orbit yaw/position must continue changing during aim;
- body position must continue changing during aimed locomotion.

### Static audit

After `b37788fe...`:
- braces: 1090 / 1090;
- parentheses: 5077 / 5077;
- brackets: 363 / 363;
- one framed-focus helper;
- one manual camera postprocess wrapper;
- target call site `0x00418458` is patched through the existing target-validated direct-call helper;
- final `0x0041865F -> LoadIntoMikeCamera` remains untouched;
- no `nullptr`, lambdas, or C++11 `auto` introduced;
- no GitHub CI status is attached yet.

### Exact next runtime

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`b37788fe...` or newer**.

Test:
1. hold manual aim and verify Spider-Man now sits visibly below the reticle;
2. move mouse left/right/up/down and make sure the actual camera still orbits/tilts;
3. judge whether the 96-unit vertical framing is:
   - too low,
   - about right,
   - too high;
4. aim at several world points and fire webs;
5. move W/A/S/D + diagonals while aiming and rotating camera;
6. release aim and verify normal camera returns cleanly;
7. quick hip-fire sanity.

If framing is correct but Spider-Man still obscures the ray horizontally, next refinement is a small camera-right shoulder offset. Do not reintroduce independent manual free-view.


## IMPLEMENTATION CHECKPOINT — UNIFIED TPS MANUAL-AIM CAMERA READY (2026-10-04)

**Status: implemented and committed; NOT runtime-tested yet.**

Source commit:
- `44dbfec52838fdadbd5b82556b15ced208297220` — `gameplay: unify manual aim with TPS orbit camera`

### What changed

The previous manual-aim camera architecture has been removed from the active path.

Old behavior:
- mouse/right-stick changed `gSpideyManualAimViewYawOffset/PitchOffset`;
- mode-3 orbit yaw stayed separate;
- post-CM code rewrote `field_144`;
- final `LoadIntoMikeCamera` call was wrapped to publish a second orientation.

New behavior:
- manual aim and ordinary gameplay share the same `gSpideyModernCameraYaw`;
- mouse/right-stick updates the real mode-3 orbit yaw while manual aim is held;
- vertical look updates the same `gSpideyModernCameraYDistance`;
- untouched `CM_Normal` owns camera position, collision and focus;
- the post-CM free-view `field_144` rewrite is removed;
- the final `0x0041865F -> LoadIntoMikeCamera` call is left retail/untouched;
- manual reticle/web code continues to use the resulting camera ray;
- working manual-aim locomotion re-entry guard is unchanged.

This is intentionally a first unified-TPS pass:
- no shoulder offset yet;
- no camera-side movement remapping yet;
- no change to hip-fire targeting;
- no change to web-fire behavior;
- no change to locomotion mask.

### New telemetry

Startup should report:
- `modern_camera_install ... manual_aim_free_view=0 manual_tps_unified=1 ... manual_publish=0 ...`

During manual aim + camera input:
- `modern_manual_camera event=tps_orbit input_seq=... yaw=... retail_yaw=... y_dist=... camera_pos=... camera_focus=... reticle_policy=camera_ray`
- ordinary `modern_camera event=update` also includes `manual_tps_unified=1`.

Success criterion:
- while aim is held, mouse/right-stick visibly orbits the actual gameplay camera;
- Spider-Man continues moving normally at the same time;
- reticle remains coherent with the camera view;
- webs still travel where the reticle/view aims.

If camera movement works but reticle lands on/too close to Spider-Man, the next refinement is a proper over-the-shoulder camera/reticle offset. Do **not** reintroduce the old independent free-view accumulator.

### Static source audit

After `44dbfec...`:
- braces balanced: 1078 / 1078;
- parentheses balanced: 5048 / 5048;
- brackets balanced: 363 / 363;
- no C++11 `auto`, `nullptr`, or lambdas introduced;
- no active post-CM manual free-view rewrite remains;
- no direct patch of `0x0041865F` remains;
- one explicit `event=tps_orbit` telemetry path is present;
- GitHub combined status currently has no attached CI result; do not call this CI-green.

### Timer result from tested 759e58dc runtime

The elapsed-time 1 ms dispatcher materially fixed the previous timer drift.

Observed in `spidey-decomp(20261004-194202).log`:
- long settled gameplay windows repeatedly report `count=60 hz=60.000`;
- `vblank_one=60`, `vblank_multi=0` in those windows;
- `timer_callbacks` and `timer_virtual_ticks` remain matched;
- `timer_source_callbacks` and `timer_source_ms` remain matched over long runs;
- the previous persistent 58–59 Hz one-shot re-arm drift is gone.

There are still isolated larger stalls/transitions. Do not continue timer surgery unless the user reports a persistent hitch after the next camera test.

### Exact next runtime

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source:
- **`44dbfec5...` or newer**.

Test:
1. hold manual aim and rotate mouse left/right/up/down;
2. confirm the actual camera orbits/tilts like ordinary modern camera control;
3. keep aim held and move W/A/S/D + diagonals while rotating camera;
4. fire webs while aim + movement + camera rotation are simultaneous;
5. check whether reticle stays useful or ends up centered on/obscured by Spider-Man;
6. release aim and verify seamless return to normal camera;
7. quick hip-fire sanity;
8. report whether any periodic hitch is still perceptible.

Return one consolidated log.


## RUNTIME CHECKPOINT — AIMED LOCOMOTION FIXED; MANUAL CAMERA STILL NOT TPS-STYLE (2026-10-04)

Tested revision:
- `759e58dc4e27f06cb7678a1a06e4ed3743463328`

Runtime log:
- `spidey-decomp(20261004-194202).log`

### User-visible result

**Success: movement while manual aiming now works.**

The user can move Spider-Man around while manual aim remains active. The prior stand/run vibration/reset loop is gone.

**Remaining issue: the actual gameplay camera still does not move the way a modern third-person shooter camera should while manual aiming.**

Desired behavior is now explicit:
- manual aim remains held;
- mouse/right stick rotates/orbits the actual third-person gameplay camera;
- Spider-Man can keep moving simultaneously;
- reticle remains valid relative to that camera;
- web direction continues to follow the reticle/camera aim rather than Spider-Man's facing.

### Movement proof

The re-entry guard is doing exactly what it was intended to do.

Representative runtime sequence:
- aim locomotion mask engages with `actual_aim_state=0`;
- `CheckForwards` reaches run state;
- `enter_suppressed` then rises rapidly while `raw_reclear=0`;
- later samples show Spider-Man at substantially different world positions;
- releasing movement restores the real aim flag cleanly.

Examples observed in the runtime:
- mask starts around movement call 102;
- later `enter_suppressed=267` with body position already changed;
- subsequent aimed movement sessions reach `enter_suppressed=343`, `377`, `416`, etc.;
- body position and velocity both change during/after aimed movement.

**Freeze this locomotion implementation unless a regression is demonstrated.**

### Why the camera still fails conceptually

The final publish hook does execute:
- `modern_manual_camera event=publish` appears repeatedly;
- manual aim angles change significantly while mouse input is moved.

But the current architecture is the wrong model for the user's requested behavior.

Current code has two camera spaces during manual aim:
1. the ordinary mode-3 orbit camera keeps its own `gSpideyModernCameraYaw` / vertical orbit;
2. manual aim diverts mouse/right-stick deltas into a separate `gSpideyManualAimViewYawOffset/PitchOffset`;
3. that secondary free-view changes `camera->field_144` after CM_Normal;
4. a final publish shim tries to orient the visible transform toward the free-view target.

Runtime confirms this split:
- `modern_manual_camera event=update` changes view yaw/pitch and focus;
- ordinary `modern_camera event=update` keeps the underlying mode-3 orbit yaw separate;
- user still experiences a fixed/non-TPS camera.

This should not be extended further.

### New target architecture — unified third-person shooter camera

Manual aim should use the **same mode-3 orbit camera** as ordinary modern camera movement.

While manual aim is active:
- mouse/right stick updates `gSpideyModernCameraYaw` and `gSpideyModernCameraYDistance` exactly like normal mode-3 camera control;
- CM_Normal remains responsible for camera position/collision/orbit;
- no independent manual-view yaw/pitch accumulator should fight the orbit camera;
- no manual post-CM focus rewrite should own the visible view;
- no final `LoadIntoMikeCamera` quaternion override should be required;
- manual reticle/web direction should be derived from the resulting camera ray.

This matches a modern TPS control model:
- one view orientation;
- one orbit camera;
- movement can happen underneath it;
- reticle/web aim is camera-relative.

First implementation should keep this narrow:
- do not add shoulder offsets yet;
- do not alter the now-working locomotion mask;
- do not change hip-fire targeting;
- do not resume shadow work.


## IMPLEMENTATION CHECKPOINT — FINAL MANUAL CAMERA PUBLISH + AIM RE-ENTRY GUARD + ELAPSED-TIME 60 HZ DISPATCH READY (2026-10-04)

**Status: source implemented and committed; NOT runtime-tested yet.**

Source commits after tested revision `93d63347505da81a769c1d58ba62361c4650f4b5`:
- `71d6bf609bca7ba9cacc860aa008db03c302da74` — `gameplay: publish free aim camera and guard aim reentry`
- `b4d961a4a349bf397a45b5025aaeba353a3f479f` — `timing: remove one-shot drift and timestamp web fire`
- `26a03d05afe21e82517bee43400129c1190176b5` — `timing: anchor 60hz dispatcher to elapsed milliseconds`

### Manual-aim visible camera

The previous build already changed the free-aim world focus, but only after `CM_Normal`. That correctly moved the reticle/web ray while leaving the final visible camera quaternion unchanged.

New implementation:
- patches only `CCamera::AI` final direct publish call:
  - `0x0041865F -> CCamera::LoadIntoMikeCamera @ 0x00416A20`;
- only while effective manual aim is active in ordinary mode 3:
  1. save retail `camera->field_214` and `field_23A`;
  2. derive aim angles from `camera->mPos -> camera->field_144` using retail `Utils_CalcAim`;
  3. construct the rotation with retail `M3dMaths_RotMatrixYXZ`;
  4. convert it with retail `MToQ`;
  5. temporarily place that quaternion in `field_214`;
  6. call untouched retail `LoadIntoMikeCamera`, which publishes the visible `gMikeCamera[0].Transform`;
  7. restore the internal retail quaternion/heading immediately afterward.
- camera position, collision, interpolation and all non-mode-3/scripted modes remain retail-owned.

Expected startup marker:
- `modern_camera_install ... manual_publish=1 manual_publish_call=0x0041865F retail_publish=0x00416A20`

Expected runtime marker:
- `modern_manual_camera event=publish ... aim_angles=... pos=... focus=... published_heading=...`

Success criterion:
- while manual aim is held, moving mouse/right stick visibly rotates the gameplay view;
- reticle/web direction stays on the same visible camera ray.

### Manual-aim locomotion re-entry guard

The prior runtime showed the raw `field_8EA` value reasserting to 1 while the locomotion sidecar remained active, followed by stand/run oscillation and zero body delta.

New implementation:
- adds `SpideyModernAimEnterLookaroundMode`;
- scans the known SpideyAI0/player-AI range `0x004B0000..0x004B9000` for direct calls targeting `EnterLookaroundMode @ 0x004C3580`;
- each exact direct call is patched through the existing target-validated direct-call helper;
- normal initial manual-aim entry still calls untouched retail;
- if the modern locomotion sidecar is already active, redundant retail re-entry is suppressed;
- if retail has reasserted raw `field_8EA=1` while the mask is active, the wrapper clears it back to 0 instead of allowing another aim-state reinitialization;
- CheckForwards also has a defensive raw-flag re-clear at entry while the same mask is active.

New movement telemetry:
- `enter_retail=<count>`
- `enter_suppressed=<count>`
- `raw_reclear=<count>`

Expected startup marker:
- `modern_manual_aim_install ... enter_reentry_calls=<nonzero expected> enter_target=0x004C3580 ...`

Success criterion:
- while aim + movement are held:
  - `locomotion_mask=1`;
  - `actual_aim_state=0` stays stable through movement;
  - `enter_suppressed` rises if held-aim retail code attempts to re-enter;
  - Spider-Man body position actually changes / `body_delta != 0`;
  - no repeated stand/run vibration.

### Timer pacing — remove one-shot re-arm drift

The first phased timer runtime materially removed the old 24-frame ~32 ms beat, but settled gameplay commonly ran at only 58–59 Hz.

Root cause targeted:
- chained 16/17 ms one-shot timers were re-armed from inside their callback;
- callback/re-arm scheduler latency accumulated into a slower delivery clock.

New source:
- one real WinMM periodic timer runs at 1 ms;
- it does **not** call retail TimerCallback every millisecond;
- it checks the next absolute 60 Hz deadline:
  - `floor(n * 1000 / 60) + 1`;
- when that deadline is reached, it invokes untouched retail TimerCallback once with the exact 16/17 ms interval in `STimerInfo.field_4`;
- the heartbeat is anchored to absolute elapsed WinMM `timeGetTime`, not counted callbacks;
- if a 1 ms heartbeat is late or skipped, the missing callback does not permanently slow the delivery phase;
- fallback is `GetTickCount` only if `timeGetTime` cannot be resolved;
- existing atomic `timeKillEvent`-before-`timeSetEvent` fail-closed install remains.

Expected startup:
- `timer_pacing_install ... time_get_time=0x... policy=periodic_1ms_source_dispatch_16_17ms_60hz ...`
- `timer_pacing event=intercept ... source_period_ms=1 ... source_clock=timeGetTime ...`

Expected steady timing:
- `timer_source_callbacks` grows at roughly 1000/sec;
- `timer_source_ms` follows real elapsed milliseconds;
- dispatched `timer_callbacks`, logic and ordinary gameplay presents should return near 60/sec;
- the old exact 24-frame 31–33 ms cadence should remain absent.

### Direct FireWeb hitch timestamp

The previous `check_web_shot_calls` counter never moved despite actual user web shots, so it is no longer used as the primary manual correlation signal.

New implementation:
- scans the main EXE direct-call range for calls to `CPlayer::FireWeb @ 0x004C5DD0`;
- each exact direct call is wrapped;
- wrapper increments:
  - `fire_web_calls`;
  - `last_fire_frame`;
- untouched retail FireWeb executes immediately afterward.

Slow-frame diagnostics:
- capture threshold is lowered from 25 ms to **18 ms**;
- capture is gameplay-only, in-memory, and still flushes only with the existing timing window;
- each event now includes:
  - `fire_web_calls`;
  - `last_fire_frame`;
  - existing web-target/check-web counters.

Expected install:
- `timing_install ... fire_web_hooks=<nonzero expected> ... slow_threshold_us=18000`

Expected event:
- `[TIMING] slow_present_events threshold_us=18000 ... fire_web_calls=... last_fire_frame=...`

### Static audit

After `26a03d05...`:
- braces balanced: 1084 / 1084;
- parentheses balanced: 5080 / 5080;
- brackets balanced: 363 / 363;
- no `nullptr` or lambdas introduced;
- only "auto" occurrences are existing comment text, not C++11 `auto`;
- old one-shot interval helper removed;
- final-camera retail helper signatures verified against headers:
  - `Utils_CalcAim(CSVector*, CVector*, CVector*)`;
  - `M3dMaths_RotMatrixYXZ(SVECTOR*, MATRIX*)`;
  - `MToQ(MATRIX const&, CQuat&)`.
- no GitHub CI/status result is attached yet; do not describe this source as CI-green.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Required source revision:
- **`26a03d05...` or newer**.

In one session:

1. **Manual camera**
   - hold manual aim;
   - move mouse left/right/up/down through a clear range;
   - verify the actual visible camera now rotates, not only the cursor;
   - verify cursor/web direction remains aligned with the view.

2. **Movement while aiming**
   - hold aim + W/A/S/D individually and diagonals;
   - verify Spider-Man actually translates rather than vibrating in place;
   - rotate the camera while moving;
   - fire while aim + move + camera are all active.

3. **Aim release**
   - release aim while moving;
   - verify clean return to normal movement/camera;
   - quick pause/unpause / ordinary camera transition sanity check.

4. **Hitch**
   - play long enough to encounter several perceived small hitches;
   - firing a web near each visible hitch is useful again because FireWeb is now timestamped directly;
   - report whether the hitch feels gone, reduced, unchanged, or worse.

5. **Hip-fire**
   - quick sanity only; do not retest extensively unless a regression appears.

Return one consolidated `spidey-decomp.log`.

Do not resume real-shadow work until this runtime is evaluated.


## RUNTIME CHECKPOINT — MANUAL AIM RAY WORKS; FINAL CAMERA TRANSFORM + AIM RE-ENTRY RESET IDENTIFIED (2026-10-04)

Tested revision:
- `93d63347505da81a769c1d58ba62361c4650f4b5`

Runtime logs:
- `spidey-decomp(20261004-190816).log` — manual aim / firing / hitch correlation
- `spidey-decomp(20261004-191012).log` — dedicated movement-while-aiming test

### User-visible result

Manual aim:
- much better than the prior build;
- reticle/cursor now moves around;
- webbing mostly follows the aimed direction and appears reasonably accurate;
- however the **visible gameplay camera itself does not rotate while manual aim is held**.

Movement while aiming:
- still does not translate Spider-Man;
- visually Spider-Man "vibrates" / repeatedly tries to move and is reset back to the same position.

Hitch:
- user still observes a small hitch;
- near the end of the first run, the user tried to fire a web at each visible hitch (one known mistimed shot).

### Manual camera evidence — final publish seam is the missing layer

The new free-aim state is alive:
- `modern_manual_camera event=update` shows changing `view_yaw`, `view_pitch`, offsets, and changing world-space `focus`;
- manual web direction follows that changed focus well enough to be visibly useful.

Therefore the problem is no longer mouse/right-stick input or the free-aim world ray.

Canonical `CCamera::AI @ 0x00417CB0` retained binary was re-scanned:
- `0x00418414 -> CM_Normal @ 0x00418E00`;
- `0x00418458 -> shared postprocess/collision/orientation @ 0x00416B10`;
- `0x0041865F -> CCamera::LoadIntoMikeCamera @ 0x00416A20`.

`LoadIntoMikeCamera` publishes `camera->field_214` through `QToM` into `gMikeCamera[0].Transform`.

Interpretation:
- the current manual-aim code changes `field_144` after CM_Normal;
- that is sufficient for the reticle/web ray;
- but the visible transform quaternion has already been produced from the old Spider-Man-centered orientation;
- the correct next seam is the final `LoadIntoMikeCamera` call, not another change to `field_144` or the web ray.

Planned implementation:
- patch only direct call `0x0041865F -> 0x00416A20`;
- during effective manual aim, build a temporary camera quaternion from `camera->mPos -> camera->field_144` using the retail `Utils_CalcAim -> M3dMaths_RotMatrixYXZ -> MToQ` convention;
- call untouched retail `LoadIntoMikeCamera`;
- restore the internal retail quaternion afterward so normal mode-3 collision/interpolation state is not contaminated;
- all non-manual camera modes remain untouched.

### Movement evidence — mask is engaged, then retail reasserts aim

Dedicated movement log proves:
- locomotion mask engages: `locomotion_mask=1`;
- first masked samples show `actual_aim_state=0`;
- `CheckForwards` can enter `state=0x10`, `anim=1`, `result=1`;
- immediately afterward, while the sidecar still reports `locomotion_mask=1`, `actual_aim_state` is back to `1`;
- subsequent samples oscillate between stand `0x1/anim=0` and run `0x10/anim=1`;
- `body_delta=0,0,0` throughout.

This exactly matches the user's visible vibration/reset.

Canonical `EnterLookaroundMode @ 0x004C3580` retained binary was inspected and contains an explicit:
- `mov byte ptr [esi+0x8EA], 1`.

Strong current hypothesis:
- hiding `field_8EA` makes untouched retail held-aim logic think manual aim is inactive;
- retail calls `EnterLookaroundMode` again;
- that reasserts `field_8EA=1` and reinitializes the standing aim state;
- movement then starts again on the next pass, creating the vibration loop.

Next implementation:
- do **not** globally fight/reclear `field_8EA` after every write;
- intercept direct SpideyAI0 calls to `EnterLookaroundMode`;
- if the modern effective-aim sidecar is already active, suppress redundant retail re-entry and keep the raw locomotion-facing aim flag masked;
- initial manual-aim entry still calls untouched retail.

### Timer / hitch result

The phased timer is definitely active:
- `timer_pacing_install set_event=1 kill_event=1 ... atomic_cleanup=1`;
- retail 16 ms request is intercepted by the chained 16/17 ms source.

Most importantly, the old stable **24-frame / ~0.4 s sequence of 31–33 ms gameplay frames is no longer present in settled gameplay**.

Both new logs contain long ordinary gameplay windows with:
- 58–59 presents/sec;
- zero `over25ms` / `over30ms`;
- max present intervals around 18–19 ms.

So the old 62.5-vs-60 beat appears materially fixed.

There is still one transition-era `timer_unexpected_delta=1` event with a multi-vblank jump, and the user still sees a smaller hitch. Do not declare frame pacing fully solved yet.

The old `check_web_shot_calls` correlation remains zero even when the user intentionally fires near hitches, so it is not a reliable fire timestamp.

Next hitch diagnostic:
- record the actual retail **web action / fire event** directly;
- lower the small-hitch capture threshold below the old 25 ms diagnostic cutoff so sub-25 ms graph blips can be correlated;
- keep this buffered/in-memory and flush only with the existing timing window.

### Frozen validated behavior

Do not regress:
- hip-fire camera targeting;
- manual aim world ray / web direction;
- existing mode-3 orbit camera outside manual aim;
- phased timer behavior that removed the old 24-frame ~32 ms cadence.


## BUILD CHECKPOINT — VC6 TIMER-HOOK TYPE ERRORS FIXED (2026-10-04)

The first attempt to test the new free-aim / locomotion / timer build did **not** reach runtime.

User build:
- requested revision: `9e3679f7b5dc79d78f3acb2b4f55fc8f6645c75d`
- matching compiler: VC6 `cl.exe`
- failure occurred compiling `main.cpp`
- game was never launched, so **none of the new runtime behavior has been tested yet**.

Exact VC6 errors:
- `main.cpp(12585): C2110 cannot add two pointers`
- `main.cpp(12607): C2440 cannot convert unsigned long to unsigned long *`
- `main.cpp(12857): C2664 InterlockedExchange volatile long * -> long *`
- same InterlockedExchange qualifier error at lines 12879 and 12932 in that revision.

Root cause:
- the older VC6 Platform SDK types parts of `IMAGE_THUNK_DATA` as pointer members where modern headers expose raw integer RVA/function values;
- VC6's `InterlockedExchange` prototype also takes `LONG*` rather than accepting the volatile-qualified pointer used by newer SDKs.

Fix commits:
- `9b7e6b32767adc4f941c0ef1a31331ae2c4885ca` — `compat: fix VC6 timer hook thunk types`
  - copies the raw 32-bit `AddressOfData` thunk value into a `DWORD` before RVA arithmetic;
  - copies the function pointer bits in/out of `FirstThunk` with `memcpy` instead of header-version-dependent union assignment;
  - applies the VC6-safe cast to the first InterlockedExchange site.
- `54185f2192320881a3c37d59d1e2b39b8c4ec18c` — `compat: finish VC6 interlocked timer casts`
  - applies the same cast to the remaining two InterlockedExchange sites.

Behavioral intent is unchanged:
- no timer cadence algorithm was changed;
- no manual-aim camera code was changed;
- no locomotion mask logic was changed.

Static source sanity after the compatibility edits:
- braces balanced;
- parentheses balanced;
- brackets balanced;
- all three timer-active InterlockedExchange calls now use the VC6-compatible cast.

### Exact next step

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` again.

The build must include **`54185f21...` or newer**.

If compilation succeeds, perform the same combined runtime test already documented below:
- free manual aim;
- movement while aiming;
- aim + move + camera + fire;
- clean aim release / pause / camera-mode regression;
- residual ~0.4 s hitch check;
- quick hip-fire sanity;
- upload the new single consolidated `spidey-decomp.log`.

If VC6 reports a new compile error, send the console output before doing any gameplay testing.


## IMPLEMENTATION CHECKPOINT — FREE MANUAL-AIM VIEW + LOCOMOTION MASK + 60 HZ TIMER PHASE FIX READY (2026-10-04)

**Status: implemented and committed, NOT runtime-tested yet.**

Source commit chain after the tested `8958de0676dda897b5c8dc346493276d4c5ffffd` runtime:
- `edf6dcc169efce90f9655f8038eeda9337278902` — `timing: replace 16ms beat with phased 60hz timer`
- `af820d29ebf2344bf087860a4287df0c6307c85d` — `gameplay: decouple manual aim view and locomotion state`
- `a50f64b3afb22b55c30d508619e2d09691542f38` — `timing: harden phased timer install fail-closed`
- `c7c20392e10c354c4b292810af4feb8dd229e54a` — `gameplay: harden manual aim ownership transitions`

### Recovery note

A stream interruption occurred immediately after the first aim implementation was written. Recovery from live GitHub proved the source commit had already survived:
- `af820d29...` was one commit ahead of the known timer checkpoint;
- no aim implementation had to be reconstructed from chat memory;
- subsequent hardening was applied on top of that exact committed source.

### Timer pacing implementation

The residual ~0.4 s micro-hitch matched the retail 16 ms / 60 Hz beat exactly in the prior runtime.

The new timer path:
- leaves retail `TimerCallback`, `gTimerVblankRelated`, pause state, and `MyVSync` intact;
- intercepts only the main EXE's retail 16 ms periodic WinMM timer request;
- replaces the fixed 16 ms (62.5 Hz) source with chained one-shot 16/17 ms deadlines aligned to 60 Hz;
- updates retail `STimerInfo.field_4` with the actual scheduled interval before invoking the untouched callback;
- records callback/vblank behavior in the existing once-per-second timing telemetry;
- falls back to untouched retail timing if the hook cannot be installed safely.

Safety hardening:
- `timeKillEvent` is hooked **before** `timeSetEvent`;
- `timeSetEvent` is not intercepted at all unless cleanup interception already succeeded;
- bound imports without an `OriginalFirstThunk` fail closed instead of interpreting resolved function pointers as import-name RVAs;
- the timer can never deliberately return its synthetic timer ID without owning the matching cleanup path.

Expected markers:
- `timer_pacing_install ... install_order=kill_then_set atomic_cleanup=1 ...`
- `timer_pacing event=intercept ... policy=chained_oneshot_60hz_deadline_plus_1ms ...`
- `timing_present ... timer_active=1 timer_callbacks=... timer_virtual_ticks=... timer_unexpected_delta=... timer_last_interval_ms=...`

Success criteria:
- the previous stable ~24-frame / ~0.4 s sequence of 31–33 ms presents disappears or is materially reduced;
- `timer_unexpected_delta=0`;
- gameplay Logic remains approximately 60 Hz and game speed remains correct.

### Manual aim free-view implementation

The previous runtime proved the camera ray/web direction itself was correct but `camera.field_144` remained Spider-Man's body position.

New behavior:
- ordinary mode-3 retail camera still generates the camera position and collision response;
- on entering manual aim, a separate view yaw/pitch is seeded from the exact current `camera.field_144 - camera.mPos` direction;
- mouse/right-stick intent changes the **manual view direction** instead of continuing to force the final focus back onto Spider-Man;
- after retail mode-3 position generation, `camera.field_144` is republished as a forward world-space focus point from that independent view yaw/pitch;
- the already-validated `field_DC0` path then follows that new camera ray automatically, so web direction and reticle aim share one source.

Current bounded free-look offsets:
- yaw: ±768 engine angle units from entry direction;
- pitch: ±512 engine angle units from entry direction;
- final pitch additionally clamps to a safe ±900 engine units.

Expected markers:
- `modern_manual_camera event=acquire ... base_yaw=... base_pitch=...`
- `modern_manual_camera event=update ... view_yaw=... view_pitch=... offset=... focus=...`
- camera install line includes `manual_aim_free_view=1`.

The runtime log from the prior test also confirms `G_MECHLIST @ 0x006A9038` head is the live Spider-Man actor (`region name=spidey` and body position matches the camera focus), so using that head to identify the current player in the camera wrapper is grounded by runtime evidence.

### Manual-aim locomotion implementation

Prior runtime:
- CheckForwards received movement;
- run state `0x10` / run animation could begin;
- the following frame was forced back to stand;
- position/velocity never committed;
- no wall/ceiling/input/collision blocker was present.

New behavior:
- while **manual aim + movement** are both held, retail `field_8EA` is masked to 0 across the ordinary locomotion state machine;
- the saved real aim state is retained in an out-of-band effective-aim sidecar;
- the modern camera/reticle wrappers use effective aim, so they remain logically in manual aim while locomotion sees a normal movable player;
- the held aim-control byte is still hidden only around retail CheckForwards, preserving the already-proven earlier gate fix;
- when aim or movement is released, the real `field_8EA` value is restored;
- present-time validation is the fail-safe;
- player ownership or camera-mode changes now force immediate restoration and release the manual free-view sidecar.

Expected movement markers now include:
- `aim_state=<effective>`
- `actual_aim_state=<retail field_8EA>`
- `locomotion_mask=1`
- `mask_count=...`
- `restore_count=...`

### Exact combined runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` and confirm the loaded revision is **`c7c20392...` or newer**.

In one gameplay session:

1. **Manual aim camera**
   - enter manual aim;
   - sweep mouse left/right/up/down through a useful range;
   - verify the view can now aim away from Spider-Man instead of remaining locked onto his body;
   - verify the reticle/web direction stays aligned with where the camera looks;
   - note whether the current yaw/pitch limits feel too restrictive.

2. **Manual aim + locomotion**
   - while still aiming, hold W/A/S/D individually and diagonals;
   - verify Spider-Man actually translates rather than only entering a run pose;
   - rotate the aim camera while moving;
   - fire webs while moving and confirm manual aim remains active.

3. **Aim exit / camera ownership regression**
   - release manual aim while moving;
   - verify the camera/reticle exits cleanly and movement continues normally;
   - quick pause/unpause;
   - if convenient, trigger any ordinary camera-mode transition and verify no stuck aim state.

4. **Hitch / timer**
   - play long enough to cover many old 0.4 s hitch intervals;
   - report whether the tiny regular frametime blip is gone, reduced, unchanged, or worse;
   - no need to fire webs as hitch timestamps this time because the cadence source is now directly instrumented.

5. **Hip-fire sanity**
   - only a quick regression check; do not spend time re-testing the already validated selector.

Return the single consolidated `spidey-decomp.log`.

Do not resume shadow-map implementation until this combined test is evaluated.


## RUNTIME CHECKPOINT — CAMERA RAY VALIDATED; AIM PIVOT + LOCOMOTION RESET + 24-FRAME HITCH IDENTIFIED (2026-10-04)

User runtime:
- `spidey-decomp(20261004-182002).log`
- tested revision: `8958de0676dda897b5c8dc346493276d4c5ffffd`

### User-visible result

Manual aim:
- camera/reticle behavior is noticeably better than the prior inverted/off-screen version;
- webs now consistently travel in the direction the user can aim;
- however the camera still stays centered on Spider-Man instead of behaving as a proper free/manual aiming view;
- Spider-Man still cannot move while manual aim is held.

Hitching:
- the small recurring frametime blip remains;
- near the end, the user again attempted to fire a web whenever the hitch was visible.

### Manual-aim evidence

The unmodified visible camera ray is now validated:
- manual aim `field_DC0` is collinear with `camera.field_144 - camera.mPos`;
- user confirms webs travel along the intended aim direction;
- do not reintroduce X/Y reflection or change the hip-fire selector.

The remaining camera problem is now upstream:
- during manual aim, `camera.field_144` remains exactly the Spider-Man body position;
- the mode-3 camera therefore continues looking/orbiting around Spider-Man as its focus anchor;
- proper manual aim needs a separate camera/view aim target after retail mode-3 position generation, not another reticle-ray sign change.

Locomotion telemetry is decisive:
- normal standing samples: `state=0x1 anim=0 collision=0x2 aim_state=1 wall=0 ceiling=0 ignore_input=0 ground_grace=4`;
- movement input is received correctly;
- CheckForwards can transition to `state=0x10 anim=1 result=1`;
- reticle samples can still observe that movement state later in the frame;
- on a subsequent frame the player is forced back to `state=0x1 anim=0`;
- `body_delta=0,0,0` and `body_vel=0,0,0` throughout;
- no wall/ceiling/ignore-input/collision blocker is present.

Interpretation:
- this is a later manual-aim state reset, not an input or collision failure;
- the next locomotion implementation should mask `field_8EA` across the normal movement portion of SpideyAI0 instead of only removing gates inside CheckForwards.

### Hitch evidence — hypothesis promoted to runtime-confirmed cadence match

The new exact slow-frame buffer shows a stable sequence of ~31-33 ms frames spaced almost perfectly every **24 presented frames**.

Representative late-game sequence:
- 9484
- 9508
- 9532
- 9556
- 9580
- 9604
- 9628
- 9652
- 9676
- 9700

That is 24 frames / 60 Hz = **0.4 seconds**.

This exactly matches the already-documented retail timer beat:
- retail requests a 16 ms multimedia timer = 62.5 callbacks/sec;
- engine virtual-vblank target is 60 Hz;
- difference = 2.5 Hz;
- beat period = 1 / 2.5 = 0.4 s;
- equivalent = one cadence correction every 24 60-Hz frames.

The web-shot correlation counter remained zero and therefore did not capture the user's fire-button timestamp. More importantly, the slow frames continue on the same 24-frame phase regardless of web-target call changes, so web targeting is not the cause.

This is now strong runtime confirmation that the residual constant hitch is the legacy **16 ms multimedia timer / 60 Hz virtual-vblank mismatch**, not Renderer11 and not diagnostic logging.

### Next implementation targets

1. **Modern timer/vblank pacing**
   - replace only the game's retail 16 ms periodic multimedia-timer source with a 60 Hz high-resolution cadence;
   - preserve the retail `MyVSync` work rather than directly incrementing only the Vblanks integer;
   - fail closed to retail timing if the timer callback / MyVSync target cannot be resolved safely;
   - log install/resolution and keep existing cadence telemetry for validation.

2. **Manual aim camera**
   - keep retail mode-3 camera position/collision generation;
   - during manual aim, decouple final look target from Spider-Man and maintain an independent mouse/right-stick aim yaw/pitch;
   - publish a forward aim focus after retail mode-3 returns so downstream orientation uses the free aim target;
   - existing field_DC0/web direction should automatically follow that new camera focus.

3. **Manual-aim movement**
   - mask `field_8EA` across the locomotion section beginning at the wrapped CheckForwards call;
   - restore the aim state at a safe later boundary;
   - retain a fail-safe restoration at end-of-frame so an early player-AI exit cannot leave manual aim disabled.


## IMPLEMENTATION CHECKPOINT — CAMERA-RAY MANUAL AIM + LOW-OVERHEAD HITCH CORRELATION (2026-10-04)

Latest gameplay source commit:
- `303232155bf7bc61235aa18a883d6a9b89f4cc7a` — `gameplay: align manual aim ray and add hitch diagnostics`
- **UNTESTED at runtime.** Do not describe the manual-aim behavior below as validated until the user runs the new build.

### What changed

1. **Manual aim now uses the exact visible camera ray already validated by hip-fire.**
   - Removed the disproven third-pass X/Y reflection.
   - `field_DC0` is now:
     - X = `camera.mPos.x + (camera.field_144.x - camera.mPos.x) * 8`
     - Y = `camera.mPos.y + (camera.field_144.y - camera.mPos.y) * 8`
     - Z = `camera.mPos.z + (camera.field_144.z - camera.mPos.z) * 8`
   - This keeps the aim point collinear with `camera.field_144 - camera.mPos`, exactly matching the camera-ray convention that the now-working hip-fire selector uses.
   - Expected effect: remove the artificial inversion/off-screen drift caused by reflecting X/Y while leaving Z forward.
   - If the legacy GTE reticle projection still does not land exactly on camera center, the next fix should replace/bypass only the reticle visual projection rather than changing the world ray again.

2. **Manual-movement logging is now throttled instead of writing every successful call.**
   - Previous condition `if (first calls || result)` caused a file open/close for effectively every successful CheckForwards evaluation while WASD was held.
   - New condition logs:
     - first six calls;
     - axes/state/result transitions;
     - one periodic sample every 60 movement calls.
   - Expanded state includes body position/velocity, animation, collision flags, `field_8EA`, wall/ceiling state, ignore-input timer, and ground-grace timer.
   - This deliberately reduces the chance that diagnostic logging itself creates or amplifies the periodic frametime spike.

3. **Reticle telemetry now measures actual body translation.**
   - Adds `body_delta=x,y,z` between sampled reticle calls.
   - During held movement it samples every 30 reticle calls, plus the normal first/periodic samples.
   - This will show whether the post-CheckForwards path ever commits even a small amount of position movement.

4. **Exact slow-present events are captured in memory and published once per timing window.**
   - Any present interval over 25 ms records:
     - present frame;
     - interval in microseconds;
     - total camera-web-target calls;
     - total `check_web_shot` calls.
   - No file open/write is added to the hot path.
   - The existing once-per-second timing flush now emits:
     - `slow_event_count`
     - `slow_event_stored`
     - one `[TIMING] slow_present_events ...` line containing the buffered events.
   - This directly supports the user's deliberate "fire a web when the hitch happens" timing method.

5. **Hip-fire targeting logic was not changed.**
   - The user reports it is now essentially correct.
   - The runtime log contains successful `source=modern_camera_scan` acquisitions.
   - Keep this path frozen unless a future regression is demonstrated.

### Movement status

This commit does **not** claim the remaining manual-aim locomotion freeze is fixed.

The last runtime proves:
- CheckForwards receives full WASD axes;
- CheckForwards can return success;
- state/animation can transition;
- but later samples still show unchanged `mPos` and zero `mVel`.

Therefore the next log must identify which post-CheckForwards state/collision/input condition prevents translation. Prefer a narrow scoped state mask around that exact locomotion gate rather than globally clearing `field_8EA`.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` and verify the loaded runtime contains `30323215...` or a newer commit.

Test one gameplay session:

1. **Manual reticle**
   - enter manual aim;
   - move camera left/right/up/down;
   - report whether the reticle now follows naturally and whether it ever leaves the screen;
   - specifically note whether it is centered on the camera/look direction or merely moving in the correct direction.

2. **Manual-aim movement**
   - while aim is held, hold W, A, S, and D individually for roughly two seconds each;
   - then try diagonal movement;
   - report whether Spider-Man actually translates, only twists/animates, or remains fully still.

3. **Aim + movement + firing**
   - try moving while rotating camera and firing;
   - no special hip-fire stress test is needed beyond a quick regression check because hip-fire is already provisionally solved.

4. **Hitch correlation**
   - play long enough to observe several of the small periodic frametime blips;
   - continue the useful method of firing a web on/near each observed hitch when practical;
   - return the single consolidated `spidey-decomp.log`.

Expected new markers:
- `modern_manual_aim event=movement ... body_pos=... body_vel=... collision=... aim_state=... wall=... ceiling=... ignore_input=... ground_grace=...`
- `modern_manual_aim event=reticle ... body_delta=... collision=...`
- `timing_present ... slow_event_count=... slow_event_stored=...`
- `[TIMING] slow_present_events frame=... interval_us=... web_calls=... check_web_shot_calls=...`

Do not resume real-shadow implementation until this runtime is evaluated.


## RUNTIME CHECKPOINT — THIRD PASS TESTED; HIP-FIRE VALIDATED, MANUAL AIM STILL BROKEN (2026-10-04)

User runtime log:
- `spidey-decomp(20261004-175613).log`
- runtime-reported revision: `b779d80f11cd94704c4acccb95cdd8ac1dcd70ec`
- IMPORTANT: GitHub `dev` is still `ea74466184ba3727858fa13988f3d356a349adcc`. The `b779d80f...` runtime revision is not present on recoverable GitHub `dev`, so treat it as a local/runtime build identity rather than a committed recovery frontier.

### User-visible result

- Hip-fire aiming is now described by the user as "pretty much perfect and fixed".
- Manual aim remains unusable:
  - reticle/cursor direction is still inverted;
  - Spider-Man cannot translate while aim is held;
  - the reticle can drift off-screen while the camera moves;
  - desired behavior is a camera-locked reticle that looks exactly where the visible camera looks.
- The small periodic frametime blip returned. The user intentionally attempted a web shot on each visible graph blip near the end of the run to provide a timing marker.

### Decisive log evidence

Manual aim third-pass hooks are installed and retail SetupLookaroundCamera is bypassed:
- `modern_manual_aim_install ... retail_setup=0` behavior is active.
- `CheckForwards` receives full movement axes and repeatedly returns success while aim is held, including state `0x10`.
- Despite that, later reticle telemetry keeps `body_pos` unchanged and `body_vel=0,0,0`; therefore the remaining locomotion block is after the wrapped CheckForwards evaluation.
- The current third-pass reticle point reflects X/Y around the camera origin but keeps Z forward. That point is not collinear with the visible camera ray and is now disproven by runtime behavior.

Hip-fire is now positively validated:
- `source=modern_camera_scan` appears with a real target and high camera-ray centeredness score.
- This uses the unmodified visible ray `camera.field_144 - camera.mPos`.
- Do not change the hip-fire selector while fixing manual aim.

Frametime telemetry:
- gameplay windows repeatedly show 2–3 present intervals over 30 ms per second with maxima around 31–33 ms.
- normal gameplay windows usually show single-vblank progression rather than multi-vblank skips.
- current logging still opens/closes `spidey-decomp.log` for each telemetry line, so new diagnostics must avoid adding per-frame file I/O.

### Next implementation

1. Manual reticle:
   - remove the X/Y reflection and use the exact same unmodified camera ray already proven by hip-fire;
   - keep the reticle world point on that ray;
   - if legacy projection still prevents exact camera-center lock, bypass/replace the visual projection in a follow-up rather than re-inverting the world ray.

2. Locomotion:
   - instrument the post-CheckForwards movement state more precisely;
   - prefer a narrow temporary `field_8EA` locomotion-state mask across the movement portion of SpideyAI0, with guaranteed restoration before lookaround/fire/render state is consumed;
   - do not globally clear manual-aim state.

3. Hitch telemetry:
   - capture exact slow-present frames and deltas in memory;
   - publish them in the existing once-per-second timing line;
   - correlate against web-shot/check-web-shot call counts without adding hot-path file writes.

4. Logging:
   - stop logging every successful manual-movement call;
   - log transitions/periodic samples instead so the diagnostic build itself is less likely to create the hitch being investigated.


## LIVE FRONTIER — MANUAL AIM THIRD PASS IMPLEMENTED; RETAIL TARGET LIST FIXED (2026-10-04)

Latest tested runtime remains:
- `7a6af671ec671d7f61a1003b296f59d655b39dcd`
- log `spidey-decomp(20261004-093356).log`

That log proves:
- the movement wrapper receives full WASD axes (`±127`);
- `CheckForwards` can return `1` while manual aim is held;
- state repeatedly becomes `0x10` / sometimes `0x00400000`, matching the user's visible “tries to move / twists but stays stuck” behavior;
- retail SetupLookaroundCamera was still running every frame and the user confirmed WASD still moved the old reticle;
- the user confirmed the reticle response was reversed on both X and Y.

Hip-fire diagnostic breakthrough:
- the direct modern camera scan never produced `source=modern_camera_scan` in the tested log;
- canonical `SelectTargetBaddy @ 0x004C8410` starts from the body-list head at `0x0056E990`;
- the modern scan incorrectly used `G_MECHLIST`, which is the player/mech list at `0x006A9038`;
- therefore the modern scan was walking the wrong candidate universe.

Implemented commit:
- `b074592eb6bd8d5b0b6f323165de71e8d97eb248` — `gameplay: isolate modern aim from legacy lookaround`

### Manual aim third pass

1. **Retail SetupLookaroundCamera is bypassed entirely while modern manual aim is active in mode 3.**
   - The wrapper still calls retail outside modern manual aim.
   - During modern aim, `field_8EA` remains set so enter/exit/fire code still sees aim mode.
   - `field_DC0` and `field_DE4` are supplied directly.
   - This removes the second legacy controller that was still consuming lookaround state and steering the pose/joints while normal locomotion was simultaneously trying to run.
   - Goal: WASD belongs only to movement; mouse/right-stick/camera owns aim.

2. **Reticle X/Y sign convention corrected.**
   - Z remains on the actual forward camera ray so the point stays in front of the camera.
   - X and Y are reflected around camera origin before being stored in `field_DC0`.
   - Desired runtime behavior: left=left, right=right, up=up, down=down.

3. **Movement diagnostics expanded.**
   - reticle telemetry now includes body position, velocity, state and animation;
   - if Spider-Man remains stuck, the next log will show whether velocity/root translation is being generated after the legacy lookaround controller is removed.

### Hip-fire third pass

`SpideyCameraSelectModernTarget` now:
- starts from `*(CBody**)0x0056E990`, the exact retail SelectTargetBaddy list head;
- advances through `mNextItem` (+0x20), matching retail;
- retains targettable / non-zombie / valid-radius / range / retail-player-LOS filters;
- ranks by visible camera-ray centeredness;
- logs:
  - `camera_scan_nodes`
  - `camera_scan_eligible`
  - `camera_scan_candidates`
  - `camera_scan_score`
- emits periodic scan diagnostics even when the selected target does not change.

The old retail camera-origin/orientation selectors remain fallback-only.

### Exact next runtime

Update to `b074592e...` or newer and test:

1. Enter manual aim.
2. Hold WASD in all four directions:
   - Spider-Man should actually translate;
   - WASD should NOT move the reticle independently.
3. Move mouse:
   - reticle should track naturally: left=left, right=right, up=up, down=down.
4. Aim + move simultaneously and fire.
5. Hip-fire at close and medium enemies while Spider-Man faces elsewhere.
6. Sweep camera across multiple enemies.
7. Return consolidated log and subjective targeting behavior.

Expected new markers:
- `modern_manual_aim event=reticle ... retail_setup=0 ... body_pos=... body_vel=... state=... anim=...`
- `camera_web_target ... camera_scan_nodes=... camera_scan_eligible=... camera_scan_candidates=...`
- successful direct acquisition should finally show `source=modern_camera_scan`.

Do not resume real-shadow implementation until this test is evaluated. Shadow caster probing remains PASSED.


## CHECKPOINT — MANUAL AIM THIRD-PASS FRONTIER (2026-10-04)

Latest tested runtime revision:
- `7a6af671ec671d7f61a1003b296f59d655b39dcd`
- user log: `spidey-decomp(20261004-093356).log`

Relevant source already present in that runtime:
- `aabf69a90786b639c4e32e1d74e64cae88e3430e` — manual-aim movement/reticle wrappers;
- `b44b3bdca3337c0cfe4a57dc8a042feaa0a946ea` — direct camera-ray hip-fire scan.

### Latest user-observed behavior

Manual aim is still not correct, but the new test narrows the failure substantially:

1. **Spider-Man now tries to move while manual aim is held.**
   - His body visibly twists/leans as though locomotion is being requested.
   - Actual translation remains blocked, so there is still a later movement/state constraint after the CheckForwards path we already opened.

2. **WASD still moves the legacy aim cursor/reticle.**
   - Therefore retail `SetupLookaroundCamera` continues consuming the E2D/E2E movement axes and mutating lookaround state even though we overwrite `field_DC0` afterward.
   - The next pass must stop legacy WASD-to-reticle consumption rather than only overwriting the final point.

3. **Mouse-controlled reticle direction is inverted on BOTH axes.**
   - Horizontal: moving camera left/right produces the opposite reticle direction.
   - Vertical: moving camera up/down produces the opposite reticle direction.
   - The mode-3 ray used by `SpideyModernAimApplyCameraPoint` therefore has the wrong sign/convention for the reticle projection path.
   - Both X and Y need to be inverted back so left=left, right=right, up=up, down=down.

4. **Hip-fire targeting is still not considered solved.**
   - Continue evaluating the new direct camera-ray scan independently of manual aim.
   - Do not conflate manual-reticle behavior with hip-fire acquisition.

### What this means technically

The first two manual-aim patches are proven to execute, but the remaining blockers are now lower-level:

- `CheckForwards` receives movement intent, because Spider-Man visibly enters a movement/twist response.
- Something later in locomotion/state resolution prevents position translation while `field_8EA`/manual-aim state is active.
- Retail `SetupLookaroundCamera @ 0x004C38A0` still reads the same E2D/E2E axes that should belong to movement, so WASD continues steering its internal lookaround accumulators.
- Simply forcing `field_DC0` after the retail call is not enough; the legacy axis consumption must be bypassed or replaced while keeping trigger/web/reticle state that retail still owns.

### Exact RE/implementation resume point

When the user says **"continue"**, resume here:

1. Parse `spidey-decomp(20261004-093356).log` for the new:
   - `modern_manual_aim event=movement`
   - `modern_manual_aim event=reticle`
   - `camera_web_target ... source=modern_camera_scan`
   markers if available.

2. **Movement freeze**
   - Trace the post-`CheckForwards` path after the function returns true / movement intent is present.
   - Find the next `field_8EA`, lookaround-state, animation-state, velocity, or translation gate that prevents locomotion from committing.
   - Prefer a narrow wrapper/temporary-state mask around normal locomotion over globally clearing `field_8EA`, because manual aim/reticle/web state still needs that flag.

3. **WASD still steering reticle**
   - RE the exact E2D/E2E reads inside `SetupLookaroundCamera @ 0x004C38A0`.
   - Best likely solution: while modern manual aim is active, call the retail routine with temporary zero lookaround axes (or patch only the legacy axis reads), then immediately restore E2D/E2E so normal locomotion keeps the original input.
   - Preserve any non-axis trigger/web state retail SetupLookaroundCamera manages.

4. **Reticle inversion**
   - Fix the sign convention in `SpideyModernAimApplyCameraPoint`.
   - The current `camera.field_144 - camera.mPos` direction is opposite to what the reticle projection path expects on both horizontal and vertical axes.
   - Verify whether the correct basis is `camera.mPos - camera.field_144`, or an equivalent camera-basis vector with X/Y sign correction, before committing.
   - Desired behavior: mouse/right-stick left -> reticle left, right -> right, up -> up, down -> down.

5. **Hip fire**
   - Continue testing/adjusting `SpideyCameraSelectModernTarget` separately.
   - If target flicker remains, inspect scan telemetry and consider short target hysteresis / sticky retention only after confirming the raw camera-ray score is correct.

6. **Shadows**
   - World-space caster probe remains PASSED.
   - Do not redo shadow probing.
   - Resume real Renderer11 shadow work only after manual aim/hip-fire are stable enough to stop contaminating gameplay tests.

### Important recovery instruction

Do NOT restart from the older first-pass manual aim model. The current live source already contains:
- mode 7 -> mode 3 manual-aim camera patch;
- CheckForwards field_8EA gate removal;
- scoped movement-control wrapper;
- SetupLookaroundCamera wrapper;
- camera-ray field_DC0 override;
- direct modern camera-ray hip-fire scan with retail fallback.

The next work is to fix the three remaining behaviors above, not to rebuild those pieces.


## LIVE FRONTIER — MANUAL AIM INPUT DECOUPLING + DIRECT CAMERA-RAY HIP FIRE (2026-10-04)

Latest user runtime: `bf1fb237dda30e51d2525ce9479e8c82843c0db9`, log `spidey-decomp(20261004-085912).log`.

### Runtime result from the first manual-aim attempt

The first patch installed exactly as intended at the byte/call-site level, but its behavioral model was incomplete:
- `0x004C370B` mode-7 -> mode-3 patch installed;
- `0x004BF8C5` aim-only CheckForwards JNE patch installed;
- the mouse continued driving the mode-3 camera;
- nevertheless manual aim still used WASD to move the reticle and Spider-Man remained stationary;
- the reticle did not follow the camera/mouse.

This proved that the retail reticle is not derived from camera orientation.

Canonical `RenderLookaroundReticle @ 0x004C4940` projects `CPlayer::field_DC0`, an independent world-space aim point. `SetupLookaroundCamera @ 0x004C38A0` updates that lookaround state separately from the visible camera.

The canonical input helper `0x004BD510` reads the ordinary signed movement axes into `field_E2D/E2E`; it does not special-case `field_8EA`. The canonical `CheckForwards @ 0x004BF8A0` contains an earlier gate before the already-NOPed `field_8EA` branch:
`if (input[0x40] && (field_E1C & 1)) return 0`.
That held-control gate is now bypassed only for the duration of the movement evaluation while manual aim is active.

### New manual-aim implementation

Commit:
- `aabf69a90786b639c4e32e1d74e64cae88e3430e` — `gameplay: decouple manual aim from movement axes`

New scoped wrappers:
- `SpideyAI0 0x004B231A -> CheckForwards 0x004BF8A0` is wrapped;
  - only while `field_8EA` is active, save `input[0x40]`, temporarily clear it, call untouched retail CheckForwards, then restore it under `__finally`;
  - `field_E2D/E2E` are not cleared, so WASD/analogue remain real locomotion input;
  - the previous exact-byte `field_8EA` JNE removal remains.
- `SpideyAI0 0x004B8673 -> SetupLookaroundCamera 0x004C38A0` is wrapped;
  - retail SetupLookaroundCamera still runs so trigger/web/lookaround state stays intact;
  - before and after the retail call, `field_DC0` is set to an extended ray through active mode-3 `camera.mPos -> camera.field_144`;
  - `field_DE4=1` while modern manual aim is active;
  - therefore rendering and the following frame's attack logic see the visible camera's center ray, not the legacy WASD-derived lookaround point.

New telemetry:
- `modern_manual_aim event=movement ... aim_control=... axes=... state=... result=...`
- `modern_manual_aim event=reticle ... aim_point=... camera_pos=... camera_focus=...`

### Hip-fire result and new selector

The `bf1fb237` run confirms the previous retail-wrapper selector still flickers:
- a mode-3 camera-origin selection can acquire an enemy with player LOS;
- a subsequent call at effectively the same camera heading can immediately return no target.

So camera-transformed retail scoring is no longer the primary modern path.

Commit:
- `b44b3bdca3337c0cfe4a57dc8a042feaa0a946ea` — `gameplay: score hip-fire targets on visible camera ray`

New mode-3 primary selector:
- iterate `G_MECHLIST`;
- preserve canonical retail basic eligibility:
  - `mRMinor != 0`;
  - `CBODY_TARGETTABLE`;
  - not `CBODY_ZOMBIE`;
- compute real player-to-candidate Euclidean range instead of relying on cached `mPlayerDist`;
- preserve `arg1` as the range;
- use `arg2 / 4096` as the camera cone threshold (current runtime arg is 2896, approximately the retail 45-degree cosine);
- score candidates directly against the visible ray `camera.field_144 - camera.mPos`;
- require untouched retail player LOS `0x004E67A0`;
- pick the most camera-centered candidate, distance as tie-breaker.

The old retail camera-origin/orientation wrappers remain only as fallbacks for unusual targetable bodies the modern scan does not select.

New telemetry source:
`source=modern_camera_scan camera_scan_candidates=N camera_scan_score=...`

### Exact next runtime

Update to `b44b3bdc...` or newer, then:
1. enter manual aim and hold it;
2. WASD should move Spider-Man, not the reticle;
3. mouse should rotate the camera and the reticle should remain aligned with the visible camera center ray;
4. fire while moving/aiming;
5. hip-fire at close and medium enemies while Spider-Man faces away from the camera target;
6. orbit across multiple enemies and check whether acquisition is stable and chooses the visually centered enemy;
7. send the consolidated log.

Do not advance real-shadow implementation until this gameplay test is evaluated; the world-space caster probe is already considered passed.


## LIVE FRONTIER — PAUSE PASSED; MODERN MANUAL AIM + CAMERA-ORIGIN TARGETING (2026-10-04)

Runtime evidence: `turn122file0` from the user's latest current-build test.

### Runtime conclusions

Pause lifecycle fix is **validated**:
- user reports pause no longer crashes;
- log contains six `pause_menu_box_refresh` events, all `mode=in_place`;
- Options expands from parent `target_rect=0,83,512,75` to `0,83,512,107` and Back restores the parent size;
- no exception entries were logged.

World-space shadow-caster probe is **validated**:
- Spider-Man samples are consistently valid `CSuper` casters: region `spidey`, 18 parts, model-0 36 verts / 82 normals / 46 faces, live pose buffer, changing pose matrices/translations;
- NPC samples are valid super models too: `henchman` / `thug`, 15-16 parts, model-0 18 verts / 36 normals / 18 faces;
- inactive NPCs can legitimately have no current pose buffer; an active thug later exposes a live pose buffer and changing transform;
- therefore the next real-shadow implementation should submit original local model geometry + live per-part pose + `CSuper::mTransform` before projection instead of attempting to invert Renderer11's existing XYZRHW replay.

Web targeting remained inconsistent:
- 30 logged selections were all labeled `path=select_auto_aim`; no `check_web_shot` event appeared in this session;
- user clarified manual aiming was used several times, so caller label is not treated as a fire-mode identifier;
- target results repeatedly transition valid -> null while the modern mode-3 camera remains active.

### Targeting parallax root cause

Canonical `SelectTargetBaddy @ 0x004C8410` does more than transform by `player+0x89C`:
- it filters target-table / zombie / distance state;
- it uses cached player distance for range weighting;
- for angular scoring it constructs `candidate.mPos - player.mPos`, then transforms that vector and scores `-localZ`;
- it finally performs retail LOS.

The previous camera patch changed the orientation matrix only. Because the visible third-person camera is behind/above Spider-Man, close enemies can be screen-centered yet still fall outside the retail ~45-degree cone when the direction vector originates at Spider-Man. This explains camera-centered misses without requiring a wider cone.

Implemented:
- `58742eb419fa755d2c044425ae2cbc44963ac731` — `gameplay: modernize manual aim and camera target origin`
- `f34aa5ddc3efd3c2571dd0de8e9483276108b1fd` — `gameplay: validate camera targets with retail LOS`

New targeting behavior in mode 3:
1. preserve retail target filtering, cached range weighting and scorer;
2. supply the active render camera orientation;
3. temporarily supply the active render camera world position as the scorer origin, fixing third-person parallax;
4. restore Spider-Man's world position and scoring matrix immediately under `__finally`;
5. revalidate the selected target with untouched retail `Utils_LineOfSight @ 0x004E67A0` from Spider-Man's real position;
6. if camera-origin selection cannot produce an acceptable shot, fall back to the previous orientation-only retail call.

Telemetry now reports:
- `source=render_camera_origin`;
- `source=render_camera_orientation_fallback`;
- `camera_origin_candidate=<0|1>`;
- `player_los=<0|1>`.

### Modern manual aim RE + implementation

Retail manual aim is not camera enum LOOKAROUND in this build.

`CPlayer::EnterLookaroundMode @ 0x004C3580`:
- sets `player+0x8EA = 1` for the aim/reticle state;
- calls `CCamera::PushMode`;
- explicitly pushes mode `7` and calls `CCamera::SetMode`, which runtime telemetry identifies as `FRONT`.

This is why manual aim temporarily leaves the modern mode-3 orbit path.

The movement lock is narrow:
- `CPlayer::CheckForwards(bool) @ 0x004BF8A0`;
- `cmp byte ptr [esi+0x8EA],0`;
- `jne 0x004BFA0A` at `0x004BF8C5`;
- the JNE skips ordinary forward locomotion solely because manual aim is active.

Modern aim patch:
- `0x004C370B`: validated bytes `6A 07` -> `6A 03`, so retail still owns the aim/reticle state but keeps ordinary mode-3 camera ownership; existing relative mouse + Input11 right stick therefore continue to drive camera/aim;
- `0x004BF8C5`: validated `0F 85 3F 01 00 00` -> six NOPs, enabling `CheckForwards` movement while aiming;
- all other `field_8EA` restrictions (jump/swing/special moves etc.) remain retail for this first version;
- both machine-code changes use exact-byte fail-closed validation through new `SpideyPatchBytes`.

Expected marker:
`modern_manual_aim_install camera_mode=1 ... movement=1 ...`

### Next implementation / validation

Before changing more aim-state restrictions, runtime-test this narrow first version:
1. enter manual aim;
2. move the camera/reticle with mouse and, if available, right stick;
3. walk/strafe using normal movement controls while still aiming;
4. fire at close and medium enemies centered by the camera while Spider-Man faces elsewhere;
5. report any snap when leaving manual aim;
6. brief pause regression.

In parallel, shadow work may now advance from probing to a dedicated Renderer11 world-space caster ABI + directional depth pass.


## LIVE FRONTIER — TARGETING SECOND PATH + WORLD-SPACE SHADOW PROBE (2026-10-04)

Source commits:
- `23260d11fb8a0b0533b9d0588cb96b927c8def44` — `gameplay: align CheckWebShot targeting to camera`
- `62d8711b09eb633a3ddc8a5271aeff0f55a697c6` — `renderer: probe animated world-space shadow casters`
- `0b72b0a340a3676f4678b8d0fc6888d24af242b7` — `renderer: harden world-space caster probe`

### Web targeting
The earlier camera-forward wrapper covered only the `SelectAutoAimTarget -> SelectTargetBaddy` call at `0x004C5B2F`. The user reported targeting was improved but still not complete.

The same wrapper logic is now shared by both retail paths:
- `0x004C5B2F -> 0x004C8410` — SelectAutoAimTarget path;
- `0x004C09E2 -> 0x004C8410` — CheckWebShot path.

Both preserve the untouched retail `SelectTargetBaddy` implementation and only swap the temporary scoring matrix while modern DEMO-camera control is active. Camera row 2 is negated so camera-forward maps to retail's negative-local-Z scoring convention. Telemetry now records `path=select_auto_aim` or `path=check_web_shot`.

Both call-site installs go through `SpideyPatchDirectCall`, so an opcode/target mismatch fails closed instead of modifying an unexpected executable site.

### Real-shadow groundwork
The current Renderer11 replay still receives pretransformed `FVF 0x144 / XYZRHW` vertices, so a true light-space shadow pass cannot recover reliable world geometry from the existing replay alone.

Historical M3D source in `thps2-stuff/m3d.mik` gives us a cleaner route:
- `RenderSuperItem` has the live `CSuper` object transform;
- each animated part has an `SMatrix` animation transform;
- each part resolves an `SModel` with local `SVECTOR` vertices and face data;
- the original engine combines the super transform + animation transform before the GTE projection stage.

A guarded runtime probe is now wired into the camera telemetry cadence. Every 300 gameplay frames it inspects:
- retail mech-list head through `G_MECHLIST`;
- the current web-target body when different;
- PSX region/super-model validity;
- region name and part count;
- model-0 vertex/normal/face counts and one local vertex sample;
- `mpDecompressedFrame` / `mpPoseBuffer` pointers and one matrix sample;
- actor world position;
- the game's current ground-shadow contact position/normal/scale.

All actor/model reads are inside SEH, and log output uses copied values only, so a stale target pointer cannot be dereferenced after the guard.

Expected telemetry:
`[SHADOW] world_space_probe ... label=mech_head ...`
`[SHADOW] world_space_probe ... label=web_target ...`

This is intentionally non-visual. Its purpose is to validate the exact animated world-space caster inputs before expanding the Renderer11 ABI and adding a depth-map pass.

### Next combined runtime validation
Use `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` after the current batch is complete, then validate in one session:
1. pause/unpause repeatedly and enter/exit custom Options several times;
2. test web shots with Spider-Man facing away from a target but the camera centered on it, then camera away while Spider-Man faces it;
3. keep at least one NPC targetable for >5 seconds so both world-space probe labels can appear;
4. return the consolidated `spidey-decomp*.log`.

If the pause lifecycle hardening + both targeting call sites are good and the world-space probe shows valid animated model/pose data, next implementation step is the Renderer11 world-space caster submission + first directional shadow-map pass.


## LIVE FRONTIER — PAUSE BOX HEAP LIFECYCLE HARDENED (2026-10-04)

Source commit:
- `f7c2532a956610768609e6732ec64e83b89bd463` — `pause: resize expanding box in place`

Reason:
- the previous dynamic Pause Options container called retail `CMenu::Zoom @ 0x0043FC60`;
- `CMenu::Zoom` calls `KillBox()`, deletes the live heap-owned `CExpandingBox`, then allocates a replacement;
- our hook runs inside the pause menu's live update/confirm frame, so deleting that object there creates an avoidable lifecycle/use-after-free risk if surrounding retail code still references the old box.

New behavior:
- preserve the existing `CExpandingBox*`;
- derive the same target rectangle retail Zoom would use from `GetMenuHeight()`, zoom type, font and menu geometry;
- update `field_1C/field_20/field_C/field_10` in place;
- clamp current width/height only when shrinking;
- guard the box write with SEH and log `mode=in_place`, old/target rectangles and any write fault;
- no gameplay/menu row semantics changed.

Static verification after commit:
- replacement function present on live `dev`;
- old `SpideyRetailMenuZoomFn zoom` call absent from the refresh function;
- local brace/parenthesis counts balanced.

NEXT:
1. finish the remaining camera-web targeting path now that the user reports targeting is improved but still not complete;
2. then begin the DX11 world-space capture/shadow bridge groundwork;
3. next user build should validate pause repeatedly plus both targeting paths in one session.


**Project:** Spider-Man 2000 PC decomp / developer build  
**Repository:** `legentus/spidey-decomp`  
**Working branch:** `dev`  
**Upstream-sync branch:** `master`  
**Upstream baseline:** `krystalgamer/spidey-decomp`  
**Baseline commit:** `4eb5635bf1e0aae9edff771d3b9b0db16971ff28` — `patch: CFT4Bit::SetTransparency`  
**Last live update:** 2026-09-29

## Immediate Goal

Establish **Spider-Man 2000 Developer Build 0.1**:

1. Reproduce the existing matching/decomp Windows build.
2. Install it against the user's exact retail PC game.
3. Prove rebuilt C++ executes reliably.
4. Preserve a clean recovery path to the stock game.
5. Select and fix the first real bug only after the baseline works.

## Repository Policy

- Keep `master` clean for upstream synchronization.
- Put our active development work on `dev` and feature branches.
- Do not commit original retail game binaries or PKR assets.
- Preserve original matching/reconstruction work where possible.
- Separate intentional modernization / developer functionality from matching work.
- No fabricated addresses, layouts, offsets, or runtime state; verify before patching.

## Live-Documentation Rule

This file is updated **during work**, not only at the end of a chat. After any meaningful discovery, failed path, code change, build frontier, or test result, update this file so work can resume after an interruption.

## Verified State

- User fork exists and is writable.
- `master` is the upstream-sync branch.
- `dev` was created from upstream head `4eb5635bf1e0aae9edff771d3b9b0db16971ff28`.
- First live-status commit on `dev`: `b2a2beb8f49ec89693ef556f031d5f26d5b92a30`.
- The Windows matching build is the preserved old Visual Studio/NMAKE project (`spider.mak` / `build.bat`), **not** the CMake target.
- CMake currently builds a portability executable; upstream CI uses the old Windows project to produce `Release\\spider.dll`.
- CI renames/copies `Release\\spider.dll` to `binkw32.dll` and validates it with Tobey Validator.

## Bootstrap Architecture — VERIFIED FROM SOURCE

The current reconstructed Windows build is an incremental **Bink proxy + live EXE patcher**:

1. Retail `SpideyPC.exe` normally loads `binkw32.dll`.
2. The reconstructed `spider.dll` is installed/renamed as `binkw32.dll`.
3. The original retail Bink DLL is expected to exist as `binkw32_.dll`.
4. `forwards.h` forwards the retail Bink export surface to `binkw32_.dll`.
5. Our DLL's `DllMain` runs on `DLL_PROCESS_ATTACH`.
6. It allocates a console, sets the title to `spidey-decomp - <commit>`, runs runtime structure assertions, makes the retail EXE text range writable, applies `game_patches()`, then restores protection.
7. Patch macros in `my_patch.h` redirect selected retail function entries/calls to reconstructed C++ functions in the proxy DLL.
8. This lets the project replace reconstructed functions incrementally while the rest of the original executable remains intact.

### Important implication

We **do not need a standalone fully rebuilt executable before fixing bugs**. We can validate one reconstructed subsystem/function at a time inside the retail game.

## Current Frontier

**ACTIVE:** perform the first baseline build/install/launch from `dev` before making gameplay-source changes.

CI workflow was updated on `dev` at commit `9748899a10ce5ac708f54e4f9c7d8c4de3845351` to:
- run on pushes to `dev`;
- allow manual `workflow_dispatch`;
- allow PR validation targeting `dev`.

**Observed after the push:** GitHub's Actions-runs API currently reports zero runs for branch `dev`, and the commit has no combined status entries. Therefore the CI build is **not yet verified**. Do not assume the DLL was built. This may require enabling Actions for the newly created fork or another workflow-side fix; exact cause not yet proven.

To avoid blocking progress on fork Actions initialization, a local one-command Windows matching-build path has now been added and documented.

### Added build/install helpers

- `scripts/setup_matching_toolchain.ps1` — commit `5b5ea2d37e94feef9dfc6a8d2681f339cc6960b1`
- `scripts/build_matching.ps1` — commit `a3a18cd8b5ea8af3793b0fd4bb5fa2db9ab509e1`
- `scripts/install_dev_proxy.ps1` — commit `903ad122a41eb5da1e6ae28602c30637451e83ab`
- `scripts/restore_stock_bink.ps1` — commit `74a08d8da003111d671934cc02f6a37e11453d14`
- `docs/BUILD_AND_INSTALL.md` — commit `384a592aa6c16e3f61308fd54b47fcdc9a122190`

These changes do **not** modify reconstructed gameplay/engine code.

## Baseline Test Plan

Once CI produces the DLL:

1. Back up the retail game's original `binkw32.dll`.
2. Rename the original to `binkw32_.dll`.
3. Install our generated artifact as `binkw32.dll`.
4. Launch `SpideyPC.exe`.
5. Verify the decomp console opens and shows the commit/version.
6. Capture any assertion output/crash before changing source behavior.

Do **not** move on to a gameplay bug until this baseline is confirmed.

## Next Action

**User-side baseline test is now the blocking step.**

From a Windows clone of this repository:

```powershell
git checkout dev
powershell -ExecutionPolicy Bypass -File .\scripts\build_matching.ps1
```

Then install the generated proxy against the user's retail game directory:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install_dev_proxy.ps1 -GameDir "<folder containing SpideyPC.exe>"
```

Launch `SpideyPC.exe` and capture:
- whether the `spidey-decomp` console appears;
- the full console output, especially any validation failures;
- whether the game reaches the menu / gameplay;
- the SHA-256 printed by the build/install helper.

If the fork's GitHub Actions page shows workflows disabled, enable Actions there as a secondary CI path. As of the latest check, the API still reports zero `dev` workflow runs.

**Do not start bug-fix source changes until this baseline result is recorded.**


## User Workflow Change — 2026-09-29

User requested a BAT-first local workflow matching the Destroy All Humans recomp style.

**ACTIVE NOW:** add top-level Windows BAT launchers so the user can keep one local clone, update it from `origin/dev`, build, install, run, and restore without typing PowerShell commands.

Planned BAT interface:
- `SETUP_FIRST_TIME.bat`
- `UPDATE_PROJECT.bat`
- `BUILD_DEV.bat`
- `INSTALL_DEV_BUILD.bat`
- `RUN_GAME.bat`
- `UPDATE_BUILD_INSTALL.bat`
- `RESTORE_STOCK_GAME.bat`

Local game path will be stored in an ignored local config file and never committed.


## BAT Workflow — READY

Top-level BAT workflow is now committed on `dev`:

- `SETUP_FIRST_TIME.bat` — configure / change retail game folder.
- `UPDATE_PROJECT.bat` — fetch + fast-forward local `dev` from the user's fork.
- `BUILD_DEV.bat` — build the matching proxy.
- `INSTALL_DEV_BUILD.bat` — install the built proxy into the configured game folder.
- `BUILD_AND_INSTALL.bat` — build + install in one step.
- `RUN_GAME.bat` — launch `SpideyPC.exe` with the configured working directory.
- `RESTORE_STOCK_GAME.bat` — restore the preserved retail Bink DLL.
- `SPIDEY_DEV_MENU.bat` — numbered menu for all of the above.

`spidey_local_config.bat` is ignored by Git and stores only the local retail game path.

Documentation was rewritten BAT-first at commit `7839787e9730eb2321636b51060a539d9762c88c`.

A standalone bootstrap package was also generated for the user as `Spider-Man-2000-Dev-Bootstrap.zip`; its `GET_SPIDEY_PROJECT.bat` clones `legentus/spidey-decomp` branch `dev` into a local folder (default: Documents\\Spider-Man-2000-Dev), or updates an existing clone, then runs first-time setup.

### Normal local loop

```text
UPDATE_PROJECT.bat
        ↓
BUILD_AND_INSTALL.bat
        ↓
RUN_GAME.bat
```

### Next blocking step

User should bootstrap/clone locally, run the BAT workflow, and report the first build + launch result. Capture the complete build console output and any `spidey-decomp` runtime console output. Do not begin gameplay-source bug fixes until the baseline proxy has been tested against the user's exact retail installation.


## Bootstrap Git Dependency Fix — 2026-09-29

First user run of the standalone bootstrap stopped because `git.exe` was not in PATH.

**ACTIVE FIX:** replace the bootstrap with a self-healing version that:
1. checks PATH for Git;
2. checks normal Git for Windows install locations even if PATH is stale/missing;
3. installs Git automatically with `winget` when available;
4. falls back to downloading the current 64-bit Git for Windows installer from the official `git-for-windows/git` GitHub release and runs it silently;
5. continues cloning/updating `legentus/spidey-decomp` branch `dev` in the same run.

User should not need to manually install Git or edit PATH.


### Bootstrap Git dependency fix completed

Committed fixes:
- `GET_SPIDEY_PROJECT.bat` now auto-detects Git, auto-installs Git for Windows when absent, and continues the clone/update in the same run.
- `UPDATE_PROJECT.bat` now finds Git in standard install locations even when PATH is stale.
- `SETUP_FIRST_TIME.bat` no longer tells the user to install Git manually.
- Git path quoting was hardened for installs under `C:\Program Files`.

Latest bootstrap commit: `2309c4613a9e97d25206ae3211ed46ca794802e7`.

**Next user action:** discard the old bootstrap ZIP/BAT, run the newly generated `GET_SPIDEY_PROJECT.bat`, and report the complete output if it stops again.


## Bootstrap Fix #2 — Portable Git — 2026-09-29

Second bootstrap attempt reached the official Git for Windows installer download (`Git-2.56.0-64-bit.exe`) but the installer path returned failure before the project clone.

**Decision:** stop relying on a system-wide Git installation entirely.

New design:
- Prefer an existing system Git when available.
- Otherwise download the official **MinGit 64-bit portable ZIP** from the latest `git-for-windows/git` GitHub release.
- Extract it under the local Spider-Man development folder (no admin/UAC, no installer, no PATH persistence required).
- Use that portable `git.exe` for clone/update operations.
- Keep future `UPDATE_PROJECT.bat` able to use the bundled portable Git when system Git is unavailable.

This should make the project self-contained on a clean Windows machine.


### Portable MinGit bootstrap implemented

Verified official package used by the bootstrap:
- release: `git-for-windows/git v2.56.0.windows.1`
- asset: `MinGit-2.56.0-64-bit.zip`
- SHA-256: `064b440ff870ed5198527e8f3a92cdf5bd2fd0fedf5e718af95e3fdaddeff718`

Committed:
- `GET_SPIDEY_PROJECT.bat` switched from installer/winget flow to verified portable MinGit: `c9d9210e66102e10d550dcf203c07c7fe710455e`
- `UPDATE_PROJECT.bat` now also detects the private portable Git location: `18784b2d615b4e2bb393a3a4e5bb37e192070be2`

Portable Git location:
`%LOCALAPPDATA%\Spidey2000Dev\MinGit`

No administrator rights, system-wide Git install, or persistent PATH modification should be required.

**Next user action:** run the new portable-MinGit bootstrap. If it fails, capture all output; the expected progression is download -> SHA-256 verify -> extract -> clone `dev` -> first-time game path setup.


## Bootstrap Fix #3 — Remove PowerShell Dependency — 2026-09-29

User's Windows environment does not expose `powershell.exe`, so the portable-MinGit bootstrap stopped before download.

**New bootstrap rule:** no PowerShell dependency.

Next implementation will use only Windows command-line tools expected on current Windows builds:
- `curl.exe` for download
- `certutil.exe -hashfile ... SHA256` for checksum verification
- `tar.exe -xf` for ZIP extraction

If any of those are missing, the bootstrap will print exactly which tool is unavailable rather than failing generically.

Goal remains: zero manual prerequisites and no admin/system-wide Git install.


### PowerShell-free user workflow implemented

Changes committed:
- `build.bat` now supports a private matching compiler location through `SPIDEY_MSVC_ROOT`: `6c7b9be6e18dace8e986c41be7da4de854b6cc11`
- `BUILD_DEV.bat` no longer calls PowerShell and downloads/extracts the preserved compiler with `curl.exe` + `tar.exe`: `09d91dd5375dfeb1528241555b0c4d325cdbb7cd`
- `INSTALL_DEV_BUILD.bat` no longer calls PowerShell: `6ad38bea3b5af1a6a2c9107f40cdf65bd40184a4`
- `RESTORE_STOCK_GAME.bat` no longer calls PowerShell: `a69f146f6d83ecb4e80b1a22361be9649ae32b20`
- `GET_SPIDEY_PROJECT.bat` no longer calls PowerShell; portable MinGit setup now uses only `curl.exe`, `certutil.exe`, and `tar.exe`: `79667480f51ba0a130e5df8cd1e172a98a4eb86e`

Private local tools:
- MinGit: `%LOCALAPPDATA%\Spidey2000Dev\MinGit`
- preserved matching compiler: `%LOCALAPPDATA%\Spidey2000Dev\MatchingVS`

No administrator rights, PowerShell, system-wide Git installation, or writes to `C:\vs` are required by the new user workflow.

**Next user action:** discard all older bootstrap BATs and run the bootstrap generated from commit `79667480f51ba0a130e5df8cd1e172a98a4eb86e`. Expected path: portable MinGit download -> SHA-256 verification -> tar extraction -> clone `dev` -> configure game folder.


## Bootstrap Fix #4 — Eliminate Git/curl/PowerShell dependencies — 2026-09-29

User's environment also does not expose `curl.exe`. Continuing to add prerequisite probes is the wrong design.

**New final local-update design:**
- Do not require Git on the user's PC at all.
- Do not require PowerShell.
- Do not require curl.
- Bootstrap/update downloads the GitHub `dev` branch archive directly:
  `https://github.com/legentus/spidey-decomp/archive/refs/heads/dev.zip`
- Download is performed through Windows Script Host (`cscript.exe`) using built-in Windows HTTP/COM components.
- ZIP extraction is performed through Windows Shell COM, with `tar.exe` only as an optional fast path when present.
- Local refresh preserves `spidey_local_config.bat`.
- The user's normal update workflow remains a single `UPDATE_PROJECT.bat`.

This is now preferred over maintaining a local Git clone because the user's goal is a self-updating working copy, not local source-control operations.


## DAH Workflow Review / Root Cause — 2026-09-29

Reviewed the actual working DAH port workflow in `legentus/DAH-Port`:
- `UPDATE_DAH_PORT.bat` is intentionally tiny and delegates to `tools/UPDATE_DAH_PORT.ps1`.
- `TEST_LATEST_BUILD.bat` is intentionally tiny and delegates to `tools/TEST_LATEST_BUILD.ps1`.
- The real logic lives in the tools scripts; the user-facing BAT layer stays stable.

Important correction for Spider-Man:
- Earlier failures of `where powershell.exe` and `where curl.exe` do **not** prove those Windows components are absent.
- On normal Windows they live under explicit system paths such as:
  - `%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\System32\curl.exe`
  - `%SystemRoot%\System32\tar.exe`
  - `%SystemRoot%\System32\certutil.exe`
  - `%SystemRoot%\System32\cscript.exe`
- A damaged/minimal PATH can therefore make `where` fail even though the tools exist.

**New direction:** mirror the proven DAH pattern. User-facing BATs will explicitly repair/discover Windows system paths first, then delegate to stable tool scripts. Stop adding layers of Git installers/portable prerequisites.


## DAH-style workflow conversion completed

The Spider-Man local workflow now mirrors the proven DAH port structure:

User-facing BATs:
- `GET_SPIDEY_PROJECT.bat` — first-time bootstrap wrapper
- `UPDATE_SPIDEY_PROJECT.bat` — update-only wrapper
- `TEST_LATEST_BUILD.bat` — update -> restart if workflow changed -> build -> install -> launch
- legacy names `UPDATE_PROJECT.bat` and `BUILD_AND_INSTALL.bat` now forward to the new workflow

Real logic:
- `tools/BOOTSTRAP_SPIDEY_PROJECT.ps1`
- `tools/UPDATE_SPIDEY_PROJECT.ps1`
- `tools/TEST_LATEST_BUILD.ps1`

Key fix from DAH review:
- BAT wrappers no longer depend on PATH to find PowerShell.
- They explicitly check standard Windows locations:
  - `%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\Sysnative\WindowsPowerShell\v1.0\powershell.exe`
  - `%SystemRoot%\SysWOW64\WindowsPowerShell\v1.0\powershell.exe`
- They also prepend standard Windows system directories to PATH before launching the tool script.

Local source updates do not require Git:
- updater downloads the `dev` branch ZIP from GitHub;
- records the exact remote commit in `LOCAL_DEV_REVISION.txt`;
- preserves `spidey_local_config.bat`, logs, build outputs, and local revision state;
- `TEST_LATEST_BUILD.ps1` restarts itself after an update exactly like the DAH workflow.

Latest-test behavior:
1. update local project;
2. restart if the workflow changed;
3. read/configure retail Spider-Man folder;
4. download preserved matching compiler if needed;
5. build `Release\spider.dll`;
6. stage/install it as `binkw32.dll`;
7. preserve retail Bink as `binkw32_.dll`;
8. record revision + proxy SHA-256 under `logs\<timestamp>`;
9. launch `SpideyPC.exe`.

Latest safety cleanup:
- bootstrap refuses to mirror into a non-empty unrelated folder;
- `LOCAL_DEV_REVISION.txt` and `logs/` are ignored.

**Next user action:** use the newly packaged DAH-style bootstrap containing only `GET_SPIDEY_PROJECT.bat` and `tools\BOOTSTRAP_SPIDEY_PROJECT.ps1`. Discard all earlier bootstrap ZIPs.


## Local Bootstrap Success — 2026-09-29

User confirmed the DAH-style bootstrap completed successfully.

Local state now present:
- full Spider-Man decomp/dev project downloaded locally;
- `UPDATE_SPIDEY_PROJECT.bat` present and working copy established;
- `TEST_LATEST_BUILD.bat` present;
- local game path configured;
- local project is ready for the first baseline build/install/launch.

**Immediate next step:** run `TEST_LATEST_BUILD.bat` with the current unmodified gameplay/decomp source.

Baseline success criteria:
1. updater reports local project current;
2. matching compiler toolchain is downloaded/extracted if not already present;
3. `Release\spider.dll` builds successfully;
4. proxy is staged/installed as `binkw32.dll`;
5. original retail Bink is preserved as `binkw32_.dll`;
6. `SpideyPC.exe` launches;
7. `spidey-decomp` console appears and prints revision/validation output;
8. game reaches menu/gameplay without immediate failure.

Do not make gameplay/source changes until this exact baseline is captured.


## First TEST_LATEST_BUILD run — updater false failure — 2026-09-29

User ran `TEST_LATEST_BUILD.bat` from:
`F:\Spider-Man 2000 Recomp\project main`

Observed:
- local revision before update: `94a77b0b3271c57d5eb40bb835969bf1fa9cfd8e`
- remote revision: `3c13230edf34ae2241c43bcc0b2059a387169f4d`
- dev archive downloaded successfully;
- archive extracted successfully;
- local project refresh completed successfully;
- updater printed `[OK] Local project is current.`;
- immediately afterward the parent latest-test script printed `[ERROR] Update failed.`;
- process exit code was 3.

Root cause identified:
- `robocopy` uses exit codes 0-7 for successful/acceptable outcomes.
- the updater correctly treated codes below 8 as success, but left `$LASTEXITCODE` equal to robocopy's code (3 in this run).
- `TEST_LATEST_BUILD.ps1` then checked that stale `$LASTEXITCODE` and misclassified the successful update as a failure.
- the updater also has a success-path `exit 0` when already current; because the updater is invoked inside the latest-test PowerShell process, that should be replaced with a normal return so it cannot terminate the parent test workflow.

**ACTIVE FIX:** normalize `$global:LASTEXITCODE = 0` on all successful updater returns and avoid `exit` on successful updater paths.


### Updater exit-code fix committed

Fix commit:
`3b90e5d8b39081bb1ca1e3dc053b9d62b813cdb1`

Changes:
- successful "already current" path now uses `return` instead of `exit 0`;
- all successful updater completions explicitly set `$global:LASTEXITCODE = 0`;
- this prevents successful robocopy codes 1-7 (observed code 3) from being misread by `TEST_LATEST_BUILD.ps1` as an update failure.

Recovery from the user's current local state:
1. run `UPDATE_SPIDEY_PROJECT.bat` once by itself so the fixed updater is pulled into the local project;
2. then run `TEST_LATEST_BUILD.bat` again.


## First successful full build; install blocked by Program Files permissions — 2026-09-29

User's latest TEST_LATEST_BUILD run reached the actual build and produced a proxy successfully.

Observed:
- updater reported local/remote revision `16c61b783962d845d9c0056db463b4109fb1585b` and `[OK] Already current`;
- configured game path: `C:\Program Files (x86)\Activision\Spider-Man`;
- preserved matching toolchain downloaded/extracted successfully;
- full NMAKE/MSVC6-era build completed successfully;
- linker produced `Release\spider.dll`;
- staged proxy SHA-256:
  `5C444AE81948E054B835C8E0DD2D31C2098EEA063B7BF9B896B5BDE3873F1B72`;
- first install attempt failed at renaming retail `binkw32.dll` to `binkw32_.dll` with AccessDenied because the game is installed under Program Files (x86).

**ACTIVE FIX:** TEST_LATEST_BUILD should detect that the configured game directory is not writable and automatically relaunch itself elevated once, then continue the update/build/install/launch workflow. The user should not have to manually right-click Run as administrator.


### Automatic elevation fix committed

Fix commit:
`1339465e6da0741c4b1204712a3101c105ea1c0f`

`tools/TEST_LATEST_BUILD.ps1` now:
- probes write access to the configured game directory before build/install;
- if the game directory is protected (observed under `C:\Program Files (x86)\Activision\Spider-Man`), automatically relaunches itself with UAC elevation;
- resumes with `-PostUpdate -Elevated` to avoid re-running the updater unnecessarily;
- confirms elevated write access before continuing;
- preserves the same automatic build/install/launch workflow.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` once to pull this fix, then run `TEST_LATEST_BUILD.bat`. Accept the Windows UAC prompt when it appears. Expected next frontier is actual proxy install + game launch.


## First successful proxy install + game launch — 2026-09-29

User ran the fixed elevated `TEST_LATEST_BUILD.bat`.

Observed successful baseline:
- revision tested: `8a7a069ed2650f8c390b10922b47737cb0bd8c0d`;
- game: `C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe`;
- elevated game-folder access confirmed;
- matching toolchain found at `C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS`;
- incremental rebuild completed successfully;
- proxy SHA-256 remained:
  `5C444AE81948E054B835C8E0DD2D31C2098EEA063B7BF9B896B5BDE3873F1B72`;
- retail `binkw32.dll` was successfully preserved as `binkw32_.dll`;
- rebuilt proxy installed as live `binkw32.dll`;
- game launched successfully;
- session log directory:
  `F:\Spider-Man 2000 Recomp\project main\logs\20260929-024052`.

This is the first confirmed build/install/launch baseline for the local Spider-Man dev workflow.

**Next frontier:** capture/verify the runtime console assertions and whether the game reaches menu/gameplay cleanly under the proxy. Once confirmed, perform one low-risk deliberate source-level proof change before choosing the first real gameplay bug.


## Disc-image auto-mount workflow — 2026-09-29

User accepted the non-crack compatibility path: automatically mount a backup image of their own Spider-Man disc before launch.

Planned TEST_LATEST_BUILD behavior:
1. read `SPIDEY_DISC_IMAGE` from the local config;
2. if missing, prompt once for an ISO path and save it locally;
3. mount the ISO with Windows' built-in disk-image support;
4. build/install the current proxy as usual;
5. launch `SpideyPC.exe`;
6. wait for the game process to exit;
7. unmount the ISO only if this script mounted it.

The disc image path remains local-only and is not committed.


### ISO auto-mount implementation completed

Final clean implementation commit:
`6d5405a419fea28e4e2bf011d099dc1c6be079b5`

`tools/TEST_LATEST_BUILD.ps1` now:
- reads `SPIDEY_DISC_IMAGE` from `spidey_local_config.bat`;
- prompts once for the user's Spider-Man ISO if not configured;
- validates that the file exists and is an `.iso`;
- saves the ISO path locally;
- detects whether that ISO is already mounted;
- mounts it with Windows `Mount-DiskImage` if needed;
- reports the assigned drive letter when available;
- launches `SpideyPC.exe`;
- keeps the image mounted for the full lifetime of the game process;
- automatically dismounts the ISO after the game exits only when the script mounted it;
- leaves an already-mounted ISO alone;
- records the disc-image path in the per-run test-session log.

During implementation a malformed intermediate script commit was detected during verification and immediately replaced before user testing. Commit `6d5405a419fea28e4e2bf011d099dc1c6be079b5` is the clean replacement.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. On the first run only, enter the path to the Spider-Man ISO. Accept the existing UAC prompt for the Program Files game install. Verify that the CD-ROM dialog no longer appears and that the game reaches the menu/gameplay.


## ISO auto-mount test — mounted successfully but CD check still fails — 2026-09-29

User tested revision `4d315296553698cd972253e35153cf2a6ff5b239` with configured disc image:
`F:\Spider-Man.iso`

Observed:
- matching proxy built successfully;
- proxy SHA-256: `9568710A82A82C781F4233AFA34086A5AAD5C1AB2DEBD34AB42D8C919220D144`;
- proxy installed successfully;
- ISO mounted successfully as drive `I:`;
- `SpideyPC.exe` launched;
- game still displayed the original "Please insert the Spider-Man CD-ROM" error;
- game exited with code 1;
- launcher then unmounted the ISO successfully.

Conclusion:
- automatic mounting works;
- a plain Windows-mounted ISO does not satisfy the game's original CD validation;
- next investigation is to determine what disc characteristics the retail check expects (for example data layout, volume identity, mixed-mode/audio TOC, or another property) and whether the current ISO representation preserves them.


## ISO mount compatibility test result — 2026-09-29

User confirmed the configured ISO mounted successfully as drive I:, the dev proxy built/installed, and the game launched, but the game still displayed its original disc-required dialog and exited with code 1.

Current conclusion:
- mounting works;
- Windows built-in ISO presentation is not sufficient for this legacy game check on the tested system.

Next step:
- add a pre-launch compatibility diagnostic for the mounted optical drive;
- support a fuller virtual optical-drive backend when the built-in Windows mount is not compatible;
- preserve the current automatic mount/unmount workflow.


Latest test-runner update: commit 9bcdaacc9fccf59384cc6813fb91cd31cf3443ad adds pre-launch disc compatibility diagnostics and an alternate virtual-drive backend. Next action: update locally and rerun TEST_LATEST_BUILD.bat.


## Cracked EXE boot succeeds; first runtime crash — 2026-09-29

User is now testing with a cracked Spider-Man PC executable, so the physical-disc/ISO workaround is no longer part of the active workflow.

Latest run:
- tested revision: `9bcdaacc9fccf59384cc6813fb91cd31cf3443ad`;
- proxy SHA-256: `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`;
- proxy installed successfully;
- game passed the previous disc gate and actually booted;
- process later exited with code `-1073741819` = `0xC0000005` (access violation).

Important risk:
- the decomp proxy applies many fixed-address runtime patches into `SpideyPC.exe`;
- the cracked executable may differ from the original retail executable at those addresses even if it otherwise boots;
- before treating the crash as a decomp bug, we must fingerprint the exact running EXE and validate that its PE layout and patched bytes are compatible with the hardcoded addresses.

**ACTIVE NEXT STEP:** remove the disc-image requirement from TEST_LATEST_BUILD and add automatic executable fingerprint + patch-site compatibility logging before launch.


## Crash-diagnostic frontier — 2026-09-29

The ISO/MCI/virtual-drive testing path is superseded and no longer active. User is testing with a cracked EXE that boots without the disc gate.

Latest known runtime result:
- game boots under rebuilt proxy;
- process later exits with `0xC0000005` access violation.

Diagnostics now added:
- `tools/TEST_LATEST_BUILD.ps1` reset to the pre-ISO workflow;
- exact running `SpideyPC.exe` is fingerprinted every test:
  - SHA-256
  - file size
  - PE machine
  - timestamp
  - entry point RVA
  - image base
  - image size
  - section layout
- fingerprint saved under the timestamped test-session directory as `game-exe-fingerprint.txt`;
- proxy installs an unhandled-exception filter and writes `spidey-decomp-crash.log` containing:
  - exception code
  - fault address
  - fault module/path
  - module base + module-relative offset
  - x86 register state;
- launcher waits for the game to exit and copies the native crash log into the same session directory when present;
- launcher labels `-1073741819` explicitly as `0xC0000005`.

Relevant commits:
- `295313a4b71b74f0b79cdac05142bf76beec737c` — remove ISO workflow; add EXE fingerprint/crash collection;
- `ab73cf5e3ecb3312390390906acb95f98a6b111f` — native unhandled-exception crash logger;
- `a21dc7cb3743291ff9c14c8f747423bcb8bba2f3` — fix session-log/fingerprint ordering.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. After the crash, provide the new console output plus the timestamped session's `game-exe-fingerprint.txt` and `spidey-decomp-crash.log` if generated.


## Critical build-system finding — stale DLL was executed — 2026-09-29

Latest user run at revision `bfaee84279eab635aaefadce36bb412683751de2`:
- `main.cpp` was recompiled;
- NMAKE output did **not** show a subsequent `link.exe` step;
- staged proxy SHA-256 remained `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`, identical to the previous pre-crash-logger DLL;
- therefore the newly added crash-logger code was not present in the DLL actually launched;
- this explains why no `spidey-decomp-crash.log` was generated.

The exact tested EXE fingerprint is:
- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- file size 1507328
- PE timestamp `0x3B7A3167`
- image base `0x00400000`
- image size `0x02A0D000`
- .text RVA `0x1000`, raw size `0x13A000`

The cracked EXE preserves the expected fixed-address layout at a coarse PE level.

**ACTIVE FIX:** make TEST_LATEST_BUILD force a clean relink/rebuild whenever source revision changes and verify that the output DLL timestamp/hash changed when compilation occurred. Also replace the overwriteable unhandled-exception filter with a first-priority vectored exception handler filtered to access violations.


## Stale-DLL root cause fixed; vectored crash capture ready — 2026-09-29

Analysis of the previous test showed the diagnostic code had not actually been linked into the DLL that ran:
- NMAKE recompiled `main.cpp`;
- no `link.exe` step followed;
- proxy SHA-256 remained exactly `5508A0BE8D13CE694BEEEED14CB0F5ACFF30FF2D92AA7BD4233A5A0D9AD99CDC`;
- therefore `Release\spider.dll` was stale and the missing crash log was expected.

Fixes now active:
- `build.bat` supports `SPIDEY_FORCE_CLEAN`;
- `TEST_LATEST_BUILD.ps1` forces a clean matching build for every test;
- test runner verifies `Release\spider.dll` was freshly regenerated after build start;
- stale DLLs are rejected instead of staged;
- native crash logger now uses a first-priority vectored exception handler when available;
- vectored API is resolved dynamically for compatibility with the preserved Visual C++ 6 headers;
- handler only logs access violations;
- log records:
  - exception address/code;
  - read/write/execute operation;
  - invalid target address;
  - fault module/base/offset;
  - x86 registers;
  - 16 stack DWORDs;
- fallback top-level exception filter remains if vectored handlers are unavailable.

Relevant commits:
- `68a8192f1bf858152bbacf458f72280cd44d5d77` — forced-clean support;
- `4e805ffce4f42cc66213c820e7e60d35f80c53d4` — clean test build + fresh-DLL validation;
- `dcf07ccc811aff34939eaa3eabc8b162baff8add` — repaired VS6-compatible vectored crash handler;
- `b817c98657be1a477996cb2c145e47b08c0c9cfe` — reliable CLEAN failure propagation.

Static verification passed:
- no stale old handler symbol;
- no duplicated crash-handler tail;
- forced-clean path present once;
- fresh-DLL check present;
- fingerprint and crash-log collection paths present.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. The build should now visibly perform a full CLEAN, compile, and `link.exe`. The proxy SHA should differ from the stale `5508A0BE...` build. After the crash, return the launcher output and `spidey-decomp-crash.log` from the timestamped session folder.


## First actionable crash mapped — 2026-09-29

Clean rebuild/runtime diagnostics succeeded:
- fresh proxy SHA-256: `8279675802F6E838B9041BA66F9DE82B8FA678698BB26DE5BF318240E755BEE8`;
- exact EXE fingerprint unchanged;
- native crash log captured.

Crash:
- exception `0xC0000005`;
- fault address `0x00503AF7`;
- null read target `0x00000000`;
- mapped through `tools/names.json` to `DXSOUND_ShutDown()+0x7`;
- retail/reconstructed shutdown immediately dereferences `g_pDSBuffer->Stop()`, so this is a secondary cleanup crash caused by a null primary sound buffer.

Important control-flow finding:
- the game's DirectX error macros call `DXINIT_ShutDown()` on any failed HRESULT;
- that shutdown path reaches `DXSOUND_ShutDown()`;
- if failure occurs before `DXSOUND_Init()` creates `g_pDSBuffer`, cleanup itself crashes;
- observed register `EDI=0x80004001` is consistent with a preceding `E_NOTIMPL` HRESULT, but the exact first failing DirectX call is not yet proven.

**ACTIVE DIAGNOSTIC:** hook the retail error reporters at their mapped addresses:
- `displayDIError` 0x004FC240
- `displayDSError` 0x004FC630
- `displayD3DError` 0x004FC820

The wrappers will record HRESULT + original source file + original source line to `spidey-decomp-dxerror.log` before normal error handling continues.


## DirectX first-failure logger ready — 2026-09-29

Latest crash analysis:
- clean rebuild confirmed by visible full compile + link;
- fresh proxy SHA-256 `8279675802F6E838B9041BA66F9DE82B8FA678698BB26DE5BF318240E755BEE8`;
- crash captured at `0x00503AF7`;
- mapped to `DXSOUND_ShutDown()+0x7`;
- access type: read;
- target address: `0x00000000`;
- this is a secondary cleanup crash caused by null `g_pDSBuffer`.

DirectX error macros call global shutdown on failed HRESULTs, so the real bug is an earlier DirectX failure. The observed register value `0x80004001` suggests E_NOTIMPL but is not sufficient to identify the exact API call.

New diagnostics:
- retail `displayDIError` at `0x004FC240` hooked;
- retail `displayDSError` at `0x004FC630` hooked;
- retail `displayD3DError` at `0x004FC820` hooked;
- wrappers append kind/HRESULT/original source file/original source line to `spidey-decomp-dxerror.log`;
- normal reconstructed error display still runs afterward;
- latest-test launcher removes stale DirectX logs before launch and copies the new log into the timestamped session folder after exit.

Commits:
- `4bc1e3039f44032038db849e62cb1ea07dbe3d75` — DirectX first-failure hooks;
- `42a1f715ccc1222fb6118a7a20f5e4756e8543ec` — collect DirectX error log.

Static verification:
- each retail error address patched exactly once;
- logger path present;
- launcher cleanup/copy path present.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat`, then `TEST_LATEST_BUILD.bat`. Return the new `spidey-decomp-dxerror.log` plus the crash log and launcher output.


## Primary DirectX failure confirmed — caller-address diagnostic next — 2026-09-29

Latest test at revision `9cf5b139661f11bd613ef646d2e0f3e2a477ba52`:
- clean forced rebuild/link succeeded;
- proxy SHA-256 `3929CF3E440064D1846030A368D5409F2BD3A48B7CA493167F88A1FB5CC875EE`;
- EXE fingerprint unchanged;
- DirectX diagnostic captured:
  `D3D error=0x80004001 file=C:\backup\SpideyPC\SpideyPC\D3d\DXinit.cpp line=1005`;
- `0x80004001` is E_NOTIMPL;
- subsequent cleanup still faults at `DXSOUND_ShutDown()+0x7` reading address 0.

The embedded original source line cannot be mapped directly to the current reconstructed source because line numbering has diverged.

**ACTIVE NEXT DIAGNOSTIC:** replace the three DirectX error wrappers with x86 naked trampolines that preserve normal calling semantics while recording the retail return/call-site address. This will identify the exact instruction/API call that produced E_NOTIMPL.


## Exact DirectX call-site capture ready — 2026-09-29

The latest DirectX error log proves the primary failure:
- kind: D3D
- HRESULT: `0x80004001` (E_NOTIMPL)
- original source: `C:\backup\SpideyPC\SpideyPC\D3d\DXinit.cpp`
- original source line: `1005`
- secondary crash remains `DXSOUND_ShutDown()+0x7` null-reading `g_pDSBuffer`.

Because the reconstructed `DXinit.cpp` line numbering no longer matches the original Neversoft source, line 1005 cannot by itself identify the exact API call.

New diagnostic commit:
- `dcf0e7ce21468230de9b8c07b4d5d02c5e74d229`

Changes:
- DI/DS/D3D error wrappers are now x86 naked trampolines;
- trampolines capture the untouched retail return address directly from the entry stack;
- logger records:
  - HRESULT
  - original file/line
  - `caller_return`
  - probable direct-call site `caller_return - 5`;
- each trampoline restores the original stack exactly and tail-jumps to the normal reconstructed error display function;
- normal error/cleanup behavior is otherwise preserved.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. Return `spidey-decomp-dxerror.log`; its new caller/call-site fields should identify the exact failing DirectDraw/Direct3D instruction.


## Updater transport failure — 2026-09-29

Latest user attempt failed before build/test:
- UPDATE_SPIDEY_PROJECT.ps1 could not query the GitHub dev revision;
- PowerShell reported: "The underlying connection was closed: An unexpected error occurred on a send.";
- no source/build/runtime failure occurred.

**ACTIVE FIX:** make GitHub SHA lookup best-effort rather than mandatory:
1. retry GitHub API several times;
2. fall back to local Git `ls-remote` when available;
3. fall back to explicit Windows curl when available;
4. if exact remote SHA still cannot be resolved, continue by downloading/refeshing the dev branch archive and identify the local source state with the archive SHA-256 instead of aborting;
5. add retries/fallback for archive download too.

Because the currently installed updater cannot fetch its own fix when the API path fails, provide a minimal replacement ZIP containing only `tools\UPDATE_SPIDEY_PROJECT.ps1`.


### Hardened updater committed

Commit:
`af7c0e4cf915b0e7187ad6046cc42e1865c7a28a`

New updater behavior:
- retries GitHub commit API up to 3 times;
- falls back to `git ls-remote` when Git is available;
- falls back to Windows `curl.exe` when available;
- if exact commit SHA still cannot be resolved, does not abort;
- instead downloads the dev branch archive, refreshes the local tree, and records an `archive-<sha256-prefix>` source identity;
- archive download itself retries PowerShell transport and falls back to curl;
- normal exact-SHA tracking remains when GitHub revision lookup succeeds.

Because the installed updater can fail before fetching this change, a minimal replacement ZIP is being provided containing only:
`tools\UPDATE_SPIDEY_PROJECT.ps1`

**Next user action:** extract that ZIP into the local project root and overwrite the existing updater script, then run `UPDATE_SPIDEY_PROJECT.bat` followed by `TEST_LATEST_BUILD.bat`.


## Exact DirectX error call site captured — 2026-09-29

Latest test at revision `13b50f409ab2afccbb4846a9d5a1b34c082c6ea0`:
- forced clean build/link succeeded;
- proxy SHA-256 `F07384C326D25AA2E6551B5C9F8E21A2F65DEAFA18B6A03305643CB8843B90F7`;
- EXE fingerprint unchanged;
- primary DirectX error:
  - kind: D3D
  - HRESULT: `0x80004001` (E_NOTIMPL)
  - original source: `DXinit.cpp`
  - original source line: 1005
  - caller return: `0x004FFBB1`
  - probable direct call site: `0x004FFBAC`;
- secondary cleanup crash remains:
  - `DXSOUND_ShutDown()+0x7`
  - read from `0x00000000`.

**ACTIVE NEXT STEP:** map `0x004FFBAC` inside retail `initDirectDraw7()` to the exact DirectDraw/Direct3D method, then patch/tolerate that specific modern-Windows compatibility failure while preserving diagnostics.


## Runtime instruction-window dump added — 2026-09-29

Exact error reporter call site from latest run:
- caller return: `0x004FFBB1`
- displayD3DError call site: `0x004FFBAC`
- HRESULT: `0x80004001` (E_NOTIMPL)

The direct error-report call is not itself the failing COM API call; the failing DirectDraw/Direct3D vtable call occurs earlier in the same retail block.

Commit:
`e683078040b6e8e96a93aac5d91f1e8abb2f8a67`

The DX error logger now also records:
- `code_window_base = call_site - 0x60`
- 160 raw instruction bytes spanning 96 bytes before and 64 bytes after the error-report call.

This will allow offline disassembly of the exact retail code around the failure and identification of the failing COM method/vtable slot without requiring the user to upload the executable.

Static verification passed:
- code-window base field present;
- 160-byte dump present;
- structured exception guard present.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`, then return the new `spidey-decomp-dxerror.log`. No additional files should be necessary unless the fault changes.


## Root DirectDraw failure decoded — compatibility patch selected — 2026-09-29

The 160-byte runtime instruction window conclusively maps the primary failure:

Retail code:
- `0x004FFB72: call [vtable+0x50]`
  - `IDirectDraw7::SetCooperativeLevel(hwnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN)`
- `0x004FFB94: call [vtable+0x54]`
  - `IDirectDraw7::SetDisplayMode(width, height, bpp, refresh, flags)`
- return stored in EDI at `0x004FFB97`;
- failed HRESULT then reaches `displayD3DError` at `0x004FFBAC`.

Argument globals visible directly in the retail instruction stream:
- `0x006B78E4` = requested width;
- `0x006B78E8` = requested height;
- `0x006B78EC` = requested color depth;
- `0x006B7900` = retail `IDirectDraw7*`.

Thus the primary startup blocker is specifically `IDirectDraw7::SetDisplayMode` returning `DDERR_UNSUPPORTED / E_NOTIMPL`.

**ACTIVE FIX:** patch only the five-byte sequence at `0x004FFB94` (`FF 51 54 8B F8`) with a direct call to a compatibility thunk that:
1. calls the original `IDirectDraw7::SetDisplayMode` with the untouched requested parameters;
2. if it succeeds, preserves original behavior;
3. if it returns `DDERR_UNSUPPORTED` and requested bpp is 16, retries the same mode at 32 bpp;
4. on successful 32-bpp retry, updates retail `gColorCount` at `0x006B78EC` to 32;
5. returns the final HRESULT in EAX and mirrors the overwritten `mov edi,eax` behavior before resuming at `0x004FFB99`;
6. refuses to install if the expected original five bytes are not present;
7. logs both attempts/results for the test session.

This is deliberately narrower than forcing windowed mode or globally ignoring DirectDraw failures.


## First DirectDraw compatibility fix implemented — 2026-09-29

Root failure proven from retail runtime bytes:
- `IDirectDraw7::SetDisplayMode` call at `0x004FFB94`;
- requested mode is passed from retail globals:
  - width `0x006B78E4`
  - height `0x006B78E8`
  - bpp `0x006B78EC`;
- returned HRESULT `DDERR_UNSUPPORTED / E_NOTIMPL`.

Compatibility implementation commit:
`edab1663977c21afff7e2a051d0ee740e410097c`

Behavior:
- validates exact retail bytes at `0x004FFB94` are `FF 51 54 8B F8`;
- if bytes differ, refuses to install and logs the mismatch;
- replaces only those five bytes with a call to an x86 compatibility thunk;
- thunk calls the original `IDirectDraw7::SetDisplayMode` through the live retail COM object;
- original requested mode remains the first attempt;
- only when result is `DDERR_UNSUPPORTED` and requested bpp is 16:
  - retries same width/height/refresh/flags at 32 bpp;
  - if retry succeeds, writes 32 to retail `gColorCount` at `0x006B78EC`;
- thunk reproduces overwritten `mov edi,eax`;
- thunk performs original six-argument stdcall cleanup and resumes at `0x004FFB99`;
- instruction cache is flushed after patching;
- no global DirectDraw errors are ignored;
- no windowed-mode forcing is applied.

Compatibility log:
`spidey-decomp-compat.log`

Example expected successful line:
`SetDisplayMode 640x480x16 first=0x80004001 retry_bpp=32 retry=0x00000000`

Launcher collection commit:
`63298fb64d6281bd31681f99f041791cb82014b4`

Static verification passed:
- exact byte guard present;
- exact retail call site present;
- retry condition limited to unsupported 16-bpp mode;
- retail bpp global updated only after successful 32-bpp retry;
- EDI/result semantics restored;
- original 24-byte stdcall argument cleanup preserved;
- launcher removes stale compat logs and copies the new one into the timestamped test folder.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` then `TEST_LATEST_BUILD.bat`. If the game progresses farther, return the launcher output and all generated diagnostic logs. If it still exits at startup, `spidey-decomp-compat.log` is the primary file needed.


## 16->32 compatibility attempt did not clear startup failure — 2026-09-29

Latest test at revision `d86523d538a7b97ac32fac0d5b6748378a2be4a2`:
- forced clean build/link succeeded;
- proxy SHA-256 `7FC5396C91CA478E11A3DB95C565DC3F33C3B7BF2A16262C312A74F004CDDF16`;
- EXE fingerprint unchanged;
- live instruction window proves the compatibility patch installed at `0x004FFB94`:
  original `FF 51 54 8B F8` is now a direct `E8 rel32` call;
- nevertheless the same D3D `0x80004001` error reaches `displayD3DError`;
- secondary crash remains `DXSOUND_ShutDown()+0x7` null read;
- no `spidey-decomp-compat.log` was collected.

Interpretation:
- the compatibility thunk executed far enough to return an HRESULT into retail EDI;
- the returned HRESULT is still `0x80004001`;
- we do not yet know whether:
  1. requested bpp was not 16, so retry condition did not run; or
  2. 16->32 retry ran and also returned E_NOTIMPL.

**ACTIVE FIX/DIAGNOSTIC:** record SetDisplayMode arguments and first/retry HRESULTs in static runtime state inside the proxy, then append that state through the already-proven DirectX error logger. This avoids relying on a separate compatibility log file and will conclusively distinguish those two cases on the next run.


## SetDisplayMode attempt state now embedded in proven DX logger — 2026-09-29

Commit:
`cf319fed1a753cc16d91802b211b5681ae51b4fe`

Reason:
- the SetDisplayMode compatibility thunk is definitely installed in live retail code;
- the game still returns `0x80004001`;
- the separate `spidey-decomp-compat.log` did not appear, so it cannot be trusted as the sole diagnostic channel.

New runtime state captured by the proxy:
- whether compatibility helper executed;
- requested width;
- requested height;
- requested bpp;
- requested refresh;
- requested flags;
- first SetDisplayMode HRESULT;
- whether 32-bpp retry was attempted;
- retry HRESULT.

The already-working `spidey-decomp-dxerror.log` now prints that state as:
`compat_state width=... height=... bpp=... first=... retry_attempted=... retry=...`

It also independently reads the retail mode globals:
- `0x006B78E4` width
- `0x006B78E8` height
- `0x006B78EC` bpp

This makes the next test conclusive even if the standalone compat log is still absent.

Static verification:
- seen: PASS
- first: PASS
- retryFlag: PASS
- retryResult: PASS
- compatState: PASS
- retailGlobals: PASS

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`, then return only the new `spidey-decomp-dxerror.log` unless the crash behavior changes.


## Naked SetDisplayMode thunk proven unreliable — replacing with full-block helper — 2026-09-29

Latest uploaded logs:
- primary D3D error remains `0x80004001`;
- retail mode globals at failure are `640x480x16`;
- live retail code contains the installed direct `E8 rel32` compatibility call at the old SetDisplayMode site;
- however DX logger reports `compat_state not_seen`;
- secondary crash remains unchanged at `DXSOUND_ShutDown()+0x7`.

Conclusion:
- patch installation is proven;
- the current naked thunk/stack-forwarding path is not reaching the C++ compatibility helper correctly;
- do not infer that the 32-bpp retry itself failed, because the helper never recorded execution.

**ACTIVE FIX:** remove the naked forwarding thunk and replace the complete original 36-byte retail SetDisplayMode argument-setup/call/result block at `0x004FFB75..0x004FFB98` with:
1. a direct call to a normal zero-argument C++ helper;
2. helper reads retail globals directly:
   - lpDD `0x006B7900`
   - width `0x006B78E4`
   - height `0x006B78E8`
   - bpp `0x006B78EC`;
3. helper performs original SetDisplayMode call and conditional 16->32 retry;
4. patched retail block executes `mov edi,eax` after the helper call;
5. remaining bytes are NOP-filled through `0x004FFB98`;
6. exact original 36-byte sequence is validated before patching.

This removes all custom stack argument forwarding from the compatibility path.


## Full-block SetDisplayMode compatibility helper implemented — 2026-09-29

Commit:
`41f9a8b296b85ff77dfdac861ba640efcc5c1e47`

The previous naked forwarding thunk has been removed entirely.

New patch strategy:
- validates the exact original 36-byte retail sequence at `0x004FFB75..0x004FFB98`;
- that sequence covers:
  - loading retail bpp/width/height/lpDD globals;
  - pushing SetDisplayMode arguments;
  - indirect COM call through vtable slot +0x54;
  - `mov edi,eax`;
- replaces the whole sequence with:
  - `call SpideyCompatSetDisplayModeFromGlobals`;
  - `mov edi,eax`;
  - NOP padding through `0x004FFB98`;
- the normal C++ helper reads exact retail globals itself:
  - lpDD `0x006B7900`
  - width `0x006B78E4`
  - height `0x006B78E8`
  - bpp `0x006B78EC`;
- helper performs original SetDisplayMode call;
- if and only if first result is DDERR_UNSUPPORTED and bpp is 16, retries same mode at 32 bpp;
- successful 32-bpp retry updates retail bpp global to 32;
- helper attempt state remains embedded in the proven DX error logger.

Static verification passed:
- old naked thunk removed;
- zero-argument retail-global helper present;
- 36-byte exact signature guard present;
- direct helper call begins at 0x004FFB75;
- `mov edi,eax` restored immediately after helper call;
- remaining 29 bytes are NOP-filled;
- instruction cache flush covers all 36 bytes;
- DX logger still prints compatibility state.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. Return the new `spidey-decomp-dxerror.log`; if behavior changes or a new crash appears, return all generated logs.


## Full-block helper still not observed; switching to direct retail 32-bpp probe — 2026-09-29

User will provide all generated logs for every test going forward; treat the full log set as the standard test handoff.

Latest run at revision `f04d01c5ee21951abe2a10714fb9035c60f5c682`:
- clean build/link succeeded;
- proxy SHA-256 `57231C2CD56FDCA55E1673BEB0ADC6285D20640F52E9D54798273BBDC3AF4BAF`;
- EXE fingerprint unchanged;
- live instruction window proves the full 36-byte replacement is installed:
  - retail block now begins with direct `E8 rel32`;
  - followed by `mov edi,eax`;
  - remaining bytes are NOP-filled;
- DX logger nevertheless still reports `compat_state not_seen`;
- retail mode globals remain `640x480x16`;
- primary HRESULT remains `0x80004001`;
- secondary cleanup crash remains unchanged at `DXSOUND_ShutDown()+0x7`.

Conclusion:
- stop spending test cycles on the DLL helper/detour path;
- test the actual compatibility hypothesis using an in-place retail instruction edit with no cross-module call.

**ACTIVE FIX:** restore the original SetDisplayMode argument/call block and patch only its first six bytes:
- original: `8B 15 EC 78 6B 00` = `mov edx,[0x006B78EC]` (load requested bpp);
- replacement: `BA 20 00 00 00 90` = `mov edx,32; nop`.

All remaining retail instructions, argument pushes, COM vtable call, EDI assignment, and error handling stay untouched.

Purpose of this probe:
- conclusively determine whether 32-bpp SetDisplayMode works on the current Windows/DirectDraw stack;
- if the line-1005 error disappears or moves, 16-bpp mode switching is the compatibility blocker;
- if E_NOTIMPL remains at the same site, the blocker is SetDisplayMode/exclusive mode itself rather than color depth.


## Direct retail 32-bpp SetDisplayMode probe ready — 2026-09-29

Implementation commit:
`77253607b2c3fc3f3b058699d983a0d66e0bcd09`

The previous cross-module helper/detour path has been removed from this compatibility test.

Current probe:
- exact patch site: `0x004FFB75`;
- validates original bytes:
  `8B 15 EC 78 6B 00`
  = `mov edx,[0x006B78EC]`;
- replaces only those six bytes with:
  `BA 20 00 00 00 90`
  = `mov edx,32; nop`;
- original retail SetDisplayMode argument pushes remain intact;
- original retail `IDirectDraw7::SetDisplayMode` vtable call remains intact;
- original `mov edi,eax` remains intact;
- original DirectX error handling remains intact;
- no DLL helper is called by this probe.

Static verification passed:
- exact six-byte guard present;
- only bpp-load instruction is replaced;
- old full-block helper removed;
- old 36-byte patch removed;
- no direct helper call remains at 0x004FFB75;
- instruction cache flush covers six bytes;
- existing DX/crash diagnostics remain enabled.

Interpretation for next run:
- if `DXinit.cpp:1005 / 0x80004001` disappears or moves, 16-bpp SetDisplayMode is the compatibility problem;
- if the exact same error remains, exclusive SetDisplayMode itself is unsupported and the next fix should move to windowed/borderless DirectDraw initialization instead of color-depth retrying.

**Next user action:** update and rerun `TEST_LATEST_BUILD.bat`. User will provide all generated logs by default.


## 32-bpp retail probe still E_NOTIMPL — exclusive fullscreen path is the blocker — 2026-09-29

Latest test at revision `377991e26604e6fdaa23dda955a38a732ba05882`:
- clean forced rebuild/link succeeded;
- proxy SHA-256 `36434A263AD9921061F9DC4435A1DDF6F653D43215706A83C94C84F577258480`;
- EXE fingerprint unchanged;
- live retail code confirms the direct bpp patch installed:
  `BA 20 00 00 00 90` = `mov edx,32; nop`;
- the original retail SetDisplayMode vtable call remains intact;
- despite forcing 32 bpp, the exact same D3D error remains:
  - HRESULT `0x80004001` / DDERR_UNSUPPORTED / E_NOTIMPL
  - original `DXinit.cpp` line 1005
  - same error-report call site;
- secondary cleanup crash remains `DXSOUND_ShutDown()+0x7` null read.

Conclusion:
- 16-bit color depth is NOT the compatibility blocker;
- exclusive fullscreen `IDirectDraw7::SetDisplayMode` itself is unsupported/failing on this environment;
- stop testing alternate bpp values.

**ACTIVE NEXT STEP:** identify and patch the retail branch that selects the game's existing windowed DirectDraw path (`gDxOptionRelated`) instead of the exclusive fullscreen path. Reuse the game's own windowed surface/clipper code rather than suppressing SetDisplayMode errors or inventing a new renderer path.


## Built-in windowed DirectDraw route implemented — 2026-09-29

Implementation commit:
`36570947fa515636c9f3326282295c0e5fd2a37a`

Latest evidence:
- forcing the retail SetDisplayMode call to 32 bpp still produced the same `0x80004001` at the same original error site;
- therefore color depth is not the blocker;
- exclusive fullscreen SetDisplayMode is the compatibility failure.

Relevant reconstructed source:
- retail caller uses `DXINIT_DirectX8(hwnd, hInstance, 2)`;
- `DXINIT_DirectX8` computes `gDxOptionRelated = a3 & 1`;
- `initDirectDraw7` uses `gDxOptionRelated != 0` to select the game's own windowed DirectDraw path using `DDSCL_NORMAL`, primary/offscreen surfaces, and a clipper;
- changing the third DXINIT argument from 2 to 3 preserves bit 1 and enables bit 0.

Current patch strategy:
- no SetDisplayMode detour;
- no bpp forcing;
- scans retail .text `0x00401000..0x0053B000` for direct calls targeting retail `DXINIT_DirectX8` at `0x004FDE90`;
- within a bounded 20-byte window before each matching call, looks for exactly one `push 2` (`6A 02`);
- requires exactly one unambiguous call-site match in the whole text range;
- verifies the call still resolves to `0x004FDE90`;
- patches only the immediate byte from `2` to `3`;
- flushes instruction cache;
- logs push site/call site/target to `spidey-decomp-compat.log`;
- refuses to patch on ambiguity or verification failure.

Static verification passed:
- new windowed initializer installer present;
- previous SetDisplayMode installer removed;
- previous direct 32-bpp probe removed;
- target address correct;
- bounded scan + uniqueness check present;
- immediate changes only `02 -> 03`;
- launcher still collects compat, DX, crash, session, and fingerprint logs.

User preference:
- user will send all logs after every test; treat the complete log set as the standard test input.

**Next user action:** run `UPDATE_SPIDEY_PROJECT.bat` then `TEST_LATEST_BUILD.bat`, and provide all generated logs.


## MAJOR MILESTONE: reaches start screen; new crash is stack overflow — 2026-09-29

Latest test at revision `172735331305286580fc1b76e1859c680f4fa77b`:
- clean forced build/link succeeded;
- proxy SHA-256 `3858F003D643B50DECAE2BA985AFF1FA4100AB2B44FEF1B20E65E81F1824C81E`;
- EXE fingerprint unchanged;
- compatibility patch installed successfully:
  - push site `0x00515BA9`
  - call site `0x00515BAD`
  - target `DXINIT_DirectX8 = 0x004FDE90`
  - argument changed `2 -> 3`;
- game successfully passed DirectDraw initialization;
- all splash screens played;
- game reached the start/title screen;
- pressing Enter/Start then crashed.

New process exit code:
- decimal `-1073741571`
- NTSTATUS `0xC00000FD`
- **STATUS_STACK_OVERFLOW**

This is a new failure class and confirms the previous DirectDraw startup blocker is fixed/worked around.

No `spidey-decomp-crash.log` was produced because the current vectored handler only logs `EXCEPTION_ACCESS_VIOLATION`.

**ACTIVE NEXT STEP:**
1. extend native crash diagnostics to handle `STATUS_STACK_OVERFLOW`;
2. reserve emergency exception stack space early using `SetThreadStackGuarantee` when available;
3. make the stack-overflow logging path minimal/safe;
4. capture EIP/registers plus a bounded stack window at the overflow;
5. trace the title/start-screen transition in source to identify likely recursion/re-entry caused by an active reconstructed patch.


## Start-screen stack-overflow diagnostic frontier — 2026-09-29

Latest runtime milestone:
- game reaches the title/start screen successfully;
- pressing Enter/Start causes process exit `0xC00000FD` (stack overflow);
- previous DirectDraw startup failure is no longer the active blocker.

Diagnostics added:
- `53f5b503cedd7f07e80e924ab798eb57579eac52`: stack-overflow-aware native crash capture with reserved exception stack, extended stack dump, and EBP return-chain logging;
- `3750f34e62c2be7c161665194402af28b05150f3`: launcher labels `0xC00000FD` explicitly.

A source audit of the currently active reconstructed modules did not find the simplest direct self-recursion pattern.

Next test:
- update and rebuild;
- reach the title screen;
- press Enter once;
- provide all generated logs, especially `spidey-decomp-crash.log`.


## Stack overflow pinpointed at retail texture lookup — 2026-09-29

Latest crash log:
- exception: `0xC00000FD` (stack overflow);
- EIP / exception address: `0x004C9460`;
- ESP: `0x000C2000`;
- stack contains an extremely repetitive alternating pattern:
  - `0x1004D9DC`
  - `0xE90B5F6E`
  repeated throughout the captured window;
- EBP chain is unreadable because the stack is exhausted.

Important source correlation:
- reconstructed `Spool_FindTextureEntry(u32 checksum)` is currently `@SMALLTODO`;
- it explicitly calls retail address `0x004C9460` as a temporary fallback;
- `patch_spool()` patches nearby spool functions including `0x004C9430` and `0x004C95C0`, but not `0x004C9460` itself.

This strongly suggests the title/menu transition is entering a recursion/re-entry loop involving retail texture lookup and one of the reconstructed spool/texture functions.

ACTIVE NEXT STEP:
1. map retail `0x004C9460` in names/symbol data;
2. locate all calls/references to `0x004C9460`;
3. map proxy address/offset `0x1004D9DC` to a reconstructed function;
4. remove the recursion at its source rather than increasing stack size.


## Texture-lookup recursion fix implemented — 2026-09-29

Crash evidence:
- stack overflow occurs at retail `Spool_FindTextureEntry(u32)` = `0x004C9460`;
- captured stack repeats the same DLL return address `0x1004D9DC` and checksum `0xE90B5F6E`, consistent with recursive re-entry through the temporary retail fallback.

Source finding:
- reconstructed `Spool_FindTextureEntry(u32 checksum)` contained:
  `func_ptr func = (func_ptr)0x004C9460; return func(checksum);`
- immediately after that unreachable return, the full hash-table lookup implementation was already present;
- upstream currently contains the same temporary fallback, so no upstream fix exists to merge.

Fix commit:
- `d33111220ab81d2f6ad4b1597cac6f72f1597799`
- removes the retail `0x004C9460` call-through;
- activates the existing `TextureChecksumHashTable[checksum & 511]` traversal;
- preserves the existing default-texture behavior.

Future address-resolution tooling:
- `d5d70161b5cc4d5a99bcc8724f0cc947e01ace02`: Release linker now emits `Release\spider.map`;
- `9b407fb170283f3dc2c1daa9a9de266d2a3f62ba`: test launcher copies it into each session as `proxy-link-map.txt`.

Static verification passed:
- `0x004C9460` fallback is absent from the reconstructed checksum lookup;
- hash-table traversal is reachable;
- default texture fallback remains;
- Release linker has /MAP enabled;
- clean target removes stale map;
- session logger captures the linker map.

Next runtime test:
- update and run the latest build;
- let the game reach the title/menu normally;
- do not assume Enter is required; simply note whether it crashes on its own or after input;
- provide all generated logs, including the new `proxy-link-map.txt`.


## Stack overflow fixed; missing-texture path now exposes access violation — 2026-09-29

Latest test at revision `446608a2b34fea0a5153ee9f41e670134d8c6af5`:
- clean forced build/link succeeded;
- proxy SHA-256 `0D022AAAA2CF37FC96E2B19A1EBF1DA89446CEE49148403C4A04D1CE136ED513`;
- EXE fingerprint unchanged;
- windowed DirectDraw compatibility patch still installs correctly;
- title screen remains reachable;
- user reports the game says it cannot find a texture after Enter;
- previous stack overflow is gone;
- new crash is `0xC0000005` at DLL address `0x1004DB78`;
- access is a read from `0x00000004`;
- crash stack contains checksum `0xE90B5F6E`.

This is strong evidence that removing the retail `0x004C9460` call-through broke the recursion successfully and exposed the next real issue in the reconstructed texture-miss fallback.

ACTIVE NEXT STEP:
1. resolve `0x1004DB78` against the captured `proxy-link-map.txt`;
2. inspect the exact source operation at that symbol/offset;
3. harden the missing-texture fallback so an absent checksum does not dereference an unavailable default texture;
4. preserve logging of the missing checksum for later asset-table correctness work.


## Start-menu highlight texture lookup corrected for hybrid retail/DLL state — 2026-09-29

Latest runtime result:
- previous texture-lookup stack overflow is gone;
- pressing Enter reaches `PShell_DrawHighlight`;
- missing checksum observed in crash stack: `0xE90B5F6E`;
- new failure is `0xC0000005` inside DLL `Spool_FindTextureEntry(u32)` at `0x1004DB78`;
- access target is `0x00000004`, consistent with dereferencing a null `SAnimFrame*` to read `pTexture`;
- linker map resolves function start:
  - `Spool_FindTextureEntry(u32) = 0x1004DB30`
  - crash = function + `0x48`;
- retail caller `0x0047A59E` maps to `PShell_DrawHighlight + 0xE`.

Root hybrid-state issue:
- reconstructed `TextureChecksumHashTable[512]` is DLL-owned state;
- retail menu/asset loading is expected to populate the retail game's live hash table, not necessarily the DLL copy;
- reconstructed `gAnimTable[13]` is also DLL-owned and is explicitly zeroed by `Bit_Init()`;
- live retail animation table is already defined as `G_ANIM_TABLE = 0x0056EA64`;
- therefore the old default fallback `gAnimTable[13]->pTexture` is unsafe in this hybrid runtime.

Live texture-table inference:
- `TextureChecksumHashTable` is 512 pointers = `0x800` bytes;
- reconstructed declaration order places it immediately before live `G_LOWGRAPHICS = 0x006B78F8`;
- inferred base is therefore `0x006B70F8`;
- this base is NOT trusted blindly.

Implementation commit:
`0d3a720777cb35a0785839604ed7f21a14224ba3`

New runtime behavior:
1. inspect untouched retail `Spool_FindTextureEntry` at `0x004C9460` (known size 132 bytes);
2. search those retail bytes for absolute address `0x006B70F8`;
3. only if the retail function itself embeds that exact address, accept it as the verified live texture hash-table base;
4. search the verified retail table first;
5. search the DLL-owned reconstructed table second;
6. on genuine miss, use live `G_ANIM_TABLE[13]` as default first, then DLL `gAnimTable[13]` only if populated;
7. log resolver state and genuine misses to `spidey-decomp-compat.log`;
8. if the inferred base is not corroborated by retail code, do not use it and dump the full 132-byte retail function to the compat log for exact follow-up analysis;
9. string texture lookup now reads live `G_TEXTUREENTRIES` instead of the DLL copy.

Secondary hardening commit:
`7a28adb2e2d7b2b9efe1ec07cee7424086a51885`
- `Spool_TextureAccess` no longer directly dereferences DLL `gAnimTable[13]`;
- it uses the same safe default helper;
- if no default exists, returns `-1` instead of dereferencing null.

Static verification passed:
- live hash resolver present;
- inferred base must be corroborated by retail machine code;
- retail table is searched before DLL table;
- local table remains as secondary path;
- live retail animation table is preferred for defaults;
- no remaining direct `gAnimTable[13]->pTexture` dereferences in spool.cpp;
- string lookup uses live retail texture entries;
- existing linker-map generation and collection remain enabled.

Next runtime test:
- update and run latest build;
- reach start screen and press Enter;
- provide all generated logs;
- especially inspect `spidey-decomp-compat.log` for either:
  - `texture_hash_table verified retail_base=0x006B70F8`, or
  - `texture_hash_table UNRESOLVED ... retail_code=...`.


## Runtime assertion logging added for texture/menu diagnostics — 2026-09-29

Additional diagnostics:
- `776f2a8c3d57b750f8a2b2ddb2db054c7f6d3e74`
  - `DoAssert` now preserves varargs formatting with `_vsnprintf`;
  - console output now shows actual values instead of raw format strings;
  - failed assertions are appended to `spidey-decomp-runtime.log`.
- `84c98daaa30c0c23282c02c1a8c3882b7f45b5d7`
  - latest-test launcher removes stale runtime log;
  - copies fresh runtime log into the timestamped session;
  - reports it as `[RUNTIME]`.

Current test frontier:
- DirectDraw startup compatibility remains solved by the built-in windowed path;
- prior texture-lookup stack overflow is solved;
- current focus is start-menu highlight texture lookup after Enter;
- live retail texture hash-table resolver + safe retail default texture handling are implemented;
- next run should reveal whether `0x006B70F8` is corroborated by retail code and whether the menu highlight texture is found in the live table.

Next user action:
- run `UPDATE_SPIDEY_PROJECT.bat`;
- run `TEST_LATEST_BUILD.bat`;
- reach the start screen and press Enter;
- provide all generated logs, including:
  - `spidey-decomp-compat.log`
  - `spidey-decomp-runtime.log`
  - `spidey-decomp-crash.log` if present
  - `proxy-link-map.txt`
  - launcher output/session/fingerprint files.


## PLAYABLE MILESTONE + new priorities — 2026-09-29

User runtime report:
- game now boots through splash/title;
- user can enter the first level and control Spider-Man;
- user successfully quit back to main menu;
- **no sound at all** during current runtime;
- entering/changing Options crashes reproducibly;
- user provided two independent options-menu crash sessions;
- full modern controller support is now an explicit project requirement:
  - Xbox-style controller support;
  - analog stick/trigger handling;
  - configurable button mappings;
  - Xbox button/UI prompts.

Both options-menu crash logs agree on:
- exception: `0xC0000005`;
- exact retail EIP: `0x0043EB29`;
- fault module: `SpideyPC.exe`;
- first session read target `0x8A14244A`;
- second session read target `0x8A14270A`;
- both have `ESI=0x0043EAF0`;
- crash is therefore reproducible in the same retail function/path rather than a random heap fault.

DirectDraw compatibility remains active and game is now playable.

Texture compatibility observation from both sessions:
- inferred `0x006B70F8` hash-table base was NOT corroborated;
- retail machine code itself reveals an indexed absolute base operand `0x006AB934` in the lookup sequence;
- default texture resolves non-null (`0x006AD3C8`);
- current texture compatibility logging is very noisy and should be cleaned up after crash triage.

ACTIVE WORKSTREAMS:
1. map and fix options crash at `0x0043EB29`;
2. diagnose missing DirectSound/audio path without regressing gameplay;
3. design/implement modern XInput/Xbox controller layer with remapping and Xbox prompts after stability hooks are in place.


## Options crash root-cause fixed + exact retail texture table decoded — 2026-09-29

Options crash:
- both independent sessions faulted at retail `0x0043EB29`;
- `tools/names.json` maps function start `0x0043EAF0` to `Font::height(char*)`;
- crash offset: `Font::height + 0x39`;
- both linker maps show DLL return address `0x100237EC` immediately after the reconstructed `Font::height` wrapper calls retail;
- reconstructed wrapper incorrectly invoked the retail C++ instance method as a free function:
  `typedef i32 (*func_ptr)(char*); return func(txt);`
- this failed to pass the `Font* this` pointer in ECX.

Fix commit:
`a0a8aee2f2fcbd931c3d9eb80c8270da38711563`
- removes invalid retail call-through;
- uses already reconstructed implementation:
  `heightAboveBaseline(txt) + heightBelowBaseline(txt)`.

Texture resolver correction:
- retail `Spool_FindTextureEntry` bytes explicitly decode:
  - `mov eax,[eax*4 + 0x006AB934]` => exact live retail texture hash table base `0x006AB934`;
  - default-texture flag at `0x006B2F08`;
  - retail animation-table slot 13 at `0x0056EA98`;
- previous inferred `0x006B70F8` was correctly rejected by runtime validation.

Correction commit:
`b3af5be5087b6cf7a2003aaf70daeb0c621c24d7`
- live hash resolver now verifies and uses exact `0x006AB934`;
- reads live retail default-texture flag;
- throttles repeated identical texture-miss logging.

Both commits are implemented but not yet runtime-tested.

Current priorities:
1. runtime-test the Options fix while preserving first-level playability;
2. diagnose/fix total absence of audio;
3. add XInput/Xbox controller support through the existing PCINPUT/Pad abstraction, preserving remapping support and adding Xbox button prompts/UI.


## Audio diagnostics + first XInput/Xbox backend implemented — 2026-09-29

### Audio diagnostic implementation

Retail `DXSOUND_Init` machine code was decoded from the preserved function artifact and gives exact retail globals:
- `g_pDS = 0x006B7920`;
- `gDxSoundBuffers[128] = 0x006BBAD4`;
- `gDxSoundHolder[32] = 0x006BBD50`;
- `g_pDSBuffer = 0x006BBF1C`.

The retail function:
- creates the primary buffer through `g_pDS`;
- calls `SetVolume(0)`;
- starts the primary buffer looping;
- already reports failed DirectSound HRESULTs through the hooked DS error reporter.

`SDDXSoundHolder` is verified 12 bytes and its first field is `LPDIRECTSOUNDBUFFER pDSB`, so active-voice diagnostics read the correct field.

Commit:
`7d42a06b1c70f8c706748926a8bfb3f5754d68ff`

Diagnostic behavior:
- the existing `DXINIT_DirectX8` call-site compatibility patch now redirects through a wrapper that calls untouched retail `0x004FDE90` and logs DirectSound state afterward;
- direct retail calls to:
  - `SFX_Init = 0x004718B0`
  - `SFX_SpoolInLevelSFX = 0x004719B0`
  are redirected through behavior-preserving diagnostic wrappers;
- wrappers call the original retail function, then log:
  - retail DirectSound device pointer;
  - primary buffer pointer;
  - number of non-null loaded sample buffers;
  - number of active voice buffers;
- output: `spidey-decomp-audio.log`.

Launcher collection:
`d32ef72818aecd7c4747ad43b3fe9f8bee79e922`

No reconstructed audio subsystem has been substituted yet; this remains diagnostic-only to preserve the playable retail path.

### XInput / Xbox controller backend phase 1

Retail controller conventions were verified from `DXINPUT_PollController = 0x00501E50` machine code:
- X/Y axes use range `-1000..+1000`;
- POV uses DirectInput hundredths-of-degrees;
- button state semantics:
  - `0xFF` newly pressed;
  - `0x7F` held;
  - `0x80` newly released;
  - `0x00` idle.

Retail controller entrypoints:
- setup: `0x00501890`;
- poll: `0x00501E50`;
- get button state: `0x00501FB0`;
- setup FF: `0x00501FC0`;
- start FF: `0x005021A0`;
- stop FF: `0x005021E0`;
- get button count: `0x00502210`.

Implementation commit:
`568c9c20148a382c77c34e6c246afa9e556222f0`

Backend:
- dynamically loads `xinput1_4.dll`, then `xinput1_3.dll`, then `xinput9_1_0.dll`;
- scans users 0..3 and uses first connected XInput controller;
- keyboard/mouse path remains untouched when no controller exists;
- left stick converted to retail -1000..+1000 range with XInput deadzone;
- D-pad converted to retail POV angles;
- LT/RT exposed as remappable digital buttons with threshold;
- press/held/release states match retail encoding;
- XInput rumble integrated with existing force-feedback start/stop interface.

Stable Xbox button index mapping deliberately preserves the retail default action table:
- 0 = X
- 1 = A
- 2 = View
- 3 = B
- 4 = Y
- 5 = LT
- 6 = LB
- 7 = RT
- 8 = LS
- 9 = RB
- 10 = RS
- 11 = Menu
- 12..15 = D-pad U/D/L/R

This makes existing defaults map naturally:
- Smart Bomb -> X
- Jump -> A
- Crouch -> B
- Select Weapon -> Y
- shoulder/trigger actions -> LB/RB/RT
- Start -> Menu

Xbox configuration UI:
- retail `initActionMaps = 0x0050D0F0`;
- exact `sprintf("button %i")` call is at `0x0050D28C`;
- only that call is redirected to Xbox-name formatter;
- Options controller mappings show `A/B/X/Y/LB/RB/LT/RT/View/Menu/LS/RS` rather than generic button numbers.

Controller log:
- `spidey-decomp-controller.log`;
- records selected XInput DLL, rumble availability and connected user index.

Launcher collection:
`deee34e11ad38c85c4f22980d32dd212c775a40e`

### Controller feature scope still remaining

Phase 1 covers:
- XInput detection;
- left-stick movement;
- D-pad;
- Xbox face/shoulder/trigger/Menu/View/stick-click buttons;
- remapping through the existing game mapping system;
- Xbox labels in the controller configuration UI;
- rumble.

Still to implement after runtime validation:
- right-stick integration where appropriate for Spider-Man's camera/UI semantics;
- broader in-game Xbox prompt/icon replacement outside the controller configuration screen;
- persistence/UX edge-case testing across disconnect/reconnect and restored defaults.

### Next runtime test

Run latest update/build and verify:
1. title -> Options no longer crashes;
2. changing several Options values works;
3. first level still loads and remains playable;
4. note whether any sound is heard;
5. with Xbox/XInput controller connected:
   - left-stick movement;
   - A/B/X/Y;
   - LB/RB/RT;
   - Menu/Start;
   - D-pad/menu navigation;
   - controller remapping screen and Xbox labels;
   - rumble if encountered.

Return all logs. New important logs:
- `spidey-decomp-audio.log`
- `spidey-decomp-controller.log`
plus usual compat/runtime/crash/map/session/fingerprint logs.


## REGRESSION: black screen + proxy DLL crash — 2026-09-29

Runtime test of revision:
`8c167ecf781029226ae8cd36a422cf597861e905`

Observed by user:
- game launched to a black screen;
- never reached normal playable/title state;
- eventually crashed.

Crash log:
- exception `0xC0000005`;
- write access violation;
- fault address `0x1002C8D1`;
- write target `0x0000001F`;
- fault module is rebuilt proxy `binkw32.dll`;
- proxy base `0x10000000`;
- proxy offset `0x0002C8D1`.
This is a NEW regression in our DLL, not the previous retail Options crash at `0x0043EB29`.

Other evidence from same failed run:
- retail EXE fingerprint is unchanged;
- windowed DirectDraw compatibility patch installed;
- verified texture hash table `0x006AB934` still accepted;
- XInput DLL `xinput1_4.dll` loaded with rumble support;
- audio diagnostics show retail DirectSound device + primary buffer are valid;
- `SFX_Init` loaded 42 buffers;
- level/menu SFX spool raised this to 44 buffers;
- no active voices were observed at those diagnostic checkpoints;
- a D3D diagnostic fired with error value `0x00000004` from retail caller `0x004FDDD4` / probable call site `0x004FDDCF`, before the windowed compatibility state had been marked seen.

IMMEDIATE NEXT ACTION:
1. map proxy crash `0x1002C8D1` exactly through this run's link map;
2. identify which new change owns that instruction;
3. revert/fix only the crashing regression before further feature work;
4. preserve the audio evidence, because it already proves DirectSound initialization and bank loading are succeeding.


## Black-screen regression mapped + startup-active experiments parked — 2026-09-29

Exact crash mapping from uploaded current-build linker map:
- proxy crash: `0x1002C8D1`;
- current `DCMem_New` start: `0x1002C880`;
- fault offset: `DCMem_New + 0x51`;
- stack return `0x10037B33`;
- current `PCTex_CreateTexture256` start: `0x100379A0`;
- caller offset: `PCTex_CreateTexture256 + 0x193`.

The source and original retail machine code establish the failure mechanism:
- `PCTex_CreateTexture256` allocates its temporary converted texture buffer with
  `DCMem_New(2 * rounded_width * rounded_height, 0, 1, 0, 1)`;
- `DCMem_New` calls `Mem_CoreNew`;
- `DCMem_New` has no null check before calculating its alignment result;
- if the underlying allocation returns null, the aligned result becomes `0x20` and it writes the alignment byte to `0x1F`;
- current crash log is exactly a write AV to `0x0000001F`.

This identifies the immediate fault but does NOT yet prove why the underlying texture allocation failed.

Important regression-scope facts:
- `PCTex_CreateTexture256` and `DCMem_New` were already active in the previously playable build;
- therefore the crash is most likely an exposed consequence of one of the newly activated startup paths rather than a newly introduced allocator implementation;
- audio diagnostics already established that DirectSound initialized correctly and sample banks loaded before the crash;
- XInput DLL loaded but no controller connection was logged.

Stability rollback / bisect commits:

`50e5ea75f25e20c7792b7ba320c00697b7b8aceb`
- keeps exact decoded retail texture table address `0x006AB934` and verification;
- disables runtime consumption of that table for now;
- restores prior DLL-owned default-texture gating behavior;
- reason: previous playable build had the retail table unresolved, while the failed build was first to actively consume the decoded retail table.

`7eef8ba810b23727a976dfe593849c7c3114834b`
- restores direct retail `DXINIT_DirectX8` call flow;
- keeps only the proven RealWinMain argument patch `2 -> 3`;
- disables active SFX diagnostic call redirections now that their evidence has been captured;
- parks XInput/Xbox runtime hooks and Options label hook;
- XInput/audio implementation remains compiled in source for later one-at-a-time reactivation.

The Options `Font::height` fix remains ACTIVE.

NEXT TEST PURPOSE:
- confirm known-good title/gameplay startup is restored;
- test Options crash fix independently of texture/audio/controller experiments.

If startup is restored:
1. test entering Options and changing settings;
2. confirm first level still plays;
3. audio is expected to remain unresolved for this isolation run;
4. controller phase 1 is intentionally inactive for this isolation run.

If the same `DCMem_New/PCTex_CreateTexture256` crash persists after this rollback, next step is to add narrowly scoped allocation telemetry around the exact PCTex buffer request and game-heap state.


## Black screen persists after startup-hook rollback — 2026-09-29

Runtime test of revision:
`4e4f2b1ddc706b49237512669abcc92ff9b92238`

Observed:
- game window appears;
- screen remains completely black;
- no sound;
- no automatic crash;
- process remains alive/hung until user force-closes it in Task Manager.

Uploaded evidence:
- retail EXE fingerprint unchanged;
- windowed DirectDraw arg patch installs at the expected retail RealWinMain call;
- retail texture table address `0x006AB934` verifies, but runtime consumption is disabled (`runtime_use=0`);
- one texture miss is logged for checksum `0xE90B5F6E`, falling back to non-null default texture `0x006AD3C8`;
- no crash log exists for this run because the process did not fault.

Conclusion:
- black-screen regression is NOT caused solely by the parked XInput hooks, parked audio wrappers, or active consumption of the retail texture hash table;
- regression predates those changes and must be isolated against the last user-confirmed playable revision rather than by further speculative patches.

Immediate next action:
1. identify exact last revision/run that reached title + first level;
2. diff startup-affecting code from that revision to current dev;
3. restore/bisect only those deltas;
4. do not add new feature work until title/gameplay baseline is recovered.


## Exact confirmed-playable runtime restored; Options fix reduced to one trampoline — 2026-09-29

Historical evidence lookup:
- both user-provided Options-crash sessions were built from revision
  `35e73ed3c4ca8c06b581df83f0f0913f0c915982`;
- that is therefore the exact last user-confirmed playable runtime:
  - splash/title visible;
  - first level entered;
  - Spider-Man controllable;
  - return to main menu worked;
  - only entering Options crashed.

Diff from that exact baseline to the black-screen tree showed only three runtime C++ files changed:
- `main.cpp`;
- `spool.cpp`;
- `FontTools.cpp`.
Other differences were docs and launcher logging only.

Recovery commits:
- `ae7bb3432ecfd03bfa6ebbd8e3122465755f64eb`
  - restores `main.cpp` byte-for-byte from confirmed playable `35e73ed...`;
  - removes all active/inactive audio/XInput experiment code from the current runtime file for this isolation build.
- `2dbb4d896280e82ad921f701213926669a483e9c`
  - restores `spool.cpp` byte-for-byte from confirmed playable `35e73ed...`;
  - returns texture lookup/default behavior exactly to the runtime that was known playable.
- `b8bc0957721c29499737741f1fd9c31740e7cf49`
  - starts from confirmed-playable `FontTools.cpp`;
  - changes ONLY the broken `Font::height` retail trampoline;
  - old broken declaration:
    `typedef i32 (*func_ptr)(char*);`
  - new declaration:
    `typedef i32 (FASTCALL *func_ptr)(Font*, void*, char*);`
  - call:
    `return func(this, 0, txt);`
  - rationale: x86 C++ instance method needs `this` in ECX; this mirrors the already-proven `Font::width` retail trampoline pattern and preserves retail behavior rather than substituting reconstructed height logic.

Mechanical verification:
- current `main.cpp` == exact contents at playable `35e73ed...`: PASS;
- current `spool.cpp` == exact contents at playable `35e73ed...`: PASS;
- old broken Font::height free-function trampoline absent: PASS;
- FASTCALL Font*/dummy-EDX trampoline present: PASS;
- compare against playable baseline now shows runtime-code difference ONLY in `FontTools.cpp`;
- remaining non-runtime differences are documentation and test-log collection.

NEXT TEST:
1. update/build;
2. verify splash/title returns;
3. if title returns, enter Options and change settings;
4. load first level once;
5. send all logs.

Interpretation:
- if black screen STILL occurs, then the single Font::height trampoline change itself is implicated and should be reverted for a pure baseline confirmation;
- if startup returns, the prior black-screen regression was in the post-playable main/spool experiment set and is now eliminated;
- audio/controller feature work remains paused until the playable baseline is reconfirmed.


## Black screen persists with only Font::height differing from confirmed playable runtime — 2026-09-29

Latest tested revision:
`d6c077ff95193cf033ea310169e10d24777a275f`

User result:
- black screen;
- no sound;
- no automatic crash reported.

Uploaded evidence:
- retail EXE fingerprint remains unchanged:
  SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`;
- DirectDraw windowed compatibility patch still installs at:
  - push `0x00515BA9`
  - call `0x00515BAD`
  - target `0x004FDE90`;
- texture behavior matches the confirmed-playable-era implementation:
  - inferred retail base `0x006B70F8` remains unresolved;
  - checksum `0xE90B5F6E` repeatedly misses;
  - non-null default texture `0x006AD3C8` is returned.

At this revision:
- `main.cpp` is byte-for-byte equal to confirmed-playable `35e73ed...`;
- `spool.cpp` is byte-for-byte equal to confirmed-playable `35e73ed...`;
- the only remaining runtime C++ difference is the isolated `Font::height` FASTCALL trampoline.

NEXT ACTION:
- restore `FontTools.cpp` exactly from confirmed-playable `35e73ed...`;
- also restore the test runner from that exact revision to remove non-runtime launcher/logging deltas from the control test;
- perform a PURE BASELINE test with no runtime code differences from the user-confirmed playable revision.

If that pure baseline still black-screens, source regression is ruled out and investigation must move to local/environment state (game config, preserved retail Bink DLL, generated state/files, registry/settings, or other installation differences).


## PURE confirmed-playable baseline prepared — 2026-09-29

Control revision source:
`35e73ed3c4ca8c06b581df83f0f0913f0c915982`

This is the exact revision from both user sessions that:
- reached splash/title;
- entered first level;
- allowed Spider-Man control;
- returned to main menu;
- crashed only when entering Options.

Pure-baseline restoration:
- `a3acab00d81c22d63732a190846bb8aa03b4fce3`
  - restores `FontTools.cpp` exactly from `35e73ed...`;
  - removes the isolated Font::height experiment entirely.
- `00b2165e8fc792c900d29416fa165a078e01f8b6`
  - restores `tools/TEST_LATEST_BUILD.ps1` exactly from `35e73ed...`.

Mechanical GitHub comparison against `35e73ed...` now reports:
- NO runtime-source differences;
- NO test-runner differences;
- ONLY `docs/CURRENT_STATUS.md` differs.

Therefore the next test is a true source/runtime control test.

Parallel external-state investigation:
- retail `SPIDEYDX_LoadSettings = 0x00515680` is real and reads persistent settings before startup;
- if pure baseline still black-screens, likely causes move outside current source tree:
  - persistent game/settings state;
  - preserved retail `binkw32_.dll` contents;
  - local installation/generated data state;
  - graphics/runtime/driver state;
  - other external environment changes.
- do NOT resume Options/audio/controller feature work until this control test result is known.

NEXT TEST:
- run updater;
- run latest test;
- no feature validation needed;
- report only whether splash/title returns or remains black, plus all logs as usual.


## Pure baseline runs with audio/input but renders black — 2026-09-30

Latest tested revision:
`f24a5d1b734a33e60d476ddcc43c34b8cb7d7643`

This revision is mechanically identical to the last user-confirmed playable runtime `35e73ed3c4ca8c06b581df83f0f0913f0c915982` except for documentation.

User result:
- game boots;
- audio is audible again;
- Start input works;
- user can enter the main menu after pressing Start;
- screen remains completely black throughout;
- no crash reported.

Uploaded evidence:
- retail EXE fingerprint remains unchanged:
  SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`;
- windowed DirectDraw compatibility patch installs at the expected retail caller:
  - push `0x00515BA9`
  - call `0x00515BAD`
  - target `DXINIT_DirectX8 = 0x004FDE90`
  - argument `2 -> 3`;
- no crash evidence.

Conclusion:
- source regression is ruled out by the pure-baseline control;
- game logic, input, audio, and shell progression are functioning;
- active failure is specifically visible presentation/render output;
- investigate the existing retail windowed DirectDraw path:
  1. primary/front surface creation;
  2. offscreen/back/scene surface creation;
  3. clipper/window association;
  4. final Blt/Flip/present call and its HRESULT;
  5. source/destination rectangles and surface-loss state.

Do not resume Options/audio/controller feature work until visible rendering is restored.


## Windowed presentation probe implemented — 2026-09-30

Current observed runtime:
- pure confirmed-playable source runs;
- audio is audible;
- Start input works;
- main-menu state advances;
- visible output remains black.

Retail render/present path decoded:
- `DXPOLY_EndScene = 0x00502A40`;
- when presentation is requested, retail calls:
  `DXPOLY_Flip = 0x00502990`;
- exact call site:
  `0x00502D41`
  with original bytes:
  `E8 4A FC FF FF`;
- windowed `DXPOLY_Flip` checks `gDxOptionRelated = 0x006B78F4`;
- windowed path performs:
  primary surface `0x006B7904`
  `Blt(gRect, scene surface 0x006B7908, ... DDBLT_WAIT ...)`;
- stored destination rectangle is `gRect = 0x006B5958`;
- retail game HWND is `0x006B58D0`;
- retail resolution globals used for diagnostics:
  - width `0x02E096F8`
  - height `0x02E0970C`
  - bpp `0x02E098E4`;
- low-graphics flag: `0x006B78F8`.

Implementation commit:
`6db8ea90d2e6ebe27aa61a5eeaccdc890674915f`

Probe behavior:
1. exact-byte guard verifies retail call site and target;
2. replaces ONLY the direct call at `0x00502D41`;
3. wrapper calls untouched retail `DXPOLY_Flip(0x00502990)`;
4. before present:
   - reads current client rect;
   - converts it to screen coordinates;
   - compares against stored retail `gRect`;
   - if stale/different, refreshes `gRect` before the retail Blt;
   - logs whether correction occurred;
   - records scene-surface pointer, dimensions, pitch, bpp, caps, loss state;
   - samples a 3x3 pixel grid from the scene surface and records hash/non-black count;
5. after retail present:
   - records primary-surface state;
   - samples a 3x3 pixel grid over the current destination rectangle;
6. logs frames 1..5, every 120th frame, and every frame where the destination rectangle is corrected.

Output:
`spidey-decomp-present.log`

Launcher collection commit:
`938229983c79c8d91436f54c9d07a13b90bd3ef8`

Static verification passed:
- exact call-site guard present;
- retail flip entry itself is not patched;
- wrapper always returns through untouched retail flip;
- destination rectangle refresh is bounded to a real current client rectangle;
- scene/primary samples enabled;
- log cadence is sparse;
- launcher clears and captures fresh presentation log.

Interpretation of next run:
- scene non-black + primary non-black => DirectDraw rendering and Blt work; investigate desktop/window composition/visibility;
- scene non-black + primary black => final Blt/presentation failure despite no reported HRESULT;
- scene black => rendering into offscreen scene surface is failing/being cleared;
- `corrected=1` followed by visible output => stale window destination rectangle was the compatibility bug.

Next user action:
- update/build;
- launch normally;
- if still black, let it run through splash/start/menu for at least ~10 seconds;
- send all logs, especially `spidey-decomp-present.log`.


## PRESENTATION ROOT CAUSE ISOLATED — scene renders, primary never updates — 2026-09-30

Latest test revision:
`527aa0ba2fea79865d4b06e683defd2e59a169cc`

User result:
- game remains visually a black box;
- audio/game state continue to run underneath.

Presentation probe evidence:
- probe installed successfully at retail `DXPOLY_EndScene -> DXPOLY_Flip` call site `0x00502D41`;
- retail windowed state:
  - option=1
  - lowgfx=0;
- stored destination rect and live client rect agree exactly:
  `0,0,640,480`;
- therefore stale rectangle is ruled out;
- scene surface:
  - ptr non-null;
  - not lost;
  - 640x480;
  - 32bpp;
  - GetDC succeeds;
  - 3x3 sample reports 9/9 non-black pixels;
- primary surface:
  - ptr non-null;
  - not lost;
  - 1920x1080;
  - 32bpp;
  - GetDC succeeds;
  - 3x3 sample reports 9/9 non-black pixels.

CRITICAL TEMPORAL EVIDENCE:
- scene sample hash changes from
  `0x7E0B5BDA`
  to
  `0x3B302417`
  by frame 360, proving the game is continuing to render changing frames;
- primary sample hash remains
  `0x3565BD06`
  on every sampled frame through frame 480;
- therefore the retail windowed presentation path is not propagating the rendered scene to the visible primary/display output.

Conclusion:
- game rendering itself is working;
- game loop/audio/input are working;
- stale gRect is ruled out;
- active compatibility defect is specifically the retail DirectDraw primary-surface windowed Blt/presentation behavior on this system.

NEXT IMPLEMENTATION:
- preserve retail renderer and offscreen scene surface;
- preserve retail DXPOLY_Flip call for state/error behavior;
- add a windowed compatibility presenter that copies the already-rendered scene surface directly into the HWND client DC after retail flip;
- use scene-surface GetDC + window GetDC + StretchBlt/BitBlt;
- only activate when gDxOptionRelated indicates windowed mode;
- log copy dimensions and Win32 result;
- leave fullscreen retail path untouched.


## Direct-window compatibility presenter implemented — 2026-09-30

New runtime evidence from `spidey-decomp-present.log` proves:
- scene surface contains non-black pixels from frame 1 onward;
- scene sample hash changes during runtime, so rendered content is updating;
- primary surface sample hash remains constant across all sampled frames;
- stored and live client rects both remain `0,0,640,480`;
- therefore stale rectangle is ruled out;
- failure is specifically the retail windowed DirectDraw primary-surface presentation step.

Implementation commit:
`cf3d827a11958464c73a2ac8a1ee1d1532a03d6a`

Compatibility behavior:
1. existing exact-call-site wrapper still invokes untouched retail `DXPOLY_Flip(0x00502990)` first;
2. only when retail `gDxOptionRelated` indicates windowed mode:
   - gets the already-rendered scene surface at retail `0x006B7908`;
   - reads actual HWND client dimensions;
   - gets a GDI DC from the scene surface;
   - gets the real HWND client DC;
   - uses `BitBlt` when scene/client sizes match;
   - uses `StretchBlt(COLORONCOLOR)` when sizes differ;
   - calls `GdiFlush`;
   - releases both DCs;
3. fullscreen path is untouched;
4. original renderer, D3D device, scene surface and retail flip still run normally.

Current observed dimensions make the common path:
- scene = 640x480;
- HWND client = 640x480;
- therefore direct `BitBlt`.

Presentation log now also records:
`compat_present frame=<n> result=<0/1> error=<win32> src=<w>x<h> dst=<w>x<h> stretch=<0/1>`

Static verification passed:
- retail flip occurs before compatibility copy;
- windowed guard present;
- source is scene surface, not primary;
- BitBlt + StretchBlt fallback present;
- fullscreen untouched;
- result logging present.

NEXT TEST:
- update/build;
- launch normally;
- if image appears, verify splash/title/menu visibility;
- if still black, let it run at least 10 seconds and provide all logs;
- key line will be `compat_present ... result=...` in `spidey-decomp-present.log`.


## Presentation probe result: scene renders, visible presentation path is broken — 2026-09-30

Latest tested revision:
`527aa0ba2fea79865d4b06e683defd2e59a169cc`

User result:
- game remains a black visible window/box;
- game audio is audible;
- game continues accepting input and advancing state underneath the black output.

Presentation probe evidence:
- probe installed successfully at retail `DXPOLY_EndScene -> DXPOLY_Flip` call site `0x00502D41`;
- retail flip target remains `0x00502990`;
- windowed mode flag is active;
- HWND is valid;
- stored and live client rectangles both remain `0,0,640,480`;
- no stale-rectangle correction was needed;
- offscreen scene surface:
  - valid pointer;
  - `640x480`;
  - 32 bpp;
  - not lost;
  - GetDC succeeds;
  - all sampled points are non-black;
- primary DirectDraw surface:
  - valid pointer;
  - desktop-sized `1920x1080`;
  - 32 bpp;
  - not lost;
  - GetDC succeeds;
  - all sampled points are non-black.

Critical temporal result:
- scene sample hash is initially `0x7E0B5BDA`;
- by frames 360/480 it changes to `0x3B302417`, proving rendered scene content changes over time;
- primary sample hash remains frozen at `0x3565BD06` across frames 1..480;
- therefore the game renderer is generating changing visible pixels in the offscreen scene surface, but the changing scene is not reaching the user-visible window through the legacy DirectDraw primary-surface path.

Conclusion:
- rendering generation is working;
- game logic/input/audio are working;
- the black-window bug is isolated to presentation/composition after the offscreen scene surface;
- stale `gRect` is ruled out;
- surface-loss is ruled out;
- source scene being black is ruled out;
- do NOT modify gameplay, texture generation, D3D scene rendering, audio, or input while fixing this.

**NEXT FRONTIER / RECOMMENDED NEXT ACTION:**
Implement a narrow compatibility presenter that bypasses the legacy DirectDraw primary-surface/DWM path:
1. keep retail rendering into `g_pDDS_Scene` unchanged;
2. after retail `DXPOLY_Flip` (or instead of its windowed primary Blt), acquire the scene surface DC or lock/read the 32-bpp scene surface;
3. present those already-rendered pixels directly to the actual game HWND using a controlled GDI path (`BitBlt`/compatible DC or `StretchDIBits`);
4. first implement as a diagnostic compatibility probe, not a renderer rewrite;
5. if the image appears, classify the bug as modern-Windows/DWM incompatibility with legacy DirectDraw primary-surface presentation and retain the direct HWND presenter as the compatibility solution;
6. once visible rendering is restored, resume the parked Options fix, audio follow-up, and XInput/Xbox controller work one at a time.

Do not spend another cycle on DirectDraw SetDisplayMode, bpp, texture lookup, or scene rendering before trying the direct HWND presentation probe.


## New-chat recovery checkpoint — 2026-09-29

Recovery sources checked:
- full disconnect-safe handoff ZIP manifest: PASS;
- prior Spider-Man 2000 project conversation context recovered;
- live GitHub branch `dev` confirmed;
- current branch head before this checkpoint: `974fbf073a9134de63f9d0102508bfcdb19a8805`;
- latest archived presentation test remains revision `527aa0ba2fea79865d4b06e683defd2e59a169cc`.

Important correction to the handoff wording:
- the direct-to-HWND compatibility presenter is **already implemented** in ancestor commit
  `cf3d827a11958464c73a2ac8a1ee1d1532a03d6a`;
- it is present in the current `dev` history through documentation checkpoint
  `6969ecf9c5290cee586cf0db4c6c4c922df4b0dd`;
- the archived latest runtime logs predate that implementation, so there is no runtime result for the direct-window presenter yet.

Therefore the exact current frontier is **runtime validation**, not reimplementation.

NEXT TEST:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. observe whether splash/title/menu become visible;
4. if still black, leave the game running at least ~10 seconds;
5. return all generated logs, especially `spidey-decomp-present.log`;
6. key evidence is the new line:
   `compat_present frame=<n> result=<0/1> error=<win32> src=<w>x<h> dst=<w>x<h> stretch=<0/1>`.

Do not redo SetDisplayMode/bpp, texture-table, stale-rectangle, scene-black, or surface-loss investigation before this test.


### Authoritative retail Google Drive source

Retail PC game source folder:
https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

Verified direct folder inventory includes:
- `SpideyPC.exe` — 1,507,328 bytes;
- `binkw32.dll` — original retail Bink DLL;
- `data.pkr`;
- `media.pkr`;
- `texture.dat`;
- setup/support binaries;
- `Docs` and `Uninstall` folders.

This Drive folder is reference/input material only. Do not commit retail binaries or PKR assets to Git.


## Runtime result: direct presenter works for menu; mouse crash + resolution/splash follow-up — 2026-09-29

Tested revision:
`a74074b77fe18adb254cd1a5b67c448a81b4d3cd`

User-visible result:
- splash screens remain black;
- once the game reaches the start menu, the image becomes visible;
- moving the mouse causes an immediate crash;
- resolution cannot currently be changed as desired;
- user explicitly requires native 2560x1440 (1440p) support.

Presentation evidence:
- direct HWND presenter is executing successfully:
  `compat_present ... result=1 error=0 src=640x480 dst=640x480 stretch=0`;
- scene surface remains 640x480 / 32 bpp;
- window client remains 640x480;
- retail resolution globals report 1280x1024 / 32 bpp during this run;
- therefore the direct GDI compatibility presenter is sufficient to expose normal shell/menu scene rendering, but splash/movie presentation uses a different path and is not yet handled by this presenter.

Crash evidence:
- exception: `0xC0000005`;
- retail EIP: `0x0043EB29`;
- this is the exact previously identified `Font::height(char*) + 0x39` failure site;
- current build's DLL stack return `0x100236FC` maps to reconstructed `Font::height(char*)` at `0x100236F0 + 0xC`;
- another return `0x1002CC0F` maps to `Mess_TextHeight + 0xF`;
- conclusion: mouse movement is driving a shell/UI text-height path and reproducing the same broken retail C++ instance-method trampoline that previously crashed Options.

Immediate implementation order:
1. restore the isolated, previously prepared FASTCALL `Font::height` trampoline fix that passes `this` in ECX;
2. inspect resolution enumeration/storage/surface creation and add native 2560x1440 support without hardcoding only the presenter;
3. trace the Bink/splash presentation path separately from the normal scene presenter so movies become visible too.

Do not regress the now-working direct HWND menu presentation path.


## Compatibility fix batch ready: mouse crash + splash movies + native modern resolutions — 2026-09-29

Implementation commits:
- `0addc001023f61886c34c33c50a7ae314f09a11f`
  - fixes reconstructed `Font::height(char*)` trampoline;
  - retail method is now invoked with FASTCALL-compatible `this` in ECX;
  - directly addresses the latest mouse-move crash at retail `0x0043EB29`.
- `23a5e77e51313edf52e06d9aa3904cc6c86ade78`
  - adds movie/splash presentation compatibility;
  - scans retail `PCMOVIE_NextFrame 0x0050B5A0..0x0050B790`;
  - verified retail machine code contains exactly one direct `DXPOLY_Flip` call at `0x0050B71A`;
  - redirects that movie-specific call through the already working direct-HWND presenter;
  - does NOT globally replace `DXPOLY_Flip`.
  - adds modern-resolution restoration/injection:
    - saved settings globals: `0x02E096F8/0x02E0970C/0x02E098E4`;
    - live DX globals: `0x006B78E4/0x006B78E8/0x006B78EC`;
    - retail game-resolution mirrors: `0x00568154/0x00568158`;
    - retail display-mode context:
      - count `0x006B5998`;
      - surfaces `0x006B599C`;
      - flags `0x006B789C`.
  - startup wrapper restores saved resolution into the actual live render globals before retail DX initialization.
  - imports modern 32-bit Windows display modes and explicitly guarantees a `2560x1440x32` entry.
- `562b5a27ba7f21a740a431ea0f73c55a99afdea3`
  - mode augmentation is now applied after every retail `initDirectDraw7` call, not only first startup;
  - this is necessary because retail `DXINIT_SetDisplayOptions` rebuilds DirectDraw and would otherwise wipe the augmented modern mode table.
- `778b60ff68a63be4be1736e04cefb365689d937a`
  - presentation diagnostics now distinguish:
    - `saved_res=<w>x<h>x<bpp>`;
    - `live_res=<w>x<h>x<bpp>`;
  - scene-surface dimensions remain independently logged.

Static/reverse-engineering checks completed:
- uploaded crash DLL return `0x100236FC` maps to `Font::height + 0xC`;
- uploaded secondary return `0x1002CC0F` maps to `Mess_TextHeight + 0xF`;
- retail `PCMOVIE_NextFrame` contains one direct flip call:
  `0x0050B71A -> 0x00502990`;
- retail `DXINIT_SetDisplayOptions` contains a direct reinit call:
  `0x005006B0 -> initDirectDraw7 0x004FEDD0`;
- retail `DXINIT_DirectX8` contains two direct `initDirectDraw7` calls:
  `0x004FDED4` and `0x004FDEFE`;
- all modern-mode entries use the retail-valid flag plus accelerated-resolution flag (`1 | 4`);
- 2560x1440 is an actual render-mode entry, not only presenter scaling.

NEXT TEST:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. verify splash/legal/logo movies are now visible;
4. at start menu, move the mouse around repeatedly and confirm no crash;
5. enter Options -> Display/Video;
6. verify modern resolutions appear, including `2560x1440`;
7. select/apply 2560x1440;
8. return to menu/game and verify the image is still visible;
9. provide all generated logs.

Most important next-log evidence:
- `spidey-decomp-compat.log`
  - `restore_saved_resolution ...`
  - `modern_mode_reinit patched_calls=...`
  - `modern_modes before=... after=... added=... windows_1440=...`
- `spidey-decomp-present.log`
  - `movie_present installed ...`
  - `saved_res=...`
  - `live_res=...`
  - `scene_pre ... width=2560 height=1440 ...` after 1440p is applied.

If 2560x1440 appears in the menu but applying it fails, the next target is the retail `DXINIT_SetDisplayOptions` transition itself; do not regress mode enumeration or fall back to scaled 640x480.


## Runtime result: native 1280x1024 works; borderless collapse + game-heap exhaustion before menu — 2026-09-29

Tested revision:
`07c84285d214aad27470b6460dda1a6fabb0b908`

User-visible result:
- startup begins at the larger monitor/window aspect;
- DirectDraw initialization then collapses the game into the classic smaller legacy-sized box;
- game crashes before reaching the start menu.

Confirmed resolution behavior from logs:
- saved mode = `1280x1024x32`;
- live DX mode = `1280x1024x32`;
- actual scene surface = `1280x1024x32`;
- therefore the native-render-resolution restoration is working;
- current desktop/primary surface = `1920x1080x32`;
- the size/aspect switch is the retail windowed `initDirectDraw7` path resizing the HWND client to the selected render resolution, not the renderer falling back to 640x480.

Modern-mode injection also worked:
- initial retail mode context: 4 entries;
- augmented context: 24 entries;
- Windows enumeration reports 2560x1440;
- explicit 2560x1440 entry installed;
- all three verified retail `initDirectDraw7` call sites are wrapped.

Crash diagnosis:
- exception target: write to `0x0000001F`;
- apparent module name is `binkw32.dll`, but this is the project's proxy DLL loaded at preferred base `0x10000000`, not proof of a RAD Bink decoder fault;
- crash EIP `0x1002C6D1` maps via the uploaded linker map to reconstructed `DCMem_New` at `0x1002C680 + 0x51`;
- stack return `0x10037993` maps into reconstructed `PCTex_CreateTexture256` (`0x10037800..`);
- `DCMem_New` currently does not handle `Mem_CoreNew` returning NULL:
  - NULL base produces alignment offset 32;
  - calculated result becomes `0x20`;
  - writing the alignment byte at `result - 1` writes to exactly `0x1F`, matching the crash.
- conclusion: the fixed game heap is exhausted during texture creation/reload and the reconstructed allocator turns the OOM into an access violation.

Next implementation:
1. add a compatibility fallback allocation path for `DCMem_New` when the original game heap is exhausted, while retaining the exact 32-byte alignment contract;
2. mark fallback blocks with a sentinel heap id and teach delete/shrink/handle paths to recognize them safely;
3. keep the borderless HWND at monitor dimensions after every DirectDraw reinit instead of allowing retail `MoveWindow` to shrink it to the internal render resolution;
4. present the internal scene aspect-fit/centered in that monitor-sized window so legacy 4:3/5:4 modes do not become either a tiny window or a horizontally distorted fullscreen image;
5. retain native 2560x1440 scene support and the movie-specific presenter.

Do not revert native scene-resolution restoration: this test proves it is now functioning.


## Fix batch ready: borderless monitor window + allocator OOM fallback — 2026-09-29

Implementation commits:
- `d0adc9dbb511f071d553843b19fbc1069c6c1765`
  - `DCMem_New` now detects original fixed-heap allocation failure before alignment;
  - allocates an ABI-compatible fallback block from the process C heap;
  - preserves the existing 32-byte aligned returned-pointer contract;
  - marks fallback blocks with signed 4-bit `ParentHeap = -1`;
  - `Mem_DeleteX` frees fallback blocks with `free()`;
  - `Mem_ShrinkX` treats fallback blocks safely;
  - `Mem_MakeHandle` recognizes fallback blocks;
  - logs each fallback as `mem_fallback alloc=... requested=... block=...`.
- `bb38a72588bb868aab8a82a829f90e54171b111c`
  - hardens `Mem_NewTop` against a completely empty free list;
  - returns NULL instead of dereferencing a null free-block pointer, allowing the DCMem fallback to engage.
- `5374ca47c1b35d571d5fc1bac72f845b650fdae2`
  - keeps the game HWND borderless and monitor-sized after every wrapped `initDirectDraw7` and after full `DXINIT_DirectX8`;
  - stops retail windowed DirectDraw initialization from shrinking the desktop-sized window to the selected internal render resolution;
  - presenter now aspect-fits the internal scene into the monitor-sized client area;
  - legacy 4:3/5:4 modes are centered with black bars instead of becoming a small window or being horizontally stretched;
  - matching-aspect modern modes (for example 2560x1440 on a 16:9 target) fill the window naturally;
  - presentation log now records `present=x,y,WxH` and `aspect_fit=`.
- `6a94e9c3bb337e2c9e30bb329410b48931a24c57`
  - replaces `unsigned long long` aspect math with 32-bit `unsigned long` products because the project explicitly supports pre-MSVC-1300 toolchains;
  - all supported resolution products are safely below 32-bit overflow.

Static checks:
- current crash `0x1002C6D1` is `DCMem_New + 0x51` in the proxy linker map;
- caller return `0x10037993` is inside `PCTex_CreateTexture256`;
- write target `0x1F` exactly matches NULL base + 32-byte alignment math;
- native scene rendering was proven at 1280x1024 before this fix;
- 2560x1440 remains in the augmented mode table;
- movie-specific presentation remains enabled.

Build validation note:
- connector-side source checks passed;
- this environment cannot network-clone the private/current repo into the local compiler container, so no independent local binary compile was possible here;
- changes were kept compatible with the project's legacy MSVC constraints visible in `my_types.h`.

NEXT TEST:
1. `UPDATE_SPIDEY_PROJECT.bat`
2. `TEST_LATEST_BUILD.bat`
3. Observe startup window:
   - it should remain monitor-sized instead of collapsing into the legacy small box;
   - a 1280x1024 internal mode on a 16:9 desktop should be centered/aspect-fit with bars.
4. Let all splash/movie screens run through.
5. Confirm whether the game reaches the start menu without crashing.
6. Move the mouse repeatedly at the start menu.
7. Open display options and check whether 2560x1440 remains listed.
8. If possible select/apply 2560x1440 and report the visual result.
9. Upload all generated logs.

Most useful new evidence:
- `spidey-decomp-compat.log`
  - `borderless_monitor_window ...`
  - any `mem_fallback ... requested=...` lines;
- `spidey-decomp-present.log`
  - `dst=<monitor size>`;
  - `present=x,y,WxH aspect_fit=1` for legacy aspect modes;
  - `aspect_fit=0` for matching 16:9 modes;
- crash log if any.

Do not revert the native-resolution or modern-mode work unless a later test demonstrates a renderer-level incompatibility; this test already proved the real scene can render at 1280x1024.


## Build-only regression fixed: legacy SDK monitor API incompatibility — 2026-09-29

Tested revision:
`ed7db84793b6c03867baf0ab475919ea7cd120dc`

Result:
- runtime test did not start because the matching MSVC 6-era toolchain failed compiling `main.cpp`;
- allocator changes compiled;
- failure was isolated to the new borderless-window helper using APIs absent from the project's old Windows headers:
  - `MonitorFromWindow`;
  - `MONITOR_DEFAULTTONEAREST`;
  - `MONITORINFO`;
  - `GetMonitorInfoA`.

Build log errors begin at `main.cpp(927)` and cascade from those missing declarations.

Fix:
- commit `00e09b932a55f7dea60386b462f1c7f4ced54f36`;
- replaced multi-monitor API usage with old-SDK-safe `GetSystemMetrics(0)` / `GetSystemMetrics(1)`;
- this matches the retail game's existing primary-display sizing approach in `RealWinMain`;
- borderless behavior remains:
  - popup/no caption frame;
  - window forced to primary display origin `0,0`;
  - width/height set to primary desktop dimensions;
- log now reports:
  `borderless_monitor_window ... source=GetSystemMetrics`.

NEXT ACTION:
1. run `UPDATE_SPIDEY_PROJECT.bat`;
2. run `TEST_LATEST_BUILD.bat`;
3. first confirm matching build succeeds;
4. only if it launches, continue the existing runtime checks for borderless sizing, splash playback, allocator fallback, menu reachability, mouse movement, and 2560x1440.


## Workflow convenience: one-click update + build/test BAT — 2026-09-29

Added:
- `UPDATE_AND_TEST_LATEST_BUILD.bat`
- commit `d31834725336edbb629948d48a9b3a2baec988a8`

Behavior:
1. runs `UPDATE_SPIDEY_PROJECT.bat`;
2. stops immediately if update fails;
3. re-enters the project directory;
4. runs `TEST_LATEST_BUILD.bat` (forced clean matching build, install, launch, log capture);
5. propagates failure exit codes.

This is now the preferred normal test workflow after the file has been pulled once:
`UPDATE_AND_TEST_LATEST_BUILD.bat`

For the first use on a checkout that predates this file, run `UPDATE_SPIDEY_PROJECT.bat` once to obtain it.


## Runtime result: movies visible, borderless correct, crash moved into PCTex_CreateTexture256 — 2026-09-29

Tested revision:
`fde5f00bd2306d5a877c8ba5b3c15d1196942923`

User-visible:
- startup/splash movies are now visible;
- movies could not be skipped with input;
- game crashes during transition into the start menu.

Confirmed presentation:
- primary desktop/window target = 2560x1440;
- saved/live internal render = 1280x1024x32;
- presenter aspect-fits that 5:4 scene to 1800x1440 at x=380;
- borderless HWND remains 2560x1440;
- movie-specific presenter is installed and visibly working.

New crash:
- EIP = `0x10037CB1`;
- access = write to NULL (`0x00000000`);
- current linker map places `PCTex_CreateTexture256` at `0x10037AF0`;
- fault offset inside that function = `+0x1C1`;
- registers at failure include EDI=0 and 64x64-looking dimension values (EBX/EBP=0x40);
- this is no longer the previous `DCMem_New + 0x51 -> write 0x1F` crash;
- compat log contains no `mem_fallback` entry before failure.

Retail/reference disassembly note:
- repository retail blob `tools/functions/5300640.bin` is the original `PCTex_CreateTexture256` body;
- current reconstructed function remains tagged `@AlmostMatching`;
- the early body allocates a temporary 16-bit conversion buffer, optionally clears it, resolves a palette, then converts indexed source bytes into that buffer before D3D texture creation.
- next patch should guard and independently backstop this transient conversion buffer, then log exact call arguments/stage.

Movie skip:
- `GameFMV_PlayMovie` calls `Pad_Update()` every movie frame and only checks skip triggers after 60 frames;
- borderless helper currently uses `SWP_NOACTIVATE`, which can leave the launch console as the active window and DirectInput foreground devices unable to report movie-skip input;
- `PCINPUT_GetMappedStates` also does not initialize its output masks before polling, so failed/unfocused polls can leave undefined values.

NEXT PATCH:
1. remove `SWP_NOACTIVATE` from borderless resize so the game is activated when brought to the top;
2. initialize mapped-state masks to zero in `Pad_Update`;
3. harden `PCTex_CreateTexture256` transient conversion-buffer allocation:
   - retain original DCMem path first;
   - if it still returns NULL, use a process-heap temporary buffer;
   - never continue conversion with a null destination;
   - free with the matching allocator;
   - log entry dimensions/source/palette/buffer ownership and failure stage;
4. add guards for null source/palette and failed PVR creation before indexing the global texture table.


## Fix batch ready: movie input activation + guarded CreateTexture256 — 2026-09-29

Implementation:
- `0d02ad0bd067093de2e9453fb70fa59a604e28bc`
  - removed `SWP_NOACTIVATE` from the borderless `SetWindowPos` call;
  - bringing the HWND to `HWND_TOP` can now activate it, which is required by foreground DirectInput devices used by movie skipping.
- `c27acf0ce7735ecbd1da63b2d9d75ed7b67c6108`
  - initializes `Pad_Update` mapped-state masks to zero before `PCINPUT_GetMappedStates`;
  - prevents failed/unfocused polls from leaving undefined stack input state.
- `fe6711fe9d4cfece53463707cb479db5055f77f7`
  - first texture hardening draft; superseded immediately by scoped correction below.
- `da9cfd83e6eeead93fd93a20b3611839b3eed714`
  - cleanly reapplies texture hardening only to `PCTex_CreateTexture256`;
  - restores `PCTex_CreateTexture16` exactly to its pre-diagnostic state after catching an over-broad edit during static review;
  - logs each CreateTexture256 call and stage to `spidey-decomp-texture.log`;
  - retains DCMem as the first allocation path;
  - if DCMem still returns NULL, uses a temporary process-heap conversion buffer;
  - never enters indexed-color conversion with a null destination;
  - guards null source and null palette;
  - returns cleanly if PVR creation/recreation fails instead of continuing with an invalid texture handle;
  - frees the temporary conversion buffer with its matching allocator.
- `141a4e479265848dd2bd8f881f0e5c484f57a8f5`
  - `TEST_LATEST_BUILD.ps1` now clears/captures `spidey-decomp-texture.log` into the normal timestamped test-session folder automatically.

Static verification after correction:
- `compatConversionBuffer` and `textureCall` now occur only inside `PCTex_CreateTexture256`;
- `PCTex_CreateTexture16` no longer contains any of the Create256 diagnostics;
- borderless presenter and 2560x1440 mode injection remain unchanged;
- movie presenter remains installed.

NEXT TEST:
- use the standard one-click `UPDATE_AND_TEST_LATEST_BUILD.bat`;
- check whether a key/button can skip a splash after the 60-frame skip delay;
- if not skipped, let movies finish;
- confirm whether the game reaches the start menu;
- move the mouse at the start menu if it reaches it;
- upload the whole new test-session output, including the automatically captured `spidey-decomp-texture.log`.


## Runtime result: menu reached; frontend textures/caps wrong; Alt+Tab loses input — 2026-09-29

Tested revision:
`8783b22198653ca0b310f55e1ed4f84efeef2916`

User-visible:
- game now boots through the splash movies and reaches the main menu;
- main menu renders with a white/missing background and visibly broken composition;
- frontend/start menu is unstable visually;
- Alt+Tab out and back causes controls to stop responding.

Confirmed presentation:
- borderless HWND stays 2560x1440;
- startup scene is 1280x1024x32 and aspect-fit to 1800x1440;
- at frontend takeover (present frame ~195), retail live mode changes to 640x480x16 while the actual offscreen scene is 640x480x32;
- presenter correctly keeps the 2560x1440 window and aspect-fits the 640x480 scene to 1920x1440 at x=320.

Texture diagnostics:
- CreateTexture256 calls 1-25 create normally.
- During frontend texture reload, ordinary 64x64 / 128x128 / 512x512 assets begin calculating impossible conversion buffer sizes:
  - 64x64 -> -679215104 bytes;
  - 128x128 -> 1578106880 bytes;
  - 512x512 -> -520093696 bytes.
- those conversions are rejected by the new guard rather than crashing, which explains why the game now survives but menu art is missing.

Verified root cause:
- retail initDirect3D7 function blob `tools/functions/5235120.bin` contains:
  - load device from `0x006B791C`;
  - push `0x006B5780`;
  - call the device vtable GetCaps method.
- therefore the real retail `D3DDEVICEDESC7` base is `0x006B5780`.
- reconstructed `PCTex.cpp` currently defines `G_D3DDEV_CAPS` at `0x006B5788`, eight bytes too far into the structure.
- `PCTex_UpdateForSoftwareRenderer` copies `dwMaxTextureWidth`, `dwMaxTextureHeight`, and `dwMaxTextureAspectRatio` from this misbased struct during frontend renderer reload.
- bad high-bit cap values make the signed aspect-ratio comparison succeed and then multiply normal texture dimensions by garbage, producing the huge/negative conversion sizes above.

Input diagnostics:
- `DXINPUT_PollKeyboard` only reacquires on `DIERR_INPUTLOST`; after Alt+Tab DirectInput may instead return `DIERR_NOTACQUIRED`, leaving keyboard input permanently unacquired.
- `DXINPUT_PollMouse` is still a MEDIUMTODO stub returning a magic nonzero value without writing either output delta; `PCINPUT_UpdateMouse` then consumes uninitialized deltas. This can directly explain frontend mouse/control instability.

Next implementation:
1. correct `G_D3DDEV_CAPS` from `0x006B5788` to verified retail `0x006B5780`;
2. add a narrow caps sanity log around frontend texture reload;
3. make keyboard polling reacquire on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
4. implement buffered DirectInput mouse polling with the same press/held/release state semantics as keyboard;
5. instrument and preserve saved resolution when the retail frontend issues its hardcoded 640x480x16 display reset, without blocking genuine non-640x480 display-option changes.


## Recovered after input-stream interruption: frontend corruption root cause proven — 2026-09-29

Recovered from the interrupted investigation and re-verified against live `dev`:
- current HEAD before recovery: `8783b22198653ca0b310f55e1ed4f84efeef2916`;
- prior movie/input/texture hardening commits are present;
- the newly discovered D3D caps correction had NOT yet been committed when the stream failed.

Last verified runtime result:
- game now reaches and runs the main menu;
- splash movies are visible;
- frontend presentation remains borderless at 2560x1440;
- at the movie -> frontend transition, live internal mode changes from saved 1280x1024x32 to 640x480x16;
- menu screenshot shows white/missing background composition and broken frontend rendering;
- Alt+Tab out/in causes controls to stop responding.

Texture evidence:
- the same ordinary 64x64 and 128x128 textures create successfully before the frontend renderer reinit;
- after the renderer reinit, those calls begin calculating impossible conversion sizes such as -679215104 and 1578106880 bytes;
- failed texture creation explains the missing/white frontend art rather than bad retail assets.

Retail disassembly breakthrough:
- original retail `initDirect3D7` performs `IDirect3DDevice7::GetCaps` using destination address `0x006B5780`;
- reconstructed `PCTex.cpp` currently defines `G_D3DDEV_CAPS` at `0x006B5788`;
- this is an 8-byte offset error;
- therefore PCTex reads shifted/wrong `D3DDEVICEDESC7` fields after renderer reinit, including `dwMaxTextureWidth`, `dwMaxTextureHeight`, `dwMaxTextureAspectRatio`, and texture-cap flags;
- this directly explains the impossible rounded texture dimensions/conversion byte counts after frontend reinit.

Immediate next actions:
1. correct `G_D3DDEV_CAPS` to retail-proven `0x006B5780`;
2. retain diagnostic logging for one test to prove post-reinit texture sizes normalize;
3. instrument/redirect direct callers of retail `DXINIT_SetDisplayOptions(0x00500250)` so the exact source/arguments of the 640x480x16 frontend switch are known before changing semantics;
4. inspect DirectInput foreground-device poll/reacquire behavior on focus loss and restore.


## Post-interruption recovery audit: more source work survived — 2026-09-29

Live `dev` audit after the user's pasted interruption transcript:
- current HEAD at audit: `8675a76b7109c18e77f0d56295f060fefc2ddb26`;
- no important source progress was lost;
- two implementation commits survived beyond what was visible in the interrupted transcript:
  - `99bcb6fc62be5398ea177c986975d408d2e390c0` — fixes `G_D3DDEV_CAPS` from `0x006B5788` to retail-proven `0x006B5780` and logs post-reinit caps;
  - `8675a76b7109c18e77f0d56295f060fefc2ddb26` — DirectInput focus recovery and buffered mouse polling.

Verified input work in `8675a76b...`:
- keyboard reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
- keyboard state is cleared before reacquire to avoid stuck transitions;
- mouse polling is no longer the old magic-value stub;
- mouse now uses buffered `GetDeviceData`, zeroes deltas, reacquires on focus restoration, and tracks button press/held/release transitions.

True remaining frontier:
1. test the D3D-cap fix against the broken white frontend art;
2. identify the exact caller/arguments responsible for the movie->frontend `640x480x16` reset;
3. preserve the saved/native render resolution for that automatic frontend reset while still allowing genuine user-selected resolution changes;
4. runtime-test Alt+Tab input recovery from `8675a76b...`.


## Current true frontier after recovery audit — 2026-09-29

Additional surviving commits discovered during live branch audit:
- `d88c160afe1474645a5d69ce0f89e76d578b1738`
  - adds `SpideyCompatSetDisplayOptions`;
  - intercepts the retail frontend's exact `640x480x16` compatibility reset only when the windowed compatibility path is active;
  - substitutes the validated saved width/height/bpp for that legacy reset;
  - leaves non-640x480 display-option requests unchanged;
  - logs requested vs applied mode and whether the saved mode was preserved;
  - re-injects modern modes and restores the borderless desktop-sized window after the retail mode change.
- `88b5d1775c04d3386ef99336e7f54402579207bb`
  - installs the display-options compatibility wrapper from `game_patches()`.

Static verification at HEAD after recovery:
- D3D caps base is retail-proven `0x006B5780`;
- caps reload diagnostic is present;
- keyboard reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
- mouse now has buffered DirectInput polling and reacquire logic;
- frontend 640x480x16 reset wrapper exists and is installed;
- only exact legacy 640x480x16 requests are substituted, so normal user resolution choices remain available;
- modern mode reinjection and borderless restore still run after the retail display-options call;
- texture diagnostic capture remains enabled.

No further source change is needed before the next runtime test. The next test is specifically intended to validate all three recovered fixes together:
1. white/missing frontend art should be corrected by the caps-base fix;
2. movie->frontend should remain at saved 1280x1024x32 rather than dropping to 640x480x16;
3. Alt+Tab out/in should reacquire keyboard and mouse input.

Expected diagnostic evidence:
- `spidey-decomp-texture.log`: sane `caps_reload` values and no absurd negative/GB-scale conversion sizes for ordinary 64x64/128x128 assets;
- `spidey-decomp-compat.log`: `display_options request=640x480x16 apply=1280x1024x32 ... preserve_saved=1`;
- `spidey-decomp-present.log`: live scene remains at saved resolution through frontend takeover instead of switching to 640x480x16.


## Fix batch ready: frontend textures + saved mode + Alt-Tab input — 2026-09-29

Implementation commits:
- `99bcb6fc62be5398ea177c986975d408d2e390c0`
  - corrected retail D3D caps base in `PCTex.cpp` from incorrect `0x006B5788` to verified `0x006B5780`;
  - verification source is untouched retail `initDirect3D7` blob `tools/functions/5235120.bin`: device GetCaps is called with `0x006B5780`;
  - added `caps_reload` diagnostics with max texture width/height/aspect/caps.
- `8675a76b7109c18e77f0d56295f060fefc2ddb26`
  - keyboard polling now reacquires on both `DIERR_INPUTLOST` and `DIERR_NOTACQUIRED`;
  - clears stale key state before reacquiring;
  - replaced `DXINPUT_PollMouse` MEDIUMTODO magic-value stub with real buffered DirectInput mouse polling;
  - mouse polling now reacquires after focus loss and emits the existing 0xFF/new, 0x7F/held, 0x80/released state semantics.
- `d88c160afe1474645a5d69ce0f89e76d578b1738`
  - added same-signature wrapper for retail `DXINIT_SetDisplayOptions` at `0x00500250`;
  - logs requested/applied display modes;
  - only intercepts the legacy windowed frontend reset `640x480x16`;
  - when saved render settings are valid and non-640x480, applies saved width/height/bpp instead;
  - leaves genuine non-640x480 option changes untouched.
- `88b5d1775c04d3386ef99336e7f54402579207bb`
  - installs the display-options wrapper during game patch startup.
- `a4abe97e848a223f5dd85a4dd724a10a92f5506e`
  - corrected retail texture checksum hash table resolution;
  - untouched retail `Spool_FindTextureEntry` blob `tools/functions/5018720.bin` disassembles to:
    - `and eax, 0x1FF`
    - `mov eax, [eax*4 + 0x006AB934]`
  - therefore verified hash table base is `0x006AB934`;
  - removed the old inferred `0x006B70F8` runtime check, which could never pass because `PATCH_PUSH_RET` had already overwritten retail entry `0x004C9460`.
- `472898d0d110d59aaecd621b9677983c249ca6cc`
  - `WM_ACTIVATE` now checks `LOWORD(wParam)` rather than the entire WPARAM, so the minimized flag cannot make an inactive window appear active.

Why the white frontend occurred:
- after frontend renderer reset, the misbased D3D caps struct made `dwMaxTextureAspectRatio` read unrelated high-bit data;
- CreateTexture256 then multiplied normal dimensions by that bogus cap:
  - 64x64 paths attempted -679215104-byte conversions;
  - 128x128 paths attempted 1578106880-byte conversions;
  - 512x512 paths attempted -520093696-byte conversions;
- the new allocation guard prevented the old crash but correctly skipped those impossible textures, exposing missing/white menu art;
- separately, the texture checksum resolver was returning the default texture for repeated misses because it used the wrong hash table base.

Static verification:
- no remaining `0x006B5788` D3D-cap macro in current PCTex.cpp;
- CreateTexture256 diagnostics remain scoped only to CreateTexture256;
- DXINPUT_PollMouse no longer contains the stub printf/magic return;
- keyboard and mouse both handle `DIERR_NOTACQUIRED`;
- display-options wrapper is installed in `game_patches`;
- retail texture table resolver now uses `0x006AB934` with SEH-protected traversal;
- GitHub Actions currently reports no workflow runs for this branch, so matching-MSVC compile validation still occurs through the user's one-click BAT.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. let or skip splash movies;
3. verify main-menu background/art is restored;
4. verify the frontend does not switch the internal live render mode back to 640x480x16;
5. move/click the mouse and navigate menus;
6. Alt+Tab out and back, then verify keyboard and mouse controls recover;
7. if stable, open Display Options and check/select 2560x1440;
8. upload the full session including `spidey-decomp-texture.log`.

Expected useful log changes:
- `caps_reload base=0x006B5780 ...` with sane max texture values;
- formerly failing 64/128/512 CreateTexture256 calls should reach `stage=done` rather than `conversion_alloc_failed`;
- `texture_hash_table retail_blob_verified base=0x006AB934`;
- repeated checksum misses should drop sharply or disappear;
- `display_options_compat patched_calls=...`;
- legacy reset should log `request=640x480x16 apply=<saved mode> preserve_saved=1`;
- present log should remain at the saved live render resolution through frontend takeover.


## Runtime result: texture/resolution fixes work; frontend flicker + focus input remain — 2026-09-29

Tested revision:
`ba5cff2c4e5a6271900923534eb13f7771d7162a`

User-visible:
- game boots through startup and reaches the start/main menus;
- the proper main-menu background is restored;
- unrelated-looking building/city images rapidly appear/disappear over both the start and main menus;
- Alt+Tab out and back still causes controls to stop responding.

Confirmed fixes from logs:
- display-options wrapper installed across 4 retail call sites;
- frontend request `640x480x16 option4=0 option5=4` was intercepted and changed to `1280x1024x32` with `preserve_saved=1`;
- live scene remains 1280x1024x32 through frontend presentation;
- borderless target remains 2560x1440;
- texture hash table resolves to retail-proven `0x006AB934`;
- all previously failing CreateTexture256 assets now create at sane sizes:
  - 64x64 -> 64x64;
  - 128x128 -> 128x128;
  - 512x512 -> 512x512;
- no absurd negative/GB-scale conversion sizes remain.

Interpretation:
- white/missing frontend art was fixed by the D3D caps correction;
- the new flicker is not an allocation/texture-creation failure;
- strongest current regression candidate is forcing the retail frontend's intentional 640x480x16 internal canvas to 1280x1024x32;
- prior test at the retail 640x480 frontend mode did not report these building/city flashes, while this artifact appeared immediately after saved-mode preservation was introduced.

Next implementation:
1. stop substituting the exact frontend `640x480x16 option4=0 option5=4` request;
2. keep the outer HWND borderless/desktop-sized and continue aspect-fit presentation, so the window will not shrink;
3. retain the fixed D3D caps address and texture hash table;
4. later handle native frontend/widescreen as a separate UI/rendering project instead of forcing legacy frontend assumptions into a larger canvas;
5. add explicit DirectInput focus-transition handling:
   - unacquire/clear on deactivation;
   - reacquire keyboard/mouse/controller on activation;
   - log WM_ACTIVATE state plus Acquire/GetDeviceData results to a dedicated input log;
6. capture that input log in the one-click test workflow.


## Fix batch ready: legacy frontend canvas + explicit focus reacquire — 2026-09-29

Based on runtime revision:
`ba5cff2c4e5a6271900923534eb13f7771d7162a`

Implementation:
- `9b90226fdd517136a80170ef56a468285eb44973`
  - stops replacing the frontend's exact `640x480x16 option4=0 option5=4` request with the saved gameplay resolution;
  - keeps the retail frontend's intended internal canvas;
  - retains the borderless desktop-sized HWND and aspect-fit presenter;
  - keeps modern resolution enumeration, D3D caps correction, texture hash fix, and startup saved-resolution restore;
  - logs `frontend_legacy=1` for this exact automatic frontend request.
- `61d0ab1813d04baf8a2f71bd7de6f0beaa8aa53b`
  - adds explicit DirectInput application activation handling;
  - clears keyboard/mouse/controller transition state on every focus transition;
  - unacquires foreground devices when the app deactivates;
  - explicitly reacquires keyboard, mouse, and controller when the app becomes active;
  - logs activation state, Acquire HRESULTs, foreground/active/focus HWNDs;
  - adds poll-path reacquire diagnostics for keyboard and mouse.
- `a4d653a123412d43454ee7a3d132417b5fdf086b`
  - exposes `DXINPUT_HandleActivation`.
- `3cd51d4d81d85ff04b06533f99d11b53f7f294a3`
  - handles `WM_ACTIVATEAPP` in `SpideyWndProc` and routes app focus transitions to DirectInput.
- `e502f3b177a03a06864127350c8b9c4a3f561774`
  - one-click test workflow now clears/captures `spidey-decomp-input.log`.

Reason for frontend-canvas change:
- the prior white/missing background problem is conclusively fixed: all formerly failing 64x64, 128x128, and 512x512 CreateTexture256 calls now complete normally;
- the flickering unrelated building/city imagery first appeared in the build that forced the retail frontend canvas from 640x480x16 to 1280x1024x32;
- the renderer already clears the scene each BeginScene, so this is not simply uncleared desktop memory;
- allowing the legacy frontend canvas while scaling only at presentation is the narrowest regression test.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. verify startup movies still render;
3. check start menu and main menu for the rapidly flashing building/city imagery;
4. the inner frontend should now be 640x480, but the outer window must remain borderless 2560x1440 and aspect-fit;
5. Alt+Tab out, wait briefly, Alt+Tab back;
6. test keyboard and mouse input after return;
7. upload full session, especially:
   - `spidey-decomp-compat.log`;
   - `spidey-decomp-present.log`;
   - `spidey-decomp-texture.log`;
   - NEW `spidey-decomp-input.log`.

Expected diagnostics:
- compat: `display_options request=640x480x16 apply=640x480x16 ... frontend_legacy=1`;
- present: frontend scene returns to 640x480 while outer destination remains 2560x1440;
- input: deactivate/activate pairs plus explicit DirectInput Acquire results.


## Runtime result: legacy frontend canvas restored; visual flicker + Alt+Tab input still remain — 2026-09-29

Tested revision:
`42fe9d5b3eda8cd376eb585f986a4bb5b9638f9f`

User-visible:
- start/main menu behavior is improved compared with the forced-1280 frontend build;
- proper menu/background assets remain present;
- rapid transient images (described as building/city imagery) still flash in and out over both start and main menus;
- Alt+Tab out/back still kills controls completely.

Confirmed from logs:
- exact retail frontend request now passes through unchanged:
  `640x480x16 option4=0 option5=4 preserve_saved=0 frontend_legacy=1`;
- borderless HWND remains 2560x1440;
- presenter sees a real 640x480x32 scene and aspect-fits it to 1920x1440 at x=320;
- corrected D3D caps remain sane:
  `max_w=16384 max_h=16384 max_aspect=16384 tex_caps=0x00000CCD`;
- all CreateTexture256 calls continue to complete at sane sizes, including 512x512 and 512x240 frontend assets;
- therefore the remaining flashes are not the prior bad-caps/failed-texture bug.

New DirectDraw teardown clue:
- dxerror log reports `D3D error=0x00000004` at retail call site `0x004FDDCF`;
- retail disassembly proves this site is not a D3D draw failure:
  - it loads movie DirectDraw object `0x006B7900`;
  - calls COM vtable +8 = `Release()`;
  - return value 4 is the remaining COM reference count;
- retail movie NextFrame uses movie surface `0x00AC0A3C`;
- retail PCMOVIE_Stop closes Bink/file state but does not release `0x00AC0A3C`;
- multiple startup movies can therefore leave movie surfaces alive across the frontend DirectDraw rebuild.
- this is now the strongest renderer-state lead for old/foreign imagery flashing through after startup movies.

Input evidence:
- no `spidey-decomp-input.log` was supplied with this test session, despite the workflow being configured to capture it if created;
- regardless of whether it was omitted manually or never created, WM_ACTIVATEAPP-only recovery did not solve runtime behavior.

Next implementation:
1. release the retail movie surface at `0x00AC0A3C` on final movie frame and on direct PCMOVIE_Stop call paths;
2. log movie-surface Release() refcounts before frontend DirectDraw teardown;
3. make keyboard/mouse focus recovery poll-driven as well as message-driven:
   - compare `GetForegroundWindow()` with the DirectInput HWND;
   - unacquire/clear while background;
   - explicitly Acquire again on foreground transition before GetDeviceData;
   - log transition/result even if WM_ACTIVATEAPP is missed;
4. ensure input log is created at DirectInput initialization so absence itself is diagnostic;
5. retain legacy frontend canvas, D3D caps fix, texture hash fix, borderless presentation, and modern resolution support.


## Fix batch ready: release leaked movie surface + foreground-polled DirectInput recovery — 2026-09-29

Implementation:
- `f84ee7f885ed6f36faf6ec5656d1c131e563c9c9`
  - changes the movie-specific flip hook to a dedicated wrapper;
  - reads the real retail Bink pointer at `0x00AC0BA4`;
  - when the final movie frame has just been presented, releases the real retail movie surface at `0x00AC0A3C` and clears the slot;
  - logs surface size and Release() remaining-ref count;
  - also scans direct retail callers of `PCMOVIE_Stop 0x0050B790` and redirects them through a wrapper that performs the same movie-surface cleanup after the untouched retail stop logic;
  - this covers both normal movie completion and direct stop/skip paths where available.
- `52c9f7f7204412668cbfadefd449da998e125383`
  - adds foreground-window polling as a second DirectInput recovery mechanism;
  - keyboard/mouse polling compares `GetForegroundWindow()` against `gDxInputHwnd` every poll;
  - foreground transitions invoke the existing activation handler even if `WM_ACTIVATEAPP` is missed;
  - polling is suppressed while the game is not foreground;
  - DirectInput initialization now always creates `spidey-decomp-input.log` with HWND/foreground/focus state.
- `12986a56b9365ef1c8ce8a549fd273898e9098dd`
  - old-MSVC compile correction: moves the foreground-sync helper below the `gDxInputHwnd` definition.

Retail evidence behind movie cleanup:
- retail `PCMOVIE_NextFrame 0x0050B5A0` uses movie surface pointer `0x00AC0A3C`;
- the function blits that surface into scene `0x006B7908` and calls `DXPOLY_Flip` at `0x0050B71A`;
- retail `PCMOVIE_Stop 0x0050B790` closes Bink/file state but does not Release the movie surface;
- DirectDraw teardown later calls Release() on movie DirectDraw object `0x006B7900` and reports remaining refcount 4;
- multiple startup movies leaking one surface reference each is consistent with that count and with stale movie/display state surviving into the frontend rebuild.

Static verification:
- movie cleanup references the retail surface/Bink globals, not reconstructed DLL-owned placeholders;
- final-frame release occurs only after the movie frame has been presented;
- direct Stop callers use untouched retail Stop first, then surface cleanup;
- foreground polling compiles against globals declared before use;
- legacy 640x480 frontend canvas remains enabled;
- fixed D3D caps, texture hash, borderless presentation, and modern resolution support remain intact.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. observe all startup movies;
3. check start + main menus for rapidly flashing building/city imagery;
4. Alt+Tab out for a second, then return and test keyboard + mouse;
5. upload the full session.

Most useful evidence:
- `spidey-decomp-present.log`
  - `movie_surface_release reason=final_frame ... remaining_refs=...`;
  - `movie_stop_compat patched_calls=...`;
- `spidey-decomp-dxerror.log`
  - compare the old movie DirectDraw remaining refcount 4 after cleanup;
- `spidey-decomp-input.log`
  - now guaranteed to be created at input initialization;
  - should show foreground deactivate/reactivate and Acquire results.


## Runtime result: movie leak ruled out; retail input path identified; double-present race targeted — 2026-09-30

Tested revision:
`e16babda924aa82eb1910577e848317bc7c51f20`

User-visible:
- rapid building/city imagery still flashes over start and main menus;
- Alt+Tab out/back still removes all menu control.

What this run conclusively ruled out:
- movie-surface leak is NOT the source of menu flashing;
- `spidey-decomp-present.log` shows all four startup movie surfaces released with `remaining_refs=0`;
- flashing remained unchanged after that cleanup.

Input root cause discovered:
- the reconstructed `DXINPUT_*` changes in `DXsound.cpp` were never on the retail host's execution path;
- `game_patches()` did not patch retail `DXINPUT_*` entries/callers;
- this explains why no `spidey-decomp-input.log` was ever created and why several iterations of reconstructed reacquire logic had no runtime effect.

Retail input mapping recovered from untouched retail function blobs:
- `DXINPUT_Initialize = 0x005013D0`;
- `DXINPUT_Release = 0x00501440`;
- `DXINPUT_SetKeyState = 0x00501510`;
- `DXINPUT_SetMouseButtonState = 0x00501530`;
- `DXINPUT_GetKeyName = 0x00501550`;
- `DXINPUT_SetupKeyboard = 0x00501590`;
- `DXINPUT_SetupMouse = 0x00501710`;
- `DXINPUT_SetupController = 0x00501890`;
- `DXINPUT_PollKeyboard = 0x00501B80`;
- `DXINPUT_GetKeyState = 0x00501CB0`;
- `DXINPUT_PollMouse = 0x00501CC0`;
- `DXINPUT_GetMouseButtonState = 0x00501E40`;
- `DXINPUT_PollController = 0x00501E50`;
- `DXINPUT_GetControllerButtonState = 0x00501FB0`;
- `DXINPUT_StartForceFeedbackEffect = 0x005021A0`;
- `DXINPUT_StopForceFeedbackEffect = 0x005021E0`;
- `DXINPUT_GetNumControllerButtons = 0x00502210`.

Retail input globals verified from those same functions:
- DirectInput object `0x006B7A30`;
- input HWND `0x006B7A60`;
- keyboard device `0x006B7A5C`;
- mouse device `0x006B7A64`;
- controller device `0x006B7A2C`;
- keyboard transition state `0x006B792C`;
- mouse-button state `0x006B7A54`;
- controller-button state `0x006B7A34`.

Presentation race diagnosis:
- compatibility HWND is 2560x1440;
- retail DirectDraw primary remains 1920x1080;
- in windowed mode retail `DXPOLY_Flip` first Blts scene -> legacy primary, then `SpideyDiagDXPOLYFlip` immediately GDI-stretches scene -> HWND;
- this means two independent presentation paths paint the same visible window every frame through different-sized targets;
- after texture/caps/movie fixes all succeeded, this double-present path is now the strongest explanation for rapid transient foreign imagery.

Fix commit:
- `92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`
  - windowed compatibility mode no longer calls retail `DXPOLY_Flip`; direct scene -> HWND presentation is the sole windowed presenter;
  - original retail Flip remains untouched for non-windowed mode;
  - present log now records `present_path ... retail_flip=0 direct_hwnd=1`;
  - installs retail-call-site wrappers for `DXINPUT_PollKeyboard 0x00501B80` and `DXINPUT_PollMouse 0x00501CC0`;
  - wrappers operate on the actual retail keyboard/mouse/controller DirectInput objects and state arrays;
  - foreground transition explicitly Unacquires on background and Acquires on return;
  - keyboard failures get one explicit Acquire + retail retry;
  - input log is created by the installer itself, proving the hook installed even before first input poll.

Static verification:
- retail input addresses above were recovered from exact original function blobs, not inferred from reconstructed DLL layout;
- wrappers call untouched retail poll functions by absolute address, so original key/mouse transition semantics remain in charge;
- installer rewrites only direct E8 call sites targeting the two retail poll functions;
- the DLL wrapper calls retail by function pointer, so it cannot be recursively repatched;
- legacy 640x480 frontend canvas, fixed D3D caps, correct texture hash table, modern mode list, movie cleanup, and borderless presentation remain enabled.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. verify splash movies and frontend boot;
3. watch start/main menu for flashing building/city imagery;
4. Alt+Tab out for a second and back;
5. test keyboard, mouse, and controller if available;
6. upload full session.

Key expected logs:
- `spidey-decomp-present.log`: `present_path ... windowed=1 retail_flip=0 direct_hwnd=1`;
- `spidey-decomp-input.log`: installer line with nonzero keyboard/mouse call counts, then `foreground_acquire` / `background_unacquire` transitions around Alt+Tab.


## Runtime result: 2560x1440 saved mode fails D3D7 CreateDevice before splash — 2026-09-30

Tested revision:
`e7f678156efbcca850cca86d94c38d52b50dddec`

Observed:
- clean matching build succeeded;
- game crashed before the first splash/movie frame;
- retail-input compatibility installer DID run:
  - keyboard direct-call sites patched = 2;
  - mouse direct-call sites patched = 1;
- no retail input poll/foreground-transition entry was logged before the crash;
- no presentation frame was reached.

Critical difference from the preceding successful menu run:
- previous successful runtime restored `1440x1080x32`;
- this failed runtime restored saved `2560x1440x32`.

D3D failure:
- retail call site `0x004FEA30` invokes `IDirect3D7::CreateDevice(pGUID, g_pDDS_Scene, &g_D3DDevice7)`;
- it returned `0x88760082 = DDERR_INVALIDOBJECT`;
- the scene DirectDraw surface had already been created, but Direct3D7 rejected it as a valid render target/device surface at this mode;
- source windowed scene creation uses `DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN`.

Secondary crash:
- after CreateDevice failure, retail cleanup calls function `0x00503AF0`;
- at `0x00503AF7` it dereferences global `0x006BBF1C`;
- that global is NULL in this failure state, producing the observed C0000005 read from address 0;
- this cleanup AV is secondary; the root failure is the 2560x1440 CreateDevice rejection.

Interpretation:
- the new retail input hooks did not cause this startup crash;
- the single-presenter path also did not execute before the crash;
- native 2560x1440 is NOT runtime-safe on the current DirectDraw7/D3D7 render-target path and must not remain selectable/persisted as if verified;
- 1440x1080x32 is the latest verified working saved internal mode on the same machine/runtime.

Immediate recovery plan:
1. quarantine exact `2560x1440x32` from the selectable modern-mode list until a valid D3D7 render-target path is implemented;
2. if the persisted saved mode is exactly 2560x1440, recover to the last verified `1440x1080x32` mode before retail DX initialization and update the in-memory saved setting so restart is not bricked;
3. add a guarded wrapper for retail cleanup function `0x00503AF0` so a future DirectX init failure cannot turn into a null-deref crash;
4. retain the new retail input polling hooks and sole windowed direct-HWND presenter for the next test;
5. continue 2560x1440 support as a separate renderer-compatibility task rather than claiming it is already supported.


## Recovery patch ready: quarantine 2560x1440 and guard failed-D3D cleanup — 2026-09-30

Implementation:
- `17a75488bc16a865e8b1b8f787544ea06b1228ae`
  - removes exact 2560x1440 from modern mode enumeration on the current DirectDraw7/D3D7 path;
  - removes the old unconditional explicit 2560x1440 mode injection;
  - logs `explicit_2560x1440=0 quarantined_2560x1440=1`;
  - if persisted saved settings are 2560x1440, startup recovers to the last runtime-verified working `1440x1080x32`;
  - writes that recovered mode back into the in-memory saved-setting fields so subsequent display-option paths do not immediately reapply the broken mode;
  - direct `DXINIT_SetDisplayOptions(2560,1440,...)` requests are also recovered to 1440x1080 until native 1440p render-target support is fixed;
  - installs a direct-call-site wrapper around retail cleanup function `0x00503AF0`;
  - if retail global `0x006BBF1C` is NULL, the cleanup call is skipped and logged instead of dereferencing NULL at `0x00503AF7`;
  - if the object exists, untouched retail cleanup runs normally.

The 2560x1440 mode is quarantined, not abandoned:
- current failure is specifically `IDirect3D7::CreateDevice` rejecting the 2560x1440 windowed scene surface with `DDERR_INVALIDOBJECT`;
- windowed scene creation currently uses `DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN`;
- native 1440p support remains an open renderer-compatibility task, likely requiring a render-target allocation/path change rather than simple mode enumeration.

Retained for the next runtime test:
- retail keyboard/mouse poll call-site wrappers from `92f0d4add60dfdd6c7a9b50decc6b35a8bf01507`;
- single windowed scene->HWND presentation path (retail Flip skipped only in compatibility/windowed mode);
- fixed D3D caps base;
- fixed retail texture hash table;
- legacy 640x480 frontend canvas;
- movie-surface cleanup.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. expected startup compat log:
   - `restore_saved_resolution request=2560x1440x32 apply=1440x1080x32 quarantined_2560x1440=1`;
   - modern mode log shows `explicit_2560x1440=0 quarantined_2560x1440=1`;
3. verify splash movies return;
4. verify whether start/main menu building/city flashes are gone under the single-presenter path;
5. Alt+Tab out/back and test menu keyboard/mouse;
6. upload the full session, especially input/present/compat logs.

If this boots, the test finally isolates the intended two fixes because the unrelated persisted-2560 startup failure is removed.


## Runtime success: Alt+Tab fixed and building flashes gone; DPI/native 1440p + widescreen frontier — 2026-09-30

Tested revision:
`8892e08060938d5a9f0e0ff028fb9dfaa7175f4e`

User-visible success:
- Alt+Tab out/back now preserves menu controls;
- the previous rapidly flashing building/city imagery is gone;
- remaining visual issue is an intermittent whole-screen flash;
- screen is still pillarboxed because current internal render/frontend modes are 4:3;
- 2560x1440 is absent from Display Settings because the preceding crash-recovery patch deliberately quarantined it.

Runtime proof for input fix:
- retail input hook installed with keyboard call sites=2 and mouse call sites=1;
- initial foreground Acquire returned 1 (already acquired / harmless legacy state);
- background transition explicitly Unacquired devices;
- foreground return explicitly Acquired keyboard/mouse/controller and all three returned `0x00000000`;
- user confirmed controls continue working after tabbing back in.
This closes the Alt+Tab input-loss bug.

Runtime proof for single-presenter fix:
- every logged compatibility frame uses `retail_flip=0 direct_hwnd=1`;
- user confirmed the old flashing building/city imagery disappeared.
This closes the old double-present/foreign-primary-content artifact.

Remaining whole-screen flash diagnosis:
- aspect-fit presenter still clears the ENTIRE client to black before every StretchBlt;
- current 1440x1080 -> 2560x1440 presentation uses `present=320,0,1920x1440 aspect_fit=1` every frame;
- frontend 640x480 -> 2560x1440 uses the same pillarboxed 1920x1440 destination;
- a GDI-visible FillRect between frames can therefore expose a full black frame before StretchBlt.

Physical-resolution/DPI breakthrough:
- compatibility HWND/client reports 2560x1440;
- DirectDraw primary still reports 1920x1080;
- exact ratio is 4/3 in both dimensions (2560/1920 and 1440/1080), strongly indicating process DPI virtualization/scaling;
- this explains why a 2560x1440 offscreen scene surface could be created but D3D7 CreateDevice rejected it while DirectDraw considered the primary only 1920x1080.

Widescreen evidence:
- current presenter is correctly preserving source aspect, not stretching:
  - 1440x1080 (4:3) -> 1920x1440 with 320px side bars;
  - frontend 640x480 (4:3) -> 1920x1440 with the same side bars.
- do NOT remove bars by stretching; true widescreen requires a 16:9 internal render target and then validation/correction of camera projection and UI mapping.
- source `PCSHELL_CoordsDCtoPC` maps virtual 512x240 shell coordinates independently to live X/Y resolution, so UI behavior at 16:9 must be checked separately.
- source `M3d_RenderSetup` remains retail (not replaced by patch_ps2m3d), so camera/projection behavior must be runtime-validated once a real 16:9 render target boots before changing FOV math.

Implementation:
- `543b456f90495cdb8123b5437d8c1e039f832bde`
  - dynamically resolves `SetProcessDPIAware` / `IsProcessDPIAware` from already-loaded user32 using old-SDK-safe GetProcAddress;
  - enables process DPI awareness during DLL_PROCESS_ATTACH before retail creates its window/DirectDraw objects;
  - logs DPI set result, actual awareness state, and physical screen metrics;
  - re-enables 2560x1440 mode only when process DPI awareness is active;
  - filters DPI-aware enumerated render modes above the physical screen dimensions;
  - startup/set-display only quarantine 2560x1440 if DPI awareness could not be established;
  - removes the full-client black FillRect from aspect-fit presentation and clears only actual side/top/bottom bars;
  - retains the retail-input fix and sole direct-HWND windowed presenter.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. first inspect `spidey-decomp-compat.log` for:
   - `dpi_awareness ... process_aware=1 metrics=2560x1440`;
   - `modern_modes ... native_2560x1440=1 dpi_aware=1`;
3. verify the intermittent whole-screen black flash is gone/reduced;
4. verify Alt+Tab remains fixed;
5. open Display Settings and confirm 2560x1440 has returned;
6. select/apply 2560x1440:
   - if it boots/renders, capture screenshot + full logs so widescreen/FOV/UI behavior can be classified;
   - if D3D7 still rejects it, cleanup guard should prevent the old null-deref and logs will show the remaining renderer limitation cleanly.

Do not implement projection/FOV stretching before this test: first determine whether DPI-aware DirectDraw now exposes a true 2560x1440 primary and whether retail M3d projection naturally handles the 16:9 render target.


## Two-session 1440p comparison: selectable != renderable — 2026-09-30

Compared user ZIPs:
- `initialloadNOTat1440.zip`, session 20260930-021227;
- `second attempt to load at 1440.zip`, session 20260930-021422.
Both tested revision:
`423cbf0ed7f9c6e72bb41890d6c40b1c4779cae4`.

First session (booted below 2560x1440):
- early DPI call reported `process_aware=1 metrics=2560x1440`;
- saved startup mode was `1920x1440x32`;
- modern mode injection exposed native `2560x1440`;
- game booted successfully;
- full-screen black flashing was gone after bar-only clear change;
- sole direct-HWND presenter remained active;
- DirectDraw primary STILL reported 1920x1080 even though HWND/client and Win32 metrics were 2560x1440;
- frontend stayed internally 640x480 after startup;
- selecting/scrolling to 2560x1440 only changed the saved-resolution fields (`saved_res=2560x1440x32`) while `live_res` remained 640x480;
- therefore this session did NOT prove a live 2560x1440 D3D render target.

Second session (boot with persisted 2560x1440):
- early DPI call again reported `process_aware=1 metrics=2560x1440`;
- startup restored `2560x1440x32` live;
- mode list exposed 2560x1440;
- before any splash/present frame, retail `IDirect3D7::CreateDevice` failed with `0x88760082 = DDERR_INVALIDOBJECT` at call site `0x004FEA4A`;
- cleanup guard prevented the old secondary null-deref and logged `cleanup_503AF0 skipped null_global=0x006BBF1C`;
- retail input hook installed but no poll occurred before D3D failure.

Conclusion:
- DPI awareness is useful for physical Win32 metrics and mode enumeration, but it does not by itself make DirectDraw expose a 2560x1440 primary; DirectDraw still reports 1920x1080;
- 2560x1440 has never actually rendered on the current D3D7 device path;
- the renderer must be made tolerant of a render target larger than the legacy DirectDraw primary/device bootstrap target.

Next experiment:
1. move DPI-awareness setup to the very first DLL_PROCESS_ATTACH work and request per-monitor-v2 dynamically before AllocConsole/other UI work;
2. retain 2560x1440 in mode selection;
3. wrap retail `initDirect3D7 0x004FE1B0` only for the 2560x1440 path;
4. hook IDirect3D7::CreateDevice during that call:
   - first try untouched CreateDevice on the real 2560x1440 scene;
   - on DDERR_INVALIDOBJECT, create a known-good 1920x1080 video-memory 3D bootstrap surface;
   - copy/create a matching Z-buffer attachment when possible;
   - create the retail-selected D3D device on the bootstrap;
   - try `SetRenderTarget(real_2560x1440_scene)`;
   - if SetRenderTarget succeeds, continue retail init at true 2560x1440;
   - if it fails, switch the retail scene/global live resolution to the 1920x1080 bootstrap so startup remains safe instead of exiting/crashing.
5. log every HRESULT/caps transition so the next test distinguishes true 1440p from safe 1080p fallback.

Widescreen note:
- menus remaining 640x480/4:3 are separate from gameplay render resolution;
- do not stretch the 4:3 frontend;
- once a live 16:9 gameplay target is working, validate retail projection/FOV and then patch UI safe-area mapping independently.


## DX11 migration started — Phase 0 bridge scaffold — 2026-09-30

Decision:
- stop investing heavily in making DirectDraw7/Direct3D7 the long-term modern renderer;
- migrate to Direct3D 11 incrementally;
- keep the current D3D7 path as a temporary reference/fallback until DX11 reaches parity;
- do NOT pursue DX12 for this project: it adds explicit synchronization/descriptor/command-list complexity without meaningful benefit for Spider-Man 2000.

Architecture:
- legacy retail-compatible proxy stays `binkw32.dll` / rebuilt `spider.dll`, compiled by the preserved VC6-era matching toolchain;
- new modern x86 renderer is `spidey_renderer11.dll`, compiled with VS 2022 / current Windows SDK;
- the two communicate through a versioned C ABI resolved dynamically with `LoadLibraryA` / `GetProcAddress`;
- modern D3D11/DXGI headers never enter the matching proxy build.

Implemented Phase 0 files:
- `renderer11/include/spidey_renderer11_api.h`
  - ABI version 1;
  - GetAbiVersion / GetBackendName / Probe;
  - Initialize / Resize / BeginFrame / Present / Shutdown.
- `renderer11/src/spidey_renderer11.cpp`
  - hardware D3D11 device probe;
  - D3D11 device + immediate context;
  - DXGI swap chain;
  - RGBA8 backbuffer RTV;
  - D24S8 depth buffer;
  - viewport setup;
  - resize;
  - clear + present;
  - adapter/feature-level logging to `spidey-renderer11.log`.
- `renderer11/CMakeLists.txt`
  - modern x86 DLL target linked to d3d11 + dxgi.
- `renderer11/spidey_renderer11.def`
  - stable undecorated export names for the x86 C ABI.
- `scripts/build_renderer11.ps1`
  - configures/builds with Visual Studio 17 2022, Win32;
  - copies output to `out/renderer11/spidey_renderer11.dll`.
- `docs/DX11_MIGRATION.md`
  - full staged migration plan.

Legacy bridge:
- proxy loads `spidey_renderer11.dll` during the existing DX initialization wrapper;
- verifies ABI=1;
- calls `SpideyRenderer11_Probe`;
- records backend/probe state in `spidey-decomp-compat.log`;
- does NOT switch visible rendering yet.

One-click workflow:
- still builds the matching proxy first;
- now builds the DX11 helper second;
- installs `spidey_renderer11.dll` next to `SpideyPC.exe`;
- records its SHA-256 in the session metadata;
- clears/captures `spidey-renderer11.log`.

D3D7 safety during migration:
- exact 2560x1440 is again quarantined from the legacy D3D7 mode table regardless of DPI awareness;
- a persisted 2560x1440 D3D7 setting is recovered to the last verified safe 1440x1080x32 mode;
- this avoids another pre-splash D3D7 CreateDevice crash while DX11 is only in probe/scaffold mode;
- 2560x1440 will return through DXGI once DX11 owns rendering/presentation.

Relevant commits:
- `09db45a1bd6cd09ed429078b3bbcaa641573c1ed` — stable C bridge API;
- `ab4c7f5461c8814ab06d233e2199c8d4febc3a72` — CMake target;
- `343de8e6f50b9448444b461f69a08c6e2a0f3f03` — D3D11 device/swap-chain implementation;
- `b1fd7e3d3e93268db95c68dfd7015dbb72be66ce` — modern build script;
- `e754aca6117048555db0fb2bf2a43bfd7bed812a` — legacy probe + D3D7 safety;
- `168d4d3f215dcd8c51e60b47b81f6b691e834ee7` — one-click build/install/log integration;
- `1496e2c29f6e1c35a49be9e93bd55bc3b51decbd` — migration documentation;
- `453146284b3c6bedfe06277da93d45d0fef5c44f` / `b745a89c5170edef9008289044b4b4ad42273e63` — undecorated x86 exports;
- `550f0af1f897201430ac94770bd35b455d774fc6` — CMake-path fallback correction.

Phase 0 next test:
- run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
- this is a plumbing/probe test, NOT a visible DX11-rendering test yet;
- expected:
  - old game still renders through the known-good D3D7 path;
  - Alt+Tab fix remains working;
  - `spidey-decomp-compat.log` contains:
    `renderer11_bridge loaded ... abi=1 expected=1 backend=Direct3D 11 probe=1`;
  - new `spidey-renderer11.log` contains a successful hardware probe and D3D feature level.

After Phase 0 passes:
1. Phase 1: DX11 takes over final presentation while D3D7 still renders the scene;
2. Phase 2: migrate texture ownership;
3. Phase 3: migrate 2D/frontend primitives;
4. Phase 4: emulate D3D7 fixed-function 3D states/shaders and triangle fans;
5. Phase 5: true 16:9 projection/FOV + UI safe-area work;
6. Phase 6: DX11 becomes the default renderer and D3D7 becomes diagnostic/reference only.


## Full new-chat handoff refreshed for DX11 frontier — 2026-09-30

- Rewrote `docs/NEW_CHAT_HANDOFF.md` so it no longer points at the obsolete black-screen/DirectDraw frontier.
- New handoff is centered on the current Direct3D 11 migration.
- Source frontier before handoff refresh: `0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0`.
- Handoff document commit: `2cbc00900e27f3ce42e357fdc7c9576641250385`.
- Exact next action remains the first DX11 Phase 0 plumbing/probe runtime test using `UPDATE_AND_TEST_LATEST_BUILD.bat`.
- The external full handoff ZIP should include:
  - current documentation;
  - DX11 migration/bridge source snapshots;
  - repo/Drive/upstream links;
  - latest 1440p two-session evidence;
  - previous Sep-30 full handoff as historical baseline;
  - disconnect/live-documentation protocol.


## First DX11 Phase 0 test: renderer11 export linker failure — 2026-09-30

User ran `UPDATE_AND_TEST_LATEST_BUILD.bat` against runtime revision
`0fbc7b6a90c630ff8070fd6a9ed9ba14f0c101f0`.

Observed:
- legacy matching proxy force-cleaned, compiled, and linked successfully;
- CMake configured `renderer11` as VS 2022 Win32/x86 successfully;
- `spidey_renderer11.cpp` compiled successfully;
- final renderer11 DLL link failed before installation/launch;
- all eight exports named by `renderer11/spidey_renderer11.def` were reported unresolved;
- therefore no DX11 runtime probe occurred and this is NOT a game/runtime-rendering failure.

Root cause:
- the .def currently aliases each public export to an explicitly underscore-prefixed x86 C symbol, e.g.
  `SpideyRenderer11_Probe=_SpideyRenderer11_Probe`;
- module-definition export resolution already performs the x86 C-name decoration lookup for an undecorated export entry;
- explicitly supplying the underscore-prefixed alias causes an additional decoration lookup / wrong internal name on the modern linker path;
- a local MSVC-ABI-compatible i686 COFF reproduction with clang-cl + lld-link confirms the behavior:
  explicit `Foo=_Foo` fails looking for `__Foo`, while plain `Foo` resolves the object symbol `_Foo` and links.

ACTIVE FIX:
- change the .def EXPORTS list to plain undecorated public names with no `=_Name` aliases;
- keep the C ABI and `extern "C" __cdecl` source definitions unchanged;
- rerun the same Phase 0 one-click test after this build-only correction.

Do not advance to DX11 Phase 1 until the helper builds, installs, loads, ABI-checks, and probes successfully.


### DX11 export linker fix committed

Fix commit:
`c89c6034a0bfb70cf99b38b1fec7745bd8ddc166` — `renderer11: fix x86 DEF export decoration`

Change:
- `renderer11/spidey_renderer11.def` now lists only the eight undecorated public export names;
- removed explicit `=_Spidey...` aliases;
- source declarations/definitions remain `extern "C" __cdecl` and ABI version remains 1;
- proxy lookup strings in `main.cpp` already request the same undecorated names, so no bridge-side change is required.

Static validation:
- independent i686 MSVC-ABI COFF reproduction links successfully with plain .def names and its PE export table contains exactly the undecorated names;
- the previous explicit underscore alias form reproduces the unresolved-name failure.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. confirm the updater pulls commit `c89c6034a0bfb70cf99b38b1fec7745bd8ddc166` or later;
3. renderer11 should now pass the DLL link stage;
4. if build/install succeeds, let the game launch normally and upload the complete session output;
5. Phase 0 success still requires:
   - `renderer11_bridge loaded ... abi=1 expected=1 backend=Direct3D 11 probe=1`;
   - `spidey-renderer11.log` with successful D3D11 probe / feature level;
   - known-good D3D7 visible rendering and Alt+Tab behavior unchanged.

If a new failure appears, stop at that exact Phase 0 layer and fix it before Phase 1.


## DX11 Phase 0 PASSED — 2026-09-30

Tested revision:
`da13c63f51a25f6492c8d32a8f0165d28648228a`

User result:
- game booted normally through splash/start flow and reached the main menu;
- existing D3D7 visible rendering remained intact;
- expected 4:3 side bars remain;
- 2560x1440 remains intentionally absent because the legacy D3D7 2560x1440 mode is still quarantined.

Runtime proof:
- renderer11 bridge loaded successfully;
- ABI matched;
- backend reported `Direct3D 11`;
- hardware probe returned success;
- renderer log reported `hr=0x00000000 feature_level=0xB100` (D3D feature level 11.1);
- saved 2560x1440 was safely recovered to 1440x1080 for the still-active D3D7 path;
- current presenter remains 1440x1080 -> aspect-fit 1920x1440 inside the 2560x1440 client, hence 320-pixel bars on each side.

Phase 0 is CLOSED.

NEXT FRONTIER — PHASE 1:
- make DX11 own final presentation while D3D7 temporarily continues rendering the scene;
- retain a fail-safe fallback to the current direct-HWND GDI presenter;
- do not re-enable legacy D3D7 2560x1440 yet;
- only after DX11 presentation is verified should native 16:9 scene ownership / 2560x1440 rendering advance.


## DX11 Phase 1 implementation ready for runtime test — 2026-09-30

Goal:
DX11 owns the final HWND presentation while the legacy D3D7 renderer continues producing the scene surface.

Implementation commits:
- `3d333706da0788b012892cca6f68f32c242d9d23` — bridge ABI bumped to 2 and HDC presenter entry added;
- `a392d3665f1617e1add6e83491840ea8ac109232` — exported `SpideyRenderer11_PresentHdc`;
- `72bdb8f5051b3bfa6e4f36073e0cc28cf925e9db` — link GDI32;
- `a799860a24cf739c92effc2211897c083fe34bee` — GDI-compatible B8G8R8A8 DX11 swap chain + HDC-to-backbuffer presenter;
- `7eecc345ebd26f964fe2e0413dcd32834831c46e` — legacy proxy routes the windowed presentation path through DX11;
- `4de672aa0b26e73f42d403adea40733d951688a1` — preserve GDI-compatible swap-chain flag across resize;
- `cec2b0723bda00f2b8b41b17bd2b351e525ba7f2` — portable HDC bridge typedef.

Design:
- Phase 1 ABI is now version 2.
- The modern helper creates a GDI-compatible DX11/DXGI swap-chain backbuffer.
- The legacy D3D7 scene remains the render source.
- The proxy obtains the scene surface HDC and passes it to `SpideyRenderer11_PresentHdc`.
- Renderer11 aspect-fits/copies that source into the hidden DX11 backbuffer, then calls DXGI `Present`.
- Because the backbuffer is not visible until DXGI Present, bar clearing/copying cannot expose the old GDI intermediate-frame flicker.
- On DX11 initialize/resize/present failure, Phase 1 is disabled for that session and the known-good direct scene->HWND GDI presenter is used automatically.
- No D3D7 native-2560x1440 change is included here. The 2560x1440 option remains intentionally quarantined until a later scene-rendering phase.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. build must succeed with renderer ABI 2;
3. game should reach the main menu;
4. expected compat log:
   - `renderer11_bridge loaded ... abi=2 expected=2 ... phase1_exports=1`;
   - `renderer11_phase1 initialize ... size=2560x1440 result=1`;
5. expected renderer log:
   - `initialize device_ready ... width=2560 height=1440 ...`;
   - `targets ready width=2560 height=1440`;
   - repeating `present_hdc frame=... src=... dst=2560x1440 ...`;
6. expected present log:
   - `compat_present_dx11 ... result=1`;
   - `present_path ... dx11=1 direct_hwnd=0 compat_result=2`.
7. If DX11 presentation fails, upload logs; expected fallback marker:
   - `renderer11_phase1 present_failed disabling_dx11_present_fallback=gdi_hwnd`;
   - game should still boot using `dx11=0 direct_hwnd=1 compat_result=1`.

Visual expectation for this test:
- image should look broadly the same as the current successful build;
- black side bars are still expected when the source is 4:3;
- 2560x1440 is still intentionally absent from the D3D7 mode list.


## Phase 1 first test stopped before launch: stale renderer11 object cache — 2026-09-30

Tested revision:
`25f2c51a52c6788950aa86fc6b025e9e234a29ee`

Observed:
- matching proxy force-cleaned, compiled, and linked successfully;
- renderer11 CMake configure/generate succeeded;
- renderer11 link then failed with exactly one unresolved export:
  `SpideyRenderer11_PresentHdc`;
- the build output did NOT show `spidey_renderer11.cpp` recompiling before the link;
- therefore the game never launched and this was not a runtime crash.

Root cause:
- the ZIP updater intentionally preserves `out/`;
- the Phase 1 source/API changed, but the preserved CMake/MSBuild tree contained an older renderer11 object file from Phase 0;
- archive extraction/source-refresh timestamps can be older than preserved object timestamps, so MSBuild considered the stale object current;
- the new .def file requested `SpideyRenderer11_PresentHdc`, while the reused old object did not contain it.

Fix:
- commit `422f8dd97e56a2d1a2016642637f3ce57e8a148e` — `renderer11: force clean modern builds after source refresh`;
- `scripts/build_renderer11.ps1` now deletes `out/renderer11/build` before every modern renderer configure/build;
- stale renderer11 DLL/PDB artifacts are also removed before rebuilding;
- this makes every test compile the modern DLL from the exact current source regardless of archive timestamps.

NEXT TEST:
1. rerun `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. expected output now includes:
   - `[..] Removing cached Direct3D 11 build tree...`;
   - a fresh CMake configure;
   - `spidey_renderer11.cpp` compiling;
   - successful renderer11 link;
3. only after that does the actual Phase 1 runtime presentation test begin.


## DX11 Phase 1 PASSED — 2026-09-30

Tested revision:
`a16d29d7fc6c4b60a817ca25ddcd5ef4ef599ad3`

User-visible result:
- game booted successfully;
- reached the main menu;
- started a new game;
- entered live gameplay and ran around for roughly a minute;
- user exited normally after the gameplay test.

Runtime proof:
- renderer bridge loaded with ABI 2 and `phase1_exports=1`;
- DX11 Phase 1 initialized successfully at a 2560x1440 client size;
- renderer11 created the hardware D3D11 device/swap chain and 2560x1440 targets;
- present log shows the visible windowed path consistently using `dx11=1 direct_hwnd=0 compat_result=2`;
- renderer11 `present_hdc` continued through at least frame 5160 with no logged fallback;
- frontend transitions continued to use the legacy 640x480 source while gameplay returned to the 1920x1440 D3D7 scene source;
- legacy DirectDraw primary remains 1920x1080 but is no longer the visible presentation owner;
- 2560x1440 remains intentionally quarantined from the legacy D3D7 render-mode path;
- black side bars therefore remain expected because the active gameplay source is still 1920x1440 (4:3), aspect-fitted to the DX11 2560x1440 swap chain at x=320.

Architecture now:
1. Retail/reconstructed D3D7 still creates and renders the scene.
2. The scene surface HDC is handed across the ABI-2 bridge.
3. `spidey_renderer11.dll` owns the real 2560x1440 DXGI swap chain.
4. DX11 copies/aspect-fits the legacy scene into its backbuffer.
5. DXGI Present is now the sole visible presentation path in compatibility/windowed mode.
6. Direct-HWND GDI presentation remains only as automatic fallback.

Phase 1 is CLOSED.

NEXT FRONTIER:
- remove the transitional GDI/HDC copy from the normal DX11 path and establish DX11-owned frame/texture upload;
- then migrate render resources/2D/fixed-function responsibilities until D3D7 no longer constrains scene dimensions;
- only once DX11 owns the scene render target should 2560x1440 be re-enabled as a true internal render resolution and 16:9 projection/FOV/UI work proceed.

Do not interpret the current 2560x1440 swap-chain size as native 2560x1440 game rendering yet: the current gameplay source remains 1920x1440 and the frontend remains 640x480.


## DX11 Phase 2A implementation ready for runtime test — 2026-09-30

Objective:
Remove GDI/HDC from the normal DX11 presentation path and establish a real DX11 texture/shader frame path while legacy D3D7 still renders the source scene.

Implementation:
- renderer ABI bumped to 3;
- new export: `SpideyRenderer11_PresentPixels`;
- modern helper now links `d3dcompiler`;
- D3D7 scene surface is locked after rendering completes;
- 32-bit X8R8G8B8/BGRA-compatible scene pixels are uploaded row-by-row into a dynamic `DXGI_FORMAT_B8G8R8A8_UNORM` D3D11 texture;
- a shader-model-4 fullscreen triangle samples that texture;
- point filtering preserves legacy pixel/UI sharpness;
- culling is explicitly disabled for deterministic fullscreen rendering;
- the DX11 render target is cleared to black, an aspect-fit viewport is selected, the textured triangle is drawn, and DXGI Present displays it;
- normal success returns presenter path code 3.

Fallback chain:
1. preferred: D3D7 surface Lock -> DX11 dynamic texture -> shader draw -> DXGI Present;
2. fallback: proven DX11 GDI-compatible HDC presenter;
3. final fallback: proven direct scene->HWND GDI presenter.

Legacy surface lock behavior:
- first attempt uses `DDLOCK_WAIT | DDLOCK_READONLY`;
- if that fails, retry with `DDLOCK_WAIT` only;
- unsupported/failing lock or unexpected pixel masks do not disable DX11; they fall back to the ABI-2 HDC bridge;
- only an actual `PresentPixels` failure disables the pixel path for the remainder of that process, again falling back to DX11/HDC.

Implementation commits:
- `1cf7b67e63ea14b084791203dd8b08a460e17964` — ABI 3 / pixel presenter declaration;
- `81d5262c786ed356385ebb60a26eecef87f11ee7` — pixel presenter export;
- `37da7baf56d80827628e546bc368290fd3b6a23f` — link d3dcompiler;
- `1cbc08494845077ce6a22e673db468247109f6ed` — shader/upload presentation pipeline;
- `f79703467838744641f94a1984b2861112ebe5dd` — proxy prefers locked-surface pixel upload;
- `57986686d1aa0fdc2bda6af6a0cebdbdf5d8e8fb` — DirectDraw lock retry;
- `5d79869e929dbde1292cccf10b44915b3d50f1b6` — point-filtered parity;
- `6574be387bafa9e13f77a11d0eb63df605e5c853` — deterministic no-cull fullscreen pass.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected build/runtime markers:
- clean modern renderer build;
- `renderer11_bridge loaded ... abi=3 expected=3 ... phase2_exports=1`;
- `renderer11_phase2 initialize ... result=1`;
- renderer log:
  - `blit_pipeline ready shader_model=4_0 filter=point cull=none`;
  - `upload_texture ready width=...`;
  - `present_pixels frame=...`;
- present log:
  - `compat_present_dx11_pixels ... result=1`;
  - `present_path ... dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

If the legacy surface cannot be locked or its pixel layout differs, expected safe fallback:
- `compat_present_dx11_pixels ... skipped ... fallback=dx11_hdc`;
- `present_path ... dx11=1 dx11_pixels=0 dx11_hdc=1 ... compat_result=2`.

Visual expectation:
- no intended visual change yet;
- 4:3 pillarboxing remains expected;
- 2560x1440 remains quarantined as a legacy D3D7 scene mode;
- this test proves the final image passes through a D3D11 texture and shader rather than GDI.

Texture/render migration map discovered while implementing Phase 2A:
- `PCTex_CreateTexturePVRInId` is the central legacy DirectDraw texture creation/upload path;
- `PCTex_ReleaseSysTexture` and `PCTex_ReleaseAllTextures` centralize legacy texture destruction;
- `PCTex_GetDirect3DTexture` exposes each texture's DirectDraw surface;
- `PCGfx_ProcessTexture` chooses the current texture and calls `DXPOLY_SetTexture`;
- queued textured quads place the DirectDraw texture in `DXPOLY::field_4`;
- `PCGfx_BeginScene` / `PCGfx_EndScene` bracket `DXPOLY_BeginScene` / `DXPOLY_EndScene`.

That map is the basis for Phase 2B: mirror legacy texture handles into DX11 SRVs and migrate 2D/textured draw submission away from D3D7 incrementally.


## DX11 Phase 2A PASSED — 2026-09-30

Tested revision:
`93c667546ee25ca567e01e5a0bc36daf0c4428cb`

User-visible result:
- game loaded normally with the Phase 2A shader-upload path active.

Runtime proof:
- ABI 3 bridge loaded with `phase2_exports=1`;
- DX11 initialized at 2560x1440;
- the fullscreen shader pipeline compiled/created successfully;
- a 1920x1440 BGRA8 upload texture was created for the gameplay/boot source;
- the D3D7 scene surface locked successfully on the first attempt (`lock_retry=0`);
- visible presentation used `dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`;
- frontend transition rebuilt the DX11 upload texture at 640x480 and continued on the same shader path;
- full uploaded logs contain no `failed`, `fallback`, `dx11_hdc=1`, or `direct_hwnd=1` marker;
- shader presentation continued through at least frame 3240.

Phase 2A is CLOSED.

NEXT FRONTIER — Phase 2B:
Introduce DX11 sidecar texture ownership keyed by the game's existing PCTex IDs. Mirror legacy texture creation/destruction into the renderer11 DLL while leaving D3D7 draws intact. This creates a verified DX11 texture inventory before any primitive class is switched over.


## DX11 Phase 2B implementation ready for runtime test — 2026-09-30

Objective:
Establish DX11 ownership of the game's individual texture resources before migrating primitive submission.

Architecture:
- renderer ABI is now version 4;
- renderer11 owns a 1024-entry sidecar texture table keyed by the existing PCTex texture ID;
- each mirrored texture is normalized to `DXGI_FORMAT_B8G8R8A8_UNORM` and receives a DX11 SRV;
- 16-bit, 24-bit, and 32-bit legacy RGB mask formats are accepted and converted using the actual DirectDraw surface masks;
- each sidecar stores the final legacy DirectDraw texture-surface handle;
- renderer11 maintains a fast legacy-handle -> texture-ID index;
- PCTex creation mirrors from the final converted system-memory staging surface, so DX11 receives the exact pixel result D3D7 receives after legacy palette/PVR conversion;
- PCTex release/release-all destroys the matching DX11 sidecar resource;
- DX11 is explicitly initialized after retail D3D7 init, with a lazy texture-init fallback if boot order ever creates a texture earlier.

DXPOLY coverage instrumentation:
- the existing D3D7 render path remains unchanged;
- `renderScene()` counts total polygons, textured polygons, mirrored texture hits, missing texture hits, and resident DX11 texture count;
- coverage is logged for the first five scenes and every 120th scene;
- no DX11 primitive draw is active yet.

Key implementation commits:
- `e7ca821bfff4745401b85d746a0e6d1d0f0f4e00` — DX11 game texture sidecar;
- `3452fa305e263df85c7ba616690ed7edc4f5c350` — correct resident counting on replacement;
- `779cdc4d1e58eb7b1b025997a71759b1efc020b3` — legacy bridge declarations;
- `22195d7063fd9547102ad1976dcf0cb46481fa87` — legacy->DX11 texture bridge and early initialization;
- `69ed6607724e88804dfdd884bf44de5f10a00b42` — PCTex creation/release lifetime mirroring;
- `522c81a3f0eac472647795b18dc5c92462f8dc20` — lazy texture initialization fallback;
- `d2c5566fbd906de64f041e14026d84714df576bb` / `5126e963741ca6fa37999d0f106d700a181d35eb` / `830c56dd3c89818941ce2db893bf82009d03f77c` — legacy surface-handle association;
- `e875b685f470a424465f32dfeb310da13bb659f7` / `a95f2c3f241d3d0665fc341454292216a0018962` — fast handle resolver;
- `cf9074a3e506c18bcd329d8e301518b376c9a173` — DXPOLY migration-coverage instrumentation.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected:
- renderer bridge log: `abi=4 expected=4 ... phase2b_exports=1`;
- compat log: `renderer11_phase2b early_initialize ... result=1` (or, if boot ordering differs, a successful `lazy_texture_initialize`);
- renderer log should contain many `texture_update id=... resident=...` and `texture_handle id=... handle=...` entries;
- legacy texture log should contain matching `dx11_mirror ... result=1` and `dx11_associate ... result=1`;
- `dx11_draw_coverage` lines should show how many real textured polygons resolve to mirrored DX11 textures;
- Phase 2A presentation should remain `dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

Important:
A successful 2B test still renders polygons with D3D7. It proves resource parity and handle coverage. Phase 2C will use these SRVs and the already-decoded `SDXPolyField` layout to replace the centralized D3D7 `DrawPrimitive(D3DPT_TRIANGLEFAN,...)` path incrementally.


## DX11 Phase 2B resource mirroring PASSED; retail draw hook required — 2026-09-30

Tested revision:
`ba3e494f52edf269d22f7cde82e78527978a2cdd`

User result:
- game booted normally;
- user entered gameplay and played successfully.

Resource-mirroring proof from the uploaded logs:
- renderer bridge loaded ABI 4 with `phase2b_exports=1`;
- early DX11 initialization succeeded at 2560x1440;
- 2,002 PCTex mirror operations succeeded;
- 2,002 legacy-handle associations succeeded;
- zero `dx11_mirror ... result=0` records were emitted;
- all observed mirrored source textures in this run were 16-bit A1R5G5B5-style surfaces (R=0x7C00 G=0x03E0 B=0x001F A=0x8000);
- resident DX11 texture count reached 568;
- Phase 2A presentation remained `dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3` through gameplay.

Important discovery:
- no `dx11_draw_coverage` lines were emitted at all;
- therefore the retail EXE is still executing its own `renderScene()` / DXPOLY loop;
- the reconstructed `DXsound.cpp::renderScene()` implementation is not a live patched path;
- the PCTex hooks are live because `patch_pctex()` redirects those functions, but DXPOLY has not yet been redirected.

Retail D3D7 device address recovered from existing runtime disassembly evidence:
- prior DX error diagnostics around the verified retail CreateDevice call at `0x004FEA4A` include:
  `A1 18 79 6B 00` -> retail IDirect3D7* at `0x006B7918`;
  `8B 15 08 79 6B 00` -> scene surface at `0x006B7908`;
  `68 1C 79 6B 00` -> address of the CreateDevice output slot;
- therefore the live retail `IDirect3DDevice7*` is stored at `0x006B791C`.

NEXT FRONTIER:
- hook the live retail IDirect3DDevice7 vtable rather than the reconstructed DXPOLY queue;
- first hook is diagnostic/pass-through only:
  - validate the device pointer with GetCaps;
  - wrap BeginScene/EndScene/DrawPrimitive (and optionally Clear) while always calling the original D3D7 methods;
  - resolve each current texture surface against the already-proven DX11 sidecar table;
  - log real retail draw coverage and FVF/primitive usage;
- do not suppress or alter D3D7 rendering until runtime coverage is proven.


## Phase 2C0 live retail DrawPrimitive probe ready — 2026-09-30

Purpose:
Observe the actual retail D3D7 primitive stream before suppressing or replacing any D3D7 draw.

Why this is needed:
- Phase 2B proved PCTex resource mirroring, but reconstructed `DXsound.cpp::renderScene()` is not on the live retail execution path;
- therefore coverage must be measured at the live COM device boundary rather than the proxy-owned reconstructed scene queue.

Implementation:
- recovered retail `IDirect3DDevice7*` slot: `0x006B791C`;
- validates the live pointer with `GetCaps`;
- resolves and validates the device vtable;
- hooks only `IDirect3DDevice7::DrawPrimitive` at vtable index 25;
- original method is preserved and ALWAYS called; this build does not suppress, duplicate, or replace a primitive;
- first/unusual draws log:
  - primitive type;
  - FVF;
  - vertex count/flags;
  - stage-0 legacy texture pointer;
  - resolved DX11 sidecar texture ID;
  - first TL vertex when FVF is 0x144;
  - viewport;
  - depth, alpha blend/test, fog, texture color/alpha ops, addressing, and filtering state;
- per presented frame logs aggregate calls/textured/mirrored/missing/primitive/FVF counts;
- probe is revalidated after display-option changes because retail can recreate the D3D7 device;
- probe is also revalidated at the verified Flip boundary;
- `tools/TEST_LATEST_BUILD.ps1` now captures `spidey-decomp-draw.log`.

Code commits:
- `cfa1db3759beba806a1e5a45fc92088e5d756c83` — live pass-through D3D7 DrawPrimitive probe;
- `524d17a3f7823cf54b5df586e8ee05ccfa0df986` — test harness captures draw log;
- `071e69d34a6a3903094e4b4e038cf7804ae767d5` — fixed-function/viewport state snapshot on sampled draws.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat` and boot through menus into gameplay.

Expected new log:
`spidey-decomp-draw.log`

Expected installation marker:
`draw_probe installed device_slot=0x006B791C ... index=25 ... getcaps_hr=0x00000000`

Expected live draw samples:
`draw_sample ... primitive=... fvf=... count=... texture=... mirrored_id=... v0=... state=... viewport=...`

Expected frame coverage:
`draw_frame frame=... calls=... textured=... mirrored=... missing=... triangle_fan=... fvf_0x144=... other_primitive=... other_fvf=... resident=...`

Safety expectation:
- visuals should be unchanged;
- every probed draw is still executed by the original D3D7 DrawPrimitive;
- Phase 2A framebuffer upload/presentation remains the visible path.

GO/NO-GO for actual Phase 2C rendering:
- GO when live retail draws are overwhelmingly/fully TRIANGLEFAN + FVF 0x144 and textured draws resolve to DX11 mirrored IDs with negligible missing coverage;
- any other primitive/FVF/state pattern will be implemented explicitly before D3D7 suppression.


## Phase 2C0 retail DrawPrimitive probe PASSED — gameplay run 2026-09-30

Tested revision:
`88c37819fb3935ed7d8a8dd649044a111e4b5752`

User result:
- booted successfully;
- entered gameplay and played for a while;
- no new visible regression reported.

Probe validation:
- retail EXE fingerprint still matches the known target;
- live D3D7 device slot `0x006B791C` validated with `GetCaps = S_OK`;
- DrawPrimitive vtable hook installed successfully across D3D7 device recreation;
- Phase 2A DX11 presentation remained active and stable.

Full draw-log aggregate:
- logged active draws: **2,423,890**;
- triangle fans: **2,423,890 / 2,423,890 (100%)**;
- FVF 0x144: **2,423,890 / 2,423,890 (100%)**;
- other primitive types: **0**;
- other FVFs: **0**;
- textured draws: **2,384,258**;
- DX11-mirrored textured draws: **2,383,026**;
- unresolved textured draws: **1,232**;
- mirrored coverage: **99.9483%** of textured draws;
- unresolved draws involve only **9 distinct DirectDraw surface pointers**;
- max observed calls in a logged frame: **9,316**;
- resident DX11 texture set reached 568.

Observed baseline fixed-function state:
- primitive = D3DPT_TRIANGLEFAN (6);
- FVF = 324 / 0x144;
- transformed/lit vertex shape matches XYZRHW + diffuse + UV;
- common depth: Z enabled, ZWRITE enabled for opaque and disabled for translucent, ZFUNC=4 (LESSEQUAL);
- common opaque blend: ALPHABLEND=0, SRC=2, DST=1;
- common translucent blend observed on unresolved surfaces: ALPHABLEND=1, SRC=5, DST=6;
- texture color/alpha op commonly 4 (MODULATE), ARG1=2 (TEXTURE), ARG2=0 (DIFFUSE);
- addressing observed WRAP(1) and CLAMP(3);
- MAG/MIN filter observed value 2 (linear);
- viewport tracks the retail internal target (e.g. 640x480 frontend).

Important conclusion:
The live retail renderer is dramatically narrower than a general D3D7 backend. The complete observed primitive stream is one primitive family + one FVF. This makes a direct DX11 TL-vertex compatibility pipeline practical.

NEXT FRONTIER:
1. add on-demand mirroring for the 9 transient/unresolved DirectDraw texture surfaces at the draw boundary;
2. extend renderer11 ABI for parallel retail primitive submission;
3. render triangle-fan/FVF-0x144 draws into a separate DX11 offscreen scene target while D3D7 remains the visible reference;
4. initially emulate the reconstructed fixed-function subset:
   - depth enable/write/compare;
   - blend modes used by DXPOLY_SetBlendMode;
   - texture modulate/select behavior;
   - texture alpha enable;
   - wrap/clamp;
   - point/linear filter;
   - viewport;
5. compare/log DX11 shadow frame coverage before suppressing any D3D7 draw.


## CHECKPOINT — live retail draw stream validated; ready for parallel DX11 geometry — 2026-09-30

Reason for checkpoint:
User explicitly requested a checkpoint before further renderer changes.

Last tested runtime revision:
`88c37819fb3935ed7d8a8dd649044a111e4b5752`

Observed runtime status:
- game booted cleanly;
- user entered gameplay and played without a reported regression;
- retail D3D7 DrawPrimitive probe installed successfully on the live device;
- retail device slot `0x006B791C` validated with `GetCaps=S_OK`;
- Phase 2A DX11 shader presentation remained active;
- Phase 2B DX11 texture sidecar remained stable.

Retail primitive stream — full-run aggregate:
- total observed active DrawPrimitive calls: **2,423,890**;
- **100%** primitive type = `D3DPT_TRIANGLEFAN`;
- **100%** FVF = `0x144` / decimal 324;
- other primitive types = **0**;
- other FVFs = **0**;
- textured draws = **2,384,258**;
- textured draws resolving to mirrored DX11 textures = **2,383,026**;
- unresolved textured draws = **1,232**;
- DX11 texture-handle coverage = **99.9483%**;
- unresolved draws are concentrated in only **9 distinct DirectDraw surface pointers**;
- maximum observed DrawPrimitive calls in a logged frame = **9,316**;
- resident DX11 texture set reached **568**.

Verified live vertex/state shape:
- transformed/lit vertex data is XYZRHW + diffuse + UV;
- triangle-fan stream matches the reconstructed `SDXPolyField` layout;
- common depth state: Z enabled, opaque Z-write enabled, translucent Z-write disabled, ZFUNC=LESSEQUAL;
- opaque: alpha blend disabled, SRC=ONE, DEST=ZERO-equivalent D3D7 values observed as SRC=2 DST=1;
- translucent path observed SRC=5 DST=6;
- texture color/alpha op commonly MODULATE;
- ADDRESSU/V observed WRAP and CLAMP;
- MAG/MIN filtering observed LINEAR;
- viewport follows the retail internal target, e.g. 640x480 in frontend.

Critical architecture conclusion:
The live renderer is not a broad arbitrary D3D7 workload. The complete observed primitive stream is one primitive family and one FVF, which makes a focused DX11 compatibility renderer practical.

Important dead-end avoided:
- reconstructed `DXsound.cpp::renderScene()` is NOT the live runtime scene loop;
- do not base the migration on reconstructed `gSceneBuffer`;
- the correct migration seam is the live retail `IDirect3DDevice7::DrawPrimitive` COM boundary.

NEXT SAFE IMPLEMENTATION STEP:
1. add on-demand mirroring at the live draw boundary for the 9 unresolved DirectDraw surfaces;
2. extend renderer11 ABI with a retail-TL-vertex draw submission entry point;
3. create a separate DX11 offscreen scene color/depth target;
4. shadow every eligible retail triangle-fan draw into that DX11 target while ALWAYS still calling original D3D7 DrawPrimitive;
5. emulate only the observed fixed-function subset first:
   - viewport;
   - depth enable/write/compare;
   - blend;
   - texture modulate/alpha;
   - wrap/clamp;
   - point/linear filtering;
6. log shadowed/skipped draw coverage and never make the DX11 shadow scene visible until coverage/state parity is demonstrated;
7. after parity, compare the offscreen DX11 scene against the D3D7 reference before suppressing any D3D7 draw.

Do NOT yet:
- expose 2560x1440 as a D3D7 scene mode;
- remove D3D7 DrawPrimitive;
- replace the visible framebuffer with the new geometry shadow target;
- discard the Phase 2A framebuffer-upload fallback.


## DX11 Phase 2C1 shadow geometry implementation ready for runtime test — 2026-09-30

Goal:
Render the real retail D3D7 primitive stream into a completely offscreen DX11 scene target while preserving the original D3D7 renderer as the visible/reference path.

Safety model:
- retail D3D7 DrawPrimitive is ALWAYS called;
- no D3D7 primitive is suppressed;
- the DX11 shadow color/depth targets are never copied to the swap chain;
- the existing Phase 2A D3D7-scene -> DX11 PresentPixels path remains the only visible presentation path;
- shadow replay is sampled on frames 1-5 and every 120th frame to avoid doubling ~9k draw calls every frame during parity bring-up.

ABI / resource changes:
- renderer11 ABI bumped to 5;
- persistent PCTex IDs remain 0..1023;
- synthetic transient DX11 texture slots use 1024..2047;
- unresolved DirectDraw texture surfaces are AddRef'd/queued during live draws and locked/mirrored only after retail EndScene at the verified Flip boundary;
- shadow commands retain unresolved legacy handles, so transient textures mirrored at Flip can still resolve before the same frame is replayed.

Live D3D7 state hooks:
- IDirect3DDevice7::SetRenderTarget (vtable 8);
- Clear (10);
- SetViewport (13);
- SetRenderState (20);
- DrawPrimitive (25);
- SetTexture (35);
- SetTextureStageState (37).

State-cache behavior:
- current viewport/render/depth/blend/texture-stage/texture/render-target state is initialized from the live D3D7 device after hook install;
- setter hooks update the cache only after the original retail call succeeds;
- sampled/unusual draws still query the live D3D7 state and log `cache_mismatch=0/1` so state-block/bypass behavior cannot silently invalidate the shadow renderer;
- shadow clears/draws are accepted only when the current retail render target equals the main scene surface at `0x006B7908`.

DX11 shadow renderer:
- TL vertex input matches retail FVF 0x144: XYZRHW + diffuse + UV;
- triangle fans are expanded to ordered triangle lists;
- XYZRHW is converted to clip space while preserving reciprocal-W for perspective interpolation;
- vertex colors are decoded from packed D3DCOLOR;
- texture MODULATE + diffuse is implemented;
- alpha operation MODULATE and SELECTARG2(diffuse alpha) are implemented;
- untextured draws use a 1x1 white texture;
- depth enable/write/compare states are cached;
- D3D7 blend factors are mapped to DX11, including safe alpha-slot normalization for color-derived blend factors;
- wrap/clamp and point/linear sampler states are cached;
- offscreen scene target is BGRA8 with D24S8 depth;
- all expanded vertices for a sampled frame are uploaded in one dynamic-buffer Map, then original draw ordering/state changes are replayed.

Parity diagnostics:
- renderer11 samples the same 3x3 quarter/center coordinates used by `SpideyLogSurfaceState`;
- DX11 `shadow_frame` logs include `sample_hash` and `nonblack`;
- this allows direct frame-number comparison against `scene_pre sample_hash` in `spidey-decomp-present.log`;
- exact hash equality is not required for the first test, but `nonblack > 0` and stable render counts are mandatory before making the shadow scene visible.

Implementation commits after the user checkpoint:
- `4524794d0a5da2745f2fffc2108d34d892b18b77` — ABI 5 shadow/transient API;
- `23c6bfef4f9eac04f8c1cab407d53cd0ee3bf21f` — exports;
- `2883b8fd0d68d4e20b34b591ae6cadf0e61ce807` — batched shadow storage;
- `b2d41be98188a229dfd28a7bd5afb38797062924` — offscreen TL-vertex pipeline/state caches;
- `ad0679b1a0e68f63dcc0ec683d9fa033f4689c83` — transient textures + shadow submission/replay;
- `da49f46328f6950c7821d562495076dfd4f70196` — late transient resolve before replay;
- `be2ebc7fb700ba3c434b795bf64fc70cc4bed684` — proxy loads ABI 5;
- `e38017cd7c06a4df905a55fce374b47c0301e428` / `aff06e2351c1260d8a2d733d9cd8251acff57d15` — legacy bridge structures/functions;
- `4c713592b8124faafb30b4469bedbaddb5a37d2d` / `927395f5eda8cbacb54f4e809a2bd8fee937af6b` — D3D7 state cache and setter wrappers;
- `db064b0d3cfb5222510d775db03c37ea9dec4e3a` — live draws enqueue into shadow queue;
- `e46d6ba656238e81e1b9811307b920f1275e7285` — per-frame shadow/transient coverage;
- `86a335e337cdcfb3dd747f494217f3f74723e78d` — live vtable state-hook installation;
- `755bfc8a17b3ba0d8f2e73baa2be78ad4a77105c` — EndScene/Flip shadow replay;
- `47bf8226116ca134bae8a6ffe1c0bd2b99addaf3` / `b1d49b8afa1db0eda6365ba572a6a41c39114402` / `e8a23ac33e9b0b3bf4faa359943c5707fcb6d38b` — sampled replay/capture and clean accounting;
- `dca790dedb85049a367f2f5cd3e44763b8cfe058` — cached-vs-live state validation;
- `21895ce6559193850657cd3e3ac783249aea86e2` / `c6451e4c57eec7936712c5e1c7afba82d7d8e819` / `247dd1de95de04b48f2c36738766a6257538d4e7` — main-scene render-target isolation;
- `6e94d5649a327ac452e7ac5619be1bb23def5f30` — matching 3x3 DX11 sample hash;
- `d4de1a0d6f49dc7eee973b26cbd4a089d607c8e1` — D3D7 blend-factor normalization for DX11 alpha slots.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`, boot through the frontend, start/enter gameplay, move around for at least a short interval, then exit normally.

Expected build/runtime markers:
- compat: `renderer11_bridge loaded ... abi=5 expected=5 ... phase2c_exports=1`;
- draw log: seven `state_hook installed` markers and
  `draw_probe installed ... state_hooks=7 shadow_state_valid=1`;
- sampled draw lines should report `cache_mismatch=0`;
- first unresolved legacy surfaces should produce `transient_mirror ... result=10xx` after EndScene;
- sampled `draw_frame` lines should show nonzero `shadow_submit`, ideally zero `shadow_skip`, and classify any non-main-target draws under `shadow_offscreen_skip`;
- renderer log should show:
  - `shadow pipeline ready shader_model=4_0 tl_vertex=1`;
  - `shadow targets ready width=...`;
  - `shadow vertex_buffer ready bytes=...`;
  - `shadow_frame ... replay=1 ... rendered=... skipped_render=... sampled=1 sample_hash=... nonblack=...`;
- visible present path must remain:
  `dx11=1 dx11_pixels=1 dx11_hdc=0 direct_hwnd=0 compat_result=3`.

GO criteria for Phase 2C2:
- normal boot/gameplay remains stable;
- no state-hook cache mismatches;
- transient surface mirroring succeeds or remaining misses are specifically characterized;
- sampled-frame shadow_submit/queued/rendered counts agree for main-scene draws;
- skipped_render is zero or fully explained;
- DX11 shadow sample is nonblack on actual rendered gameplay frames.

If those pass, next step is Phase 2C2: make a diagnostic copy/view of the DX11 shadow scene for visual parity inspection, correct any half-pixel/blend/depth differences, then prepare the first controlled switch where DX11 geometry becomes visible while D3D7 remains a fallback.


## DX11 Phase 2C1 PASSED — offscreen retail geometry shadow validated — 2026-09-30

Tested revision:
`ba57b5c7adaf49878b23f8a5cdc09bca33eb747a`

User-visible result:
- normal boot succeeded;
- no visible regression reported;
- existing D3D7-reference -> DX11 PresentPixels path remained the visible output.

Runtime validation:
- ABI 5 bridge loaded successfully with `phase2c_exports=1`;
- all seven retail D3D7 state/draw hooks installed successfully;
- live device validated with GetCaps=S_OK;
- sampled draw-state diagnostics reported **zero cache mismatches**;
- every sampled active retail frame had:
  - `shadow_submit == calls`;
  - `shadow_skip == 0`;
  - `shadow_offscreen_skip == 0`;
- renderer shadow replay had:
  - `queued == submitted == rendered`;
  - `skipped_submit == 0`;
  - `skipped_render == 0`;
- large gameplay samples successfully replayed thousands of real draws, including 8,271 draws at frame 2760 and 8,139 at frame 3240;
- shadow target sampled non-black on every compared active frame;
- the unusual frame 3120 had matching non-black occupancy on both paths: 8/9 samples;
- the remaining transient texture gap was successfully closed in this run:
  - 8 previously-unmapped DirectDraw surfaces were mirrored into synthetic IDs 1024..1031;
  - subsequent sampled frames reported `missing=0`;
  - one recovered surface was RGB565, demonstrating the transient conversion path is not limited to A1R5G5B5;
- resident DX11 texture count reached 576;
- renderer emitted no shadow setup/map/state creation failures.

Quantitative parity:
- the DX11 3x3 shadow sample hashes do not yet equal the retail D3D7 scene hashes;
- hash byte ordering was verified equivalent: both compute FNV-style accumulation over COLORREF-compatible 0x00BBGGRR values;
- therefore the mismatch represents actual pixel-value/rasterization/state differences, not merely diagnostic byte ordering;
- non-black occupancy nevertheless matches on every compared logged frame, strongly indicating the scene is structurally present and positioned plausibly.

Phase 2C1 conclusion:
The live retail primitive stream can now be reconstructed into an independent DX11 color/depth scene with complete sampled draw coverage and no observed state-cache divergence. D3D7 remains authoritative/visible.

NEXT FRONTIER — Phase 2C2:
- add exact nine-pixel sample-value logging to both D3D7 and DX11 diagnostics;
- add an opt-in runtime preview toggle that presents the DX11 shadow color target while D3D7 continues rendering in the background;
- run shadow capture continuously only while preview is enabled;
- preserve instant switch back to the known-good D3D7-reference presentation path;
- use direct visual comparison + per-pixel samples to correct half-pixel, blend, depth, texture/color, or viewport differences before suppressing any D3D7 draw.


## DX11 Phase 2C2 live shadow preview ready for runtime test — 2026-09-30

Phase 2C1 validation source run:
`ba57b5c7adaf49878b23f8a5cdc09bca33eb747a`

Validated before implementing 2C2:
- normal boot succeeded;
- all 7 live retail D3D7 hooks installed;
- ABI 5 bridge loaded;
- zero sampled cache mismatches;
- all sampled active frames had shadow_submit == retail calls;
- shadow_skip == 0;
- shadow_offscreen_skip == 0;
- renderer replay had queued == submitted == rendered and skipped_render == 0;
- 8 transient surfaces were recovered into synthetic IDs 1024..1031;
- subsequent sampled frames reported missing=0;
- resident DX11 texture count reached 576;
- shadow target was nonblack on every compared active sample;
- D3D7 and DX11 nonblack occupancy matched on every compared sampled frame, including 8/9 at frame 3120;
- renderer emitted no shadow setup/map/state creation failure.

Observed parity limitation:
- the 3x3 DX11 shadow hash does not yet equal the D3D7 scene hash;
- diagnostic byte ordering was verified equivalent, so the mismatch represents actual pixel/raster/state differences rather than hash encoding.

Phase 2C2 implementation:
- renderer ABI bumped to 6;
- new exports:
  - `SpideyRenderer11_ShadowSetContinuous(int)`;
  - `SpideyRenderer11_PresentShadow(int preserveAspect, int vsync)`;
- F10 toggles live DX11 shadow preview;
- enabling preview has a deliberate one-frame warmup so the first visible shadow frame is fully captured;
- while preview is enabled:
  - every main-scene retail triangle fan is shadow-captured;
  - shadow replay runs every frame;
  - the DX11 shadow color target is presented directly to the existing DXGI swap chain;
  - D3D7 still executes every original DrawPrimitive in the background;
- pressing F10 again immediately returns to the known-good D3D7 scene -> PresentPixels path;
- if PresentShadow fails, the proxy automatically falls back to the D3D7-reference presenter;
- preview remains aspect-fitted, so current 4:3 pillarboxing is expected;
- exact nine COLORREF sample values are now logged on both:
  - D3D7 `scene_pre ... samples=...`;
  - DX11 `shadow_frame ... samples=...`;
- expensive DX11 pixel readback remains sampled (first frames/every 120th), even while shadow rendering/presentation runs continuously;
- `present_path` now identifies `dx11_shadow=1`, preview enabled/ready state, and path code 4.

Implementation commits:
- `2b4b69b7e7de94ee73280e78e19d325193d88003` — ABI 6 declarations;
- `64e2b6d11e5b4f1a9e2fd3a3101fd54a7906acec` — ABI 6 exports;
- `2b36515e2598a0a16644ecb3c7e0ae9d19ee5fc1` — continuous shadow replay, direct shadow presentation, exact DX11 sample colors;
- `b99323d9a13f440f701b6a97840dfdee048e2fbe` — legacy preview-control declarations;
- `13c382f8dd1216a8c866652b7435e34b28618d92` — proxy loads ABI 6 preview exports;
- `2d6d5b8a962b93126f68b8af1aa0184b96e6bc30` — proxy preview bridge wrappers;
- `2fd8bddda9d05496571b87775dcdd289e6d802ea` — exact D3D7 sample colors;
- `ecb3eb6245c949a8a92a0eeba22050956e177d6e` — continuous capture while preview is active;
- `b279fcf9edc5beeff5159f0d5fd5565f48a73b15` — F10 preview toggle and path-4 presentation;
- `72f202b65d9eff538f2995dbf53119730a574bf0` — sampled-only parity readback during continuous preview.

NEXT TEST:
1. Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.
2. Boot normally into gameplay.
3. First verify the game still looks normal before touching F10.
4. Press **F10 once** to switch visible output to the DX11 shadow renderer.
5. Move around/look around for several seconds and inspect:
   - geometry placement;
   - textures;
   - HUD/menu;
   - transparency/blending;
   - depth/occlusion;
   - any half-pixel or shimmering offset;
   - missing/black/flickering elements.
6. Press **F10 again** and confirm the normal D3D7-reference image returns instantly.
7. If useful, toggle back and forth several times.
8. Exit normally and provide screenshots plus the captured logs.

Expected ABI/runtime markers:
- compat: `abi=6 expected=6 ... phase2c2_exports=1`;
- normal presentation before F10:
  `dx11_shadow=0 dx11_pixels=1 ... compat_result=3`;
- toggle:
  `shadow_preview_toggle ... enabled=1 key=F10`;
  renderer: `shadow_continuous enabled=1`;
- after one warmup frame:
  `present_path ... dx11_shadow=1 ... shadow_preview=1 shadow_ready=1 compat_result=4`;
  renderer: `present_shadow ...`;
- F10 off:
  `shadow_preview_toggle ... enabled=0`;
  renderer: `shadow_continuous enabled=0`;
  presentation returns to compat_result=3.

GOAL OF THIS TEST:
Visually characterize the first independently-rendered DX11 Spider-Man scene. Do not suppress D3D7 yet. Use the new exact sample colors + screenshots to correct parity before making DX11 authoritative.


## DX11 Phase 2C2 PASSED — live shadow renderer visually validated — 2026-09-30

Tested revision:
`27022600ea3d7328abe3f5cebd718ea73581134a`

User visual result:
- F10 DX11 shadow preview looked correct;
- user reported it may have looked slightly better than the D3D7-reference image;
- repeated toggling remained stable.

Runtime validation:
- ABI 6 loaded with `phase2c2_exports=1`;
- all seven retail state/draw hooks installed;
- repeated F10 on/off transitions were observed throughout the run;
- while DX11 shadow was visible, `present_path` reported:
  - `dx11_shadow=1`;
  - `dx11_pixels=0`;
  - `shadow_preview=1`;
  - `shadow_ready=1`;
  - `compat_result=4`;
- renderer11 continuously presented the shadow target at 1920x1440 aspect-fitted into the 2560x1440 swap chain;
- no renderer11 `failed`, `rejected`, `setup_failed`, `map_failed`, `present_shadow present_failed`, or no-free-slot diagnostics occurred;
- no `cache_mismatch=1` occurred;
- all logged sampled DX11 frames had `skipped_submit=0` and `skipped_render=0`;
- representative gameplay frame 4080:
  - retail calls = 5,007;
  - shadow_submit = 5,007;
  - missing = 0;
  - renderer queued/submitted/rendered = 5,007/5,007/5,007;
  - skipped_render = 0;
- transient texture recovery continued to work, including RGB565 surfaces;
- later frontend transition at frame 4200 still replayed all 3,587 draws successfully after transient recovery.

Exact pixel parity:
- matched D3D7/DX11 sample sets remain close but not bit-identical;
- across 35 matched nine-pixel sample frames, median absolute per-channel difference was 1 level;
- mean absolute per-channel difference was about 2.78 levels;
- this is consistent with small legacy-vs-DX11 raster/filter/color-math differences rather than missing scene content;
- visual inspection found no objectionable discrepancy.

Phase 2C2 conclusion:
The independent DX11 geometry renderer is visually viable and stable enough to become the default visible renderer.

NEXT FRONTIER — Phase 2C3:
- make DX11 geometry/shadow presentation enabled by default;
- retain F10 as an immediate A/B fallback to the D3D7-reference image;
- keep D3D7 DrawPrimitive executing in the background for one more validation stage;
- after default-DX11 runtime validation, begin a controlled mode that suppresses original main-scene D3D7 DrawPrimitive while leaving all state/texture/device plumbing intact.


## DX11 Phase 2C3 — DX11 geometry is now the default visible renderer — 2026-09-30

Implementation:
- `gSpideyShadowPreviewEnabled` now defaults to 1;
- the first Flip synchronizes renderer11 continuous shadow replay before the first shadow EndFrame;
- the default windowed/compat presentation path is therefore:
  retail state + primitive interception -> renderer11 DX11 scene -> DXGI swap chain;
- the old D3D7 scene -> PresentPixels path remains intact as the built-in reference fallback;
- **F10 now acts as the A/B reference toggle**:
  - default/on = DX11 geometry visible;
  - off = D3D7-rendered reference image visible;
  - toggling DX11 back on keeps the deliberate one-frame warmup to avoid presenting a partially captured frame;
- original D3D7 DrawPrimitive is still executed in the background in both modes for this validation stage.

Source commit:
- `4fc226ceb3e11003e6872feef9c1ad7a042a985e` — make DX11 geometry the default visible renderer.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. do **not press F10 initially**;
3. confirm startup, frontend, level load, HUD, and gameplay all look correct with DX11 now active by default;
4. play for several minutes;
5. press F10 once and confirm the old D3D7-reference image appears;
6. press F10 again and confirm DX11 returns after one warmup frame;
7. return to frontend / change level if convenient, to validate device/mode transitions while DX11 remains default;
8. exit normally and provide logs.

Expected markers:
- `shadow_default frame=... enabled=1 mode=dx11_geometry key=F10_reference_toggle`;
- renderer: `shadow_continuous enabled=1`;
- normal default presentation after startup:
  `dx11_shadow=1 dx11_pixels=0 shadow_preview=1 shadow_ready=1 compat_result=4`;
- F10 reference mode:
  `enabled=0` followed by `dx11_shadow=0 dx11_pixels=1 ... compat_result=3`.

If Phase 2C3 passes:
- begin Phase 2D: controlled suppression of original **main-scene** D3D7 DrawPrimitive while DX11 is authoritative;
- keep D3D7 state setters, texture/resource creation, and any offscreen passes intact initially;
- provide a reference-mode switch that re-enables D3D7 draws with a warmup frame;
- prove the visible game no longer depends on D3D7 geometry rendering before moving scene/depth/resource ownership further into DX11.


## DX11 Phase 2C3 PASSED; Phase 3A native desktop gameplay implemented — 2026-09-30

Latest user validation:
- DX11 geometry default-visible run completed cleanly;
- user reported no visual issues;
- F10 reference/DX11 toggling remained clean;
- the user noted tiny frametime hitches roughly every 0.5–1 second, but reports they were already present before the DX11 conversion and occur regardless of renderer;
- treat the hitch as a separate profiling item after native-resolution/aspect work unless new evidence ties it to renderer diagnostics.

Runtime evidence:
- ABI 6 / seven retail D3D7 hooks remained healthy;
- default mode starts with DX11 geometry enabled;
- once scene geometry becomes active, presentation runs through `dx11_shadow=1`, `dx11_pixels=0`, `compat_result=4`;
- sampled gameplay draws continue with zero shadow skips and zero missing textures after transient capture.

Phase 3A implementation:
- preserve a safe physical D3D7 compatibility surface;
- separate that from the game's **logical gameplay resolution**;
- after each retail display-mode transition:
  - frontend remains its original physical/logical 640x480 path;
  - gameplay logical resolution is set to the borderless client/desktop dimensions;
- on the current 2560x1440 desktop this creates:
  - D3D7 compatibility backing: 1920x1440 (current known-good mode);
  - game logical gameplay viewport: 2560x1440;
  - DX11 shadow color/depth target: 2560x1440;
- shadow DrawPrimitive state uses the modern logical viewport while the retail D3D7 device keeps its physical viewport;
- renderer11 therefore presents the gameplay shadow target 1:1 to the 2560x1440 swap chain instead of aspect-fitting a 1920x1440 target;
- frontend remains 4:3/pillarboxed for this first native-gameplay phase;
- F10 fallback now restores legacy logical dimensions before exposing the D3D7 reference, and keeps the completed DX11 frame visible during that one-frame handoff;
- F10 back to DX11 restores the desktop-native logical dimensions with the existing warmup.

Aspect handling:
- this is desktop/client driven rather than hard-coded 2560x1440;
- 16:9, 16:10, ultrawide, and other client aspects feed the same logical-resolution path;
- the first required runtime proof is 2560x1440/16:9 on the user's current display.

True Hor+ verification:
- the DrawPrimitive probe now logs per-frame TL-vertex X/Y ranges;
- it also reports how many vertices extend outside the physical 4:3 D3D7 width/height;
- if logical 2560x1440 causes vertices to populate X > 1920 while the vertical range remains appropriate, the original engine projection is naturally responding to the new logical width and true Hor+ is active;
- if geometry remains confined to the physical 4:3 range, the next step is a narrowly scoped upstream projection/FOV hook rather than stretching the image.

Implementation commits:
- `dee4d8165afd880cf2961d084cccd2ecd3585425` — desktop-native logical-resolution controls;
- `e3ce9f6bc8ebe4595a4c548291e97559b8eb609f` — apply modern logical gameplay dimensions after retail mode changes;
- `b4e57dc4e5e6309e8dc3c209eb2df437e4c36e70` — bind captured draws to modern logical viewport + geometry-range telemetry;
- `2884cde0e82513d3e413dbf75b7f3e10976b4a21` — native-sized DX11 target + clean F10 legacy handoff.

NEXT TEST:
Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Expected gameplay on the current system:
- compat log:
  `logical_render_resolution ... modern=1 frontend=0 logical=2560x1440 physical=1920x1440 client=2560x1440`;
- renderer log:
  `shadow targets ready width=2560 height=1440`;
  `present_shadow ... src=2560x1440 dst=2560x1440 rect=0,0,2560x1440`;
- present log should show DX11 shadow default without the old 320-pixel pillarbox;
- draw log should show:
  `modern=1 logical=2560x1440 physical=1920x1440 xrange=... outside_physical_x=...`.

Visual checks:
- gameplay fills 16:9 without horizontal stretch;
- compare vertical framing against the old 4:3 view: desired behavior is same vertical FOV with more world visible left/right;
- inspect HUD placement and cutscene/gameplay transitions;
- frontend is intentionally still 4:3;
- F10 should fall back to a clean 4:3 D3D7 reference, then restore native DX11 on the next toggle.


## Phase 3A runtime result — native settings were not exposed — 2026-09-30

Tested revision:
- `c8f41c98065585bac837fa3218896981dd98f44c`.

User result:
- no visible widescreen difference;
- 2560x1440 was not selectable in Display Options;
- there was no user-facing 16:9/aspect-ratio setting.

Runtime evidence:
- every observed Display Options apply remained the retail frontend request `640x480x16`;
- modern mode injection grew the mode table from 4 to 23 entries and detected the Windows 2560x1440 desktop, but the injector was still explicitly omitting 2560x1440 because of the earlier D3D7 CreateDevice failure;
- DX11 itself was already initialized with a 2560x1440 swap target, so the missing setting is a menu/configuration-layer problem, not a DX11 capability problem.

Root cause / revised design:
- the old 2560x1440 quarantine belongs only at the **legacy D3D7 physical backing** layer now;
- 2560x1440 must be exposed to the retail Screen Size selector and preserved as the user's selected/output resolution;
- selecting 2560x1440 will use a known-good 1920x1440 D3D7 compatibility backing while DX11 owns the requested 2560x1440 output;
- the original `PCSHELL_DoDisplayOptions` is at `0x0050D9B0` and is 1476 bytes;
- its three rows are Screen Size, Color Depth, Brightness;
- row 0 uses `DXINIT_GetPrevResolution`/`DXINIT_GetNextResolution`;
- row 1 uses `DXINIT_GetPrevColorDepth`/`DXINIT_GetNextColorDepth`;
- row 2 is brightness.

Aspect-ratio implementation plan:
- because DX11 output is always 32-bit, repurpose the obsolete Color Depth row as **Aspect Ratio** without changing CMenu row count/layout;
- patch the exact row-1 formatter and left/right helper calls in retail `PCSHELL_DoDisplayOptions`;
- modes: AUTO, 4:3, 5:4, 16:9, 16:10, 21:9, 32:9;
- apply the known retail projection/aspect scalar at runtime VA `0x00550064`;
- AUTO scalar = `(4 * height) / (3 * width)`; known explicit values include 4:3=1.0 and 16:9=0.75;
- persist the aspect selection in a small modern-video INI next to the game.

Current action:
- expose 2560x1440 in the retail resolution list;
- preserve requested modern resolution separately from the safe D3D7 backing;
- install the in-game Aspect Ratio row patch;
- then provide one new runtime frontier for the user to test.


## Phase 3B — in-game modern video settings implemented — 2026-09-30

Goal:
Make modern output resolution and aspect ratio explicit, selectable settings in the retail Display Options screen.

Retail UI reverse engineering:
- exact function: `PCSHELL_DoDisplayOptions = 0x0050D9B0`, size 1476 bytes;
- retained historical retail function bytes were used to verify all patch sites;
- byte-verified direct calls:
  - `0x0050DBBB -> 0x00529F90` (row-1 value formatter / sprintf);
  - `0x0050DDAB -> 0x005010C0` (previous color depth);
  - `0x0050DDCE -> 0x00501060` (next color depth);
- row-1 label pointer slot: `0x0054BBD4`;
- retail aspect/projection scalar: runtime VA `0x00550064`.

Resolution changes:
- 2560x1440 is no longer omitted from the injected retail resolution table;
- the existing Screen Size row can therefore enumerate it through the original `DXINIT_GetPrevResolution` / `DXINIT_GetNextResolution` logic;
- selected/output resolution is now separate from the legacy D3D7 physical backing;
- selecting 2560x1440 preserves/saves **2560x1440x32**;
- only the hidden legacy D3D7 backing is remapped to known-good **1920x1440x32**;
- startup restoration also preserves 2560x1440 instead of overwriting the saved config with a fallback;
- logical DX11 render dimensions now prefer the explicitly selected Screen Size rather than always following the desktop client.

Aspect-ratio menu:
- the obsolete Color Depth row is repurposed in-place as **Aspect Ratio**;
- this preserves the retail three-row CMenu layout and all existing input/drawing behavior;
- selectable values:
  - AUTO
  - 4:3
  - 5:4
  - 16:9
  - 16:10
  - 21:9
  - 32:9
- DX11 output is fixed at 32-bit, so removing user-facing color-depth selection does not remove a meaningful modern renderer option;
- explicit projection scalars:
  - 4:3 = 1.0
  - 5:4 = 1.06667
  - 16:9 = 0.75
  - 16:10 = 0.83333
  - 21:9 = 0.57143
  - 32:9 = 0.375
- AUTO computes `(4 * height) / (3 * width)` from the selected output resolution;
- the chosen scalar is written live to `0x00550064`.

Persistence:
- aspect selection is stored in `spidey-modern-video.ini` beside `SpideyPC.exe`;
- menu left/right changes save immediately;
- startup reloads the aspect mode;
- final display-mode application re-applies the scalar after Screen Size changes.

Safety:
- each retail call patch verifies opcode `E8` and the exact expected original target before writing;
- a mismatch is logged and the patch is skipped rather than writing to an unknown executable;
- the existing frontend 640x480 compatibility canvas remains intact;
- F10 DX11/reference fallback remains intact.

Implementation commits:
- `a1b685189f9e614b8fda21968bd4d824cb05877c` — expose 2560x1440 in Screen Size;
- `15de90532e319e5c161602b837437bc73b8492e7` — preserve selected output across legacy backing remap;
- `b2cad1aa4e1b833fe1b7751cdac60eb797d7e912` — separate selected output from D3D7 physical resolution at apply time;
- `99a459e5f3b163011faf17ba00f12c5aec30e76a` — repurpose Color Depth as Aspect Ratio using exact retail call patches;
- `f9f23c6dc59dfc460c9b17d12eb28d1d6746cc87` — install/persist/apply aspect selection;
- `6235292cdba5abeff5103d6a8bce9d6ba014fef4` — pin modern-video INI to the game directory.

NEXT TEST:
1. run `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. open Options -> Display Options;
3. verify the second row says **Aspect Ratio**;
4. cycle it and confirm AUTO / 4:3 / 5:4 / 16:9 / 16:10 / 21:9 / 32:9 are visible;
5. on Screen Size, cycle until **2560x1440** is visible;
6. select **2560x1440 + 16:9**;
7. leave/apply the menu, start gameplay, and inspect framing/HUD;
8. optionally re-open Display Options and confirm the selected resolution remains 2560x1440;
9. exit and provide the generated logs.

Expected log markers:
- `modern_modes ... windows_1440=1 ui_2560x1440_exposed=1 ...`;
- three `display_menu_patch ... installed=1` lines;
- `display_menu_mod ... label=1 format=1 prev=1 next=1 modes=7`;
- `display_aspect ... label=16:9 scalar=0.750000 selected=2560x1440`;
- `display_options selected=2560x1440x32 physical=1920x1440x32 ... legacy_backing_remap=1 preserve_selected=1`;
- gameplay: `logical_render_resolution ... logical=2560x1440 physical=1920x1440 selected=2560x1440`;
- renderer11 shadow target/presentation at 2560x1440.

Pass criteria:
- settings are actually visible/selectable in the original Display Options screen;
- 2560x1440 survives menu exit/re-entry and restart;
- 16:9 visibly affects gameplay projection without stretching;
- frontend remains stable.


## Phase 3C — transactional Display Options + Apply row — 2026-09-30

Tested revision that exposed the bug:
- `91ffd2884f2591eab48374386c342fbcdb1c30e6`.

User result:
- Screen Size and Aspect Ratio rows were visible;
- changing them did not reliably affect rendering;
- 2560x1440 would appear while cycling but reverted to 1920x1440 when Enter was pressed or the menu was reopened;
- user requested a dedicated **Apply** row so display changes can be committed without restarting the game.

Runtime evidence:
- all three original Phase 3B aspect call patches installed successfully;
- aspect cycling itself worked: log reached `16:9 scalar=0.750000`;
- committed output remained `1920x1440` throughout all `display_options` wrapper calls;
- presentation telemetry briefly observed saved globals at `2560x1440x32` while the user was cycling Screen Size, then later back at `1920x1440x32`;
- therefore retail Screen Size navigation was directly mutating the saved globals as a temporary menu variable, while the frontend display-options call restored our still-committed 1920x1440 selection.

Additional retail bug exposed by the Color Depth -> Aspect Ratio repurpose:
- after changing original Color Depth, retail calls `DXINIT_GetNextResolution` / `DXINIT_GetPrevResolution` to find a resolution compatible with the new bpp;
- those calls remained active after row 1 became Aspect Ratio, so aspect changes could silently mutate Screen Size;
- exact obsolete compatibility call sites:
  - `0x0050DDFB -> 0x00500E20`;
  - `0x0050DE1F -> 0x00500F40`.

Transactional menu design:
- add a fourth original CMenu entry: **Apply**;
- Screen Size and Aspect Ratio now edit separate pending values;
- pending Screen Size formatting no longer reads the committed/saved globals;
- pending resolution stepping still uses the original retail mode-table algorithms, but on local temporary width/height values;
- Aspect Ratio stepping modifies only pending aspect state;
- the old color-depth compatibility-resolution searches are disabled;
- pressing Enter on Screen Size / Aspect Ratio / Brightness no longer calls `DXINIT_SetDisplayOptions`;
- pressing Enter on **Apply** atomically:
  1. copies pending resolution/aspect to committed modern state;
  2. writes selected output to the retail saved width/height/bpp globals;
  3. writes the aspect projection scalar;
  4. applies the current frontend/device state safely;
  5. calls retail `SPIDEYDX_SaveSettings @ 0x00515850` immediately;
  6. resets pending state to the newly committed selection.
- Back/Escape without Apply leaves committed resolution/aspect unchanged.

Retail save verification:
- retained retail `SPIDEYDX_SaveSettings` bytes were disassembled;
- it serializes:
  - width from `0x02E096F8`;
  - height from `0x02E0970C`;
  - bpp from `0x02E098E4`;
  - brightness from `0x00562D60`;
- Apply therefore persists exactly the fields the original game saves to `Spidey.cfg`.

Exact new byte-verified call patches:
- `0x0050DA72 -> 0x0043FFF0`: intercept third AddEntry and append Apply;
- `0x0050DB56 -> 0x00529F90`: format pending Screen Size;
- `0x0050DCF8 -> 0x00500250`: Enter/confirm becomes Apply-only commit;
- `0x0050DDFB -> 0x00500E20`: disable aspect->resolution compatibility step;
- `0x0050DE1F -> 0x00500F40`: disable fallback aspect->resolution compatibility step;
- `0x0050DE71 -> 0x00500F40`: previous Screen Size operates on pending state;
- `0x0050DE88 -> 0x00500E20`: next Screen Size operates on pending state;
- existing row-1 formatter/prev/next patches remain byte-verified.

Additional fix:
- frontend-mode recognition no longer incorrectly requires brightness option value 4; changing brightness can no longer cause the 640x480 frontend request to be mistaken for gameplay resolution.

Implementation commits:
- `cdc6b5206e9aaed1c67a502b24f7cc0a657b82e2` — stage resolution/aspect and add Apply row;
- `080152a3d206063038fbe18f26d54f8b8d1edfe7` — Apply-only atomic commit, immediate retail save, and frontend-brightness classification fix.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. open Display Options and verify four rows:
   - Screen Size
   - Aspect Ratio
   - Brightness
   - Apply
3. set Screen Size to **2560x1440**;
4. set Aspect Ratio to **16:9**;
5. move to Apply and press Enter;
6. verify the menu remains usable and Screen Size still reads 2560x1440;
7. back out and reopen Display Options; it must still read 2560x1440 + 16:9;
8. start gameplay without restarting the process;
9. verify gameplay uses the selected modern resolution/aspect;
10. exit and provide logs.

Expected new log markers:
- `display_menu_mod ... rows=4 ... applyentry=1 applyconfirm=1`;
- `display_pending_reset reason=menu_open ...`;
- Screen Size cycling:
  `display_pending_resolution direction=... value=2560x1440 committed=1920x1440`;
- Aspect cycling:
  `display_pending_aspect ... value=16:9 committed=...`;
- Apply:
  `display_aspect reason=display_menu_apply_commit ...`;
  `display_apply committed=1 selected=2560x1440x32 aspect=16:9 ... saved_now=1`;
- reopening:
  `display_pending_reset reason=menu_open selected=2560x1440 aspect=16:9`;
- gameplay transition:
  `display_options selected=2560x1440x32 physical=1920x1440x32 ... legacy_backing_remap=1`;
  `logical_render_resolution ... logical=2560x1440 physical=1920x1440 selected=2560x1440`.


Phase 3C install-order correction:
- the Apply-specific `0x0050DCF8` hook must install before `SpideyInstallDisplayOptionsCompat`, because the generic installer scans and rewrites all remaining direct calls to `0x00500250`;
- install order is now:
  1. modern mode reinit compat;
  2. transactional Display Options / Apply hooks;
  3. generic display-options compat for all remaining retail call sites;
- expected `display_options_compat patched_calls` changes from 4 to **3** by design;
- correction commit: `016b7438fd7008d8e6075105c0b4f2e9201d5703`.


## Phase 3C pre-runtime build blocker — MSVC6 __thiscall typedef — 2026-09-30

User test of revision `2f14244f05ad2c1946a6668b89fe60064ca9b409` did not reach runtime.

Build result:
- forced clean matching build started normally;
- compile stopped at `main.cpp(1573)`;
- MSVC6 error: `C4234: nonstandard extension used : '__thiscall' keyword reserved for future use`;
- therefore no Phase 3C runtime conclusions can be drawn from this attempt.

Root cause:
- the new Apply-row code declared retail `CMenu::AddEntry @ 0x0043FFF0` through an explicit `__thiscall` function-pointer typedef;
- this matching compiler does not support spelling `__thiscall` there.

Compatibility fix:
- declare the raw retail function pointer as `__fastcall(CMenu*, void*, const char*)`;
- pass an unused dummy second parameter;
- this is ABI-compatible with the x86 retail member call for this target:
  - `CMenu* this` remains in ECX;
  - unused dummy occupies EDX;
  - the actual label argument remains on the stack;
  - both conventions use callee stack cleanup for the stacked argument.
- no retail address or Phase 3C menu logic changed.

Next action:
1. update/build again with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. if matching build succeeds, perform the existing four-row Display Options / 2560x1440 + 16:9 + Apply runtime test;
3. only then evaluate pending/committed settings behavior.


## Phase 3C runtime result — PASS; Phase 3D begins — 2026-09-30

Tested revision:
- `9fa0e6ab1027e259052fe0b8486f9e6ade57b0f4`.

User result:
- selected resolution and aspect ratio now apply in gameplay;
- requested 2560x1440 + 16:9 survives Apply and menu re-entry;
- gameplay visibly uses the requested modern output/aspect;
- new issues observed:
  1. level backgrounds/backdrops are strongly distorted, especially while the camera moves;
  2. main menu/frontend remains low-resolution 4:3 instead of following the selected output/aspect.

Runtime proof from `spidey-decomp-compat(20260930-195855).log`:
- every transactional Display Options patch installed, including `apply_entry=1` and `apply_confirm=1`;
- `display_apply committed=1 selected=2560x1440x32 aspect=16:9 ... saved_now=1`;
- reopen resets pending state from committed `2560x1440 + 16:9`;
- gameplay transition reports:
  `logical_render_resolution reason=display_options_gameplay modern=1 frontend=0 logical=2560x1440 physical=1920x1440 selected=2560x1440`.
- frontend transition still reports:
  `logical_render_resolution reason=display_options_frontend modern=0 frontend=1 logical=640x480 physical=640x480 selected=2560x1440`.

Phase 3C conclusion:
- transactional Screen Size / Aspect Ratio / Apply behavior is validated;
- 2560x1440 remains physically quarantined from the legacy D3D7 device as intended;
- do not reopen the old Apply/persistence issue unless new evidence regresses it.

Phase 3D current goals:
1. remove the intentional 640x480 frontend logical-resolution lock while preserving retail UI semantics and stability;
2. diagnose/fix moving gameplay backdrop/background distortion separately from general widescreen output.

Initial backdrop evidence:
- gameplay DX11 shadow draws are active at logical 2560x1440 with physical D3D7 backing 1920x1440;
- frame telemetry includes transformed vertices with extreme screen-coordinate ranges (millions), while normal frontend pre-transformed geometry stays around the original 640x480 coordinate space;
- investigate special transformed/backdrop geometry and RHW/viewport conversion before changing projection globally.


### Phase 3D implementation frontier — modern frontend + TL clipping correction — 2026-09-30

Source commits:
- `653680d8bb6e4661964cfdb83a90bdc4f85737a6` — preserve D3D7 transformed-vertex screen-space clipping in renderer11;
- `f74f2ee31e85cfe291bd8dbaa95f9c687c340335` — drive frontend logical resolution / DX11 replay target from selected modern output.

#### Background/backdrop distortion correction

Observed geometry is D3D7 FVF 0x144 / XYZRHW: X/Y/Z are already transformed screen-space values and RHW is carried for perspective interpolation.

Previous renderer11 conversion:
- computed NDC from X/Y;
- reconstructed clip W as `1 / RHW`;
- multiplied X/Y/Z by that clip W;
- let DX11 clip in varying homogeneous W.

That is not faithful to already-transformed TL geometry when very large off-screen triangles cross viewport boundaries. Gameplay telemetry contains exactly those extreme transformed coordinates, and the visual symptom is moving/warping backgrounds.

New conversion:
- keep output clip position in screen-space-derived NDC with fixed shader `W = 1`;
- store source RHW in the input position.w payload only;
- perform screen-space clipping with constant W;
- preserve RHW texture perspective explicitly by interpolating `uv * rhw` and `rhw`, then dividing in the pixel shader;
- depth remains source post-transform Z;
- F10 D3D7 reference path remains available.

Expected renderer marker:
`shadow pipeline ready shader_model=4_0 tl_vertex=screen_space manual_uv_perspective=1`.

#### Modern frontend/menu resolution

The frontend still requests a safe legacy 640x480x16 D3D7 device. That physical compatibility backing is intentionally retained.

What changes:
- selected modern output is now allowed in frontend as the game's logical render resolution;
- the shell/global logical width/height can therefore be 2560x1440 while the hidden legacy frontend device remains 640x480;
- DX11 shadow capture/replay viewport and target use the selected modern output in frontend as well as gameplay;
- F10 reference mode still calls the physical-resolution logical path.

This deliberately separates:
- **legacy physical backing**: 640x480 frontend / 1920x1440 backing for selected 2560x1440 gameplay;
- **modern logical + visible DX11 output**: selected resolution/aspect in both frontend and gameplay.

Expected frontend markers after selecting 2560x1440:
- `logical_render_resolution reason=display_options_frontend modern=1 frontend=1 logical=2560x1440 physical=640x480 selected=2560x1440`;
- draw frames in frontend: `modern=1 logical=2560x1440 physical=640x480`;
- renderer11 shadow target remains `2560x1440` across gameplay -> frontend transitions.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. confirm build succeeds and renderer log contains `tl_vertex=screen_space manual_uv_perspective=1`;
3. confirm Display Options still show/persist 2560x1440 + 16:9;
4. inspect the main menu before gameplay and after returning from gameplay:
   - it should now use the selected modern resolution/aspect rather than a 640x480 4:3 render;
   - check menu artwork, text, cursor/mouse hit-testing, and Options navigation;
5. enter a level and specifically watch distant level backgrounds/backdrops while moving and rotating the camera;
6. verify foreground geometry, HUD, transparency, and texture perspective remain stable;
7. press F10 only if an A/B reference is useful; DX11 should be default and D3D7 should still provide the old reference;
8. exit and provide full logs plus screenshots of main menu and a gameplay scene if distortion remains.

Do not remove the 640x480 frontend D3D7 backing yet. This test is about modern logical/DX11 frontend ownership while retaining the safe compatibility device.


## Phase 3D runtime result — visual pass; live-apply/input follow-ups — 2026-09-30

Tested revision:
- `e836711981744db3916b6dad636f88879ead0bfb`.

User result:
- modern frontend/main-menu rendering is a major visual improvement;
- the transformed-vertex clipping correction makes gameplay backgrounds/backdrops look substantially better;
- two remaining UX issues:
  1. changing Display Options and pressing Apply commits/saves the selection, but the visible running scene does not always reflect the new setting until restart;
  2. after leaving gameplay and returning to the main menu, the mouse cursor still moves but mouse clicking/hover selection no longer works correctly.

Runtime proof:
- renderer11 initializes at 2560x1440 and the new pipeline marker is active:
  `tl_vertex=screen_space manual_uv_perspective=1`;
- frontend is now modern logical 2560x1440 while retaining the safe hidden 640x480 D3D7 backing;
- Apply commits are recorded for both 4:3 and 16:9 in the same run, including `saved_now=1`;
- gameplay is modern logical 2560x1440 over the 1920x1440 legacy compatibility backing;
- steady-state observed main-scene frames are entirely the expected triangle-fan / FVF 0x144 subset and submit all observed draws to renderer11 with `shadow_skip=0`;
- transient texture misses occur during resource/mode transitions but settle to zero after mirroring.

Interpretation:
- persistence/transaction logic is working;
- remaining Apply issue is **live presentation/layout activation**, not failure to save the setting;
- mouse device is still live (cursor movement continues and input log shows successful foreground acquisition); likely failure is stale frontend mouse coordinate/bounds state across gameplay -> frontend logical-resolution transition.

### DX11 migration status after Phase 3D

Current validated position:
- DX11 owns the visible modern output path;
- DX11 mirrors retail textures and fixed-function state;
- DX11 replays effectively 100% of the observed current main-scene primitive stream in sampled frontend/gameplay frames;
- modern 2560x1440 logical rendering and selected aspect behavior are operational;
- transformed TL-vertex clipping/perspective semantics are now much closer to retail D3D7 and visually validated.

Still legacy-dependent:
- retail D3D7 device/surfaces still exist as compatibility/source infrastructure;
- retail D3D7 still produces the transformed primitive/state stream that renderer11 mirrors/replays;
- some offscreen/resource/movie/device-lifecycle responsibilities still pass through legacy DirectDraw/D3D7 infrastructure;
- F10 intentionally preserves a D3D7 reference path for A/B diagnosis.

Next renderer milestone is therefore not basic draw coverage; it is **removing D3D7 as the producer/dependency beneath the already-working DX11 visible path**.

### New major modernization goals accepted

#### Modern controller layer
Target:
- complete modern gamepad support;
- left-stick movement;
- right-stick camera;
- triggers/bumpers/start/back/stick buttons;
- deadzones, sensitivity, inversion;
- rumble;
- controller menu navigation;
- persistent per-action remapping;
- dynamic button-prompt UI/glyphs based on last active input device;
- retain keyboard/mouse interoperability.

Existing game already has useful foundations:
- controller action-mapping tables and setters/getters;
- controller polling and button-state paths;
- legacy controller configuration/menu concepts;
- force-feedback entry points.

Plan:
- add a normalized modern controller state layer rather than exposing raw legacy DirectInput joystick assumptions directly to gameplay;
- feed that state into the existing action mapping where appropriate;
- keep a dedicated right-stick axis pair available for modern camera control.

#### Modern mouse/right-stick camera
Target:
- user-controlled third-person camera during normal gameplay;
- mouse and right-stick yaw/pitch;
- Spider-Man movement no longer forcibly dictates camera heading;
- preserve scripted, boss, cutscene, fixed, special traversal, and camera-collision behavior;
- optional configurable recenter behavior rather than mandatory continuous recentering.

Existing game camera has strong reusable foundations:
- camera modes include NORMAL, LOOSE, USER, LOOKAROUND and others;
- camera angle/distance/offset setters already exist;
- player `PutCameraBehind` is a concrete current recenter path that follows Spider-Man heading;
- lookaround setup/exit machinery and camera-angle locks already exist in retail.

Recommended architecture:
1. finish current live Apply + frontend mouse-state fixes;
2. stabilize the DX11/D3D7 lifecycle boundary;
3. create normalized modern input/controller layer;
4. add mouse/right-stick free-look as an overlay on existing camera state, suppressing automatic `PutCameraBehind` recenter only in normal user-controlled gameplay;
5. add button glyphs/remapping UI after normalized action state is stable.

Immediate next work:
- make Apply activate selected output/aspect in the current running frontend/gameplay without restart;
- synchronize frontend mouse coordinate/bounds state whenever legacy physical backing and modern logical output diverge or transition.


### Phase 3D follow-up implementation — live Apply + frontend mouse transition — 2026-09-30

Implementation commits:
- `e6797491c77a83842dee5c70d2e77b8d2ed5653e` — resynchronize retail frontend mouse bounds after display/mode transitions;
- `70efef29479e6fc66cb42e54ec6001992bca425f` — make selected Aspect Ratio define the live non-stretched DX11 content canvas inside the selected Screen Size.

#### Retail mouse RE / fix

Original-symbol database and retained retail function bytes identify:
- `PCINPUT_SetMouseBounds @ 0x0050A6B0`;
- `PCINPUT_SetMousePosition @ 0x0050A700`;
- `PCINPUT_GetMousePosition @ 0x0050A750`;
- `PCINPUT_IsMouseOver @ 0x0050A820`;
- `PCINPUT_UpdateMouse @ 0x0050A8A0`;
- `PCSHELL_Initialize @ 0x0050C010`;
- `PCSHELL_IsMouseOver @ 0x0050C5F0`.

Retained retail disassembly proves:
- `PCSHELL_Initialize` establishes mouse bounds from live legacy dimensions `0x006B78E4/0x006B78E8`, minus 32 pixels;
- that bounds initialization occurs only when the shell cursor sprite is first created;
- `PCINPUT_SetMouseBounds` stores the four clamp limits at retail globals `0x00AC0924/2C/28/30`;
- `PCINPUT_UpdateMouse` clamps mouse X/Y against those stored bounds;
- `PCINPUT_IsMouseOver` uses current mouse X/Y and the current resolution-dependent hotspot calculation.

Therefore gameplay -> frontend can leave gameplay-sized mouse clamp bounds alive after the frontend physical D3D7 canvas returns to 640x480.

New behavior:
- every frontend display transition explicitly calls the retail mouse-bounds API using the current frontend physical canvas;
- current mouse position is preserved when already valid;
- if the old gameplay position lies outside the new frontend domain it is safely recentered;
- input log marker:
  `retail_input event=frontend_bounds_sync ...`.

#### Live Aspect / Apply behavior

Apply was already committing and persisting state correctly. The latest test proved both 4:3 and 16:9 commits in one run, but the frontend had no immediate visible aspect treatment because Screen Size remained the full DX11 canvas.

New model:
- **Screen Size** = selected output/swap-chain extent;
- **Aspect Ratio** = largest non-stretched logical content canvas that fits inside that output;
- AUTO = use the full selected output aspect;
- explicit aspect modes aspect-fit inside the selected output.

Examples at 2560x1440:
- 16:9 -> 2560x1440 content;
- 4:3 -> 1920x1440 content, centered/pillarboxed by the existing aspect-preserving DX11 presenter;
- 5:4 -> 1800x1440 content;
- 16:10 -> 2304x1440 content;
- 21:9 -> 2560x1097 content;
- 32:9 -> 2560x720 content.

The retail projection scalar remains applied as before. The logical content canvas is refreshed immediately inside the existing Apply path, so no restart should be required.

Expected compat marker now includes both output and live content:
`logical_render_resolution ... selected=<output> content=<aspect-fit-content> aspect=<mode>`.

NEXT TEST:
1. update/build with `UPDATE_AND_TEST_LATEST_BUILD.bat`;
2. in Display Options at 2560x1440:
   - switch 16:9 -> 4:3 and press Apply;
   - verify the running frontend changes immediately to centered 4:3/pillarboxed content without restart;
   - switch back to 16:9 and Apply;
   - verify it immediately returns to full 2560x1440;
3. verify the Display Options values remain committed after closing/reopening;
4. enter gameplay, then back out to main menu;
5. move and click the mouse across several main-menu items;
6. verify input log contains `frontend_bounds_sync` after the mode transition;
7. enter gameplay once more to ensure the mouse fix did not disturb keyboard/gameplay input;
8. provide the full new log set; screenshots are useful if any aspect mode is stretched or miscentered.


### Phase 3E prepared — guarded removal of D3D7 main-scene DrawPrimitive — 2026-09-30

Implementation:
- `502bd2864c5e6fd8a1e268f18656591a8b709d0c` — opt-in F9 D3D7 main-scene draw suppression trial.

Purpose:
- distinguish "DX11 can replay everything visibly" from "the retail D3D7 color/depth main-scene draw is still required for hidden side effects";
- prove the next renderer boundary without deleting the fallback.

Behavior:
- default OFF, so the existing Apply/mouse regression test is unchanged;
- F9 toggles only the diagnostic suppression flag;
- a retail main-scene `DrawPrimitive` is suppressed only when ALL are true:
  - DX11 geometry mode is enabled;
  - DX11 has completed its warmup and is the ready visible path;
  - the draw targets the retail main scene, not an offscreen surface;
  - the exact draw was accepted by the DX11 triangle-fan replay path;
  - the draw has no texture or its texture is already mirrored/resident;
- unsupported state, unresolved transient texture, offscreen draw, warmup frame, and F10 reference mode all fall back to the original D3D7 call;
- F10 therefore remains a complete D3D7 reference even if F9 suppression is armed.

New telemetry:
- present log:
  `d3d7_main_draw_suppression frame=... enabled=... effective=... dx11=... ready=... key=F9`;
- draw-frame log adds:
  - `d3d7_suppress=<0|1>`;
  - `d3d7_suppressed=<count>`;
  - `d3d7_fallback=<count>`.

Interpretation for a future F9 test:
- if image/gameplay remains unchanged while `d3d7_suppressed` approaches main-scene draw count and fallback stays near zero in steady state, D3D7 main-scene rasterization is no longer functionally required for the visible path;
- any missing effects/readback-dependent behavior means those specific dependencies must be identified before permanent suppression;
- do NOT suppress offscreen D3D7 work yet because transient textures/resource generation may depend on it.

This diagnostic is intentionally layered behind the current Phase 3D user-facing fixes. First validate live Apply + mouse return; then use F9 as the next renderer-isolation experiment.

### Camera modernization note — do not over-commit to legacy camera internals

User explicitly wants the option to modernize the camera beyond the feel/limitations of the original system after trying an initial implementation.

Therefore:
- existing camera modes/helpers are useful RE anchors and may provide collision/script transition knowledge;
- they are **not** an architectural requirement for the final modern camera;
- design the modern input/camera boundary so a later dedicated camera controller can own yaw, pitch, distance, smoothing, collision and recenter policy directly while only yielding to explicit scripted/cinematic camera ownership.

Useful retail camera hook anchors from the original symbol database:
- `CCamera::PushMode = 0x00416720`;
- `CCamera::PopMode = 0x00416780`;
- `CCamera::SetCamAngle = 0x004178E0`;
- `CCamera::SetCamXZDistance = 0x004179F0`;
- `CCamera::SetCamYDistance = 0x00417A70`;
- `CPlayer::SetCamAngleLock = 0x004B9E10`;
- `CPlayer::EnterLookaroundMode = 0x004C3580`;
- `CPlayer::ExitLookaroundMode = 0x004C3810`;
- `CPlayer::SetupLookaroundCamera = 0x004C38A0`;
- `CPlayer::PutCameraBehind = 0x004C64A0`.

Recommended long-term camera split:
1. normalized mouse/right-stick input produces camera intent;
2. modern camera controller owns ordinary gameplay camera transform;
3. retail scripted/boss/cutscene state can temporarily claim camera ownership;
4. on return to ordinary gameplay, modern camera resumes without being forced behind Spider-Man;
5. legacy collision/lookaround code may be reused selectively or replaced entirely based on feel/testing.


#### Phase 3E static scene-dependency audit

Additional static checks before runtime F9 use:
- `PCMovie.cpp` copies decoded movie frames into `g_pDDS_Scene` with DirectDraw `Blt`; F9 only intercepts D3D7 `DrawPrimitive`, so the movie copy is outside the suppression scope;
- retained retail `DXPOLY_SaveScreen @ 0x005033E0` was disassembled from the original function bytes and references the primary surface `0x006B7904`, not the scene surface `0x006B7908`;
- D3D7 offscreen render-target draws are never suppressed.

This does not prove there are no obscure same-frame scene-surface consumers, but it removes two obvious risks and keeps the trial reversible/fail-closed. Runtime F9 testing remains the authority before making D3D7 main-scene suppression permanent.


### Modern input/camera architecture documented

New design document:
- `docs/MODERN_INPUT_CAMERA.md`
- commit `811e5713b4afd8c5d5f62f3324ad75202e29ffec`.

Key compatibility seam:
- `PCINPUT_GetMappedStates @ 0x0050A190` is the preferred initial digital-action injection point;
- `Pad_Update @ 0x00505720` remains the downstream legacy action consumer;
- right-stick/mouse camera intent must stay separate from the legacy digital mask.

The document explicitly preserves the option to replace ordinary legacy camera ownership with a dedicated modern gameplay camera after evaluating the first free-look prototype.


## Modern Input Phase 0 implementation begins — passive helper foundation — 2026-09-30

New source:
- `input11/CMakeLists.txt`;
- `input11/include/spidey_input11_api.h`;
- `input11/spidey_input11.def`;
- `input11/src/spidey_input11.cpp`;
- `scripts/build_input11.ps1`.

Initial commits:
- `22bc97d441a12cf4fd9a7d1f51e7142fc6e410d7`;
- `21cffdd9262d176ca8e7eef493d4d7a2f9553554`;
- `254349907ce351ed05b1cf4e886d9aee34f52065`;
- `9686366d3fd65369918d84c8d422b60ab45e7806`;
- `e50cb442fdda4a006fe79a6e28c9dafcd76867f3`.

Architecture:
- modern input lives in a separate VS2022 Win32 helper DLL, matching the successful renderer11 split;
- proxy compatibility layer will communicate through a versioned C ABI;
- ABI v1 exposes one normalized controller snapshot:
  - connected/device family/user index/packet number;
  - semantic button bitset;
  - normalized `moveX/moveY`;
  - normalized `cameraX/cameraY`;
  - normalized left/right triggers;
- rumble API is included from the start.

First provider:
- dynamic XInput loader;
- probes `xinput1_4.dll`, `xinput1_3.dll`, `xinput9_1_0.dll`, `xinput1_2.dll`, `xinput1_1.dll` in order;
- no XInput import library dependency is added to the matching proxy;
- standard radial left/right-stick deadzones and trigger threshold are normalized in the helper;
- up to four XInput users are scanned, preserving the current active user while connected.

Important scope:
- this first provider proves the modern input ABI with Xbox/XInput-compatible pads;
- PlayStation/native HID/generic-pad support is intentionally **not** encoded as XInput assumptions in the ABI and can be added as later providers;
- modern input is still passive and does not replace retail gameplay controls yet.

Next implementation steps:
1. add helper build/install/log collection to `TEST_LATEST_BUILD.ps1`;
2. load/probe/poll `spidey_input11.dll` from the proxy without injecting actions yet;
3. validate helper telemetry with and without a controller connected;
4. only after passive validation, merge semantic controller actions into `PCINPUT_GetMappedStates @ 0x0050A190`.


### Modern Input Phase 0 — passive bridge integrated — 2026-09-30

Implementation now includes:
- legacy-safe `input11_legacy_bridge.h`;
- proxy-side dynamic loading/probing of `spidey_input11.dll`;
- ABI v1 validation;
- per-frame passive polling from the completed-frame path;
- no gameplay action injection yet;
- connection/state telemetry in `spidey-decomp-input.log`;
- helper-provider lifecycle log in `spidey-input11.log`;
- standard updater/test workflow builds, installs and collects the helper automatically;
- 32-bit `spidey_input11_probe.exe` preflight runs before the game launches.

Important commits:
- `44cbb396a1951a995d23e134012d6c0b339a1415` — legacy-safe bridge header;
- `0dcc44185aa4dacb23246efe6a8e86b0f11cdbb8` — proxy passive load/poll integration;
- `9c46347e3b5db7f981f59f4015dfc475a0c2ed75` — build/install/log collection;
- `9bb457f7ba3ac53a07991e73a709a84a3cdc3b91` — 32-bit preflight source;
- `67eb23e911e2e864ac8e99e255704a4bf01b4ed8` — build probe target;
- `04f0110c820896974173b26b02ab9b429f27d4b6` — export probe artifact;
- `84d18970a2d01999682a41308763080fc2d6d057` — run preflight before game;
- `af0beffbd61db96eb44e726882a97da80f2ec00e` — isolate preflight working directory.

Expected passive runtime markers:
- updater console:
  `abi=1 expected=1 backend=spidey_input11/xinput-dynamic probe=1`;
- proxy input log:
  `input11_bridge loaded ... abi=1 expected=1 ... passive=1`;
- no controller:
  `input11_state ... connected=0 ... passive=1`;
- connected XInput pad:
  `input11_state ... connected=1 family=1 user=<n> ... move=... camera=... triggers=... passive=1`;
- helper log:
  `backend loaded provider=xinput dll=<chosen xinput dll> abi=1`;
  plus controller connect/disconnect transitions.

Safety:
- this phase cannot alter player movement/buttons/camera because the normalized state is observation-only;
- retail DirectInput remains authoritative until passive runtime proof is received.

Provider roadmap note:
- current XInput provider is intentionally dependency-free and limited to the Phase 0 proof;
- broad PlayStation/generic gamepad support remains provider-pluggable behind the same ABI.


### Retail PC analogue-input RE result — digitalized joystick confirmed

Original retail `Pad_Update @ 0x00505720` and `PCINPUT_GetMappedStates @ 0x0050A190` were disassembled from retained retail function bytes.

Confirmed retail PC behavior:
- `Pad_Update` calls `PCINPUT_GetMappedStates`, updates digital `SButton` fields, and expires vibration;
- it does **not** populate the `SControl` raw/processed analogue movement or aim fields;
- `PCINPUT_GetMappedStates` calls `DXINPUT_PollController(&gControllerX, &gControllerY, &gControllerAxesRelatedTwo)`;
- controller X/Y magnitude is reduced to four digital direction bits:
  - X < -250 -> left bit 0x4;
  - X > +250 -> right bit 0x8;
  - Y < -250 -> up bit 0x1;
  - Y > +250 -> down bit 0x2;
- POV/hat data is likewise reduced to those same directional bits;
- controller button mappings are then OR'd into the action masks.

Conclusion:
- the retail PC controller path is fundamentally **digitalized**, despite the inherited console-era `SControl` analogue fields;
- modern left-stick magnitude and right-stick camera should not be forced through the old PC joystick-direction path;
- the new normalized helper should feed true analogue channels directly into a modern movement/camera layer while optionally producing the legacy action mask for compatibility.

Passive telemetry commit:
- `8de9028d3ba6cba8db164956c813948396a365bb` logs the legacy raw/processed analogue fields beside modern helper state so the next runtime can verify whether they remain zero/unused in actual PC play.

This materially reduces risk for the modern camera plan: right-stick camera is not replacing a hidden working PC right-stick system; it is adding one that the retail PC path does not provide.


### Passive camera ownership telemetry + gameplay-camera RE — 2026-09-30

Implementation:
- `2d5dc86b59e4a738e831fe88e8efdf1dba9937e4` — passive active-camera/mode/transform telemetry;
- `4cd7bf549696c76d8ad7176c6e3682c4ce9f4516` — automatic camera-log collection in the standard test session.

Telemetry is observation-only:
- retail active-camera pointer is read from `0x0056F3B8`, the same pointer used by retail `CPlayer::PutCameraBehind`;
- no camera fields/functions are modified or called by the telemetry path;
- `spidey-decomp-camera.log` records:
  - active camera pointer;
  - camera mode number + enum name;
  - pushed/previous mode;
  - `field_236` heading and `field_23A` transform-derived heading;
  - camera position;
  - focus/tripod target;
  - XZ/Y camera distances;
  - zoom;
  - collision-ray IDs;
  - modern-input connection state and normalized right-stick `cameraX/cameraY`;
- logs on camera-pointer changes, camera-mode changes, and periodically;
- with a modern controller connected periodic sampling increases to every 60 completed frames so right-stick intent can be correlated with unchanged retail camera behavior.

#### Important retail camera naming correction

Static disassembly proves `CAMERAMODE_DEMO == 3` is not merely a demo/cutscene camera in this PC build. It is the baseline mode used by ordinary player gameplay camera presets.

Evidence:
- `CPlayer::SetFallingCamera @ 0x004BF5D0`;
- `CPlayer::SetSwingCamera @ 0x004BF690`;
- `CPlayer::SetFloorCamera @ 0x004BF720`;
- `CPlayer::SetWallCamera @ 0x004BF7A0`;
- `CPlayer::SetCeilingCamera @ 0x004BF820`.

Every one:
1. loads active camera from `0x0056F3B8`;
2. only applies its normal movement-state camera preset when `camera->mCameraMode == 3`;
3. drives existing camera offset/distance setters;
4. writes a player camera-preset/state ID at player + `0x540`:
   - floor = 0;
   - wall = 1;
   - ceiling = 2;
   - swing = 4;
   - falling = 5.

Therefore mode 3 should be treated as the leading **ordinary gameplay ownership** candidate for a future modern-camera controller, despite the legacy enum name `DEMO`.

#### PutCameraBehind — confirmed recenter mechanism

Retail `CPlayer::PutCameraBehind @ 0x004C64A0`:
- reads the same active camera pointer at `0x0056F3B8`;
- in ordinary non-crawl flow computes the player's effective heading and calls `CCamera::SetCamAngle`;
- in crawl/oriented-surface flow derives an angle from the current surface/orientation and calls `SetCamAngle`;
- when the camera is mode 3 it also adjusts Y-distance/angle behavior for certain crawl/surface states.

This is the concrete forced-recenter path the first modern free-look prototype will need to gate/suppress during modern ordinary-gameplay ownership.

#### CCamera::AI / CM_Normal static notes

Original `CCamera::AI @ 0x00417CB0` bytes were independently disassembled:
- updates global angle/distance/offset interpolation;
- updates tripod/focus state;
- dispatches mode-specific camera behavior;
- applies collision/orientation/shake processing;
- ends by loading the result into the engine camera through `CCamera::LoadIntoMikeCamera`.

The dispatch for modes 3..17 uses a compact lookup table immediately after the function body; the retained per-function binary archive does not include those adjacent table bytes. The exact lookup table has **not** been guessed. Runtime mode telemetry plus known explicit mode functions will be used to ground ownership before modifying dispatch.

Current safe architectural conclusion:
- modern ordinary gameplay camera can initially claim mode 3;
- other modes remain retail-owned until runtime telemetry or specific RE proves they are safe to absorb;
- this keeps scripted/boss/special cameras intact while allowing a later full modern camera for normal play.


### Passive action-map + raw mouse intent telemetry — 2026-09-30

Implementation:
- `69829c9ea9639ef9792b2c2a07b2f3f797d11d08` — one-time read-only dump of the retail controller/action descriptor table;
- `e0f1993de6d7db714c5a2a091bae07788232b7bc` — mirror raw relative mouse deltas from the existing DirectInput compatibility wrapper;
- `dfbb8bec595a54d25d496b7cb2190d17397f2393` — static-audit fix: move shared raw-mouse telemetry declarations before first use in the VC6-era translation unit;
- `201fc404605746f2fe5811f692d1c39b7fa6bcc6` — throttle activity-driven camera telemetry so short mouse/right-stick input is captured without logging every frame.

#### Retail action descriptor table

Original controller-menu code/disassembly establishes:
- table base: `0x00568690`;
- count: 11;
- stride: `0x1C`;
- +0x00: action bit/mask;
- +0x04: inline 16-byte retail action label;
- +0x14: keyboard mapping;
- +0x18: controller mapping.

The first four controller-menu rows correspond to movement/direction and are disabled for joystick-button remapping; later rows are button-remappable.

The passive bridge now dumps the table once:
`retail_action_map index=<n> action=0x.... label=<retail text> keyboard=... controller=... passive=1`.

This deliberately uses the game's own runtime labels rather than guessing semantic names for action bits.

#### Raw relative mouse path

Retail machine-code RE:
- `DXINPUT_PollMouse @ 0x00501CC0` consumes buffered DirectInput mouse events and accumulates relative X/Y deltas before any absolute cursor integration;
- `PCINPUT_UpdateMouse @ 0x0050A8A0` later scales/integrates those deltas into the shell cursor and clamps them to mouse bounds.

The project already owns every direct call to `DXINPUT_PollMouse` through `SpideyCompatRetailPollMouse` for Alt+Tab recovery.

New passive behavior:
- call retail PollMouse unchanged;
- on successful polls, mirror/accumulate the returned relative X/Y deltas;
- once per completed frame, camera telemetry snapshots and clears those accumulators;
- `spidey-decomp-camera.log` now includes:
  - `input_mouse=<dx>,<dy>`;
  - `mouse_polls=<count>`;
  - existing `input_camera=<right-stick-x>,<right-stick-y>`.

Activity sampling:
- mode/camera changes still log immediately;
- periodic samples remain;
- non-zero mouse or right-stick intent adds a throttled `event=input_intent` sample at most every 15 frames.

This gives Stage-A camera work one unified evidence stream for mouse and right-stick intent without adding another mouse-capture subsystem.

#### SetCamAngle ownership clue

Original `CCamera::SetCamAngle @ 0x004178E0` bytes explicitly compare `mCameraMode` and skip angle changes when mode is:
- 15 = LOOSE;
- 16 = USER;
- 17 = LOOKAROUND.

Because `CPlayer::PutCameraBehind` recenters through `SetCamAngle`, this suggests a potentially useful Stage-A compatibility mechanism: an appropriate user-controlled camera mode can naturally reject legacy recenter requests.

Do **not** switch modes based on this fact alone. The behavior of the mode-specific AI/dispatch must still be validated at runtime. The final modern camera remains free to replace ordinary legacy camera ownership entirely.

#### Static audit result

Before runtime handoff:
- raw mouse telemetry declarations now occur before their camera use;
- exactly one action-map logger, one passive camera sampler and one raw-mouse state set are present;
- new proxy code contains no C++11-only `auto`, `nullptr`, or lambda syntax;
- telemetry varargs were mechanically checked:
  - camera state: 25 specifiers / 25 data arguments;
  - input11 state: 21 / 21;
  - retail action map: 5 / 5;
- input helper remains C++17 only inside the separate VS2022 Win32 DLL;
- XInput is dynamically loaded; the old proxy does not link against XInput;
- the 32-bit preflight, helper install, helper log capture and camera log capture are all wired into the standard test harness.

No gameplay input or camera ownership is changed by any of these passive additions.


### Combined passive runtime boundary prepared

Static work is intentionally stopping before action injection or camera mutation.

Reason:
- modern input helper/build/preflight is wired;
- retail semantic action table will self-report at runtime;
- legacy analogue fields will be observed beside true modern stick magnitude;
- raw relative mouse and right-stick intent are now correlated with passive camera mode/transform state;
- live Apply and frontend mouse-return fixes are still awaiting runtime confirmation;
- Phase 3E F9 suppression is ready but remains opt-in/default-off.

The retained `CCamera::AI` binary was checked for the mode-dispatch tables referenced at `0x0041866C` and `0x0041868C`. The archived function blob ends exactly at `0x0041866C`; the table data is not present elsewhere found in the repo. No table contents were inferred or fabricated.

Next authoritative step is the combined runtime test documented at the top of `docs/NEW_CHAT_HANDOFF.md`. Do not enable modern controller action injection or camera ownership until the passive logs validate these boundaries.


## Priority shift from user runtime observations — 2026-09-30

User explicitly reprioritized current work ahead of the optional F9 renderer-isolation test.

Current priority order:

1. **Proper widescreen**
   - current implementation visibly stretches a 4:3 presentation to 16:9;
   - this is not acceptable as the final widescreen implementation;
   - target is true widescreen/Hor+ gameplay geometry with correct projection and non-stretched 2D/HUD behavior.

2. **Frontend/menu mouse hit-testing**
   - cursor motion itself is responsive;
   - hover/selection regions do not line up reliably with the visible menu after returning from a level;
   - user often has to place the pointer slightly above the visible option to select/click it;
   - treat this as a coordinate-transform / bounds-sync issue, not a mouse-acquisition issue unless new evidence proves otherwise.

3. **High-frame-rate simulation speed**
   - at higher render frame rates the game feels sped up;
   - renderer isolation/F9 is deprioritized because DX11 alone is not expected to fix simulation speed;
   - must identify the simulation/game-timer cadence and decouple gameplay/physics from render rate;
   - compare two design families:
     a. fixed 30 Hz simulation with interpolation for rendering;
     b. delta-time / fixed-step accumulator conversion with rate-independent gameplay;
   - do not choose until original timer/update semantics are grounded.

4. **Audio output modernization**
   - current game sound is effectively stuck on one headset/output device;
   - default behavior must become Windows **system default** output;
   - add an Audio menu that lists output devices and includes `System Default` as the default selection;
   - manual device selection should persist.

5. **Uncap main-menu framerate**
   - current main menu is 30 FPS;
   - user wants it uncapped;
   - this must not reintroduce sped-up menu animation/input timing, so frontend pacing and simulation timing need to be separated.

Renderer isolation:
- F9 D3D7-main-draw suppression test is postponed until these priorities are addressed or until renderer-isolation evidence becomes directly useful to one of them.

Immediate investigation plan:
- trace widescreen projection/FOV and 2D scaling separately;
- trace shell mouse coordinate conversion/hit-test bounds, especially gameplay -> frontend transition;
- trace frame timer/update loop and identify where 30 Hz assumptions enter simulation;
- trace DirectSound device creation and existing sound options/menu;
- trace frontend 30 FPS limiter and determine whether it shares the same timer path as gameplay.


### Frontend mouse hover/click coordinate bug — root cause fixed — 2026-09-30

User symptom:
- cursor motion itself is responsive;
- after returning from gameplay, visible menu items often do not highlight/click at the visible cursor position;
- moving the cursor slightly **above** the intended option makes selection work.

Static retail/source RE found an exact coordinate-space mismatch:

`PCSHELL_IsMouseOver` converts shell/DC hit rectangles into PC pixels using:
- live DX width `gDxResolutionX @ 0x006B78E4`;
- live DX height `gDxResolutionY @ 0x006B78E8`.

But retail:
- `PCINPUT_IsMouseOver @ 0x0050A820`;
- `PCINPUT_GetMouseHotspotPosition @ 0x0050A770`

scale the cursor hotspot using:
- gameplay logical width `0x00568154`;
- gameplay logical height `0x00568158`.

After gameplay at 2560x1440, the frontend can be back on its proven 640x480 canvas while the gameplay logical dimensions remain 2560x1440. A nominal 15-pixel Y hotspot therefore becomes:

`15 * 1440 / 480 = 45`

instead of 15 pixels.

That moves the effective hit-test point about 30 pixels below the visible cursor, which directly explains why hovering above an item can select it.

Fix commit:
- `4381062061b7a625c036dff7d851a3fac9430818` — `input: unify menu mouse hotspot coordinate space`.

Implementation:
- replaces retail `PCINPUT_IsMouseOver @ 0x0050A820`;
- replaces retail `PCINPUT_GetMouseHotspotPosition @ 0x0050A770`;
- hotspot scaling now uses the same live DX canvas dimensions as `PCSHELL_CoordsDCtoPC`;
- preserves retail strict `>` / `<` hitbox semantics;
- preserves cursor movement/acquisition behavior;
- byte-verifies the expected retail entry bytes before installing either replacement;
- logs:
  `mouse_coordinate_compat mouse_over=1 hotspot=1 ... basis=live_dx_canvas`.

This is intentionally separate from the existing frontend bounds/reacquire fix: bounds control where the cursor may move; this patch fixes where the shell believes the cursor's clickable hotspot actually is.


### Timing / main-menu cap RE — fixed-step evidence — 2026-09-30

Retail timing is now grounded enough to reject a blind global delta-time conversion.

#### Engine vblank clock

`PCTIMER_Init` creates a multimedia timer (nominal 16 ms) and converts real milliseconds into a 60 Hz virtual-vblank clock:
`gTimerMsInterval = timer_ms * 60 / 1000`.

The timer callback advances `gTimerVblankRelated` and calls `MyVSync` until retail `Vblanks @ 0x006B4CA0` catches up.

Retail `Pause @ 0x004E5D60` is confirmed machine-code busy-wait:
- target = `Vblanks + Time`;
- spin until `Vblanks >= target`.

#### Gameplay cadence

`PlayAway @ 0x004559D0` contains the normal gameplay loop.

Per loop:
1. snapshot `Vblanks`;
2. run `Logic @ 0x00455400`;
3. run display/render work;
4. if the current `Vblanks` still equals the snapshot, call `Pause(1)`;
5. begin the next gameplay loop.

Therefore normal retail gameplay is explicitly constrained to **at most one simulation/update pass per 60 Hz engine vblank**.

`Logic` also increments multiple gameplay/frame counters once per call, reinforcing that large parts of the game are authored as fixed-step/per-update code.

Current implication:
- do not retrofit global delta-time multipliers through gameplay/physics as the first modernization;
- first measure whether modern runtime is actually executing Logic faster than the intended 60 Hz cadence;
- preferred architecture is fixed-step simulation at the retail-authored cadence plus independent rendering/interpolation, rather than changing thousands of fixed-point/per-frame constants.

#### Main menu 30 FPS cap — exact mechanism

`Shell_MainMenu @ 0x00493990` has a distinct pacing sequence near `0x00494150`:

- compare current `Vblanks` to the loop's saved starting vblank;
- if no vblank elapsed during update/render, call `Pause(1)`;
- then unconditionally call a second `Pause(1)`.

On the 60 Hz engine clock this deliberately produces approximately **30 menu loops/frames per second**.

This confirms the user's observed 30 FPS menu cap.

Important:
- simply NOPing both waits would make the shell loop run as fast as possible and can accelerate loop-count-based menu animation;
- the correct uncapping design should separate shell simulation/update cadence from render/presentation cadence, just as gameplay modernization should separate fixed simulation from rendering.

Current timing direction:
- gameplay: preserve 60 Hz fixed simulation unless runtime telemetry proves another intended cadence;
- shell/menu: preserve 30 Hz logical update semantics initially;
- render/present: allow independent high/uncapped cadence;
- interpolate visual state between fixed logical updates where useful/feasible.

This approach is safer than global delta-time conversion for this 2000 fixed-point engine and directly addresses the user's request to avoid physics/gameplay speed changes at high FPS.


## Recovery after input-stream interruption — 2026-10-01

The user supplied the interrupted-session transcript and asked whether any work was lost.

Recovery audit:
- live `dev` was inspected directly;
- the last pre-audio timing/documentation checkpoint was:
  - `e497ed897cfb6aeca55a08d5a5b39a4ad0e973b7` — `docs: ground gameplay and shell fixed-step timing`;
- live `dev` is three implementation commits ahead of that checkpoint:
  - `ea594c7ce6e8349a46ca29b4d4e5d834b5eb6b24` — `audio: add persisted output-device selection backend`;
  - `4aa5d59a4000c03ab5a9c3c80ae775058dfe4379` — `audio: collect device-selection log in test sessions`;
  - `7826aec4b1fb6adb2d2afcf14284ed807b4ae188` — `audio: add output-device row with safe shell restart`.

### Audio work that definitely survived

Backend:
- dynamically resolves `DirectSoundCreate8` and `DirectSoundEnumerateA`;
- enumerates playback devices;
- creates an authored row 0 named `(System Default)`;
- persists manual device selection by DirectSound GUID in `spidey-modern-audio.ini`;
- default/fallback policy is row 0 / system default;
- hooks the retail DirectSoundCreate8 import thunk at `0x00517A70`;
- logs enumeration/selection/create results to `spidey-decomp-audio.log`.

Audio menu:
- retail `Shell_SFXMusic @ 0x004977D0` is extended from five rows to six;
- row 5 is `Output: <device name>`;
- uses the retail left/right trigger masks for selection;
- refreshes devices when the Audio menu opens;
- switching output is shell-only and performs a controlled DirectSound shutdown/recreate;
- after recreation it runs retail `DXSOUND_Init @ 0x005039F0` and re-spools the `menu` SFX bank;
- if a manually selected device fails to initialize, it falls back to `(System Default)`;
- the resulting selection is persisted;
- install marker:
  `audio_menu_mod retail=0x004977D0 rows=6 output_row=5 ... live_restart=shell_only`.

### Other work confirmed safe

Mouse:
- `4381062061b7a625c036dff7d851a3fac9430818` survives on `dev`;
- frontend cursor hotspot/hit-test scaling now uses the same live DX canvas as shell hit rectangles.

Timing:
- retail gameplay is grounded as a maximum 60 Hz fixed-step/update loop;
- retail main menu's ~30 FPS cap is grounded as two one-vblank waits per normal loop;
- no global delta-time conversion was committed;
- no menu-FPS uncapping patch was committed yet.

Widescreen:
- substantial static RE survived in the interrupted transcript;
- `M3d_RenderSetup @ 0x00472DC0` was identified as the upstream 3D projection path;
- the aspect scalar `0x00550064` is consumed inside that projection math;
- the engine's separate PSX-style pixel-aspect state was also identified;
- no final Hor+ implementation was committed before the interruption.

### What was actually lost

No substantive implementation commit was lost.

The only missing durable state from the interrupted tail was:
- final written documentation summarizing the new audio implementation;
- any analysis performed after commit `7826aec4...` but before the stream terminated.

That analysis was short and can be reconstructed from the supplied transcript plus the surviving source. The authoritative implementation frontier is the live `dev` branch, not the interrupted chat text.


## Widescreen recovery RE — projection patch vs render-domain separation — 2026-10-01

Recovery package and live branch were re-verified before further engineering:
- handoff SHA-256: `8bee9904a6ea22854f87c0301425c2b5ba0d771bccc319f7d73e5c0f9b0432a6`;
- ZIP integrity: PASS;
- live `dev` HEAD still exactly `ce20a6eb1caf00df0816551f54a5c88a6fcd0c95`;
- no branch/package divergence exists.

### Established aspect scalar is valid

The current aspect values are not speculative.

The public `r57zone/Spider-Man-Settings` utility writes the aspect-ratio float at executable file offset `0x150064`, which maps to retail VA `0x00550064`, and uses the same values already present in this project:
- 4:3 = 1.0
- 5:4 = 1.06667
- 16:9 = 0.75
- 16:10 = 0.83333
- 21:9 = 0.57143
- 32:9 = 0.375

That independent implementation credits the original address discovery and documents widescreen support, while also documenting two limitations relevant to our current work:
- HUD remains stretched;
- edge polygons can appear late because culling/optimization still assumes the old view.

Therefore do **not** replace `0x00550064` with a guessed FOV constant. The remaining work is to separate world projection, source-screen coordinates, 2D/HUD layout, frontend layout, and culling.

### Runtime evidence proves the render domains are already decoupled

Latest 2560x1440/16:9 evidence shows:
- selected visible output = `2560x1440`;
- aspect scalar = `0.750000`;
- gameplay logical dimensions at `0x00568154/58` = `2560x1440`;
- compatibility D3D7 backing can remain `1920x1440`;
- early pixel-present path preserves that 4:3 backing as `rect=320,0,1920x1440` on the 2560x1440 DX11 target;
- once geometry replay becomes active, the DX11 shadow target is `2560x1440`;
- captured transformed vertices span `x=0..2560`, `y=0..1440`.

The captured D3D7 viewport state can still report `640x480` while the transformed XYZRHW stream spans the 2560x1440 logical domain. For shadow preview the proxy currently compensates by overriding the replay viewport to the modern logical dimensions.

Implication:
- D3D7 `SetViewport` state alone is **not** a trustworthy normalization basis after compatibility remapping;
- transformed XYZRHW coordinates, the logical projection canvas, the hidden backing surface, and the visible DX11 target must be modeled as separate domains;
- a blanket “modern canvas” policy for both 3D and 2D is the wrong final architecture.

### M3d_RenderSetup confirms the upstream split

Retail `M3d_RenderSetup @ 0x00472DC0`:
- consumes `PixelAspectX @ 0x00654F58` and `PixelAspectY @ 0x00654F5C`;
- computes viewport center/scale fields from the supplied `SViewport`;
- consumes logical output width/height at `0x00568154/58`;
- consumes aspect scalar `0x00550064` in the projection coefficient divided by viewport Zoom.

This confirms that `0x00550064` is an upstream 3D projection control, while logical width/height also participate in screen-space conversion.

### Current engineering direction

Priority 1 remains true Hor+ widescreen, but the implementation should now be split deliberately:

1. retain the proven aspect scalar for 3D projection;
2. give geometry replay an explicit logical/source-screen normalization basis instead of inheriting arbitrary retail D3D7 viewport state;
3. classify/separate 2D/HUD/frontend layout from 3D world projection before applying any widescreen transform;
4. preserve the original frontend coordinate semantics until its modern layout pass is handled explicitly;
5. trace the old-view culling/frustum boundary separately so widened projection does not reveal late-appearing edge geometry.

Do not request a runtime test yet. Continue static classification and implementation until the 3D-vs-2D boundary is explicit enough for a meaningful widescreen build.


### Rejected 2D classifier — depth/RHW relation is shared by projected 3D — 2026-10-01

A tempting classifier was investigated from the decompiled `PCGfx_DrawQuad2D` path.

That routine constructs transformed vertices with:
- `z = (depth - 10) / 8038`
- `rhw = 276 / depth`

Several obvious frontend/HUD samples in the captured D3D7 stream satisfy that relationship closely.

However, cross-checking the broader captured stream found ordinary world-geometry samples that satisfy the same relationship as well. This is therefore a generic transformed-projection characteristic in this renderer, **not** a unique marker for 2D.

Decision:
- do **not** classify HUD/frontend draws from `z`/`rhw`, depth bands, screen bounds, or similar numeric heuristics;
- do **not** bake a guessed draw-stream classifier into the DX11 bridge;
- derive the 2D/3D split from higher-level call provenance or an exact retail 2D submission hook instead.

This dead end is recorded so a later recovery does not rediscover and accidentally ship the same unsafe heuristic.


## Stream-recovery tail reconstruction — 2026-10-01

A second stream-recovery audit was performed after the user supplied the visible interrupted transcript.

Live branch state at recovery:
- `dev` HEAD = `d86a2e63ad20c086b79e1960a534d2828979a771`
- parent = `4e64a99039ce08403ab645c4823b3d07f772eb1b`
- no implementation commit exists after `d86a2e63`; therefore no committed source change was lost.

The only missing state was uncommitted static analysis performed after `d86a2e63`. It has now been reconstructed below.

### Exact higher-level 2D entrypoints

The repo's authoritative `tools/names.json` identifies:
- `PCGfx_DrawQuad2D @ 0x00507470`
- `PCGfx_DrawQPoly2D @ 0x00507910`
- `PCGfx_DrawQPoly3D @ 0x00508550`
- `DXPOLY_DrawPoly @ 0x00503100`

The retail dump for `PCGfx_DrawQuad2D` is 1181 bytes and its structure matches the decompiled source signature/body. This provides a grounded higher-level 2D provenance point and is preferable to classifying flattened D3D7 draws by screen-space heuristics.

Do not install the 2D safe-area hook yet merely because this entrypoint is known. The user's more important symptom is camera-dependent level-background distortion, which must be separated from HUD/frontend layout first.

### Background-system check

`CBackground` in `backgrnd.cpp` is a `CBody`/model-backed object and not simply a fullscreen 2D backdrop. Therefore the reported moving-background distortion can plausibly be part of the transformed 3D path and should not automatically be grouped with HUD/menu stretching.

### 3D texture-coordinate preparation — high-priority hypothesis

Retail `PCGfx_DrawQPoly3D @ 0x00508550` was disassembled from the repo dump.

For each transformed vertex it:
- computes RHW into vertex offset `+0x0C` from `0x00568184 / depth`;
- computes normalized Z into `+0x08`;
- tests global `0x006B78F8`;
- when that global is nonzero, multiplies texture U and V at offsets `+0x14/+0x18` by the computed RHW.

This occurs for all four quad vertices at retail addresses around:
- `0x0050889F`
- `0x005088F9`
- `0x00508953`
- `0x005089B0`

The current DX11 replay shader also performs:
- `uvOverW = input.uv * rhw`
- then interpolates and resolves `uv = uvOverW / rhw` in the pixel shader.

This creates a **possible double perspective-preparation path** if the captured retail U/V values are already RHW-weighted when `0x006B78F8` is active. Such a mismatch would be consistent with camera-dependent texture/background warping.

Important: this is a strong hypothesis, **not yet a committed fix**. Before changing the shader, determine the exact semantics/runtime value of `0x006B78F8` and confirm whether the D3D7 draw stream presented to the hook contains already-weighted or raw U/V.

### DXPOLY_Init clue

Retail `DXPOLY_Init @ 0x00502220` also reads `0x006B78F8`:
- on entry it conditionally calls `0x00515270` when the global is nonzero;
- later it uses the same global to choose Direct3D render-state value 1 vs 3 for state ID `0x16`.

The same initializer explicitly sets render-state ID `0x04` to 1.

Do not name `0x006B78F8` or infer its purpose until the old Direct3D render-state IDs and call semantics are fully resolved. The immediate next RE step is to map those state IDs and follow all reads/writes to `0x006B78F8`.

### Recovery conclusion

Nothing substantive needs to be redone:
- both documentation checkpoints survived;
- no source implementation after them was lost;
- the interrupted analysis frontier is reconstructed here;
- next work should continue from the `PCGfx_DrawQPoly3D` / DXPOLY perspective-coordinate investigation before touching the 2D safe-area hook or requesting another runtime test.


## User-validated five-item priority refresh — 2026-09-30

Latest runtime correction from the user:
- The previously reported level/background warping or distortion while moving is **FIXED**. Treat it as closed unless it regresses.
- Stop pursuing the interrupted UV/RHW distortion hypothesis as an active bug. Keep the RE notes only as historical evidence.

Active priorities, in order:

1. **Proper widescreen / Hor+**
   - Current widescreen still behaves as stretched 4:3 rather than true widescreen.
   - Preserve vertical FOV and expand horizontal view.
   - Keep 3D projection, culling, HUD/2D, and frontend/menu layout as separate domains.

2. **Menu mouse hover/click alignment and responsiveness**
   - Cursor movement is fine.
   - Hover/selection does not reliably line up with the cursor, especially after leaving a level; the user sometimes must hover slightly above the intended item.
   - Existing mouse-coordinate fix is not considered user-validated yet.

3. **Frame-rate-independent game speed**
   - Higher render FPS makes gameplay feel sped up.
   - Choose and implement the correct architecture between original-cadence fixed-step + interpolation and broader delta-time conversion.
   - Current RE evidence still favors fixed-step simulation with independent rendering/interpolation rather than a risky global delta-time rewrite.

4. **Modern audio output selection**
   - Default output must follow the OS/system default device rather than behaving as though pinned to the headset.
   - Audio menu must allow manual device selection, with `(System Default)` as the default setting.
   - Existing backend/menu implementation must be validated and completed as needed.

5. **Uncapped main-menu rendering**
   - Main menu is currently ~30 FPS.
   - Uncap menu rendering/presentation without uncapping menu logic or otherwise tying shell/game simulation speed to render FPS.
   - This must be designed together with priority 3 so removing the menu cap does not accelerate shell logic.

Immediate work resumes at priority 1. Do not ask for a new runtime test until a meaningful widescreen implementation batch is ready.


### Hor+ RE: projection scalar and object culling use different FOV inputs — 2026-09-30

Retail control flow is now grounded end-to-end:

- `M3d_RenderSetup @ 0x00472DC0` constructs fixed-point clip/culling vectors in `0x0065CEB8..0x0065CF06`.
- It rotates three vectors from the second set through the live camera rotation and stores the resulting 3x3 basis at `0x00628620`.
- `M3d_Render @ 0x004739A0` immediately copies that 3x3 basis into `0x00610B60` via `sub_46D810`.
- The same render entry writes the camera position to `0x00610BF0/F4/F8` via `sub_46E250`.
- It then calls `M3dAsm_BoundingSpherePreprocessing @ 0x0046FAD0`, which consumes that basis/position and marks objects outside the camera-space planes as non-rendered.

Crucial ordering/result:
- the culling-vector construction occurs around `0x00472FC8..0x004731C6`;
- the selected widescreen scalar `0x00550064` is not read until later at `0x00473504`, during the floating-point projection-matrix path;
- therefore the current aspect scalar can change projected horizontal FOV without automatically widening the object-culling side planes.

This is a real architectural mismatch for Hor+: projection and visibility currently derive horizontal FOV from different inputs. The final widescreen implementation must keep the side culling planes synchronized with the selected aspect while preserving the vertical planes/FOV.

Additional exact helper semantics recovered:
- `sub_46D810(source)` copies an 18-byte / 3x3 signed-short matrix to `0x00610B60`.
- `sub_46E250(x,y,z)` writes the three camera-position globals at `0x00610BF0/F4/F8`.
- `M3d_Render` calls both immediately before `M3dAsm_BoundingSpherePreprocessing`.

Do not patch the culler with arbitrary multipliers yet. Next derive which of the three source normals in `0x0065CED0..` are horizontal vs vertical and how their slope is calculated from the viewport/Zoom terms, then apply the selected aspect at the vector-construction stage so projection and culling remain mathematically matched.


## Hor+ implementation checkpoint before interruption — 2026-09-30

### Source implementation now committed

Commit:
- `e59539d4a501cc1269cf0b23988c15ede041bd80` — `widescreen: synchronize Hor+ side-plane culling`

Implementation in `main.cpp`:
- patches the single retail call at `M3d_Render + 0x2B`, call site `0x004739CB -> sub_46D810 @ 0x0046D810`;
- wrapper `SpideyCompatLoadCullBasis` leaves the first row of the second 3x3 frustum matrix unchanged;
- treats rows 1/2 as the mirrored horizontal side-plane pair;
- decomposes the pair into common forward component + opposing right component;
- scales only the horizontal/right component by the same selected aspect scalar at `0x00550064`;
- renormalizes both side planes back to the engine's 4096-length fixed-point convention;
- passes the adjusted 3x3 matrix to the original retail `sub_46D810`;
- adds one-shot `horplus_cull` and install telemetry in `spidey-decomp-compat.log`.

No vertical plane, near/far depth, physics, UI, or timing code is changed by this patch.

### Frustum/culling mapping is now grounded

Retail `M3d_RenderSetup @ 0x00472DC0` constructs six normalized fixed-point frustum normals as two 3x3 matrices.

Grounded data flow:
- first 3x3 matrix starts at `0x0065CEB8`;
- second 3x3 matrix starts at `0x0065CED0`;
- the second matrix is rotated into camera/world space and stored at `0x00628620`;
- `M3d_Render` copies that basis into `0x00610B60` via `sub_46D810`;
- `sub_46E250` writes camera position into `0x00610BF0/F4/F8`;
- `M3dAsm_BoundingSpherePreprocessing @ 0x0046FAD0` consumes the basis + camera position and marks objects outside those planes non-rendered.

Algebraic inspection of the setup code shows:
- row 0 of the second 3x3 matrix is the opposing vertical plane;
- rows 1 and 2 are the mirrored left/right side planes;
- their shared component is camera-forward;
- their opposing component is camera-right;
- widening horizontal FOV therefore requires reducing the right-component magnitude while preserving the forward component, then renormalizing.

This matches the selected aspect scalar convention:
- 4:3 = 1.0;
- 16:9 = 0.75;
- wider formats use progressively smaller scalars.

Important architectural result:
- retail projection reads `0x00550064` later in `M3d_RenderSetup`;
- retail object-culling planes are constructed earlier and do not automatically read that scalar;
- without the new wrapper, projected Hor+ and object visibility can disagree at the widened left/right edges.

### Exact 2D/3D submission provenance now identified

Retail function boundaries:
- `PCGfx_DrawQuad2D @ 0x00507470`;
- `PCGfx_DrawQPoly2D @ 0x00507910`;
- `PCGfx_DrawQPoly3D @ 0x00508550`;
- `DXPOLY_DrawPoly @ 0x00503100`.

The two exact 2D functions both end in direct calls to `DXPOLY_DrawPoly`:
- `0x005078F0 -> 0x00503100`;
- `0x00507D83 -> 0x00503100`.

This gives a reliable higher-level 2D provenance boundary. Do not use z/RHW, depth, bounds, or other vertex heuristics to identify HUD/frontend draws.

### Unfinished next step

The current DX11 replay still applies one modern viewport override to all captured main-scene draws. The next implementation task is to preserve exact 2D-vs-3D provenance through the retail queued/sorted polygon path so:
- world geometry uses the Hor+ modern viewport/projection;
- HUD/frontend 2D retains its own stable layout/safe-area transform instead of being stretched with world geometry.

Work stopped while tracing how `DXPOLY_DrawPoly @ 0x00503100` stores/sorts queued polygons and how to attach provenance without modifying retail vertex semantics.

Do not resume the old UV/RHW distortion investigation unless the previously fixed visual distortion regresses.


## Combined widescreen/timing implementation checkpoint — 2026-09-30

Source work added after the prior Hor+ culling checkpoint:

- `a3421ff146f6144cb608e9fbc3792eff0373aaf1` — exact 2D polygon provenance sidecar in `main.cpp`.
- `fe6f93a75feb2bbce622dbc7c1078089cb97b3f1` — renderer legacy bridge carries `drawClass`.
- `63f7e9b8056d336b802f8f15dcbd978da1fbc915` — renderer11 ABI bumped 6 -> 7 for draw provenance.
- `7a075095130225954338804eedeb354d3c2097d9` — gameplay Logic and completed-frame present-rate telemetry.
- `c03238788d38b3ff7f7fd2a77428bde1894fc744` — test launcher captures `spidey-decomp-timing.log`.
- `f1a0899fa3e9dfc72753c000c8ced13e83a1249c` — separate 2D and 3D transformed-vertex coordinate ranges in draw telemetry.

### Exact 2D provenance implementation

Retail-proven direct calls:
- `PCGfx_DrawQuad2D: 0x005078F0 -> DXPOLY_DrawPoly 0x00503100`
- `PCGfx_DrawQPoly2D: 0x00507D83 -> DXPOLY_DrawPoly 0x00503100`

Both calls are patched to a wrapper that tags the originating `DXPOLY*` in a DLL-owned fixed-size sidecar table.

Why the sidecar is required:
- `DXPOLY_DrawPoly` queues sorted primitives when sort slot >= 0;
- `DXPOLY_EndScene` later walks those queued `DXPOLY*` objects and submits their vertices with `vertices = poly + 0x10`;
- therefore the final D3D7/DX11 DrawPrimitive hook recovers the exact queued polygon as `vertices - 0x10` and can identify whether it originated from the exact 2D path without modifying any retail structure or using heuristics.

The resulting renderer shadow state carries:
- `drawClass = 1` for exact 2D provenance;
- `drawClass = 0` otherwise.

Draw telemetry now records:
- `class_2d`
- `class_3d`
- `tagged_2d`
- separate `class2d_x/y` and `class3d_x/y` coordinate ranges.

This is the evidence needed to apply the next HUD/frontend safe-area transform against the *actual transformed coordinate basis* rather than the unreliable legacy D3D7 viewport value.

### Timing telemetry implementation

Retail gameplay call site:
- `PlayAway 0x004559D0`
- direct gameplay update call `0x00455A8B -> Logic 0x00455400`

That exact call is wrapped for observation only. It does not change simulation cadence.

New `spidey-decomp-timing.log` records approximately once per second:
- `timing_logic ... hz=<actual Logic calls/sec>`
- `timing_present ... hz=<completed frames/sec>`
- frontend state
- engine `Vblanks @ 0x006B4CA0`
- modern logical and legacy physical dimensions.

This will distinguish:
- true >60 Hz gameplay simulation (actual speed-up source), from
- 60 Hz fixed simulation with higher/different presentation cadence.

### Frontend coordinate grounding

Reconstructed `PCSHELL_CoordsDCtoPC` is matching and confirms authored shell coordinates are Dreamcast-style:
- X domain: 0..512
- Y domain: 0..240

Retail converts them as:
- `x = x / 512 * gDxResolutionX`
- `y = y / 240 * gDxResolutionY`

Therefore the main menu can remain on the stable retail 640x480 compatibility canvas while DX11 remaps it into a modern high-resolution layout. It is not necessary to force retail frontend internals themselves to run natively at 2560x1440.

### Next runtime pass

A runtime pass is now higher-value than more static guessing because it will provide, in one session:
1. exact 2D vs 3D transformed coordinate ranges;
2. whether the Hor+ culling-plane adjustment behaves correctly;
3. actual gameplay Logic Hz vs present Hz;
4. whether the already-committed mouse hotspot fix resolves post-level hover/click alignment;
5. whether the already-committed System Default/manual audio output work behaves correctly.

Do not use F9. F10 reference switching is unnecessary for this pass.


## Audio-menu crash/layout regression — runtime-grounded fix — 2026-10-01

User runtime report from revision `4431517a65548f0dcf085850a4e1ebdf865effe9`:
- sixth Audio-menu output selector was partially/off-screen;
- after exercising output selection, changing Stereo -> Mono -> Stereo appeared to crash.

Uploaded runtime evidence:
- device enumeration and every manual device-create/restart logged success;
- the crash itself is `0xC0000005` inside `binkw32_.DLL + 0x849A`, reading address `0x00000099`;
- the session did not reach gameplay timing; timing data in this run is frontend-only.

### Root cause found for unsafe live device switching

The existing implementation changed output devices by:
1. retail DirectSound shutdown;
2. creating a new selected `IDirectSound8`;
3. writing the new pointer to `G_PDS @ 0x006B7920`;
4. running `DXSOUND_Init` and respooling the menu SFX bank.

Static source RE of `PCMovie.cpp` proves Bink audio is initialized differently:
- `PCMOVIE_Init` calls `BinkSetSoundSystem(BinkOpenDirectSound, G_PDS)`;
- then sets `G_PC_MOVIE_INITED @ 0x00AC0BA0`;
- future `PCMOVIE_Init` calls do not rebind Bink while that flag is set.

Therefore replacing `G_PDS` live can leave Bink holding the old DirectSound object. A later Bink/audio operation can then touch stale state. This matches the observed fault module much better than treating the stereo/mono retail handler itself as the primary cause.

### Fix commit

`6bb4d8a44542ca27b491a9688e8a56cd8027ff90` — `audio: defer device apply and stabilize six-row menu`

Behavior now:
- Audio Output left/right changes the persisted selected device immediately;
- it updates the visible label immediately;
- it **does not** tear down/recreate DirectSound while the Audio menu/Bink subsystem is live;
- selected device is applied on the next normal audio initialization / next game launch through the already-installed `DirectSoundCreate8` selection hook.

This intentionally prioritizes Bink safety over unsafe live device swapping until a fully grounded Bink rebind/reopen sequence is implemented.

### Six-row layout fix

Retail Audio menu constructor is:
- x = 270
- y = 90
- justification = 2
- hi/low scale = 256
- line separation = 20

Retail authored five rows. The added sixth row therefore extended one line lower.

The add-entry wrapper now:
- appends the Output row;
- shifts the full menu upward by one line separation (`mY -= mLineSep`) once six rows exist;
- compacts long device names to a 20-character middle-ellipsis form;
- strips common `Speakers (...)` / `Virtual Speakers (...)` wrappers before compacting.

This keeps useful distinguishing suffixes such as Game/Chat while preventing long Windows endpoint names from forcing the row out of the authored menu footprint.

### Stereo/mono instrumentation

Retail Stereo/Mono path is now grounded:
- `Shell_SFXMusic @ 0x004977D0`
- stereo case call `0x00497DD5 -> DCSetBootROMSoundMode @ 0x00472AA0`
- `DCSetBootROMSoundMode` ultimately calls `syCfgSetSoundMode`.

The exact call is wrapped observation-only and logs before/after:
- requested stereo state;
- retail boot sound-mode byte `0x0061919D`;
- runtime `G_PDS`;
- Bink initialized flag `0x00AC0BA0`;
- active Bink handle `0x00AC0BA4`.

If Stereo/Mono still crashes without any live DirectSound replacement, the next log will isolate that as a separate retail/Bink interaction.

### Timing note from this crashed run

The timing log contains only `frontend=1` samples before the crash. It confirms the menu commonly runs near 30 Hz, with a brief ~86-88 Hz transitional period after the display/frontend reconfiguration, but provides no `timing_logic` gameplay data. Do not draw conclusions about gameplay simulation speed from this run.


## Frontend/settings architecture correction + live audio + display modes — 2026-10-01

### User runtime confirmations / screenshot evidence

User confirmed:
- **post-level mouse location / menu hover alignment is FIXED**. Preserve that behavior.
- Audio Output must apply without restarting the game.
- frontend/settings must stop behaving as a 640x480 layout underneath the selected modern output;
- Display settings must expose Fullscreen Exclusive, Borderless, and Windowed;
- Audio/settings controls were visibly offset.

Screenshot of the Audio screen made the offset cause concrete:
- menu text had been shifted upward by the previous six-row workaround;
- the three sliders/arrows stayed at their original hard-coded retail Y positions;
- Output still extended below the intended composition.

This proved the previous text-only `menu->mY -= menu->mLineSep` workaround was invalid by itself.

### Frontend logical-canvas correction

Commit:
- `685b00ff9a95c034612951e96b88863a5c4b2a3d` — `frontend: unify modern canvas and remove audio row offset`

Changes:
- removed the text-only Audio row shift;
- frontend mouse bounds now prefer the modern logical canvas;
- mouse canvas helpers use `gSpideyModernLogicalWidth/Height` while the DX11 visible path is active;
- the retail shell's legacy 640x480x16 display request is no longer allowed to become the visible/logical frontend canvas when a modern mode is selected;
- the selected resolution remains authoritative for frontend logical rendering;
- only the hidden D3D7 compatibility producer may be remapped (currently 2560x1440 -> 1920x1440 because retail D3D7 CreateDevice is runtime-proven to reject 2560x1440).

The hidden compatibility producer is temporary legacy plumbing. It must not define frontend text, slider, hit-test, or mouse coordinates.

### Live-safe audio output switching

Commit:
- `117b6b0fa6b03084943d1c827fd1dfd00bdf8b22` — `audio: apply output device live without invalidating Bink`

Behavior:
- selecting a new endpoint again applies immediately in-session;
- before retail DirectSound shutdown, if Bink has already been initialized, the current DirectSound object receives an extra retained reference;
- retail may then release its game-owned reference and rebuild the game's DirectSound/SFX path on the newly selected endpoint;
- Bink cannot be left with a dangling backend pointer;
- when no Bink movie handle is active, `G_PC_MOVIE_INITED @ 0x00AC0BA0` is cleared, retail `PCMOVIE_Init @ 0x0050B0F0` rebinds Bink to current `G_PDS`, then the retained old DirectSound reference is released;
- a per-frame safe point performs that delayed rebind automatically if a movie was active during the endpoint change.

This satisfies the requirement that an audio endpoint change not require restarting the game while preserving the root-cause fix for the prior `binkw32_.DLL` stale-DirectSound crash.

### Audio screen full-layout alignment

Commit:
- `b0eb4f66df689fa75001e418766b9d12b6618de7` — `audio: align six-row text sliders and hit regions`

Retail grounding:
- Audio `CMenu` line separation = 20;
- slider drawing calls:
  - `0x00497978 -> DrawSlider @ 0x00498060`
  - `0x00497998 -> DrawSlider`
  - `0x004979B8 -> DrawSlider`
- slider mouse logic:
  - `0x00497BE9 -> sub_497F80 @ 0x00497F80`
- disassembly proves `sub_497F80(x,y,value)` builds its hover rectangle directly from the supplied Y.

The six-row layout now moves consistently by one authored row:
- `CMenu` text Y: -20;
- all three slider graphics/arrows Y: -20;
- slider mouse hit-region Y: -20.

Therefore the screenshot's text-vs-slider mismatch is addressed at all three matching coordinate paths, not by another visual-only nudge.

### Display-mode support

Commits:
- `9f0107bdf8ef0d398d6fc3bbb5ea9b47f0af75f7` — renderer API exposes fullscreen-state control;
- `106cd597b04c50d4a62450d231f491ea74578033` — DX11 implementation of `SetFullscreenState`;
- `b8853c19d5ccf66d09f2352905719a4c110c98c4` — persistent Display Mode state/menu;
- `c1c6c3a630677225dd13468a62fe0b8717f3eef7` — renderer ABI 8 for window-mode control;
- `e7d9500b270fba9218ce5ded408f52dc3fcfbb8f` — main bridge loads/applies DX11 window mode;
- `cb0d64ad3754bccec48817b9135b0b7d29bbdce8` — corrected exclusive transition ordering;
- `5c3b1340f3a1ce436fd4068f8680bd67cdc46234` — stable transition/menu sizing pass.

Display menu is now five rows:
1. Resolution
2. Aspect Ratio
3. Brightness
4. Display Mode
5. Apply

Display Mode values:
- Fullscreen Exclusive
- Borderless
- Windowed

Persistence:
- `[Video] WindowMode` in `spidey-modern-video.ini`;
- default = Borderless to preserve current behavior.

Retail `PCSHELL_DoDisplayOptions @ 0x0050D9B0` switch logic was disassembled:
- retail only implements row-specific left/right behavior for rows 0, 1, and 2;
- row 3+ falls through without hidden setting mutation;
- therefore row 3 is a safe custom Display Mode row and Apply can move to row 4.

DX11:
- real Fullscreen Exclusive uses `IDXGISwapChain::SetFullscreenState(TRUE)` plus `ResizeTarget` for the selected mode;
- Borderless/Windowed exit DXGI exclusive and use corresponding Win32 styles;
- renderer ABI is now 8 and the new export is mandatory;
- release path explicitly leaves exclusive mode before releasing the swap chain.

### Current pre-test audit status

Static checks currently pass for:
- main expected renderer ABI = 8;
- renderer header ABI = 8;
- new fullscreen export declared, implemented, loaded, and required;
- Display menu logs rows=5 / row3 Display Mode / row4 Apply;
- Apply handler requires line 4;
- all four Audio layout hooks are present;
- live audio-retained-Bink path is enabled;
- old intentional `keep frontend 640x480 canvas` behavior is no longer present.

Before requesting runtime testing:
1. harden transition ordering so leaving exclusive happens before retail graphics-producer rebuild;
2. harden live-audio failure fallback;
3. run one more source/log-format sanity audit.

Do not regress the confirmed post-level mouse fix.


### Final frontend/settings batch before runtime test — 2026-10-01

Additional commits after the architecture checkpoint:
- `e04f9b9e2d677152ebcb0b8448c4c4afde9a3a59` — live-audio rollback safety + pre-retail exclusive-release hardening;
- `c296af57c6af30c64f2db3f0941b0c68f485ea24` — persisted WindowMode is loaded before early DirectX bootstrap; old forced-borderless startup call removed;
- `85e45c41841093d4e489eb5dda12d4b107cb8ee9` — separately drawn Stereo/Mono value is shifted with the six-row Audio layout.

Audio screenshot follow-up:
- retail Stereo/Mono value is a standalone `Mess_DrawText` call at `0x00497A1B -> Mess_DrawText @ 0x00458700`;
- original Y is 149, matching the original Stereo menu row around Y=150;
- it is now wrapped and shifted by the same 20 units as the CMenu rows, all three slider graphics, and slider mouse hit regions.

Audio layout therefore has one consistent authored shift:
- CMenu rows: -20;
- three DrawSlider calls: -20;
- slider mouse logic: -20;
- standalone Stereo/Mono value: -20.

Live audio failure safety:
- a temporary AddRef keeps the previous working DirectSound object available across retail shutdown;
- if selected endpoint creation and System Default fallback both fail, the prior DirectSound object and prior selected-device index are restored;
- Bink stale-pointer retention/rebind remains independent and is still released only at a safe no-active-Bink point.

Display/window startup:
- `SpideyRestoreSavedRenderResolution` now explicitly loads `AspectMode` and `WindowMode` before early renderer startup;
- the previous unconditional `SpideyKeepBorderlessMonitorWindow(hwnd)` early-start call is gone;
- early startup applies the persisted selected WindowMode instead.

Window-mode transitions:
- leaving Fullscreen Exclusive calls DXGI `SetFullscreenState(FALSE)` before retail rebuilds the hidden graphics producer / before normal Win32 styles are applied;
- entering Fullscreen Exclusive establishes the popup window first, then DXGI takes exclusive ownership and applies the selected target mode;
- DX11 release always exits exclusive before swap-chain destruction.

Static final audit:
- current `main.cpp` brace count balanced;
- renderer main/header ABI both 8;
- `SpideyRenderer11_SetFullscreenState` declared, implemented, resolved, and required;
- Display menu has 5 rows and Apply checks row 4;
- WindowMode has all three labels and INI persistence;
- Audio has exact hooks for all three slider draws, slider hit logic, and Stereo/Mono value;
- no startup call remains that unconditionally forces Borderless;
- confirmed-good post-level mouse fix remains installed.

The next runtime test should validate this batch before resuming the still-open FPS priorities:
1. Audio text/sliders/stereo/output vertical alignment;
2. Output Device switches live without game restart and without Bink/stereo crash;
3. Stereo -> Mono -> Stereo stability;
4. frontend/settings are rendered and hit-tested on the selected logical resolution rather than a 640x480 frontend layout;
5. Display Mode cycles Fullscreen Exclusive / Borderless / Windowed and Apply changes modes correctly;
6. post-level mouse alignment remains fixed.

After this batch is validated, resume:
- true Hor+ visual validation / remaining 2D layout work if needed;
- gameplay Logic Hz vs Present Hz collection;
- fixed-step/interpolation game-speed correction;
- uncapped main-menu rendering with shell logic cadence preserved.


## Exclusive fullscreen runtime crash — 2026-10-01

Fresh combined runtime test of the frontend/audio/display batch reached the new Fullscreen Exclusive path and then failed immediately after DXGI took exclusive ownership.

### Grounded runtime evidence

- Borderless and Windowed transitions completed before the failure.
- The final renderer event is:
  - `fullscreen_state exclusive=1 width=1920 height=1440 hr=0x00000000`
- The main bridge simultaneously reports:
  - `renderer11_window_mode reason=display_options_enter_exclusive mode=0 label=Fullscreen Exclusive exclusive=1 selected=1920x1440 result=1`
- Immediately after the mode switch, retail D3D7 reports `0x887601C2` at:
  - `DXPoly.cpp:785` / retail call site `0x00502912`
  - `DXinit.cpp:1105` / retail call site `0x004FD986`
- Project DirectDraw headers define `0x887601C2` as `DDERR_SURFACELOST`: the DirectDraw surface is gone and must be restored.

### Current diagnosis

This is not a DXGI capability failure: `SetFullscreenState(TRUE)` succeeded. The exclusive display-mode switch invalidates the still-live hidden retail DirectDraw/D3D7 producer surfaces. Retail then continues submitting against those lost surfaces and reaches its fatal D3D error path.

Do not remove Fullscreen Exclusive or reinterpret it as borderless. Fix the DXGI/D3D7 ownership boundary: either restore/rebuild the hidden producer after the DXGI mode switch if that coexistence is valid, or avoid executing obsolete D3D7 main-target work while true exclusive is active and the DX11 shadow renderer is authoritative. Preserve the confirmed background-distortion and post-level mouse fixes.



## DX11-authoritative renderer pivot — 2026-10-01

The project has now crossed the architectural boundary requested after the first true-exclusive runtime crash: **DX11 is the authoritative visible/main-scene renderer after its first complete replayable frame. Direct3D 7 / DirectDraw remain temporarily only as compatibility/resource providers and as a fail-closed offscreen fallback while the remaining legacy dependencies are ported.**

This is intentionally not a one-off repair for `DDERR_SURFACELOST`. The goal is to eliminate the class of problems caused by keeping two display/rendering owners alive.

### Runtime evidence that justified the pivot

The failed Exclusive test established:

- DXGI successfully entered true exclusive:
  - `fullscreen_state exclusive=1 width=1920 height=1440 hr=0x00000000`
  - bridge result for `display_options_enter_exclusive` was success.
- Retail then failed with `0x887601C2 == DDERR_SURFACELOST` in both the DXPOLY path and DXINIT.
- Exact failing legacy operations were grounded as:
  - `IDirect3DDevice7::BeginScene` through Device7 vtable slot 5.
  - `IDirectDrawSurface7::Blt` through Surface7 vtable slot 5.
- Immediately before that transition, the captured frame vocabulary was already fully representable by the DX11 replay:
  - frame 2400: 121/121 main draws submitted to DX11, `shadow_skip=0`, `missing=0`.
  - frame 2640: 104/104 submitted, including 68 2D + 36 3D draws, `shadow_skip=0`, `missing=0`, no offscreen fallback.
  - frame 2760: 134/134 submitted, `shadow_skip=0`, `missing=0`.
  - all observed main draws in the tested frontier were triangle-fan / FVF 324, with resident mirrored textures where required.

### Implemented migration commits

- `df5cf4bbbfc00ca340b750325a747780202866ac` — **renderer: make DX11 authoritative for main scene**
  - main-scene `BeginScene` / `EndScene` can now be virtualized once DX11 is authoritative.
  - `IDirectDrawSurface7::Blt` on the retail primary/main-scene surfaces is suppressed on the authoritative path.
  - main-scene clear is submitted to DX11 and no longer requires the legacy display surface.
  - any main draw successfully accepted by DX11 with its texture ready is automatically suppressed on D3D7; F9 is no longer required.
  - presentation now prefers the DX11 shadow/replay frame in Windowed, Borderless and true Exclusive.
  - retail Flip is fallback-only before DX11 becomes authoritative.
  - a genuinely unsupported/offscreen path may lazily start a real D3D7 fallback scene rather than making every frame depend on D3D7.

- `566aec873acf194c21e1ca609b52ae3b51732c8c` — **renderer: defer exclusive until DX11 frame is authoritative**
  - persisted Fullscreen Exclusive no longer lets DXGI seize the display during early startup before the first complete DX11 frame exists.
  - requested Exclusive stays non-exclusive through replay warmup.
  - after the first successful complete shadow frame, the bridge calls `SpideyApplyRendererWindowMode("dx11_authoritative_ready")` and only then enters true DXGI exclusive.
  - window-mode telemetry records requested vs actual exclusive plus the deferred state.

- `f194ca2a924644dd94225235de6988f5cf96e4bb` — **renderer: stop forwarding main-scene state to D3D7**
  - main-scene `SetViewport`, `SetRenderState`, `SetTexture`, and `SetTextureStageState` now update the DX11 shadow-state cache and return success without forwarding to D3D7.
  - a lazy legacy offscreen fallback replays the cached fixed-function state into D3D7 only when that fallback is genuinely needed.
  - added explicit telemetry for main-state suppression and fallback-scene starts.

- `0b3c6cce0b9e1715296557a47172740b9cd8881e` — **renderer: retire D3D7 reference toggles**
  - the old F9 main-draw-suppression diagnostic and F10 D3D7-reference display toggle are retired.
  - once DX11 owns the output, runtime cannot intentionally switch back to a potentially lost DirectDraw display surface.

### Current ownership model

After DX11 replay warmup:

```
retail game draw/state calls
        |
        v
compatibility interception / provenance
        |
        +--> DX11 state + textures + primitive replay --> DXGI --> visible frame
        |
        +--> D3D7 only when a still-unported offscreen/compatibility dependency
             explicitly requires it
```

The authoritative main frame must no longer require a successful D3D7 `BeginScene`, main-target `Clear`, main-target `Blt`, fixed-function state submission, main-scene `DrawPrimitive`, or retail Flip.

### New telemetry to judge remaining D3D7 dependency

`spidey-decomp-draw.log` frame summaries now expose:

- `dx11_authoritative=`
- `d3d7_suppressed=`
- `d3d7_fallback=`
- `begin_suppressed=`
- `end_suppressed=`
- `clear_suppressed=`
- `state_suppressed=`
- `blt_suppressed=`
- `fallback_scene_begins=`

The desired normal main-frame state is:

- `dx11_authoritative=1`
- `d3d7_suppressed > 0` whenever there are main-scene draws
- `missing=0`
- `shadow_skip=0`
- `d3d7_fallback=0`
- `fallback_scene_begins=0`

The desired presentation state is:

- DX11 shadow/replay presentation active.
- retail Flip inactive after warmup.
- Exclusive entered only after `dx11_authoritative_ready`.
- no `DDERR_SURFACELOST` fatal path.

### Static validation completed before runtime handoff

- Verified the exact COM layout from the checked-in DirectX headers:
  - `IDirect3DDevice7` slot 5 = `BeginScene`, slot 6 = `EndScene`.
  - `IDirectDrawSurface7` slot 5 = `Blt`.
- `main.cpp` source-level structure passed a local consistency scan after the migration:
  - braces balanced,
  - parentheses balanced,
  - brackets balanced.
- F9/F10 runtime key handlers are absent after retirement.
- GitHub Actions/check-run data was not exposed through the connected GitHub endpoint for these pushes, so this checkpoint does **not** claim a CI-green Windows build. The next normal local update/build is the build/runtime validation.

### Regression guards

Do not regress these while removing the remaining D3D7 dependencies:

- moving-background distortion/warp remains fixed;
- post-level mouse hover/click alignment remains fixed;
- six-row Audio layout + live endpoint/Bink lifetime work remains intact;
- selected modern frontend logical resolution remains authoritative;
- Hor+ projection/culling work remains intact.

### Next renderer migration after the first authoritative runtime validation

If this first DX11-authoritative runtime pass is stable:

1. **Move texture ownership upstream.**
   - stop treating a D3D7 texture surface as the source of truth;
   - upload decoded/source texture pixels directly into DX11;
   - retain a legacy surface/handle only where retail code still requires object identity.

2. **Port transient/offscreen render targets and copies.**
   - enumerate every non-main render-target fallback;
   - provide native DX11 equivalents for transient compositing/render-to-texture/movie paths;
   - drive `fallback_scene_begins` and `d3d7_fallback` to zero.

3. **Remove DirectDraw display ownership completely.**
   - DXGI/Win32 alone own resolution, window modes, resize, fullscreen and presentation.

4. **Remove the D3D7 device/init path.**
   - only after no remaining game/resource code requires real D3D7 COM rendering objects.



### DX11-authoritative hardening after initial checkpoint

Two additional commits close pre-runtime edge cases:

- `f4385217703d0a4e5669e17cd0ea95dbd337d76a` — **meta: tag DX11 migration helpers**
  - adds the repository status metadata to the new migration helper functions; no runtime behavior change.

- `4c726ebd6f4ecf0c1996bc64512596935228fc18` — **renderer: isolate legacy rebuilds from DXGI exclusive**
  - if DXGI Exclusive is already active and retail needs to rebuild its temporary compatibility device/surfaces, DXGI exclusive is explicitly released first;
  - the retail compatibility rebuild runs without competing exclusive ownership;
  - the replacement D3D7 device/main surfaces receive all interception hooks;
  - only then may DX11 reacquire true Exclusive.
  - This also covers the edge case of changing resolution while already in Exclusive, not only Windowed/Borderless -> Exclusive.

Final source-level audit at `4c726eb`:
- balanced braces/parentheses/brackets;
- `draw_frame` telemetry: 51 printf format fields / 51 supplied values;
- no F9/F10 reference-toggle key handlers remain;
- compatibility probe installation is ordered before `display_options_enter_exclusive`;
- the explicit `renderer11_release_exclusive_for_compat` path is present.

**Runtime-test frontier:** `4c726ebd6f4ecf0c1996bc64512596935228fc18` plus the status/handoff-only commits that follow it.


## DX11-authoritative startup crash — runtime result + fix (2026-10-01)

### User runtime result

The first DX11-authoritative build crashed almost immediately on launch. The user heard/briefly reached the first splash movie, then the process aborted.

Test session revision:
- `5411665738851098256555b12c3b3ac048b12cfa`

The failure was **not** caused by incorrect COM vtable slot selection:
- Device7 hooks installed for BeginScene=5, EndScene=6, SetRenderTarget=8, Clear=10, SetViewport=13, SetRenderState=20, DrawPrimitive=25, SetTexture=35, SetTextureStageState=37.
- Surface7 Blt hook installed.
- draw probe reported all 9 state hooks installed.

### Exact root cause

The first completed `DXPOLY_Flip` had no replayable geometry:
- `shadow_frame frame=1 target=1920x1440 queued=0 submitted=0 skipped_submit=0 rendered=0 ...`

However, `SpideyRenderer11_ShadowEndFrame` incorrectly returned success for an empty command stream.

The proxy interpreted that success as a complete DX11 frame:
- set `gSpideyShadowPreviewReady=1`;
- cleared exclusive deferral;
- called `SpideyApplyRendererWindowMode("dx11_authoritative_ready")`;
- DXGI successfully entered Exclusive.

The renderer then immediately rejected DX11 shadow presentation because no shadow target/SRV existed:
- `present_shadow rejected ... srv=0x00000000 shadow=0x0`.

At the same moment, the still-live DirectDraw surfaces used by the startup Bink movie became lost:
- `scene_pre ... lost_hr=0x887601C2`
- `primary_post ... lost_hr=0x887601C2`

PCMovie then failed while using its DirectDraw movie surface/path, producing the startup abort.

### Relevant architectural finding

Retail movie playback is still a genuine legacy producer:

```
BinkDoFrame
  -> lock g_MovieDD7Surface
  -> BinkCopyToBuffer
  -> unlock
  -> g_pDDS_Scene->Blt(g_MovieDD7Surface)
  -> DXPOLY_Flip
```

Therefore Bink/PCMovie must remain on the compatibility side until movie surfaces/copies are migrated to DX11. DXGI Exclusive must not invalidate those surfaces mid-frame.

### Fix commits

- `3f3d8fd6e6e8bf88a6cc596c0e8a20835277ba6b` — **renderer11: require a complete frame before takeover**
  - `ShadowEndFrame` now returns 0 for an empty queue.
  - it also returns 0 when no replay occurred.
  - after replay it returns 1 only when:
    - rendered > 0,
    - rendered == queued commands,
    - skipped submit == 0,
    - skipped render == 0.
  - frame logs now expose `presentable=0/1`.
  - This fixes the contract itself instead of special-casing frame 1 in the proxy.

- `a4e005885f0eb1780b3366a990e20ffc8db3fcc2` — **renderer: keep DXGI takeover out of retail movies**
  - proxy readiness is forced false while either retail Bink handle `0x00AC0BA4` or movie surface `0x00AC0A3C` is live.
  - takeover-gate telemetry logs movie handle/surface, shadow result, ready state and deferred state.
  - added a compatibility wrapper around retail movie-frame routine `0x0050B5A0`.
  - before the retail movie frame runs, any active DXGI Exclusive ownership is released, exclusive takeover is deferred again, and DX11 authoritative readiness is cleared.
  - this protects later Bink/cutscene playback too, not only startup.

- `ac275ef13e289e687eb3093c424d9d5ccfbd9e76` — **renderer: restore legacy movie surfaces after exclusive**
  - when DXGI Exclusive is released for a legacy compatibility producer, the retail primary and scene surfaces are explicitly restored if they are still marked `DDERR_SURFACELOST`.
  - telemetry records `primary_restore` and `scene_restore` HRESULTs.

### Expected next runtime behavior

On startup:
1. frame 1 has no DX11 geometry -> `ShadowEndFrame presentable=0`;
2. DX11 authoritative readiness remains false;
3. requested Exclusive stays deferred;
4. startup Bink movie continues using the known-good legacy DirectDraw compatibility path;
5. once no movie is active and a complete non-empty DX11 frame renders successfully, readiness becomes true;
6. only then does DXGI acquire true Exclusive and DX11 become the visible main renderer.

During later movies/cutscenes:
1. movie-frame wrapper releases DXGI Exclusive before retail touches its surfaces;
2. legacy primary/scene are restored if necessary;
3. movie remains compatibility-rendered;
4. after the movie ends, a later complete DX11 frame can reacquire Exclusive.

### What to verify in the next runtime

Use the normal updater/build and simply boot first.

Success indicators:
- `shadow_frame frame=1 ... queued=0 ... presentable=0`
- no `renderer11_window_mode reason=dx11_authoritative_ready ... exclusive=1` during the empty startup frame
- `dx11_takeover_gate ... movie_blocks=1 ... ready=0 deferred=1` while splash/Bink is active
- `movie_frame_compat patched_calls` should be nonzero
- startup movies survive
- after movies finish, first complete non-empty DX11 frame reports `presentable=1`
- only then does `dx11_authoritative_ready` enter Exclusive
- no PCMovie D3D error / no `DDERR_SURFACELOST` abort

If boot reaches the menu, continue the existing Windowed/Borderless/Exclusive and gameplay validation from the prior handoff.


## Runtime result: training stable, normal-level transition crash + post-training cursor floor — 2026-10-01

Tested revision:
- `8afcffd25e907f4fc34f8f2aaa8154e9a924e60a`

### User-visible result

- Game now boots successfully through the startup movies.
- User could enter and play the training level.
- Returning from training to the menu exposed a cursor clamp/floor: the cursor could not move below a horizontal Y boundary.
- Starting a normal level then exited/crashed during the transition.

This is a meaningful improvement over the previous startup failure: the DX11-authoritative main renderer survived a full training gameplay session.

### DX11 gameplay evidence

Sampled gameplay frames show the main renderer is not the failure:
- thousands of D3D7 main-scene draws are successfully captured, submitted to DX11, and suppressed from the legacy renderer;
- `missing=0`;
- `shadow_skip=0`;
- `d3d7_fallback=0`;
- `fallback_scene_begins=0`;
- sampled shadow frames are `presentable=1`.

Therefore do **not** roll back the DX11-authoritative main-scene path because of this level-transition crash.

### Fatal transition path

The new D3D error log contains:
- `PCTex.cpp:1740 error=0x00000001`;
- `PCMovie.cpp:897 error=0x887601C2 == DDERR_SURFACELOST`;
- `DXinit.cpp:1105 error=0x887601C2 == DDERR_SURFACELOST`.

Static source inspection separates the first entry from the fatal errors:
- reconstructed PCTex code stored `pTempSurf->Release()` into an `HRESULT` and passed the returned COM reference count into `D3D_ERROR_LOG_AND_QUIT`;
- a return count of `1` is not a failed HRESULT, so this line was diagnostic noise, not the process-exit cause.

The real PCMovie path is:
```
movie surface Lock/Copy/Unlock
  -> g_pDDS_Scene->Blt(movie surface)
  -> DXPOLY_Flip
```

The scene/primary DirectDraw surfaces had already been invalidated by an earlier DXGI-exclusive interval. The previous movie fix only attempted restoration when DXGI was **currently** Exclusive. In this test the user later switched to Windowed, so by the time the next level movie started the helper returned early even though the old DirectDraw scene surface remained `DDERR_SURFACELOST`. PCMovie then blitted into the dead scene surface and the retail fatal-error path exited.

### Cursor-floor root cause

The same session also exposed an ownership bug in the display wrapper.

The user explicitly applied:
- selected output = `2560x1440`;
- window mode = Windowed.

A later **internal retail** `DXINIT_SetDisplayOptions` request then changed:
- selected output from `2560x1440` to `1920x1440`;
- while pending menu selection still remained `2560x1440`.

That produced:
- selected = `1920x1440`;
- 16:9 logical content = `1920x1080`;
- physical compatibility backing = `1920x1440`.

The existing confirmed-good post-level mouse sync then correctly clamped to that now-wrong logical `1920x1080` domain, yielding bounds `0,0,1888,1048`. Visually, inside the taller output/window, that appears as the reported horizontal cursor floor.

The mouse conversion itself is not being reverted. The ownership bug is that retail compatibility transitions were allowed to overwrite the modern selected output.

### Fix commits

- `d7ef76af697d4c7cf9ac64e1a24bbbecd4123892` — **display: preserve modern output across retail transitions**
  - internal retail display requests may change only the hidden compatibility producer;
  - they no longer overwrite `gSpideySelectedOutputWidth/Height`;
  - the modern selection is seeded from retail only if no valid modern selection exists yet;
  - modern Display -> Apply remains the owner of the user-facing DX11 resolution;
  - display telemetry now logs requested vs selected vs physical dimensions and whether selection was seeded.
  - This should keep a user-selected 2560x1440 output at 2560x1440 through training -> menu -> level transitions and therefore keep the previously validated mouse domain intact.

- `9c8a2c30696d2fcbfaa4af435168ae1202590cc9` — **renderer: always restore legacy compatibility surfaces**
  - DXGI Exclusive release remains conditional on actual exclusive ownership;
  - **primary/scene DirectDraw `IsLost/Restore` checks now run unconditionally** whenever the compatibility helper is invoked;
  - this covers surfaces invalidated by an earlier exclusive interval even if the current visible mode is Windowed or Borderless;
  - movie-frame compatibility therefore repairs `g_pDDS_Scene` before the retail movie Blt;
  - display-option/rebuild compatibility receives the same protection;
  - telemetry now records `had_exclusive`, release result, lost-state HRESULTs and restore HRESULTs separately.

- `d8edc7e24bfa341145f100fbb4d3a09f3d51a494` — **pctex: stop treating Release refcount as HRESULT**
  - removes the false `D3D error=0x00000001` diagnostic from texture staging cleanup;
  - does not change the actual texture data/mirroring path.

### Static audit

At `d8edc7e`:
- `main.cpp` braces/parentheses/brackets balanced;
- `PCTex.cpp` braces/parentheses/brackets balanced;
- unconditional legacy-surface repair is present;
- modern-output seed guard is present;
- the PCTex `Release()` HRESULT misuse is gone.

### Next runtime test

Use the normal updater/build.

Focused pass:
1. boot through splash movies;
2. keep/apply 2560x1440 in the desired window mode;
3. enter training;
4. return to the menu;
5. verify cursor can reach the full intended menu area and hover/click alignment remains correct;
6. start the same normal level that crashed this session;
7. if it enters, play for ~30-60 seconds.

Most important log evidence:
- display transition should show retail `requested=1920x1440...` (if retail still asks for it) while modern `selected=2560x1440...` remains unchanged;
- post-training frontend mouse sync should therefore use 2560x1440 rather than 1920x1080;
- movie-frame compatibility should show `scene_lost=DDERR_SURFACELOST` followed by successful `scene_restore=0x00000000` when needed;
- no fatal `PCMovie.cpp:897` / `DXinit.cpp:1105` surface-lost entries;
- DX11 gameplay should continue with `missing=0`, `shadow_skip=0`, `d3d7_fallback=0`.


## Runtime result: level-transition crash fixed; cursor floor + oversized frontend text remain — 2026-10-01

Tested revision:
- `78cfba2b22fa6a4dc079f4ddd14f9ca1793e5f8e`

### User-visible result

- **No crash** in this pass.
- The previously failing normal-level transition now survives.
- After returning from a level, the shell cursor still cannot move below a visible horizontal threshold.
- Audio Output remains partially cut off.
- Display Mode values including Fullscreen Exclusive / Windowed / Borderless remain too large / clipped.
- User explicitly requested that frontend text scale down with higher resolution instead of retaining the oversized legacy visual scale.

Treat the level-transition surface-lost crash as fixed unless it regresses.

### Mouse-floor evidence

Fresh input telemetry still showed:
- frontend logical canvas = `1920x1080`;
- mouse bounds = `0,0,1888,1048`.

The live HWND client in the same session is approximately `1920x1421`.

Therefore the old frontend sync was still using the **logical render canvas as the raw virtual-mouse clamp**. That is the wrong ownership boundary:
- raw relative cursor motion belongs to the live client domain;
- shell cursor drawing and menu hit testing belong to the logical frontend domain.

### Mouse-domain fix

Commit:
- `8ed74f4a8a54b8b884434813969e099d8ff817f5` — **input: decouple frontend cursor bounds from logical canvas**

Behavior:
- raw `gMouseX/gMouseY` bounds now use the actual live HWND client size (fallback to selected/legacy dimensions only if no valid client is available);
- `PCINPUT_GetMousePosition @ 0x0050A750` is now hooked so the visible shell cursor receives a client->logical mapping;
- `PCINPUT_GetMouseHotspotPosition @ 0x0050A770` uses the exact same mapping before adding the hotspot offset;
- `PCINPUT_IsMouseOver @ 0x0050A820` therefore continues to use the same logical coordinate domain as the visible cursor;
- existing strict retail hit-test boundaries are preserved;
- new telemetry records client size, raw bounds/position, mapped logical position, and logical size.

This is intentionally a three-path fix: changing only the raw bounds would have risked reintroducing the previously fixed post-level hover/click offset.

### Frontend typography grounding

Retail shell layout is authored on a fixed Dreamcast-style coordinate grid:
- X = 0..512;
- Y = 0..240.

`PCSHELL_CoordsDCtoPC` expands those coordinates to the live render dimensions.

Retail font scale is separate:
- `Mess_SetScale @ 0x00458620`;
- `G_SCALE @ 0x0060D5A4`;
- `Mess_DrawText` sets the active Font scale from `G_SCALE`;
- `Mess_TextWidth` uses the same scale, so changing it affects both rendering **and menu width measurement**.

The normal/small shell font helpers still request scale 256, which was correct at the 640x480 PC baseline but becomes visually oversized when the shell coordinate canvas expands to 1080p/1440p.

### Resolution-aware frontend text fix

Commit:
- `ef7df27e9b24349ef4a7413464d50e99ec69d498` — **frontend: scale text with modern resolution**

Behavior:
- retail code can continue requesting its authored text scales;
- frontend-only effective scale is:
  - `requested * 480 / logical_height`;
- 640x480 keeps scale 256 unchanged;
- 1920x1080 maps 256 -> approximately 113;
- 2560x1440 maps 256 -> approximately 85;
- proportional differences between normal/small/menu-selected scales are preserved;
- scale is clamped to avoid pathological tiny values;
- gameplay/non-frontend text keeps the retail-requested scale unchanged;
- the last raw requested scale is remembered and re-evaluated immediately whenever logical/frontend resolution state changes, so level -> menu transitions do not wait for a later font-reset call;
- the override is deliberately installed **after `patch_mess()`**, because `patch_mess()` also owns retail `Mess_SetScale`.

Because `Mess_TextWidth` reads the same effective scale, menu boxes/centering/longest-label measurement now use the smaller modern text too. This is not a final-pixel-only shrink.

### Audio/display layout interaction

Existing six-row Audio layout remains intact:
- Audio text is shifted up one authored row;
- all three slider draws use the same shift;
- slider mouse hit regions use the same shift;
- Stereo/Mono value uses the same shift.

The new typography scaling is applied globally to frontend text, including the added Audio Output and Display Mode labels/values. No additional per-label pixel nudges were added in this pass.

### Static audit

At `ef7df27`:
- main.cpp braces / parentheses / brackets balanced;
- frontend scale override is installed after `patch_mess()`;
- mouse position hook at `0x0050A750` is present;
- raw frontend bounds are based on the client domain;
- hit-test hotspot and visible cursor position share the same client->logical mapping;
- frontend text scaling uses the 480-line PC baseline and is re-applied on logical transitions.

### Next focused runtime test

1. Run normal updater/build.
2. Boot to menu.
3. Open Audio:
   - verify Output row is fully visible;
   - verify text is noticeably smaller / resolution-appropriate;
   - verify sliders and hit regions remain aligned.
4. Open Display:
   - cycle Fullscreen Exclusive / Borderless / Windowed;
   - verify each complete value is visible.
5. Enter a level and return to menu.
6. Move the cursor through the entire usable frontend area, especially downward.
7. Verify hover/click still line up with the visible cursor after returning from the level.
8. Upload fresh input/compat/draw logs if any of these still fail.

Key expected input telemetry:
- `frontend_bounds_sync ... client=... bounds=... basis=client_to_logical`;
- raw Y bound should reflect the live client, not logical `1080-32`.

Key expected typography telemetry:
- `frontend_text_scale ... requested=256 effective=113 ... logical=1920x1080` at 1080-high logical frontend;
- or approximately `effective=85` at 1440-high logical frontend.


## Follow-up runtime: Display Apply resets text + mouse trapped in upper-left box — 2026-10-01

Tested revision:
- `56374190919f6a12ff8abbe6602fee4dcba75f1b`

### User-visible result
- Changing resolution makes frontend text large again immediately.
- Text becomes correctly small again only after entering a level and returning.
- Mouse is not merely hitting a lower floor: it is confined to an invisible rectangular region in the upper-left portion of the screen.

### Log-grounded text diagnosis
Fresh compat telemetry proves the scale hook itself is working:
- frontend setup reaches `requested=256 effective=113 frontend=1 logical=1920x1080`.
- But Display Apply calls the modern selected resolution through `SpideyCompatSetDisplayOptions`, which classifies that rebuild as gameplay and logs `frontend=0`.
- The same transition then sets `requested=256 effective=256`, so the font becomes retail-sized again even though the user is still physically in the shell.
- A later level -> frontend transition reasserts frontend mode and returns the effective scale to the smaller value.

Fix:
- `b04a78b492dfe41842d42cf6017328b48d4138e6` — **frontend: fix apply scale reset and mouse double conversion**
- `SpideyDisplayConfirmOrApply` now remembers whether the shell was live before the rebuild.
- Immediately after the modern display rebuild, if the shell was live it restores `gSpideyFrontendLegacyMode=1`, refreshes the modern logical size, reapplies the logical render resolution (which reapplies the effective text scale), and resynchronizes mouse bounds.
- Expected new compat line after Apply: `frontend_text_scale reason=display_apply_frontend_restore ... frontend=1`.
- The text should no longer wait for a level roundtrip.

### Log-grounded mouse diagnosis
The previous raw-bound fix succeeded:
- client = `2560x1440`
- raw bounds = `0,0,2528,1408`
So the remaining upper-left rectangle was not caused by raw clamping.

The bug was a double transform:
1. raw virtual mouse was mapped from client coordinates into modern logical `1920x1080`;
2. retail `PCSHELL_CoordsPCtoDC` then performed its own PC-pixel -> 512x240 shell conversion.

The shell expects `PCINPUT_GetMousePosition` and hotspot/hit-test coordinates in the retail DirectX PC canvas, not the modern DX11 logical canvas.

Mouse fix in `b04a78b`:
- raw bounds remain full HWND client size;
- raw client coordinates are mapped exactly once into the retail PC canvas (`0x006B78E4/0x006B78E8`, same backing domain consumed by PCSHELL);
- visible cursor, hotspot, and `IsMouseOver` all use that same retail-PC mapping;
- modern logical dimensions are now telemetry only for this path.

Expected new input telemetry:
- `basis=client_to_retail_pc_canvas`
- `shell_canvas=<retail backing>`
- `modern_logical=<DX11 content canvas>`

Validation metadata follow-up:
- `19eb5bca0824e31d09e66ccce737e9ebca653d30` — **meta: validate frontend mouse ownership helpers**
- adds repo-validator status comments to the new frontend/mouse helpers and cleans stale comments that still referred to the old logical mapping.

### Focused retest
1. Open Display and change resolution; Apply.
2. Verify text stays small immediately after Apply.
3. Repeat across 2-3 resolutions.
4. Move cursor to all four screen edges/corners while still in frontend.
5. Enter a level and return.
6. Again reach all four edges/corners and check hover/click alignment.
7. Upload fresh compat + input logs if either issue persists.


## Permanent runtime logging policy — single attachment only (2026-10-01)

User requirement:
- **All routine runtime diagnostics must be consolidated into one log file** so test sessions do not consume the ChatGPT attachment limit.
- Future chats/tests should request **only `spidey-decomp.log`** unless there is an exceptional, explicitly stated reason for a different artifact.

Implementation commits:
- `66d39d8f5ceb24d60cf87954e4059c940311b151` — **logging: consolidate proxy telemetry into one file**
  - all proxy-side COMPAT/AUDIO/INPUT/CAMERA/TEXTURE/DRAW/TIMING/PRESENT/DXERROR/RUNTIME logging now opens `spidey-decomp.log`;
  - proxy opens prefix their category, e.g. `[COMPAT]`, `[DRAW]`, `[PRESENT]`, `[INPUT]`, etc.
- `ae8b8332d3fa94ee8258e1d77a74099e6c2c00db` — **logging: fold crash diagnostics into consolidated log**
  - vectored crash handler now appends its crash section to the same file rather than creating `spidey-decomp-crash.log`.
- `1d49799222c71696ee57b222fc33e3d142574c11` — **logging: send renderer11 telemetry to consolidated log**
  - renderer helper writes each line as `[RENDERER11] ...`.
- `897c4f8b6e711405beaff7585802bc3590331b90` — **logging: send input11 telemetry to consolidated log**
  - modern input helper writes each line as `[INPUT11] ...`.
- `ea9b5d679d300a8f749be8f806171974c5ffdde8` — **logging: archive one consolidated runtime log**
  - test launcher deletes stale legacy split logs;
  - seeds `spidey-decomp.log` with `[SESSION]` revision, DLL hashes, game path, EXE hash/PE metadata and start time;
  - appends exit code/end time after the game exits;
  - copies only the consolidated runtime log into the timestamped session folder;
  - launcher explicitly tells the user to attach only `spidey-decomp.log`.
- `c87954c7616d18321a22be8846aa0e1134078fbe` — **logging: make runtime session folder single-file**
  - routine session no longer creates separate `test-session.txt`, `game-exe-fingerprint.txt`, or copied `proxy-link-map.txt` artifacts;
  - the important session/fingerprint identity is already embedded in `[SESSION]` records in the consolidated log;
  - linker/source information remains available from the live repo/build when needed.

Current consolidated categories include:
- `[SESSION]`
- `[COMPAT]`
- `[AUDIO]`
- `[INPUT]`
- `[CAMERA]`
- `[TEXTURE]`
- `[DRAW]`
- `[TIMING]`
- `[PRESENT]`
- `[DXERROR]`
- `[CRASH]`
- `[RUNTIME]`
- `[RENDERER11]`
- `[INPUT11]`

Static audit:
- proxy source contains zero old split-log filenames;
- renderer11 source contains zero `spidey-renderer11.log` references;
- input11 source contains zero `spidey-input11.log` references;
- old filenames remain only in the launcher cleanup array;
- main.cpp structural delimiter balance remains clean.

**Do not regress this.** Historical documentation may mention uploading several logs, but those instructions are superseded by this policy.


## Google Drive log retrieval + Display Options back-out text regression — 2026-10-01

### Permanent Google Drive runtime-log location

Project Drive root:
- https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

Runtime Logs folder:
- https://drive.google.com/drive/folders/1Lly3NKgwHt2tHq7chejgt9gvsOTyPu5s

The assistant has verified that the connected Google Drive can list and read this folder directly.

**Future test workflow:**
- user puts the newest runtime log in this `Logs` folder;
- assistant should read the newest log directly from Drive;
- do not ask the user to upload it into chat;
- after the single-log migration, the normal runtime artifact is only `spidey-decomp.log`;
- historical split logs currently in the folder are from pre-consolidation sessions and may still be read when diagnosing those sessions.

### Runtime session analyzed from Drive

Drive session identity:
- revision `b0c4910d007d7c3562552cceaed45dfb196c10bf`
- started 2026-10-01 00:54:12 -04:00.

User-visible behavior:
- applying/changing resolution initially corrected the frontend text size;
- backing out of Display Options immediately made frontend text large again;
- mouse was not tested in this session.

### Exact text-scale trace

At shell/frontend entry:
- `frontend_text_scale ... requested=256 effective=113 frontend=1 logical=1920x1080`.

After selecting/applying 2560x1440:
- the compatibility display rebuild temporarily logged gameplay classification:
  `frontend_text_scale reason=display_options_gameplay requested=256 effective=256 frontend=0 logical=2560x1440`.
- the explicit post-Apply restore then correctly fixed it:
  `frontend_text_scale reason=display_apply_frontend_restore requested=256 effective=85 frontend=1 logical=2560x1440`.

Immediately after backing out of Display Options:
- another retail display-options transition occurred at the already-selected 2560x1440 mode;
- that transition again classified the still-live shell as gameplay;
- text scale returned to:
  `requested=256 effective=256 frontend=0 logical=2560x1440`.

Therefore Apply itself was fixed; the remaining bug was the frontend/gameplay **classification policy** inside `SpideyCompatSetDisplayOptions`.

### Root-cause correction

Previous code inferred frontend ownership from:
1. an exact `640x480x16 option4=0` DirectDraw request, and in one Apply path;
2. `0x006B78F4`.

Static source review corrected a mistaken assumption:
- `0x006B78F4` is the retail DirectDraw/windowed option flag (`gDxOptionRelated`);
- it is **not** a shell/frontend lifecycle flag.

The real shell code provides lifecycle boundaries:
- retail `PShell_Initialise = 0x0048D790`;
- retail `PShell_Cleanup = 0x0048D880`;
- reconstructed shell sets `gShellInitialized=1` at the end of `PShell_Initialise`;
- it clears `gShellInitialized=0` in `PShell_Cleanup`.

### Fix

Commit:
- `02077c1218251814dcecfa7f02881a12f0752178` — **frontend: drive text scaling from shell lifecycle**

Implementation:
- adds independent `gSpideyFrontendUiActive`;
- patches all direct retail CALLs targeting `PShell_Initialise @ 0x0048D790` and `PShell_Cleanup @ 0x0048D880` to lifecycle wrappers while leaving the original function entries untouched;
- shell initialise wrapper marks frontend active **before** calling retail so `PShell_NormalFont/Mess_SetScale` inside initialization already sees frontend policy;
- reasserts frontend active after initialization;
- cleanup wrapper calls retail then marks frontend inactive and immediately restores non-frontend text scale;
- exact `640x480x16 option4=0` shell request remains only as an early-entry fallback signal;
- modern resolution display rebuilds while the shell is active can no longer set frontend ownership false;
- Display Apply now gets its `liveFrontend` state from actual frontend ownership, not `0x006B78F4`;
- resolution-aware typography predicate follows actual frontend ownership;
- display-options telemetry now distinguishes:
  - `frontend_active=`
  - `frontend_legacy_request=`
- lifecycle install telemetry reports how many direct init/cleanup call sites were patched;
- lifecycle transitions log `frontend_lifecycle reason=... active=...`.

Static audit after implementation:
- source delimiters balanced;
- no remaining use of `0x006B78F4` as the Display Apply frontend signal;
- lifecycle installer is called from `game_patches()`;
- old request signature is retained only for compatibility-backing behavior / early fallback, not ownership revocation.

### Next test

Use normal updater/build.

Test only:
1. enter Display Options;
2. change resolution and Apply;
3. verify text becomes/stays resolution-appropriate;
4. back out to parent Options menu;
5. verify text **remains** resolution-appropriate;
6. move through a few other frontend menus to ensure scale persists;
7. optionally test the mouse if convenient.

After exit, place the single new `spidey-decomp.log` into the Drive `Logs` folder. The assistant should pull it from Drive directly.

Expected new log evidence:
- `[COMPAT] frontend_lifecycle_install ... initialise_calls=>0 ... cleanup_calls=>0`;
- while navigating frontend menus:
  `frontend_active=1` even when display options request modern 2560x1440;
- no post-back-out `frontend_text_scale ... effective=256 frontend=0` while shell remains active;
- on actual transition to gameplay:
  `frontend_lifecycle ... active=0` after `pshell_cleanup_post`.


## Runtime: frontend scale fixed; gameplay/pause UI still retail-density — gameplay UI scaling implementation (2026-10-01)

Latest Drive log:
- Drive Logs file: `spidey-decomp.log`
- file ID: `156DWlCKknurrXEr6WLx9C8Fq2ksdO6lq`
- modified: 2026-10-01T05:16:11.724Z
- tested revision: `0b2d306b48beadfb1d0de6dc45a8dad4f4fb3faf`
- process exit code: 0

User result:
- frontend/main-menu text now stays at the correct resolution-aware size;
- gameplay UI text remains at retail density;
- pause menu remains visually retail-sized;
- gameplay HUD/widgets do not follow the compact high-resolution UI density policy.

### Log evidence

The shell lifecycle fix is working:
- lifecycle hook install patched 3 PShell initialise call sites and 3 cleanup call sites;
- frontend remains active through menu navigation at 2560x1440;
- actual gameplay transition occurs only at `PShell_Cleanup`.

At gameplay transition the old typography policy explicitly reverted to retail scale:
`frontend_text_scale reason=pshell_cleanup_post requested=256 effective=256 frontend=0 logical=2560x1440`.

This directly explains gameplay/pause text remaining large.

Gameplay DX11 rendering itself is healthy and already uses the selected logical canvas:
- steady gameplay samples show `modern=1 logical=2560x1440`;
- thousands of 3D draws are DX11-authoritative;
- ordinary sampled gameplay frames have `missing=0` and `shadow_skip=0`;
- gameplay 2D draws already span the modern canvas (for example x roughly 75..2410 and y 78..1338 in steady HUD frames);
- pause/menu-like gameplay frames raise 2D draw counts substantially (200+ 2D draws), confirming the pause UI is flowing through the gameplay 2D path.

### Gameplay/pause text fix

Commit:
- `563fdd671d89904a4f59bb910ab798a7c8615776` — **ui: scale gameplay and pause text with resolution**

Change:
- `Mess_SetScale` compatibility no longer limits resolution-aware scaling to PShell/frontend mode;
- modern-resolution UI text now uses the same density policy in frontend, gameplay, pause menus, mission text, and HUD text;
- policy remains based on the 640x480 baseline:
  `effective = requested * 480 / logical_height`;
- at 2560x1440, requested scale 256 -> effective ~85;
- at 1920x1080, requested scale 256 -> effective ~113;
- telemetry renamed to `ui_text_scale ... scope=frontend_gameplay_pause`.

This preserves the user-approved high-resolution frontend density while extending it to gameplay/pause typography.

### Grounded gameplay widget RE

The gameplay panel system has an existing retail resolution scaler.

Reference globals:
- `Xres = 512`
- `Yres = 240`
- set by retail `M3dInit_InitAtStart @ 0x00453200`.

Retail panel draw functions:
- `DCPanel_DrawTexturedPoly_1 @ 0x004624A0`
- `DCPanel_DrawTexturedPoly_0 @ 0x004626A0`
- `DCPanel_DrawTexturedPoly @ 0x00462930`

Assembly proves these functions multiply panel `POLY_FT4` coordinates by:
- live logical width / 512;
- live logical height / 240;
before submitting through `PCGfx_DrawQPoly2D @ 0x00507910`.

Therefore the gameplay panel already preserves the same *relative* size at higher resolutions. That is why it still looks like the original UI density instead of becoming compact like the newly fixed text.

Panel rectangle setup:
- `Panel_SetStretchedScreenCoords(SAnimFrame*) @ 0x00462C30`
- `Panel_SetStretchedScreenCoords(Texture*) @ 0x00462CD0`.

Known panel users:
- Timer: `Panel_DisplayTimer @ 0x00461D00`
- Compass: `Panel_DisplayCompass @ 0x00463860`
- Boss/extra health: `Panel_DisplayHealthBar @ 0x00464270`
- broad composite gameplay panel routine is the previously unnamed `sub_4658C0`, which calls Timer/Compass and then multiple player-HUD rectangle setups before the health-bar path.

The six player-HUD setup calls inside `sub_4658C0` include:
- 0x004659C1
- 0x00465D70
- 0x00465FA0
- 0x004661D7
- 0x004663FB
- 0x00466623

The panel helper path is therefore a safe UI-specific place to apply compact density. Do **not** globally scale arbitrary DX11 2D primitives or SlicedImage2 objects; those paths also include effects/fonts/cursor/full-screen content and would risk double-scaling.

### Gameplay HUD/widget compact-density fix

Commit:
- `d680f800c244ab13dfcc805ceb9225b08a657115` — **ui: scale gameplay HUD widgets with resolution**

Implementation:
- patches direct retail calls to:
  - `0x00462C30` (SAnimFrame rectangle setup)
  - `0x00462CD0` (Texture rectangle setup)
- leaves the retail function entries untouched; wrappers call retail first, then compact the resulting `POLY_FT4`;
- only applies while actual PShell/frontend is inactive;
- only applies above the 640x480 baseline;
- density:
  - X = `640 / logical_width`
  - Y = `480 / logical_height`
- at 2560x1440 this is X=0.25, Y=0.333333;
- because retail panel rendering later expands from 512x240 to the modern canvas, this inverse density produces baseline-like pixel density instead of proportionally enlarging the HUD with resolution;
- each rectangle is anchored to left/center/right and top/center/bottom based on its original position, so compacting does not collapse all HUD elements toward the upper-left;
- full-screen non-panel 2D effects are untouched;
- frontend shell UI is untouched by this panel transform;
- all known timer/compass/health/player panel consumers share the policy consistently.

New telemetry:
- `gameplay_ui_scale_install ... frame_calls=... texture_calls=...`
- first 16 compacted rectangles:
  `gameplay_ui_scale source=... logical=... density=... anchor=... before=... after=...`.

Static audit at `d680f80`:
- main.cpp brace/paren/bracket balance clean;
- one install call in game_patches;
- both panel wrappers present;
- validator `// @Ok` metadata added to all new helpers;
- text scaling scope includes frontend/gameplay/pause.

### Next runtime test

Run `UPDATE_AND_TEST_LATEST_BUILD.bat`.

Focused checks:
1. main menu text remains correct;
2. enter gameplay at 2560x1440;
3. inspect Spider-Man HUD widgets (health/web/icon etc.) for compact high-resolution density;
4. pause:
   - pause text should now use the compact modern scale;
   - inspect any pause-menu chrome/background/highlight elements and note anything still oversized;
5. optionally compare at 1920x1080 vs 2560x1440 to confirm UI density changes consistently with resolution;
6. exit normally.

Put the new single `spidey-decomp.log` in the Drive `Logs` folder and tell the assistant the run is complete. Pull it directly from Drive.

Expected log:
- `[COMPAT] ui_text_scale ... requested=256 effective=85 ... logical=2560x1440 ... scope=frontend_gameplay_pause` during gameplay/pause;
- `[COMPAT] gameplay_ui_scale_install ... frame_calls=>0 texture_calls=>0`;
- `[COMPAT] gameplay_ui_scale ... density=0.250000,0.333333 ...` for 2560x1440.


## Gameplay UI runtime-scale controls + health-bar fill synchronization frontier (2026-10-01)

### User runtime report that opened this frontier

The first gameplay-HUD screenshot after the resolution-aware panel work showed two distinct facts:

- the HUD holders/frames routed through the panel coordinate helpers were being compacted;
- the colored health/web fill primitives were **not** following those holders and remained detached/oversized;
- the compacted HUD was also slightly smaller than the user wanted.

The screenshot is in the Google Drive `Logs` folder as:

- `Screenshot 2026-10-01 015811.png`

The `spidey-decomp.log` currently beside it is **not** the matching run. Its session revision is still:

- `0b2d306b48beadfb1d0de6dc45a8dad4f4fb3faf`

Therefore that log must not be used as runtime validation for the gameplay-UI commits or the work below. A fresh consolidated log is required after the next test.

### Retail binary RE: why the bars separated from their holders

A user-uploaded `SpideyPC.exe` was inspected directly. Its full-file SHA-256 in this chat is:

- `0A11A49F3F63D4BB15650F322D654553FC47234B01885B852A058FF074689DEA`

It has the expected retail PE timestamp `0x3B7A3167` and image size `0x02A0D000`. The installed-game runtime log previously reported a different full-file SHA-256, so these two files must not be treated as byte-for-byte identical. However, every machine-code call site used by this RE matches the expected retail address/target bytes, so the uploaded EXE is used only as grounded static evidence for these specific routines.

`Panel_DisplayHealthBar @ 0x00464270` contains five fill/render calls that bypass the two panel coordinate helpers already compacted by `d680f80`:

- `0x004644E3 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x00464707 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x00464936 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x0046497D -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`
- `0x0046499F -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`

This directly explains the screenshot: holders use the patched frame/texture coordinate setup while the colored fill geometry can take its own QPoly/flat-shaded path.

The fix is deliberately call-site scoped. It does **not** globally scale every `PCGfx_DrawQPoly2D` or flat 2D primitive in the game.

### New user-facing scale policy

Implemented on `dev`:

- `2b97806c42b2ef69bff237d3c37833a2e4f034ea` — **ui: add live gameplay and text scale controls**
- `a8b3394b1ccd32621ed3729922256cc224e55533` — **ui: scale health bar fills with HUD holders**
- `af814fa6526e31fa228f765cdd484e057debb595` — **chore: annotate new UI compatibility helpers**

Display Options is expanded from five rows to seven:

0. Screen Size
1. Aspect Ratio
2. Brightness
3. Gameplay UI Scale
4. Menu/Text Scale
5. Display Mode
6. Apply

Both scale controls:

- use independent pending state;
- are displayed with retail-style sliders;
- support the same left/right trigger path used by the retail shell;
- use a 50%–200% range in 5% steps;
- persist in `spidey-modern-video.ini`;
- do **not** change the live game until Apply is activated.

Defaults:

- Gameplay UI Scale: **125%**
- Menu/Text Scale: **100%**

The 125% gameplay default intentionally loosens the earlier aggressive high-resolution compaction. At 2560x1440 the former density was approximately `0.250000,0.333333`; the new default becomes approximately `0.312500,0.416667`.

### Live Apply behavior

The Apply hook now distinguishes display changes from UI-only changes.

If only Gameplay UI Scale and/or Menu/Text Scale changed:

- no DirectDraw/DXGI device rebuild is performed;
- the committed text multiplier is applied immediately to the live message scale;
- gameplay HUD wrappers read the committed gameplay multiplier on the next draw;
- the settings are persisted immediately;
- no game restart or level reload is required.

Resolution/aspect/window-mode changes retain the existing live display-rebuild path.

Expected telemetry includes:

- `ui_scale_settings load ...`
- `display_pending_ui_scale kind=gameplay_ui ...`
- `display_pending_ui_scale kind=menu_text ...`
- `display_apply ... gameplay_ui=... text=... display_changed=0 ui_changed=1 in_level=...`
- `ui_text_scale ... user_percent=...`
- `gameplay_ui_scale ... user_percent=...`

### Display Options while a level is paused

Retail `Front_Update` uses the pause-menu `CMenu_Update` call at:

- `0x004415F8 -> CMenu_Update @ 0x00440600`

The retail pause menu owns Continue / Restart level / Quit logic and dispatches choices by comparing their strings after the update. The compatibility wrapper adds a fourth `Display Options` entry. Because retail has no matching branch for that new string, the wrapper consumes confirm on that row and opens:

- `PCSHELL_DoDisplayOptions @ 0x0050D9B0`

while the game remains paused.

This gives the player access to the new Gameplay UI Scale and Menu/Text Scale sliders **during a level**, even though retail did not expose Display Options there.

Expected telemetry:

- `pause_display_options entry_added=1 ...`
- `pause_display_options phase=open ...`
- `pause_display_options phase=close ...`

### Health/web fill synchronization

The three direct health-bar QPoly calls and two direct flat-shaded calls listed above now have narrow wrappers that apply the same committed gameplay-UI density as the holder geometry.

Expected telemetry:

- `gameplay_ui_scale_install ... health_qpoly=1,1,1 health_flat=1,1 ...`
- `gameplay_ui_fill_scale source=qpoly ...`
- `gameplay_ui_fill_scale source=flat ...`

This is the first build where the screenshot-specific “bars detached from holders” defect is addressed directly.

### Static validation completed before runtime

Grounded static checks completed:

- all five health-bar call-site opcodes/targets were verified directly in the uploaded retail EXE;
- the pause-menu update call `0x004415F8 -> 0x00440600` was verified directly;
- the Display Options draw/update/apply calls `0x0050DC26`, `0x0050DCA0`, and `0x0050DCF8` were verified directly;
- the final `main.cpp` lexical state terminates in normal code;
- braces, parentheses, and brackets are balanced after stripping comments/string/character literals;
- each newly introduced helper definition is unique and carries an `// @Ok` validator annotation.

GitHub Actions runs are not being spawned by the connector-written commits on this repository, so there is no CI result to claim. Do not describe this frontier as runtime-tested or CI-proven yet.

### Mandatory next runtime test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Primary resolution: **2560x1440**.

In one run:

1. Main-menu regression guard:
   - menu remains correctly scaled and usable.
2. Enter gameplay:
   - verify the health/web colored bars remain inside/aligned with their holders;
   - verify the default 125% gameplay scale is slightly larger than the previous screenshot.
3. Pause during the level:
   - verify a new `Display Options` row is present.
4. Open Display Options from the pause menu:
   - verify separate `Gameplay UI Scale` and `Menu/Text Scale` slider rows are visible.
5. Change Gameplay UI Scale only, then press Apply:
   - HUD size must change immediately after returning to the paused/gameplay view;
   - no restart or level reload.
6. Change Menu/Text Scale only, then press Apply:
   - pause/menu text size must change immediately;
   - no restart or level reload.
7. Re-open the menu:
   - committed values should still be selected.
8. Exit normally so the single consolidated log is complete.

Put only the resulting `spidey-decomp.log` in the Drive `Logs` folder. The assistant should pull it directly and correlate it with screenshots rather than asking for a log upload.


## eb8ca93 runtime failure + corrective pause/HUD/Alt+Tab frontier (2026-10-01)

### Runtime-tested build and evidence

User tested:

- session revision `eb8ca938760616236453e7258f1d6d2f09e94eaa`;
- selected output `2560x1440`;
- Gameplay UI Scale default `125%`;
- Menu/Text Scale default `100%`.

The matching uploaded screenshot is:

- `Screenshot 2026-10-01 162717.png`

The matching uploaded consolidated log is the 2026-10-01 16:26 session.

Observed failures:

1. The synthetic `Display Options` row appeared in the in-level pause menu but activating/clicking it did nothing.
2. The colored gameplay gauge bars were still detached from their small holders.
3. Alt+Tab caused the game to become effectively non-responsive and it later exited/crashed.

### Important correction: the first health-fill RE target was not the live HUD path

The `a8b3394` hooks all installed successfully:

- three direct QPoly calls inside `Panel_DisplayHealthBar @ 0x00464270`;
- two direct flat-shaded calls inside that function.

However, the full matching runtime log contains **zero** `gameplay_ui_fill_scale` samples from those wrappers.

At the same time, the holder scaling did execute:

- `gameplay_ui_scale source=anim_frame ... density=0.312500,0.416667 user_percent=125 ...`.

Therefore the screenshot's giant live colored gauges are **not** produced by the five `Panel_DisplayHealthBar` call sites targeted in `a8b3394`. Keep those hooks harmlessly scoped, but do not treat them as the solution to the visible player HUD.

This supersedes the earlier inference that the five `Panel_DisplayHealthBar` calls directly explained the screenshot.

### Retail RE: actual live composite panel gauge draws

Further disassembly of the uploaded retail EXE grounded the real live path.

The broad composite panel routine beginning at:

- `0x004658C0`

contains the player HUD/gauge drawing and only calls `Panel_DisplayHealthBar @ 0x00464270` near the end.

Direct QPoly calls in the broad panel routine:

- `0x00465D08 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x00465F46 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x00466176 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x004663AA -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x004665D1 -> PCGfx_DrawQPoly2D @ 0x00507910`
- `0x004667F6 -> PCGfx_DrawQPoly2D @ 0x00507910`

Direct authored-space gouraud gauge calls:

- `0x0046687B -> DCDrawGouraudPoly_0 @ 0x00462FB0`
- `0x00466931 -> DCDrawGouraudPoly_0 @ 0x00462FB0`
- `0x004669C7 -> DCDrawGouraudPoly_0 @ 0x00462FB0`

Direct authored-space flat gauge calls:

- `0x004668D1 -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`
- `0x00466A1B -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`
- `0x00466A65 -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`

The surrounding machine code uses small 512x240-style authored positions/sizes for these gouraud/flat calls, consistent with the visible player gauges.

### Pause-menu failure: confirm was being sampled at the wrong layer

The old pause compatibility patch replaced:

- `0x004415F8 -> CMenu_Update @ 0x00440600`.

The runtime log showed the install succeeded, but there were **zero**:

- `pause_display_options phase=open`;
- `pause_display_options entry_added`;
- `display_pending_ui_scale`

events during the failed in-level activation.

Retail disassembly shows why. The actual pause confirm dispatch occurs *after* `CMenu_Update`:

The exact retail code also verifies that the pause-menu pointer is the global at `0x005FAED0`:

- `0x004415F2 mov ecx,[0x005FAED0]` immediately before `CMenu_Update`;
- `0x00441616 mov eax,[0x005FAED0]` immediately after the confirm trigger, before retail selection/hit dispatch.

Therefore the corrective confirm wrapper's menu lookup uses the same object retail is dispatching; `0x005FAED0` is not an inferred/guessed pointer.

- `0x004415F8 call CMenu_Update`
- pushes `1, 1, 0x100`
- `0x00441606 call PCSHELL_CheckTriggers @ 0x0050C180`

Retail then dispatches the selected pause row by comparing the entry string against its known choices such as Continue / Restart level / Quit.

The synthetic `Display Options` row has no retail branch. Sampling `PCSHELL_CheckTriggers` from inside the CMenu update wrapper was therefore the wrong timing and could miss/consume the edge.

### Corrective implementation

Commit:

- `430461dae00a22d5aef9ada42591ff997fc15827` — **fix: repair pause options HUD gauges and alt-tab**

#### Pause Display Options

The existing row-insertion wrapper remains at:

- `0x004415F8 -> CMenu_Update`

and a new narrow confirm hook is installed at:

- `0x00441606 -> PCSHELL_CheckTriggers @ 0x0050C180`.

Behavior:

- call the real retail trigger function first;
- if the current row is not synthetic Display Options, preserve the original result exactly;
- if the current row *is* Display Options and confirm fires:
  - consume the retail result so its unknown-row dispatcher does nothing;
  - set a deferred-open flag;
  - on the next pause update, open `PCSHELL_DoDisplayOptions @ 0x0050D9B0` outside the trigger call stack.

Expected telemetry:

- `display_menu_patch name=pause_display_confirm installed=1 address=0x00441606 ...`
- `display_menu_mod ... pause_access=1 pause_confirm=1 ...`
- `pause_display_options phase=confirm_intercept ...`
- `pause_display_options phase=open ...`
- `pause_display_options phase=close ...`

#### Live player-gauge scaling

The broad panel QPoly sites listed above are patched to the existing narrow QPoly HUD wrapper.

A new authored-space gouraud wrapper was added for `DCDrawGouraudPoly_0 @ 0x00462FB0`, using the same:

- committed Gameplay UI Scale;
- X/Y density;
- left/center/right and top/center/bottom anchor policy

as the panel holders.

The three broad-panel flat sites use the existing authored-space flat wrapper.

New install telemetry:

- `panel_qpoly=6`
- `panel_gouraud=3`
- `panel_flat=3`

Expected runtime samples now include:

- `gameplay_ui_fill_scale source=qpoly ...`
- `gameplay_ui_fill_scale source=gouraud ...`
- `gameplay_ui_fill_scale source=flat ...`

Do not call the bar/holder issue fixed until those wrappers execute and a screenshot confirms alignment.

### Alt+Tab evidence and corrective policy

The failed run does **not** show an immediate focus-loss device crash.

At the focus edge:

- DirectInput keyboard/mouse/controller are successfully unacquired;
- the game remains in its render/timing loop for many more seconds;
- there is no matching foreground reacquire before termination;
- the session eventually ends with exit code `-805306369` / unsigned `0xCFFFFFFF`;
- no explicit crash or DX-error diagnostic identifies a faulting call.

This matches the user's description that the game appears to stop responding after Alt+Tab rather than failing instantly at unacquire.

The previous foreground-sync path returned immediately on every frame while backgrounded and did not service the game window's messages.

`430461d` adds a background-only message pump:

- up to 64 window-targeted `PeekMessageA(... PM_REMOVE)` messages per input poll;
- `TranslateMessage` + `DispatchMessageA`;
- only for the game's HWND;
- therefore thread-level `WM_QUIT` is not intentionally consumed.

Expected telemetry when messages are processed:

- `[INPUT] background_message_pump calls=... pumped=... total_messages=...`

This is a grounded first fix for the non-responsive window symptom, but it is not yet runtime validated. If the process still exits with `0xCFFFFFFF`, the next step is to add focused WndProc/background-state termination telemetry rather than assuming a renderer/device-loss fault.

### Static validation of 430461d

After commit:

- `main.cpp` lexical state ends in normal code;
- braces, parentheses, and brackets balance;
- new helpers carry `// @Ok`;
- Display menu install format/argument counts were audited;
- gameplay HUD install format/argument counts were audited;
- corrective changes are limited to `main.cpp`.

No GitHub Actions run is available for connector-written commits, so this remains a runtime-test frontier.

### Mandatory next runtime test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

At 2560x1440, test these in order:

1. **Boot/main-menu regression guard**
   - game reaches menu normally.
2. **Gameplay HUD**
   - verify horizontal and vertical colored bars are now inside/aligned with their holders;
   - take one screenshot.
3. **Pause -> Display Options**
   - select/click Display Options;
   - verify the Display menu actually opens;
   - verify Gameplay UI Scale and Menu/Text Scale rows exist.
4. **Live Apply**
   - change Gameplay UI Scale, press Apply, return to gameplay;
   - confirm HUD changes without restart/reload;
   - change Menu/Text Scale and confirm the same live behavior.
5. **Alt+Tab**
   - while in a level, Alt+Tab out for several seconds;
   - return to the game;
   - verify the window responds and input reacquires.
6. Exit normally if possible.

Provide screenshots if anything is still visually wrong. Use the single consolidated `spidey-decomp.log`; the assistant should inspect it for:

- `pause_confirm=1`;
- `phase=confirm_intercept/open/close`;
- `panel_qpoly=6 panel_gouraud=3 panel_flat=3`;
- live `gameplay_ui_fill_scale` samples;
- `background_message_pump`;
- a post-Alt+Tab `foreground_acquire`.


## Matching-build VC6 compile fix (2026-10-01)

The first test attempt at revision `72c838e99458d8f8e3071548fbf8706ae29dd2af` did not reach runtime because the forced clean **matching build failed** in `main.cpp`.

The matching compiler reported:

- `main.cpp(5931): error C2374: 'i' : redefinition; multiple initialization`
- previous declaration at `main.cpp(5911)`
- `main.cpp(5951): error C2374: 'i' : redefinition; multiple initialization`
- previous declaration at `main.cpp(5911)`

Cause: the matching toolchain is MSVC 6-era and uses the old/non-standard for-loop variable scope, so three consecutive `for (int i = ...)` loops inside `SpideyInstallGameplayUiScaleCompat()` collide in the same function scope.

Fix commit:

- `37acd2c6bb31454217144e2cd3ec778543b81f9c` — **build: fix VC6 loop variable scoping**

The three loops now use unique identifiers:

- `qpolyIndex`
- `gouraudIndex`
- `flatIndex`

A follow-up source scan over all newly added compatibility regions found no other repeated same-name `for (int ...)` declarations in the same new function bodies.

This was a compile-only failure; none of the `430461d` pause/HUD/Alt+Tab runtime fixes have been tested yet.

### Next action

Run `UPDATE_AND_TEST_LATEST_BUILD.bat` again. If the matching build succeeds, continue with the existing mandatory runtime test for:

1. gameplay gauge/holder alignment;
2. Pause -> Display Options activation;
3. live Gameplay UI Scale + Menu/Text Scale Apply behavior;
4. Alt+Tab out and back into a running level.


## e5afb28 runtime: in-level retail Display Options crash + HUD double-scale proof (2026-10-01)

### Runtime evidence

The user tested revision:

- `e5afb28dedf2ba978aba0d9c12326cac22ed78f2`

with the installed retail executable:

- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- PE timestamp `0x3B7A3167`
- image size `0x02A0D000`

The matching consolidated log is the user-uploaded `spidey-decomp(1).log`, started 2026-10-01 17:32:05 local.

User observations:
- selecting the synthetic in-level `Display Options` row crashed;
- the gameplay HUD became even smaller;
- colored bars were still not visually inside their holders;
- no screenshot was available for this run, so the next build must provide enough coordinate telemetry to diagnose without one.

### In-level Display Options crash is now explained

The pause-confirm interception itself worked:

- `pause_display_options phase=confirm_intercept line=4 rows=5 mask=0x00000100`
- `pause_display_options phase=open gameplay_ui=125 text=100`
- `display_pending_reset reason=menu_open ...`

The process then immediately raised `0xC0000005`.

The grounded game-side fault is:

- `EIP = 0x0048DABF`
- read target `0x00000004`
- module `SpideyPC.exe`

Retail disassembly of `Shell_DrawBackground @ 0x0048DA90` shows:

- it reads the frontend background object from `0x006A7780`;
- at `0x0048DABF` it dereferences `[eax+4]`.

During gameplay that frontend object is null, so directly invoking `PCSHELL_DoDisplayOptions @ 0x0050D9B0` from the pause loop enters frontend/Shell drawing with uninitialized frontend resources and crashes.

This is an architectural dead end:

**Do not call retail `PCSHELL_DoDisplayOptions` from gameplay again.**

The normal title/frontend Display Options screen remains valid. Only the in-level route is replaced.

### HUD log proves the six broad-panel QPolys were being scaled twice

At 2560x1440 and Gameplay UI Scale 125%, the panel-holder wrapper logged:

- authored holder before:
  `54,42 -> 86,58`
- compact authored holder after:
  `17,18 -> 27,24`

Retail panel rendering later expands authored panel space by:

- X: logical width / 512 = `2560 / 512 = 5`
- Y: logical height / 240 = `1440 / 240 = 6`

Therefore that holder's live-screen bounds are:

- `17*5,18*6 -> 27*5,24*6`
- `85,108 -> 135,144`

The immediately following broad-panel QPoly arrived at our wrapper with **exactly**:

- `85,108 -> 135,144`

The same exact relationship repeats for the subsequent HUD elements:
- compact holder `5,5 -> 19,18` projects to `25,30 -> 95,108`; QPoly before = `25,30 -> 95,108`;
- compact holder `18,9 -> 26,15` projects to `90,54 -> 130,90`; QPoly before = `90,54 -> 130,90`.

Therefore those six `Panel_Display/sub_4658C0` QPolys are **already in post-holder live-screen space** when they reach `PCGfx_DrawQPoly2D`.

The previous wrapper then multiplied them by the UI density again, e.g.:

- `85,108 -> 135,144`
- became `26.56,45 -> 42.19,60`.

That is the grounded reason the latest build visibly shrank the HUD even further.

### Corrected gameplay-QPoly policy

Commit:

- `021f1f602cee997cb92e125cfacb36b03d741b5c` — **fix: use pause-native UI controls and stop QPoly double scaling**

The six live broad-panel QPoly sites:

- `0x00465D08`
- `0x00465F46`
- `0x00466176`
- `0x004663AA`
- `0x004665D1`
- `0x004667F6`

now use a **passthrough** wrapper.

They are no longer compacted a second time.

New telemetry:

- `gameplay_ui_alignment source=panel_qpoly policy=passthrough seq=... coords=...`

The holder telemetry now also emits:

- `live_after=left,top,right,bottom`

so the next log can numerically compare holder live bounds against QPoly bounds without requiring a screenshot.

The three separate `Panel_DisplayHealthBar` QPoly sites remain isolated behind their prior wrapper because those calls did not execute in the earlier player-HUD session and are a distinct path.

### Remaining Gouraud/flat gauge path: measure before deciding

The latest successful runtime also proves these broad-panel calls execute:

Gouraud examples:
- `88,25,30,6 -> 28,10,9,3`
- `58,25,31,6 -> 18,10,10,3`

Flat example:
- `31,41,11,26 -> 10,17,3,11`

Unlike the six QPoly calls, the old log did not include enough final/live-coordinate evidence to prove whether these were aligned or double-scaled.

Do **not** guess.

New diagnostic commits:

- `2e0e5687253014d466268eb207e47907d612f678` — **diag: log projected HUD fill bounds**
- `bf5544fc221245c5692c218bfbe63e92d9c84007` — **diag: trace in-level pause scale controls**

The next log will identify the three Gouraud and three broad-panel flat call streams separately and report:

- authored `before`;
- compacted `after`;
- projected `live_after`;
- stable sequence index `seq=0..2`.

Telemetry:
- `gameplay_ui_alignment source=panel_gouraud seq=... live_after=...`
- `gameplay_ui_alignment source=panel_flat seq=... live_after=...`

The holder sample budget is increased to 48 so enough holder/live-bound pairings survive in the same run.

### In-level scale controls no longer use frontend/PShell state

The unsafe gameplay-to-`PCSHELL_DoDisplayOptions` bridge has been removed completely.

In-level pause now stays inside the existing gameplay pause `CMenu`.

Three synthetic rows are appended directly:

- `Gameplay UI Scale: N%`
- `Menu/Text Scale: N%`
- `Apply UI Scale`

The existing title/frontend Display Options screen still contains its own two scale sliders and Apply row.

Pause hooks:

- `0x00440CAC -> CMenu::Display @ 0x004401B0` — pause-only display wrapper for the two sliders;
- `0x004415F8 -> CMenu_Update @ 0x00440600` — row insertion + left/right/mouse adjustment;
- `0x00441606 -> PCSHELL_CheckTriggers @ 0x0050C180` — consumes confirm only for the synthetic rows and commits on `Apply UI Scale`.

Apply behavior:
- commits Gameplay UI Scale and Menu/Text Scale;
- persists `spidey-modern-video.ini`;
- no renderer/DirectDraw/DXGI rebuild;
- gameplay geometry wrappers read the committed gameplay scale on subsequent draws;
- message text scaling reads the committed text scale on subsequent `Mess_SetScale` calls.

A follow-up guard commit:

- `2e0d41555264938714256bec926fe6e501cd8a06` — **fix: guard pause slider rendering**

ensures the pause-display wrapper never tries to draw the synthetic sliders until those rows are actually present.

### Pause diagnostics for the next run

The first 12 inline-pause display frames emit phase markers:

- `pause_ui_display phase=begin ...`
- `pause_ui_display phase=before_gameplay_slider ...`
- `pause_ui_display phase=after_gameplay_slider ...`
- `pause_ui_display phase=before_text_slider ...`
- `pause_ui_display phase=after_text_slider ...`
- `pause_ui_display phase=end ...`

If the generic retail slider renderer is unsafe in the gameplay pause context, the last emitted phase identifies the exact failing call.

Control telemetry:
- `pause_ui_controls rows_added=3 ... retail_display_options_disabled=1`
- `pause_ui_adjust kind=gameplay_ui ...`
- `pause_ui_adjust kind=menu_text ...`
- `pause_ui_apply old_gameplay=... new_gameplay=... old_text=... new_text=...`
- `pause_ui_confirm action=apply ...`

### Static validation of the current frontier

After `bf5544f`:

- unsafe `SpideyOpenDisplayOptionsFromPause` is absent;
- no direct gameplay call to `PCSHELL_DoDisplayOptions` remains;
- old synthetic `Display Options` pause label is absent;
- pause-native Apply label is present;
- QPoly passthrough policy is present;
- holder, QPoly, Gouraud, and flat alignment telemetry is present;
- new helpers each have exactly one `// @Ok` validator annotation;
- source lexical state ends in code;
- braces, brackets, and parentheses balance;
- VC6 loop-scope audit is clean for the new code;
- installer loops retain distinct `qpolyIndex`, `gouraudIndex`, and `flatIndex`.

Connector-written commits do not currently trigger the repository Actions workflow, so this frontier is **not** claimed as matching-build or runtime proven until the user runs the updater/test script.

### Mandatory next test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

At 2560x1440:

1. Confirm the matching build succeeds.
2. Enter gameplay and spend several seconds with the HUD visible.
   - A screenshot is welcome but no longer required for coordinate diagnosis.
3. Pause.
   - There should be **no in-level Display Options submenu** now.
   - The pause menu itself should contain Gameplay UI Scale, Menu/Text Scale, and Apply UI Scale.
4. Move Gameplay UI Scale left/right.
   - confirm its displayed percentage changes.
5. Move Menu/Text Scale left/right.
6. Select Apply UI Scale.
   - no crash;
   - no level reload/restart;
   - return to gameplay and verify the new HUD size applies.
7. If stable, test Alt+Tab out and back once.
8. Exit normally if possible.

For the next log, inspect:
- `gameplay_ui_scale ... live_after=...`
- `gameplay_ui_alignment source=panel_qpoly ...`
- `gameplay_ui_alignment source=panel_gouraud ... live_after=...`
- `gameplay_ui_alignment source=panel_flat ... live_after=...`
- all `pause_ui_*` events;
- any crash record;
- Alt+Tab `background_message_pump` and post-return `foreground_acquire`.

Do not make another visual HUD transform until the new holder/fill live bounds are compared numerically.


## 21774ea runtime: pause slider-resource crash; health fixed, cartridge/compass still unanchored (2026-10-02)

### Runtime tested revision

The user tested:

- `21774eae1695fa6541453c0c87ccdd579457e34f`

with the same grounded retail executable fingerprint:

- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- PE timestamp `0x3B7A3167`
- image size `0x02A0D000`

Runtime observations:

- the health bar/fill is now correctly inside its holder;
- the cartridge-count number is no longer spatially attached to the scaled top-left HUD group;
- the compass arrow is not inside the scaled bottom-right compass holder;
- pressing Pause crashes immediately as the in-level scale controls begin drawing.

### Health/QPoly correction is runtime confirmed

At 2560x1440, Gameplay UI Scale 125%, the log shows exact holder/fill agreement after the QPoly passthrough correction.

Example:

- holder authored: `54,42 -> 86,58`
- holder compact authored: `17,18 -> 27,24`
- holder projected live: `85,108 -> 135,144`
- adjacent panel QPoly arrives as exactly `85,108 -> 135,144`

The same exact relationships repeat across the six broad-panel QPoly elements. The user's visual confirmation that the health bar is now inside its holder therefore confirms the passthrough policy and it must not be reverted.

### Pause crash is the retail slider animation, not CMenu row insertion

The final runtime sequence is:

- `pause_ui_controls rows_added=3 rows=7 y=85 ...`
- `pause_ui_display phase=begin ... selected=Continue ...`
- `pause_ui_display phase=before_gameplay_slider ... y=133 value=128`
- immediate `0xC0000005`
- fault EIP `0x00462B3B`
- read target `0x0000001C`

Static RE of the original retail functions proves the chain:

1. pause graphical wrapper called retail slider draw at `0x00498060`;
2. slider draw calls `Spool_FindAnim @ 0x004CAB50`;
3. in gameplay the frontend slider animation is not loaded, so the returned frame pointer is null;
4. slider code advances that null base to `0x18`;
5. it calls `Panel_DrawTexturedPoly_1 @ 0x00462B30`;
6. `0x00462B3B` executes `mov ecx,[esi+4]`;
7. with `esi=0x18`, the read target is exactly `0x1C`, matching the runtime crash.

Therefore frontend graphical slider resources must not be used in the gameplay pause menu.

### Resource-free in-level UI controls

Commits:

- `e89ca49fec856f00ee60a3fa4165e3200d4ee5c2` — **fix: use resource-free pause UI scale controls**
- `ed4f6736b4d0a514989c711c82003fa3d023630e` — **fix: avoid font escape glyphs in pause scale bars**

The custom pause `CMenu::Display` hook at `0x00440CAC` is removed entirely.

There is now no call to:
- retail graphical slider draw `0x00498060`;
- pause graphical slider mouse handler;
- frontend-only slider animation resources.

The three pause rows remain:

- `UI Scale =====----- 125%` (example)
- `Menu Text ===------- 100%` (example)
- `Apply UI Scale`

The bars are plain text. `[` and `]` were deliberately avoided because Spider-Man's font parser treats brackets as escape/control characters.

Controls:
- left/right changes the selected scale in 5% steps;
- Apply commits both values live and persists `spidey-modern-video.ini`;
- no renderer/device rebuild;
- no level reload;
- normal frontend Display Options retains its graphical sliders.

### Cartridge count: targeted HUD anchoring

Static RE of `sub_4658C0` proves the cartridge-count text path:

- `Mess_SetScale @ 0x00458620` called at `0x00465A36`;
- `Mess_DrawText @ 0x00458700` called at **`0x00465A83`**;
- the draw uses fixed virtual HUD coordinates (first coordinate `0x5F` / 95, second based on the panel's vertical HUD state), independent of the holder poly transformed by `Panel_SetStretchedScreenCoords`.

That explains why the count did not stay attached after only the holder/graphic transforms were compacted.

Commit:

- `20f02f7d8dfdbd71224f8bf52631aaf9e7e87a9b` — **fix: anchor cartridge count and compass geometry to scaled HUD**

The callsite `0x00465A83` now uses `SpideyCompatCartridgeCountText`.

Policy:
- transform cartridge text x/y in the same top-left compact coordinate policy as the gameplay HUD;
- temporarily render the cartridge digits at a resolution-aware font scale derived from **Gameplay UI Scale**, not Menu Text Scale;
- restore the previous text scale immediately after the one cartridge draw, so normal menu/mission typography remains independent.

New telemetry:

`gameplay_ui_alignment source=cartridge_text policy=top_left_compact before=... after=... density=... requested_scale=... hud_scale=... saved_text_scale=... gameplay_percent=... text_percent=... text=...`

This provides exact source and corrected coordinates on the next run.

### Compass arrow: targeted bottom-right geometry anchoring

The existing holder telemetry proves the compass-frame group is compacted to:

- authored before: `406,183 -> 457,223`
- authored after: `479,216 -> 495,233`
- projected live bounds: **`2395,1296 -> 2475,1398`** at 2560x1440 / 125%.

Static RE of `Panel_DisplayCompass @ 0x00463860` shows three direct dynamic QPoly draws that do **not** pass through the holder's `Panel_SetStretchedScreenCoords` transform:

- `0x00463D19 -> PCGfx_DrawQPoly2D`
- `0x00464035 -> PCGfx_DrawQPoly2D`
- `0x00464257 -> PCGfx_DrawQPoly2D`

Those manually generated compass polygons are the missing coordinate path. They are now routed through `SpideyCompatCompassQPoly2D`, which applies the same gameplay density transform around the **bottom-right logical-screen anchor** used by the compact compass holder.

New telemetry:

`gameplay_ui_alignment source=compass_qpoly seq=0..2 policy=bottom_right_compact logical=... density=... before=... after=...`

This lets the next log prove whether the dynamic arrow geometry is within the holder's `2395,1296 -> 2475,1398` live region.

### Static validation after ed4f673

Current source checks:

- lexical parser ends in code;
- braces, parentheses and brackets balance;
- no `SpideyPauseMenuDisplay` remains;
- no pause call to `0x00498060`;
- no pause slider-mouse call remains;
- pause controls use resource-free text rows;
- new cartridge callsite `0x00465A83` is installed;
- all three compass QPoly callsites are installed;
- each new helper has exactly one `// @Ok` annotation;
- new loops use VC6-safe declaration/scope style;
- existing three panel installer loops remain distinct (`qpolyIndex`, `gouraudIndex`, `flatIndex`).

This is source/static validated, **not yet matching-build or runtime proven**.

### Mandatory next test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Then:

1. Enter the same gameplay scene and leave the HUD visible for several seconds.
2. Check the top-left HUD:
   - health fill should remain in its holder;
   - cartridge-count number should now move/scale with that HUD cluster.
3. Check the bottom-right compass:
   - arrow/dynamic compass geometry should now be inside the compact holder.
4. Press Pause.
   - pause must open without crashing;
   - the two resource-free text scale rows and Apply row should be visible.
5. Select UI Scale and use left/right at least once.
6. Select Menu Text and use left/right at least once.
7. Choose Apply UI Scale.
8. Return to gameplay.
   - confirm the HUD change applies immediately without restart/reload.
9. If stable, Alt+Tab out/back once.
10. Send the consolidated log and a screenshot if convenient.

On the next log inspect:
- `gameplay_ui_alignment source=cartridge_text`;
- `gameplay_ui_alignment source=compass_qpoly`;
- holder `gameplay_ui_scale ... live_after=`;
- `pause_ui_controls`;
- `pause_ui_adjust`;
- `pause_ui_apply`;
- any crash lines.

Do not change the proven broad-panel QPoly passthrough policy unless new runtime evidence contradicts it.


## 0876ea5 runtime: scale inputs detected but never committed; compass double-transform isolated; custom pause Options submenu (2026-10-03)

### Runtime tested revision

The user tested:

- `0876ea54ecd535bcfaa19e77742040bbc69f9b36`

with the same grounded retail executable fingerprint:

- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- PE timestamp `0x3B7A3167`
- image size `0x02A0D000`

User observations:
- health and web-cartridge HUD pieces are now in the correct place;
- the compass arrow is still outside its holder;
- changing UI Scale or Text Scale appears to do nothing;
- the user wants a dedicated in-level **Options** submenu, not the retail frontend Options menu.

### Scale-control runtime proof: adjustment worked; Apply did not

The log proves both scale rows were receiving left/right input:

- Gameplay UI pending value moved through values including 120, 125, 130, 135 and eventually up to 200;
- Text Scale pending value moved through values including 95, 100, 105, 110, 115, 120, 125, 130, 135 and 140;
- throughout those events the committed values remained Gameplay UI = 125 and Text = 100.

There is no `pause_ui_apply` event in the run.

Therefore the old problem was **not** the left/right adjustment path. The problem was that the old confirm wrapper looked up the pause menu through stale/global state and never committed the pending values.

The new handler now captures the actual CMenu pointer from the hooked pause-menu update and uses that same owner for confirm/apply dispatch. It no longer reads `*(CMenu**)0x005FAED0`.

### Custom in-level Options submenu

Commit:

- `d45947c0241e9f226f0a8dd326f2a23fa113194c` — **feat: add standalone pause Options submenu**

Follow-up repair:

- `018b16c3ecdd1f8ad8746a0d75e4013a43152f8a` — **fix: repair atomic pause options handler region**

Current pause behavior:

Parent pause menu:
- all original retail rows remain;
- exactly one new row is appended: **Options**.

Selecting **Options** opens a custom submenu built entirely from the already-live gameplay `CMenu`; it does **not** call `Shell_Options`, `PCSHELL_DoDisplayOptions`, or any frontend-only PShell resource.

Custom submenu rows:

1. `UI Scale: N%`
2. `Text Scale: N%`
3. `Apply Settings`
4. `Back`

Behavior:
- UI Scale changes only the pending gameplay-HUD value in 5% steps;
- Text Scale changes only the pending text value in 5% steps;
- Apply Settings commits both values and persists `spidey-modern-video.ini`;
- gameplay scale takes effect on subsequent HUD draws;
- text scale is explicitly re-applied immediately through `SpideyApplyFrontendTextScale("pause_options_apply")`;
- Back restores the exact parent pause-menu state;
- Back without Apply discards uncommitted changes;
- Apply followed by further edits then Back preserves the applied values and discards only the later pending edits.

The existing title/frontend Display Options menu remains independent and unchanged.

### Parent-menu preservation / object safety

The submenu reuses the same live pause `CMenu` object.

The project validates:

- `sizeof(CMenu) == 0x53C`;
- vtable at offset `+0x0`;
- expanding-box pointer at offset `+0x4`;
- menu data begins at `+0x8`.

The custom submenu snapshots only bytes `+0x8 .. +0x53B`. It deliberately never copies or restores the live vtable or box pointer.

Hardening commit:

- `47129a8ef6d9de995206b61e9ea04f3dcef79b98` — **guard: lock pause Options snapshot to CMenu layout**

This adds a C++98/MSVC6-compatible compile-time size guard:

`sizeof(CMenu) == 0x53C`

and sizes the snapshot as:

`sizeof(CMenu) - 8`.

A future layout drift now fails the build instead of silently corrupting memory.

### Pause Options edge-case simulation

The custom submenu state machine was simulated before requesting another runtime test.

PASS cases:

1. normal parent -> Options -> adjust UI/Text -> Apply -> Back;
2. Back without Apply:
   - pending changes are discarded;
   - committed settings remain unchanged;
3. min/max bounds:
   - 50% cannot decrement further;
   - 200% cannot increment further;
4. Apply with no changes:
   - safe no-op;
5. menu pointer changes while Options is active:
   - submenu ownership is abandoned safely;
   - pending values reset to committed;
6. retail rebuilds the same menu object while Options is active:
   - missing submenu shape is detected;
   - stale state is abandoned safely;
7. parent menu already has 40 rows:
   - Options insertion is refused instead of overflowing the fixed 40-entry array;
8. Apply followed by more pending edits then Back:
   - already-applied values remain committed;
   - post-Apply edits are discarded.

Runtime telemetry for these paths:

- `pause_options_entry ...`
- `pause_options_pending_reset ...`
- `pause_options_state action=enter ...`
- `pause_options_adjust ...`
- `pause_options_confirm action=apply ...`
- `pause_ui_apply ...`
- `pause_options_state action=restore ...`
- `pause_options_state action=abandon ...`

### Compass runtime proof: only one of the three QPolys is the arrow path needing transform

At 2560x1440 / Gameplay UI 125%, the compact compass holder is:

- authored: `406,183 -> 457,223`
- live compact bounds: approximately `2395,1296 -> 2475,1398`.

The three prior compass QPoly hooks produced:

1. `0x00463D19` irregular dynamic polygon:
   - before: `2185,1170 / 2135,1188 / 2055,1152 / 2130,1224`
   - after compact transform: `2442.81,1327.50 / 2427.19,1335 / 2402.19,1320 / 2425.63,1350`
   - **all four transformed points are inside the compact holder bounds**.

2. `0x00464035` rectangle:
   - before is already in live HUD space;
   - applying the compact transform again pushes it to approximately `2521..2546 / 1380..1423`, outside the holder.

3. `0x00464257` rectangle:
   - likewise already in live HUD space;
   - the second transform pushes it to approximately `2496..2521 / 1380..1423`.

Therefore the last build was transforming two already-live compass polygons twice.

Commit:

- `5f309f4709902995a1648882a1b5b8ffa2a30f8b` — **fix: transform only dynamic compass arrow geometry**

Current compass policy:

- hook and compact only `0x00463D19`;
- leave `0x00464035` and `0x00464257` on the retail live-space path;
- new telemetry names the transformed call explicitly:
  `gameplay_ui_alignment source=compass_arrow_qpoly ... call=0x00463D19 ...`.

A geometry simulation across UI Scale values 50%, 100%, 125%, 150% and 200% confirms all four arrow vertices remain inside the correspondingly scaled holder at every tested value.

### Static validation at current frontier

Current `dev` source has been re-audited after `47129a8`:

- lexical state ends in normal C++ code;
- braces, parentheses and brackets balance;
- each new Options helper has exactly one `// @Ok` annotation;
- confirm dispatch uses `gSpideyPauseMenuOwner`;
- stale `*(CMenu**)0x005FAED0` lookup is absent;
- custom Options menu is installed;
- retail frontend Options is not invoked from gameplay;
- compile-time `CMenu == 0x53C` guard is present;
- parent snapshot is `sizeof(CMenu) - 8`;
- only compass callsite `0x00463D19` is hooked;
- old compass transform sites `0x00464035` and `0x00464257` are absent from the compatibility installer;
- health/web-cartridge fixes remain untouched.

This frontier is **source/static validated and simulation-tested, but not yet matching-build/runtime proven**.

### Mandatory next runtime test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Then at 2560x1440:

1. enter gameplay and inspect the HUD:
   - health remains correct;
   - web cartridge remains correct;
   - compass arrow should now sit inside its holder;
2. press Pause;
3. confirm the parent pause menu contains one new row:
   - `Options`;
4. select `Options`;
5. confirm the custom submenu contains:
   - `UI Scale: N%`
   - `Text Scale: N%`
   - `Apply Settings`
   - `Back`;
6. change UI Scale to a clearly different value, e.g. 150%;
7. change Text Scale to a clearly different value, e.g. 125%;
8. choose `Apply Settings`;
9. return to gameplay:
   - HUD geometry should change immediately on the next draw;
   - text should reflect the new scale without restart/reload;
10. pause again -> Options:
   - values should still show the applied values;
11. change both values again, but choose `Back` without Apply;
12. reopen Options:
   - values should have reverted to the last applied values;
13. if stable, Alt+Tab out/back once;
14. inspect the consolidated log for all `pause_options_*`, `pause_ui_apply`, `ui_text_scale reason=pause_options_apply`, and `compass_arrow_qpoly` events.

Do not modify the health or web-cartridge paths unless new runtime evidence contradicts their now-confirmed alignment.


## Pause Options runtime success + gameplay UI-scale row fix (2026-10-03)

### Runtime result received

The user tested the custom pause Options frontier from session revision:

- `bfdaf30d9c6eb75983862e168d753b9903fbf8ae`

That session still carried the latest code checkpoint:

- `47129a8ef6d9de995206b61e9ea04f3dcef79b98`

User result:

- the custom in-level **Options** submenu works;
- **Text Scale** works correctly and applies live;
- the **Gameplay UI Scale** control was not exposed/usable;
- once Gameplay UI Scale is fixed, the user wants to move directly to camera work.

The runtime log proves the backend settings/apply path is healthy:

- submenu entered with `rows=4`, Gameplay UI = 125, Text = 100;
- every scale adjustment event in the successful interaction was `kind=menu_text line=1`;
- Text moved through 105 / 110 / 115%;
- Apply completed with:
  - `old_gameplay=125 new_gameplay=125`
  - `old_text=100 new_text=115`
  - settings saved successfully;
- Back restored the parent pause menu cleanly.

Therefore this is not a Gameplay UI scaling-backend failure. The first custom actionable row was placed at submenu entry 0, while the runtime behavior shows the first usable scale row was entry 1.

### Fix implemented

Commits:

- `ba5691d4fb84feeb6334269bb1edce1778c23627` — **fix: expose gameplay UI scale in pause Options**
- `4159190bcaa7341896561f913504cc34f437c987` — **chore: tighten pause Options row fix**

The custom submenu is now five rows:

0. `Options` — disabled heading / sacrificial retail first row
1. `UI Scale: N%`
2. `Text Scale: N%`
3. `Apply Settings`
4. `Back`

Implementation details:

- row 0 is explicitly marked disabled (`what=1`);
- initial selected line is row 1, so the submenu opens directly on Gameplay UI Scale;
- the menu is centered using the original parent Y now that parent/submenu are both five rows;
- submenu-shape validation now requires all five expected entries;
- entry telemetry now records:
  - row count;
  - selected line;
  - cursor line;
  - heading-disabled state;
  - pending Gameplay UI/Text values.

Expected enter telemetry:

`pause_options_state action=enter ... rows=5 line=1 cursor=0 ... heading_disabled=1 ...`

Expected Gameplay UI adjustment telemetry:

`pause_options_adjust kind=gameplay_ui line=1 ... pending_gameplay=...`

Expected Text adjustment telemetry:

`pause_options_adjust kind=menu_text line=2 ... pending_text=...`

Expected Apply telemetry should now normally report `line=3 rows=5`.

### Compass evidence from the same runtime

The current single-hook compass-arrow path continued to emit compact bottom-right geometry. Sample transformed arrow vertices remained inside the previously measured compact holder region at the tested 125% Gameplay UI setting.

No new runtime evidence contradicts the current policy:

- transform only `0x00463D19`;
- keep `0x00464035` and `0x00464257` on their already-live retail paths.

Do not reopen the compass/broad HUD transforms unless the user reports a visual problem.

### Mandatory next runtime test

Run:

`UPDATE_AND_TEST_LATEST_BUILD.bat`

Then perform only the short regression needed to close this frontier:

1. Enter gameplay and Pause -> Options.
2. Confirm the submenu now visibly contains:
   - `UI Scale: N%`
   - `Text Scale: N%`
   - `Apply Settings`
   - `Back`.
3. The highlight should initially be on **UI Scale**.
4. Change UI Scale to a clearly different value, e.g. 150%.
5. Optionally nudge Text Scale once to confirm it still works.
6. Choose Apply Settings.
7. Return to gameplay and confirm the HUD geometry changes immediately.
8. Reopen Options and confirm the applied UI Scale value persisted.
9. Back out and confirm normal pause navigation still works.

Inspect the consolidated log for:

- `pause_options_state action=enter ... rows=5 line=1 ... heading_disabled=1`;
- `pause_options_adjust kind=gameplay_ui line=1`;
- `pause_options_adjust kind=menu_text line=2` if tested;
- `pause_ui_apply old_gameplay=... new_gameplay=...`;
- `pause_options_confirm action=apply line=3 rows=5`;
- any `[CRASH]` lines.

If this passes, checkpoint it immediately and move to the requested **camera implementation**. Do not spend another runtime cycle on already-proven UI paths unless this short test exposes a regression.


### Static validation of the row-0 fix

Post-edit audit of current `main.cpp` passes:

- lexer ends in normal code state;
- braces, parentheses and brackets balance to zero with no negative-depth transition;
- exactly one five-entry pause-submenu initialization loop is present;
- exactly one five-row submenu shape guard is present;
- row 0 is explicitly disabled;
- initial submenu selection is explicitly row 1;
- the old `mNumLines != 4` shape guard is absent;
- Gameplay UI and Text Scale adjustment dispatch remain separate;
- no health/web-cartridge/compass transform code was changed.

No GitHub Actions run was surfaced through the current connector for this contents-API commit, so matching-build status is not being claimed here. The user-side updater/build remains the authoritative compile/runtime check for this final UI-scale regression.


## Fast update-and-test workflow (2026-10-03)

The user requested a materially faster iteration loop for frequent runtime tests.

New launcher:

- `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`

Supporting workflow:

- `tools/FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1`
- `tools/TEST_LATEST_BUILD.ps1 -Fast`

Commits:

- `85094cccb709b96c1daa4f1553f6c902e7cf0b05` — **build: add fast incremental test mode**
- `8741876dd89a7d847fa99bb3ca35f76d7cf711f0` — **build: add fast update-and-test workflow**
- `f3ce676a468e0bbd2dbb478636c5c1e74a20dfa5` — **build: add fast update-and-test launcher**

The normal `UPDATE_AND_TEST_LATEST_BUILD.bat` remains as the conservative full-build fallback.

Fast-path behavior:

1. performs the dev update exactly once;
2. compares the previous and new exact dev SHAs through GitHub;
3. for ordinary changed `.cpp` files:
   - touches only those translation units;
   - also touches `main.cpp` so the new runtime revision is embedded;
   - uses incremental matching `nmake` instead of forced CLEAN;
4. if a proxy header / makefile / resource / build-system file changed:
   - automatically falls back to the forced-clean matching build;
5. skips rebuilding Renderer11 when its source/build inputs did not change and an artifact exists;
6. skips rebuilding Input11 when its source/build inputs did not change and both DLL/probe artifacts exist;
7. still runs the modern-input preflight;
8. still installs the exact artifacts, resets/creates the consolidated log, launches the game, waits for exit, and archives the single `spidey-decomp.log`;
9. if compare information is missing, ambiguous, or too large:
   - automatically uses the normal safe full rebuild for that run.

This specifically removes two major routine costs from the old loop:

- the update check is no longer performed twice;
- the matching VC6 build is no longer forcibly cleaned for normal `.cpp`-only edits;
- modern renderer/input trees are no longer regenerated when unrelated gameplay/UI code changes.

Static PowerShell delimiter audit after creation:

- fast workflow: braces=0, parentheses=0, brackets=0, terminal_state=code;
- updated test workflow: braces=0, parentheses=0, brackets=0, terminal_state=code.

The first user-side execution remains the authoritative Windows/PowerShell + VC6 runtime validation.


Fast-launcher bootstrap hardening:

- `f5bf78d8184ed03c042b5a49c1ab862275ee10dd` — preserve the pre-bootstrap local revision before the old updater fetches the fast helper;
- `27cc5a7f63b332cf1ffc29a8e427170731db2d3c` — consume that preserved revision in the fast PowerShell workflow.

This prevents the first use of the new BAT from incorrectly treating an old built DLL as current after the bootstrap updater advances `LOCAL_DEV_REVISION.txt`.


## Fast update/test launcher self-overwrite fix (2026-10-03)

User's first run of `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` failed after the normal updater refreshed the project from:

- local: `aa7fa6ace33e3784c196a3ed25a31d16f82cd0d2`
- remote: `717049a7ef915330352cf8415bdfd9a68bb2e1f9`

Observed console tail:

`'D_TEST_LATEST_BUILD.ps1" (' is not recognized as an internal or external command`

followed by:

`[ERROR] Fast workflow was not found after updating.`

### Root cause

The BAT was executing from the project root while `UPDATE_SPIDEY_PROJECT.ps1` performed a `robocopy /MIR` refresh of that same project tree.

The update replaced `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` while `cmd.exe` was still reading it. When control returned from the updater, CMD resumed at the old byte offset in the newly replaced BAT and landed in the middle of a command line. That produced the mangled `'D_TEST_LATEST_BUILD.ps1" ('` command.

The fast PowerShell workflow itself **was** present in live `dev`; the apparent "not found" result came from the corrupted BAT control flow.

### Fix

Commit:

- `c81b2073b456bcdd065c0a5215b5df73a8bee34a` — **fix: make fast launcher update-safe**

New launcher architecture:

1. the project-root BAT immediately copies itself to a unique BAT under `%TEMP%`;
2. the project-root BAT calls that temporary copy;
3. the temporary copy receives the original project root explicitly;
4. all update/build/test work runs from the temporary BAT;
5. if the fast PowerShell workflow is not installed yet, the temporary runner invokes `tools\UPDATE_SPIDEY_PROJECT.ps1 -NoPause` directly;
6. after bootstrap/update, it invokes `tools\FAST_UPDATE_AND_TEST_LATEST_BUILD.ps1` from the refreshed project.

The project-root BAT keeps the temporary `CALL` and its immediate `EXIT /B` on the same physical command line so CMD parses the continuation before the project copy can be replaced.

### User recovery

Because the failed bootstrap already successfully refreshed the project to `717049a7...`, the supporting fast PowerShell files should now exist locally.

Replace the old downloaded `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` with the corrected version from commit `c81b2073...` and run it again.

No source/gameplay code changed as part of this launcher repair.


## Fast incremental matching-build relink fix (2026-10-03)

User reran the corrected fast launcher successfully through update/elevation/toolchain setup on revision:

- `b8cc4d5f79c4e2c03a4db2179859b03242d31491`

Observed behavior:

- fast mode correctly selected an incremental matching build;
- `nmake` recompiled `main.cpp`;
- the VC6-generated NMAKE project returned success without regenerating `Release\spider.dll`;
- the existing freshness guard caught this and aborted before installation:
  - stale DLL timestamp: `2026-10-04T01:30:09.4315047Z`
  - build start: `2026-10-04T02:01:29.7768220Z`
- Renderer11 and Input11 both rebuilt successfully during that run and their input preflight passed.

This is a safe failure: no stale proxy was installed or launched.

### Cause / compatibility behavior

The preserved Visual Studio 6 / NMAKE dependency graph can rebuild an object on the first incremental invocation without relinking the parent DLL target in that same pass. The generated `spider.mak` does declare `Release\spider.dll` as dependent on `LINK32_OBJS`, including `Release\main.obj`, but this runtime result proves one NMAKE invocation is not sufficient for our touched-source fast path.

### Fix

Commits:

- `0a976aceb29068e2414c9aa60a52d8d60720fe76` — **fix: relink proxy after incremental object rebuild**
- `1ede4e7c43fa51d243f06acfddfcab8c6e738ffc` — **fix: preserve same-revision fast reuse**

Fast matching-build behavior is now:

1. run the normal incremental NMAKE pass;
2. if the DLL is still older than the build start, run a second cheap NMAKE pass;
3. the second pass sees the freshly rebuilt object(s) and should relink `spider.dll`;
4. if the DLL is somehow still stale, automatically fall back to one forced-clean matching build;
5. if the dev revision did not change and reuse is explicitly allowed, skip this relink/fallback logic so a true no-change run stays fast.

The existing final freshness check remains in place, so stale DLLs still cannot be installed.

### Next user action

Run the same corrected `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` again. It will update from `b8cc4d5...` to the relink-fixed `dev` revision. Since Renderer11/Input11 were successfully rebuilt during the failed run and their sources did not change, the next fast run should reuse those bridge artifacts.


## Pause Options final interaction polish (2026-10-03)

Runtime test on revision `e5ca5a25fc6b0d6ccdbb85b446b18b03c0a825c0` confirmed the custom pause Options submenu is now functionally usable:

- all expected rows appeared;
- UI Scale adjustments worked;
- Text Scale adjustments worked;
- Apply Settings committed both values;
- values persisted;
- no crash occurred.

The uploaded consolidated runtime log also confirms the canonical retail executable fingerprint:

- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- PE timestamp `0x3B7A3167`
- image size `0x02A0D000`

Observed successful custom confirmations were still coming through runtime mask `0x00000100`, matching the mouse-oriented pause trigger path. The user reported that Enter could not open Options or activate Apply Settings.

The retail keyboard mapping table in `PCInput.cpp` maps action `0x00001000` to DIK `0x1C` (Enter) by default. Therefore the custom pause confirmation wrapper now probes retail confirm action `0x00001000` only while a custom Options row owns the selection. Normal retail pause rows are left untouched.

User also requested that `Quit` remain the bottom-most parent pause row. Previously our `Options` row was appended after the retail final row.

Implemented:

- `9581ef731b21568da55a99cd49eb7de4cf593e7c` — **fix: keyboard confirm and pause Options order**
  - custom pause confirmation now accepts native retail action `0x00001000` in addition to the already-working patched-call trigger;
  - Open, Apply Settings, Back and confirm-consumption for scale rows all flow through the same custom handler;
  - confirmation telemetry records `source=confirm_action_0x1000` when native confirm is what triggered the action;
  - parent Options insertion now appends once through retail `CMenu::AddEntry`, then swaps the complete new `SEntry` with the previous final retail row;
  - the previous final row therefore remains bottom-most;
  - if the previous final row was selected during insertion, selection follows it to its new index so insertion does not unexpectedly jump the highlight.
- `648b293e3590c1346decfab8028a6b64c2e4b479` — **chore: refresh pause Options telemetry**
  - startup telemetry now states `pause_parent_insert=before_last`;
  - submenu row count corrected to 5;
  - native confirm action recorded as `0x1000`.

Static validation after the source change:

- lexical delimiter balance: braces 0, parentheses 0, brackets 0;
- custom submenu shape guard remains 5 rows;
- existing mouse confirmation path remains intact;
- normal retail pause rows are not given the synthetic/native confirm probe;
- complete `SEntry` structures are swapped, preserving label, colors, scaling and row flags.

### Exact next runtime test

Use `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Verify only:

1. Pause menu order has `Options` immediately above `Quit`, with `Quit` still last.
2. Highlight `Options` with keyboard navigation and press Enter; submenu should open.
3. Change one scale value.
4. Highlight `Apply Settings` and press Enter; value should apply.
5. Highlight `Back` and press Enter; parent pause menu should restore.
6. Mouse click behavior should still work.

Expected useful telemetry:

- `pause_options_entry ... options_row=... quit_row=... previous_last=...`
- `pause_options_confirm action=open ... source=confirm_action_0x1000`
- `pause_options_confirm action=apply ... source=confirm_action_0x1000`

If this passes, close the pause-options/UI-scale milestone and move directly to the already-instrumented camera work. No additional UI test should be requested unless this specific interaction polish fails.


## Pause Options Enter mask correction (2026-10-04)

The runtime test on revision `b7acebd821d7bbefa95c7e69b2071be68045055e` confirmed:

- parent row ordering fix works: telemetry reports `options_row=3 quit_row=4 previous_last=Quit`;
- mouse click still opens Options and activates custom submenu actions;
- Enter still does not open Options, Apply Settings, or Back;
- no custom confirmation telemetry reported the prior synthetic `source=confirm_action_0x1000` path.

Exact retail disassembly was recovered from the project copy of `SpideyPC.exe` and inspected:

- `PCSHELL_CheckTriggers @ 0x0050C180`;
- mask `0x00000010` takes the direct keyboard path and calls the key-state helper with DIK `0x1C` (Enter);
- mask `0x00000100` takes the mouse-left-button path, matching the working runtime click behavior;
- mask `0x00001000` is a different input path and was incorrectly identified as Enter in the previous patch.

Implemented:

- `85df9590dc42d03145756e05e67f59b44a69f45d` — **fix: use retail Enter mask for pause Options**
  - custom pause confirmation fallback changed from `0x00001000` to the verified retail Enter mask `0x00000010`;
  - source telemetry now reports `source=keyboard_enter_mask_0x10` when Enter activates Open/Apply;
  - startup telemetry now records `pause_keyboard_enter_mask=0x10`;
  - the working mouse mask `0x00000100` path remains unchanged;
  - normal retail pause rows are still untouched by the custom fallback.

Static source validation after the correction:

- braces: 0;
- parentheses: 0;
- brackets: 0;
- no negative delimiter depth;
- stale `0x00001000` custom-confirm probe removed.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Verify:

1. `Options` remains immediately above `Quit`.
2. Highlight `Options` and press Enter.
3. Highlight `Apply Settings` and press Enter.
4. Highlight `Back` and press Enter.
5. Mouse clicks still work.

Expected telemetry on Enter:

- `pause_options_confirm action=open ... source=keyboard_enter_mask_0x10`
- `pause_options_confirm action=apply ... source=keyboard_enter_mask_0x10`

If this passes, close the pause Options/UI milestone and proceed directly to camera implementation.


## Pause Options Enter: raw DirectInput latch path (2026-10-04)

Runtime test on revision `081479c4741962a808bb7e326c3edb3e67685d9a` still showed:

- Options row placement is correct: `options_row=3 quit_row=4 previous_last=Quit`;
- mouse click opens Options successfully;
- keyboard Enter still does not open Options;
- Enter still does not activate Apply Settings or Back.

The uploaded consolidated log also remained on the canonical retail executable fingerprint:

- SHA-256 `D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C`
- PE timestamp `0x3B7A3167`
- image size `0x02A0D000`.

### Important correction to the previous diagnosis

The prior `0x10` Enter code **was present in the tested source**. The startup telemetry string still said `pause_confirm_action=0x1000`, but that string itself had not been refreshed; it was not proof that the functional source patch was missing.

Exact retail disassembly now explains why the `0x10` fallback can still fail:

- pause flow calls retail `CMenu::Update @ 0x00440600` before the patched confirm call at `0x00441606`;
- `PCSHELL_CheckTriggers @ 0x0050C180` has a dedicated Enter one-shot latch at `0x00AC1238`;
- its `0x10` path checks DIK `0x1C`, but the latch can suppress a second/late query in the same frame;
- retail raw keyboard state is stored by DirectInput with:
  - `0xFF` = fresh press;
  - `0x7F` = still held after the next poll;
  - `0x80` = release;
  - `0x00` = idle.

### New implementation

Commits:

- `869237591a593eea214bc90f44b6e7cce42f57d7` — initial native DirectInput edge read at `PCINPUT_IsKeyPressed @ 0x0050A650`.
- `c55f7dbc1a6bf38b76912d8ee44e17fb667e01a6` — **fix: latch raw DirectInput Enter for pause Options**
  - reads retail raw key byte through `DXINPUT_GetKeyState @ 0x00501CB0` for DIK `0x1C`;
  - derives physical down from the low seven bits, accepting both fresh `0xFF` and held `0x7F`;
  - maintains an independent pause-menu latch so one physical press produces one custom activation;
  - therefore remains robust if a second retail keyboard poll converted `0xFF` to `0x7F` before the custom confirm hook;
  - does not use `GetAsyncKeyState`, Windows messages, or any external keyboard side path;
  - preserves the existing mouse confirmation path;
  - adds `pause_enter_state raw=... down=... edge=... held=... line=... selected=...` diagnostics;
  - adds explicit Back confirmation telemetry.
- `8fd5f5309facc7faf88fd53c4d3c05abb7609dbc` — startup telemetry now reports `pause_keyboard_source=raw_directinput_dik_0x1c`.

Static source validation after the raw-input patch:

- braces balanced;
- parentheses balanced;
- brackets balanced;
- no negative delimiter depth.

### Exact next runtime test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`.

Before entering gameplay, confirm the log startup includes:

`pause_keyboard_source=raw_directinput_dik_0x1c`

Then test only:

1. highlight Options and press Enter;
2. highlight Apply Settings and press Enter;
3. highlight Back and press Enter;
4. verify mouse clicks still work.

Expected successful keyboard telemetry:

- `pause_enter_state raw=0xFF ... edge=1 ...` or, if an extra poll occurred, `raw=0x7F ... edge=1 ...`;
- `pause_options_confirm action=open ... source=raw_directinput_enter_edge`;
- `pause_options_confirm action=apply ... source=raw_directinput_enter_edge`;
- `pause_options_confirm action=back ... source=raw_directinput_enter_edge`.

If activation still fails, the raw key-state telemetry is sufficient to distinguish “Enter never reaches DirectInput” from “custom routing logic failed” without another speculative mapping change.


## Pause Options milestone closed; modern orbit camera Stage A implemented (2026-10-04)

User runtime confirmation after the raw DirectInput Enter-latch fix:

- Enter now opens the pause-menu Options submenu;
- Enter activates Apply Settings;
- Enter activates Back;
- mouse interaction still works;
- Options remains immediately above Quit.

The pause/options/UI-scale milestone is therefore closed unless a regression is reported.

### New user camera requirement

User requested a modern 3D gameplay camera that can orbit around Spider-Man:

- left/right rotation through a full 360-degree range where ordinary gameplay permits it;
- up/down camera pitch with sensible limits;
- mouse and right-stick control;
- preserve scripted/special camera behavior where practical;
- implement the best useful first version rather than waiting for a perfect full replacement.

### Exact camera RE completed

Canonical SpideyPC.exe disassembly established the mode-3 seam inside CCamera::AI @ 0x00417CB0.

For ordinary mode 3:

- dispatch entry 0x00418412;
- call site 0x00418414;
- retail mode-3 generator target 0x00418E00;
- rejoins the shared retail path at 0x00418456.

After the mode-specific call, retail still executes:

- camera post-processing/collision/orientation at 0x00416B10;
- later shake/orientation handling;
- CCamera::LoadIntoMikeCamera @ 0x00416A20.

Immediately before dispatch, mode 3 derives:

- XZ distance at 0x00548860;
- Y distance at 0x00548864;
- radial distance at 0x0054885C;
- vertical angle at 0x00548858 = ratan2(-Y, XZ).

CM_Normal @ 0x00418E00 consumes those values plus CCamera angle fields around +0x234/+0x236/+0x238 and builds the desired position at +0x24C.

### Stage-A implementation

Commits:

- e15892a8c76eef180a932b25d6bfb13e62795adb — feat: add mode-3 modern orbit camera prototype
- d2d546551255be6d0b83919236d0aed7098dcbd3 — guard: reset modern camera ownership cleanly
- 09f0cd9cb65a65868d97a70f1bce8910118d75ba — docs: document mode-3 orbit prototype

Implementation boundary:

- patches only direct call 0x00418414 -> 0x00418E00;
- replacement wrapper is SpideyModernMode3Camera;
- wrapper calls retail CM_Normal after injecting modern yaw/pitch inputs;
- retail collision/orientation/final-publish stages remain intact;
- all camera modes other than 3 remain retail-owned.

Activation policy:

- if the user never moves mouse/right stick, mode 3 remains functionally retail;
- first camera intent seeds yaw from live camera->field_236 and vertical distance from live retail Y distance;
- modern yaw then wraps through 0..4095 with no horizontal clamp;
- modern pitch is represented through Y distance and the exact retail-derived vertical angle/radius inputs;
- ownership drops on camera pointer change, mode != 3, or camera detach;
- returning from a scripted/special camera requires new input intent and reseeds from the current retail camera.

Initial controls/tuning:

- relative DirectInput mouse X: yaw scale 3;
- relative DirectInput mouse Y: pitch scale 2, mouse-up = look-up;
- Input11 right stick X: 32 angle units/frame at full deflection;
- Input11 right stick Y: 7 Y-distance units/frame at full deflection;
- vertical Y-distance clamp: -480..+260;
- retail XZ distance remains untouched;
- one input snapshot is consumed at most once per completed frame.

Telemetry:

- modern_camera_install installed=1 call=0x00418414 retail_mode3=0x00418E00 ...
- modern_camera event=acquire ...
- modern_camera event=update ... yaw=... retail_yaw=... y_dist=... input ... retail_overrode_yaw=...
- modern_camera event=release ...

retail_overrode_yaw is specifically logged because CM_Normal can conditionally recompute field_236 when auxiliary orientation angles are active; this will tell us whether wall/ceiling/special mode-3 states fight the first free-look layer.

### Static audit

- direct call target is grounded by canonical retail disassembly;
- ABI uses the same VC6-safe __fastcall thiscall shim pattern already used elsewhere in main.cpp;
- braces/parentheses/brackets balance;
- no C++11-only proxy syntax added;
- modern input is consumed once per completed-frame snapshot;
- camera ownership cleanup covers detach/pointer replacement/non-mode-3 transitions;
- Renderer11/Input11 source did not need modification for this first camera prototype.

### Next runtime test

Run FAST_UPDATE_AND_TEST_LATEST_BUILD.bat.

In ordinary gameplay:

1. stand still and move mouse left/right;
2. attempt a broad/full orbit around Spider-Man;
3. move mouse up/down through the available pitch range;
4. stop moving the mouse and verify the chosen view does not instantly snap behind Spider-Man;
5. walk/run/jump and rotate while moving;
6. swing briefly and rotate if stable;
7. approach a wall/corner and observe whether retail collision still prevents catastrophic camera clipping;
8. if a controller is connected, test the right stick in both axes;
9. report whether horizontal/vertical directions or sensitivity feel reversed/too fast/too slow.

If the camera behaves badly, do not immediately replace the whole system. Use modern_camera telemetry first:
- no acquire => input seam problem;
- yaw changes but view does not => CM_Normal/post-processing ownership problem;
- retail_overrode_yaw=1 => auxiliary orientation logic is fighting yaw;
- good floor orbit but bad wall/ceiling => classify those mode-3 sub-states before deciding whether to special-case them or advance to Stage B.

User should provide the single consolidated spidey-decomp.log from the run.


## Orbit camera validated; sensitivity + hitch mitigation + camera web targeting implemented (2026-10-04)

User runtime report on revision 5ec06e1506840c48f97c16380cc4a49f3b65ef8a:
- modern 3D orbit camera worked extremely well;
- no meaningful camera-control defects were reported;
- user requested camera sensitivity in the custom pause Options menu;
- user reported constant frametime hitching plus a larger roughly half-second freeze every several seconds;
- user requested enemy web selection to follow camera facing rather than Spider-Man body facing.

Runtime evidence:
- modern camera hook installed successfully;
- sampled modern camera updates retained the requested yaw with retail_overrode_yaw=0;
- gameplay timing has normal 59-61 Hz windows but intermittent degraded windows in the mid-40 Hz range, consistent with visible stalls.

### Camera sensitivity

New setting:
- Camera Sensitivity
- default 100 percent = exact first-prototype feel;
- range 25..200 percent;
- step 5 percent;
- applies to mouse X/Y and right-stick X/Y;
- persisted in spidey-modern-video.ini under [Controls] CameraSensitivityPercent.

Pause -> Options now contains six rows:
0 Options heading
1 UI Scale
2 Text Scale
3 Camera Sensitivity
4 Apply Settings
5 Back

Commits:
- 2ca650606bf1c851859c699be7e019b131f69e5b
- fdfe39ec6000856c655dbf137bf90c66d9c59aa8
- f6fcaa31edb7ae5cc8ea8cb8df2091ecfd302413
- fa0f7e230c210019d8c6066076585fb84144b8ee
- fde069d43a284c27ca385ef427a6751f3b5bd06f

### Periodic hitch root cause

Two diagnostic readback paths were still active every 120 frames:

1. main.cpp SpideyLogSurfaceState:
   DirectDraw GetDC -> nine GetPixel calls -> ReleaseDC.

2. renderer11 ShadowEndFrame:
   nine CopySubresourceRegion calls into a staging texture -> blocking D3D11_MAP_READ.

These are explicit CPU/GPU synchronization points. At ~60 FPS, 120 frames is approximately two seconds, closely matching the repeating freeze report.

Both are now default OFF:
- 61419a368c7a7e962673a74f7052fe55465254ff — disable periodic DX11 readback
- 6cb305796f272043654ed3819bb92e7c6c53249a — disable periodic DirectDraw pixel sampling

Optional diagnostic re-enable:
- SPIDEY_RENDERER11_DIAG_READBACK=1
- SPIDEY_DIAG_SURFACE_READBACK=1

Do not claim all frametime issues are solved until runtime validation. These are high-confidence causes for the large periodic stall; smaller hitching may still have another cause.

### Camera-forward web targeting RE

Canonical retained-function addresses:
- CheckWebShot 0x004C0510
- SelectAutoAimTarget 0x004C5AA0
- FireWeb 0x004C5DD0
- SelectTargetBaddy 0x004C8410
- SelectTargetSwitch 0x004C8570

SelectAutoAimTarget call site:
- 0x004C5B2F -> SelectTargetBaddy 0x004C8410.

SelectTargetBaddy:
- keeps candidate eligibility/range/LOS in retail code;
- transforms candidate-relative world vectors through player + 0x89C before scoring centeredness;
- therefore retail enemy selection is body-orientation-centered.

The five retained function blobs were verified byte-for-byte against the materialized same-build executable using their Git blob SHA values. The targeting RE therefore matches the repo's canonical retained function bytes despite the separately materialized EXE having a different whole-file SHA.

Implementation:
- 954882bb63a86c39011bbc3996104f905b8559aa — gameplay: aim web auto-targeting from camera.

Behavior:
- patches only 0x004C5B2F;
- other SelectTargetBaddy callers remain unchanged;
- in ordinary camera mode 3, temporarily substitutes a target-scoring orientation built from active camera field_214 through retail QToM @ 0x0047C7F0;
- calls untouched retail SelectTargetBaddy;
- restores player field_89C immediately;
- preserves candidate filtering, score/range rules, LOS and target result behavior;
- non-mode-3 camera falls back to original player-facing targeting.

Telemetry:
- camera_web_target_install ...
- camera_web_target event=select source=render_camera_transform ... target=...

### Static audit

- main.cpp delimiter balance clean: braces/parens/brackets all zero with no negative depth;
- Renderer11 delimiter balance clean;
- custom pause submenu shape is consistently 6; stale 5-row shape guard removed;
- direct camera web patch target is guarded by SpideyPatchDirectCall;
- no nullptr added to VC6 proxy;
- sensitivity uses existing VC6-safe code style;
- Renderer11 readback remains available only through explicit env opt-in.

### Exact next runtime test

Run FAST_UPDATE_AND_TEST_LATEST_BUILD.bat and test all three items in one session:

1. Camera Sensitivity:
   - verify row appears;
   - set 50 percent, Apply, confirm slower camera;
   - optionally set 150 percent, Apply, confirm faster camera;
   - reopen Options and verify persistence.

2. Frametime:
   - play through several old freeze intervals;
   - report whether the large periodic freeze disappeared;
   - separately report whether smaller constant hitching remains.

3. Camera web targeting:
   - face Spider-Man away from an enemy;
   - center that enemy with camera;
   - fire enemy-targeting web;
   - target should follow camera center rather than body heading;
   - test left/right and above/below targets if practical.

4. Brief movement/swing/camera regression check.

Upload only the single consolidated spidey-decomp.log.



### Camera telemetry logging overhead reduced

After the first orbit-camera success, high-frequency input-intent telemetry is no longer necessary.

Commit:
- c71240e223884de59665e63c1b8984b606301845 — perf: throttle validated camera telemetry

Changes:
- modern_camera update sampling: once per 60 completed input snapshots while actively moving instead of once per 10;
- passive input-intent camera_state sampling: once per 60 frames instead of once per 15;
- mode changes, ownership transitions, web-target changes and periodic state still log.

Reason:
- the consolidated log uses synchronous file open/write/close operations;
- reducing validated diagnostic traffic removes unnecessary I/O from the gameplay path while retaining enough evidence for sensitivity/web-target validation.

The uploaded camera test log gives a strong periodic-stall correlation:
- at frame 1200 the old shadow-target and DirectDraw pixel samples both execute;
- the immediately following timing window falls to approximately 31-32 Hz.
This further supports disabling both 120-frame blocking readbacks.



## New-chat handoff package checkpoint — 2026-10-04

A full new-chat handoff is being produced after an interrupted engineering turn.

Verified source frontier before packaging:
- dev: `0c0f22614ca9e6b835e2b524d896659e47f8aec5`
- latest source/perf commit: `c71240e223884de59665e63c1b8984b606301845` — throttle validated camera telemetry
- latest gameplay implementation before that: `954882bb63a86c39011bbc3996104f905b8559aa` — camera-forward web auto-targeting

No source work from the interrupted turn is missing. The combined sensitivity / hitch / camera-web-target runtime remains the exact next step. Do not redo RE before that test.


## Residual frame-pacing audit + low-overhead cadence telemetry (2026-10-04)

Continuation after the full camera/hitch/web-target handoff verified that live `dev` exactly matched handoff HEAD `df1adf4f2664a5183146efd639dac2a65ae5ecf9` before new work began.

### Static renderer audit

The prior runtime log confirms normal gameplay presentation is the authoritative DX11 shadow path:
- `dx11_shadow=1`;
- `dx11_hdc=0`;
- `direct_hwnd=0`.

Therefore the Renderer11 `Flush + GetDC` HDC fallback is not the normal gameplay presentation path and is not a primary residual-stutter suspect.

For sampled gameplay frames from frame 600 onward, the old log shows approximately:
- mean queued DX11 shadow commands: 5,357 per sampled frame;
- mean vertices: 16,299;
- mean vertices per command: ~3.04;
- observed queued range: 3,842..9,120 commands.

This is a real steady CPU-efficiency concern: thousands of tiny Draw calls plus repeated state/hash lookups are being issued every frame. However the sampled low-Hz windows do not show a positive correlation with command count, so do not mislabel raw command volume as the cause of the old intermittent stalls. Treat state-call reduction / safe batching as a later optimization after frame pacing is classified.

The proxy also still performs diagnostic per-vertex screen-range accounting on main-scene D3D7 producer draws. That work is not render-critical and is another lower-risk cleanup candidate, but it is not being changed before the combined baseline runtime.

### Retail multimedia-timer cadence is a strong micro-stutter hypothesis

`PCTimer.cpp` documents the retail timer model:
- `PCTIMER_Init` requests a 16 ms multimedia timer;
- `field_4 = 16`;
- `gTimerMsInterval = field_4 * 60 / 1000 = 0.96`;
- each timer callback accumulates that fractional 60-Hz value and advances `Vblanks` while the accumulated integer is ahead.

If retail `MyVSync` advances `Vblanks` as expected, a simple simulation of that exact 0.96-per-16-ms accumulator produces mostly 16 ms advances with a 32 ms gap roughly every 25 timer ticks (~0.4 s). That is a plausible source for persistent small cadence hitching after the two blocking 120-frame readbacks are gone.

Important confidence boundary:
- `PCTIMER_Init` is documented as reconstructed retail behavior;
- the current source body of `MyVSync` itself is still an incomplete decomp stub;
- therefore this is a strong hypothesis to validate with runtime cadence evidence, not yet a timing replacement to ship blindly.

### Telemetry-only continuation commits

- `08e4acdafaa38b267fcb57862c8e2a5ed8539703` — perf: add low-overhead frame cadence telemetry
- `f69abaf545193bb1441d3421661e3f752daf896d` — compat: keep cadence telemetry VC6-safe

The new instrumentation does not alter renderer, camera, targeting, timer, or gameplay behavior.

It adds only in-memory per-present aggregation:
- QPC frame-interval count;
- intervals over 20 / 25 / 30 / 50 ms;
- maximum interval in microseconds;
- per-present engine `Vblanks` delta counts: same / one / multi;
- maximum observed vblank delta.

Those aggregates are appended to the existing once-per-second `timing_present` line, so no additional per-frame file I/O was introduced.

Static post-edit checks:
- `main.cpp` braces balanced 899/899;
- parentheses balanced 4457/4457;
- brackets balanced 310/310;
- no `1000000LL` suffix remains; multiplication uses an explicit `LONGLONG` cast for the proxy's compatibility constraints.

### Exact next runtime remains one combined test

Run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` and test:
1. Camera Sensitivity at 50 percent, Apply, then optionally 150 percent; verify persistence.
2. Play through several old major-freeze intervals; report whether the large repeating freeze is gone.
3. Separately report whether the smaller constant/micro hitch remains.
4. Face Spider-Man away from an enemy, center the enemy with the camera, fire enemy-targeting web and verify camera-forward selection.
5. Brief movement/swing/camera regression pass.
6. Upload only the single consolidated `spidey-decomp.log`.

Interpretation after that run:
- large freeze gone + `over25ms/over30ms` cadence repeating with characteristic `Vblanks` behavior => prioritize modernizing the retail timer/vblank pacing;
- large freeze gone + cadence telemetry clean but residual hitch visible => profile renderer/producer CPU overhead next;
- web target wrong => use `camera_web_target` evidence before changing matrix axes/signs;
- no source RE from the completed camera/targeting batch should be redone.


## Batch-file cleanup (2026-10-04)

The repository BAT surface was reduced to the workflows that still serve a current purpose.

Removed as obsolete/superseded:
- BUILD_AND_INSTALL.bat
- BUILD_DEV.bat
- INSTALL_DEV_BUILD.bat
- SETUP_FIRST_TIME.bat
- SPIDEY_DEV_MENU.bat
- UPDATE_AND_TEST_LATEST_BUILD.bat
- UPDATE_PROJECT.bat
- scripts/update_project_worker.bat

Remaining BATs:
- FAST_UPDATE_AND_TEST_LATEST_BUILD.bat — authoritative update/build/install/test launcher
- TEST_LATEST_BUILD.bat — non-fast full test/recovery launcher
- UPDATE_SPIDEY_PROJECT.bat — standalone updater/recovery entry point
- GET_SPIDEY_PROJECT.bat — first-machine bootstrap launcher
- RUN_GAME.bat — launch the currently installed build with no update/download/rebuild
- RESTORE_STOCK_GAME.bat — restore preserved retail Bink DLL
- build.bat — low-level matching proxy build used by the PowerShell test workflow
- clean.bat — low-level matching build cleanup helper

RUN_GAME.bat was refreshed to be self-contained:
- it never downloads, updates, rebuilds, or replaces DLLs;
- it uses spidey_local_config.bat when present;
- otherwise it tries the normal Spider-Man install path and prompts once if needed;
- it saves the resolved game path for later launches.

Post-cleanup audit:
- exactly 8 BAT files remain;
- no remaining BAT references any deleted BAT.


## Camera-forward web targeting axis correction (2026-10-04)

Runtime evidence from spidey-decomp(10).log showed that the existing camera-transform hook was active in mode 3 but target selection oscillated between a valid baddy and null while the user could not reliably hit enemies.

Canonical retained-function RE then narrowed the failure to the SelectTargetBaddy scoring transform, not FireWeb:
- SelectAutoAimTarget @ 0x004C5AA0 stores the SelectTargetBaddy result in player+0xDCC.
- FireWeb @ 0x004C5DD0 reads player+0xDCC and directly consumes the selected target when present.
- SelectTargetBaddy @ 0x004C8410 preserves retail eligibility, range weighting and LOS.
- Its directional block transforms player->candidate through player+0x89C, normalizes the result, then uses NEGATED LOCAL Z as the centeredness/forward score.
- The camera matrix produced by the same QToM path used by CCamera::LoadIntoMikeCamera represents visible camera-forward as positive local Z.

Therefore the old raw camera-matrix substitution had an axis-convention mismatch: a centered camera-forward target could be scored as backwards.

Implemented commit:
- 5e3dd2e066de9bd89d94dd675f152baaf85d22e3 — gameplay: align camera web aim with retail forward axis

Implementation:
- continue to patch only the SelectAutoAimTarget -> SelectTargetBaddy call at 0x004C5B2F;
- continue to call untouched retail SelectTargetBaddy;
- continue to restore player+0x89C immediately after the call;
- after QToM(camera->field_214), negate matrix row 2 for the temporary scoring matrix so camera-forward maps to retail's expected negative-local-Z direction;
- no range, eligibility, LOS, FireWeb or player-facing logic was replaced.

Static source delimiter validation after the edit:
- braces 899/899
- parentheses 4457/4457
- brackets 322/322

Runtime validation still required:
- center a baddy with the camera while Spider-Man's body is turned away and fire an enemy-targeting web;
- repeat left/right and above/below if practical;
- verify normal straight-ahead targeting still works;
- upload the single consolidated spidey-decomp.log if anything is wrong.


## Character blob-shadow camera anchoring (2026-10-04)

User runtime screenshot showed standard character floor/blob shadows sliding away from their owning NPC as the camera rotated. This is the normal CBody/CQuadBit shadow path, not the Renderer11 whole-frame "shadow replay" terminology.

Canonical RE:
- CBody::UpdateShadow @ 0x004605A0 creates/updates a CQuadBit using mShadowPos and a fixed downward normal. Its world-space placement/orientation path itself is camera-independent.
- CQuadBit::OrientUsing @ 0x00409400 builds the world-space quad corners from the supplied position/normal and does not consume camera state.
- Bit_Init @ 0x00407FC0 registers QuadBitList with retail DisplayQuadBitList @ 0x004097E0.
- The exact registration immediate is the PUSH at 0x004081D4, whose dword operand at 0x004081D5 is retail 0x004097E0.
- DisplayQuadBitList subtracts gMikeCamera[0].Position (retail SCamera base 0x0056F1B0) from the quad's world-space vertices and then projects them via gte_rtps.
- DisplayQuadBitList does not call gte_SetRotMatrix itself. It assumes the shared GTE rotation state still contains the active camera rotation.
- M3d_RenderSetup @ 0x00472DC0 does call gte_SetRotMatrix @ 0x0046D7B0 with SCamera::Transform (offset 0x34), but later model rendering can overwrite the same shared GTE rotation state before registered bit lists are displayed.
- Retail CCamera::LoadIntoMikeCamera publishes gMikeCamera[0].Transform at 0x0056F1E4.
- patch_CBit() does not replace Bit_Init, so replacing the registered QuadBit display callback at the canonical Bit_Init registration site is live and does not conflict with the existing decomp patches.

Implemented commit:
- a24b4d27f586c97af175e5202bb9fb7db2268cbe — render: restore camera transform for world quad bits

Implementation:
- patch only the QuadBitList display function pointer immediate in retail Bit_Init;
- validate opcode 0x68 at 0x004081D4 and expected target 0x004097E0 before writing;
- register SpideyDisplayQuadBitListCameraAnchored instead;
- immediately before calling untouched retail DisplayQuadBitList, call untouched retail gte_SetRotMatrix(0x0056F1E4);
- leave the camera rotation active afterward, which is the semantically correct projection basis for subsequent world-space bit rendering;
- do not modify mShadowPos, shadow collision/ground-height data, CQuadBit corner generation, or Renderer11 replay coordinates.

Startup telemetry:
- [DRAW] quadbit_camera_anchor installed=... registration_push=0x004081D4 retail_display=0x004097E0 ... camera_transform=0x0056F1E4 gte_set_rot=0x0046D7B0 reason=...

Static validation after source edit:
- main.cpp braces 905/905
- parentheses 4481/4481
- brackets 323/323
- patch_CBit verified not to replace Bit_Init.

Runtime validation required:
1. Find a thug/cop/NPC with the normal floor blob.
2. Keep the NPC stationary and orbit the camera around them.
3. The blob should remain under the NPC instead of sliding with camera angle.
4. Also watch other world-space QuadBit effects briefly for regression because the correction restores the camera basis for the entire QuadBit list, which is what the retail projector expects.


## Pause Options dynamic container sizing (2026-10-04)

User screenshot showed the six-row custom Pause -> Options list extending below its purple expanding-box container: `Back` was visibly outside the box.

Root cause:
- the custom submenu intentionally reuses the live retail `CMenu`;
- parent state is snapshotted from offset +8 onward, leaving the live `CExpandingBox* ptr_to` at +4 outside the snapshot;
- the submenu rows were replaced, but the existing expanding box was still sized for the previous menu shape.

Canonical retail menu behavior:
- `CMenu::Zoom @ 0x0043FC60`;
- retail Zoom deletes the old expanding box and creates a replacement using the menu's current `GetMenuHeight()`;
- `GetMenuHeight()` derives height from the currently active rows, per-entry extra spacing, and `mLineSep`.

Implemented commit:
- `dd35f977e54e963d0deaede43c895cd7a8d2a95e` — `pause: resize menu box to current rows`

Implementation:
- added `SpideyPauseRefreshMenuBox`, which calls untouched retail `CMenu::Zoom` with the menu's existing `mZoomBoxType`;
- after the six custom Options rows are authored, rebuild the box from that exact row list;
- after Back restores the parent CMenu snapshot, rebuild the box again from the restored parent rows;
- when the one-time Options row is injected into the parent pause menu, rebuild the parent box too;
- no hard-coded submenu height was introduced, so future row additions/removals naturally resize the container through retail `GetMenuHeight()`;
- invalid zoom types are guarded rather than passed to retail.

Telemetry:
- `pause_menu_box_refresh reason=enter_options refreshed=1 ...`
- `pause_menu_box_refresh reason=back refreshed=1 ...`
- `pause_menu_box_refresh reason=add_options_parent refreshed=1 ...`

Static post-edit structure check:
- main.cpp braces: 909 / 909
- parentheses: 4503 / 4503
- brackets: 323 / 323

Runtime validation can be folded into the already-pending web-target + blob-shadow test:
- open Pause -> Options and confirm the purple container encloses all six rows through Back;
- press Back and verify the parent pause container still encloses Options and Quit.


## Runtime report recovery — uploaded log was pre-fix build (2026-10-04)

User reported:
- crash when pausing during gameplay in a level;
- camera-based web targeting felt better but remained inconsistent, especially against enemies that had aggroed;
- Spider-Man/NPC floor blob shadows still slid with camera movement.

Uploaded consolidated log:
- `spidey-decomp(20261004-071556).log`
- session revision: `ef78ea5ff959f4518a96567aed340bd935c251e5`

Important interpretation:
- this is an older installed build, from before the current web-axis, QuadBit camera-anchor, and dynamic pause-box commits;
- the log contains the older `camera_web_target_install` hook;
- it does **not** contain `quadbit_camera_anchor` or `pause_menu_box_refresh`, confirming the newer fixes were not present in that run;
- the log ends with `[SESSION] exit_code=0`, so this specific consolidated log does not capture an actual crash termination.

Therefore:
- do not regress or rewrite the current web/shadow/pause fixes based on this stale runtime;
- first validate current dev `414617b67870d9bf1880fdd66c919f81cd22bba4` using `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
- the separate `PLAY_CURRENT_BUILD.bat` intentionally launches whatever is already installed and can therefore run an older revision if the update/install BAT has not been run first.

Expected current-build startup evidence:
- session revision at or after `414617b67870d9bf1880fdd66c919f81cd22bba4`;
- `camera_web_target_install ... forward_axis=negative_local_z`;
- `quadbit_camera_anchor installed=1 ... reason=ok`;
- `pause_menu_box_refresh ... refreshed=1` after opening/injecting pause Options.

If the latest build still crashes on pause:
- preserve/upload the new consolidated log;
- also upload any `spidey-decomp-crash*.log` if generated;
- do not diagnose the old ef78 run as a crash reproduction because its session closed normally.


## Stale direct-launch runtime identified + play-current logging hardened (2026-10-04)

User reported:
- pause crash during gameplay;
- camera-based web targeting improved but still intermittent, especially against aggro enemies;
- character blob shadows still camera-relative.

The uploaded `spidey-decomp(20261004-071556).log` does **not** represent the current source frontier:
- session revision in the file is `ef78ea5ff959f4518a96567aed340bd935c251e5`;
- live `dev` at diagnosis was `414617b67870d9bf1880fdd66c919f81cd22bba4`;
- therefore the run predates:
  - `5e3dd2e...` camera-web negative-local-Z axis correction;
  - `a24b4d27...` QuadBit camera-basis restoration;
  - `dd35f977...` dynamic Pause Options expanding-box resize.
- the uploaded log ends with `[SESSION] exit_code=0`, so it does not contain the reported pause crash.

Interpretation:
- do not treat the reported shadow failure as evidence against `a24b4d27...`; that hook was absent from the tested build;
- do not treat the aggro-targeting behavior as proof that the separate `CheckWebShot -> SelectTargetBaddy` path must be patched yet; the tested build also lacked the newer axis correction;
- do not debug the reported pause crash from this log because the crash session is not present.

Workflow hardening commits:
- `f892eaa163459fdca5e7504e6e0a83cfcdff6d9a` — runtime stamps `runtime_revision=<RUNTIME_VERSION>` into the consolidated log on DLL attach, so direct launches identify the actually loaded proxy.
- `5709bdef9269f6d6e9e02c2c0ced6a74f3fbaf56` — `RUN_GAME.bat` now clears stale logs, seeds a fresh current-installed-build session, waits for game exit, records exit code, and points to the one uploadable `spidey-decomp.log`; it still performs no update/download/build/install.

Next runtime:
1. run `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat` **once** to install the current dev build;
2. then direct playtesting may use `RUN_GAME.bat` without updating/rebuilding;
3. re-test pause, aggro/non-aggro camera web targeting, and blob-shadow camera anchoring;
4. upload the freshly generated `spidey-decomp.log` if anything fails.


## VC6 build fix for runtime revision logging (2026-10-04)

User's FAST_UPDATE_AND_TEST_LATEST_BUILD run at revision `5709bdef9269f6d6e9e02c2c0ced6a74f3fbaf56` failed in `main.cpp` with VC6 C2360:

- initialization of `runtimeVersionLog` skipped by later switch case labels;
- declaration was inside `DllMain` directly under `case DLL_PROCESS_ATTACH:`.

Fix:
- commit `71290b003e895f179c2ab1921fb59bdb39b1753e` — `compat: scope process attach locals for VC6`;
- wrapped the `DLL_PROCESS_ATTACH` case body in its own braces so the local `FILE* runtimeVersionLog` lifetime no longer crosses `DLL_THREAD_ATTACH`, `DLL_THREAD_DETACH`, or `DLL_PROCESS_DETACH` labels;
- runtime behavior is otherwise unchanged.

Static structure after edit:
- braces 911/911
- parentheses 4507/4507
- brackets 323/323

Next action:
- rerun `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`;
- if it compiles, continue with the current runtime test for pause crash, aggro/non-aggro web targeting, blob-shadow anchoring, and dynamic pause box.

## 2026-10-06 — TRUE FULL-ENGINE 20-FPS GROUND-TRUTH DIAGNOSTIC

Previous targeted cadence experiments are closed as insufficient:
- scripted SpideyAI0 at authored 20 Hz: still fails;
- scripted SpideyAI0 + active camera at authored 20 Hz: still fails;
- forced Wait05->Wait06 trigger recovery: fired and still failed.

Latest paired player+camera runtime proved the route still fails while synthesized control remains active for several seconds after the visible cutscene, matching the user's observation that Spider-Man keeps moving/jumping autonomously in the building hole until control returns.

New diagnostic goal: stop guessing individual consumers and reproduce the known-good 20-FPS engine cadence globally with all existing logging still active.

Retail timer behavior makes this exact:
- `TimerCallback` converts callback milliseconds into canonical 60-Hz ticks with `interval_ms * 60 / 1000`;
- a ~50 ms callback therefore produces about 3 canonical ticks;
- the modern timer compatibility layer previously dispatched untouched retail TimerCallback at ~60 Hz using `floor(n*1000/60)+1` deadlines;
- diagnostic mode changes that dispatcher to `floor(n*1000/20)+1`, allowing 50/51 ms intervals;
- expected active callback delta becomes 3 vblanks instead of 1.

This intentionally makes the whole retail engine run at the authored 20-FPS quantum for the diagnostic: Logic, EveryFrame, animation, player, camera, collision, triggers, level scripts, and presentation scheduling. Existing Chase player/camera wrappers naturally collapse to one retail call per engine update because `field_80` is already about 3.

This is NOT the final native-60 solution. If Chase works here, this run becomes the working reference trace. The next step is to diff it against the failing 60-Hz trace and identify the first state divergence, then restore only that dependency to authored cadence while returning the rest of the engine to 60 Hz.

Forced-clean matching VC6 build: PASS.

### Installed full-engine 20-FPS ground-truth build

- behavior commit: `85ae7108eaaf19b826a1fe924904c89cd62ed380`
- proxy SHA-256: `3543F54E9E79CF70603AE5ED2397A243ED1F383B309AD07A42D56E85B2EE55E5`
- renderer11 SHA-256: `A1F6E5C21892DAA82555C8620B2FA9FFE822B7F917E7197289BB927688DFD458`
- input11 SHA-256: `44680FE5EBE1CD63CDDD14B3423745A4DBE6707BE2506EDC37E5C27671D31F50`
- forced-clean matching proxy build: PASS
- renderer11 rebuild: PASS
- input11 rebuild + 32-bit ABI preflight: PASS
- prepare/install into game folder: PASS
- game intentionally not launched.

TEST INSTRUCTION: run `TEST_LATEST_BUILD.bat`, not direct `SpideyPC.exe`, so the harness seeds and archives the full reference log. This diagnostic intentionally runs the whole engine at about 20 FPS/Hz. Test Chase Venom and report whether Spider-Man physically follows Venom through the building. Exit normally afterward so the working/broken reference trace can be captured.


## 2026-10-06 — working full-20 Chase reference isolated the next 60-Hz dependency

Dedicated recovery/frontier document:
- `docs/NEW_CHAT_HANDOFF_2026-10-06_CHASE_GOOD20_BADDYLIST_FRONTIER.md`

Ground-truth run `logs/20261006-020549/spidey-decomp.log` successfully traversed the building at full-engine 20 FPS. Direct comparison with failing `logs/20261006-012109/spidey-decomp.log` shows the decisive failure is camera/actor phase, not merely steering math: the same final type-3 code-9 worker runs with camera heading ~1029 in the working run but ~2080 and rotating in the failing run, turning the same scripted forward input into a side-wall collision.

Retail RE proves Venom lives on `BaddyList @ 0x0056E990`, updated by `Logic` at call site `0x004554F5` before pending trigger commands are consumed. The live candidate restores the global timer to native 60 Hz and cadence-gates only BaddyList during L5A1 synthesized control, alongside the already-gated scripted player/camera/synth paths. New trace telemetry records camera mode/interpolation plus node-74 pulse state.

A forced-clean matching VC6 build of this uncommitted candidate passed before PC access switched to Local Commander.

PC workflow update:
- use Local Commander as the primary local-machine interface from this point forward;
- local checkout remains authoritative;
- Desktop Commander is no longer the preferred path for this project.


## 2026-10-06 02:34 — 20 FPS reference diff checkpoint / Local Commander migration

Local Commander is now the preferred local-PC interface.

A successful full-engine 20 FPS reference was captured at `logs/20261006-020549/spidey-decomp.log`. Spider-Man visibly follows Venom through the building correctly there.

Direct diff against failing 60 Hz `logs/20261006-012109/spidey-decomp.log` found:
- traces identical through sample 21;
- first divergence at sample 22 is camera transform heading (60 Hz 183 vs working 20 FPS 168);
- sample ~37 includes an anomalous 6-tick 60-Hz player update vs expected 3 ticks in the working reference;
- decisive final code-9 worker uses the same `axes=0,127` in both runs;
- working 20 FPS camera is locked around heading 1029, producing desired world heading ~2053 and a straight +Z traversal through the building;
- failing 60 Hz camera begins around 2080 and rotates through ~2300→3000→3700, turning the same camera-relative input into the wall.

The failure target is therefore the separate L5A1 camera-shot/state lifetime that should remain active during the final code-9 worker.

Full details: `docs/CHASE_VENOM_20VS60_REFERENCE_DIFF_2026-10-06.md`.

Current `main.cpp` contains WIP/UNTESTED 60-Hz follow-up work:
- global timer restored from reference 20 FPS back to native 60 Hz;
- BaddyList/Venom 20-Hz cadence experiment at Logic call site `0x004554F5`;
- extra camera mode/interpolation/node-74 pulse telemetry.

Do not call this WIP runtime-tested until a new Chase run is performed.

## 2026-10-06 — Chase node76 / ControlBaddyList phase candidate

New retail proof supersedes the BaddyList-only hypothesis:

- L5A1 node 76 is type 203 = `CScriptOnlyBaddy`;
- it links directly to node 74, the 128-frame fixed-camera transition;
- node76 is constructed onto `ControlBaddyList @ 0x0056E994`;
- Venom remains on `BaddyList @ 0x0056E990`;
- Logic updates BaddyList at `0x4554F5`, then ControlBaddyList at `0x455501`, then pending trigger commands at `0x45551D`.

Current source phase-locks **both** list dispatches to the same 20-Hz-equivalent canonical tick during L5A1 synthesized control. Global engine/timer remains 60 Hz. Forced-clean VC6 build: PASS. Runtime test is still pending.

Full RE: `docs/CHASE_VENOM_20VS60_REFERENCE_DIFF_2026-10-06.md`.

### Static root-cause refinement

The L5A1 camera path is a long chain of type-203 `CScriptOnlyBaddy` controllers on ControlBaddyList. Each stage uses elapsed-time-correct `field_230`, but stage completion/pulse handoff is quantized to that list's AI dispatch. At 60 Hz repeated handoffs can occur up to two canonical ticks earlier per stage, cumulatively advancing the camera rail. This explains why the final code-9 worker sees camera heading ~2080+ instead of the working ~1029.

The current phase-locked BaddyList + ControlBaddyList candidate is therefore the strongest 60-Hz fix candidate so far.


## 2026-10-06 03:00 — RUNTIME-PROVEN 60-FPS CHASE FIX

Authoritative successful 60-Hz session:
- archive: `logs/20261006-030007/spidey-decomp.log`;
- runtime revision: `3ec28e4b`;
- session start: `2026-10-06T03:00:07-04:00`;
- proxy SHA-256: `34678FF856BEF480FAEDB5F7F37F1AE13B023FFF251040F0C6A3EE1BF68AC657`;
- renderer11 SHA-256: `7FF6D9C594AD9DCE81442FBD1982EA7725FBAFBD313AA6711A8102AFEBC59A35`;
- input11 SHA-256: `9A5C3933049A03CA7FA4318F28FF55C9280BE442FC845C81F4570E319A5DC4F5`.

User result: **WORKS at 60 FPS** — Spider-Man physically follows Venom through the building correctly.

Global timing proof:
- modern timer remains `target_hz=60`;
- expected vblanks per callback = 1;
- this is NOT the temporary full-engine 20-FPS diagnostic.

Phase-lock stats:
- BaddyList installed = 1;
- BaddyList calls = 5452;
- BaddyList retail dispatches = 850;
- BaddyList held dispatches = 1708;
- ControlBaddyList installed = 1;
- ControlBaddyList calls = 5452;
- ControlBaddyList retail dispatches = 850;
- ControlBaddyList held dispatches = 1708;
- shared gate events = 850;
- max shared elapsed = 3 canonical ticks.

Existing Chase compatibility in the successful run:
- scripted player AI retail calls = 854;
- active camera AI retail calls = 853;
- synth retail updates = 854;
- render/physics presentation remains 60 Hz.

Decisive final-worker proof:
- final type-3/code-9 worker begins at trace sample ~693;
- camera transform heading remains exactly `1029` through the worker;
- desired world heading remains `2053`;
- Spider-Man advances almost purely +Z through the building;
- collision remains ground-only then drops to 0 as he clears the interior;
- worker reaches timer 0 naturally.

This matches the successful full-engine 20-FPS reference and directly fixes the prior failure where the same code-9 input was interpreted against a camera heading that had already advanced to ~2080 and kept rotating.

Root cause:
- Venom lives on `BaddyList @ 0x0056E990`;
- the L5A1 script-only camera-rail controller lives on `ControlBaddyList @ 0x0056E994`;
- at 60 Hz, allowing these lists to hand off independently every Logic pass lets the long script-only camera chain quantize transitions on 1-tick boundaries and cumulatively outrun the authored player/synth phase;
- phase-locking both list dispatches to the same 3-canonical-tick authored boundary keeps Venom and the camera rail synchronized while the rest of the game remains 60 Hz.

Behavior commit:
`3ec28e4b2b7abb51f6166256df750690721fa475`

Current HEAD may contain documentation-only commits after this behavior commit; gameplay behavior is unchanged unless explicitly noted.


### Production promotion — Chase 60-FPS phase-lock policy

The successful 60-FPS run was compared directly against the successful full-engine 20-FPS reference using `tools/research/compare_good20_good60.py`.

Final type-3/code-9 worker equivalence:
- both runs: 51 samples, timer 150 -> 0 in 3-tick steps;
- both runs: camera heading locked at `1029` until worker completion;
- both runs: desired world heading `2053`;
- both runs: identical Z progression range through the building;
- aligned maximum camera-heading difference: **0**;
- aligned maximum desired-world-heading difference: **0**;
- aligned maximum body-angle difference: **2**;
- aligned maximum X difference: `109302` fixed-point (~26.7 world units), non-fatal and visually compatible.

This confirms the phase-locked 60-FPS run reproduces the authored through-building route closely enough to be treated as the production compatibility solution.

Production behavior remains exactly `3ec28e4b2b7abb51f6166256df750690721fa475` and is tagged:
`chase60-known-good-20261006`

No further gameplay-source cleanup is being applied at this checkpoint to avoid perturbing the runtime-proven behavior.


## 2026-10-06 — Venom Chase bar fragmentation fix candidate

Scope is specifically Venom Chase / L5A1 (`0x501`).

Retail `Venom_DisplayProgressBar @ 0x004E7E10` constructs the top chase meter from separate textured quads. The repeated strip uses `Panel_SetStretchedScreenCoords(Texture*)` at `0x004E837E`, with four additional helper calls for the surrounding chase-meter elements.

The fresh `logs/20261006-031423/spidey-decomp.log` proves the breakup is caused by the existing gameplay-HUD compactor choosing left/center/right anchors independently for adjacent pieces. At 2560x1440 / UI scale 180%:
- authored adjacent `192..211` -> left-anchored `86..95`;
- next `210..229` -> center-anchored `235..244`;
- authored adjacent `282..301` -> center `268..276`;
- next `300..319` -> right `417..425`.

Implemented candidate:
- override all five `Venom_DisplayProgressBar` texture-coordinate calls after the generic HUD patch;
- L5A1 only uses one shared top-center anchor `256,0`;
- all existing gameplay UI density/user-scale behavior is preserved;
- non-L5A1 behavior falls back to the generic HUD wrapper;
- Chase timing/gameplay source is untouched.

Full RE:
`docs/VENOM_CHASE_BAR_RE_2026-10-06.md`

Static validation:
- `git diff --check`: PASS.
- local compilation from chat is currently blocked by Local Commander's executable allowlist; next test must use `TEST_LATEST_BUILD.bat`.


## 2026-10-06 — Mysterio boss UI/camera + laser timing frontier

Latest Mysterio run: `logs/20261006-033344/spidey-decomp.log`.

Implemented but not yet runtime-tested:
- Mysterio boss type 311 common-health fill geometry now follows the same high-resolution HUD transform as its holder;
- modern camera/manual-aim ownership is explicitly disabled while the Mysterio boss bar is active, preserving the original retail boss camera;
- telemetry at `0x0045F489 -> CMysterio::FireBoobies` counts distinct state-6 laser attacks and canonical timing without changing behavior.

Static RE proves `FireBoobies` is staged and does not simply create two new beams every 60-Hz frame. Existing beam liveness fix remains untouched.

Full details:
`docs/MYSTERIO_60FPS_FRONTIER_2026-10-06.md`

Next: checkpoint this 60-FPS candidate, then make a temporary full-engine 20-FPS reference variant with the same telemetry to compare actual laser attack cadence.


### Temporary Mysterio full-engine 20-FPS reference build

The preserved 60-FPS Mysterio UI/camera/telemetry source is commit:
`0b3586fb`.

A temporary diagnostic variant now changes only the modern timer dispatcher:
- target callback cadence: 20 Hz;
- expected retail vblank advance per active callback: 3;
- interval clamp widened from 20 ms to 60 ms;
- first delivery target ~51 ms;
- retail TimerCallback itself remains untouched.

Purpose:
- reproduce the original full-engine 20-FPS Mysterio behavior;
- compare `mysterio_laser_attack` canonical attack timing against 60 FPS;
- judge visible laser frequency/pattern;
- simultaneously sanity-check the new Mysterio health-fill alignment and retail-only boss-camera policy.

The whole game running at about 20 FPS is intentional in this diagnostic and must be reverted after the reference log is captured.


### 2026-10-06 — Mysterio 20-FPS reference build compile fix

User's first build of temporary 20-FPS reference commit `f807ad2f` failed in `main.cpp`:
- C2065 at the laser telemetry wrapper: `SpideyIsMysterioBossActive` undeclared;
- C2373 later: helper redefinition/different type modifiers.

Root cause:
- the new telemetry wrapper is physically earlier in `main.cpp` than the helper definition added for the Mysterio boss-type gate.

Fix:
- added a forward declaration `static int SpideyIsMysterioBossActive();` immediately before the Mysterio FireBoobies telemetry typedef/wrapper;
- no timing, HUD, camera, or gameplay logic changed;
- restored generated `runtime_version.h` after the failed build;
- `git diff --check` passes.

The intended full-engine 20-FPS Mysterio reference behavior remains unchanged.


## 2026-10-06 — Mysterio 20-FPS reference result and 60-FPS AI cadence candidate

Successful full-engine 20-FPS reference archive:
`logs/20261006-040238/spidey-decomp.log`

Reference identity:
- runtime revision: `47cd318d` source lineage from temporary `f807ad2f` 20-FPS timer mode;
- timer telemetry confirms `target_hz=20`, expected 3 canonical vblanks/callback;
- Mysterio laser liveness and FireBoobies telemetry both installed.

User result:
- Mysterio health fill still does NOT align with its holder. The first common-boss fill hook hypothesis is therefore NOT sufficient and remains unresolved.
- Mysterio lasers visibly behave differently/correctly at the authored 20-FPS reference cadence.
- production goal: reproduce that laser behavior at 60 FPS without lowering the rest of the engine.

Static RE:
- Mysterio vtable: `0x0053BAB4`;
- virtual AI slot: `0x0053BABC`;
- retail target: `CMysterio::AI @ 0x0045EF10`;
- the state-6 `FireBoobies @ 0x0045D200` dispatcher is inside this AI;
- in the 20-FPS reference, telemetry observed 224 FireBoobies calls across four distinct state-6 attack entries;
- 198 of those calls occurred in substate 2, the active beam create/refresh stage;
- at authored 20 FPS those active refresh opportunities occur about every 50 ms;
- at native 60 Hz the same AI state would otherwise be serviced about three times in that interval.

Telemetry correction:
- the first reference wrapper accidentally read the proxy-linked `gTimerRelated` symbol and logged tick 0;
- production telemetry now reads retail canonical clock directly from `0x006B4CA8`.

New production candidate:
- global timer restored to native 60 Hz / one canonical vblank per callback;
- only Mysterio's virtual AI is cadence-gated while boss type 311 is active;
- accumulate retail canonical ticks from `0x006B4CA8`;
- hold AI until >=3 ticks;
- call untouched retail `CMysterio::AI` once with `field_80=accumulated` (normally 3), then restore the live field;
- CBody EveryFrame/animation, player, physics, retail boss camera, rendering, other baddies, and all other engine systems remain 60 Hz;
- existing elapsed-time laser-liveness fix remains active;
- Mysterio retail-camera-only guards remain active;
- Venom Chase phase-lock behavior is untouched.

New telemetry:
- `mysterio_ai_20hz_install`
- `mysterio_ai_20hz_stats`
- corrected `mysterio_laser_attack ... tick=...`

Expected healthy 60-FPS candidate stats:
- Mysterio AI wrapper calls at ~60 Hz while active;
- retail AI calls approximately 1/3 of active wrapper calls;
- held calls approximately 2/3;
- max elapsed normally 3;
- global timer telemetry remains target_hz=60.

Next runtime test:
run `TEST_LATEST_BUILD.bat`, enter Mysterio, confirm lasers match the 20-FPS reference while rendering/game remains 60 FPS. Health-bar alignment is a separate unresolved UI task.


## 2026-10-06 — Mysterio whole-AI cadence experiment FAILED; narrowed to FireBoobies-only

Crash archive:
`logs/20261006-041611/spidey-decomp.log`

Runtime revision:
`dfb3ecf66b4120fcb0001880314fa4d530d87751`

Observed result:
- game crashed during Mysterio fight;
- user also observed ordinary webbing destroying Mysterio body/soft-spot nodes, suspected incorrect;
- this was the first run using the whole-`CMysterio::AI` 20-Hz wrapper.

Whole-AI wrapper stats prove it was active:
- calls=1727;
- retail_calls=577;
- held_calls=1150;
- max_elapsed=6;
- global timer remained native 60 Hz.

Crash signature:
- exception `0xC0000005`;
- EIP `0x30007754`;
- fault module `binkw32_.DLL`;
- read target `0x0000021C`;
- process exit `-1073741819`.

The fault address itself is in Bink, but the whole-AI experiment is retired because it also changed unrelated boss behavior/state upkeep. Do not restore it as a production fix.

### Narrow laser fix

New candidate keeps `CMysterio::AI` at native 60 Hz and cadence-gates ONLY:
`0x0045F489 -> CMysterio::FireBoobies @ 0x0045D200`

Policy:
- while boss type 311 is active, accumulate retail canonical ticks from `0x006B4CA8`;
- hold FireBoobies until >=3 ticks;
- then call untouched retail FireBoobies with temporary `field_80=accumulated` (normally 3);
- immediately restore live `field_80`;
- reset cadence state when FireBoobies exits AI state 6 or after a large call gap;
- all other Mysterio AI/damage/state/object upkeep remains 60 Hz;
- player, physics, retail boss camera, rendering, other baddies and world logic remain 60 Hz;
- existing elapsed-time laser liveness fix remains active.

New stats:
`mysterio_laser_20hz_stats ... policy=fireboobies_only_20hz_global_ai_60hz`

### Mysterio soft-spot damage RE

Retail soft spot:
- constructor `CSoftSpot @ 0x0045F700`;
- retail mType = `0x149`;
- vtable = `0x0053BB88`;
- Hit slot = `0x0053BB94 -> CSoftSpot::Hit @ 0x0045F940`.

Critical retail damage gate:
- `CSoftSpot::Hit` begins by testing `SHitInfo.field_0 & 0x04`;
- hits without bit `0x04` return without applying node damage.

Confirmed destructive impact path:
- `CSplat` builds `SHitInfo.field_0 = 0x1E`;
- `0x1E` includes destructive bit `0x04`;
- it then invokes the target virtual `Hit()`.

A new telemetry-only vtable wrapper logs every soft-spot hit and then calls retail unchanged:
- incoming hit flags;
- whether bit `0x04` is present;
- damage;
- soft-spot part index;
- HP before/after;
- player `field_8F8` active web mode;
- retail Hit result.

This telemetry is intended to determine whether the user's ordinary web attack is really arriving with a destructive retail hit class before changing any damage behavior.

Health-bar alignment remains unresolved and is not part of this laser/crash candidate.


## 2026-10-06 — Mysterio laser crash follow-up: SetPos-only authored cadence

Two broad Mysterio cadence experiments are now explicitly RETIRED:

1. Whole `CMysterio::AI` 20-Hz wrapper
   - changed unrelated boss behavior/state upkeep;
   - coincided with incorrect node vulnerability and a runtime crash;
   - do not restore.

2. Whole `CMysterio::FireBoobies` 20-Hz wrapper
   - user reported crash exactly when Mysterio began firing lasers;
   - static RE shows this was also too broad because FireBoobies itself owns a multi-stage state machine and already uses elapsed-time `CBaddy::RunTimer` internally;
   - do not restore.

### Retail FireBoobies finding

Detailed disassembly of `CMysterio::FireBoobies @ 0x0045D200` shows:
- `field_320` is the attack substate;
- substate 0 performs attack setup;
- substate 1 waits for animation/state readiness then transitions to substate 2;
- substate 2 calls `CBaddy::RunTimer(&field_394)`, refreshes/aims the two laser beams, and invokes `CMysterioLaser::SetPos @ 0x0045B5E0`;
- therefore the state machine itself must continue to execute every native 60-Hz Logic update.

The key authored-cadence difference is below that state machine:
- at original 20 FPS, active substate-2 beam `SetPos` geometry/collision refresh happens once per ~50 ms update;
- at native 60 FPS, the same two direct SetPos calls can execute every ~16.7 ms;
- this is a much narrower and safer frame-rate dependency than throttling Mysterio AI or FireBoobies.

### New candidate

Global timer remains native 60 Hz.

`CMysterio::AI` remains 60 Hz.

`CMysterio::FireBoobies` remains 60 Hz and is now telemetry-only/pass-through.

Only the two beam SetPos callsites are authored-cadence sampled:
- left beam: `0x0045D3AB -> CMysterioLaser::SetPos @ 0x0045B5E0`;
- right beam: `0x0045D44E -> CMysterioLaser::SetPos @ 0x0045B5E0`.

Policy:
- first SetPos for a beam runs immediately;
- later calls use retail canonical clock `0x006B4CA8`;
- calls less than 3 canonical ticks after the previous retail SetPos return 0 to FireBoobies without advancing beam geometry/collision;
- every >=3 tick boundary calls untouched retail SetPos;
- retail FireBoobies interprets return 0 as “beam still active”, so held samples do not force an attack-state transition;
- existing elapsed-time laser-liveness wrapper remains active, allowing the beam to survive the held 60-Hz-only samples until the next authored SetPos update.

Telemetry:
- `mysterio_laser_setpos_20hz_stats`;
- `mysterio_laser_attack event=retail_passthrough ... policy=fireboobies_60hz_setpos_20hz`.

### Soft-spot telemetry compile correction

The first attempt referenced decomp-header member `CPlayer::field_8F8`, but that retail offset is not declared in the current CPlayer header. Matching VC6 failed with C2039 at main.cpp(4446).

Corrected telemetry reads the retail byte directly:
`*(unsigned char*)((unsigned char*)player + 0x8F8)`.

No damage behavior is changed.

### Build validation

- `git diff --check`: PASS.
- forced-clean matching VC6 build: PASS.
- full link of `Release\\spider.dll`: PASS.
- no runtime test has been performed on the SetPos-only candidate yet.

Health-bar alignment remains unresolved and unchanged.


## 2026-10-06 — world-space QuadBit drift root-cause candidate

User reports:
- Mysterio helmet/head effect still moves relative to the helmet when the camera moves, even with retail/default Mysterio camera active;
- blob shadows slide away from characters when the camera moves;
- Venom wrap/tentacle effect has similar drift;
- therefore issue is not primarily modern-camera ownership.

Retail camera evidence from latest Mysterio run `logs/20261006-045343/spidey-decomp.log`:
- camera mode during fight is retail LOOKAROUND (mode 17);
- modern camera inactive;
- helmet effect still drifts.

### QuadBit renderer RE

Mysterio helmet ring derives from `CQuadBit`.
Retail QuadBit display:
`DisplayQuadBitList @ 0x004097E0`.

DisplayQuadBitList uses two parallel transforms:

1. GTE path:
   - subtracts `gMikeCamera.Position @ 0x0056F1B4/B8/BC`;
   - feeds result to `gte_rtps`;
   - therefore requires current GTE rotation matrix to be active camera rotation.

2. DCX path:
   - calls `DCX_XformVector @ 0x00402700` on original world-space corners;
   - DCX_XformVector reads active 4x4 matrix `0x0056E668`;
   - DisplayQuadBitList refreshes that active matrix by copying `0x0056E6F8` at entry.

Existing compatibility restored only GTE rotation using `0x0056F1E4`.

Critical finding:
- `M3d_RenderSetup @ 0x00472DC0` builds pristine per-frame DCX camera/projection matrix as:
  `matrix4x4_ml(result, 0x0056E778, 0x0056E570)`
  and copies result to `0x0056E6F8`.
- `DCModel_RenderModel` and `DC_PSXModel_RenderModel` later reuse/overwrite `0x0056E6F8` with model-local transforms.
- therefore restoring only GTE camera rotation leaves QuadBits in mixed camera/model space.

New renderer-only candidate:
- immediately before retail DisplayQuadBitList:
  1. recompute exact retail DCX matrix via `matrix4x4_ml(rebuilt, 0x0056E778, 0x0056E570)`;
  2. copy 64-byte result back to `0x0056E6F8`;
  3. restore GTE camera rotation from `0x0056F1E4`;
  4. call untouched retail DisplayQuadBitList.

This directly targets Mysterio helmet ring and other CQuadBit world effects, including likely blob-shadow quads.

Venom wrap classification:
- `CVenomWrap : CNonRenderedBit`, not CQuadBit;
- do not claim this QuadBit fix will solve Venom wrap; trace its produced renderer primitive separately after QuadBit runtime result.

Validation:
- git diff --check PASS;
- forced-clean matching VC6 compile/link PASS.

No runtime test of the full DCX+GTE QuadBit restore yet.


## 2026-10-06 — latest Mysterio/effect run + GTE Hor+ root-cause candidate

Latest tested runtime:
`logs/20261006-052334/spidey-decomp.log`

Session revision:
`4cdd166921307f5b6c92d736c7c4e873ae5e328f`

User result:
- Mysterio helmet/head effect still moves relative to Mysterio as the camera moves.
- Standard character/NPC blob shadows still move away from their owners with camera movement.
- Mysterio health fill is still not inside its holder.

### Negative result: full QuadBit camera-matrix restore is not sufficient

The latest log proves the `4cdd1669` candidate was actually running:
- runtime revision matches `4cdd1669`;
- `quadbit_camera_anchor installed=1`.

The Mysterio fight also runs retail camera mode 17 / LOOKAROUND with modern camera inactive, yet the helmet effect still drifts. Therefore:
- not a stale DLL;
- not primarily modern camera ownership;
- restoring GTE rotation + rebuilding the DCX camera/projection matrix alone does not solve the drift.

### New root-cause evidence: model Hor+ projection and GTE projection are different domains

Retail `gte_rtps @ 0x0046DBC0` uses:
- projection scale / GeomScreen: `0x0054F03C`;
- horizontal GTE canvas basis: `0x0054F040`;
- vertical GTE canvas basis: `0x0054F044`.

Retail executable defaults:
- `0x0054F03C = 276`;
- `0x0054F040 = 512`;
- `0x0054F044 = 240`.

`SetGeomScreen @ 0x00470610` writes only `0x0054F03C`.

`M3dInit_InitAtStart @ 0x00453200` initializes:
- `0x0061B5FC = 512`;
- `0x00628614 = 240`.

`DisplayQuadBitList @ 0x004097E0`:
1. projects each CQuadBit world corner through `gte_rtps`;
2. converts the fixed 512x240 projected canvas to current output using:
   - X scale = `gGameResolutionX / 512`;
   - Y scale = `gGameResolutionY / 240`;
3. submits through `PCGfx_DrawQPoly3D @ 0x00508550`.

`PCGfx_DrawQPoly3D` does not apply another X/Y projection transform; supplied X/Y become the transformed-screen coordinates.

Critical mismatch:
- model/floating-point projection in `M3d_RenderSetup @ 0x00472DC0` reads widescreen/aspect scalar `0x00550064` at `0x00473504`;
- `gte_rtps` never reads `0x00550064`;
- therefore models can be Hor+ while CQuadBit effects remain on original 4:3 horizontal FOV.
- standard floor/blob shadows and Mysterio helmet ring are both CQuadBit, matching the shared symptom.

### New QuadBit Hor+ candidate

Do NOT scale GeomScreen globally: it is shared by X and Y and would change vertical FOV.

Instead patch only the two retail DisplayQuadBitList -> PCGfx_DrawQPoly3D calls:
- `0x0040A1A9 -> 0x00508550`;
- `0x0040A367 -> 0x00508550`.

New wrapper keeps all Y/depth/RHW/UV/color data unchanged and applies only:

`fixedX = screenCenterX + (x - screenCenterX) * *(float*)0x00550064`

where `screenCenterX = gGameResolutionX / 2`.

At:
- 4:3 scalar 1.0 => no change;
- 16:9 scalar 0.75 => horizontal displacement contracts to match the widened Hor+ model projection;
- wider/narrower supported aspect modes inherit their existing scalar automatically.

Telemetry:
- `quadbit_horplus ... x0=before->after ...`;
- QuadBit install line now records both patched QPoly3D callsites.

Forced-clean matching VC6 compile/link: PASS.

### Mysterio health-bar coordinate-domain split

Latest log shows Mysterio QPoly health geometry reaching the existing generic fill scaler already in live 2560x1440 coordinates, e.g.:
- before `2415..2515 x 168..276`;
- current generic scaler changes it to `2494.75..2539.75 x 100.8..165.6`.

Retail common boss branch confirms:
- QPoly sites `0x00464C8A`, `0x00464EA5`, `0x004650B0` explicitly multiply source geometry by `gGameResolutionX/512` and `gGameResolutionY/240` BEFORE `PCGfx_DrawQPoly2D`;
- flat `0x004650EB` and Gouraud `0x0046512D/0x00465162` remain authored in the 512x240 panel coordinate domain.

Therefore the first Mysterio fix incorrectly treated all six boss-fill calls as one coordinate domain.

Do not guess another health transform yet. Added dedicated telemetry for exact boss holder/fill relationships:
- holder texture call `0x00464CDE` after generic holder compaction;
- holder frame call `0x00464EF8` after generic holder compaction;
- three live-space QPoly fill inputs;
- flat/Gouraud authored-space inputs.

Telemetry prefix:
`mysterio_health_alignment`

This health telemetry is observation-only; no new Mysterio health-bar transform has been promoted yet.

### Separate useful finding from latest runtime

Mysterio soft-spot telemetry confirms ordinary player web mode 1 can arrive with destructive hit flag `0x04` and reduce a 100-HP soft spot to zero. Keep this separate from renderer/UI work; user previously observed ordinary webbing destroying nodes and expected only web balls to do so.

### Next runtime test

Run the normal harness:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

In Mysterio:
1. rotate/move camera and watch helmet effect;
2. watch any standard floor/blob shadow while moving camera;
3. check Mysterio health fill vs holder;
4. exit normally.

Expected log:
- QuadBit install line should show both Hor+ call patches = 1;
- `quadbit_horplus` samples should show scalar 0.75 at 2560x1440;
- `mysterio_health_alignment` should give holder and fill bounds needed for the exact health fix.


## 2026-10-06 17:04 — pending Hor+ test log captured before visual verdict

Archive:
`logs/20261006-170406/spidey-decomp.log`

Runtime revision `c39b9a43` carries behavior commit `3cc47634`.

Objective proof:
- QuadBit camera anchor installed;
- both Hor+ QPoly3D call patches installed and executed;
- 2560x1440 aspect scalar = 0.75;
- live samples show X correction occurred.

Do not classify the visual effect result until user reports it.

Mysterio health telemetry is decisive:
- compacted holder rect `268..277 x 32..42` maps to live `1340..1385 x 192..252`;
- Mysterio QPoly fill arrives BEFORE our current generic compactor at exactly `1340..1385 x 192..252`;
- same exact relationship repeats for holder frame segments.

Therefore Mysterio QPoly health geometry is already correctly live-scaled to the holder before the current wrapper; the current Mysterio QPoly wrapper double-compacts it. Next health fix should pass active-Mysterio QPoly calls through unchanged and treat authored flat/Gouraud calls separately.


## 2026-10-06 — Mysterio HUD passthrough + QuadBit GTE translation frontier

### Runtime verdict consumed
- Newest archived user test: `logs/20261006-170406/spidey-decomp.log`.
- Runtime revision in that session: `c39b9a430ebd5e08ad4e2798a445f47eabc8a69c`; active QuadBit Hor+ behavior originated at `3cc47634`.
- User visual verdict: Mysterio health bar still fragmented/outside its holder; Mysterio helmet circle, blob shadows, and similar GFX still move relative to their owners when the camera moves.
- The Hor+ wrapper definitely executed in the log, so Hor+ alone is a negative result rather than an untested patch.

### Mysterio health-bar diagnosis and source fix
- `mysterio_health_alignment` telemetry proves the QPoly fill reaches `SpideyCompatMysterioBossQPoly2D` already in live-resolution coordinates.
- Example at 2560x1440: holder authored rect `268..277 x 32..42` becomes live `1340..1385 x 192..252`; the immediately following QPoly fill is already exactly `1340..1385 x 192..252` before the compatibility scaler touches it.
- Root cause: the Mysterio-only wrapper then called `SpideyCompatHealthBarQPoly2D`, compacting/scaling those live pixels a second time.
- Fix now in `main.cpp`: while Mysterio is active, that QPoly path logs `fill_qpoly_live_passthrough` and calls retail `PCGfx_DrawQPoly2D @ 0x00507910` directly.
- Authored-space flat/Gouraud health pieces remain on their existing compatibility paths. Venom Chase UI code is untouched.

### Shared CQuadBit GFX diagnosis
- The affected systems share the retail `CQuadBit` path:
  - body blob shadows are allocated as `CQuadBit` in `CBody::UpdateShadow`;
  - `CMysterioHeadCircle : public CQuadBit`;
  - multiple Venom/Mysterio sparks/effects also derive from `CQuadBit`.
- Retail `DisplayQuadBitList @ 0x004097E0` subtracts camera position from QuadBit world vertices, then projects with the emulated GTE path.
- `gte_rtps @ 0x0046DBC0` consumes separate GTE rotation registers at `0x00610B20...` and translation registers at:
  - X: `0x00610B34`
  - Y: `0x00610B38`
  - Z: `0x00610B3C`
- Previous compatibility wrapper restored camera rotation with `gte_SetRotMatrix @ 0x0046D7B0` but did **not** clear those translation registers.

### Strong retail comparison
- Retail sibling effect renderer `CSimpleTexturedRibbon_Display @ 0x0040AA00` follows the same camera-relative GTE projection pattern.
- At `0x0040ABD2` it calls `gte_SetRotMatrix @ 0x0046D7B0`.
- Immediately after, at `0x0040ABD7`, it calls `0x0046E460`, a helper whose entire body zeros `0x00610B34/38/3C`.
- Only then does it execute the GTE transform/projection.
- This is the critical difference: after camera subtraction, translation must be zero. Inheriting model-local GTE translation can move world effects relative to the correctly rendered owner as camera/model state changes.

### New QuadBit candidate fix + probe
- `SpideyDisplayQuadBitListCameraAnchored` now:
  1. rebuilds/restores the DCX camera-projection matrix as before;
  2. samples the preexisting GTE translation registers;
  3. restores the active camera rotation;
  4. calls retail zero-translation helper `0x0046E460`;
  5. calls retail `DisplayQuadBitList`.
- New log marker:
  `quadbit_gte_state ... pre_trans=X,Y,Z ... action=zero_before_retail`
- Startup marker now includes:
  `gte_zero_trans=0x0046E460`
- The probe logs the first eight calls even if translation is zero, plus later nonzero samples up to the cap. A nonzero `pre_trans` directly confirms the state leak existed in the old path.

### Build validation
- Forced-clean matching VC6 compile/link: PASS twice after source changes.
- Candidate `Release/spider.dll` before commit/revision stamping:
  - size: 909,312 bytes
  - SHA-256: `8f65f350ec16a41f0f85478b0adf7053e52b2ca6f30e11995abbfee85b44ef4a`
  - embedded `gte_zero_trans=0x0046E460` marker: confirmed.
- No Venom Chase behavior/UI code, Mysterio AI cadence code, or FireBoobies cadence code was changed.

### Next runtime test
Run only through `TEST_LATEST_BUILD.bat`. In the Mysterio fight verify:
1. the health fill is inside its holder instead of fragmented across the screen;
2. the helmet/head-circle effect stays attached while rotating/moving the camera;
3. blob shadows remain under NPCs while rotating/moving the camera;
4. if convenient, observe another QuadBit-derived effect such as Venom/Mysterio particles.
After exit, inspect the newly archived consolidated log for `fill_qpoly_live_passthrough`, `gte_zero_trans=0x0046E460`, and `quadbit_gte_state`.


## 2026-10-06 — Mysterio holder composite fix + retail View-matrix effect anchoring + 60-Hz laser emitter follow

### User/runtime result consumed
Latest user run: `logs/20261006-203245/spidey-decomp.log`.

User visual result:
- Mysterio health fill is now inside the holder correctly.
- Holder itself is split: one/start piece appears near screen center while the rest is upper-right.
- helmet/head-circle FX still moves outside Mysterio's helmet.
- chest lasers do not remain attached to their shooter ports.

### Holder split root cause
Runtime `mysterio_health_alignment` telemetry shows adjacent holder pieces straddle the generic gameplay-HUD anchor thresholds:
- first/start piece center is around authored X~292 and is classified center-anchored;
- following holder piece center is around authored X~312 and is classified right-anchored.
The generic per-poly 40/60 percent heuristic therefore tears one composite boss-bar holder into two coordinate systems.

Fix now in `main.cpp`:
- Mysterio holder texture/frame wrappers call retail coordinate setup directly;
- every Mysterio holder piece then uses one shared top-right anchor:
  - anchor X = 512
  - anchor Y = 0
- health-fill QPoly live-space passthrough remains intact.
This mirrors the previously successful Venom composite-bar strategy: one composite HUD element must use one common anchor.

New holder telemetry labels:
- `holder_texture_shared_top_right`
- `holder_frame_shared_top_right`

### QuadBit camera-root-cause correction
The previous QuadBit camera wrapper restored `SCamera::Transform @ 0x0056F1E4`.
That was the wrong matrix for camera-relative effect projection.

Retail proof:
`CSimpleTexturedRibbon_Display @ 0x0040AA00` performs camera-relative world projection and at:
- `0x0040ABA0`: pushes `0x0056F224`;
- `0x0040ABD2`: calls `gte_SetRotMatrix @ 0x0046D7B0`;
- `0x0040ABD7`: calls zero-GTE-translation helper `0x0046E460`;
- then loads/projects the camera-relative point through GTE RTPS.

Therefore the retail-proven effect projection matrix is `SCamera::View @ 0x0056F224`, not `Transform @ 0x0056F1E4`.

QuadBit compatibility now restores:
- `SCamera::View @ 0x0056F224`;
- zero GTE translation;
- rebuilt pristine DCX camera*projection matrix before retail QuadBit draw.

This is the strongest current root-cause candidate for:
- Mysterio helmet/head-circle drift;
- blob-shadow drift;
- Venom-style QuadBit FX drift.

The prior stale-GTE-translation hypothesis was falsified by the latest run: all sampled `pre_trans` values were zero.

### Rejected Gouraud ribbon vtable experiment
Mysterio laser visuals are not QuadBits. `CMysterioLaser` owns two `CGouraudRibbon` objects at offsets `laser+0x3C` and `laser+0x40`. Retail identification remains useful: constructor `0x004F16C0`, vtable `0x0053C70C`, and `CGouraudRibbon::Display @ 0x004F1860`.

Final static review disproved the attempted `vtable+8` Display hook. The recovered class does not declare `Display()` virtual, the special-display dispatcher invokes the second vtable entry (`vtable+4`), and the freshly built proxy vtable shows that entry is inherited `CBit::Move`. Therefore `0x0053C714` is not a valid `CGouraudRibbon::Display` slot. The experimental global hook was removed before the source checkpoint.

Do **not** expect `gouraud_ribbon_camera_install` or `gouraud_ribbon_camera_restore` in the next correct build. If ribbon projection still needs a global fix after the emitter-follow test, trace its real indirect display registration rather than guessing another vtable slot.

### Mysterio laser emitter-follow correction
The existing safe laser policy keeps:
- global engine at 60 Hz;
- Mysterio AI / FireBoobies at 60 Hz;
- only the two `CMysterioLaser::SetPos @ 0x0045B5E0` calls sampled at authored 20-Hz cadence;
- elapsed-time liveness compatibility active.

User-visible problem with that policy:
Mysterio's chest mesh animates every 60-Hz frame, while beam geometry previously remained frozen for the two held samples between retail SetPos updates. That can visibly pull beam origins away from the chest ports even if simulation timing is correct.

New visual-only bridge:
- each laser gate remembers the last chest-emitter world position;
- on held 60-Hz SetPos samples, retail simulation/collision still does NOT advance;
- the existing two `CGouraudRibbon` point arrays are translated by the emitter delta;
- correction is tapered linearly:
  - near/start point gets 100% emitter delta;
  - intermediate points get proportionally less;
  - far endpoint gets 0%;
- next authored retail SetPos fully rebuilds beam geometry/collision normally.

This preserves the authored 20-Hz beam simulation while keeping the visible origin attached to the 60-Hz animated chest.

Telemetry:
- `mysterio_laser_visual_follow ...`
- extended `mysterio_laser_setpos_20hz_stats ... visual_follow_calls=... visual_follow_points=... policy=20hz_sim_60hz_emitter_follow_fireboobies_ai_60hz`

### Validation
- `git diff --check`: PASS.
- Fresh Win32 MSVC syntax/type build compiled `main.cpp` successfully with warnings only.
- The project-wide modern build still fails on known legacy-source incompatibilities outside this candidate (for example `DXinit.cpp: DS_INCOMPLETE` and old member-function macro syntax).
- An interrupted matching VC6 run produced a linked `Release/spider.dll`, but that binary was built **before** the invalid Gouraud `vtable+8` experiment was removed. It is superseded and must not be deployed as the final candidate.
- The committed source is authoritative. Use `TEST_LATEST_BUILD.bat` so the normal matching VC6 harness rebuilds the exact checkpoint before runtime testing.

### Next runtime test
Use `TEST_LATEST_BUILD.bat` only.

High-value visual checks:
1. Mysterio holder should remain one intact top-right composite; fill should stay inside it.
2. Mysterio helmet/head-circle FX should remain attached while moving/rotating the camera.
3. NPC/body blob shadows should remain under their owners while moving/rotating the camera.
4. Mysterio chest lasers should visibly originate from the shooter ports throughout animation, not lag behind between authored SetPos samples.
5. Confirm no laser-attack crash/regression.

After exit inspect the newest archived log for:
- `holder_texture_shared_top_right`
- `holder_frame_shared_top_right`
- `quadbit_camera_anchor ... camera_view=0x0056F224`
- `mysterio_laser_visual_follow`
- `mysterio_laser_setpos_20hz_stats`


### Runtime confirmation — camera-relative world FX anchoring fixed
User confirmed on the successful `a567bb58` runtime:
- Mysterio helmet/head-circle FX now remains correctly attached.
- character/NPC blob shadows no longer slide around when the camera moves.
- Mysterio chest lasers visually originate from the correct FireBoobies/chest emitter slots.
- Mysterio health bar and holder are both visually correct.

This is important evidence that restoring the retail-proven `SCamera::View @ 0x0056F224` for legacy effect projection fixed a **shared renderer compatibility bug**, not merely a Mysterio-specific attachment problem.

The earlier behavior where camera motion visibly displaced blob shadows from their owners is now confirmed fixed.

Next cross-check requested by user:
- Venom Chase / Venom body FX and tentacle-style effects should be tested to see whether they now remain attached correctly under camera motion as well.
- If confirmed, promote the View-matrix correction to the general legacy world-effect anchoring fix for QuadBit/ribbon-style compatibility paths.


## 2026-10-06 — MAJOR CONFIRMATION: game-wide legacy world-effect anchoring bug fixed

User confirmed the `a567bb58` View-matrix compatibility change fixed the previously recurring camera-relative FX displacement across multiple unrelated game systems.

Confirmed fixed in runtime:
- Mysterio helmet/head-circle FX stays attached to his helmet while the camera moves.
- NPC/character blob shadows stay beneath their owners while the camera moves.
- Mysterio laser visuals originate from the correct chest/FireBoobies emitter slots.
- Venom Chase / Venom tentacle-body FX now remain attached to Venom while moving the camera.

This proves the issue was not Mysterio-specific, shadow-specific, Venom-specific, or caused only by the custom camera.

Root cause:
- legacy world-space effect renderers expect retail camera `SCamera::View @ 0x0056F224` to be loaded into the GTE effect-projection path;
- the earlier compatibility wrapper restored `SCamera::Transform @ 0x0056F1E4` instead;
- this caused world-space FX to project in a different camera basis from their owning rendered models, making them visibly slide when the camera moved.

Retail proof remains:
- `CSimpleTexturedRibbon_Display @ 0x0040AA00`
- `0x0040ABA0: push 0x0056F224`
- `0x0040ABD2: call gte_SetRotMatrix @ 0x0046D7B0`
- `0x0040ABD7: call zero-GTE-translation @ 0x0046E460`

Production compatibility now restores the retail-proven View matrix before affected legacy effect rendering.

Status:
**RESOLVED — broad DX11 migration renderer bug.**

Do not reintroduce per-effect screen-space compensation for these classes unless new evidence proves a separate issue. The general View-matrix correction is now validated across Mysterio, blob shadows, and Venom FX.


## 2026-10-06 — Mysterio FireBoobies firing-rate comparison and scoped LookAt cadence candidate

### User observation
After the successful `a567bb58` renderer/attachment build:
- Mysterio health bar/holder: correct.
- helmet FX: correct.
- chest laser emitter attachment: correct.
- blob shadows: correct.
- Venom tentacle/body FX: correct.
- user reported the **rate at which Mysterio fires the lasers still feels somewhat too fast**.

### Direct 60-FPS vs true 20-FPS comparison
Successful 60-FPS run:
`logs/20261006-211658/spidey-decomp.log`

True full-engine 20-FPS Mysterio reference:
`logs/20261006-040238/spidey-decomp.log`

The current FireBoobies wrapper runs at native 60 Hz with `field_80=1`.
The true 20-FPS reference enters FireBoobies with `field_80=3`.

Measured attack phase timing:

Current 60-FPS complete attacks:
- attack 1: total 35 canonical ticks = ~583.3 ms
  - setup -> active beam: 7 ticks = ~116.7 ms
  - active beam stage: 28 ticks = ~466.7 ms
- attack 2: total ~283.3 ms
- attack 3: total ~1383.3 ms
- attack 4: total ~433.3 ms
- attack 5: total ~2766.7 ms
- every observed setup -> active transition: 7 ticks = ~116.7 ms.

True 20-FPS reference:
- attack 1: total 105 canonical-tick equivalent = ~1750 ms
  - setup -> active beam: 15 ticks equivalent = ~250 ms
  - active beam stage: 90 ticks equivalent = ~1500 ms
- attack 2: setup ~300 ms
- attack 3: setup ~300 ms
- attack 4: setup ~250 ms.

This confirms the user's speed impression is real:
**FireBoobies reaches active firing in ~117 ms at 60 FPS versus ~250–300 ms in the true 20-FPS reference.**

A particularly important comparison:
- first attack uses 36 FireBoobies calls in the 60-FPS run;
- first attack also uses 36 FireBoobies calls in the 20-FPS reference;
- those same 36 state-machine calls consume ~0.58 s at 60 Hz versus ~1.75 s at 20 Hz.
Therefore at least part of the attack behavior is call-rate dependent rather than purely elapsed-time dependent.

### SetPos and laser Move investigation
`CMysterioLaser::Move` is NOT the remaining attack-speed source.
The compatibility replacement now only implements the retail SetPos/Move liveness handshake with elapsed-time grace and does not advance attack geometry.

`CMysterioLaser::SetPos @ 0x0045B5E0` returns a collision/hit result; it does not contain a hidden beam-lifetime counter.
The existing 20-Hz SetPos sampling therefore remains valid and should not be removed.

### FireBoobies setup transition root cause
Detailed FireBoobies RE:
- substate 0 starts attack setup and creates a turn/look controller through helper `0x0045CF10`;
- substate 1 waits for `field_288 & 1`;
- that bit is the completion signal from the AI look/turn controller;
- once received, FireBoobies transitions to substate 2 and begins active laser work.

The global `CSuper` animation advance is already elapsed-time correct:
- frame advance uses `field_80 * mAnimSpeed / 2`;
- therefore animation itself is not the ~3x speed source.

Helper `0x0045CF10` creates:
`CAIProc_LookAt`

Retail LookAt execution:
`CAIProc_LookAt::Execute @ 0x004013D0`

Its turn step calls:
`0x00401528 -> CBaddy::YawTowards @ 0x004030C0`

Critical finding:
`CBaddy::YawTowards` applies:
`step = yaw_error * turn_factor / 256`
**once per call**, with no `field_80` scaling.

Thus:
- true 20-FPS reference: LookAt/YawTowards advances once per ~50 ms;
- native 60 FPS: it advances once per ~16.7 ms;
- Mysterio therefore converges toward the firing heading much faster and raises the completion flag earlier.

The exact 20-FPS reference revision `47cd318d` uses the same current `CAIProc_RotY` half-step code, so this discrepancy is NOT source drift and is specifically in the retail LookAt/YawTowards path.

### New narrowly scoped candidate
Do NOT globally alter YawTowards yet.

Patch only the retail LookAt callsite:
`0x00401528 -> CBaddy::YawTowards @ 0x004030C0`

Wrapper behavior:
- every non-Mysterio baddy: untouched retail pass-through;
- Mysterio outside FireBoobies state 6: untouched retail pass-through;
- active Mysterio in state 6:
  - LookAt itself still executes every 60-Hz AI update;
  - first YawTowards call of a new attack establishes a canonical 3-tick phase and runs immediately;
  - all state-6 LookAt YawTowards calls on the sampled phase execute retail;
  - calls on the other two canonical ticks return the current normalized angular error without modifying yaw;
  - a long gap/new attack resets the phase.

This preserves 60-Hz AI/state upkeep while making the actual proportional turn step advance on the same 20-Hz cadence as the known-good reference.

New stats:
`mysterio_yawtowards_20hz_stats installed=... calls=... retail_calls=... held_calls=... phase_resets=... callsite=0x00401528 retail=0x004030C0 policy=lookat_alive_60hz_yaw_step_20hz_fireboobies_state6`

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- full link: PASS
- candidate pre-commit `Release/spider.dll`:
  - size: 913,408 bytes
  - SHA-256: `3d86988a3e64103aa02cb02e3900797a6619a7c1c201afe1bcd135afdf4391d9`

### Next runtime check
Use `TEST_LATEST_BUILD.bat`.

Primary question:
- does Mysterio's FireBoobies wind-up / firing rhythm now feel like the true 20-FPS reference rather than the too-fast ~117-ms setup?

Also confirm no regressions to:
- correct chest-emitter attachment;
- correct helmet FX;
- corrected blob shadows;
- corrected health bar/holder;
- laser attack stability.


## 2026-10-06 — Intro-movie alt-tab fatal exit: DirectSound lost-buffer recovery

User reported a reproducible crash/termination when tabbing out while the startup/intro movies are playing.

### Captured failing run
Archive:
\`logs/20261006-213539/spidey-decomp.log\`

Runtime revision:
\`61bdd58baf722a0e22c7902ffd13f80acb3b2ad8\`

Session:
- started 2026-10-06 21:35:39 -04:00;
- abnormal process exit: \`-805306369 == 0xCFFFFFFF\`;
- no normal proxy SEH/access-violation crash record was emitted.

Focus evidence:
- retail input bridge records foreground/input loss during movie playback;
- later foreground reacquire is visible;
- movie compatibility path continues running.

Movie evidence:
- the movie surface itself shuts down normally:
  \`movie_surface_release reason=stop_call ... remaining_refs=0\`
- immediately afterward the frontend/display rebuild begins.

### Root cause
After the focus transition, retail DirectSound primary buffer is lost.

Retail \`DXSOUND_ShutDown @ 0x00503AF0\`:
- reads primary sound buffer from \`0x006BBF1C\`;
- calls virtual \`Stop()\`;
- the failing run returns \`DSERR_BUFFERLOST == 0x88780096\`;
- retail logs the DirectSound error;
- because the HRESULT is negative, retail calls:
  - \`DXINIT_ShutDown @ 0x005001B0\`;
  - CRT \`_exit @ 0x0052A101\`.

Therefore this was not a random crash. A normal focus-loss condition during Bink playback was being treated by the original 2000-era DirectSound shutdown code as fatal and deliberately terminating the process.

The same run subsequently shows repeated \`DXSOUND_Load\` lost-buffer errors during frontend rebuild. The apparent D3D \`0x0000046C\` line at \`shutdownDirect3D7\` comes from old cleanup/refcount logging and is not the primary fatal cause.

### Fix
Existing compatibility hook:
\`SpideyCompatCleanup503AF0\`
already intercepts all retail calls to \`DXSOUND_ShutDown @ 0x00503AF0\`.

It now:
- treats global \`0x006BBF1C\` as the retail primary \`LPDIRECTSOUNDBUFFER\`;
- checks \`GetStatus\`;
- detects either:
  - \`statusHr == DSERR_BUFFERLOST\`, or
  - \`DSBSTATUS_BUFFERLOST\` in status;
- calls \`Restore()\` before entering retail shutdown;
- retries \`Restore()\` up to 8 times with 10 ms spacing while DirectSound still reports \`DSERR_BUFFERLOST\`;
- on successful restore, calls untouched retail \`DXSOUND_ShutDown\`, whose original \`Stop/Unload\` flow can now complete normally;
- if restore still fails because foreground/priority has not returned yet, it does **not** enter retail's known-fatal \`Stop()->_exit\` path; a later cleanup call may retry.

New telemetry:
\`cleanup_503AF0 buffer_lost status_hr=... status=... restore_hr=... attempts=... recoveries=... failures=... foreground=... game_hwnd=...\`

### Movie compatibility log cleanup
The failing 67-second run emitted approximately 74,000 identical:
\`renderer11_release_exclusive_for_compat reason=movie_frame\`
records because the Bink next-frame loop probes the compatibility path much more frequently than actual displayed movie frames.

This logging is now throttled for \`reason=movie_frame\`:
- first 4 probes;
- every 3000th probe;
- always if exclusive state changes, a DirectDraw surface is lost, or a restore fails.

The compatibility checks themselves still run; only repetitive file I/O is reduced.

### Validation
- \`git diff --check\`: PASS
- forced-clean matching VC6 build: PASS
- full link: PASS
- candidate pre-commit \`Release/spider.dll\`:
  - size: 913,408 bytes
  - SHA-256: \`6bb4aa3096ce1d6de3cbe498444b7813aec5f5d08b089b0d466349c25fcaa343\`

### Next runtime
Run \`TEST_LATEST_BUILD.bat\`.

Test:
1. allow an intro movie to begin;
2. Alt-Tab away during playback;
3. return to the game;
4. let the movie finish/skip and verify frontend continues without process termination;
5. then continue to Mysterio so the pending FireBoobies LookAt 20-Hz cadence candidate can also be evaluated.

This fix is isolated from Mysterio timing, gameplay logic, and the validated game-wide View-matrix world-FX anchoring correction.


## 2026-10-06 — Mysterio complete; Race-to-Bugle Scorpion HUD candidate + Scorpion crash / ceiling zip tracing

### Mysterio FireBoobies — RESOLVED
User confirmed the `61bdd58b` Mysterio cadence build now feels correct:
- health bar and holder correct;
- helmet/head-circle FX attached;
- FireBoobies beam origins attached to the correct chest emitters;
- firing/wind-up cadence now feels correct compared with the true 20-FPS reference.

Treat the Mysterio laser timing frontier as resolved unless a new regression is observed.

### New user reports
From runtime `logs/20261006-213726/spidey-decomp.log`:
1. In **Race to the Bugle**, the Scorpion/Jonah chase/progress bar is fragmented in the same way the older Mysterio/Venom composite bars were.
2. Game terminated while fighting Scorpion.
3. Ceiling web-zip initiates the animation but Spider-Man remains stuck and never travels to the ceiling.

The run was revision `61bdd58baf722a0e22c7902ffd13f80acb3b2ad8`; it predates the later DirectSound movie-alt-tab recovery commit `7f07f7d8`.

The Scorpion-fight termination ends with process code `0xCFFFFFFF`, but this log does **not** show the DirectSound `DSERR_BUFFERLOST` signature from the already-understood intro-movie alt-tab failure and does not contain a normal access-violation/SEH crash marker. Treat it as a separate unresolved gameplay termination until telemetry identifies the active state.

### Race to the Bugle chase bar — candidate fix implemented
Retail `Panel_DisplayHealthBar @ 0x00464270` has a Scorpion-specific branch:
- `itemType == 310 / 0x136`
- textures include Scorpion, boss/progress holder, Jonah, wounded Scorpion.

The authored holder is one top-right composite, but the generic modern-UI compatibility path was allowing each piece to choose its anchor independently.

Recovered Scorpion-specific holder calls:
- `0x004651CF -> Panel_SetCoordsTexture @ 0x00462CD0`
- `0x004653E3 -> Panel_SetCoordsTexture @ 0x00462CD0`
- `0x004655F2 -> Panel_SetCoordsFrame @ 0x00462C30`

These three pieces use authored X around 406–448 and Y around 46–61 and belong to one composite.

Implemented:
- `SpideyCompactScorpionChaseBarPoly`
- `SpideyCompatScorpionChaseBarTexture`
- `SpideyCompatScorpionChaseBarFrame`
- fixed shared anchor `X=512, Y=0`
- only the three item-310 holder callsites are redirected.
- fill/head primitives are **not** given another generic transform; preserve their existing live coordinate path.

Telemetry:
`scorpion_chase_bar_scale ... policy=shared_top_right_anchor ...`

### Scorpion fight termination — safe retail AI telemetry
Retail proof:
- `CScorpion::AI @ 0x00488590`
- live retail vtable `0x0053BE2C`
- verified vtable slot 2 = `0x00488590` directly from the retail EXE.

Installed a telemetry-only vtable wrapper:
- validates slot 2 before patching;
- records Scorpion state/anim/health/countdown fields **before** each selected retail AI call;
- logs on state/animation/health change and periodically every 30 calls;
- then calls retail AI unchanged.
- deliberately does not dereference the object after retail AI returns, avoiding any lifetime hazard if a death/teardown path destroys the object.

Captured fields include:
- state `+0x31C`
- state auxiliary/pad
- animation/frame/finished
- health
- `field_80`
- `field_1F8`
- `field_BD8`
- `field_BF8`
- target handle
- world position.

Markers:
- `scorpion_ai_telemetry_install ... vtable=0x0053BE2C slot=2 ...`
- `scorpion_ai phase=pre ...`

Do **not** yet patch Scorpion's known raw per-call countdowns (`field_BD8 -= 1`, `field_1F8--`). The decompiled Scorpion source contains incomplete/NotOk state reconstruction, so first identify the exact failing retail state.

### Ceiling web-zip — retail path recovered and telemetry added
Correct retail availability function:
`CPlayer::CheckZipWebAvailability @ 0x004C30D0`

Recovered callers:
- `0x004C0EE0` -> calls availability at `0x004C104F`
- `0x004C1460` -> calls availability at `0x004C165C`
- `CPlayer_SetupLookaroundCamera @ 0x004C38A0` -> call at `0x004C43E5`

The first two are the unnamed R1/R2 zip initiators. Successful initiation:
- validates the target;
- stores zip target around player `+0xDC0`;
- stores surface normal around `+0xDA0`;
- calls `CPlayer::FireWeb @ 0x004C5DD0`;
- sets web mode byte `+0x8F8 = 8`;
- plays animation `0x10E / 270`;
- sets player state `field_E1C = 0x40000`.

Main player AI `SpideyAI0 @ 0x004B13F0` handles state `0x40000` at the dispatch around `0x004B50DB -> 0x004B51B2`.
The state progresses through:
- animation 270 (`0x10E`)
- animation 271 (`0x10F`)
- landing/finish animation 272 (`0x110`)
- `TidyUpZipWebLandingPosition @ 0x004C4A20`.

Player physics `CPlayer::DoPhysics @ 0x00466CE0` has the authored collision-free zip movement window:
- state `0x40000`;
- animation 270 once frame >= 13, or animation 271;
- retail adds `mVel` directly and returns.

The native-60 compatibility conversion from commit `6b56677c` deliberately uses `mVel >> 1` when `field_80 == 1`. Do not revert this blindly; it is part of the 30->60 physics split and could double zip speed if the actual failure is elsewhere.

Added observational zip telemetry directly in the replacement `CPlayer::DoPhysics`:
- logs entry into state `0x40000`;
- samples at roughly one record every 3 canonical ticks, plus animation changes;
- records state, `field_80`, animation, frame, fractional frame, animation-finished flag;
- position, velocity, acceleration;
- stored target and target delta;
- stored normal;
- when the collision-free movement window is active, logs a matching `post_move` sample;
- logs state exit.

Markers:
- `[TIMING] web_zip_physics phase=enter ...`
- `[TIMING] web_zip_physics phase=pre ...`
- `[TIMING] web_zip_physics phase=post_move ...`
- `[TIMING] web_zip_physics phase=exit ...`

No web-zip behavior was changed in this instrumentation batch.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- incremental rebuild after telemetry lifetime hardening: PASS
- linked `Release/spider.dll`: 917,504 bytes
- pre-commit SHA-256: `5b2ec0fb5dd869946d1a213d0e911a1c274ab29be78db960ff996cb0074d7ad2`

### Next runtime test
Run only:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

High-value checks:
1. Race to the Bugle: confirm whether the Scorpion/Jonah composite bar is now intact.
2. Reproduce one ceiling web-zip softlock (or confirm it unexpectedly works).
3. Fight Scorpion until the previous termination point or until enough combat has occurred.

After exit, pull the newest consolidated log directly. Do not ask for upload while Local Commander is available.


## 2026-10-07 — Web-zip deep RE: manual zip vs scripted move-to-point, direct-authored velocity 60-Hz candidate

### Triggering runtime
Newest reproduction:
`logs/20261007-001513/spidey-decomp.log`
revision:
`89c2952063fa953ae388a4608018785f525b9454`

User loaded a level whose in-engine opening sequence starts with Spider-Man visually web-zipping from the ceiling toward the floor. Spider-Man remained suspended at the ceiling/start point.

Log evidence:
- camera is `CAMERAMODE_DEMO (3)`;
- session exited normally (`exit_code=0`);
- there are **zero** `web_zip_physics` records;
- therefore this scripted sequence did not enter the ordinary manual zip travel state `field_E1C=0x40000`;
- Spider-Man's world position is bit-for-bit identical at frames 1200, 1500 and 1800:
  `-31195060,-3138734,-6942891`.
This rules out a simple visible overshoot as the whole explanation for that reproduction: the scripted sequence is holding/snap-backing the player at one exact point.

### Previously missing web-zip decomp
The source still contained placeholder stubs for:
- `CPlayer::CheckJumpingR1ZipWeb`
- `CPlayer::CheckJumpingR2ZipWeb`
- `CPlayer::CheckZipWebAvailability`

Retail mapping:
- first jumping zip handler: `0x004C0EE0`
- second jumping zip handler: `0x004C1460`
- `CPlayer::CheckZipWebAvailability @ 0x004C30D0`
- `CPlayer::TidyUpZipWebLandingPosition @ 0x004C4A20`
- `CPlayer::FireWeb @ 0x004C5DD0`

The two large jumping handlers are still retail-live; no source patch replaces them.

### CheckZipWebAvailability reconstructed
The old source incorrectly declared this function `void`. Retail returns its boolean result in AL.

Source/header now use:
`u8 CPlayer::CheckZipWebAvailability(SLineInfo*, i32)`

Recovered retail behavior:
- minimum line distance:
  - state 4 -> > 8
  - other states -> > 16
- must be less than caller max distance;
- rejects `pFace[3] & 0x40000`;
- loads player `field_A8` as GTE rotation row 0;
- constructs a relative fixed-point vector from:
  `lineInfo.Position + field_C84 * field_EA8 - mPos`;
- transforms it and reads transformed X;
- state 4 accepts immediately after distance/face checks;
- other states require transformed X > `0x40`.

The new C++ reconstruction uses the existing retail-compatible GTE helper functions and builds cleanly. This source function remains unpatched at runtime; the retail implementation is still authoritative.

### Manual zip path recovered
Both retail jump-zip handlers:
- raycast a candidate surface;
- call `CheckZipWebAvailability`;
- store target at player `+0xDC0`;
- store normal at `+0xDA0`;
- fire the web;
- start animation 270 / `0x10E`;
- set `field_E1C=0x40000`.

`SpideyAI0 @ 0x004B13F0` state `0x40000`:
- handles prep anims and transition into 270/271;
- clears crawl mode when zip travel takes over;
- detects target crossing / landing;
- sets a **fresh authored mVel every active AI update** using speed `0xF0`;
- eventually plays landing anim 272 and calls TidyUpZipWebLandingPosition.

### Retail DoPhysics proof
Our entire replacement `CPlayer::DoPhysics` is live via:
`PATCH_PUSH_RET(0x00466CE0, CPlayer::DoPhysics)`.

Retail `DoPhysics @ 0x00466CE0`:
- swing check;
- crawl check;
- retail velocity integration;
- special zip no-collision movement when:
  - state `0x40000`
  - anim 270 frame >= 13, or anim 271;
- retail then adds full `mVel` and returns.

The crawl-before-zip ordering in our source exactly matches retail. Do not reorder these branches.

### Scripted cutscene movement RE
`CPlayer_SynthesizeAnalogueInput @ 0x004BC300` processes linked script command blocks at player `+0x1BC` while `field_1AC` is active.

Dispatcher table `0x004BD44C` maps command types 2..15.

Recovered **type 5** handler at `0x004BD0FD`:
- block contains a 3D target and authored speed;
- computes `Utils_Dist(target, playerPos)`;
- completes/removes block once distance < `0x40`;
- otherwise normalizes `target - playerPos`;
- multiplies by authored speed;
- writes that vector directly to `player->mVel`.

This provides a second path which can visually represent scripted zip/travel without ever entering manual state `0x40000`.

The caller `sub_4BD510` invokes `CPlayer_SynthesizeAnalogueInput` at `0x004BD572` when scripted control is active. Its subsequent analogue/ramp processing does not itself overwrite mVel, although later state-specific SpideyAI0 code can still touch velocity.

### Native-60 direct-authored velocity mismatch
Commit `6b56677c` converted player physics from retail cadence to native 60 Hz using a continuous half-step integrator plus half displacement when `field_80==1`.

That is correct only if velocity persists between the two 60-Hz half-steps.

Manual zip and script command type 5 violate that assumption because they author a fresh mVel every AI update.

Retail friction setup in SpideyAI0 proves:
- manual state `0x40000` uses friction `1,1,1`.

For an authored velocity V with zero acceleration and friction 1:
- one retail 30-Hz solve: post-friction velocity = 0.5V, displacement = 0.5V.
- current native-60 continuous half-step: post-half damping ~=0.707V, displacement ~=0.354V each 60-Hz frame.
- over the same 33.3 ms, two re-authored 60-Hz frames move ~=0.707V rather than retail 0.5V.

This is a ~41% excess displacement because AI destroys the velocity persistence assumed by the square-root damping derivation.

### Candidate implemented: direct-authored motion integration
Added `SpideyPhysicsGetDirectAuthoredMotion`.

It recognizes only:
1. manual zip state `0x40000` while anim 270/271;
2. an active scripted command chain containing type 5.

When `field_80==1` and one of those modes is active:
- use the **full retail acceleration+friction solve** for mVel;
- retain the already-existing **half displacement** at 60 Hz.

Therefore each 60-Hz frame consumes half of a retail-authored movement step without assuming velocity survives into the next frame.

Applied in:
- `CPlayer::DoPhysics`
- `CPlayer::DoCrawlingPhysics`

All other player movement keeps the prior continuous native-60 half-step.

This also deliberately covers a script type-5 worker while crawl mode is still active; the next runtime trace will tell whether crawl physics itself is snapping the scripted movement back to the ceiling.

### New generic DEMO script telemetry
The existing wrapper around retail `CPlayer_SynthesizeAnalogueInput` now records capped DEMO-camera snapshots even outside L5A1.

Marker:
`[TIMING] script_motion ...`

Captured:
- pre/post retail synth phase;
- worker type bitmask;
- head command type/size/data;
- player state;
- crawl/wall/ceiling flags;
- animation/frame/fraction/finished;
- field_80;
- position;
- velocity;
- acceleration;
- collision bits.

It logs on important state/command changes plus a low-rate periodic sample, capped at 256 records.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 917,504 bytes
- pre-commit SHA-256:
  `c376cc0fc1c86b8bce30ac6eb8b1b09d47e32270ec076ac76ae23aaa051679ea`

### Next test
Run:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

Highest-value reproduction:
1. load the same level with the scripted ceiling -> floor zip/drop;
2. observe whether Spider-Man now travels;
3. if still stuck, exit normally if possible.

Then inspect `script_motion` in the newest consolidated log:
- if type 5 is active and crawl=1 while velocity changes but position does not, the remaining bug is the scripted surface-detach/crawl handoff;
- if type 5 is active and crawl=0, compare pre/post mVel and successive positions to verify authored-motion integration;
- if no type 5 appears, use the worker mask/head to identify and decompile the actual command type;
- manual R1/R2 zip should also be retested afterward because its state-0x40000 authored velocity now uses the corrected integration class.


## 2026-10-07 01:26 run — zip never enters retail travel state; direct R1/R2 gate tracing added

### Runtime pulled
The test harness created `logs/20261007-012633` but had not archived the log, so the authoritative live consolidated log was pulled from:
`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

The completed 191,243-byte log was copied into:
`logs/20261007-012633/spidey-decomp.log`

Runtime revision:
`ad37f39a0ef96663c449121674f75e7c4bb02242`

### User-visible result
- Spider-Man still became stuck during the ceiling/web-zip-looking sequence.
- Pressing Space/jump broke him loose and allowed him to fall to the ground.
- Ordinary web-zip still cannot be initiated.

### Runtime proof from this run
There are **zero**:
- `web_zip_physics` records;
- `script_motion` records.

Therefore the visible stuck sequence never reached:
- manual zip travel state `field_E1C=0x40000`; or
- the previously instrumented scripted type-5 move-to-point path.

World position while stuck is exact and stationary:
- frame 1981 / camera focus: `-31195060,-3138734,-6942891`
- frame 2100 / body: `-31195060,-3138734,-6942891`

After the user presses Space:
- by frame 2257 Y has changed to approximately `-397312`;
- by frame 2400 Spider-Man is grounded at `-30998474,-393216,-6950740`.

This makes the surface/crawl/jump handoff significant for the intro symptom, but it does **not** explain why ordinary manual zip never starts.

### Manual zip caller coverage
Full retail `SpideyAI0 @ 0x004B13F0` disassembly shows six R1 and five R2 zip callsites:

R1 `0x004C0EE0`:
- `0x004B22BE`
- `0x004B2785`
- `0x004B2954`
- `0x004B2C5E`
- `0x004B2E00`
- `0x004B32B4`

R2 `0x004C1460`:
- `0x004B22FA`
- `0x004B2794`
- `0x004B2963`
- `0x004B2C6D`
- `0x004B2E0F`

### R1 retail rejection gates recovered
`CPlayer::CheckJumpingR1ZipWeb @ 0x004C0EE0` returns false before raycast if any of these are true:
1. `field_8EA != 0` (retail manual-aim/lookaround state);
2. `mHeldObject @ +0xE48 != 0`;
3. byte `player+0x550 != 0`;
4. processed input held byte `player->field_E0C + 0x60 == 0`.

If these pass, retail:
- raycasts approximately `field_C84 * 0xC00` from the player;
- validates hit/face;
- calls `CheckZipWebAvailability(..., 0xC00)`;
- then sets target/normal, creates/fires web, runs animation 270, and sets `field_E1C=0x40000`.

### R2 retail rejection gates recovered
`CPlayer::CheckJumpingR2ZipWeb @ 0x004C1460` shares the first three gates, then uses processed input/state at:
- input `+0x70`;
- global `0x0060CFC7`;
- script-active `field_1AC`;
- state bits `field_E1C & 6`;
- raw player byte `+0xE8D`;
- input `+0x100`.

On success it performs its own raycast, calls `CheckZipWebAvailability(..., 0x800)`, then enters the same animation/state-0x40000 zip path.

### Processed input model recovered
Retail player constructor `0x004B9EB0` hard-wires:
`player->field_E0C = 0x00661100`.

`Pad_Update @ 0x00505720` uses 0x10-byte button-state records.

Relevant raw action -> processed-record mapping:
- raw `0x100` -> `0x00661140`;
- raw `0x200` -> `0x00661160`;
- raw `0x400` -> `0x00661170`;
- additional raw `0x10` -> `0x00661200`.

Retail action map identifies:
- `0x0100` = aim;
- `0x0200` = zipline;
- `0x0400` = swing.

Therefore **R1's +0x60 gate is exactly the processed held state for the retail zipline action**.

`Pad_Update` helper `0x00479540` record layout:
- +0 = currently held;
- +1 = newly pressed transition/latch;
- +2 = rapid re-press flag;
- +4 = held duration;
- +8 = released duration;
- +0xC = transition timer.

### player+0x550
Static xrefs show this is a player-local latch which is repeatedly set by `SpideyAI0` and automatically cleared when processed `input+0x70` (raw `0x400`, retail swing action) is no longer held. It is therefore closely associated with swing/surface transition suppression and is an explicit zip blocker. Keep it in the next trace rather than guessing its final semantic name yet.

### Decomp signature corrections
The old source declared both:
- `CheckJumpingR1ZipWeb`
- `CheckJumpingR2ZipWeb`

as `void`.

Retail returns success/failure in AL and every SpideyAI0 callsite tests that result. Their source/header signatures are now corrected to `u8`. The source bodies remain TODO stubs and are not runtime-patched as implementations; runtime still calls retail through diagnostic wrappers.

### New direct zip telemetry candidate
All 11 direct SpideyAI0 calls are patched to wrappers which call the real retail functions unchanged.

Markers:
- `web_zip_check kind=r1 ...`
- `web_zip_check kind=r2 ...`

Each record captures:
- retail result;
- state before/after;
- animation/frame/finished;
- `field_8EA`;
- held-object state;
- byte +0x550;
- input-ignore timer;
- crawl/wall/ceiling flags;
- byte +0xE8D;
- script-active flag;
- global `0x0060CFC7`;
- full processed records at +0x40, +0x60, +0x70 and +0x100;
- axes;
- position and velocity.

Both retail availability callsites are also wrapped:
- R1 callsite `0x004C104F`;
- R2 callsite `0x004C165C`.

Marker:
`web_zip_availability ...`

It classifies rejection as:
- distance_low;
- distance_high;
- face_no_zip;
- orientation_or_other;
- accepted.

### New generic player-state trace
After each retail Logic pass, a low-rate/change-driven `player_state_trace` captures player state independently of `field_1AC`.

This is specifically intended to explain the ceiling-stuck-until-Space path:
- state;
- animation/frame/finished;
- crawl/aim/wall/ceiling;
- input-ignore;
- script state;
- held object;
- processed jump/aim/zip/swing/extra button records;
- position/velocity/acceleration.

### Validation
- forced-clean matching VC6 build: PASS
- `git diff --check`: PASS after whitespace cleanup
- built `Release/spider.dll`: 925,696 bytes
- SHA-256: `888d57755bf9df7e9b60e405d8fb296601a46d87106f3b2d4988dc2ace237562`

### Next runtime test
Use only:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

Highest-value sequence:
1. reproduce the opening ceiling-stuck sequence once; if needed press Space as before;
2. after gaining control, deliberately attempt ordinary web-zip several times while standing/jumping in a known valid area;
3. exit normally.

Interpretation:
- `input60 held=0` during attempted zip => input/action translation failure upstream;
- `input60 held=1` + `aim_flag=1` => stale/manual-aim compatibility state blocks retail zip;
- `input60 held=1` + `gate_550=1` => stale swing-related latch blocks zip;
- early gates clear but no `web_zip_availability` => raycast found no candidate;
- availability logs reject => exact geometric reason now visible;
- availability accepted but retail result false => investigate later web allocation/animation state;
- retail result=1 and state becomes 0x40000 but movement still fails => return to authored-velocity/physics path with now-confirmed entry.


## 2026-10-07 — Hostage Situation (L1A3) opening auto-web-zip path decoded

### User clarification
The broken ceiling-to-floor zip occurs automatically at the very beginning of **Hostage Situation**. It is an in-engine scripted/cutscene action and does **not** require the player to press the zip button.

This changes the interpretation of the bug:
- normal R1/R2 web-zip remains a separate broken feature worth tracing;
- the L1A3 opening must be followed from the level trigger/script data into synthesized player input.

### Exact retail level identified
Hostage Situation = **L1A3**.

The retail install stores the level in `data.pkr`. The existing PKR format was reused to extract the relevant files directly and verify each CRC:

- `l1a3.kat` — 336,088 bytes — CRC `1C4104A2`
- `L1A3_G.psx` — 1,482,332 bytes — CRC `33CD2F43`
- `L1A3_L.psx` — 291,872 bytes — CRC `B90C2C57`
- `L1A3_O.psx` — 133,640 bytes — CRC `2856A963`
- `L1A3_T.trg` — 20,718 bytes — CRC `886E2FFD`

The extracted retail copies were used only for RE and are not to be committed.

### TRG format recovered
`Trig_LoadTRG @ 0x004DEB50` proves:
- magic `_TRG` at +0;
- version `0x00010002` at +4;
- node count u16 at +8;
- node-offset table at +0x0C;
- every node offset is file-relative.

`L1A3_T.trg` contains 470 nodes.

### Actual startup restart
The runtime stuck position is far closer to restart node 1 than any other L1A3 restart.

Node 1:
- type 8 restart;
- name `Re_Start_Death`;
- authored position `(-7413,-977,-1681)`;
- links:
  `66,132,134,214,165,301,395`.

Its restart command list configures fog/camera/render settings, sets camera mode 3 (DEMO), then ends with `SendPulse`.

Decoded restart opcodes from retail command-dispatch strings include:
- `0xD5` SetFightMusicTime
- `0x68` SetFoggingParams
- `0xB7` SetSpideyLookAroundCamValue
- `0xA6` SetOTPushback
- `0xA9` SetOTPushback2
- `0xB8` SetSpideyShadowRGB
- `0xBF` visibility-by-name
- `0xA0` SetCamMode
- `0x12E` EndLevelNode
- `0x03` SendPulse

There is no direct movement command in the restart list itself.

### Opening controllers are CScriptOnlyBaddy objects
Six restart-linked type-1 nodes all use item type `0xCB / 203`:
- 66
- 132
- 134
- 165
- 214
- 395

`Trig_CreateObject @ 0x004DEE70` maps item type 0xCB to:
`CScriptOnlyBaddy::CScriptOnlyBaddy @ 0x004075B0`.

Their virtual AI is:
`CScriptOnlyBaddy::AI @ 0x00407840`,
vtable `0x0053B2E8`, AI slot +8 / slot 2.

### Node 214 is the auto-input startup branch
Node 214 has the compact script:
- `0x4280, 4` — delay 4
- `0x4100` — terminate/pulse linked nodes

Its links include **node 234**.

Node 234 is a type-6 command point whose command list begins:
- `0xC3, 0x001E` = `WideScreen = 30`
- `0xC7` = **CPlayer_SwitchToSynthesizedInput**
- followed by the synthesized-input bytecode
- then command-list terminator.

Retail command `0xC7` handler is at `0x004E1485`; its direct call is:
`0x004E1499 -> CPlayer_SwitchToSynthesizedInput @ 0x004BC1A0`.

Therefore the Hostage Situation opening movement is conclusively automatic scripted player input.

### Node 234 synthesized-input program decoded
The program passed after `0xC7` is:

`0000 0003 000A 006E
 0082 0003 000A 0018
 0035 0003 0006 000A
 0000 00FF`

`CPlayer_SynthesizeAnalogueInput @ 0x004BC300` treats each entry as a relative delay, synth opcode and parameters.

Synth opcode 3 dispatches to `0x004BC92B` and creates worker type 3.

The program therefore means:

1. delay 0:
   - synth opcode 3
   - action 10
   - duration 110
2. delay 130:
   - synth opcode 3
   - action 10
   - duration 24
3. delay 53:
   - synth opcode 3
   - action 6
   - duration 10
4. end.

### Critical unification: synth action 6 is the retail ZIPLINE input
Worker type 3 executes at `0x004BCC2D`.

Action 10 dispatches to `0x004BCC9F`:
- marks the synthetic axis state;
- writes `field_E2D = 0x81` (-127).

Action 6 uses the ordinary button-record branch at `0x004BCC6D`:
- selects synthetic input record index 6;
- asserts its held byte;
- asserts its press-transition byte when newly pressed.

The synthetic input records are later copied into the real processed input array at `player->field_E0C`.

Record index 6 maps to:
`player->field_E0C + 0x60 = 0x00661160`.

Earlier retail input RE already proved:
- raw/action `0x0200` = ZIPLINE;
- R1 web-zip `0x004C0EE0` requires processed held byte `+0x60 != 0`.

Therefore **Hostage Situation automatically synthesizes the exact same retail zipline held input that a player-generated R1 zip uses**.

This is the key connection:
- the cutscene source is automatic, not button-driven;
- after synthesis, both automatic and manual zip converge on the same retail R1 zip path.

### Why the previous zero script_motion result matters
The `ad37f39a...` L1A3 run produced zero `script_motion` records even though static retail data proves node 214 should eventually pulse node 234 and execute `0xC7`.

This means the missing handoff is now bounded to:
1. node 214 not being created;
2. node 214 being created but not reaching terminal `0x4100`;
3. node 214 reaching terminal state but node 234 not being pulsed/executed;
4. `0xC7` executing but synth state being cleared before the existing wrapper observes it.

The old generic downstream telemetry could not distinguish these.

### New exact L1A3 startup telemetry
No gameplay behavior is changed in this batch.

Added three exact retail hooks:

#### 1. L1A3 object-creation trace
Direct call:
`0x004DFC7B -> Trig_CreateObject @ 0x004DEE70`

Wrapper logs the startup nodes when L1A3 is active.

Marker:
`l1a3_startup event=create_object ...`

This proves whether node 214 is actually instantiated.

#### 2. CScriptOnlyBaddy AI trace
Verified vtable:
- `0x0053B2E8`
- slot 2 = `0x00407840`

Wrapper calls retail AI unchanged and records only the L1A3 startup controllers.

For node 214 it captures:
- canonical tick;
- field_80;
- active flag +0x20C;
- delay +0x230;
- special flag +0x234;
- raw +0x238;
- current script pointer +0x24C;
- current command word.

Marker:
`l1a3_startup event=scriptonly_ai ... node=214 ...`

This will show the countdown and whether the next command becomes `0x4100`.

The wrapper deliberately does not dereference the controller after retail AI returns because terminal script commands can hand lifetime back to retail list management.

#### 3. Direct node-234 synth-switch trace
Direct call:
`0x004E1499 -> CPlayer_SwitchToSynthesizedInput @ 0x004BC1A0`

The wrapper logs:
- first 16 synth words;
- player state;
- synth-active byte +0x1AC;
- synth clock +0x1B0;
- parser flag +0x1B4;
- script pointer +0x1B8;
- worker list +0x1BC;
- aim and crawl flags;
- before and after retail.

Marker:
`l1a3_startup event=synth_switch phase=pre/post ...`

The expected words for Hostage Situation are:
`0000,0003,000A,006E,0082,0003,000A,0018,0035,0003,0006,000A,0000,00FF,...`

### Existing downstream traces retained
The current candidate still includes:
- all 6 R1 web-zip callsite wrappers;
- all 5 R2 callsite wrappers;
- R1/R2 `CheckZipWebAvailability` wrappers;
- `player_state_trace`;
- `script_motion`;
- manual-zip physics trace.

Thus one L1A3 run can now show the entire chain:
restart controller -> node 214 -> node 234 -> C7 -> synth action 6 -> processed zipline held bit -> retail R1 check -> availability -> state 0x40000 -> zip physics.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 925,696 bytes
- pre-commit SHA-256:
  `7901a5d63f389a2aa92bd08d8303cd11a5cc4b040059975296269d1945039a5e`

### Next runtime
Run:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

For the highest-value test, simply load **Hostage Situation** and let its opening sequence run. No zip button input is required.

If Spider-Man remains suspended, exit or press Space afterward only if needed to regain control. The new `l1a3_startup` records should identify the exact broken handoff without requiring any additional manual actions.


## 2026-10-07 — Generic web-zip compatibility candidate: stale modern-aim gate

### Direction
The Hostage Situation opening remains a useful automatic reproducer, but this batch deliberately targets **generic web-zipping**, not L1A3-specific behavior.

Static RE now proves:
- manual R1/R2 web-zip and Hostage Situation's synthesized ZIPLINE action converge on the same retail zip starters;
- retail R1 is `0x004C0EE0`;
- retail R2 is `0x004C1460`;
- both reject immediately before raycast when `player->field_8EA != 0`;
- `field_8EA` is the retail lookaround/manual-aim ownership flag;
- `EnterLookaroundMode @ 0x004C3580` is the only relevant setter and sets it to 1;
- retail lookaround camera mode is 7 at `0x004C370B`;
- our modern manual-aim compatibility intentionally patches that visible camera mode from 7 to 3 so aiming remains in the modern third-person camera.

The actual zip geometry is player/surface owned:
- R1 ray direction uses `field_C84`, the player's local surface/up basis;
- `CheckZipWebAvailability` uses `field_A8`, the surface normal Spider-Man is aligned to;
- the render/DX11 camera does not own either vector.

Therefore DX11 camera matrices are not directly involved in zip collision/raycast geometry.

### Other shared gates checked
R1's early gates are:
1. `field_8EA == 0`
2. no held object
3. player byte `+0x550 == 0`
4. processed ZIPLINE held record `input+0x60 != 0`

The `+0x550` latch is transient and is cleared early in `SpideyAI0` whenever the swing record `input+0x70` is not held. It is not a good candidate for unconditional bypass.

Input update order is correct:
- `ReadAnalogueInput @ 0x004B1CF9` runs before every R1/R2 callsite;
- Hostage Situation's synthesized action 6 therefore exists in time for the same-frame retail R1 checks.

The scripted timing is also already elapsed-time based:
- `CScriptOnlyBaddy::AI` subtracts `field_80` from delay `+0x230`;
- synthesized-input worker type 3 subtracts `field_80` from its duration.
So the automatic ZIPLINE pulse is not simply 3x too short at native 60 Hz.

### Candidate fix
The existing R1/R2 diagnostic wrappers still call the untouched retail functions.

Added a compatibility-only stale-aim repair before each retail call:

`SpideyZipClearStaleModernAimGate`

It clears `field_8EA` only when all of these are true:
- `field_8EA != 0`;
- processed real aim control at `input+0x40` is **not held**;
- current camera mode is modern mode 3 / `CAMERAMODE_DEMO`;
- the modern aimed-locomotion sidecar does **not** currently own the player.

This combination represents a modern-camera-specific stale retail aim bit:
- genuine retail lookaround would use camera mode 7;
- genuine active modern aim still has the aim control held or its sidecar active.

Behavior:
- temporarily clear stale `field_8EA`;
- call retail R1/R2 unchanged;
- if retail zip still fails, restore `field_8EA=1`;
- if retail succeeds and enters the canonical zip path, keep it zero because retail itself expects zip to start from non-lookaround state.

No held-object, swing-latch, raycast, face-filter, target, web allocation, animation or physics condition is bypassed.

Telemetry:
- `web_zip_compat event=clear_stale_modern_aim ...`
- `web_zip_compat event=stale_aim_zip_success ...`

Existing direct traces remain:
- all 6 R1 callsites;
- all 5 R2 callsites;
- both `CheckZipWebAvailability` callsites;
- `player_state_trace`;
- `web_zip_physics`;
- exact L1A3 startup controller / synth-switch tracing.

### Static geometry conclusion
`field_A8` is explicitly documented/reconstructed in player crawling physics as the surface normal Spider-Man is aligned to.
`field_C84` is used throughout crawling/ground probing as the player's local away-from-surface/up basis.

Modern camera code does not author these fields.

This makes a renderer/DX11 projection problem unlikely for web-zip initiation.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 929,792 bytes
- pre-commit SHA-256:
  `3750fc34b2953a28f49e96a76f3b0178de9b279570fa5dacf99e296e1f6e28f6`

### Next runtime
Run:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

Best test:
1. Load **Hostage Situation** and let the opening automatic zip run without pressing zip.
2. If Spider-Man still hangs, press Space only afterward if needed to regain control.
3. Once controllable, try ordinary web-zip several times in known-valid geometry.

Interpretation:
- if `web_zip_compat ... stale_aim_zip_success` appears and zip works, modern manual-aim ownership was the shared blocker;
- if stale-aim compatibility never fires, inspect `web_zip_check` for which gate actually blocks;
- if R1 reaches `web_zip_availability` but rejects, the problem is geometric/collision rather than camera ownership;
- if availability succeeds and state becomes `0x40000` but travel still fails, continue in the already-instrumented zip animation/physics path.


## 2026-10-07 — Web-zip root cause isolated: native-60 special-move threshold canceled displacement

Latest tested runtime:
- log: `logs/20261007-023252/spidey-decomp.log`
- revision: `9be749173827aa546245d2f098e870fb010ea018`
- process exit: 0

### Runtime proof
The stale-modern-aim compatibility candidate did not fire:
- zero `web_zip_compat` records;
- `aim_flag=0` at the successful zip starts.

The input/gate/raycast path is working.

Scripted Hostage Situation R1 zip:
- tick 2428;
- `input60 held=1 pressed=1`;
- `CheckZipWebAvailability result=1 reason=accepted`;
- distance 765, max 3072;
- retail R1 returns 1;
- player state changes `0x00000001 -> 0x00040000`;
- animation becomes 260, then 270/271.

Manual zip later in the same run:
- R2 call 237 at tick 2822 returns 1;
- availability accepted;
- player state `0x00000002 -> 0x00040000`;
- animation 270.

Therefore both scripted and manual web-zip initiation are functional.

During the scripted zip:
- animation 270 reaches frame 13;
- AI authors zip velocity approximately `4095,982800,-8`;
- animation continues to 271 and finishes;
- player position remains exactly `-31195060,-3138734,-6942891` across the whole active zip interval;
- zero reconstructed `web_zip_physics` events appear because reconstructed `CPlayer::DoPhysics` is intentionally not installed globally.

### Live physics ownership
`game_patches()` intentionally does **not** call `patch_physics()`.
Retail `DoPhysics @ 0x00466CE0` remains live, with only narrow native-60 compatibility hooks from `SpideyInstallPlayerPhysics60Compat()`.

### Root cause
Retail special zip displacement at `0x00466D90+`:

```
0x00466D90  cmp [player+E1C], 0x40000
...
0x00466DA3  cmp anim, 270
0x00466DA9  cmp frame, 13
...
0x00466DB3  cmp anim, 271
...
0x00466DB9  push &mVel
0x00466DBC  call CVector::operator+=     ; mPos += mVel
0x00466DC1  mov esi,[player+80]          ; field_80
0x00466DC7  cmp esi,2
0x00466DCA  jle return
...
```

The old native-60 compatibility changed the immediate at `0x00466DC9` from 2 to 0.

For `field_80==1`, that was wrong:
- retail first adds the full zip velocity;
- patched comparison no longer returns;
- the catch-up path receives `field_80-2 == -1`;
- that path subtracts the same authored displacement;
- net displacement becomes zero.

This exactly matches the runtime: nonzero authored velocity + unchanged position.

### Corrected candidate
Removed the `0x00466DC9: 2 -> 0` special-move threshold patch.

Added one narrow direct-call hook:
- callsite: `0x00466DBC`
- retail target: `CVector::operator+= @ 0x004E7590`
- wrapper: `SpideyZipSpecialMoveAdd60`

Behavior:
- only for player state `0x40000`, animation 270/271, and `field_80==1`:
  - add `mVel >> 1` to `mPos`;
- all other elapsed-tick values call retail `operator+=` unchanged;
- retail's original `cmp field_80,2 / jle` remains untouched.

This preserves the authored 30-Hz displacement as two half-steps at native 60 Hz without entering the retail catch-up path with a negative count.

Telemetry:
- `web_zip_move_halfstep sample=... before=... velocity=... after=...`

The four generic native-60 threshold rewrites for normal movement / velocity restoration / fall / crawling remain unchanged.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 929,792 bytes
- pre-commit SHA-256:
  `5b0994759a80e649383d9e8a16bb1389c83abcecd57bb8c45127f3c4a4ced503`

### Next test
Run `TEST_LATEST_BUILD.bat`.

Primary:
1. Hostage Situation opening automatic ceiling zip.
2. Confirm Spider-Man actually travels off the ceiling instead of remaining suspended.

Secondary:
3. Try several manual web-zips.

Expected next log:
- successful R1/R2 -> `0x40000`;
- `web_zip_move_halfstep` records with changing before/after position;
- no long interval where animation 270/271 advances while position stays constant.


## 2026-10-07 — Live aimed web-zip wall tunneling root cause and fix

Live runtime:
- revision `4079f2cd79d69ea5f8b4704e1fc0aa283bb07d08`
- live log: `C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

### User-visible issue
Holding modern Aim and pressing Zipline could launch Spider-Man through nearby geometry / far past a wall, after which he could fall into the abyss.

### Exact runtime proof
At ticks 13440–13449:
- Aim processed record `input40 held=1`;
- Zipline processed record `input60 held=1`;
- retail `field_8EA=1`;
- R1/R2 both return 0 as retail intends.

Then modern aimed-locomotion masking engages while Aim remains physically held:
- `modern_manual_aim ... actual_aim_state=0 locomotion_mask=1`;
- retail `field_8EA` becomes 0 temporarily so locomotion can proceed.

At tick 13456:
- Aim is still held: `input40=1`;
- Zipline is pressed: `input60=1,1`;
- `field_8EA=0` only because the modern locomotion mask is active;
- untouched retail R1 therefore returns 1 and enters state `0x40000`.

Accepted retail zip target:
- player: `-11987681,-397312,-2740846`
- target: `-11987681,-4397056,-2740846`
- target normal: approximately +Y.

The target is purely along the legacy surface-normal axis and is **not** the modern camera reticle target. Nearby modern reticle telemetry points far away in X/Z.

The zip travel phase is intentionally a special no-collision displacement path, so allowing old surface-normal zip while the player visually believes they are aiming elsewhere can tunnel through intervening geometry.

### Compatibility fix
Added `SpideyZipBlockedByModernAimLocomotion`.

It blocks R1/R2 only when:
- physical Aim processed record is still held;
- retail `field_8EA` has been temporarily masked to 0;
- `gSpideyModernAimLocomotionMaskedPlayer == player`.

This exactly restores retail semantics across the modern locomotion mask:
- genuine retail aim already blocks on `field_8EA`;
- modern aim no longer leaks zip permission while the compatibility layer hides that bit.

Unaffected:
- quick/manual zip with Aim released;
- Hostage Situation scripted zip (no Aim held);
- raycast/availability;
- 60-Hz zip half-displacement fix;
- landing/cleanup.

Telemetry:
`web_zip_compat event=block_modern_aim_zip ...`

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-07 — Camera-directed aimed Zipline + full ±90° modern pitch + floor/ceiling camera boundaries

### Requested behavior
This batch supersedes the temporary policy from commit `7be83fa1` that blocked Zipline while modern Aim was held.

Desired behavior is now:
1. Aim + Zipline should zip toward the actual center-reticle surface.
2. Aim released + Zipline should retain the original retail quick/surface-normal Zipline.
3. Normal and manual-aim camera pitch should reach straight up and straight down (±90°).
4. Floors, ceilings and walls should constrain the modern camera arm so the camera does not cross world geometry, including while ceiling-crawling.

### Live evidence that motivated aimed Zipline
The `4079f2cd` live log showed:
- while retail aim was visible (`field_8EA=1`), Aim + Zipline was rejected;
- the modern aimed-locomotion mask temporarily hid `field_8EA`;
- R1 then accepted while physical Aim was still held;
- the accepted target was the legacy surface-normal target, not the reticle target;
- the no-collision zip travel path could therefore tunnel through geometry toward a point unrelated to what the player was aiming at.

The correct compatibility behavior is not to block aimed Zipline, but to provide a camera-directed target query.

### Camera-directed R1 aimed Zipline
Added `SpideyTryModernAimedR1Zip`.

Activation:
- processed Aim record `input+0x40` held;
- processed Zipline record `input+0x60` held;
- modern manual aim effectively active;
- gameplay mode-3 camera active.

Target acquisition:
- refresh the same center-camera ray used by the visible reticle via `SpideyModernAimApplyCameraPoint`;
- cast from final camera position to that ray endpoint using retail world collision:
  - `M3dColij_InitLineInfo @ 0x004524C0`;
  - `M3dZone_LineToItem @ 0x004549A0`;
- if the reticle ray finds no world surface, aimed Zipline returns false instead of falling back to legacy surface-normal zip;
- convert `camera-hit - player-position` to a normalized 4096-scale direction;
- temporarily substitute only `player->field_C84` with that direction and clear retail aim gate `field_8EA`;
- call untouched retail R1 `0x004C0EE0`;
- immediately restore Spider-Man's real `field_C84`.

Retail still owns:
- held-object / other zip gates;
- player-to-surface raycast;
- maximum distance;
- non-zippable face rules;
- final hit position and normal;
- web creation;
- animation 270;
- state `0x40000`;
- travel/landing state machine.

Important RE correction:
- R1 register `EDI` is `player+8` / `mPos`, so the copy to `player+0x558` is a position snapshot, not a copy of `field_C84`.
- Temporarily steering `field_C84` therefore does not poison that downstream position snapshot.

### Aimed availability policy
Retail `CheckZipWebAvailability @ 0x004C30D0` contains an orientation check tied to the old surface-normal-only R1 design.

For an active camera-directed aimed R1 only, the wrapper now:
- preserves retail minimum distance;
- preserves retail maximum distance;
- preserves `pFace[3] & 0x40000` non-zippable rejection;
- treats acquisition through the center-camera ray as the orientation criterion.

Quick/non-aimed Zipline uses retail availability unchanged.

Telemetry:
- `web_zip_aimed event=no_camera_hit ...`
- `web_zip_aimed event=retail_r1 ... camera_hit=... aimed_dir=... retail_target=... retail_normal=...`
- `web_zip_availability ... reason=accepted_aimed_camera`

### Aim exit/re-entry after aimed zip
A successful aimed zip deliberately exits manual aim before retail zip travel owns state `0x40000`.

Added:
- `gSpideyModernAimZipReleaseLatchPlayer`;
- `SpideyModernAimDropForZip`;
- `SpideyModernAimValidateZipReleaseLatchAtFrameEnd`.

Behavior:
- clear retail `field_8EA`;
- discard any active aimed-locomotion mask without restoring aim=1;
- suppress immediate `EnterLookaroundMode` re-entry while the physical Aim key remains held;
- clear the latch when Aim is released.

This prevents manual-aim control from fighting animation/travel after a successful aimed zip.

### Full ±90° camera pitch
The previous modern camera did not store pitch as an angle. It stored a Y-distance offset clamped to `-480..260`, then derived angle from fixed horizontal distance. That architecture cannot reach a true vertical view.

Modern mode-3 camera now stores:
- `gSpideyModernCameraPitch` in the game's 4096-unit full-circle angle convention;
- `gSpideyModernCameraRadius` as the orbit radius.

Pitch clamp:
- `-1024..+1024` = `-90°..+90°`.

On modern-camera acquisition:
- preserve the current retail view by deriving radius and signed pitch from the existing XZ/Y distances.

Each update:
- mouse/right-stick modify explicit pitch;
- solve:
  - horizontal arm = `cos(pitch) * radius`;
  - vertical arm = `-sin(pitch) * radius`;
- publish coherent retail globals:
  - `0x00548860` XZ distance;
  - `0x00548864` Y distance;
  - `0x0054885C` radial distance;
  - `0x00548858` vertical angle.

At the exact ±90° clamp, horizontal arm is kept at a minimum value of 1 rather than literal zero to avoid legacy normalization/divide edge cases. Visually this is effectively straight up/down.

Manual aim uses the same unified mode-3 orbit, so the same ±90° range applies in both normal look and manual aim.

### Modern camera floor/ceiling/wall collision
Added `SpideyModernCameraClipToWorld`.

After mode-3 generates the desired modern camera position and before the shared retail postprocess computes final orientation:
- cast from camera focus toward desired camera position using retail world-line collision;
- start the ray 16 world units into the playable side of Spider-Man's current surface using `field_C84`, avoiding self-contact with the floor/ceiling/wall plane;
- if geometry blocks the camera arm, shorten it to the first hit minus a 24-world-unit safety margin;
- then run retail camera postprocess/orientation normally.

This treats:
- walls;
- floors;
- ceilings
as equivalent camera boundaries and specifically covers ceiling-crawl cases where the previous unrestricted vertical orbit could put the camera above/through the ceiling.

Telemetry:
`modern_camera_collision hit=... pitch=... start=... desired=... contact=... clipped=...`

Installer telemetry now reports:
- `pitch_limit_units=1024 pitch_limit_degrees=90`;
- `collision=world_arm_clip`;
- collision margin/start inset.

### Scope preserved
Untouched:
- native-60 web-zip half-displacement fix at retail `0x00466DBC`;
- quick Zipline when Aim is released;
- Hostage Situation synthesized Zipline path;
- Mysterio boss retail-camera takeover;
- scripted player-input camera surrender;
- legacy FX View-matrix anchoring fixes.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 929,792 bytes
- pre-commit SHA-256:
  `4c21d00ef1f6c152b9dedb062c959c29966402283aa9474825135d81d9021f2d`

### Runtime test
Run:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

High-value checks:
1. Normal camera: look continuously up and down and verify both ends reach effectively straight vertical.
2. Hold manual Aim and repeat; reticle should stay on the same camera ray through the full pitch range.
3. Stand near a ceiling/floor/wall and deliberately rotate the camera into it; camera should shorten its arm instead of crossing the surface.
4. Ceiling crawl and look upward into the ceiling; camera must remain on the playable side.
5. Hold Aim, put the reticle on a visible zippable wall/ceiling, press Zipline (keyboard `3` in current config); Spider-Man should zip toward that reticle surface.
6. Aim into open space and press Zipline; no legacy fallback zip should occur.
7. Release Aim and press Zipline; original quick/surface-normal retail zip should still work.
8. Hostage Situation opening scripted zip should remain functional.


## 2026-10-07 — Live-session camera clipping + stale Zipline reticle fixes

### Live session access
While the game is running, the harness session directory can remain empty because the consolidated log is archived only after process exit.

For the active test started at `20261007-031631`, the live source is:
`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

Live runtime revision:
`ceb17d4cdc5ef9c6758d907894ab634909ccf982`

### Runtime observations
Aimed Zipline works in the live session:
- tick 3248: `web_zip_aimed ... result=1`
- tick 3457: `web_zip_aimed ... result=1`

The player-state trace shows aim state clears after zip:
- `aim=0`
- `field_8EA=0`

Therefore the reticle persistence is not an aim-state failure.

Camera collision is also firing:
- 64 logged `modern_camera_collision` hits before the telemetry cap;
- hits include ceiling-crawl state and world contacts.

So the camera bug is not “collision ray never sees geometry.”

### Reticle root cause
`CPlayer::RenderLookaroundReticle` draws whenever `field_DE4 != 0`.

Modern aim explicitly sets:
- `field_DC0` = reticle world point;
- `field_DE4 = 1`.

Retail `ExitLookaroundMode` normally clears:
- `field_8EA`;
- `field_DE4`;
- `Screen_TargetOn(false)`.

Our aimed Zipline path intentionally bypasses normal retail lookaround exit semantics so it can enter state `0x40000` cleanly. It cleared `field_8EA`, but did not clear `field_DE4`.

Retail R1/R2 themselves do not set `field_DE4`.

Quick Zipline updates `field_DC0` to its new zip target. Therefore if `field_DE4` was stale from a prior aim session, even a non-aimed quick zip makes the stale reticle appear at the new zip destination. This exactly matches the reported “reticle/decal stays where I zipped to” behavior.

### Reticle fix
- `SpideyModernAimDropForZip` now also clears `field_DE4` and calls `Screen_TargetOn(false)`.
- Every successful R1 and R2 Zipline wrapper now clears `field_DE4` and calls `Screen_TargetOn(false)`, regardless of aimed vs quick path.

This makes successful Zipline a hard cleanup boundary for the lookaround target marker.

### Camera clipping root cause
The modern world-arm clamp was executed before calling:
`CCamera_MoveToDesiredPos @ 0x00416B10`.

The camera AI caller proves the order:
- `0x00418458: call CCamera_MoveToDesiredPos`
- `0x0041845D+`: immediately uses final `mPos` and `field_144`
- `0x0041846C: call Utils_CalcAim @ 0x004E62D0`

So our earlier sequence was:
1. mode-3 chooses desired camera;
2. custom collision clamp shortens `mPos`;
3. retail `MoveToDesiredPos` moves `mPos` again;
4. retail computes orientation from the moved position.

This allowed the final rendered camera to cross geometry after our successful collision hit.

### Camera fix
`SpideyModernAimCameraPostprocess` now:
1. applies framed focus for manual aim if needed;
2. calls retail `CCamera_MoveToDesiredPos`;
3. then runs `SpideyModernCameraClipToWorld` on the final `camera->mPos`;
4. returns to retail caller, which immediately runs `Utils_CalcAim` from the clipped final position.

Thus walls/floors/ceilings now constrain the final camera position rather than an intermediate one, and retail orientation/publish remains coherent.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-07 — Wall/ceiling manual-aim reticle framing

### User-visible result before this candidate
The previous camera/Zipline batch is nearly correct:
- final-position camera collision is working much better;
- aimed Zipline works;
- quick Zipline works;
- stale post-Zipline reticle cleanup is fixed;
- floor manual aim behaves correctly.

Remaining issue:
- while ceiling-crawling, manual aim reticle stays visually centered on Spider-Man instead of giving a usable world target;
- wall-crawl likely has the same class of issue.

### Live-log proof
Active manual-aim ceiling sample:
- player body: `-6990084,-4009898,4049717`
- camera: `-6990068,-4337594,4049717`
- old framed focus: `-6990084,-4403114,4049717`
- `wall=0 ceiling=1`.

The old framing code always did:
`focus = player->mPos + (0,-96*4096,0)`.

In this ceiling case, camera->player is already almost exactly vertical, so the fixed world-Y framing vector is nearly collinear with the camera ray. The visible reticle therefore projects back across Spider-Man instead of separating from his body.

### Fix
Floor behavior is intentionally preserved exactly.

For wall/ceiling states only (`field_8E8 || field_8E9`):
1. compute the normalized camera->player view direction;
2. project engine world-up `(0,-1,0)` into the camera image plane;
3. normalize that projected vector to a 4096-scale screen-up axis;
4. apply the existing 96-world-unit framing offset along that screen-up axis rather than fixed world -Y.

At a mathematically vertical view, world-up projection is undefined. The modern orbit already retains a tiny horizontal arm at the ±90° pole, so the fallback uses that horizontal radial direction with the correct pole sign.

This gives a framing offset that is always perpendicular to the view ray, preventing the reticle from collapsing onto Spider-Man on ceilings/walls.

Unchanged:
- ordinary floor manual aim retains exact `(0,-4096,0)` framing axis;
- ±90° camera pitch;
- final-position wall/floor/ceiling camera collision;
- aimed and quick Zipline behavior;
- native-60 Zipline displacement;
- reticle cleanup on successful Zipline.

### New telemetry
Manual-aim framing records now include:
- `framing_mode=0` => preserved floor/world-up path;
- `framing_mode=1` => projected screen-up path;
- `framing_mode=2` => exact vertical-pole radial fallback;
- `framing_axis=x,y,z` => actual normalized 4096-scale framing direction.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-07 — Surface-only first-person manual aim (wall/ceiling)

### Requested policy
The projected third-person wall/ceiling framing from `1e839d2` did not solve the remaining reticle problem.

New explicit policy:
- **floor manual aim:** preserve the current validated third-person mode-3 camera unchanged;
- **wall or ceiling manual aim:** switch to a true first-person/front camera;
- releasing Aim returns to the pushed third-person crawler camera;
- aimed Zipline continues to use the exact visible center-camera ray.

### Retail RE proof
`CPlayer::EnterLookaroundMode @ 0x004C3580` is already a real first-person/front-camera entry path:
- sets `player+0x8EA = 1`;
- calls `CCamera::PushMode @ 0x00416720`;
- `0x004C370B: push 7`;
- calls `CCamera::SetMode @ 0x004167F0`;
- camera enum mode 7 is `CAMERAMODE_FRONT`.

The previous modern-manual-aim installer globally patched that `push 7` to `push 3`, which prevented retail FRONT mode from ever being used.

`CPlayer::ExitLookaroundMode @ 0x004C3810`:
- clears retail aim/reticle state;
- calls `CCamera::PopMode @ 0x00416780`.
Therefore normal Aim release already provides the correct first-person -> third-person lifecycle.

### Camera dispatcher proof
`CCamera::AI @ 0x00417CB0` dispatch mapping:
- mode 3 -> `CM_Normal @ 0x00418E00`;
- mode 7 -> dispatcher target `0x00418456`, immediately after all mode-specific generators.

Mode 7 therefore has no independent camera-AI generator. Its first-person camera position/orientation is built by `CPlayer::SetupLookaroundCamera @ 0x004C38A0`; camera AI then runs shared:
- `CCamera_MoveToDesiredPos @ 0x00416B10`;
- `Utils_CalcAim @ 0x004E62D0`;
- `LoadIntoMikeCamera @ 0x00416A20`.

### Retail look-angle RE
Retail Setup consumes:
- `0x006A818C`: pitch target;
- `0x006A82B4`: smoothed pitch;
- `0x006A7FFC`: body-relative yaw target;
- `0x006A8D54`: smoothed body-relative yaw;
- `0x006A8D44`: previous effective body heading.

Setup feeds the smoothed pitch/yaw to `M3dMaths_RotMatrixYXZ @ 0x0046E730`.

Old `SpideyAI0` updates those targets from movement axes, causing the historical WASD-cursor/body-twist conflict.

### New surface first-person controller
Added:
- `SpideySurfaceFirstPersonAimPrepare`;
- `SpideySurfaceFirstPersonAimRelease`.

On wall/ceiling + active Aim + `CAMERAMODE_FRONT`:
1. seed world yaw/pitch from the current visible camera ray;
2. update yaw/pitch from the same modern mouse/right-stick deltas and sensitivity used by mode-3 camera;
3. clamp pitch to ±1024 game-angle units (±90 degrees);
4. convert world yaw to body-relative yaw using `CPlayer::GetEffectiveHeading`;
5. write target and smoothed retail globals together, eliminating old keyboard-era look smoothing/lag;
6. run retail `SetupLookaroundCamera` for true first-person placement/collision.

Telemetry:
`modern_manual_camera event=surface_first_person_angles ...`

### Dynamic mode policy
The old binary patch:
`0x004C370B: push 7 -> push 3`
is removed.

`SpideyModernAimEnterLookaroundMode` now:
- calls retail Enter normally;
- keeps mode 7 when `field_8E8 || field_8E9` (wall/ceiling);
- switches immediately back to mode 3 only on ordinary floor aim.

Transitions while Aim stays held are also handled:
- floor -> wall/ceiling: mode 3 -> FRONT;
- wall/ceiling -> floor: FRONT -> mode 3.

### Movement policy
Floor manual aim keeps the existing modern locomotion mask and third-person movement behavior.

Surface first-person aim deliberately does **not** hide `field_8EA`.
Retail aim owns the player while in first-person, so crawling movement pauses while Aim is held. This prevents the old simultaneous movement/lookaround controller conflict.

Ordinary wall/ceiling crawling with Aim released is untouched.

### Reticle / aimed Zipline
`SpideyModernAimApplyCameraPoint` now accepts:
- floor mode 3 -> existing framed TPS focus;
- wall/ceiling mode 7 -> final retail FRONT camera focus directly.

The shared camera postprocess rebuilds `field_DC0` after the final camera move in both policies.

`SpideyTryModernAimedR1Zip` now accepts surface mode 7 as well as floor mode 3. Aim+Zipline therefore continues to use the exact visible first-person center ray.

On successful aimed Zipline from FRONT mode:
- call retail `CCamera::PopMode @ 0x00416780`;
- clear first-person sidecar;
- clear aim/reticle state;
- then let existing state `0x40000` travel own the player.

### Legacy action isolation
Retail `SetupLookaroundCamera` also contains its original lookaround zip/swing action tail controlled by `player+0x54F`.

RE:
- `SpideyAI0 @ 0x004B1EA1` sets `+0x54F = 1`;
- Setup reads it around `0x004C4411/0x004C4535`;
- Setup normally clears it on all exits.

For the surface first-person camera call, `+0x54F` is forced to zero so retail Setup remains camera/reticle-only and cannot compete with the already-working custom camera-directed aimed Zipline. It is intentionally left zero after the call, matching retail Setup cleanup semantics.

### Previous wall/ceiling framing
The projected-screen-up framing added in `1e839d2` remains in source but is no longer used for active wall/ceiling manual aim because those states now run mode 7. Floor behavior remains mode-0 fixed world-up framing exactly as before.

### Validation
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-07 — Robust new-chat handoff checkpoint: surface first-person aim frontier

Dedicated recovery document:

`docs/NEW_CHAT_HANDOFF_2026-10-07_SURFACE_FIRST_PERSON_AIM_FRONTIER.md`

Authoritative **behavior/source** frontier before the docs-only handoff commit:

`56b191cef67887fdd2562a5ac99f0aeb01247820`
— `Use first person aim on walls and ceilings`

Important distinction:
- `1a82373aa03a81b779de5ca30168323c7c467081` is the user-confirmed “damn near perfect” camera/Zipline baseline;
- `1e839d2ef3ccde2bcb6aa28c9bfbe08f0523062a` is the superseded projected-screen-up wall/ceiling framing experiment;
- `56b191ce...` is the new surface-first-person wall/ceiling manual-aim candidate and is **pending runtime validation** at handoff time.

The prior reported ceiling drop-attack “crash” on `1e839d2` is not proven as a code fault:
- `logs/20261007-033631/spidey-decomp.log` exits `0xC000013A`, not access violation;
- drop attack state/animations progress past the reported moment;
- a later same-revision run `logs/20261007-034231/spidey-decomp.log` exits cleanly with code 0.

Immediate next-chat action:
1. pull newest runtime log directly;
2. verify `[SESSION] revision=`;
3. if it is `56b191...`, inspect `modern_manual_camera event=surface_first_person_angles`;
4. combine with the user's visual verdict on wall/ceiling first-person aim and aimed Zipline;
5. do not regress the user-confirmed floor camera/Zipline fixes from `1a82373...`.

A maximum-context handoff ZIP is being created from this checkpoint with:
- start-here recovery note;
- current docs;
- key source snapshot;
- Git/commit/diff ledgers;
- build/test scripts;
- high-value camera/web-zip runtime logs;
- SHA-256 manifest.


## 2026-10-07 — Surface first-person aim runtime verdict + no-flip/movement candidate

### Runtime evaluated

Live consolidated log:

`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

Runtime revision:

`7f6aeadacf098bb871b0b5f62a610ad80ab56319`

This docs HEAD contains behavior commit:

`56b191cef67887fdd2562a5ac99f0aeb01247820`

The run exited cleanly:

`[SESSION] exit_code=0`

### What the log proved

The `56b191ce` candidate did enter retail FRONT / mode 7 for both crawl surface types:

- ceiling samples: `wall=0 ceiling=1 mode=7`;
- wall samples: `wall=1 ceiling=0 mode=7`;
- `modern_manual_camera event=surface_first_person_angles` responded to real mouse deltas;
- camera-directed aimed Zipline still succeeded once:
  `web_zip_aimed event=retail_r1 attempt=1 ... success=1 result=1`.

Therefore the failure was not “surface first-person code did not run.”

### User-visible failure / corrected interpretation

User reported:

- the camera flips into a Spider-Man/surface-relative perspective instead of simply entering first person;
- the camera should **not rotate/flip at Aim entry**;
- while first-person aiming on a wall/ceiling, Spider-Man must still be able to crawl/move.

This identifies the design problem in `56b191ce`:

- retail `CAMERAMODE_FRONT (7)` is not a neutral first-person placement layer;
- `SetupLookaroundCamera` owns orientation in Spider-Man's crawl/surface basis, producing the visible flip;
- keeping `field_8EA` visible and handing control to retail aim also intentionally stopped crawl locomotion.

### New candidate policy

Retail FRONT ownership is removed from active modern manual aim.

All modern manual aim now remains in ordinary camera mode 3:

- **floor:** preserve the already validated third-person manual-aim camera unchanged;
- **wall/ceiling:** preserve the exact current mode-3 yaw/pitch and move only the final camera position into first person;
- no surface-relative orientation flip;
- mouse/right-stick continue through the existing mode-3 modern camera controller;
- wall/ceiling locomotion uses the same scoped `field_8EA` masking path already proven for moving while floor aiming;
- retail `SetupLookaroundCamera` stays bypassed during modern aim, so WASD/left-stick are not consumed by the legacy cursor controller.

### Custom first-person camera pose

Added:

`SpideySurfaceFirstPersonApplyCameraPose`

The helper runs only when:

- manual Aim is effectively active;
- `field_8E8 || field_8E9`;
- camera mode is 3.

Placement:

1. start from `player->mPos`;
2. use `player->field_89C` to rotate a local -Y eye/head offset into world space;
3. eye-height offset: 72 world units;
4. add 12 world units along `field_C84` to remain on the playable side of the crawl surface/body;
5. add 8 world units along the unchanged visible view ray to keep the camera clear of Spider-Man's face;
6. set `camera->mPos` to that eye position;
7. set `camera->field_144` 2048 world units along the same view ray.

View direction:

- if the modern mode-3 camera is active, derive forward directly from `gSpideyModernCameraYaw/gSpideyModernCameraPitch`;
- otherwise normalize the currently visible `camera->mPos -> field_144` ray.

This means pressing Aim changes **position only**, not orientation.

New telemetry:

`modern_manual_camera event=surface_first_person_pose ... movement_policy=modern_masked`

### Movement change

Removed the `56b191ce` surface special-case in `SpideyModernAimCheckForwards` that returned directly to retail with `field_8EA` visible.

Wall/ceiling aim now uses the normal modern aim locomotion mask:

- hide held Aim only while retail movement evaluates;
- keep E2D/E2E movement axes intact;
- keep the effective-aim sidecar active;
- restore retail aim state when movement/Aim is released.

### Reticle / aimed Zipline

Surface first-person is now recognized as:

`wall_or_ceiling && camera->mCameraMode == CAMERAMODE_DEMO`

instead of mode 7.

The final first-person camera pose is applied before the shared caller rebuilds orientation, then:

- `field_DC0` is rebuilt from the final eye-position camera ray;
- aimed Zipline accepts the same surface mode-3 policy;
- floor aimed Zipline behavior is unchanged.

### Camera collision policy for this candidate

The third-person orbit-arm collision clamp is skipped while surface first-person aim is active, because its focus-to-orbit-arm assumptions do not apply to an eye-position camera.

The first-person eye pose is intentionally kept close to Spider-Man and offset to the playable side of the crawl plane. If runtime shows eye-camera clipping near adjacent geometry, add a dedicated short eye-position collision probe rather than reusing the third-person arm clamp.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS
- `Release/spider.dll`: 933,888 bytes
- SHA-256:
  `e9b01b13cea6c3f60e985a0c7725cf9355878a85e513ecc376f45521f0e4b032`

### Next runtime test

Use the normal local test workflow and validate in one session:

1. crawl on a ceiling without Aim and place the camera at an obvious non-default yaw/pitch;
2. press/hold Aim;
3. camera should move into first person **without changing/flipping the view direction**;
4. move mouse through yaw/pitch; reticle should stay on the center view ray;
5. hold Aim and use WASD/left stick; Spider-Man should continue crawling;
6. release Aim; ordinary third-person crawler camera should resume;
7. repeat on a wall;
8. test Aim + Zipline once from the first-person surface view;
9. briefly verify floor third-person aim is unchanged.

Expected new marker:

`modern_manual_camera event=surface_first_person_pose`

Preserve the user-confirmed `1a82373...` floor/camera/Zipline baseline while evaluating this candidate.


## 2026-10-07 — Surface first-person camera VALIDATED; live combat hang narrowed

Runtime revision:

`91d5e3d8468234ea8a053c09a224041378a6781d`

Live log:

`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

### Surface first-person result

User report:

> camera is perfect now

The log confirms the new position-only first-person policy ran on both walls and ceilings:

`modern_manual_camera event=surface_first_person_pose ... mode=3 ... movement_policy=modern_masked`

Examples include:

- wall: `wall=1 ceiling=0`;
- ceiling: `wall=0 ceiling=1`;
- mode remains `3` throughout;
- view forward changes continuously under modern camera input.

Treat commit `91d5e3d8...` surface first-person behavior as **runtime validated**. Do not reopen the old retail FRONT/mode-7 design.

### New freeze report

User reported the game froze while fighting.

Console also displayed:

`D3D Error: [C:\backup\SpideyPC\SpideyPC\D3d\PCTex.cpp], line 1740: Unknown (00000001)`

### D3D warning is not the freeze cause

The consolidated log captured that D3D error at log line ~1636:

`[DXERROR] D3D error=0x00000001 ... PCTex.cpp line=1740 caller_return=0x0050F2BC call_site=0x0050F2B7`

The game then continued for more than 5,400 additional log lines and thousands of gameplay frames before the freeze.

The companion texture log also completed all texture creation entries normally and stopped long before the freeze point.

Therefore do **not** treat the PCTex warning as causal for this hang.

### Hang behavior

There is no normal:

`[SESSION] exit_code=...`

The log simply stops.

Live process inspection while frozen showed:

- `SpideyPC.exe` still alive;
- Windows reports the process responding;
- process CPU increased by ~1.58 CPU-seconds over ~1.5 wall-seconds;
- one game thread is in `Running` state while the other sampled threads are waiting/suspended.

This is a **busy/infinite-loop style hang**, not a blocking deadlock and not a clean crash.

A non-destructive minidump/current-EIP capture was attempted, but Windows denied thread-context/dump access with access-denied / `0x80070005` at Local Commander's current integrity level. The frozen game was not terminated by the diagnostic attempt.

### Freeze is unrelated to the new surface camera

At the final freeze boundary:

- `wall=0`;
- `ceiling=0`;
- `aim=0`;
- camera mode is normal mode 3;
- no `surface_first_person_pose` activity is present near the stop.

So the validated wall/ceiling first-person work is not the active code path at the hang.

### Combat context

The active target near the freeze is a normal:

`name=thug`

There are no live Mysterio laser/soft-spot runtime events in this session.

This is therefore a generic/normal combat freeze, not the prior Mysterio laser path.

### Strongest boundary found

Immediately before the log stops, modern auto-aim targeting transitions from a valid thug target to no target:

`camera_web_target event=select ... source=render_camera_no_target ... target=0x00000000 ...`

The wrapper itself returned and logged successfully.

Retail disassembly immediately after the patched `SelectTargetBaddy` call at:

`0x004C5B2F`

shows the null-target branch:

- `cmp eax, 0`;
- `je 0x004C5C1E`;
- load existing helper at `player+0x878`;
- if present, call `CBody::DeleteFrom @ 0x00460280`;
- then invoke its virtual destructor;
- clear `player+0x878`.

The helper is allocated as a plain 0xF8-byte `CBody`, initialized with the retail item group string `"items"`, and used by the auto-aim path.

The decompiled base `CBody::~CBody` destroys `mpShadow`.

Therefore the current leading boundary is:

**target becomes invalid / lost -> auto-aim helper at player+0x878 is detached and destroyed -> busy spin occurs before the next gameplay heartbeat.**

This is not yet proof of which exact substep spins. Next diagnostic should distinguish:

1. entry/return of the `CBody::DeleteFrom` call at `0x004C5AF9 / 0x004C5C2F`;
2. entry/return of the following virtual destructor;
3. `CBody::~CBody -> delete mpShadow`;
4. heap/object cleanup immediately after destruction.

Do not bypass the cleanup speculatively until the exact substep is observed.

### Next combat-hang diagnostic

Add behavior-neutral breadcrumbs around the two auto-aim helper cleanup sites in `sub_4C5AA0` / SelectAutoAimTarget:

- replacement-old-target cleanup around `0x004C5AF9`;
- lost-target cleanup around `0x004C5C2F`;
- virtual destructor entry/return if practical;
- capture helper pointer, vtable, next/previous list links, `mCBodyFlags`, `mpShadow`, list head, and target pointer.

The next reproduction should identify the exact spinning cleanup stage without changing combat behavior.


## 2026-10-07 — Auto-aim target-helper cleanup hang diagnostic candidate

Built from validated surface-first-person baseline:

`91d5e3d8468234ea8a053c09a224041378a6781d`

with docs checkpoint:

`8c16191e5dc9e22a8b3cb0b76ffb0bc729948a90`

### Purpose

The previous live freeze was narrowed to:

`SelectTargetBaddy returns NULL -> helper at player+0x878 is detached/destroyed -> CPU spin before next gameplay heartbeat`

This candidate adds **behavior-neutral telemetry only** around every cleanup boundary needed to determine exactly which call does not return.

No target scoring, combat logic, camera behavior, shadow behavior, object deletion behavior, or heap behavior is intentionally changed.

### Confirmed retail cleanup chain

Retail `sub_4C5AA0`:

- replacement-old-target cleanup:
  - direct `CBody::DeleteFrom` call at `0x004C5AF9 -> 0x00460280`;
- lost-target cleanup:
  - direct `CBody::DeleteFrom` call at `0x004C5C2F -> 0x00460280`;
- helper virtual deleting destructor through base `CBody` vtable slot 0.

Confirmed base `CBody` vtable:

`0x0053BBD4`

slot 0:

`0x004601E0` — scalar deleting destructor.

Confirmed scalar deleting destructor chain:

- `0x004601E0` scalar deleting destructor;
- direct base destructor call at `0x004601E3 -> CBody::~CBody @ 0x00460200`;
- if delete flag is set, direct heap delete call at `0x004601F0 -> 0x00458210`.

Confirmed `CBody::~CBody @ 0x00460200`:

- reads `body+0xCC == mpShadow`;
- if non-null, invokes the shadow object's virtual deleting destructor;
- then returns.

This makes `body_destructor_enter` without `body_destructor_return` a high-signal indicator that the spin is inside the marker's shadow deletion path.

### Added diagnostic hooks

New installer:

`SpideyInstallAutoAimCleanupDiagnostics()`

It installs:

1. `0x004C5AF9` -> `SpideyAutoAimCleanupDeleteFromReplace`
2. `0x004C5C2F` -> `SpideyAutoAimCleanupDeleteFromLost`
3. `0x004601E3` -> `SpideyAutoAimCleanupBodyDestructor`
4. `0x004601F0` -> `SpideyAutoAimCleanupMemDelete`
5. `CBody` vtable slot `0x0053BBD4` -> `SpideyAutoAimCleanupScalarDeletingDestructor`

The vtable patch validates the expected retail slot value `0x004601E0` and fails closed on mismatch.

The two direct `DeleteFrom` call hooks use exact direct-call target validation and fail closed on mismatch.

### Candidate tracking / noise control

The direct `DeleteFrom` wrapper records the helper being cleaned as:

`gSpideyAutoAimCleanupCandidate`

and records whether the operation is:

- `replace_old_target`
- `lost_target`

Destructor/base-destructor/heap-delete wrappers only emit detailed telemetry when the object equals that candidate.

All wrappers forward to the original retail/currently-patched target and return its result unchanged.

### New log marker

Each boundary writes:

`[TIMING] autoaim_cleanup ...`

Important fields:

- `seq`
- `event`
- `stage`
- `body`
- `list`
- `list_head`
- `vtable`
- `next`
- `previous`
- `cbody_flags`
- `shadow`
- `destructor_flags`
- guarded read-success flags

Possible event progression for a normal cleanup:

1. `deletefrom_enter`
2. `deletefrom_return`
3. `scalar_destructor_enter`
4. `body_destructor_enter`
5. `body_destructor_return`
6. `operator_delete_enter`
7. `operator_delete_return`
8. `scalar_destructor_return`

### How to interpret the next freeze

Last marker is:

- `deletefrom_enter`
  - spin is inside `CBody::DeleteFrom` / unsuspend-list manipulation;
- `deletefrom_return`
  - spin occurs between detach and virtual destructor dispatch;
- `scalar_destructor_enter`
  - scalar deleting destructor entered but has not reached base destructor;
- `body_destructor_enter`
  - spin is inside `CBody::~CBody`, whose only meaningful owned-object cleanup is `delete mpShadow`; this strongly points at target-marker shadow destruction;
- `body_destructor_return`
  - base destructor completed; next suspect is the scalar destructor's delete branch;
- `operator_delete_enter`
  - spin is inside `Mem_Delete / heap deletion`;
- `operator_delete_return` but no `scalar_destructor_return`
  - heap delete returned; scalar destructor epilogue/return is implicated;
- complete eight-event sequence
  - this helper cleanup returned successfully and the hang lies later.

### Toolchain correction

Initial implementation used `__thiscall` function-pointer typedefs. The matching VC6 compiler rejects that form with C4234.

They were converted to the repository's already-proven x86 pattern:

- wrapper/function pointer uses `__fastcall`;
- ECX carries `this`;
- dummy EDX argument occupies the fastcall second register;
- original explicit arguments remain on the stack.

No ABI behavior change is intended.

### Validation

- `git diff --check`: PASS before build
- forced-clean matching VC6 build: PASS
- output:
  - `Release/spider.dll`
  - size: 937,984 bytes
  - SHA-256:
    `0bdbbb06034ddefdf5e8cdbf5f6e23cc2e147b6a5b407ea937d48ea1593281f1`

### Next runtime test

Use normal `TEST_LATEST_BUILD.bat` workflow.

Primary test:

1. enter a normal thug/henchman fight;
2. fight normally until target acquisition/loss happens repeatedly;
3. if the game freezes, leave it frozen long enough for the last breadcrumb to flush;
4. the live `spidey-decomp.log` can then be pulled directly through Local Commander.

No need to retest the wall/ceiling camera beyond a brief sanity check; that behavior is already user-validated.


### Final revision-stamped diagnostic artifact

Behavior commit:

`3eb5747307cd296d14d2761afb67d07fba2d4d7b`

After committing/pushing the source, a second forced-clean matching VC6 build was performed with:

`RUNTIME_VERSION = 3eb5747307cd296d14d2761afb67d07fba2d4d7b`

Verification of `Release/spider.dll`:

- size: 937,984 bytes
- SHA-256:
  `69f30643e93ad6f77b23ee1d711b02656821efcc5e556c7c07987336657cc68c`
- the full behavior commit string is present in the binary: PASS

An attempt to use `TEST_LATEST_BUILD.ps1 -PrepareOnly` confirmed the correct revision but hit the normal Windows UAC boundary for writing into:

`C:\Program Files (x86)\Activision\Spider-Man`

No forced/unsupported install was attempted.

The repository `Release/spider.dll` is therefore the exact revision-stamped diagnostic artifact. The user should use the normal `TEST_LATEST_BUILD.bat` workflow and accept its elevation prompt so the same source is rebuilt/installed through the supported path.

The tracked `runtime_version.h` was restored to its repository-empty state after producing the stamped artifact.


## 2026-10-07 — Repeated ceiling-smash combat-freeze root cause and fix candidate

### Frozen runtime evidence

Runtime revision: `a41372adca719085eeeb247333708ead915370ae`.

The auto-aim cleanup diagnostic completed all eight cleanup stages repeatedly, including the final target-loss event immediately before the freeze. The target-helper detach/destructor/heap path is therefore ruled out.

The decisive difference is the ceiling-smash landing state:

- healthy 133 -> 134 landings: `state=0x00000001`;
- frozen landing: `tick=2896 state=0x01000000 anim=134 collision=0x00000002`.

So animation 134 is normal; the failure is that the special smash state does not return to stand mode.

### Retail state and bad branch

`0x01000000` is entered by `CPlayer::CheckCeilingJumpingSmashPunch @ 0x004C0B80`.

It records:
- `field_8C8 = previous field_8C4`;
- `field_8C4 = gTimerRelated`;
- `field_E1C = 0x01000000`;
- animation 133 for the ceiling-smash variant.

`SpideyAI0 @ 0x004B13F0` handles this state at `0x004B631E`.

The landing recovery at `0x004B6570..` computes:

`field_8C4 - field_8C8`

and compares it with `0x78` (120 canonical timer ticks) at `0x004B657E`.

Normal/fall-through recovery does:
- `field_8DC = 0`;
- `field_AE4 = 1`;
- `field_AE5 = 0`;
- `CPlayer::SwitchToStandMode @ 0x004BE4B0`.

If interval <120, the `jl` at `0x004B6581` instead reaches `0x004B65A3`, which only writes:

`field_8DC = 0x29A`

and leaves `field_E1C = 0x01000000`.

### Runtime proof the bad branch fired

The frozen session has two smash entries:

- tick 2786
- tick 2885

Difference: **99 ticks**.

99 < 120, so the second landing necessarily takes the `field_8DC=0x29A` branch.

The first smash lands at tick 2800 as:

`state=0x00000001 anim=134`

The second lands at tick 2896 as:

`state=0x01000000 anim=134`

and the process then enters the one-core busy hang before another Logic return.

### Why field_8DC matters

`CPlayer::Hit` is vtable slot 3:

`0x0053C470 -> 0x004BD890`.

Its state gate around `0x004BDB4C` explicitly tests `field_E1C & 0x01000000` and `field_8DC`. A nonzero `field_8DC` changes the combat-hit path while the player remains stranded in the special smash state.

### Fix implemented

The disproven auto-aim cleanup instrumentation was removed.

Added:

`SpideyFixRepeatedSmashRecoveryBeforeLogic()`

It runs immediately before each retail `Logic @ 0x00455400` call and repairs only this exact proven tuple:

- `field_E1C == 0x01000000`
- `mAnim == 134`
- grounded: `mCollision & 2`
- `field_8DC == 0x29A`

When matched it replays the healthy retail recovery exactly:

1. `field_8DC = 0`
2. `field_AE4 = 1`
3. `field_AE5 = 0`
4. call retail `CPlayer::SwitchToStandMode @ 0x004BE4B0`

This is intentionally narrower than disabling the retail <120 branch globally.

New markers:

`repeated_smash_recovery event=repair_before_logic ...`

`repeated_smash_recovery event=repair_return ...`

The entry marker records current/previous smash timestamps and `smash_interval`; the reproduced case should be near 99.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

### Next runtime test

Use `TEST_LATEST_BUILD.bat`.

Highest-value reproduction:
1. perform one ceiling smash/drop attack;
2. perform a second within less than 120 canonical ticks (~2 seconds);
3. land near enemies during combat;
4. verify no freeze;
5. confirm `repair_before_logic` followed by `repair_return`;
6. continue fighting for several more target-loss/hit cycles.

The validated wall/ceiling first-person camera remains unchanged.


### Final revision-stamped repeated-smash fix artifact

Behavior commit:

`c245b3ab` — **Fix repeated ceiling smash combat freeze**

A final forced-clean matching VC6 build was produced with that behavior commit stamped into `RUNTIME_VERSION`.

`Release/spider.dll` verification:

- size: 937,984 bytes
- SHA-256: `f066829d80d96c60461f01b57f42a2541fc159d57cc280317cf525a970d8b2d7`
- behavior commit string present in DLL: PASS

The frozen diagnostic process may now be terminated. Use the normal `TEST_LATEST_BUILD.bat` workflow to install and run this fix candidate.


## 2026-10-07 — Second ceiling drop-attack freeze: healthy landing, post-landing Logic hang trace

### Frozen runtime analyzed live

Runtime revision:

`a83e057881b51db2666e426f4516882ae286dffa`

This runtime contains behavior fix:

`c245b3abce64619ca22394fe6841f3968e7727dd` — repeated ceiling-smash bad-recovery repair.

The process froze again after a ceiling drop attack and remained open for live analysis.

### Important result: previous fix did NOT miss its condition

No:

`repeated_smash_recovery ...`

marker appears in this run.

The final drop attack is:

- tick 5386:
  `state=0x01000000 anim=133 ... ceiling=1`
- tick 5400:
  `state=0x01000000 anim=133 frame=5 finished=1 ... collision=0`
- tick 5410:
  `state=0x00000001 anim=134 ... collision=0x2`
- tick 5418:
  `state=0x00000010 anim=1 ... collision=0x2`
- tick 5419:
  both R1 and R2 Zip checks return normally while still in `state=0x10 anim=1`.

Therefore:

- the smash state transitioned back to stand normally;
- animation 134 itself completed the expected landing handoff;
- the player entered ordinary run state;
- the `field_8DC=0x29A` pathological tuple did not occur;
- the prior `c245b3ab` fix remains valid for the first freeze pattern but is not the whole story.

### Same-run comparison

This same runtime session contains several earlier successful ceiling smash/drop attacks:

- tick 2433 -> healthy 134 recovery at 2454;
- tick 2566 -> healthy 134 recovery at 2578;
- tick 2771 -> healthy 134 recovery at 2795;
- tick 3374 -> healthy 134 recovery at 3398;
- final attack tick 5386 -> healthy 134 recovery at 5410, then freeze shortly after ordinary run state resumed.

Healthy and frozen attacks therefore share the visible:

`133 -> 134 -> state 1 -> state 0x10`

sequence.

The failure is now classified as a **post-landing Logic busy-spin**, not necessarily a state-transition failure.

### Stronger Logic-level hypothesis

Retail `Logic @ 0x00455400` ordering is:

1. player `Ob_AI` at `0x004554A8`;
2. additional world/body lists;
3. `BaddyList` at `0x004554F5`;
4. `ControlBaddyList` at `0x00455501`;
5. pending triggers and later world/camera work.

The last live telemetry before the freeze comes from inside the player AI and shows normal state.

A one-core spin can therefore still be:

- inside the next player update;
- inside player `DoPhysics`;
- inside `CPlayer::Hit`;
- inside post-R2 `CheckWebShot` / `CheckForwards`;
- or after the player returns, while Logic iterates a world/body/baddy list altered by the drop attack.

### New behavior-neutral armed trace

Added a short diagnostic window that arms when:

`state=0x01000000 && anim=133`

is observed before Logic.

The window remains active for 240 canonical 60-Hz ticks after the last observed smash frame.

New marker family:

`drop_hang_trace ...`

Each record includes:

- sequence;
- generation;
- Logic pass;
- canonical tick;
- player state/animation/frame;
- collision;
- wall/ceiling flags;
- player position/velocity;
- list pointer and current list head where applicable.

### Outer Logic breadcrumbs

While armed:

- `logic_enter`
- `logic_return`

If the next freeze ends with `logic_enter` and no `logic_return`, the spin is inside retail Logic.

### Logic body-list breadcrumbs

Direct `Ob_AI` calls in Logic are wrapped for telemetry at:

- `0x004554A8` — player list;
- `0x004554B9`;
- `0x004554C5`;
- `0x004554D1`;
- `0x004554DD`;
- `0x004554E9`.

The existing Chase cadence wrapper continues to own:

- `0x004554F5` — BaddyList;
- `0x00455501` — ControlBaddyList.

Its actual retail `Ob_AI` calls are now routed through the same trace helper, without changing cadence decisions.

Markers:

- `ob_ai_enter`
- `ob_ai_return`

The `list=...` field identifies the exact list. A final unmatched `ob_ai_enter` will identify the spinning list.

### Player-path breadcrumbs

Added behavior-neutral wrappers around:

- player `DoPhysics` direct call:
  `0x004B1B18 -> 0x00466CE0`
  - `player_do_physics_enter`
  - `player_do_physics_return`

- post-R2 ground-branch `CheckWebShot`:
  `0x004B2309 -> 0x004C2090`
  - `check_web_shot_enter`
  - `check_web_shot_return`

- existing modern `CheckForwards` wrapper:
  - `check_forwards_enter`
  - `check_forwards_return`

- `CPlayer::Hit` vtable slot:
  `0x0053C470 -> retail 0x004BD890`
  - `player_hit_enter`
  - `player_hit_return`

All wrappers forward arguments/results unchanged.

### Installer marker

`drop_hang_trace_install ... policy=telemetry_only_after_ceiling_smash`

Expected:

- six direct non-baddy Logic list hooks;
- player physics hook;
- ground-branch CheckWebShot hook;
- player Hit vtable hook;
- Baddy/Control lists reported as using the shared Chase wrapper.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

### Next reproduction

1. use normal `TEST_LATEST_BUILD.bat`;
2. enter a fight;
3. perform ceiling drop attacks normally;
4. if the game freezes, leave it frozen;
5. pull the final `drop_hang_trace` lines.

Interpretation should be immediate:

- `logic_enter` only -> inside Logic;
- `ob_ai_enter list=...` without matching return -> exact spinning body list;
- `player_do_physics_enter` only -> player physics/collision;
- `player_hit_enter` only -> player damage/hit handler;
- `check_web_shot_enter` only -> CheckWebShot;
- `check_forwards_enter` only -> locomotion path;
- all player/list events return but no `logic_return` -> later Logic subsystem after list dispatch.

The validated first-person camera behavior is unchanged.


### Final revision-stamped post-drop hang diagnostic artifact

Behavior/diagnostic commit:

`a6c361de` — **Instrument post-drop Logic hang path**

A forced-clean matching VC6 build was produced with that commit stamped into `RUNTIME_VERSION`.

`Release/spider.dll` verification:

- size: 942,080 bytes
- SHA-256: `4155940e680a9c4e41f406f407fa349f22c04ceffdab5a149501971c9a917c72`
- behavior commit string present in DLL: PASS

The currently frozen `a83e0578...` game process has yielded all available evidence through the present Local Commander interface and may be terminated.

Use normal `TEST_LATEST_BUILD.bat` for the next run. If another ceiling-drop freeze occurs, leave the process open and inspect the final `drop_hang_trace` sequence first.


## 2026-10-07 — Bomb timer holder/text alignment fix

### User report

While the game was live in the bomb level, the countdown timer was visibly outside/misaligned with its holder.

### Retail timer draw path

Retail `Panel_DisplayTimer @ 0x00461D00` builds the visible countdown as a composite HUD element.

The actual countdown text is formatted as `00:00` and drawn at:

- call site `0x0046236F`
- target `Mess_DrawText @ 0x00458700`
- authored position: `x=234`, `y=timerY+38`.

The black timer backing is drawn immediately afterward at:

- call site `0x0046239F`
- target `DCPanel_DrawFlatShadedPoly @ 0x00462D60`
- authored rectangle:
  - `x=222`
  - `y=timerY+23`
  - `width=78`
  - `height=20`.

Therefore the retail text inset is exactly:

- +12 px from the backing left edge;
- +15 px from the backing top edge.

The timer also contains two textured frame pieces built through:

`Panel_SetStretchedScreenCoords @ 0x00462CD0`

inside the same function.

### Root cause

The modern gameplay HUD compatibility globally redirects all direct calls to:

`Panel_SetStretchedScreenCoords @ 0x00462CD0`

through `SpideyCompatPanelSetCoordsTexture`, which compacts/scales those textured timer frame pieces.

The timer text call `0x0046236F` and the timer backing call `0x0046239F` were not on that same transform path.

Result:

- textured timer frame/holder: modern compacted coordinates;
- `00:00` text: raw retail coordinates;
- black backing: raw retail coordinates.

This split coordinate domain explains the visible timer-outside-holder bug.

### Fix

Added bomb-timer-specific wrappers:

`SpideyCompatBombTimerText`

at:

`0x0046236F -> Mess_DrawText @ 0x00458700`

and:

`SpideyCompatBombTimerBacking`

at:

`0x0046239F -> DCPanel_DrawFlatShadedPoly @ 0x00462D60`.

The timer text wrapper derives its anchor from the actual authored backing rectangle:

`222..300 x (textY-15)..(textY+5)`

and applies the same gameplay HUD density/anchor policy used by the compacted timer frame.

It also temporarily scales the active message-font scale with:

`SpideyGetGameplayHudTextScale(savedScale)`

so the text size shrinks with the compacted holder, then restores the retail scale after drawing.

The backing wrapper applies the same center/top anchor transform used by generic compacted panel geometry.

This preserves the authored text inset inside the holder while keeping the complete timer assembly together at modern resolutions.

### New telemetry

`bomb_timer_alignment source=text ...`

records:
- authored and transformed text position;
- backing authored rectangle;
- selected anchors;
- gameplay HUD density;
- original and HUD text scales;
- rendered timer string.

`bomb_timer_alignment source=backing ...`

records:
- backing before/after rectangle;
- anchors;
- HUD density.

Installer telemetry now includes:

`bomb_timer=text:<n>,backing:<n>`.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

No camera, combat-freeze, or drop-hang diagnostic behavior was changed.


### Final revision-stamped bomb-timer fix artifact

Behavior commit:

`44382012` — **Fix bomb timer holder alignment**

A forced-clean matching VC6 build was produced with that commit stamped into `RUNTIME_VERSION`.

`Release/spider.dll` verification:

- size: 942,080 bytes
- SHA-256: `a38c65a8d62ebe711692c545930cdca2cd4078779beb815112ba9450e81dabf8`
- behavior commit string present in DLL: PASS

Use the normal `TEST_LATEST_BUILD.bat` workflow. In the bomb level, verify that the visible `00:00` countdown sits inside the timer holder. The live log should contain both:

- `bomb_timer_alignment source=text ...`
- `bomb_timer_alignment source=backing ...`

and startup installer telemetry should report `bomb_timer=text:1,backing:1`.


## 2026-10-07 — BaddyList freeze boundary nailed; cycle repair + per-baddy AI trace candidate

### Live freeze boundary

The post-drop trace from runtime `fa31e626...` narrowed the busy-spin to one exact unmatched call:

`logic_pass=45 -> ob_ai_enter list=0x0056E990 list_head=0x16FAD4A0 -> no ob_ai_return`

Everything before it returned normally:

- player `Ob_AI`;
- player `DoPhysics`;
- `CheckWebShot`;
- `CheckForwards`;
- earlier Logic body lists.

`0x0056E990` is confirmed retail **BaddyList**.

Therefore the active freeze is inside:

`Ob_AI(&BaddyList, 0) @ 0x00460FC0`.

### Retail Ob_AI traversal facts

Retail `Ob_AI`:

- starts from `*list`;
- saves each node's `mNextItem @ +0x20` before processing the node;
- executes the node's virtual AI callback through vtable slot +8;
- advances to the previously saved next pointer.

This leaves two realistic one-core-spin mechanisms:

1. BaddyList's `mNextItem` chain is cyclic/self-referential;
2. one individual baddy virtual AI callback never returns.

### New BaddyList validator / repair

Added:

`SpideyValidateAndRepairBaddyList`

It runs only for list pointer:

`0x0056E990`.

Behavior:

- walks up to 128 nodes;
- records every visited pointer;
- checks each node's:
  - body pointer;
  - vtable;
  - type;
  - item flags;
  - CBody flags;
  - next;
  - previous;
  - expected previous;
- detects repeated-node/self-loop cycles;
- if and only if an actual repeated pointer is proven, cuts the cycle at the predecessor by writing:
  `previous->mNextItem = NULL`;
- otherwise leaves the list unchanged.

This is an actual defensive repair for a proven cyclic-list condition, not a generic list rewrite.

New markers:

`baddy_list_guard event=node ...`

`baddy_list_guard event=cycle_detected ... action=cut_previous_next`

`baddy_list_guard event=summary ... cycle=<0|1> repaired=<0|1> ...`

A normal acyclic BaddyList is behaviorally untouched.

### Per-baddy virtual AI tracing

To avoid another broad diagnostic round when the list is acyclic, the same candidate dynamically wraps vtable AI slot +8 for each unique BaddyList vtable observed while the post-drop trace window is active.

Each vtable record preserves the exact current AI function pointer before patching, including any existing compatibility target.

Wrapper:

`SpideyBaddyAITraceThunk`

forwards to that preserved target unchanged.

Markers:

`baddy_ai_trace event=vtable_install ...`

`baddy_ai_trace event=enter ... body=... vtable=... type=... original_ai=...`

`baddy_ai_trace event=return ...`

If the next freeze has an acyclic list, the final unmatched `baddy_ai_trace event=enter` identifies the exact baddy object/type/vtable/AI function that spins.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

### Next runtime interpretation

If the game no longer freezes and the log contains:

`baddy_list_guard event=cycle_detected ... repaired=1`

then the root cause was a corrupted/cyclic BaddyList and the new guard prevented retail `Ob_AI` from spinning.

If no cycle is reported and the game still freezes, inspect the final:

`baddy_ai_trace event=enter`

without a matching return. That gives the exact enemy AI callback for the next targeted repair.

Bomb-timer alignment, first-person camera behavior, and the earlier repeated-smash recovery fix remain intact.


## 2026-10-07 — Exact frozen enemy AI identified; narrow post-drop type-304 thug guard

### Live freeze on acff80d7

The BaddyList validator proved the list itself is healthy at the freeze:

- 6 nodes;
- no repeated pointer;
- no self-loop;
- no previous-link mismatch;
- no read fault;
- no cycle repair occurred.

The per-baddy AI trace then identified the exact non-returning callback.

Final sequence:

- type-305 thug at body 0x169D2A40 -> AI 0x00442D10 returned;
- type-305 thug at body 0x169D5DA0 -> AI 0x00442D10 returned;
- type-304 thug at body 0x169D68A0 -> AI 0x004DB280 returned;
- **type-304 thug at body 0x169D6F60 -> AI 0x004DB280 entered and never returned**.

Exact last marker:

`baddy_ai_trace event=enter call=2974 tick=9091 body=0x169D6F60 vtable=0x0053C550 type=304 ... original_ai=0x004DB280 ... state=0x00000001 anim=134`

There is no matching `baddy_ai_trace event=return`.

Symbol resolution:

`0x004DB280 = CThug_AI`.

Therefore this freeze is not a BaddyList traversal cycle. It is an infinite/busy loop inside retail `CThug_AI` for a type-304 thug during the immediate grounded ceiling-drop landing transition.

### Targeted compatibility fix

Added a very narrow post-drop safety window inside `SpideyBaddyAITraceThunk`.

When the armed ceiling-smash trace sees:

- player animation 134;
- grounded collision;

it sets:

`gSpideyPostDropThugSafetyUntilTick = currentTick + 8`.

During only that short window, calls are skipped when:

- baddy `mType == 304`;
- current tick is still inside the safety window.

Other enemy classes continue updating normally.

Type-304 thug AI resumes immediately after the 8-tick window.

This avoids invoking the exact proven pathological retail callback during the transition where the live freeze occurs, without disabling thug AI generally.

New marker:

`baddy_ai_trace event=post_drop_thug_skip ... policy=skip_type304_ai_for_8_ticks_after_grounded_ceiling_smash_landing`

### Why this is deliberately narrow

- BaddyList structure is proven healthy.
- Multiple type-304 thug calls return normally before the landing edge.
- The same retail CThug_AI callback hangs only during the immediate post-drop transition.
- A global CThug_AI bypass would unnecessarily break enemy behavior.
- An 8-tick compatibility quarantine is limited to the proven ceiling-drop landing hazard.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

Existing camera, bomb-timer alignment, BaddyList validation, and diagnostic tracing remain intact.


## 2026-10-07 — Bomb timer gray/striped backing artifact

Live runtime `d1fdb316...` at 2560x1440 showed the countdown correctly aligned inside the timer holder, but the user reported a gray box that appeared in horizontal/scan-line-like strips.

Live telemetry:
- timer text: authored `234,68` -> compacted `246,41`;
- legacy backing: authored `222,53,78,20` -> compacted `241,32,35,12`;
- HUD density: `0.45 x 0.60`.

Retail call at `0x0046239F` draws a solid black `78x20` flat-shaded quad at `z=0.5` immediately behind the countdown. After modern HUD compaction this becomes a tiny overlapping flat primitive under the textured timer frame.

Fix:
- `SpideyCompatBombTimerBacking` no longer forwards to `DCPanel_DrawFlatShadedPoly`;
- only the legacy timer backing quad is suppressed;
- textured timer holder remains;
- countdown text remains at the validated aligned position;
- new telemetry reports `source=backing action=suppressed`.

Validation:
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

The post-drop type-304 thug AI freeze fix remains unchanged.


## 2026-10-07 — Controller power-off crash: DirectInput hot-unplug support

### Live crash evidence

Runtime revision:

`d1fdb316e630ae5412d912f417b584f8a0ceaa35`

The game exited immediately after the controller was powered off.

Final high-signal log sequence:

`[DXERROR] DI error=0x80040209 file=...DXInput.cpp line=746 caller_return=0x00501EDA call_site=0x00501ED5`

followed by teardown-side:

`[DXERROR] D3D error=0x0000033B ... DXinit.cpp line=1185`

and final process exit:

`[SESSION] exit_code=-2147220983`

Signed exit code `-2147220983` is exactly HRESULT:

`0x80040209 = DIERR_UNPLUGGED`.

Therefore:
- root cause is legacy DirectInput controller hot-unplug handling;
- the D3D error is teardown fallout, not the initiating fault;
- this is not primarily an overlay-rendering crash.

### Retail DXINPUT_PollController root cause

Retail:

`DXINPUT_PollController @ 0x00501E50`

does:

1. `IDirectInputDevice8::Poll`;
2. on retail-recognized lost/not-acquired HRESULTs, calls `Acquire`;
3. if `Acquire` returns a failure other than `DIERR_OTHERAPPHASPRIO`, calls:
   `displayDIError @ 0x004FC240`
   at callsite `0x00501ED5`;
4. if HRESULT is still negative, enters the retail fatal/error-exit path.

When the controller powers off, DirectInput returns:

`DIERR_UNPLUGGED = 0x80040209`

which retail does not treat as a recoverable optional-device loss.

### Compatibility gap

The project already owned all direct calls to:
- `DXINPUT_PollKeyboard @ 0x00501B80`;
- `DXINPUT_PollMouse @ 0x00501CC0`.

It did **not** own:
- `DXINPUT_PollController @ 0x00501E50`.

So controller disconnects still executed untouched retail fatal behavior.

### Fix implemented

Added:

`SpideyCompatRetailPollController`

and redirected every direct call to retail `0x00501E50` through it.

The compatibility poll reproduces retail controller behavior:
- foreground gating;
- `Poll()`;
- recoverable `Acquire()`;
- buffered `GetDeviceData()`;
- 32-byte retail controller button transition array at `0x006B7A34`;
- X axis / Y axis / POV updates;
- retail button offset range based on `0x006B7A58`;
- normal return value semantics:
  - `1` = controller frame available;
  - `0` = no controller frame.

### Recoverable failures

These are now treated as nonfatal controller loss:
- `DIERR_INPUTLOST`;
- `DIERR_NOTACQUIRED`;
- `DIERR_UNPLUGGED`;
- `DIERR_OTHERAPPHASPRIO`.

On disconnect:
- X/Y are zeroed;
- POV is reset to -1;
- retail controller button state is cleared;
- game continues on keyboard/mouse instead of exiting.

Unknown controller `GetDeviceData` failures are also controller-local/nonfatal; they disable only that controller frame.

### Hot replug

The modern `input11` backend already independently detects XInput connection transitions.

It is now used as a reconnect signal for the legacy DirectInput controller path.

If:
- legacy DirectInput controller remains dead/disconnected;
- `input11` reports a controller is connected;
- at least 60 controller polls have elapsed since the previous rebuild attempt;

then the compatibility layer:
1. releases stale force-feedback effect `0x006B7A68`;
2. clears FF-active flag `0x006B7928`;
3. unacquires/releases stale legacy controller object `0x006B7A2C`;
4. clears legacy button count/state;
5. calls retail `DXINPUT_SetupController @ 0x00501890` to enumerate/configure a fresh DirectInput device.

This preserves existing legacy button mappings while allowing power-off/power-on without restarting the game.

### Telemetry

Installer now reports:

`retail_input_compat installed ... controller_calls=<n> ... controller=0x00501E50 policy=controller_unplug_nonfatal_hot_rebuild`

Runtime transitions log:

`controller_hotplug event=disconnect ...`

`controller_hotplug event=rebuild ...`

`controller_hotplug event=reconnected ...`

Fields include:
- Poll HRESULT;
- Acquire HRESULT;
- setup result;
- current retail controller pointer;
- input11 connection state;
- disconnect/reconnect counters.

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

Existing camera, bomb-timer, post-drop thug-AI, and BaddyList compatibility fixes remain unchanged.


## 2026-10-07 — Ceiling takedown freeze refined to exact victim

### Live freeze on be0464c7

The game froze again after a ceiling takedown.

Final unmatched callback:

`baddy_ai_trace event=enter call=367 tick=4544 body=0x167B5CE0 vtable=0x0053C550 type=304 original_ai=0x004DB280`

No matching return exists.

`0x004DB280 = CThug_AI`.

So the same retail thug AI is still the spinner, but this freeze happened after the previous 8-tick all-type304 quarantine had already expired.

### Timing

Ceiling smash/takedown begins:

`tick=4496 state=0x01000000 anim=133`

Grounded landing:

`tick=4527 state=0x00000001 anim=134`

Old safety window:

`4527..4535`

Freeze:

`tick=4544`

Therefore the failure occurs **17 ticks after landing**, outside the old +8 window.

### Critical victim discriminator

During the ceiling takedown descent, the exact body that later freezes is uniquely identifiable.

At tick 4498/4499/4501 etc:

`body=0x167B5CE0 type=304 flags=0x0006`

All other type-304 thugs are:

`flags=0x0002`

The extra bit is:

`0x0004`

The exact same body `0x167B5CE0` later hangs in `CThug_AI` at tick 4544.

This gives a much cleaner relationship than "all type-304 thugs after landing": the spinner is the **actual takedown victim**.

### Refined fix

The previous all-type304 +8-tick quarantine has been replaced.

New behavior:

1. While the ceiling-smash state is active:
   - player `state=0x01000000`;
   - animation `133`;
   - baddy type `304`;
   - baddy item flags contain `0x0004`;
   the compatibility layer remembers that exact `CBody*` as the takedown victim.

2. When Spider-Man reaches grounded landing animation `134`, the layer opens a **30 canonical tick** quarantine window for only that remembered victim.

3. During that window, only:
   `body == gSpideyPostDropVictim`
   has its retail `CThug_AI` call skipped.

4. Every other type-304 thug, every type-305 thug, and all other enemy classes continue updating normally.

5. After the 30-tick window expires, the victim pointer and safety state are cleared and normal retail AI resumes.

Why 30:
- reproduced freeze is +17 ticks after landing;
- 30 covers the proven failure with margin;
- it is still only about half a second at the canonical 60-Hz cadence;
- only the actual victim is paused.

### New telemetry

`baddy_ai_trace event=takedown_victim_identified ...`

`baddy_ai_trace event=post_drop_thug_skip ... policy=skip_identified_takedown_victim_for_30_ticks_after_landing`

`baddy_ai_trace event=takedown_victim_quarantine_expired ...`

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

External process-memory inspection was attempted through the newer Local Commander read-only API, but Windows denied `OpenProcess` against the elevated frozen game process. The injected in-process trace was sufficient to identify the exact victim body and callback.

Controller hot-unplug support from `be0464c7` remains intact.


## 2026-10-07 — Jump/drop attack proves smash freeze is not ceiling-only

### Live freeze on 1905931d

User reported another freeze after what was likely a normal jump/drop attack rather than a ceiling-start takedown.

The process remained open and the trace again ended on:

`baddy_ai_trace event=enter call=7482 tick=5140 body=0x16FC68A0 vtable=0x0053C550 type=304 original_ai=0x004DB280`

with no matching return.

So the spinner is again:

`CThug_AI @ 0x004DB280`.

### Player move identity

Despite not starting from a ceiling, this move uses the exact same retail player smash state and animation sequence:

- tick 5120: `state=0x01000000 anim=133`
- tick 5133: grounded `state=0x00000001 anim=134`
- tick 5140: type-304 thug AI enters and never returns.

Therefore the engine reuses the ceiling-smash/drop-attack state for ordinary airborne drop attacks as well.

### Why the 1905931d victim heuristic missed this case

The previous refinement identified authored ceiling takedown victims by:

`type=304 && (mFlags & 0x0004)`.

In this run, active BaddyList at attack time contained only:

- type 305 body `0x16FC5DA0`;
- type 304 body `0x16FC68A0`.

The type-304 victim remained:

`flags=0x0002`

throughout anim 133, so the explicit `0x4` ceiling-victim marker never appeared and no quarantine armed.

Immediately before the attack, the camera/auto-aim system had already selected the same body:

`target=0x16FC68A0`.

It was also the only type-304 thug in BaddyList.

### Generalized victim selection

Victim selection during smash state/anim 133 is now two-tiered:

1. **priority 2**:
   type-304 thug with explicit item flag `0x0004`.
   This preserves the exact ceiling-takedown identification from the previous run.

2. **priority 1 fallback**:
   nearest type-304 thug to Spider-Man during smash descent.

The closest candidate is updated throughout anim 133. An explicit `0x4` candidate always overrides distance selection.

Selection is scoped to the current `drop_hang_trace` generation so stale victims from previous attacks cannot carry forward.

### Recovery behavior

Once grounded landing anim 134 is reached:

- only the selected victim body is quarantined;
- quarantine length remains 30 canonical ticks;
- all other type-304 thugs and all other enemy types continue normal retail AI;
- victim state is cleared after expiry.

New selection telemetry:

`baddy_ai_trace event=drop_victim_candidate ... priority=... distance_sq_low=... distance_sq_high=... policy=flag4_else_nearest_type304`

Expiry telemetry:

`baddy_ai_trace event=drop_victim_quarantine_expired ...`

### Validation

- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS

This generalizes the freeze mitigation to both ceiling-start and ordinary airborne drop attacks while keeping the AI quarantine victim-specific.


## 2026-10-07 — Plain combat freeze proves CThug_AI is the common subsystem

Live runtime `708ea29e` froze during ordinary fighting with no drop attack involved.

Final unmatched callback:
`baddy_ai_trace event=enter call=1278 tick=2906 body=0x170CAF60 type=304 original_ai=0x004DB280`

No matching return.

Player state at freeze:
- `state=0x00000800`
- `anim=105`
- grounded normal combat.

BaddyList was structurally healthy with 7 nodes; multiple other type-304 thugs returned normally immediately before the stuck one.

The stuck thug had earlier transitioned:
`mCBodyFlags 0x0218 -> 0x0208`
which matches `CThug::TakeHit()` clearing bit `0x10`.

This proves the prior ceiling/drop-specific guards were symptom mitigations. The common failing subsystem is retail:
`CThug_AI @ 0x004DB280`.

Project history also shows the full-engine 20-FPS reference is known-good, while ordinary baddies remain at 60-Hz dispatch in the modern engine.

### New authored-cadence candidate

Added per-body type-304 AI cadence gating inside `SpideyBaddyAITraceThunk`:

- engine/render/physics remain 60 Hz;
- each type-304 thug gets at most one retail `CThug_AI` dispatch per three canonical 60-Hz timer ticks;
- effective thug AI cadence = 20 Hz;
- cadence state is tracked independently per thug body;
- type-305 and all other enemy classes are unchanged;
- existing victim-specific post-drop guard remains as an extra narrow safety layer.

Telemetry:
`baddy_ai_trace event=type304_cadence_hold ... policy=type304_ai_20hz_on_60hz_engine`

Rationale:
Every captured combat freeze—ceiling takedown, airborne drop attack, and ordinary combat—ends inside the same type-304 CThug_AI callback. Restoring authored AI dispatch cadence addresses the common state-machine timing issue instead of guessing which player move caused it.

Validation:
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-07 — 64b06d5e still froze; combat trace window was insufficient

Runtime `64b06d5e` froze again.

The new type-304 cadence gate was definitely active:
`baddy_ai_trace event=type304_cadence_hold ... policy=type304_ai_20hz_on_60hz_engine`

However, the existing detailed `drop_hang_trace` window expired around tick 4765, while the game continued running until after tick 6442 before freezing.

Therefore this run does **not** provide an exact unmatched callback at the final freeze boundary. It disproves only the theory that 60-Hz type-304 dispatch by itself was sufficient to cause all freezes.

The last visible player state before the freeze was:
- `state=0x00000800`
- `anim=106`
- grounded normal combat.

### Diagnostic extension

`SpideyDropHangTraceUpdateArmBeforeLogic()` now also arms/keeps detailed Logic/Baddy tracing active whenever:
`player->field_E1C == 0x00000800`.

New marker:
`drop_hang_trace ... event=arm_combat_state_0x800`

While the player remains in combat state 0x800:
- outer Logic enter/return remains traced;
- player physics and input-gate wrappers remain traced;
- BaddyList and ControlBaddyList enter/return remain traced;
- per-baddy AI enter/return remains traced;
- type-304 cadence-hold telemetry remains available.

This is intentionally diagnostic. No new root-cause behavior fix is claimed yet.

Validation:
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — Exact combat freeze boundary captured; thug state/substate trace added

Runtime `29fda2cc` froze during ordinary combat with the extended combat trace active.

The exact unmatched callback is now proven:

`baddy_ai_trace event=enter call=720 tick=3636 body=0x13172760 vtable=0x0053C550 type=304 flags=0x0002 cbody_flags=0x0208 original_ai=0x004DB280 player_state=0x00000800 player_anim=105`

There is no matching `baddy_ai_trace event=return`.

Therefore:
- BaddyList structure is healthy;
- neighboring type-305 and type-304 baddies all returned normally;
- the same retail `CThug_AI @ 0x004DB280` remains the actual spinner;
- 20-Hz type-304 cadence did not prevent the bad state;
- the frozen thug is again the one with `mCBodyFlags=0x0208`, while most neighboring type-304 thugs are `0x0218`.

Source correlation:
`CThug::TakeHit()` explicitly clears bit `0x10` via:
`mCBodyFlags &= 0xFFEF`.
That exactly produces the observed `0x0218 -> 0x0208` transition seen repeatedly before freezes.

### New state-level trace

The per-baddy AI entry telemetry now reads and logs, for type-304 thugs:
- `field_31C.bothFlags` as `thug_state`;
- `dumbAssPad` as `thug_substate`;
- `field_318`;
- `field_310`;
- `mHealth`.

New entry format includes:
`thug_state=... thug_substate=... thug_field318=... thug_field310=... thug_health=...`

This should identify the exact CThug state handler/substate that is spinning on the next reproduction, allowing a direct state repair rather than another broad CThug_AI suppression or player-move workaround.

Validation:
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — State 26/substate 1 isolated; nested helper call tracing added

Live runtime `fa18d7ff` froze again during normal combat.

Exact unmatched thug AI entry:

`body=0x12F7D8A0 type=304 cbody_flags=0x0208 thug_state=26 thug_substate=1 thug_field318=2 thug_field310=0 thug_health=0 original_ai=0x004DB280`

No matching return.

### State transition history

Same thug:
- tick 9015: state 17 / substate 10 / health 0 -> retail AI returned.
- tick 9018: state 17 / substate 10 / health 0 -> retail AI returned.
- tick 9021: state 17 / substate 10 / health 0 -> retail AI returned.
- tick 9022/9023: state 26 / substate 0, cadence-held.
- tick 9024: first actual state-26/substate-0 execution -> returned normally and advanced substate.
- tick 9025/9026: state 26 / substate 1, cadence-held.
- tick 9027: first actual state-26/substate-1 execution -> entered CThug_AI and never returned.

Therefore the hang is specifically inside the first real execution of **state 26 / substate 1**.

### Retail dispatch mapping

`CThug_AI` state jump table:
- state 26 -> `0x004DB7F8`;
- `0x004DB7F8` calls `sub_4D8E50 @ 0x004D8E50`.

`sub_4D8E50` is a 9-substate thug handler.
Its substate jump table:
- substate 0 -> `0x004D8E6B`;
- **substate 1 -> `0x004D8FD0`**.

Substate 1 performs:
1. `CBaddy::SetHeight(0, 100, 600) @ 0x004041C0`;
2. if `mAnimFinished`, `CBaddy::Die(1) @ 0x00404320`;
3. then advances the substate.

### Nested helper analysis

`CBaddy::SetHeight` is already decompiled in `baddy.cpp`. It may call:
`Utils_GetGroundHeight @ 0x004E6840`.

`Utils_GetGroundHeight` is marked `@NotOk` in source and calls:
- `M3dColij_InitLineInfo @ 0x004524C0`;
- `M3dZone_LineToItem @ 0x004549A0`.

The ground-height ray is vertical, and retail has a vertical-line fast path; the general M3dZone grid-walk loop is therefore less likely but not yet eliminated.

`CBaddy::Die(1)` first calls:
`CBody::IsDead @ 0x00460700`,
which simply checks mCBodyFlags bit 0x40.
At `0x0208`, the thug has health 0 but is not yet flagged dead, so `Die(1)` executes its case-1 body and calls `CBody::KillShadow @ 0x00460570`.

### New callsite-level telemetry

Added wrappers only at state-26/substate-1 callsites:
- `0x004D8FDC -> CBaddy::SetHeight`;
- `0x004D8FF8 -> CBaddy::Die`.

Markers:
- `state26_nested event=setheight_enter`
- `state26_nested event=setheight_return`
- `state26_nested event=die_enter`
- `state26_nested event=die_return`

Each logs:
- tick/body;
- thug state/substate;
- health;
- mCBodyFlags;
- thug animation/frame/mAnimFinished;
- arguments/result;
- enter/return counters.

This is telemetry-only; retail behavior is forwarded unchanged.

Validation:
- `git diff --check`: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — Active fix for type-304 state-26/substate-1 combat freeze

Repeated live captures isolated the freeze to a type-304 thug with health 0, mCBodyFlags 0x0208, state 26, substate 1. The first real execution of state 26/substate 1 never returned.

Retail mapping:
- CThug_AI state 26 -> 0x004DB7F8
- state handler -> sub_4D8E50 @ 0x004D8E50
- substate 1 -> 0x004D8FD0

Substate 1 calls CBaddy::SetHeight(0,100,600) before checking mAnimFinished.

Critical observation:
state-26/substate-0 already calls SetHeight(1,100,600), forcing a fresh ground-height query and caching the intended ground target in field_2A0.

### Active fix

At only callsite 0x004D8FDC, when:
- type == 304
- state == 26
- substate == 1
- health <= 0

the wrapper bypasses retail CBaddy::SetHeight and reuses cached field_2A0.

It runs only the bounded vertical-settle half of retail SetHeight:
- targetY = field_2A0 - (field_21E << 12)
- move mPos.vy one quarter toward target
- use the same snap thresholds: 12288 or 122880 depending on field_2A4
- preserve retail return semantics 1/2

This removes the repeated Utils_GetGroundHeight -> M3dZone_LineToItem world-collision query from the proven bad dead-thug state while preserving authored corpse ground placement.

All other SetHeight calls remain retail.

Validation:
- git diff --check: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — Global authored BaddyList cadence on 60-Hz engine

Live runtime fbbccc8b froze again. The final log stopped on:
- CThug_AI entry;
- type 304;
- state 26/substate 1;
- health -60;
- before the state-26 SetHeight compatibility wrapper was entered.

Therefore the state-26 SetHeight path was not the freeze boundary in this run.

This rules out the DX11 renderer as the direct source of this specific hang because the log stops inside the gameplay Logic/BaddyList update before Logic returns or rendering/present is reached.

The stronger timing issue is architectural:
- previous experiment throttled only CThug_AI to 20 Hz;
- Ob_AI/EveryFrame for the same thug still ran at 60 Hz;
- generic baddy bookkeeping, animation/state timers, collision/messages and CThug_AI were therefore on mismatched cadences.

Project precedent from the validated Venom chase already proved that actor-only cadence gating is insufficient; BaddyList and ControlBaddyList must share an authored-phase gate.

### Fix candidate

The existing phase-locked BaddyList + ControlBaddyList wrapper is now the global policy on the 60-Hz engine instead of being limited to L5A1 scripted chase.

- BaddyList @ 0x0056E990 advances once per 3 canonical 60-Hz ticks.
- ControlBaddyList @ 0x0056E994 advances on the same due tick.
- player/render/physics remain 60 Hz.
- the per-type-304 CThug_AI 20-Hz gate was removed to avoid double-throttling thugs to ~6.7 Hz.
- existing chase phase behavior is naturally preserved by the global policy.

Telemetry policy string:
global_authored_baddy_and_control_lists_20hz_on_60hz_engine

Validation:
- git diff --check: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — Live freeze on global baddy 20 Hz; root narrowed to CThug_AI pre-dispatch

Runtime `719a53e7` froze again while fighting.

The live process stayed alive/responding. The absolute log tail shows:
- Logic entered normally;
- player physics returned normally;
- every non-BaddyList object list returned;
- BaddyList entered;
- neighboring type-304/type-305 enemies returned normally;
- the final line is a type-304 thug entering retail `CThug_AI @ 0x004DB280`;
- no matching AI return;
- no BaddyList return;
- no Logic return.

Exact frozen thug:
- body `0x167AB0E0`
- type 304
- `mCBodyFlags=0x0208`
- `thug_state=26`
- `thug_substate=1`
- `thug_health=-10`
- player state `0x00000800`
- player anim 106

The state-26 SetHeight compatibility callsite was not reached. Therefore this freeze occurs before the state-26 handler call inside the generic pre-dispatch region of `CThug_AI`.

This directly rules out DX11/present as the freeze boundary for this run because gameplay Logic never returned.

The global BaddyList 20-Hz experiment also failed to prevent the freeze, so normal baddy-world cadence is restored to 60 Hz outside the already-validated L5A1 Venom chase phase gate.

### Direct targeted fix

For a type-304 thug with state 26 and health <= 0, the vtable AI thunk no longer calls full retail `CThug_AI`.

Instead it directly invokes the already-isolated retail state-26 handler:
`sub_4D8E50 @ 0x004D8E50`.

Rationale:
- every fully captured freeze reaches full CThug_AI and stops before state-26 dispatch;
- the state-26 handler itself has returned normally in captured frames;
- dead state-26 thugs no longer need generic combat pre-dispatch bookkeeping;
- bypass is limited to already-dead type-304 state-26 objects.

Telemetry:
- `baddy_ai_trace event=dead_state26_direct_enter`
- `baddy_ai_trace event=dead_state26_direct_return`

Validation:
- normal BaddyList/ControlBaddyList updates restored to 60 Hz outside L5A1 chase;
- `git diff --check`: PASS;
- forced-clean matching VC6 build: PASS.


## 2026-10-08 — In-process hang sampler added; unsafe state-26 direct bypass removed

The latest reported "freeze" on revision 681bea81 was actually a real access violation introduced by the direct state-26 bypass.

Crash:
- exception 0xC0000005
- EIP 0x00461132
- read target 0x00000047
- ESI = 0x00000001
- EDI = dead thug body 0x168D1760

Retail disassembly around 0x00461132 shows this is inside Ob_AI immediately after the virtual AI callback:
`test byte ptr [esi + 0x46], 8`.

Ob_AI expects ESI to remain the current body pointer across the AI callback. The direct call to sub_4D8E50 returned with ESI clobbered, violating the original CThug_AI caller/register contract. Therefore the state-26 direct bypass is removed.

### New runtime analysis tool: in-process hang sampler

External GetThreadContext/process-memory inspection is unreliable when SpideyPC is running at a higher Windows integrity level than Local Commander. To capture the actual CPU hot loop during a freeze, spider.dll now contains its own watchdog sampler.

Behavior:
- starts lazily on the first real Logic call, not from DllMain;
- records the actual Logic/main-thread ID;
- marks each Logic entry/return;
- if a Logic pass remains entered for >= 750 ms, the watchdog opens that exact thread;
- performs 64 samples at 10 ms intervals;
- for each sample:
  - SuspendThread;
  - GetThreadContext(CONTEXT_FULL);
  - capture EIP/ESP/EBP/EAX/EBX/ECX/EDX/ESI/EDI/EFLAGS;
  - capture 16 instruction bytes at EIP;
  - capture 32 stack dwords from ESP;
  - ResumeThread immediately;
- writes samples under [HANG] / `hang_sampler` in the consolidated runtime log;
- sampling automatically stops if the Logic pass returns.

This is designed to identify a tight busy loop by repeated real EIP samples and stack context, eliminating the need to infer the frozen instruction from function enter/return breadcrumbs.

Markers:
- `hang_sampler event=installed`
- `hang_sampler event=trigger`
- `hang_sampler sample ... eip=...`
- `hang_sampler event=burst_complete`

Validation:
- unsafe dead-state direct bypass removed;
- normal baddy updates remain 60 Hz outside the existing validated Venom chase exception;
- git diff --check: PASS;
- forced-clean matching VC6 build: PASS.


## 2026-10-08 — Root cause of combat freeze proven by in-process EIP sampler

Runtime `221976f5` froze again while fighting. The new in-process hang sampler captured 64 instruction-level samples from the actual stuck Logic/main thread.

The samples overwhelmingly alternated between:
- `0x100023C0 / 0x100023C2` in `spider.dll`;
- `0x00401110` in `SpideyPC.exe`.

Stable stack return address:
- `0x004DB38C`, immediately after `CThug_AI`'s virtual call through the AI-procedure object at `field_28C`.

Frozen object context:
- thug body `0x16FCB8A0`;
- AI-procedure object `ECX=0x16FCCF00`;
- retail vtable around `0x0053B240`.

### Exact recursion

`spider.map` identified `0x100023C0` as the MSVC-generated virtual thunk for the patched `CAIProc_RotY::Execute` member.

Old live DLL bytes at `0x100023C0`:

`8B 01 FF 60 04`

which is:

`mov eax,[ecx]`
`jmp dword ptr [eax+4]`

Retail vtable `0x0053B240` slot +4 points to:
`0x00401110`.

But `patch_ai()` had installed:

`PATCH_PUSH_RET(0x00401110, CAIProc_RotY::Execute)`

which overwrote retail `0x00401110` with:

`push 0x100023C0`
`ret`

Therefore the runtime loop was exactly:

`0x00401110 -> 0x100023C0 -> vtable[+4] -> 0x00401110 -> ...`

This is the actual CPU busy-spin responsible for the recurring combat freezes.

### Fix

Do not patch a retail virtual entry point directly to a virtual-member symbol.

Added concrete non-virtual fastcall bridge:

`SpideyCAIProcRotYExecute(CAIProc_RotY* self, void*)`

which invokes:

`self->CAIProc_RotY::Execute()`

using a qualified direct call.

`patch_ai()` now installs:

`PATCH_PUSH_RET(0x00401110, SpideyCAIProcRotYExecute)`

instead of the virtual member symbol.

The compiled bridge at `0x100023C0` now begins with:

`E9 9B F2 FF FF`

a direct relative jump to the concrete native implementation. It no longer contains the recursive virtual-dispatch thunk.

The in-process hang sampler remains enabled for future freezes.

Validation:
- git diff --check: PASS;
- forced-clean matching VC6 build: PASS;
- compiled bridge bytes verified non-virtual/direct.


## 2026-10-08 — Heavy freeze diagnostics temporarily disabled

After confirming the RotY virtual-patch recursion fix, the heavy runtime diagnostics were temporarily disabled to remove their likely hitching overhead.

Disabled at install/startup:
- SpideyInstallDropHangTraceDiagnostics()
- SpideyInstallState26NestedTrace()
- SpideyHangSamplerEnsureStarted() / per-Logic hang-sampler state tracking

The diagnostic implementations remain in source and can be re-enabled immediately if another freeze or bug appears.

Preserved:
- actual CAIProc_RotY recursion fix
- normal 60-Hz baddy updates outside the existing validated Venom chase exception
- lightweight runtime/crash/error logging

Validation:
- git diff --check: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-08 — XAudio2 2.9 voice backend Phase 1

Added a modern XAudio2 2.9 active-voice backend while retaining the retail DirectSound bank loader temporarily as the PCM source.

### Architecture

Higher-level game audio is unchanged:
- SFX_Play / SFX_PlayPos
- SFX_Stop / SFX_ModifyPos / SFX_ModifyVol
- existing sound IDs, aliases, banks and voice handles

Retail DirectSound remains responsible only for loading/decoding the source bank buffers in Phase 1.

When a voice is opened, the XAudio2 backend:
1. looks up the already-loaded retail DirectSound source buffer from the retail gDxSoundBuffers array at 0x006BBAD4;
2. queries its actual WAVEFORMATEX and buffer size;
3. copies the decoded PCM payload once;
4. creates an XAudio2 source voice;
5. handles Play/Stop/Close/Volume/Pan/Pitch/IsPlaying through XAudio2.

### Live retail entry points replaced only when XAudio2 initializes successfully

- DXSOUND_Open @ 0x00504110
- DXSOUND_Close @ 0x005041C0
- DXSOUND_Play @ 0x00504230
- DXSOUND_Stop @ 0x005042A0
- DXSOUND_SetVolume @ 0x005042F0
- DXSOUND_SetPan @ 0x00504350
- DXSOUND_SetPitch @ 0x005043B0
- DXSOUND_IsPlaying @ 0x00504420

If XAudio2 initialization fails, none of those retail entry points are patched and DirectSound remains the complete fallback.

Environment override for A/B testing:
SPIDEY_AUDIO_BACKEND=directsound

### VC6-compatible XAudio2 layer

Because the matching project uses Visual C++ 6.0, modern xaudio2.h is not included directly.
Added:
- xaudio2_compat.h: minimal ABI-compatible XAudio2 2.9 declarations
- xaudio2_backend.h/.cpp: dynamic xaudio2_9.dll backend

Runtime load order:
1. xaudio2_9.dll
2. xaudio2_9redist.dll
3. fall back to untouched retail DirectSound

One-time backend/error status is written to spidey-audio.log. No per-frame audio logging is enabled.

### Compatibility behavior

- volume preserves retail DirectSound hundredth-dB conversion;
- pan preserves the game's 0..31 pan convention and maps it to an XAudio output matrix;
- pitch follows the verified retail formula: baseFrequency * pitch / 1200;
- looping maps to XAUDIO2_LOOP_INFINITE;
- voice reclamation uses XAudio2 BuffersQueued for DXSOUND_IsPlaying;
- pause/unpause remains compatible because the game pauses SFX by muting volume, not by stopping voices.

### Validation

- xaudio2_9.dll exists in both System32 and SysWOW64 on the test PC;
- git diff --check: PASS;
- forced-clean matching VC6 build: PASS;
- xaudio2_backend.cpp is compiled and linked into spider.dll.

Next test:
- launch with default XAudio2 backend;
- verify menu SFX, first-level ambience/chopper, positional pan, pitch, loops, pause/unpause and voice cleanup;
- compare with SPIDEY_AUDIO_BACKEND=directsound only if a behavior difference needs isolation.


## 2026-10-09 — L1A1 repeating audio A/B: suppress missile-owned alias-1 loop only

Investigation of the first-level repeating sound identified L1A1 level-specific alias 1 as a confirmed looping entry.

### Bank evidence

Extracted and decompressed the real files from data.pkr:

- l1a1.sfx
  - PKR record offset: 0x09F29BA8
  - file offset: 0x09EE2F00
  - uncompressed size: 724 bytes
  - compressed size: 216 bytes
- l1a1.kat
  - PKR record offset: 0x09F241AC
  - file offset: 0x05EFF754
  - uncompressed size: 319692 bytes
  - compressed size: 277864 bytes

Decoded L1A1 alias 1:
- alias: 1
- resolved bank asset index: 0x21 / 33
- resolved flags: 0xC0210001
- loop bit 0x80000000: SET
- asset 33:
  - compressed/sample payload size field: 15120 bytes
  - sample rate: 12000 Hz
  - format field: 4
  - approximate whole-buffer repeat duration at nominal pitch: ~2.52 seconds

Retail code uses the same alias 1 from two distinct owners:
- helicopter engine loop:
  - CChopper::AI
  - SFX_PlayPos(0x8001, ...)
  - retail callsite 0x00426313
- helicopter missile loop:
  - CChopperMissile::CommonInitialisation
  - SFX_PlayPos(0x8001, ...)
  - retail callsite 0x0042435B

Missile destructor guards field_10C and calls SFX_Stop when nonzero.

### A/B test patch

Added:
SpideySuppressChopperMissileLoop(u32, CVector*, i32)

The wrapper returns 0 and performs no audio call.

Only the missile initializer callsite is patched:
- 0x0042435B -> SpideySuppressChopperMissileLoop

The helicopter's own alias-1 start at 0x00426313 remains untouched.

This creates a clean diagnostic:
- if the persistent repeating sound disappears, the unwanted voice is missile-owned;
- if it remains, the remaining source is the helicopter-owned alias-1 loop / loop seam.

Compiled wrapper verification:
- wrapper VA in current build: 0x10048BB0
- bytes disassemble to:
  - xor eax,eax
  - ret

Validation:
- git diff --check: PASS
- forced-clean matching VC6 build: PASS


## 2026-10-09 — XAudio2 COM initialization fix + live WASAPI loopback analyzer

The first XAudio2 2.9 build was falling back to retail DirectSound with:
- backend=directsound reason=mastering_voice_failed

A direct XAudio2 probe reproduced the failure:
- XAudio2Create: 0x00000000
- CreateMasteringVoice: 0x800401F0 (CO_E_NOTINITIALIZED)

Calling CoInitializeEx(NULL, COINIT_MULTITHREADED) before XAudio2 fixed the same probe:
- CoInitializeEx: 0x00000000
- XAudio2Create: 0x00000000
- CreateMasteringVoice: 0x00000000

### Backend changes

xaudio2_backend.cpp now:
- initializes COM before XAudio2 2.9;
- accepts RPC_E_CHANGED_MODE as an already-initialized COM apartment case;
- balances successful CoInitializeEx calls with CoUninitialize;
- logs exact HRESULT values for COM/XAudio2 initialization failures;
- keeps DirectSound fallback if modern backend startup still fails;
- emits one one-time format record when L1A1 alias 1 / bank asset 33 is opened through XAudio2:
  - format tag
  - channels
  - sample rate
  - bit depth
  - block align
  - copied PCM byte count

VC6 does not declare CoInitializeEx in its old platform headers, so a local ABI-compatible declaration is used. ole32.lib was already part of the project link set.

### Missile A/B conclusion and restoration

Previous build 255ece2d suppressed only the CChopperMissile alias-1 callsite at 0x0042435B.
The audible ~2.5 second repeat remained.

Therefore the missile loop is ruled out as the persistent repeating sound reported by the user.
The missile suppression patch has now been removed and retail missile audio restored.

The remaining confirmed owner is the helicopter's own alias-1 start at 0x00426313.

### Actual speaker-output capture

Added tools/SpideyLoopbackCapture.cs:
- small Windows Core Audio / WASAPI loopback recorder;
- captures the actual system render stream to WAV;
- does not require game hooks or high-frequency logging.

A 12-second live capture of the running game produced:
- stereo
- 48000 Hz
- 32-bit IEEE float output

Envelope autocorrelation showed:
- strong recurrence near 2.48 seconds;
- strongest recurrence near 4.95 seconds (~2 cycles).

This matches the decoded L1A1 alias-1 / asset-33 nominal whole-buffer repeat duration (~2.52 seconds) closely enough to identify that asset as the audible repeating source.

### Current next test

Run the new build and verify spidey-audio.log reports:
backend=xaudio2 ...

Then enter L1A1 and capture/listen again. The missile audio is restored. If the helicopter alias-1 repeat remains under true XAudio2, use the one-time asset-33 source-format telemetry to implement a narrowly targeted loop-seam treatment in the modern backend rather than muting helicopter audio.


### Reusable loopback analysis tool

Added tools/analyze_loopback.py.

It requires only the Python standard library and:
- parses RIFF/WAVE and WAVEFORMATEXTENSIBLE;
- supports IEEE float32 and PCM 8/16/24/32-bit captures;
- builds a configurable RMS envelope;
- calculates normalized autocorrelation over candidate periods;
- reports the strongest repeat periods;
- can score expected periods directly with --expected.

Validation against the captured L1A1 sample:
python tools/analyze_loopback.py logs/live_loopback.wav --expected 2.52 --top 8

Results reproduced:
- strongest period: 4.96 s, correlation 0.429026;
- 2.52 s expected-period correlation: 0.202974.

Together with SpideyLoopbackCapture.cs, future audio regressions can now be captured from actual speaker output and fingerprinted without invasive per-frame game logging.
