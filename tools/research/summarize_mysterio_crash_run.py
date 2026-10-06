from pathlib import Path
import re
p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-041611\spidey-decomp.log")
t=p.read_text(errors="ignore").replace("\\n","\n")
print("=== HEAD ===")
print("\n".join(t.splitlines()[:30]))
print("\n=== KEY EVENTS ===")
keys=[
"timer_pacing event=intercept",
"mysterio_ai_20hz_install",
"mysterio_ai_20hz_stats",
"mysterio_laser_attack",
"high_fps_compat mysterio_laser=",
"modern_camera event=release reason=mysterio",
"crash",
"exception",
"fatal",
"access violation",
"gameplay_ui_scale_install"
]
for k in keys:
 xs=[x for x in t.splitlines() if k.lower() in x.lower()]
 print("\n--",k,"count",len(xs),"--")
 for x in xs[-80:]:
  print(x)
print("\n=== TAIL ===")
for x in t.splitlines()[-220:]:
 print(x)
