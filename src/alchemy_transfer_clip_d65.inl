// D65: exact transfer-child clip rate, NOT IAction time. Native callbacks,
// inventory/recipe effects and action durations remain owned by KCD2. Current
// single-CAF layer is restored before native controller Update/Queue can change it.
// INVESTIGATE_D67R1[TRANSFER_EVENTS]: immediate pickup/return needs the exact
// native Attach/Detach event interval, not a zero Blend or normalized-time seek.
// Scanner 0x842248 -> dispatch 0x84351C -> resource handler 0x6C29D8 owns attachment/use;
// controller 0x4B3F81..0x4B3FC2 checks one-shot finish before scope/proc update 0x4B404F.
// Before phase-specific compression, prove those callbacks drain once before
// child retirement (including a large-dt frame) and preserve procedural FX.
// This whitelist intentionally excludes UseResource, whose own child clock
// and native kettle Update 0x17AEC54 are separate from a transfer clip rate.
namespace transfer65=kcd2vr::transfer_clip_d65;
static std::atomic<float> g_alchemy_transfer_rate_d65{1.f};
static std::atomic<bool> g_alchemy_transfer_pending_d65{false},g_alchemy_transfer_stop_d65{false};
static std::atomic_flag g_alchemy_transfer_lock_d65=ATOMIC_FLAG_INIT;
static transfer65::Ledger g_alchemy_transfer_d65;
struct AlchemyTransferLockD65 {
    bool held=false;AlchemyTransferLockD65() noexcept:held(!g_alchemy_transfer_lock_d65.test_and_set(std::memory_order_acquire)){}
    ~AlchemyTransferLockD65(){if(held)g_alchemy_transfer_lock_d65.clear(std::memory_order_release);}
    explicit operator bool() const noexcept{return held;}
};
struct AlchemyTransferStageD65 {std::uintptr_t vft,start,slotPin;unsigned char bytes[4];};
constexpr AlchemyTransferStageD65 kAlchemyTransferStagesD65[]{
    {0x3A5C308,0x6C5B48,0x6C5BF4,{0x48,0x8D,0x5F,0x70}}, // TakeIngredient
    {0x3A5C4C0,0x6C59E8,0x6C5A5B,{0x48,0x8D,0x7E,0x70}}, // PlaceIngredient
    {0x3B87A38,0x2E158AC,0x2E15974,{0x48,0x8D,0x5F,0x70}}, // ReturnIngredient
    {0x3A5CCD0,0x2E15B78,0x2E15C40,{0x48,0x8D,0x5F,0x70}}, // TakeObject
    {0x3C23010,0x2E15A24,0x2E15A87,{0x48,0x8D,0x7B,0x70}}, // ReturnObject
    {0x3F60940,0x2E15718,0x2E157E0,{0x48,0x8D,0x5F,0x70}}, // PlaceBowl
    {0x3A5C150,0x6C62A4,0x6C6354,{0x48,0x8D,0x7B,0x70}}, // MovePot
    {0x3A5BB90,0x6C3AC0,0x6C3B16,{0x48,0x8D,0x7B,0x70}}, // PlaceMortar
};
static bool AlchemyTransferStageQualifiedD65(std::uintptr_t base,std::uintptr_t vft) noexcept {
    if(!base||!retail_build112::Ready(base))return false;
    for(const auto& s:kAlchemyTransferStagesD65)if(vft==base+s.vft)
        return Rd<std::uintptr_t>(reinterpret_cast<void*>(vft),0xF8,0)==base+s.start&&
            MatchBytes(base+s.slotPin,s.bytes,sizeof(s.bytes));
    return false;
}
static void AlchemyTransferReceiptD65(const char* phase,const transfer65::Key& key,float rate) noexcept {
    if(!DiagOn())return;
    namespace cap=kcd2vr::diagnostics::capture168;const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(cap::Domain::Interaction,now))return;
    static kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    unsigned code=0;for(const char* p=phase;*p;++p)code=code*33+unsigned(*p);
    if(!gate.observe(cap::gate.session[unsigned(cap::Domain::Interaction)].load(),now,{key.child,key.token,key.layer,int(code)},0,96))return;
    char line[400]{};std::snprintf(line,sizeof(line),"D65 ALCHEMY_TRANSFER_CLIP phase=%s parent=%llX child=%llX token=%u life=%llu layer=%d rate=%.2f eventPath=NATIVE IActionSpeed=UNCHANGED",
        phase,(unsigned long long)key.parent,(unsigned long long)key.child,key.token,(unsigned long long)key.lifetime,key.layer,double(rate));
    diagnostic_trace::Write(line);
}
// The setter changes a shared layer, so a native replacement must not inherit
// our rate. Restore to the CURRENT unique layer-owning scope's native cache,
// never an obsolete child's saved rate. No stale action pointer is dereferenced.
static bool AlchemyTransferNativeRateD65(const transfer65::Key& key,float& native) noexcept {
    auto* ctrl=g_henry_ctrl.load(std::memory_order_acquire);native=-1.f;
    if(!ctrl||reinterpret_cast<std::uintptr_t>(ctrl)!=key.controller||!IsHenryController(ctrl))return false;
    unsigned n=0;unsigned char* scopes=nullptr;
    if(!SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlScopeCount,&n,sizeof(n))||
       !SehCopy(static_cast<unsigned char*>(ctrl)+kCtrlScopes,&scopes,sizeof(scopes))||!n||n>kMaxScopes||!scopes)return false;
    unsigned owners=0;
    for(unsigned i=0;i<n;++i){auto* scope=scopes+i*kScopeStride;int layer=-1;unsigned layers=0;
        if(!SehCopy(scope+kScopeLayer,&layer,sizeof(layer))||!SehCopy(scope+kScopeNumLayers,&layers,sizeof(layers)))return false;
        if(!layers)continue; // an unallocated scope owns no clip layer
        if(layer<0||layer>15||layers>16)return false;
        if(key.layer<layer||unsigned(key.layer-layer)>=layers)continue;
        float value=-1;if(!SehCopy(scope+kScopeSpeedBias,&value,sizeof(value))||!std::isfinite(value)||value<0.f||value>8.f)return false;
        if(++owners>1)return false;native=value;
    }
    return owners==1;
}
// Separate clip lock: native Update can migrate between workers. No general
// action transaction or obsolete cached thread is required to clean our layer.
static bool RestoreAlchemyTransferClipD65(void* currentCallback=nullptr) noexcept {
    if(!g_alchemy_transfer_pending_d65.load(std::memory_order_acquire))return true;
    const bool callback=currentCallback&&currentCallback==g_henry_ctrl.load(std::memory_order_acquire)&&
        IsHenryController(currentCallback)&&retail_build112::Ready(WhBase());
    if(!callback&&!OnOwnerThread13())return false;
    AlchemyTransferLockD65 lock;if(!lock)return false;
    auto& l=g_alchemy_transfer_d65;if(!l.active){g_alchemy_transfer_pending_d65.store(false,std::memory_order_release);return true;}
    if(currentCallback&&reinterpret_cast<std::uintptr_t>(currentCallback)!=l.key.controller)return false;
    void* skel=nullptr;std::uintptr_t character=0;if(!PickupLayerSourceH23(skel,character))return false;
    const bool same=character==l.key.character&&reinterpret_cast<std::uintptr_t>(skel)==l.key.skeleton;
    const float current=same?Rd<float>(skel,0x50+0x40*l.key.layer,-1.f):-1.f;
    const auto decision=l.restore(true,same,current);const auto key=l.key;
    if(decision==transfer65::Restore::Pending)return false;
    float native=l.saved;
    if(decision==transfer65::Restore::GiveBack){
        if(!AlchemyTransferNativeRateD65(key,native))return false;
        if(native!=l.written&&!SetPickupLayerScaleH23(skel,key.layer,native))return false;
    }
    const bool foreign=decision==transfer65::Restore::Foreign||(decision==transfer65::Restore::GiveBack&&native!=l.saved);
    AlchemyTransferReceiptD65(foreign?"FOREIGN_STAND_DOWN":decision==transfer65::Restore::Gone?"SKELETON_REPLACED":"RESTORED",key,native);
    l.settled(foreign);g_alchemy_transfer_pending_d65.store(false,std::memory_order_release);return true;
}
// Exact currently-started director stage, not an arbitrary same-instance root.
static bool ReadAlchemyTransferD65(void* ctrl,transfer65::Key& key) noexcept {
    namespace act=kcd2vr::activity_input163;key={};const auto b=g_base.load(std::memory_order_acquire);const auto& a=act::witness175;
    if(!OnAlchemyUpdateOwnerR27()||!ctrl||ctrl!=g_henry_ctrl.load(std::memory_order_acquire)||!IsHenryController(ctrl)||
       !b||!retail_build112::Ready(b)||!a.ready.load(std::memory_order_acquire)||a.transitionDepth175.load(std::memory_order_acquire))return false;
    const auto life=a.lifetime175.load(std::memory_order_acquire),instance=a.active[0].load(std::memory_order_acquire);
    const auto read=[](std::uintptr_t p,auto& out){return SehCopy(reinterpret_cast<void*>(p),&out,sizeof(out));};
    std::uintptr_t vft=0,player=0,first=0,last=0,parent=0,back=0,child=0;unsigned id=0;unsigned char started=0;int slot=-1;
    if(!life||!instance||!read(instance,vft)||vft!=b+0x3F60B08||!read(instance+0x2C,started)||started!=1||
       !read(instance+0x18,player)||!player||!read(player,vft)||vft!=b+0x3AA4B98||!read(player+0x30,id)||!id||id!=Rd<unsigned>(ctrl,kCtrlEntityId,0)||
       !read(instance+0x78,vft)||vft!=b+0x3BFA5B0||!read(instance+0xB0,first)||!read(instance+0xB8,last)||
       !first||last<first+16||last-first>256||(last-first)%8||!read(first+8,parent)||!parent||!read(parent,vft)||!AlchemyTransferStageQualifiedD65(b,vft)||
       !read(parent+0x58,back)||back!=instance||!read(parent+0x40,back)||back!=instance+0x78||!read(parent+0x3C,slot)||slot!=1||
       !read(parent+0x28,started)||started!=1||!read(parent+0x70,child)||!child||!read(child,vft)||vft!=b+0x3A47948||
       !read(child+0x268,back)||back!=instance)return false;
    auto* action=reinterpret_cast<void*>(child);void* scope=Rd<void*>(action,kActRootScope,nullptr);
    if(!scope||Rd<int>(action,kActStatus,-1)!=2||Rd<void*>(action,kActContext,nullptr)!=Rd<void*>(ctrl,kCtrlContext,nullptr))return false;
    const auto n=Rd<unsigned>(ctrl,kCtrlScopeCount,0);auto* scopes=Rd<unsigned char*>(ctrl,kCtrlScopes,nullptr);
    if(!scopes||!n||n>kMaxScopes)return false;
    bool member=false;for(unsigned i=0;i<n;++i)if(scope==scopes+i*kScopeStride&&Rd<void*>(scope,kScopeAction,nullptr)==action)member=true;
    if(!member)return false;
    key.controller=reinterpret_cast<std::uintptr_t>(ctrl);key.instance=instance;key.lifetime=life;key.parent=parent;key.child=child;
    key.scope=reinterpret_cast<std::uintptr_t>(scope);key.token=Rd<std::uint32_t>(action,kActToken,~0u);if(key.token==~0u)return false;
    key.layer=Rd<int>(scope,kScopeLayer,-1);key.mode=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    void* skel=nullptr;if(!PickupLayerSourceH23(skel,key.character))return false;key.skeleton=reinterpret_cast<std::uintptr_t>(skel);
    return read(first+8,back)&&back==parent&&read(parent+0x70,back)&&back==child&&
        life==a.lifetime175.load(std::memory_order_acquire)&&instance==a.active[0].load(std::memory_order_acquire)&&
        a.ready.load(std::memory_order_acquire)&&!a.transitionDepth175.load(std::memory_order_acquire);
}
static bool AlchemyTransferInputD65(const transfer65::Key& key,std::uint64_t now) noexcept {
    namespace ab=kcd2vr::subsystems::alchemy::bridge67;
    if(!kcd2vr::workstation178r20c1::fullVrAlchemy63(now)||
       !kcd2vr::minigame_hands45::freeTrackedMode(kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy))||
       !kcd2vr::workstation178r20c1::ownsActivityWrist45(now))return false;
    ab::Control c{};kcd2vr::workstation_sample46::Frame f{};
    if(!ab::control.read(c))return false;
    const int gestureSideR93=c.plateBindingR93?c.plateGestureSideR93:c.side;
    if(gestureSideR93<0||gestureSideR93>1||!c.enabled||(c.passenger&&!(c.gestureTransferD67&&c.trackedPresentation71&&c.parent==key.parent))||c.instance!=key.instance||c.lifetime!=key.lifetime||
       (c.parent&&c.parent!=key.parent)||!semi46::fresh(c.tick,now)||c.modeEpoch!=key.mode||
       c.modeEpoch!=kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||c.side<0||c.side>1||
       !frik_pose_probe::WorkstationFrame46(f)||!f.allowed||!kcd2vr::workstation_sample46::fresh(f,now)||
       !f.headValid||!f.hand[gestureSideR93].valid||!kcd2vr::workstation_sample46::finite(f.hand[gestureSideR93])||
       !c.poseEpoch||!c.sourceEpoch||c.poseEpoch!=f.poseEpoch||c.sourceEpoch!=f.sourceEpoch)return false;
    if(c.plateBindingR93){ab::NativeTable table{};ab::NativeTool held{},other{};
        if(!ab::table.read(table)||!ab::tools[0].read(held)||!ab::tools[1].read(other)||
           !ab::plateControlCurrentR93(c,table,held,other,now,f.poseEpoch))return false;}
    return true;
}
static void TickAlchemyTransferClipD65(void* ctrl,std::uint64_t now) noexcept {
    const float rate=transfer65::rate(g_alchemy_transfer_rate_d65.load(std::memory_order_acquire));
    if(rate<=1.f||g_alchemy_transfer_stop_d65.load(std::memory_order_acquire)||g_alchemy_transfer_pending_d65.load(std::memory_order_acquire)||
       !OnAlchemyUpdateOwnerR27()||g_owner.state!=Idle)return;
    AlchemyTransferLockD65 lock;if(!lock)return;
    transfer65::Key key{};if(!ReadAlchemyTransferD65(ctrl,key)||!AlchemyTransferInputD65(key,now)||key.layer<0||key.layer>15)return;
    auto* action=reinterpret_cast<void*>(key.child);auto* scope=reinterpret_cast<void*>(key.scope);auto* skel=reinterpret_cast<void*>(key.skeleton);
    transfer65::Admission admission{true,true,true,true,
        Rd<unsigned>(scope,kScopeNumLayers,0)>=1&&Rd<unsigned>(scope,kScopeNumLayers,0)<=unsigned(16-key.layer)&&NumAnims(skel,key.layer)==1,
        (g_semi_alchemy46.written&&g_semi_alchemy46.owner.child==key.child)||
        (g_alchemy_stage19c4.written&&g_alchemy_stage19c4.proof.child==key.child)||g_pickup_clock_live_h23.load(std::memory_order_acquire)};
    const float time=AnimTime(skel,key.layer,0),layer=Rd<float>(skel,0x50+0x40*key.layer,-1.f);
    // A root scope may own several layers. Only its first, single-CAF layer is
    // written, and its current unique scope owner must independently agree.
    // Additional procedural/additive layers and IAction time remain native.
    float nativeRateD67=-1.f;
    if(!AlchemyTransferNativeRateD65(key,nativeRateD67)||nativeRateD67!=1.f)return;
    if(!transfer65::eligible(admission,rate)||Rd<float>(action,kActSpeedBias,-1.f)!=1.f||Rd<float>(scope,kScopeSpeedBias,-1.f)!=1.f||
       !std::isfinite(time)||time<0.f||time>=1.f)return;
    transfer65::Key again{};if(!ReadAlchemyTransferD65(ctrl,again)||!transfer65::same(key,again)||
       !AlchemyTransferInputD65(key,std::uint64_t(GetTickCount64()))||
       transfer65::rate(g_alchemy_transfer_rate_d65.load(std::memory_order_acquire))!=rate||g_alchemy_transfer_stop_d65.load())return;
    auto& l=g_alchemy_transfer_d65;if(!l.begin(key,layer,rate))return;
    g_alchemy_transfer_pending_d65.store(true,std::memory_order_release); // before setter: unknown result still needs cleanup
    if(SetPickupLayerScaleH23(skel,key.layer,rate)){l.confirmed();AlchemyTransferReceiptD65("LEASED",key,rate);}
    else AlchemyTransferReceiptD65("WRITE_UNCERTAIN_RESTORE_PENDING",key,rate);
}
static bool StopAlchemyTransferClipD65() noexcept {
    g_alchemy_transfer_stop_d65.store(true,std::memory_order_release);
    return RestoreAlchemyTransferClipD65();
}
