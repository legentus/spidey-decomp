import pefile,struct
exe=r'C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe'
pe=pefile.PE(exe,fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
off=pe.get_offset_from_rva(0x53B2E8-base)
raw=open(exe,'rb').read()
for i in range(20):
    v=struct.unpack_from('<I',raw,off+i*4)[0]
    print(i,hex(0x53B2E8+i*4),hex(v))