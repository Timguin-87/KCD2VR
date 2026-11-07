// Full production TU, actual command gate/consumer/leaf and returned boundary.
// Only original native primitives and their actual binding memory effects are host services.
#define main LegacyH36Main
#include "holster_side_retirement_h36_bridge.cpp"
#undef main
#include <stdexcept>
extern "C" __declspec(dllimport) void* __stdcall VirtualAlloc(void*,std::size_t,unsigned long,unsigned long);
namespace kcd2vr::retail_build112 {extern bool hostRetailVerified112;}
static RigH36* liveH36=nullptr;
static bool deferDrawH36=true,deferStowH36=true,failNativeH36=false,throwNativeH36=false,insideAckH36=false;
static unsigned drawCallsH36=0,stowCallsH36=0,findCallsH36=0;
static void(*hostInitialSubmitServiceH36)(void*,void*)=nullptr;
static std::array<unsigned char,0xA80> playerH36{};
static std::array<unsigned char,0x100> modelH36{},amH36{},characterH36{},handH36{},storedH36{},leftItemH36{},linkH36{},entityH36{},handObjectH36{},storedObjectH36{};
static std::array<unsigned char,0x300> nativeSlotsH36{};
static std::array<unsigned char,0x180> definitionH36{};static std::array<unsigned char,0x60> classH36{};
static void* fakeImageH36=nullptr;static bool mutateAttachmentManagerH36=false;static bool findFaultH36=false,mutateHandDuringFindH36=false,mutateBindingDuringFindH36=false;
static void* __fastcall HostFindH36(void* model,const char* name){++findCallsH36;if(findFaultH36||model!=modelH36.data())return nullptr;
 if(mutateAttachmentManagerH36&&!std::strcmp(name,"stored_shield")){void* am=nullptr;std::memcpy(handH36.data()+0x28,&am,8);}
 if(mutateHandDuringFindH36&&!std::strcmp(name,"stored_shield")){std::uint64_t key=99;std::memcpy(nativeSlotsH36.data()+0x268,&key,8);}
 if(mutateBindingDuringFindH36&&!std::strcmp(name,"stored_shield")){void* object=handObjectH36.data();std::memcpy(handH36.data()+0x20,&object,8);}
 return !std::strcmp(name,"hand_left")?static_cast<void*>(handH36.data()):!std::strcmp(name,"stored_shield")?static_cast<void*>(storedH36.data()):nullptr;}
template<std::size_t N,class T>static void putH36(std::array<unsigned char,N>& a,std::size_t at,T value){std::memcpy(a.data()+at,&value,sizeof(value));}
static void bindHandH36(){putH36(storedH36,0x20,static_cast<void*>(nullptr));putH36(handH36,0x20,static_cast<void*>(handObjectH36.data()));}
static void clearHandH36(){putH36(handH36,0x20,static_cast<void*>(nullptr));}
static void bindStoredH36(){putH36(storedH36,0x20,static_cast<void*>(storedObjectH36.data()));}
static bool HostStowH36(void* manager,std::uint64_t key){++stowCallsH36;if(throwNativeH36)throw std::runtime_error("explicit native exception");if(failNativeH36)return false;
 auto&r=*liveH36;if(manager!=r.gear.manager||key!=88)std::abort();r.gear.handKey[1]=0;putH36(nativeSlotsH36,0x268,std::uint64_t(0));r.gear.heldIndex[1]=-1;r.gear.items[1].held=false;
 if(!deferStowH36){clearHandH36();bindStoredH36();}
 PhysLeftEquipmentRefreshH36(r.gear,hostTickCount64);insideAckH36=!g_native_held_equipment_pending_h36;
 return true;}
