import re
paths={
'fail60':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-012109\spidey-decomp.log',
'good20':r'F:\Spider-Man 2000 Recomp\project main\logs\20261006-020549\spidey-decomp.log'}
rx=re.compile(r'chase_synth_trace i=(\d+) tick=(\d+) elapsed=(\d+) field80=(\d+) synth=(\d+) script_active=(\d+) script_clock=(-?\d+) axes=(-?\d+),(-?\d+) ramp=(-?\d+) state=0x([0-9A-Fa-f]+) pos=(-?\d+),(-?\d+),(-?\d+) angle_y=(-?\d+) heading_valid=(\d+) camera_heading=(-?\d+) input_basis_e34=(-?\d+) desired_relative_e32=(-?\d+) desired_world=(-?\d+) wall=(\d+) ceiling=(\d+) collision=0x([0-9A-Fa-f]+).*?worker_mask_before=0x([0-9A-Fa-f]+).*?head_before=(-?\d+),(-?\d+),(-?\d+),(-?\d+)')
for name,p in paths.items():
 t=open(p,errors='ignore').read(); rows=[]
 for m in rx.finditer(t):
  g=m.groups()
  rows.append(dict(i=int(g[0]),tick=int(g[1]),elapsed=int(g[2]),f80=int(g[3]),synth=int(g[4]),sa=int(g[5]),clock=int(g[6]),ax=int(g[7]),ay=int(g[8]),state=int(g[10],16),x=int(g[11]),y=int(g[12]),z=int(g[13]),ang=int(g[14]),cam=int(g[16]),world=int(g[19]),wall=int(g[20]),coll=int(g[22],16),mask=int(g[23],16),ht=int(g[24]),hs=int(g[25]),hc=int(g[26]),timer=int(g[27])))
 print('\n===',name,'FINAL WINDOW ===')
 for r in rows:
  if 680 <= r['i'] <= 775:
   print("i={i:3} tick={tick:5} e={elapsed} head={ht},{hc},{timer:3} axes={ax:4},{ay:4} pos={x},{y},{z} ang={ang:4} cam={cam:4} world={world:4} state={state:06x} coll={coll:x}".format(**r))
