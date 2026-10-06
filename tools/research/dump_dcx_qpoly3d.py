from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
syms={int(v["address"]):v.get("name","") for v in j.values()}
fmt=Formatter(FormatterSyntax.INTEL)
for base in [0x00402700,0x00508550]:
    p=root/"tools/functions"/f"{base}.bin"
    print("\n===",hex(base),syms.get(base,""),"size",p.stat().st_size if p.exists() else None,"===")
    for ins in Decoder(32,p.read_bytes(),ip=base):
        s=fmt.format(ins); extra=""
        if ins.is_call_near:
            extra=" ; "+syms.get(ins.near_branch_target,"")
        print(f"{ins.ip:08X}: {s}{extra}")
