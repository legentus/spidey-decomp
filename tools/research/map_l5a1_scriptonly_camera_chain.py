from pathlib import Path
import struct
b=Path(r'F:\Spider-Man 2000 Recomp\project main\logs\l5a1_extract\L5A1_T.trg').read_bytes()
n=struct.unpack_from('<I',b,8)[0]; offs=list(struct.unpack_from('<%dI'%n,b,12))
def words(i):
 o=offs[i]; e=offs[i+1] if i+1<n else len(b); x=b[o:e]; return list(struct.unpack_from('<%dH'%(len(x)//2),x))
def links(i,w):
 t=w[0]
 li=3 if t==1 else (1 if t in (2,3,6,8,9,10,12,13,1000,1001) else (2 if t in (5,20,1002) else None))
 if li is None: return []
 c=w[li]; return w[li+1:li+1+c]
def delay203(w):
 # type1/203 layout in this level ends with 0x4280, delay, 0x4100
 for k in range(len(w)-2):
  if w[k]==0x4280 and w[k+2]==0x4100: return w[k+1]
 return None
def cam_info(i):
 w=words(i)
 if not w or w[0]!=6: return None
 # search opcode 186,frames
 for k in range(len(w)-1):
  if w[k]==186: return (w[k+1], links(i,w))
 return None
print('TYPE203 CAMERA-RAIL CONTROLLERS')
for i in range(n):
 w=words(i)
 if len(w)>2 and w[0]==1 and w[1]==203:
  ls=links(i,w); cams=[]; nxt=[]
  for x in ls:
   if x>=n: continue
   wi=words(x)
   ci=cam_info(x)
   if ci: cams.append((x,ci[0],ci[1]))
   if len(wi)>1 and wi[0]==1 and wi[1]==203: nxt.append(x)
  d=delay203(w)
  if cams or nxt or 60<=i<=120:
   print(f'node {i:3d} delay={d!s:>4} links={ls} cameras={cams} next203={nxt}')