# Spider-Man 2000 — Full New-Chat Handoff — 2026-10-07 — Surface First-Person Aim Frontier

This is the authoritative start-here recovery note for the current Spider-Man 2000 decomp/runtime-compat project.

## 0. Start here in the new chat

Use Local Commander against:

`F:\Spider-Man 2000 Recomp\project main`

Then, in order:

1. run `system_status`;
2. run `git_status`;
3. verify branch `dev`;
4. read this file completely;
5. read the tail of `docs/CURRENT_STATUS.md`;
6. pull the newest live/archive runtime log **before changing camera/aim code**;
7. verify the runtime revision in the newest log before interpreting behavior.

Authoritative remote:

`origin -> https://github.com/legentus/spidey-decomp.git`

Authoritative branch:

`dev`

## 1. Exact Git frontier

Current behavior/source commit at handoff creation:

`56b191cef67887fdd2562a5ac99f0aeb01247820`

Subject:

`Use first person aim on walls and ceilings`

This is the current behavior candidate.

The handoff/documentation commit created after this note may move HEAD forward without changing runtime behavior. Always distinguish the **behavior commit above** from a later docs-only handoff commit.

Working tree was clean and synchronized with `origin/dev` before this handoff note was created.

## 2. Current user-visible frontier

The immediately preceding camera/Zipline build at:

`1a82373aa03a81b779de5ca30168323c7c467081`

was described by the user as:

> “wow that is damn near perfect”

Confirmed good at that point:

- generic web-zip travels correctly at native 60 Hz;
- quick/non-aimed Zipline works;
- camera-directed aimed Zipline works;
- aimed Zipline targets the visible reticle/camera ray instead of legacy surface-normal direction;
- stale reticle after Zipline was fixed;
- final camera collision against walls/floors/ceilings was substantially corrected;
- normal modern camera pitch reaches effectively ±90 degrees;
- floor manual aim works well;
- aimed Zipline no longer tunnels through geometry because of the old mismatched target path.

The one remaining user complaint was:

- while ceiling crawling, manual aim reticle stayed centered on Spider-Man and could not target the world properly;
- user suspected wall crawling had the same issue.

## 3. Current behavior candidate — surface-only first-person manual aim

Commit:

`56b191cef67887fdd2562a5ac99f0aeb01247820`

The prior projected-third-person wall/ceiling framing experiment from `1e839d2` was superseded.

Current explicit policy:

- **floor manual aim**: preserve validated third-person mode-3 camera;
- **wall or ceiling manual aim**: use true retail FRONT / mode-7 first-person camera;
- release Aim: return to pushed third-person crawler camera;
- aimed Zipline: continue using the exact visible center-camera ray;
- floor modern camera, full ±90 pitch, and final camera collision remain intact.

Retail proof behind this:

`CPlayer::EnterLookaroundMode @ 0x004C3580`
- sets `player+0x8EA = 1`;
- calls `CCamera::PushMode @ 0x00416720`;
- pushes mode `7`;
- calls `CCamera::SetMode @ 0x004167F0`;
- mode 7 is `CAMERAMODE_FRONT`.

`CPlayer::ExitLookaroundMode @ 0x004C3810`
- clears aim/reticle state;
- calls `CCamera::PopMode @ 0x00416780`.

`CCamera::AI @ 0x00417CB0`
- mode 3 -> `CM_Normal @ 0x00418E00`;
- mode 7 goes straight to shared postprocessing after player-owned first-person setup.

`CPlayer::SetupLookaroundCamera @ 0x004C38A0`
owns the actual first-person placement/orientation.

### Current surface first-person controller

New helpers in `main.cpp`:

- `SpideySurfaceFirstPersonAimPrepare`;
- `SpideySurfaceFirstPersonAimRelease`.

On wall/ceiling + active Aim + mode 7:

1. seed world yaw/pitch from the visible camera ray;
2. apply modern mouse/right-stick camera deltas;
3. clamp pitch to ±1024 game-angle units = ±90 degrees;
4. convert world yaw to body-relative yaw using `CPlayer::GetEffectiveHeading`;
5. write both target and smoothed retail look-angle globals together;
6. run retail `SetupLookaroundCamera`.

Retail look-angle globals:

- `0x006A818C` pitch target;
- `0x006A82B4` smoothed pitch;
- `0x006A7FFC` body-relative yaw target;
- `0x006A8D54` smoothed body-relative yaw;
- `0x006A8D44` previous/effective body heading.

Telemetry:

