// Included ONCE in action_time_owner59c11's private native-owner scope, after
// SehCopy/Wr/IsHenryController/OnOwnerThread13 and existing common definitions.
// Integration is supplied separately: this file creates no second Update hook.
namespace xr34=kcd2vr::crossbow_native_xr34;
static xr34::Client g_crossbow_xr34;
static std::uintptr_t g_crossbow_base_xr34=0;
// D72: a pinned native callback is the authority for THIS Henry reload even
// when the engine schedules the same controller on a different job thread.
// The enclosing OwnerTransaction19c4 serializes mutation; this witness never
// changes the general pickup/drop thread owner and expires before returning
// to an original native call. XR34Current still proves every native backlink,
// live item, queued/root membership and child token below.
struct XR34BoundaryWitnessD72 {std::uintptr_t controller=0,parent=0;};
static thread_local XR34BoundaryWitnessD72 g_xr34_boundary_d72;
struct XR34BoundaryScopeD72 {
    XR34BoundaryWitnessD72 previous{};
    XR34BoundaryScopeD72(void* controller,void* parent=nullptr) noexcept : previous(g_xr34_boundary_d72) {
        g_xr34_boundary_d72={};
        if(controller&&controller==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(controller))
            g_xr34_boundary_d72={reinterpret_cast<std::uintptr_t>(controller),reinterpret_cast<std::uintptr_t>(parent)};
    }
    ~XR34BoundaryScopeD72(){g_xr34_boundary_d72=previous;}
};
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
#endif
template<class Work> static auto XR34WithBoundaryD73(void* controller,void* parent,Work&& work) noexcept -> decltype(work()) {
    const auto previous=g_xr34_boundary_d72;
    g_xr34_boundary_d72={};
    if(controller&&controller==g_henry_ctrl.load(std::memory_order_acquire)&&IsHenryController(controller))
        g_xr34_boundary_d72={reinterpret_cast<std::uintptr_t>(controller),reinterpret_cast<std::uintptr_t>(parent)};
#if defined(_MSC_VER)
    __try {return work();}
    __finally {g_xr34_boundary_d72=previous;}
#else
    struct Restore {XR34BoundaryWitnessD72 old;~Restore(){g_xr34_boundary_d72=old;}} restore{previous};
    return work();
#endif
}
#if defined(_MSC_VER)
#pragma pop_macro("__try")
#endif
static bool XR34BoundaryCurrentD72(const xr34::Owner& o) noexcept {
    const auto& w=g_xr34_boundary_d72;
    return w.controller&&w.controller==o.controller&&(!w.parent||w.parent==o.parent);
}
template<class T> static bool XR34Read(std::uintptr_t p,T& value) noexcept {
    return p&&SehCopy(reinterpret_cast<const void*>(p),&value,sizeof(value));
}
static bool XR34ReadBytes(std::uintptr_t p,void* dst,std::size_t n) noexcept {
    return SehCopy(reinterpret_cast<const void*>(p),dst,n);
}
static bool XR34WriteBytes(std::uintptr_t p,const void* src,std::size_t n) noexcept {
    // The client has exactly one allowed destination: its qualified child+5C.
    const auto& state=g_crossbow_xr34.status();
    if(!p||p!=state.owner.child+0x5C||n!=sizeof(float)||!src)return false;
    float speed=0;std::memcpy(&speed,src,sizeof(speed));
    return std::isfinite(speed)&&speed>=0&&speed<=(xr34::handDrivenD81(state.owner.source)?xr34::kClockTransportMaxD72:8.f)&&Wr(reinterpret_cast<void*>(state.owner.child),0x5C,speed);
}
// Copied static diagnostic only. Never used to admit/retry an action.
static thread_local const char* g_xr34_current_leaf_d69=nullptr;
static bool XR34Current(const xr34::Owner& o) noexcept {
    g_xr34_current_leaf_d69="CURRENT_NOT_COMPLETE";
    const auto leaf=[](const char* why) noexcept {g_xr34_current_leaf_d69=why;return false;};
    const auto b=g_base.load(std::memory_order_acquire);const auto& s=o.source;
    const auto identity=xr34::identityReader.load(std::memory_order_acquire);
    if(!b)return leaf("CURRENT_NO_BASE");
    if(!xr34::Registered())return leaf("CURRENT_NOT_REGISTERED");
    if(!identity)return leaf("CURRENT_NO_IDENTITY_READER");
    if(!identity(o))return leaf("CURRENT_PASSENGER_IDENTITY");
    if(!s.allowed)return leaf("CURRENT_SOURCE_NOT_ALLOWED");
    if(!OnOwnerThread13()&&!XR34BoundaryCurrentD72(o))return leaf("CURRENT_NOT_OWNER_THREAD");
    if(o.controller!=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load()))return leaf("CURRENT_CONTROLLER_REPLACED");
    if(!IsHenryController(reinterpret_cast<void*>(o.controller)))return leaf("CURRENT_CONTROLLER_NOT_HENRY");
    const xr34::Ops io{&XR34ReadBytes};
    const auto member=xr34::membership(io,b,o);
    if(member!=xr34::Member::Queued&&member!=xr34::Member::Root)return leaf(member==xr34::Member::Unknown?"CURRENT_MEMBERSHIP_UNKNOWN":member==xr34::Member::Exiting?"CURRENT_CHILD_EXITING":"CURRENT_CHILD_ABSENT");
    std::uintptr_t v=0,main=0,parent=0,child=0,context=0,characters=0,character=0,item=0,ammo=0;
    std::uint64_t key=0;unsigned actor=0,token=0;int status=-1;
    if(!XR34Read(s.player,v)||v!=b+0x3AA4B98)return leaf("CURRENT_PLAYER_CLASS");
    if(!XR34Read(s.player+0x30,actor)||actor!=s.actor)return leaf("CURRENT_PLAYER_ID");
    if(!XR34Read(s.equipment+0x40,item)||item!=s.weaponItem)return leaf("CURRENT_WEAPON_SLOT");
    if(!XR34Read(item,v)||v!=b+0x3A2DA88)return leaf("CURRENT_WEAPON_CLASS");
    if(!XR34Read(item+0x30,key)||key!=s.key)return leaf("CURRENT_WEAPON_KEY");
    if(!XR34Read(s.equipment+0x48,ammo)||ammo!=s.ammoItem)return leaf("CURRENT_AMMO_SLOT");
    if(!XR34Read(ammo,v)||v!=b+0x3A2DA88)return leaf("CURRENT_AMMO_CLASS");
    if(!XR34Read(ammo+0x30,key)||key!=s.ammoKey)return leaf("CURRENT_AMMO_KEY");
    if(!XR34Read(s.equipment+0x50,main)||main!=o.main)return leaf("CURRENT_MAIN_SLOT");
    if(!XR34Read(main,v)||v!=b+0x3E7B1B0)return leaf("CURRENT_MAIN_ACTION_CLASS_OR_UNREADABLE");
    if(!XR34Read(main+0x98,parent)||parent!=o.parent)return leaf("CURRENT_MAIN_PARENT");
    if(!XR34Read(parent,v)||v!=b+0x3E78CF8)return leaf("CURRENT_PARENT_CLASS");
    if(!XR34Read(parent+0x80,main)||main!=o.main)return leaf("CURRENT_PARENT_MAIN");
    if(!XR34Read(parent+0x70,child)||child!=o.child)return leaf("CURRENT_PARENT_CHILD");
    if(!XR34Read(child,v)||v!=b+0x3A450B0)return leaf("CURRENT_CHILD_CLASS");
    if(!XR34Read(child+8,context)||context!=o.context)return leaf("CURRENT_CHILD_CONTEXT");
    if(!XR34Read(child+0x54,token)||token!=o.token)return leaf("CURRENT_CHILD_TOKEN");
    if(!XR34Read(child+0x28,status)||status!=(member==xr34::Member::Queued?1:2))return leaf("CURRENT_CHILD_STATUS");
    if(!XR34Read(o.controller+0x20,context)||context!=o.context)return leaf("CURRENT_CONTROLLER_CONTEXT");
    if(!XR34Read(o.controller+0x28,characters)||!XR34Read(characters+0x10,character)||character!=s.character)return leaf("CURRENT_CONTROLLER_CHARACTER");
    void* actorNow=nullptr;void* equipmentNow=nullptr;
