from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\mysterio.cpp")
for i,line in enumerate(p.read_text(errors="ignore").splitlines(),1):
    if any(k.lower() in line.lower() for k in ["CSoftSpot","softspot","Hit(","Web","Damage","field_358"]):
        print(f"{i}: {line}")
