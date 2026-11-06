// Native-frame input ownership only. No result flag, phase or window writes.
#include "../diagnostics/hmd_dodge_context_h24.h"
static bool NativeHmdGuardYieldH24(const NativeOwner104& owner,std::uint64_t now) noexcept {
 namespace hd=kcd2vr::hmd_dodge_h24;auto& g=g_native_guard112;
 struct Pending {void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0,lease=0;bool released=false;};
 static Pending pending;
 const bool same=g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch;
 if(pending.released&&(pending.actor!=owner.actor||pending.state!=owner.state||pending.epoch!=owner.epoch))pending={};
 const bool manual=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
 hd::Request request{};const bool dodge=hd::currentRequest(reinterpret_cast<std::uintptr_t>(owner.entity),
  reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state),owner.epoch,now,request);
 if(dodge&&same&&g.held&&!manual){
  const auto lease=g.leaseH14;
  if(!PhysCallAction6698(owner.player,3,2))return true;
  NativeOwner104 current{};
  if(!NativeResolve104(owner.player,current)||current.actor!=owner.actor||current.state!=owner.state||current.epoch!=owner.epoch||
     g.owner.actor!=owner.actor||g.owner.state!=owner.state||g.owner.epoch!=owner.epoch||g.leaseH14!=lease)return true;
  pending={owner.actor,owner.state,owner.epoch,lease,true};
  g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;
 }
 if(pending.released){
  if((g.held&&g.leaseH14!=pending.lease)||manual){
   // A newer owned/manual input supersedes this old lease. Do not release or
   // ACK it through that new input, and do not stall the unrelated owner.
   if(g.held&&g.leaseH14!=pending.lease)pending={};
   hd::publishGuard(reinterpret_cast<std::uintptr_t>(owner.entity),reinterpret_cast<std::uintptr_t>(owner.actor),
    reinterpret_cast<std::uintptr_t>(owner.state),owner.epoch,now,same&&g.held);
   return dodge;
  }
  const int ack=NativeGuardReleaseAckH14(owner);
  hd::publishGuard(reinterpret_cast<std::uintptr_t>(owner.entity),reinterpret_cast<std::uintptr_t>(owner.actor),
   reinterpret_cast<std::uintptr_t>(owner.state),owner.epoch,now,ack!=0);
  if(ack!=0)return true; // An expired dodge request is not LT-up acknowledgement.
  pending={};if(dodge&&!manual)hd::acknowledgeRelease(request.serial);
 }else{
  hd::publishGuard(reinterpret_cast<std::uintptr_t>(owner.entity),reinterpret_cast<std::uintptr_t>(owner.actor),
   reinterpret_cast<std::uintptr_t>(owner.state),owner.epoch,now,same&&g.held);
  if(dodge&&!manual&&!(same&&g.held))hd::acknowledgeRelease(request.serial);
 }
 return dodge; // No new posture/parry LT while the exact HMD request owns input.
}
static bool NativeWeaponContactParryH24(const NativeOwner104& owner,const PhysFrame6698& f,
 const NativeParryOpponent176& enemy,const nc104::Observed& native,std::uint64_t now) noexcept {
 namespace wp=kcd2vr::weapon_parry_h24;auto& g=g_native_guard112;
 struct Delivery {WeaponParryContactH24 event{};std::uint64_t spent=0,pressedLease=0,pendingLease=0;bool pending=false,pressed=false;NativeOwner104 cleanup{};std::uint64_t cleanupLease=0;bool cleanupUp=false;};static Delivery d;
 if(d.cleanup.player){
  const bool manualCleanup=g_guard_manual_owner112.load()==d.cleanup.player&&g_guard_manual112.load();
  if(owner.player!=d.cleanup.player||owner.actor!=d.cleanup.actor||owner.state!=d.cleanup.state||
     g.leaseH14!=d.cleanupLease||manualCleanup){
   // Exact input owner retirement/supersession, NOT an LT-up ACK. An old
   // cleanup debt may not disable a new actor, guard lease or manual input.
   // No pointer from the retired owner is dereferenced or called here.
   d.cleanup={};d.cleanupLease=0;d.cleanupUp=false;d.pending=d.pressed=false;d.event={};return false;
  }
  if(!d.cleanupUp){if(!PhysCallAction6698(d.cleanup.player,3,2))return true;d.cleanupUp=true;}
  if(NativeGuardReleaseAckH14(owner)!=0)return true;
  d.cleanup={};d.cleanupLease=0;d.cleanupUp=false;d.pending=d.pressed=false;d.event={};return true;
 }
 WeaponParryContactH24 contact{};const bool struck=ReadWeaponParryContactH24(owner,f,enemy,now,contact);
 const wp::Pair pair{reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state),
  reinterpret_cast<std::uintptr_t>(enemy.actor),reinterpret_cast<std::uintptr_t>(enemy.state),owner.epoch,f.request};
 const bool manual=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
 if(manual){d.pending=d.pressed=false;d.event={};return false;}
 void* manager=nullptr;void* back=nullptr;std::uint64_t ownKey=0;
 if(!Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||manager!=g_henry_item_attach_manager.load()||
    !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
    !NativeHandKey6697(manager,0,ownKey)||ownKey!=f.profiles[0].key)return false;
 const auto sourceCurrent=[&](){
  NativeOwner104 live{};PhysFrame6698 frame{};void* currentManager=nullptr;void* currentBack=nullptr;std::uint64_t currentKey=0;
  return g.leaseH14==d.pendingLease&&g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
   !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
   !animation_filter::nativeBodyOwned112.load()&&f.request==g_weapon_request_epoch6694.load()&&owner.epoch==g_pose_epoch6689.load()&&
   NativeResolve104(owner.player,live)&&live.actor==owner.actor&&live.state==owner.state&&live.epoch==owner.epoch&&
   Read(static_cast<unsigned char*>(owner.player)+0x9D0,currentManager)&&currentManager==manager&&currentManager==g_henry_item_attach_manager.load()&&
   PhysVft6698(currentManager,0x3EC9958)&&Read(static_cast<unsigned char*>(currentManager)+0x38,currentBack)&&currentBack==owner.player&&
   NativeHandKey6697(currentManager,0,currentKey)&&currentKey==f.profiles[0].key&&g_phys_frame6698.read(frame)&&
   frame.player==owner.player&&frame.manager==manager&&frame.request==f.request&&frame.input.bodyEpoch==owner.epoch&&
   frame.input.allowed&&frame.input.tracked[0]&&frame.input.sample>=d.event.sample&&frame.input.tick&&
   now>=frame.input.tick&&now-frame.input.tick<=80&&frame.output.active[0]&&frame.output.owned[0]&&
   frame.profiles[0].key==f.profiles[0].key&&frame.profiles[0].generation==f.profiles[0].generation&&
   NativeWeaponParrySourceCurrentH24(d.event,frame,now)&&
   !(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load());
 };
 const auto current=[&](const WeaponParryContactH24& c){return enemy.incoming&&enemy.attacking&&
  wp::current(c,pair,now,f.profiles[0].key,f.profiles[0].generation,0);};
 const auto release=[&](){
  if(!NativeHeldGuardOwnerH14(owner))return true;
  const auto lease=g.leaseH14;
  if(!PhysCallAction6698(owner.player,3,2))return false;
  if(!NativeGuardAwaitReleaseH14(f,lease))return false;
  g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;return true;
 };
 if(d.pressed){
  if(!NativeHeldGuardOwnerH14(owner)||g.leaseH14!=d.pressedLease||
     g.key!=d.event.weapon||g.generation!=d.event.generation||g.side!=int(d.event.side)){
   // A later posture/shield/manual lease supersedes this contact input. It
   // cannot be released through the old event, even on the same actor.
   d.pressed=false;d.pressedLease=0;d.event={};return false;
  }
  // Preserve the input through native adoption, then release our own lease.
  // Stale tracking/contact cannot keep a stationary positional guard alive.
  if(!current(d.event)||!NativeWeaponParrySourceCurrentH24(d.event,f,now)||native.blocking||native.phase==128||native.phase==256){
   if(!release())return true;d.pressed=false;d.event={};return true;
  }
  return true;
 }
 if(struck&&contact.serial!=d.spent&&!d.pending){
  int count=-1;
  if(!NativeParryWindow55c2(owner,count)||count<=0)return false;
  d.event=contact;d.pending=true;d.spent=contact.serial;d.pendingLease=g.leaseH14;
  // Contact supersedes only our old motion refresh, never a manual press.
  g.pendingPress46=g.pendingBlockMetadataH14=g.pendingLegacyMotionH14=false;
  if(!release())return true;
 }
 if(!d.pending)return false;
 if(g.leaseH14!=d.pendingLease){d.pending=false;d.event={};return false;} // Newer input supersedes the released lease.
 if(!current(d.event)){d.pending=false;d.event={};return false;}
 const int ack=NativeGuardReleaseAckH14(owner);if(ack!=0)return true;
 NativeOwner104 again{};nc104::Observed observed{};NativeParryOpponent176 checked{};int window=-1;
 if(!NativeResolve104(owner.player,again)||again.actor!=owner.actor||again.state!=owner.state||again.epoch!=owner.epoch||
    !NativeObserve104(again,observed)||
    !(native_admission147::guardInput(observed)||nc104::blockMetadataH14(observed)||native_admission147::freeGuardHeld44c2(observed,true))||
    !NativeParryOpponentRead176(again,checked)||checked.actor!=enemy.actor||checked.state!=enemy.state||
    checked.id!=enemy.id||!checked.incoming||!checked.attacking||
    !NativeParryWindow55c2(again,window)||window<=0){d.pending=false;d.event={};return true;}
 // Recheck the current attacking hand item again after any LT-up native callback.
 npc172::Witness hit{};hit.ownerRead=true;hit.owner=d.event.enemy.entity;hit.entityId=d.event.enemy.id;hit.collider=d.event.enemy.collider;
 wp::Held held{};
 if(!wp::held(WhBase(),reinterpret_cast<std::uintptr_t>(checked.state),d.event.enemy.hand,hit,
    [](std::uintptr_t at,auto& value){return Read(reinterpret_cast<const void*>(at),value);},
    [](std::uint64_t key){return reinterpret_cast<std::uintptr_t>(PhysItemLookup6698(key));},
    [](std::uint32_t id){return reinterpret_cast<std::uintptr_t>(EntityById6694(id));},held)||held.key!=d.event.enemy.key){d.pending=false;return true;}
 if(!sourceCurrent()){d.pending=false;return true;}
 // Bind the exact current primary owner only AFTER the old input's ACK. A
 // shield/posture owner is never silently borrowed for an off-posture parry.
 g.owner=again;g.key=f.profiles[0].key;g.side=0;g.generation=f.profiles[0].generation;
 const auto pressLease=g.leaseH14;
 if(!PhysCallAction6698(owner.player,3,1))return true;
 NativeOwner104 afterPress{};
 if(!NativeResolve104(owner.player,afterPress)||afterPress.actor!=owner.actor||afterPress.state!=owner.state||
    afterPress.epoch!=owner.epoch||!sourceCurrent()||g.leaseH14!=pressLease||
    g.owner.actor!=owner.actor||g.owner.state!=owner.state||g.owner.epoch!=owner.epoch){
  // Original LT-down entered, but the source changed during its callback. Keep
  // an exact input cleanup obligation; no time expiry becomes release proof.
  d.cleanup=owner;d.cleanupLease=pressLease;d.cleanupUp=false;d.pending=false;return true;
 }
 NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
 d.pending=false;d.pressed=true;d.pressedLease=g.leaseH14;
 NativeParryTrace176(again,&checked,diagnostics::parry176::Cause::WeaponContact,now,false,true,
  diagnostics::parry176::publication.load(std::memory_order_acquire));
 return true;
}
