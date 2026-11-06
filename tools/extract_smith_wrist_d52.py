#!/usr/bin/env python3
"""Current A2 adapter and guided-wrist position statement for explicit-leaf tests.

Native iron lookup is the existing smith_tool_contact fixture's explicit leaf.
All contact admission, A2 solve/recovery and J27 fallbacks remain production.
"""
from pathlib import Path
import argparse, hashlib, json
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
rel='src/vrik/smith_anvil_contact_a2.inl';adapter=(root/rel).read_text()
start=adapter.index('static bool SmithIronA2(')
end=adapter.index('template<class CoherentStateA2>',start)
removed=adapter[start:end]
assert removed.count('static bool SmithIronA2(')==1
adapter=adapter[:start]+'// HOST LEAF: current native iron read supplied by existing fixture.\n'+adapter[end:]
# D56's complete independent adapter/reader is exercised by smith_native_floor_d56_test.
# This retained D52 fixture supplies an inactive floor owner to isolate its A2/J27 contract.
assert adapter.count('#include "smith_native_floor_d56.inl"')==1
adapter=adapter.replace('#include "smith_native_floor_d56.inl"','// HOST LEAF: inactive D56 floor; complete adapter tested separately.')
co=(root/'src/vrik/coherent_pose.inl').read_text()
line='            if(hw>.001f)goals[side]=Add(goals[side],Mul(Sub(FromT(s.original[h]),goals[side]),hw));'
assert co.count(line)==1
assert co.index(line)<co.index('#include "../subsystems/vrik/pose_final_wrist_d52.inl"')<co.index('        if(!accommodateShoulderR38(side,goals[side]')
free=(root/'src/subsystems/physics/pose_free_hand_contact.inl').read_text()
arm=(root/'src/subsystems/vrik/pose_arm_solver.inl').read_text()
assert 'SmithAnvilContactA2(' not in free
assert 'pc::slerp(PQ(handDesired)' not in arm
assert 'SetWorldRotation6689(s,h,handDesired);' in arm
(a.out/'smith_anvil_adapter_d52.inc').write_text(adapter)
(a.out/'smith_guidance_position_d52.inc').write_text(line+'\n')
paths=[rel,'src/vrik/coherent_pose.inl','src/subsystems/vrik/pose_final_wrist_d52.inl','src/subsystems/vrik/pose_arm_solver.inl','src/vrik/smith_tool_contact_j27.inl','src/subsystems/physics/pose_free_hand_contact.inl']
receipt={'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in paths},
 'order_assertions':['fixture position before final wrist/contact','contact before shoulder accommodation','no later orientation reblend','same accepted quaternion reaches final hand writer'],
 'native_leaves':['current iron entity/transform','native bounds','native world queries','native root reads'],
 'limitations':'Host synthetic contact; not native SEH, DLL injection, renderer, headset or recipe proof.'}
(a.out/'smith_wrist_d52_provenance.json').write_text(json.dumps(receipt,indent=2)+'\n')
print('D52 extracted current smith A2 adapter and guided position; caller order verified.')
