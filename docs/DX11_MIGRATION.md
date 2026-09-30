# Direct3D 11 Renderer Migration

## Goal

Replace Spider-Man 2000's DirectDraw 7 / Direct3D 7 renderer with a modern Direct3D 11 backend without rewriting gameplay, AI, physics, level loading, or content systems.

The migration is intentionally incremental. The retail-compatible proxy remains responsible for patching the original executable, while a separate modern x86 renderer DLL owns Direct3D 11 resources.

## Architecture

### Legacy proxy

Artifact:
- `binkw32.dll` (rebuilt `spider.dll`)

Responsibilities:
- retail EXE patching and verified address hooks;
- Bink forwarding;
- game/runtime compatibility shims;
- translating legacy renderer calls into a stable C ABI.

Toolchain:
- preserved VC6-era matching toolchain.

The legacy proxy must not include D3D11/DXGI headers.

### Modern renderer

Artifact:
- `spidey_renderer11.dll`

Responsibilities:
- Direct3D 11 device and immediate context;
- DXGI swap chain;
- render-target and depth-buffer allocation;
- shader/state management;
- texture resources;
- vertex/index streaming;
- final presentation.

Toolchain:
- modern MSVC / Windows SDK;
- x86 / Win32 target.

### Bridge ABI

Header:
- `renderer11/include/spidey_renderer11_api.h`

ABI version:
- `SPIDEY_RENDERER11_ABI_VERSION = 1`

Initial exports:
- `SpideyRenderer11_GetAbiVersion`
- `SpideyRenderer11_GetBackendName`
- `SpideyRenderer11_Probe`
- `SpideyRenderer11_Initialize`
- `SpideyRenderer11_Resize`
- `SpideyRenderer11_BeginFrame`
- `SpideyRenderer11_Present`
- `SpideyRenderer11_Shutdown`

The proxy resolves these functions dynamically with `LoadLibraryA` / `GetProcAddress`, so the old compiler never needs to understand modern DirectX types.

## Migration phases

### Phase 0 — bridge and device scaffold

Status: IMPLEMENTED, awaiting first build/runtime validation.

- Build `spidey_renderer11.dll` as a modern x86 DLL.
- Install it beside `SpideyPC.exe`.
- Legacy proxy loads the DLL during the existing DX initialization wrapper.
- Validate ABI version.
- Probe hardware D3D11 device creation.
- Do not alter visible rendering yet.
- Keep D3D7 at a known-good internal resolution during this phase.

Success criteria:
- matching proxy still builds;
- DX11 helper builds with VS 2022;
- one-click workflow installs both DLLs;
- `spidey-decomp-compat.log` records bridge ABI/probe success;
- `spidey-renderer11.log` records hardware feature level.

### Phase 1 — DX11 owns presentation

- Initialize the DX11 swap chain at the physical client size.
- Keep D3D7 drawing the game initially.
- Copy/import the completed legacy scene into a DX11 texture or staging path.
- Present exclusively through DXGI.
- Remove GDI `BitBlt/StretchBlt` presentation.

Purpose:
- eliminate legacy DirectDraw-primary presentation;
- establish stable native 16:9 output and resize behavior;
- keep scene rendering unchanged while presentation is modernized.

### Phase 2 — textures

Migrate `PCTex` resource ownership.

- convert indexed/paletted/16-bit source textures to modern RGBA upload data;
- create `ID3D11Texture2D` + shader-resource views;
- map legacy texture handles to DX11 resources;
- retain retail texture lookup/spooling semantics.

Success criteria:
- menu and gameplay textures match D3D7 output;
- no DirectDraw texture surfaces are needed for migrated assets.

### Phase 3 — 2D / frontend primitives

Migrate:
- sprites;
- quads;
- menu/background draws;
- font/text geometry;
- HUD.

Use a small orthographic shader path.

This phase is also where the 512x240 virtual shell coordinate system gets a proper 16:9 safe-area policy instead of being stretched independently on X/Y.

### Phase 4 — 3D fixed-function emulation

