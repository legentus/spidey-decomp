# L1A3 Hostage Situation Auto-Web-Zip Frontier — 2026-10-07


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
