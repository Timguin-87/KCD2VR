// Literal production Restore, WriteQuat, native buffer validation, recovery,
// post-callback qualifier and callback/publication boundaries. Native identity,
// memory, scheduling and API leaves are controlled; this is not Windows SEH.
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
std::atomic<std::uint64_t> g_player_lifetime_d44{10},g_pose_epoch6689{1},g_prebuild_writes{0};
std::atomic<int> g_fail_stage{0};
struct PoseState {
 std::uint32_t count=kCount;bool ready=true;
 std::array<QuatT28,kPoseCapacity6689> original{};
} g_pose6689;
struct NativeBuffer {std::array<QuatT28,kCount> joints{};} nativePose,otherPose;
int nativeCharacter=1,otherCharacter=2,nativeSkeleton=3,otherSkeleton=4;
int playerPose=5,charPlus8Slot=6,otherCharSlot=7,nativeAnimation=8,otherAnimation=9;
std::atomic<void*> g_last_player_pose{&playerPose};
struct Descriptor {void* pose=&nativePose;void* owner=&nativeSkeleton;void* characterSlot=&charPlus8Slot;} descriptor;
void* qualifiedPlayer=&nativeCharacter;void* qualifiedSkin=&nativeCharacter;
void* qualifiedPlayerAnim=&nativeAnimation;void* qualifiedSkinAnim=&nativeAnimation;
void* qualifiedSkeleton=&nativeSkeleton;
bool descriptorReadable=true,playerProof=true,skinProof=true,ownerProof=true,countProof=true,rawProof=true;
bool leaseQualified=true,readOK=true,setterOK=true,ignoreSetter=false,writeBeforeFailure=true,useOtherGetter=false;
unsigned nativeCount=kCount,failMask=1u<<2;
unsigned reads=0,nativeReads=0,otherReads=0,writes=0,retired=0,admissions=0,setterQualifications=0;
unsigned publications=0,publicationRejections=0,finalConsumers=0,consumersSeeingFault=0,descriptorReads=0;
std::function<void()> duringRead,beforePost;
std::vector<int> writeOrder;
namespace diagnostic_trace {std::vector<std::string> messages;void Write(const char* s){messages.emplace_back(s);}}
void RetireTrackedFrame51(){++retired;}
std::uint64_t GetTickCount64(){return 1000;}
bool PlayerIdentityAccepts37(const void* character,std::uint64_t){return leaseQualified&&character==qualifiedPlayer;}
bool Safe(const void*,std::size_t){return readOK;}
const QuatT28* Get(void* pose,int j){
 ++reads;if(pose==&nativePose)++nativeReads;else ++otherReads;
 if(duringRead)duringRead();return &static_cast<NativeBuffer*>(pose)->joints.at(j);
}
const QuatT28* OtherGet(void* pose,int j){return Get(pose,j);}
void Set(void* pose,int j,const QuatT28* value){
 ++writes;writeOrder.push_back(j);
 if((failMask&(1u<<j))&&!writeBeforeFailure)throw std::runtime_error("controlled failure before write");
 if(!ignoreSetter)static_cast<NativeBuffer*>(pose)->joints.at(j)=*value;
 if(failMask&(1u<<j))throw std::runtime_error("controlled failure after write");
}
bool ValidateBuffer1Setter(void*,PoseSetFn& setter){++setterQualifications;setter=Set;return setterOK;}
template<class T> bool Read(const void* p,T& out) noexcept {
 ++descriptorReads;if(!descriptorReadable)return false;std::memcpy(&out,p,sizeof(out));return true;
}
bool ReadValidatedCharInstanceFromPlayerPose(void* pose,void*& character,void*& animation) noexcept {
 character=qualifiedPlayer;animation=qualifiedPlayerAnim;return playerProof&&pose==&playerPose;
}
bool ReadValidatedCharInstanceFromSkinning(void* slot,void*& character,void*& animation) noexcept {
 character=qualifiedSkin;animation=qualifiedSkinAnim;return skinProof&&slot==&charPlus8Slot;
}
bool ReadValidatedDefaultSkeletonFromPose(void*,void*& owner) noexcept {owner=qualifiedSkeleton;return ownerProof;}
bool ReadJointCount(void*,std::uint32_t& count) noexcept {count=nativeCount;return countProof;}
bool ValidateRawPoseApi(void*,PoseGetFn& reference,PoseGetFn& current) noexcept {
 reference=Get;current=useOtherGetter?OtherGet:Get;return rawProof;
}
#include "pose_restore_write_r96r1.inc"
#include "pose_restore_read_r96.inc"
#include "pose_restore_shared_r96.inc"
#include "pose_restore_post_helper_r96r1.inc"
struct CoherentTransaction6689 {
 void* pose=&nativePose;PoseSetFn setter=Set;
 restoreR96::Owner restoreOwnerR96{};
 std::array<bool,kPoseCapacity6689> attempted{};
 bool committed=true,restored=false,traceH29=true,traceRestoreOkH29=true;
 unsigned traceRestoreWritesH29=0;std::uint64_t motionSessionD68=1,epoch=1,policyGeneration57=2;
 int tracking=0;
#include "pose_restore_method_r96.inc"
};
bool CoherentOwnerCurrent57(const CoherentTransaction6689&){return true;}
void PublishCommitted6689(CoherentTransaction6689& tx){
 const auto rejectPublish169r2=[&](unsigned){++publicationRejections;g_committed_pose_valid6689=false;};
#include "pose_restore_publish_gate_r96r1.inc"
 ++publications;g_committed_pose_valid6689=true;
}
void FinisherLatchCommittedD83(void*,int,std::uint64_t,std::uint64_t){}
void FinishNativePaletteD67R1(int&,CoherentTransaction6689&){++finalConsumers;consumersSeeingFault+=bool(g_pose_restore_fault6689);}
void FinishFingerOutput28(int&,CoherentTransaction6689&){
 ++finalConsumers;consumersSeeingFault+=bool(g_pose_restore_fault6689);if(beforePost)beforePost();
}
// The extracted statements preserve their production order. The unentered
// !committed diagnostic branch and unrelated palette internals are not copied.
void CallbackTail(CoherentTransaction6689& transaction){
 void* desc=&descriptor;void* pose=&nativePose;void* owner=&nativeSkeleton;
 void* charPlus8=&charPlus8Slot;void* skinChar=&nativeCharacter;std::uint32_t count=kCount;
 const bool committed=transaction.committed;int palette=1,finger=2;
 int* paletteD67R1=&palette;int* fingerOutput28=&finger;
#include "pose_restore_tail_r96r1.inc"
}
bool InitCoherent6689(PoseState&,void*,void*,void*,PoseGetFn,unsigned,int){++admissions;return true;}
bool RejectCoherent6693(CoherentTransaction6689&,int){return false;}
bool Outer(void* pose=&otherPose){
 void* skinChar=&nativeCharacter;void* owner=&nativeSkeleton;bool rigOk=true,rawOk=true,correlated=true;
 PoseGetFn b1fn=Get;std::uint32_t count=kCount;
#include "pose_restore_outer_r96.inc"
 CoherentTransaction6689 tx;auto& s=g_pose6689;void* character=skinChar;PoseGetFn get=b1fn;int rig=0;
#include "pose_restore_gate_r96.inc"
 return true;
}
void Check(bool value,const char* reason){if(!value)throw std::runtime_error(reason);}
void Reset(){
 g_pose_restore_fault6689=false;g_committed_pose_valid6689=true;g_player_lifetime_d44=10;g_pose_epoch6689=1;
 g_pose6689={};g_pose_restore_debt_r96={};g_last_player_pose=&playerPose;descriptor={};
 qualifiedPlayer=qualifiedSkin=&nativeCharacter;qualifiedPlayerAnim=qualifiedSkinAnim=&nativeAnimation;qualifiedSkeleton=&nativeSkeleton;
 descriptorReadable=playerProof=skinProof=ownerProof=countProof=rawProof=leaseQualified=readOK=setterOK=writeBeforeFailure=true;
 ignoreSetter=useOtherGetter=false;nativeCount=kCount;failMask=1u<<2;
 reads=nativeReads=otherReads=writes=retired=admissions=setterQualifications=0;
 publications=publicationRejections=finalConsumers=consumersSeeingFault=descriptorReads=0;
 duringRead={};beforePost={};writeOrder.clear();diagnostic_trace::messages.clear();
 kcd2vr::inventory_lifecycle_t8::state=2;kcd2vr::inventory_lifecycle_t8::loadEpisode18=1;kcd2vr::inventory_lifecycle_t8::loadingObserved18=false;
 for(unsigned j=0;j<kCount;++j){nativePose.joints[j]={{0,0,0,1},{float(j),.25f,1.f}};g_pose6689.original[j]=nativePose.joints[j];}
 otherPose=nativePose;
}
CoherentTransaction6689 Dirty(){
 CoherentTransaction6689 tx;tx.restoreOwnerR96=PoseRestoreOwnerR96(&nativeSkeleton,&nativeCharacter,&nativePose,kCount);
 for(int j:{1,2,4}){tx.attempted[j]=true;nativePose.joints[j].t[2]+=2.f;}return tx;
}
void CompleteLoad(){kcd2vr::inventory_lifecycle_t8::Publish(true,true);kcd2vr::inventory_lifecycle_t8::Publish(true,false);}
void StillBlocked(const char* why){
 auto tx=Dirty();CallbackTail(tx);
 Check(g_pose_restore_fault6689&&g_pose_restore_debt_r96.active&&publications==0&&!g_committed_pose_valid6689,why);
 Check(writes==3&&setterQualifications==0,"post-callback path retried a native write");
}
int main(){
 struct Test {const char* name;std::function<void()> run;};
 const std::vector<Test> cases={
  {"post_effect_failure_reopens_future_rotating_buffer",[]{
   auto tx=Dirty();CallbackTail(tx);
   Check(!g_pose_restore_fault6689&&!g_pose_restore_debt_r96.active,"POST_READBACK_REOPENS: exact originals remained globally faulted");
   Check(!g_committed_pose_valid6689&&publications==0&&publicationRejections==1&&finalConsumers==2&&consumersSeeingFault==2,"failed callback published or a final consumer saw reopened authority");
   Check(writes==3&&setterQualifications==0&&!tx.traceRestoreOkH29&&!g_pose6689.ready,"readback replayed setter or erased failure evidence");
   const auto oldReads=nativeReads;Check(Outer()&&admissions==1&&nativeReads==oldReads&&otherReads==1,"fresh rotating buffer remained blocked or old pointer was read");
  }},
  {"multiple_failed_joints_all_repaid",[]{failMask=(1u<<2)|(1u<<4);auto tx=Dirty();CallbackTail(tx);Check(!g_pose_restore_fault6689&&writes==3,"POST_READBACK_ALL_JOINTS: exact multiple-joint restitution remained blocked");}},
  {"native_animation_in_unfailed_joint_is_allowed",[]{beforePost=[]{nativePose.joints[5].t[0]+=1.f;};auto tx=Dirty();CallbackTail(tx);Check(!g_pose_restore_fault6689&&writes==3,"POST_READBACK_UNFAILED_JOINT: unrelated native animation blocked exact failed-joint proof");}},
  {"no_fault_keeps_ordinary_publication",[]{failMask=0;auto tx=Dirty();CallbackTail(tx);Check(!g_pose_restore_fault6689&&publications==1&&g_committed_pose_valid6689&&reads==0&&descriptorReads==0,"ordinary callback gained fault work or lost publication");}},
  {"existing_same_buffer_entry_recovery_unchanged",[]{auto tx=Dirty();tx.Restore();Check(g_pose_restore_fault6689&&Outer(&nativePose)&&!g_pose_restore_fault6689,"existing exact native entry readback changed");}},
  {"unapplied_restore_failure_keeps_debt",[]{writeBeforeFailure=false;StillBlocked("unapplied failed joint was forgiven");}},
  {"completed_load_cannot_enter_post_restitution",[]{writeBeforeFailure=false;beforePost=[]{CompleteLoad();failMask=0;};StillBlocked("read-only post callback replayed old joint values after load");}},
  {"new_lifetime_cannot_be_forgiven_by_post_observer",[]{beforePost=[]{++g_player_lifetime_d44;};StillBlocked("post observer forgave replacement lifetime");}},
  {"replacement_player_cannot_be_forgiven_by_post_observer",[]{beforePost=[]{qualifiedPlayer=qualifiedSkin=&otherCharacter;};StillBlocked("post observer forgave replacement actor");}},
  {"rotated_descriptor_never_reads_old_pose",[]{beforePost=[]{descriptor.pose=&otherPose;};StillBlocked("changed descriptor pose was accepted");Check(reads==0,"old pose was read after descriptor rotation");}},
  {"changed_descriptor_owner_is_refused",[]{beforePost=[]{descriptor.owner=&otherSkeleton;};StillBlocked("changed descriptor skeleton was accepted");}},
  {"changed_descriptor_character_is_refused",[]{beforePost=[]{descriptor.characterSlot=&otherCharSlot;};StillBlocked("changed descriptor character was accepted");}},
  {"unreadable_descriptor_is_refused",[]{beforePost=[]{descriptorReadable=false;};StillBlocked("unreadable descriptor was accepted");}},
  {"missing_current_player_is_refused",[]{beforePost=[]{g_last_player_pose=nullptr;};StillBlocked("missing player identity was accepted");}},
  {"unqualified_player_or_skin_identity_is_refused",[]{beforePost=[]{playerProof=skinProof=false;};StillBlocked("unqualified native identity was accepted");}},
  {"different_current_player_is_refused",[]{beforePost=[]{qualifiedPlayer=&otherCharacter;};StillBlocked("different current player was accepted");}},
  {"different_current_animation_is_refused",[]{beforePost=[]{qualifiedSkinAnim=&otherAnimation;};StillBlocked("different current skeleton animation was accepted");}},
  {"changed_pose_owner_link_is_refused",[]{beforePost=[]{qualifiedSkeleton=&otherSkeleton;};StillBlocked("changed pose-owner link was accepted");}},
  {"changed_joint_count_is_refused",[]{beforePost=[]{nativeCount=kCount-1;};StillBlocked("changed native joint count was accepted");}},
  {"unqualified_raw_getter_is_refused",[]{beforePost=[]{rawProof=false;};StillBlocked("unqualified raw getter was called");Check(reads==0,"unqualified getter was used");}},
  {"expired_C37_identity_is_refused",[]{beforePost=[]{leaseQualified=false;};StillBlocked("expired player identity was accepted");}},
  {"unreadable_native_buffer_keeps_debt",[]{beforePost=[]{readOK=false;};StillBlocked("unreadable native pose was accepted");}},
  {"nonfinite_unfailed_joint_keeps_debt",[]{beforePost=[]{nativePose.joints[5].q[3]=std::numeric_limits<float>::quiet_NaN();};StillBlocked("invalid pose outside debt was accepted");}},
  {"approximate_failed_joint_is_not_exact_repayment",[]{beforePost=[]{nativePose.joints[2].t[0]+=1e-6f;};StillBlocked("approximate old joint value was treated as exact");}},
  {"C37_changes_during_copy_keep_debt",[]{duringRead=[]{++g_player_lifetime_d44;};StillBlocked("identity changed during copy but debt was cleared");}},
  {"descriptor_rotates_during_copy_keep_debt",[]{duringRead=[]{descriptor.pose=&otherPose;};StillBlocked("descriptor changed during copy but debt was cleared");}},
  {"getter_changes_during_copy_keep_debt",[]{duringRead=[]{useOtherGetter=true;};StillBlocked("getter changed during copy but debt was cleared");}},
  {"current_player_changes_during_copy_keep_debt",[]{duringRead=[]{qualifiedPlayer=&otherCharacter;};StillBlocked("player changed during copy but debt was cleared");}}
 };
 unsigned passed=0;
 for(const auto& test:cases){Reset();try{test.run();++passed;std::cout<<"PASS "<<test.name<<'\n';}catch(const std::exception& e){std::cout<<"FAIL "<<test.name<<" : "<<e.what()<<'\n';}}
 std::cout<<"R96R1 post-callback restoration "<<passed<<'/'<<cases.size()<<"; literal native boundaries; Windows/game/HMD NOT_RUN\n";
 return passed==cases.size()?0:1;
}
