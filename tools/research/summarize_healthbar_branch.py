from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
base=0x00464270
blob=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions\4604528.bin").read_bytes()
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
syms={int(v["address"]):v.get("name","") for v in j.values()}
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,blob,ip=base):
    if 0x004649B0 <= ins.ip <= 0x00465882:
        s=fmt.format(ins)
        op=s.split(None,1)[0].lower() if s else ""
        if ins.is_call_near or op in ("cmp","jmp","je","jne","jl","jg","jle","jge","test","ja","jb","jbe","jae"):
            extra=""
            if ins.is_call_near:
                t=ins.near_branch_target
                extra=" ; "+syms.get(t,"")
            print(f"{ins.ip:08X}: {s}{extra}")
