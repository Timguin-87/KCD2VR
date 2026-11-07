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
#include "diagnostics/capture168.h"
#include "subsystems/throw_drop/runtime/ammo_eligibility_t20.h"
#include "subsystems/throw_drop/inventory_lifecycle_t8.h"
#include "subsystems/throw_drop/ammo_readiness_k32.h"
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
static bool AmmoTransientPinsT20(std::uintptr_t*) noexcept;
bool SehCopy(const void* p,void* out,std::size_t n){std::memcpy(out,p,n);return true;}
#include "t30_transient.inc"
#include "t30_native.inc"

// This fixture has no bow-owned detour. Only pristine Shoot bytes qualify.
bool ShootPrologueMatchesD65C(const unsigned char* bytes,std::size_t n){return MatchBytes(WhBase()+0x289F7A0,bytes,n);}
#include "t30_paidnative.inc"

struct LeafHook{bool enabled=true;bool is_enabled(){return enabled;}};
std::optional<LeafHook> g_create_ammo_hook_t17{LeafHook{}},g_ammo_category_hook_t20{LeafHook{}},g_ammo_raw_hook_t20{LeafHook{}};
int g_create_ammo_original_t17=1,g_ammo_category_original_t20=1,g_ammo_raw_original_t20=1;
std::atomic<bool> g_native_fault13{false},g_native_ready13{true};bool option=true,currentSource=true;
bool contextCurrent=true,readOK=true;unsigned censusReads=0;
namespace native_script_t2 {struct Context {bool Current()const{return contextCurrent;}};}
namespace ae20=kcd2vr::ammo_eligibility_t20;
namespace inventory_lifecycle_t8=kcd2vr::inventory_lifecycle_t8;
bool AmmoHostReadT20(std::uintptr_t p,void* out,std::size_t n){++censusReads;if(!readOK)return false;std::memcpy(out,reinterpret_cast<void*>(p),n);return true;}
#include "k39_census.inc"
#include "t30_paid.inc"

bool AmmoServiceSelectedT22(){return option;}
namespace adm23 {bool CurrentSource(int,std::uint64_t){return currentSource;}}
namespace st2 {using Input=int;}
struct Context{std::uint64_t now=1000;};int in=0;
std::uint64_t GetTickCount64(){return 1000;}
#include "k39_ready.inc"
bool Selected(){AmmoReadinessK39 r{};return AmmoServiceReadyK39(in,r);}
#include "k39_session.inc"
#include "k39_service_scan.inc"
unsigned noteLinesK39=0;bool g_ammo_lua_ready22=false;
void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){++noteLinesK39;}
void NotesK39(unsigned preflight27,const AmmoReadinessK39& readinessK39,std::uint64_t tick){
 struct Input {bool rightHipPouchD66=false;} in;
 Context ctx{};ctx.now=tick;const bool selected=!std::strcmp(readinessK39.reason,"READY");
#include "k39_notes.inc"
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
const auto entries=WhBase()+0x5500000,row=entries+0x1000,metadata=row+0x1000;
Put(WhBase()+ae20::itemRegistry+0x78,entries);Put(WhBase()+ae20::itemRegistry+0x80,1);
Put(entries+0x10,row);Put(row,WhBase()+ae20::missileItemVft);Put(row+0xE8,metadata);Put(metadata,WhBase()+ae20::missileClassVft);
*reinterpret_cast<unsigned char*>(metadata+0x98)=2;
inventory_lifecycle_t8::state=0;inventory_lifecycle_t8::loadEpisode18=17;
native_script_t2::Context ctx{};bool transient=false;AmmoReadinessK39 r{};
ck(ObserveAmmoMasksT20(ctx,&transient)&&AmmoMaskLeaseT20());ck(AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"READY")&&r.sourceChecked&&r.source);
ck(ArmReloadNative36()&&created==16);ck(AmmoServiceReadyK39(in,r));
// Actual compatible owned reload hook must still qualify. Foreign bytes do not.
image[0x40B730]^=1;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"CARRIER_NATIVE_PIN")&&r.failedRva==0x40B730);image[0x40B730]^=1;
for(auto [address,why]:std::array<std::pair<std::uintptr_t,const char*>,4>{{{0x509810,"CARRIER_NATIVE_PIN"},{0x4533E4,"PAID_NATIVE_PIN"},{0x479790,"PAID_NATIVE_PIN"},{0x3B0C6C8,"CARRIER_NATIVE_PIN"}}}){
 image[address]^=1;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,why)&&r.failedRva==address&&!r.sourceChecked);image[address]^=1;ck(Selected());}
