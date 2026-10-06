# Claude decomp collaboration workflow

Claude must work only on the `claude-decomp` branch. ChatGPT/local development owns `dev`.

## Before every Claude work batch

Run:

```text
git fetch origin
git checkout claude-decomp
git merge --ff-only origin/dev
```

If the fast-forward fails, stop. Do not rebase, force-push, resolve history, or push to `dev`/ `master`.

## Claude's scope

Claude is acting as a parallel reverse-engineering/decompilation worker. Prefer work that does not overlap the function or subsystem ChatGPT is actively editing.

Good tasks:
- decompile untouched retail functions from assembly to faithful C/C++;
- recover structs/fields/call relationships;
- add narrowly scoped RE notes;
- make small coherent commits grouped by function or tightly related function family.

Each commit should identify the important function names/addresses and what was recovered.

Avoid unless explicitly assigned:
- gameplay behavior fixes;
- timing policy changes;
- renderer/input workflow changes;
- `TEST_LATEST_BUILD.bat`, `FAST_UPDATE_AND_TEST_LATEST_BUILD.bat`, `UPDATE_SPIDEY_PROJECT.bat`;
- `tools/SYNC_CLAUDE_DECOMP.ps1` or `SYNC_CLAUDE_DECOMP.bat`;
- rewriting `docs/CURRENT_STATUS.md` / `docs/NEW_CHAT_HANDOFF.md`;
- force pushes, history rewrites, branch deletion, or direct pushes to `dev` or `master`.

## When Claude finishes a batch

Claude commits and pushes only:

```text
git push origin claude-decomp
```

Then the user runs `SYNC_CLAUDE_DECOMP.bat` in the local authoritative checkout.

The sync script:
1. refuses a dirty local tree;
2. fetches/pins Claude's current remote tip;
3. refuses unexpected `dev` divergence;
4. publishes any local `dev` commits first;
5. creates a local rollback branch;
6. merges the pinned Claude snapshot without force or auto-conflict resolution;
7. aborts safely on conflicts;
8. pushes successful integrated `dev` back to GitHub.

After a successful sync, Claude should fast-forward `claude-decomp` to the new `origin/dev` before starting another batch.
