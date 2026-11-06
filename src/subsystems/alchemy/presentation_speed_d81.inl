// The D65 slider only changed a CAF after Update, leaving the native parent at
// normal speed. Use the already pinned IAction +5C clock instead; native Update
// propagates it to the action's own scope/layers and dispatches the real events.
namespace ap81=kcd2vr::subsystems::alchemy::presentation_d81;
static SemiAlchemyLease46 g_alchemy_presentation_d81{};
static SemiAlchemyLease46 g_alchemy_presentation_previous_d81{};
static semi46::WorkWitness g_alchemy_presentation_foreign_d81{};
static std::atomic<bool> g_alchemy_presentation_pending_d81{false};

// F5 observes the actual pre-Update clock decision. The old slider-only
// receipts could not distinguish a faster CAF from a faster native action.
// No diagnostic native reads, formatting or queueing happen while F5 is off.
enum class AlchemyPresentationResultD84 {Applied,TrackedWork,ResourceWork,StaleInput,ForeignOwner,NativeChanged,WriteRefused,NativeWork};
static void NoteAlchemyPresentationD84(const semi46::WorkWitness& w,ap81::Kind kind,
    AlchemyPresentationResultD84 result,float requested,float effective,float dt,float scale,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    if(!cap::active(cap::Domain::Interaction,now))return;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gateD84;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    if(!gateD84.observe(session,now,{w.child,w.token,int(kind),int(result)},100,1024,500))return;
    constexpr const char* outcomes[]{"APPLIED","TRACKED_WORK_OWNS_CLOCK","ATTACHED_RESOURCE_WORK",
        "STALE_OR_UNOWNED_TRACKING","FOREIGN_CLOCK_OWNER","NATIVE_CHILD_CHANGED","WRITE_NOT_VERIFIED","NATIVE_BOOK_OR_WORK_TIME"};
    char line[720]{};
    std::snprintf(line,sizeof(line),
        "D84 F5_ALCHEMY_PRESENTATION session=%llu tick=%llu instance=%llX life=%llu parent=%llX child=%llX token=%u kind=%u result=%s requestedRate=%.3f appliedRate=%.3f nativeDt=%.6f controllerScale=%.3f timing=BEFORE_NATIVE_UPDATE globalDt=UNCHANGED eventOutcome=NOT_OBSERVED zeroRate=D85_CARAFE_WAIT_ONLY",
        (unsigned long long)session,(unsigned long long)now,(unsigned long long)w.instance,(unsigned long long)w.lifetime,
        (unsigned long long)w.parent,(unsigned long long)w.child,w.token,unsigned(kind),outcomes[unsigned(result)],
        double(requested),double(result==AlchemyPresentationResultD84::Applied?effective:0.f),double(dt),double(scale));
    diagnostic_trace::Write(line);
}

