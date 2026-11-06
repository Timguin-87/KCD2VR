from pathlib import Path
import hashlib,json,sys
check="--check" in sys.argv
root=Path(__file__).resolve().parents[2]
a=(root/'src/diagnostics/native_hit_probe134.inl').read_text()
a=a[a.index('void __fastcall ActorUpdateDetour'):]; a=a[:a.index('    if(!Active())return;')]+'}\n'
b=(root/'src/subsystems/vrik/host.inl').read_text(); b=b[b.index('static void ApplyRoomscalePad58'):]; b=b[:b.index('// M2_SLOT_END 5017')]
c=(root/'src/diagnostics/geometry_hit178r27.inl').read_text(); c=c[c.index('int HenryInCombat178r31'):];c=c[:c.index('\n#include')]
for name,body in [('actual_actor_gameplay_prefix.inc',a),('actual_roomscale_pad.inc',b),('actual_combat_cache_getter.inc',c)]:
 dest=Path(__file__).parent/name
 if check: assert dest.read_text()==body, f'stale production fixture: {name}'
 else: dest.write_text(body)
if not check: (Path(__file__).parent/'PROVENANCE.json').write_text(json.dumps({f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/diagnostics/native_hit_probe134.inl','src/diagnostics/geometry_hit178r27.inl','src/diagnostics/henry_combat_context_h20.inl','src/subsystems/vrik/host.inl','src/vrik/roomscale58.h']},indent=2))
