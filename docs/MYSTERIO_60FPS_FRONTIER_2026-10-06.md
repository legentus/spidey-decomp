# Mysterio Boss Fight 60 FPS Frontier — 2026-10-06

## User report

In the Mysterio boss fight:
- Mysterio's health bar fill is detached from its holder at modern resolution/UI scaling.
- the lasers may be firing too frequently at 60 FPS.
- the fight should use the original retail boss camera, not the modern mode-3 camera or modern manual-aim camera logic.
- a full-engine 20 FPS reference run is approved to compare laser cadence.

## Current runtime evidence

Latest Mysterio session:
`logs/20261006-033344/spidey-decomp.log`

Runtime:
- revision `3fbe1482532deb4c5fb00976b02bc62cee97f27e`
- 2560x1440
- Gameplay UI Scale 180%
- gameplay UI density X=0.45 / Y=0.60

### Health bar

Retail Mysterio boss bar type:
- decimal 311 / hex `0x137`
- `Panel_CreateHealthBar(...,311)` loads `mysterio`, `boss`, and `mysterio_wounded`.

Active bar globals used as a stable boss-fight gate:
- `gHealthBarItemType @ 0x0060F654`
- `gHealthBarOne @ 0x0060F788`

The existing player/special-health fill hooks never execute in the Mysterio session. Mysterio uses the common boss branch in:
`Panel_DisplayHealthBar @ 0x00464270`

Common-boss direct geometry that bypassed the already-scaled holder:
- QPoly: `0x00464C8A`
- QPoly: `0x00464EA5`
- QPoly: `0x004650B0`
- flat: `0x004650EB`
- Gouraud: `0x0046512D`
- Gouraud: `0x00465162`

Candidate behavior:
- while active boss type == 311, those fill draws route through the existing gameplay-HUD scaling transforms;
- otherwise they call retail unchanged;
- holder behavior is unchanged.

### Retail-only Mysterio camera

Retail boss camera symbol:
`CCamera_CM_Boss3 @ 0x004192F0`

Modern camera/aim is now explicitly disabled while Mysterio boss type 311 is active:
- `SpideyModernAimIsEffectivelyActive` returns false;
- `SpideyModernMode3Camera` releases modern ownership and calls retail directly;
- `SpideyModernAimCameraPostprocess` also releases modern ownership and calls retail directly, covering transition frames.

This makes the retail boss camera authoritative throughout the fight without changing modern camera behavior elsewhere.

### Laser timing RE

Existing high-FPS repair:
- `CMysterioLaser::Move` liveness is already elapsed-time based using canonical `gTimerRelated`;
- it fixes premature beam destruction, not attack production cadence.

Laser producer:
- `CMysterio::FireBoobies @ 0x0045D200`
- creates/refreshes both `CMysterioLaser` objects.

Only caller:
- `CMysterio::AI @ 0x0045EF10`
- direct state-6 dispatch call `0x0045F489 -> 0x0045D200`.

AI state jump table:
- state 6 -> FireBoobies.

FireBoobies is staged by `dumbAssPad / field_320`:
- substate 0: attack setup;
- substate 1: rotation/animation setup;
- substate 2: beam create/refresh;
- then returns Mysterio to AI state 1.
It does NOT blindly construct two new beams every render frame.

Major Mysterio timers seen in AI use `CBaddy::RunTimer`, which subtracts elapsed `field_80`, so the remaining laser-frequency issue is not yet proven to be a simple raw countdown.

New telemetry patches the one state-6 callsite only:
`0x0045F489 -> SpideyMysterioFireBoobiesTelemetry`

It logs:
- canonical `gTimerRelated` tick;
- distinct laser attack number;
- call/substate count;
- `field_80`;
- `field_39C`;
- arm availability;
- state/substate before and after retail FireBoobies.

Policy is telemetry-only; no laser attack timing is changed in this 60-FPS checkpoint.

## Next diagnostic

Preserve this 60-FPS source checkpoint first.

Then create a temporary full-engine 20-FPS timer build using the same method that established the Venom Chase ground truth:
- ~50 ms retail TimerCallback delivery;
- retail converts that to ~3 canonical 60-Hz ticks;
- whole engine therefore executes authored 20-FPS update quanta;
- health-bar and retail-camera fixes remain in place;
- laser attack telemetry stays identical.

Compare 60 vs 20:
- distinct state-6 attack starts per canonical second;
- canonical tick spacing between attacks;
- substate durations;
- user-visible beam frequency/pattern.

Do not promote any broad Mysterio 20-Hz cadence fix until that comparison identifies the first real divergence.


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
