# Scorpion / Race to the Bugle / Web-Zip Frontier — 2026-10-06


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
