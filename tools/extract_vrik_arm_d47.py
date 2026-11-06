#!/usr/bin/env python3
"""Extract the live arm/reach/FK and acceptance boundary for a host fixture.
No alternate solver or detached copy: these bodies are verbatim production.
The fixture replaces native memory/services and supplies the already-admitted
pose/ownership inputs. It does not emulate WHGame or a renderer.
"""
from pathlib import Path
import argparse, hashlib, json
from source_text import read_source
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
paths={n:root/n for n in ('src/vrik/coherent_pose.inl','src/subsystems/vrik/pose_anatomy.inl','src/subsystems/vrik/pose_reach_input.inl','src/subsystems/vrik/pose_fingers.inl','src/subsystems/vrik/pose_leg_solver.inl','src/subsystems/vrik/pose_arm_solver.inl','src/subsystems/vrik/pose_final_wrist_d52.inl','src/subsystems/vrik/native_ik_observer.inl','src/vrik/elbow_continuity.h','src/vrik/frik_elbow_lab.h','src/vrik/elbow_shoulder_d45.h','src/vrik/pose_math.h','src/vrik/arm_constraints.h','src/vrik/upper_body.h','src/vrik/visual_selection59c7f.h','src/vrik/shoulder_reach_d38.h','src/frik_pose_probe.cpp')}
src={k:v.read_text() for k,v in paths.items()};co=src['src/vrik/coherent_pose.inl'];an=src['src/subsystems/vrik/pose_anatomy.inl'];reach=src['src/subsystems/vrik/pose_reach_input.inl'];finger=src['src/subsystems/vrik/pose_fingers.inl'];leg=src['src/subsystems/vrik/pose_leg_solver.inl']
def region(text,start,end):
 assert text.count(start)==1,(start,text.count(start));lo=text.index(start);hi=text.index(end,lo);return text[lo:hi]
def braced(text,start):
 assert text.count(start)==1,(start,text.count(start));lo=text.index(start);begin=text.index('{',lo);level=1;i=begin+1
 while level:
  level+=(text[i]=='{')-(text[i]=='}');i+=1
 return text[lo:i]
expanded=read_source(root/'src/frik_pose_probe.cpp')
rig_types=region(expanded,'struct RigMap {','\n};')+'\n};\n'
pose_types=region(expanded,'struct V3 {','static QuatT28 AbsoluteFromRelative')+braced(expanded,'static QuatT28 AbsoluteFromRelative')+'\n'
parts={
 'arm_types_d47.inc':rig_types+pose_types,
 'fist_mask_d48.inc':braced(expanded,'static unsigned FistHandMaskD48() noexcept')+'\n',
 'arm_math_d47.inc':region(co,'static pc::Vec3 PV(', 'constexpr std::size_t kPoseCapacity6689'),
 'arm_fk_d47.inc':region(an,'static bool Compose6689(', 'static bool Descendant6689(')+region(finger,'static Q4 DesiredHand6689(', 'static std::atomic<float> g_relaxed_middle101')+braced(leg,'static void SetWorldPosition128(')+'\n',
 'arm_reach_d47.inc':region(reach,'    V3 gestureShouldersR38[2]', '    V3 modelIntentGoalsH30[2]'),
 'arm_caller_d47.inc':region(co,'#include "../subsystems/vrik/pose_final_wrist_d52.inl"', '        // The native book/parchment grip'),
 'arm_history_type_d47.inc':braced(co,'struct CoherentMotionHistoryD47')+';\n',
 'arm_stage_d47.inc':region(co,'    auto& historyD47=tx.motionD47;\n    historyD47.ready=', '#ifdef KCD2VR_REACH_VIEW38_H'),
 'arm_publish_d47.inc':braced(co,'    if(pairedFramePublishedR38&&tx.motionD47.ready)')+'\n',
 'arm_restore_d47.inc':braced(co,'    void Restore() noexcept')+'\n',
 'arm_write_d47.inc':region(co,'    tx.committed=pc::commit(', '    if(tx.kickFrame176.active&&(g_kick_cancel176.load(std::memory_order_acquire)!=tx.kickCancel176||\n       g_kick_busy176.load(std::memory_order_acquire)!=tx.kickSerial176||!CombatExperiment175Enabled())){'),
 'arm_ordinary_owner_d47.inc':braced(co,'    if(!tx.acquisitionRequired57){')+'\n',
 'arm_publish_guard_d47.inc':region(co,'    if(!tx.committed||!tx.restored||g_pose_restore_fault6689.load()', '    auto& s=g_pose6689;CommittedFrame6695 packet{};'),
}
# Live include remains live, rooted at the selected source checkout.
parts['arm_caller_d47.inc']=parts['arm_caller_d47.inc'].replace('#include "../subsystems/vrik/pose_arm_solver.inl"','#include "subsystems/vrik/pose_arm_solver.inl"')
parts['arm_caller_d47.inc']=parts['arm_caller_d47.inc'].replace('#include "../subsystems/vrik/pose_final_wrist_d52.inl"','#include "subsystems/vrik/pose_final_wrist_d52.inl"')
# Prove the test boundary is at final publication, never in Begin/temporary write.
begin=co.index('static bool BeginCoherent6689(');publish=co.index('static void PublishCommitted6689(')
history=co.index('    if(pairedFramePublishedR38&&tx.motionD47.ready)')
assert publish<history
late=co.rfind('    if(!CoherentOwnerCurrent57(tx)){rejectPublish169r2(__LINE__);return;}',publish,history)
assert late>publish
parts['arm_late_guard_d47.inc']=co[late:co.index('\n',late)+1]
for name in ('s.elbow[side]=','s.pronation[side]=','s.handoverW178r14[side]=','s.handSmoothH50[side]='):
 assert name not in co[begin:publish],('history leaked into temporary write',name)
assert 'BuildManualArrowPoses64(packet,s.original.data(),s.count,tx.physicalInput,tx.physicalOutput,tx.physicalProfiles[1],s.rest.data(),s.rig.hand_r);' in co
for name,body in parts.items():(a.out/name).write_text('// Generated verbatim production; regenerate with extract_vrik_arm_d47.py.\n'+body)
provenance={'source_sha256':{n:hashlib.sha256(p.read_bytes()).hexdigest() for n,p in paths.items()},'extracted_sha256':{n:hashlib.sha256(b.encode()).hexdigest() for n,b in parts.items()},'live_arm_include':'src/subsystems/vrik/pose_arm_solver.inl','verified_order':['native write stages history','Restore required','publish owner admission','late owner recheck','paired history acceptance'],'limits':'In-memory native buffers and explicit service leaves; no native ABI, skinning palette, stereo renderer or HMD test.'}
(a.out/'vrik_arm_d47_provenance.json').write_text(json.dumps(provenance,indent=2)+'\n')
print(f'D47 extracted {len(parts)} current production blocks; verified final history boundary.')
