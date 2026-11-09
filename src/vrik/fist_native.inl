// V66.101R1: native-input prototype, not physical hit/damage authority.
// Included after physical_native: reuses exact owner/read leaves, not its weapon
// arming state. All native attack calls execute at the verified outer frame.
namespace fm=kcd2vr::fists;
namespace cc=kcd2vr::diagnostics::combat_capture;
namespace fp3=kcd2vr::diagnostics::punch_c54r3;
struct FistOwner101 {void* player=nullptr;void* manager=nullptr;std::uint64_t tick=0,epoch=0,request=0;bool empty=false,responsiveCombatH27=false;};
static bool FistCombatReadyH27(void*) noexcept;
struct FistFrame101 {fm::Sample sample{};fm::Punch punch{};void* player=nullptr;std::uint64_t request=0;fm::Phase phase[2]{};bool released[2]{};std::uint64_t diagnosticOnsetC54R3[2]{};};
static kcd2vr::core::SnapshotMailbox<FistOwner101> g_fist_owner101;
static kcd2vr::core::SnapshotMailbox<FistFrame101> g_fist_frame101;
struct FistContactFrameH11 {
    void* player=nullptr;std::uint64_t request=0,epoch=0,tick=0,sample=0,timeNsH31=0;std::uintptr_t character=0;
    pm::Vec3 palm[2]{},previous[2]{};bool valid[2]{};
    bool completedH31=false; // this exact stamp was observed after full pose commit
    ph::Pose modelWristH31[2]{},modelSlotH31[2]{},worldSlotH31[2]{},previousSlotH31[2]{}; // previous slot is ORIGINAL WORLD
    std::uintptr_t skeletonH31=0;std::uint32_t jointsH31=0;double scaleH31=0;
    int wristBoneH31[2]{-1,-1},slotBoneH31[2]{-1,-1};
    bool slotValidH31[2]{},pairH31[2]{};
    std::uint64_t previousTickH31[2]{},previousSampleH31[2]{},previousTimeNsH31[2]{},gestureSampleH31[2]{};
    ph::Vec3 gestureDirectionH31[2]{};
};
static kcd2vr::core::SnapshotMailbox<FistContactFrameH11> g_fist_contact_h11;
// R10: the farthest completed fist segment of the current punch per side (Henry's real reach), for the native window.
struct FistPeakR10 {int side=-1;kcd2vr::fist_reach_r10::Peak peak{};FistContactFrameH11 palms{};};
static kcd2vr::core::SnapshotMailbox<FistPeakR10> g_fist_peak_r10_right,g_fist_peak_r10_left;
static kcd2vr::core::SnapshotMailbox<FistPeakR10>& FistPeakMailboxR10(int side) noexcept {return side?g_fist_peak_r10_left:g_fist_peak_r10_right;}
static std::atomic<std::uint64_t> g_fist_seen101{0};
static fm::Punch g_fist_pending101{};
static std::uint64_t g_fist_motion_request101=0;
static fm::State g_fist_motion101; // committed pose producer only, protected by g_pose_mutex6689
static std::atomic<std::uint64_t> g_fist_epoch101{1};
static std::atomic<bool> g_fist_fault101{false};
static CombatInputFn101 g_combat_input_original101=nullptr;
static std::optional<DetourModKit::hook::Hook> g_combat_input_hook101;
static std::recursive_mutex g_fist_native_mutex101;
static std::atomic_flag g_fist_input_guard101=ATOMIC_FLAG_INIT;
static std::atomic<unsigned> g_fist_quarantine101{0};
static std::atomic<void*> g_fist_quarantine_owner101{nullptr};
static std::uint64_t g_fist_action_names101[3]{};
static std::atomic<bool> g_kick_r3_down101{false};
static std::atomic<void*> g_guard_manual_owner112{nullptr};
static std::atomic<void*> g_guard_physical_owner112{nullptr};
static std::atomic<bool> g_guard_manual112{false},g_guard_physical112{false};
static std::atomic<std::uint64_t> g_guard_manual_occurrence_r24{0}; // actual native block edges, not a held-bit identity
static void NativeFreeKickGuardEdgeR24(void*,int) noexcept;
// One observed external prepare, never an accepted-attack claim. The lease is
// copied before/after the original callback; no borrowed request is retained.
struct ExternalPrepare176 {
    void* input=nullptr;void* player=nullptr;void* manager=nullptr;
    void* actor=nullptr;void* state=nullptr;void* entity=nullptr;
    std::uint64_t epoch=0,key[2]{},serial=0,takeover=0;
    int equipment=-1;bool attempted=false;
};
static std::recursive_mutex g_external_prepare_mutex176;
static std::atomic<std::uint64_t> g_external_prepare_generation176{1},g_external_prepare_takeover176{1};
static std::atomic<std::uint64_t> g_external_prepare_serial176{0},g_external_prepare_observed_takeover176{0};
static ExternalPrepare176 g_external_prepare176;
static void ExternalClear176() noexcept {
    g_external_prepare_serial176.store(0,std::memory_order_release);g_external_prepare176={};
}
static bool ExternalIdentity176(ExternalPrepare176&,void*,void*,void*) noexcept;
static ExternalPrepare176 ExternalBefore176(void*,void*,void*,const char*,int,bool) noexcept;
static void ExternalAfter176(const ExternalPrepare176&) noexcept;
static void ExternalPrepareDrain176() noexcept;
static bool ExternalMustWait176(void*,unsigned) noexcept;
static void ExternalPrepareTakeover176() noexcept {g_external_prepare_takeover176.fetch_add(1,std::memory_order_acq_rel);}
static void ExternalPrepareForget176() noexcept {
    g_external_prepare_generation176.fetch_add(1,std::memory_order_acq_rel);
    std::unique_lock<std::recursive_mutex> lock(g_external_prepare_mutex176,std::try_to_lock);
    g_external_prepare_serial176.store(0,std::memory_order_release);
    if(lock.owns_lock())g_external_prepare176={};
}
static void NativeGuardRelease112(const char* reason="OWNER_CLEANUP") noexcept;
// The integrated F4 mode retains weapon/holster processing while exact empty
// hands select boxing. The native pump publishes a short-lived selection;
// callbacks never enumerate equipment or change modes/calibration.
static std::atomic<std::uint64_t> g_f4_fists_tick178r22c2{0},g_f4_fists_epoch178r22c2{0};
static std::atomic<bool> g_normal_fists44{false};
static bool FistModePermits178r22c2() noexcept {
    return g_weapon_control_mode.load()==0||(g_normal_fists44.load()&&FullWeaponMode120(g_weapon_control_mode.load()))||g_combat_experiment175.load(std::memory_order_acquire);
}
static bool FistContactRequestedH31() noexcept {
    return g_census_mode.load()==5||g_combat_experiment175.load(std::memory_order_acquire)||
        (g_normal_fists44.load()&&g_census_mode.load()>=4&&FullWeaponMode120(g_weapon_control_mode.load()));
}
static bool FistSelected101() noexcept {
    // Explicit F5=2 transfers empty-hand input to grabbing. Hand/finger contact
    // rendering stays selected, but it must not also produce punches/guards.
    if(kcd2vr::workstation178r20c1::ownsWrist(GetTickCount64()))return false;
    if(g_census_mode.load()==5)return true;
    if(!g_combat_experiment175.load(std::memory_order_acquire)&&
       !(g_normal_fists44.load()&&g_census_mode.load()>=4&&FullWeaponMode120(g_weapon_control_mode.load())))return false;
    const auto tick=g_f4_fists_tick178r22c2.load(std::memory_order_acquire),now=std::uint64_t(GetTickCount64());
    return tick&&now>=tick&&now-tick<=80&&g_f4_fists_epoch178r22c2.load()==g_pose_epoch6689.load();
}
static bool FistHandsClosedH11() noexcept {
    TrackingSnapshot t{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid||
       t.head_held||t.right_held||t.left_held||now<t.tick||now-t.tick>80)return false;
    return (t.right_grip>.60f&&t.right_trigger>.60f)||(t.left_grip>.60f&&t.left_trigger>.60f);
}
static bool CombatActionName101(const std::uint64_t* action,char* out,std::size_t cap) noexcept {
    std::uint64_t p=0;if(!Read(action,p)||!p)return false;
    for(std::size_t i=0;i+1<cap;++i){if(!Read(reinterpret_cast<void*>(p+i),out[i]))return false;if(!out[i])return true;}
    out[cap-1]=0;return false;
}
static unsigned FistActionGroup101(const char* n) noexcept {
    if(!std::strcmp(n,"prepare_to_attack")||!std::strcmp(n,"attack_primary")||!std::strcmp(n,"attack_primary_mouse"))return 1;
    if(!std::strcmp(n,"attack_special"))return 2;
    if(!std::strcmp(n,"block"))return 4;
    if(!std::strcmp(n,"attack_abort"))return 8;
    return 0;
}
static bool FistExactInput101(void* input,void*& player,void*& manager) noexcept {
    if(!PhysVft6698(input,0x3EC99A8)||!Read(static_cast<unsigned char*>(input)+0x10,player)||!PhysVft6698(player,0x3AA4B98)||
       !Read(static_cast<unsigned char*>(player)+0x9D0,manager)||!manager||manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958))return false;
    void* back=nullptr;return Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==player;
}
// F12=6 also handles crossbows/firearms. Their native trigger/reload must remain
// available: selection of the rung alone is not ownership of a melee input.
static bool WeaponMotionInput119(void* player,void* manager,std::uint64_t now) noexcept {
    PhysFrame6698 f{};
    if(!FullWeaponMode120(g_weapon_control_mode.load())||!g_phys_input6698.load()||g_phys_fault6698.load()||
       !g_phys_frame6698.read(f)||f.player!=player||f.manager!=manager||f.request!=g_weapon_request_epoch6694.load()||
       f.input.bodyEpoch!=g_pose_epoch6689.load()||!f.input.allowed||now<f.input.tick||now-f.input.tick>100)return false;
    for(int side=0;side<2;++side)if(f.output.active[side]&&(f.profiles[side].melee()||f.profiles[side].bow())){
        std::uint64_t key=0;if(NativeHandKey6697(manager,side,key)&&key&&key==f.profiles[side].key)return true;
    }
    return false;
}
static void XiOwnedCall178r13(void* input,const std::uint64_t* action,int mode,float value,void* player,unsigned group,const char* name,std::uint64_t now) noexcept; // native_input117.inl
static void __fastcall CombatInputDetour101Body(void* input,const std::uint64_t* action,int mode,float value) noexcept;
static void __fastcall CombatInputDetour101(void* input,const std::uint64_t* action,int mode,float value) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::CombatInput};
    // V66.178R22: time the whole detour; the body subtracts the game handler's own time.
    const auto t0=std::chrono::steady_clock::now();CombatInputDetour101Body(input,action,mode,value);
    kcd2vr::attack_xi178r8::detourUs+=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-t0).count());++kcd2vr::attack_xi178r8::detourCalls;
}
static void __fastcall CombatInputDetour101Body(void* input,const std::uint64_t* action,int mode,float value) noexcept {
    const auto now=std::uint64_t(GetTickCount64());void* player=nullptr;void* manager=nullptr;
    // V66.178R13: an event the XI emitter dispatched for the mod's own attack/guard/special state is never
    // suppressed here (the gesture-owned filter below exists for RAW controller input in F4).
    const bool ownedXi178r13=kcd2vr::attack_xi178r8::ownedDispatch;unsigned group178r13=0;
    const auto parryPublication176=diagnostics::parry176::publication.load(std::memory_order_acquire);
    // OnAction is an exact player input hook, not a frame-rate polling API.
    const bool interested=mode==1||mode==2||g_guard_manual112.load()||g_guard_physical112.load()||FistSelected101()||FullWeaponMode120(g_weapon_control_mode.load())||cc::active(now)||g_fist_quarantine101!=0;
    if(!interested){if(g_combat_input_original101){const auto h0=std::chrono::steady_clock::now();{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::CombatInput};g_combat_input_original101(input,action,mode,value);}kcd2vr::attack_xi178r8::handlerUs+=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-h0).count());}return;}
    // Permission validation is shared only inside this callback. Every byte of
    // the action/owner is still read anew, and the cache never crosses native calls.
    ReadScope101R2 pages;
    char name[96]{};bool suppress=false;
    if(FistExactInput101(input,player,manager)&&CombatActionName101(action,name,sizeof(name))){
        // R155: the game dispatched a hold-to-look action (any input) = its action map is enabled = the prompt is offered.
        // lb_prompt_x_r153 holds the next X press through even when the hint rows do not list the prompt. tick only.
        if(kcd2vr::lb_prompt_x_r153::Matches(name))kcd2vr::lb_prompt_x_r153::holdMapSeenMs.store(now,std::memory_order_release);
        const auto group=FistActionGroup101(name);group178r13=group;
        const bool gestureF4=F4GestureInputOwned176()&&g_gameplay_allowed.load();
        if(group==4&&!gestureF4){
            if(mode==1||mode==2)g_guard_manual_occurrence_r24.fetch_add(1,std::memory_order_acq_rel);
            g_guard_manual_owner112.store(player,std::memory_order_release);
            g_guard_manual112.store(mode!=2,std::memory_order_release);
            if(!ownedXi178r13)NativeFreeKickGuardEdgeR24(player,mode);
        }
        // Physical fists replace punch input. Manual native block and contextual
        // special stay usable, and share the physical guard's balanced lease.
        const bool authoredF4=CombatExperiment175Enabled()&&(!g_gameplay_allowed.load()||kcd2vr::vr_combat111::ClinchInput55c1(now));
        const bool consume=!ownedXi178r13&&!diagnostics::lab::NativeReference()&&!authoredF4&&(ExternalMustWait176(player,group)||(gestureF4&&group)||(FistSelected101()&&(group==1||group==8))||((group==1||group==8)&&WeaponMotionInput119(player,manager,now)));
        if(ownedXi178r13&&group){suppress=false;}
        else if(group&&!g_fist_input_guard101.test_and_set(std::memory_order_acquire)){
            if(g_fist_quarantine_owner101!=player){g_fist_quarantine_owner101=player;g_fist_quarantine101=0;}
            if(authoredF4)g_fist_quarantine101&=~group; // native UI/activity phases own their input
            suppress=!authoredF4&&(consume||(g_fist_quarantine101&group)!=0);
            if(suppress){if(mode==2)g_fist_quarantine101&=~group;else g_fist_quarantine101|=group;}
            g_fist_input_guard101.clear(std::memory_order_release);
        }else if(group&&!authoredF4&&(consume||(g_fist_quarantine_owner101.load()==player&&(g_fist_quarantine101.load()&group))))suppress=true;
        // Manual LT and physical guard are two sources of one native block.
        // A manual release must not release the still-held physical source.
        if(!ownedXi178r13&&!authoredF4&&group==4&&mode==2&&g_guard_physical_owner112.load()==player&&g_guard_physical112.load(std::memory_order_acquire))suppress=true;
        // Native press/release edges are retained exactly. Repeated holds and
        // direction-axis samples are bounded to 10Hz, and unrelated input is not dumped.
        const bool zone=!std::strcmp(name,"combat_zone_mouse_x")||!std::strcmp(name,"combat_zone_mouse_y");
        const bool reference=(cc::sourceMask.load()&(cc::CombatReference|cc::InteractionReference))!=0||cc::captureOnly168r1(now)||cc::interactionOnly168r1(now);
        bool capture=cc::active(now)&&(group||zone||reference);
        if(capture&&(zone||(mode!=1&&mode!=2))){
            static std::atomic<std::uint64_t> last[9]{};
            const int index=zone?(name[std::strlen(name)-1]=='x'?6:7):int(group); // group 0: bounded contextual holds
            auto prior=last[index].load();capture=now>=prior&&now-prior>=100&&last[index].compare_exchange_strong(prior,now);
        }
        if(capture){
            cc::Event e{};e.kind=cc::Kind::Input;e.tick=now;e.actor=reinterpret_cast<std::uintptr_t>(player);e.mode=mode;e.value=value;e.flags=suppress?1:0;e.poseEpoch=g_pose_epoch6689.load();
            TrackingSnapshot t{};if(ReadTrackingSnapshot(t)){e.sequence=t.sequence;e.grip[0]=t.right_grip;e.grip[1]=t.left_grip;e.trigger[0]=t.right_trigger;e.trigger[1]=t.left_trigger;
                e.hand[0]=t.right.x;e.hand[1]=t.right.y;e.hand[2]=t.right.z;e.hand[3]=t.left.x;e.hand[4]=t.left.y;e.hand[5]=t.left.z;
                e.trackingFlags=1u|(t.right_valid?2u:0u)|(t.left_valid?4u:0u)|(t.right_held?8u:0u)|(t.left_held?16u:0u);
            }
            (void)NativeHandKey6697(manager,0,e.key[0]);(void)NativeHandKey6697(manager,1,e.key[1]);if(g_kick_r3_down101.load())e.flags|=2;
            cc::name(e,name);cc::push(e);
        }
    }
    if(!suppress&&g_combat_input_original101){
        const auto external176=ExternalBefore176(input,player,manager,name,mode,ownedXi178r13);
        const auto h0=std::chrono::steady_clock::now();
        if(ownedXi178r13&&player)XiOwnedCall178r13(input,action,mode,value,player,group178r13,name,now);
        else{InvalidateReadCache101R2();{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::CombatInput};g_combat_input_original101(input,action,mode,value);}InvalidateReadCache101R2();}
        {const auto us=std::uint64_t(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-h0).count());kcd2vr::attack_xi178r8::handlerUs+=us;kcd2vr::attack_xi178r8::recordDispatch(name,mode,us);}
        ExternalAfter176(external176);
        // V176: observe a completed, unsuppressed manual edge. This does not
        // manufacture a perfect block or run when the native press was consumed.
        if(player&&mode==1&&!std::strcmp(name,"block"))NativeManualBlockEdge176(player,now,parryPublication176);
    }
}
static bool ArmCombatInput101(bool framePump) noexcept {
    if(g_fist_fault101.load())return false;
    const auto base=WhBase();
    if(g_combat_contract101.load()!=base||!base){
        if(!SupportedWhBuild(base))return false;
        const unsigned char expected[16]{0x48,0x89,0x5C,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x33,0xDB,0x48,0x8B,0xF9,0x48};
        unsigned char actual[16]{};if(!Read(reinterpret_cast<void*>(base+0x8E5AEC),actual)||std::memcmp(actual,expected,16))return false;
        g_combat_contract101=base;
    }
    if(!g_combat_input_original101){
        if(!SupportedWhBuild(base))return false;
        const unsigned char expected[16]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x10,0x48,0x89,0x70,0x18,0xF3,0x0F,0x11,0x58,0x20};
        unsigned char actual[16]{};if(!Read(reinterpret_cast<void*>(base+0x8E5324),actual)||std::memcmp(actual,expected,16))return false;
        DetourModKit::hook::InlineRequest req{"kcd2vr_v66101_combat_input",DetourModKit::hook::Target{DetourModKit::Address{base+0x8E5324}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(req),&CombatInputDetour101);if(!h)return false;
        g_combat_input_hook101=std::move(*h);g_combat_input_original101=g_combat_input_hook101->original<CombatInputFn101>();
        if(!g_combat_input_original101||!g_combat_input_hook101->enable()){g_combat_input_hook101.reset();g_combat_input_original101=nullptr;return false;}
        if(!Read(reinterpret_cast<void*>(base+0x8E5324),g_combat_own_patch101)){
            (void)g_combat_input_hook101->disable();g_combat_input_hook101.reset();g_combat_input_original101=nullptr;return false;}
        g_combat_bypass101.store(g_combat_input_original101,std::memory_order_release);
        g_combat_hook_base101.store(base,std::memory_order_release);
    }
    g_combat_bypass101.store(g_combat_input_original101,std::memory_order_release);
    return !framePump||native_stereo_outer_lab::EnsurePhysicalFramePump6698();
}
// Cancellation must abort BEFORE clearing the controller's cancelled latch.
static bool FistDispatchPunchR19(void*,const std::uint64_t*,int,bool) noexcept;
static thread_local bool g_fist_release_call_h31=false;
static bool FistCall101(void* player,int actionIndex,int mode,bool punchR19=false,unsigned* failure=nullptr) noexcept {
    if(failure)*failure=fp3::NativeCallGuard;
    if(!retail_build112::Ready(WhBase())||!g_combat_input_original101||actionIndex<0||actionIndex>2||!PhysVft6698(player,0x3AA4B98))return false;
    void* input=nullptr;void* exact=nullptr;void* manager=nullptr;
    if(failure)*failure=fp3::NativeInputIdentity;
    if(!Read(static_cast<unsigned char*>(player)+0xB88,input)||!FistExactInput101(input,exact,manager)||exact!=player)return false;
    auto& names=g_fist_action_names101;static const char* text[]{"prepare_to_attack","attack_primary","attack_abort"};
    __try {
        if(!names[actionIndex])reinterpret_cast<void(__fastcall*)(std::uint64_t*,const char*)>(WhBase()+0x8E5AEC)(&names[actionIndex],text[actionIndex]);
        if(!names[actionIndex]){if(failure)*failure=fp3::NativeString;return false;}
        // Original trampoline bypasses only OUR raw-trigger filter. Native action
        // context, combat eligibility, stamina and hit rules all remain in control.
        if(punchR19){const bool priorH31=g_fist_release_call_h31;g_fist_release_call_h31=actionIndex==1&&mode==2;
            const bool returned=FistDispatchPunchR19(player,&names[actionIndex],mode,actionIndex==0);g_fist_release_call_h31=priorH31;
            if(failure)*failure=returned?fp3::Returned:fp3::NativeCallGuard;return returned;}
        g_combat_input_original101(input,&names[actionIndex],mode,mode==2?0.f:1.f);if(failure)*failure=fp3::Returned;return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){g_fist_release_call_h31=false;if(failure)*failure=fp3::NativeException;g_fist_fault101=true;return false;}
}
struct FistNativeState101 {void* player=nullptr;std::uint64_t request=0,epoch=0,lastSample=0,pendingTick=0,pendingSample=0,lastAttack=0;int side=-1,motion167=0;bool kickH35=false;std::uint64_t lastKickSerialH35=0;double pendingSpeed178r22c2=0;std::uintptr_t diagnosticCharacterC54R3=0;std::uint64_t diagnosticOnsetC54R3=0;bool prepared=false,owned=false,waitingGuardU16=false;fm::Punch guardPunchU16{};void* guardActorH19=nullptr;void* guardStateH19=nullptr;};
static FistNativeState101 g_fist_native101;
static bool FistAbort119(void* player) noexcept {
    // Exact retail: abort press sets controller +38 to 2. Primary release only
    // fires from 1 and resets to 0; abort release alone does not clear the latch.
    // Same verified lifecycle as PhysAbortAttack117, with the fist trampoline.
    if(!FistCall101(player,2,1))return false;
    const bool cleared=FistCall101(player,1,2);
    const bool released=FistCall101(player,2,2);
    return cleared&&released;
}
static fp3::Join FistJoinC54R3() noexcept {const auto& n=g_fist_native101;return {reinterpret_cast<std::uintptr_t>(n.player),n.diagnosticCharacterC54R3,n.request,n.epoch,n.pendingSample,n.diagnosticOnsetC54R3,n.side};}
static void FistCancel101(unsigned reason=fp3::Retired) noexcept {
    fp3::retireContext();
    auto& n=g_fist_native101;if(n.prepared&&n.player){NativeTimingCancel178r22c2();const bool aborted=FistAbort119(n.player);
        fp3::native(fp3::Stage::Cancel,reason,FistJoinC54R3(),GetTickCount64(),nullptr,aborted?1u:0u);}n.prepared=false;n.pendingSample=0;n.waitingGuardU16=false;n.guardPunchU16={};n.guardActorH19=nullptr;n.guardStateH19=nullptr;
}
static bool FistReadOwner101(FistOwner101& out,unsigned* failure=nullptr) noexcept {
    out={};out.manager=g_henry_item_attach_manager.load();void* back=nullptr;
    // Same reads/order as the original predicate, with the exact refusing leaf retained.
    if(!PhysVft6698(out.manager,0x3EC9958)){if(failure)*failure=fp3::ManagerVft;return false;}
    if(!Read(static_cast<unsigned char*>(out.manager)+0x38,out.player)){if(failure)*failure=fp3::ManagerPlayerRead;return false;}
    if(!PhysVft6698(out.player,0x3AA4B98)){if(failure)*failure=fp3::PlayerVft;return false;}
    if(!Read(static_cast<unsigned char*>(out.player)+0x9D0,back)){if(failure)*failure=fp3::PlayerManagerRead;return false;}
    if(back!=out.manager){if(failure)*failure=fp3::OwnerBacklink;return false;}
    std::uint64_t keys[2]{};
    if(!NativeHandKey6697(out.manager,0,keys[0])){if(failure)*failure=fp3::RightHeldKeyRead;return false;}
    if(!NativeHandKey6697(out.manager,1,keys[1])){if(failure)*failure=fp3::LeftHeldKeyRead;return false;}
    out.empty=PhysEmpty6698(keys[0])&&PhysEmpty6698(keys[1]);out.responsiveCombatH27=out.empty&&FistCombatReadyH27(out.player);out.tick=GetTickCount64();out.epoch=g_pose_epoch6689.load();out.request=g_fist_epoch101.load();if(failure)*failure=fp3::None;return true;
}
static void IntegratedFistSelection178r22c2() noexcept {
    FistOwner101 owner{};
    const bool empty=(g_combat_experiment175.load()||(g_normal_fists44.load()&&g_census_mode.load()>=4&&FullWeaponMode120(g_weapon_control_mode.load())))&&g_gameplay_allowed.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!diagnostics::lab::NativeReference()&&
        FistReadOwner101(owner)&&owner.empty;
    g_f4_fists_epoch178r22c2.store(empty?owner.epoch:0,std::memory_order_relaxed);
    g_f4_fists_tick178r22c2.store(empty?owner.tick:0,std::memory_order_release);
}
static bool FistSelectPunchH11(void*,int,std::uint64_t,bool kickH35) noexcept;
// H35: one immutable R3 kick request for the fist lane, captured with the owner it was pressed for (player, request epoch,
// body epoch, tracking sequence, tick). Consumed once by FistPublish101 and only for that same owner; never re-stamped.
struct FistKickRequestH35 {std::uint64_t serial=0,tick=0,request=0,epoch=0,sequence=0;std::uintptr_t actor=0;void* player=nullptr;int side=0;};
static kcd2vr::core::SnapshotMailbox<FistKickRequestH35> g_fist_kick_request_h35;
static std::atomic<bool> g_fist_kick_enabled_h35{false}; // H41 new authored-kick experiment starts OFF
static std::atomic<std::uint64_t> g_fist_kick_seen_h35{0};
static std::atomic<std::uint64_t> g_fist_kick_serial_h35{0},g_fist_kick_consumed_serial_h35{0}; // identity = monotonic serial, consumed only at a successful publication
static bool FistBeginGuardWaitH19(void*,std::uint64_t) noexcept;
static int FistGuardReleaseAckU16(void*,std::uint64_t) noexcept;
static thread_local bool g_fist_native_tick_active_t22=false; // observation only, never native action authority
static void FistNativeTickBodyT22() noexcept {
    std::unique_lock<std::recursive_mutex> guard(g_fist_native_mutex101,std::try_to_lock);if(!guard.owns_lock())return;
    auto& n=g_fist_native101;const auto now=std::uint64_t(GetTickCount64());
    FistOwner101 owner{};
    const bool prerequisites=FistSelected101()&&!g_fist_fault101.load()&&FistModePermits178r22c2()&&!diagnostics::lab::NativeReference()&&
        !animation_filter::nativeBodyOwned112.load()&&
        g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load();
    unsigned ownerFailureC54R3=fp3::OwnerUnresolved;
    const bool ownerResolved=prerequisites&&FistReadOwner101(owner,&ownerFailureC54R3);
    const bool active=ownerResolved&&owner.empty;
    unsigned inactiveWhyC54R3=fp3::None;
    if(fp3::active(now)){
        static fp3::Rate gateRate;
        unsigned mask=(!FistSelected101()?1u:0u)|(g_fist_fault101.load()?2u:0u)|(!FistModePermits178r22c2()?4u:0u)|
            (diagnostics::lab::NativeReference()?8u:0u)|(animation_filter::nativeBodyOwned112.load()?16u:0u)|
            (!g_gameplay_allowed.load()?32u:0u)|((!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load())?64u:0u)|
            (!ownerResolved?128u:0u)|(ownerResolved&&!owner.empty?256u:0u);
        inactiveWhyC54R3=!FistSelected101()?fp3::NotSelected:g_fist_fault101.load()?fp3::Fault:!FistModePermits178r22c2()?fp3::Mode:diagnostics::lab::NativeReference()?fp3::NativeReference:animation_filter::nativeBodyOwned112.load()?fp3::NativeBody:!g_gameplay_allowed.load()?fp3::Gameplay:!prerequisites?fp3::Tracking:!ownerResolved?ownerFailureC54R3:fp3::Occupied;
        if(!active)fp3::native(fp3::Stage::NativeGate,inactiveWhyC54R3,FistJoinC54R3(),now,&gateRate,mask);
    }
    if(!active){
        static std::uint64_t gateLogged167=0;
        if(FistSelected101()&&diagnostics::capture168::handCapture(now)&&(!gateLogged167||now<gateLogged167||now-gateLogged167>=1000)){
            gateLogged167=now;cc::Event event{};event.kind=cc::Kind::State;event.tick=now;
            event.flags=(g_fist_fault101.load()?1u:0u)|(animation_filter::nativeBodyOwned112.load()?2u:0u)|
                (!g_gameplay_allowed.load()?4u:0u)|((!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load())?8u:0u)|
                (!ownerResolved?16u:0u)|(ownerResolved&&!owner.empty?32u:0u);
            cc::name(event,"F10_FIST_GATE_CLOSED reasonBits=FAULT:1,NATIVE_BODY:2,CONTEXT:4,TRACKING:8,OWNER_UNRESOLVED:16,OCCUPIED:32");
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Hands,now)){
                char m[240]{};std::snprintf(m,sizeof(m),"C7F F10_HANDS FIST_GATE_CLOSED reasonBits=%u meaning=FAULT:1,NATIVE_BODY:2,CONTEXT:4,TRACKING:8,OWNER_UNRESOLVED:16,OCCUPIED:32",event.flags);
                diagnostic_trace::Write(m);
            }else cc::push(event);
        }
        FistCancel101(inactiveWhyC54R3);n.owned=false;g_fist_owner101.publish(FistOwner101{});return;
    }
    if(!n.owned||n.player!=owner.player||n.request!=owner.request||n.epoch!=owner.epoch){
        FistCancel101(fp3::OwnerChanged);n={};n.player=owner.player;n.request=owner.request;n.epoch=owner.epoch;
        // Native phase cancellation precedes accepting a physical stroke.
        if(!FistAbort119(n.player)){fp3::native(fp3::Stage::OwnerAcquired,fp3::EntryAbortFailed,FistJoinC54R3(),now);g_fist_fault101=true;return;}n.owned=true;
        fp3::native(fp3::Stage::OwnerAcquired,fp3::Returned,FistJoinC54R3(),now);
    }
    g_fist_owner101.publish(owner);
    FistFrame101 frame{};
    const bool frameRead=g_fist_frame101.read(frame);
    const bool have=frameRead&&frame.request==n.request&&frame.player==n.player&&frame.sample.epoch==n.epoch&&frame.sample.valid&&
        frame.sample.tick<=now&&now-frame.sample.tick<=100;
    if(!have){const unsigned why=!frameRead?fp3::FrameAbsent:frame.request!=n.request?fp3::FrameRequest:frame.player!=n.player?fp3::FramePlayer:
        frame.sample.epoch!=n.epoch?fp3::FrameEpoch:!frame.sample.valid?fp3::FrameInvalid:frame.sample.tick>now?fp3::FrameFuture:fp3::FrameStale;
        static fp3::Rate frameRate;fp3::native(fp3::Stage::Frame,why,FistJoinC54R3(),now,&frameRate,0,frameRead&&now>=frame.sample.tick?now-frame.sample.tick:0);FistCancel101(why);return;}
    if(frame.punch.valid&&HandClaimBlockedT24(frame.punch.side,frame.sample.actor,frame.sample.epoch,g_weapon_request_epoch6694.load()))frame.punch={};
    if(frame.sample.responsiveCombatH27&&!owner.responsiveCombatH27){FistCancel101(fp3::OwnerChanged);return;}
    static std::uint64_t logged119=0,stroke119=0;
    if(diagnostics::capture168::handCapture(now)&&(now-logged119>=500||(frame.punch.valid&&frame.punch.sequence!=stroke119))){
        logged119=now;if(frame.punch.valid)stroke119=frame.punch.sequence;char msg[400]{};
        std::snprintf(msg,sizeof(msg),
            "V66.119 FIST_INTENT R={phase:%u released:%d grip:%.2f trigger:%.2f blocked:%d} L={phase:%u released:%d grip:%.2f trigger:%.2f blocked:%d} stroke=%d speed=%.3f travel=%.3f nativeHit=NOT_INFERRED",
            unsigned(frame.phase[0]),frame.released[0],frame.sample.grip[0],frame.sample.trigger[0],frame.sample.blocked[0],
            unsigned(frame.phase[1]),frame.released[1],frame.sample.grip[1],frame.sample.trigger[1],frame.sample.blocked[1],
            frame.punch.valid,frame.punch.speed,frame.punch.travel);diagnostic_trace::Write(msg);
    }
    // U16: a deliberate punch may release our held block, but LT-up is only
    // queued native input. Keep this exact original gesture until native Update
    // acknowledges release, inside its ORIGINAL100ms gesture lifetime.
    bool resumedGuardU16=false;
    if(n.waitingGuardU16){
        const auto& punch=n.guardPunchU16;
        if(!punch.valid||(punch.kickH35&&!g_fist_kick_enabled_h35.load())||punch.side<0||punch.side>1||now<punch.tick||now-punch.tick>100||
           (!punch.kickH35&&(!fm::held(frame.sample,punch.side)||HandClaimBlockedT24(punch.side,frame.sample.actor,frame.sample.epoch,g_weapon_request_epoch6694.load())))){FistCancel101();return;} // H35: a kick resumes without the closed fist
        const int ack=FistGuardReleaseAckU16(n.player,n.epoch);
        if(ack<0){FistCancel101();return;}
        if(ack!=0)return;
        frame.punch=punch;n.waitingGuardU16=false;n.guardPunchU16={};n.guardActorH19=nullptr;n.guardStateH19=nullptr;resumedGuardU16=true;
        if(cc::active(now))diagnostic_trace::Write("H19 FIST_GUARD_WAIT edge=NATIVE_LT_UP_ACK_SAME_STATE replay=ORIGINAL_GESTURE_ONLY");
    }
    // V66.110: restore the headset/reference-proven controller-style lifecycle.
    // prepare_to_attack is a PRESS edge; attack_primary is a RELEASE edge on a
    // later native frame.  Do not collapse both into attack_primary_mouse.
    if(n.prepared){
        // Preserve CB1 kick eligibility and serials; a punch owns only its actual hand.
        const bool validSideT24=n.side>=0&&n.side<2;
        const bool occupiedT24=!validSideT24||(n.kickH35?(!frame.sample.empty[0]||!frame.sample.empty[1]):
            (!frame.sample.empty[n.side]||frame.sample.claimedT24[n.side]||HandClaimBlockedT24(n.side,frame.sample.actor,n.epoch,g_weapon_request_epoch6694.load())));
        if(now<n.pendingTick||now-n.pendingTick>150||occupiedT24||
           !(n.kickH35||fm::held(frame.sample,n.side))){const unsigned why=now<n.pendingTick?fp3::PreparedFuture:now-n.pendingTick>150?fp3::PreparedExpired:
               occupiedT24?fp3::PendingOccupied:fp3::GripReleased;
            fp3::native(fp3::Stage::Pending,why,FistJoinC54R3(),now,nullptr,0,now>=n.pendingTick?now-n.pendingTick:0,(validSideT24?frame.sample.grip[n.side]:0.f),(validSideT24?frame.sample.trigger[n.side]:0.f));FistCancel101(why);return;} // H29R1 both Trigger and Grip must stay above60% for punches.
        if(now>n.pendingTick){
            fp3::NativeContext priorContext{};(void)fp3::readContext(priorContext,now);
            (void)fp3::publishContext(FistJoinC54R3(),now,fp3::Stage::Release,priorContext.nativeActor);
            unsigned callWhy=fp3::None;const bool called=FistCall101(n.player,1,2,true,&callWhy);
            fp3::native(fp3::Stage::Release,callWhy,FistJoinC54R3(),now,nullptr,called?1u:0u,now-n.pendingTick,frame.sample.grip[n.side],frame.sample.trigger[n.side]);
            if(!called){NativeTimingCancel178r22c2();FistAbort119(n.player);}
            ObserveF11Native167(n.player,n.kickH35?"R3_KICK_COMMIT":"BOXING_COMMIT",n.side,n.pendingSample,-1,{},n.motion167,0,called,false,-1);
            n.prepared=false;n.lastAttack=now;
            if(diagnostics::capture168::handCapture(now)){
                char m[224]{};std::snprintf(m,sizeof(m),
                    "V66.110-FIST staged native prepare->primary-release side=%c dispatch=%s",n.side?'L':'R',called?"RETURNED":"FAILED");
                diagnostic_trace::Write(m);
            }
        }
        return;
    }
    if(frame.punch.kickH35&&!g_fist_kick_enabled_h35.load(std::memory_order_acquire))return; // no new prepare after OFF; existing prepared release/cleanup above stays native
    const bool seenH35=frame.punch.kickH35?(!frame.punch.kickSerialH35||(frame.punch.kickSerialH35<=n.lastKickSerialH35||frame.punch.kickSerialH35<=g_fist_kick_seen_h35.load(std::memory_order_acquire))):frame.punch.sequence<=n.lastSample;
    if(!frame.punch.valid||(!resumedGuardU16&&seenH35)){static fp3::Rate idleRate;
        fp3::Join join{reinterpret_cast<std::uintptr_t>(n.player),frame.sample.actor,n.request,n.epoch,frame.sample.sequence,0,-1};
        fp3::native(fp3::Stage::Pending,frame.punch.valid?fp3::AlreadySeen:fp3::NoPunch,join,now,&idleRate);return;}
    if(frame.punch.kickH35){n.lastKickSerialH35=frame.punch.kickSerialH35;g_fist_kick_seen_h35.store(n.lastKickSerialH35,std::memory_order_release);}
    else {n.lastSample=frame.punch.sequence;g_fist_seen101.store(n.lastSample,std::memory_order_release);}
    fp3::Join strokeJoin{reinterpret_cast<std::uintptr_t>(n.player),frame.sample.actor,n.request,n.epoch,frame.punch.sequence,frame.punch.side>=0&&frame.punch.side<2?frame.diagnosticOnsetC54R3[frame.punch.side]:0,frame.punch.side};
    // This exact gesture is consumed once above. Offer every fresh recognized
    // punch to the original native input selector; its current action/stamina
    // rules decide whether another attack can start, without a second mod timer.
    if(now<frame.punch.tick||now-frame.punch.tick>100){
        fp3::native(fp3::Stage::Prepare,now<frame.punch.tick?fp3::StrokeFuture:fp3::StrokeStale,strokeJoin,now,nullptr,0,now>=frame.punch.tick?now-frame.punch.tick:0);return;}
    // A deliberate punch releases only OUR guard. A manual LT remains native.
    if(!resumedGuardU16&&NativePhysicalGuardHeld117(n.player)){
        if(!FistBeginGuardWaitH19(n.player,n.epoch))return;
        n.waitingGuardU16=true;n.guardPunchU16=frame.punch;
        NativeGuardRelease112("U16_FIST_WAIT_NATIVE_LT_UP");
        if(cc::active(now))diagnostic_trace::Write("U16 FIST_GUARD_WAIT edge=QUEUED maxOriginalGestureAgeMs=100 nativePunch=NOT_YET_REQUESTED");
        return;
    }
    if(!FistSelectPunchH11(n.player,frame.punch.side,frame.sample.epoch,frame.punch.kickH35))return;
    // Publish before prepare: an ordinary native unarmed action may start at
    // that edge. One intent spans prepare/release; never publish a second one.
    if(!frame.punch.kickH35)NativeTimingFist178r22c2(n.player,frame.punch.sequence,frame.punch.tick,frame.punch.side,frame.punch.speed); // H35: a kick publishes no fist contact timing (no punch speed, no knuckle contact; the kick row's own hit timing is the game's)
    (void)fp3::publishContext(strokeJoin,now,fp3::Stage::Prepare);
    unsigned prepareWhy=fp3::None;const bool prepared=FistCall101(n.player,0,1,true,&prepareWhy);
    if(!prepared)fp3::retireContext();
    fp3::native(fp3::Stage::Prepare,prepareWhy,strokeJoin,now,nullptr,prepared?1u:0u);
    if(prepared){
        // R58C2: preparing a punch is not permission to replay the native arm for 1s.
        // F10 and F4 retain tracked wrists. Native admission/hit timing still owns semantics;
        // protected reactions/clinch handover is unchanged. VrFistNativeArm is retired.
        n.diagnosticCharacterC54R3=frame.sample.actor;n.diagnosticOnsetC54R3=strokeJoin.onset;
        n.prepared=true;n.side=frame.punch.side;n.motion167=frame.punch.motion167;n.pendingTick=now;n.pendingSample=frame.punch.sequence;n.pendingSpeed178r22c2=frame.punch.speed;n.kickH35=frame.punch.kickH35;
        if(frame.punch.kickH35)diagnostic_trace::Write("H35 R3_KICK stage=NATIVE_PREPARE zone=5 lane=FIST_PREPARE_RELEASE (release on the next native frame; the game selects the kick row)");
        ObserveF11Native167(n.player,n.kickH35?"R3_KICK_PREPARE":"BOXING_PREPARE",n.side,n.pendingSample,-1,{},n.motion167,0,true,false,-1);
        if(diagnostics::capture168::handCapture(now))diagnostic_trace::Write("V66.110-FIST native prepare_to_attack PRESS; release deferred to later game frame");
    }else {NativeTimingCancel178r22c2();FistAbort119(n.player);}

}
static void FistNativeTick101() noexcept {
    const bool previous=g_fist_native_tick_active_t22;
    g_fist_native_tick_active_t22=true;
    __try {FistNativeTickBodyT22();}
    __finally {g_fist_native_tick_active_t22=previous;}
}
static void FistPublish101(const TrackingSnapshot& t,const CommittedFrame6695& packet,const bool* blocked) noexcept {
    if(!FistSelected101()){static fp3::Rate unselected;fp3::native(fp3::Stage::ProducerGate,fp3::NotSelected,
        {0,packet.stamp.actor,g_fist_epoch101.load(),packet.stamp.epoch,t.sequence,0,-1},t.tick,&unselected);return;}
    FistOwner101 owner{};FistFrame101 f{};f.request=g_fist_epoch101.load();f.sample.epoch=packet.stamp.epoch;f.sample.sequence=t.sequence;f.sample.tick=t.tick;f.sample.timeNs=t.timeNs;f.sample.actor=packet.stamp.actor;
    const bool ownerReadC54R3=g_fist_owner101.read(owner);
    f.sample.valid=ownerReadC54R3&&owner.empty&&owner.request==f.request&&owner.epoch==packet.stamp.epoch&&std::uint64_t(GetTickCount64())>=owner.tick&&std::uint64_t(GetTickCount64())-owner.tick<=100&&t.allowed&&t.head_valid&&t.right_valid&&t.left_valid;
    f.sample.boxing167=FocusedTestMode167()==1; // stronger comparison only when explicitly selected
    f.sample.openPalm44=false; // Closed fists only; actual combat permits Grip closure, no open-palm attacks.
    f.sample.responsiveCombatH27=f.sample.valid&&owner.responsiveCombatH27; // current native combat owner only; Grip interactions outside combat stay unchanged
    f.sample.contactStop175=true;
    if(t.head_held||t.right_held||t.left_held)f.sample.valid=false;
    FistContactFrameH11 contactH11{},priorH11{};
    contactH11.player=owner.player;contactH11.request=f.request;contactH11.epoch=f.sample.epoch;contactH11.tick=t.tick;contactH11.sample=packet.stamp.sample;contactH11.timeNsH31=t.timeNs;contactH11.character=packet.stamp.actor;
    CommittedFrame6695 completedH31{};
    contactH11.completedH31=g_committed_frame6695.read(completedH31)&&
        completedH31.stamp.epoch==packet.stamp.epoch&&completedH31.stamp.sample==packet.stamp.sample&&
        completedH31.stamp.actor==packet.stamp.actor;
    contactH11.skeletonH31=packet.fistSkeletonH31;contactH11.jointsH31=packet.fistJointCountH31;contactH11.scaleH31=packet.world[7];
    const pm::Quat worldQH11{packet.world[0],packet.world[1],packet.world[2],packet.world[3]};
    const pm::Vec3 rootH11{packet.world[4],packet.world[5],packet.world[6]};
    const double scaleH11=packet.world[7];
    const bool worldH11=pm::unit(worldQH11).has_value()&&pm::finite(rootH11)&&std::isfinite(scaleH11)&&scaleH11>.1&&scaleH11<10;
    // Distinct completed locates can share a coarse millisecond tick. Preserve
    // their original producer chronology; raw milliseconds still own freshness.
    const bool priorKnownH11=g_fist_contact_h11.read(priorH11)&&priorH11.player==owner.player&&priorH11.request==f.request&&
        priorH11.epoch==f.sample.epoch&&priorH11.sample<contactH11.sample&&priorH11.tick<=t.tick&&t.tick-priorH11.tick<=60&&
        kcd2vr::motion_time::elapsed(priorH11.tick,t.tick,priorH11.timeNsH31,t.timeNs)>0;
    for(int sideH11=0;sideH11<2;++sideH11){
        contactH11.wristBoneH31[sideH11]=packet.fistWristBoneH31[sideH11];contactH11.slotBoneH31[sideH11]=packet.fistSlotBoneH31[sideH11];
        contactH11.modelWristH31[sideH11]=PhysPose6698(packet.allowed[sideH11]);contactH11.modelSlotH31[sideH11]=PhysPose6698(packet.fistSlotH31[sideH11]);
        auto scaledSlotH31=contactH11.modelSlotH31[sideH11];scaledSlotH31.p=scaledSlotH31.p*scaleH11;
        contactH11.worldSlotH31[sideH11]=ph::compose(PhysWorld6698(packet.world),scaledSlotH31);
        contactH11.slotValidH31[sideH11]=packet.fistSlotValidH31[sideH11]&&ph::finite(contactH11.modelWristH31[sideH11])&&ph::finite(contactH11.modelSlotH31[sideH11]);
        contactH11.valid[sideH11]=f.sample.valid&&contactH11.completedH31&&worldH11&&
            packet.palmValid59c9[sideH11]&&packet.valid[sideH11]&&completedH31.valid[sideH11]&&
            (sideH11?t.left_grip>.60f&&t.left_trigger>.60f:t.right_grip>.60f&&t.right_trigger>.60f);
        const auto& palmH11=packet.palm59c9[sideH11];
        contactH11.palm[sideH11]=rootH11+pm::rotate(worldQH11,pm::Vec3{palmH11.x,palmH11.y,palmH11.z}*scaleH11);
        contactH11.previous[sideH11]=priorKnownH11&&priorH11.valid[sideH11]&&
            pm::length(contactH11.palm[sideH11]-priorH11.palm[sideH11])<=.25?priorH11.palm[sideH11]:contactH11.palm[sideH11];
        const bool sameTopologyH31=priorKnownH11&&priorH11.completedH31&&priorH11.skeletonH31==contactH11.skeletonH31&&
            priorH11.jointsH31==contactH11.jointsH31&&priorH11.character==contactH11.character&&
            std::abs(priorH11.scaleH31-contactH11.scaleH31)<1.e-6&&
            priorH11.slotBoneH31[sideH11]==contactH11.slotBoneH31[sideH11]&&priorH11.wristBoneH31[sideH11]==contactH11.wristBoneH31[sideH11];
        if(sameTopologyH31){contactH11.gestureSampleH31[sideH11]=priorH11.gestureSampleH31[sideH11];contactH11.gestureDirectionH31[sideH11]=priorH11.gestureDirectionH31[sideH11];}
        contactH11.pairH31[sideH11]=sameTopologyH31&&priorH11.valid[sideH11]&&priorH11.slotValidH31[sideH11]&&contactH11.slotValidH31[sideH11]&&
            priorH11.sample<contactH11.sample&&pm::length(contactH11.worldSlotH31[sideH11].p-priorH11.worldSlotH31[sideH11].p)<=.25;
        if(contactH11.pairH31[sideH11]){contactH11.previousSlotH31[sideH11]=priorH11.worldSlotH31[sideH11];contactH11.previousTickH31[sideH11]=priorH11.tick;contactH11.previousSampleH31[sideH11]=priorH11.sample;contactH11.previousTimeNsH31[sideH11]=priorH11.timeNsH31;}
        // Repeated eye consumption of the same publication is idempotent.
        if(priorH11.player==owner.player&&priorH11.request==f.request&&priorH11.epoch==f.sample.epoch&&
           priorH11.tick==t.tick&&priorH11.sample==contactH11.sample&&priorH11.timeNsH31==t.timeNs&&priorH11.valid[sideH11]&&
           priorH11.skeletonH31==contactH11.skeletonH31&&std::abs(priorH11.scaleH31-contactH11.scaleH31)<1.e-6&&
           priorH11.slotValidH31[sideH11]==contactH11.slotValidH31[sideH11]&&
           pm::length(priorH11.worldSlotH31[sideH11].p-contactH11.worldSlotH31[sideH11].p)<.0001&&
           pm::angle(priorH11.worldSlotH31[sideH11].q,contactH11.worldSlotH31[sideH11].q)<.0001&&
           pm::length(contactH11.palm[sideH11]-priorH11.palm[sideH11])<.0001){
            contactH11.previous[sideH11]=priorH11.previous[sideH11];
            contactH11.pairH31[sideH11]=priorH11.pairH31[sideH11];contactH11.previousSlotH31[sideH11]=priorH11.previousSlotH31[sideH11];
            contactH11.previousTickH31[sideH11]=priorH11.previousTickH31[sideH11];contactH11.previousSampleH31[sideH11]=priorH11.previousSampleH31[sideH11];
            contactH11.previousTimeNsH31[sideH11]=priorH11.previousTimeNsH31[sideH11];
            contactH11.gestureSampleH31[sideH11]=priorH11.gestureSampleH31[sideH11];contactH11.gestureDirectionH31[sideH11]=priorH11.gestureDirectionH31[sideH11];
        }
    }
    f.player=owner.player;f.sample.empty[0]=f.sample.empty[1]=f.sample.valid;
    for(int sideT24=0;sideT24<2;++sideT24){
        const bool claimedT24=HandClaimBlockedT24(sideT24,packet.stamp.actor,packet.stamp.epoch,g_weapon_request_epoch6694.load());
        f.sample.claimedT24[sideT24]=claimedT24;
        contactH11.valid[sideT24]=contactH11.valid[sideT24]&&!claimedT24;
    }
    if(!f.sample.valid&&fp3::active(t.tick)){static fp3::Rate invalid;
        const unsigned mask=(!ownerReadC54R3?1u:0u)|(!owner.empty?2u:0u)|(owner.request!=f.request?4u:0u)|(owner.epoch!=packet.stamp.epoch?8u:0u)|
            (GetTickCount64()<owner.tick?16u:0u)|(GetTickCount64()>=owner.tick&&GetTickCount64()-owner.tick>100?32u:0u)|(!t.allowed?64u:0u)|
            (!t.head_valid?128u:0u)|(!t.right_valid?256u:0u)|(!t.left_valid?512u:0u)|(t.head_held?1024u:0u)|(t.right_held?2048u:0u)|(t.left_held?4096u:0u);
        fp3::native(fp3::Stage::ProducerGate,fp3::OwnerPublicationStale,{reinterpret_cast<std::uintptr_t>(owner.player),packet.stamp.actor,f.request,packet.stamp.epoch,t.sequence,0,-1},t.tick,&invalid,mask);}

    f.sample.predictionPolicyD69=t.handPolicyD69;
    f.sample.strokeExperimentD49=kcd2vr::fist_stroke_d49::enabled.load(std::memory_order_acquire);
    f.sample.headOrientation={t.hq.x,t.hq.y,t.hq.z,t.hq.w};f.sample.head={t.head.x,t.head.y,t.head.z};f.sample.controller[0]={t.right.x,t.right.y,t.right.z};f.sample.controller[1]={t.left.x,t.left.y,t.left.z};
    f.sample.grip[0]=t.right_grip;f.sample.grip[1]=t.left_grip;f.sample.trigger[0]=t.right_trigger;f.sample.trigger[1]=t.left_trigger;
    // Compare physical motion in the same standing model basis. Only this
    // completed pose's exact native stance is removed; contacts remain world-correct.
    for(int i=0;i<2;++i){f.sample.allowed[i]={packet.allowed[i].t[0],packet.allowed[i].t[1],packet.allowed[i].t[2]-packet.nativeCrouchOffsetR93};f.sample.blocked[i]=blocked[i];
        f.sample.freshContact174[i]=FistContactFresh174(i);}
    // A fresh transaction is required, but actor identity is the character and
    // native eligibility is independently rechecked through owner.player.
    const bool sameRequest=g_fist_motion_request101==f.request;
    const bool changedStrokeModeD49=sameRequest&&(g_fist_motion101.strokeExperimentD49!=f.sample.strokeExperimentD49||g_fist_motion101.predictionPolicyD69!=f.sample.predictionPolicyD69);
    auto staged=sameRequest?g_fist_motion101:fm::State{};
    const bool capture=fp3::active(t.tick);fm::Trace trace{};f.punch=fm::step(staged,f.sample,capture?&trace:nullptr);
    static fp3::Observer observer;observer.observe(f.sample,trace,f.punch,f.request,reinterpret_cast<std::uintptr_t>(f.player));
    if(f.punch.valid){const int side=f.punch.side;const auto direction=pm::unit(f.sample.allowed[side]-staged.hand[side].allowedOrigin);
        if(direction){contactH11.gestureSampleH31[side]=f.punch.sequence;contactH11.gestureDirectionH31[side]=pm::rotate(worldQH11,*direction);}}
    if(capture)for(int side=0;side<2;++side)f.diagnosticOnsetC54R3[side]=observer.hand[side].onset;
    for(int side=0;side<2;++side){f.phase[side]=staged.hand[side].phase;f.released[side]=staged.hand[side].released;}
    // Mode changes retire only an unentered recognizer nomination under this
    // publication owner. Native in-flight work and R3 kick requests are separate.
    auto pending=sameRequest?fm::retainUnenteredPendingD49(g_fist_pending101,g_fist_motion101.strokeExperimentD49,f.sample.strokeExperimentD49):fm::Punch{};
    if(changedStrokeModeD49&&!pending.kickH35)pending={}; // no native in-flight cancellation
    if(!f.sample.valid||pending.side<0||pending.side>1||(!pending.kickH35&&(!f.sample.empty[pending.side]||f.sample.claimedT24[pending.side]))||pending.epoch!=f.sample.epoch||pending.actor!=f.sample.actor||t.tick<pending.tick||t.tick-pending.tick>100||
       (pending.kickH35?(!g_fist_kick_enabled_h35.load()||pending.kickSerialH35<=g_fist_kick_seen_h35.load()):pending.sequence<=g_fist_seen101.load()))pending={};
    if(capture&&g_fist_pending101.valid&&!pending.valid){
        const auto& prior=g_fist_pending101;const unsigned why=changedStrokeModeD49?fp3::Mode:prior.sequence<=g_fist_seen101.load()?fp3::PendingSeen:fp3::PendingExpired;
        fp3::native(fp3::Stage::Publication,why,{reinterpret_cast<std::uintptr_t>(f.player),prior.actor,f.request,prior.epoch,prior.sequence,0,prior.side},t.tick);
    }
    if(capture&&pending.valid&&f.punch.valid&&pending.sequence!=f.punch.sequence)
        fp3::native(fp3::Stage::Publication,fp3::ReplacedPending,{reinterpret_cast<std::uintptr_t>(f.player),f.sample.actor,f.request,f.sample.epoch,f.punch.sequence,f.diagnosticOnsetC54R3[f.punch.side],f.punch.side},t.tick);
    const bool newlyRecognizedC54R3=f.punch.valid;
    if(!pending.valid&&f.punch.valid)pending=f.punch;
    // H35: the R3 kick request becomes a one-shot "punch" with the kick flag, only for the SAME owner it was pressed for
    // (player, request epoch, body epoch), with its ORIGINAL tick and tracking sequence (no fresh onset), when no gesture punch
    // is pending and the current sample is valid. Anything else drops it with one quiet line. Consumed exactly once.
    std::uint64_t kickSerialH35=0;const char* kickDropH35=nullptr;
    {FistKickRequestH35 k{};
     if(g_fist_kick_request_h35.read(k)&&k.serial&&k.serial>g_fist_kick_consumed_serial_h35.load(std::memory_order_acquire)){
        const auto consumedAtH35=std::uint64_t(GetTickCount64());
        // A repeated eye/older XR callback cannot consume a newly pressed R3.
        // Its ORIGINAL press clock remains pending until an observed later sample.
        const bool waitingSampleH35=t.tick<k.tick&&consumedAtH35>=k.tick&&consumedAtH35-k.tick<=150;
        if(!waitingSampleH35){
         kickSerialH35=k.serial;
         kickDropH35=!g_fist_kick_enabled_h35.load()?"EXPERIMENT_OFF":pending.valid?"PUNCH_PENDING":!f.sample.valid?"HANDS_NOT_VALID":
            (k.player!=f.player||k.request!=f.request||k.epoch!=f.sample.epoch||k.actor!=f.sample.actor)?"OWNER_CHANGED":
            (consumedAtH35<k.tick||consumedAtH35-k.tick>150)?"REQUEST_STALE":k.serial<=g_fist_kick_seen_h35.load()?"SERIAL_SEEN":nullptr;
         if(!kickDropH35){fm::Punch p{};p.valid=true;p.side=k.side;p.sequence=k.sequence;p.tick=k.tick;p.epoch=k.epoch;p.actor=k.actor;p.kickH35=true;p.kickSerialH35=k.serial;pending=p;}
        }
    }}
    f.punch=pending;
    if(!g_fist_frame101.publish(f)){static fp3::Rate busy;fp3::native(fp3::Stage::Publication,fp3::MailboxBusy, // H35: a BUSY publication leaves the kick request unconsumed (retried next sample, same original request)
        {reinterpret_cast<std::uintptr_t>(f.player),f.sample.actor,f.request,f.sample.epoch,f.sample.sequence,0,-1},t.tick,&busy);return;}g_fist_pending101=pending;g_fist_motion101=staged;g_fist_motion_request101=f.request;
    if(kickSerialH35){g_fist_kick_consumed_serial_h35.store(kickSerialH35,std::memory_order_release); // H35: consumed exactly once, at this successful publication
        if(kickDropH35){char m[160]{};std::snprintf(m,sizeof(m),"H35 R3_KICK stage=DROPPED reason=%s",kickDropH35);diagnostic_trace::Write(m);}}
    (void)g_fist_contact_h11.publish(contactH11);
    {namespace fr=kcd2vr::fist_reach_r10;static fr::Peak peaksR10[2]{}; // R10: remember where this punch really reached
     for(int sideR10=0;sideR10<2;++sideR10){
        const bool okR10=contactH11.completedH31&&contactH11.valid[sideR10]&&contactH11.pairH31[sideR10]&&contactH11.slotValidH31[sideR10];
        if(fr::track(peaksR10[sideR10],contactH11.gestureSampleH31[sideR10],okR10,contactH11.previousSlotH31[sideR10].p,
               contactH11.worldSlotH31[sideR10].p,contactH11.gestureDirectionH31[sideR10],contactH11.sample,contactH11.tick)){
            FistPeakR10 peakR10{};peakR10.side=sideR10;peakR10.peak=peaksR10[sideR10];peakR10.palms=contactH11;
            (void)FistPeakMailboxR10(sideR10).publish(peakR10);}
     }}
    if(capture&&newlyRecognizedC54R3&&f.punch.valid)
        fp3::native(fp3::Stage::Publication,fp3::Returned,{reinterpret_cast<std::uintptr_t>(f.player),f.sample.actor,f.request,f.sample.epoch,f.punch.sequence,f.diagnosticOnsetC54R3[f.punch.side],f.punch.side},t.tick);
    static kcd2vr::f11_tests167::Touch touch167;
    for(int side=0;side<2;++side)
        if(touch167.update(side,f.sample.contactStop175&&f.sample.valid,blocked[side],f.request,t.sequence,t.tick))
            (void)kcd2vr::haptics101::publish_event(side,0xF1100001u+side,.22f,30,t.tick);
}
static void ServiceRuntimePerf101R2() noexcept {
 const auto now=std::uint64_t(GetTickCount64());
 const bool recording=diagnostics::capture168::any(now);
 if(recording)ServiceBodyEvidence122();
 static std::uint64_t last=0;if(now-last<5000)return;const auto windowMs=last?now-last:5000;last=now;
 ServiceAttackTiming178r22c2();
 kcd2vr::diagnostics::pose_cost_d60::drain(now,[](const char* line){diagnostic_trace::Write(line);});
 {char hc[900]{};if(kcd2vr::hookcost178r22::summary(hc,sizeof(hc),windowMs))diagnostic_trace::Write(hc);} // D48: drains every window; emits only under the finite F2 lease
 if(g_census_mode.load()<2&&g_weapon_control_mode.load()<2)return;
 static const char* names[]{"POSE_SOLVE","NATIVE_SKINNING","PHYSICAL_PLACEMENT","NATIVE_FRAME","EQUIPMENT_READ","NATIVE_STATE_READBACK","EQUIPMENT_LIVE","EQUIPMENT_METADATA","NATIVE_BEGIN","NATIVE_SPECIAL","NATIVE_FIST","NATIVE_GUARD","NATIVE_WEAPON","NATIVE_RHYTHM","NATIVE_ACTION_PREPARE","NATIVE_ACTION_RELEASE","NATIVE_ACTION_CANCEL","NATIVE_ZONE"};
 static_assert(sizeof(names)/sizeof(*names)==sizeof(g_perf101R2)/sizeof(*g_perf101R2));
 for(int i=0;i<int(sizeof(names)/sizeof(*names));++i){auto& p=g_perf101R2[i];const auto count=p.count.exchange(0),ns=p.ns.exchange(0),mx=p.maximum.exchange(0),queries=p.queries122.exchange(0),reuse=p.reuse122.exchange(0),queryNs=p.queryNs125.exchange(0);unsigned masks[4]{};for(int j=0;j<4;++j)masks[j]=p.modeMask125[j].exchange(0);if(!recording||!count)continue;
  char m[640]{};std::snprintf(m,sizeof(m),"V66.122 PERF stage=%s calls=%llu avgUs=%.3f maxUs=%.3f pageQueries=%llu pageReuse=%llu timing=WALL scopesMayOverlap=1 modesAtPrintF10=%d modesAtPrintF12=%d windowMayMixModes=1 avgPageQueryUs=%.3f entryModeMaskF10=%X entryModeMaskF12=%X entryModeMaskF3=%X entryModeMaskF5=%X",names[i],(unsigned long long)count,double(ns)/count/1000.0,double(mx)/1000.0,(unsigned long long)queries,(unsigned long long)reuse,g_census_mode.load(),g_weapon_control_mode.load(),double(queryNs)/count/1000.0,masks[0],masks[1],masks[2],masks[3]);diagnostic_trace::Write(m);
 }
 if(!recording)return;
 char m[768]{};std::snprintf(m,sizeof(m),"V66.101R2 GATES prepareReject=%llu prepareOK=%llu published=%llu handMismatch=%llu frameAbsent=%llu frameIdentity=%llu frameInactive=%llu frameStale=%llu placed=%llu publishBusy=%llu pageQueries=%llu pageReuse=%llu",
  (unsigned long long)g_physical_gates101R2[0].load(),(unsigned long long)g_physical_gates101R2[1].load(),(unsigned long long)g_physical_gates101R2[2].load(),(unsigned long long)g_physical_gates101R2[3].load(),(unsigned long long)g_physical_gates101R2[4].load(),(unsigned long long)g_physical_gates101R2[5].load(),(unsigned long long)g_physical_gates101R2[6].load(),(unsigned long long)g_physical_gates101R2[7].load(),(unsigned long long)g_physical_gates101R2[8].load(),(unsigned long long)g_physical_gates101R2[9].load(),(unsigned long long)g_read_queries101R2.load(),(unsigned long long)g_read_hits101R2.load());diagnostic_trace::Write(m);
}
static void ServicePunchCaptureC54R3() noexcept {
    const auto now=std::uint64_t(GetTickCount64());const bool capture=fp3::active(now);fp3::Record r{};unsigned budget=capture?64:256;
    while(budget--&&fp3::pop(r)){
        if(r.session!=fp3::session()){++fp3::staleDiscarded;continue;}
        if(!capture||!fp3::active(r.tick)){++fp3::offDiscarded;continue;}
        char message[1900]{};const int length=fp3::format(r,message,sizeof(message));
        if(length<0||length>=int(sizeof(message))){++fp3::formatClipped;std::snprintf(message,sizeof(message),
            "C54R3 PUNCH_HEALTH status=FORMAT_CLIPPED session=%llu sample=%llu stage=%s requiredBytes=%d capacity=%zu evidence=INCOMPLETE",
            (unsigned long long)r.session,(unsigned long long)r.join.sample,fp3::stageName(r.stage),length,sizeof(message));}
        diagnostic_trace::Write(message);
    }
    static bool wasCapture=false;static std::uint64_t last=0,seenSession=0;
    if(!capture){if(wasCapture){char message[500]{};std::snprintf(message,sizeof(message),
        "V66.168 CAPTURE domain=COMBAT detail=PUNCH_HEALTH status=END session=%llu accepted=%llu drained=%llu dropped=%llu staleSessionDiscarded=%llu offDiscarded=%llu formatClipped=%llu totals=CUMULATIVE absenceWithLoss=INCONCLUSIVE",
        (unsigned long long)seenSession,(unsigned long long)fp3::accepted.load(),(unsigned long long)fp3::drained.load(),(unsigned long long)fp3::dropped.load(),
        (unsigned long long)fp3::staleDiscarded.load(),(unsigned long long)fp3::offDiscarded.load(),(unsigned long long)fp3::formatClipped.load());diagnostic_trace::Write(message);}wasCapture=false;return;}
    wasCapture=true;
    if(seenSession!=fp3::session()||!last||now<last||now-last>=5000){seenSession=fp3::session();last=now;char message[1400]{};
        std::snprintf(message,sizeof(message),"C54R3 PUNCH_HEALTH session=%llu accepted=%llu drained=%llu dropped=%llu staleSessionDiscarded=%llu offDiscarded=%llu formatClipped=%llu queueCapacity=255 producer=NONBLOCKING capture=F3_ONLY noNativeReads=1 absenceWithDrops=INCONCLUSIVE reasonBits=1:INVALID,2:DUPLICATE,4:ZERO_DT,8:DISCONTINUITY,16:JUMP,32:SPEED_OUTLIER,64:NEED_RELEASE,128:OPEN,256:MIDDLE,512:SETTLING,1024:NO_ONSET,2048:BLOCKED_ONSET,4096:REVERSED,8192:TIMEOUT,16384:TRAVEL,32768:ALLOWED_TRAVEL,65536:SPEED,131072:ALLOWED_SPEED,262144:SLAP_CONTACT,524288:REARM,1048576:OTHER_HAND,2097152:TOWARD_HEAD_D49 trialCurrentTrackedSpeedMin=0.100",
            (unsigned long long)seenSession,(unsigned long long)fp3::accepted.load(),(unsigned long long)fp3::drained.load(),(unsigned long long)fp3::dropped.load(),
            (unsigned long long)fp3::staleDiscarded.load(),(unsigned long long)fp3::offDiscarded.load(),(unsigned long long)fp3::formatClipped.load());diagnostic_trace::Write(message);}
}
static void ServiceCombatCapture101() noexcept {
    ServicePunchCaptureC54R3();
    ServicePassiveContact169R1();
    ServiceRuntimePerf101R2();
    const bool recording=cc::active(std::uint64_t(GetTickCount64()));
    cc::Event e{};int budget=48;
    while(budget--&&cc::pop(e)){
        if(!recording||!cc::active(e.tick))continue;
        if(e.kind==cc::Kind::PhysicalImpact177){
            char m[720]{};std::snprintf(m,sizeof(m),
                "V66.177R1 F4_PHYSICAL_IMPACT sample=%llu tick=%llu epoch=%llu request=%llu key=%016llX generation=%llu side=%d targetId=%u rawPart=%d support=%u point=(%.3f,%.3f,%.3f) approachSpeed=%.3f estimatedEffectiveKg=%.4f estimatedNormalJ=%.4f estimatedImpulseNs=%.4f material=UNKNOWN targetVelocity=UNMEASURED nativeDamage=NOT_INFERRED units=MOD_DESIGN_ESTIMATES",
                (unsigned long long)e.sequence,(unsigned long long)e.tick,(unsigned long long)e.poseEpoch,(unsigned long long)e.key[1],
                (unsigned long long)e.key[0],(unsigned long long)e.aliasHash,e.mode,unsigned(e.id),e.layer,e.flags,
                e.current[0],e.current[1],e.current[2],e.hand[0],e.phase,e.value,e.hand[1]);
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::MeleeCommand174r2){
            char m[920]{};std::snprintf(m,sizeof(m),
                "V66.174R2 MELEE_COMMAND session=%llu sample=%llu tick=%llu command=%d side=%d reason=%s releaseReason=%d flags=%u key=%016llX generation=%llu epoch=%llu request=%llu net=%.3f fastTravel=%.3f handSpeed=%.3f tipSpeed=%.3f ageMs=%.1f coherence=%.3f axisKnown=%.0f axisCos=%.3f modelAxisCos=%.3f guardOrigin59=%.0f handNet59=%.3f guardCutPass59=%.0f flagsMeaning=KNOWN:1,BLOCKED:2,GUARD:4,ACTIVE:8,QUEUED:16 stage=POSE_PUBLICATION nativeDispatch=NOT_INFERRED",
                (unsigned long long)e.session,(unsigned long long)e.sequence,(unsigned long long)e.tick,e.mode,e.id,e.name,e.layer,e.flags,
                (unsigned long long)e.key[0],(unsigned long long)e.aliasHash,(unsigned long long)e.poseEpoch,(unsigned long long)e.key[1],
                e.value,e.phase,e.hand[0],e.hand[1],e.hand[2],e.hand[3],e.current[2],e.current[0],e.current[1],e.current[4],e.current[3],e.current[5]);
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::F11Native167){
            char m[720]{};std::snprintf(m,sizeof(m),
                "V66.175 MOTION_NATIVE historicalRung=%u session=%llu tick=%llu sample=%llu edge=%s side=%d gesture=%llu originZone=%d originName=%s originChest=(%.3f,%.3f,%.3f) guardObserved=%d attackZoneObserved=%.0f typeObserved=%.0f phaseObserved=%.0f handObserved=%.0f inputObserved=%.0f inputPassed=%.0f flags=%u key=%016llX outcome=NATIVE_NOT_FORCED flagsMeaning=RETURNED:1,STATE_OBSERVED:2,HANDLER_OBSERVED:4",
                cc::f11Rung167(e.source),(unsigned long long)e.session,(unsigned long long)e.tick,(unsigned long long)e.sequence,e.name,e.mode,
                (unsigned long long)e.aliasHash,e.id,kcd2vr::thrust_origin167::name(e.id),e.hand[0],e.hand[1],e.hand[2],e.layer,
                e.current[0],e.current[1],e.current[2],e.current[3],e.current[4],e.value,e.flags,(unsigned long long)e.key[0]);
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::ContactPath){
            char raw[sizeof(e.contact114)*2+1]{};constexpr char digits[]="0123456789abcdef";
            for(std::size_t i=0;i<sizeof(e.contact114);++i){raw[2*i]=digits[e.contact114[i]>>4];raw[2*i+1]=digits[e.contact114[i]&15];}
            char m[1100]{};std::snprintf(m,sizeof(m),
                "V66.122 CONTACT_PATH session=%llu sequence=%llu tick=%llu owner=%llX targetId=%u HenryRole=%s seenMask=%u stopMask=%u candidateCopied=%d raw136=%s prediction=%llu sweep=%llu sample=%llu redirected=%d causalJoin=SAME_NATIVE_STACK_OR_NONE damage=UNCLAIMED F11=%u",
                (unsigned long long)e.session,(unsigned long long)e.sequence,(unsigned long long)e.tick,(unsigned long long)e.actor,unsigned(e.id),
                e.trackingFlags==2?"TARGET":"SOURCE",e.flags,unsigned(e.layer),(e.flags&16)!=0,raw,(unsigned long long)e.parent164[0],(unsigned long long)e.parent164[1],(unsigned long long)e.parent164[2],e.mode,cc::f11Rung167(e.source));
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::CloseQuery){
            char raw[sizeof(e.contact114)*2+1]{};constexpr char digits[]="0123456789abcdef";
            for(std::size_t i=0;i<sizeof(e.contact114);++i){raw[2*i]=digits[e.contact114[i]>>4];raw[2*i+1]=digits[e.contact114[i]&15];}
            char m[780]{};std::snprintf(m,sizeof(m),
                "V66.164 CLOSE_QUERY session=%llu seq=%llu tick=%llu actor=%llu opponent=%llu opponentPointerPresent=%u input=%d query3C=%d query9C=%d query94=%u query98=%llu queryPointer=%llX raw136=%s source=687BBC_RETURN actionAccepted=UNCLAIMED mutation=NONE intentSerial=%llu intentSource=%llu intentJoin=SAME_NATIVE_STACK_OR_NONE",
                (unsigned long long)e.session,(unsigned long long)e.sequence,(unsigned long long)e.tick,(unsigned long long)e.key[0],(unsigned long long)e.key[1],e.flags,e.id,e.mode,e.layer,e.trackingFlags,
                (unsigned long long)e.parent164[0],(unsigned long long)e.aliasHash,raw,(unsigned long long)e.parent164[1],(unsigned long long)e.parent164[2]);diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::InteractionBody){
            char m[600]{};std::snprintf(m,sizeof(m),
                "V66.122 INTERACTION_BODY session=%llu requestSeq=%llu tick=%llu stage=%s targetId=%u entity=%llX body=%llX bodyVftRva=%llX callerRva=%llX ageMs=%.0f bodyReadable=%u source=%s heldConstraint=UNCLAIMED mutation=NONE",
                (unsigned long long)e.session,(unsigned long long)e.sequence,(unsigned long long)e.tick,e.flags==2?"SELECTED_TARGET_PROBE":e.flags?"RETURN":"WATCH",
                unsigned(e.id),(unsigned long long)e.actor,(unsigned long long)e.key[0],(unsigned long long)e.key[1],
                (unsigned long long)e.aliasHash,e.value,e.trackingFlags,e.flags==2?"VERIFIED_GETTER_ONE_SHOT":e.flags?"NATIVE_GETTER_RETURN":"ACTION_TARGET");
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::Material){
            char m[1152]{};std::snprintf(m,sizeof(m),
                "V66.116 MATERIAL_EFFECT session=%llu serial=%llu tick=%llu epoch=%llu F3=%d stage=%u sourceId=%llu targetId=%llu HenryRole=%s row=\"%s\" surfaceId=%d effectId=%d nativeReturned=%.0f pos=(%.4f,%.4f,%.4f) normal=(%.4f,%.4f,%.4f) scale=%.4f mutation=NONE damage=UNCLAIMED prediction=%llu sweep=%llu contact=%llu causalJoin=SAME_NATIVE_STACK_OR_NONE",
                (unsigned long long)e.session,(unsigned long long)e.sequence,(unsigned long long)e.tick,(unsigned long long)e.poseEpoch,e.mode,e.flags,
                (unsigned long long)e.key[0],(unsigned long long)e.key[1],e.trackingFlags==1?"SOURCE":"TARGET",e.name,e.id,e.layer,e.value,
                e.hand[0],e.hand[1],e.hand[2],e.hand[3],e.hand[4],e.hand[5],e.phase,
                (unsigned long long)e.parent164[0],(unsigned long long)e.parent164[1],(unsigned long long)e.parent164[2]);
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::R3){
            char m[320]{};std::snprintf(m,sizeof(m),"V66.115R1 R3_REFERENCE session=%llu sourceMask=%u tick=%llu down=%d F3=%d F10=%d F12=%d nativeAction=NOT_INFERRED",
                (unsigned long long)e.session,e.source,(unsigned long long)e.tick,e.flags!=0,e.mode,e.id,e.layer);
            diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::Contact){
            char raw[sizeof(e.contact114)*2+1]{};constexpr char digits[]="0123456789abcdef";
            for(std::size_t i=0;i<sizeof(e.contact114);++i){raw[2*i]=digits[e.contact114[i]>>4];raw[2*i+1]=digits[e.contact114[i]&15];}
            char m[896]{};std::snprintf(m,sizeof(m),"V66.114 NATIVE_CONTACT tick=%llu source=WHGame+D51854 ownerCollisions=%llX targetId=%u HenryRole=%s sourceShape=%llX targetShape=%llX mode=%d raw136=%s eligibility=PRE_NATIVE_FILTERS damage=UNCLAIMED geometry=RAW_UNQUALIFIED",
                (unsigned long long)e.tick,(unsigned long long)e.actor,unsigned(e.id),e.flags==2?"TARGET":"SOURCE",
                (unsigned long long)e.key[0],(unsigned long long)e.key[1],e.mode,raw);diagnostic_trace::Write(m);continue;
        }
        if(e.kind==cc::Kind::Sweep){
            char m[1024]{};std::snprintf(m,sizeof(m),
                "V66.113-REFERENCE-SWEEP session=%llu sourceMask=%u tick=%llu owner=HENRY collision=%llX callerRva=%llX bone=%d hand=%d dt=%.6f prev=(%.6f,%.6f,%.6f;%.6f,%.6f,%.6f,%.6f) cur=(%.6f,%.6f,%.6f;%.6f,%.6f,%.6f,%.6f) resultNonzero=%u mutation=NONE acceptedDamage=UNCLAIMED",
                (unsigned long long)e.session,e.source,(unsigned long long)e.tick,(unsigned long long)e.actor,(unsigned long long)e.key[0],e.id,e.layer,e.value,
                e.previous[0],e.previous[1],e.previous[2],e.previous[3],e.previous[4],e.previous[5],e.previous[6],
                e.current[0],e.current[1],e.current[2],e.current[3],e.current[4],e.current[5],e.current[6],e.flags);
            diagnostic_trace::Write(m);
            if(!std::strcmp(e.name,"C54R3_PUNCH_TEMPORAL")&&fp3::active(e.tick)){
                char context[700]{};std::snprintf(context,sizeof(context),
                    "C54R3 PUNCH_PATH stage=NATIVE_SWEEP tick=%llu sample=%llu onset=%llu request=%llu epoch=%llu player=%llX nativeActor=%llX side=%d nativeHand=%d bone=%d callerRva=%llX resultNonzero=%u actorMatch=%s join=TEMPORAL_TRANSPORT_CONTEXT_ONLY windowMs=500 nativeContact=NOT_INFERRED nativeAdmission=NOT_INFERRED writes=NONE",
                    (unsigned long long)e.tick,(unsigned long long)e.sequence,(unsigned long long)e.parent164[0],(unsigned long long)e.parent164[1],(unsigned long long)e.poseEpoch,
                    (unsigned long long)e.parent164[2],(unsigned long long)e.aliasHash,e.mode,e.layer,e.id,(unsigned long long)e.key[0],e.flags,e.trackingFlags==1?"EXACT":"UNKNOWN");diagnostic_trace::Write(context);
            }
            continue;
        }
        char m[1408]{};std::snprintf(m,sizeof(m),"V66.101R1-COMBAT_CAPTURE session=%llu kind=%d tick=%llu sample=%llu epoch=%llu actor=%llX mode=%d value=%.4f id=%d layer=%d phase=%.5f flags=%u keys=%016llX/%016llX grip=%.3f/%.3f trigger=%.3f/%.3f name=\"%s\" sourceMask=%u aliasHash=%016llX trackingFlags=%u trackingLocalHand=(%.5f,%.5f,%.5f;%.5f,%.5f,%.5f) meaning=OBSERVED_OR_REQUESTED_NOT_ACCEPTED_HIT",
            (unsigned long long)e.session,int(e.kind),(unsigned long long)e.tick,(unsigned long long)e.sequence,(unsigned long long)e.poseEpoch,(unsigned long long)e.actor,e.mode,e.value,e.id,e.layer,e.phase,e.flags,(unsigned long long)e.key[0],(unsigned long long)e.key[1],e.grip[0],e.grip[1],e.trigger[0],e.trigger[1],e.name,e.source,(unsigned long long)e.aliasHash,e.trackingFlags,e.hand[0],e.hand[1],e.hand[2],e.hand[3],e.hand[4],e.hand[5]);diagnostic_trace::Write(m);
    }
    if(!recording)return;
    static std::uint64_t reportedSession=0,reportedDrops=0;
    const auto captureSession=cc::session.load(),captureDrops=cc::dropped.load();
    if(captureSession!=reportedSession||captureDrops!=reportedDrops){
        char status[256]{};std::snprintf(status,sizeof(status),
            "V66.113 CAPTURE_HEALTH session=%llu sourceMask=%u untilTick=%llu droppedTotal=%llu queueCapacity=511 overflow=DROP_NOT_BLOCK",
            (unsigned long long)captureSession,cc::sourceMask.load(),(unsigned long long)cc::until.load(),(unsigned long long)captureDrops);
        diagnostic_trace::Write(status);reportedSession=captureSession;reportedDrops=captureDrops;
    }
    static std::uint64_t lastPhysical=0;const auto physicalNow=std::uint64_t(GetTickCount64());
    if(g_weapon_control_mode.load()>=4&&physicalNow-lastPhysical>=5000){
        lastPhysical=physicalNow;PhysEquipment6698 equipment{};PhysGripBank6698 grips{};
        const bool have=g_phys_equipment6698.read(equipment);g_phys_grips6698.read(grips);
        char m[640]{};int primaries=0,supports=0;
        for(const auto& g:grips.grip){primaries+=g.ready[0];supports+=g.ready[1];}
        std::snprintf(m,sizeof(m),"V66.101R1-WEAPON SNAP rung=%d armed=%d fault=%d equipment=%d slots=%d gripReadyRight=%d gripReadyLeft=%d socketSamples=%llu stalePacket=%llu busy=%llu matched=%llu noSocket=%llu status=OBSERVED_NOT_VISUAL_ACCEPTANCE",
            g_weapon_control_mode.load(),g_phys_armed6698.load(),g_phys_fault6698.load(),have&&equipment.valid,equipment.count,primaries,supports,
            (unsigned long long)g_socket_capture_counts101[0].load(),(unsigned long long)g_socket_capture_counts101[1].load(),(unsigned long long)g_socket_capture_counts101[2].load(),(unsigned long long)g_socket_capture_counts101[3].load(),(unsigned long long)g_socket_capture_counts101[4].load());
        diagnostic_trace::Write(m);
    }
    static std::uint64_t last=0,drops=0;const auto now=std::uint64_t(GetTickCount64());const auto lost=cc::dropped.load();
    if(now-last>5000&&lost!=drops){last=now;drops=lost;char m[160]{};std::snprintf(m,sizeof(m),"V66.101R1-COMBAT_CAPTURE dropped=%llu boundedQueue=512",(unsigned long long)lost);diagnostic_trace::Write(m);}
}

