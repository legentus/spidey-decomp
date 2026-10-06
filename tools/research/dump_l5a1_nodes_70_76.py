from pathlib import Path
import struct,re
p=Path(r'F:\Spider-Man 2000 Recomp\project main\logs\l5a1_extract\L5A1_T.trg')
b=p.read_bytes()
n=struct.unpack_from('<I',b,8)[0]
offs=list(struct.unpack_from('<%dI'%n,b,12))
for i in range(70,77):
    o=offs[i]; e=offs[i+1] if i+1<n else len(b)
    x=b[o:e]
    w=list(struct.unpack_from('<%dH'%(len(x)//2),x))
    s=[m.group().decode('latin1') for m in re.finditer(rb'[ -~]{3,}',x)]
    print()
    print('NODE',i,'off',hex(o),'len',e-o,'type',w[0] if w else None,'strings',s)
    print('WORDS',w[:140])
    print('HEX',x[:220].hex(' '))