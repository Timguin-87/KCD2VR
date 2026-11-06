// F12=7 only. Exact retail pose-input substitution inside the ORIGINAL predictor.
// See review/V66.120/NATIVE_BOUNDARY.md. No action object, damage or result writes.
using PredictionFn120=void*(__fastcall*)(void*,float);
using InterpolateFn120=QuatT28*(__fastcall*)(QuatT28*,const QuatT28*,const QuatT28*,const float*,float,float);
static PredictionFn120 g_prediction_original120=nullptr;
static InterpolateFn120 g_interpolate_original120=nullptr;
static std::optional<DetourModKit::hook::Hook> g_prediction_hook120,g_prediction_mid120,g_interpolate_hook120;
static std::atomic<std::uintptr_t> g_prediction_contract120{0};
static std::atomic<bool> g_prediction_fault120{false};
static std::optional<DetourModKit::hook::Hook> g_prediction_source_begin126,g_prediction_source_read126,g_prediction_source_end126;
#include "prediction_source126.inl"
static std::array<std::atomic<std::uint64_t>,11> g_prediction_reject120{};
static std::atomic<std::uint64_t> g_prediction_segments120{0},g_prediction_sweeps120{0};
// Count EVERY first-failing predicate, not just whichever rejection happens to
// win the once-per-second print slot. Cumulative counters survive quiet periods.
static constexpr const char* kPredictionGates122[]{"MODE_REFERENCE","RETAIL_CONTRACT_FAULT","PHYSICAL_OWNER",
    "GAMEPLAY_OWNER","TRACKING","NATIVE_BODY","COMMITTED_POSE_AGE","FRAME_MAILBOX","FRAME_ALLOWED_AGE",
    "FRAME_EPOCH","FRAME_HAND_OWNER","CONTACT_DEFERRED","CONTACT_UNKNOWN","MELEE_PROFILE","NATIVE_IDENTITY",
    "NATIVE_STATE_READ","NATIVE_ATTACK_TYPE","NATIVE_ADMISSION","HELD_ITEM_IDENTITY","NATIVE_DT"};
static std::array<std::atomic<std::uint64_t>,std::size(kPredictionGates122)> g_prediction_gates122{};
static bool PredictionRejectContext122(const char* why) noexcept {
    if(!PredictionCapture120())return false;
    for(std::size_t i=0;i<std::size(kPredictionGates122);++i)if(!std::strcmp(why,kPredictionGates122[i])){++g_prediction_gates122[i];break;}
    return false;
}
// A private INPUT RECORD, not an emulated action object. Audited RDI reads stop
// at A7C89F; RDI then becomes the real actor. R12 retains the original wrapper.
struct PredictionInputs120 {
    void* player=nullptr;
    unsigned char pad0[0x74-sizeof(void*)]{};
    QuatT28 target{};
    unsigned char pad1[0x154-0x74-sizeof(QuatT28)]{};
    float direction[3]{};
    unsigned char pad2[0x16C-0x160]{};
    QuatT28 previous{};
};
static_assert(offsetof(PredictionInputs120,target)==0x74&&offsetof(PredictionInputs120,direction)==0x154&&
              offsetof(PredictionInputs120,previous)==0x16C&&sizeof(PredictionInputs120)==0x188);
