from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
import json
root=Path(r"F:\Spider-Man 2000 Recomp\project main")
base=0x00508550
p=root/"tools/functions"/f"{base}.bin"
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,p.read_bytes(),ip=base):
    s=fmt.format(ins)
    print(f"{ins.ip:08X}: {s}")