Translate the D3D7 fixed-function behavior used by `DXPOLY` into D3D11 shaders/state objects.

Known state families already centralized in the source:
- depth enable/write/function;
- alpha blending and source/destination blend factors;
- fog color/start/end;
- texture addressing U/V;
- min/mag filtering;
- texture-stage color/alpha operations;
- texture binding;
- triangle-fan submission.

Triangle fans will be expanded to triangle-list indices because D3D11 has no triangle-fan primitive topology.

### Phase 5 — true widescreen / FOV

Once gameplay is rendered to a real 16:9 target:

1. validate retail `M3d_RenderSetup` projection behavior;
2. preserve vertical FOV and expand horizontal FOV where needed;
3. separate gameplay viewport aspect from legacy 4:3 frontend layout;
4. add HUD/menu safe areas;
5. test cutscenes, model previews, mirrors/special cameras, and scripted camera modes.

Do not fake widescreen by stretching a 4:3 framebuffer.

### Phase 6 — make DX11 default

Only after visual parity:
- disable legacy D3D7 scene rendering by default;
- keep an optional D3D7 diagnostic/reference path temporarily;
- remove DirectDraw presentation dependencies;
- move modern resolution enumeration to DXGI;
- allow native 2560x1440 and higher resolutions based on actual DXGI output modes.

## Current known D3D7 limitation

The old renderer cannot currently boot with a 2560x1440 internal scene target.

Observed behavior:
- DirectDraw scene-surface creation succeeds;
- `IDirect3D7::CreateDevice` rejects the surface with `DDERR_INVALIDOBJECT`;
- this occurs before the first splash frame.

Therefore exact 2560x1440 remains quarantined from the legacy D3D7 mode list while the DX11 backend is only in scaffold/probe mode.

This is intentional safety behavior, not the final resolution policy.

## Build workflow

The standard one-click test workflow now builds both components:

1. matching VC6 proxy;
2. modern x86 `spidey_renderer11.dll`;
3. installs both into the retail game directory;
4. launches the game;
5. captures legacy compatibility logs plus `spidey-renderer11.log`.

DX11 build script:
- `scripts/build_renderer11.ps1`

DX11 output:
- `out/renderer11/spidey_renderer11.dll`

## Engineering rules

- Never replace high-level game systems solely to make renderer migration easier.
- Preserve retail behavior first, modernize second.
- Keep the DX11 ABI C-compatible and versioned.
- Keep modern DirectX headers out of the legacy matching proxy.
- Migrate one rendering responsibility at a time.
- Maintain a known-good D3D7 reference path until DX11 reaches parity.
- Do not call a resolution "supported" until a live DX11 render target, camera projection, UI, and presentation have all been runtime-validated.


## Phase 1 runtime bridge (implemented)

The first post-Phase-0 presentation bridge is implemented with ABI version 2.

For this transitional phase, D3D7 still renders the game scene. The legacy proxy retrieves the D3D7 scene HDC and passes it across the C ABI to `SpideyRenderer11_PresentHdc`. Renderer11 owns a GDI-compatible B8G8R8A8 DXGI swap chain, copies/aspect-fits the scene into its backbuffer, and performs the visible DXGI Present.

This is intentionally a migration bridge, not the final renderer architecture. It proves ownership of final presentation without yet porting textures, 2D primitives, or fixed-function 3D rendering. A runtime failure automatically falls back to the known-good direct-HWND presenter.

Native 2560x1440 remains quarantined from D3D7 during this phase. True 16:9 / 2560x1440 rendering follows after DX11 presentation is proven stable.


## Phase 1 status: PASSED

Runtime validation on revision `a16d29d7fc6c4b60a817ca25ddcd5ef4ef599ad3` confirmed:
- ABI 2 bridge loaded successfully;
- a 2560x1440 DX11/DXGI swap chain initialized;
- the D3D7 scene was presented through `SpideyRenderer11_PresentHdc`;
- `dx11=1 direct_hwnd=0` remained active through frontend transitions and live gameplay;
- at least 5160 presented frames were logged without falling back to the old direct-HWND path;
- the user reached the main menu, started a new game, entered gameplay, moved around, and exited normally.

