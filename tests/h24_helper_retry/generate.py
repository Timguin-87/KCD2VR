from pathlib import Path
import hashlib,json,sys
root=Path(__file__).resolve().parents[2];check='--check' in sys.argv
sources={'src/vrik/physical_native.inl':('struct PhysEquipmentItem6698 {','struct PhysEquipment6698 {','actual_item.inc'),'src/subsystems/physics/native_equipment_snapshot.inl':('// Metadata-refresh-only helper retry.','static bool PhysResolveItem6698(','actual_retry.inc')}
for f,(a,b,out) in sources.items():
 s=(root/f).read_text();body=s[s.index(a):s.index(b)];dest=Path(__file__).parent/out
 if check: assert dest.read_text()==body,f'stale production fixture {out}'
 else: dest.write_text(body)
if not check:(Path(__file__).parent/'PROVENANCE.json').write_text(json.dumps({f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in sources},indent=2))
