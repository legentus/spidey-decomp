# NEW CHAT HANDOFF - Spider-Man 2000 Decomp

## Project identity

Authoritative checkout:
F:\Spider-Man 2000 Recomp\project main

Game root:
C:\Program Files (x86)\Activision\Spider-Man

Game executable:
C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe

Branch:
dev

Remote:
origin -> https://github.com/legentus/spidey-decomp.git

Packaged revision:
382362f3e14195a6bab552bf1c445cb33262d2ac

Current build:
F:\Spider-Man 2000 Recomp\project main\Release\spider.dll

DLL SHA-256:
7cfb0f4354e3f15f9db34386efc8cb8248394bbd80097944bd6fe5297bb510d8

## Required workflow

- Use Local Commander for the user's Windows machine.
- Treat the F: checkout as authoritative.
- Keep docs/CURRENT_STATUS.md updated after meaningful milestones.
- Run git diff --check and a clean matching build before runtime tests.
- Commit and push meaningful changes to origin/dev.
- Prefer narrow, evidence-based patches over broad speculative fixes.
- Preserve known-good camera/effects/gameplay work.
- Heavy diagnostics are currently intentionally disabled because they caused hitching.
- The user prefers progress over repeated diagnostic-only iterations.

## Build and install

Matching compiler:
%LOCALAPPDATA%\Spidey2000Dev\MatchingVS

Forced clean:
SPIDEY_FORCE_CLEAN=1

Normal user-facing test workflow:
F:\Spider-Man 2000 Recomp\project main\TEST_LATEST_BUILD.bat

The install workflow preserves the original retail Bink DLL as binkw32_.dll and installs the decomp proxy as binkw32.dll.

## Major validated work - do not regress

### Modern camera/manual aim

Validated as good.
Key commit:
91d5e3d8468234ea8a053c09a224041378a6781d

Behavior:
- floor TPS manual aim;
- wall/ceiling mode 3 authoritative view plus custom first-person eye;
- no camera flip;
- mouse/right-stick look;
- WASD/left-stick crawl;
- aimed R1 zip;
- never reintroduce CAMERAMODE_FRONT / mode 7.

### Legacy effects / Mysterio

Validated:
- Mysterio helmet effect;
- shadow blobs;
- chest laser origin;
- Venom tentacles;
- Mysterio health bar;
- View restore fix for legacy effect projection;
- intro movie DirectSound lost-buffer recovery.

### Combat freeze root cause - fixed

The in-process sampler caught the exact infinite loop.

Retail 0x00401110 (CAIProc_RotY::Execute) had been patched with a virtual-member thunk:
mov eax,[ecx]
jmp [eax+4]

The vtable slot pointed back to retail 0x00401110, creating:
0x00401110 -> thunk -> vtable[+4] -> 0x00401110 -> infinite recursion.

Fix commit:
63fa967f0e744245e6f8913c48f608db119692d9

Fix:
a non-virtual fastcall bridge directly calls self->CAIProc_RotY::Execute().

Heavy freeze diagnostics were disabled afterward in:
a4611f67b6d9c0b25786ffb6cb0605ccbd9b8180

Keep them off unless a freeze returns.

## Current audio work

### XAudio2 Phase 1

Commit:
1ae991e0df5251dc80f6ceef4ccf0c6f31c22038

Files:
xaudio2_compat.h
xaudio2_backend.h
xaudio2_backend.cpp

Architecture:
- game-level SFX_* API remains unchanged;
- retail DirectSound still loads/decodes source buffers for Phase 1;
- active voices route through XAudio2 when initialization succeeds;
- failure leaves DirectSound untouched;
- SPIDEY_AUDIO_BACKEND=directsound forces legacy backend.

Live entry points replaced when XAudio2 is active:
DXSOUND_Open 0x00504110
DXSOUND_Close 0x005041C0
DXSOUND_Play 0x00504230
DXSOUND_Stop 0x005042A0
DXSOUND_SetVolume 0x005042F0
DXSOUND_SetPan 0x00504350
DXSOUND_SetPitch 0x005043B0
DXSOUND_IsPlaying 0x00504420

