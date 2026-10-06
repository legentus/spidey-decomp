import json
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    n=v.get("name","")
    if "SoftSpot" in n or "Mysterio" in n and ("Hit" in n or "Damage" in n):
        print(f"{int(v['address']):08X} {n}")
