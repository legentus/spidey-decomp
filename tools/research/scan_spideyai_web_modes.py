from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
base=0x004B13F0
p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
fmt=Formatter(FormatterSyntax.INTEL)
ins=list(Decoder(32,p.read_bytes(),ip=base))
for i,x in enumerate(ins):
    s=fmt.format(x).lower()
    if "8f8h" in s or (x.is_call_near and x.near_branch_target==0x004C5DD0):
        print("\n---",f"{x.ip:08X}",fmt.format(x),"---")
        for q in ins[max(0,i-18):min(len(ins),i+18)]:
            print(f"{q.ip:08X}: {fmt.format(q)}")
