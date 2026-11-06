#!/usr/bin/env python3
"""Extract actual XR sampler and timing composition; platform/native leaves stay in tests."""
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parent))
from source_text import read_source as _read_source
import argparse,hashlib,json
ap=argparse.ArgumentParser();ap.add_argument('--source',type=Path);ap.add_argument('--out',type=Path);args=ap.parse_args()
r=args.source or Path(__file__).resolve().parents[1];out=args.out or r/'tests';out.mkdir(parents=True,exist_ok=True)
def read_source(path):return _read_source(path,root=r)
s=read_source(r/'src/xr_manager.cpp');parts=[];meta=[]
def cut(t,a,b):
 i=t.index(a);return t[i:t.index(b,i+len(a))]
def function(t,name):
 start=t.index(name);b=t.index('{',start);d=1;i=b+1
 while d:d+=(t[i]=='{')-(t[i]=='}');i+=1
 return t[start:i],start
for name in ('bool XrManager::SampleHeadPoseAtTime(', 'bool XrManager::SampleLateHeadPose('):
 code,start=function(s,name);parts.append(code)
 meta.append({'source':'src/xr_manager.cpp','function':name,'start_line':s[:start].count('\n')+1,'sha256':hashlib.sha256(code.encode()).hexdigest()})
start='    if(tracking59c6::enabled.load(std::memory_order_acquire)&&!tracking59c6::cameraNow.load(std::memory_order_acquire))'
if start in s:
 block=cut(s,start,'    // NOW mode retains')
else:block='    (void)source59c6;(void)out_head_pose;(void)headFlags59c6;(void)inputFresh51; // Original source has no early predicted-family publication.\n'
parts.append('void XrManager::PublishPredictedForTest(const tracking59c6::Stamp& source59c6,const XrPose& out_head_pose,XrSpaceLocationFlags headFlags59c6,bool inputFresh51){\n'+block+'}\n')
frik=read_source(r/'src/frik_pose_probe.cpp');prod=read_source(r/'src/subsystems/vrik/tracking_producer.inl');reader=read_source(r/'src/subsystems/vrik/tracking_reader.inl');cam=read_source(r/'src/camera_hook.cpp');physical=read_source(r/'src/subsystems/base/pose_physical_executor.inl')
packet=cut(prod,'    TrackingSnapshot ready{};','    const int mode=g_mode.load(')
pipeline=cut(frik,'struct V3 {','static V3 Add(')+reader
pipeline+='\nstatic void PublishActualPacket(TrackingPose head,bool hv,TrackingPose left,bool lv,TrackingPose right,bool rv,std::uint64_t poseNow,std::uint64_t sourceNs,std::int64_t poseTime){\n const bool allowed=true,poseAllowed=true;const auto sourceEpoch=tracking59c6::epoch.load();\n'+packet+'}\n'
pipeline+=cut(cam,'static void CopyCurrentPoseAndOrigin(','// Keep RAII out of')
pipeline+='struct CameraMini {std::mutex s_pose_mutex;XrPose s_current_pose{},s_origin_pose{};std::atomic<float>s_current_scale{1};std::atomic<std::uint64_t>s_current_pose_frame_index{0};\nvoid Publish(const XrPose& pose,float scale,std::uint64_t frame_index){\n'+cut(cam,'    std::lock_guard<std::mutex> lock(s_pose_mutex);\n    s_current_pose = pose;','    const bool had_origin=')+'}\n XrPose Consume(){XrPose p{},o{};float scale=1;CopyCurrentPoseAndOrigin(s_pose_mutex,s_current_pose,s_origin_pose,s_current_scale,p,o,scale);return p;} };\n'
pipeline+='static ph::Input Physical(const TrackingSnapshot& ts){ph::Input pi{};struct{std::uint64_t epoch=11;}tx;\n'+cut(physical,'pi.timeNs=ts.timeNs;','\n        // Accepted physical Grip draw')+'\nconst bool valid[2]{ts.right_valid,ts.left_valid};for(int side=0;side<2;++side){const V3 rawP=side?ts.left:ts.right;pi.hands[side].p={rawP.x-ts.head.x,0,0};pi.intentHands[side]=pi.hands[side];\n'+cut(physical,'pi.intentTracked[side]=valid[side]','\n            // A grace-held coordinate')+'\n'+cut(physical,'            pi.poseTime[side]=','\n            pi.shoulders[side]=')+'\n pi.tracked[side]=valid[side];pi.grip[side]=1;pi.shoulders[side]={side?-.2:.2,0,0};}pi.attacks=false;pi.contact=false;pi.dynamics=false;return pi;}\n'
payload='// Verbatim source bodies/statements; test-only class/platform wrappers are identified below.\n#if defined(KCD2VR_HF1_PIPELINE_TEST)\n'+pipeline+'\n#else\n'+'\n\n'.join(parts)+'\n#endif\n'
(out/'generated_late_pose59c6.inc').write_text(payload)
sources=['src/xr_manager.cpp','src/camera_hook.cpp','src/frik_pose_probe.cpp','src/subsystems/vrik/tracking_reader.inl','src/subsystems/vrik/tracking_producer.inl','src/subsystems/base/pose_physical_executor.inl','src/subsystems/hud_ui_controls/runtime/xr_input.cpp','src/subsystems/hud_ui_controls/runtime/xr_input.h']
meta.append({'sources':{n:hashlib.sha256((r/n).read_bytes()).hexdigest()for n in sources},'early_predicted_body_present':start in s,'output_sha256':hashlib.sha256(payload.encode()).hexdigest(),'scope':'Actual complete samplers, literal early publication block, packet transaction/reader/camera publication prefix and physical metadata/held/target statements; OpenXR/native/context/render scheduling leaves controlled.'})
(out/'tracking59c6_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')
