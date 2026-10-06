from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
target=0x004C5DD0
fmt=Formatter(FormatterSyntax.INTEL)
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    a=int(v["address"])
    p=root/"tools/functions"/f"{a}.bin"
    if not p.exists(): continue
    insns=list(Decoder(32,p.read_bytes(),ip=a))
    for idx,ins in enumerate(insns):
        if ins.is_call_near and ins.near_branch_target==target:
            print(f"\n{a:08X} {v.get('name','')} call={ins.ip:08X}")
            for q in insns[max(0,idx-14):idx+3]:
                print(f"  {q.ip:08X}: {fmt.format(q)}")
