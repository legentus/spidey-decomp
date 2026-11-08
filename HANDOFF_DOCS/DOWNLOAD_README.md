# Downloadable Handoff Branch

This branch exists only to make the 2026-11-07 Spider-Man 2000 handoff downloadable as a normal GitHub ZIP.

Source baseline: 382362f3e14195a6bab552bf1c445cb33262d2ac

Start with:
- HANDOFF_DOCS/START_HERE.md
- HANDOFF_DOCS/NEW_CHAT_HANDOFF.md
- HANDOFF_DOCS/WORKFLOW_AND_PATHS.md
- HANDOFF_DOCS/NEXT_STEPS.md

The exact current spider.dll is stored as base64 chunks in .handoff_chunks because GitHub's text contents API was the reliable transfer path available to the handoff process.

After extracting this branch ZIP, run:

    python RECONSTRUCT_HANDOFF_BUILD.py

That creates:

    BUILD/spider.dll

Expected SHA-256:

    7cfb0f4354e3f15f9db34386efc8cb8248394bbd80097944bd6fe5297bb510d8

The user's authoritative real working checkout remains:

    F:\Spider-Man 2000 Recomp\project main

Do not develop inside this handoff copy unless intentionally creating a separate clone.