struct PredictionScope120 {
    PredictionInputs120 input{};
    void* wrapper=nullptr;void* action=nullptr;void* collisions=nullptr;
    int bone=-1,hand=-1;bool redirected=false,nativeBodyRequired125=false,sourceCorrelated126=false;
    double framePivotM126=-1,framePivotRad126=-1;
    unsigned char admitted147=0,enabled147=0,actionMode147=0;
    std::uint64_t entryTick147=0;
    unsigned interpolations=0,sweeps=0,correlated122=0;
    kcd2vr::native_transform166::Check inverse166{};
    double pivotDistance=-1,pivotAngle=-1;int reason=0;
    const char* context121="NOT_CHECKED";nc104::Observed state121{};
    std::uint64_t sample=0,key=0,generation=0;
    std::uint64_t epoch176=0,request176=0,frameTick176=0;
    kcd2vr::contact176::Ticket observedRelease176{};
    bool releaseIdentity176=false;
    ph::Pose authored{},expected{},desired{};
    kcd2vr::contact176::Payload authority176{};
    bool fistH31=false;std::uint64_t fistGenerationH31=0,fistPreviousTickH31=0,fistTickH31=0,fistGestureSampleH31=0;
    FistContactFrameH11 fistContactH31{}; // synchronous immutable swept occurrence, never a deferred pointer lease
    bool fistSuppressR10=false; // R10: refused while the VR fist is valid -> no native (animated bone) sweep this call
    bool fistReplayR10=false;   // R10: this sweep is the punch's remembered farthest real segment
};
static thread_local PredictionScope120* g_prediction_scope120=nullptr;
static thread_local NativeTrajectoryLease112 g_prediction_lease120;
struct PredictionScopeGuard120 {
    PredictionScope120* prior;
    explicit PredictionScopeGuard120(PredictionScope120* p):prior(g_prediction_scope120){g_prediction_scope120=p;}
    ~PredictionScopeGuard120(){g_prediction_scope120=prior;}
};
#include "fist_prediction_h31.inl"
// The original native caller supplies admission. Request preparation state is
// observed separately and is never used as a substitute for this call boundary.
static bool PredictionContext120(PhysFrame6698& f,NativeOwner104& owner,PredictionScope120& s,bool observeProtected125=false) noexcept {
    const auto now=std::uint64_t(GetTickCount64()),b=WhBase();
    const auto no=[&](const char* why){s.context121=why;return PredictionRejectContext122(why);};
    if(g_weapon_control_mode.load()!=7||diagnostics::lab::NativeReference())return no("MODE_REFERENCE");
    if(!b||!retail_build112::Ready(b)||g_prediction_contract120.load()!=b||g_prediction_fault120.load())return no("RETAIL_CONTRACT_FAULT");
    if(!g_phys_armed6698.load()||g_phys_fault6698.load()||g_native_fault104.load()||FistSelected101())return no("PHYSICAL_OWNER");
    if(!g_gameplay_allowed.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_menu.load())return no("GAMEPLAY_OWNER");
    if(!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load())return no("TRACKING");
    s.nativeBodyRequired125=animation_filter::nativeBodyOwned112.load()||animation_filter::PlayerNativeBodyRequiredForMotion122();
    if(s.nativeBodyRequired125&&!observeProtected125)return no("NATIVE_BODY");
    const auto tick=animation_filter::combatPoseTick112.load();
    if(!tick||now<tick||now-tick>80)return no("COMMITTED_POSE_AGE");
    if(!g_phys_frame6698.read(f))return no("FRAME_MAILBOX");
    if(PhysicalCombat177Enabled()){
        const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
        TrackingSnapshot tracking{};
        if(!ready||f.input.sample!=ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||
           ready!=g_phys_ready_sample6698.load(std::memory_order_acquire))return no("FRAME_MAILBOX");
        if(!f.input.physicalCombat177||!ReadTrackingSnapshot(tracking)||!tracking.allowed||
           !tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
           tracking.head_held||tracking.right_held||tracking.left_held)return no("TRACKING");
    }
    if(!f.input.allowed||!f.input.attacks||!f.input.sample||now<f.input.tick||now-f.input.tick>80)return no("FRAME_ALLOWED_AGE");
    if(f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load())return no("FRAME_EPOCH");
    if(!f.input.tracked[0]||!f.input.tracked[1]||!f.output.active[0]||!f.output.owned[0])return no("FRAME_HAND_OWNER");
    // The physics owner already publishes the tested prefix, never the untested
    // desired endpoint. Deferral alone must not discard that safe applied motion.
    if(!f.output.contactKnown[0]&&!(f.output.sweepDeferred115[0]&&f.output.contactPrefixKnown126[0]))
        return no(f.output.sweepDeferred115[0]?"CONTACT_DEFERRED":"CONTACT_UNKNOWN");
    if(!f.profiles[0].primary||!f.profiles[0].melee())return no("MELEE_PROFILE");
    if(!NativeResolve104(f.player,owner))return no("NATIVE_IDENTITY");
    if(!NativeObserve104(owner,s.state121))return no("NATIVE_STATE_READ");
    if(s.state121.type!=0&&s.state121.type!=1)return no("NATIVE_ATTACK_TYPE");
    void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    if(!Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||
       manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
       !NativeHandKey6697(manager,0,key)||!key||key!=f.profiles[0].key)return no("HELD_ITEM_IDENTITY");
    s.context121="PASS";return true;
}
static bool PredictionPrepare120(void* wrapper,float dt,std::uintptr_t caller,PredictionScope120& s) noexcept {
    kcd2vr::fist_action_h12::Binding fistH31{};int currentFistTypeH31=-1;
    if(FistSelected101()&&kcd2vr::fist_action_h12::current(fistH31,GetTickCount64())&&
       ClockRead50(fistH31.intent.state+0x218,&currentFistTypeH31,4)&&kcd2vr::fist_action_h12::punch(currentFistTypeH31))
        return FistPredictionPrepareH31(wrapper,dt,caller,s);
    auto& lease=g_prediction_lease120;
    const auto reject=[&](int r){s.reason=r;if(PredictionCapture120())++g_prediction_reject120[r];lease={};return false;};
    ReadScope101R2 reads;PhysFrame6698 f{};NativeOwner104 owner{};
    if(caller!=WhBase()+0xA7CDEE)return reject(1);
    if(!std::isfinite(dt)||dt<=0||dt>.05f){s.context121="NATIVE_DT";PredictionRejectContext122(s.context121);return reject(2);}
    if(!PredictionContext120(f,owner,s,true))return reject(2);
    s.sample=f.input.sample;s.key=f.profiles[0].key;s.generation=f.profiles[0].generation;
    s.epoch176=f.input.bodyEpoch;s.request176=f.request;s.frameTick176=f.input.tick;
    // Copied observation only. This record never grants admission or replaces
    // the gameplay ticket independently captured after trajectory qualification.
    if(CombatExperiment175Enabled()&&PredictionCapture120()){
        auto& t=s.observedRelease176;(void)kcd2vr::contact176::released.read(t);
        s.releaseIdentity176=t.serial&&t.player==reinterpret_cast<std::uintptr_t>(owner.player)&&
            t.entity==reinterpret_cast<std::uintptr_t>(owner.entity)&&t.epoch==s.epoch176&&
            t.request==s.request176&&t.key==s.key&&t.generation==s.generation;
    }
    unsigned char* action=nullptr;void* player=nullptr;void* back=nullptr;unsigned char hit=1;
    std::int16_t bone=-1;int hand=-1;QuatT28 authored{},world{},inverse{};
    if(!Read(wrapper,action)||!action||!Read(action,player)||player!=owner.player||
       !Read(static_cast<unsigned char*>(owner.actor)+0x218,s.collisions)||!s.collisions||
       !Read(static_cast<unsigned char*>(s.collisions)+8,back)||back!=owner.actor||
       !Read(action+0x308,hit)||hit||!Read(action+0xC,bone)||bone<0||!Read(action+0x40C,hand)||
       !PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0xAC),authored)||
       !PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0x20),world)||
       !PhysCopySweep112(reinterpret_cast<QuatT28*>(action+0x3C),inverse))return reject(3);
    s.wrapper=wrapper;s.action=action;s.bone=bone;s.hand=hand;
    s.entryTick147=std::uint64_t(GetTickCount64());
    if(!Read(static_cast<unsigned char*>(wrapper)+0x10,s.admitted147)||
       !Read(action+0x411,s.enabled147)||!Read(action+0x2D0,s.actionMode147)||
       !native_admission147::predictor(s.admitted147,s.enabled147,s.actionMode147,hit,s.state121.type)){
        s.context121="NATIVE_ADMISSION";PredictionRejectContext122(s.context121);return reject(10);
    }
    if(PredictionCapture120())PublishContactIdentity114(owner.player,owner.actor,owner.entity);
    CommittedFrame6695 body{};PhysSocketBank101 sockets{};
    const bool haveLiveSockets=g_phys_sockets101.read(sockets); // optional asynchronous evidence only
    if(!g_committed_frame6695.read(body)||body.stamp.epoch!=f.input.bodyEpoch||body.stamp.sample!=f.input.sample||
       !body.valid[0]||!std::isfinite(f.world[7])||f.world[7]<.5f||f.world[7]>2.f)return reject(4);
    const auto& p=f.profiles[0];const auto& capture=f.correlation126[0];
    const auto* socket=&capture.socket;const auto now=std::uint64_t(GetTickCount64());
    // Profile, native weapon and socket come from ONE committed pose transaction.
    // A newer attachment callback must not invalidate an otherwise current frame.
    // Compare topology only when the live mailbox still carries the SAME callback.
    const bool sameCapture=capture.stamp.epoch==body.stamp.epoch&&capture.stamp.sample==body.stamp.sample&&
        capture.stamp.actor==body.stamp.actor&&capture.stamp.tick==body.stamp.tick;
    bool topology=true;
    if(socket->callback&&haveLiveSockets)for(const auto& v:sockets.sockets)if(v.valid&&v.key==p.key&&v.generation==p.generation&&v.joint==bone&&
        v.character==socket->character&&v.callback==socket->callback){
        topology=!std::memcmp(&v.relative,&socket->relative,sizeof(v.relative))&&!std::memcmp(v.scale,socket->scale,sizeof(v.scale));break;}
    if(!sameCapture||!socket->valid||socket->key!=p.key||socket->generation!=p.generation||socket->joint!=bone||
       reinterpret_cast<std::uintptr_t>(socket->character)!=body.stamp.actor||now<socket->tick||now-socket->tick>100||
       !topology||!p.nativeSeed){
        s.context121=!sameCapture?"CAPTURE_STAMP":!topology?"SOCKET_TOPOLOGY_SAME_CALLBACK":"CAPTURE_SOCKET";return reject(5);
    }
    const auto actual=PhysPose6698(authored),w=PhysPose6698(world),inv=PhysPose6698(inverse);
    // V66.136: re-base BOTH the expected native weapon and the tracked VR weapon on
    // the predictor's OWN current character world (+0x20), not on the committed
    // pose frame's world. The V135R1 headset log rejected every F12=7 attempt on
    // reasons 7/6: the two worlds are sampled at different times, so any walking
    // or turning exceeded the 1.5 cm / 0.04 rad gates. Same-time worlds remove
    // that skew; tracked weapon pose relative to Henry is unchanged.
    const auto toWorld=[&](ph::Pose weapon){auto joint=ph::compose(weapon,ph::inverse(PhysPose6698(socket->relative)));
        joint.p=joint.p*f.world[7];return ph::compose(w,joint);};
    auto expected=toWorld(p.nativeWeapon);const auto desired=toWorld(f.output.weapons[0]);
    s.framePivotM126=pm::length(expected.p-actual.p);s.framePivotRad126=pm::angle(expected.q,actual.q);
    ph::Pose sourceExpected{};
    s.sourceCorrelated126=PredictionSourceMatches126(action,bone,f,*socket,actual,w,sourceExpected);
    if(s.sourceCorrelated126)expected=sourceExpected;
    s.authored=actual;s.expected=expected;s.desired=desired;
    s.pivotDistance=pm::length(expected.p-actual.p);s.pivotAngle=pm::angle(expected.q,actual.q);
    // A protected/unknown FIFO may be OBSERVED to diagnose the missing pivot
    // correlation, but can never acquire a lease or install borrowed inputs.
    if(s.nativeBodyRequired125){s.context121="NATIVE_BODY";PredictionRejectContext122(s.context121);return reject(2);}
    if(!ph::finite(expected)||!ph::finite(desired)||s.pivotDistance>.03||s.pivotAngle>.08)return reject(6);
    // A7CEE0 writes +20/+3C as world/inverse; callback 1E58E70 uses +3C.
    s.inverse166=kcd2vr::native_transform166::InversePair(w,inv);
    // `w` is the predictor's current character world while f.world belongs to
    // the earlier committed skinning packet. Comparing their positions/angles
    // is therefore a temporal-skew test, not an identity test. Identity is already
    // proven above by exact Henry/player/manager/item/body-epoch chains. Keep the
    // native world/inverse self-consistency check tight instead of replacing the
    // stale comparison with a dangerous 30 cm / 20 degree allowance.
    if(!s.inverse166.valid)return reject(7);
    const auto lab=diagnostics::lab::Snapshot();
    const bool same=lease.producer==wrapper&&lease.action==action&&lease.actor==owner.actor&&lease.lab==lab&&
        lease.epoch==body.stamp.epoch&&lease.key==p.key&&lease.generation==p.generation&&lease.request==f.request&&
        lease.bone==bone&&lease.hand==hand;
    ph::Pose from{};
    if(PhysicalCombat177Enabled()){
        // The native action/window and current pivot have ALREADY passed every
        // identity/correlation check above. Two completed producer poses replace
        // three warm-up native callbacks; there is no replay of a missed swing.
        // V66.178R6: 125 ms / 0.8 m / 2.2 rad instead of 50 ms / 0.5 m / 1.2 rad. The headset
        // steps at 18-30 fps; a committed cut moves 70-145 deg between two completed
        // producer poses there, and the old bounds rejected every real swing (reject 8).
        const double pairSeconds59c6=kcd2vr::motion_time::elapsed(f.previousTick177,f.input.tick,f.previousNs59c6,f.input.timeNs);
        if(hand!=0||!f.previousSample177||!f.previousTick177||f.input.tick<f.previousTick177||
           pairSeconds59c6<=0||pairSeconds59c6>.125||f.input.sample<=f.previousSample177)return reject(8);
        from=toWorld(f.previousWeapon177); // same current native world/socket for both endpoints
        if(!ph::finite(from)||pm::length(desired.p-from.p)>.8||pm::angle(desired.q,from.q)>2.2)return reject(8);
        if(same&&f.input.sample<lease.sample)return reject(8);
        if(same&&f.input.sample==lease.sample)from=desired; // duplicate callback = zero interval
        lease={};lease.producer=wrapper;lease.action=action;lease.actor=owner.actor;lease.lab=lab;
        lease.epoch=body.stamp.epoch;lease.key=p.key;lease.generation=p.generation;lease.request=f.request;
        lease.bone=bone;lease.hand=hand;lease.sample=f.input.sample;lease.tick=f.input.tick;
        lease.last=desired;lease.correlated=2;s.correlated122=2;s.context121="PASS_COMPLETED_PAIR177";
    }else{
    if(!same){lease={};lease.producer=wrapper;lease.action=action;lease.actor=owner.actor;lease.lab=lab;
        lease.epoch=body.stamp.epoch;lease.key=p.key;lease.generation=p.generation;lease.request=f.request;
        lease.bone=bone;lease.hand=hand;lease.sample=f.input.sample;lease.tick=f.input.tick;
        lease.last=desired;lease.correlated=1;s.correlated122=1;s.reason=9;if(PredictionCapture120())++g_prediction_reject120[9];return false;}
    if(f.input.sample<lease.sample||f.input.tick<lease.tick||f.input.tick-lease.tick>125|| // V66.178R6: 18-30 fps
       pm::length(desired.p-lease.last.p)>.8||pm::angle(desired.q,lease.last.q)>2.2)return reject(8);
    const bool fresh=f.input.sample>lease.sample&&f.input.tick>lease.tick;
    from=fresh?lease.last:desired;
    if(fresh&&lease.correlated<3)++lease.correlated;
    lease.sample=f.input.sample;lease.tick=f.input.tick;lease.last=desired;
    s.correlated122=lease.correlated;
    // V147: A7CD4C has already admitted THIS native action and enabled THIS
    // wrapper for this call. The request candidate at state+2D8 normally clears
    // during execution. Neither it nor a mod input ticket grants this lease.
    s.context121="PASS_NATIVE_CALLER";
    if(lease.correlated<3){s.reason=9;if(PredictionCapture120())++g_prediction_reject120[9];return false;}
    }
    s.input.player=player;s.input.previous=PhysQuat6698(from);s.input.target=PhysQuat6698(desired);
    // The retail loop ignores translation and rotation about its Z axis. This
    // local direction is ONLY subdivision metadata; the interpolation below
    // supplies the full tracked quaternion and translation. At most one native
    // step under our 1.2 rad bound (retail step is 2.094395 rad). Duplicate
    // samples submit zero motion, never replay the preceding tracked segment.
    const auto z=pm::rotate(desired.q,{0,0,-1});
    const auto axis=*pm::unit(pm::cross(z,std::abs(z.z)<.8?pm::Vec3{0,0,1}:pm::Vec3{0,1,0}));
    const auto direction=pm::rotate(pm::from_rotation_vector(axis*std::max(.01,pm::angle(from.q,desired.q))),z);
    s.input.direction[0]=float(direction.x);s.input.direction[1]=float(direction.y);s.input.direction[2]=float(direction.z);
    // V176 F4 admission metadata is independent of trace164 / recording. This
    // is a completed coherent pose and a physical release, not hit acceptance.
    if(CombatExperiment175Enabled()){
        auto& a=s.authority176;
        a.identity={reinterpret_cast<std::uintptr_t>(owner.player),reinterpret_cast<std::uintptr_t>(owner.entity),
            reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(s.collisions),
            f.input.bodyEpoch,f.request,p.key,p.generation,f.input.sample,f.input.tick};
        (void)kcd2vr::contact176::released.read(a.ticket);
        a.wrapper=reinterpret_cast<std::uintptr_t>(wrapper);a.action=reinterpret_cast<std::uintptr_t>(action);
        a.hand=hand;a.bone=bone;a.dt=dt;
        a.moving=pm::length(desired.p-from.p)>1.e-6||pm::angle(desired.q,from.q)>1.e-6;
    }
    return true;
}
static bool F4DefenseOwned176(void* player) noexcept {
    return NativePhysicalGuardHeld117(player)||(g_guard_manual_owner112.load()==player&&g_guard_manual112.load());
}
static bool CurrentF4ContactLive176(kcd2vr::contact176::Live& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64()),b=WhBase();
    if(!CombatExperiment175Enabled()||g_weapon_control_mode.load()!=7||!b||!retail_build112::Ready(b)||
       g_prediction_contract120.load()!=b||g_prediction_fault120.load()||!g_phys_armed6698.load()||
       g_phys_fault6698.load()||g_native_fault104.load()||!g_gameplay_allowed.load()||
       g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_menu.load()||
       animation_filter::nativeBodyOwned112.load()||FistSelected101())return false;
    TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held)return false;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.input.allowed||!f.input.attacks||!f.input.tracked[0]||!f.input.tracked[1]||
       !f.output.active[0]||!f.output.owned[0]||!f.profiles[0].primary||!f.profiles[0].melee()||
       !kcd2vr::contact176::fresh(f.input.tick,now,150)||f.request!=g_weapon_request_epoch6694.load()|| // V66.178R6: 150 ms (18-30 fps)
       f.input.bodyEpoch!=g_pose_epoch6689.load())return false;
    ReadScope101R2 reads;NativeOwner104 owner{};nc104::Observed state{};
    void* collisions=nullptr;void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    if(!NativeResolve104(f.player,owner)||!NativeObserve104(owner,state)||(state.type!=0&&state.type!=1)||
       state.blocking||F4DefenseOwned176(owner.player)||
       !Read(static_cast<unsigned char*>(owner.actor)+0x218,collisions)||!PhysVft6698(collisions,0x3B91300)||
       !Read(static_cast<unsigned char*>(collisions)+8,back)||back!=owner.actor||
       !Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||
       manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
       !NativeHandKey6697(manager,0,key)||!key||key!=f.profiles[0].key)return false;
    out.identity={reinterpret_cast<std::uintptr_t>(owner.player),reinterpret_cast<std::uintptr_t>(owner.entity),
        reinterpret_cast<std::uintptr_t>(owner.actor),reinterpret_cast<std::uintptr_t>(collisions),
        f.input.bodyEpoch,f.request,key,f.profiles[0].generation,f.input.sample,f.input.tick};
    (void)kcd2vr::contact176::released.read(out.ticket);out.now=now;return true;
}
static kcd2vr::contact176::Payload PredictionAuthority176(std::uintptr_t caller,void* collisions,int hand,int bone,float dt) noexcept {
    const auto* s=g_prediction_scope120;
    if(!s||!s->redirected||caller!=WhBase()+0xA7C9C1||
       collisions!=s->collisions||hand!=s->hand||bone!=s->bone)return {};
    if(s->fistH31){
        // The original native shape query consumes only the current committed
        // slot transform. No replay, changed primitive, or guessed palm radius.
        if(s->fistReplayR10||!s->interpolations||!s->sweeps||!std::isfinite(dt)||dt<=0||dt>.05f)return {};
        kcd2vr::contact176::Payload a{};const auto& f=s->fistContactH31;
        a.identity.player=reinterpret_cast<std::uintptr_t>(f.player);a.identity.collisions=reinterpret_cast<std::uintptr_t>(collisions);
        a.identity.epoch=f.epoch;a.identity.request=f.request;a.identity.sample=f.sample;a.identity.tick=f.tick;a.identity.generation=s->fistGenerationH31;
        a.wrapper=reinterpret_cast<std::uintptr_t>(s->wrapper);a.action=reinterpret_cast<std::uintptr_t>(s->action);
        a.hand=hand;a.bone=bone;a.dt=dt;a.redirected=true;a.nativeFistShapeH31=true;return a;
    }
    if(!CombatExperiment175Enabled()||dt!=s->authority176.dt)return {};
    auto a=s->authority176;a.redirected=true;return a;
}
static void PredictionMid120(DetourModKit::hook::MidContext& ctx) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::PredictionMid};
    using namespace DetourModKit::hook;
    auto* s=g_prediction_scope120;
    if(!s||s->redirected||gpr(ctx,Gpr::Rdi)!=reinterpret_cast<std::uintptr_t>(s->action)||
       gpr(ctx,Gpr::R12)!=reinterpret_cast<std::uintptr_t>(s->wrapper))return;
    gpr(ctx,Gpr::Rdi)=reinterpret_cast<std::uintptr_t>(&s->input);s->redirected=true;
    diagnostics::trace164::current.redirected=true;
}
static QuatT28* PredictionInterpolate120(std::uintptr_t caller,QuatT28* out,const QuatT28* from,
    const QuatT28* to,const float* axis,float angle,float fraction) noexcept {
    auto* result=g_interpolate_original120?g_interpolate_original120(out,from,to,axis,angle,fraction):nullptr;
    auto* s=g_prediction_scope120;
    if(!s||!s->redirected||caller!=WhBase()+0xA7C96D)return result;
    // Contract pins the caller-owned output, exact pointers and fraction. A bad
    // fraction cannot move geometry: use the previous pose and latch OFF.
    if(result!=out||!out||from!=&s->input.previous||to!=&s->input.target||
       !std::isfinite(fraction)||fraction<0||fraction>1.00001f){
        g_prediction_fault120=true;if(out)*out=s->input.previous;return out?out:result;
    }
    *out=PhysQuat6698(ph::blend(PhysPose6698(*from),PhysPose6698(*to),std::clamp(double(fraction),0.0,1.0)));
    ++s->interpolations;return result;
}
static QuatT28* __fastcall PredictionInterpolateDetour120(QuatT28* out,const QuatT28* from,const QuatT28* to,
    const float* axis,float angle,float fraction) noexcept {
    return PredictionInterpolate120(reinterpret_cast<std::uintptr_t>(_ReturnAddress()),out,from,to,axis,angle,fraction);
}
static bool PredictionSweep120(std::uintptr_t caller,void* collisions,int hand,int bone) noexcept {
    auto* s=g_prediction_scope120;
    if(!s||!s->redirected||caller!=WhBase()+0xA7C9C1||s->collisions!=collisions||s->hand!=hand||s->bone!=bone)return false;
    ++s->sweeps;if(PredictionCapture120())++g_prediction_sweeps120;return true;
}
// F3-only native-action observation. A TLS fingerprint avoids shared mutable
// stage state; one atomic slot caps all threads at 40 records/sec. Sampling or
// dropped transitions are never gameplay authority or proof of clip completion.
struct PredictionStageObserver176 {
    std::array<std::uint64_t,18> last{};
    std::uint64_t session=0,lastTick=0;bool have=false;
};
static thread_local PredictionStageObserver176 g_prediction_stage176;
static std::atomic<std::uint64_t> g_prediction_stage_next176{0};
static void ObservePredictionStage176(const PredictionScope120& s,bool ready,bool nativeNonNull) noexcept {
    if(!CombatExperiment175Enabled()||!PredictionCapture120())return;
    const auto now=std::uint64_t(GetTickCount64());
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    auto& observer=g_prediction_stage176;const auto& t=s.observedRelease176;
    const auto age=[&](std::uint64_t tick){return tick&&now>=tick?now-tick:UINT64_MAX;};
    const bool ticketPolicy=t.serial&&t.policy==kcd2vr::contact176::policy.load(std::memory_order_acquire);
    const bool ticketAge=t.serial&&age(t.tick)<=kcd2vr::contact176::kReleaseSafetyCapMs;
    const std::array<std::uint64_t,18> key{{reinterpret_cast<std::uintptr_t>(s.wrapper),reinterpret_cast<std::uintptr_t>(s.action),
        s.epoch176,s.request176,s.key,s.generation,t.serial,std::uint64_t(s.state121.phase),std::uint64_t(s.state121.type),
        std::uint64_t(s.state121.input),std::uint64_t(s.state121.zone),std::uint64_t(s.reason),
        reinterpret_cast<std::uintptr_t>(s.context121),std::uint64_t(s.admitted147)|(std::uint64_t(s.enabled147)<<8)|(std::uint64_t(s.actionMode147)<<16),
        std::uint64_t(ready)|(std::uint64_t(s.redirected)<<1)|(std::uint64_t(nativeNonNull)<<2),
        std::uint64_t(s.releaseIdentity176)|(std::uint64_t(ticketPolicy)<<1)|(std::uint64_t(ticketAge)<<2),
        std::uint64_t(s.nativeBodyRequired125)|(std::uint64_t(s.sourceCorrelated126)<<1),
        std::uint64_t(s.interpolations!=0)|(std::uint64_t(s.sweeps!=0)<<1)}};
    if(observer.have&&observer.session==session&&observer.last==key&&now>=observer.lastTick&&now-observer.lastTick<500)return;
    auto due=g_prediction_stage_next176.load(std::memory_order_relaxed);
    if(now<due||now>UINT64_MAX-25||!g_prediction_stage_next176.compare_exchange_strong(due,now+25,std::memory_order_acq_rel))return;
    if(!PredictionCapture120())return; // expiry/off during the copied observation
    observer={key,session,now,true};
    const double deltaM=ready?pm::length(PhysPose6698(s.input.target).p-PhysPose6698(s.input.previous).p):-1;
    const double deltaRad=ready?pm::angle(PhysPose6698(s.input.target).q,PhysPose6698(s.input.previous).q):-1;
    const double authoredToVrM=s.pivotDistance>=0?pm::length(s.desired.p-s.authored.p):-1;
    const double authoredToVrRad=s.pivotDistance>=0?pm::angle(s.desired.q,s.authored.q):-1;
    char text[1536]{};std::snprintf(text,sizeof(text),
        "V66.176 F4_ATTACK_STAGE tick=%llu epoch=%llu request=%llu key=%016llX generation=%llu sample=%llu poseAgeMs=%llu release=%llu releaseSample=%llu releaseTick=%llu releaseAgeMs=%llu releaseIdentity=%d releasePolicy=%d releaseWithin120ms=%d phase=%d type=%d input=%d executedZone=%d wrapper=%p action=%p admitted=%u enabled=%u actionMode=%u ready=%d redirected=%d reason=%d context=%s interpolations=%u sweeps=%u vrDeltaM=%.5f vrDeltaRad=%.5f authoredToVrM=%.5f authoredToVrRad=%.5f nativeBody=%d sourceCorrelated=%d nativeResultNonNull=%d motionOwner=COMPLETED_VR_WHEN_QUALIFIED nativeActionWindow=RETAINED enqueueSuppressionRequested=%d independentAttackState=UNPROVEN damage=UNCLAIMED stateTiming=BEFORE_NATIVE resultTiming=AFTER_NATIVE sampling=CHANGE_OR_500MS_MAX40HZ",
        (unsigned long long)now,(unsigned long long)s.epoch176,(unsigned long long)s.request176,(unsigned long long)s.key,
        (unsigned long long)s.generation,(unsigned long long)s.sample,(unsigned long long)age(s.frameTick176),
        (unsigned long long)t.serial,(unsigned long long)t.sample,(unsigned long long)t.tick,(unsigned long long)age(t.tick),
        s.releaseIdentity176,ticketPolicy,ticketAge,s.state121.phase,s.state121.type,s.state121.input,s.state121.zone,
        s.wrapper,s.action,unsigned(s.admitted147),unsigned(s.enabled147),unsigned(s.actionMode147),ready,s.redirected,s.reason,
        s.context121,s.interpolations,s.sweeps,deltaM,deltaRad,authoredToVrM,authoredToVrRad,s.nativeBodyRequired125,
        s.sourceCorrelated126,nativeNonNull,diagnostics::lab::Get(diagnostics::lab::Key::F3)==2);
    diagnostic_trace::Write(text);
}
static void* PredictionRun120(void* wrapper,float dt,std::uintptr_t caller) noexcept {
    PredictionScopeGuard120 noInheritance(nullptr);
    kcd2vr::contact176::SweepScope noAuthority176; // nested predictors cannot inherit another sweep
    diagnostics::trace164::Scope trace164;
    const auto native=[&](){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::Prediction};return g_prediction_original120?g_prediction_original120(wrapper,dt):nullptr;};
    if(g_weapon_control_mode.load()!=7){g_prediction_lease120={};return native();}
    // Three guarded pointer reads reject unrelated actors BEFORE pose mailboxes
    // or native state inspection. NPC calls must not reset Henry's warmup lease.
    {ReadScope101R2 reads;void* action=nullptr;void* player=nullptr;void* henry=nullptr;
     auto* manager=static_cast<unsigned char*>(g_henry_item_attach_manager.load());
     if(!manager||!Read(wrapper,action)||!action||!Read(action,player)||
        !Read(manager+0x38,henry)||!player||player!=henry)return native();}
    PredictionScope120 s{};
    const bool ready=PredictionPrepare120(wrapper,dt,caller,s);
    // Nested unrelated predictions cannot inherit Henry's borrowed inputs.
    PredictionScopeGuard120 scope(ready?&s:nullptr);
    if(PredictionCapture120()){
        auto& c=diagnostics::trace164::current;c.prediction=diagnostics::trace164::serial();
        c.sample=s.sample;c.key=s.key;c.generation=s.generation;c.collisions=reinterpret_cast<std::uintptr_t>(s.collisions);
    }
    // R10: while the VR fist is valid the animated fist may not hit in its place (VrFistVrOnlyHits).
    const bool suppressR10=!ready&&s.fistSuppressR10;
    if(suppressR10)kcd2vr::fist_reach_r10::suppressed.fetch_add(1,std::memory_order_relaxed);
    void* result=suppressR10?nullptr:native();
    if(ready){if(s.redirected){if(PredictionCapture120())++g_prediction_segments120;}else g_prediction_fault120=true;}
    ObservePredictionStage176(s,ready,result!=nullptr);
    // Bounded aggregate reason diagnostics, no per-NPC polling or scans.
    if(PredictionCapture120()){
        static std::atomic<std::uint64_t> next{0};const auto now=std::uint64_t(GetTickCount64());auto due=next.load();
        if(now>=due&&next.compare_exchange_strong(due,now+1000)){
            // Read-only witness AFTER original returns. It is deliberately not
            // admission proof for the earlier call; observationTick labels it.
            ReadScope101R2 witnessReads;PhysFrame6698 witness{};NativeOwner104 witnessOwner{};nc104::Observed observed{};
            const bool gotFrame=g_phys_frame6698.read(witness);
            const bool gotOwner=gotFrame&&NativeResolve104(witness.player,witnessOwner);
            const bool gotState=gotOwner&&NativeObserve104(witnessOwner,observed);
            const auto age=[&](std::uint64_t t){return t&&now>=t?now-t:UINT64_MAX;};
            char counts[1100]{};std::size_t used=0;
            for(std::size_t i=0;i<std::size(kPredictionGates122);++i){const auto n=g_prediction_gates122[i].load();if(!n)continue;
                const auto wrote=std::snprintf(counts+used,sizeof(counts)-used,"%s%s:%llu",used?",":"",kPredictionGates122[i],(unsigned long long)n);
                if(wrote<0||std::size_t(wrote)>=sizeof(counts)-used)break;used+=std::size_t(wrote);}
            char evidence[1900]{};std::snprintf(evidence,sizeof(evidence),
                "V66.122 PREDICTION_WITNESS observationTick=%llu dt=%.6f frameRead=%d ownerRead=%d stateRead=%d poseAgeMs=%llu frameAgeMs=%llu frameAllowed=%d attacks=%d handActive=%d handOwned=%d contactKnown=%d deferred=%d releaseReason=%u bodyEpoch=%llu currentEpoch=%llu native={phase:%d type:%d input:%d hand:%d requested:%d requestCandidate:%d blocking:%d} gates={%s} stateTiming=AFTER_NATIVE_RETURN correlation=PRE_STRIKE_READ_ONLY writeRequiresNativeCallerAdmission=1",
                (unsigned long long)now,double(dt),gotFrame,gotOwner,gotState,(unsigned long long)age(animation_filter::combatPoseTick112.load()),
                (unsigned long long)age(witness.input.tick),witness.input.allowed,witness.input.attacks,witness.output.active[0],witness.output.owned[0],
                witness.output.contactKnown[0],witness.output.sweepDeferred115[0],witness.output.releaseReason121[0],
                (unsigned long long)witness.input.bodyEpoch,(unsigned long long)g_pose_epoch6689.load(),observed.phase,observed.type,observed.input,
                observed.hand,observed.requested,observed.requestCandidate,observed.blocking,counts);diagnostic_trace::Write(evidence);
            char entry147[640]{};std::snprintf(entry147,sizeof(entry147),
                "V66.147 PREDICTION_ENTRY tick=%llu wrapper=%p action=%p nativeAdmission=%u actionEnabled=%u actionMode=%u phase=%d type=%d requestCandidate=%d sample=%llu key=%016llX generation=%llu ready=%d reason=%d stateTiming=BEFORE_NATIVE_CALL acceptedDamage=UNCLAIMED",
                (unsigned long long)s.entryTick147,s.wrapper,s.action,unsigned(s.admitted147),unsigned(s.enabled147),unsigned(s.actionMode147),
                s.state121.phase,s.state121.type,s.state121.requestCandidate,(unsigned long long)s.sample,(unsigned long long)s.key,(unsigned long long)s.generation,ready,s.reason);
            diagnostic_trace::Write(entry147);
            char m[1500]{};std::snprintf(m,sizeof(m),
                "V66.120 PREDICTION traceId=%llu F12=7 ready=%d redirected=%d reason=%d pivotM=%.6f pivotRad=%.6f interpolation=%u sweeps=%u totalSegments=%llu totalSweeps=%llu fault=%d rejectCaller=%llu context=%llu owner=%llu frame=%llu socket=%llu pivot=%llu transform=%llu discontinuity=%llu warmup=%llu nativeResult=%d contextGate=%s state={phase:%d type:%d requested:%d requestCandidate:%d} rejectAdmission=%llu correlated=%u sourceCorrelated=%d framePivotM=%.6f framePivotRad=%.6f inverseErrorM=%.6f inverseAngle=%.6f qNormError=%.8f legacyClosureM=%.6f acceptedDamage=UNCLAIMED",
                (unsigned long long)diagnostics::trace164::current.prediction,ready,s.redirected,s.reason,s.pivotDistance,s.pivotAngle,s.interpolations,s.sweeps,
                (unsigned long long)g_prediction_segments120.load(),(unsigned long long)g_prediction_sweeps120.load(),g_prediction_fault120.load(),
                (unsigned long long)g_prediction_reject120[1].load(),(unsigned long long)g_prediction_reject120[2].load(),
                (unsigned long long)g_prediction_reject120[3].load(),(unsigned long long)g_prediction_reject120[4].load(),
                (unsigned long long)g_prediction_reject120[5].load(),(unsigned long long)g_prediction_reject120[6].load(),
                (unsigned long long)g_prediction_reject120[7].load(),(unsigned long long)g_prediction_reject120[8].load(),
                (unsigned long long)g_prediction_reject120[9].load(),result!=nullptr,s.context121,s.state121.phase,s.state121.type,s.state121.requested,s.state121.requestCandidate,
                (unsigned long long)g_prediction_reject120[10].load(),s.correlated122,s.sourceCorrelated126,s.framePivotM126,s.framePivotRad126,s.inverse166.translationError,s.inverse166.angleError,s.inverse166.normError,s.inverse166.legacyClosure);diagnostic_trace::Write(m);
            {namespace fr=kcd2vr::fist_reach_r10;char r10[200]{};std::snprintf(r10,sizeof(r10),"R10 FIST_REACH livePairs=%llu peakReplays=%llu suppressedNativeSweeps=%llu vrOnly=%d peakReplay=%d",
                (unsigned long long)fr::current.load(),(unsigned long long)fr::replayed.load(),(unsigned long long)fr::suppressed.load(),fr::vrOnlyHits.load()?1:0,fr::peakReplay.load()?1:0);diagnostic_trace::Write(r10);}
            if(PredictionCapture120()&&s.pivotDistance>=0){
                const auto& a=s.authored;const auto& e=s.expected;const auto& v=s.desired;
                std::snprintf(m,sizeof(m),
                    "V66.120 PREDICTION_CORRELATION sample=%llu key=%016llX generation=%llu bone=%d nativeHand=%d dt=%.6f authored=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) expectedSocketBone=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) allowedVRBone=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) space=WORLD nativeBodyRequired=%d inputsRedirected=%d",
                    (unsigned long long)s.sample,(unsigned long long)s.key,(unsigned long long)s.generation,s.bone,s.hand,double(dt),
                    a.p.x,a.p.y,a.p.z,a.q.x,a.q.y,a.q.z,a.q.w,e.p.x,e.p.y,e.p.z,e.q.x,e.q.y,e.q.z,e.q.w,
                    v.p.x,v.p.y,v.p.z,v.q.x,v.q.y,v.q.z,v.q.w,s.nativeBodyRequired125,s.redirected);diagnostic_trace::Write(m);
            }
        }
    }
    return result;
}
static void* __fastcall PredictionDetour120(void* wrapper,float dt) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::Prediction};
    return PredictionRun120(wrapper,dt,reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
}

