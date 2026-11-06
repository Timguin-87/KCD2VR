// Actual owned reload installation + actual native knife pin/paid predicates.
// Real DMK mid hooks operate only on an allocated private test image, never the game.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <DetourModKit/hook.hpp>
#include <array>
#include <atomic>
#include <optional>
#include <vector>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "diagnostics/reload_lifecycle178r36c2.h"
namespace kcd2vr::frik_pose_probe {bool NativeReloadHandRemoveEntryQualifiedT30(std::uintptr_t) noexcept;}
namespace fixture {
namespace rl36=kcd2vr::diagnostics::reload36c2;
using ReloadContext36=DetourModKit::hook::MidContext;
using Hook=DetourModKit::hook::Hook;
unsigned char* image=nullptr;bool retailOK=true,copyOK=true;unsigned created=0,failFactory=~0u;
std::uintptr_t WhBase(){return reinterpret_cast<std::uintptr_t>(image);}
namespace retail_build112 {bool Ready(std::uintptr_t b){return b&&b==WhBase()&&retailOK;}bool Verify(std::uintptr_t b){return Ready(b);}}
template<class T>bool Read(const void* p,T& v){if(!copyOK)return false;std::memcpy(&v,p,sizeof(v));return true;}
template<class T>T Rd(void* p,std::size_t off,T fallback){T v{};if(!p)return fallback;std::memcpy(&v,static_cast<unsigned char*>(p)+off,sizeof(v));return v;}
bool MatchBytes(std::uintptr_t p,const unsigned char* b,std::size_t n){return !std::memcmp(reinterpret_cast<void*>(p),b,n);}
void Put(std::uintptr_t p,std::uintptr_t v){std::memcpy(reinterpret_cast<void*>(p),&v,8);}
bool ReloadCallerPins37(std::uintptr_t){return false;} // separate observer expansion not needed by owner
std::uintptr_t g_reload_attempted36=0;
std::atomic<bool> g_reload_ready36{false},g_reload_enabled36{false};bool g_reload_callers37=false;
void EndReloadNative36(const char*){}
#include "t30_callbacks.inc"
struct ReloadSite36 {std::uintptr_t rva;std::array<unsigned char,16> bytes;void(*callback)(ReloadContext36&) noexcept;};
#include "t30_sites.inc"
auto TestMidAt(DetourModKit::hook::MidRequest request,void(*callback)(ReloadContext36&) noexcept){
 if(created++==failFactory)return DetourModKit::Result<Hook>{std::unexpected(DetourModKit::Error{DetourModKit::ErrorCode::InvalidHookState,"test factory refusal"})};
 return DetourModKit::hook::mid_at(std::move(request),callback);
}
#include "t30_owner.inc"
#include "t30_shutdown.inc"
static bool AmmoTransientPinsT20() noexcept;
bool SehCopy(const void* p,void* out,std::size_t n){std::memcpy(out,p,n);return true;}
#include "t30_transient.inc"
#include "t30_native.inc"
#include "t30_oldnative.inc"
#include "t30_paidnative.inc"
#include "t30_oldpaidnative.inc"
struct LeafHook{bool enabled=true;bool is_enabled(){return enabled;}};
std::optional<LeafHook> g_create_ammo_hook_t17{LeafHook{}},g_ammo_category_hook_t20{LeafHook{}},g_ammo_raw_hook_t20{LeafHook{}};
int g_create_ammo_original_t17=1,g_ammo_category_original_t20=1,g_ammo_raw_original_t20=1;
std::atomic<bool> g_native_fault13{false},g_native_ready13{true};bool mask=true,option=true,currentSource=true;
bool AmmoMaskLeaseT20(){return mask;}
#include "t30_paid.inc"
#include "t30_oldpaid.inc"
bool AmmoServiceSelectedT22(){return option;}
namespace adm23 {bool CurrentSource(int,std::uint64_t){return currentSource;}}
struct Context{std::uint64_t now=1000;};int in=0;
std::uint64_t GetTickCount64(){return 1000;}
bool Selected(bool old){
 if(old){return AmmoServiceSelectedT22()&&g_native_ready13.load()&&OldAmmoNativePinsT18()&&OldAmmoPaidPinsT20()&&adm23::CurrentSource(in,GetTickCount64());}
 return AmmoServiceSelectedT22()&&g_native_ready13.load()&&AmmoNativePinsT18()&&AmmoPaidPinsT20()&&adm23::CurrentSource(in,GetTickCount64());
}
}
namespace kcd2vr::frik_pose_probe {
using namespace fixture;
#include "t30_query.inc"
}
int main(){using namespace fixture;unsigned checks=0;auto ck=[&](bool ok){++checks;if(!ok)std::fprintf(stderr,"FAILED check=%u\n",checks);assert(ok);};
image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x6000000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));assert(image);
std::memset(image,0x90,0x6000000);
struct Pin{std::uintptr_t rva;std::vector<unsigned char> bytes;};const Pin pins[]{
#include "t30_pins.inc"
};for(auto& p:pins)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
Put(WhBase()+0x3A83DC0+0x80,WhBase()+0x838AE0);Put(WhBase()+0x3B0C698+0x30,WhBase()+0xD85828);Put(WhBase()+0x3B0C698+8,WhBase()+0x1A724F0);
ck(Selected(true)&&Selected(false));ck(!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase()));
ck(ArmReloadNative36()&&created==16&&g_reload_hooks36[7]&&g_reload_hooks36[7]->is_enabled());
std::array<unsigned char,16> patch{};std::memcpy(patch.data(),image+0x40B730,16);
ck(std::memcmp(patch.data(),rl36::kHandRemovePristineT30,16)!=0);
ck(!OldAmmoNativePinsT18()&&!OldAmmoPaidNativePinsT20()&&!Selected(true));
ck(AmmoNativePinsT18()&&AmmoPaidNativePinsT20()&&Selected(false));
ck(kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase()));
// The selector cannot bypass any other live admission after resolving this one conflict.
mask=false;ck(!Selected(false));mask=true;g_native_fault13=true;ck(!Selected(false));g_native_fault13=false;
g_create_ammo_hook_t17->enabled=false;ck(!Selected(false));g_create_ammo_hook_t17->enabled=true;
// A foreign patch cannot be PUBLISHED as ours: the real backend rejects it.
image[0x40B730]^=1;
ck(!g_reload_hand_entry_t30.publish(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),true,CopyReloadHandEntryT30,[]{return bool(g_reload_hooks36[7]->enable());}));
std::memcpy(image+0x40B730,patch.data(),16);
ck(g_reload_hand_entry_t30.publish(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),true,CopyReloadHandEntryT30,[]{return bool(g_reload_hooks36[7]->enable());})&&Selected(false));
image[0x479790]^=1;ck(!Selected(false));image[0x479790]^=1;
Put(WhBase()+0x3B0C698+0x30,0);ck(!Selected(false));Put(WhBase()+0x3B0C698+0x30,WhBase()+0xD85828);
// Every FOREIGN change remains a mismatch, even while our observer object exists.
for(unsigned offset:{0u,7u,15u}){image[0x40B730+offset]^=0x01;ck(!Selected(false));image[0x40B730+offset]^=0x01;}
copyOK=false;ck(!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase())&&!Selected(false));copyOK=true;
ck(!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase()+1));
ck(!g_reload_hand_entry_t30.qualifies(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandBegin36),CopyReloadHandEntryT30));
retailOK=false;ck(!Selected(false));retailOK=true;
g_reload_hand_entry_t30.clear();ck(!Selected(false)); // actual enabled patch without owner receipt is denied
ck(g_reload_hand_entry_t30.publish(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),true,CopyReloadHandEntryT30,[]{return bool(g_reload_hooks36[7]->enable());}));
ck(Selected(false));
ck(!g_reload_hand_entry_t30.qualifies(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),
    [](std::uintptr_t at,void* out,std::size_t n){const bool ok=CopyReloadHandEntryT30(at,out,n);g_reload_hand_entry_t30.clear();return ok;}));
