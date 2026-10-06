import pefile
from iced_x86 import Decoder,Formatter,FormatterSyntax
exe=r'C:\Program Files (x86)\Activision\Spider-Man\SpideyPC.exe'
pe=pefile.PE(exe,fast_load=True); base=pe.OPTIONAL_HEADER.ImageBase
start=0x455450; end=0x455540
off=pe.get_offset_from_rva(start-base); raw=open(exe,'rb').read()[off:off+end-start]
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,raw,ip=start): print(f'{ins.ip:08X}: {fmt.format(ins)}')