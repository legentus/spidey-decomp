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