static bool __fastcall HostOriginalDrawH36(void* manager,void* item,int mode){
 if(throwNativeH36)throw std::runtime_error("explicit original native exception");if(failNativeH36)return false;
 auto&r=*liveH36;if(manager!=r.gear.manager||item!=leftItemH36.data()||mode!=2)std::abort();r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));if(hostInitialSubmitServiceH36)hostInitialSubmitServiceH36(manager,item);if(!deferDrawH36)bindHandH36();return true;
}
static void HostDrawH36(void* manager,std::uint64_t key,int mode){++drawCallsH36;auto&r=*liveH36;if(key!=88)std::abort();
 (void)ItemAttachOperationDetour(manager,leftItemH36.data(),mode); // actual existing hook, original primitive explicit
 PhysLeftEquipmentRefreshH36(r.gear,hostTickCount64);insideAckH36=!g_native_held_equipment_pending_h36;}
static void HostSelectH36(void*,int){std::abort();}static void HostLegacyDrawH36(void*){std::abort();}static void HostLegacyStowH36(void*){std::abort();}
namespace retail_build112=kcd2vr::retail_build112;namespace diagnostic_trace=kcd2vr::diagnostic_trace;namespace diagnostics=kcd2vr::diagnostics;
#include "generated_holster_completion_h36.inc"
struct CompletionRigH36:RigH36{
 CompletionRigH36(){
  liveH36=this;hostInitialSubmitServiceH36=nullptr;mutateAttachmentManagerH36=false;drawCallsH36=stowCallsH36=findCallsH36=0;insideAckH36=false;deferDrawH36=deferStowH36=true;failNativeH36=throwNativeH36=findFaultH36=mutateHandDuringFindH36=mutateBindingDuringFindH36=false;
#ifdef H36R1_HAS_COMPLETION
  g_left_native_membership_h36=0;g_left_equipment_successors_h36={};g_left_equipment_established_h36={};g_left_equipment_overflow_h36=false;
#endif
  if(!fakeImageH36){fakeImageH36=VirtualAlloc(nullptr,0x5500000,0x3000,0x40);if(!fakeImageH36)std::abort();hostModuleHandleW=fakeImageH36;
   auto at=[&](std::uintptr_t off){return static_cast<unsigned char*>(fakeImageH36)+off;};
   unsigned char jmp[]{0x48,0xB8,0,0,0,0,0,0,0,0,0xFF,0xE0};auto fn=reinterpret_cast<std::uintptr_t>(&HostFindH36);std::memcpy(jmp+2,&fn,8);std::memcpy(at(0x1085F74),jmp,sizeof(jmp));
   for(const auto& fp:ph::equipmentFingerprint114)std::memcpy(at(fp.rva),fp.bytes.data(),16);
   const char* name="hand_left";std::memcpy(at(0x40AC2B0),&name,8);
   static std::uintptr_t registry[1],entry[3];entry[1]=reinterpret_cast<std::uintptr_t>("stored_shield");registry[0]=reinterpret_cast<std::uintptr_t>(entry);
   auto begin=reinterpret_cast<std::uintptr_t>(registry),end=begin+8;int first=0;std::memcpy(at(0x5325648),&begin,8);std::memcpy(at(0x5325650),&end,8);std::memcpy(at(0x5325660),&first,4);
  }
  kcd2vr::retail_build112::hostRetailVerified112=true;g_weapon_control_mode=6;
  definitionH36={};classH36={};nativeSlotsH36={};playerH36={};modelH36={};amH36={};characterH36={};handH36={};storedH36={};leftItemH36={};linkH36={};entityH36={};handObjectH36={};storedObjectH36={};
  const auto b=reinterpret_cast<std::uintptr_t>(fakeImageH36);gear.player=playerH36.data();g_henry_item_attach_manager=gear.manager;g_player_char37=reinterpret_cast<std::uintptr_t>(characterH36.data());
  f::achieved.owner.player=reinterpret_cast<std::uintptr_t>(gear.player);f::achieved.owner.character=g_player_char37;
  putH36(playerH36,0,b+0x3AA4B98);putH36(playerH36,0x990,static_cast<void*>(nativeSlotsH36.data()));putH36(nativeSlotsH36,0x260,std::uint64_t(77));putH36(playerH36,0x9D0,static_cast<void*>(manager.data()));putH36(playerH36,0x1B0,static_cast<void*>(modelH36.data()));
  putH36(manager,0,b+0x3EC9958);putH36(manager,0x38,static_cast<void*>(playerH36.data()));putH36(modelH36,0x20,static_cast<void*>(playerH36.data()));
  putH36(characterH36,0,b+0x40602F8);putH36(amH36,0x70,static_cast<void*>(characterH36.data()));
  for(auto* a:{&handH36,&storedH36}){putH36(*a,0,b+0x3A657C0);putH36(*a,0x28,static_cast<void*>(amH36.data()));}
  for(auto* a:{&handObjectH36,&storedObjectH36}){putH36(*a,0,b+0x3A68F68);putH36(*a,8,std::uint32_t(80));}
  bindStoredH36();putH36(leftItemH36,0,b+0x3A2DA88);putH36(leftItemH36,0x30,std::uint64_t(88));putH36(leftItemH36,0x88,static_cast<void*>(linkH36.data()));
  putH36(linkH36,0,b+0x3EC85E0);putH36(linkH36,0x30,std::uint32_t(80));putH36(linkH36,0x38,static_cast<void*>(entityH36.data()));putH36(linkH36,0x58,static_cast<void*>(leftItemH36.data()));
  putH36(leftItemH36,0x48,static_cast<void*>(definitionH36.data()));putH36(definitionH36,0,b+0x3A4D890);putH36(definitionH36,0xE8,static_cast<void*>(classH36.data()));putH36(classH36,0x25,static_cast<unsigned char>(0));
  g_item_attach_operation_original=&HostOriginalDrawH36;
  auto&l=gear.items[1];l.item=leftItemH36.data();l.worldLink54=linkH36.data();l.entity=entityH36.data();
  currentTalk();
 }
 void currentTalk(){kcd2vr::quick_talk_owner_h4::mailbox.publish({hostTickCount64,9,10,reinterpret_cast<std::uintptr_t>(gear.player),0});}
 void advance(){in.tick+=20;++in.sample;hostTickCount64=in.tick;gear.tick=in.tick;
#ifdef H36R2_HAS_INITIAL_OWNER
 gear.leftNativeMembershipH36=g_left_native_membership_h36.load(); // explicit fresh native snapshot metadata, same producer order
#endif
 currentTalk();}
 void dispatch(ph::Command c){auto frame=RigH36::frame();g_phys_commands6698.push(c);ActualCompletionDrainH36(frame,gear,hostTickCount64);}
 void draw(){dispatch(drawCommand());}
 void refresh(){PhysLeftEquipmentRefreshH36(gear,hostTickCount64);}
 ph::Command stowCommand(){auto c=drawCommand();c.kind=ph::CommandKind::Holster;return c;}
 void establish(){draw();advance();bindHandH36();refresh();}
};
#ifndef H36_MAIN_NAME
#define H36_MAIN_NAME main
#endif
int H36_MAIN_NAME(){tests=failed=0;
 // Actual ring read is enqueue-success proof. A later empty-hand Talk claim
 // must not re-arbitrate this original occurrence before native attachment ACK.
 for(unsigned talkCase=0;talkCase<4;++talkCase){CompletionRigH36 r;const auto original=r.drawCommand();
  ck(g_phys_commands6698.push(original),"actual ring accepts original equipment occurrence");
  r.advance();const unsigned mask=talkCase==1||talkCase==2?2u:talkCase==3?1u:0u;
  kcd2vr::quick_talk_owner_h4::mailbox.publish({hostTickCount64,9,10,reinterpret_cast<std::uintptr_t>(r.gear.player),mask});
  if(talkCase==2)kcd2vr::quick_talk_owner_h4::mailbox.publish({hostTickCount64,9,10,reinterpret_cast<std::uintptr_t>(r.gear.player),0});
  ph::Command queued{};ck(g_phys_commands6698.peek(queued)&&queued.tick==original.tick&&queued.sample==original.sample&&queued.key==original.key&&queued.generation==original.generation&&queued.epoch==original.epoch&&queued.bodyEpoch==original.bodyEpoch,"later utility neither retags original command clock nor borrows another item/epoch");
  auto frame=r.frame();ActualCompletionDrainH36(frame,r.gear,hostTickCount64);
  ck(drawCallsH36==1&&!g_phys_commands6698.peek(queued),"current queued Draw executes once through actual consumer despite later/retracted/opposite Talk");
  ActualCompletionDrainH36(frame,r.gear,hostTickCount64);ck(drawCallsH36==1,"same native callback cannot replay already popped original draw");
 }
 {CompletionRigH36 r;const auto stagedOnly=r.drawCommand();(void)stagedOnly;r.advance();
  kcd2vr::quick_talk_owner_h4::mailbox.publish({hostTickCount64,9,10,reinterpret_cast<std::uintptr_t>(r.gear.player),2});
  ActualCompletionDrainH36(r.frame(),r.gear,hostTickCount64);ck(!drawCallsH36&&!g_native_held_equipment_pending_h36,"proposed/staged command without actual enqueue cannot invoke native Draw");}
 for(unsigned stale=0;stale<7;++stale){CompletionRigH36 r;auto c=r.drawCommand();
  if(stale==0)++c.bodyEpoch;if(stale==1)++c.epoch;if(stale==2)++c.generation;if(stale==3)++c.key;if(stale==4)c.tick-=101;if(stale==5)++c.tick;if(stale==6)c.side=2;
  kcd2vr::quick_talk_owner_h4::mailbox.publish({hostTickCount64,9,10,reinterpret_cast<std::uintptr_t>(r.gear.player),2});r.dispatch(c);
  ck(!drawCallsH36,"later Talk removal grants no stale identity/generation/epoch/side/future/original100ms exemption");}
 {CompletionRigH36 r;auto filler=r.drawCommand();filler.kind=ph::CommandKind::None;filler.key=0;
  for(unsigned n=0;n<g_phys_commands6698.q.size()-1;++n)ck(g_phys_commands6698.push(filler),"actual ring capacity control accepts prior non-equipment entry");
  ck(!g_phys_commands6698.push(r.drawCommand()),"full actual ring refuses original Draw without enqueue receipt");
  g_phys_commands6698.head=g_phys_commands6698.tail=0; // Explicitly retire fixture fillers; not a gameplay retry.
  ActualCompletionDrainH36(r.frame(),r.gear,hostTickCount64);ck(!drawCallsH36&&!g_native_held_equipment_pending_h36,"failed enqueue never invents native draw or native obligation");}
 {CompletionRigH36 r;r.draw();ck(drawCallsH36==1,"actual queued current native Draw invokes original host leaf once");ck(g_native_held_equipment_pending_h36&&!insideAckH36,"native current key before returned real binding cannot ACK Draw");
  r.advance();bindHandH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"positive original hand bound and former stowed empty close synchronous-known Draw");
  r.advance();r.dispatch(r.stowCommand());ck(stowCallsH36==1&&!insideAckH36,"actual Stow leaf called once; normal return is recorded after callback");ck(g_native_held_equipment_pending_h36,"empty key with original hand attachment still bound cannot ACK Stow");
  r.advance();clearHandH36();r.refresh();ck(g_native_held_equipment_pending_h36,"special Stow hand cleared but new stored attach pending cannot ACK");
  r.advance();bindStoredH36();r.refresh();ck(!g_native_held_equipment_pending_h36&&!f::equipmentSuspendedH36,"actual same original stored binding closes whole standard draw/stow cycle");
  r.currentSource();NativeHeldSourceT4 s{};g_native_held_source_t4.read(s);ck(NativeHeldSourceCurrentT4(s.input),"fresh same right source is eligible again after exact completed left cycle");
  kcd2vr::animation_filter::nativeBodyOwned112=true;ck(!NativeHeldSourceCurrentT4(s.input),"independent existing native body action still owns the body after equipment ACK");}
 {CompletionRigH36 r;r.draw();r.advance();putH36(handH36,0x20,static_cast<void*>(handObjectH36.data()));r.refresh();ck(g_native_held_equipment_pending_h36,"hand binding without old stored binding retired cannot close Draw");
  putH36(storedH36,0x20,static_cast<void*>(nullptr));r.refresh();ck(!g_native_held_equipment_pending_h36,"real stored retirement closes original Draw without assuming queue ordering");}
 {CompletionRigH36 r;r.establish();r.advance();deferStowH36=false;r.dispatch(r.stowCommand());ck(!insideAckH36&&g_native_held_equipment_pending_h36,"reentrant native callback cannot ACK before original wrapper returned");r.refresh();ck(!g_native_held_equipment_pending_h36,"synchronous real transitions ACK after original normal return only");}
 {CompletionRigH36 r;r.establish();r.advance();auto c=r.stowCommand();r.dispatch(c);r.advance();clearHandH36();bindStoredH36();findFaultH36=true;r.refresh();ck(g_native_held_equipment_pending_h36,"faulted native socket lookup is UNKNOWN not absence");findFaultH36=false;r.refresh();ck(!g_native_held_equipment_pending_h36,"same original positive graph can finish after read recovers");}
 {CompletionRigH36 r;r.establish();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();mutateHandDuringFindH36=true;r.refresh();ck(g_native_held_equipment_pending_h36,"native hand changes during socket lookup cannot be completed from the copied empty snapshot");}
 {CompletionRigH36 r;r.establish();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();mutateBindingDuringFindH36=true;r.refresh();ck(g_native_held_equipment_pending_h36,"binding changed during later lookup cannot ACK from former empty socket");}
 for(unsigned change=0;change<11;++change){CompletionRigH36 r;r.establish();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();
  if(change==0)++r.gear.request;if(change==1)++r.gear.epoch;if(change==2)++g_player_char37;if(change==3)putH36(modelH36,0x20,static_cast<void*>(nullptr));
  if(change==4)putH36(handH36,0x28,static_cast<void*>(nullptr));if(change==5)putH36(storedObjectH36,8,std::uint32_t(81));if(change==6)r.gear.items[1].item=leftItemH36.data()+8;
  if(change==7)r.gear.tick-=101;if(change==8)++r.gear.items[1].profile.generation;if(change==9)r.gear.items[1].entity=entityH36.data()+8;if(change==10)putH36(linkH36,0x58,static_cast<void*>(nullptr));r.refresh();ck(g_native_held_equipment_pending_h36,"changed original identity/graph/entity/item or stale snapshot cannot ACK Stow");}
 {CompletionRigH36 r;putH36(leftItemH36,0x88,static_cast<void*>(nullptr));r.gear.items[1].worldLink54=nullptr;r.gear.items[1].entity=nullptr;r.gear.items[1].profile.entity=0;r.draw();
  r.advance();putH36(leftItemH36,0x88,static_cast<void*>(linkH36.data()));r.gear.items[1].worldLink54=linkH36.data();r.gear.items[1].entity=entityH36.data();r.gear.items[1].profile.entity=80;bindHandH36();r.refresh();
  ck(drawCallsH36==0&&!g_native_held_equipment_pending_h36,"unproven initial draw preflight invokes no native job and later plausible binding invents no obligation");}
 {CompletionRigH36 r;r.establish();r.advance();throwNativeH36=true;r.dispatch(r.stowCommand());throwNativeH36=false;r.advance();clearHandH36();bindStoredH36();r.refresh();ck(g_native_held_equipment_pending_h36,"exception after original entry cannot fabricate normal-return receipt");}
 {CompletionRigH36 r;r.establish();r.advance();failNativeH36=true;r.dispatch(r.stowCommand());failNativeH36=false;r.advance();r.gear.handKey[1]=0;clearHandH36();bindStoredH36();r.refresh();ck(g_native_held_equipment_pending_h36,"false wrapper cannot certify original invoked transition");}
 {CompletionRigH36 r;r.establish();r.advance();auto original=r.stowCommand();r.dispatch(original);r.advance();r.draw();r.advance();bindHandH36();r.refresh();
  ck(g_native_held_equipment_pending_h36,"new Draw cannot erase unknown original Stow or reuse original ACK");
#ifdef H36R1_HAS_COMPLETION
  ck(g_left_equipment_obligation_h36.command.sample==original.sample&&g_left_equipment_obligation_h36.uncertain&&g_left_equipment_successors_h36[0].active,"every original immutable operation survives native successor");
#endif
 }
 {CompletionRigH36 r;r.establish();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();putH36(leftItemH36,0x88,static_cast<void*>(nullptr));r.gear.items[1].worldLink54=nullptr;r.gear.items[1].entity=nullptr;r.gear.items[1].profile.entity=0;++r.gear.items[1].profile.generation;r.refresh();
  ck(!g_native_held_equipment_pending_h36,"ordinary Stow exact native world-link retirement plus held binding gone closes original operation");}

 {CompletionRigH36 r;for(unsigned cycle=0;cycle<3;++cycle){r.establish();ck(!g_native_held_equipment_pending_h36,"completed prior cycle permits next distinct original Draw");r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"next distinct original Stow completes without reholster/manual reset workaround");r.advance();}}
 {CompletionRigH36 r;r.establish();r.advance();auto original=r.stowCommand();r.dispatch(original);f::enabled=false;r.in.tick+=2000;hostTickCount64=r.in.tick;r.gear.tick=r.in.tick;r.refresh();
  ck(g_native_held_equipment_pending_h36&&stowCallsH36==1,"OFF and elapsed time cannot ACK entered operation or retry old native call");
  r.advance();clearHandH36();bindStoredH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"real original terminal evidence may finish while F9 is OFF; OFF itself is not evidence");}
