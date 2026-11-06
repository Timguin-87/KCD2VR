#!/usr/bin/env python3
"""Execute the production self-contact caller, not a rewritten eligibility gate.

Native geometry reads/publication mailboxes are explicit fixture boundaries.
The actual self-contact prefix and (for D38) post-world DS1 block are extracted
from the selected source. D38 is a useful negative control for world ordering,
collision-off and native-use/hand-over admission. This is not a native game test.
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
output = args.output or Path(tempfile.mkdtemp(prefix="kcd2vr_pair_adapter_d39_"))
output.mkdir(parents=True, exist_ok=True)
body = (source / "src/subsystems/physics/pose_free_hand_contact.inl").read_text()
prefix = body[:body.index("    if(physicalMode")]
coherent = (source / "src/vrik/coherent_pose.inl").read_text()
old_marker = "    // DS1 (DeepSeek, D35): empty hands must not pass through each other."
post_world = coherent[coherent.index(old_marker):coherent.index("    V3 stagedBend[2]")] if old_marker in coherent else ""
header_root = Path(__file__).resolve().parents[1]
code = r'''
#include "vrik/hand_shape59c7f.h"
#include "vrik/upper_body.h"
#include "vrik/body_contact164.h"
#include "vrik/hand_separation_ds1.h"
PAIR_STATE_HEADER
#include "data/finger_pair_i10_r29_d39.h"
#include <atomic>
#include <cstdio>
namespace pc=kcd2vr::pose_core;namespace hp=kcd2vr::hand_pair_contact_d39;
namespace ph {std::atomic<bool> handSeparationDs1{true};}
namespace kcd2vr::tracking59c6 {std::atomic<std::uint64_t> epoch{5};}
using V3=pc::Vec3;using Q4=pc::Quat;
static V3 PV(V3 p){return p;}static V3 VP(V3 p){return p;}static Q4 PQ(Q4 p){return p;}
static V3 Add(V3 a,V3 b){return a+b;}static V3 FromT(V3 p){return p;}
struct Tracking {bool head_valid=true,head_held=false,right_held=false,left_held=false;
 std::uint64_t source_epoch=5,sequence=76354,tick=1096862234,timeNs=1096862261499000;
 float left_grip=0,right_grip=0,left_trigger=0,right_trigger=0;};
struct Tx {hp::State handPairD39{};bool physicalActive=false,reloadWorkingHandXR34=false,bowContext169r2=false,acquisitionRequired57=false;
 std::uint64_t epoch=7,policyGeneration57=20,control51=8,nativeHeldAdmissionT4=2,doorSerial66[2]{};
 bool fingerTrialH7[2]{};float fingerOpenBiasH7[2]{};
 struct {bool owned[2]{};} physicalOutput;
 struct {struct {bool support=false;} weapon[2];} physicalState;
};
struct State {void* character=reinterpret_cast<void*>(0x2432B186720);void* owner=reinterpret_cast<void*>(0x1234);
 hp::State handPairD39{};float handoverW178r14[2]{};struct {std::uint64_t serial=0;} pendingUse55;
 V3 work[2]{{100,100,100},{100,100,101}};
};
struct {int pelvis=0,spine3=1;} rig;
struct FingerContactGeometry103 {pc::FingerCurve103 curve[5]{};float input[5]{};Q4 orientation{};};
struct ModelWorld6692 {float scale=1;};
static bool geometryAvailable=true,worldAvailable=true,actionOwner=false,guard=false;
static std::uint64_t clockNow=1096862234;
static auto shapes=pair_i10_d39::samples()[0];
static bool (*g_wristOwnedByController59c12)(int) noexcept=+[](int) noexcept{return actionOwner;};
static std::uint64_t GetTickCount64(){return clockNow;}
static Q4 DesiredHand6689(State&,int,Q4,Q4){return {};}
static bool PalmGeometry103(State&,int,Q4,float,float,int,V3 (&)[6],FingerContactGeometry103&,float,bool){return false;}
static bool SimplePalmGeometryR24(State&,int side,Q4,V3 (&out)[6],kcd2vr::hand_shape59c7f::Shape* shape){
 for(auto& p:out)p={};if(shape)*shape=shapes.hand[side];return geometryAvailable;
}
static bool ReadModelWorld6692(void*,ModelWorld6692&){return worldAvailable;}
static bool QualifiedCombatFistH34(Tracking&,std::uint64_t,int,std::uint64_t){return guard;}
static int checks=0,failures=0;
static void ck(bool b,const char* m){++checks;if(!b){++failures;std::printf("FAIL %s\n",m);}}
enum Mode {Normal,CollisionOff,NativeCapture,NativeUse,Held,Door,Passenger,Reading,Reload,Bow,Handover,Acquisition,Pending,ActionOwner,HeldTracking,WrongTracking,Disabled,Performance,WorldWall,UnknownD56,UnknownClaimedD56,UnknownOtherReasonD56};
struct Result {V3 goal[2]{};hp::State staged{};};
static Result caller(Mode mode,State& s,Tracking ts,bool near=false){
 Tx tx{};const bool collisionMode=mode!=CollisionOff,nativeFingerCapture=mode==NativeCapture;
 const bool preciseContactModeR24=false,perfContactModeDs2=mode==Performance,readingVisual32=mode==Reading;
 const bool valid[2]{true,true};bool freeContact164R1[2]{true,true};unsigned contactYield164R1[2]{};bool otherOwnerH50[2]{};
 bool propHeld5916c1[2]{},presenceOwnedH24[2]{},doorPose66[2]{},doorPoseH40[2]{},nativePassengerHand46[2]{};
 std::uint64_t heldFingerKey[2]{};void* character=s.character;const auto contactNow164R1=ts.tick;clockNow=ts.tick;
 const std::uint64_t contactControl=4;const Q4 frame{};const Q4 visQH50[2]{};
 const V3 gestureShouldersR38[2]{{100.2,100,101},{99.8,100,101}};
 V3 goals[2]{near?V3{.04,0,1.3}:shapes.requested[0],near?V3{0,0,1.3}:shapes.requested[1]};
 geometryAvailable=!near;worldAvailable=true;actionOwner=mode==ActionOwner;guard=false;ph::handSeparationDs1=mode!=Disabled;
 s.handoverW178r14[0]=mode==Handover?.5f:0;s.pendingUse55.serial=mode==Pending?23:0;
 tx.acquisitionRequired57=mode==Acquisition;tx.reloadWorkingHandXR34=mode==Reload;tx.bowContext169r2=mode==Bow;
 if(mode==NativeUse)freeContact164R1[0]=false;
 if(mode==UnknownD56||mode==UnknownClaimedD56||mode==UnknownOtherReasonD56){
  freeContact164R1[0]=false;contactYield164R1[0]=mode==UnknownOtherReasonD56?17u:16u;otherOwnerH50[0]=mode==UnknownClaimedD56;
 }
 if(mode==Held){freeContact164R1[0]=false;heldFingerKey[0]=44;}
 if(mode==Door){freeContact164R1[0]=false;doorPose66[0]=true;tx.doorSerial66[0]=7;}
 if(mode==Passenger){freeContact164R1[0]=false;nativePassengerHand46[0]=true;}
 if(mode==HeldTracking)ts.right_held=true;if(mode==WrongTracking)++ts.source_epoch;
ACTUAL_SELF_CONTACT_PREFIX
 // Explicit native query boundary: a measured wall keeps the right wrist at
 // x <= 4 cm. Any correction after this boundary must preserve that result.
 if(mode==WorldWall)goals[0].x=std::min(goals[0].x,.04);
ACTUAL_POST_WORLD_DS1
 return {{goals[0],goals[1]},tx.handPairD39};
}
int main(){
 State s{};Tracking t{};
 for(Mode mode:{CollisionOff,NativeCapture,NativeUse,Held,Door,Passenger,Reading,Reload,Bow,Handover,Acquisition,Pending,ActionOwner,HeldTracking,WrongTracking,Disabled,UnknownClaimedD56,UnknownOtherReasonD56}){
  const auto r=caller(mode,s,t,true);
  const int before=failures;
  ck(pc::length(r.goal[0]-V3{.04,0,1.3})<1e-10&&pc::length(r.goal[1]-V3{0,0,1.3})<1e-10,"actual caller leaves collision-disabled/claimed/held/grace endpoints untouched");
  ck(!r.staged.touching,"ineligible actual caller cannot publish continuity history");
  if(failures!=before)std::printf("  refused-mode case=%d (see Mode enum)\n",int(mode));
 }
 const auto near=caller(Normal,s,t,true);ck(near.goal[0].x>.04&&near.goal[1].x<0,"coarse fallback remains active for qualified free wrists");
 const auto wall=caller(WorldWall,s,t,true);ck(wall.goal[0].x<=.04,"pair correction precedes measured wall and cannot overwrite checked endpoint");
 const auto perf=caller(Performance,s,t,true);ck(pc::length(perf.goal[0]-V3{.04,0,1.3})<1e-10,"missing-shape fallback respects performance mode");
 shapes=pair_i10_d39::samples()[0];const auto a=caller(Normal,s,t);
 hp::commit(s.handPairD39,a.staged,true,true);
 shapes=pair_i10_d39::samples()[1];t.sequence=76358;t.tick=1096862250;t.timeNs=1096862272508700;
 const auto b=caller(UnknownD56,s,t);const auto delta=pc::length(b.goal[0]-a.goal[0]);
 ck(delta<.005,"actual free-hand caller preserves recorded pair continuity across EMPTY to UNKNOWN");
 hp::commit(s.handPairD39,b.staged,true,true);
 ++t.sequence;t.tick+=10;t.timeNs+=10000000;
 const auto emptyAgain=caller(Normal,s,t);
 ck(pc::length(emptyAgain.goal[0]-b.goal[0])<.005,"UNKNOWN to EMPTY cannot reintroduce a pair-separation jump with stationary current geometry");
 s.work[0]={0,0,1};s.work[1]={0,0,1.6};
 const auto torsoKnown=caller(Normal,s,t),torsoUnknown=caller(UnknownD56,s,t);
 ck(pc::length(torsoKnown.goal[0]-torsoUnknown.goal[0])<1e-10&&pc::length(torsoKnown.goal[1]-torsoUnknown.goal[1])<1e-10,
    "current torso plus sampled pair geometry uses the same presentation endpoints during occupancy-only uncertainty");
 const auto stateBefore=s.handPairD39;const auto failedAttempt=caller(Normal,s,t);(void)failedAttempt;
 ck(s.handPairD39.sample==stateBefore.sample&&pc::length(s.handPairD39.normal-stateBefore.normal)<1e-12,"staging caller does not mutate committed owner state");
 std::printf("actual hand-pair adapter: %d checks, %d failures; recorded right step %.6f mm\n",checks,failures,delta*1000);
 return failures?1:0;
}
'''.replace("ACTUAL_SELF_CONTACT_PREFIX", prefix).replace("ACTUAL_POST_WORLD_DS1", post_world)
if "pose_hand_pair_d39.inl" in prefix:
    code = code.replace("PAIR_STATE_HEADER", '#include "vrik/hand_pair_contact_d39.h"')
else:
    # D38 has no pair history. Empty fixture storage lets the same externally
    # observable assertions run without grafting D39 math into its old caller.
    code = code.replace("PAIR_STATE_HEADER", '''namespace kcd2vr::hand_pair_contact_d39 {
    struct State {std::uint64_t sample=0;pose_core::Vec3 normal{};bool touching=false;};
    inline void commit(State& a,const State& b,bool p,bool c){if(p&&c)a=b;}
    }''')
fixture = output / "production_hand_pair_adapter_d39.cpp"
fixture.write_text(code)
binary = output / "production_hand_pair_adapter_d39"
# The new continuity header is only fixture storage for the baseline run. Its
# old extracted caller still calls the baseline stateless kernel/DS1 verbatim.
subprocess.run(["c++", "-std=c++17", "-O2", "-I", str(source / "src"), "-I", str(header_root / "src"),
                "-I", str(source / "src/subsystems/physics"), "-I", str(header_root / "tests"), str(fixture), "-o", str(binary)], check=True)
result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout, end="")
(output / "hand_pair_adapter_result.txt").write_text(result.stdout)
raise SystemExit(result.returncode)
