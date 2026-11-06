# Offline retail DBA905/CHR800 inspector, no game writes or runtime dependency.
# Format cross-check: Markemp/Cryengine-Converter (ChunkController_905,
# CompiledBone800 and packed quaternion formats); see M31 handoff for reference.
from pathlib import Path
import zipfile,struct,zlib,json,math
import numpy as np
ROOT=Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data')
def chunks(b):
 _,_,n,at=struct.unpack_from('<4sIII',b)
 for i in range(n):
  k,v,ident,size,off=struct.unpack_from('<HHIII',b,at+i*16);yield k,v,b[off:off+size]
def load(p,n):
 with zipfile.ZipFile(ROOT/p) as z:return z.read(n)
b=next(x for k,v,x in chunks(load('Animations.pak','Animations/assets/book/book.dba')) if k==0x100d)
P,R,T,A=struct.unpack_from('<4I',b);p=16;groups=[]
for n,k in [(T,7),(P,9),(R,9)]:
 lengths=struct.unpack_from('<'+'H'*n,b,p);p+=n*2;counts=struct.unpack_from('<'+'I'*k,b,p);p+=k*4;formats=[i for i,c in enumerate(counts) for _ in range(c)];assert len(formats)==n;groups.append((lengths,formats))
offsets=[]
for n in [T,P,R+1]:offsets.append(struct.unpack_from('<'+'i'*n,b,p));p+=n*4
assert offsets[0][0]<0
pad=struct.unpack_from('<I',b,p)[0];p=(p+4+pad+3)&~3
anims=[]
for i in range(A):
 n=struct.unpack_from('<H',b,p)[0];p+=2;name=b[p:p+n].decode();p+=n;motion=struct.unpack_from('<IIifii',b,p);p+=132;foot=struct.unpack_from('<H',b,p)[0];p+=2+foot;count=struct.unpack_from('<H',b,p)[0];p+=6
 anims.append(dict(name=name,motion=motion,count=count))
for a in anims:
 a['controllers']=[struct.unpack_from('<Iiiii',b,p+j*20) for j in range(a['count'])];p+=a['count']*20
trackend=((p+3)&~3)-offsets[2][-1];assert trackend==len(b),(trackend,len(b))

def track(group,index):
 length,fmt=groups[group][0][index],groups[group][1][index];at=trackend+offsets[group][index]
 if group==0:
  if fmt in (0,1,2):return np.array(struct.unpack_from('<'+('f' if fmt==0 else 'H' if fmt==1 else 'B')*length,b,at))
  assert fmt==6
  start,end,n=struct.unpack_from('<3H',b,at);data=[]
  for i in range(3,length):
   word=struct.unpack_from('<H',b,at+i*2)[0]
   for bit in range(16):
    if word&(1<<bit):data.append(start+(i-3)*16+bit)
  assert len(data)==n and data[-1]==end
  return np.array(data)
 if group==1:
  assert fmt==2
  return np.array([struct.unpack_from('<3f',b,at+i*12) for i in range(length)])
 assert fmt in (5,8)
 out=[]
 for i in range(length):
  value=int.from_bytes(b[at+i*(6 if fmt==5 else 8):at+(i+1)*(6 if fmt==5 else 8)],'little');missing=value>>(46 if fmt==5 else 62);q=np.zeros(4);shift=0;target=0
  for j in range(4):
   if j==missing:continue
   bits=15 if fmt==5 else (21 if target<2 else 20);scale=23170 if fmt==5 else (1482909 if bits==21 else 741454);q[j]=((value>>shift)&((1<<bits)-1))/scale-math.sqrt(.5);shift+=bits;target+=1
  q[missing]=math.sqrt(max(0,1-float(q@q)));out.append(q)
 return np.array(out)

def qmat(q):
 x,y,z,w=q/np.linalg.norm(q)
 return np.array([[1-2*y*y-2*z*z,2*x*y-2*z*w,2*x*z+2*y*w],[2*x*y+2*z*w,1-2*x*x-2*z*z,2*y*z-2*x*w],[2*x*z-2*y*w,2*y*z+2*x*w,1-2*x*x-2*y*y]])

