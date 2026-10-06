from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax
fmt=Formatter(FormatterSyntax.INTEL)
for base in [0x00453200,0x00472DC0,0x00443C10]:
    p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
    print("\n===",hex(base),"===")
    ins=list(Decoder(32,p.read_bytes(),ip=base))
    for i,x in enumerate(ins):
        s=fmt.format(x)
        if any(t in s.lower() for t in ["61b5fch","628614h","628618h","654f54h"]):
            for q in ins[max(0,i-18):min(len(ins),i+26)]:
                print(f"{q.ip:08X}: {fmt.format(q)}")
            print()