Phase 1 therefore proves DX11 ownership of final visible presentation. It does not yet prove native 2560x1440 scene rendering: gameplay is still rendered by D3D7 at 1920x1440 and aspect-fitted into the 2560x1440 DX11 swap chain.


## Phase 2A — shader-upload presentation bridge

Status: implemented, awaiting runtime validation.

ABI 3 adds `SpideyRenderer11_PresentPixels`. The legacy D3D7 scene surface is locked after rendering, its 32-bit BGRA-compatible pixels are copied into a dynamic D3D11 texture, and a shader-model-4 fullscreen triangle performs the visible draw into the existing DX11 swap-chain render target.

The normal Phase 2A path contains no GDI/HDC presentation. The ABI-2 HDC path remains as an intermediate fallback, followed by the direct-HWND compatibility presenter as the final fallback.

This is deliberately a CPU upload bridge. It is not the final renderer architecture; its purpose is to establish:
- DX11 texture ownership;
- DX11 shader compilation/state;
- DX11 viewport/aspect handling;
- DX11 textured draw submission;
- a safe foundation for replacing individual D3D7 texture and primitive responsibilities.

### Phase 2B hook map

The reconstructed source exposes a clean migration seam:
- `PCTex_CreateTexturePVRInId`: central creation/conversion/upload point for game textures;
- `PCTex_ReleaseSysTexture`: central destruction path;
- `PCTex_GetDirect3DTexture`: legacy texture-handle lookup;
- `PCGfx_ProcessTexture`: selects a texture and forwards it to `DXPOLY_SetTexture`;
- `PCGfx_Draw*`: queues `DXPOLY` records containing the current DirectDraw texture surface;
- `PCGfx_BeginScene` / `PCGfx_EndScene`: high-level scene bracketing.

Phase 2B should introduce a sidecar DX11 texture table keyed by the existing 0..1023 game texture IDs, populate it from the same converted pixel data used by `PCTex_CreateTexturePVRInId`, release it in the same lifetime paths, and initially use it for migrated 2D/textured primitives while D3D7 remains available for non-migrated draws.


## Phase 2B — per-texture DX11 sidecar ownership

Status: implemented, awaiting runtime validation.

Renderer ABI 4 introduces per-game-texture DX11 ownership while retaining the legacy D3D7 draw path for parity testing. The existing PCTex ID (0..1023) is the canonical identity.

Creation path:
1. PCTex performs all original PVR/palette/pixel-format conversion into its system-memory staging DirectDraw surface.
2. The finished staging surface is locked and its real pitch/BPP/channel masks are passed to renderer11.
3. Renderer11 converts the surface into BGRA8 and creates a D3D11 texture + SRV under the same PCTex ID.
4. The final managed DirectDraw texture pointer is associated with that sidecar ID.

Destruction follows the existing PCTex release lifecycle and removes the corresponding DX11 texture/SRV and legacy-handle mapping.

The centralized `renderScene()` loop has also been mapped:
- sorted slots: 4096 down to 0;
- each `DXPOLY` contains a legacy texture-surface pointer, blend mode, texture flags, vertex count, and up to four `SDXPolyField` vertices;
- each vertex is 0x1C bytes: X, Y, Z-like value, RHW-like value, packed diffuse color, U, V;
- D3D7 submission is `DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vertices, count, 0)`;
- per-poly state selects blend mode, address U/V, texture alpha, and point/bilinear filtering.

For Phase 2B the loop only resolves each non-null legacy texture handle against the DX11 sidecar index and logs coverage. D3D7 still draws every polygon. This provides the evidence needed to activate Phase 2C without guessing texture coverage.


## Phase 2C0 — live retail primitive interception

Status: pass-through diagnostic hook implemented, awaiting runtime validation.

