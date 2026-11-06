"""Read-only verification of the R59.10C1 game-hit notification against retail WHGame."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import argparse,hashlib,json,re,struct
from pathlib import Path
from native_research112 import PE
p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');args=p.parse_args()
pe=PE(args.dll);sha=hashlib.sha256(pe.data).hexdigest()
assert pe.stamp==0x6A350E20 and sha=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
root=Path(__file__).resolve().parents[1]
s=read_source(root/'src/diagnostics/game_hit_event5910c1.inl')
results=[]
for name,rva in [('kQueueHitEntry5910c1',0x725698),('kHitGlobalRead5910c1',0x9155F1),('kNativeHitEventCall5910c1',0xC526BC)]:
 raw=re.search(r'\b'+name+r'\[\]\{([^}]+)',s).group(1);wanted=bytes(int(x,16) for x in re.findall(r'0x([0-9A-Fa-f]+)',raw))
 actual=pe.read(rva,len(wanted));assert actual==wanted,name
 results.append(dict(name=name,rva=hex(rva),bytes=actual.hex()))
assert 0x9155F8+struct.unpack('<i',pe.read(0x9155F4,4))[0]==0x549D388
assert struct.unpack('<Q',pe.read(0x3AF1678+0x528,8))[0]-pe.base==0x725698
col=struct.unpack('<Q',pe.read(0x3AF1678-8,8))[0]-pe.base
rtti=struct.unpack('<6I',pe.read(col,24))
assert rtti[0]==1 and rtti[1]==0 and rtti[5]==col
assert pe.read(rtti[3]+16,len(b'.?AVCGameRules@@\0'))==b'.?AVCGameRules@@\0'
# Confirm appended element size and the copy helper's last block at +90.
assert pe.read(0x7256B2,8)==bytes.fromhex('49814108a0000000')
assert pe.read(0x39DC0A8,11)==bytes.fromhex('0f104402100f11440110c3')
out=dict(sha256=sha,timestamp=hex(pe.stamp),pins=results,global_rva='0x549D388',service_offset='0x1A0',
 game_rules_vtable='0x3AF1678',vtable_slot='0x528',queue_rva='0x725698',copy_size='0xA0',
 game_rule_rtti=True,live_native_execution=False)
text=json.dumps(out,indent=2);print(text)
if args.out:Path(args.out).write_text(text+'\n')
