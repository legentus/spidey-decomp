from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
target=0x0045D200
fmt=Formatter(FormatterSyntax.INTEL)
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    a=int(v["address"])
    if not (0x00458000 <= a <= 0x00461000):
        continue
    p=root/"tools/functions"/f"{a}.bin"
    if not p.exists(): continue
    hits=[]
    for ins in Decoder(32,p.read_bytes(),ip=a):
        if ins.is_call_near and ins.near_branch_target==target:
            hits.append(ins.ip)
    if hits:
        print(f"{a:08X} {v.get('name','')} calls:",",".join(f"{x:08X}" for x in hits))
