// Runs only around the existing Henry Update callback. Native code11 keeps
// its charge and hit evaluator; its presentation delay is shortened after an
// observed VR lift/reversal. Heat/world dt and native code10 are untouched.
namespace sf81=kcd2vr::subsystems::smithing::fall_d81;
static smithNative46::Lease g_smith_fall_d81{};
static sf81::Receipt g_smith_fall_abandoned_d81{};
static std::atomic<bool> g_smith_fall_pending_d81{false};

static bool RestoreSmithFallD81() noexcept {
    auto& l=g_smith_fall_d81;if(!l.written)return true;
    if(!OnAlchemyUpdateOwnerR27()||l.controller!=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load()))return false;
    const auto before=l;
    const auto rd=[](std::uintptr_t p,auto& v){return SemiSmithRead46(p,v);};
    const auto result=smithNative46::restore(rd,SemiSmithWrite46,g_base.load(),l);
    if(result==smithNative46::Restore::Pending)return false;
    if(result==smithNative46::Restore::Foreign||result==smithNative46::Restore::Ended){
        g_smith_fall_abandoned_d81.instance=before.owner.instance;
        g_smith_fall_abandoned_d81.lifetime=before.owner.lifetime;
        g_smith_fall_abandoned_d81.request=before.owner.request;
    }
    g_smith_fall_pending_d81.store(false,std::memory_order_release);return true;
}
static bool RestoreSmithFallBoundaryD81(void* ctrl) noexcept {
    if(!g_smith_fall_pending_d81.load(std::memory_order_acquire))return true;
    if(ctrl!=g_henry_ctrl.load(std::memory_order_acquire))return false;
    OwnerTransaction19c4 transaction;if(!transaction)return false;
    AlchemyUpdateScopeR27 scope(ctrl);return RestoreSmithFallD81();
}
static bool SmithFallInputD81(sf81::Receipt& r,std::uint64_t now) noexcept {
    smith46::ClockControl c{};
    if(g_semi_smith_stop46.load()||!smith46::clockReady.load()||!smith46::clockControl.read(c)||
       !sf81::fresh(c.fallD81,now)||c.enabled||c.request||
       kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Smithing)==kcd2vr::minigame_hands45::Native||
       !kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||!kcd2vr::workstation178r20c1::gesturesOn(now)||
       !kcd2vr::workstation178r20c1::smithAutoHold36.load())return false;
    r=c.fallD81;
    if(r.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load()||
       (g_smith_fall_abandoned_d81.instance==r.instance&&g_smith_fall_abandoned_d81.lifetime==r.lifetime&&
        g_smith_fall_abandoned_d81.request==r.request))return false;
    kcd2vr::workstation178r20c1::WorkstationOwnerH51 owner{};
    kcd2vr::workstation_sample46::Frame frame{};
    return kcd2vr::workstation178r20c1::workstationOwnerH51(now,owner)&&
        owner.kind==kcd2vr::minigame_hands45::Smithing&&owner.instance==r.instance&&owner.lifetime==r.lifetime&&
        frik_pose_probe::WorkstationFrame46(frame)&&frame.allowed&&kcd2vr::workstation_sample46::fresh(frame,now)&&
        frame.hand[0].valid&&kcd2vr::workstation_sample46::finite(frame.hand[0])&&frame.poseEpoch==r.poseEpoch&&
        frame.sourceEpoch==r.sourceEpoch&&frame.sequence>=r.sequence;
}
static bool SmithFallParentD81(void* ctrl,const sf81::Receipt& r,smithNative46::Parent& p) noexcept {
    namespace act=kcd2vr::activity_input163;const auto& w=act::witness175;
    if(!OnAlchemyUpdateOwnerR27()||!w.ready.load()||w.transitionDepth175.load()||
       !act::trackedHands59c7f[unsigned(act::Kind175::Smithing)].load()||
       w.active[unsigned(act::Kind175::Smithing)].load()!=r.instance||
       w.active[unsigned(act::Kind175::Alchemy)].load()||w.lifetime175.load()!=r.lifetime)return false;
    const auto rd=[](std::uintptr_t at,auto& v){return SemiSmithRead46(at,v);};
    if(!smithNative46::parent(rd,g_base.load(),r.instance,0,0,Rd<unsigned>(ctrl,kCtrlEntityId,0),false,p)||
       p.phase!=16||p.code!=11)return false;
    if(!w.ready.load()||w.transitionDepth175.load()||w.active[unsigned(act::Kind175::Smithing)].load()!=r.instance||
       w.lifetime175.load()!=r.lifetime)return false;
    p.lifetime=r.lifetime;p.request=r.request;return true;
}
static void TickSmithFallD81(void* ctrl,float dt,std::uint64_t now) noexcept {
    if(!smith46::clockInterested.load(std::memory_order_acquire)||!OnAlchemyUpdateOwnerR27()||g_owner.state!=Idle||g_smith_fall_pending_d81.load()||
       g_semi_smith46.written||!SemiSmithPins46())return;
    const float speed=sf81::rate(dt,Rd<float>(ctrl,kCtrlTimeScale,-1.f));if(speed<=1.f)return;
    sf81::Receipt r{};smithNative46::Parent p{};
    if(!SmithFallInputD81(r,now)||!SmithFallParentD81(ctrl,r,p))return;
    const auto rd=[](std::uintptr_t at,auto& v){return SemiSmithRead46(at,v);};
    const auto controller=reinterpret_cast<std::uintptr_t>(ctrl),base=g_base.load();
    const auto member=smithNative46::membership(rd,base,controller,p.child);smithNative46::Child child{};
    if(member.kind!=smithNative46::Membership::Root||!smithNative46::child(rd,base,controller,p.child,member.kind,child)||child.speed!=1.f)return;
    sf81::Receipt again{};smithNative46::Parent finalParent{};
    if(!SmithFallInputD81(again,GetTickCount64())||!sf81::same(r,again)||again.request!=r.request||
       !SmithFallParentD81(ctrl,again,finalParent)||finalParent.parent!=p.parent||finalParent.child!=p.child)return;
    auto& l=g_smith_fall_d81;l={};l.owner=p;l.controller=controller;l.token=child.token;
    l.poseEpoch=r.poseEpoch;l.sourceEpoch=r.sourceEpoch;l.modeEpoch67=r.modeEpoch;
    l.saved=l.lastWritten=1.f;l.candidate=speed;l.written=l.uncertainWrite=true;
    g_smith_fall_pending_d81.store(true,std::memory_order_release);
    if(SemiSmithWrite46(p.child+0x5C,speed)&&Rd<float>(reinterpret_cast<void*>(p.child),0x5C,-1.f)==speed){
        l.lastWritten=speed;l.uncertainWrite=false;
    }else (void)RestoreSmithFallD81();
}