def sample(data,times,time,quat=False):
 if len(data)==1:return data[0]
 assert len(data)==len(times)
 j=int(np.clip(np.searchsorted(times,time)-1,0,len(times)-2));u=float(np.clip((time-times[j])/(times[j+1]-times[j]),0,1));a=data[j];c=data[j+1]
 if quat and a@c<0:c=-c
 v=a*(1-u)+c*u
 return v/np.linalg.norm(v) if quat else v

chr=load('IPL_Characters-part0.pak','Objects/characters/assets/parchment_folded/parchment_folded.chr');bones=[];streams={}
for k,v,c in chunks(chr):
 if k==0x2000:
  assert (len(c)-32)%584==0
  for i in range((len(c)-32)//584):
   row=c[32+i*584:32+(i+1)*584];ident=struct.unpack_from('<I',row)[0];name=row[312:568].split(bytes([0]))[0].decode();assert zlib.crc32(name.encode())==ident
   inv=np.eye(4);inv[:3,:]=np.array(struct.unpack_from('<12f',row,216)).reshape(3,4);world=np.eye(4);world[:3,:]=np.array(struct.unpack_from('<12f',row,264)).reshape(3,4);assert np.max(np.abs(inv@world-np.eye(4)))<1e-5
   parent=struct.unpack_from('<i',row,572)[0];bones.append(dict(id=ident,name=name,parent=i+parent if parent else -1,world=world,inv=inv))
 if k==0x1016:
  _,s,n,stride=struct.unpack_from('<4I',c);streams[s]=(n,stride,c[24:])
verts=np.array([struct.unpack_from('<3f',streams[0][2],i*12) for i in range(94)]);uv=np.array([struct.unpack_from('<2f',streams[8][2],i*8) for i in range(94)]);indices=np.array(struct.unpack('<'+'H'*264,streams[5][2])).reshape(-1,3)
skin=[struct.unpack_from('<4H4B',streams[9][2],i*12) for i in range(94)]
for bone in bones:bone['local']=bones[bone['parent']]['inv']@bone['world'] if bone['parent']>=0 else bone['world']

def frame(a,t):
 controls={c[0]:c for c in a['controllers']};worlds=[]
 for bone in bones:
  m=bone['local'].copy();c=controls.get(bone['id'])
  if c:
   if c[1]>=0 and c[2]>=0:m[:3,3]=sample(track(1,c[2]),track(0,c[1]),t)
   if c[3]>=0 and c[4]>=0:m[:3,:3]=qmat(sample(track(2,c[4]),track(0,c[3]),t,True))
  worlds.append(worlds[bone['parent']]@m if bone['parent']>=0 else m)
 result=[]
 for vert,row in zip(verts,skin):
  v=np.append(vert,1);point=np.zeros(4)
  assert sum(row[4:])==255
  for boneid,weight in zip(row[:4],row[4:]):
   if weight:point+=weight/255*(worlds[boneid]@bones[boneid]['inv']@v)
  result.append(point[:3])
 return np.array(result)

if __name__=='__main__':
 report={};snapshots={}
 for a in anims:
  if not any(a['name'].endswith('/'+n+'.caf') for n in ['open','close','reading_stand_inv_in','reading_stand_inv_loop','reading_stand_inv_out']):continue
  name=a['name'].split('/')[-1][:-4];matched=[bone['name'] for bone in bones if bone['id'] in {c[0] for c in a['controllers']}];start,end=a['motion'][4:6];snapshots[name]=[frame(a,start+(end-start)*t) for t in [0,.25,.5,.75,1]]
  report[name]={'matched_bones':matched,'duration':(end-start)*a['motion'][2]*a['motion'][3],'bbox_start':np.ptp(snapshots[name][0],axis=0).tolist(),'bbox_end':np.ptp(snapshots[name][-1],axis=0).tolist(),'vertex_travel_max':float(np.max(np.linalg.norm(snapshots[name][-1]-snapshots[name][0],axis=1)))}
 Path(__file__).with_suffix('.json').write_text(json.dumps(report,indent=2));np.savez(Path(__file__).with_suffix('.npz'),vertices=verts,triangles=indices,uv=uv,**{k:np.array(v) for k,v in snapshots.items()});print(json.dumps(report,indent=2))
