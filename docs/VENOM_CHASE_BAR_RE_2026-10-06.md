# Venom Chase Bar Fragmentation RE — 2026-10-06

## Scope

This work is specifically for **Venom Chase / L5A1**, retail level ID `0x501`.

The affected HUD is the chase meter at the top of the screen that shows Spider-Man and Venom portrait markers moving closer/farther apart.

Retail draw function:
- `Venom_DisplayProgressBar @ 0x004E7E10`.

## Retail construction

`Venom_DisplayProgressBar` builds the chase HUD from multiple independent textured quads.

Relevant `Panel_SetStretchedScreenCoords(Texture*) @ 0x00462CD0` call sites inside the function:
- `0x004E7F44`
- `0x004E8160`
- `0x004E837E` — repeated meter-strip segment loop
- `0x004E859F`
- `0x004E87B6`

The repeated bar loop increments its virtual X position by `0x12` (18) each piece and spans the top-center of the 512x240 retail HUD canvas.

## Root cause

The existing high-resolution gameplay-HUD compatibility wrapper patches all calls to `Panel_SetStretchedScreenCoords`.

After retail generates a quad, `SpideyCompactGameplayUiPoly` chooses the quad's horizontal anchor independently:
- left anchor `0` when center < 40% of 512;
- center anchor `256` from 40–60%;
- right anchor `512` above 60%.

That policy is correct for independent HUD widgets, but incorrect for the Venom chase meter because its many quads are parts of one continuous composite.

### Direct L5A1 runtime proof

Fresh confirmed-working-Chase log:
`logs/20261006-031423/spidey-decomp.log`

Runtime settings:
- logical resolution: 2560x1440;
- gameplay UI scale: 180%;
- resulting density: X `0.45`, Y `0.60`.

Consecutive authored chase-bar pieces demonstrate the break:

`192..211`
- generic anchor: left / 0;
- transformed: `86..95`.

Immediately following `210..229`
- generic anchor: center / 256;
- transformed: `235..244`.

The authored pieces overlap/abut at 210–211, but the generic transform moves them roughly 140 virtual HUD units apart.

Second discontinuity:

`282..301`
- center anchor;
- transformed: `268..276`.

Immediately following `300..319`
- right anchor / 512;
- transformed: `417..425`.

This exactly explains the user's report that the chase meter is visibly broken into separated pieces.

The defect is therefore primarily caused by the existing per-widget HUD-anchor compatibility transform, not by Chase gameplay timing and not by a generic DX11 texture-sampling defect.

## Implemented candidate

Only the five coordinate-helper calls inside `Venom_DisplayProgressBar` are overridden after the generic HUD hook is installed.

In L5A1 only:
- every chase-meter piece uses one shared horizontal anchor: `X=256`;
- every piece uses the common top anchor: `Y=0`;
- the same existing user gameplay-UI density multiplier is retained;
- authored adjacent coordinates are transformed through the same affine mapping and therefore remain adjacent.

Outside level `0x501`, these call sites fall back to the existing generic gameplay-HUD transform.

No Chase timing/gameplay code is changed.

New telemetry:
`venom_chase_bar_scale level=0x501 policy=shared_top_center_anchor ...`

Install telemetry adds:
`venom_chase_bar_calls=5 venom_chase_bar_policy=level_0x501_shared_top_center_anchor`

## Validation status

- `git diff --check`: PASS.
- Local Commander currently blocks shell, batch, and matching `nmake.exe` execution by allowlist, so the local compile/test could not be launched from chat.
- Candidate must be built/tested with `TEST_LATEST_BUILD.bat`.
- Existing known-good Chase timing behavior at `3ec28e4b` remains untouched.

## Test

Run:
`F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat`

Then enter **Venom Chase / L5A1** and verify:
1. chase meter is one continuous bar instead of separated chunks;
2. Spider-Man/Venom portrait markers still move correctly;
3. normal Chase traversal through the building remains fixed;
4. other gameplay HUD widgets remain unchanged.

Exit normally afterward so the consolidated log can confirm all five L5A1 chase-bar hooks installed and executed.
