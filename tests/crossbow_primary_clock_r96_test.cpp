// Literal production primary placement-retention function and real mailbox.
// Native owner/binding/readback are controlled leaves. A deterministic hook
// models publication during the earlier native-owner check, without sleeps.
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include "vrik/pose_math.h"
#include <array>
#include <cstdio>
#include <cstring>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
namespace xcXR34=kcd2vr::crossbow_native_xr34;
namespace pm=kcd2vr::pose_core;
#include "crossbow_primary_types_r96.inc"
static kcd2vr::core::SnapshotMailbox<RangedPlacedXR34> g_ranged_primary_placed_r95;
static std::uint64_t clockR96=1000;
static unsigned nativeChecksR96=0;
static bool nativeOwnerR96=true,claimFreeR96=true,placementCurrentR96=true,bindingCurrentR96=true,finalOwnerR96=true;
static std::array<unsigned char,32> selfR96{};
static int attachmentR96=0,entityR96=0,itemR96=0;
static xcXR34::Owner ownerR96{};
static std::function<void()> duringNativeOwnerR96;
static std::uint64_t GetTickCount64(){return clockR96;}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner& owner,bool installed) noexcept {
 ++nativeChecksR96;if(duringNativeOwnerR96)duringNativeOwnerR96();
 return installed&&nativeOwnerR96&&xcXR34::sameOwner(owner,ownerR96);
}
namespace kcd2vr::reload_throw_claim_t25 {
bool workingHandUnclaimed(const xcXR34::Source&) noexcept{return claimFreeR96;}
}
static bool RangedPlacementStillCurrentXR34(const RangedPlacedXR34&) noexcept{return placementCurrentR96;}
template<class T> bool Read(const void* at,T& out){
 if(at!=selfR96.data()+8||sizeof(T)!=sizeof(unsigned))return false;
 std::memcpy(&out,at,sizeof(T));return true;
}
static bool RangedBoundWeaponIdentityD72(const xcXR34::Owner& owner,unsigned id,void* entity,void*& bound) noexcept {
 bound=&attachmentR96;return bindingCurrentR96&&xcXR34::sameOwner(owner,ownerR96)&&id==77&&entity==&entityR96;
}
static bool RangedOwnerXR34(const xcXR34::Owner& owner) noexcept{return finalOwnerR96&&xcXR34::sameOwner(owner,ownerR96);}
#include "crossbow_primary_function_r96.inc"

static RangedPlacedXR34 Receipt(std::uint64_t tick){
 RangedPlacedXR34 p{};p.owner=ownerR96;p.tick=tick;p.valid=true;p.bodyPublicationR95=2;
 p.id=77;p.entity=&entityR96;p.toolItem=&itemR96;p.toolKey=ownerR96.source.key;
 p.socketR95.self=selfR96.data();p.socketR95.attachment=&attachmentR96;return p;
}
static bool Retain(){return RangedRetainPrimaryPlacementR95(ownerR96,selfR96.data(),&attachmentR96,77,&entityR96);}
static void Check(bool condition,const char* reason){if(!condition)throw std::runtime_error(reason);}
static void Reset(){
 clockR96=1000;nativeChecksR96=0;duringNativeOwnerR96={};
 nativeOwnerR96=claimFreeR96=placementCurrentR96=bindingCurrentR96=finalOwnerR96=true;
 ownerR96={};ownerR96.serial=9;ownerR96.main=0x100;ownerR96.parent=0x200;ownerR96.child=0x300;ownerR96.token=17;
 ownerR96.source.key=123;ownerR96.source.weaponItem=reinterpret_cast<std::uintptr_t>(&itemR96);
 const unsigned id=77;std::memcpy(selfR96.data()+8,&id,sizeof(id));
 Check(g_ranged_primary_placed_r95.publish(Receipt(980)),"fixture mailbox publish failed");
}
int main(){
 struct Test {const char* name;std::function<void()> fn;};
 const std::vector<Test> tests={
  {"existing_fresh_receipt",[]{Check(Retain(),"ordinary fresh receipt rejected");}},
  {"publication_during_native_owner_check",[]{
    duringNativeOwnerR96=[]{clockR96=1002;Check(g_ranged_primary_placed_r95.publish(Receipt(1001)),"concurrent receipt publish failed");};
    Check(Retain(),"later valid receipt was rejected against a clock sampled before publication");
    RangedPlacedXR34 after{};Check(nativeChecksR96==1&&g_ranged_primary_placed_r95.read(after)&&after.tick==1001,
        "retention renewed the original receipt age or bypassed native-owner qualification");
  }},
  {"eighty_ms_boundary",[]{g_ranged_primary_placed_r95.publish(Receipt(920));Check(Retain(),"existing 80ms bound narrowed");}},
  {"stale_receipt_refused",[]{g_ranged_primary_placed_r95.publish(Receipt(919));Check(!Retain(),"81ms receipt accepted");}},
  {"truly_future_receipt_refused",[]{g_ranged_primary_placed_r95.publish(Receipt(1001));Check(!Retain(),"future tick accepted");}},
  {"changed_receipt_owner_refused",[]{auto p=Receipt(980);++p.owner.serial;g_ranged_primary_placed_r95.publish(p);Check(!Retain(),"replacement reload borrowed prior placement");}},
  {"changed_native_owner_refused",[]{nativeOwnerR96=false;Check(!Retain(),"native owner refusal bypassed");}},
  {"missing_body_publication_refused",[]{auto p=Receipt(980);p.bodyPublicationR95=0;g_ranged_primary_placed_r95.publish(p);Check(!Retain(),"unpublished placement accepted");}},
  {"changed_attachment_refused",[]{auto p=Receipt(980);p.socketR95.attachment=&entityR96;g_ranged_primary_placed_r95.publish(p);Check(!Retain(),"replacement attachment borrowed placement");}},
  {"readback_moved_refused",[]{placementCurrentR96=false;Check(!Retain(),"moved destination accepted");}},
  {"native_binding_lost_refused",[]{bindingCurrentR96=false;Check(!Retain(),"lost native binding accepted");}},
  {"working_hand_claim_refused",[]{claimFreeR96=false;Check(!Retain(),"competing working-hand claim accepted");}},
  {"final_owner_loss_refused",[]{finalOwnerR96=false;Check(!Retain(),"owner loss after receipt read ignored");}}
 };
 unsigned passed=0;
 for(const auto& t:tests){Reset();try{t.fn();++passed;std::printf("PASS %s\n",t.name);}catch(const std::exception& e){std::printf("FAIL %s: %s\n",t.name,e.what());}}
 std::printf("R96 actual primary retain clock %u/%zu; native/scheduler leaves controlled; game/HMD NOT_RUN\n",passed,tests.size());
 return passed==tests.size()?0:1;
}