#if defined(KCD2VR_XR34_HOST_TEST)
    // Explicit native read-only leaves only; the fixture still executes all
    // real current-owner/container/token/backlink qualification above.
    actorNow=HostXR34ResolveMainOwner(o.main);
    if(actorNow==reinterpret_cast<void*>(s.player))equipmentNow=HostXR34Equipment(actorNow);
#else
    __try {
        actorNow=reinterpret_cast<void*(__fastcall*)(void*)>(b+0x9115E0)(reinterpret_cast<void*>(o.main+0x50));
        if(actorNow==reinterpret_cast<void*>(s.player))
            equipmentNow=reinterpret_cast<void*(__fastcall*)(void*)>(b+0x7F3304)(actorNow);
    }__except(EXCEPTION_EXECUTE_HANDLER){return leaf("CURRENT_NATIVE_READ_FAULT");}
#endif
    if(actorNow!=reinterpret_cast<void*>(s.player))return leaf("CURRENT_NATIVE_ACTOR");
    if(equipmentNow!=reinterpret_cast<void*>(s.equipment))return leaf("CURRENT_NATIVE_EQUIPMENT");
    g_xr34_current_leaf_d69=nullptr;return true;
}
static bool XR34Spatial(const xr34::Owner& o,const xr34::Source& source,xr34::Spatial& spatial) noexcept {
    const auto fn=xr34::spatialReader.load(std::memory_order_acquire);
    return fn&&XR34Current(o)&&fn(o,source,spatial)&&XR34Current(o);
}
static bool XR34QuiverR95(const xr34::Owner& owner,xr34::QuiverInputR95& input) noexcept {
    const auto reader=xr34::quiverReaderR95.load(std::memory_order_acquire);
    return reader&&reader(owner,input);
}
static bool XR34BindingR95(const xr34::Owner& o,xr34::Spatial& binding) noexcept {
    const auto fn=xr34::bindingReader.load(std::memory_order_acquire);
    return fn&&XR34Current(o)&&fn(o,binding)&&XR34Current(o);
}
static bool XR34NativeClip(const xr34::Owner& o,int animation,int scopeId,xr34::Clip& c) noexcept {
    c={};const auto b=g_base.load(std::memory_order_acquire);
    const auto refuse=[](const char* why) noexcept {xr34::lastClipRefusalD72.store(why,std::memory_order_relaxed);return false;};
    xr34::lastClipRefusalD72.store(nullptr,std::memory_order_relaxed);
    if(!XR34Current(o))return refuse("CLIP_CURRENT_OWNER");if(animation<0)return refuse("CLIP_ANIMATION_ID");
    std::uintptr_t scope=0,sc=0,character=0,defaultSkeleton=0,set=0,v=0,header=0;
    unsigned nativeScope=~0u;
    if(!XR34Read(o.child+0x30,scope)||!scope||!XR34Read(scope,v)||v!=b+0x3AEA100||
       !XR34Read(scope+0x10,nativeScope)||nativeScope!=unsigned(scopeId)||
       !XR34Read(scope+0x20,sc)||!XR34Read(sc+0x10,character)||character!=o.source.character||
       !XR34Read(character,v)||v!=b+0x40602F8||!XR34Read(character+0xA70,defaultSkeleton)||
       !XR34Read(defaultSkeleton+0x218,set)||!XR34Read(set,v)||v!=b+0x406BB38)return refuse("CLIP_SCOPE_CHARACTER_ANIMATION_SET");
    float duration=0;
    // Exact calls from native289D788 and named retail GetDuration_sec. These
    // are current read-only native leaves, not SDK slot-number extrapolation.
#if defined(KCD2VR_XR34_HOST_TEST)
    if(!HostXR34ClipHeader(set,animation,header,duration))return refuse("CLIP_HEADER_NATIVE_LOOKUP");
#else
    __try {
        header=reinterpret_cast<std::uintptr_t(__fastcall*)(void*,int)>(b+0x955DF4)(reinterpret_cast<void*>(set),animation);
        duration=reinterpret_cast<float(__fastcall*)(void*,int)>(b+0xAA78BC)(reinterpret_cast<void*>(set),animation);
    }__except(EXCEPTION_EXECUTE_HANDLER){return refuse("CLIP_HEADER_NATIVE_EXCEPTION");}
#endif
    std::uintptr_t entries=0;unsigned count=0;unsigned char segments=0;
    float total=0,start=0,end=0,first=0,last=0;
    if(!header||!XR34Read(header+0x34,total)||!XR34Read(header+0x2C,start)||!XR34Read(header+0x30,end)||
       !XR34Read(header+0xA4,segments)||segments!=1||!XR34Read(header+0x60,first)||first!=0||
       !XR34Read(header+0x64,last)||last!=1||!std::isfinite(duration)||duration<=.1f||duration>60||
       !std::isfinite(total)||!std::isfinite(start)||!std::isfinite(end)||
       std::abs(total-duration)>.001f||std::abs((end-start)-duration)>.001f||
       !XR34Read(header+0x48,v)||v!=b+0x3A7D9D0||!XR34Read(header+0x50,entries)||
       !entries||!XR34Read(entries-4,count)||!count||count>256)return refuse("CLIP_HEADER_SEGMENT_DURATION_OR_EVENT_LIST");
    c.list=header+0x48;c.entries=entries;c.animation=header;c.duration=duration;
    c.id=animation;c.segment=0;c.count=int(count);c.path=b+0x406B7C0; // native CAF disabled-path singleton
    unsigned spawn=0,ammo=0,cock=0,load=0,effectsD87=0;
    for(unsigned i=0;i<count;++i){const auto at=entries+std::size_t(i)*0x50;
        float time=-1;unsigned crc=0;int segment=-1;
        if(!XR34Read(at,time)||!XR34Read(at+8,crc)||!XR34Read(at+0xC,segment))return refuse("CLIP_EVENT_READ");
        if(kcd2vr::crossbow_clock_entry_d87::nativeEffect(crc)){
            if(segment!=0||!std::isfinite(time)||time<0||time>1)return refuse("CLIP_NATIVE_EFFECT_TIME_OR_SEGMENT");
            ++effectsD87;
            if(c.firstNativeEffectR92<0||time<c.firstNativeEffectR92)c.firstNativeEffectR92=time;
        }
        const bool marker=crc==0xAFE0B3FFu||crc==0x34857AFAu||crc==0x8ABB129Au||crc==0xEF667DEAu||crc==0x3D6870EBu||crc==xr34::kDespawnLeverTypoCrcD66C;
        if(!marker)continue;
        std::uintptr_t filter=0,custom=0;char initial=1;
        if(segment!=0||!std::isfinite(time)||time<0||time>1||!XR34Read(at+0x28,filter)||
           !filter||!XR34Read(filter,initial)||initial||!XR34Read(at+0x48,custom)||custom)return refuse("CLIP_EVENT_SEGMENT_FILTER_OR_CUSTOM");
        if(crc==0xAFE0B3FFu){c.spawn=time;++spawn;}
        if(crc==0x34857AFAu){c.ammo=time;++ammo;}
        if(crc==0x8ABB129Au){c.cock=time;++cock;}
        if(crc==0xEF667DEAu){c.load=time;++load;}
        if(crc==0x3D6870EBu||crc==xr34::kDespawnLeverTypoCrcD66C)c.despawn=time;
    }
    if(cock>1||load>1||spawn>1||ammo>1||(cock&&load))return refuse("CLIP_DUPLICATE_OR_AMBIGUOUS_SEMANTIC_MARKERS");
    // Recorded CAF14307 is an ordinary audio/foley transition in the already
    // qualified native reload child, not an ambiguous cock+load clip. Only
    // zero semantic markers can pass through at native time; an incomplete
    // tool/ammo sequence still fails closed rather than inventing an endpoint.
    if(!cock&&!load){
        if(spawn||ammo||c.despawn>=0||effectsD87)return refuse("CLIP_SEMANTIC_MARKERS_WITHOUT_ENDPOINT");
        c.nativeTransitionD80=true;c.qualified=true;return XR34Current(o);
    }
    c.cocking=cock==1;c.loading=load==1;
    if((c.cocking&&o.source.classId!=10&&spawn!=1)||(c.loading&&ammo!=1))return refuse("CLIP_REQUIRED_TOOL_OR_AMMO_MARKER_MISSING");
    c.qualified=true;return XR34Current(o);
}
static bool XR34NativeClock(const xr34::Owner& o,const xr34::Clip& c,xr34::Clock& result) noexcept {
    result={};const auto b=g_base.load(std::memory_order_acquire);
    // Existing reads only. Allocate/reset the receipt payload only while one
    // finite F5/F7/F11 recorder is requested; no clock calls on the OFF path.
    static thread_local kcd2vr::crossbow_clock_entry_d87::SelectionObservation storageD87;
    auto* observed=kcd2vr::reload_diagnostics_d48::requested()?&storageD87:nullptr;
    if(observed)*observed={};
    const auto refuse=[&](const char* why) noexcept {
        if(observed)kcd2vr::reload_diagnostics_d48::recordNativeFaultD87(g_crossbow_xr34.status(),
            observed->why?observed->why:why,GetTickCount64(),*observed,c.list,c.list,c.id,c.segment);
        return false;
    };
    if(!XR34Current(o))return refuse("CLOCK_OWNER_NOT_CURRENT");
    std::uintptr_t root=0,v=0,context=0,character=0,entries=0;unsigned layer=0,layers=0,flags=0;
    float shift=0,pending[2]{},duration=0;unsigned char previousSegment=255,segment=255;
    if(!XR34Read(o.child+0x30,root)||!root)return refuse("CLOCK_ROOT_READ");
    if(!XR34Read(root,v)||v!=b+0x3AEA100)return refuse("CLOCK_ROOT_VFT");
    if(!XR34Read(root+0x20,context)||!XR34Read(context+0x10,character)||character!=o.source.character)return refuse("CLOCK_ROOT_CHARACTER");
    if(!XR34Read(root+0x30,layer)||layer>15||!XR34Read(root+0x34,layers))return refuse("CLOCK_LAYER_READ");
    if(observed){observed->layer=layer;observed->layers=layers;}
    if(!XR34Read(root+0x60,pending)||pending[0]!=0||pending[1]!=0)return refuse("CLOCK_PENDING_BLEND");
    if(!XR34Read(root+0x12C,shift)||!std::isfinite(shift))return refuse("CLOCK_TIME_SHIFT");
    if(!xr34::selectedScopeClockEntryD82(&XR34ReadBytes,character,layer,layers,c.id,entries,c.nativeTransitionD80,observed))return refuse("CLOCK_SCOPE_FIFO");
    if(!XR34Read(entries,v)||v!=b+0x3A70A60)return refuse("CLOCK_ENTRY_VFT");
    if(!XR34Read(entries+8,v)||v)return refuse("CLOCK_ENTRY_SAMPLER");
    if(!XR34Read(entries+0x14,previousSegment)||previousSegment!=0)return refuse("CLOCK_PREVIOUS_SEGMENT");
    if(!XR34Read(entries+0x16,segment)||segment!=0)return refuse("CLOCK_CURRENT_SEGMENT");
    if(!XR34Read(entries+0x4C,flags)||(flags&1))return refuse("CLOCK_ENTRY_LOOP");
    if(!XR34Read(entries+0x34,duration)||!std::isfinite(duration)||std::abs(duration-c.duration)>.001f)return refuse("CLOCK_DURATION");
    if(!XR34Read(entries+0x44,result.normalized))return refuse("CLOCK_NORMALIZED_READ");
    if(!XR34Read(entries+0x2C,result.playbackScale))return refuse("CLOCK_PLAYBACK_READ");
    if(!XR34Read(o.controller+0x9C,result.controllerScale))return refuse("CLOCK_CONTROLLER_SCALE_READ");
    if(!XR34Read(character+0xA9C,result.characterScale))return refuse("CLOCK_CHARACTER_SCALE_READ");
    if(!XR34Read(o.child+0x10,result.activeTime))return refuse("CLOCK_CHILD_TIME_READ");
#if defined(KCD2VR_XR34_HOST_TEST)
    if(!HostXR34Remaining(root,result.remaining))return refuse("CLOCK_REMAINING_READ");
    result.remaining-=shift;
#else
    __try {result.remaining=reinterpret_cast<float(__fastcall*)(void*)>(b+0x4B2898)(reinterpret_cast<void*>(root))-shift;}
    __except(EXCEPTION_EXECUTE_HANDLER){return refuse("CLOCK_REMAINING_EXCEPTION");}
#endif
    if(!std::isfinite(result.remaining)||!std::isfinite(result.activeTime)||!std::isfinite(result.playbackScale)||
       !std::isfinite(result.controllerScale)||!std::isfinite(result.characterScale))return refuse("CLOCK_FINITE_FIELDS");
    result.entry=entries;result.segment=0;result.qualified=true;
    if(!XR34Current(o))return refuse("CLOCK_OWNER_CHANGED_AFTER_READ");
    if(observed&&observed->duplicate)kcd2vr::reload_diagnostics_d48::recordNativeFaultD87(g_crossbow_xr34.status(),
        "CLOCK_INERT_DUPLICATE_QUALIFIED",GetTickCount64(),*observed,c.list,c.list,c.id,c.segment);
    return true;
}
static void XR34PublishPassengerNative() noexcept {
    xr34::Passenger p{};p.publication=xr34::passengerPublication.load(std::memory_order_acquire);const auto& state=g_crossbow_xr34.status();
    const auto now=std::uint64_t(GetTickCount64());
    if(state.sourceEstablished&&state.clockOwned&&state.phase!=xr34::Phase::Retiring&&state.reason!=xr34::Reason::NativeAck&&XR34Current(state.owner)&&
       xr34::ReadSource(p.frame)&&xr34::fresh(p.frame,now)&&
       xr34::sameSource(p.frame,state.owner.source)&&g_crossbow_xr34.clip().qualified&&
       XR34NativeClock(state.owner,g_crossbow_xr34.clip(),p.clock)&&
       XR34Spatial(state.owner,p.frame,p.binding)&&p.binding.qualified&&p.binding.serial==state.owner.serial&&
       p.binding.sample==p.frame.sample&&p.binding.tick==p.frame.tick&&
       XR34Read(state.owner.child+0x30,p.root)&&XR34Read(state.owner.child+0x28,p.childStatus)&&p.childStatus==2){
        xr34::Source after{};
        if(xr34::ReadSource(after)&&after.publication==p.frame.publication&&XR34Current(state.owner)){
            p.owner=state.owner;p.clip=g_crossbow_xr34.clip();p.workingHand=1-p.frame.primary;
            p.constraintD78=state.constraintD78;p.valid=true;
        }
    }
    (void)xr34::passengerMailbox.publish(p);
    xr34::Admission admission{};
    const auto binding=xr34::bindingReader.load(std::memory_order_acquire);
    if(!state.sourceEstablished&&state.clockOwned&&state.phase!=xr34::Phase::Retiring){
        // D85 xbow: same terms and order as before; the first missing one is
        // kept as a static literal so a lost first pose can say which stage.
        const char* why=nullptr;
        if(now<state.started||now>state.firstPoseDeadline)why="FIRST_POSE_WINDOW_OVER";
        else if(!g_crossbow_xr34.clip().qualified)why="CLIP_NOT_SELECTED";
        else if(!XR34Current(state.owner))why=g_xr34_current_leaf_d69?g_xr34_current_leaf_d69:"OWNER_NOT_CURRENT";
        else if(!XR34NativeClock(state.owner,g_crossbow_xr34.clip(),admission.clock))why="NATIVE_CLOCK_NOT_QUALIFIED";
        else if(!binding||!binding(state.owner,admission.binding)||!admission.binding.qualified)why="NATIVE_BINDING_NOT_QUALIFIED";
        else if(!XR34Read(state.owner.child+0x30,admission.root)||!XR34Read(state.owner.child+0x28,admission.childStatus)||
           admission.childStatus!=2)why="CHILD_NOT_INSTALLED";
        if(!why){
            admission.owner=state.owner;admission.clip=g_crossbow_xr34.clip();admission.publication=p.publication;
            admission.started=state.started;admission.deadline=state.firstPoseDeadline;admission.tick=now;admission.valid=true;
        }
        xr34::lastBootstrapRefusalD85.store(why,std::memory_order_relaxed);
    }
    (void)xr34::admissionMailbox.publish(admission);
    // D77 copies the exact already-produced packets; no extra native reads.
    kcd2vr::diagnostics::activity_links_d77::observeReload(state,p,0,now);
    kcd2vr::diagnostics::activity_links_d77::observeReload(state,admission,1,now);
}
static void XR34FallbackVisibleD65C() noexcept; // D65C
// A busy callback may read this immutable identity only to prove that a scan
// belongs to another CAF. It grants no interval, event, pose or clock credit.
struct XR34PublishedClipD82 {
    std::uint64_t publication=0,serial=0;
    std::uintptr_t character=0,list=0,path=0;
    int animation=-1,segment=-1;bool valid=false;
};
static kcd2vr::core::SnapshotMailbox<XR34PublishedClipD82> g_xr34_clip_d82;
static void XR34PublishNative(bool nativeMutationR95=false) noexcept {
    // Any native boundary/rate mutation invalidates the old post-Update packet.
    // Only XR34PostControllerNative may republish a valid passenger.
    XR34FallbackVisibleD65C(); // D65C
    // R95: an unchanged pre-controller observation is not a pose revocation.
    // In particular a paused setup must keep the first real tool placement
    // usable while the next native callback is pending. Actual state/clock
    // mutations still revoke the old witness before native Update.
    xr34::Status published{};
    const auto sameStatus=[](const xr34::Status& a,const xr34::Status& b) noexcept {
        const auto sameVec=[](xr34::Vec x,xr34::Vec y) noexcept {return x.x==y.x&&x.y==y.y&&x.z==y.z;};
        // Source is immutable for a serial. Compare semantic identity and
        // every mutable status/constraint field, never object padding.
        const auto& x=a.owner;const auto& y=b.owner;
        const auto& c=a.constraintD78;const auto& d=b.constraintD78;
        return x.serial==y.serial&&xr34::sameSource(x.source,y.source)&&x.controller==y.controller&&
            x.main==y.main&&x.parent==y.parent&&x.child==y.child&&x.context==y.context&&x.root==y.root&&x.token==y.token&&
            a.phase==b.phase&&a.reason==b.reason&&a.progress==b.progress&&a.speed==b.speed&&
            a.started==b.started&&a.firstPoseDeadline==b.firstPoseDeadline&&a.clockOwned==b.clockOwned&&
            a.cocking==b.cocking&&a.loading==b.loading&&a.placeTool==b.placeTool&&a.placeBolt==b.placeBolt&&
            a.sourceEstablished==b.sourceEstablished&&a.toolTakenD72==b.toolTakenD72&&a.toolEngagedD72==b.toolEngagedD72&&
            a.boltTakenD72==b.boltTakenD72&&a.toolMountedD78==b.toolMountedD78&&
            sameVec(c.start,d.start)&&sameVec(c.axis,d.axis)&&c.travel==d.travel&&c.progress==d.progress&&
            c.active==d.active&&c.mounted==d.mounted&&sameVec(c.stringGripD78,d.stringGripD78);
    };
    const bool changed=nativeMutationR95||!xr34::statusMailbox.read(published)||
        !sameStatus(published,g_crossbow_xr34.status());
    const auto publication=changed?xr34::passengerPublication.fetch_add(1,std::memory_order_acq_rel)+1:
        xr34::passengerPublication.load(std::memory_order_acquire);
    xr34::activeNativeLease.store((g_crossbow_xr34.status().clockOwned&&g_crossbow_xr34.status().phase!=xr34::Phase::Retiring)?g_crossbow_xr34.status().owner.serial:0,std::memory_order_release);
    (void)xr34::statusMailbox.publish(g_crossbow_xr34.status());
    const auto& current=g_crossbow_xr34.status();const auto& clip=g_crossbow_xr34.clip();
    XR34PublishedClipD82 identity{};
    if(current.clockOwned&&current.phase!=xr34::Phase::Retiring&&clip.qualified)
        identity={publication,current.owner.serial,current.owner.source.character,clip.list,clip.path,clip.id,clip.segment,true};
    (void)g_xr34_clip_d82.publish(identity);
    xr34::cleanupInterested.store(g_crossbow_xr34.pending(),std::memory_order_release);
    namespace instant=kcd2vr::instant_d72;const auto& status=g_crossbow_xr34.status();
    const auto visible=status.phase==xr34::Phase::Retiring?instant::State::Retiring:
        status.clockOwned&&status.owner.source.instantD72?instant::State::Active:
        !instant::Reloads()?instant::State::Native:xr34::Registered()?instant::State::Available:instant::State::Unsupported;
    instant::Publish(instant::Family::Reloads,visible);
}
static bool XR34NewAdmissionT25(const xr34::Owner& o) noexcept {
    const auto& s=o.source;
    return s.primary>=0&&s.primary<2&&s.player&&s.manager&&s.character&&s.body&&s.request&&
        !kcd2vr::hand_claim_t24::competing(1-s.primary,{s.player,s.manager,s.character,s.body,s.request})&&
        kcd2vr::frik_pose_probe::ThrowEquipmentAdmissionT25(1-s.primary,s.player,s.manager,
            s.character,s.body,s.request)==kcd2vr::frik_pose_probe::ThrowAuthorityT22::Allowed;
}
static bool XR34ConfigureNative() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b))return false;
    if(g_crossbow_base_xr34==b)return true;
    // Shared interior native consumers stay byte-qualified even when an entry
    // is hooked by our original-first observer. Root verifies hook entries.
    constexpr unsigned char seed[]{0xF3,0x0F,0x11,0x43,0x5C};
    constexpr unsigned char consume[]{0x0F,0x28,0xCF,0xF3,0x0F,0x59,0x4B,0x5C,0x48,0x8B,0xCB,0xFF,0x50,0x30};
    if(!MatchBytes(b+0x2897C05,seed,sizeof(seed))||!MatchBytes(b+0x4B3F12,consume,sizeof(consume))||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x406BB38),0x68,0)!=b+0xAA78BC||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x406BB38),0xF0,0)!=b+0x955DF4)return false;
    xr34::Ops io{&XR34ReadBytes,&XR34WriteBytes,&xr34::ReadSource,&XR34Spatial,&XR34NativeClip,&XR34NativeClock,&XR34Current,&XR34NewAdmissionT25,&XR34BindingR95,&XR34QuiverR95};
    g_crossbow_xr34.configure(b,io);g_crossbow_base_xr34=b;return true;
}
// Called AFTER the exact native OnStart returns true: child is already queued,
// native stat speed is seeded, no event-bearing Update has been granted by us.
// Finish does only caller-owned POD/atomic publication. It never restores a
// native rate while holding the shared new-admission token.
static void XR34FinishAdmissionT25(void* opaque) noexcept {
    const bool started=opaque&&*static_cast<const bool*>(opaque);
    if(!started)g_crossbow_xr34.markStartFailure();
    XR34PublishNative(); // raw cleanupInterested remains even on mailbox failure
}
// Donor refusal labels under finite F5/F7/F11 capture, or legacy F3 combat capture.
// D65C: one visible line when the physical reload is wanted but the native
// child could not be taken (the game then reloads on its own, at native speed).
static constexpr const char* kXR34StartHeadD85="crossbow: native reload";
// D85 xbow: a start refusal is the player's business only when HIS grip asked
// the game for this reload (NPC and trigger auto-reload starts stay quiet).
static bool XR34ShortLineD85(char (&line)[kcd2vr::crossbow_refusal_d85::kLine+1],const char* head,
    const char* why,const char* detail,std::uint64_t now) noexcept {
    namespace cr=kcd2vr::crossbow_refusal_d85;
    const bool start=head&&!std::strcmp(head,kXR34StartHeadD85);
    const auto asked=xr34::gripRequestTickD85.load(std::memory_order_acquire);
    if(start&&!(asked&&now>=asked&&now-asked<=250))return false;
    cr::line(line,start?cr::LineKind::NotTaken:cr::LineKind::Stopped,why,detail);return true;
}
#if defined(KCD2VR_XR34_HOST_TEST)
// Host fixture leaves: the line itself is a game-thread dispatch; count it.
static unsigned g_xr34_visible_lines_d65c=0;
static char g_xr34_visible_text_d85[kcd2vr::crossbow_refusal_d85::kLine+1]{};
static unsigned g_xr34_short_lines_d85=0;
static bool CrossbowReloadWantedD65C() noexcept {return true;}
// R96 host fixture leaf: NATIVE-DRIVEN unless a test selects HAND ONLY.
static bool g_xr34_hand_only_route_r96=false;
static bool XR34HandOnlyRouteR96() noexcept {return g_xr34_hand_only_route_r96;}
static void XR34VisibleD65C(const char* head,const char* why,const char* detail) noexcept {++g_xr34_visible_lines_d65c;
    if(XR34ShortLineD85(g_xr34_visible_text_d85,head,why,detail,std::uint64_t(GetTickCount64())))++g_xr34_short_lines_d85;}
