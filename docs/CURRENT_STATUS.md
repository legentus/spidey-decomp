# CURRENT STATUS

**Project:** Spider-Man 2000 PC decomp / developer build  
**Repository:** `legentus/spidey-decomp`  
**Working branch:** `dev`  
**Upstream-sync branch:** `master`  
**Upstream baseline:** `krystalgamer/spidey-decomp`  
**Baseline commit:** `4eb5635bf1e0aae9edff771d3b9b0db16971ff28` — `patch: CFT4Bit::SetTransparency`  
**Last live update:** 2026-09-29

## Immediate Goal

Establish **Spider-Man 2000 Developer Build 0.1**:

1. Reproduce the existing matching/decomp build.
2. Understand exactly how the current `spider.dll` / `binkw32.dll` bootstrap works.
3. Add a harmless developer-build proof (logging or similarly obvious behavior).
4. Launch against the user's exact PC game install and prove our rebuilt source executes.
5. Only after that, select and fix the first real bug.

## Repository Policy

- Keep `master` clean for upstream synchronization.
- Put our active development work on `dev` and feature branches.
- Do not commit original retail game binaries or PKR assets.
- Preserve original matching/reconstruction work where possible.
- Separate intentional modernization / developer functionality from matching work.
- No fabricated addresses, layouts, offsets, or runtime state; verify before patching.

## Live-Documentation Rule

This file is updated **during work**, not only at the end of a chat. After any meaningful discovery, failed path, code change, build frontier, or test result, update this file so work can resume after an interruption.

## Verified State

- User fork exists and is writable.
- Default branch is `master`.
- Fork head observed at `4eb5635bf1e0aae9edff771d3b9b0db16971ff28`.
- Existing project contains the reconstructed PC game code plus old MSVC/NMAKE build files, CMake support, CI, and validation tooling.
- Current upstream CI is known to build `Release\\spider.dll`, copy/rename it to `binkw32.dll`, and validate it. We still need to inspect the exact bootstrap/export mechanism before changing code.

## Current Investigation Frontier

**ACTIVE:** inspect the build/bootstrap path and identify the smallest safe source-level developer proof.

Next files to inspect:
- `build.bat`
- `spider.mak`
- `spider.def` or equivalent export definition, if present
- `CMakeLists.txt`
- `.github/workflows/c-cpp.yml`
- `bink*.cpp/.h` and DLL entry/bootstrap code
- validation/bootstrap-related source

## Next Action

Trace exactly how the reconstructed DLL is loaded by the retail executable and determine the safest minimal developer-build modification that proves rebuilt C++ is executing without disturbing matching work.