#include "prediction_contract120.inl"
#include "prediction_admission_contract147.inl"
#include "prediction_source_contract126.inl"
static bool ArmPrediction120() noexcept {
    const auto b=WhBase();if(b&&g_prediction_contract120.load()==b)return !g_prediction_fault120.load();
    if(!PredictionContract120(b)||!PredictionAdmissionContract147(b)||!PredictionSourceContract126(b)||g_prediction_hook120||g_prediction_mid120||g_interpolate_hook120||
       g_prediction_source_begin126||g_prediction_source_read126||g_prediction_source_end126)return false;
    using namespace DetourModKit::hook;
    Options options{};options.fail_if_already_hooked=true;
    auto interpolation=inline_at(InlineRequest{"kcd2vr_prediction_interpolation120",Target{DetourModKit::Address{b+0xA7CA90}},options},&PredictionInterpolateDetour120);
    auto mid=mid_at(MidRequest{"kcd2vr_prediction_inputs120",Target{DetourModKit::Address{b+0xA7C65F}},options},&PredictionMid120);
    auto prediction=inline_at(InlineRequest{"kcd2vr_prediction120",Target{DetourModKit::Address{b+0xA7C624}},options},&PredictionDetour120);
    auto sourceBegin=mid_at(MidRequest{"kcd2vr_prediction_source_begin126",Target{DetourModKit::Address{b+0xA7EA5E}},options},&PredictionSourceBeginMid126);
    auto sourceRead=mid_at(MidRequest{"kcd2vr_prediction_source_read126",Target{DetourModKit::Address{b+0xA7D136}},options},&PredictionSourceReadMid126);
    auto sourceEnd=mid_at(MidRequest{"kcd2vr_prediction_source_end126",Target{DetourModKit::Address{b+0xA7EA76}},options},&PredictionSourceEndMid126);
    if(!interpolation||!mid||!prediction||!sourceBegin||!sourceRead||!sourceEnd)return false;
    g_prediction_source_begin126=std::move(*sourceBegin);g_prediction_source_read126=std::move(*sourceRead);g_prediction_source_end126=std::move(*sourceEnd);
    g_interpolate_hook120=std::move(*interpolation);g_prediction_mid120=std::move(*mid);g_prediction_hook120=std::move(*prediction);
    g_interpolate_original120=g_interpolate_hook120->original<InterpolateFn120>();
    g_prediction_original120=g_prediction_hook120->original<PredictionFn120>();
    // Publish originals before any enable. Retain installed hooks for process
    // lifetime like other motion hooks; no hot teardown of inline trampolines.
    // Partial enable is inert because the contract is published LAST.
    if(!g_interpolate_original120||!g_prediction_original120||!g_interpolate_hook120->enable()||
       !g_prediction_mid120->enable()||!g_prediction_hook120->enable()||!g_prediction_source_begin126->enable()||
       !g_prediction_source_read126->enable()||!g_prediction_source_end126->enable())return false;
    g_prediction_contract120.store(b,std::memory_order_release);return true;
}
