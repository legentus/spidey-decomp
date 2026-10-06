from pathlib import Path
import struct
b=Path(r'F:\Spider-Man 2000 Recomp\project main\logs\l5a1_extract\L5A1_T.trg').read_bytes()
n=struct.unpack_from('<I',b,8)[0]; offs=list(struct.unpack_from('<%dI'%n,b,12))
def raw(i):
    o=offs[i]; e=offs[i+1] if i+1<n else len(b); return b[o:e]
def words(i):
    x=raw(i); return list(struct.unpack_from('<%dH'%(len(x)//2),x))
def links(i):
    w=words(i); t=w[0]
    li=3 if t==1 else (1 if t in (2,3,6,8,9,10,12,13,1000,1001) else (2 if t in (5,20,1002) else None))
    if li is None: return []
    c=w[li]; return w[li+1:li+1+c]
def pos_type1(i):
    x=raw(i); w=words(i); cnt=w[3]; p=8+2*cnt
    while p<len(x) and x[p]!=0xFF: p+=1
    p=(p+4)&~3
    xyz=struct.unpack_from('<iii',x,p)
    return tuple(v<<12 for v in xyz),p
for i in [76,72,75]:
    w=words(i); print('NODE',i,'type',w[0],'links',links(i))
    if w[0]==1: print('pos',pos_type1(i))
    if w[0]==12:
        # probable camera target: 3 int32 position units followed by two 16-bit angles
        x=raw(i); p=4
        xyz=struct.unpack_from('<iii',x,p)
        print('probable target xyz units',xyz,'fixed',tuple(v<<12 for v in xyz),'tail_u16',struct.unpack_from('<HH',x,p+12))