#else
static bool CrossbowReloadWantedD65C() noexcept {
    return kcd2vr::frik_pose_probe::ReloadMode47C2()==2||kcd2vr::ranged_options_d65c::crossbowReload.load(std::memory_order_acquire);
}
static bool XR34HandOnlyRouteR96() noexcept {return kcd2vr::ranged_options_d65c::crossbowHandOnlyR90.load(std::memory_order_acquire);}
static void XR34VisibleD65C(const char* head,const char* why,const char* detail) noexcept {
    using namespace diagnostics::capture168;
    const auto now=std::uint64_t(GetTickCount64());
    const auto ticket=kcd2vr::diagnostics::reload_capture_d84::select(now);
    // D85 xbow: one short line, always in KCD2VR.log. R90: on screen only while F2 records.
    {char line[kcd2vr::crossbow_refusal_d85::kLine+1]{};
        const std::uint64_t f2R90=active(Domain::Bug,now)?gate.session[unsigned(Domain::Bug)].load(std::memory_order_acquire):0;
        if(XR34ShortLineD85(line,head,why,detail,now))(void)kcd2vr::visible_failure::publish(line,now,f2R90,Domain::Bug);}
    if(!diagnostic_trace::Enabled()||(!ticket.session&&!active(Domain::Combat,now)))return;
    char row[320]{};std::snprintf(row,sizeof(row),"D65 CROSSBOW_RELOAD state=%s why=%s detail=%s recorder=%s session=%llu captureMask=0x%X",head,why?why:"-",detail?detail:"-",
        name(ticket.session?ticket.domain:Domain::Combat),(unsigned long long)(ticket.session?ticket.session:gate.session[unsigned(Domain::Combat)].load()),ticket.activeMask);
    diagnostic_trace::WriteFor(ticket.session?ticket.domain:Domain::Combat,row);
}
#endif
static void XR34StartWhy47(const char* why,void* parent) noexcept {
    if(kcd2vr::reload_diagnostics_d48::requested())
        kcd2vr::reload_diagnostics_d48::recordStart(why,GetTickCount64(),reinterpret_cast<std::uintptr_t>(parent));
    if(why&&std::strcmp(why,"TAKEN")!=0&&CrossbowReloadWantedD65C()){
        const char* source=xr34::lastSourceRefusalD65C.load(std::memory_order_relaxed);
        XR34VisibleD65C(kXR34StartHeadD85,why,std::strcmp(why,"NO_SOURCE_PUBLISHED")==0?source:nullptr);
    }
}
static void XR34FallbackVisibleD65C() noexcept {
    static std::uint64_t seen=0;
    const auto serial=xr34::fallbackSerialD65C.load(std::memory_order_acquire);
    if(serial==seen)return;seen=serial;
    // D85 xbow: name the paused stage that ran out (first tracked pose or the
    // string/lever pose join) instead of an unrelated old clip refusal.
    const unsigned stage=xr34::fallbackStageD85.exchange(0,std::memory_order_relaxed);
    const char* bootstrap=xr34::lastBootstrapRefusalD85.load(std::memory_order_relaxed);
    const char* detail=stage==1?(bootstrap?bootstrap:"ADMITTED_NO_TRACKED_POSE"):stage==2?"NO_STRING_OR_LEVER_POSE":stage==3?"NO_FIRST_HELD_ITEM_PLACEMENT":
        xr34::lastClipRefusalD72.load(std::memory_order_relaxed);
    XR34VisibleD65C("crossbow: reload gesture lost, native finishes it",xr34::reasonName(xr34::Reason(xr34::fallbackReasonD65C.load(std::memory_order_relaxed))),detail);
}
static void XR34PostStartNativeImplD72(void* parent,bool nativeResult) noexcept {
    g_crossbow_xr34.resetStartReasonD69();g_xr34_current_leaf_d69=nullptr;
    if(!nativeResult)return;
    const auto now=std::uint64_t(GetTickCount64());xr34::Source s{};
    if(!xr34::Registered()){XR34StartWhy47("NOT_REGISTERED",parent);return;}
    if(!xr34::ReadSource(s)){XR34StartWhy47("NO_SOURCE_PUBLISHED",parent);return;}
    if(!xr34::fresh(s,now)){XR34StartWhy47("SOURCE_OLDER_THAN_80MS",parent);return;}
    // A native trigger/automatic request must remain a real native escape.
    // Only the exact synchronous Grip request may nominate a NEW clock owner;
    // later metadata in that action keeps this original intent without expiry.
    if(!g_crossbow_xr34.acceptsNewActionR95(s,now)){
        XR34StartWhy47("NATIVE_REQUEST_WITHOUT_PHYSICAL_GRIP",parent);return;}
    // R96: with the HAND ONLY route the bolt is ours (bow pattern), so XR34
    // takes only a NEW physical chest Grip (the goat's foot/cranequin cocking
    // with the game's real spanner). The R95 cocking->loading continuation is
    // not taken: that load request is the hand route's, or stays fully native.
    // R108: and only the CHEST Grip (entry kind 2). newGripR95 also admits a
    // string (1) or quiver (3) Grip; under HAND ONLY those are the hand
    // route's (crossbow_hand_d89) and must never start a second owner here.
    if(XR34HandOnlyRouteR96()&&(!g_crossbow_xr34.newGripR95(s,now)||s.entryGripD72!=2)){
        XR34StartWhy47("HAND_ONLY_ROUTE_LOAD_IS_NOT_XR34_R96",parent);return;}
    // R111: HAND ONLY + VrHeavyCrossbowHandPullR111: the heavy crossbow is spanned
    // by hand (crossbow_hand_d89), never by the game's cranequin action. Its chest
    // Grip no longer becomes a request (crossbow_refusal_d85 decideGrip); this is
    // the second lock so no path can give XR34 a class 15 cocking.
    if(XR34HandOnlyRouteR96()&&s.classId==15&&kcd2vr::ranged_options_d65c::heavyCrossbowHandPullR111.load(std::memory_order_acquire)){
        XR34StartWhy47("HEAVY_STRING_PULLED_BY_HAND_R111",parent);return;}
    // R117: the same second lock for the goat's foot with VrMediumCrossbowHandPullR117.
    if(XR34HandOnlyRouteR96()&&s.classId==14&&kcd2vr::ranged_options_d65c::mediumCrossbowHandPullR117.load(std::memory_order_acquire)){
        XR34StartWhy47("MEDIUM_STRING_PULLED_BY_HAND_R117",parent);return;}
    if(!XR34ConfigureNative()){XR34StartWhy47("NATIVE_BYTES_DIFFER",parent);return;}
    // XR35: only NEW LEFT working-hand ownership yields to retained equipment debt.
    // OLD restore/publication above and the original native call remain ungated.
    if(s.primary==0&&kcd2vr::native_weapon_body_t4::EquipmentAdmissionSuspendedH36()){XR34StartWhy47("EQUIPMENT_ADMISSION_SUSPENDED(holster)",parent);return;}
    xr34::Owner o{};o.source=s;o.parent=reinterpret_cast<std::uintptr_t>(parent);
    o.controller=reinterpret_cast<std::uintptr_t>(g_henry_ctrl.load());
    if(s.primary<0||s.primary>1||!XR34Read(s.equipment+0x50,o.main)||!XR34Read(o.parent+0x70,o.child)||
       !XR34Read(o.controller+0x20,o.context)||!XR34Read(o.child+0x54,o.token)){XR34StartWhy47("ACTION_CHILD_UNREADABLE",parent);return;}
    // A reload OnStart also occurs for NPCs. Prove this incoming Henry action
    // before it can retire an earlier Henry lease, even on the same OS thread.
    if(!XR34Current(o)){XR34StartWhy47(g_xr34_current_leaf_d69?g_xr34_current_leaf_d69:"CURRENT_OWNER_REFUSED",parent);return;}
    const bool restored=g_crossbow_xr34.restore(xr34::Reason::OwnerChanged);
    XR34PublishNative();if(!restored){XR34StartWhy47("OLD_LEASE_NOT_RESTORED",parent);return;}
    float speed=0;if(!XR34Read(o.child+0x5C,speed)){XR34StartWhy47("ACTION_SPEED_UNREADABLE",parent);return;}
    namespace ma=kcd2vr::mutual_admission_t25;
    ma::Token token=ma::TryBegin(1-s.primary,ma::Kind::Reload);
    if(!ma::valid(token)){XR34StartWhy47("WORKING_HAND_BUSY(door/grab/throw lease)",parent);return;}
    bool started=false;const char* startWhyD69="START_NOT_RETURNED";
    __try{started=g_crossbow_xr34.startNewAfterRestore(o,speed,now);
        startWhyD69=g_crossbow_xr34.startReasonD69()==xr34::StartReasonD69::CurrentOwner&&g_xr34_current_leaf_d69?
            g_xr34_current_leaf_d69:xr34::startReasonNameD69(g_crossbow_xr34.startReasonD69());}
    __finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}
    // Snapshot before Finish/publication invokes any further owner observer.
    // Existing finite diagnostics only; no extra pointer reads or native calls.
    XR34StartWhy47(started?"TAKEN":startWhyD69,parent);
    // Normal failed writes are serviced promptly OUTSIDE the lease. An escaping
    // SEH already published phase Retiring/raw debt before Finish unlocked it.
    if(!started&&g_crossbow_xr34.pending()){
        (void)g_crossbow_xr34.restore(xr34::Reason::NativeFallback);XR34PublishNative();
    }
}
static void XR34PostStartNative(void* parent,bool nativeResult) noexcept {
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),parent,[&]() { 
    XR34PostStartNativeImplD72(parent,nativeResult);
    });
}
static void XR34PostMetadataNative(void* parent,void* child,int animation,int scope) noexcept {
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),parent,[&]() { 
    const auto& o=g_crossbow_xr34.status().owner;
    if(o.parent==reinterpret_cast<std::uintptr_t>(parent)&&o.child==reinterpret_cast<std::uintptr_t>(child)){
        // Native289D7AC..289D7B8 ignores metadata for a secondary scope. The
        // original has already returned, so mirror that filter before asking
        // the root-clip parser. Secondary look/weapon callbacks must not cancel
        // the actual Henry reload lease or replace its semantic milestones.
        std::uintptr_t root=0,vft=0;unsigned nativeScope=~0u;
        if(XR34Current(o)&&XR34Read(o.child+0x30,root)&&root&&XR34Read(root,vft)&&
           vft==g_base.load(std::memory_order_acquire)+0x3AEA100&&XR34Read(root+0x10,nativeScope)&&
           nativeScope!=unsigned(scope))return;
        (void)g_crossbow_xr34.metadata(animation,scope,std::uint64_t(GetTickCount64()));
    }
    XR34PublishNative(true);
    });
}
static void XR34PreControllerNative(void* ctrl,float dt) noexcept {
    return XR34WithBoundaryD73(ctrl,nullptr,[&]() { 
    if(!g_xr34_boundary_d72.controller){
        // Losing Henry/source identity withdraws admission, but must not strand
        // our zero speed. restore() independently proves the retained native
        // container, child context/token and exactly our last speed before write.
        if(reinterpret_cast<std::uintptr_t>(ctrl)==g_crossbow_xr34.status().owner.controller){
            (void)g_crossbow_xr34.restore(xr34::Reason::OwnerChanged);XR34PublishNative();}
        return;
    }
    if(!xr34::Interested()||!XR34ConfigureNative())return;
    if(g_crossbow_xr34.pending()&&xr34::revokedNativeLease.load(std::memory_order_acquire)==g_crossbow_xr34.status().owner.serial)
        (void)g_crossbow_xr34.restore(xr34::Reason::NativeFallback);
    else g_crossbow_xr34.tick(reinterpret_cast<std::uintptr_t>(ctrl),dt,std::uint64_t(GetTickCount64()));
    XR34PublishNative();
    });
}
static void XR34PostControllerNative(void* ctrl) noexcept {
    return XR34WithBoundaryD73(ctrl,nullptr,[&]() { 
    if(reinterpret_cast<std::uintptr_t>(ctrl)!=g_crossbow_xr34.status().owner.controller||!XR34BoundaryCurrentD72(g_crossbow_xr34.status().owner))return;
    XR34PublishPassengerNative(); // original native Update has returned, no timestamp rebasing
    });
}
static void XR34BeforeStopNative(void* parent) noexcept {
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),parent,[&]() { 
    if(!XR34BoundaryCurrentD72(g_crossbow_xr34.status().owner))return;
    g_crossbow_xr34.stopBefore(reinterpret_cast<std::uintptr_t>(parent));XR34PublishNative();
    });
}
static void XR34BeforeCompleteNative(void* parent,void* child) noexcept {
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),parent,[&]() { 
    if(!XR34BoundaryCurrentD72(g_crossbow_xr34.status().owner))return;
    if(reinterpret_cast<std::uintptr_t>(child)==g_crossbow_xr34.status().owner.child)
        g_crossbow_xr34.completionBefore(reinterpret_cast<std::uintptr_t>(parent),reinterpret_cast<std::uintptr_t>(child));
    XR34PublishNative();
    });
}
struct XR34ScanWitness {std::uint64_t serial=0;std::uintptr_t skel=0,list=0,path=0,entries=0;
    int animation=-1,segment=-1,count=0;float from=0,to=0;bool includeStart=false,valid=false;};
