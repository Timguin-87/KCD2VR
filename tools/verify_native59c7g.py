"""Read-only verification of C7G part-status and immersive-FOV ABI pins."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
from verify_v66112_native import PE
p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');a=p.parse_args()
pe=PE(a.dll);sha=hashlib.sha256(pe.data).hexdigest()
assert pe.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
r=Path(__file__).resolve().parents[1];checks=[]
source=read_source(r/'src/subsystems/gore/native_part_anchor.inl')
for addr,n,values in re.findall(r'\{0x([0-9A-F]+),(\d+),\{([^}]+)\}\}',source):
 addr=int(addr,16);raw=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',values));assert len(raw)==int(n)
 assert pe.read(addr,len(raw))==raw,hex(addr);checks.append(hex(addr))
assert len(checks)==10
for vf,fn in ((0x3A48B30,0x8A9348),(0x3A48DC0,0x8A6C90)):
 assert struct.unpack('<Q',pe.read(vf+0x30,8))[0]-pe.base==fn;checks.append(hex(vf+0x30))
source=read_source(r/'src/immersive_fov59c7g.cpp')
for label,addr in [('entry',0x53909c),('fov',0x5390fd),('caller',0x7f12e1),('receiver',0x7f12a7),('tail',0x5391ca)]:
 vals=re.search(r'const unsigned char '+label+r'\[\]\{([^}]+)\}',source).group(1)
 raw=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',vals));assert pe.read(addr,len(raw))==raw,hex(addr)
 checks.append(hex(addr))
source=read_source(r/'src/diagnostics/carry_observer59c7g.inl')
for addr,n,values in re.findall(r'\{0x([0-9A-F]+),(\d+),\{([^}]+)\}\}',source):
 addr=int(addr,16);raw=bytes(int(v,16) for v in re.findall(r'0x([0-9A-Fa-f]+)',values));assert len(raw)==int(n)
 assert pe.read(addr,len(raw))==raw,hex(addr);checks.append(hex(addr))
assert struct.unpack('<Q',pe.read(0x3A2F450,8))[0]-pe.base==0xB26C08
checks.append('0x3a2f450')
for slot,fn in ((0x3A48DC8,0x1A72480),(0x3A48E10,0x5D6F44)):
 assert struct.unpack('<Q',pe.read(slot,8))[0]-pe.base==fn;checks.append(hex(slot))
assert pe.read(0x1A72480,6)==bytes.fromhex('b802000000c3');checks.append('0x1a72480')
out={'sha256':sha,'timestamp':hex(pe.stamp),'verified_addresses':checks,'native_runtime_calls':False,
 'physics':'GetStatus type 1; part id +4; index +8; pose +18/+3C; scale +4C; timeBack input +78 is zero; buffer size 0x80. No constraint/action writes.',
 'camera':'SetFrustum +53909C takes camera,width,height,FOV,near,far,pixelAspect. CView+E8 call +7F12E1. Native rebuild tail +537BA4.',
 'unsupported':'Custom lens +48 is passed through. Region-material domain, native combo dispatch, ragdoll grabbing and kick-off remain unqualified.'}
if a.out:Path(a.out).write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
