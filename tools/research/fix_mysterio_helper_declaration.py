from pathlib import Path
p=Path(r"F:\Spider-Man 2000 Recomp\project main\main.cpp")
s=p.read_text(encoding="utf-8")
needle='''typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);
'''
insert='''static int SpideyIsMysterioBossActive();

typedef void (__fastcall *SpideyRetailMysterioFireBoobiesFn)(
		CMysterio*,
		void*);
'''
if needle not in s:
    raise SystemExit("needle missing")
s=s.replace(needle,insert,1)
p.write_text(s,encoding="utf-8")
print("inserted forward declaration")