// H35: the R3 producer for the unarmed kick. One completed FistFrame101 snapshot (the last successful publication) supplies
// EVERY identity the request carries - player, request epoch, body epoch, actor, tracking sequence, sample tick - validated
// against the current owner at the moment of the press; nothing is re-stamped later. Identity of the command = a monotonic
// serial. The public tri-state separates selected-lane refusal from positively armed/OFF fallback.
static void SetFistKickEnabledImplH35(bool on) noexcept {g_fist_kick_enabled_h35.store(on,std::memory_order_release);}
static bool FistKickEnabledImplH35() noexcept {return g_fist_kick_enabled_h35.load(std::memory_order_acquire);}
static FistKickResultH35 RequestFistKickStatusImplH35(int side) noexcept {
    // CL-H: R3 never takes the fist prepare/release lane any more - in free stance it made the game throw a heavy punch.
    // Empty-hand R3 goes to the game's own special attack (NativeSpecialTick168). VrKickFistLane is kept for old INIs.
    constexpr bool r3UsesFistLaneCLH=false;
    if(!r3UsesFistLaneCLH||!FistKickEnabledImplH35())return FistKickResultH35::NotThisLane;
    const auto now=std::uint64_t(GetTickCount64());
    const auto refuse=[&](const char*why){(void)kcd2vr::visible_failure::publish(why,now);return FistKickResultH35::Refused;};
    FistOwner101 owner{};
    if(!FistReadOwner101(owner)||!owner.player)return refuse("kick: current hand owner unavailable");
    if(!owner.empty)return FistKickResultH35::NotThisLane; // positively armed = original native special
    if(side<0||side>1||!FistSelected101()||g_fist_fault101.load())return refuse("kick: unarmed kick lane unavailable");
    FistFrame101 frame{};
    if(!g_fist_frame101.read(frame)||!frame.sample.valid||!frame.sample.sequence||!frame.sample.tick||now<frame.sample.tick||now-frame.sample.tick>100||
       frame.player!=owner.player||frame.request!=owner.request||frame.sample.epoch!=owner.epoch||!frame.sample.actor||
       frame.sample.actor!=g_player_char37.load(std::memory_order_acquire))return refuse("kick: no current completed unarmed frame");
    auto serial=g_fist_kick_serial_h35.load(std::memory_order_acquire);
    if(serial==UINT64_MAX||!g_fist_kick_serial_h35.compare_exchange_strong(serial,serial+1,std::memory_order_acq_rel))return refuse("kick: request publication busy");
    FistKickRequestH35 k{};k.serial=serial+1;k.tick=now;
    k.player=frame.player;k.request=frame.request;k.epoch=frame.sample.epoch;k.actor=frame.sample.actor;k.sequence=frame.sample.sequence;k.side=side;
    return g_fist_kick_request_h35.publish(k)?FistKickResultH35::Queued:refuse("kick: request publication busy");
}

