#include "../src/diagnostics/geometry_scope40c1.h"
#include <atomic>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <initializer_list>
static std::uint64_t GetTickCount64(){return 100;}
template<class T>T Rd(const void* p,std::uintptr_t off,T fallback){if(!p)return fallback;T v{};std::memcpy(&v,static_cast<const char*>(p)+off,sizeof(v));return v;}
static int actor,otherActor,entity,state,type=1;
static bool CombatActor(const void* a,const void*& e,const void*& s,bool){if(a!=&actor&&a!=&otherActor)return false;e=a==&actor?&entity:a;s=&state;return true;}
static bool IsPlayer(const void* e){return e==&entity;}
static int I(const void*,unsigned){return type;}
static constexpr unsigned kAttackType=0;
namespace frik_pose_probe {
static bool physical=true,integrated=false;
bool PhysicalCombat177Enabled(){return physical;}
bool CombatIntegrated178r22c2Enabled(){return integrated;}
bool CombatExperiment175Enabled(){return physical||integrated;}
}
namespace diagnostic_trace {void Write(const char*){}}
static std::atomic<bool> g_live178r35{true};
#include "../src/subsystems/hybrid/sweep_owner.inl"
int main(){
 const void* rpg[2]{nullptr,&actor};int contact,target;
 const void* request[4]{nullptr,nullptr,&target,&contact};
 // The production rejection no longer even reads a VR ticket, tracking frame,
 // producer-ready or producer-fault flag. No freshness fallback can authorize it.
 assert(NativeSweepRejected178r40(0xC52534,rpg,request));
 type=0;assert(NativeSweepRejected178r40(0xC52534,rpg,request));type=1;
 {kcd2vr::geometry40c1::Scope injected(&actor,&contact,&target);
  assert(!NativeSweepRejected178r40(0xC52534,rpg,request));
  int unrelated;request[3]=&unrelated;assert(NativeSweepRejected178r40(0xC52534,rpg,request));request[3]=&contact;
 }
 for(int t:{2,3,4,9}){type=t;assert(!NativeSweepRejected178r40(0xC52534,rpg,request));}type=1;
 assert(!NativeSweepRejected178r40(0xD538D2,rpg,request));
 rpg[1]=&otherActor;assert(!NativeSweepRejected178r40(0xC52534,rpg,request));rpg[1]=&actor;
 frik_pose_probe::physical=false;assert(!NativeSweepRejected178r40(0xC52534,rpg,request));
 frik_pose_probe::physical=true;frik_pose_probe::integrated=true;
 assert(NativeSweepRejected178r40(0xC52534,rpg,request)); // R59C3 rung 1 is also contact-only
}