Phase 2B established reliable DX11 texture ownership, but its reconstructed `renderScene()` coverage logger produced no runtime records. This proves that the live retail EXE still owns the DXPOLY scene queue and primitive loop.

The migration seam therefore moves one layer lower: the live retail `IDirect3DDevice7` COM interface.

Recovered retail globals from the verified CreateDevice machine-code window:
- `IDirect3D7*`: `0x006B7918`;
- scene surface: `0x006B7908`;
- `IDirect3DDevice7*` output slot: `0x006B791C`.

The Phase 2C0 probe:
- validates `*(IDirect3DDevice7**)0x006B791C` through `GetCaps`;
- patches only vtable index 25 (`DrawPrimitive`);
- preserves and always invokes the original D3D7 method;
- resolves the current stage-0 DirectDraw texture against the Phase 2B DX11 sidecar index;
- samples FVF 0x144 TL vertices and captures relevant fixed-function state;
- aggregates per-frame primitive/FVF/texture coverage at the already-verified Flip boundary;
- reinstalls after display mode changes that may recreate the D3D7 device.

This step deliberately does not render any primitive with DX11 yet. Its output determines the exact fixed-function subset Phase 2C must emulate.

Once coverage/state data is validated, the next renderer should initially shadow the retail primitive stream into an offscreen DX11 target while D3D7 remains the visible source. Only after visual/state parity is established should the original D3D7 `DrawPrimitive` calls be suppressed.


## Checkpoint: retail stream is narrow enough for direct DX11 emulation

Runtime probing of the live retail `IDirect3DDevice7::DrawPrimitive` stream is complete enough to begin parallel DX11 geometry work safely.

Observed over a gameplay run:
- 2,423,890 draws;
- 100% `D3DPT_TRIANGLEFAN`;
- 100% FVF `0x144`;
- 2,384,258 textured draws;
- 2,383,026 already resolve to mirrored renderer11 textures;
- 1,232 misses across only 9 DirectDraw surface handles;
- 99.9483% texture coverage.

This narrows Phase 2C considerably. Rather than implementing a generic D3D7 compatibility layer, renderer11 can initially target the exact retail TL-vertex path:
- XYZRHW + diffuse + UV;
- triangle fan expansion;
- fixed-function texture modulation;
- observed DXPOLY blend modes;
- depth test/write;
- wrap/clamp;
- point/linear sampling;
- retail viewport mapping.

The next implementation must be a **shadow renderer** first. Retail D3D7 remains authoritative and visible while the same live DrawPrimitive calls are submitted to a separate DX11 color/depth target. Only after shadow coverage and state parity are proven should DX11 geometry become visible or original D3D7 draws be suppressed.

The remaining 9 unresolved legacy surface handles should be mirrored on-demand at the DrawPrimitive boundary rather than blocking the shadow-rendering work.


## Phase 2C2 — live DX11 shadow preview

Status: implemented, awaiting runtime validation.

Phase 2C1 proved that the retail primitive stream can be reconstructed into an independent offscreen DX11 target with complete sampled draw coverage and no observed state-cache divergence. Phase 2C2 makes that target inspectable without removing D3D7.

Preview architecture:
- renderer ABI 6;
- F10 is an opt-in runtime preview toggle;
- D3D7 continues executing every retail draw even while DX11 is visible;
- preview enable performs one warmup frame, then the DX11 shadow SRV is drawn directly into the existing swap-chain render target and presented through DXGI;
- the normal D3D7 scene -> PresentPixels path remains immediately available by pressing F10 again;
- a failed DX11 shadow present automatically falls back to the D3D7-reference presenter.

While preview is active:
- the proxy captures every eligible main-scene DrawPrimitive rather than only sampled frames;
- renderer11 replays the shadow command buffer every frame;
- parity readback remains sampled to avoid a per-frame GPU synchronization stall.

Parity logging:
- retail D3D7 `scene_pre` now logs the exact nine COLORREF values used by its hash;
- renderer11 `shadow_frame` logs the corresponding nine DX11 values;
- this supports diagnosis of half-pixel/rasterization, texture conversion, blend, depth, or color discrepancies rather than relying only on a single aggregate hash.

