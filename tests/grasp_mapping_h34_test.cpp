// Actual production InitCoherent and grasp helpers. Reuse the retained native
// memory/companion/map leaves; native fist service is explicitly absent here.
// This fixture does not run or claim coverage of the retained combat main.
#include "../src/core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false,responsiveCombatH27=false;};
static kcd2vr::core::SnapshotMailbox<FistOwner101> g_fist_owner101;
static std::atomic<std::uint64_t> g_fist_epoch101{1};
static bool FistSelected101() noexcept {return false;}
static bool FistModePermits178r22c2() noexcept {return false;}
#define main retained_pose_main_grasp_mapping_h34
#include "pose_adapter_test.cpp"
#undef main
static kcd2vr::grasp13::Acquisition54 graspActionH34{};
static bool readGraspActionH34(kcd2vr::grasp13::Acquisition54& out) noexcept {out=graspActionH34;return true;}
int main(){
 namespace g=kcd2vr::grasp13;unsigned checked=0,failed=0;
 auto ck=[&](bool v,const char* why){++checked;if(!v){++failed;std::printf("FAIL %s\n",why);}};
 Init();static CoherentState6689 s{};void* ownerA=&fixture;void* ownerB=fixture.parent.data();
 ck(InitCoherent6689(s,ownerA,character.data(),&fixture,MockGet,Count,rig),"actual original skeleton initialized");
 auto original=[&](float qx){auto locals=s.restLocal;
  for(int i=0;i<s.fingerCount[0];++i){const int id=s.fingerIds[0][i];locals[id].q[0]=qx;locals[id].q[3]=std::sqrt(1-qx*qx);}
  for(unsigned k=0;k<s.count;++k){const int id=s.order[k],p=s.parent[id];s.original[id]=p<0?locals[id]:AbsoluteFromRelative(s.original[p],locals[id]);}
 };
 g::Binding b{};b.character=reinterpret_cast<std::uintptr_t>(character.data());b.manager=2;b.item=3;b.logic=4;
 b.token=5;b.key=6;b.epoch=7;b.entity=8;b.actor=9;b.side=0;b.attached=b.place=true;
 auto action=[&](){graspActionH34={};auto& a=graspActionH34;a.character=b.character;a.epoch=b.epoch;a.side=0;
  a.action=10+b.token;a.scope=20+b.token;a.serial=30+b.token;a.startedAt=900;a.tick=1000;a.active=true;
  a.graspToken=b.token;a.graspKey=b.key;a.graspItem=b.item;a.graspEntity=b.entity;};
 action();g_graspAcquisition54=&readGraspActionH34;original(.1f);
 ck(OriginalGraspFingers55(s,0,b,7,100,1000,1000,b.fingers55,&graspActionH34),"original canonical native reference captured");
 b.gripReady55=true;b.grip55.p[0]=.07f;
 ck(ApplyGraspFingers54(s,0,b,7,1000)&&s.graspFingers54[0].ready,"original mapping applies exact canonical reference");
 ck(InitCoherent6689(s,ownerA,character.data(),&fixture,MockGet,Count,rig)&&s.graspFingers54[0].ready&&ApplyGraspFingers54(s,0,b,7,1000),
    "unchanged native owner/map fast path preserves the valid paired reference");
 const auto oldCount=s.fingerCount[0];const int oldFirst=s.fingerIds[0][0];mockFingerMapSwapH34=true;
 ck(InitCoherent6689(s,ownerB,character.data(),&fixture,MockGet,Count,rig)&&s.character==character.data()&&s.count==Count&&s.fingerCount[0]==oldCount&&s.fingerIds[0][0]!=oldFirst,
    "actual Init replaces skeleton and ordered map with SAME character/body/count");
 ck(!s.graspFingers54[0].ready,"Init retires previous mapping's cached rotations");
 original(.3f);ck(!ApplyGraspFingers54(s,0,b,7,1000)&&!s.graspFingers54[0].ready,"old canonical Binding payload cannot apply to replacement map");
 s.graspFingers54[0]={};ck(!ApplyGraspFingers54(s,0,b,7,1000),"clearing cache cannot relearn stale retained Binding payload");
 // A genuine new native acquisition, not a live-pose opportunistic regrip.
 ++b.token;b.gripReady55=false;b.fingers55={};action();original(.3f);
 ck(OriginalGraspFingers55(s,0,b,7,101,1000,1000,b.fingers55,&graspActionH34),"new resource captures actual replacement mapping reference");
 b.gripReady55=true;ck(ApplyGraspFingers54(s,0,b,7,1000)&&std::fabs(s.local[s.fingerIds[0][0]].q[0]-.3f)<1e-6,
    "new native canonical reacquisition applies to replacement map");
 std::swap(s.fingerIds[0][0],s.fingerIds[0][3]);
 ck(!ApplyGraspFingers54(s,0,b,7,1000)&&!s.graspFingers54[0].ready,"changed ordered IDs cannot reuse a cache within the same Init generation");
 // Even a fresh initialization of the same owner/map is a new local lifetime.
 s.ready=false;ck(InitCoherent6689(s,ownerB,character.data(),&fixture,MockGet,Count,rig),"actual same-owner map reinitialization succeeds");
 original(.4f);ck(!ApplyGraspFingers54(s,0,b,7,1000),"old canonical payload cannot cross same-address mapping reinitialization");
 std::printf("grasp_mapping_h34=%u/%u actualInit=YES nativeExecution=NONE\n",checked-failed,checked);return failed?1:0;
}
