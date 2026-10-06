from pathlib import Path
import pefile,struct
exe=Path(r"C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe")
pe=pefile.PE(str(exe),fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
with exe.open('rb') as f:
    f.seek(pe.get_offset_from_rva(0x0045F66C-base))
    tbl=struct.unpack('<10I',f.read(40))
for i,t in enumerate(tbl):
    print(i,hex(t))
