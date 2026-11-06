// Production-connected host test: the actual finisher header/inl are included.
// The runner extracts unchanged production quaternion/pose helpers and types.
// Only native pose access, clock, diagnostics and context publications are leaves.
#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#define __fastcall
namespace kcd2vr::dialogue_fingers_d78 {struct VisualInput{};}
#include "finisher_production_helpers.inc"
#include "vrik/finisher_upper_d83.h"

namespace kcd2vr::tracking59c6 {std::atomic<std::uint64_t> epoch{1};}
namespace diagnostics::lab {bool nativeReference=false;bool NativeReference(){return nativeReference;}}
namespace diagnostic_trace {std::vector<std::string> messages;void Write(const char* s){messages.emplace_back(s);}}
std::atomic<std::uint64_t> g_pose_epoch6689{7},g_pose_policy_generation57{2},g_player_lifetime_d44{1};
std::atomic<bool> g_pose_restore_fault6689{false},g_committed_pose_valid6689{true},g_gameplay_allowed{true};
std::atomic<bool> g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_reading{false},g_ctx_scripted{false};
std::atomic<int> g_cal_state{2},g_census_mode{7},g_weapon_control_mode{0};
std::atomic<float> g_cal_frame_qx{0},g_cal_frame_qy{0},g_cal_frame_qz{0},g_cal_frame_qw{1};
constexpr std::size_t kPoseCapacity6689=1024;
struct CommittedFixture {bool ready=true;void* character{};RigMap rig{};std::uint32_t count{};std::array<QuatT28,kPoseCapacity6689> work{};} g_pose6689;
constexpr int kCount=21;
struct PoseFixture {std::array<QuatT28,kCount> joints{};} nativePose;
std::array<QuatT28,kCount> original;
std::array<int,kCount> parents;
std::uint64_t fixtureClock=1000;
std::uint64_t GetTickCount64(){return fixtureClock;}
int nativeCharacter1=0,nativeCharacter2=0,nativeOwner=0;
void* currentCharacter=&nativeCharacter1;
RigMap rig;
TrackingSnapshot packet;
std::function<void()> beforeSetter;
std::function<void(int)> duringWrite;
bool readOK=true,setterOK=true;
int overlayCalls=0,restoreCalls=0,failOverlayAt=0,failRestoreJoint=-1;
std::array<int,kCount> restorePerJoint{};
const QuatT28* GetPose(void* p,int j){return &static_cast<PoseFixture*>(p)->joints.at(j);}
void SetPose(void* p,int j,const QuatT28* q){static_cast<PoseFixture*>(p)->joints.at(j)=*q;}
bool ReadParentDirect(void*,int j,int& p){if(j<0||j>=kCount)return false;p=parents[j];return true;}
bool ReadNativeBuffer6689(void* p,PoseGetFn get,std::uint32_t n,QuatT28* out){
    if(!readOK)return false;for(std::uint32_t j=0;j<n;++j)out[j]=*get(p,int(j));return true;
}
bool ValidateBuffer1Setter(void*,PoseSetFn& setter){if(beforeSetter)beforeSetter();setter=SetPose;return setterOK;}
bool WriteQuat(PoseSetFn setter,void* p,int j,const QuatT28& q){
    const bool restoring=std::memcmp(&q,&original[j],sizeof(q))==0;
    if(restoring){++restoreCalls;++restorePerJoint[j];if(j==failRestoreJoint)return false;}
    else ++overlayCalls;
    setter(p,j,&q); // failed overlay call is deliberately allowed to touch memory
    if(!restoring&&duringWrite)duringWrite(j);
    return restoring||failOverlayAt!=overlayCalls;
}
unsigned retiredRestoreFramesR96=0;
void RetireTrackedFrame51(){++retiredRestoreFramesR96;}
bool PlayerIdentityAccepts37(const void* character,std::uint64_t){return character==currentCharacter;}
#include "pose_restore_shared_r96.inc"
#include "vrik/finisher_upper_d83.inl"

