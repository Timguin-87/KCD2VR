// Source-extracted production Restore/recovery and current outer callback gate.
// Native memory, C37 publication, diagnostics and time are explicit host leaves.
#include <array>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#define __fastcall
#define __try try
#define __except(x) catch(...)
#include "pose_restore_types_r96.inc"
constexpr std::size_t kPoseCapacity6689=1024;
constexpr unsigned kCount=6;
std::atomic<bool> g_pose_restore_fault6689{false},g_committed_pose_valid6689{true};
std::atomic<std::uint64_t> g_player_lifetime_d44{1};
struct CoherentFixture {
 std::uint32_t count=kCount;bool ready=true;
 std::array<QuatT28,kPoseCapacity6689> original{};
} g_pose6689;
struct NativeBuffer {std::array<QuatT28,kCount> joints{};} nativePose,otherPose;
int nativeCharacter=1,otherCharacter=2,nativeSkeleton=3,otherSkeleton=4;
unsigned retired=0,reads=0,writes=0,admissions=0;
bool readOK=true,qualified=true,setterOK=true,ignoreSetter=false;int failJoint=2;
std::function<void()> duringRead,beforeSetter;
unsigned setterQualifications=0;
std::vector<int> writeOrder;
namespace diagnostic_trace {std::vector<std::string> messages;void Write(const char* s){messages.emplace_back(s);}}
void RetireTrackedFrame51(){++retired;}
std::uint64_t GetTickCount64(){return 1000;}
bool PlayerIdentityAccepts37(const void*,std::uint64_t){return qualified;}
bool Safe(const void*,std::size_t){return readOK;}
const QuatT28* Get(void* pose,int j){++reads;if(duringRead)duringRead();return &static_cast<NativeBuffer*>(pose)->joints.at(j);}
void Set(void* pose,int j,const QuatT28* value){if(!ignoreSetter)static_cast<NativeBuffer*>(pose)->joints.at(j)=*value;}
bool ValidateBuffer1Setter(void*,PoseSetFn& setter){++setterQualifications;if(beforeSetter)beforeSetter();setter=Set;return setterOK;}
bool WriteQuat(PoseSetFn set,void* pose,int j,const QuatT28& value){
 ++writes;writeOrder.push_back(j);if(j==failJoint)return false;set(pose,j,&value);return true;
}
#include "pose_restore_read_r96.inc"
#include "pose_restore_shared_r96.inc"
struct CoherentTransaction6689 {
 void* pose=&nativePose;PoseSetFn setter=Set;
#if R96_RESTORE_DEBT_PRESENT
 restoreR96::Owner restoreOwnerR96{};
#endif
 std::array<bool,kPoseCapacity6689> attempted{};
 bool restored=false,traceH29=true,traceRestoreOkH29=true;
 unsigned traceRestoreWritesH29=0;std::uint64_t motionSessionD68=1;
#include "pose_restore_method_r96.inc"
};
bool InitCoherent6689(CoherentFixture&,void*,void*,void*,PoseGetFn,unsigned,int){++admissions;return true;}
bool RejectCoherent6693(CoherentTransaction6689&,int){return false;}
bool Outer(void* skinChar=&nativeCharacter,void* owner=&nativeSkeleton,void* pose=&nativePose,
 bool rigOk=true,bool rawOk=true,bool correlated=true){
 PoseGetFn b1fn=Get;std::uint32_t count=kCount;
#include "pose_restore_outer_r96.inc"
 // Real BeginCoherent gate follows the outer callback. It must be reachable
 // despite the fault which blocks every ordinary body/attachment consumer.
 CoherentTransaction6689 tx;auto& s=g_pose6689;void* character=skinChar;PoseGetFn get=b1fn;int rig=0;
#include "pose_restore_gate_r96.inc"
 return true;
}
void Check(bool v,const char* why){if(!v)throw std::runtime_error(why);}
void Reset(){
 g_pose_restore_fault6689=false;g_committed_pose_valid6689=true;g_player_lifetime_d44=10;
 g_pose6689={};retired=reads=writes=admissions=setterQualifications=0;qualified=readOK=setterOK=true;ignoreSetter=false;failJoint=2;duringRead={};beforeSetter={};writeOrder.clear();
 kcd2vr::inventory_lifecycle_t8::state=2;kcd2vr::inventory_lifecycle_t8::loadEpisode18=1;kcd2vr::inventory_lifecycle_t8::loadingObserved18=false;
 diagnostic_trace::messages.clear();
#if R96_RESTORE_DEBT_PRESENT
 g_pose_restore_debt_r96={};
#endif
 for(unsigned j=0;j<kCount;++j){nativePose.joints[j]={{0,0,0,1},{float(j),.25f,1.f}};g_pose6689.original[j]=nativePose.joints[j];}
 otherPose=nativePose;
}
CoherentTransaction6689 Dirty(){
 CoherentTransaction6689 tx;
#if R96_RESTORE_DEBT_PRESENT
 tx.restoreOwnerR96=PoseRestoreOwnerR96(&nativeSkeleton,&nativeCharacter,&nativePose,kCount);
#endif
 for(int j:{1,2,4}){tx.attempted[j]=true;nativePose.joints[j].t[2]+=2.f;}
 return tx;
}
void Fault(){auto tx=Dirty();tx.Restore();Check(g_pose_restore_fault6689&&!g_committed_pose_valid6689,"failed native restoration was not shared");}
void NativeRestores(){nativePose.joints[2]=g_pose6689.original[2];}
void CompleteLoad(){kcd2vr::inventory_lifecycle_t8::Publish(true,true);kcd2vr::inventory_lifecycle_t8::Publish(true,false);}
int main(){
 struct Test {const char* name;std::function<void()> run;};
 std::vector<Test> cases={
  {"failed_write_keeps_exact_reverse_attempts",[]{auto tx=Dirty();tx.Restore();Check(writeOrder==std::vector<int>{4,2,1},"restore order/attempt mask changed");Check(!tx.traceRestoreOkH29&&tx.traceRestoreWritesH29==3,"trace lost failure");Check(g_pose_restore_fault6689&&!g_committed_pose_valid6689,"shared publication did not suspend");const auto n=writes;tx.Restore();Check(writes==n,"restore ran twice");}},
  {"dirty_same_owner_stays_blocked",[]{Fault();const auto n=writes;Check(!Outer()&&g_pose_restore_fault6689&&admissions==0&&writes==n,"dirty native owner was forgiven");}},
  {"rotating_pose_buffer_is_not_new_player",[]{Fault();Check(!Outer(&nativeCharacter,&nativeSkeleton,&otherPose)&&g_pose_restore_fault6689,"different CPoseData incorrectly retired same actor debt");}},
  {"different_skeleton_is_not_new_player",[]{Fault();Check(!Outer(&nativeCharacter,&otherSkeleton,&otherPose)&&g_pose_restore_fault6689,"different skeleton incorrectly retired same actor debt");}},
  {"same_owner_exact_native_readback_recovers_outer_entry",[]{Fault();NativeRestores();const auto n=writes;Check(Outer()&&!g_pose_restore_fault6689&&admissions==1,"same owner native restoration cannot reach body admission");Check(writes==n&&retired==1&&!g_pose6689.ready&&!g_committed_pose_valid6689,"recovery wrote old native memory or published stale pose");}},
  {"qualified_replacement_actor_recovers_outer_entry",[]{Fault();const auto n=writes;Check(Outer(&otherCharacter,&otherSkeleton,&otherPose)&&!g_pose_restore_fault6689,"qualified replacement player could not recover");Check(writes==n&&admissions==1&&retired==1,"replacement performed old pointer writes");}},
  {"same_address_new_C37_lifetime_recovers",[]{Fault();++g_player_lifetime_d44;Check(Outer()&&!g_pose_restore_fault6689&&retired==1,"positively qualified same-address new player lifetime stayed poisoned");}},
  {"unqualified_replacement_cannot_forgive",[]{Fault();qualified=false;Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose)&&g_pose_restore_fault6689&&admissions==0,"unqualified identity cleared debt");}},
  {"uncorrelated_replacement_cannot_forgive",[]{Fault();Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose,true,true,false)&&g_pose_restore_fault6689,"uncorrelated native callback cleared debt");}},
  {"unvalidated_rig_or_raw_API_cannot_forgive",[]{Fault();Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose,false,true)&&!Outer(&otherCharacter,&otherSkeleton,&otherPose,true,false)&&reads==0,"unvalidated outer callback read/cleared debt");}},
  {"unreadable_replacement_cannot_forgive",[]{Fault();readOK=false;Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose)&&g_pose_restore_fault6689,"unreadable replacement cleared debt");}},
  {"invalid_pose_anywhere_cannot_forgive",[]{Fault();otherPose.joints[5].q[3]=std::numeric_limits<float>::quiet_NaN();Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose)&&g_pose_restore_fault6689,"invalid native pose cleared debt");}},
  {"C37_lifetime_change_during_read_cannot_forgive",[]{Fault();duringRead=[]{++g_player_lifetime_d44;};Check(!Outer(&otherCharacter,&otherSkeleton,&otherPose)&&g_pose_restore_fault6689,"torn C37 lifetime cleared debt");}},
  {"same_owner_readback_must_match_exact_failed_joint",[]{Fault();NativeRestores();nativePose.joints[2].t[0]+=1e-6f;Check(!Outer()&&g_pose_restore_fault6689,"approximate readback forgave exact debt");}},
  {"same_address_load_repays_exact_debt_with_fresh_setter",[]{Fault();CompleteLoad();failJoint=-1;const auto n=writes;Check(Outer()&&!g_pose_restore_fault6689&&admissions==1,"same-address completed load did not repair current native owner");Check(writes==n+1&&writeOrder.back()==2&&setterQualifications==1,"repair wrote outside retained failed joints or reused stale setter");}},
  {"load_UI_alone_never_forgives_failed_restitution",[]{Fault();CompleteLoad();const auto n=writes;Check(!Outer()&&g_pose_restore_fault6689&&writes==n+1,"load was treated as debt forgiveness");failJoint=-1;Check(!Outer()&&writes==n+1&&setterQualifications==1,"failed repair retried every callback");CompleteLoad();Check(Outer()&&writes==n+2&&!g_pose_restore_fault6689,"new completed load could not requalify the exact repair");}},
  {"unknown_UI_or_mode_recheck_cannot_start_restitution",[]{Fault();auto& l=kcd2vr::inventory_lifecycle_t8::state;l.fetch_add(2);kcd2vr::inventory_lifecycle_t8::Publish(false,false);kcd2vr::inventory_lifecycle_t8::Publish(true,false);const auto n=writes;failJoint=-1;Check(!Outer()&&writes==n&&setterQualifications==0,"unknown UI/mode recheck impersonated a completed load");}},
  {"loading_in_progress_cannot_start_restitution",[]{Fault();kcd2vr::inventory_lifecycle_t8::Publish(true,true);const auto n=writes;failJoint=-1;Check(!Outer()&&writes==n&&setterQualifications==0,"loading screen admitted a native repair");}},
  {"same_actor_different_buffer_never_receives_old_debt",[]{Fault();CompleteLoad();failJoint=-1;const auto n=writes;Check(!Outer(&nativeCharacter,&nativeSkeleton,&otherPose)&&writes==n&&setterQualifications==0,"load replayed old debt into a different native buffer");Check(Outer()&&writes==n+1,"wrong-buffer callback consumed matching-buffer opportunity");}},
  {"fresh_setter_failure_consumes_only_this_load_attempt",[]{Fault();CompleteLoad();failJoint=-1;setterOK=false;const auto n=writes;Check(!Outer()&&writes==n&&setterQualifications==1,"unqualified setter wrote");setterOK=true;Check(!Outer()&&writes==n&&setterQualifications==1,"setter failure looped within same load");CompleteLoad();Check(Outer()&&setterQualifications==2,"next true load never requalified setter");}},
  {"owner_changes_during_setter_qualification_no_write",[]{Fault();CompleteLoad();failJoint=-1;beforeSetter=[]{++g_player_lifetime_d44;};const auto n=writes;Check(!Outer()&&g_pose_restore_fault6689&&writes==n,"retired owner received a repair write");}},
  {"new_load_begins_during_qualification_no_write",[]{Fault();CompleteLoad();failJoint=-1;beforeSetter=[]{kcd2vr::inventory_lifecycle_t8::Publish(true,true);};const auto n=writes;Check(!Outer()&&g_pose_restore_fault6689&&writes==n,"new loading episode received a repair write");}},
  {"setter_success_without_exact_readback_keeps_debt",[]{Fault();CompleteLoad();failJoint=-1;ignoreSetter=true;const auto n=writes;Check(!Outer()&&g_pose_restore_fault6689&&writes==n+1,"setter return alone forgave debt");Check(!Outer()&&writes==n+1,"readback mismatch retried each frame");}},
  {"unfailed_joint_does_not_block_native_readback",[]{Fault();NativeRestores();nativePose.joints[5].t[0]+=1.f;Check(Outer()&&!g_pose_restore_fault6689,"normal animation in unfailed joint blocked exact debt proof");}},
  {"missing_record_keeps_existing_fault_conservative",[]{g_pose_restore_fault6689=true;Check(!Outer()&&g_pose_restore_fault6689,"fault with no exact debt was silently reset");}},
  {"success_restore_never_adds_fault",[]{failJoint=-1;auto tx=Dirty();tx.Restore();Check(!g_pose_restore_fault6689&&g_committed_pose_valid6689&&tx.traceRestoreOkH29,"successful restore changed ordinary admission");}},
  {"no_fault_skips_extra_native_recovery_read",[]{Check(Outer()&&reads==1&&admissions==1&&retired==0,"ordinary callback gained recovery work");}}
 };
 unsigned passed=0;
 for(const auto& test:cases){Reset();try{test.run();++passed;std::cout<<"PASS "<<test.name<<'\n';}catch(const std::exception& e){std::cout<<"FAIL "<<test.name<<" : "<<e.what()<<'\n';}}
 std::cout<<"R96 restoration recovery "<<passed<<'/'<<cases.size()<<"; literal production boundaries; native/Windows/HMD NOT_RUN\n";
 return passed==cases.size()?0:1;
}
