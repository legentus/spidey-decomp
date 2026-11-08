# NEXT STEPS

## 1. Validate real XAudio2 in-game

Install current build with TEST_LATEST_BUILD.bat.

Check:
C:\Program Files (x86)\Activision\Spider-Man\spidey-audio.log

Expected:
backend=xaudio2 module=xaudio2_9.dll ...

If it fails, use the exact HRESULT logged by the new backend.

## 2. Re-test L1A1 helicopter loop under XAudio2

Look for one-time:
l1a1_alias1_xaudio_source ...

If the repeat remains:
- record 10-15 seconds with SpideyLoopbackCapture;
- run tools/analyze_loopback.py with --expected 2.52;
- compare to RUNTIME_EVIDENCE/live_loopback.wav;
- use actual copied PCM format and byte count to design the fix.

Do not globally disable loops and do not permanently mute helicopter audio.

## 3. Later remove the remaining DirectSound dependency

Phase 1 copies PCM from retail DirectSound source buffers.

Once XAudio2 voice behavior is stable, directly decode/load the KAT bank formats so DirectSound can be removed as an asset-loader dependency too.

## Regression checklist

- menu SFX;
- cutscene/movie audio;
- helicopter;
- missiles;
- positional panning;
- pitch;
- loops;
- pause/unpause;
- overlapping SFX;
- voice reclamation;
- alt-tab behavior;
- no combat hitching;
- camera/manual aim unchanged.
