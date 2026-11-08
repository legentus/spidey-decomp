# WORKFLOW AND PATHS

Authoritative repo:
F:\Spider-Man 2000 Recomp\project main

Handoff folder:
F:\Spider-Man 2000 Recomp\Handoff

Game root:
C:\Program Files (x86)\Activision\Spider-Man

Game EXE:
C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe

Live proxy:
C:\Program Files (x86)\Activision\Spider-Man\binkw32.dll

Preserved retail Bink:
C:\Program Files (x86)\Activision\Spider-Man\binkw32_.dll

Main runtime log:
C:\Program Files (x86)\Activision\Spider-Man\spidey-decomp.log

Audio log:
C:\Program Files (x86)\Activision\Spider-Man\spidey-audio.log

Build output:
F:\Spider-Man 2000 Recomp\project main\Release\spider.dll

## Normal loop

1. Inspect source and retail evidence.
2. Preserve known-good state before risky edits.
3. Make smallest justified change.
4. git diff --check.
5. Forced-clean matching VC6 build.
6. Verify map/symbol/bytes when ABI matters.
7. Commit and push.
8. Stamp exact revision for runtime tests when needed.
9. Run TEST_LATEST_BUILD.bat.
10. User tests.
11. Pull logs/live state/WASAPI capture as needed.
12. Update docs/CURRENT_STATUS.md.

## Diagnostics

Heavy combat diagnostics are OFF.

Only re-enable if a real freeze returns:
- drop-hang tracing;
- per-baddy AI tracing;
- state-26 nested tracing;
- in-process hang sampler.

For audio prefer:
- one-time backend/format logs;
- WASAPI loopback capture;
- offline waveform analysis.
