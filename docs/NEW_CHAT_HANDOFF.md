# Spider-Man 2000 PC Decomp / Modernization — NEW CHAT HANDOFF

Date: 2026-09-30  
Active repository: https://github.com/legentus/spidey-decomp  
Active branch: `dev`  
Upstream: https://github.com/krystalgamer/spidey-decomp  
Tools upstream: https://github.com/krystalgamer/spidey-tools  
User orchestration repo: https://github.com/legentus/Spiderman-2000
Retail Google Drive source: https://drive.google.com/drive/u/0/folders/1xtk0kTTi9LNQnVLo3_NHkB5mkfzmfGKx

## READ THIS FIRST

Do **not** restart the investigation.

Before changing code:
1. Read this file.
2. Read `docs/CURRENT_STATUS.md` completely, starting from the newest entries at the bottom.
3. Confirm the current `dev` head.
4. Treat the repository, not chat memory, as authoritative for the current source.
5. Preserve the user's disconnect-safe workflow: update `docs/CURRENT_STATUS.md` **during** substantial work after important findings, failed hypotheses, code changes, and before asking for a runtime test. Do not wait until the end of the chat.

The user will normally send **all logs from every test**. Treat the complete log set as standard input.

## MANDATORY WORK STYLE

- Never fabricate reverse-engineering state, offsets, bytes, signatures, or call conventions.
- Verify retail/source behavior before patching.
- Use exact-byte guards for retail instruction patches when possible.
- Do static verification before asking the user to test.
- Ask for an in-game test only when it will resolve a real runtime uncertainty.
- Keep `master` clean/upstream-oriented; active development is on `dev`.
- Use GitHub connector writes for repo updates.
- The user's normal UI is BAT-first. Do not make manual PowerShell the everyday workflow.
- Never promise background work.
- Retail game binaries/assets do not belong in Git.
- The user's executable can be debugged for compatibility, but do not create or distribute DRM bypasses.

## USER / LOCAL WORKFLOW

Local project:
`F:\Spider-Man 2000 Recomp\project main`

Game:
`C:\Program Files (x86)\Activision\Spider-Man`

Matching VS toolchain:
`C:\Users\alh60\AppData\Local\Spidey2000Dev\MatchingVS`

Normal test:
```bat
UPDATE_SPIDEY_PROJECT.bat
TEST_LATEST_BUILD.bat
```

The test script:
- force-cleans the matching VS6-style build;
- installs rebuilt `Release\spider.dll` as game `binkw32.dll`;
- preserves retail Bink as `binkw32_.dll`;
- fingerprints `SpideyPC.exe`;
- launches the game;
- collects logs into the timestamped project `logs` folder.

## EXACT GAME EXE FINGERPRINT

Current and repeatedly verified:

```text
path=C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe
sha256=D55A0BB0E920C497CE1CA76F08ED2E62FEEFCB6FF3C2901C0D59890F099BA93C
file_size=1507328
machine=0x014C
timestamp=0x3B7A3167
entrypoint_rva=0x0012B46F
image_base=0x00400000
size_of_image=0x02A0D000
characteristics=0x010F
.text va=0x00001000 vsize=0x00139CE5 raw=0x00001000 rawsize=0x0013A000
.rdata va=0x0013B000 vsize=0x0000AFE8 raw=0x0013B000 rawsize=0x0000B000
.data va=0x00146000 vsize=0x028C5538 raw=0x00146000 rawsize=0x00029000
.rsrc va=0x02A0C000 vsize=0x00000DE8 raw=0x0016F000 rawsize=0x00001000
```

## PROXY ARCHITECTURE

The project is not yet a standalone replacement EXE.

Current Windows architecture:
1. retail `SpideyPC.exe` loads `binkw32.dll`;
2. rebuilt `spider.dll` is installed as proxy `binkw32.dll`;
3. retail Bink is preserved as `binkw32_.dll`;
4. proxy forwards Bink exports;
5. proxy patches selected retail functions to reconstructed C++.

This lets us incrementally replace/fix code while the retail executable remains the host.

## CURRENT DEV FRONTIER

Presentation-probe implementation:
- `6db8ea90d2e6ebe27aa61a5eeaccdc890674915f`
  - `diag: trace and correct windowed DirectDraw presentation`

Presentation-log collection:
- `938229983c79c8d91436f54c9d07a13b90bd3ef8`

Latest evidence/status checkpoint immediately before this handoff:
- `54b57daa9576ac7b41131441cdd599be68628dc7`
  - `docs: isolate black screen to legacy DirectDraw presentation`

The branch head will advance when this handoff document itself is committed. No runtime code is changed by the handoff commit.

## CURRENT USER-VISIBLE BEHAVIOR

Latest user report:
- visible game window is a black box;
- **audio works**;
- input works;
- user can press Start and hear/enter the main menu state;
- no crash is required for the black condition;
- game logic continues underneath the black window.

