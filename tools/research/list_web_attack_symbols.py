import json,re
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
pat=re.compile(r"web|impact|projectile|ball",re.I)
for v in sorted(j.values(),key=lambda x:int(x["address"])):
    n=v.get("name","")
    if pat.search(n):
        print(f"{int(v['address']):08X} {n}")
