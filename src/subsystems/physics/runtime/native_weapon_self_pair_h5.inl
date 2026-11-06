// Included inside the single weapon owner. No raw Henry handle or Context
// escapes T4; all native pair operations use the existing typed H24 API.
bool PairControlCurrentH5(const Input& in,std::uint64_t word) noexcept {
    return (word&1u)&&in.npcContactControlH5==word&&Fresh(in)&&
        deepseek_h29::ReadWeaponNpcContactControlH5()==word;
}
bool PairSourceCurrentH5(phase::Context& c,const Input& in) noexcept {
    auto& p=selfPairH5;
    return p.attempted&&p.source.physicsId==in.playerBodyId&&
        p.source.body==reinterpret_cast<void*>(in.playerBody)&&
        p.source.entity==reinterpret_cast<void*>(in.playerEntityPointer)&&
        PairControlCurrentH5(in,p.controlWord)&&phase::CurrentLivingH24(c,p.living,p.source)&&
        phase::SelfIgnorePairPresentH24(c,record.lease,p.living,p.id)&&PairControlCurrentH5(in,p.controlWord);
}
bool PairClassH5(phase::Context& c,void* body,unsigned ignore) noexcept {
    if(!body||(ignore!=0&&ignore!=4)||!c.Current())return false;
#if defined(KCD2VR_WEAPON_BODY_T4_HOST_TEST)
    return HostPairClassH5(c,body,ignore);
#else
    // Same pinned 28-byte retail class26 layout as the hand's H24 pair owner.
    // Replace ONLY our own body's ignore word; never edit Henry or an NPC.
    struct Class {int type=26;unsigned orType=0,orIgnore=0,orThird=0,
        andType=~0u,andIgnore=0,andThird=~0u;} cc;
    static_assert(sizeof(Class)==28);cc.orIgnore=ignore;
    using Fn=int(__fastcall*)(void*,void*,int);
    __try {
        unsigned type=~0u,observed=~0u;
        return Slot(module+0x3A48DC0,0x20,0x6F1BE4)&&
            reinterpret_cast<Fn>(module+0x6F1BE4)(body,&cc,1)>0&&
            ReadValue(std::uintptr_t(body)+0xEC,type)&&type==0&&
            ReadValue(std::uintptr_t(body)+0xF0,observed)&&observed==ignore;
    }__except(EXCEPTION_EXECUTE_HANDLER){fault=true;return false;}
#endif
}
bool ClosePairClassH5(phase::Context& c) noexcept {
    activeGeneration.store(0,std::memory_order_release);
    if(!selfPairH5.attempted&&!selfPairH5.living.Held())return true;
    selfPairH5.closing=true;
    void* body=phase::NativeHandle(c,record.lease,true);
    if(!body||!PairClassH5(c,body,4))return false;
    selfPairH5.open=false;return true;
}
void ClearPairH5() noexcept {
    // Caller has either never retained, or has observed original Absent and
    // released the living reference. The lease itself is never overwritten.
    selfPairH5.source={};selfPairH5.causal={};selfPairH5.id=0;
    selfPairH5.controlWord=selfPairH5.removeTick=0;selfPairH5.attempted=false;
    selfPairH5.removeAttempted=selfPairH5.open=selfPairH5.uncertain=selfPairH5.closing=false;
}
int RetirePairH5(phase::Context& c) noexcept {
    auto& p=selfPairH5;
    if(!p.attempted&&!p.living.Held())return 1;
    if(!ClosePairClassH5(c))return -1;
    void* body=phase::NativeHandle(c,record.lease,true);if(!body)return 0;
    // Exclusion and achieved-owner revocation precede detachment and removal.
    if(record.hasGeometry||record.partAttempted){
        if(!RemoveGeometry(c,body))return -1;
        record.hasGeometry=record.partAttempted=record.colliding=false;
    }
    if(p.uncertain)return -1; // no guessed id, no fabricated Absent/release
    if(p.id){
        phase::ConstraintSnapshot status{};
        if(!phase::ReadConstraint(c,record.lease,int(p.id),status))return 0;
        namespace admission=physics::self_pair_admission_h24;
        const auto seen=p.causal.observe(true,status.exists,
            status.buddy==p.source.body&&(status.flags&0x300u)==0x300u);
        if(seen==admission::Observation::WrongPair)return -1;
        if(seen==admission::Observation::AwaitPresent)return p.causal.timedOut(Now())?-1:0;
        if(status.exists){
            if(!p.removeAttempted){
                p.removeAttempted=true; // never repeat an uncertain native call
                p.removeTick=Now(); // cleanup starts here, not at the original Add
                if(phase::RemoveSelfIgnorePairH24(c,record.lease,p.id).kind!=phase::ConstraintResult::Kind::Submitted)return -1;
            }
            phase::ConstraintSnapshot absent{};
            if(!phase::ReadConstraint(c,record.lease,int(p.id),absent))return 0;
            if(absent.exists){
                if(absent.buddy!=p.source.body||(absent.flags&0x300u)!=0x300u)return -1;
                return p.removeTick&&Now()>=p.removeTick&&Now()-p.removeTick>1000?-1:0;
            }
        }
    }
    if(p.living.Held()&&!phase::ReleaseLivingH24(c,p.living))return -1;
    ClearPairH5();return 1;
}
Reason PreparePairH5(phase::Context& c,void* body) noexcept {
    auto& p=selfPairH5;const auto word=record.input.npcContactControlH5;
    if(p.attempted){
        if(!PairControlCurrentH5(record.input,p.controlWord)||
           !phase::CurrentLivingH24(c,p.living,p.source))return Reason::SelfPairClosingH5;
        if(!phase::SelfIgnorePairPresentH24(c,record.lease,p.living,p.id))
            return p.open||p.causal.timedOut(Now())?Reason::SelfPairClosingH5:Reason::SelfPairPendingH5;
        p.causal.presentSeen=true;
        if(!p.open){
            // Treat a possibly partial class write as open until safe closure.
            p.open=true;
            if(!PairControlCurrentH5(record.input,p.controlWord)||!PairClassH5(c,body,0)||
               !phase::CurrentLivingH24(c,p.living,p.source)||
               !phase::SelfIgnorePairPresentH24(c,record.lease,p.living,p.id)||
               !PairControlCurrentH5(record.input,p.controlWord))return Reason::SelfPairClosingH5;
        }
        return Reason::None;
    }
    if(!PairControlCurrentH5(record.input,word))return Reason::None;
    if(p.refusedWord==word)return Reason::SelfPairRefusedH5;
    p.attempted=true;p.controlWord=word;
    p.source={record.input.playerBodyId,reinterpret_cast<void*>(record.input.playerBody),
        reinterpret_cast<void*>(record.input.playerEntityPointer)};
    if(!phase::AcquireLivingH24(c,p.source,p.living)){
        if(p.living.Held()&&!phase::ReleaseLivingH24(c,p.living)){record.quarantined=true;return Reason::SelfPairClosingH5;}
        p.refusedWord=word;ClearPairH5();return Reason::SelfPairRefusedH5;
    }
    if(!PairControlCurrentH5(record.input,word)){return Reason::SelfPairClosingH5;}
    const auto added=phase::AddSelfIgnorePairH24(c,record.lease,p.living);
    if(added.kind!=phase::ConstraintResult::Kind::Submitted||!added.id||added.id>=0x80000000u){
        p.uncertain=added.kind==phase::ConstraintResult::Kind::Uncertain||
            (added.kind==phase::ConstraintResult::Kind::Submitted&&(!added.id||added.id>=0x80000000u));
        if(p.uncertain){record.quarantined=true;return Reason::SelfPairClosingH5;}
        if(!phase::ReleaseLivingH24(c,p.living)){record.quarantined=true;return Reason::SelfPairClosingH5;}
        p.refusedWord=word;ClearPairH5();return Reason::SelfPairRefusedH5;
    }
    p.id=added.id;p.causal.submittedTick=Now();
    return PairControlCurrentH5(record.input,word)?Reason::SelfPairPendingH5:Reason::SelfPairClosingH5;
}
