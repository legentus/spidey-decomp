from pathlib import Path
import struct,json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
j=json.load(open(root/"tools/names.json"))["functions"]
targets=[0x0056E668,0x0056E6F8,0x0056F1E4,0x0056F1B4]
for target in targets:
    pat=struct.pack("<I",target)
    print("\nTARGET",hex(target))
    for v in sorted(j.values(),key=lambda x:int(x["address"])):
        a=int(v["address"])
        p=root/"tools/functions"/f"{a}.bin"
        if not p.exists(): continue
        b=p.read_bytes()
        if pat in b:
            offs=[]
            start=0
            while True:
                k=b.find(pat,start)
                if k<0: break
                offs.append(k); start=k+1
            print(f"{a:08X} {v.get('name','')} refs",",".join(hex(x) for x in offs))
