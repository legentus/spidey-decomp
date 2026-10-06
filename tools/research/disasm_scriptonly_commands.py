from pathlib import Path
import json
from iced_x86 import Decoder,Formatter,FormatterSyntax
for base in (0x407A50,0x407A40):
 p=Path(r'F:\Spider-Man 2000 Recomp\project main\tools\functions')/f'{base:d}.bin'
 print('\nFUNC',hex(base),'blob',p.exists(),p.stat().st_size if p.exists() else None)
 data=p.read_bytes()
 j=json.load(open(r'F:\Spider-Man 2000 Recomp\project main\tools\names.json'))['functions']
 syms={int(v['address']):v.get('name','') for v in j.values()}
 fmt=Formatter(FormatterSyntax.INTEL)
 for ins in Decoder(32,data,ip=base):
  s=fmt.format(ins); extra=''
  if ins.is_call_near or ins.is_jmp_near:
   t=ins.near_branch_target
   if t in syms: extra=' ; '+syms[t]
  print(f'{ins.ip:08X}: {s}{extra}')