#ifdef H36R1_HAS_COMPLETION
 {CompletionRigH36 r;auto first=r.drawCommand();for(unsigned n=0;n<9;++n){r.gear.handKey[1]=0;r.gear.heldIndex[1]=-1;r.gear.items[1].held=false;putH36(nativeSlotsH36,0x268,std::uint64_t(0));r.draw();r.advance();}
  ck(drawCallsH36==8&&!g_left_equipment_overflow_h36,"bounded ledger exhaustion defers the ninth original call before native entry without inventing unknown overflow");
  ck(g_native_held_equipment_pending_h36&&g_left_equipment_obligation_h36.command.sample==first.sample&&g_left_equipment_successors_h36[6].active,"capacity refusal never overwrites original unknown operations or clears physical admission fence");}
#endif

#ifdef H36R1_HAS_COMPLETION
 {CompletionRigH36 r;r.establish();const auto serial=g_left_native_membership_h36.load();
  ck(serial==1,"actual original native0-to-item observer establishes one membership serial");
  ck(!ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),1,false,true,0)&&g_left_native_membership_h36==serial,"native false return cannot publish membership transition");
  ck(!ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),1,true,false,0)&&g_left_native_membership_h36==serial,"unknown pre-call hand cannot publish transition");
  ck(!ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),1,true,true,88)&&g_left_native_membership_h36==serial,"same-WUID no-op native Draw cannot create a new membership");
  ck(!ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),0,true,true,0)&&g_left_native_membership_h36==serial,"right side cannot publish left transition");
  ck(!ObserveLeftNativeDrawH36(static_cast<unsigned char*>(r.gear.manager)+1,leftItemH36.data(),1,true,true,0)&&g_left_native_membership_h36==serial,"foreign manager cannot publish original Henry transition");
  putH36(nativeSlotsH36,0x268,std::uint64_t(99));ck(!ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),1,true,true,0)&&g_left_native_membership_h36==serial,"current native key mismatch cannot publish original item transition");}
 {CompletionRigH36 r;r.establish();r.advance();
  // Actual observer catches an unobserved native STOW/Draw of the same WUID,
  // even if the allocator reused the exact attachment-object address.
  ck(ObserveLeftNativeDrawH36(r.gear.manager,leftItemH36.data(),1,true,true,0),"unobserved actual different-to-item native transition advances causal serial");
  r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();r.refresh();ck(g_native_held_equipment_pending_h36,"same-WUID/object-pointer ABA cannot reuse a former established Draw receipt");}
