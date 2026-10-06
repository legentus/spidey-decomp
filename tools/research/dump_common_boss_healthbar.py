from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x00464270
blob=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions\4604528.bin").read_bytes()
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,blob,ip=base):
    if 0x00464A60 <= ins.ip <= 0x00465168:
        print(f"{ins.ip:08X}: {fmt.format(ins)}")
