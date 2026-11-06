#include "native_paper_pins_m44.inl"
// M44 replaces the R1 Lua BONE lifecycle. Native-frame owner only. The caller
// retains D72's world backend and qualified actor/body/activation/ACK checks.
// Native construction mirrors SetAttachmentCGF37DC8AC and D43 skin allocation;
// no game object is allocated with the mod CRT, and no Lua thunk is invoked.
enum class PaperObservationM44 {Current,Retired,Unknown};
struct PaperSlotM44 {
 native_paper_m43::Binding binding{};
 bool allocated=false,constructed=false,assigned=false,bindEntered=false,bound=false,quarantined=false;
};
struct PaperOwnerM44 {
 native_paper_m43::Frame frame{};PaperSlotM44 slots[3]{};
 bool active=false,uncertainCreation=false;std::uint64_t due=0;
};
static PaperOwnerM44 paperOwnerM44{};
static std::atomic_flag paperBusyM44=ATOMIC_FLAG_INIT;

PaperObservationM44 PaperCharacterM44(const native_paper_m43::Frame& f,std::uintptr_t& character,std::uintptr_t& manager) noexcept {
 std::uintptr_t system=0,vt=0,lookup=0,get=0;
 if(!Copy(base+0x492D8A0,&system,8)||!Copy(system,&vt,8)||vt!=base+0x4077048||
    !Copy(vt+0x70,&lookup,8)||lookup!=base+0x4B4F98)return PaperObservationM44::Unknown;
 __try{
  const auto actor=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,std::uint32_t)>(lookup)(reinterpret_cast<void*>(system),f.actorId);
  if(actor!=f.actor)return PaperObservationM44::Retired;
  if(!Copy(actor,&vt,8)||vt!=base+0x4072C78||!Copy(vt+0x328,&get,8)||get!=base+0x6411AC)return PaperObservationM44::Unknown;
  character=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int)>(get)(reinterpret_cast<void*>(actor),0);
 }__except(EXCEPTION_EXECUTE_HANDLER){return PaperObservationM44::Unknown;}
 if(!character)return PaperObservationM44::Unknown; // temporary slot0 absence is NOT destruction
 if(f.character&&character!=f.character)return PaperObservationM44::Retired;
 manager=character+8;
 return Copy(manager,&vt,8)&&vt==base+0x4060548?PaperObservationM44::Current:PaperObservationM44::Unknown;
}
bool PaperFindM44(std::uintptr_t manager,unsigned slot,std::uintptr_t& bone) noexcept {
 std::uintptr_t vt=0,fn=0;
 if(slot>=3||!Copy(manager,&vt,8)||vt!=base+0x4060548||!Copy(vt+0x40,&fn,8)||fn!=base+0x971B2C)return false;
 __try{bone=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,const char*)>(fn)(reinterpret_cast<void*>(manager),native_paper_m43::names[slot]);return true;}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
