from pathlib import Path
import struct,json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
for target in [0x0054F03C,0x0054F040,0x0054F044]:
    pat=struct.pack("<I",target)
    print("\nTARGET",hex(target))
    for v in sorted(j.values(),key=lambda x:int(x["address"])):
        a=int(v["address"]); p=root/"tools/functions"/f"{a}.bin"
        if not p.exists(): continue
        b=p.read_bytes()
        if pat in b:
            print(f"{a:08X} {v.get('name','')}")