static thread_local XR34ScanWitness g_crossbow_scan_xr34;
// Plain POD begin/end API is intentional: the existing native scanner owns a
// Windows __try/__finally, which cannot contain a C++ destructor-bearing local.
static XR34ScanWitness XR34BeginScanNative(void* skel,bool includeStart,float from,float to,const void* information,int segment) noexcept {
    const auto previous=g_crossbow_scan_xr34;g_crossbow_scan_xr34={};
    // This hook also sees NPC/idle/look scans. Keep the inactive path at one
    // atomic read; no native memory copy or owner transaction is needed there.
    if(!xr34::cleanupInterested.load(std::memory_order_acquire))return previous;
    struct Info {std::uintptr_t list,path;int first,animation,last;float weight;} info{};
    const bool infoRead=SehCopy(information,&info,sizeof(info));
    OwnerTransaction19c4 tx;
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),nullptr,[&]() { 
    if(!tx||!g_xr34_boundary_d72.controller){
        // Failure to observe a scan grants no event witness, but a scan is
        // not itself a reload effect. Empty/audio scans also run on workers.
        // Revoking here cancelled valid reloads merely on lock contention.
        // XR34ExistingEventObserver independently rejects every effectful
        // event without its exact witness (including a busy owner boundary).
        // Keep the witness empty; never authorize time/ammo from this path.
        return previous;
    }
    const auto& state=g_crossbow_xr34.status();const auto& c=g_crossbow_xr34.clip();
    if(!state.clockOwned||!c.qualified||reinterpret_cast<std::uintptr_t>(skel)!=state.owner.source.character+0x110||!XR34Current(state.owner))return previous;
    if(!infoRead||info.list!=c.list||info.animation!=c.id||segment!=c.segment)return previous;
    if(info.path!=c.path||info.first<0||info.last<info.first||
       info.last-info.first>64||info.last>=c.count)return previous;
    if(g_crossbow_xr34.interval(info.list,info.animation,segment,from,to,
        kcd2vr::reload_diagnostics_d48::requested()?GetTickCount64():0)){
        g_crossbow_scan_xr34={state.owner.serial,reinterpret_cast<std::uintptr_t>(skel),c.list,c.path,c.entries,
            c.id,segment,c.count,from,to,includeStart,true};
        // Successful observation changes no clock, binding or pose. Keep the
        // completed passenger usable until an actual event changes ownership.
    }else XR34PublishNative(); // interval rejection restored native ownership
    return previous;
    });
}
static void XR34EndScanNative(const XR34ScanWitness& previous) noexcept {g_crossbow_scan_xr34=previous;}
struct XR34ScanNative {XR34ScanWitness previous{};bool current=false;
    XR34ScanNative(void* skel,bool start,float from,float to,const void* info,int segment) noexcept {
        previous=XR34BeginScanNative(skel,start,from,to,info,segment);current=g_crossbow_scan_xr34.valid;
    }
    ~XR34ScanNative(){XR34EndScanNative(previous);}
};
static void XR34BeforeEventNative(void* parent,const void* event) noexcept {
    return XR34WithBoundaryD73(g_henry_ctrl.load(std::memory_order_acquire),parent,[&]() { 
    const auto& state=g_crossbow_xr34.status();const auto& c=g_crossbow_xr34.clip();
    if(!state.clockOwned||reinterpret_cast<std::uintptr_t>(parent)!=state.owner.parent)return;
    const auto& scan=g_crossbow_scan_xr34;std::uintptr_t entry=0,path=0;int animation=-1,segment=-1;
    unsigned crc=0,originalCrc=0;float time=-1,originalTime=-1;
    const auto at=reinterpret_cast<std::uintptr_t>(event);
    const bool valid=scan.valid&&scan.serial==state.owner.serial&&scan.list==c.list&&scan.entries==c.entries&&
        XR34Current(state.owner)&&XR34Read(at+0x50,entry)&&XR34Read(at+0x10,path)&&path==scan.path&&
        XR34Read(at+0x18,animation)&&animation==scan.animation&&XR34Read(at,time)&&
        XR34Read(at+0x1C,crc)&&entry>=scan.entries&&entry-scan.entries<std::size_t(scan.count)*0x50&&
        (entry-scan.entries)%0x50==0&&XR34Read(entry,originalTime)&&originalTime==time&&
        XR34Read(entry+8,originalCrc)&&originalCrc==crc&&XR34Read(entry+0xC,segment)&&segment==scan.segment&&
        std::isfinite(time)&&(scan.includeStart?time>=scan.from:time>scan.from)&&time<=scan.to;
    if(valid)g_crossbow_xr34.eventBefore(crc);
    else {
        if(kcd2vr::reload_diagnostics_d48::requested())kcd2vr::reload_diagnostics_d48::recordNativeFaultD87(state,
            scan.valid?"EVENT_SCAN_JOIN_REFUSED":"EVENT_WITHOUT_SCAN",GetTickCount64(),{},scan.list,c.list,animation,segment,
            scan.from,scan.to,g_crossbow_xr34.permittedIntervalD87(),(scan.valid?1u:0u)|(scan.includeStart?2u:0u));
        (void)g_crossbow_xr34.restore(xr34::Reason::UnapprovedInterval);
    }
    XR34PublishNative(true); // withdrawal is visible BEFORE original native transfer/cleanup
    });
}
// Exact retail Reloading::OnAnimEvent at 289CCD0..289CE4F returns without an
// effect for every other CRC. Such audio/foley/look events may be dispatched
// from another Henry layer. They neither spend ammo nor advance this reload,
// so they must not invalidate its pose or revoke it on a busy owner lock.
// Keep ALL native effects here, including the seven non-milestone flags and
// cleanup cases; a missing scan for any of them still fails closed.
static bool XR34NativeEventEffectD82(unsigned crc) noexcept {
    return kcd2vr::crossbow_clock_entry_d87::nativeEffect(crc);
}
static void XR34ExistingEventObserver(void* parent,const void* event) noexcept {
    if(!xr34::cleanupInterested.load(std::memory_order_acquire))return;
    unsigned crc=0;
    if(XR34Read(reinterpret_cast<std::uintptr_t>(event)+0x1C,crc)&&!XR34NativeEventEffectD82(crc))return;
    OwnerTransaction19c4 tx;if(tx)XR34BeforeEventNative(parent,event);else xr34::RevokeNativeParent(parent);
}

