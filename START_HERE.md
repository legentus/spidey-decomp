# START HERE - Downloadable Spider-Man 2000 Handoff

This branch is a downloadable handoff snapshot for the Spider-Man 2000 decomp project.

## Exact development baseline

Source baseline:
382362f3e14195a6bab552bf1c445cb33262d2ac

Authoritative real working folder:
F:\Spider-Man 2000 Recomp\project main

Development branch:
dev

The handoff branch exists only to package/share the work. Do not treat it as the development branch.

## Read these first

1. HANDOFF_DOCS/NEW_CHAT_HANDOFF.md
2. HANDOFF_DOCS/WORKFLOW_AND_PATHS.md
3. HANDOFF_DOCS/NEXT_STEPS.md
4. docs/CURRENT_STATUS.md for the exhaustive historical journal

## Source layout

Because this is a GitHub branch archive, the complete source tree is directly at the ZIP root rather than under a SOURCE/ subfolder.

## Exact current DLL

The verified current build was too large for the reliable text-file transfer path, so it is stored losslessly as base64 chunks in:

.handoff_chunks/

Run after extracting:

    python RECONSTRUCT_HANDOFF_BUILD.py

This creates:

    BUILD/spider.dll

Expected SHA-256:

    7cfb0f4354e3f15f9db34386efc8cb8248394bbd80097944bd6fe5297bb510d8

## Current immediate task

Validate the COM-fixed XAudio2 build in-game, confirm spidey-audio.log says backend=xaudio2, then continue investigating/smoothing the confirmed L1A1 helicopter alias-1 loop.

See HANDOFF_DOCS/NEW_CHAT_HANDOFF.md for the full technical state.
