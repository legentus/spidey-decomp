import json
j=json.load(open(r'F:\Spider-Man 2000 Recomp\project main\tools\names.json'))['functions']
for v in sorted(j.values(), key=lambda x:x['address']):
    n=v.get('name','')
    if 'CScriptOnlyBaddy' in n:
        print(f"{v['address']:08X} {v['address']} {n}")