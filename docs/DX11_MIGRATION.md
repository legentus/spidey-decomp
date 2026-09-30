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
