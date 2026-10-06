from pathlib import Path
import re

p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-031423\spidey-decomp.log")
t=p.read_text(errors="ignore")
flat=t.replace("\\n","\n")

print("---HEAD---")
print("\n".join(flat.splitlines()[:18]))

print("---STATS---")
for pat in [
    "timer_pacing event=intercept",
    "chase_world_ai_20hz_stats",
    "chase_player_ai_20hz_stats",
    "chase_camera_ai_20hz_stats",
    "chase_synth_20hz_stats",
    "chase_scheduler_stats",
]:
    xs=[x for x in flat.splitlines() if pat in x]
    print(pat, xs[-5:])

rx=re.compile(
    r"chase_synth_trace i=(\d+).*?tick=(\d+).*?elapsed=(\d+).*?field80=(\d+).*?"
    r"synth=(\d+).*?script_active=(\d+).*?script_clock=(-?\d+).*?"
    r"axes=(-?\d+),(-?\d+).*?state=0x([0-9A-Fa-f]+).*?"
    r"pos=(-?\d+),(-?\d+),(-?\d+).*?angle_y=(-?\d+).*?"
    r"camera_heading=(-?\d+).*?camera_mode=(-?\d+).*?camera_interp=(-?\d+).*?"
    r"desired_world=(-?\d+).*?collision=0x([0-9A-Fa-f]+).*?"
    r"head_before=(-?\d+),(-?\d+),(-?\d+),(-?\d+)"
)
rows=[]
for m in rx.finditer(t):
    g=m.groups()
    rows.append(dict(
        i=int(g[0]),tick=int(g[1]),e=int(g[2]),f80=int(g[3]),s=int(g[4]),sa=int(g[5]),
        clock=int(g[6]),ax=int(g[7]),ay=int(g[8]),state=int(g[9],16),
        x=int(g[10]),y=int(g[11]),z=int(g[12]),ang=int(g[13]),cam=int(g[14]),
        mode=int(g[15]),interp=int(g[16]),world=int(g[17]),coll=int(g[18],16),
        ht=int(g[19]),hs=int(g[20]),hc=int(g[21]),timer=int(g[22])
    ))

print("trace_rows",len(rows))
c9=[r for r in rows if r["ht"]==3 and r["hc"]==9]
print("code9_count",len(c9))
if c9:
    print("code9_first",c9[0])
    print("code9_last",c9[-1])
    print("cam_unique",sorted(set(r["cam"] for r in c9)))
    print("world_unique",sorted(set(r["world"] for r in c9)))
    print("x_range",min(r["x"] for r in c9),max(r["x"] for r in c9))
    print("z_range",min(r["z"] for r in c9),max(r["z"] for r in c9))
    print("coll_unique",sorted(set(r["coll"] for r in c9)))

print("---TAIL---")
for r in rows[-25:]:
    print(r)
