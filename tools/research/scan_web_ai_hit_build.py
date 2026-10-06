from pathlib import Path
from iced_x86 import Decoder,Formatter,FormatterSyntax

for base in [0x004F5ED0,0x004F6C10]:
    p=Path(r"F:\Spider-Man 2000 Recomp\project main\tools\functions")/f"{base}.bin"
    print("\n==",hex(base),p.exists(),p.stat().st_size if p.exists() else None,"==")
    fmt=Formatter(FormatterSyntax.INTEL)
    for ins in Decoder(32,p.read_bytes(),ip=base):
        s=fmt.format(ins)
        low=s.lower()
        if ("call " in low or "byte ptr [esp" in low or "mov [esp" in low or
            "cmp " in low and any(x in low for x in ["4","8","0eh","14h"]) or
            "push 4" in low or "push 0eh" in low or "push 14h" in low):
            print(f"{ins.ip:08X}: {s}")
