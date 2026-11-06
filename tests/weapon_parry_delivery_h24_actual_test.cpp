#include "../src/vrik/weapon_parry_contact_h24.h"
#include "../src/diagnostics/parry_trace176.h"
#include <atomic>
#include <unordered_map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
using WeaponParryContactH24=kcd2vr::weapon_parry_h24::Contact;
namespace npc172=kcd2vr::npc_contact172;namespace diagnostics=kcd2vr::diagnostics;
struct NativeOwner104{void*player=nullptr;void*actor=nullptr;void*state=nullptr;void*entity=nullptr;std::uint64_t epoch=0;};
struct NativeParryOpponent176{void*actor=nullptr;void*state=nullptr;void*entity=nullptr;unsigned id=0;bool incoming=true,attacking=true;};
struct PhysFrame6698{struct Profile{std::uint64_t key=0,generation=0;} profiles[2];void*manager=nullptr;void*player=nullptr;std::uint64_t request=1;struct Input{std::uint64_t bodyEpoch=5,sample=1,tick=100;bool allowed=true,tracked[2]{true,true};} input;struct Output{bool active[2]{true,true},owned[2]{true,true};}output;};
namespace nc104{struct Observed{bool blocking=false;int phase=2;};}
namespace native_admission147{bool guardInput(nc104::Observed s){return s.phase==2||s.phase==128;}bool freeGuardHeld44c2(nc104::Observed s,bool own){return own&&s.phase==256;}}
namespace nc104{bool blockMetadataH14(Observed s){return s.phase==128;}}
struct Guard{NativeOwner104 owner{};bool held=false,releaseWaitH14=false,pendingPress46=false,pendingBlockMetadataH14=false,pendingLegacyMotionH14=false;std::uint64_t leaseH14=1,key=0,generation=0,lastHold=0;int side=0;};
static std::atomic<bool> g_gameplay_allowed{true},g_head_valid{true},g_right_valid{true},g_left_valid{true},g_ctx_menu{false},g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_scripted{false};
namespace animation_filter{static std::atomic<bool> nativeBodyOwned112{false};}
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1},g_pose_epoch6689{5};
static PhysFrame6698 fixtureSource;
struct Mailbox{bool read(PhysFrame6698&f){f=fixtureSource;return true;}};static Mailbox g_phys_frame6698;
static Guard g_native_guard112;static std::atomic<void*> g_guard_manual_owner112{nullptr},g_guard_physical_owner112{nullptr},g_henry_item_attach_manager{nullptr};static std::atomic<bool> g_guard_manual112{false},g_guard_physical112{false};
static NativeOwner104 live{};static NativeParryOpponent176 enemy{};static nc104::Observed observed{};static WeaponParryContactH24 contact{};static bool contactNow=false;static int ack=0,window=1,presses=0,releases=0,traces=0;static bool callbackChangeLease=false,callbackChangeManager=false;
static std::unordered_map<std::uintptr_t,std::uint64_t> mem;
static std::uintptr_t WhBase(){return 0x180000000;}
template<class T>bool Read(const void*p,T&out){auto i=mem.find(reinterpret_cast<std::uintptr_t>(p));if(i==mem.end())return false;std::memcpy(&out,&i->second,sizeof(out));return true;}
static bool NativeResolve104(void*,NativeOwner104& o){o=live;return true;}
static bool NativeObserve104(const NativeOwner104&,nc104::Observed&o){o=observed;return true;}
static bool NativeParryOpponentRead176(const NativeOwner104&,NativeParryOpponent176&o){o=enemy;return true;}
static bool NativeParryWindow55c2(const NativeOwner104&,int&out){out=window;return true;}
static bool ReadWeaponParryContactH24(const NativeOwner104&,const PhysFrame6698&,const NativeParryOpponent176&,std::uint64_t,WeaponParryContactH24&out){out=contact;return contactNow;}
static bool PhysVft6698(void*p,std::uintptr_t rva){std::uintptr_t v=0;return Read(p,v)&&v==WhBase()+rva;}
static bool NativeHandKey6697(void*,int,std::uint64_t&key){key=0x22;return true;}
static bool NativeHeldGuardOwnerH14(const NativeOwner104&o){return g_native_guard112.held&&g_native_guard112.owner.actor==o.actor&&g_native_guard112.owner.state==o.state&&g_native_guard112.owner.epoch==o.epoch;}
static int NativeGuardReleaseAckH14(const NativeOwner104&){return ack;}
static bool NativeGuardAwaitReleaseH14(const PhysFrame6698&,std::uint64_t lease){g_native_guard112.releaseWaitH14=true;return g_native_guard112.leaseH14==lease;}
static bool PhysCallAction6698(void*,int name,int mode){if(name!=3)std::abort();if(mode==1){++presses;if(callbackChangeManager)mem[0x1000+0x9D0]=0xA000;}else if(mode==2){++releases;if(callbackChangeLease)++g_native_guard112.leaseH14;}else std::abort();return true;}
static void NativeGuardMarkHeldH14(){g_native_guard112.held=true;++g_native_guard112.leaseH14;}
static void*PhysItemLookup6698(std::uint64_t key){return key==0x99?reinterpret_cast<void*>(0x20000):nullptr;}
static void*EntityById6694(unsigned id){return id==7?reinterpret_cast<void*>(0x40000):nullptr;}
static void NativeParryTrace176(const NativeOwner104&,const NativeParryOpponent176*,diagnostics::parry176::Cause cause,std::uint64_t,bool,bool,std::uint64_t){if(cause!=diagnostics::parry176::Cause::WeaponContact)std::abort();++traces;}
static bool fixtureNativeSourceCurrent=true;
static bool NativeWeaponParrySourceCurrentH24(const WeaponParryContactH24&,const PhysFrame6698&,std::uint64_t){return fixtureNativeSourceCurrent;}
#include "../src/vrik/weapon_parry_delivery_h24.inl"
static unsigned checks=0;static void check(bool x){++checks;if(!x){std::printf("FAIL %u\n",checks);std::exit(1);}}
int main(){
 namespace wp=kcd2vr::weapon_parry_h24;namespace hd=kcd2vr::hmd_dodge_h24;
 live={reinterpret_cast<void*>(0x1000),reinterpret_cast<void*>(0x2000),reinterpret_cast<void*>(0x3000),reinterpret_cast<void*>(0x4000),5};
 enemy={reinterpret_cast<void*>(0x5000),reinterpret_cast<void*>(0x10000),reinterpret_cast<void*>(0x6000),88,true,true};
 PhysFrame6698 f{};f.profiles[0]={0x22,2};f.manager=reinterpret_cast<void*>(0x9000);g_henry_item_attach_manager=f.manager;g_native_guard112.owner=live;f.player=live.player;fixtureSource=f;
 auto B=WhBase();mem={{0x1000+0x9D0,0x9000},{0x9000,B+0x3EC9958},{0x9000+0x38,0x1000},{0x10000+0x8C0,B+0x3B0A5E8},{0x10000+0x8C8,0x99},{0x20000,B+0x3A2DA88},{0x20000+0x30,0x99},{0x20000+0x88,0x30000},{0x30000+0x30,7},{0x40000,B+0x4072C78},{0x40000+0xC,7},{0x40000+0xA8,0x50000},{0x50000+8,0x60000},{0x60000+0x19,0},{0x60000+0x20,1},{0x60000+0x28,0x70000},{0x70000,B+0x3AC3478},{0x70000+0x38,0x80000}};
 contact={wp::Pair{0x2000,0x3000,0x5000,0x10000,5,1},wp::Held{0x99,0x20000,0x40000,0x80000,7,0},1,100,1,0x22,2,0,3};contactNow=true;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,100));check(presses==1&&releases==0&&traces==1); // Off-posture interception needs no prior LT.
 observed.phase=128;contactNow=false;check(NativeWeaponContactParryH24(live,f,enemy,observed,101));check(releases==1&&!g_native_guard112.held);
 observed.phase=2;contact.serial=2;contact.tick=110;contactNow=true;window=0;check(!NativeWeaponContactParryH24(live,f,enemy,observed,110));check(presses==1); // No fabricated PB from a closed native window.
 window=1;contact.serial=3;contact.tick=120;g_native_guard112.held=true;ack=1;check(NativeWeaponContactParryH24(live,f,enemy,observed,120));check(releases==2&&presses==1);
 contactNow=false;check(NativeWeaponContactParryH24(live,f,enemy,observed,121));check(presses==1);ack=0;window=0;check(NativeWeaponContactParryH24(live,f,enemy,observed,122));check(presses==1);
 // Native LT-down callback replaces the manager: no old-frame owner adoption.
 contact.serial=4;contact.tick=150;contactNow=true;window=1;ack=0;callbackChangeManager=true;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,150));check(presses==2&&!g_native_guard112.held);
 callbackChangeManager=false;contactNow=false;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,151));check(releases==3); // Exact entered-input cleanup before a future press.
 mem[0x1000+0x9D0]=0x9000;
 // HMD release is acknowledged ONLY after actual native context0 clear.
 hd::published.publish({0x4000,0x2000,0x3000,0x5000,0x10000,200,5,true});
 g_native_guard112.held=true;check(!NativeHmdGuardYieldH24(live,200));
 hd::Request request{};check(hd::beginRequest(0x4000,5,200,200,request));ack=1;
 check(NativeHmdGuardYieldH24(live,200));check(!g_native_guard112.held&&hd::releaseAck.load()!=request.serial);
 ack=0;check(NativeHmdGuardYieldH24(live,201));check(hd::releaseAck.load()==request.serial);check(!NativeHmdGuardYieldH24(live,321));
 hd::published.publish({0x4000,0x2000,0x3000,0x5000,0x10000,400,5,true});g_native_guard112.held=true;
 check(!NativeHmdGuardYieldH24(live,400));check(hd::beginRequest(0x4000,5,400,400,request));ack=1;check(NativeHmdGuardYieldH24(live,400));
 check(NativeHmdGuardYieldH24(live,521)); // Request expiry is not release ACK.
 ack=0;check(!NativeHmdGuardYieldH24(live,522));
 // A synchronous native callback replacing our lease must not clear it.
 hd::published.publish({0x4000,0x2000,0x3000,0x5000,0x10000,600,5,true});g_native_guard112.held=true;
 check(!NativeHmdGuardYieldH24(live,600));check(hd::beginRequest(0x4000,5,600,600,request));callbackChangeLease=true;
 check(NativeHmdGuardYieldH24(live,600));check(g_native_guard112.held);check(hd::releaseAck.load()!=request.serial);
 callbackChangeLease=false;check(!NativeHmdGuardYieldH24(live,721));check(g_native_guard112.held);
 // Old pending release must not stall a newer owned lease after dodge expiry.
 hd::cancelRequest();ack=1;g_native_guard112.held=true;
 hd::published.publish({0x4000,0x2000,0x3000,0x5000,0x10000,800,5,true});
 check(!NativeHmdGuardYieldH24(live,800));check(hd::beginRequest(0x4000,5,800,800,request));
 check(NativeHmdGuardYieldH24(live,800));g_native_guard112.held=true;++g_native_guard112.leaseH14;
 const int noForeignRelease=releases;check(!NativeHmdGuardYieldH24(live,921));check(releases==noForeignRelease&&g_native_guard112.held);
 // Entered parry press losing its source cannot disable a different actor.
 hd::cancelRequest();g_native_guard112.held=false;ack=0;observed.phase=2;window=1;
 fixtureSource.input.tick=1000;contact.tick=1000;contact.serial=5;contactNow=true;callbackChangeManager=true;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,1000));callbackChangeManager=false;contactNow=false;
 live.actor=reinterpret_cast<void*>(0x2F00);mem[0x1000+0x9D0]=0x9000;
 const int foreignCleanup=releases;check(!NativeWeaponContactParryH24(live,f,enemy,observed,1001));check(releases==foreignCleanup);
 // A later same-actor posture lease is not released through the old contact.
 live.actor=reinterpret_cast<void*>(0x2000);g_native_guard112.owner=live;g_native_guard112.held=false;
 fixtureSource.input.tick=1100;contact.tick=1100;contact.serial=6;contactNow=true;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,1100));check(g_native_guard112.held);
 ++g_native_guard112.leaseH14;g_native_guard112.key=0x77;contactNow=false;observed.phase=128;
 const int postureReleases=releases;
 check(!NativeWeaponContactParryH24(live,f,enemy,observed,1101));check(g_native_guard112.held&&releases==postureReleases);
 // A new same-actor lease while awaiting LT-up cannot be borrowed at ACK0.
 observed.phase=2;g_native_guard112.key=0x22;g_native_guard112.generation=2;
 contact.tick=1200;fixtureSource.input.tick=1200;contact.serial=7;contactNow=true;ack=1;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,1200));
 ++g_native_guard112.leaseH14;g_native_guard112.held=true;ack=0;contactNow=false;
 const int pendingPresses=presses,pendingReleases=releases;
 check(!NativeWeaponContactParryH24(live,f,enemy,observed,1201));
 check(presses==pendingPresses&&releases==pendingReleases&&g_native_guard112.held);
 // Native body/source retirement revokes a contact input before80ms expiry.
 g_native_guard112.held=false;g_native_guard112.owner=live;g_native_guard112.key=0x22;g_native_guard112.generation=2;
 contact.tick=2000;fixtureSource.input.tick=2000;contact.serial=8;contactNow=true;observed.phase=2;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,2000));check(g_native_guard112.held);
 const int physicalRetireReleases=releases;fixtureNativeSourceCurrent=false;contactNow=false;
 check(NativeWeaponContactParryH24(live,f,enemy,observed,2001));check(!g_native_guard112.held&&releases==physicalRetireReleases+1);
 fixtureNativeSourceCurrent=true;
 std::printf("PASS %u actual native contact/HMD delivery checks\n",checks);
}
