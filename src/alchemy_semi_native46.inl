// C46: native alchemy working-child ownership. Included only in the existing
// Henry action-controller adapter; native animation/results still run normally.
namespace semi46=kcd2vr::alchemy_semi46;
struct SemiAlchemyLease46 {
    semi46::WorkWitness owner{};void* controller=nullptr;void* root=nullptr;
    float saved=1.f,lastWritten=1.f,previousWritten=1.f;std::uint64_t consumed=0,poseEpoch=0,sourceEpoch=0;double consumedSeconds=0;
    kcd2vr::alchemy_work_clock_d47::Ledger clock{}; // consumedSeconds is producer cursor, not native completion
    kcd2vr::alchemy_resource_scope_d67r1::Proof resourceRoot3D67R1{};
    bool written=false,pendingNoted=false,writeUncertain=false,turnFastD86=false;
} g_semi_alchemy46;
semi46::WorkWitness g_semi_retired46{};
std::atomic<bool> g_semi_pending46{false},g_semi_stop46{false};
std::atomic<unsigned> g_semi_inflight46{0};
std::optional<DetourModKit::hook::Hook> g_semi_hooks46[5];
struct SemiResourceState46 {
    semi46::WorkWitness owner{};int phase=0,verb=-1;unsigned char raised=0,tags[20]{};
    float firstWork=0,useTime=0,detachTime67=0;std::uintptr_t eventList=0,skel=0;int animationId=-1;
    kcd2vr::subsystems::alchemy::resource_turn_d86::Proof turnD86{};
} g_semi_resource46;

// D67R1: WHGame+4B3A20 scales this installed action's scope/procedural dt;
// 4B3B43..5F writes the same bias to every allocated layer. FullBody is the
// normal 0+3 root, not three necessarily active CAFs. Qualify only the observed
// single-CAF/empty-extra-slot profile, including other native scope writers.
// This does not retime the separate kettle update at 17AEC54 or create events.
static kcd2vr::alchemy_resource_scope_d67r1::Result SemiResourceRoot3D67R1(
    const SemiResourceState46& interval,void* ctrl,
    const kcd2vr::alchemy_resource_scope_d67r1::Proof* previous=nullptr) noexcept {
    namespace rs=kcd2vr::alchemy_resource_scope_d67r1;
    rs::Result bad{};const auto b=g_base.load(std::memory_order_acquire);
    constexpr unsigned char layerWrite[]{0x8B,0x53,0x30,0x0F,0x28,0xD7,0x48,0x8B,0x06,0x41,0x03,0xD6,0x48,0x8B,0xCE,0xFF,0x90,0xB8,0x00,0x00,0x00,0x41,0xFF,0xC6,0x44,0x3B,0x73,0x34,0x72,0xE2};
    constexpr unsigned char scopeClock[]{0xF3,0x0F,0x59,0x73,0x58,0x0F,0x28,0xC6,0xF3,0x0F,0x58,0x43,0x60,0xF3,0x0F,0x58,0x43,0x64,0xF3,0x0F,0x58,0x83,0x1C,0x01,0x00,0x00};
    constexpr unsigned char procArg[]{0x0F,0x28,0xCE,0x48,0x8B,0xCB};
    constexpr unsigned char procTail[]{0xE9,0x28,0x06,0x00,0x00};
    // The verified-image lease may be pending/revoked even at the same base.
    // Cache immutable byte pins only; never cache permission to use the image.
    if(!b||!retail_build112::Ready(b))return bad;
    static std::atomic<std::uintptr_t> pinnedD67R1{0};
    if(pinnedD67R1.load(std::memory_order_acquire)!=b){
        if(!retail_build112::Ready(b)||!MatchBytes(b+0x4B3B43,layerWrite,sizeof(layerWrite))||
           !MatchBytes(b+0x4B3AA2,scopeClock,sizeof(scopeClock))||!MatchBytes(b+0x4B3AC4,procArg,sizeof(procArg))||
           !MatchBytes(b+0x4B3AF3,procTail,sizeof(procTail))){bad.reason=rs::Reason::Pins;return bad;}
        pinnedD67R1.store(b,std::memory_order_release); // immutable exact-image code; live objects are checked below
    }
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A47948),0x30,0)!=b+0x1A7E0E0||
       !OnAlchemyUpdateOwnerR27()||interval.owner.kind!=semi46::Resource||!interval.skel||
       interval.skel!=reinterpret_cast<std::uintptr_t>(animation_filter::PlayerSkeletonAnim()))return bad;
    const auto read=[](std::uintptr_t p,auto& v){return p&&SehCopy(reinterpret_cast<void*>(p),&v,sizeof(v));};
    std::uintptr_t root=0,character=0;
    if(!read(interval.owner.child+kActRootScope,root)||!read(interval.skel+kSkelChar,character))return bad;
    rs::Key key{b,reinterpret_cast<std::uintptr_t>(ctrl),root,interval.owner.child,interval.owner.instance,character,interval.skel,interval.owner.token,interval.animationId};
    rs::Rates rates{};
    if(g_semi_alchemy46.written&&semi46::same(interval.owner,g_semi_alchemy46.owner)){
        rates.saved=g_semi_alchemy46.saved;rates.last=g_semi_alchemy46.lastWritten;rates.previous=g_semi_alchemy46.previousWritten;
    }else{
        // D81's exact same-child presentation write may still be cached by
        // native scopes while Attach hands the action to physical pour work.
        float presentation=1.f;if(AlchemyPresentationRateD81(interval.owner,presentation))rates.last=presentation;
    }
    rates.otherWriter=(g_alchemy_stage19c4.written&&g_alchemy_stage19c4.proof.child==interval.owner.child)||g_pickup_clock_live_h23.load(std::memory_order_acquire);
    return rs::inspect(read,key,rates,previous);
}

