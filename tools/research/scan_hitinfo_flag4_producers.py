from pathlib import Path
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
for fn in ["web.cpp","spidey.cpp","weapons.cpp","bullet.cpp","bit.cpp","baddy.cpp"]:
    p=root/fn
    if not p.exists(): continue
    lines=p.read_text(errors="ignore").splitlines()
    print("\n==",fn,"==")
    for i,line in enumerate(lines,1):
        low=line.lower()
        if "shitinfo" in low or "field_0" in low or "webball" in low or "web ball" in low:
            print(f"{i}: {line}")
