import pefile,struct
exe=r'C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe'
pe=pefile.PE(exe,fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
def rd(va,n):
    off=pe.get_offset_from_rva(va-base)
    with open(exe,'rb') as f: f.seek(off); return f.read(n)
# type 0xCB..0x140 use byte remap at 0x4DFB58 then handler table at 0x4DFB0C
remap=rd(0x4DFB58,0x76)
mx=max(remap)
handlers=struct.unpack('<%dI'%(mx+1),rd(0x4DFB0C,(mx+1)*4))
for typ in [203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220]:
    idx=remap[typ-0xCB]
    print(typ,'idx',idx,'handler',hex(handlers[idx]))