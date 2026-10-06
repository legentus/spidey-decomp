# Spider-Man 2000 — Full New-Chat Handoff
## 2026-10-06 — QuadBit Hor+ / Mysterio HUD & Laser Frontier

This document is the authoritative start-here recovery note for the current Spider-Man 2000 project frontier.

## 0. Start here in the new chat

Use Local Commander against:

`F:\Spider-Man 2000 Recomp\project main`

Then:
1. run `system_status`;
2. run `git_status`;
3. read this document completely;
4. read the tail of `docs/CURRENT_STATUS.md`;
5. do NOT resurrect retired Mysterio cadence experiments;
6. do NOT disturb the known-good Venom Chase phase-lock behavior;
7. first ask/pull the result of the currently pending runtime test for behavior commit `3cc47634`.

Authoritative branch:
`dev`

Remote:
`origin -> https://github.com/legentus/spidey-decomp`

## 1. Exact Git frontier

Current behavior commit:
`3cc47634b0094bd58d38593bd6177c706eef6572`
— `render: sync QuadBit projection with Hor+`

Current HEAD before this handoff document is added:
`c39b9a430ebd5e08ad4e2798a445f47eabc8a69c`
— docs-only `docs: normalize current status EOF`

Important: any later handoff-only commit is documentation/recovery only unless explicitly stated otherwise.

Known-good Venom Chase behavior:
`3ec28e4b2b7abb51f6166256df750690721fa475`
tag:
`chase60-known-good-20261006`

Known-good Venom Chase UI/bar fix:
`3fbe1482532deb4c5fb00976b02bc62cee97f27e`

## 2. Current runtime test — PENDING

The user is currently testing the new QuadBit Hor+ candidate.

Expected source behavior:
`3cc47634`

Do NOT mark this test passed or failed until the user reports back and the newest harness log is pulled.

Test targets:
- Mysterio helmet/head effect: should remain spatially attached while camera moves;
- ordinary character/NPC blob shadows: should remain under their owners while camera moves;
- Mysterio health fill vs holder: still expected to be unresolved unless telemetry reveals the exact mismatch;
- exit normally so `mysterio_health_alignment` and `quadbit_horplus` telemetry are archived.

Expected log markers:
- `quadbit_camera_anchor ... horplus_calls=1,1`
- `quadbit_horplus ... scalar=0.750000` at 2560x1440
- `mysterio_health_alignment ...`

## 3. Latest completed negative runtime before current pending test

Archive:
`logs\20261006-052334\spidey-decomp.log`

Runtime revision:
`4cdd166921307f5b6c92d736c7c4e873ae5e328f`

User result:
- Mysterio helmet/head effect still drifted relative to Mysterio as camera moved;
- NPC/character blob shadows still drifted from owners with camera movement;
- Mysterio health fill still was not inside its holder.

The log proves:
- correct `4cdd1669` runtime;
- QuadBit camera anchor installed;
- Mysterio retail camera mode 17/LOOKAROUND was active;
- therefore the drift is not caused primarily by modern camera ownership;
- full GTE rotation + DCX camera-matrix restoration was NOT sufficient.

## 4. World-space effect / QuadBit RE frontier

### 4.1 Affected class

Mysterio helmet ring/head effect:
- derives from `CQuadBit`.

Blob shadows:
- same world-space QuadBit family/path.

Retail draw:
`DisplayQuadBitList @ 0x004097E0`

Venom wrap/tentacle:
- `CVenomWrap : CNonRenderedBit`
- NOT the same class;
- do not assume a QuadBit fix solves the Venom body wrap.

### 4.2 DisplayQuadBitList has two transform domains

GTE path:
- subtracts `gMikeCamera.Position @ 0x0056F1B4/B8/BC`;
- calls `gte_rtps`;
- needs active camera rotation matrix.

DCX path:
- `DCX_XformVector @ 0x00402700`;
- active 4x4 at `0x0056E668`;
- DisplayQuadBitList copies `0x0056E6F8` into active DCX state before draw.