// R11: first real committed contact of the current punch with an NPC body (fist = weapon, fist_clock_r11.h). copied
// after the whole pose committed, from the same synchronous hand ray the F10 collisions use. no native call here.
static kcd2vr::core::SnapshotMailbox<kcd2vr::fist_clock_r11::Contact> g_fist_contact_r11[2];
static void FistPublishContactR11(const kcd2vr::passive169r1::Sample& sample,const kcd2vr::npc_contact172::Witness& target,std::uint64_t queryTick) noexcept {
    namespace fc=kcd2vr::fist_clock_r11;namespace p=kcd2vr::passive169r1;namespace nc=kcd2vr::npc_contact172;
    if(!fc::enabled.load(std::memory_order_relaxed)||g_weapon_control_mode.load()!=7||!FistSelected101()||
       diagnostics::lab::NativeReference()||!g_gameplay_allowed.load()||!queryTick||sample.side<0||sample.side>1||sample.source!=p::Source::HandWorld||!sample.eligible||!sample.worldCoherent||
       sample.status!=p::Status::Contact||!target.ownerRead||!target.entityId||
       (target.body!=nc::Body::Living&&target.body!=nc::Body::Articulated))return;
    const int side=sample.side;const auto now=std::uint64_t(GetTickCount64());FistContactFrameH11 f{};
    if(!g_fist_contact_h11.read(f)||!f.completedH31||!f.valid[side]||!f.pairH31[side]||!f.gestureSampleH31[side]||
       f.sample!=sample.sequence||f.epoch!=sample.epoch||f.character!=sample.actor||
       f.request!=g_fist_epoch101.load()||f.epoch!=g_pose_epoch6689.load()||!f.player||
       now<queryTick||now-queryTick>60||!f.tick||now<f.tick||now-f.tick>60)return;
    fc::Contact c{};c.gesture=f.gestureSampleH31[side];c.epoch=f.epoch;c.request=f.request;c.sample=f.sample;
    c.tick=queryTick;c.player=reinterpret_cast<std::uintptr_t>(f.player);c.character=f.character;c.side=side;c.point=sample.point;
    c.from=f.previous[side];c.to=f.palm[side];c.entity=target.entityId;c.living=true;
    if(!fc::qualifies(c,f.gestureDirectionH31[side]))return;
    static fc::Producer producers[2]{};
    auto staged=producers[side];if(!fc::firstOf(staged,c))return; // retry a busy mailbox without spending the contact
    if(!g_fist_contact_r11[side].publish(staged.pending))staged.published=false;
    producers[side]=staged;
}
static bool FistTouchedR11(const kcd2vr::fist_action_h12::Binding& b,std::uintptr_t character,std::uint64_t now) noexcept {
    const int side=b.intent.side;if(side<0||side>1)return false;kcd2vr::fist_clock_r11::Contact c{};
    return g_fist_contact_r11[side].read(c)&&kcd2vr::fist_clock_r11::touches(c,b.intent.sample,side,b.intent.epoch,
        b.intent.request,b.intent.player,character,now);
}
