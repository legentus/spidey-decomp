from pathlib import Path
import json
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x4075B0
p=Path(r'F:\Spider-Man 2000 Recomp\project main\tools\functions')/f'{base:d}.bin'
data=p.read_bytes()
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,data,ip=base):
    print(f'{ins.ip:08X}: {fmt.format(ins)}')