#endif

#ifdef H36R1_HAS_COMPLETION
 {CompletionRigH36 r;throwNativeH36=true;r.draw();throwNativeH36=false;ck(drawCallsH36==1&&g_left_native_membership_h36==0&&g_native_held_equipment_pending_h36,"actual existing hook exception from original native cannot publish transition or complete original Draw");}
 {CompletionRigH36 r;failNativeH36=true;r.draw();failNativeH36=false;ck(drawCallsH36==1&&g_left_native_membership_h36==0&&g_native_held_equipment_pending_h36,"actual existing hook false original return cannot publish native membership");}
 {CompletionRigH36 r;r.establish();const auto serial=g_left_native_membership_h36.load();
  ck(ItemAttachOperationDetour(r.gear.manager,leftItemH36.data(),2)&&g_left_native_membership_h36==serial,"actual existing native same-WUID no-op hook preserves causal serial");}
#endif

#ifdef H36R1_HAS_COMPLETION
 {CompletionRigH36 r;r.currentSource();NativeHeldSourceT4 old{};g_native_held_source_t4.read(old);
  ck(NativeHeldSourceCurrentT4(old.input),"unchanged same-owner right source with current left membership remains allowed");
  // Actual original native hook changes membership; no per-frame native poll.
  HostDrawH36(r.gear.manager,88,2);
  ck(!NativeHeldSourceCurrentT4(old.input),"actual observed left DRAW invalidates former right source immediately by atomic membership identity");}
