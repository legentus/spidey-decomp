from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x0045EF10
blob=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions\4583184.bin").read_bytes()
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,blob,ip=base):
    if 0x0045F300 <= ins.ip <= 0x0045F560:
        print(f"{ins.ip:08X}: {fmt.format(ins)}")