ck(g_reload_hand_entry_t30.publish(WhBase(),reinterpret_cast<std::uintptr_t>(&ReloadHandRemove36),true,CopyReloadHandEntryT30,[]{return bool(g_reload_hooks36[7]->enable());}));
// Owner OFF: witness revoked before native prologue restoration; native pristine remains valid.
ShutdownReloadNative36();ck(!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase()));
ck(Selected(false)&&std::memcmp(image+0x40B730,rl36::kHandRemovePristineT30,16)==0);
created=0;failFactory=8;ck(!ArmReloadNative36());ck(!kcd2vr::frik_pose_probe::NativeReloadHandRemoveEntryQualifiedT30(WhBase())&&Selected(false)); // actual rollback revokes
ShutdownReloadNative36();failFactory=~0u;
// Publisher rejects unverified native source, failed owner verification and changed readback.
rl36::HandRemoveEntryT30 separate;
std::memcpy(image+0x40B730,patch.data(),16);
ck(!separate.publish(WhBase(),7,false,CopyReloadHandEntryT30,[]{return true;}));
ck(!separate.publish(WhBase(),7,true,CopyReloadHandEntryT30,[]{return false;}));
ck(!separate.publish(WhBase(),7,true,CopyReloadHandEntryT30,[&]{image[0x40B730]^=1;return true;}));
std::memcpy(image+0x40B730,rl36::kHandRemovePristineT30,16);
ck(!separate.publish(WhBase(),7,true,CopyReloadHandEntryT30,[]{return true;}));
VirtualFree(image,0,MEM_RELEASE);
std::printf("{\"checks\":%u,\"realDmkMidHooks\":16,\"realOwnedHook\":\"40B730\",\"oldSelector\":false,\"correctedSelector\":true,\"game\":false}\n",checks);
}
