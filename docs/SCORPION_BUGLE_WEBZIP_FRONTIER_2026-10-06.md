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