#endif

#ifdef H36R2_HAS_INITIAL_OWNER
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();r.advance();
  r.dispatch(r.stowCommand());ck(stowCallsH36==1&&g_native_held_equipment_pending_h36,"initially held original STOW invokes once without invented prior DRAW");
  r.advance();clearHandH36();r.refresh();ck(g_native_held_equipment_pending_h36,"initially held special STOW still waits for original stored binding");
  bindStoredH36();r.refresh();ck(!g_native_held_equipment_pending_h36,"fresh initially held original item completes via newly occupied original empty socket");}
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();r.advance();deferStowH36=false;r.dispatch(r.stowCommand());r.refresh();
  ck(stowCallsH36==1&&!g_native_held_equipment_pending_h36,"initially held synchronous STOW closes only after normal original return");}
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();putH36(leftItemH36,0x88,static_cast<void*>(nullptr));r.gear.items[1].worldLink54=nullptr;r.gear.items[1].entity=nullptr;r.gear.items[1].profile.entity=0;++r.gear.items[1].profile.generation;r.refresh();
  ck(!g_native_held_equipment_pending_h36,"initially held ordinary STOW uses actual original link retirement with no DRAW history");}
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();r.advance();findFaultH36=true;r.dispatch(r.stowCommand());findFaultH36=false;r.advance();clearHandH36();bindStoredH36();r.refresh();
  ck(stowCallsH36==0&&!g_native_held_equipment_pending_h36,"missing original pre-call witness rejects stow before native entry without a permanent debt");}
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();r.advance();r.dispatch(r.stowCommand());++g_left_native_membership_h36;r.advance();clearHandH36();bindStoredH36();r.refresh();
  ck(g_native_held_equipment_pending_h36,"initially held STOW cannot ACK across intervening native membership incarnation");}
 {CompletionRigH36 r;r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();bindStoredH36();r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();r.refresh();
  ck(stowCallsH36==0&&!g_native_held_equipment_pending_h36,"duplicate stored binding rejects preparation before native stow without inventing cleanup work");}
#endif
 for(unsigned alreadyHeld=0;alreadyHeld<2;++alreadyHeld){CompletionRigH36 r;
  if(alreadyHeld){r.ackLeft();putH36(nativeSlotsH36,0x268,std::uint64_t(88));bindHandH36();}else r.establish();
  r.advance();r.dispatch(r.stowCommand());r.advance();clearHandH36();bindStoredH36();mutateAttachmentManagerH36=true;r.refresh();
  ck(g_native_held_equipment_pending_h36,"changed actual hand attachment manager during later socket lookup cannot falsely complete original STOW");}
 std::printf("holster_completion_h36=%u/%u nativeLeaves=EXPLICIT_HOST actualOriginalCallback=YES retailNativeExecution=NONE\n",tests-failed,tests);return failed?1:0;}