This is still a diagnostic parity stage. Native 16:9 and removal of the D3D7 DrawPrimitive path remain blocked until visual parity is acceptable.


## Phase 2C3 — DX11 geometry promoted to default visibility

Phase 2C2 passed runtime and visual validation. The user repeatedly switched the live shadow renderer on/off with F10 and reported that the DX11 image looked correct and possibly slightly better.

Runtime evidence from the validated F10 run:
- ABI 6 and all 7 live D3D7 hooks succeeded;
- continuous shadow presentation was stable;
- no renderer setup/map/present failures;
- no cache mismatches;
- no sampled shadow submission or replay skips;
- representative frame 4080 replayed 5,007/5,007 retail draws;
- exact D3D7/DX11 samples are close but not bit-identical, with median absolute channel delta of 1 across matched sample sets.

Phase 2C3 changes default ownership of the **visible image**:
- DX11 shadow/geometry rendering starts enabled;
- renderer11 continuous replay is synchronized at first Flip;
- DX11 scene -> DXGI is the normal presentation path;
- F10 is retained as a live D3D7-reference A/B switch;
- original D3D7 DrawPrimitive is still executed for now, so this is not yet removal of D3D7 geometry work.

The next gate after default-visible validation is controlled suppression of original main-scene D3D7 DrawPrimitive while the DX11 path remains authoritative.


## Phase 3A — desktop-native gameplay resolution and aspect

Status: implemented, awaiting runtime validation.

The renderer migration is now far enough along that physical D3D7 surface dimensions no longer need to define the visible game's resolution. Phase 3A separates three concepts:

1. **physical D3D7 compatibility surface** — retained only so the legacy device/state/resource path remains alive;
2. **logical game resolution** — the dimensions the retail engine sees for gameplay projection/layout;
3. **DX11 render/swap target** — the actual modern visible resolution.

For gameplay, logical dimensions now follow the borderless client. On a 2560x1440 desktop the engine is told 2560x1440 while the known-good D3D7 backing remains 1920x1440. Captured TL vertices are replayed using a 2560x1440 logical viewport into a 2560x1440 DX11 color/depth target.

The frontend remains on its original 640x480 logical/physical canvas for the first pass. This intentionally separates gameplay widescreen/FOV work from menu/HUD modernization.

The implementation is aspect-generic: the logical target follows the client rather than hard-coding 16:9. 16:9 is the first validation target; 16:10/ultrawide can use the same mechanism once projection behavior is proven.

A geometry-range diagnostic was added to establish whether the retail software projection naturally becomes Hor+ when `gGameResolutionX/Y` are widened. If not, the next stage will hook only the projection/FOV setup while keeping this native DX11 target architecture.


## Phase 3B — explicit modern video settings

Status: implemented; awaiting runtime validation.

The original Display Options screen is retained rather than replaced. Its three rows are now used as:
1. Screen Size
2. Aspect Ratio
3. Brightness

Screen Size continues using the retail resolution-navigation code, but the injected table now exposes 2560x1440. A selected modern output is persisted independently from the D3D7 compatibility backing. This is the key architectural change that lets the UI honestly say 2560x1440 while the old device quietly receives 1920x1440 only where required.

The old Color Depth row is obsolete because the modern DX11 output is always 32-bit. Its value formatter and left/right handlers are therefore redirected, with exact retail target verification, to a seven-mode aspect selector. The selector writes the game's existing projection/aspect scalar at runtime rather than adding a separate post-process stretch.

Aspect modes:
- AUTO
- 4:3
- 5:4
- 16:9
- 16:10
- 21:9
- 32:9

AUTO derives the correction from the selected Screen Size. Explicit modes use their corresponding projection scalar. The aspect choice persists in `spidey-modern-video.ini` beside the executable.

This phase intentionally leaves the retail 640x480 frontend canvas intact. The immediate target is native-resolution/widescreen gameplay with a stable original menu; frontend/HUD layout modernization remains a later phase after gameplay projection is validated.


