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
