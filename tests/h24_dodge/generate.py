from pathlib import Path
import hashlib,json,sys,os
check="--check" in sys.argv
def emit(name,body):
 p=Path(__file__).parent/name
 if check: assert p.read_text()==body, f"stale production fixture: {name}"
 else:
  q=p.with_name(p.name+".tmp");q.write_text(body);os.replace(q,p)
root=Path(__file__).resolve().parents[2]
s=(root/'src/subsystems/vrik/host.inl').read_text();a=s.index('static void ApplyRoomscalePad58(');b=s.index('// M2_SLOT_END 5017',a)
body=s[a:b];emit('actual_roomscale_pad.inc',body)
s=(root/'src/combat_diag_probe.cpp').read_text();a=s.index('bool IncomingHmdDodgeH24(');b=s.index('\n}',a)+2
a2=s.index('bool EngagedHmdDodgeD74(');b2=s.index('\n}',a2)+2
emit('actual_cache_getter.inc',s[a:b]+'\n'+s[a2:b2])
a=s.index('bool NativeBlockIndicatorH24(');b=s.index('\n}',a)+2
emit('actual_block_getter.inc',s[a:b])
files=['src/subsystems/vrik/host.inl','src/combat_diag_probe.cpp','src/diagnostics/hmd_dodge_context_h24.h','src/diagnostics/henry_combat_context_h20.inl']
emit('PROVENANCE.json',json.dumps({f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in files},indent=2))
