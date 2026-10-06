from pathlib import Path
import json
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x4E7E10
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions\5144080.bin")
print("blob",p.exists(),p.stat().st_size if p.exists() else None)
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
syms={int(v["address"]):v.get("name","") for v in j.values()}
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,p.read_bytes(),ip=base):
 s=fmt.format(ins); extra=""
 if ins.is_call_near or ins.is_jmp_near:
  t=ins.near_branch_target
  if t in syms: extra=" ; "+syms[t]
 print(f"{ins.ip:08X}: {s}{extra}")
