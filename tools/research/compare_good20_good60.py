import re
paths={
'good20':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-020549\spidey-decomp.log',
'good60':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-030007\spidey-decomp.log'}
rx=re.compile(
 r'chase_synth_trace i=(?P<i>\d+) tick=(?P<tick>\d+) elapsed=(?P<elapsed>\d+) field80=(?P<field80>\d+) '
 r'synth=(?P<synth>\d+) script_active=(?P<script_active>\d+) script_clock=(?P<script_clock>-?\d+) '
 r'axes=(?P<ax>-?\d+),(?P<ay>-?\d+) ramp=(?P<ramp>-?\d+) state=0x(?P<state>[0-9A-Fa-f]+) '
 r'pos=(?P<x>-?\d+),(?P<y>-?\d+),(?P<z>-?\d+) angle_y=(?P<angle>-?\d+) heading_valid=(?P<hv>\d+) '
 r'camera_heading=(?P<cam>-?\d+).*?desired_world=(?P<world>-?\d+).*?collision=0x(?P<coll>[0-9A-Fa-f]+).*?'
 r'head_before=(?P<hbt>-?\d+),(?P<hbs>-?\d+),(?P<hb2>-?\d+),(?P<hb3>-?\d+)')
def parse(p):
    t=open(p,errors='ignore').read()
    out=[]
    for m in rx.finditer(t):
        g=m.groupdict()
        r={k:(int(v,16) if k in ('state','coll') else int(v)) for k,v in g.items()}
        out.append(r)
    return out
runs={k:parse(v) for k,v in paths.items()}
print({k:len(v) for k,v in runs.items()})
for name,rows in runs.items():
    c9=[r for r in rows if r['hbt']==3 and r['hb2']==9]
    print(name,'code9',len(c9))
    if c9:
        print(' first',c9[0])
        print(' last ',c9[-1])
        print(' cam_unique',sorted(set(r['cam'] for r in c9))[:20], 'count',len(set(r['cam'] for r in c9)))
        print(' world_unique',sorted(set(r['world'] for r in c9))[:20], 'count',len(set(r['world'] for r in c9)))
        print(' x_range',min(r['x'] for r in c9),max(r['x'] for r in c9))
        print(' z_range',min(r['z'] for r in c9),max(r['z'] for r in c9))
# align by final code9 timer value
a={r['hb3']:r for r in runs['good20'] if r['hbt']==3 and r['hb2']==9}
b={r['hb3']:r for r in runs['good60'] if r['hbt']==3 and r['hb2']==9}
common=sorted(set(a)&set(b),reverse=True)
print('common timers',len(common),common[:5],common[-5:])
maxdx=maxdz=maxda=maxdc=0
for tm in common:
    ra,rb=a[tm],b[tm]
    maxdx=max(maxdx,abs(ra['x']-rb['x']))
    maxdz=max(maxdz,abs(ra['z']-rb['z']))
    maxda=max(maxda,abs(ra['angle']-rb['angle']))
    maxdc=max(maxdc,abs(ra['cam']-rb['cam']))
print('max diffs aligned timer: x',maxdx,'z',maxdz,'angle',maxda,'cam',maxdc)
