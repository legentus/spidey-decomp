from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
base=0x00464270
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
entry=None
for v in j.values():
    if int(v["address"])==base:
        entry=v
        break
print("entry",entry)
blob=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/(str(base)+".bin")
print("blob",blob,blob.exists(),blob.stat().st_size if blob.exists() else None)
syms={int(v["address"]):v.get("name","") for v in j.values()}
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,blob.read_bytes(),ip=base):
    s=fmt.format(ins); extra=""
    if ins.is_call_near or ins.is_jmp_near:
        t=ins.near_branch_target
        if t in syms: extra=" ; "+syms[t]
    print(f"{ins.ip:08X}: {s}{extra}")