void Check(bool value,const char* text){if(!value)throw std::runtime_error(text);}
bool Exact(){return std::memcmp(nativePose.joints.data(),original.data(),sizeof(original))==0;}
bool SameJoint(int j){return std::memcmp(&nativePose.joints[j],&original[j],sizeof(QuatT28))==0;}
float Deg(float radians){return radians*180.f/3.14159265358979323846f;}
float Yaw(const QuatT28& p){return Deg(2.f*std::atan2(p.q[2],p.q[3]));}
Q4 XrYaw(float deg){const float h=deg*.5f*3.14159265358979323846f/180.f;return {0,std::sin(h),0,std::cos(h)};}
template<class F> void LatchWith(F fn,std::uint64_t epoch,std::uint64_t policy){
    if constexpr(std::is_invocable_v<F,void*,const TrackingSnapshot&,std::uint64_t,std::uint64_t>)
        fn(currentCharacter,packet,epoch,policy);
    else fn(currentCharacter,packet); // exact immutable Claude D83 negative control
}
void Latch(std::uint64_t epoch=7,std::uint64_t policy=2){LatchWith(FinisherLatchCommittedD83,epoch,policy);}
constexpr const char* kAlias="combat_rg_sz1_dz2_masterstrikedeath_lngsw_master";
void Begin(FinisherWriteD83& w,std::uint64_t tick=1300,const char* alias=kAlias,bool have=true,unsigned side=fu83::Player){
    fixtureClock=tick;packet.tick=tick;
    FinisherBeginD83(w,side,alias,&nativePose,&nativeOwner,currentCharacter,rig,true,GetPose,kCount,packet,have,tick,true);
}
void Reset(){
    g_pose_epoch6689=7;g_pose_policy_generation57=2;kcd2vr::tracking59c6::epoch=1;g_player_lifetime_d44=1;
#if R96_RESTORE_DEBT_PRESENT
    g_pose_restore_debt_r96={};retiredRestoreFramesR96=0;
#endif
    g_pose_restore_fault6689=false;g_committed_pose_valid6689=true;g_gameplay_allowed=true;
    g_ctx_menu=false;g_ctx_dialogue=false;g_ctx_cutscene=false;g_ctx_reading=false;g_ctx_scripted=false;
    g_cal_state=2;g_census_mode=7;g_weapon_control_mode=0;diagnostics::lab::nativeReference=false;
    g_cal_frame_qx=0;g_cal_frame_qy=0;g_cal_frame_qz=0;g_cal_frame_qw=1;
    fu83::player=true;fu83::npc=true;
    g_finisher_fault_d83=false;g_finisher_latch_d83={};g_finisher_rig_d83={};g_finisher_state_d83={};
    g_finisher_cur_d83={};g_finisher_local_d83={};g_finisher_model_d83={};
    beforeSetter={};duringWrite={};readOK=true;setterOK=true;
    overlayCalls=restoreCalls=failOverlayAt=0;failRestoreJoint=-1;restorePerJoint={};
    diagnostic_trace::messages.clear();fixtureClock=1000;currentCharacter=&nativeCharacter1;
    // Head descendants 16/17; torso socket 18 and its child 19 must stay native.
    // Bone 20 is an intermediate neck connector, exercising parent-first paths.
    parents={-1,0,1,2,20,4,3,6,7,8,9,3,11,12,13,14,5,5,3,18,3};
    rig=RigMap{};rig.pelvis=0;rig.spine=1;rig.spine1=1;rig.spine2=2;rig.spine3=3;rig.neck=4;rig.head=5;
    rig.clav_l=6;rig.upper_l=7;rig.fore_l=8;rig.hand_l=9;rig.item_l=10;
    rig.clav_r=11;rig.upper_r=12;rig.fore_r=13;rig.hand_r=14;rig.item_r=15;
    for(int j=0;j<kCount;++j){nativePose.joints[j]={{0,0,0,1},{float(j%3)*.13f,float(j%5)*.07f,float(j)*.1f}};}
    original=nativePose.joints;
    g_pose6689={};g_pose6689.character=currentCharacter;g_pose6689.count=kCount;g_pose6689.rig=rig;
    std::copy(original.begin(),original.end(),g_pose6689.work.begin());
    packet={};packet.source_epoch=1;packet.tick=1000;packet.head_valid=true;packet.allowed=true;packet.hq={};
    Latch();packet.hq=XrYaw(60);
}
void Warm(){
    for(std::uint64_t t:{1000u,1100u,1200u}){FinisherWriteD83 w;Begin(w,t);if(t==1200)Check(w.wrote,"qualified overlay never became active");FinisherRestoreD83(w);Check(Exact(),"warm-up did not restore native pose");}
    overlayCalls=restoreCalls=0;restorePerJoint={};
}
void Refused(){FinisherWriteD83 w;Begin(w);Check(!w.wrote&&Exact()&&overlayCalls==0,"invalid context/source still wrote native pose");}
struct Test {const char* name;std::function<void()> run;};
int main(){
    std::vector<Test> tests={
      {"recorded_aliases",[]{Check(fu83::Classify(kAlias,0)==fu83::Player,"recorded player masterstrike rejected");Check(fu83::Classify("combat_combo_c11_right_shsw_lngsw_slave",0)==fu83::Npc,"recorded NPC combo rejected");}},
      {"protected_aliases",[]{for(const char* s:{"combat_clinch_finish_master","combat_stealth_kill_master","combat_masterstrikedeath_slave","combat_finish_unknown","idle_finish_master"})Check(fu83::Classify(s,0)==fu83::None,"unrelated/death clip admitted");}},
      {"victim_execution_stays_native",[]{Check(fu83::Classify("combat_execution_slave",0)==fu83::None,"Henry execution admitted");Check(fu83::Classify("combat_finish_kill_slave",0)==fu83::None,"Henry kill admitted");}},
      {"clamps_and_cumulative_shares",[]{Warm();FinisherWriteD83 w;Begin(w);Check(w.wrote,"no overlay");const int chain[]{2,3,4,5};const float expected[]{4.5f,11.25f,27.f,45.f};for(int n=0;n<4;++n)Check(std::fabs(Yaw(nativePose.joints[chain[n]])-expected[n])<.002f,"incorrect cumulative head correction");FinisherRestoreD83(w);Check(Exact(),"not byte-exact after restore");}},
      {"quaternion_swing_limit",[]{const auto q=fu83::Clamp({std::sin(.7f),0,0,std::cos(.7f)});Check(std::fabs(Deg(2*std::atan2(q.x,q.w))-30.f)<.002f,"swing clamp incorrect");}},
      {"native_arms_hands_weapon_lower_body",[]{Warm();FinisherWriteD83 w;Begin(w);for(int j:{0,1,6,7,8,9,10,11,12,13,14,15})Check(SameJoint(j),"native-owned joint changed");FinisherRestoreD83(w);}},
      {"torso_attachment_native",[]{Warm();FinisherWriteD83 w;Begin(w);Check(SameJoint(18)&&SameJoint(19),"torso attachment was rotated as anatomy");FinisherRestoreD83(w);}},
      {"face_and_connector_follow_chain",[]{Warm();FinisherWriteD83 w;Begin(w);Check(!SameJoint(16)&&!SameJoint(17)&&!SameJoint(20),"anatomical descendants did not follow");auto rel=RelativeFromAbsolute(nativePose.joints[5],nativePose.joints[16]);auto want=RelativeFromAbsolute(original[5],original[16]);for(int i=0;i<3;++i)Check(std::fabs(rel.t[i]-want.t[i])<1e-5f,"face local position changed");FinisherRestoreD83(w);}},
      {"restore_idempotent",[]{Warm();FinisherWriteD83 w;Begin(w);FinisherRestoreD83(w);const int n=restoreCalls;FinisherRestoreD83(w);Check(Exact()&&restoreCalls==n,"second restore wrote again");}},
      {"tracking_loss_fades_to_native",[]{Warm();packet.head_valid=false;{FinisherWriteD83 w;Begin(w,1300);Check(w.wrote&&g_finisher_state_d83.weight>0&&g_finisher_state_d83.weight<1,"tracking loss did not fade");FinisherRestoreD83(w);}{FinisherWriteD83 w;Begin(w,1400);Check(!w.wrote&&Exact(),"tracking loss retained overlay");}}},
      {"source_reset_retains_pose_epoch_but_rejects_latch",[]{Warm();kcd2vr::tracking59c6::epoch=2;packet.source_epoch=2;Refused();}},
      {"packet_source_mismatch",[]{Warm();packet.source_epoch=2;Refused();}},
      {"shared_restore_fault_blocks_overlay",[]{Warm();g_pose_restore_fault6689=true;Refused();}},
      {"menu_blocks_overlay",[]{Warm();g_ctx_menu=true;Refused();}},
      {"dialogue_blocks_overlay",[]{Warm();g_ctx_dialogue=true;Refused();}},
      {"cutscene_blocks_overlay",[]{Warm();g_ctx_cutscene=true;Refused();}},
      {"reading_blocks_overlay",[]{Warm();g_ctx_reading=true;Refused();}},
      {"scripted_blocks_overlay",[]{Warm();g_ctx_scripted=true;Refused();}},
      {"gameplay_gate_blocks_overlay",[]{Warm();g_gameplay_allowed=false;Refused();}},
      {"snapshot_permission_blocks_overlay",[]{Warm();packet.allowed=false;Refused();}},
      {"native_reference_blocks_overlay",[]{Warm();diagnostics::lab::nativeReference=true;Refused();}},
      {"native_weapon_mode_blocks_overlay",[]{Warm();g_weapon_control_mode=2;Refused();}},
      {"disabled_pose_mode_blocks_overlay",[]{Warm();g_census_mode=0;Refused();}},
      {"policy_in_flight_blocks_overlay",[]{Warm();g_pose_policy_generation57=3;Refused();}},
      {"setter_qualification_source_reset",[]{Warm();beforeSetter=[]{kcd2vr::tracking59c6::epoch=2;};Refused();}},
      {"setter_qualification_policy_change",[]{Warm();beforeSetter=[]{g_pose_policy_generation57=4;};Refused();}},
      {"policy_change_during_write_rolls_back",[]{Warm();duringWrite=[](int){g_pose_policy_generation57=4;};FinisherWriteD83 w;Begin(w);Check(!w.wrote&&Exact()&&overlayCalls>0,"retired policy left overlay installed");}},
      {"failed_partial_write_restores_attempted_joint",[]{Warm();failOverlayAt=3;FinisherWriteD83 w;Begin(w);Check(!w.wrote&&Exact()&&g_finisher_fault_d83,"failed partially writing leaf was not restored");Check(restoreCalls==3,"restored joints that were never attempted");}},
      {"failed_restore_invalidates_shared_publication",[]{Warm();FinisherWriteD83 w;Begin(w);failRestoreJoint=4;FinisherRestoreD83(w);Check(!w.wrote&&g_finisher_fault_d83&&g_pose_restore_fault6689&&!g_committed_pose_valid6689,"restore failure remained private to overlay");}},
#if R96_RESTORE_DEBT_PRESENT
      {"finisher_records_exact_shared_debt",[]{Warm();FinisherWriteD83 w;Begin(w);failRestoreJoint=4;FinisherRestoreD83(w);Check(g_pose_restore_debt_r96.active&&g_pose_restore_debt_r96.failed[4],"finisher failed joint omitted from debt");Check(g_pose_restore_debt_r96.owner.character==reinterpret_cast<std::uintptr_t>(currentCharacter)&&g_pose_restore_debt_r96.owner.pose==reinterpret_cast<std::uintptr_t>(&nativePose),"finisher debt owner differs from actual native writer");Check(std::memcmp(&g_pose_restore_debt_r96.original[4],&original[4],sizeof(QuatT28))==0,"finisher forgot exact native original");}},
      {"finisher_native_readback_reopens_shared_body_only",[]{Warm();FinisherWriteD83 w;Begin(w);failRestoreJoint=4;FinisherRestoreD83(w);nativePose.joints[4]=original[4];const auto n=restoreCalls;Check(ObservePoseRestoreRecoveryR96(&nativeOwner,currentCharacter,&nativePose,GetPose,kCount,true),"native finisher restoration cannot reopen shared owner");Check(!g_pose_restore_fault6689&&g_finisher_fault_d83&&!g_committed_pose_valid6689&&retiredRestoreFramesR96==1&&restoreCalls==n,"recovery forgave finisher overlay or wrote old memory");}},
#endif
      {"partial_write_and_restore_failure_invalidates_shared",[]{Warm();failOverlayAt=3;failRestoreJoint=4;FinisherWriteD83 w;Begin(w);Check(!w.wrote&&g_finisher_fault_d83&&g_pose_restore_fault6689&&!g_committed_pose_valid6689,"partial write rollback failure was hidden");}},
      {"scope_unwind_restores",[]{Warm();try{FinisherWriteD83 w;Begin(w);Check(w.wrote,"no active overlay");throw std::runtime_error("simulated native C++ unwind");}catch(const std::runtime_error&){}Check(Exact(),"exception unwind left pose dirty");}},
      {"alias_change_restarts_blend",[]{Warm();FinisherWriteD83 w;Begin(w,1300,"combat_combo_c01_master");Check(!w.wrote&&g_finisher_state_d83.weight==0&&Exact(),"new clip reused old full correction");}},
      {"off_on_restarts_blend",[]{Warm();fu83::player=false;{FinisherWriteD83 w;Begin(w,1300);Check(!w.wrote&&Exact(),"OFF wrote pose");}fu83::player=true;{FinisherWriteD83 w;Begin(w,1400);Check(!w.wrote&&g_finisher_state_d83.weight==0,"ON reused prior full correction");}}},
      {"character_change_restarts_blend",[]{Warm();currentCharacter=&nativeCharacter2;g_pose6689.character=currentCharacter;fixtureClock=1300;packet.tick=1300;Latch();FinisherWriteD83 w;Begin(w,1300);Check(!w.wrote&&g_finisher_state_d83.weight==0,"new character reused old full correction");}},
      {"retired_transaction_cannot_stamp_new_epoch",[]{Warm();g_pose_epoch6689=8;fixtureClock=1300;packet.tick=1300;Latch(7,2);Refused();}},
      {"retired_transaction_cannot_stamp_new_policy",[]{Warm();g_pose_policy_generation57=4;fixtureClock=1300;packet.tick=1300;Latch(7,2);Refused();}},
      {"invalid_hmd_not_a_fresh_orientation",[]{Warm();packet.hq.x=std::numeric_limits<float>::quiet_NaN();Refused();}},
      {"latch_uses_original_sample_freshness",[]{g_finisher_latch_d83={};fixtureClock=2500;packet.tick=1000;Latch();Check(!g_finisher_latch_d83.valid,"stale packet got fresh latch clock");}},
      {"latch_requires_published_pose",[]{g_finisher_latch_d83={};g_committed_pose_valid6689=false;Latch();Check(!g_finisher_latch_d83.valid,"unpublished pose seeded overlay");}},
      {"chain_cycle_refused",[]{g_finisher_rig_d83={};parents[3]=4;Check(!FinisherRigD83Ready(&nativeOwner,kCount,rig),"cycle admitted");}},
      {"chain_wrong_order_refused",[]{g_finisher_rig_d83={};parents[4]=2;Check(!FinisherRigD83Ready(&nativeOwner,kCount,rig),"disconnected chest/neck chain admitted");}},
      {"idle_releases_blend",[]{Warm();FinisherIdleD83();FinisherWriteD83 w;Begin(w);Check(!w.wrote&&g_finisher_state_d83.weight==0,"idle did not release blend");}}
    };
    int passed=0,failed=0;
    for(auto& t:tests){Reset();try{t.run();++passed;std::cout<<"PASS "<<t.name<<"\n";}catch(const std::exception& e){++failed;std::cout<<"FAIL "<<t.name<<" : "<<e.what()<<"\n";}}
    std::cout<<"TOTAL passed="<<passed<<" failed="<<failed<<"\n";
    return failed?1:0;
}
