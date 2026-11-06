"""Read-only exact-image verification of C4R4's no-point articulated-only CPhysicalProxy impulse gate."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse,hashlib,json,re,struct
from native_research112 import PE
p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');a=p.parse_args();pe=PE(a.dll)
sha=hashlib.sha256(pe.data).hexdigest();assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7' and pe.stamp==0x6a350e20
s=read_source(Path(__file__).resolve().parents[1]/'src/diagnostics/geometry_hit178r27.inl');pins=[]
for name,rva in [('kProxyImpulseEntry5916c3',0x1466710),('kParentEntry5916c3',0x83a8f4),('kArticulatedType19c4r4',0x1a72e50),('kArticulatedAction19c4r4',0xa99df0),('kNoPoint19c4r4',0x146681a),('kQueuedAction19c4r4',0x146687c),('kNoSkeleton19c4r4',0x1466892)]:
 raw=re.search(name+r'\[\]\{([^}]+)',s).group(1);wanted=bytes(int(x,16) for x in re.findall(r'0x([0-9a-fA-F]+)',raw));assert pe.read(rva,len(wanted))==wanted,name
 pins.append({'name':name,'rva':hex(rva),'bytes':wanted.hex()})
vt=0x3ac3478
assert struct.unpack('<Q',pe.read(vt+0xd0,8))[0]==pe.base+0x1466710
assert struct.unpack('<Q',pe.read(vt+0xb0,8))[0]==pe.base+0x602360
assert pe.read(0x602360,5)==bytes.fromhex('488b4138c3')
assert struct.unpack('<Q',pe.read(0x4072c78+0xd8,8))[0]==pe.base+0x83a8f4
col=struct.unpack('<Q',pe.read(vt-8,8))[0]-pe.base;data=struct.unpack('<6I',pe.read(col,24))
assert data[0]==1 and data[1]==0 and data[5]==col
name=b'.?AVCPhysicalProxy@@\0'
assert pe.read(data[3]+16,len(name))==name
# CArticulatedEntity, not CLivingEntity or a presumed SDK enum.
art=0x3a48b30;acol=struct.unpack('<Q',pe.read(art-8,8))[0]-pe.base;ad=struct.unpack('<6I',pe.read(acol,24))
assert ad[0]==1 and ad[1]==0 and ad[5]==acol
aname=b'.?AVCArticulatedEntity@@\0';assert pe.read(ad[3]+16,len(aname))==aname
assert struct.unpack('<Q',pe.read(art+8,8))[0]==pe.base+0x1a72e50
assert pe.read(0x1a72e50,6)==bytes.fromhex('b806000000c3')
assert struct.unpack('<Q',pe.read(art+0x38,8))[0]==pe.base+0xa99df0
assert struct.unpack('<Q',pe.read(art+0x30,8))[0]==pe.base+0x8a9348
# Normal scheduling argument is passed to the native request-copy helper.
assert pe.read(0xa99e65,27)==bytes.fromhex('44894424204c8bca4c8b4168488bd1488d8dd8010000e800c2daff')
# bPos false leaves point unused and bypasses the second skeleton dispatch.
assert pe.read(0x146684b,6)==bytes.fromhex('0f85d906f000')
assert pe.read(0x2366f2a,23)==bytes.fromhex('f2410f1007418b4708f20f1144247c894584e910f90fff')
assert 'NativeImpulse19c4r4(proxy,r,impulse,false,why)' in s
out={'sha256':sha,'timestamp':hex(pe.stamp),'proxy_vtable':hex(vt),'COL':hex(col),'pins':pins,
 'articulated_vtable':hex(art),'articulated_gettype':'native6; never assumed living/dead state',
 'no_point':'146681A/146684B leaves point+angular UNUSED; bPos false at1466892 skips2366F41 skeleton pass',
 'scheduling':'146687C r8d=0; Articulated Action A99E65 passes zero to846080 native copied request',
 'body_getter':'proxy+0x38','root_walk':'0x421500; child-parent forwarding intentionally rejected','native_execution':False}
text=json.dumps(out,indent=2);print(text)
if a.out:Path(a.out).write_text(text+'\n')
