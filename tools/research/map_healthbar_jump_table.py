from pathlib import Path
import pefile,struct
exe=Path(r"C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe")
pe=pefile.PE(str(exe),fast_load=True)
base=pe.OPTIONAL_HEADER.ImageBase
with exe.open('rb') as f:
    def rd(va,n):
        f.seek(pe.get_offset_from_rva(va-base)); return f.read(n)
    tbl=struct.unpack('<8I',rd(0x0046588C,32))
for i,t in enumerate(tbl):
    print(hex(0x133+i),hex(t))
