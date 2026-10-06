from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import struct,json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
fmt=Formatter(FormatterSyntax.INTEL)
base=0x00476A00
p=root/"tools/functions"/f"{base}.bin"
print("=== matrix4x4_ml ===")
for ins in Decoder(32,p.read_bytes(),ip=base):
    print(f"{ins.ip:08X}: {fmt.format(ins)}")
pat=struct.pack("<I",0x0056E778)
j=json.load(open(root/"tools/names.json"))["functions"]
print("\n=== refs 0x56E778 ===")
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    a=int(v["address"]); q=root/"tools/functions"/f"{a}.bin"
    if q.exists() and pat in q.read_bytes():
        print(f"{a:08X} {v.get('name','')}")