#if !defined(KCD2VR_XR34_HOST_TEST)
// Only new entry hooks. Finalizer and event scan reuse their existing owners.
// Retail2897A40 retains original RDX;2897ADE forwards its pointer to289C80C.
// Preserve this opaque native input without guessing its gameplay type.
using XR34StartFn=bool(__fastcall*)(void*,void*);
using XR34MetadataFn=void(__fastcall*)(void*,void*,int,int);
using XR34StopFn=void(__fastcall*)(void*);
static XR34StartFn g_crossbow_start_xr34=nullptr;
static XR34MetadataFn g_crossbow_metadata_xr34=nullptr;
static XR34StopFn g_crossbow_stop_xr34=nullptr;
static std::array<std::optional<DetourModKit::hook::Hook>,3> g_crossbow_hooks_xr34;
static std::atomic<unsigned> g_crossbow_inflight_xr34{0};
#if defined(_MSC_VER)
#pragma push_macro("__try")
#undef __try
#endif
// C++ transaction locals live in small bodies, outside the SEH-finally wrapper.
// The same existing counter MUST be released even if an original native entry
// raises an SEH exception that the game catches farther up under /EHsc.
static bool XR34StartBody(void* parent,void* originalStartInput) {
    const bool result=g_crossbow_start_xr34?g_crossbow_start_xr34(parent,originalStartInput):false;
    {OwnerTransaction19c4 tx;if(tx)XR34PostStartNative(parent,result);}return result;
}
static bool __fastcall XR34StartDetour(void* parent,void* originalStartInput) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);bool result=false;
#if defined(_MSC_VER)
    __try{result=XR34StartBody(parent,originalStartInput);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{result=XR34StartBody(parent,originalStartInput);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
    return result;
}
static void XR34MetadataBody(void* parent,void* child,int animation,int scope) {
    if(g_crossbow_metadata_xr34)g_crossbow_metadata_xr34(parent,child,animation,scope);
    OwnerTransaction19c4 tx;if(tx)XR34PostMetadataNative(parent,child,animation,scope);
}
static void __fastcall XR34MetadataDetour(void* parent,void* child,int animation,int scope) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);
#if defined(_MSC_VER)
    __try{XR34MetadataBody(parent,child,animation,scope);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{XR34MetadataBody(parent,child,animation,scope);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
static void XR34StopBody(void* parent) {
    {OwnerTransaction19c4 tx;if(tx)XR34BeforeStopNative(parent);else xr34::RevokeNativeParent(parent);}
    if(g_crossbow_stop_xr34)g_crossbow_stop_xr34(parent);
}
static void __fastcall XR34StopDetour(void* parent) {
    g_crossbow_inflight_xr34.fetch_add(1,std::memory_order_acq_rel);
#if defined(_MSC_VER)
    __try{XR34StopBody(parent);}
    __finally{g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try{XR34StopBody(parent);}
    catch(...){g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);throw;}
    g_crossbow_inflight_xr34.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
#if defined(_MSC_VER)
#pragma pop_macro("__try")
#endif
static void XR34ExistingCompleteObserver(void* parent,void* child) noexcept {
    OwnerTransaction19c4 tx;if(tx)XR34BeforeCompleteNative(parent,child);else xr34::RevokeNativeParent(parent);
}
// Root must first restore the current lease on its exact native owner, before
// trying to disarm ANY executor or callback. Unknown restoration retains all.
static bool StopCrossbowNativeXR34() noexcept {
    xr34::WithdrawRegistration();
    if(g_crossbow_xr34.pending()&&!OnOwnerThread13())return false;
    const bool ok=g_crossbow_xr34.restore(xr34::Reason::NativeStop);XR34PublishNative();return ok;
}
static bool DisarmCrossbowNativeXR34() noexcept {
    bool disabled=true;for(auto& h:g_crossbow_hooks_xr34)if(h&&h->is_enabled()){
        const auto result=h->disable();disabled=bool(result)&&!h->is_enabled()&&disabled;}
    if(!disabled||g_crossbow_inflight_xr34.load(std::memory_order_acquire))return false;
    xr34::completionObserver.store(nullptr,std::memory_order_release);xr34::eventObserver.store(nullptr,std::memory_order_release);
    for(auto& h:g_crossbow_hooks_xr34)h.reset();
    g_crossbow_start_xr34=nullptr;g_crossbow_metadata_xr34=nullptr;g_crossbow_stop_xr34=nullptr;return true;
}
static bool InstallCrossbowNativeXR34() noexcept {
    for(const auto& h:g_crossbow_hooks_xr34)if(h)return false;
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!XR34ConfigureNative())return false;
    constexpr unsigned char start[]{0x48,0x89,0x5C,0x24,0x18,0x48,0x89,0x74,0x24,0x20,0x55,0x57,0x41,0x54,0x41,0x56};
    constexpr unsigned char metadata[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
    constexpr unsigned char stop[]{0x40,0x53,0x48,0x83,0xEC,0x40,0x48,0x8B,0x05,0x97,0x5F,0x06,0x02,0x48,0x33,0xC4};
    if(!MatchBytes(b+0x2897A14,start,sizeof(start))||!MatchBytes(b+0x289D788,metadata,sizeof(metadata))||
       !MatchBytes(b+0x289901C,stop,sizeof(stop)))return false;
    const auto install=[&](unsigned index,const char* name,std::uintptr_t rva,auto detour,auto& original){
        DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
        auto hook=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{name,
            DetourModKit::hook::Target{DetourModKit::Address{b+rva}},options},detour);
        if(!hook)return false;g_crossbow_hooks_xr34[index]=std::move(*hook);
        original=g_crossbow_hooks_xr34[index]->original<std::decay_t<decltype(original)>>();
        return original&&g_crossbow_hooks_xr34[index]->enable();
    };
    const bool ok=install(0,"kcd2vr_xr34_reload_start",0x2897A14,&XR34StartDetour,g_crossbow_start_xr34)&&
        install(1,"kcd2vr_xr34_reload_metadata",0x289D788,&XR34MetadataDetour,g_crossbow_metadata_xr34)&&
        install(2,"kcd2vr_xr34_reload_stop",0x289901C,&XR34StopDetour,g_crossbow_stop_xr34);
    if(!ok){(void)DisarmCrossbowNativeXR34();return false;}
    xr34::completionObserver.store(&XR34ExistingCompleteObserver,std::memory_order_release);
    xr34::eventObserver.store(&XR34ExistingEventObserver,std::memory_order_release);
    // This executor alone cannot register the feature. The current frik source
    // checks this hook set, scanner, finalizer and working-hand owner together.
    return true;
}
#endif
