import re
paths=[
r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-025806\spidey-decomp.log',
r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-030007\spidey-decomp.log']
rx=re.compile(r'chase_synth_trace i=(\d+).*?tick=(\d+).*?elapsed=(\d+).*?field80=(\d+).*?synth=(\d+).*?pos=(-?\d+),(-?\d+),(-?\d+).*?angle_y=(-?\d+).*?camera_heading=(-?\d+).*?camera_mode=(-?\d+).*?camera_interp=(-?\d+).*?desired_world=(-?\d+).*?collision=0x([0-9A-Fa-f]+).*?head_before=(-?\d+),(-?\d+),(-?\d+),(-?\d+)')
for p in paths:
    t=open(p,errors='ignore').read()
    print('\n===',p,'===')
    flat=t.replace('\\n','\n')
    for pat in ['chase_world_ai_20hz_install','chase_world_ai_20hz_stats','chase_player_ai_20hz_stats','chase_camera_ai_20hz_stats','chase_synth_20hz_stats']:
        ms=[line for line in flat.splitlines() if pat in line]
        print(pat, ms[-3:])
    rows=[]
    for m in rx.finditer(t):
        g=m.groups()
        rows.append(dict(i=int(g[0]),tick=int(g[1]),e=int(g[2]),f80=int(g[3]),s=int(g[4]),x=int(g[5]),y=int(g[6]),z=int(g[7]),ang=int(g[8]),cam=int(g[9]),mode=int(g[10]),interp=int(g[11]),world=int(g[12]),coll=int(g[13],16),ht=int(g[14]),hs=int(g[15]),hc=int(g[16]),timer=int(g[17])))
    print('trace_rows',len(rows))
    code9=[r for r in rows if r['ht']==3 and r['hc']==9]
    print('code9_count',len(code9))
    if code9:
        seen=[]
        for r in code9:
            key=(r['cam'],r['mode'],r['interp'],r['world'],r['coll'])
            if not seen or key!=seen[-1][0]:
                seen.append((key,r))
        for _,r in seen[:24]:
            print(' code9',r)
        if len(seen)>24:
            print(' ...')
            for _,r in seen[-12:]:
                print(' code9',r)
