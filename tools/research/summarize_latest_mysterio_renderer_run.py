from pathlib import Path
import re
p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-045343\spidey-decomp.log")
t=p.read_text(errors="ignore").replace("\\n","\n")
print("=== HEAD ===")
print("\n".join(t.splitlines()[:25]))
keys=[
"mysterio_laser_setpos_20hz_stats",
"mysterio_laser_attack",
"mysterio_softspot_hit",
"modern_camera event=release reason=mysterio",
"gameplay_ui_scale_install",
"mysterio_boss",
"shadow",
"blob",
"tentacle",
"effect",
"gfx",
"qpoly",
"renderer",
"camera"
]
for k in keys:
    xs=[x for x in t.splitlines() if k.lower() in x.lower()]
    print("\n--",k,"count",len(xs),"--")
    for x in xs[-120:]:
        print(x)
print("\n=== TAIL ===")
for x in t.splitlines()[-180:]:
    print(x)