`M3d_RenderSetup @ 0x00472DC0` originally computes:
`matrix4x4_ml(result, 0x0056E778, 0x0056E570)`
and stores the result to `0x0056E6F8`.

Model rendering later reuses/overwrites `0x0056E6F8`.

The `4cdd1669` candidate rebuilt the DCX camera/projection matrix and restored GTE camera rotation before QuadBit draw. Runtime proved this alone was insufficient.

### 4.3 Real remaining mismatch: GTE projection vs model Hor+

Retail `gte_rtps @ 0x0046DBC0` directly uses:
- `0x0054F03C` = projection scale / GeomScreen;
- `0x0054F040` = horizontal GTE canvas basis;
- `0x0054F044` = vertical GTE canvas basis.

Retail defaults read from EXE:
- `0x0054F03C = 276`
- `0x0054F040 = 512`
- `0x0054F044 = 240`

`SetGeomScreen @ 0x00470610` writes only `0x0054F03C`.

`DisplayQuadBitList`:
1. projects world corners through GTE onto fixed 512x240 space;
2. scales projected X by `gGameResolutionX / 512`;
3. scales projected Y by `gGameResolutionY / 240`;
4. submits through `PCGfx_DrawQPoly3D @ 0x00508550`.

Critical difference:
- model/floating projection in `M3d_RenderSetup` reads aspect scalar `0x00550064`;
- GTE `gte_rtps` does NOT;
- model rendering is Hor+ while QuadBits remain on original 4:3 horizontal FOV.

### 4.4 Current behavior candidate — 3cc47634

Do NOT globally alter GeomScreen: it is shared by X and Y and would alter vertical FOV.

Patch only the two direct DisplayQuadBitList -> QPoly3D calls:
- `0x0040A1A9 -> PCGfx_DrawQPoly3D @ 0x00508550`
- `0x0040A367 -> PCGfx_DrawQPoly3D @ 0x00508550`

Wrapper:
`SpideyQuadBitQPoly3DHorPlus`

Horizontal correction:
`fixedX = centerX + (x - centerX) * *(float*)0x00550064`

where:
`centerX = gGameResolutionX / 2`

At 16:9:
- scalar should be ~0.75;
- horizontal displacement contracts to match model Hor+ projection;
- Y/depth/RHW/UV/color untouched;
- 4:3 scalar 1.0 => no change.

Telemetry:
`quadbit_horplus sample=... scalar=... logical_width=... x0=before->after ...`

Forced-clean matching VC6 compile/link:
PASS before current test.

## 5. Mysterio health-bar frontier — STILL UNRESOLVED

Retail boss item type:
`311 / 0x137`

`Panel_CreateHealthBar(...,311)` loads:
- `mysterio`
- `boss`
- `mysterio_wounded`

Relevant globals:
- boss type: `0x0060F654`
- active boss: `0x0060F788`

Renderer:
`Panel_DisplayHealthBar @ 0x00464270`

Important coordinate-domain finding:
- QPoly sites `0x00464C8A`, `0x00464EA5`, `0x004650B0` already multiply authored geometry by `gGameResolutionX / 512` and `gGameResolutionY / 240` before `PCGfx_DrawQPoly2D`.
- flat `0x004650EB` and Gouraud `0x0046512D/0x00465162` remain in authored 512x240 panel space.
- therefore treating all six calls with one generic HUD transform is wrong.

Latest completed log showed a live-space QPoly example roughly:
- before generic compaction: `2415..2515 x 168..276`
- generic scaler changed it to about `2494.75..2539.75 x 100.8..165.6`

Current behavior adds observation-only telemetry; it does NOT claim a health fix.

Holder telemetry callsites:
- texture holder: `0x00464CDE`
- frame holder: `0x00464EF8`

Telemetry prefix:
`mysterio_health_alignment`

Sources:
- holder texture after compaction;
- holder frame after compaction;
- QPoly fill live-space before compaction;
- flat/Gouraud authored-space before compaction.

