from pathlib import Path
for name in ["20261006-033344","20261006-033102"]:
    p=Path(r"F:\Spider-Man 2000 Recomp\project main\logs")/name/"spidey-decomp.log"
    t=p.read_text(errors="ignore").replace("\\n","\n")
    print("\n===",name,"===")
    for key in ["gameplay_ui_fill_scale","gameplay_ui_scale source=anim_frame","gameplay_ui_scale source=texture"]:
        xs=[x for x in t.splitlines() if key in x]
        print("\n--",key,"count",len(xs),"--")
        for x in xs[:120]:
            print(x)
