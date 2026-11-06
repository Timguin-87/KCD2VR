#!/usr/bin/env python3
"""Replay the actual ordinary-frame admission with asymmetric controller loss.

This extracts the production ordinaryFrame51/ordinaryFresh51 expressions and
uses the real tracked_frame51::advance math. Native memory/context leaves
are explicit fixture values; no game or headset is run.
"""
from pathlib import Path
import argparse
import subprocess
import tempfile

p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
p.add_argument("--output", type=Path)
args = p.parse_args()
source = args.source.resolve()
output = args.output or Path(tempfile.mkdtemp(prefix="kcd2vr_head_frame_d39_"))
output.mkdir(parents=True, exist_ok=True)
text = (source / "src/subsystems/vrik/pose_frame_foundation.inl").read_text()
def declaration(name):
    start = text.index("    const bool " + name + "=")
    return text[start:text.index(";", start)+1]
reach = (source / "src/subsystems/vrik/pose_reach_input.inl").read_text()
physical = (source / "src/subsystems/base/pose_physical_executor.inl").read_text()
def assignment(body, name):
    start = body.index(name + "=")
    return body[start:body.index(";", start)+1]
code = r'''
#include "vrik/tracked_frame51.h"
#include <atomic>
#include <cstdio>
namespace pc=kcd2vr::pose_core;namespace tf=kcd2vr::tracked_frame51;
static std::atomic<bool> g_gameplay_allowed{true},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false},g_ctx_reading{false};
namespace animation_filter {std::atomic<bool> nativeBodyOwned112{false};bool kick=false;bool NativeKickActive175(){return kick;}}
struct ModelWorld6692 {};static bool worldOk=true;
static bool ReadModelWorld6692(void*,ModelWorld6692&){return worldOk;}
struct Tracking {bool head_valid=true,head_held=false,right_valid=true,left_valid=true,right_held=false,left_held=false;};
struct Admission {bool frame=false,neutral=false,prop[2]{},physical[2]{},intent[2]{};};
static Admission admit(Tracking ts,bool full=true,bool readingVisual32=false,bool legsReady=true){
 struct {bool legsReady128=true;} s;s.legsReady128=legsReady;
 void* character=reinterpret_cast<void*>(0x1234);ModelWorld6692 frameWorld51{};
FRAME_GATE
NEUTRAL_GATE
 const bool valid[2]{ts.right_valid,ts.left_valid};
 struct {bool propRequestedValid5916c1[2]{};} tx;
 struct {bool tracked[2]{},intentTracked[2]{};} pi;
 for(int side=0;side<2;++side){PROP_ASSIGNMENT PHYSICAL_ASSIGNMENT INTENT_ASSIGNMENT}
 return {ordinaryFrame51,ordinaryFresh51,{tx.propRequestedValid5916c1[0],tx.propRequestedValid5916c1[1]},
  {pi.tracked[0],pi.tracked[1]},{pi.intentTracked[0],pi.intentTracked[1]}};
}
static int checks=0,failures=0;
static void ck(bool b,const char* s){++checks;if(!b){++failures;if(failures<8)std::printf("FAIL %s\n",s);}}
int main(){
 for(unsigned bits=0;bits<64;++bits){Tracking t{bool(bits&1),bool(bits&2),bool(bits&4),bool(bits&8),bool(bits&16),bool(bits&32)};
  const auto a=admit(t);const bool expected=t.head_valid&&!t.head_held&&(t.right_valid||t.left_valid);
  ck(a.frame==expected,"ordinary body frame requires current head and at least one usable hand");
  ck(a.neutral==(expected&&t.right_valid&&t.left_valid&&!t.right_held&&!t.left_held),"neutral anatomy/heading requires BOTH fresh controllers");
  for(int side=0;side<2;++side)if(!(side?t.left_valid:t.right_valid))
   ck(!a.prop[side]&&!a.physical[side]&&!a.intent[side],"actual producers give invalid hand no prop, physical tracking or action intent authority");}
 for(int context=0;context<10;++context){Tracking t{};t.right_valid=false;
  g_gameplay_allowed=context!=0;g_ctx_dialogue=context==1;g_ctx_cutscene=context==2;g_ctx_scripted=context==3;g_ctx_reading=context==4;
  animation_filter::nativeBodyOwned112=context==5;animation_filter::kick=context==6;worldOk=context!=7;
  ck(!admit(t,context!=8,context==9).frame,"native/UI/context/world/full-body refusal remains authoritative");}
 g_gameplay_allowed=true;g_ctx_dialogue=false;g_ctx_cutscene=false;g_ctx_scripted=false;g_ctx_reading=false;
 animation_filter::nativeBodyOwned112=false;animation_filter::kick=false;worldOk=true;
 ck(!admit({},true,false,false).frame,"missing validated legs does not synthesize a full body frame");
 tf::State state{};tf::Frame frame{};tf::Input i{};i.control=8;i.scale=1;i.stamp={7,1,1000,0x1234};
 i.restHead={0,0,1.6};i.restPelvis={0,0,1.};i.nativeHead=i.restHead;i.nativePelvis=i.restPelvis;i.calibratedHead=i.restHead;i.hmdHeight=true;
 double drift=0;unsigned failuresDuringLoss=0;
 for(unsigned n=0;n<200;++n){Tracking t{};if(n>0)t.right_valid=false;
  const auto a=admit(t);i.allowed=a.frame;i.eyeNeutral=a.neutral;i.stamp.sample=1+n;i.stamp.tick=1000+16*n;i.timeNs=i.stamp.tick*1000000ull;
  // Bow/arm animation head/pelvis Z can move while the healthy HMD and current
  // model-world root are unchanged. No solved arm/head input enters this base.
  i.nativeHead.z=1.6+.05*std::sin(double(n));i.nativePelvis.z=1.+.04*std::cos(double(n));
  const bool current=tf::advance(state,i,frame);if(!current)++failuresDuringLoss;
  if(current)drift=std::max(drift,pc::length(frame.anchor-i.restHead));
 }
 ck(failuresDuringLoss==0,"more than three seconds of one-hand loss produces fresh body frames, no bounded-hold expiry");
 ck(drift<1e-10,"native animated head/height cannot move HMD-height-owned base");
 std::printf("actual head-frame admission: %d checks, %d failures; asymmetric-loss missed frames=%u, base drift=%.9f mm\n",checks,failures,failuresDuringLoss,drift*1000);
 return failures?1:0;
}
'''.replace("FRAME_GATE", declaration("ordinaryFrame51")).replace("NEUTRAL_GATE", declaration("ordinaryFresh51"))
code = code.replace("PROP_ASSIGNMENT", assignment(reach, "tx.propRequestedValid5916c1[side]"))
code = code.replace("PHYSICAL_ASSIGNMENT", assignment(physical, "pi.tracked[side]"))
code = code.replace("INTENT_ASSIGNMENT", assignment(physical, "pi.intentTracked[side]"))
fixture = output / "production_head_frame_admission_d39.cpp"
fixture.write_text(code)
binary = output / "production_head_frame_admission_d39"
subprocess.run(["c++", "-std=c++17", "-O2", "-I", str(source / "src"), str(fixture), "-o", str(binary)], check=True)
result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout, end="")
(output / "head_frame_admission_result.txt").write_text(result.stdout)
raise SystemExit(result.returncode)