`modern_manual_camera event=surface_first_person_angles ...`

### Movement policy while surface aiming

Surface first-person aim deliberately does **not** hide `field_8EA`.

This means retail aim owns the player while wall/ceiling first-person aim is held, preventing the old simultaneous crawl-movement / lookaround conflict.

Ordinary crawling with Aim released is untouched.

### Aimed Zipline in surface first-person mode

`SpideyModernAimApplyCameraPoint` accepts:

- floor mode 3 -> existing third-person framed focus;
- wall/ceiling mode 7 -> final first-person camera focus.

`SpideyTryModernAimedR1Zip` accepts both:

- floor third-person mode 3;
- surface first-person mode 7.

Successful aimed Zipline from FRONT mode:

- pops retail camera mode;
- clears first-person sidecar;
- clears aim/reticle state;
- then lets existing state `0x40000` zip travel own the player.

The old retail lookaround action tail at `player+0x54F` is forced inactive during the surface first-person setup so it does not compete with custom camera-directed aimed Zipline.

## 4. Current runtime test — PENDING

At handoff creation, the user said they were testing the latest work.

The current behavior candidate to validate is:

`56b191cef67887fdd2562a5ac99f0aeb01247820`

Do **not** assume this candidate passed until the newest runtime log is pulled and the user gives a visual verdict.

Highest-value test:

1. crawl on a ceiling;
2. hold manual Aim (current keyboard binding: Left Ctrl);
3. verify camera switches to usable first-person aim;
4. move mouse through yaw and full up/down pitch;
5. verify reticle targets the scene rather than Spider-Man;
6. Aim + Zipline (current keyboard Zipline binding: top-row `3`) at a valid visible surface;
7. verify zip follows the visible center ray;
8. release Aim and confirm third-person ceiling camera returns;
9. repeat on a wall if convenient;
10. verify ordinary floor third-person aim remains unchanged.

Expected telemetry:

- `modern_manual_camera event=surface_first_person_angles`;
- camera mode 7 / FRONT while wall/ceiling aiming;
- `web_zip_aimed ...` if aimed Zipline is used;
- mode 3 again after releasing Aim / completing aimed Zipline.

## 5. Prior wall/ceiling framing experiment — superseded

Commit:

`1e839d2ef3ccde2bcb6aa28c9bfbe08f0523062a`

Subject:

`Fix wall and ceiling manual aim framing`

It attempted to preserve third-person aiming on walls/ceilings by projecting world-up into camera screen-space.

Reason for superseding it:

- the reticle issue persisted conceptually because third-person framing on arbitrary crawl surfaces remained fragile;
- current direction is explicit first-person surface aiming.

Do not re-promote the projected-screen-up approach unless the user explicitly asks to return to third-person wall/ceiling aiming.

The code remains in source for floor/framing support but active wall/ceiling manual aim now uses mode 7.

## 6. “Crash during ceiling drop attack” investigation — do not misdiagnose

The user reported a crash while doing a ceiling drop attack on `1e839d2`.

Archived log:

`logs\20261007-033631\spidey-decomp.log`

Revision:

`1e839d2ef3ccde2bcb6aa28c9bfbe08f0523062a`

Important facts:

- no access violation marker;
- no `spidey-decomp-crash.log`;
- process exit code:
  `-1073741510 = 0xC000013A`;
- this is Windows console/control termination, **not** `0xC0000005`.

The log proves the ceiling drop attack itself advanced:

- tick 3753: state `0x01000000`, anim `133`;
- tick 3770: anim `134`, back on ground;
- later combat animations continue:
  `100 -> 103 -> 104 -> 105`;
- only afterward does process termination occur.

Manual aim was already off during the drop attack (`aim=0`), so the wall/ceiling framing helper was not active during that transition.

A later run of the same revision completed cleanly:

`logs\20261007-034231\spidey-decomp.log`

- revision `1e839d2...`;
- `exit_code=0`.

Therefore:

- do not treat the earlier `0xC000013A` run as proof of a code crash;
- do not blame the startup `PCTex.cpp line 1740: Unknown (00000001)` warning without a real exception;
- only investigate further if a reproducible fault produces a real crash signature / access violation / stack overflow / consistent terminal state.

## 7. Web-Zip root cause and final 60-Hz fix

Retail generic web-zip initiation was proven to work before movement failed.

Key log:

`logs\20261007-023252\spidey-decomp.log`

Revision:

`9be749173827aa546245d2f098e870fb010ea018`

Evidence:

