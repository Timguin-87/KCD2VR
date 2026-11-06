#include "held_map_m1.h"
#include "held_map_pose_m1.h"
#include "../paper_grip_d64.h"
#include "../native_paper_m43.h"
#include "../map_fold_m39.h"
#include "held_map_feed_t2.h"
#include "frik_pose_probe.h"
#include "retail_build112.h"
#include "subsystems/ranged_reload/entity_hide_pin_r125.h" // R125
#include "native_stereo_outer_lab.h"
#include "diagnostic_trace.h"
#include "combat_diag_probe.h"
#include "diagnostics/performance_metrics_d61.h"
#include <windows.h>
#include <cstring>
#include <cstdio>
namespace kcd2vr::held_map_m1 {namespace {
std::atomic<bool> ready{false},shown{false};
std::atomic<std::uint64_t> placed{0},showN{0},hideN{0},missingN{0},staleN{0};
std::atomic<unsigned> poseRefusal19c3{0},lastHideReasonM4{0};
std::uintptr_t base=0;
std::atomic_flag busy=ATOMIC_FLAG_INIT;
constexpr const char* names[]{"KCD2VR_StaticPaper_M41","KCD2VR_StaticPaper_M41"};
constexpr const char* regionNames[]{"Trosky","Kuttenberg"};
bool Copy(std::uintptr_t from,void* to,std::size_t n) noexcept {
 // These bounded exact-retail scalar reads already have an SEH boundary.
 // VirtualQuery for every field repeated expensive kernel queries throughout
 // each paper placement; the fault boundary remains authoritative on a race.
 if(from<0x10000||!to||!n||from+n<from)return false;
 __try{std::memcpy(to,reinterpret_cast<void*>(from),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Pin(std::uintptr_t rva,const unsigned char* bytes,unsigned n) noexcept {
 unsigned char actual[16]{};return n<=sizeof(actual)&&Copy(base+rva,actual,n)&&!std::memcmp(actual,bytes,n);
}
// R125: CEntity::Hide may carry the F2 recorder's D77 mid hook; same identity rule as EntityHide59c14.
bool HidePinR125() noexcept {
 namespace ep=::kcd2vr::entity_hide_pin_r125;std::uintptr_t slot=0;unsigned char actual[16]{};
 return Copy(base+ep::kEntityVftRva+ep::kHideSlot,&slot,8)&&Copy(base+ep::kHideRva,actual,16)&&ep::accept(base,slot,actual);
}
void* FindRawD72(PrintedRegion region,unsigned* refusal=nullptr) noexcept {
 if(refusal)*refusal=0;
 const char* name=names[static_cast<unsigned>(region)];
 std::uintptr_t system=0,vt=0,fn=0;
 if(!Copy(base+0x492D8A0,&system,8)||!Copy(system,&vt,8)||vt!=base+0x4077048||!Copy(vt+0x80,&fn,8)||fn!=base+0xA6D6F4){if(refusal)*refusal=1;return nullptr;}
 void* entity=nullptr;
 __try{entity=reinterpret_cast<void*(__fastcall*)(void*,const char*)>(fn)(reinterpret_cast<void*>(system),name);}
 __except(EXCEPTION_EXECUTE_HANDLER){if(refusal)*refusal=2;return nullptr;}
 if(!entity){if(refusal)*refusal=3;return nullptr;}
 if(!Copy(reinterpret_cast<std::uintptr_t>(entity),&vt,8)||vt!=base+0x4072C78){if(refusal)*refusal=4;return nullptr;}
 return entity;
}
// D82 build fix: MSVC C2712 - __try cannot live in Find(), it owns the timing Scope (a destructor).
// same SEH call, own function without objects, like FindRawD72/HideRawD72.
void* LookupIdRawD82(std::uintptr_t fn,std::uintptr_t system,std::uint32_t id) noexcept {
 __try{return reinterpret_cast<void*(__fastcall*)(void*,std::uint32_t)>(fn)(reinterpret_cast<void*>(system),id);}
 __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
void* Find(PrintedRegion region,unsigned* refusal=nullptr) noexcept {
 diagnostics::map_d72::Scope timing{diagnostics::map_d72::Find};
 // Resolve the name once per map activation. Thereafter rejoin the exact
 // registry ID every frame; an unreadable/replaced entity never reuses a pointer.
 struct Cache {std::uint64_t serial=0;std::uint32_t id=0;void* entity=nullptr;};
 static Cache cache{};
 const auto serial=activation.load(std::memory_order_acquire);
 if(cache.entity&&cache.id&&cache.serial==serial){
  std::uintptr_t system=0,vt=0,fn=0;void* current=nullptr;
  if(Copy(base+0x492D8A0,&system,8)&&Copy(system,&vt,8)&&vt==base+0x4077048&&
     Copy(vt+0x70,&fn,8)&&fn==base+0x4B4F98){
   current=LookupIdRawD82(fn,system,cache.id);
  }
  std::uint32_t id=0;
  if(current==cache.entity&&Copy(reinterpret_cast<std::uintptr_t>(current),&vt,8)&&vt==base+0x4072C78&&
     Copy(reinterpret_cast<std::uintptr_t>(current)+0xC,&id,4)&&id==cache.id){if(refusal)*refusal=0;return current;}
 }
 cache={};
 void* found=FindRawD72(region,refusal);std::uint32_t id=0;
 if(found&&Copy(reinterpret_cast<std::uintptr_t>(found)+0xC,&id,4)&&id)cache={serial,id,found};
 return found;
}
bool Hidden(void* entity,bool& hidden) noexcept {
 unsigned flags=0;if(!Copy(reinterpret_cast<std::uintptr_t>(entity)+8,&flags,4))return false;hidden=(flags&0x10)!=0;return true;
}
bool HideRawD72(void* entity,bool hidden) noexcept {
 __try{reinterpret_cast<void(__fastcall*)(void*,bool)>(base+0x87963C)(entity,hidden);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool Hide(void* entity,bool hidden) noexcept {
 diagnostics::map_d72::Scope timing{diagnostics::map_d72::Visibility};return HideRawD72(entity,hidden);
}
bool PlaceRawD67(void* entity,const float* qs,unsigned& refusal) noexcept {
 // INVESTIGATE_D67R1 MAP_NATIVE_COST: exact WHGame SHA bdf8f9e4...4479f7,
 // SetPosRotScale RVA 0x509810 compares PRS at +0x30/+0x3c/+0x4c and calls
 // transform invalidation 0x508c00 only when values differ (0x509917-0x50991f).
 // Thus D67's identical-pose cache below saves call/read overhead; it does not
 // prove removal of shadow or scene invalidations on already-identical poses.
 // Join MAP_TICK/MAP_PLACEMENT with the native frame and GPU scene/shadow pass
 // for this exact helper and slots 0/1/2 before changing render/material flags.
 // Source receipts and the next decision are in repair_evidence/d67r1/map/.
 refusal=0;
 std::uintptr_t vt=0,fn=0;const auto ptr=reinterpret_cast<std::uintptr_t>(entity);
 if(!Copy(ptr,&vt,8)||vt!=base+0x4072C78||!Copy(vt+0x168,&fn,8)||fn!=base+0x509810){refusal=1;return false;}
 const float scale[3]{qs[7],qs[7],qs[7]};
 __try{reinterpret_cast<void(__fastcall*)(void*,const float*,const float*,const float*,unsigned)>(fn)(entity,qs+4,qs,scale,0x10);}
 __except(EXCEPTION_EXECUTE_HANDLER){refusal=2;return false;}
 // M41 uses a static CGF/CGFM slot, not an ICharacterInstance.
 return true;
}
bool Place(void* entity,const float* qs,unsigned& refusal) noexcept {
 diagnostics::perf_d61::Scope placementCostD67{diagnostics::perf_d61::MapPlacement};
 diagnostics::map_d72::Scope nativeTransform{diagnostics::map_d72::Transform};
 return PlaceRawD67(entity,qs,refusal);
}
void* StaticSlotM41(void* entity) noexcept {
 __try{return reinterpret_cast<void*(__fastcall*)(void*,int)>(base+0x7F1E3C)(entity,0);}
 __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
const char* StaticFilenameM41(void* stat) noexcept {
 __try{return reinterpret_cast<const char*(__fastcall*)(void*)>(base+0x1A81E20)(stat);}
 __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
bool FilenameMatchesM43(const char* file,const char* expected) noexcept {
 if(!file||!expected)return false;
 char actual[160]{};bool terminated=false;
 // One region query, then bounded scalar reads only THROUGH the terminator.
 // Bulk-copying160 bytes from a40-byte native string overreads its object.
 for(unsigned offset=0;offset<sizeof(actual)&&!terminated;){
  const auto address=reinterpret_cast<std::uintptr_t>(file)+offset;MEMORY_BASIC_INFORMATION info{};
  if(!VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info))||info.State!=MEM_COMMIT||(info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return false;
  const auto available=info.RegionSize-(address-reinterpret_cast<std::uintptr_t>(info.BaseAddress));
  const auto count=std::min<std::size_t>(available,sizeof(actual)-offset);if(!count)return false;
  __try{for(unsigned j=0;j<count;++j){char c=file[offset+j];if(!c){terminated=true;break;}
   if(c=='\\')c='/';if(c>='A'&&c<='Z')c=char(c-'A'+'a');actual[offset+j]=c;}}
  __except(EXCEPTION_EXECUTE_HANDLER){return false;}
  offset+=unsigned(count);
 }
 return terminated&&!std::strcmp(actual,expected);
}
bool StaticModelReadyRawD72(void* entity,const char* expected) noexcept {
 if(!entity||!expected||!*expected)return false;
 void* stat=StaticSlotM41(entity);std::uintptr_t vt=0;
 if(!stat||!Copy(reinterpret_cast<std::uintptr_t>(stat),&vt,8)||vt!=base+0x3A2F888)return false;
 return FilenameMatchesM43(StaticFilenameM41(stat),expected)&&StaticSlotM41(entity)==stat;
}
bool StaticModelReadyM41(void* entity,const char* expected) noexcept {
 diagnostics::map_d72::Scope timing{diagnostics::map_d72::Model};return StaticModelReadyRawD72(entity,expected);
}
#include "native_paper_owner_m44.inl"
}
bool Install() noexcept {
 if(ready.load())return held_map_feed_t2::Install();
 base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
 constexpr unsigned char find[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x18,0x48,0x89,0x54,0x24,0x10,0x56};
 constexpr unsigned char prs[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x45};
 constexpr unsigned char staticSlotM41[]{0x40,0x53,0x48,0x83,0xec,0x20,0x8b,0xda,0x33,0xd2,0xe8,0x79,0x52,0xde,0xff,0x4c};
 constexpr unsigned char staticFileM41[]{0x48,0x8b,0x81,0xa0,0x0,0x0,0x0,0xc3,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc};
 constexpr unsigned char byIdD81[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x8b,0xfa,0x48,0x8b,0xd9,0xb8};
 const bool ok=retail_build112::Verify(base)&&Pin(0x4B4F98,byIdD81,sizeof(byIdD81))&&Pin(0xA6D6F4,find,sizeof(find))&&HidePinR125()&&Pin(0x509810,prs,sizeof(prs))&&Pin(0x7F1E3C,staticSlotM41,sizeof(staticSlotM41))&&Pin(0x1A81E20,staticFileM41,sizeof(staticFileM41))&&native_stereo_outer_lab::EnsurePhysicalFramePump6698()&&held_map_feed_t2::Install();
 ready.store(ok);diagnostic_trace::Write(ok?"M41 STATIC_PAPER exact static placement/model readback ready; RIGHT socket; baked diffuse; no animation/UI surface":"T2 HELD_MAP unavailable: exact retail pins, native frame or script scheduler failed");return ok;
}
void NativeFrameTick() noexcept {
 const auto currentNow=std::uint64_t(GetTickCount64());
 const auto renderer=RendererIntent43();
 if((!renderer.native||!renderer.open)&&!PaperCleanupCompleteM43())native_paper_m43::RequestRetire();
 PumpPaperM43(); // retain exact cleanup while OFF or closed; native-frame only

 if(!ready.load()||(!Requested()&&!shown.load())){
  if(native_paper_m43::visible.load()||native_paper_m43::hidePending.load())HidePaperM43();
  held_map_feed_t2::RequestReadiness(0,0,0,currentNow);return;
 }
 diagnostics::perf_d61::Scope mapCostD67{diagnostics::perf_d61::MapTick};
 if(busy.test_and_set(std::memory_order_acquire))return;
 struct Unlock{~Unlock(){busy.clear(std::memory_order_release);}}unlock;
 const auto now=std::uint64_t(GetTickCount64());
 float hand[8]{},pose[8]{};unsigned refusal=0;
 std::uintptr_t source=0;std::uint64_t epoch=0,tick=0;
 const bool usable=Selected()&&Requested()&&Allowed(now)&&
  frik_pose_probe::ReadHeldMapRightPoseM41(hand,&refusal,&source,&epoch,&tick)&&held_map_pose_m1::Place(hand,pose);
 poseRefusal19c3.store(refusal);
 // D64 final placement uses the selected artwork dimensions and RIGHT edge anchor.
 std::uint32_t actor=0;std::uintptr_t vt=0;
 if(usable&&source&&epoch&&Copy(source,&vt,8)&&vt==base+0x4072C78)Copy(source+0xC,&actor,4);
 static std::uint64_t next=0,serial=~0ull;static unsigned attempts=0;
 static std::uint32_t ownedIdM41=0;static std::uintptr_t ownedEntityM41=0;
 const auto currentSerial=renderer.serial;if(serial!=currentSerial){serial=currentSerial;next=0;attempts=0;}
 // When visible we retain the exact hide obligation even after B/owner loss.
 if(!shown.load()&&(now<next||attempts>=70))return;
 if(!usable&&!shown.load())return;
 unsigned why=0;void* entity=Find(PrintedRegion::Trosky,&why);
 if(!entity){
  if(why==3){shown.store(false);if(usable&&actor&&epoch){(void)held_map_feed_t2::ReportHelperMissing(actor,epoch,currentSerial);held_map_feed_t2::RequestReadiness(actor,epoch,tick,now,0);}}
  ++missingN;next=now+200;++attempts;
  if(attempts==1||attempts==70){char line[180]{};std::snprintf(line,sizeof(line),"M41 STATIC_PAPER helperMissing reason=%u attempt=%u/70",why,attempts);diagnostic_trace::Write(line);}return;
 }
 std::uint32_t helperId=0;if(!Copy(reinterpret_cast<std::uintptr_t>(entity)+0xC,&helperId,4)||!helperId)return;
 if(usable&&actor&&epoch)held_map_feed_t2::RequestReadiness(actor,epoch,tick,now,helperId);
 char expected[160]{};
 const bool acknowledged=held_map_feed_t2::StaticModelM41(actor,epoch,currentSerial,helperId,expected);
 const bool original=ownedIdM41==helperId&&ownedEntityM41==reinterpret_cast<std::uintptr_t>(entity);
 // Name/VFT/model alone never permits touching a replacement entity. A closed
 // Lua receipt retires the original utility if its named successor is foreign.
 if(!acknowledged&&!original){if(!Requested()&&closedAt31.load()==currentSerial)shown.store(false);next=now+200;++attempts;return;}
 if(acknowledged){ownedIdM41=helperId;ownedEntityM41=reinterpret_cast<std::uintptr_t>(entity);}
 bool hidden=true;if(!Hidden(entity,hidden))return;
 const auto sameHelperM41=[&]()noexcept{std::uintptr_t liveVt=0;std::uint32_t liveId=0;
  return Copy(reinterpret_cast<std::uintptr_t>(entity),&liveVt,8)&&liveVt==base+0x4072C78&&
   Copy(reinterpret_cast<std::uintptr_t>(entity)+0xC,&liveId,4)&&liveId==helperId;};
 const auto sameAckM41=[&]()noexcept{char currentPath[160]{};
  return sameHelperM41()&&held_map_feed_t2::StaticModelM41(actor,epoch,currentSerial,helperId,currentPath)&&!std::strcmp(currentPath,expected);};
 const auto hideCurrent=[&]()noexcept{HidePaperM43();if(!sameHelperM41())return;if(!hidden&&Hide(entity,true)){hidden=true;++hideN;}shown.store(!hidden);};
 if(!usable||!actor||!epoch){lastHideReasonM4.store(!Requested()?1:!Allowed(now)?2:10+refusal);++staleN;hideCurrent();return;}
 if(!acknowledged||!StaticModelReadyM41(entity,expected)){
  hideCurrent();lastHideReasonM4.store(21);next=now+200;++attempts;
  if(attempts==1||attempts==70)diagnostic_trace::Write("M41 STATIC_PAPER modelPending; exact loaded slot filename required, visibility=HIDDEN");return;
 }
 const auto rejoinRightM41=[&](bool update)noexcept{
  const auto time=std::uint64_t(GetTickCount64());unsigned why=0;
  if(!Selected()||!RendererIntentMatches43(renderer)||!renderer.open||!Allowed(time))return false;
  if(update){
   // Sample late, after helper/model work. Both support hands come from one
   // completed publication, without waiting for another pose or adding delay.
   frik_pose_probe::HeldMapSnapshotD78R1 hands{};const bool twoHand=twoHandD64.load();
   if(!frik_pose_probe::ReadHeldMapSnapshotD78R1(hands,twoHand,&why)||
      hands.sourceEntity!=source||hands.sourceEpoch!=epoch||!hands.sourceTick||
      time<hands.sourceTick||time-hands.sourceTick>80)return false;
   const auto* sheet=map_geometry_d64::find(expected);if(!sheet)return false;
   static paper_grip_d64::Support support{};static std::uint64_t supportSerial=~0ull;
   if(supportSerial!=currentSerial){support={};supportSerial=currentSerial;}
   if(!paper_grip_d64::place(hands.right,*sheet,paperSizeD64.load(),hands.haveLeft?hands.left:nullptr,
       hands.leftGrip,twoHand,support,pose))return false;
   tick=hands.sourceTick;
  }else{
   // Preserve the current owner/body/tracking rejoin at each native boundary.
   float fresh[8]{};std::uintptr_t who=0;std::uint64_t body=0,when=0;
   if(!frik_pose_probe::ReadHeldMapRightPoseM41(fresh,&why,&who,&body,&when)||who!=source||body!=epoch||
      !when||time<when||time-when>80)return false;
  }
  return time>=tick&&time-tick<=80;
 };
 if(!rejoinRightM41(true)||!sameAckM41()){hideCurrent();return;}
 if(renderer.native){
  // Hide the old WORLD helper before even a pending native skeleton binding.
  // A failed hide is not a visibility receipt, and never permits two renderers.
  const bool wasHidden=hidden;
  if((!hidden&&!Hide(entity,true))||!sameHelperM41()||!Hidden(entity,hidden)||!hidden){hideCurrent();return;}
  if(!wasHidden)++hideN;
  if(!rejoinRightM41(false)||!sameAckM41()){hideCurrent();return;}
  if(!PublishPaperM43(entity,helperId,source,actor,epoch,currentSerial,tick,pose,expected)){
   // False can mean unresolved native joint: keep its original hidden binding.
   // The provider owns cleanup on actual failure; do not recreate every frame.
   shown.store(false);next=now+200;++attempts;lastHideReasonM4.store(23);return;
  }
  if(!rejoinRightM41(false)||!sameAckM41()){hideCurrent();return;}
  ++placed;if(!shown.load()){++showN;diagnostic_trace::Write("M44 NATIVE_PAPER shown; exact CHARACTER_CGF; worldTransformCalls=0; size/support/POI retained; headsetPixels=UNOBSERVED");}
  shown.store(true);attempts=0;next=0;return;
 }
 // Never reveal the WORLD fallback while a previous native binding has debt.
 if(!PaperCleanupCompleteM43()){hideCurrent();return;}
 unsigned placement=0;
 // This private utility has a single placement owner. Identical source output
 // need not re-register the same transform again in the native scene.
 static std::uintptr_t lastEntityD67=0;static std::uint32_t lastHelperIdD67=0;static std::uint64_t lastSerialD67=0;static float lastPoseD67[8]{};
 const bool samePlacementD67=!hidden&&lastHelperIdD67==helperId&&lastEntityD67==reinterpret_cast<std::uintptr_t>(entity)&&lastSerialD67==currentSerial&&!std::memcmp(lastPoseD67,pose,sizeof(pose));
 if(!samePlacementD67&&!Place(entity,pose,placement)){hideCurrent();next=now+200;++attempts;lastHideReasonM4.store(22);return;}
 if(!samePlacementD67){lastHelperIdD67=helperId;lastEntityD67=reinterpret_cast<std::uintptr_t>(entity);lastSerialD67=currentSerial;std::memcpy(lastPoseD67,pose,sizeof(pose));++placed;}
 if(!rejoinRightM41(false)||!sameAckM41()){hideCurrent();return;}
 if(hidden){if(!Hide(entity,false))return;hidden=false;++showN;
  char line[280]{};std::snprintf(line,sizeof(line),"M41 STATIC_PAPER shown RIGHT model=%s nativeSlot=VERIFIED bakedDiffuse=PACKAGED size=ART_ASPECT_D64 edgeGrip=RIGHT support=OPTIONAL_LEFT animation=NONE headsetPixels=UNOBSERVED",expected);diagnostic_trace::Write(line);}
 shown.store(true);attempts=0;next=0;
}

void Shutdown() noexcept {Select(false);allowedUntil.store(0);native_paper_m43::RequestRetire();} // scalar request only; no native calls
bool VisibleD72() noexcept{return shown.load(std::memory_order_acquire);}
Counts Snapshot() noexcept{return {placed.load(),showN.load(),hideN.load(),missingN.load(),staleN.load(),poseRefusal19c3.load(),lastHideReasonM4.load()};}
}