This is **not currently a general startup failure**.

## WINDOWED DIRECTDRAW COMPATIBILITY — KEEP THIS

Original exclusive fullscreen path failed at:
`IDirectDraw7::SetDisplayMode`

Original HRESULT:
`0x80004001` = `E_NOTIMPL` / `DDERR_UNSUPPORTED`

Forcing 32 bpp still produced the exact same error, so 16-bit color was ruled out.

Current compatibility patch changes the retail `DXINIT_DirectX8` third argument from `2` to `3`, enabling the game's own built-in windowed DirectDraw branch while preserving the existing bit.

Verified caller:
- push site `0x00515BA9`
- call site `0x00515BAD`
- target `DXINIT_DirectX8 = 0x004FDE90`

Do **not** go back to testing 16-vs-32-bpp SetDisplayMode unless genuinely new evidence appears.

## CURRENT BLACK-SCREEN FINDING — MOST IMPORTANT SECTION

The renderer is producing real, changing pixels.

Retail presentation path:
- `DXPOLY_EndScene = 0x00502A40`
- retail `DXPOLY_Flip = 0x00502990`
- exact call site from EndScene to Flip = `0x00502D41`
- original call bytes = `E8 4A FC FF FF`

The presentation probe hooks only that call site and then invokes untouched retail `DXPOLY_Flip`.

Latest presentation log proves:

### Window
- valid HWND
- windowed flag active
- stored DirectDraw destination rect = `0,0,640,480`
- actual client rect in screen coordinates = `0,0,640,480`
- therefore stale `gRect` is **ruled out**

### Offscreen scene surface
- valid
- 640x480
- 32 bpp
- not lost
- GetDC succeeds
- 3x3 sample grid: all 9 samples non-black
- sample hash initially `0x7E0B5BDA`
- by frames 360/480 it changes to `0x3B302417`

The changing hash is direct evidence that the game is rendering changing visible content into the scene surface.

### Primary DirectDraw surface
- valid
- desktop-sized 1920x1080
- 32 bpp
- not lost
- GetDC succeeds
- sampled values are non-black
- sample hash stays frozen at `0x3565BD06` from frame 1 through frame 480

### Interpretation

The game renderer itself is **not black**.

The game logic/input/audio all function.

The offscreen scene changes while the DirectDraw primary-surface sample remains frozen and the user-visible HWND stays black.

The active bug is therefore isolated to **legacy DirectDraw presentation / modern Windows composition after the offscreen render target**.

Do not patch gameplay, textures, D3D scene generation, audio, or input to solve the black screen.

## EXACT NEXT ACTION

Implement a narrow **direct-to-HWND compatibility presentation probe**.

Preferred first experiment:

1. Leave all retail rendering into the offscreen scene surface unchanged.
2. Leave the existing windowed DirectDraw setup in place.
3. In the current presentation wrapper, after retail `DXPOLY_Flip`, directly present the already-rendered scene to the actual HWND.
4. First try a minimal GDI path:
   - `sceneSurface->GetDC(&sceneDC)`
   - `GetDC(hwnd)`
   - `BitBlt(windowDC, 0, 0, clientWidth, clientHeight, sceneDC, 0, 0, SRCCOPY)`
   - release both DCs.
5. Guard/log HRESULT/API failures and do not alter scene rendering.
6. If cross-DC `BitBlt` is unsupported or still invisible, use the more deterministic fallback:
   - Lock/read the 32-bpp scene surface;
   - respect `lPitch`;
   - present to HWND with `StretchDIBits` and a top-down 32-bpp `BITMAPINFO`.
7. Treat this initially as a diagnostic compatibility presenter, not a renderer rewrite.

Expected result:
- if the image appears, the diagnosis is confirmed: modern Windows/DWM is not exposing the legacy DirectDraw primary-surface presentation correctly;
- then keep/refine the direct HWND presenter as the modern compatibility path.

**Do not spend another runtime cycle on SetDisplayMode, bpp, scene-black hypotheses, stale gRect, or surface-loss checks before trying the direct HWND presenter.**

## LAST KNOWN VISIBLE / PLAYABLE HISTORICAL BASELINE

Revision:
`35e73ed3c4ca8c06b581df83f0f0913f0c915982`

In two user sessions at this revision:
- game had visible output;
- user reached first level;
- user controlled Spider-Man;
- user quit back to main menu;
- entering Options crashed at retail `0x0043EB29`.

The exact same source was later restored, but the user-visible output still became black. That is one reason the current issue is treated as presentation/environment compatibility rather than ordinary source regression.

## OPTIONS CRASH — PARKED UNTIL VIDEO IS VISIBLE

Two independent crashes:
- `0x0043EB29`
- maps to retail `Font::height(char*) + 0x39`

The reconstructed wrapper called the retail C++ instance method like a free function and did not correctly pass `this` in ECX.