struct MortarPlaceEvents70 {std::uintptr_t parent=0,child=0;std::uint64_t lifetime=0;unsigned token=0;int stage=0;} g_mortar_place_events70;
using MortarPlaceEventFn70=void(__fastcall*)(void*,void*,const void*);
static MortarPlaceEventFn70 g_mortar_place_original70=nullptr;
static bool MortarPlacePins70() noexcept {
 const auto b=g_base.load();if(!retail_build112::Ready(b))return false;
 static std::atomic<std::uintptr_t> pinned70{0};if(pinned70.load(std::memory_order_acquire)==b)return true;
 constexpr unsigned char entry70[]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x40,0x41,0x8b,0x40,0x1c};
 constexpr unsigned char slot70[]{0x48,0x8d,0x7b,0x70};
 const bool good70=MatchBytes(b+0x6C4A34,entry70,sizeof(entry70))&&MatchBytes(b+0x6C3B16,slot70,sizeof(slot70))&&
  Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A5BB90),0xF8,0)==b+0x6C3AC0;
 if(good70)pinned70.store(b,std::memory_order_release);return good70;
}
using SemiBellowsTick46=int(__fastcall*)(void*,float);
using SemiBookEnable46=void(__fastcall*)(void*,bool);
SemiBellowsTick46 g_semi_bellows_original46=nullptr;
SemiBookEnable46 g_semi_book_original46=nullptr;
static void SemiAlchemyReceipt46(const char* phase,const semi46::WorkWitness& w,float rate=-1.f) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    if(!DiagOn()||!(cap::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(cap::Domain::Interaction))))return;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(cap::Domain::Interaction,now))return;
    const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load();
    // Only transitions: no native-name lookup, formatting or per-frame trace.
    unsigned key=0;for(const char* p=phase;*p;++p)key=key*33+unsigned(*p);
    if(!gate.observe(session,now,{w.child,std::uint32_t(w.kind),int(w.phase),int(key)},0,96))return;
    const auto& lease=g_semi_alchemy46;
    semi46::Control sampleD49{};
    const bool sampleJoinedD49=semi46::control.read(sampleD49)&&sampleD49.instance==w.instance&&
        sampleD49.lifetime==w.lifetime&&sampleD49.parent==w.parent&&sampleD49.child==w.child&&sampleD49.token==w.token;
    char line[820]{};std::snprintf(line,sizeof(line),
        "C46 SEMI_ALCHEMY phase=%s kind=%u nativePhase=%d parent=%llX child=%llX token=%u instance=%llX lifetime=%llu speed=%.3f producerObserved=%.6f nativeObserved=%.6f pending=%.6f discarded=%.6f sample=%llu workClock=EXACT_CHILD nativeEvents=UNCHANGED worldTime=UNCHANGED nativeObservedIsLowerBound=1 producerJoined=%d producerSample=%llu physicalStop=%d producerTotal=%.6f",
        phase,w.kind,w.phase,(unsigned long long)w.parent,(unsigned long long)w.child,w.token,
        (unsigned long long)w.instance,(unsigned long long)w.lifetime,double(rate),lease.consumedSeconds,
        lease.clock.advanced,lease.clock.pending,lease.clock.discarded,(unsigned long long)lease.consumed,sampleJoinedD49,(unsigned long long)sampleD49.sequence,
        sampleD49.clockStop,sampleD49.totalSeconds);diagnostic_trace::Write(line);
}
static bool SemiAlchemyPins46() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b)||!AlchemyStagePins19c4())return false;
    static std::uintptr_t checked=0;static bool good=false;if(checked==b)return good;
    good=true;for(const auto& p:semi46::pins::all)good=good&&MatchBytes(b+p.rva,reinterpret_cast<const unsigned char*>(p.bytes),p.size);
    good=good&&Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3F60260),0x110,0)==b+0x17BAD08&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A5BD40),0xF8,0)==b+0xA95AEC&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A7D9D0),8,0)==b+0x1AAF020&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3A7D9D0),0x18,0)==b+0x1AAFB60&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+kSkelVft),0x170,0)==b+kSkelDispatch&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(b+kCtrlVft),0xE0,0)==b+0x1A73130;
    checked=b;return good;
}
static bool SemiAlchemyParent46(void* ctrl,semi46::WorkWitness& out,std::uintptr_t expected=0,bool inQueue=false,bool resourceObservation19=false) noexcept {
    out={};namespace act=kcd2vr::activity_input163;
    const auto b=g_base.load(std::memory_order_acquire);const auto& a=act::witness175;
    if(!b||!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||
       (!OnAlchemyUpdateOwnerR27()&&!(resourceObservation19&&g_owner_transaction_depth19c4&&expected))||!IsHenryController(ctrl)||
       !a.ready.load(std::memory_order_acquire)||a.transitionDepth175.load(std::memory_order_acquire))return false;
    const auto life=a.lifetime175.load(std::memory_order_acquire);
    const auto instance=a.active[unsigned(act::Kind175::Alchemy)].load(std::memory_order_acquire);
    const auto read=[](std::uintptr_t p,auto& v){return SehCopy(reinterpret_cast<void*>(p),&v,sizeof(v));};
    std::uintptr_t v=0,player=0,first=0,last=0,parent=0,back=0;unsigned id=0;unsigned char started=0;int slot=-1;
    if(!instance||!read(instance,v)||v!=b+0x3F60B08||!read(instance+0x2C,started)||started!=1||
       !read(instance+0x18,player)||!player||!read(player,v)||v!=b+0x3AA4B98||
       !read(player+0x30,id)||!id||id!=Rd<unsigned>(ctrl,kCtrlEntityId,0)||
       !read(instance+0x78,v)||v!=b+0x3BFA5B0||!read(instance+0xB0,first)||!read(instance+0xB8,last)||
       !first||last<first+16||last-first>256||(last-first)%8||!read(first+8,parent)||!parent||
       (expected&&parent!=expected)||!read(parent,v)||!read(parent+0x58,back)||back!=instance||
       !read(parent+0x40,back)||back!=instance+0x78||!read(parent+0x3C,slot)||slot!=1||
       !read(parent+0x28,started)||started!=1)return false;
    out.instance=instance;out.lifetime=life;out.parent=parent;out.tick=GetTickCount64();out.hands=3;
    std::ptrdiff_t childOffset=0;
    if(v==b+0x3F60260){out.kind=semi46::Bellows;childOffset=0x88;if(!read(parent+0x78,out.phase)||out.phase<0||out.phase>3)return false;out.working=out.phase==2;out.ready=out.phase==1;}
    else if(v==b+0x3F60770){out.kind=semi46::Mortar;childOffset=0x90;if(!read(parent+0x70,out.phase)||out.phase<0||out.phase>4)return false;out.working=out.phase==3||(inQueue&&out.phase==2);out.ready=out.phase==2&&!inQueue;}
    else if(v==b+0x3A5C670){out.kind=semi46::Resource;childOffset=0x78;out.phase=0;} // working segment is witnessed by its actual native event list
    else if(v==b+0x3A5BB90&&MortarPlacePins70()){
        kcd2vr::subsystems::alchemy::bridge67::Control intent70{};
        if(!semi46::mortarReady70.load(std::memory_order_acquire)||!kcd2vr::workstation178r20c1::fullVrAlchemy63(out.tick)||
           !kcd2vr::subsystems::alchemy::bridge67::control.read(intent70)||!intent70.enabled||intent70.passenger||!intent70.controllerMortar70||
           intent70.instance!=instance||intent70.lifetime!=life||!semi46::fresh(intent70.tick,out.tick)||
           intent70.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load())return false;
        out.kind=semi46::MortarPlace70;childOffset=0x70;out.working=true;out.phase=0;
        if(g_mortar_place_events70.parent==parent&&g_mortar_place_events70.lifetime==life)out.phase=g_mortar_place_events70.stage;
    }
    else if(v==b+0x3A5BD40){out.kind=semi46::Book;childOffset=0x98;out.phase=0;}
    else return false;
    if(!read(parent+childOffset,out.child))return false;
    if(out.child){if(!read(out.child,v)||v!=b+0x3A47948||!read(out.child+0x268,back)||back!=instance||!read(out.child+kActToken,out.token))return false;}
    else out.working=false;
    if(out.kind==semi46::MortarPlace70&&(g_mortar_place_events70.child!=out.child||g_mortar_place_events70.token!=out.token))out.phase=0;
    if(out.kind==semi46::Resource&&semi46::same(out,g_semi_resource46.owner)){
        unsigned char tags[20]{},raised=0;int verb=-1;
        if(!read(instance+0x304,verb)||!read(instance+0x428,raised)||
           !SehCopy(reinterpret_cast<void*>(out.child+kActTags),tags,sizeof(tags)))return false;
        if(verb==g_semi_resource46.verb&&raised==g_semi_resource46.raised&&std::memcmp(tags,g_semi_resource46.tags,sizeof(tags))==0){
            out.phase=g_semi_resource46.phase;out.working=out.phase==1||
                (out.phase==2&&kcd2vr::workstation178r20c1::fullVrAlchemy63(GetTickCount64()));
        }
    }
    std::uintptr_t again=0;
    if(!read(first+8,again)||again!=parent||!read(parent+0x58,again)||again!=instance||
       !read(parent+childOffset,again)||again!=out.child||!a.ready.load(std::memory_order_acquire)||
       a.transitionDepth175.load(std::memory_order_acquire)||life!=a.lifetime175.load(std::memory_order_acquire)||
       instance!=a.active[unsigned(act::Kind175::Alchemy)].load(std::memory_order_acquire))return false;
    if(resourceObservation19&&out.kind!=semi46::Resource)return false;
    return true;
}
static bool SemiAlchemyInput46(const semi46::WorkWitness& w,std::uint64_t now,semi46::Control& c) noexcept {
    namespace act=kcd2vr::activity_input163;
    if((kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)==kcd2vr::minigame_hands45::Native&&!kcd2vr::guided_crafting55::active(now))||
       !kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||
       !act::trackedHands59c7f[unsigned(act::Kind175::Alchemy)].load(std::memory_order_acquire)||
       g_semi_stop46.load(std::memory_order_acquire)||!semi46::hooksReady.load(std::memory_order_acquire)||
       !semi46::control.read(c)||!c.enabled||c.manualUse||!semi46::fresh(c.tick,now)||!c.sequence||!c.poseEpoch||!c.sourceEpoch||
       c.instance!=w.instance||c.lifetime!=w.lifetime)return false;
    const auto packed=kcd2vr::minigame_hands45::witness.load(std::memory_order_acquire);
    if(kcd2vr::minigame_hands45::kindOf(packed)!=kcd2vr::minigame_hands45::Alchemy)return false;
    kcd2vr::workstation_sample46::Frame f{};
    if(!frik_pose_probe::WorkstationFrame46(f)||!f.allowed||!kcd2vr::workstation_sample46::fresh(f,now)||
       f.poseEpoch!=c.poseEpoch||f.sourceEpoch!=c.sourceEpoch||f.sequence<c.sequence)return false;
    const auto tracked=[&](unsigned side){return c.tracked[side]&&f.hand[side].valid&&kcd2vr::workstation_sample46::finite(f.hand[side]);};
    if(kcd2vr::workstation178r20c1::fullVrAlchemy63(now)){
        kcd2vr::subsystems::alchemy::bridge67::Control requested{};
        if(!kcd2vr::subsystems::alchemy::bridge67::control.read(requested)||!requested.enabled||requested.passenger||
           requested.instance!=w.instance||requested.lifetime!=w.lifetime||!semi46::fresh(requested.tick,now)||
           requested.poseEpoch!=f.poseEpoch||requested.sourceEpoch!=f.sourceEpoch||
           requested.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||requested.side<0||requested.side>1||
           !requested.entity||!tracked(unsigned(requested.side)))return false;
        const bool kind=(w.kind==semi46::Resource&&requested.type>=13&&requested.type<=16&&requested.side==1)||
            ((w.kind==semi46::Mortar||w.kind==semi46::MortarPlace70)&&((requested.type==2&&requested.side==1)||(requested.type==3&&requested.side==0)))||
            (w.kind==semi46::Bellows&&requested.type==18&&requested.side==1&&requested.parent==w.parent&&
             ((requested.clockAdmissionD47&&requested.admissionTickR22)||
              Rd<unsigned>(reinterpret_cast<void*>(w.instance),0x2A4,21)==18));
        if(!kind||(w.kind==semi46::MortarPlace70&&!requested.controllerMortar70)||Rd<std::uint32_t>(reinterpret_cast<void*>(w.instance),0x150+4*requested.type,0)!=requested.entity)return false;
    }
    if(c.hand<-1||c.hand>1)return false;
    if(kcd2vr::guided_crafting55::active(now)&&!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)&&
       ((w.kind==semi46::Resource&&c.hand!=1)||((w.kind==semi46::Mortar||w.kind==semi46::Stir)&&c.hand!=0)))return false;
    if(c.hand>=0)return tracked(unsigned(c.hand))&&((w.kind!=semi46::Mortar&&w.kind!=semi46::Stir)||c.hand==0);
    return (w.kind==semi46::Mortar||w.kind==semi46::Stir)?tracked(0):(tracked(0)||tracked(1));
}
// -1: unreadable/unknown, 0: no native membership, 1: queued, 2: active root,
// 3: exiting root. The saved pointer is never dereferenced before membership.
static int SemiAlchemyMembership46(const SemiAlchemyLease46& l,void*& root) noexcept {
    root=nullptr;
    if(!OnAlchemyUpdateOwnerR27()||l.controller!=g_henry_ctrl.load(std::memory_order_acquire)||!IsHenryController(l.controller))return -1;
    unsigned n=0;unsigned char* scopes=nullptr;
    if(!SehCopy(static_cast<unsigned char*>(l.controller)+kCtrlScopeCount,&n,sizeof(n))||n>kMaxScopes||
       !SehCopy(static_cast<unsigned char*>(l.controller)+kCtrlScopes,&scopes,sizeof(scopes))||(n&&!scopes))return -1;
    int result=0;
    for(unsigned i=0;i<n;++i){void* playing=nullptr;void* exiting=nullptr;void* s=scopes+i*kScopeStride;
        if(!SehCopy(static_cast<unsigned char*>(s)+kScopeAction,&playing,sizeof(playing))||
           !SehCopy(static_cast<unsigned char*>(s)+kScopeExiting,&exiting,sizeof(exiting)))return -1;
        if(playing==reinterpret_cast<void*>(l.owner.child)||exiting==reinterpret_cast<void*>(l.owner.child)){
            void* nativeRoot=nullptr;if(!SehCopy(reinterpret_cast<void*>(l.owner.child+kActRootScope),&nativeRoot,sizeof(nativeRoot)))return -1;
            if(nativeRoot==s){root=s;result=playing==reinterpret_cast<void*>(l.owner.child)?2:3;}
        }
    }
    std::uintptr_t first=0,last=0;
    if(!SehCopy(static_cast<unsigned char*>(l.controller)+0x48,&first,sizeof(first))||
       !SehCopy(static_cast<unsigned char*>(l.controller)+0x50,&last,sizeof(last))||last<first||last-first>1024||(last-first)%8||(!first&&last))return -1;
    for(auto at=first;at<last;at+=8){std::uintptr_t action=0;if(!SehCopy(reinterpret_cast<void*>(at),&action,sizeof(action)))return -1;
        if(action==l.owner.child&&!result)result=1;}
    return result;
}
static bool SemiAlchemyRestore46(const char* reason="NATIVE_RESUME") noexcept {
    auto& l=g_semi_alchemy46;if(!l.written)return true;
    void* root=nullptr;const int member=SemiAlchemyMembership46(l,root);
    if(member<0){if(!l.pendingNoted){SemiAlchemyReceipt46("RESTORE_PENDING",l.owner);l.pendingNoted=true;}return false;}
    float speed=-1;bool same=false;
    if(member){auto* child=reinterpret_cast<void*>(l.owner.child);
        const auto b=g_base.load(std::memory_order_acquire);
        std::uintptr_t vft=0,back=0;std::uint32_t token=0;int status=-1;void* context=nullptr;void* expectedContext=nullptr;
        // A read failure is unknown ownership, never evidence that a paused
        // child ended. Retain the hook and lease until native membership AND
        // identity can be read, or membership proves the child is no longer live.
        if(!SehCopy(child,&vft,sizeof(vft))||!SehCopy(static_cast<unsigned char*>(child)+kActToken,&token,sizeof(token))||
           !SehCopy(static_cast<unsigned char*>(child)+0x268,&back,sizeof(back))||
           !SehCopy(static_cast<unsigned char*>(child)+kActStatus,&status,sizeof(status))||
           !SehCopy(static_cast<unsigned char*>(child)+kActContext,&context,sizeof(context))||
           !SehCopy(static_cast<unsigned char*>(l.controller)+kCtrlContext,&expectedContext,sizeof(expectedContext)))return false;
        same=vft==b+0x3A47948&&token==l.owner.token&&back==l.owner.instance&&context==expectedContext&&
            ((member==1&&status==1)||(member>=2&&status==2));
        if(same){
            if(!SehCopy(static_cast<unsigned char*>(child)+kActSpeedBias,&speed,sizeof(speed))||!std::isfinite(speed))return false;
            float active=-1.f;
            if(SehCopy(static_cast<unsigned char*>(child)+kActActiveTime,&active,sizeof(active)))
                (void)l.clock.observe(active,l.saved);
            const bool ours=speed==l.lastWritten||(l.writeUncertain&&speed==l.previousWritten);
            if(ours){
                if(!Wr<float>(child,kActSpeedBias,l.saved)||
                   !SehCopy(static_cast<unsigned char*>(child)+kActSpeedBias,&speed,sizeof(speed))||speed!=l.saved)return false;
            }
        }
    }
    SemiAlchemyReceipt46(same?reason:"CHILD_ENDED_NO_STALE_WRITE",l.owner,speed);
    g_semi_retired46=l.owner;l={};g_semi_pending46.store(false,std::memory_order_release);return true;
}
static bool SemiAlchemyAcquire46(void* ctrl,const semi46::WorkWitness& w,bool queued,bool resourceAttach=false) noexcept {
    auto& l=g_semi_alchemy46;if(g_owner.state!=Idle||l.written||!w.working||!w.child||semi46::same(w,g_semi_retired46)||!SemiAlchemyPins46())return false;
    if(w.kind==semi46::Resource&&(!resourceAttach||!semi46::same(w,g_semi_resource46.owner)||g_semi_resource46.phase!=1))return false;
    semi46::Control input{};if(!SemiAlchemyInput46(w,GetTickCount64(),input))return false;
    auto* child=reinterpret_cast<void*>(w.child);const int status=Rd<int>(child,kActStatus,-1);
    if((queued?status!=1:status!=2)||Rd<void*>(child,kActContext,nullptr)!=Rd<void*>(ctrl,kCtrlContext,nullptr)||
       Rd<std::uintptr_t>(child,0x268,0)!=w.instance||Rd<std::uint32_t>(child,kActToken,~w.token)!=w.token)return false;
    // A late mode/tracking recovery cannot claim to stop an already-crossed work
    // event. Such a clip completes natively; the next work queue is eligible.
    if(!queued&&!resourceAttach&&Rd<float>(child,kActActiveTime,-1.f)!=0.f)return false;
    const float speed=Rd<float>(child,kActSpeedBias,-1.f);if(!std::isfinite(speed)||speed<=0.f||speed>8.f)return false;
    float presentation=1.f;const bool presentationOwned=AlchemyPresentationRateD81(w,presentation);
    if(presentationOwned&&speed!=1.f&&speed!=presentation)return false;
    SemiAlchemyLease46 claim{};claim.owner=w;claim.controller=ctrl;claim.saved=presentationOwned?1.f:speed;
    claim.previousWritten=presentationOwned?presentation:speed;claim.lastWritten=0.f;claim.written=true;claim.writeUncertain=true;claim.poseEpoch=input.poseEpoch;claim.sourceEpoch=input.sourceEpoch;
    const float active=Rd<float>(child,kActActiveTime,-1.f);if(!std::isfinite(active)||active<0)return false;
    claim.clock.seed(active);
    void* root=nullptr;const int member=SemiAlchemyMembership46(claim,root);if(member<1||(queued&&member!=1)||(!queued&&member!=2))return false;
    if(w.kind==semi46::Resource&&root&&Rd<unsigned>(root,kScopeNumLayers,0)==3){
        const auto proof=SemiResourceRoot3D67R1(g_semi_resource46,ctrl);
        if(!proof)return false;
        claim.resourceRoot3D67R1=proof.proof;
    }
    // Return our intro lease before saving the work clock. Otherwise a pour
    // acquired during native Attach would later restore8x instead of native1x.
    if(presentationOwned&&(!HandoffAlchemyPresentationD81(w)||Rd<float>(child,kActSpeedBias,-1.f)!=1.f))return false;
    claim.root=root;l=claim;g_semi_pending46.store(true,std::memory_order_release);
    semi46::Control latest{};if(g_owner.state!=Idle||!SemiAlchemyInput46(w,GetTickCount64(),latest)||
        latest.poseEpoch!=claim.poseEpoch||latest.sourceEpoch!=claim.sourceEpoch){l={};g_semi_pending46.store(false,std::memory_order_release);return false;}
    if(!Wr<float>(child,kActSpeedBias,0.f)||Rd<float>(child,kActSpeedBias,-1.f)!=0.f){(void)SemiAlchemyRestore46("SEED_WRITE_REFUSED");return false;}
    l.writeUncertain=false;
    SemiAlchemyReceipt46(queued?"WORK_QUEUED_PAUSED":"WORK_INSTALLED_PAUSED",w,0.f);return true;
}
static void QueueSemiAlchemy46(void* ctrl,void* action) noexcept {
    if(!ctrl||!action||!semi46::hooksReady.load(std::memory_order_acquire)||g_semi_stop46.load(std::memory_order_acquire)||
       ctrl!=g_henry_ctrl.load(std::memory_order_acquire))return;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    AlchemyUpdateScopeR27 alchemyQueue(ctrl);
    if(!OnAlchemyUpdateOwnerR27())return;
    semi46::WorkWitness w{};if(!SemiAlchemyParent46(ctrl,w,0,true)||w.child!=reinterpret_cast<std::uintptr_t>(action)||!w.working)return;
    (void)semi46::work.publish(w);
    if(g_semi_alchemy46.written&&!semi46::same(g_semi_alchemy46.owner,w)&&!SemiAlchemyRestore46("NEXT_NATIVE_WORK"))return;
    (void)SemiAlchemyAcquire46(ctrl,w,true);
}
// Only the existing native mutation owner consumes read-only callback observations.
static void SemiResourceConsume19(void* ctrl,std::uint64_t now) noexcept;
static float SemiResourceTurnRateD86(void* ctrl,const semi46::WorkWitness& w,float dt,double effectiveDt,bool credit) noexcept;
static float SemiResourceTailRateD86(const semi46::WorkWitness& w,float ratio,double effectiveDt,bool home) noexcept;
static void TickSemiAlchemy46(void* ctrl,float dt,std::uint64_t now) noexcept {
    if(!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||!OnAlchemyUpdateOwnerR27()||!IsHenryController(ctrl))return;
    SemiResourceConsume19(ctrl,now);
    semi46::WorkWitness w{};const bool parent=SemiAlchemyParent46(ctrl,w);
    (void)semi46::work.publish(parent?w:semi46::WorkWitness{});
    semi46::Control input{};const bool current=parent&&w.working&&SemiAlchemyInput46(w,now,input)&&g_owner.state==Idle;
    auto& l=g_semi_alchemy46;
    if(l.written&&(!current||!semi46::same(l.owner,w)||input.poseEpoch!=l.poseEpoch||input.sourceEpoch!=l.sourceEpoch)&&!SemiAlchemyRestore46("WORK_OWNER_RELEASED"))return;
    if(l.written&&l.writeUncertain){(void)SemiAlchemyRestore46("UNCERTAIN_RATE_RESTORE");return;}
    if(!current)return;
    if(!l.written&&!SemiAlchemyAcquire46(ctrl,w,false))return;
    l.owner=w; // same exact child/token; keep terminal phase/tick receipts current
    void* root=nullptr;const int member=SemiAlchemyMembership46(l,root);
    if(member<0)return; // retained cleanup: never guess that an unreadable scope ended
    if(member!=2){if(member==0||member==3)(void)SemiAlchemyRestore46("WORK_NOT_ACTIVE");return;}
    if(l.resourceRoot3D67R1.child&&!SemiResourceRoot3D67R1(g_semi_resource46,ctrl,&l.resourceRoot3D67R1)){
        (void)SemiAlchemyRestore46("RESOURCE_ROOT3_OWNER_CHANGED_D67R1");return;
    }
    auto* child=reinterpret_cast<void*>(w.child);
    const float readback=Rd<float>(child,kActSpeedBias,-1.f);
    if(readback!=l.lastWritten){(void)SemiAlchemyRestore46("NATIVE_SPEED_WRITER_WINS");return;}
    if(Rd<int>(child,kActStatus,-1)!=2||Rd<std::uint32_t>(child,kActToken,~w.token)!=w.token||
       Rd<void*>(child,kActContext,nullptr)!=Rd<void*>(ctrl,kCtrlContext,nullptr)||Rd<std::uintptr_t>(child,0x268,0)!=w.instance){
        (void)SemiAlchemyRestore46("WORK_IDENTITY_CHANGED");return;}
    float controllerScale=-1.f;
    if(!SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlTimeScale,&controllerScale,sizeof(controllerScale))||
       !std::isfinite(controllerScale)||controllerScale<0.f||controllerScale>8.f){
        (void)SemiAlchemyRestore46("CONTROLLER_SCALE_UNQUALIFIED");return;}
    // Native Update calls exact GetTimeScale (+9C) before multiplying this
    // child's +5C. Compensate its existing scale in our credit conversion only;
    // never write controller/world scale or replace the native Update argument.
    const double effectiveDt=double(dt)*double(controllerScale);
    if(!l.clock.observe(Rd<float>(child,kActActiveTime,-1.f),l.saved)){
        (void)SemiAlchemyRestore46("NATIVE_TIME_UNQUALIFIED");return;}
    const bool identity=input.parent==w.parent&&input.child==w.child&&input.token==w.token;
    const bool newSample=identity&&input.sequence!=l.consumed;
    const double credit=newSample?input.totalSeconds-l.consumedSeconds:0.;
    float ratio=l.clock.rate(credit,newSample,!identity||input.clockStop,effectiveDt,
        std::isfinite(dt)&&dt>0.f&&dt<=.08f);
    if(newSample){
        if(!std::isfinite(input.totalSeconds)||input.totalSeconds<0){
            l.clock.stop();ratio=0;SemiAlchemyReceipt46("PRODUCER_TIME_INVALID_PAUSED",w,0.f);
        }else{
        // A producer may rebase after a sample gap while this exact native
        // child remains installed. Rebase paused; resuming its native speed
        // here could apply Use without a fresh physical work sample.
        if(input.totalSeconds<l.consumedSeconds){l.clock.stop();ratio=0;
            SemiAlchemyReceipt46("PRODUCER_REBASED_PAUSED",w,0.f);}
        // This cursor only acknowledges producer samples. Native progress is
        // accounted separately from active-time readback; capped credit stays
        // bounded and may run only while the current physical sample permits.
        l.consumedSeconds=input.totalSeconds;l.consumed=input.sequence;
        }
    }
    // D86: C46 owns this captured carafe, so D81's fallback rate cannot remove
    // Henry's pre-pour turn. Only the proven pre-event segment may run fast,
    // and only while fresh positive physical credit permits this exact child.
    const float turnRateD86=SemiResourceTurnRateD86(ctrl,w,dt,effectiveDt,
        identity&&!input.clockStop&&ratio>0.f&&l.clock.pending>0.);
    bool turnFastD86=turnRateD86>ratio;
    if(turnFastD86)ratio=turnRateD86;
    semi46::Control latest{};if(g_owner.state!=Idle||!SemiAlchemyInput46(w,GetTickCount64(),latest)||
        latest.poseEpoch!=l.poseEpoch||latest.sourceEpoch!=l.sourceEpoch){(void)SemiAlchemyRestore46("INPUT_REVOKED_BEFORE_WRITE");return;}
    // A newer XR sample can arrive between these two owner checks. Its arrival
    // does not revoke already-earned work while the same exact child still has
    // current motion permission. Consume only the earlier credited total here;
    // the next callback accounts for any newer credit once through the cursor.
    // Actual stop, rewind/rebase or a changed native child still discards debt.
    if(latest.sequence<input.sequence||latest.tick<input.tick||latest.clockStop||latest.parent!=input.parent||
       latest.child!=input.child||latest.token!=input.token||!std::isfinite(latest.totalSeconds)||
       latest.totalSeconds<l.consumedSeconds){
        l.clock.stop();ratio=0;turnFastD86=false;SemiAlchemyReceipt46("INPUT_CHANGED_BEFORE_WRITE_PAUSED_D50",w,0.f);
    }else if(latest.sequence!=input.sequence){
        SemiAlchemyReceipt46("INPUT_SUPERSEDED_CURRENT_D50",w,l.saved*ratio);
    }
    if(turnFastD86){
        // Rejoin the actual FIFO just before the write; an animation-thread
        // advance or changed event boundary must not inherit an earlier budget.
        const float again=SemiResourceTurnRateD86(ctrl,w,dt,effectiveDt,l.clock.pending>0.);
        if(again<ratio){ratio=std::min(ratio,again);turnFastD86=again>0.f;}
    }
    ratio=SemiResourceTailRateD86(w,ratio,effectiveDt,input.resourceReturnHomeD86&&latest.resourceReturnHomeD86);
    if(turnFastD86!=l.turnFastD86){l.turnFastD86=turnFastD86;
        SemiAlchemyReceipt46(turnFastD86?"HENRY_TURN_BEFORE_EVENT_FAST_D86":"GESTURE_WORK_CLOCK_D86",w,l.saved*ratio);}
    const float speed=l.saved*ratio;
    if(l.resourceRoot3D67R1.child&&!SemiResourceRoot3D67R1(g_semi_resource46,ctrl,&l.resourceRoot3D67R1)){
        (void)SemiAlchemyRestore46("RESOURCE_ROOT3_CHANGED_BEFORE_WRITE_D67R1");return;
    }
    if(speed!=l.lastWritten){const float previous=l.lastWritten;
        // Claim BOTH possible values before memory I/O. If write succeeds but
        // readback is unavailable, cleanup must still recognize our new value.
        l.previousWritten=previous;l.lastWritten=speed;l.writeUncertain=true;
        if(!Wr<float>(child,kActSpeedBias,speed)||Rd<float>(child,kActSpeedBias,-1.f)!=speed){(void)SemiAlchemyRestore46("RATE_WRITE_REFUSED");return;}
        l.writeUncertain=false;
        if((previous==0.f)!=(speed==0.f))SemiAlchemyReceipt46(speed==0.f?"HAND_STOPPED":"HAND_ADVANCING",w,speed);
    }
}
struct SemiCallbackScope46 {SemiCallbackScope46(){g_semi_inflight46.fetch_add(1,std::memory_order_acq_rel);}~SemiCallbackScope46(){g_semi_inflight46.fetch_sub(1,std::memory_order_acq_rel);}};
#include "alchemy_resource46.inl"
static int __fastcall SemiBellowsTickDetour46(void* self,float dt) {
    SemiCallbackScope46 inflight;
    float use=dt;
    {OwnerTransaction19c4 transaction;if(transaction){
        semi46::WorkWitness w{};semi46::Control c{};void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
        if(g_owner.state==Idle&&SemiAlchemyParent46(ctrl,w,reinterpret_cast<std::uintptr_t>(self))&&w.kind==semi46::Bellows&&w.ready&&
           SemiAlchemyInput46(w,GetTickCount64(),c)){use=0.f;(void)semi46::work.publish(w);}
    }}
    // Only the exact bellows READY timeout receives zero. Its pickup/work/
    // return, the pot, recipe logic and the entire director retain native dt.
    return g_semi_bellows_original46?g_semi_bellows_original46(self,use):0;
}
static void __fastcall SemiBookEnableDetour46(void* self,bool enabled) {
    SemiCallbackScope46 inflight;semi46::WorkWitness before{};bool qualified=false;std::uint64_t claim=0;
    {OwnerTransaction19c4 transaction;if(transaction){
        qualified=SemiAlchemyParent46(g_henry_ctrl.load(std::memory_order_acquire),before,reinterpret_cast<std::uintptr_t>(self))&&before.kind==semi46::Book;
        if(qualified){claim=semi46::bookGeneration.fetch_add(1,std::memory_order_acq_rel)+1;
            // Revoke BEFORE the native setter changes rows; a previous enabled
            // copied frame cannot leak a pending shoulder press through disable.
            (void)semi46::bookEnable.publish({before.instance,before.parent,before.lifetime,claim,false});}
    }}
    if(g_semi_book_original46)g_semi_book_original46(self,enabled);
    if(!enabled||!qualified||!semi46::hooksReady.load(std::memory_order_acquire)||g_semi_stop46.load(std::memory_order_acquire))return;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    semi46::WorkWitness after{};
    if(SemiAlchemyParent46(g_henry_ctrl.load(std::memory_order_acquire),after,reinterpret_cast<std::uintptr_t>(self))&&
       after.instance==before.instance&&after.lifetime==before.lifetime&&after.kind==semi46::Book&&
       semi46::bookGeneration.load(std::memory_order_acquire)==claim&&
       Rd<unsigned char>(reinterpret_cast<void*>(after.instance),0x300,255)==4){
        const auto generation=semi46::bookGeneration.load(std::memory_order_acquire);
        (void)semi46::bookEnable.publish({after.instance,after.parent,after.lifetime,generation,true});
        SemiAlchemyReceipt46("BOOK_NATIVE_INPUT_ENABLED",after);
    }
}
static void __fastcall MortarPlaceEventDetour70(void* self,void* character,const void* event) {
 SemiCallbackScope46 inflight;semi46::WorkWitness before{};unsigned eventId=0,useId=0,detachId=0;bool qualified=false;
 {OwnerTransaction19c4 tx;if(tx&&MortarPlacePins70()&&kcd2vr::workstation178r20c1::fullVrAlchemy63(GetTickCount64())){
   qualified=SemiAlchemyParent46(g_henry_ctrl.load(),before,reinterpret_cast<std::uintptr_t>(self))&&before.kind==semi46::MortarPlace70&&
    event&&SehCopy(static_cast<const unsigned char*>(event)+0x1C,&eventId,sizeof(eventId))&&
    SehCopy(static_cast<unsigned char*>(self)+0x68,&detachId,sizeof(detachId))&&SehCopy(static_cast<unsigned char*>(self)+0x6C,&useId,sizeof(useId))&&useId&&detachId&&useId!=detachId;
 }}
 // Real Use/Detach and recipe/attachment effects execute exactly once, untouched.
 if(g_mortar_place_original70)g_mortar_place_original70(self,character,event);
 if(!qualified)return;
 {OwnerTransaction19c4 tx;if(tx){semi46::WorkWitness after{};
   if(SemiAlchemyParent46(g_henry_ctrl.load(),after,reinterpret_cast<std::uintptr_t>(self))&&semi46::same(before,after)){
    if(g_mortar_place_events70.parent!=after.parent||g_mortar_place_events70.child!=after.child||g_mortar_place_events70.token!=after.token||g_mortar_place_events70.lifetime!=after.lifetime)
     g_mortar_place_events70={after.parent,after.child,after.lifetime,after.token,0};
    if(eventId==useId)g_mortar_place_events70.stage=std::max(g_mortar_place_events70.stage,1);
    if(eventId==detachId)g_mortar_place_events70.stage=2;
   }
 }}
}
static bool InstallSemiAlchemy46() noexcept {
    for(const auto& h:g_semi_hooks46)if(h)return semi46::hooksReady.load(std::memory_order_acquire);
    if(!SemiAlchemyPins46())return false;
    const auto b=g_base.load(std::memory_order_acquire);
    const auto add=[&](unsigned index,const char* name,std::uintptr_t rva,auto detour,auto& original){
        DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
        DetourModKit::hook::InlineRequest request{name,DetourModKit::hook::Target{DetourModKit::Address{b+rva}},options};
        auto hook=DetourModKit::hook::inline_at(std::move(request),detour);if(!hook)return false;
        g_semi_hooks46[index]=std::move(*hook);original=g_semi_hooks46[index]->template original<std::decay_t<decltype(original)>>();
        return original&&bool(g_semi_hooks46[index]->enable());
    };
    const bool good=add(0,"kcd2vr_semi_bellows_ready46",0x17BAD08,&SemiBellowsTickDetour46,g_semi_bellows_original46)&&
        add(1,"kcd2vr_recipe_book_enable46",0x9F2380,&SemiBookEnableDetour46,g_semi_book_original46)&&
        add(2,"kcd2vr_alchemy_event_interval46",0x842248,&SemiResourceScanDetour46,g_semi_scan_original46)&&
        add(3,"kcd2vr_alchemy_resource_events46",0x6C29D8,&SemiResourceEventDetour46,g_semi_resource_original46);
    const bool mortar70=good&&MortarPlacePins70()&&add(4,"kcd2vr_mortar_place_events70",0x6C4A34,&MortarPlaceEventDetour70,g_mortar_place_original70);
    semi46::mortarReady70.store(mortar70,std::memory_order_release);
    g_semi_stop46.store(!good,std::memory_order_release);semi46::hooksReady.store(good,std::memory_order_release);
    diagnostic_trace::Write(good?"C46 SEMI_ALCHEMY READY work=BELLOWS_MORTAR_CARAFE queue=BEFORE_FIRST_WORK resource=NATIVE_EVENT_INTERVAL book=NATIVE_READ_ENABLE":"C46 SEMI_ALCHEMY INSTALL_REFUSED nativeFallback=1 originals=RETAINED");
    return good;
}
static bool StopSemiAlchemy46() noexcept {
    g_semi_stop46.store(true,std::memory_order_release);semi46::hooksReady.store(false,std::memory_order_release);semi46::mortarReady70.store(false,std::memory_order_release);
    semi46::bookGeneration.fetch_add(1,std::memory_order_acq_rel);(void)semi46::bookEnable.publish({});
    if(g_semi_pending46.load(std::memory_order_acquire)&&(!OnOwnerThread13()||!SemiAlchemyRestore46("STOP_NATIVE_RESUME")))return false;
    return !g_semi_pending46.load(std::memory_order_acquire);
}
static bool DisarmSemiAlchemy46() noexcept {
    bool ok=true;for(auto& h:g_semi_hooks46)if(h&&h->is_enabled()){const auto result=h->disable();ok=bool(result)&&!h->is_enabled()&&ok;}
    // Retain handles AND originals for the process lifetime after disarm. A
    // callback already in flight must be able to finish its native call. This
    // adapter refuses hot reinstallation; physical DLL unload still needs the
    // existing external callback-quiescence gate.
    return ok&&g_semi_inflight46.load(std::memory_order_acquire)==0;
}
