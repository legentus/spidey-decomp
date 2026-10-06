import pefile,struct
exe=r'C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe'
pe=pefile.PE(exe,fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
raw=open(exe,'rb').read()
def rd(va,n):
 off=pe.get_offset_from_rva(va-base); return raw[off:off+n]
# command 0x4227..0x42B4 dispatch: index=(cmd-0x4227), remap at 0x406BB8, handler table 0x406B50
remap=rd(0x406BB8,0x8E)
mx=max(remap)
handlers=struct.unpack('<%dI'%(mx+1),rd(0x406B50,(mx+1)*4))
for cmd in [0x427F,0x4280,0x4281,0x4282,0x4283,0x4284,0x4285]:
 idx=remap[cmd-0x4227]
 print(hex(cmd),'idx',idx,'handler',hex(handlers[idx]))