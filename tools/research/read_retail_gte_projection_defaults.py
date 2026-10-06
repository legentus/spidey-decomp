import pefile,struct
exe=r"C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe"
pe=pefile.PE(exe,fast_load=True)
base=pe.OPTIONAL_HEADER.ImageBase
with open(exe,"rb") as f:
    for va in [0x0054F03C,0x0054F040,0x0054F044,0x0054F04C]:
        rva=va-base
        off=pe.get_offset_from_rva(rva)
        f.seek(off)
        b=f.read(4)
        print(hex(va),"off",hex(off),"u32",struct.unpack("<I",b)[0],"i32",struct.unpack("<i",b)[0],"float",struct.unpack("<f",b)[0])
