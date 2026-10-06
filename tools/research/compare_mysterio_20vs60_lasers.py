from pathlib import Path
import re

paths={
    "good20": Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-040238\spidey-decomp.log"),
    "old60": Path(r"F:\Spider-Man 2000 Recomp\project main\logs\20261006-033344\spidey-decomp.log"),
}

pat=re.compile(
    r"mysterio_laser_attack call=(?P<call>\d+) attack=(?P<attack>\d+) stage2_calls=(?P<stage2>\d+) "
    r"tick=(?P<tick>\d+) delta_from_last=(?P<delta>\d+) state=(?P<sb>-?\d+)->(?P<sa>-?\d+) "
    r"substate=(?P<subb>-?\d+)->(?P<suba>-?\d+) field80=(?P<f80>-?\d+) cooldown39c=(?P<cd>-?\d+) "
    r"arms=(?P<a0>\d+),(?P<a1>\d+) boss_active=(?P<boss>\d+)"
)

for name,p in paths.items():
    t=p.read_text(errors="ignore").replace("\\n","\n")
    print("\n===",name,p,"===")
    for key in [
        "timer_pacing event=intercept",
        "timer_pacing_install",
        "high_fps_compat mysterio_laser=",
        "gameplay_ui_scale_install",
        "modern_camera event=release reason=mysterio",
    ]:
        xs=[x for x in t.splitlines() if key in x]
        print("\n",key, "count",len(xs))
        for x in xs[-8:]:
            print(x)
    rows=[{k:int(v) for k,v in m.groupdict().items()} for m in pat.finditer(t)]
    print("\nlaser_rows",len(rows))
    if not rows:
        continue
    starts=[]
    last_attack=None
    for r in rows:
        if r["attack"] != last_attack:
            starts.append(r)
            last_attack=r["attack"]
    print("distinct_attacks",len(starts))
    print("attack_starts:")
    for r in starts:
        print(r)
    if len(starts)>1:
        gaps=[starts[i]["tick"]-starts[i-1]["tick"] for i in range(1,len(starts))]
        print("attack_start_gaps",gaps)
        print("gap_avg",sum(gaps)/len(gaps),"min",min(gaps),"max",max(gaps))
    print("substate_counts", {s:sum(1 for r in rows if r["subb"]==s) for s in sorted(set(r["subb"] for r in rows))})
