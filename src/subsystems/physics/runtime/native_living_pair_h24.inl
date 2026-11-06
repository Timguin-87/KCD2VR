// Same namespace and same exact-retail T4 context as native_physics_phase_t4.cpp.
// Retail proof: living vtable +10/+18 uses 5311C4/531F84; A9FE50 submits
// type5 flags300; rigid +150=BCC44C checks flag200 and exact stored buddy.
struct LivingAccessH24 {
    static bool Qualified(Context& c,const LivingSourceH24& s) noexcept {
        if(!c.Current()||s.physicsId<0||!s.body||!s.entity||nativeFaulted.load())return false;
        std::uintptr_t vt=0;void* w=nullptr;void* foreign=nullptr;
        std::int16_t kind=-1;std::int8_t sim=-1;int deleting=-1;unsigned flags=0,klass=0;
        const auto before=faults.load(std::memory_order_relaxed);
        return Call(api.lookup,c.World(),s.physicsId)==s.body&&
            faults.load(std::memory_order_relaxed)==before&&
            Read(s.body,0,vt)&&vt==base+0x3A82570&&Read(s.body,0x68,w)&&w==c.World()&&
            Read(s.body,0x20,foreign)&&foreign==s.entity&&Read(s.body,0x28,kind)&&kind==2&&
            Read(s.body,0x47,sim)&&sim==3&&Read(s.body,0x50,deleting)&&deleting==0&&
            Read(s.body,0x84,flags)&&(flags&0x80000000u)==0&&Read(s.body,0xEC,klass)&&(klass&4u)&&
            SlotIs(0x3A82570,0x10,0x5311C4)&&SlotIs(0x3A82570,0x18,0x531F84)&&
            Call(api.lookup,c.World(),s.physicsId)==s.body&&faults.load(std::memory_order_relaxed)==before;
    }
    static bool Current(Context& c,LivingLeaseH24& l) noexcept {
        return Access::Same(c,l.retained_)&&Qualified(c,l.source_);
    }
    static void* Body(Context& c,LivingLeaseH24& l) noexcept{return Current(c,l)?l.retained_.body_:nullptr;}
    static bool Take(Context&,const LivingSourceH24&,LivingLeaseH24&) noexcept;
    static bool Matches(Context& c,LivingLeaseH24& l,const LivingSourceH24& s) noexcept {
        return s.body==l.source_.body&&s.entity==l.source_.entity&&s.physicsId==l.source_.physicsId&&Current(c,l);
    }
    static bool Drop(Context& c,LivingLeaseH24& l,bool abandon) noexcept {
        if(!(abandon?AbandonInvalidated(c,l.retained_):Release(c,l.retained_)))return false;l.source_={};return true;
    }
};
bool LivingAccessH24::Take(Context& c,const LivingSourceH24& s,LivingLeaseH24& l) noexcept {
    if(l.Held()||c.Kind()!=Phase::Step||!LivingAccessH24::Qualified(c,s)||!IndependentReference(c.World()))return false;
    // Capture the qualified identity BEFORE a native call can revoke Context.
    // Getters deliberately return null/0 after loss; those are not the owner of
    // a successfully acquired native reference and must not replace it.
    const auto original=s;void* const world=c.World();const auto generation=c.Generation();
    if(!world||!generation)return false;
    const auto before=faults.load(std::memory_order_relaxed);
    if(Call(api.addRef,original.body)<=0)return false;
    auto& ref=l.retained_;ref.body_=original.body;ref.world_=world;ref.id_=original.physicsId;
    ref.generation_=generation;ref.held_=true;ref.pending_=false;l.source_=original;
    acquisitions.fetch_add(1,std::memory_order_relaxed);
    // Successful retention survives a failed second observation for cleanup.
    return faults.load(std::memory_order_relaxed)==before&&LivingAccessH24::Current(c,l);
}
bool AcquireLivingH24(Context& c,const LivingSourceH24& s,LivingLeaseH24& l) noexcept {return LivingAccessH24::Take(c,s,l);}
bool CurrentLivingH24(Context& c,LivingLeaseH24& l,const LivingSourceH24& s) noexcept {return LivingAccessH24::Matches(c,l,s);}
bool ReleaseLivingH24(Context& c,LivingLeaseH24& l) noexcept {
    // Only common strong-reference release; not the living Action/SetParams.
    return LivingAccessH24::Drop(c,l,false);
}
bool AbandonLivingH24(Context& c,LivingLeaseH24& l) noexcept {
    return LivingAccessH24::Drop(c,l,true);
}
ConstraintResult AddSelfIgnorePairH24(Context& c,Lease& source,LivingLeaseH24& living) noexcept {
    ConstraintResult out{};if(c.Kind()!=Phase::Step)return out;
    void* src=NativeHandle(c,source);void* dst=LivingAccessH24::Body(c,living);
    if(!src||!dst||src==dst||!SlotIs(kRigidVft,0x150,0xBCC44C))return out;
    auto packet=constraint_packet60::defaults();constraint_packet60::setBuddy(packet,reinterpret_cast<std::uintptr_t>(dst));
    // Deliberately inactive, exact-pair exclusion only; no force/motor constraints.
    constraint_packet60::setIgnorePairOnly(packet);
    constraint_packet60::put<float>(packet,0x64,0.f); // A9FEBB qword300 also clears following float
    constraint_packet60::setPoint(packet,0,0,0,0);constraint_packet60::setPoint(packet,1,0,0,0);
    // Native A9FE50 sets both constraint frames to identity for pair-only use.
    for(unsigned o:{0x30u,0x34u,0x38u,0x40u,0x44u,0x48u})constraint_packet60::put<float>(packet,o,0.f);
    constraint_packet60::put<float>(packet,0x3C,1.f);constraint_packet60::put<float>(packet,0x4C,1.f);
    const auto before=faults.load(std::memory_order_relaxed);
    const int result=Call(api.action,src,static_cast<void*>(&packet),1);out.nativeReturn=result;
    if(faults.load(std::memory_order_relaxed)!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0){out.kind=ConstraintResult::Kind::Submitted;out.id=std::uint32_t(result);}return out;
}
bool SelfIgnorePairPresentH24(Context& c,Lease& source,LivingLeaseH24& living,std::uint32_t id) noexcept {
    void* expected=LivingAccessH24::Body(c,living);ConstraintSnapshot observed{};
    return expected&&id&&ReadConstraint(c,source,int(id),observed)&&observed.exists&&
        observed.buddy==expected&&(observed.flags&0x300u)==0x300u;
}

ConstraintResult RemoveSelfIgnorePairH24(Context& c,Lease& source,std::uint32_t id) noexcept {
    ConstraintResult out{};
    if((c.Kind()!=Phase::Step&&c.Kind()!=Phase::Shutdown&&c.Kind()!=Phase::WorldReset)||!id||id>=0x80000000u)return out;
    void* src=NativeHandle(c,source);if(!src)return out;
    auto packet=constraint_packet60::removeDefaults();constraint_packet60::setRemoveId(packet,id);
    const auto before=faults.load(std::memory_order_relaxed);
    const int result=Call(api.action,src,static_cast<void*>(&packet),1);out.nativeReturn=result;
    if(faults.load(std::memory_order_relaxed)!=before){out.kind=ConstraintResult::Kind::Uncertain;return out;}
    if(result>0)out.kind=ConstraintResult::Kind::Submitted;return out;
}