- Hostage Situation scripted R1 zip succeeded;
- manual R2 zip succeeded;
- availability/raycast succeeded;
- state transitioned into `0x40000`;
- animations 270/271 progressed;
- velocity was nonzero;
- player position remained frozen.

Root cause:

Retail special zip displacement at `0x00466D90+` does:

1. `mPos += mVel`;
2. if `field_80 <= 2`, return;
3. otherwise apply catch-up displacement.

An old native-60 patch changed comparison threshold from 2 -> 0.

At `field_80 == 1`:

- full velocity was added;
- code fell through;
- catch-up used `field_80 - 2 == -1`;
- same displacement was subtracted;
- net movement = zero.

Correct fix commit:

`4079f2cd79d69ea5f8b4704e1fc0aa283bb07d08`

Subject:

`Fix native 60 web zip displacement`

Fix:

- remove the bad threshold rewrite at `0x00466DC9`;
- hook only vector add call `0x00466DBC -> CVector::operator+= @ 0x004E7590`;
- at zip state `0x40000`, anim 270/271, `field_80==1`, add `mVel >> 1`;
- all other elapsed-tick values use retail unchanged;
- keep original retail `cmp field_80,2 / jle`.

Telemetry:

`web_zip_move_halfstep ...`

Preserve this fix.

## 8. Camera-directed aimed Zipline

Commit lineage:

`ceb17d4cdc5ef9c6758d907894ab634909ccf982`

Subject:

`Add aimed web zip and full pitch camera collision`

Aimed Zipline no longer reuses the unrelated legacy surface-normal target.

Behavior:

- Aim + Zipline uses the actual center-camera ray;
- cast from final camera position into world;
- derive player-to-camera-hit direction;
- temporarily steer only retail R1 query direction;
- let retail own range checks, non-zippable-face checks, web creation, target/normal capture, animation 270, state `0x40000`, travel, and landing;
- restore Spider-Man's real crawl/surface basis immediately.

If aimed ray hits no valid surface:

- do not silently fall back to legacy quick zip.

Quick/non-aimed Zipline remains retail surface-normal behavior.

Current keyboard controls from live `Spidey.cfg`:

- Aim = Left Ctrl;
- Zipline = top-row `3`.

## 9. Camera pitch and collision frontier

Commit lineage:

`ceb17d4...` plus `1a82373...`

Modern camera pitch is now explicit game-angle state:

- full circle = 4096;
- pitch clamp = `-1024..+1024`;
- effectively ±90 degrees;
- at exact pole, horizontal arm retains a 1-unit component to avoid legacy normalization/divide edge cases.

Camera collision:

`SpideyModernCameraClipToWorld`

uses retail world-line collision.

Important ordering fix in:

`1a82373aa03a81b779de5ca30168323c7c467081`

The clamp must run **after**:

`CCamera_MoveToDesiredPos @ 0x00416B10`

because that routine can move `camera->mPos` after mode-3 generated the desired camera.

Retail caller then runs:

`Utils_CalcAim @ 0x004E62D0`

from the clipped final camera position.

User feedback after `1a82373`:

> “wow that is damn near perfect”

Do not move the collision clamp back before `MoveToDesiredPos`.

## 10. Stale Zipline reticle fix

Root cause:

`CPlayer::RenderLookaroundReticle` draws based on `field_DE4`, not merely `field_8EA`.

A stale `field_DE4=1` survived after aim was dropped.

Quick Zipline then changed `field_DC0` to the zip target, making the stale reticle appear stuck at the new destination.

Fix in `1a82373...`:

- successful R1/R2 Zipline clears `field_DE4`;
- calls `Screen_TargetOn(false)`;
- aimed-zip drop path does the same.

This was user-confirmed as part of the near-perfect result.

Preserve it.

## 11. High-value current logs included in this handoff

### 20261007-034231
`logs\20261007-034231\spidey-decomp.log`

Revision:
`1e839d2ef3ccde2bcb6aa28c9bfbe08f0523062a`

Exit:
`0`

Use:
- proves projected-framing revision can complete normally;
- useful comparison against earlier `0xC000013A` report.

### 20261007-033631
Revision:
`1e839d2...`

Exit:
`-1073741510 / 0xC000013A`

Use:
- reported ceiling drop-attack “crash”;
- log proves drop attack and later combat continued;
- not an access violation.

### 20261007-032721
Revision:
`1a82373aa03a81b779de5ca30168323c7c467081`

Exit:
`0`

Use:
- near-perfect baseline;
- final-position camera collision;
- stale Zipline reticle fix.

