from pathlib import Path
import re

paths=[
Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-033344\spidey-decomp.log"),
Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-033102\spidey-decomp.log"),
]
pats=re.compile(r"mysterio|level=0x|level_id|gameplay_ui_scale source=|health|laser|beam|boss",re.I)

for p in paths:
    print("\n===",p,"===")
    t=p.read_text(errors="ignore").replace("\\n","\n")
    hits=[x for x in t.splitlines() if pats.search(x)]
    print("hits",len(hits))
    for x in hits[:500]:
        print(x)
