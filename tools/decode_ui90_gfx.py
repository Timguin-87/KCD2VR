#!/usr/bin/env python3
"""Offline supplied-GFX PlaceObject decoder; never run by the mod. Does not execute ActionScript."""
import struct,zlib,json
from pathlib import Path
class Bits:
 def __init__(self,d,pos=0): self.d=d;self.b=pos*8
 def u(self,n):
  r=0
  for i in range(n):r=(r<<1)|((self.d[self.b//8]>>(7-self.b%8))&1);self.b+=1
  return r
 def s(self,n):
  if not n:return 0
  r=self.u(n);return r-(1<<n) if r&(1<<(n-1)) else r
 def pos(self):return (self.b+7)//8
 def mat(self):
  sx=sy=1.;rx=ry=0.
  if self.u(1):n=self.u(5);sx=self.s(n)/65536.;sy=self.s(n)/65536.
  if self.u(1):n=self.u(5);rx=self.s(n)/65536.;ry=self.s(n)/65536.
  n=self.u(5);x=self.s(n)/20.;y=self.s(n)/20.
  return [sx,sy,rx,ry,x,y]
 def cx(self):
  add=self.u(1);mul=self.u(1);n=self.u(4)
  if mul:[self.s(n) for _ in range(4)]
  if add:[self.s(n) for _ in range(4)]

def place(tag,d):
 if tag not in (26,70):return None
 f=d[0];f2=d[1] if tag==70 else 0;p=2 if tag==70 else 1
 depth=struct.unpack_from('<H',d,p)[0];p+=2
 if f2&8 or (f2&16 and f&2):p=d.index(0,p)+1
 cid=None
 if f&2:cid=struct.unpack_from('<H',d,p)[0];p+=2
 m=None
 if f&4:b=Bits(d,p);m=b.mat();p=b.pos()
 if f&8:b=Bits(d,p);b.cx();p=b.pos()
 if f&16:p+=2
 name=d[p:d.index(0,p)].decode('utf8','replace') if f&32 else None
 return {'depth':depth,'id':cid,'matrix':m,'name':name}

def tags(d,start=0):
 p=start
 while p+2<=len(d):
  h=struct.unpack_from('<H',d,p)[0];p+=2;t=h>>6;n=h&63
  if n==63:n=struct.unpack_from('<I',d,p)[0];p+=4
  if p+n>len(d):raise ValueError((p,n,len(d)))
  yield t,d[p:p+n];p+=n
  if t==0:break

def decode(path):
 raw=Path(path).read_bytes()
 if raw[:3] not in (b'CFX',b'GFX',b'CWS',b'FWS'):raise ValueError('Unsupported GFX/SWF signature')
 d=zlib.decompress(raw[8:]) if raw[:1]==b'C' else raw[8:]
 b=Bits(d);n=b.u(5);rect=[b.s(n) for _ in range(4)];start=b.pos()+4
 sprites={};root=[]
 for t,v in tags(d,start):
  if t==39:
   cid,frames=struct.unpack_from('<HH',v);entries=[]
   for st,sv in tags(v,4):
    entry=place(st,sv)
    if entry:entries.append(entry)
   sprites[cid]={'frames':frames,'entries':entries}
  else:
   entry=place(t,v)
   if entry:root.append(entry)
 return {'stage_twips':rect,'root':root,'sprites':sprites}
if __name__=='__main__':
 import argparse,sys
 parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('gfx',type=Path);parser.add_argument('--output',type=Path)
 a=parser.parse_args()
 try:
  result=decode(a.gfx);text=json.dumps(result,indent=2)+'\n'
  if a.output:a.output.write_text(text)
  else:print(text,end='')
 except (OSError,ValueError,IndexError,struct.error,zlib.error) as e:
  print('GFX decode failed:',e,file=sys.stderr);sys.exit(1)
