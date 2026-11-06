// Exact resource-carafe transaction, invoked only inside Henry's existing
// CActionController::Update. No animation event is swallowed or synthesized.
struct AlchemyStageLease19c4 {
    kcd2vr::alchemy_stage19c4::Proof proof{};void* controller=nullptr;void* scope=nullptr;
    float saved=1.f;bool written=false,pendingNoted=false;
} g_alchemy_stage19c4;
kcd2vr::alchemy_stage19c4::Proof g_alchemy_retired19c4{};
std::atomic<bool> g_alchemy_pending19c4{false},g_alchemy_stop19c4{false};
#ifdef KCD2VR_GRASP_HOST_TEST
void HostAlchemyClaim19c4() noexcept;
#endif
static void AlchemyStageReceipt19c4(unsigned reason,const char* phase,const kcd2vr::alchemy_stage19c4::Proof& p,float speed=-1.f) noexcept {
    if(!DiagOn())return;
    namespace cap=kcd2vr::diagnostics::capture168;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    const auto now=std::uint64_t(GetTickCount64());const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load();
    if(!gate.observe(session,now,{p.child,p.binding.entity,1,int(reason)},0,64))return;
    // Only this exact native alchemy lifetime may use the latest verdict.
    char cb1Completion[96]="UNOBSERVED";
    {kcd2vr::alchemy_finish_cb1::Result r{};if(kcd2vr::alchemy_finish_cb1::Last(r)&&r.valid&&
        r.context.instance==p.binding.instance&&r.context.player==p.binding.player&&r.context.lifetime==p.binding.lifetime)
        std::snprintf(cb1Completion,sizeof(cb1Completion),"RECIPE_RESULT(serial=%llu,%s,recipe=%d,age=%llums,delivery=UNOBSERVED)",(unsigned long long)r.serial,r.success?"SUCCESS":"FAILED",r.recipeId,(unsigned long long)(now>=r.tick?now-r.tick:0));}
    char line[448]{};std::snprintf(line,sizeof(line),
        "C19C4 F5_ALCHEMY_STAGE phase=%s instance=%llX lifetime=%llu parent=%llX child=%llX token=%u target=%u type=%d request=%llu sample=%llu speedReadback=%.3f heldStage=NATIVE_ATTACHMENT release=NATIVE_CONTINUATION nativeCompletion=%s recipeEvents=UNCHANGED",
        phase,(unsigned long long)p.binding.instance,(unsigned long long)p.binding.lifetime,(unsigned long long)p.binding.action,(unsigned long long)p.child,p.token,p.binding.entity,p.binding.type,
        (unsigned long long)p.request,(unsigned long long)p.sample,double(speed),cb1Completion);diagnostic_trace::Write(line);
}
static bool AlchemyRestorePending19c4() noexcept {
    auto& lease=g_alchemy_stage19c4;if(!lease.pendingNoted){AlchemyStageReceipt19c4(2,"RESTORE_PENDING",lease.proof);lease.pendingNoted=true;}return false;
}
static bool SameAlchemyChild19c4(const kcd2vr::alchemy_stage19c4::Proof& a,const kcd2vr::alchemy_stage19c4::Proof& b) noexcept {
    return a.child&&a.child==b.child&&a.token==b.token&&a.epoch==b.epoch&&a.request==b.request&&
        kcd2vr::alchemy_tool18::same(a.binding,b.binding);
}
static bool RetiredAlchemyChild19c4(const kcd2vr::alchemy_stage19c4::Proof& p) noexcept {
    const auto& r=g_alchemy_retired19c4;
    return r.child&&r.child==p.child&&r.token==p.token&&r.binding.action==p.binding.action&&
        r.binding.instance==p.binding.instance&&r.binding.lifetime==p.binding.lifetime;
}
static bool AlchemyStagePins19c4() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b))return false;
    static std::uintptr_t checked=0;static bool good=false;if(checked==b)return good;
    // Native child constructor: actual IAction base, owner backlink, exact vft.
    constexpr unsigned char child[]{0xE8,0x70,0xC0,0xFB,0xFF,0x48,0x8D,0x05,0x09,0x0C,0x38,0x03,0x48,0x89,0x9F,0x68,0x02,0x00,0x00};
    constexpr unsigned char speed[]{0xB8,0x00,0x00,0x80,0x3F,0x89,0x43,0x5C,0x89,0x43,0x60};
    constexpr unsigned char storage[]{0x48,0x8D,0x7B,0x78};
    constexpr unsigned char scope[]{0xF3,0x0F,0x10,0x7D,0x5C,0xF3,0x0F,0x10,0x43,0x58,0x0F,0x2E,0xC7};
    constexpr unsigned char dt[]{0x0F,0x28,0xCF,0xF3,0x0F,0x59,0x4B,0x5C,0x48,0x8B,0xCB,0xFF,0x50,0x30};
    good=MatchBytes(b+0x6C6D33,child,sizeof(child))&&MatchBytes(b+0x682DD3,speed,sizeof(speed))&&
        MatchBytes(b+0x7012EF,storage,sizeof(storage))&&MatchBytes(b+0x4B3A76,scope,sizeof(scope))&&MatchBytes(b+0x4B3F12,dt,sizeof(dt));
    checked=b;return good;
}
static bool AlchemyChildLive19c4(void* ctrl,const kcd2vr::alchemy_stage19c4::Proof& p,void*& scope) noexcept {
    const auto b=g_base.load(std::memory_order_acquire);auto* child=reinterpret_cast<void*>(p.child);
    scope=FindRootScope(ctrl,child);
    return ctrl&&ctrl==g_henry_ctrl.load(std::memory_order_acquire)&&OnAlchemyUpdateOwnerR27()&&IsHenryController(ctrl)&&
        scope&&Rd<std::uintptr_t>(child,0,0)==b+0x3A47948&&Rd<int>(child,kActStatus,-1)==2&&
        Rd<std::uint32_t>(child,kActToken,~p.token)==p.token&&Rd<void*>(child,kActContext,nullptr)==Rd<void*>(ctrl,kCtrlContext,nullptr)&&
        Rd<std::uintptr_t>(child,0x268,0)==p.binding.instance;
}
static bool RestoreAlchemyStage19c4(const char* reason="RELEASE_RESUME") noexcept {
    auto& lease=g_alchemy_stage19c4;if(!lease.written)return true;
    if(!OnAlchemyUpdateOwnerR27())return false; // exact serialized alchemy callback only
    // Unknown native memory is not proof that the paused child ended. Retain
    // cleanup until a readable current scope proves termination/replacement.
    unsigned count=0;unsigned char* scopes=nullptr;void* scope=nullptr;
    if(lease.controller!=g_henry_ctrl.load(std::memory_order_acquire)||!IsHenryController(lease.controller)||
       !SehCopy(static_cast<unsigned char*>(lease.controller)+kCtrlScopeCount,&count,sizeof(count))||count>kMaxScopes||
       !SehCopy(static_cast<unsigned char*>(lease.controller)+kCtrlScopes,&scopes,sizeof(scopes))||(count&&!scopes))return AlchemyRestorePending19c4();
    for(unsigned i=0;i<count;++i){void* action=nullptr;void* candidate=scopes+i*kScopeStride;
        if(!SehCopy(static_cast<unsigned char*>(candidate)+kScopeAction,&action,sizeof(action)))return AlchemyRestorePending19c4();
        // One IAction may occupy several scopes. Only the saved ROOT slot
        // can prove the lease; a later non-root scope must not overwrite it.
        if(candidate==lease.scope&&action==reinterpret_cast<void*>(lease.proof.child))scope=candidate;
    }
    bool same=false;float readback=-1.f;
    if(scope==lease.scope){
        auto* child=reinterpret_cast<void*>(lease.proof.child);std::uintptr_t vft=0,owner=0;void* root=nullptr;std::uint32_t token=0;int status=0;
        if(!SehCopy(child,&vft,sizeof(vft))||!SehCopy(static_cast<unsigned char*>(child)+kActRootScope,&root,sizeof(root))||
           !SehCopy(static_cast<unsigned char*>(child)+kActStatus,&status,sizeof(status))||
           !SehCopy(static_cast<unsigned char*>(child)+kActToken,&token,sizeof(token))||
           !SehCopy(static_cast<unsigned char*>(child)+0x268,&owner,sizeof(owner)))return AlchemyRestorePending19c4();
        same=vft==g_base.load()+0x3A47948&&root==scope&&status==2&&token==lease.proof.token&&owner==lease.proof.binding.instance;
    }
    if(same){
        auto* child=reinterpret_cast<void*>(lease.proof.child);float speed=0;
        if(!SehCopy(static_cast<unsigned char*>(child)+kActSpeedBias,&speed,sizeof(speed))||!std::isfinite(speed))return AlchemyRestorePending19c4();
        if(speed==0.f){if(!Wr<float>(child,kActSpeedBias,lease.saved)||Rd<float>(child,kActSpeedBias,-1.f)!=lease.saved){
            return AlchemyRestorePending19c4();}}
        readback=Rd<float>(child,kActSpeedBias,-1.f);
        // A native/foreign speed writer wins; do not replace its new value.
    }
    AlchemyStageReceipt19c4(1,same?reason:"OWNER_ENDED_NO_STALE_WRITE",lease.proof,readback);
    g_alchemy_retired19c4=lease.proof;lease={};g_alchemy_pending19c4.store(false,std::memory_order_release);return true;
}
static bool AlchemyStageCurrent19c4(void* ctrl,const kcd2vr::alchemy_stage19c4::Proof& p,
                                  const kcd2vr::alchemy_stage19c4::Control& input,std::uint64_t now,void*& scope) noexcept {
    namespace act=kcd2vr::activity_input163;
    const auto& binding=p.binding;const auto b=g_base.load(std::memory_order_acquire);
    if(g_alchemy_stop19c4.load(std::memory_order_acquire)||g_mode.load()!=Own||g_owner.state!=Idle||
       !input.enabled||!input.held[1]||!input.tick[1]||now<input.tick[1]||now-input.tick[1]>80||
       !p.tick||now<p.tick||now-p.tick>80||!p.sample||!p.epoch||
       input.instance!=binding.instance||input.lifetime!=binding.lifetime||input.request!=p.request||
       kcd2vr::combat_rules178r10::f5Rung.load()!=2||!kcd2vr::workstation178r20c1::experimentArmed36.load()||
       p.request!=kcd2vr::workstation178r20c1::experimentEpoch36.load()||
       !act::witness175.ready.load()||act::witness175.transitionDepth175.load()||
       binding.lifetime!=act::witness175.lifetime175.load()||binding.instance!=act::witness175.active[unsigned(act::Kind175::Alchemy)].load()||
       binding.side!=1||binding.type<13||binding.type>16||binding.actionClass!=b+0x3A5C670||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(binding.action),0x78,0)!=p.child||!AlchemyStagePins19c4()||
       !AlchemyChildLive19c4(ctrl,p,scope))return false;
    const auto rd=[](std::uintptr_t at,auto& value){return SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    kcd2vr::alchemy_tool18::Binding current{};
    if(!kcd2vr::alchemy_tool18::read(rd,b,binding.instance,binding.player,binding.character,binding.attachment,binding.entity,current))return false;
    current.lifetime=act::witness175.lifetime175.load();
    return kcd2vr::alchemy_tool18::same(binding,current)&&Rd<std::uintptr_t>(reinterpret_cast<void*>(binding.action),0x78,0)==p.child&&
        !act::witness175.transitionDepth175.load()&&act::witness175.ready.load()&&
        binding.lifetime==act::witness175.lifetime175.load()&&binding.instance==act::witness175.active[unsigned(act::Kind175::Alchemy)].load()&&
        kcd2vr::combat_rules178r10::f5Rung.load()==2&&kcd2vr::workstation178r20c1::experimentArmed36.load()&&
        p.request==kcd2vr::workstation178r20c1::experimentEpoch36.load();
}
static void TickAlchemyStage19c4(void* ctrl,std::uint64_t now) noexcept {
    namespace stage=kcd2vr::alchemy_stage19c4;
    if(!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||!OnAlchemyUpdateOwnerR27()||!IsHenryController(ctrl))return;
    stage::Control input{};stage::Proof proof{};void* scope=nullptr;
    const bool current=stage::control.read(input)&&stage::attached.read(proof)&&AlchemyStageCurrent19c4(ctrl,proof,input,now,scope);
    auto& lease=g_alchemy_stage19c4;
    if(lease.written&&(!current||!SameAlchemyChild19c4(lease.proof,proof))&&
       !RestoreAlchemyStage19c4(input.enabled&&input.held[1]?"OWNER_LOST_RESUME":"RELEASE_RESUME"))return;
    if(!current||RetiredAlchemyChild19c4(proof))return;
    if(lease.written){
        // A changed native speed means this module no longer owns the pause.
        if(Rd<float>(reinterpret_cast<void*>(proof.child),kActSpeedBias,-1.f)!=0.f)(void)RestoreAlchemyStage19c4("SPEED_WRITER_CHANGED");
        return;
    }
    auto* child=reinterpret_cast<void*>(proof.child);const float speed=Rd<float>(child,kActSpeedBias,-1.f);
    if(!std::isfinite(speed)||speed<=0.f||speed>8.f)return;
    stage::Control finalInput{};stage::Proof finalProof{};void* finalScope=nullptr;
    if(!stage::control.read(finalInput)||!stage::attached.read(finalProof)||!SameAlchemyChild19c4(proof,finalProof)||
       !AlchemyStageCurrent19c4(ctrl,finalProof,finalInput,now,finalScope)||scope!=finalScope)return;
    lease.proof=finalProof;lease.controller=ctrl;lease.scope=scope;lease.saved=speed;
    // Publish cleanup BEFORE the final stop check/write. A worker stopping
    // before this claim is seen by the recheck; one stopping after it must keep
    // the native update hook until this lease has been restored.
    lease.written=true;g_alchemy_pending19c4.store(true,std::memory_order_release);
#ifdef KCD2VR_GRASP_HOST_TEST
    HostAlchemyClaim19c4(); // deterministic shutdown interleaving only
#endif
    if(g_alchemy_stop19c4.load(std::memory_order_acquire)){
        lease={};g_alchemy_pending19c4.store(false,std::memory_order_release);return; // no native write occurred
    }
    if(Wr<float>(child,kActSpeedBias,0.f))AlchemyStageReceipt19c4(0,"CARAFE_PAUSED",proof,Rd<float>(child,kActSpeedBias,-1.f));
    else (void)RestoreAlchemyStage19c4("PAUSE_WRITE_REFUSED");
}
static bool StopAlchemyStage19c4() noexcept {
    g_alchemy_stop19c4.store(true,std::memory_order_release);
    if(g_alchemy_pending19c4.load(std::memory_order_acquire)&&(!OnOwnerThread13()||!RestoreAlchemyStage19c4()))return false;
    return !g_alchemy_pending19c4.load(std::memory_order_acquire);
}