### 20261007-024029
Revision:
`4079f2cd79d69ea5f8b4704e1fc0aa283bb07d08`

Use:
- live aimed-zip tunneling evidence before camera-directed aimed zip.

### 20261007-023252
Revision:
`9be749173827aa546245d2f098e870fb010ea018`

Exit:
`0`

Use:
- decisive generic web-zip zero-motion root cause.

## 12. Other major resolved systems — preserve

### Mysterio lasers
User explicitly said:

> “Okay finally mysterios lasers feel perfect so we can move past that.”

Current policy:
- FireBoobies AI remains 60 Hz;
- SetPos authored simulation sampled at 20 Hz;
- 60-Hz visual emitter follow;
- LookAt/Yaw cadence correction only where needed.

Do not resurrect whole-AI or whole-FireBoobies throttles.

### Game-wide legacy effect anchoring
Resolved:
- Mysterio helmet/head effect;
- NPC/blob shadows;
- Mysterio chest-laser origins;
- Venom tentacle/body effects.

Root:
legacy effect projection expects `SCamera::View @ 0x0056F224`, not Transform.

Preserve shared View-matrix restoration.

### Mysterio health bar
Holder/fill domain issue was fixed in later lineage; user confirmed health fill/holder correct before moving on.

### Venom Chase
Known-good authored-cadence phase-lock behavior is documented in existing handoff/status docs.

Do not regress.

### Intro movie alt-tab
DirectSound lost-buffer recovery fix is in commit:

`7f07f7d813bbeb65603c44bae5a000294e604bdc`

Preserve.

## 13. Other unresolved/secondary items

### Scorpion fight termination
Separate unresolved gameplay issue.

Telemetry wrapper exists around retail `CScorpion::AI @ 0x00488590`.

Do not patch incomplete `scorpion.cpp` blindly.

### Scorpion Race-to-Bugle bar
Shared top-right holder candidate was implemented. Refer to:

`docs/SCORPION_BUGLE_WEBZIP_FRONTIER_2026-10-06.md`

Check current status/log before revisiting.

## 14. Build/test workflow

Always use:

`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

Game:

`C:\Program Files (x86)\Activision\Spider-Man`

Live consolidated log while game runs:

`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

Archived completed logs:

`F:\Spider-Man 2000 Recomp\project main\logs\YYYYMMDD-HHMMSS\spidey-decomp.log`

Matching compiler root:

`%LOCALAPPDATA%\Spidey2000Dev\MatchingVS`

For forced-clean local build through Local Commander, invoke `build.bat` with:
- `SPIDEY_MSVC_ROOT` set to MatchingVS;
- `SPIDEY_FORCE_CLEAN=1`.

## 15. New-chat immediate action

The very first runtime question is:

**Did behavior commit `56b191ce` fix manual aiming on walls/ceilings?**

Do not ask the user to re-upload logs if Local Commander is available.

Instead:

1. pull the newest live/archive `spidey-decomp.log`;
2. verify `[SESSION] revision=`;
3. if revision is `56b191...`, inspect:
   - `modern_manual_camera event=surface_first_person_angles`;
   - camera mode changes;
   - player wall/ceiling flags;
   - `web_zip_aimed` if used;
4. combine that with the user's visual verdict.

If surface first-person aim works:
- mark `56b191ce` confirmed;
- preserve floor third-person aim;
- preserve final camera collision;
- preserve aimed/quick Zipline;
- move on to the next gameplay frontier.

If it fails:
- diagnose from mode-7 / angle telemetry first;
- do not restore the `1e839d2` screen-up framing experiment by default;
- compare retail FRONT setup globals and `SetupLookaroundCamera` lifecycle;
- keep changes scoped to wall/ceiling Aim.

## 16. Local Commander interruption recovery

Connection/input-stream interruptions are common.

On recovery:

1. `git_status`;
2. read this handoff;
3. read tail of `CURRENT_STATUS.md`;
4. pull newest log;
5. establish exact runtime revision;
6. continue from evidence.

Never assume work was lost solely because the chat/tool stream disconnected.

## 17. User workflow preferences

- fewer test pauses; prefer larger coherent batches;
- keep `docs/CURRENT_STATUS.md` live-updated;
- commit/push meaningful source changes frequently;
- static/build-check before asking for runtime test;
- only ask for in-game tests when they produce high-value evidence;
- pull logs directly through Local Commander rather than asking user to upload them;
- preserve robust handoffs at chat limits/frontier changes.
