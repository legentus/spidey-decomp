from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-170406\spidey-decomp.log")
t=p.read_text(errors="ignore").replace("\\n","\n")
print("=== HEAD ===")
print("\n".join(t.splitlines()[:24]))
for k in [
"quadbit_camera_anchor",
"quadbit_horplus",
"mysterio_health_alignment",
"gameplay_ui_scale_install",
"mysterio_laser_setpos_20hz_stats",
"mysterio_softspot_hit",
"modern_camera event=release reason=mysterio",
"session_end",
"process exit"
]:
    xs=[x for x in t.splitlines() if k.lower() in x.lower()]
    print("\n==",k,"count",len(xs),"==")
    for x in xs[-120:]:
        print(x)