Next step after pending test:
- pull newest log;
- compare exact holder rectangle to fill rectangle;
- solve by coordinate domain, not another generic-scale guess.

## 6. Mysterio laser frontier

### 6.1 Ground-truth 20-FPS reference

Archive:
`logs\20261006-040238\spidey-decomp.log`

User observed visibly different/correct laser behavior at full-engine 20 FPS.

### 6.2 Retired unsafe experiments — DO NOT RESTORE

Whole `CMysterio::AI` 20-Hz gate:
- commit lineage around `0a0eb4b9`;
- crashed during fight;
- changed unrelated boss state/upkeep;
- retired.

Whole `CMysterio::FireBoobies` 20-Hz gate:
- crashed when Mysterio began laser attack;
- static RE showed FireBoobies itself owns a multi-stage state machine and elapsed timers;
- retired.

### 6.3 Current laser policy

Commit:
`7768a49bdca71dfbe8623580e22e5cccefed4aff`
and carried forward in current source.

Engine/global timer:
60 Hz.

Mysterio AI:
60 Hz.

FireBoobies:
60 Hz pass-through/telemetry.

Only the two `CMysterioLaser::SetPos` callsites are sampled at authored cadence:
- left `0x0045D3AB`
- right `0x0045D44E`
- retail SetPos `0x0045B5E0`

This candidate compiled/linked and did not cause the earlier state-machine crash, but the user reported the lasers were not emerging from the chest nodules correctly. This may be because sampling SetPos at 20 Hz lets 60-Hz chest animation move between beam endpoint updates. Do not consider the laser work final.

## 7. Mysterio soft-spot / web damage finding

Retail:
- `CSoftSpot @ 0x0045F700`
- type `0x149`
- vtable `0x0053BB88`
- Hit slot `0x0053BB94 -> CSoftSpot::Hit @ 0x0045F940`

Retail Hit requires:
`SHitInfo.field_0 & 0x04`

Telemetry-only wrapper calls retail unchanged.

Latest runtime evidence:
- ordinary player web mode 1 reached a Mysterio soft spot with destructive flag `0x04`;
- node HP dropped `100 -> 0`.

User expects only web balls to break these nodes. This is a real follow-up item, but keep it separate from renderer/HUD work.

## 8. Venom Chase — KNOWN GOOD / DO NOT REGRESS

Behavior:
`3ec28e4b2b7abb51f6166256df750690721fa475`
tag `chase60-known-good-20261006`

Confirmed clean 60-FPS run:
`logs\20261006-031423\spidey-decomp.log`

Fix:
- phase-lock Venom `BaddyList @ 0x0056E990`
- with script-only camera `ControlBaddyList @ 0x0056E994`
- authored 3-canonical-tick cadence during Chase;
- render/global timer stays 60 Hz.

Final type-3/code-9 worker:
- camera heading 1029;
- desired world heading 2053;
- straight +Z building traversal;
- no forward wall collision;
- clean synth release.

Venom chase HUD bar:
`3fbe1482`
confirmed visually fixed by shared top-center composite anchoring.

Do not change either without explicit reason and regression test.

## 9. Build/test workflow

Always use:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

It:
- derives local revision;
- clears/seeds consolidated log;
- builds;
- installs;
- launches;
- waits;
- archives completed log.

Game:
`C:\Program Files (x86)\Activision\Spider-Man`

Live consolidated log:
`C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log`

Archived logs:
`F:\Spider-Man 2000 Recomp\project main\logs\YYYYMMDD-HHMMSS\spidey-decomp.log`

Do not launch SpideyPC.exe directly for instrumented tests.

## 10. High-value logs included in handoff ZIP

- `20261006-052334` — latest completed negative effect/health run at 4cdd1669
- `20261006-045343` — Mysterio renderer/camera evidence
- `20261006-041611` — failed whole-Mysterio-AI crash run
- `20261006-040238` — full-engine 20-FPS Mysterio laser reference
- `20261006-031423` — known-good 60-FPS Venom Chase confirmation
- `20261006-030007` — first known-good phase-lock Chase run

