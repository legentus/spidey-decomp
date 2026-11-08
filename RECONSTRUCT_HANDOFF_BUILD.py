#!/usr/bin/env python3
from pathlib import Path
import base64, hashlib, sys

root = Path(__file__).resolve().parent
parts = sorted((root / ".handoff_chunks").glob("spider.dll.part*.b64"))
if not parts:
    raise SystemExit("No DLL chunks found in .handoff_chunks")

out_dir = root / "BUILD"
out_dir.mkdir(exist_ok=True)
out = out_dir / "spider.dll"

with out.open("wb") as f:
    for p in parts:
        f.write(base64.b64decode(p.read_text(encoding="ascii").strip()))

expected = "7cfb0f4354e3f15f9db34386efc8cb8248394bbd80097944bd6fe5297bb510d8"
actual = hashlib.sha256(out.read_bytes()).hexdigest()
print("wrote:", out)
print("sha256:", actual)
if actual != expected:
    raise SystemExit("ERROR: SHA-256 mismatch; expected " + expected)
print("PASS: exact current spider.dll reconstructed.")
