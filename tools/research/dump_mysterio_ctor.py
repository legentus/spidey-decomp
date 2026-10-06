from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json,re
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
base=0x0045C910
j=json.load(open(root/"tools/names.json"))["functions"]
p=root/"tools/functions"/f"{base}.bin"
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,p.read_bytes(),ip=base):
    s=fmt.format(ins)
    if "53" in s or "mov" in s.lower() or ins.is_call_near:
        print(f"{ins.ip:08X}: {s}")
