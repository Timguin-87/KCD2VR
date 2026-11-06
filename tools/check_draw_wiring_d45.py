from pathlib import Path
import hashlib
root=Path(__file__).resolve().parent.parent
s=(root/'src/vrik/physical_native.inl').read_text()
fn=s[s.index('static PhysicalAttachmentResult101 PhysAttachmentResultImpl101('):]
reveal=fn.index('PhysRevealDrawnBowD45(equipment,item,now,!verify||verifyOk,false)')
assert fn.index('PlaceBothConsumers6689(')<reveal
assert fn.index('if(verify&&!verifyOk)')<reveal<fn.index('TracePhysicalItem28(')
assert 'PhysRevealDrawnBowD45(equipment,item,now,true,false)' not in fn
q=s[s.index('static ph::Contact103 PhysSweep6698('):s.index('\n#include "../subsystems/physics/shield_primary_b4')]
assert 'AcquisitionBurstD45 acquireBudgetD45(g_contact_budget6695,' in q
assert q.index('AcquisitionBurstD45')<q.index('WorldRayDistance6692(')
assert 'begin_work(' not in q and '.begin(' not in q
assert q in (root/'tests/generated_draw_native_d45.inc').read_text()
print('draw_wiring_d45: 7 source checks PASS; exact native sweep extraction SHA256 '+hashlib.sha256(q.encode()).hexdigest())
