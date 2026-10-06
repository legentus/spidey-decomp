from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x00472DC0
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
fmt=Formatter(FormatterSyntax.INTEL)
ins=list(Decoder(32,p.read_bytes(),ip=base))
for i,x in enumerate(ins):
    if x.is_call_near and x.near_branch_target==0x00470610:
        print("\nCALL",f"{x.ip:08X}")
        for q in ins[max(0,i-20):min(len(ins),i+14)]:
            print(f"{q.ip:08X}: {fmt.format(q)}")
