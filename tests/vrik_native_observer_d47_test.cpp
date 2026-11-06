// Actual SetIK adapter. Native RTTI, memory and owner providers are explicit
// leaves; assert F7/F11-off observation cannot suppress semantic/native work.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "diagnostics/capture168.h"
namespace diagnostics=kcd2vr::diagnostics;
struct QuatT28{float q[4]{0,0,0,1};float t[3]{};};
#include "arm_types_d47.inc"
#define __fastcall
static std::uint64_t fixtureNow=10000;
static std::uint64_t GetTickCount64(){return fixtureNow;}
static std::atomic<std::uint64_t> g_setik_calls{0},g_last_setik_tick{0},g_prebuild_writes{0},g_setik_upstream_writes{0},g_setik_hold_writes{0},g_rejects{0};
static std::atomic<int> g_solver_joint_c{-1},g_solver_joint_e{-1},g_mode{5},g_census_mode{1},g_f11_theory_phase{0},g_weapon_control_mode{4};
static std::atomic<bool> g_vrik_calibration_requested{false},g_player_identity_observe{false};
static std::atomic<bool> g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_menu{false},g_ctx_scripted{false};
static bool exactOwner=true;static int memoryReads=0,poseReads=0,discovery=0,ownerTrace=0,identity=0,attachmentCache=0,weaponTicks=0,nativeCalls=0;
static std::vector<std::string> lines;
namespace diagnostic_trace {static void Write(const char* s){lines.emplace_back(s);}}
static void Rtti(void*,char* out,std::size_t n){std::snprintf(out,n,"CPoseData");}
static bool ResolveExactPlayerSolverOwner(void*,void*& module,void*& entity){module=reinterpret_cast<void*>(0x1000);entity=reinterpret_cast<void*>(0x2000);return exactOwner;}
static bool ReadValidatedCharInstanceFromPlayerPose(void*,void*& character,void*& anim){character=reinterpret_cast<void*>(0x3000);anim=reinterpret_cast<void*>(0x4000);return true;}
namespace pid37 {enum class Source{Solver};enum class Verdict{Refreshed,Qualified,Rejected};}
static pid37::Verdict ObservePlayerIdentity37(pid37::Source,bool accepted,void*,void*,void*,std::uint64_t){++identity;return accepted?pid37::Verdict::Qualified:pid37::Verdict::Rejected;}
static constexpr std::uintptr_t kPoseInCharInstanceOffset=0x8E0;
static void TraceWeaponAttachmentOwnerV6528(void*,void*){++attachmentCache;}
static void WeaponControlTickImpl(){++weaponTicks;}
static void MaybeTraceDiscoveryObject(const char*,void*,int,int){++discovery;}
static void TracePlayerOwnershipChain(void*,int){++ownerTrace;}
static const char* CombatCensusModeName(int){return "HOST";}
static bool ReadTrackingSnapshot(TrackingSnapshot&){return true;}
template<class T> static bool Read(const void*,T& out){++memoryReads;out=T(3);return true;}
using PoseGetFn=const QuatT28*(*)(void*,int);
static const QuatT28* GetPose(void*,int){static QuatT28 q;return &q;}
static bool ValidateRawPoseApi(void*,PoseGetFn& a,PoseGetFn& b){++memoryReads;a=b=GetPose;return true;}
static bool ReadQuat(PoseGetFn get,void* pose,int id,QuatT28& q){++poseReads;q=*get(pose,id);return true;}
static void Original(void*,void*){++nativeCalls;}
static auto g_setik_original=Original;
#include "subsystems/vrik/native_ik_observer.inl"
static int checks=0,failures=0;
static void ck(bool b,const char* s){++checks;if(!b){++failures;std::printf("FAIL %s\n",s);}}
static void resetCounters(){memoryReads=poseReads=discovery=ownerTrace=identity=attachmentCache=weaponTicks=nativeCalls=0;lines.clear();}
static void invoke(){SetIkDetour(reinterpret_cast<void*>(0x5000),reinterpret_cast<void*>(0x6000));}
int main(){
 namespace cap=diagnostics::capture168;
 cap::gate.mask=0;resetCounters();invoke();
 ck(memoryReads==0&&poseReads==0&&discovery==0&&ownerTrace==0&&lines.empty(),"recorders OFF performs no optional native solver reads, discovery or formatting");
 ck(identity==1&&attachmentCache==1&&weaponTicks==1&&nativeCalls==1,"recorders OFF preserves qualified owner cache, physical-control tick and native SetIK exactly once");
 exactOwner=false;resetCounters();invoke();
 ck(identity==1&&attachmentCache==0&&weaponTicks==0&&nativeCalls==1,"NPC/unknown owner cannot acquire player control; original still called");
 exactOwner=true;cap::gate.select(cap::Domain::Vrik,true,fixtureNow);resetCounters();invoke();
 ck(memoryReads>0&&poseReads>0&&discovery==2&&ownerTrace==1&&!lines.empty(),"F7 explicitly enables native pre/post observation");
 ck(identity==1&&attachmentCache==1&&weaponTicks==1&&nativeCalls==1,"F7 observation keeps semantic/native call count unchanged");
 cap::gate.select(cap::Domain::Vrik,false,fixtureNow);fixtureNow+=2000;resetCounters();invoke();
 ck(memoryReads==0&&poseReads==0&&discovery==0&&lines.empty(),"closing F7 stops the next optional native sample immediately");
 cap::gate.select(cap::Domain::Reload,true,fixtureNow);resetCounters();invoke();
 ck(memoryReads>0&&poseReads>0&&nativeCalls==1,"F11 can observe the same native solver without changing its owner");
 fixtureNow+=cap::Duration+1;resetCounters();invoke();
 ck(memoryReads==0&&poseReads==0&&lines.empty()&&nativeCalls==1,"expired recorder cannot keep expensive legacy observation alive");
 std::printf("D47 native SetIK diagnostic boundary: %d/%d PASS; no retail ABI executed.\n",checks-failures,checks);return failures?1:0;
}
