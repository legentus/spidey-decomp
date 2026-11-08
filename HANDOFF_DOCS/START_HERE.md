# START HERE - Spider-Man 2000 Decomp Handoff

Handoff date: 2026-11-07
Authoritative working folder: F:\Spider-Man 2000 Recomp\project main
Branch: dev
Exact source commit: 382362f3e14195a6bab552bf1c445cb33262d2ac
Remote: origin -> https://github.com/legentus/spidey-decomp.git
Repo state when packaged: clean and synced with origin/dev
Built DLL SHA-256: 7cfb0f4354e3f15f9db34386efc8cb8248394bbd80097944bd6fe5297bb510d8
Built DLL size: 954368 bytes

## Contents

- SOURCE/ : full tracked source tree at the exact commit above.
- BUILD/ : current matching build outputs, including spider.dll and linker artifacts when present.
- HANDOFF_DOCS/ : onboarding, workflow, current state, and next steps.
- RUNTIME_EVIDENCE/ : useful audio capture/log evidence.
- SOURCE/docs/CURRENT_STATUS.md : exhaustive development journal and prior research.

## First actions for the next chat

1. Read HANDOFF_DOCS/NEW_CHAT_HANDOFF.md.
2. Read HANDOFF_DOCS/WORKFLOW_AND_PATHS.md.
3. Search SOURCE/docs/CURRENT_STATUS.md when historical detail or old offsets are needed.
4. Work in the real checkout at F:\Spider-Man 2000 Recomp\project main, not inside this extracted handoff.
5. Verify git status before editing.
6. Build before every runtime test.

## Immediate objective

We are currently modernizing and debugging the audio system.

The latest source fixes the XAudio2 2.9 CreateMasteringVoice failure by initializing COM first. The previous actual game run was still falling back to DirectSound, so the current 382362f3 build has not yet had its in-game XAudio2 validation.

Next runtime:
- install with TEST_LATEST_BUILD.bat;
- launch the game;
- confirm spidey-audio.log reports backend=xaudio2;
- enter L1A1;
- inspect the one-time l1a1_alias1_xaudio_source line;
- if the helicopter loop remains, capture/analyze actual speaker output with the included WASAPI tools.

Do not re-enable heavy combat diagnostics unless a freeze genuinely returns.
