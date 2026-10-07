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