option=false;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"FEATURE_OFF"));option=true;
g_native_ready13=false;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"NATIVE_NOT_READY"));g_native_ready13=true;
g_native_fault13=true;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"NATIVE_GRASP_FAULT"));g_native_fault13=false;
for(auto [hook,why]:std::array<std::pair<LeafHook*,const char*>,3>{{{&*g_create_ammo_hook_t17,"FACTORY_HOOK"},{&*g_ammo_category_hook_t20,"CATEGORY_HOOK"},{&*g_ammo_raw_hook_t20,"RAW_CATEGORY_HOOK"}}}){
 hook->enabled=false;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,why));hook->enabled=true;ck(Selected());}
currentSource=false;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"CURRENT_SOURCE")&&r.sourceChecked&&!r.source);currentSource=true;
++inventory_lifecycle_t8::loadEpisode18;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"AMMO_MASK_LEASE"));ck(ObserveAmmoMasksT20(ctx,&transient)&&Selected());
// Exercise the real scan rather than a Ready stub. Unsafe masks stay refused.
*reinterpret_cast<unsigned char*>(metadata+0x98)=3;ck(!ObserveAmmoMasksT20(ctx,&transient)&&!transient&&g_ammo_masks_t20.status==ae20::MaskStatus::BitZero&&!Selected());
char evidence[192]{};auto reads=censusReads;AmmoMaskEvidenceK39(evidence,sizeof(evidence));ck(std::strstr(evidence,"BIT_ZERO")&&censusReads==reads);
*reinterpret_cast<unsigned char*>(metadata+0x98)=2;ck(ObserveAmmoMasksT20(ctx,&transient)&&Selected());
Put(metadata,0);ck(!ObserveAmmoMasksT20(ctx,&transient)&&!transient&&g_ammo_masks_t20.status==ae20::MaskStatus::UnknownMissileClass&&!Selected());Put(metadata,WhBase()+ae20::missileClassVft);
readOK=false;ck(!ObserveAmmoMasksT20(ctx,&transient)&&transient&&!Selected());readOK=true;ck(ObserveAmmoMasksT20(ctx,&transient)&&Selected());
Put(WhBase()+ae20::itemRegistry+0x80,65537);ck(!ObserveAmmoMasksT20(ctx,&transient)&&!transient&&g_ammo_masks_t20.status==ae20::MaskStatus::TooLarge);Put(WhBase()+ae20::itemRegistry+0x80,1);ck(ObserveAmmoMasksT20(ctx,&transient)&&Selected());
g_create_ammo_original_t17=0;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"FACTORY_HOOK"));g_create_ammo_original_t17=1;
g_ammo_category_original_t20=0;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"CATEGORY_HOOK"));g_ammo_category_original_t20=1;
g_ammo_raw_original_t20=0;ck(!AmmoServiceReadyK39(in,r)&&!std::strcmp(r.reason,"RAW_CATEGORY_HOOK"));g_ammo_raw_original_t20=1;
ck(Selected());ShutdownReloadNative36();ck(Selected());
// Actual service + actual census: recovered ordinary-pickup fault must rearm.
g_ammo_hooks_ready22=true;g_ammo_masks_scanned22=~std::uint64_t(0);g_native_fault13=true;
ck(AmmoServiceReadinessWantedT22());auto beforeReads=censusReads;AmmoServiceReadinessT22(ctx);
ck(!Selected()&&!g_ammo_retry_transient22&&censusReads==beforeReads);
for(unsigned n=0;n<10;++n)ck(!AmmoServiceReadinessWantedT22());
g_native_fault13=false; // substitute ONLY the original native owner's proven settle/clear
ck(AmmoServiceReadinessWantedT22()); // CAUSAL_RECOVERED_FAULT
AmmoServiceReadinessT22(ctx);ck(Selected()&&!g_ammo_retry_owner_fault_k39&&!AmmoServiceReadinessWantedT22());
beforeReads=censusReads;for(unsigned n=0;n<10;++n){AmmoServiceReadinessT22(ctx);ck(censusReads==beforeReads);}
// A second actual owner fault does not erase already completed mask census.
g_native_fault13=true;ck(!Selected()&&!AmmoServiceReadinessWantedT22());g_native_fault13=false;ck(Selected());
// Fault-clear retry cannot forgive immutable schema or eligibility refusals.
++inventory_lifecycle_t8::loadEpisode18;g_native_fault13=true;AmmoServiceReadinessT22(ctx);g_native_fault13=false;
*reinterpret_cast<unsigned char*>(metadata+0x98)=3;AmmoServiceReadinessT22(ctx);
ck(!Selected()&&!g_ammo_retry_owner_fault_k39&&!g_ammo_retry_transient22&&!AmmoServiceReadinessWantedT22());
inventory_lifecycle_t8::state=2;ck(!AmmoServiceReadinessWantedT22());
*reinterpret_cast<unsigned char*>(metadata+0x98)=2;++inventory_lifecycle_t8::loadEpisode18;AmmoServiceReadinessT22(ctx);ck(Selected());
// Execute the actual note budget block: quiet startup cannot spend F5 receipts.
namespace cap=kcd2vr::diagnostics::capture168;
cap::gate.mask=0;
for(unsigned n=0;n<40;++n)NotesK39(n,r,1000+n*1100);ck(noteLinesK39==0);
cap::gate.select(cap::Domain::Interaction,true,1000);NotesK39(47,r,1000);ck(noteLinesK39==2);
NotesK39(47,r,2100);ck(noteLinesK39==2);
r.reason="PAID_NATIVE_PIN";r.failedRva=0x479790;NotesK39(47,r,3200);ck(noteLinesK39==4);
cap::gate.select(cap::Domain::Interaction,false,1000);NotesK39(47,r,4300);ck(noteLinesK39==4);
cap::gate.select(cap::Domain::Interaction,true,1000);NotesK39(47,r,5400);ck(noteLinesK39==6);
for(unsigned n=0;n<40;++n)NotesK39(n,r,6500+n*1100);ck(noteLinesK39==36);
cap::gate.select(cap::Domain::Interaction,false,1000);cap::gate.select(cap::Domain::Reload,true,1000);NotesK39(47,r,60000);ck(noteLinesK39==38);
cap::gate.select(cap::Domain::Reload,false,1000);
ck(!std::strcmp(AmmoReadinessTextK39("NATIVE_GRASP_FAULT"),"knives: waiting for the previous pickup to finish"));
ck(!std::strcmp(AmmoReadinessTextK39("AMMO_MASK_LEASE"),"knives: waiting for ammunition checks"));
ck(!std::strcmp(AmmoReadinessTextK39("CURRENT_SOURCE"),"knives: waiting for the current hand pose"));
ck(!std::strcmp(AmmoReadinessTextK39("PAID_NATIVE_PIN"),"knives: native knife support unavailable"));
ck(!std::strcmp(AmmoReadinessTextK39("FEATURE_OFF"),"knives: enable the left-hand knife pouch"));
VirtualFree(image,0,MEM_RELEASE);
std::printf("{\"checks\":%u,\"realDmkMidHooks\":16,\"actualReadiness\":true,\"actualCensus\":true,\"game\":false}\n",checks);
}
