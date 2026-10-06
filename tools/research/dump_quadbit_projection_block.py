from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x004097E0
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,p.read_bytes(),ip=base):
    if 0x00409A00 <= ins.ip <= 0x0040A120:
        print(f"{ins.ip:08X}: {fmt.format(ins)}")