## 11. Immediate next action in new chat

First determine the result of the currently pending `3cc47634` runtime test.

If user says effects are fixed:
- pull newest log;
- verify `quadbit_horplus` scalar/callsite telemetry;
- promote QuadBit Hor+ fix;
- use `mysterio_health_alignment` telemetry to solve health bar.

If effects still drift:
- pull newest log FIRST;
- verify both Hor+ callsites installed and samples executed;
- compare effect coordinates/model projection before trying another transform;
- do not resurrect the already-failed DCX/GTE-only or camera-ownership theories.

Regardless:
- Mysterio health bar remains unresolved until exact holder/fill telemetry is analyzed;
- Venom Chase is frozen known-good;
- Mysterio web-node damage is a separate validated follow-up;
- Venom wrap/tentacle path is CNonRenderedBit and requires separate renderer tracing.


## 12. Objective telemetry from pending test archive 20261006-170406

A new archived harness log completed while this handoff was being assembled:

`logs\20261006-170406\spidey-decomp.log`

The user's visual verdict was not yet supplied at handoff time. Do not infer whether the effect drift is visually fixed until the user reports it.

Runtime identity:
- session revision: `c39b9a430ebd5e08ad4e2798a445f47eabc8a69c` (docs-only lineage over behavior commit `3cc47634`);
- proxy SHA-256: `90E45EF0E45C5CD67DE6D0476A3479E4BE940046D153359FEE99970FDC3175EE`;
- renderer11 SHA-256: `2CB3DAD337967FDB8998F843F7A2BD325361B5C894DF40E3A63D9AFFF7A304AE`;
- input11 SHA-256: `753D082EFD52BC971F867C016F2F014916A1ECEB87A03387B323AE9C34C3D3BF`.

QuadBit Hor+ execution proof:
- `quadbit_camera_anchor installed=1`;
- both Hor+ QPoly3D callsite patches installed = `1,1`;
- wrapper executed;
- `scalar=0.750000` at logical width 2560;
- sample X coordinates were actually transformed, e.g. `1030 -> 1092.5`, `1425 -> 1388.75`.

Therefore any visual result from this run is a valid test of the `3cc47634` Hor+ hypothesis, not a stale DLL or inactive hook.

### Health-bar telemetry now gives a decisive coordinate relationship

The current Mysterio QPoly fill is already in the same live-resolution rectangle as the compacted holder BEFORE the Mysterio QPoly wrapper applies the generic health-fill scaling again.

Example pair:
- holder texture after generic compaction: authored/panel rect `268..277 x 32..42`;
- converting that holder rect to 2560x1440 by retail 512x240 scaling gives `1340..1385 x 192..252`;
- immediately following Mysterio QPoly fill before generic compaction is exactly `1340..1385 x 192..252`.

Frame example:
- holder frame after compaction `416..428 x 32..42`;
- live conversion = `2080..2140 x 192..252`;
- following QPoly fill before generic compaction = exactly `2080..2140 x 192..252`.

This repeats across the holder-frame segments.

Conclusion:
- the Mysterio QPoly fill sites `0x00464C8A`, `0x00464EA5`, `0x004650B0` are ALREADY live-resolution-scaled to match the holder's eventual live rectangle;
- routing those live-space QPolys through `SpideyCompatHealthBarQPoly2D` applies an additional HUD compaction and causes the visible fill/holder separation;
- the next health candidate should make the active-Mysterio QPoly path PASS THROUGH unchanged after telemetry;
- keep authored-space flat/Gouraud handling separate; do not lump them together with the QPoly path.

This is now a much stronger health-bar diagnosis than the earlier generic scaling hypothesis.

Other objective telemetry from this run:
- Mysterio laser SetPos sampling installed and executed: calls=16, retail_calls=6, held_calls=10, max_elapsed=3;
- ordinary player web mode 1 again delivered destructive soft-spot flag `0x04` and 100 damage, confirming the web-node issue independently.
