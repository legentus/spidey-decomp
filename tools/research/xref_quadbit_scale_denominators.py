from pathlib import Path
import struct,json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
for target in [0x0061B5FC,0x00628614,0x00628618,0x00654F54]:
    pat=struct.pack("<I",target)
    print("\nTARGET",hex(target))
    for v in sorted(j.values(),key=lambda x:int(x["address"])):
        a=int(v["address"]); p=root/"tools/functions"/f"{a}.bin"
        if not p.exists(): continue
        b=p.read_bytes()
        if pat in b:
            print(f"{a:08X} {v.get('name','')}")