## Phase 3C — transactional display settings

Display Options now uses an explicit **Apply** transaction instead of letting the retail menu mutate persisted resolution fields while the user scrolls.

Why this was required:
- the retail Screen Size row uses the saved width/height globals as its working variables;
- pressing Enter anywhere in the old menu re-called `DXINIT_SetDisplayOptions`;
- the former Color Depth row also ran resolution-compatibility searches after each change;
- after Color Depth became Aspect Ratio, those searches could change Screen Size unexpectedly;
- this conflicted with the DX11 architecture where selected output resolution and legacy D3D7 backing resolution must remain separate.

New model:
- committed modern output: `gSpideySelectedOutputWidth/Height`;
- pending menu output: separate pending width/height;
- committed aspect mode: `gSpideyAspectMode`;
- pending menu aspect: separate pending aspect mode;
- Screen Size and Aspect Ratio formatting read pending state;
- retail next/previous-resolution algorithms are still reused, but operate on local temporary values and only update pending state;
- Color Depth's obsolete aspect->resolution compatibility searches are disabled;
- a fourth retail CMenu entry, **Apply**, is appended after Brightness.

Apply semantics:
- Enter on rows 0–2 no longer rebuilds/resets display state;
- Enter on row 3 commits pending output/aspect;
- the original `SPIDEYDX_SaveSettings @ 0x00515850` is called immediately;
- frontend 640x480 remains physically unchanged while the modern selection is committed;
- when gameplay activates, the selected modern output drives logical DX11 dimensions and 2560x1440 continues using the safe 1920x1440 D3D7 backing;
- if Display Options is reached while gameplay is already active, Apply can rebuild the compatibility backing immediately.

This preserves the original menu/input/rendering code while giving the DX11 path modern, explicit configuration semantics.


## Phase 3E — guarded D3D7 main-scene raster suppression trial

Implementation frontier:
- `502bd2864c5e6fd8a1e268f18656591a8b709d0c`.

Goal:
- test whether retail D3D7 main-scene rasterization has any remaining hidden functional dependency now that DX11 replays the observed visible primitive stream.

Diagnostic control:
- F9 toggles suppression;
- default OFF;
- F10 reference mode always restores full D3D7 main-scene drawing automatically because suppression requires active/ready DX11 geometry mode.

Suppression is per-draw and fail-closed. A D3D7 main-scene draw is skipped only when:
1. the draw targets the retail main scene;
2. DX11 geometry mode is enabled and ready;
3. DX11 accepted that exact primitive/state command;
4. the texture is already mirrored/resident or the draw is untextured.

Never suppressed:
- offscreen render targets;
- unsupported primitive/state combinations;
- unresolved transient textures;
- DX11 warmup frames;
- F10 D3D7 reference mode.

This means the experiment does **not** yet remove the D3D7 device, state calls, resource surfaces, movie blits, or offscreen raster work.

Static dependency notes:
- retail `PCMovie` writes movie frames to `g_pDDS_Scene` through DirectDraw `Blt`, not through the hooked main-scene `DrawPrimitive`; F9 therefore does not suppress the movie copy itself;
- retained retail `DXPOLY_SaveScreen @ 0x005033E0` machine code accesses the primary surface at `0x006B7904`, not the main scene surface at `0x006B7908`; it is not a direct consumer of the suppressed main-scene DrawPrimitive path;
- offscreen draw work remains active specifically because transient/resource generation may depend on it.

Telemetry:
- `d3d7_main_draw_suppression ... key=F9`;
- per-frame `d3d7_suppressed`;
- per-frame `d3d7_fallback`.

Pass condition for later runtime testing:
- normal visual/game behavior while a large majority of steady-state main-scene draws are suppressed;
- no transition, readback, pause/menu, HUD, movie, or special-effect regressions;
- fallback count tends toward zero after transient resources settle.

Only after that runtime proof should suppression become default or further D3D7 state/device responsibilities be removed.
