// Included after NativeParryOpponentRead176. Native frame only; no damage,
// result flags, ray-callback input dispatch, or cached entity-name ownership.
#include "weapon_parry_contact_h24.h"
using WeaponParryContactH24=kcd2vr::weapon_parry_h24::Contact;
static kcd2vr::native_weapon_parry_join_h24::Frame WeaponParryFrameH24(const PhysFrame6698& f) noexcept {
 const auto& p=f.profiles[0];const auto& o=f.output;
 return {reinterpret_cast<std::uintptr_t>(f.player),reinterpret_cast<std::uintptr_t>(f.manager),
  f.correlation126[0].stamp.actor,p.entity,p.key,p.generation,f.request,f.input.bodyEpoch,f.input.sample,
  o.solverSourceSampleT4,o.solverSourceTickT4,o.solverLeaseT4,o.solverWorldGenT4,o.solverBodyGenT4,
  o.solverTrackingEpochH24,o.solverCancelGenerationH24};
}
static bool NativeWeaponParrySourceCurrentH24(const WeaponParryContactH24& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
 NativeOwner104 owner{};NativeParryOpponent176 enemy{};
 if(!NativeResolve104(f.player,owner)||reinterpret_cast<std::uintptr_t>(owner.actor)!=c.pair.actor||
    reinterpret_cast<std::uintptr_t>(owner.state)!=c.pair.state||owner.epoch!=c.pair.epoch||
    !NativeParryOpponentRead176(owner,enemy)||reinterpret_cast<std::uintptr_t>(enemy.actor)!=c.pair.opponent||
    reinterpret_cast<std::uintptr_t>(enemy.state)!=c.pair.opponentState||!enemy.incoming||!enemy.attacking)return false;
 kcd2vr::npc_contact172::Witness hit{};hit.ownerRead=true;hit.owner=c.enemy.entity;hit.entityId=c.enemy.id;hit.collider=c.enemy.collider;
 kcd2vr::weapon_parry_h24::Held held{};
 if(!kcd2vr::weapon_parry_h24::held(WhBase(),c.pair.opponentState,c.enemy.hand,hit,
    [](std::uintptr_t at,auto& value){return Read(reinterpret_cast<const void*>(at),value);},
    [](std::uint64_t key){return reinterpret_cast<std::uintptr_t>(PhysItemLookup6698(key));},
    [](std::uint32_t id){return reinterpret_cast<std::uintptr_t>(EntityById6694(id));},held)||held.key!=c.enemy.key)return false;
 if(!c.nativeEvent)return !f.output.solverOwnedT4[0];
 namespace ne=kcd2vr::native_weapon_contact_h24;kcd2vr::native_weapon_body_t4::ContactOwnerH24 live{};
 return f.output.solverOwnedT4[0]&&kcd2vr::native_weapon_body_t4::ReadContactOwnerH24(live)&&ne::active(live,now)&&
  ne::sameIncarnation(c.physical,live)&&kcd2vr::native_weapon_parry_join_h24::frame(live,WeaponParryFrameH24(f))&&
  c.physical.sourceSample<=f.output.solverSourceSampleT4&&c.physical.sourceTick<=f.output.solverSourceTickT4;
}
static bool ReadPhysicalWeaponParryH24(const NativeOwner104& owner,const PhysFrame6698& f,const NativeParryOpponent176& enemy,
 const kcd2vr::weapon_parry_h24::Episode& episode,int hand,std::uint64_t now,WeaponParryContactH24& out) noexcept {
 namespace ne=kcd2vr::native_weapon_contact_h24;namespace join=kcd2vr::native_weapon_parry_join_h24;
 static ne::Event pending{};static bool have=false;
 kcd2vr::native_weapon_body_t4::ContactOwnerH24 live{};
 if(!kcd2vr::native_weapon_body_t4::ReadContactOwnerH24(live)||!ne::active(live,now)||
    !join::frame(live,WeaponParryFrameH24(f)))return false;
 const kcd2vr::weapon_parry_h24::Pair pair{reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state),
  reinterpret_cast<std::uintptr_t>(enemy.actor),reinterpret_cast<std::uintptr_t>(enemy.state),owner.epoch,f.request};
 for(unsigned i=0;i<32;++i){
  if(!have){if(!ne::ReadParryEventH24(pending,now))return false;have=true;}
  if(!ne::fresh(pending.tick,now)||!ne::sameIncarnation(pending.source,live)){have=false;continue;}
  // The collision can precede its achieved-pose display packet. Retain only
  // this bounded parry-view event until that same source packet arrives.
  if(pending.source.sourceSample>f.output.solverSourceSampleT4||pending.source.sourceTick>f.output.solverSourceTickT4)return false;
  const bool ready=join::join(WhBase(),pending,live,WeaponParryFrameH24(f),pair,episode.serial,episode.onsetSample,hand,now,
   [](std::uintptr_t at,auto& value){return Read(reinterpret_cast<const void*>(at),value);},
   [](std::uint64_t key){return reinterpret_cast<std::uintptr_t>(PhysItemLookup6698(key));},
   [](std::uint32_t id){return reinterpret_cast<std::uintptr_t>(EntityById6694(id));},out);
  have=false;if(!ready)continue;
  NativeParryOpponent176 current{};
  if(!NativeParryOpponentRead176(owner,current)||current.actor!=enemy.actor||current.state!=enemy.state||
     current.entity!=enemy.entity||current.id!=enemy.id||!current.incoming||!current.attacking||
     !NativeWeaponParrySourceCurrentH24(out,f,now)){out={};return false;}
  return true;
 }
 return false;
}
static bool ReadWeaponParryContactH24(const NativeOwner104& owner,const PhysFrame6698& f,
 const NativeParryOpponent176& enemy,std::uint64_t now,WeaponParryContactH24& out) noexcept {
 namespace wp=kcd2vr::weapon_parry_h24;out={};static wp::Episode episode;
 const wp::Pair pair{reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(owner.state),
   reinterpret_cast<std::uintptr_t>(enemy.actor),reinterpret_cast<std::uintptr_t>(enemy.state),owner.epoch,f.request};
 const bool freshEpisode=episode.observe(pair,enemy.observedPhase19c4r1,f.input.sample);
 if(!retail_build112::Ready(WhBase())||!enemy.incoming||!enemy.attacking||
    !g_gameplay_allowed.load()||diagnostics::lab::NativeReference()||animation_filter::nativeBodyOwned112.load()||
    !g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
    !f.input.sample||f.input.sample!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
    !f.input.allowed||f.player!=owner.player||f.input.bodyEpoch!=owner.epoch||f.request!=g_weapon_request_epoch6694.load()||
    !f.input.tick||now<f.input.tick||now-f.input.tick>80||
    !freshEpisode)return false;
 int attackingHand=-1; // Payload +1B8, not a guessed attachment hand index.
 if(!Read(static_cast<unsigned char*>(enemy.state)+0x1B8,attackingHand)||attackingHand<0||attackingHand>1)return false;
 const auto& p=f.profiles[0];
 if(!p.melee()||p.side!=0||!p.key||!p.generation||!f.output.active[0]||!f.output.owned[0]||
    f.output.handlingOnly55c1[0]||f.output.displayLatched[0]||!f.input.tracked[0]||
    !f.input.haveWorld||!ph::finite(f.output.weapons[0])||f.publishWorldDeltaM122>.002||f.publishWorldDeltaRad122>.002)return false;
 // A native solver body has ONE collision owner. Never query a parallel ray
 // if its event source is absent, stale, unjoined or waiting for its packet.
 if(f.output.solverOwnedT4[0])return ReadPhysicalWeaponParryH24(owner,f,enemy,episode,attackingHand,now,out);
 if(g_contact_budget_sample65&&f.input.sample<g_contact_budget_sample65)return false;
 const auto toWorld=[&](pm::Vec3 x){return ph::point(PhysWorld6698(f.world),x*double(f.world[7]));};
 const auto a=toWorld(ph::point(f.output.weapons[0],p.grip[0].p));
 const auto b=toWorld(ph::point(f.output.weapons[0],p.tip));
 const auto delta=b-a;const auto len=pm::length(delta);
 if(!pm::finite(a)||!pm::finite(b)||!std::isfinite(f.world[7])||f.world[7]<=0||len<.05||len>3)return false;
 // One actual blade-axis ray per NEW incoming XR sample. A stationary blade
 // still has a nonzero grip->tip segment. Shared18ray/1000us budget unchanged.
 struct QueryScope {
  NpcQuery172 npc=g_npc_query172;bool active=g_contact_budget_active6695,pass=g_contact_pass6695;
  std::uint64_t tick=g_contact_query_tick_h18;
  QueryScope(std::uint64_t sample,std::uint64_t t,std::uint64_t now){
   BeginContactBudget65(sample,now);g_contact_budget_active6695=true;g_contact_pass6695=true;
   g_contact_query_tick_h18=t;g_npc_query172={};g_npc_query172.requested=true;
  }
  ~QueryScope(){g_npc_query172=npc;g_contact_budget_active6695=active;g_contact_pass6695=pass;g_contact_query_tick_h18=tick;}
 } scope(f.input.sample,f.input.tick,now);
 const auto hit=WorldRayDistance6692({float(a.x),float(a.y),float(a.z)}, {float(delta.x),float(delta.y),float(delta.z)});
 if(hit.state!=wc::Result::Hit||!hit.plane)return false;
 wp::Held held{};
 if(!wp::held(WhBase(),reinterpret_cast<std::uintptr_t>(enemy.state),attackingHand,hit.native172,
    [](std::uintptr_t at,auto& value){return Read(reinterpret_cast<const void*>(at),value);},
    [](std::uint64_t key){return reinterpret_cast<std::uintptr_t>(PhysItemLookup6698(key));},
    [](std::uint32_t id){return reinterpret_cast<std::uintptr_t>(EntityById6694(id));},held))return false;
 NativeParryOpponent176 checked{};
 if(!NativeParryOpponentRead176(owner,checked)||checked.actor!=enemy.actor||checked.state!=enemy.state||
    checked.entity!=enemy.entity||checked.id!=enemy.id||!checked.incoming||!checked.attacking)return false;
 out={pair,held,episode.serial,f.input.tick,f.input.sample,p.key,p.generation,0,hit.native172.part};return true;
}