A better fix was prepared using the same FASTCALL trampoline pattern as `Font::width`:
```cpp
typedef i32 (FASTCALL *func_ptr)(Font*, void*, char*);
func_ptr func = (func_ptr)0x0043EAF0;
return func(this, 0, txt);
```

Historical commit containing that isolated fix:
`b8bc0957721c29499737741f1fd9c31740e7cf49`

It is **not active now** because the project was restored to the pure playable runtime while isolating the black screen.

Once video is visible again, reintroduce/test this fix independently.

## AUDIO — CURRENTLY WORKING; DO NOT RE-DIAGNOSE NOW

The user initially reported no audio in experimental builds.

Later diagnostics proved:
- retail DirectSound device existed;
- primary DirectSound buffer existed;
- `SFX_Init` loaded 42 buffers;
- menu/level spool reached 44 buffers.

After restoring the pure baseline, the user reported **sound is audible again**.

Therefore audio is not the current blocker.

Historical audio diagnostic commit:
`7d42a06b1c70f8c706748926a8bfb3f5754d68ff`

Do not reactivate audio wrappers while fixing the black window.

## CONTROLLER / XBOX SUPPORT — REQUIRED FUTURE FEATURE, CURRENTLY PARKED

User explicitly requires:
- full modern controller support;
- Xbox/XInput behavior;
- analog support;
- configurable mappings;
- Xbox button names/prompts/UI;
- rumble.

A phase-1 XInput backend was implemented historically in:
`568c9c20148a382c77c34e6c246afa9e556222f0`

It was parked/removed from the active runtime while recovering the visible rendering baseline.

Verified retail input conventions:
- analog range `-1000..+1000`;
- POV in DirectInput hundredths-of-degrees;
- button state:
  - `0xFF` new press
  - `0x7F` held
  - `0x80` release
  - `0x00` idle

Planned stable Xbox indices:
- 0 X
- 1 A
- 2 View
- 3 B
- 4 Y
- 5 LT
- 6 LB
- 7 RT
- 8 LS
- 9 RB
- 10 RS
- 11 Menu
- 12..15 D-pad

Do not re-enable this until visible rendering is restored.

## TEXTURE / SPOOL HISTORY — DO NOT REDO DURING BLACK-SCREEN WORK

A previous startup stack overflow at retail:
`Spool_FindTextureEntry(u32) = 0x004C9460`

was tied to a temporary retail call-through and was investigated/fixed during earlier work.

The retail texture lookup machine code later revealed an exact indexed table base `0x006AB934`, but experiments consuming the live retail table introduced additional instability and were rolled back.

Current black-screen evidence proves the offscreen scene already contains non-black rendered output, so texture lookup is not the current frontier.

Do not reopen the texture-table work until visible presentation is fixed.

## OTHER IMPORTANT DEAD ENDS / LESSONS

- 16 -> 32 bpp direct SetDisplayMode test: still `0x80004001`. Color depth is not the original fullscreen blocker.
- Earlier cross-module SetDisplayMode helper/thunk experiments reported `compat_state not_seen`; superseded.
- Stale DirectDraw destination rectangle: current probe shows stored and live rect match exactly. Ruled out.
- Scene surface black: ruled out by non-black samples and changing hash.
- Scene/primary surface lost: ruled out by `IsLost() == 0`.
- Current retail EXE drift: ruled out; fingerprint repeatedly stable.
- Source regression as sole cause: pure historical runtime source was restored and black output persisted.

## BUILD / MATCHING NOTES

Matching build:
- `build.bat`
- `spider.mak`
- output `Release\spider.dll`

The test path force-cleans because old NMAKE incremental behavior previously produced a stale DLL.

Release builds can emit a linker map; recent test tooling has collected it as `proxy-link-map.txt`.

## CURRENT LOGS THAT MATTER MOST

The handoff ZIP should contain the latest:
- `spidey-decomp-present.log`
- `spidey-decomp-compat.log`
- `proxy-link-map.txt`
- `test-session*.txt`
- `game-exe-fingerprint*.txt`

It should also contain the two historical playable-baseline Options crash logs/session files at revision `35e73ed...`.

## USER GOALS AFTER CURRENT VIDEO FIX

Near-term:
1. restore visible rendering;
2. verify Options fix;
3. retain working audio;
4. implement/test full XInput/Xbox controller support;
5. continue bug fixes and modernization.

Long-term:
- plugin/runtime SDK;
- mod support;
- GUI/editor (“Spidey Studio”);
- Spider-Man 2000 Dev Build 0.1 milestone.

## RECOVERY RULE IF CHAT/INPUT STREAM FAILS

Immediately:
1. read `docs/CURRENT_STATUS.md`;
2. read this handoff file;
3. inspect current `dev` head;
4. compare how far the last code change actually landed before the interruption;
5. continue from the documented next frontier;
6. live-update `CURRENT_STATUS.md` before substantial further work.

Do not ask the user to reconstruct lost context unless repo/log evidence genuinely cannot recover it.
