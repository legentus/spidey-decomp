from pathlib import Path
import pefile,struct,json
exe=Path(r"C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe")
pe=pefile.PE(str(exe),fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
with exe.open("rb") as f:
    f.seek(pe.get_offset_from_rva(0x0053BAB4-base))
    vals=struct.unpack("<16I",f.read(64))
j=json.load(open(r"F:\Spider-Man 2000 Recomp\project main\tools\names.json"))["functions"]
syms={int(v["address"]):v.get("name","") for v in j.values()}
for i,v in enumerate(vals):
    print(i,hex(0x0053BAB4+i*4),hex(v),syms.get(v,""))
