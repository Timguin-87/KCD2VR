// C40 COUNTER STAB (DIRECT experiment, default OFF): a THRUST into the linked enemy's attack START (C39 window, same state), from the
// opposite side (C39 counterSide = native master-strike stance) with enough stroke energy gets strength x mul (cap = power rule's
// final max) on its ONE collision-owned BuildAndSubmitMeleeHit. Nothing else is written; the game picks damage, tier and reaction.
namespace counter40 {
namespace cs=kcd2vr::counter_stab40;
struct Eval {cs::Why why=cs::Why::Disabled;float before=0.f,after=0.f;bool candidate=false;bool instantR120=false;};
// C44: Henry's soul ability table (soul+0x320 {data,count}, sorted 16-byte rows, exact C_Soul vtable), one bounded copy per call.
static bool SoulAbilities44(const void* henryActor,std::array<cs::AbilityRow,512>& rows,std::size_t& count,bool& allFlag) noexcept {
    count=0;allFlag=false;const void* owner=Rd<const void*>(henryActor,0x1F8,nullptr);const void* soul=owner?Rd<const void*>(owner,0x668,nullptr):nullptr;
    if(!soul||Rd<std::uintptr_t>(soul,0,0)!=g_base+0x3F44F28ull)return false;
    const auto data=Rd<std::uintptr_t>(soul,0x320,0);const auto n=Rd<std::uint64_t>(soul,0x328,~0ull);
    if(n>512||(n&&!data))return false;
    if(n&&!SehCopy146(reinterpret_cast<const void*>(data),rows.data(),std::size_t(n)*sizeof(cs::AbilityRow)))return false;
    for(std::size_t i=1;i<n;++i)if(rows[i-1].id>rows[i].id)return false; // not the sorted native table: unknown
    count=std::size_t(n);const void* g=Rd<const void*>(reinterpret_cast<const void*>(g_base+0x53322A0ull),0,nullptr);const void* c=g?Rd<const void*>(g,0xA8,nullptr):nullptr;
    allFlag=c&&Rd<int>(c,0x60,0)!=0; // 9DCD80: this global flag makes every ability true
    return true;
}
// C44: the game's "Master strike" perk = soul ability 73 (perk_soul_ability.xml); unreadable = not learned (fail closed).
static bool MasterStrikeLearned44(const void* henryActor,bool& known) noexcept {
    static thread_local std::array<cs::AbilityRow,512> rows{};std::size_t n=0;bool all=false;known=SoulAbilities44(henryActor,rows,n,all);
    return known&&(all||cs::hasAbility(rows.data(),n,cs::kMasterStrikeAbility));
}
// DS-F19 (Claude review 12): short plain words for the VISIBLE line only; the
// receipts keep cs::name() tokens.
static const char* CounterPlain40(cs::Why w) noexcept {
    switch(w){case cs::Why::Qualified:return "ok";case cs::Why::Disabled:return "off";
        case cs::Why::NotThrust:return "not a thrust";case cs::Why::NoWindow:return "no attack window";
        case cs::Why::OtherTarget:return "other target";case cs::Why::NotAttacking:return "not attacking";
        case cs::Why::Late:return "bonus window elapsed";case cs::Why::Side:return "not opposite side";
        case cs::Why::LowEnergy:return "too weak";default:return "master strike not learned";}
}
static Eval Decide(const Pending& p,const void* henryActor,const void* ts,int origin,float power,std::uint64_t now) noexcept {
    namespace dc=kcd2vr::direct_counter39;Eval e{};e.before=e.after=power;
    // R120 INSTANT master strike (VrMasterStrikeModeR120=3): the gate hook saw the game's open master-strike window for his VR
    // press and skipped the paired clip; this blade contact on the SAME enemy is that master strike. full strike power (power
    // rule ceiling), and a stab gets the native MAJOR stab reaction through the existing counter path (majorReaction, thrust only).
    // independent of the counter-stab switch: the game already checked perk + window. one contact takes it, then it is gone.
    {namespace mg=kcd2vr::master_gate47;std::uint64_t taken=0;
     if(ts&&mg::takeGrant(reinterpret_cast<std::uintptr_t>(ts),now,&taken)){
        e.why=cs::Why::Qualified;e.instantR120=true;e.candidate=true;
        e.after=mg::instantPower(power,power59c8::g_final_max.load(std::memory_order_relaxed));
        char m[260]{};std::snprintf(m,sizeof(m),"R120 MS_INSTANT used grant=%llu serial=%u target=%u kind=%s strength=%.2f->%.2f majorReaction=%s",
            (unsigned long long)taken,p.serial,p.entityId,p.kind==1?"STAB":"CUT",double(e.before),double(e.after),p.kind==1?"REQUESTED_IF_LANDED":"NO(cut)");
        diagnostic_trace::Write(m);return e;}}
    cs::In in{};in.enabled=cs::enabled.load(std::memory_order_relaxed);in.thrust=p.kind==1;in.origin=origin;in.ratio=p.powerRatio59c8;
    bool perkKnown44=false;in.needsPerk=cs::needsPerk.load(std::memory_order_relaxed);in.perk=in.enabled&&in.needsPerk&&MasterStrikeLearned44(henryActor,perkKnown44); // read only when it decides
    in.minRatio=cs::clampEnergy(cs::minEnergy.load(std::memory_order_relaxed));in.windowMs=cs::windowMs.load(std::memory_order_relaxed);
    in.npcPhase=ts?Rd<int>(ts,0x38,-1):-1;
    const auto st=dc::openState.load(std::memory_order_acquire);in.windowOpen=st!=0;in.sameTarget=st&&st==reinterpret_cast<std::uintptr_t>(ts);
    const auto ot=in.sameTarget?dc::openTick.load(std::memory_order_relaxed):0;in.sinceStartMs=ot&&now>=ot?(long long)(now-ot):-1;
    const int offeredNow=ts?Rd<int>(ts,0x158,-1):-1;in.offered=offeredNow>=0?offeredNow:in.sameTarget?dc::openOffered.load(std::memory_order_relaxed):-1;
    e.why=cs::qualify(in);e.candidate=cs::candidate(in);
    if(e.why==cs::Why::Qualified)e.after=cs::boosted(power,cs::strengthMul.load(std::memory_order_relaxed),power59c8::g_final_max.load(std::memory_order_relaxed));
    // DS-F14: Miki's one visible line - a counter attempt that refused says why
    // (only when the stroke was actually aimed into his attack, not for every hit).
    if(e.candidate&&e.why!=cs::Why::Qualified){char visible[160]{};std::snprintf(visible,sizeof(visible),"Counter-stab bonus: %s",CounterPlain40(e.why));
        frik_pose_probe::PublishVisibleFailure(visible);}
    if((e.candidate||e.why==cs::Why::Qualified)&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[620]{};std::snprintf(line,sizeof(line),
        "C40 COUNTER_STAB edge=%s serial=%u target=%u reason=%s window=%llu sinceStartMs=%lld windowMs=%u npcPhase=%d origin=%d offered158=%d msStance=%d counterSide=%d energyRatio=%.2f minRatio=%.2f strength=%.2f->%.2f enabled=%d perkGate=%d masterStrikePerk73=%s nativeMaster=NOT_REQUESTED writes=STRENGTH_ARGUMENT_ONLY",
        e.why==cs::Why::Qualified?"QUALIFIED":"REFUSED",p.serial,p.entityId,cs::name(e.why),in.sameTarget?(unsigned long long)dc::openSerial.load(std::memory_order_relaxed):0ull,in.sinceStartMs,in.windowMs,
        in.npcPhase,in.origin,in.offered,dc::msZone(in.offered),int(dc::counterSide(in.origin,in.offered)),double(in.ratio),double(in.minRatio),double(e.before),double(e.after),in.enabled?1:0,
        in.needsPerk?1:0,!in.needsPerk?"NOT_CHECKED":!perkKnown44?"UNKNOWN(soul table unreadable)":in.perk?"LEARNED":"NOT_LEARNED");
        diagnostic_trace::Write(line);}
    return e;
}
// Henry's soul abilities that make the native hit add heavy_bleeding (read-only copy of soul+0x320, exact C_Soul vtable).
static unsigned HeavyBleedAbilities(const void* henryActor,bool& known,bool& allFlag) noexcept {
    known=false;allFlag=false;const void* owner=Rd<const void*>(henryActor,0x1F8,nullptr);const void* soul=owner?Rd<const void*>(owner,0x668,nullptr):nullptr;
    if(!soul||Rd<std::uintptr_t>(soul,0,0)!=g_base+0x3F44F28ull)return 0;
    const auto data=Rd<std::uintptr_t>(soul,0x320,0);const auto count=Rd<std::uint64_t>(soul,0x328,~0ull);
    if(count>512||(count&&!data))return 0;
    static thread_local std::array<cs::AbilityRow,512> rows{};
    if(count&&!SehCopy146(reinterpret_cast<const void*>(data),rows.data(),std::size_t(count)*sizeof(cs::AbilityRow)))return 0;
    for(std::size_t i=1;i<count;++i)if(rows[i-1].id>rows[i].id)return 0; // not the sorted native table: unknown
    known=true;const void* g=Rd<const void*>(reinterpret_cast<const void*>(g_base+0x53322A0ull),0,nullptr);const void* c=g?Rd<const void*>(g,0xA8,nullptr):nullptr;
    allFlag=c&&Rd<int>(c,0x60,0)!=0; // 9DCD80: this global flag makes every ability true
    return cs::heavyBleedBits(rows.data(),std::size_t(count));
}
static void Result(const Pending& p,const void* henryActor,const Eval& e,bool ok,bool apply,const void* hitInfo,const unsigned char* details,int kind,int dispatch,std::uint64_t now,bool majorRequested=false) noexcept {
    if(e.why!=cs::Why::Qualified||!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    bool known=false,all=false;const unsigned b=HeavyBleedAbilities(henryActor,known,all);char ab[160]{};
    if(known)std::snprintf(ab,sizeof(ab),"HorrendousWounds40=%u AimToMaim75=%u CuttingEdge81=%u AlwaysHeavyBleeding85=%u ForbiddenWeapon90=%u allAbilityFlag=%d",b&1u,(b>>1)&1u,(b>>2)&1u,(b>>3)&1u,(b>>4)&1u,all?1:0);
    else std::snprintf(ab,sizeof(ab),"UNKNOWN(soul table unreadable)");
    char line[900]{};std::snprintf(line,sizeof(line),
        "C40 COUNTER_STAB edge=RESULT serial=%u target=%u applied=%d strength=%.2f->%.2f dmg=%.2f stam=%.2f tier44=%d blocked=%d hitType=%s dispatch=%d majorRequested=%d reaction=SEE_C36_SAME_SERIAL bleeding=NOT_ADDED reason=%s henryNativeHeavyBleed{%s}",
        p.serial,p.entityId,ok&&apply?1:0,double(e.before),double(e.after),Rd<float>(hitInfo,0x30,0.f),Rd<float>(hitInfo,0x2C,0.f),Rd<std::int32_t>(hitInfo,0x44,-1),details?int(details[1]):-1,
        cs::hitTypeName(kind),dispatch,majorRequested?1:0,cs::kBleedRefusal,ab);diagnostic_trace::Write(line);
}
}
