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
