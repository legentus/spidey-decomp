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
