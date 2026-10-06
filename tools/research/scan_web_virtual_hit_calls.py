from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
fmt=Formatter(FormatterSyntax.INTEL)
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    a=int(v["address"])
    if not (0x004F5000 <= a <= 0x004FA500):
        continue
    blob=root/"tools/functions"/f"{a}.bin"
    if not blob.exists():
        continue
    hits=[]
    for ins in Decoder(32,blob.read_bytes(),ip=a):
        s=fmt.format(ins).lower()
        if s.startswith("call ") and ("+0ch]" in s or "+0c]" in s or "[eax+0ch]" in s or "[edx+0ch]" in s or "[ecx+0ch]" in s):
            hits.append((ins.ip,s))
    if hits:
        print(f"{a:08X} {v.get('name','')}")
        for ip,s in hits:
            print(f"  {ip:08X}: {s}")
