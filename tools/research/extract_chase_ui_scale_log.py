from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-031423\spidey-decomp.log")
t=p.read_text(errors="ignore").replace("\\n","\n")
xs=[x for x in t.splitlines() if "gameplay_ui_scale source=texture" in x]
print("count",len(xs))
for x in xs[:100]:
    print(x)
