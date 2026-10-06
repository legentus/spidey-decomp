import re, json, math
paths={
 'fail60':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-012109\spidey-decomp.log',
 'good20':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-020549\spidey-decomp.log',
}
rx=re.compile(
 r'chase_synth_trace i=(?P<i>\d+) tick=(?P<tick>\d+) elapsed=(?P<elapsed>\d+) field80=(?P<field80>\d+) '
 r'synth=(?P<synth>\d+) script_active=(?P<script_active>\d+) script_clock=(?P<script_clock>-?\d+) '
 r'axes=(?P<ax>-?\d+),(?P<ay>-?\d+) ramp=(?P<ramp>-?\d+) state=0x(?P<state>[0-9A-Fa-f]+) '
 r'pos=(?P<x>-?\d+),(?P<y>-?\d+),(?P<z>-?\d+) angle_y=(?P<angle>-?\d+) heading_valid=(?P<hv>\d+) '
 r'camera_heading=(?P<cam>-?\d+) input_basis_e34=(?P<e34>-?\d+) desired_relative_e32=(?P<e32>-?\d+) desired_world=(?P<world>-?\d+) '
 r'wall=(?P<wall>\d+) ceiling=(?P<ceil>\d+) collision=0x(?P<coll>[0-9A-Fa-f]+) ground_grace=(?P<grace>-?\d+) '
 r'worker_mask_before=0x(?P<mb>[0-9A-Fa-f]+) worker_mask_after=0x(?P<ma>[0-9A-Fa-f]+) '
 r'head_before=(?P<hbt>-?\d+),(?P<hbs>-?\d+),(?P<hb2>-?\d+),(?P<hb3>-?\d+) '
 r'head_after=(?P<hat>-?\d+),(?P<has>-?\d+),(?P<ha2>-?\d+),(?P<ha3>-?\d+)')
def parse(p):
 t=open(p,errors='ignore').read()
 out=[]
 for m in rx.finditer(t):
  g=m.groupdict()
  r={k:(int(v,16) if k in ('state','coll','mb','ma') else int(v)) for k,v in g.items()}
  out.append(r)
 return out
runs={k:parse(v) for k,v in paths.items()}
print({k:len(v) for k,v in runs.items()})
a,b=runs['fail60'],runs['good20']
fields=['elapsed','field80','synth','script_active','script_clock','ax','ay','ramp','state','x','y','z','angle','hv','cam','e34','e32','world','wall','ceil','coll','grace','mb','ma','hbt','hbs','hb2','hb3','hat','has','ha2','ha3']
# first differences by field
for f in fields:
 idx=None
 for i,(ra,rb) in enumerate(zip(a,b)):
  if ra[f]!=rb[f]:
   idx=i;break
 print('FIRST',f,idx, (a[idx][f],b[idx][f]) if idx is not None else None)
# earliest overall divergence ignoring tick
first=None
for i,(ra,rb) in enumerate(zip(a,b)):
 ds=[f for f in fields if ra[f]!=rb[f]]
 if ds:
  first=(i,ds);break
print('EARLIEST',first)
if first:
 i=first[0]
 for j in range(max(0,i-5),min(len(a),i+16)):
  ra,rb=a[j],b[j]
  ds=[f for f in fields if ra[f]!=rb[f]]
  print('i',j,'diff',ds)
  print(' 60 pos',ra['x'],ra['y'],ra['z'],'ang',ra['angle'],'cam',ra['cam'],'world',ra['world'],'state',hex(ra['state']),'coll',hex(ra['coll']),'head',ra['hbt'],ra['hb2'],ra['hb3'],'script',ra['script_active'],ra['script_clock'])
  print(' 20 pos',rb['x'],rb['y'],rb['z'],'ang',rb['angle'],'cam',rb['cam'],'world',rb['world'],'state',hex(rb['state']),'coll',hex(rb['coll']),'head',rb['hbt'],rb['hb2'],rb['hb3'],'script',rb['script_active'],rb['script_clock'])
# transition index signatures
for name,rows in runs.items():
 print('\nTRANSITIONS',name)
 prev=None
 for r in rows:
  sig=(r['hbt'],r['hbs'],r['hb2'],r['mb'],r['synth'])
  if sig!=prev:
   print(r['i'], 'tick',r['tick'],'head',r['hbt'],r['hbs'],r['hb2'],r['hb3'],'mask',hex(r['mb']),'synth',r['synth'],'pos',r['x'],r['y'],r['z'])
   prev=sig