### XAudio2 startup failure and fix

The first runtime logged:
backend=directsound reason=mastering_voice_failed

Direct probe:
XAudio2Create -> success
CreateMasteringVoice -> 0x800401F0 = CO_E_NOTINITIALIZED

Calling CoInitializeEx(NULL, COINIT_MULTITHREADED) made CreateMasteringVoice succeed.

Fix commit:
e1e9e2d5015aa86c9e74fb73a5fe9db99b3d0f5c

Current backend:
- initializes COM;
- tolerates RPC_E_CHANGED_MODE;
- balances successful CoInitializeEx with CoUninitialize;
- logs exact HRESULTs;
- uses a local VC6-compatible declaration for CoInitializeEx.

IMPORTANT:
The COM-fixed XAudio2 build has not yet been tested in the actual game.

### L1A1 repeating sound

User hears a constant repeating sound through the opening cutscene and gameplay.

Decoded real L1A1 data:
- level alias 1;
- bank asset 33 / 0x21;
- loop flag is set;
- source metadata: 15120 bytes, 12000 Hz, format field 4;
- nominal whole-buffer repeat around 2.52 seconds.

Retail callsites sharing alias 0x8001:
- helicopter engine: CChopper::AI at 0x00426313;
- missile creation: CChopperMissile::CommonInitialisation at 0x0042435B.

A clean A/B build suppressed only missile audio:
255ece2de2d32a41d6b2f5804dfb8b7c5cd9b71c

The repeating sound remained, so the missile is ruled out.
Missile audio is restored in current source.

Remaining source:
the helicopter's own alias-1 loop/sample.

### Actual speaker-output analysis

tools/SpideyLoopbackCapture.cs
captures Windows render output using WASAPI loopback.

tools/analyze_loopback.py
uses stdlib only and performs RMS-envelope autocorrelation.

Included evidence:
RUNTIME_EVIDENCE/live_loopback.wav

Observed:
- stereo 48 kHz IEEE float output;
- repeat near 2.48 s;
- strongest recurrence around 4.95-4.96 s, approximately two cycles;
- this closely matches the identified L1A1 alias-1 sound.

### Next exact audio test

1. Run TEST_LATEST_BUILD.bat.
2. Launch game.
3. Inspect spidey-audio.log.
4. Expect backend=xaudio2 module=xaudio2_9.dll ...
5. Enter L1A1.
6. Look for:
   l1a1_alias1_xaudio_source asset=33 format_tag=... channels=... rate=... bits=... block_align=... bytes=...
7. If the repeat remains, make a new WASAPI capture and compare against the included DirectSound capture.
8. Then implement a narrow loop-seam treatment in XAudio2. Do not simply mute all helicopter audio.

## Important source files

main.cpp
DXsound.cpp / DXsound.h
ps2lowsfx.cpp / ps2lowsfx.h
chopper.cpp / chopper.h
xaudio2_backend.cpp / xaudio2_backend.h
xaudio2_compat.h
tools/SpideyLoopbackCapture.cs
tools/analyze_loopback.py
docs/CURRENT_STATUS.md

## Important caveats

- chopper.cpp contains decomp code marked @NotOk. Verify retail assembly before assuming that decompiled function is live.
- Phase 1 XAudio2 still depends on retail DirectSound source buffers for decoding/loading.
- Do not perform COM/XAudio teardown from DllMain under loader lock without careful review.
- Runtime logs included here are from the previous DirectSound runtime and are evidence only, not proof that the current XAudio2 COM-fixed build ran.

## Other parked work

- Scorpion termination remains unresolved.
- The request to expose all main-menu options in-level was deferred when audio became the priority.

For deep chronology and old offsets:
SOURCE/docs/CURRENT_STATUS.md