PaperObservationM44 PaperObserveM44(PaperOwnerM44& owner,unsigned i) noexcept {
 const auto& b=owner.slots[i].binding;
 if(!b.bone)return PaperObservationM44::Retired;
 std::uintptr_t character=0,manager=0,bone=0,vt=0,object=0,parent=0;
 const auto state=PaperCharacterM44(owner.frame,character,manager);
 if(state!=PaperObservationM44::Current)return state;
 if(manager!=owner.frame.manager||!PaperFindM44(manager,i,bone))return PaperObservationM44::Unknown;
 if(bone!=b.bone)return PaperObservationM44::Retired; // positive missing/replaced slot, no replacement write
 if(!Copy(bone,&vt,8)||vt!=base+0x3A657C0||!Copy(bone+0x28,&parent,8)||parent!=manager||!Copy(bone+0x20,&object,8))return PaperObservationM44::Unknown;
 if(owner.slots[i].bindEntered&&!owner.slots[i].bound&&object==b.object){owner.slots[i].bound=true;owner.slots[i].quarantined=false;}
 if(object!=(owner.slots[i].bound?b.object:0))return PaperObservationM44::Retired;
 return PaperObservationM44::Current;
}
bool PaperVisibilityM44(PaperOwnerM44& owner,unsigned i,bool hide) noexcept {
 if(PaperObserveM44(owner,i)!=PaperObservationM44::Current)return false;
 const auto bone=owner.slots[i].binding.bone;std::uintptr_t fn=0;unsigned flags=0;
 if(!Copy(base+0x3A657C0+0x88,&fn,8)||fn!=base+0x5AF5E8||!Copy(bone+8,&flags,4))return false;
 if(bool(flags&0x10000)==hide)return true;
 __try{reinterpret_cast<void(__fastcall*)(void*,bool)>(fn)(reinterpret_cast<void*>(bone),hide);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 return Copy(bone+8,&flags,4)&&bool(flags&0x10000)==hide;
}
bool PaperDestroyBoneM44(PaperOwnerM44& owner,unsigned i) noexcept {
 const auto state=PaperObserveM44(owner,i);
 if(state==PaperObservationM44::Retired)return true;
 if(state!=PaperObservationM44::Current||!PaperVisibilityM44(owner,i,true))return false;
 std::uintptr_t fn=0;
 if(!Copy(base+0x4060548+0x18,&fn,8)||fn!=base+0x37507A4)return false;
 // RemoveByInterface searches the EXACT pointer under the native manager's
 // own lock (20ED870/AB9B0C). A name replacement cannot be removed by this call.
 __try{(void)reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(reinterpret_cast<void*>(owner.frame.manager),reinterpret_cast<void*>(owner.slots[i].binding.bone));}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 return PaperObserveM44(owner,i)==PaperObservationM44::Retired;
}
bool PaperReleaseUnboundM44(PaperSlotM44& slot) noexcept {
 if(slot.bound)return true;
 if(slot.quarantined||slot.bindEntered)return false; // ambiguous transfer is never double-freed
 if(!slot.allocated)return true;
 const auto object=slot.binding.object;
 __try{
  if(slot.constructed){
   std::uintptr_t vt=0;if(!Copy(object,&vt,8)||vt!=base+0x3E5F4C0)return false;
   reinterpret_cast<void*(__fastcall*)(void*,unsigned)>(base+0x27D1EE4)(reinterpret_cast<void*>(object),1);
  }else reinterpret_cast<void(__fastcall*)(void*,std::size_t)>(base+0x1AB5160)(reinterpret_cast<void*>(object),0x18);
 }__except(EXCEPTION_EXECUTE_HANDLER){slot.quarantined=true;return false;}
 slot.allocated=false;slot.binding.object=0;return true;
}
void PaperRetirePumpLockedM44(std::uint64_t now) noexcept {
 native_paper_m43::visible.store(false,std::memory_order_release);
 if(!paperOwnerM44.active){native_paper_m43::cleanupComplete.store(true);native_paper_m43::hidePending.store(false);native_paper_m43::retireRequested.store(false);return;}
 if(now<paperOwnerM44.due)return;paperOwnerM44.due=now+200;
 bool complete=!paperOwnerM44.uncertainCreation;
 if(paperOwnerM44.uncertainCreation){std::uintptr_t character=0,manager=0;
  if(PaperCharacterM44(paperOwnerM44.frame,character,manager)==PaperObservationM44::Retired){paperOwnerM44.uncertainCreation=false;complete=true;}}
 for(unsigned i=0;i<3;++i){auto& slot=paperOwnerM44.slots[i];
  if(slot.binding.bone){
   if(!PaperDestroyBoneM44(paperOwnerM44,i)){complete=false;continue;}
   slot.binding.bone=0;
   if(slot.bound){slot.allocated=false;slot.binding.object=0;} // native binding/character now owns destruction
  }
  if(!PaperReleaseUnboundM44(slot))complete=false;
 }
 native_paper_m43::cleanupComplete.store(complete,std::memory_order_release);
 native_paper_m43::hidePending.store(!complete,std::memory_order_release);
 if(complete){paperOwnerM44={};native_paper_m43::retireRequested.store(false);}
}
void PumpPaperM43() noexcept {
 if(!native_paper_m43::retireRequested.load(std::memory_order_acquire))return;
 if(paperBusyM44.test_and_set(std::memory_order_acquire))return;
 PaperRetirePumpLockedM44(std::uint64_t(GetTickCount64()));paperBusyM44.clear(std::memory_order_release);
}
void HidePaperM43() noexcept {native_paper_m43::RequestRetire();PumpPaperM43();} // NATIVE FRAME ONLY
bool PaperCleanupCompleteM43() noexcept {return native_paper_m43::cleanupComplete.load(std::memory_order_acquire);}

bool PaperMakeSlotM44(PaperOwnerM44& owner,unsigned i,std::uintptr_t stat) noexcept {
 auto& slot=owner.slots[i];const auto manager=owner.frame.manager;
 std::uintptr_t existing=0,fn=0,vt=0,parent=0,actual=0;
 if(!PaperFindM44(manager,i,existing)||existing)return false;
 if(!Copy(base+0x4060548+0x10,&fn,8)||fn!=base+0x20ED500)return false;
 __try{slot.binding.bone=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,const char*,int,const char*,bool)>(fn)(reinterpret_cast<void*>(manager),native_paper_m43::names[i],0,"RightHand",false);}
 __except(EXCEPTION_EXECUTE_HANDLER){owner.uncertainCreation=true;return false;}
 const auto bone=slot.binding.bone;if(!bone)return false; // preserve pointer receipt BEFORE any later failure
 if(!Copy(bone,&vt,8)||vt!=base+0x3A657C0||!Copy(bone+0x28,&parent,8)||parent!=manager||!Copy(bone+0x20,&actual,8)||actual)return false;
 if(!PaperVisibilityM44(owner,i,true))return false;
 __try{
  slot.binding.object=reinterpret_cast<std::uintptr_t(__fastcall*)(std::size_t)>(base+0x4F75C0)(0x18);
  if(!slot.binding.object)return false;slot.allocated=true;
  reinterpret_cast<void*(__fastcall*)(void*)>(base+0x27D1E30)(reinterpret_cast<void*>(slot.binding.object));slot.constructed=true;
  if(!Copy(slot.binding.object,&vt,8)||vt!=base+0x3E5F4C0)return false;
  slot.binding.stat=stat;
  reinterpret_cast<void*(__fastcall*)(void*,void*)>(base+0x93EC04)(reinterpret_cast<void*>(slot.binding.object+8),reinterpret_cast<void*>(stat));slot.assigned=true;
  if(!Copy(slot.binding.object+8,&actual,8)||actual!=stat)return false;
  if(PaperObserveM44(owner,i)!=PaperObservationM44::Current)return false;
  if(!Copy(base+0x3A657C0+0xD0,&fn,8)||fn!=base+0x9DFA14)return false;
  slot.bindEntered=true;
  const auto accepted=reinterpret_cast<int(__fastcall*)(void*,void*,void*,unsigned)>(fn)(reinterpret_cast<void*>(bone),reinterpret_cast<void*>(slot.binding.object),nullptr,0);
  if(!Copy(bone+0x20,&actual,8)||actual!=slot.binding.object||!accepted){slot.quarantined=true;return false;}
  slot.bound=true;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){
  // Pinned ctor27D1E30 only writes vft/null fields: it neither takes references
  // nor transfers/frees this allocation. A ctor fault can release our raw block.
  // Allocation/assign/bind faults have unknown native side effects and retain debt.
  slot.quarantined=!(slot.allocated&&!slot.constructed);return false;
 }
}
bool PaperModelsM44(void* helper,const char* expected,std::uintptr_t (&stats)[3]) noexcept {
 const char* leaf=std::strrchr(expected,'/');if(!leaf)return false;
 char poi[160]{};const auto n=std::snprintf(poi,sizeof(poi),"objects/kcd2vr/mapmarkers64/%s",leaf+1);
 if(n<0||std::size_t(n)>=sizeof(poi))return false;
 const char* paths[]{expected,poi,"objects/kcd2vr/mapmarkers64/player.cgf"};
 for(unsigned i=0;i<3;++i){
  __try{stats[i]=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int)>(base+0x7F1E3C)(helper,int(i));}
  __except(EXCEPTION_EXECUTE_HANDLER){return false;}
  std::uintptr_t vt=0;if(!stats[i]||!Copy(stats[i],&vt,8)||vt!=base+0x3A2F888||!FilenameMatchesM43(StaticFilenameM41(reinterpret_cast<void*>(stats[i])),paths[i]))return false;
 }
 return true;
}
bool PaperPlayerLocalM44(void* helper,native_paper_m43::Frame& frame) noexcept {
 std::uintptr_t matrix=0;float tm[12]{};
 __try{matrix=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int,bool)>(base+0x421528)(helper,2,true);}
 __except(EXCEPTION_EXECUTE_HANDLER){return false;}
 if(!matrix||!Copy(matrix,tm,sizeof(tm)))return false;
 for(unsigned i:{3u,7u,11u})if(!std::isfinite(tm[i])||std::abs(tm[i])>.6f)return false;
 frame.local[2].t={tm[3],tm[7],tm[11]};return true;
}
bool PublishPaperM43(void* helper,std::uint32_t helperId,std::uintptr_t actor,std::uint32_t actorId,
 std::uint64_t epoch,std::uint64_t serial,std::uint64_t tick,const float* pose,const char* expected) noexcept {
 diagnostics::perf_d61::Scope cost{diagnostics::perf_d61::MapPlacement};
 native_paper_m43::visible.store(false,std::memory_order_release);
 if(!NativePaperPinsM44()||!native_paper_m43::renderHookReady.load(std::memory_order_acquire)||paperBusyM44.test_and_set(std::memory_order_acquire))return false;
 struct Unlock{~Unlock(){paperBusyM44.clear(std::memory_order_release);}}unlock;
 const auto now=std::uint64_t(GetTickCount64());
 if(native_paper_m43::retireRequested.load()){
  PaperRetirePumpLockedM44(now);if(!PaperCleanupCompleteM43())return false;
  native_paper_m43::retireRequested.store(false);
 }
 native_paper_m43::Frame f{};f.actor=actor;f.actorId=actorId;f.helperId=helperId;f.epoch=epoch;f.activation=serial;f.tick=tick;
 if(PaperCharacterM44(f,f.character,f.manager)!=PaperObservationM44::Current||!native_paper_m43::matrix(pose,f.paper))return false;
 std::uintptr_t stats[3]{};if(!PaperModelsM44(helper,expected,stats))return false;
 if(paperOwnerM44.active){
  const auto& old=paperOwnerM44.frame;
  bool same=old.actor==actor&&old.actorId==actorId&&old.character==f.character&&old.manager==f.manager&&old.epoch==epoch&&old.activation==serial&&old.helperId==helperId;
  for(unsigned i=0;i<3;++i)if(paperOwnerM44.slots[i].binding.stat!=stats[i])same=false;
  if(!same){native_paper_m43::RequestRetire();PaperRetirePumpLockedM44(now);return false;}
 }else{
  // Record this lifetime before creation. Every partial native operation has
  // one exact owner/cleanup ledger; no retry adopts a same-name/model object.
  paperOwnerM44={};paperOwnerM44.active=true;paperOwnerM44.frame=f;native_paper_m43::cleanupComplete.store(false);
  for(unsigned i=0;i<3;++i)if(!PaperMakeSlotM44(paperOwnerM44,i,stats[i])){native_paper_m43::RequestRetire();PaperRetirePumpLockedM44(now);return false;}
 }
 for(unsigned i=0;i<3;++i){if(PaperObserveM44(paperOwnerM44,i)!=PaperObservationM44::Current){native_paper_m43::RequestRetire();PaperRetirePumpLockedM44(now);return false;}f.slots[i]=paperOwnerM44.slots[i].binding;
  // Native creation may queue skeleton resolution. Keep the ORIGINAL hidden
  // binding and wait; destroying/recreating a pending BONE prevents completion.
  std::int16_t joint=-1;if(!Copy(f.slots[i].bone+0x78,&joint,sizeof(joint))||joint<0)return false;
 }
 bool hidden=false;if(!Hidden(helper,hidden)||(!hidden&&!Hide(helper,true))||!Hidden(helper,hidden)||!hidden)return false;
 const bool player=PaperPlayerLocalM44(helper,f);
 if(!native_paper_m43::frames.publish(f)||native_paper_m43::retireRequested.load())return false;
 native_paper_m43::visible.store(true,std::memory_order_release);
 for(unsigned i=0;i<3;++i)if(!PaperVisibilityM44(paperOwnerM44,i,i==2&&!player)){native_paper_m43::RequestRetire();PaperRetirePumpLockedM44(now);return false;}
 return true;
}
