from pathlib import Path
import struct,re
b=Path(r'F:\Spider-Man 2000 Recomp\project main\logs\l5a1_extract\L5A1_T.trg').read_bytes()
n=struct.unpack_from('<I',b,8)[0]; offs=list(struct.unpack_from('<%dI'%n,b,12))
for i in [76,77,78,79,80,81,82,83,84,85,86,87,88,89]:
 o=offs[i]; e=offs[i+1] if i+1<n else len(b); x=b[o:e]
 w=list(struct.unpack_from('<%dH'%(len(x)//2),x))
 s=[m.group().decode('latin1') for m in re.finditer(rb'[ -~]{3,}',x)]
 print('\nNODE',i,'type',w[0] if w else None,'len',e-o,'strings',s)
 print('WORDS',w[:100])