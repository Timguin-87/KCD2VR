struct ActorSeen {const void* actor=nullptr;std::uint32_t entityId=0;std::uint64_t seen=0,resolved=0;};
std::array<ActorSeen,32> g_actors{};std::mutex g_actors_mutex;
void ActorSeen27(const void* actor) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    counter36::ActorUpdated(actor,now);
    std::unique_lock<std::mutex> lock(g_actors_mutex,std::try_to_lock);if(!lock.owns_lock())return;
    ActorSeen* slot=nullptr;ActorSeen* oldest=&g_actors[0];
    for(auto& a:g_actors){if(a.actor==actor){slot=&a;break;}if(a.seen<oldest->seen)oldest=&a;}
    if(!slot){slot=oldest;*slot={};slot->actor=actor;}
    slot->seen=now;
    if(slot->resolved&&now>=slot->resolved&&now-slot->resolved<(slot->entityId?1000u:50u))return;
    slot->resolved=now;const void *entity=nullptr,*state=nullptr;
    slot->entityId=(CombatActor(actor,entity,state,false)&&entity)?EntityId(entity):0;
}
const void* ActorForEntity(std::uint32_t id,std::uint64_t now) noexcept {
    if(!id)return nullptr;
    if(kcd2vr::actor_lookup_h7::callbackDepth){
        // A valid new contact must not wait for an NPC Update to populate our
        // 32-slot observer cache or for Henry to acquire that NPC as opponent.
        // Current native lookup is authoritative here: a removed/invalid map
        // member cannot fall back to a stale observed actor.
        const auto actor=NativeActorForEntityH7(id);
        if(actor){std::unique_lock<std::mutex> lock(g_actors_mutex,std::try_to_lock);
            if(lock.owns_lock()){
                ActorSeen* slot=nullptr;ActorSeen* oldest=&g_actors[0];
                for(auto& a:g_actors){if(a.actor==actor){slot=&a;break;}if(a.seen<oldest->seen)oldest=&a;}
                if(!slot)slot=oldest;*slot={actor,id,now,now};
            }}
        return actor;
    }
    // Physical-producer callers may only use the existing bounded observation;
    // they never invoke gameplay/native actor services on a physics thread.
    std::lock_guard<std::mutex> lock(g_actors_mutex);
    for(const auto& a:g_actors)if(a.actor&&a.entityId==id&&now>=a.seen&&now-a.seen<500)return a.actor;
    return nullptr;
}
// C19C3: an Update/map ordering gap is not a different impact. Retry only the
// original queued contact, within its unchanged 160 ms lease. An existing map
// candidate with a bad identity is terminal, never redirected to the opponent.
enum class TargetResolution19c3 { Ready, Missing, Invalid };
static bool OriginalContactOwned19c3(const Pending& p,std::uint64_t now) noexcept {
    return p.recovery55c1==kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire)&&
        kcd2vr::contact176::fresh(p.tick,now,handoff59c3::maximumContactAge)&&
        kcd2vr::solver_rpg_h29::originalContactOwnedH30(p.metadata59c7g,p.key,p.generation,p.epoch,p.request,
            p.sample,p.gesture59c3,p.tick,now,frik_pose_probe::GeometryOwnerValid178r30,
            frik_pose_probe::OriginalGeometryContactOwnerH30,frik_pose_probe::GeometrySolverOwnerValidH28);
}
static bool RetainedContactBody19c3(const Pending& p,const void* entity) noexcept {
    // Reuse the exact retail read-only proxy map/owner contract already used
    // by the contact impulse adapter. No native method, physics query or write.
    if(!p.metadata59c7g.collider||!entity)return false;
    const auto e=reinterpret_cast<std::uintptr_t>(entity);
    auto read=[](std::uintptr_t a,auto& v){return SehCopy146(reinterpret_cast<const void*>(a),&v,sizeof(v));};
    std::uintptr_t proxy=0,vf=0,owner=0,body=0;
    if(!read(e,vf)||vf!=g_base+kEntityVft||!kcd2vr::npc_contact172::proxy(e,1,read,proxy)||!proxy||
       !read(proxy,vf)||vf!=g_base+impulse596::kPhysicalProxyVft5916c3||
       !read(proxy+0x30,owner)||owner!=e||!read(proxy+0x38,body))return false;
    if(body==p.metadata59c7g.collider)return true; // original capsule path unchanged
    auto copy=[](std::uintptr_t at,void* out,std::size_t size){return SehCopy146(reinterpret_cast<const void*>(at),out,size);};
    // A living NPC's ray-hit skeleton can differ from its locomotion capsule.
    // Validate the CURRENT main as this SAME original collider; do not replace
    // the contact or mint a retained allocation/part/native-action lease.
    return kcd2vr::articulated_target_owner_h8::current(g_base,e,p.entityId,p.metadata59c7g.collider,proxy,
        retail_build112::Ready(g_base),copy);
}
static TargetResolution19c3 ResolveDirectTarget19c3(const Pending& p,const void* henryActor,const void* hs,
    std::uint64_t now,const void*& target,const void*& entity,const void*& state) noexcept {
    target=nullptr;entity=nullptr;state=nullptr;
    if(!p.entityId)return TargetResolution19c3::Invalid;
    target=ActorForEntity(p.entityId,now);
    if(!target){const void* opponent=Rd<const void*>(hs,0xCF8,nullptr);
        // An unrelated current opponent is not the struck actor. Its presence
        // cannot authorize damage or replace the not-yet-observed map entry.
        if(opponent&&opponent!=henryActor){const void *oe=nullptr,*os=nullptr;
            if(CombatActor(opponent,oe,os,false)&&oe&&EntityId(oe)==p.entityId)target=opponent;}}
    if(!target)return TargetResolution19c3::Missing;
    if(target==henryActor||!CombatActor(target,entity,state,false)||!entity||EntityId(entity)!=p.entityId)
        return TargetResolution19c3::Invalid;
    if((p.targetWait19c3||p.metadata59c7g.occurrenceSerialH28)&&!RetainedContactBody19c3(p,entity))return TargetResolution19c3::Invalid;
    return TargetResolution19c3::Ready;
}
template<class Lease> static bool AwaitDirectTarget19c3(Pending& p,Lease& lease,const void* henryActor,
    const void* hs,std::uint64_t now,const void*& target,const void*& entity,const void*& state) noexcept {
    if(!OriginalContactOwned19c3(p,now))return false;
    const auto result=ResolveDirectTarget19c3(p,henryActor,hs,now,target,entity,state);
    if(result==TargetResolution19c3::Ready)return true;
    if(result==TargetResolution19c3::Missing){
        if(!p.targetWait19c3&&Allow(4,2)){char m[220]{};std::snprintf(m,sizeof(m),
            "C19C3 VR_TARGET_WAIT serial=%u gesture=%llu entity=%u ageMs=%llu leaseMs=160 claim=NONE source=ACTOR_UPDATE_MAP",
            p.serial,(unsigned long long)p.gesture59c3,p.entityId,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}
        p.targetWait19c3=true;lease.wait(p); // never alter tick/queued/sample/serial/key
    }else if(Allow(4,2)){char m[200]{};std::snprintf(m,sizeof(m),
        "C19C3 VR_HIT_SKIPPED serial=%u reason=TARGET_IDENTITY_INVALID entity=%u retained=%d claim=NONE",
        p.serial,p.entityId,p.targetWait19c3?1:0);diagnostic_trace::Write(m);}
    return false;
}
// C65H1: same original contact and actual actor/body resolver as DIRECT, but
// no phase/zone publication or native claim while the target remains missing.
template<class Lease> static bool AwaitHybridTarget65h(Pending& p,Lease& lease,const void* henryActor,
 const void* hs,std::uint64_t now,const void*& target,const void*& entity,const void*& state,const char*& via) noexcept {
 if(!OriginalContactOwned19c3(p,now))return false;
 target=ActorForEntity(p.entityId,now);via="MAP";
 if(!target){const void* opponent=Rd<const void*>(hs,0xCF8,nullptr);
  if(opponent&&opponent!=henryActor){const void *oe=nullptr,*os=nullptr;
   if(CombatActor(opponent,oe,os,false)&&oe&&EntityId(oe)==p.entityId){target=opponent;via="OPPONENT_SAME_ENTITY";g_stats[6].fetch_add(1,std::memory_order_relaxed);}}}
 if(target){
  if(target==henryActor||!CombatActor(target,entity,state,false)||!entity||EntityId(entity)!=p.entityId||
     ((p.targetWait19c3||p.metadata59c7g.occurrenceSerialH28)&&!RetainedContactBody19c3(p,entity))||!OriginalContactOwned19c3(p,now)){++g_stats[3];return false;}
  if(p.targetWaits65h&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
   g_hybrid_resolved65h.fetch_add(1,std::memory_order_relaxed);
   if(Allow(4,2)){char m[240]{};std::snprintf(m,sizeof(m),"C65H1 HYBRID_TARGET_WAIT resolved serial=%u gesture=%llu waits=%u ageMs=%llu body=RETAINED nativeClaim=NOT_YET",p.serial,(unsigned long long)p.gesture59c3,p.targetWaits65h,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}}
  return true;
 }
 namespace hw=kcd2vr::hybrid_wait58;hw::Input ask{};
 ask.enabled=g_hybrid_wait65h.load(std::memory_order_acquire);ask.entityValid=p.entityId!=0;
 ask.contactOwned=OriginalContactOwned19c3(p,now);ask.colliderKnown=p.metadata59c7g.collider!=0;ask.waits=p.targetWaits65h;
 const auto decision=hw::decide(ask);
 if(decision.decision==hw::Decision::Wait){
  if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
   g_hybrid_waits65h.fetch_add(1,std::memory_order_relaxed);
   if(!p.targetWaits65h&&Allow(4,2)){char m[240]{};std::snprintf(m,sizeof(m),"C65H1 HYBRID_TARGET_WAIT serial=%u gesture=%llu target=%u ageMs=%llu leaseMs=160 nativeClaim=NONE",p.serial,(unsigned long long)p.gesture59c3,p.entityId,(unsigned long long)(now-p.tick));diagnostic_trace::Write(m);}}
  p.targetWait19c3=true;++p.targetWaits65h;lease.wait(p);return false;
 }
 ++g_stats[3];return false;
}
