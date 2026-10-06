from pathlib import Path
import json
from iced_x86 import Decoder,Formatter,FormatterSyntax
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
# print likely ctor/AI/vtable-related symbols
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    name=v.get("name","")
    if "Mysterio" in name and ("AI" in name or "Mysterio_CMysterio" in name or "Mysterio::Mysterio" in name):
        print(f"{int(v['address']):08X} {name}")