static bool RestoreAlchemyPresentationD81() noexcept {
    auto& l=g_alchemy_presentation_d81;if(!l.written)return true;
    bool restored=false;
    void* root=nullptr;const int member=SemiAlchemyMembership46(l,root);
    if(member<0)return false;
    if(member){
        auto* child=reinterpret_cast<void*>(l.owner.child);
        std::uintptr_t vft=0,back=0;std::uint32_t token=0;void* context=nullptr;void* expected=nullptr;
        if(!SehCopy(child,&vft,sizeof(vft))||!SehCopy(static_cast<unsigned char*>(child)+kActToken,&token,sizeof(token))||
           !SehCopy(static_cast<unsigned char*>(child)+0x268,&back,sizeof(back))||
           !SehCopy(static_cast<unsigned char*>(child)+kActContext,&context,sizeof(context))||
           !SehCopy(static_cast<unsigned char*>(l.controller)+kCtrlContext,&expected,sizeof(expected)))return false;
        if(vft==g_base.load()+0x3A47948&&token==l.owner.token&&back==l.owner.instance&&context==expected){
            float value=-1.f;
            if(!SehCopy(static_cast<unsigned char*>(child)+kActSpeedBias,&value,sizeof(value))||!std::isfinite(value))return false;
            if(value==l.lastWritten||(l.writeUncertain&&value==l.saved)){
                if(!Wr<float>(child,kActSpeedBias,l.saved)||Rd<float>(child,kActSpeedBias,-1.f)!=l.saved)return false;
                restored=true;
            }else g_alchemy_presentation_foreign_d81=l.owner; // a native/other writer wins for this child
        }
    }
    if(restored)g_alchemy_presentation_previous_d81=l;
    l={};g_alchemy_presentation_pending_d81.store(false,std::memory_order_release);return true;
}
static bool AlchemyPresentationRateD81(const semi46::WorkWitness& w,float& rate) noexcept {
    rate=1.f;if(semi46::same(w,g_alchemy_presentation_foreign_d81))return false;
    const auto& l=g_alchemy_presentation_d81.written?g_alchemy_presentation_d81:g_alchemy_presentation_previous_d81;
    if(!l.owner.child||!semi46::same(w,l.owner)||l.saved!=1.f||l.lastWritten<=1.f||l.writeUncertain)return false;
    rate=l.lastWritten;return true;
}
static bool HandoffAlchemyPresentationD81(const semi46::WorkWitness& w) noexcept {
    return !g_alchemy_presentation_d81.written||!semi46::same(w,g_alchemy_presentation_d81.owner)||RestoreAlchemyPresentationD81();
}
static bool RestoreAlchemyPresentationBoundaryD81(void* ctrl) noexcept {
    if(!g_alchemy_presentation_pending_d81.load(std::memory_order_acquire))return true;
    if(ctrl!=g_henry_ctrl.load(std::memory_order_acquire))return false;
    OwnerTransaction19c4 transaction;if(!transaction)return false;
    AlchemyUpdateScopeR27 scope(ctrl);return OnAlchemyUpdateOwnerR27()&&RestoreAlchemyPresentationD81();
}
static bool ReadAlchemyPresentationD81(void* ctrl,semi46::WorkWitness& w,ap81::Kind& kind) noexcept {
    w={};kind=ap81::Kind::None;transfer65::Key key{};
    if(ReadAlchemyTransferD65(ctrl,key)){
        w.instance=key.instance;w.lifetime=key.lifetime;w.parent=key.parent;w.child=key.child;w.token=key.token;
        const auto parentVft=Rd<std::uintptr_t>(reinterpret_cast<void*>(key.parent),0,0);
        kind=ap81::transferKind(parentVft-g_base.load(std::memory_order_acquire));return true;
    }
    if(!SemiAlchemyPins46()||!SemiAlchemyParent46(ctrl,w)||!w.child)return false;
    if(w.kind==semi46::Book)kind=ap81::Kind::Book;
    else if(w.kind==semi46::Resource)kind=ap81::Kind::Resource;
    else return false; // bellows/grinding/mortar work remains physically driven
    return true;
}
static bool AlchemyPresentationInputD81(const semi46::WorkWitness& w,std::uint64_t now) noexcept {
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 station{};
    kcd2vr::workstation_sample46::Frame f{};
    return kcd2vr::workstation178r20c1::workstationOwnerH51(now,station)&&station.instance==w.instance&&
        station.lifetime==w.lifetime&&station.kind==kcd2vr::minigame_hands45::Alchemy&&
        frik_pose_probe::WorkstationFrame46(f)&&f.allowed&&kcd2vr::workstation_sample46::fresh(f,now)&&
        ((f.hand[0].valid&&kcd2vr::workstation_sample46::finite(f.hand[0]))||
         (f.hand[1].valid&&kcd2vr::workstation_sample46::finite(f.hand[1])));
}
// D85 craft: the carafe (Resource) child follows the left hand. Same exact
// child, same pre-Update +5C lease and same restore as the D81 presentation;
// this only chooses 0 (wait), or the fast rate, per native stage.
namespace carafe85=kcd2vr::subsystems::alchemy::carafe_d85;
static carafe85::Controller g_carafe_d85;
static bool CarafeTrackedD85(std::uint64_t now) noexcept {
    kcd2vr::workstation_sample46::Frame f{};
    return frik_pose_probe::WorkstationFrame46(f)&&f.allowed&&kcd2vr::workstation_sample46::fresh(f,now)&&
        f.hand[1].valid&&kcd2vr::workstation_sample46::finite(f.hand[1]);
}
static carafe85::Observation CarafeObservationD85(const semi46::WorkWitness& w,std::uint64_t now) noexcept {
    namespace ab=kcd2vr::subsystems::alchemy::bridge67;
    carafe85::Observation o{};o.key={w.instance,w.parent,w.child,w.lifetime,w.token};
    const unsigned seen=carafe85::ledger.read(w.child,w.token);
    o.used=(seen&carafe85::UseSeen)!=0;o.detached=(seen&carafe85::DetachSeen)!=0;
    ab::NativeTool t{};
    const bool joined=ab::tools[1].read(t)&&t.joined&&t.side==1&&t.type>=13&&t.type<=16&&t.entity&&
        t.instance==w.instance&&t.lifetime==w.lifetime&&t.parent==w.parent&&semi46::fresh(t.tick,now)&&
        t.binding&&t.binding.action==w.parent&&t.binding.instance==w.instance&&t.binding.side==1&&t.binding.type==t.type;
    o.attached=joined||(seen&carafe85::AttachSeen)!=0;
    ab::Control c{};
    o.control=ab::control.read(c)&&c.enabled&&!c.passenger&&semi46::fresh(c.tick,now)&&c.instance==w.instance&&
        c.lifetime==w.lifetime&&(!c.parent||c.parent==w.parent)&&c.side==1&&c.type>=13&&c.type<=16&&
        c.modeEpoch==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)&&(!joined||c.entity==t.entity);
    o.bits=o.control?c.carafeD85:0u;o.tracked=CarafeTrackedD85(now);
    return o;
}
static void NoteCarafeD85(const semi46::WorkWitness& w,const carafe85::Decision& d,const carafe85::Observation& o,float applied,std::uint64_t now) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    if(!cap::active(cap::Domain::Interaction,now))return;
    if(d.announce)(void)kcd2vr::visible_failure::publishDiagnosticD74(
        carafe85::reason(d.why),now,cap::Domain::Interaction);
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gateD85;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
    if(!gateD85.observe(session,now,{w.child,w.token,int(d.stage),int(o.bits)},100,1024,500))return;
    char line[600]{};
    std::snprintf(line,sizeof(line),
        "D85 F5_CARAFE_POUR session=%llu tick=%llu parent=%llX child=%llX token=%u stage=%s why=%s rate=%.3f attached=%d used=%d detached=%d tracked=%d control=%d bits=%u nativeEvents=ORIGINAL_ONCE",
        (unsigned long long)session,(unsigned long long)now,(unsigned long long)w.parent,(unsigned long long)w.child,w.token,
        carafe85::name(d.stage),d.why==carafe85::Why::None?"NONE":carafe85::reason(d.why),double(applied),
        o.attached,o.used,o.detached,o.tracked,o.control,o.bits);
    diagnostic_trace::Write(line);
}
static void TickAlchemyPresentationD81(void* ctrl,float dt,std::uint64_t now) noexcept {
    if(!OnAlchemyUpdateOwnerR27()||g_owner.state!=Idle||g_alchemy_transfer_stop_d65.load()||
       g_alchemy_presentation_pending_d81.load()||!AlchemyStagePins19c4())return;
    const float requested=g_alchemy_transfer_rate_d65.load(),scale=Rd<float>(ctrl,kCtrlTimeScale,-1.f);
    float speed=ap81::rate(requested,dt,scale);
    // D85 craft: a mode3 carafe may need a 0 (wait) even when the slider is 1x.
    const bool carafeModeD85=kcd2vr::workstation178r20c1::fullVrAlchemy63(now);
    if(speed<=1.f&&!carafeModeD85)return;
    semi46::WorkWitness w{};ap81::Kind kind{};
    const bool readD85=ReadAlchemyPresentationD81(ctrl,w,kind);
    // D85 craft: retail reuses child storage and the same fragment token for
    // the next carafe. Forget a carafe's event ledger once its action is gone
    // (or replaced) for 300 ms, so a later one never inherits Use/Detach.
    static std::uint64_t carafeAbsentD85=0;
    if(readD85&&kind==ap81::Kind::Resource)carafeAbsentD85=0;
    else if(g_carafe_d85.key().child){
        if(!carafeAbsentD85||now<carafeAbsentD85)carafeAbsentD85=now;
        else if(now-carafeAbsentD85>=300){g_carafe_d85.reset();carafe85::ledger.clear();carafeAbsentD85=0;}
    }
    if(!readD85)return;
    if(kind==ap81::Kind::Book||kind==ap81::Kind::Work){
        NoteAlchemyPresentationD84(w,kind,AlchemyPresentationResultD84::NativeWork,requested,1.f,dt,scale,now);
        return;
    }
    if(speed<=1.f&&kind!=ap81::Kind::Resource)return;
    if(semi46::same(w,g_alchemy_presentation_foreign_d81)){
        NoteAlchemyPresentationD84(w,kind,AlchemyPresentationResultD84::ForeignOwner,requested,1.f,dt,scale,now);return;}
    const bool other=(g_semi_alchemy46.written&&g_semi_alchemy46.owner.child==w.child)||
        (g_alchemy_stage19c4.written&&g_alchemy_stage19c4.proof.child==w.child)||g_pickup_clock_live_h23.load();
    // Once genuine Attach has established a resource occurrence, leave its
    // work/release phases to the existing physical/native owner, even if its
    // clock claim was refused. Only the pre-Attach presentation is accelerated.
    const bool resourceWork=kind==ap81::Kind::Resource&&semi46::same(w,g_semi_resource46.owner);
    const bool current=AlchemyPresentationInputD81(w,now);
    if(kind==ap81::Kind::Resource&&carafeModeD85&&!other&&!resourceWork){
        // The C46 claim and any tracked owner above still win; this path runs
        // only for the never-claimed child the D79 headset capture showed.
        auto o=CarafeObservationD85(w,now);if(!current)o.tracked=false;
        const auto d=g_carafe_d85.step(o,std::max(1.f,std::isfinite(requested)?requested:1.f),now);
        speed=d.rate<=0.f?0.f:d.rate>1.f?ap81::rate(d.rate,dt,scale):1.f;
        NoteCarafeD85(w,d,o,speed,now);
        if(speed==1.f||!current)return; // native 1x needs no lease; stale input never writes
    }else if(!ap81::allowed(kind,current,other,resourceWork)){
        NoteAlchemyPresentationD84(w,kind,other?AlchemyPresentationResultD84::TrackedWork:
            resourceWork?AlchemyPresentationResultD84::ResourceWork:AlchemyPresentationResultD84::StaleInput,
            requested,1.f,dt,scale,now);return;}
    SemiAlchemyLease46 candidate{};candidate.owner=w;candidate.controller=ctrl;
    void* root=nullptr;if(SemiAlchemyMembership46(candidate,root)!=2)return;
    auto* child=reinterpret_cast<void*>(w.child);
    if(Rd<std::uintptr_t>(child,0,0)!=g_base.load()+0x3A47948||Rd<std::uint32_t>(child,kActToken,~w.token)!=w.token||
       Rd<std::uintptr_t>(child,0x268,0)!=w.instance||Rd<int>(child,kActStatus,-1)!=2||
       Rd<void*>(child,kActContext,nullptr)!=Rd<void*>(ctrl,kCtrlContext,nullptr)||Rd<float>(child,kActSpeedBias,-1.f)!=1.f){
        NoteAlchemyPresentationD84(w,kind,AlchemyPresentationResultD84::NativeChanged,requested,1.f,dt,scale,now);return;}
    semi46::WorkWitness again{};ap81::Kind againKind{};
    if(!ReadAlchemyPresentationD81(ctrl,again,againKind)||!semi46::same(w,again)||againKind!=kind||
       !AlchemyPresentationInputD81(w,GetTickCount64()))return;
    candidate.root=root;candidate.saved=1.f;candidate.lastWritten=speed;candidate.previousWritten=1.f;
    candidate.written=candidate.writeUncertain=true;g_alchemy_presentation_d81=candidate;
    g_alchemy_presentation_pending_d81.store(true,std::memory_order_release);
    if(Wr<float>(child,kActSpeedBias,speed)&&Rd<float>(child,kActSpeedBias,-1.f)==speed){
        g_alchemy_presentation_d81.writeUncertain=false;
        NoteAlchemyPresentationD84(w,kind,AlchemyPresentationResultD84::Applied,requested,speed,dt,scale,now);
    }else{
        NoteAlchemyPresentationD84(w,kind,AlchemyPresentationResultD84::WriteRefused,requested,1.f,dt,scale,now);
        (void)RestoreAlchemyPresentationD81();}
}
