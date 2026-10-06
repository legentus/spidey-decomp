from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x0045F940
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
print("blob",p,p.exists(),p.stat().st_size if p.exists() else None)
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,p.read_bytes(),ip=base):
    print(f"{ins.ip:08X}: {fmt.format(ins)}")
