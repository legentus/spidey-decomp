from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x004C5DD0
blob=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
print("blob",blob,blob.exists(),blob.stat().st_size if blob.exists() else None)
fmt=Formatter(FormatterSyntax.INTEL)
for ins in Decoder(32,blob.read_bytes(),ip=base):
    print(f"{ins.ip:08X}: {fmt.format(ins)}")
