"""Read-only retail verification. Extracts pins from the production hook."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys,re,struct,json,hashlib
from native_research112 import PE
root=Path(__file__).resolve().parents[1]
p=PE(sys.argv[1])
sha=hashlib.sha256(p.data).hexdigest()
assert sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
assert p.stamp==0x6A350E20
source=read_source(root/'src/activity_lens59c10r1.cpp')
rows=[]
for address,raw,count in re.findall(r'\{(0x[0-9A-F]+),"((?:\\x[0-9A-F]{2})+)",(\d+)\}',source):
    rva=int(address,16);expected=bytes.fromhex(raw.replace('\\x',''))
    actual=p.read(rva,len(expected));assert len(expected)==int(count)
    rows.append(dict(rva=address,expected=expected.hex(),actual=actual.hex(),match=actual==expected))
assert len(rows)==17
for slot,target in [(0x3E63CA8,0x9E5F4C),(0x3A81030,0x7F22FC),(0x3A81038,0x3B6E80)]:
    actual=struct.unpack('<Q',p.read(slot,8))[0]-p.base
    rows.append(dict(slot=hex(slot),target=hex(target),actual=hex(actual),match=actual==target))
# MSVC COL validates the actual receiver's +40 subobject, not an SDK guess.
col=struct.unpack('<Q',p.read(0x3A81020,8))[0]-p.base
signature,offset,ctor,desc,hierarchy,self_rva=struct.unpack('<6I',p.read(col,24))
name=p.read(desc+16,128).split(b'\0')[0].decode()
assert signature==1 and offset==0x40 and self_rva==col
assert name=='.?AVC_Player@entitymodule@wh@@'
dialogue_types=[]
for vft,expected,subobject in [
    (0x3E64C38,'.?AVC_DialogModule@dialogmodule@wh@@',0),
    (0x3E64C90,'.?AVC_DialogManager@dialogmodule@wh@@',0),
    (0x3E63CA8,'.?AVC_DialogCameraManager@dialogmodule@wh@@',0),
    (0x3E63A40,'.?AVC_DialogInstance@dialogmodule@wh@@',0),
    (0x3E63B60,'.?AVC_DialogInstance@dialogmodule@wh@@',0x18),
    (0x3A5A210,'.?AVC_CameraDialog@game@wh@@',0)]:
    loc=struct.unpack('<Q',p.read(vft-8,8))[0]-p.base
    sig,off,_,desc,_,self_rva=struct.unpack('<6I',p.read(loc,24))
    actual_name=p.read(desc+16,128).split(b'\0')[0].decode()
    assert sig==1 and off==subobject and self_rva==loc and actual_name==expected
    dialogue_types.append(dict(vtable=hex(vft),name=actual_name,subobject=hex(off)))
result=dict(sha256=sha,timestamp=hex(p.stamp),receiver=name,subobject=hex(offset),pins=rows,
            dialogue_types=dialogue_types,
            passed=all(r['match'] for r in rows))
print(json.dumps(result,indent=2));assert result['passed']
