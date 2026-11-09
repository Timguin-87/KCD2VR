// V147: the legacy F3=3 alignment/sweep path retains its conservative gate.
// The newly verified wrapper admission belongs only to the F12=7 predictor.
// Exact-retail, F3=3 additions. Included after native_combat104.inl.
// No action factories, clip timers, actor transforms or damage contexts are written.
// C25: kcd2vr::align24 (vrik/align_veto_receipts24.h) is included at file scope by frik_pose_probe.cpp.
using NativeAlignmentQuery112=bool(__fastcall*)(void*);
static NativeAlignmentQuery112 g_alignment_original112=nullptr;
static std::optional<DetourModKit::hook::Hook> g_alignment_hook112;
// C36 exact caller A70FA8 discards the return of14095E8; the two native
// arguments are manager and queued command. Never invoke an SDK vtable here.
using NativePositionCommand36=void(__fastcall*)(void*,const void*);
static NativePositionCommand36 g_position_original36=nullptr;
static std::optional<DetourModKit::hook::Hook> g_position_hook36;
static std::atomic<std::uint64_t> g_position_veto36{0};
// D40: a separate queued self-rotation command can start the mover before the
// late alignment query. Hook its builder AFTER the wrapper's native opponent
// alignment check; the exact two-argument caller discards the builder return.
using NativeRotationCommandD40=void(__fastcall*)(void*,const void*);
static NativeRotationCommandD40 g_rotation_original_d40=nullptr;
static std::optional<DetourModKit::hook::Hook> g_rotation_hook_d40;
static std::atomic<bool> g_no_turn_d40{true};
static std::atomic<std::uintptr_t> g_rotation_contract_d40{0};
static std::atomic<std::uint64_t> g_rotation_veto_d40{0};
static std::atomic<std::uint64_t> g_alignment_skips112{0},g_trajectory_writes112{0};
// Caller, context, producer, coherent frame, socket, pivot, discontinuity, warmup.
static std::array<std::atomic<std::uint64_t>,8> g_trajectory_reject112{};
static std::atomic<std::uintptr_t> g_motion_contract112{0};

// R125: Henry attack root-motion veto (see RootMotionMid125 below).
static std::optional<DetourModKit::hook::Hook> g_root_motion_hook125;
static std::atomic<std::uintptr_t> g_root_motion_contract125{0};
static std::atomic<std::uint64_t> g_root_motion_frames125{0};
// R139: Henry's last R112 alignment-veto decision (game thread, the 9E2D55 query of his own manager) for the
// R125 exit. R138 log 7 Nov 08:51-08:53 (F2+F3 on): the R112 veto passed on every practice-sword attack
// (C24 vetoed 47..103 per attack) but the R125 exit, which re-ran the same proof itself, never owned one frame
// (0 R125 rows) and the lunge clips moved Henry 46..70 cm. the exit now follows the decision the game thread
// already made for this frame: same owner, same refusals, nothing new to pass.
static std::atomic<std::uintptr_t> g_root_lease_animated139{0},g_root_lease_state139{0};
static std::atomic<std::uint64_t> g_root_lease_tick139{0};
inline constexpr std::uint64_t kRootLeaseMs139=60; // ~5 frames; the query runs every frame of an attack
// R160: the counter-step (CounterStepAfterUpdate160, pure rules in counter_step_r160.h) follows the same decision of
// this frame, but WITHOUT the R139 C_Player+268 read that no log has ever confirmed: tick of Henry's last vetoed query.
static std::atomic<std::uint64_t> g_counter_lease_tick160{0};
// R160: the player's own walking (final pad left stick, after roomscale follow / dodge injection), from the pad worker.
static std::atomic<float> g_move_stick_lx160{0.f},g_move_stick_ly160{0.f};
static std::atomic<std::uint64_t> g_move_stick_tick160{0};

// D42 selection changes no native eligibility or target policy. Existing
// ordinary-action and Henry ownership proofs still decide the actual veto.
static bool NativeNoTurnSelectedD42() noexcept {
    return g_no_turn_d40.load(std::memory_order_acquire)||
        kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire);
}
// Cheap published ownership only. No native animation census, late tracking
// update, hand-valid requirement or render/camera rewrite is introduced here.
static bool NativeFreeViewContextD42() noexcept {
    const auto generation=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto context=[&]() noexcept {
        return !(generation&1)&&g_gameplay_allowed.load(std::memory_order_acquire)&&
            !diagnostics::lab::NativeReference()&&!g_ctx_dialogue.load(std::memory_order_acquire)&&
            !g_ctx_menu.load(std::memory_order_acquire)&&!g_ctx_cutscene.load(std::memory_order_acquire)&&
            !g_ctx_scripted.load(std::memory_order_acquire)&&
            !animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&
            g_head_valid.load(std::memory_order_acquire)&&
            g_pose_policy_generation57.load(std::memory_order_acquire)==generation;
    };
    if(!context())return false;
    TrackingSnapshot t{};const auto now=std::uint64_t(GetTickCount64());
    return ReadTrackingSnapshot(t)&&t.allowed&&t.head_valid&&!t.head_held&&t.tick&&
        now>=t.tick&&now-t.tick<=60&&context();
}

// R59C3: visual movement ownership is a current-action question, not a
// damage-ticket question. Read the exact native main channel and backlinks.
// R114: anyPunchR114 = the live main action is any ordinary native punch (5/6/7) without a tracked punch binding
// (free targeting only, see NativeFistMotionD37). kicks/clinch/masterstrike rows still fail here.
static bool CurrentOrdinaryMotion59c3(const NativeOwner104& owner,const kcd2vr::fist_action_h12::Binding* fistD37=nullptr,
    bool anyPunchR114=false) noexcept {
    auto read=ClockRead50;const auto base=WhBase(),actor=reinterpret_cast<std::uintptr_t>(owner.actor);
    const auto eq=[&](std::uintptr_t address,std::uintptr_t value){std::uintptr_t got=0;return read(address,&got,8)&&got==value;};
    std::uintptr_t manager=0,slots=0,action=0,helper=0,row=0,type=0;
    int channel=-1,kind=-1;unsigned char active=0;
    if(!read(actor+0x200,&manager,8)||!manager||!read(manager+0x38,&slots,8)||!slots||
       !read(slots,&action,8)||!action||!eq(action,base+0x3A61C58)||!eq(action+0x40,manager)||
       !eq(action+0x70,actor)||!read(action+0x3C,&channel,4)||channel!=0||
       !read(action+0x28,&active,1)||active!=1||!read(action+0xA0,&helper,8)||!helper||
       !eq(helper,base+0x3A61040)||!eq(helper+8,actor)||!eq(helper+0x20,action+0x88)||
       !eq(action+0x88,base+0x3A61440)||!read(action+0x58,&row,8)||!row||
       !read(row+0x148,&type,8)||!type||!read(type+8,&kind,4)||
       ((fistD37||anyPunchR114)?!kcd2vr::fist_action_h12::punch(kind):(kind!=0&&kind!=1)))return false;
    if(fistD37){
        int nativeHand=-1;
        if(fistD37->action!=action||fistD37->helper!=helper||fistD37->row!=row||
           fistD37->nativeType!=kind||!read(row+0x224,&nativeHand,4)||nativeHand<0||nativeHand>1||
           nativeHand!=fistD37->nativeHand)return false;
    }
    return eq(slots,action); // replacement during the read cannot acquire ownership
}
// D37: tracked boxing owns ordinary-punch alignment too. The old weapon-only
// gate rejected every FistSelected101() frame, leaving Henry's automatic step
// enabled even with VrCombatNoStep=true. Reuse the current empty-hand owner,
// completed hand/body packet and the existing exact native main-action graph.
// No sweep, hit, reaction, kick or paired-action permission is granted here.
static bool NativeFistMotionD37(NativeOwner104& owner,void* alignmentManager,std::uint8_t* why,
    bool rotationD40=false) noexcept {
    namespace ar=kcd2vr::align24;
    const auto fail=[why](ar::Refusal r){if(why)*why=r;return false;};
    const auto now=std::uint64_t(GetTickCount64());
    if(!alignmentManager||!(rotationD40?NativeNoTurnSelectedD42():g_no_step55.load(std::memory_order_relaxed)))
        return fail(rotationD40?ar::NoTurnDisabled:ar::NoStepDisabled);
    if(g_fist_fault101.load(std::memory_order_relaxed))return fail(ar::PhysFault);
    FistOwner101 hands{};
    if(!FistReadOwner101(hands)||!hands.empty||hands.epoch!=g_pose_epoch6689.load()||
       hands.request!=g_fist_epoch101.load())return fail(ar::Equipment);
    if(!NativeResolve104(hands.player,owner)||owner.epoch!=hands.epoch)return fail(ar::Resolve);
    // R103: with the free-targeting switch, empty hands in an ordinary guard/
    // block/after-strike phase also lose the automatic turn. No punch is
    // running then, so the punch binding below cannot be required; tracking,
    // exact Henry manager identity and the protected-body census still are.
    // R114 (Miki R113, fists: "free targeting is not free anymore", F3 00:42-00:43: Henry 19 turn commands,
    // 1 vetoed, 15 NOT_OWNED + 2 ORDINARY_MOTION): during his own punch (phase 4/8/16) the turn veto still
    // needed the full per-hand punch-steering proof below (H12 binding, closed grip+trigger on the punch hand,
    // fresh palm/body packets, no hand claim on EITHER hand). that proof is for the step/position owner; the
    // turn toward the locked enemy is a camera/body yaw and does not depend on which hand steers the punch.
    // with the free-targeting switch a live ordinary native punch row (5/6/7, exact main channel + backlinks)
    // is enough, like the guard phases. kick (8), clinch, masterstrike and paired rows are not punches and keep
    // the native turn; the protected-body census and Henry manager identity still apply.
    if(rotationD40&&kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire)){
        namespace ft=kcd2vr::free_targeting_d42;
        nc104::Observed guard{};
        if(NativeObserve104(owner,guard)){
            if(!ft::freeRotationPhase(guard.phase,guard.type)){
                ft::lastRefusedPhase.store(guard.phase,std::memory_order_relaxed);return fail(ar::Phase);
            }
            if(ft::attackPhase(guard.phase)&&!CurrentOrdinaryMotion59c3(owner,nullptr,true))return fail(ar::OrdinaryMotion);
            TrackingSnapshot t{};
            if(!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid)return fail(ar::TrackingSnapshot);
            if(t.head_held||t.right_held||t.left_held)return fail(ar::HandHeld);
            if(!kcd2vr::contact176::fresh(t.tick,now,80))return fail(ar::TrackingStale);
            void* actor=nullptr;void* back=nullptr;
            if(!PhysVft6698(alignmentManager,0x3A633B0)||
               !Read(static_cast<unsigned char*>(alignmentManager)+8,actor)||actor!=owner.actor||
               !Read(static_cast<unsigned char*>(owner.actor)+0x250,back)||back!=alignmentManager)return fail(ar::ManagerIdentity);
            if(animation_filter::PlayerNativeBodyRequired112())return fail(ar::BodyRequired);
            if(hands.epoch!=g_pose_epoch6689.load()||hands.request!=g_fist_epoch101.load())return fail(ar::EpochMismatch);
            return true;
        }
    }
    // H12 already binds a successfully started native action to the tracked
    // punch side. Native row hand is the authored shape, not that side: a left
    // VR punch can use nativeHand=0, then the predictor retargets its shape.
    namespace fh=kcd2vr::fist_action_h12;fh::Binding punch{};
    if(!fh::current(punch,now)||punch.intent.kind!=kcd2vr::attack_timing178r22c2::Kind::Punch||
       punch.intent.side<0||punch.intent.side>1||!punch.intent.sample||
       punch.intent.policy!=g_attack_timing_policy22.load()||punch.intent.epoch!=hands.epoch||
       punch.intent.request!=hands.request||punch.intent.player!=reinterpret_cast<std::uintptr_t>(owner.player)||
       punch.intent.actor!=reinterpret_cast<std::uintptr_t>(owner.actor)||
       punch.intent.state!=reinterpret_cast<std::uintptr_t>(owner.state))return fail(ar::OrdinaryMotion);
    const int side=punch.intent.side;
    FistContactFrameH11 palms{};CommittedFrame6695 body{};
    if(!g_fist_contact_h11.read(palms)||!palms.completedH31||!palms.valid[side]||
       palms.player!=owner.player||palms.epoch!=hands.epoch||palms.request!=hands.request||
       !palms.character||!palms.sample||palms.sample<punch.intent.sample||
       !palms.tick||now<palms.tick||now-palms.tick>80)return fail(ar::InputStale);
    if(!g_committed_frame6695.read(body)||body.stamp.epoch!=palms.epoch||body.stamp.actor!=palms.character||
       body.stamp.sample<palms.sample||!body.valid[0]||!body.valid[1])return fail(ar::NotOwned);
    if(HandClaimBlockedT24(0,palms.character,palms.epoch,g_weapon_request_epoch6694.load())||
       HandClaimBlockedT24(1,palms.character,palms.epoch,g_weapon_request_epoch6694.load()))return fail(ar::NotOwned);
    TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||
       !tracking.left_valid)return fail(ar::TrackingSnapshot);
    if(tracking.head_held||tracking.right_held||tracking.left_held)return fail(ar::HandHeld);
    if(!kcd2vr::contact176::fresh(tracking.tick,now,80))return fail(ar::TrackingStale);
    if(!((side?tracking.left_grip:tracking.right_grip)>.60f&&
         (side?tracking.left_trigger:tracking.right_trigger)>.60f))return fail(ar::NotOwned);
    nc104::Observed observed{};
    if(!NativeObserve104(owner,observed))return fail(ar::Observe);
    if(observed.phase!=4&&observed.phase!=8&&observed.phase!=16)return fail(ar::Phase);
    if(!CurrentOrdinaryMotion59c3(owner,&punch))return fail(ar::OrdinaryMotion);
    void* actor=nullptr;void* back=nullptr;
    if(!PhysVft6698(alignmentManager,0x3A633B0)||
       !Read(static_cast<unsigned char*>(alignmentManager)+8,actor)||actor!=owner.actor||
       !Read(static_cast<unsigned char*>(owner.actor)+0x250,back)||back!=alignmentManager)return fail(ar::ManagerIdentity);
    if(animation_filter::PlayerNativeBodyRequired112())return fail(ar::BodyRequired);
    fh::Binding stillCurrent{};
    if(!fh::current(stillCurrent,now)||stillCurrent.generation!=punch.generation||
       stillCurrent.action!=punch.action||punch.intent.policy!=g_attack_timing_policy22.load()||
       hands.epoch!=g_pose_epoch6689.load()||hands.request!=g_fist_epoch101.load())return fail(ar::OrdinaryMotion);
    return true;
}
static bool NativeMotionContext112(PhysFrame6698& f,NativeOwner104& owner,void* alignmentManager=nullptr,
    std::uint8_t* why=nullptr,bool rotationD40=false) noexcept {
    // C25: `why` only names the first failing condition (receipts). Conditions,
    // their order and the result are unchanged from C4R3.
    namespace ar=kcd2vr::align24;
    const auto fail=[why](ar::Refusal r){if(why)*why=r;return false;};
    const auto now=std::uint64_t(GetTickCount64());
    // R51C3: the old gate required F12=6, but F4's hybrid uses F12=7.
    // Its admitted action (not pending input) owns this narrow alignment veto.
    const bool contactMode=alignmentManager&&g_combat_experiment175.load()&&g_physical_combat177.load()&&
        combat_diag_probe::GeometryLiveAction178r35();
    // D42 is rotation-only. This opt-in presentation boundary does not need
    // the separate contact/damage experiment; all current-action, tracked
    // body/equipment and exact Henry ownership proofs below still apply.
    const bool freeRotationD42=rotationD40&&alignmentManager&&
        kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire);
    const bool currentAction=contactMode||freeRotationD42;
    if(currentAction){ // Reject NPC managers before tracking/census/current-action inspection.
        void* actor=nullptr;void* player=nullptr;void* entity=nullptr;
        if(!Read(static_cast<unsigned char*>(alignmentManager)+8,actor)||!PhysVft6698(actor,0x3A80610)||
           !Read(static_cast<unsigned char*>(actor)+0x1F8,player)||!PhysVft6698(player,0x3AA4B98)||
           !Read(static_cast<unsigned char*>(player)+0x38,entity)||entity!=g_last_player_entity.load())return fail(ar::ContactIdentity);
    }
    const bool hybrid=contactMode;
    if(!currentAction&&diagnostics::lab::Get(diagnostics::lab::Key::F3)!=3)return fail(ar::Mode);
    if(diagnostics::lab::NativeReference())return fail(ar::LabReference);
    if(!retail_build112::Ready(WhBase())||g_motion_contract112.load()!=WhBase())return fail(ar::Contract);
    if(!g_phys_armed6698.load()||g_phys_fault6698.load()||g_native_fault104.load())return fail(ar::PhysFault);
    const auto weaponMode=g_weapon_control_mode.load();
    if((freeRotationD42?(weaponMode!=6&&weaponMode!=7):(weaponMode!=(hybrid?7:6)))||
       (!currentAction&&FistSelected101()))return fail(ar::WeaponMode);
    if(!g_gameplay_allowed.load()||
       g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_menu.load())return fail(ar::GameContext);
    if(!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load())return fail(ar::HandTracking);
    if(animation_filter::nativeBodyOwned112.load())return fail(ar::NativeBodyOwned);
    const auto poseTick=animation_filter::combatPoseTick112.load();
    if(!poseTick||now<poseTick||now-poseTick>80)return fail(ar::PoseStale);
    if(currentAction&&FistSelected101())return NativeFistMotionD37(owner,alignmentManager,why,rotationD40);
    if(!g_phys_frame6698.read(f))return fail(ar::FrameUnread);
    if(!f.input.allowed||!f.input.attacks||!f.input.sample||now<f.input.tick||now-f.input.tick>80)return fail(ar::InputStale);
    if(f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load())return fail(ar::EpochMismatch);
    if(!f.input.tracked[0]||!f.input.tracked[1]||!f.output.owned[0])return fail(ar::NotOwned);
    // The exact alignment query may keep visible ownership through checked collision recovery.
    // Native sweeps call with alignmentManager=null and retain the active-pose requirement.
    if(!f.output.active[0]&&!(hybrid&&CombatSynchronized59c10r9()&&f.output.handlingOnly55c1[0]))return fail(ar::OutputInactive);
    // V66.135: our optional world-contact query is NOT eligibility for KCD2's
    // own A7 weapon sweep. UNKNOWN contact must not suppress the native sweep.
    if(!f.profiles[0].primary||!f.profiles[0].melee())return fail(ar::Profile);
    if(!NativeResolve104(f.player,owner))return fail(ar::Resolve);
    nc104::Observed state{};
    // Only ordinary slash/thrust action states. Specials, paired actions and
    // idle locomotion cannot acquire the alignment/trajectory experiment.
    if(!NativeObserve104(owner,state))return fail(ar::Observe);
    // C48: +218 is the pending/request type; it can change while the admitted
    // ordinary main action still owns its mover. HYBRID uses the exact live
    // action row/helper/backlinks below. Legacy trajectory keeps its old gate.
    if(!currentAction&&state.type!=0&&state.type!=1)return fail(ar::ActionType);
    if(!currentAction&&!state.requestCandidate)return fail(ar::NoCandidate);
    if(currentAction){
        // R103: free targeting was only free during Henry's own slash/thrust
        // (phases 4/8/16). In guard, block, after-strike and idle-guard phases
        // the queued native turn toward the locked opponent stayed (R101 F3:
        // PHASE:15 of Henry's 36 rotation commands). With the free-targeting
        // switch those ordinary phases are vetoed too; there is no attack
        // action to prove then, so the ordinary-attack proof is only kept for
        // attack phases. Paired/protected bodies are still refused below.
        namespace ft=kcd2vr::free_targeting_d42;
        const bool freeGuardD42=freeRotationD42&&!ft::attackPhase(state.phase)&&
            ft::freeRotationPhase(state.phase,state.type);
        if(!ft::attackPhase(state.phase)&&!freeGuardD42){
            if(freeRotationD42)ft::lastRefusedPhase.store(state.phase,std::memory_order_relaxed);
            return fail(ar::Phase);
        }
        if(!freeGuardD42&&!CurrentOrdinaryMotion59c3(owner))return fail(ar::OrdinaryMotion);
        TrackingSnapshot t{};
        if(!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid)return fail(ar::TrackingSnapshot);
        if(t.head_held||t.right_held||t.left_held)return fail(ar::HandHeld);
        if(!kcd2vr::contact176::fresh(t.tick,now,80))return fail(ar::TrackingStale);
    }
    void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    const bool equipment=Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)&&manager==f.manager&&
        manager==g_henry_item_attach_manager.load()&&PhysVft6698(manager,0x3EC9958)&&
        Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==owner.player&&
        NativeHandKey6697(manager,0,key)&&key==f.profiles[0].key&&key;
    if(!equipment)return fail(ar::Equipment);
    // Cheap eligibility/identity rejection precedes the live animation census.
    // The native query is global; other actors must not trigger Henry's census.
    if(alignmentManager){
        if(!(rotationD40?NativeNoTurnSelectedD42():g_no_step55.load(std::memory_order_relaxed)))
            return fail(rotationD40?ar::NoTurnDisabled:ar::NoStepDisabled);
        void* actor=nullptr;void* alignmentBack=nullptr;
        if(!PhysVft6698(alignmentManager,0x3A633B0)||
           !Read(static_cast<unsigned char*>(alignmentManager)+8,actor)||actor!=owner.actor||
           !Read(static_cast<unsigned char*>(owner.actor)+0x250,alignmentBack)||alignmentBack!=alignmentManager)return fail(ar::ManagerIdentity);
    }
    if(animation_filter::PlayerNativeBodyRequired112())return fail(ar::BodyRequired);
    return true;
}
static bool NativeAlignmentOwned112(void* manager,std::uint8_t* why=nullptr) noexcept {
    namespace ar=kcd2vr::align24;
    const auto fail=[why](ar::Refusal r){if(why)*why=r;return false;};
    PhysFrame6698 f{};NativeOwner104 owner{};void* back=nullptr;void* actor=nullptr;
    if(!NativeMotionContext112(f,owner,manager,why))return false;
    if(!PhysVft6698(manager,0x3A633B0)||
       !Read(static_cast<unsigned char*>(manager)+8,actor)||actor!=owner.actor||
       !Read(static_cast<unsigned char*>(owner.actor)+0x250,back)||back!=manager)return fail(ar::ManagerIdentity);
    std::uintptr_t begin=0,end=0,capacity=0;std::uint32_t henry=0;
    auto p=static_cast<unsigned char*>(manager);
    if(!Read(static_cast<unsigned char*>(owner.player)+0x30,henry)||!henry)return fail(ar::HenryId);
    // R112 (Miki R111: "henry was also stepping forward", VrCombatNoStep=true): in BOTH real logs Henry's
    // manager has NO alignment records when 9E37A8 answers true - every C24 receipt in an attack phase says
    // maxRecords=0 firstRefusal=RECORD_VECTOR vetoed=0 (R111 lines 433442/435644/464605, R106 395249/397755/401074),
    // so this veto has never fired and every ordinary attack carried him forward (C38 ATTACK_STEP 31-212 cm in
    // R111, up to 296 cm in R106). the query runs before 9E2D55 builds this attack's records. an EMPTY vector
    // (begin==end, also both null) has no foreign registration to protect: the ordinary-action / Henry / manager /
    // protected-body proofs above decide. a non-empty vector is still checked record by record as before.
    if(!Read(p+0x58,begin)||!Read(p+0x60,end)||!Read(p+0x68,capacity))return fail(ar::RecordVector);
    const bool emptyR112=begin==end&&end<=capacity;
    if(!emptyR112&&(!begin||end<begin||end>capacity||(end-begin)%0x20))return fail(ar::RecordVector);
    if((end-begin)/0x20>16)return fail(ar::RecordCount);
    // +14098D8 constructs these records. The byte is a TYPE, not a bool:
    // 1 combined-parent, 2 attacker-position, 3 attacker-rotation.
    constexpr std::uintptr_t vft[]{0,0x3A62098,0x3BAD328,0x3A622C8};
    for(auto entry=begin;entry<end;entry+=0x20){
        unsigned char type=0;std::uint32_t target=0;void* clip=nullptr;
        if(!Read(reinterpret_cast<void*>(entry+0x10),type)||type<1||type>3)return fail(ar::RecordType);
        if(!Read(reinterpret_cast<void*>(entry+0x0C),target)||target!=henry)return fail(ar::RecordTarget);
        if(!Read(reinterpret_cast<void*>(entry+0x18),clip)||!PhysVft6698(clip,vft[type]))return fail(ar::RecordVtable);
    }
    std::uintptr_t again=0;
    if(Read(p+0x58,again)&&again==begin&&Read(p+0x60,again)&&again==end)return true;
    return fail(ar::RecordRace);
}

// C36: the late9E37A8 query runs AFTER the queued position command can
// start/commit the mover. Skip only that exact Henry command before it performs
// ANY mover side effect. Other commands, NPCs, callers and protected actions
// retain their original function. The original queue walker consumes its own
// entry; we do not erase a vector or patch an actor transform.
static bool NativePositionCommandOwned36(void* manager,const void* command,std::uintptr_t caller) noexcept {
    const auto b=WhBase();
    // R113 (from chatgpt R112): the current integrated native attack owns automatic position
    // in both supported contact policies. The synchronized hit-timing bit is
    // unrelated to this exact before-mover command; legacy lanes stay excluded.
    if(!b||caller!=b+0xA70FA8||!g_combat_experiment175.load()||
       !CombatIntegrated178r22c2Enabled()||!g_no_step55.load()||!manager||!command)return false;
    ReadScope101R2 scope;PhysFrame6698 f{};NativeOwner104 owner{};
    // Includes exact retail contract, Henry backlinks, current ordinary main
    // action/type, fresh physical owner, equipment and protected-body census.
    if(!NativeMotionContext112(f,owner,manager))return false;
    std::uintptr_t begin=0,end=0,capacity=0,again=0;unsigned char kind=0;
    const auto q=reinterpret_cast<std::uintptr_t>(command);
    auto p=static_cast<unsigned char*>(manager);
    if(!Read(p+0x30,begin)||!Read(p+0x38,end)||!Read(p+0x40,capacity)||
       !begin||end<=begin||end>capacity||(end-begin)%0x30||(end-begin)/0x30>64||
       q<begin||q>=end||(q-begin)%0x30||!Read(command,kind)||kind!=2)return false;
    return Read(p+0x30,again)&&again==begin&&Read(p+0x38,again)&&again==end;
}
static void NativePositionDispatch36(void* manager,const void* command,std::uintptr_t caller) noexcept {
    if(NativePositionCommandOwned36(manager,command,caller)){++g_position_veto36;return;}
    if(g_position_original36)g_position_original36(manager,command);
}
static void __fastcall NativePositionDetour36(void* manager,const void* command) noexcept {
    NativePositionDispatch36(manager,command,reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
}

// D40: kind3 in the same native queue calls14097AC from A70F96. That wrapper
// keeps its native opponent alignment/priority query (A716E0), then calls the
// builder14097F0 at14097DD. Suppress only at that builder return address AFTER
// native eligibility, before the rotation constructor14098B0 and SELF mover
// route A71AB8. Other builder callers, including combined commands, stay native.
// The native queue still consumes its own entry.
// Opponent selection, native attack eligibility, combined/paired commands and
// every unqualified caller remain native. No actor/camera transform is written.
static bool NativeRotationCommandOwnedD40(void* manager,const void* command,std::uintptr_t caller,std::uint8_t* why=nullptr) noexcept {
    namespace ar=kcd2vr::align24;
    const auto fail=[why](ar::Refusal r){if(why)*why=r;return false;};
    const auto b=WhBase();
    if(!b||caller!=b+0x14097E2||g_rotation_contract_d40.load(std::memory_order_acquire)!=b)return fail(ar::Contract);
    if(!NativeNoTurnSelectedD42())return fail(ar::NoTurnDisabled);
    if(!g_combat_experiment175.load()&&
       !kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire))return fail(ar::Mode);
    if(!manager||!command)return fail(ar::ManagerIdentity);
    const auto freePolicyD42=kcd2vr::free_targeting_d42::generation.load(std::memory_order_acquire);
    // Sample the worker-owned revocation domains before admission. The context
    // generation is odd while PublishGameStateContext is changing its flags.
    // The final join below narrows the decision window; it is not a lock or an
    // atomic transaction with the independent context/input publishers.
    const auto contextD40=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto poseEpochD40=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto weaponRequestD40=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    const bool fistD40=FistSelected101();
    const auto fistRequestD40=fistD40?g_fist_epoch101.load(std::memory_order_acquire):0;
    const auto weaponModeD40=g_weapon_control_mode.load(std::memory_order_acquire);
    if(contextD40&1)return fail(ar::GameContext);
    ReadScope101R2 scope;PhysFrame6698 f{};NativeOwner104 owner{};
    // All existing ordinary-action, live tracking, exact identity, equipment
    // and protected-body proofs apply. Only this command's policy is separate
    // from the position/late-query VrCombatNoStep setting. Both F4 modes may
    // qualify a currently active ordinary action; recovery-only output keeps
    // the existing synchronized-mode requirement inside the context predicate.
    if(!NativeMotionContext112(f,owner,manager,why,true))return false;
    std::uintptr_t begin=0,end=0,capacity=0,again=0;unsigned char kind=0;
    const auto q=reinterpret_cast<std::uintptr_t>(command);
    auto p=static_cast<unsigned char*>(manager);
    if(!Read(p+0x30,begin)||!Read(p+0x38,end)||!Read(p+0x40,capacity)||
       !begin||end<=begin||end>capacity||(end-begin)%0x30||(end-begin)/0x30>64||
       q<begin||q>=end||(q-begin)%0x30||!Read(command,kind)||kind!=3)return fail(ar::RecordVector);
    const bool currentD42=Read(p+0x30,again)&&again==begin&&Read(p+0x38,again)&&again==end&&
        g_rotation_contract_d40.load(std::memory_order_acquire)==b&&
        NativeNoTurnSelectedD42()&&
        kcd2vr::free_targeting_d42::generation.load(std::memory_order_acquire)==freePolicyD42&&
        (g_combat_experiment175.load(std::memory_order_acquire)||
         kcd2vr::free_targeting_d42::enabled.load(std::memory_order_acquire))&&
        g_weapon_control_mode.load(std::memory_order_acquire)==weaponModeD40&&
        FistSelected101()==fistD40&&
        (!fistD40||g_fist_epoch101.load(std::memory_order_acquire)==fistRequestD40)&&
        g_gameplay_allowed.load(std::memory_order_acquire)&&
        !g_ctx_dialogue.load(std::memory_order_acquire)&&!g_ctx_menu.load(std::memory_order_acquire)&&
        !g_ctx_cutscene.load(std::memory_order_acquire)&&!g_ctx_scripted.load(std::memory_order_acquire)&&
        g_head_valid.load(std::memory_order_acquire)&&g_right_valid.load(std::memory_order_acquire)&&
        g_left_valid.load(std::memory_order_acquire)&&!animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&
        g_pose_epoch6689.load(std::memory_order_acquire)==poseEpochD40&&
        g_weapon_request_epoch6694.load(std::memory_order_acquire)==weaponRequestD40&&
        g_pose_policy_generation57.load(std::memory_order_acquire)==contextD40;
    return currentD42?true:fail(ar::RecordRace);
}
static void NativeRotationDispatchD40(void* manager,const void* command,std::uintptr_t caller) noexcept {
    std::uint8_t why=kcd2vr::align24::None;
    const bool veto=NativeRotationCommandOwnedD40(manager,command,caller,&why);
    // All records describe this pinned builder caller; refusals may be NPCs or
    // protected owners. Do not relabel all candidates as Henry observations.
    if(caller==WhBase()+0x14097E2&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())){
        namespace ft=kcd2vr::free_targeting_d42;
        ft::recordRotation(veto,why);
        if(!veto&&why==kcd2vr::align24::Phase)
            ft::keptPhaseMask.fetch_or(ft::phaseBit(ft::lastRefusedPhase.load(std::memory_order_relaxed)),std::memory_order_relaxed);
    }
    if(veto){++g_rotation_veto_d40;return;}
    if(g_rotation_original_d40)g_rotation_original_d40(manager,command);
}
static void __fastcall NativeRotationDetourD40(void* manager,const void* command) noexcept {
    NativeRotationDispatchD40(manager,command,reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
}

// C25 receipts: per hidden attack, how Henry's alignment query was answered.
// Game-thread side copies one POD into a try-locked ledger (no formatting/I/O);
// ServiceAlignVetoReceipts25 formats bounded edge lines from the service tick.
static kcd2vr::align24::Ledger g_align_ledger25;
static std::atomic_flag g_align_ledger_lock25=ATOMIC_FLAG_INIT;
static std::atomic<std::uint64_t> g_align_ledger_busy25{0};
static bool AlignVetoReceiptsActive25() noexcept {
    return diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
}
static void AlignVetoReceipt25(void* manager,std::uintptr_t caller,bool original,bool vetoed,std::uint8_t why) noexcept {
    ReadScope101R2 scope;const auto b=WhBase();
    void* actor=nullptr;void* player=nullptr;void* entity=nullptr;
    // Henry only: the query is global and NPC managers are not receipts.
    if(!b||!manager||!PhysVft6698(manager,0x3A633B0)||
       !Read(static_cast<unsigned char*>(manager)+8,actor)||!PhysVft6698(actor,0x3A80610)||
       !Read(static_cast<unsigned char*>(actor)+0x1F8,player)||!PhysVft6698(player,0x3AA4B98)||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||!entity||entity!=g_last_player_entity.load())return;
    kcd2vr::align24::Call c{};
    c.tick=GetTickCount64();c.original=original;c.atVetoCaller=caller==b+0x9E2D55;c.vetoed=vetoed;
    c.refusal=(original&&c.atVetoCaller&&!vetoed)?why:kcd2vr::align24::None;
    c.key=g_attack_timing_consumed22.load(std::memory_order_relaxed);
    std::uintptr_t actionManager=0,slots=0,action=0;
    if(Read(static_cast<unsigned char*>(actor)+0x200,actionManager)&&actionManager&&
       Read(reinterpret_cast<void*>(actionManager+0x38),slots)&&slots&&Read(reinterpret_cast<void*>(slots),action))c.action=action;
    NativeOwner104 owner{};nc104::Observed state{};
    if(NativeResolve104(player,owner)&&NativeObserve104(owner,state))c.phase=state.phase;
    std::uintptr_t begin=0,end=0;const auto p=static_cast<unsigned char*>(manager);
    if(Read(p+0x58,begin)&&Read(p+0x60,end)&&begin&&end>=begin&&!((end-begin)%0x20)){
        constexpr std::uintptr_t vft[]{0,0x3A62098,0x3BAD328,0x3A622C8};
        const auto n=std::min<std::uintptr_t>((end-begin)/0x20,64);c.records=std::uint32_t((end-begin)/0x20);
        for(std::uintptr_t i=0;i<n;++i){
            const auto entry=begin+i*0x20;unsigned char type=0;void* clip=nullptr;std::uintptr_t vt=0;
            const bool typed=Read(reinterpret_cast<void*>(entry+0x10),type);
            const bool clipped=Read(reinterpret_cast<void*>(entry+0x18),clip)&&clip&&Read(clip,vt);
            if(typed&&type>=1&&type<=3&&clipped&&vt==b+vft[type])continue;
            if(!c.unexpected++){c.unexpectedType=typed?type:0xFFu;c.unexpectedVft=clipped&&vt>=b?vt-b:0;}
        }
    }
    if(g_align_ledger_lock25.test_and_set(std::memory_order_acquire)){++g_align_ledger_busy25;return;}
    g_align_ledger25.Observe(c);g_align_ledger_lock25.clear(std::memory_order_release);
}
static void ServiceAlignVetoReceipts25() noexcept {
    namespace ar=kcd2vr::align24;
    static ar::LineBudget budget;
    const auto now=std::uint64_t(GetTickCount64());const bool active=AlignVetoReceiptsActive25();
    static std::uint64_t reported36=0;static unsigned lines36=0;
    const auto vetoes36=g_position_veto36.load(std::memory_order_relaxed);
    if(active&&vetoes36!=reported36&&lines36<16){
        char m[180]{};std::snprintf(m,sizeof(m),"C36 HYBRID_POSITION_VETO count=%llu caller=A70FA8 command=2 owner=HENRY_ORDINARY beforeMoverStart=1",
            (unsigned long long)vetoes36);diagnostic_trace::Write(m);reported36=vetoes36;++lines36;
    }
    ar::Attack done[3]{};unsigned n=0;ar::Totals tot{};
    if(g_align_ledger_lock25.test_and_set(std::memory_order_acquire))return;
    if(!active)g_align_ledger25.Discard(); // no stale edges after the F3 lease ends
    else {
        g_align_ledger25.Expire(now,300);
        while(n<3&&g_align_ledger25.pending()&&budget.Take(now,3)&&g_align_ledger25.Pop(done[n]))++n;
        tot=g_align_ledger25.totals();
    }
    g_align_ledger_lock25.clear(std::memory_order_release);
    for(unsigned i=0;i<n;++i){
        const auto& a=done[i];char line[512]{};
        std::snprintf(line,sizeof(line),
            "C24 ALIGN_VETO_COVERAGE key=%llu action=%llX calls=%u trueAnswers=%u vetoed=%u refused=%u firstRefusal=%s "
            "foreignCaller=%u unexpectedRecords=%u unexpectedType=%u unexpectedVft=%llX maxRecords=%u phaseSeen=0x%X ms=%llu "
            "totals(attacks/calls/true/vetoed/refused/unexpectedCalls/dropped/busy)=%llu/%llu/%llu/%llu/%llu/%llu/%llu/%llu decision=UNCHANGED",
            (unsigned long long)a.key,(unsigned long long)a.action,a.calls,a.trueAnswers,a.vetoed,a.refused,
            a.firstRefusal==ar::None?"NONE":ar::RefusalName(a.firstRefusal),a.foreignCaller,a.unexpectedRecords,a.unexpectedType,
            (unsigned long long)a.unexpectedVft,a.maxRecords,a.phaseSeen,(unsigned long long)(a.last>=a.first?a.last-a.first:0),
            (unsigned long long)tot.attacks,(unsigned long long)tot.calls,(unsigned long long)tot.trueAnswers,(unsigned long long)tot.vetoed,
            (unsigned long long)tot.refused,(unsigned long long)tot.unexpectedCalls,(unsigned long long)tot.droppedAttacks,
            (unsigned long long)g_align_ledger_busy25.load());
        diagnostic_trace::Write(line);
    }
}

static bool NativeAlignmentResult112(void* manager,std::uintptr_t caller,bool original) noexcept {
    bool vetoed=false;std::uint8_t why=kcd2vr::align24::None;
    if(original&&caller==WhBase()+0x9E2D55){
        ReadScope101R2 scope;
        if(NativeAlignmentOwned112(manager,&why)){++g_alignment_skips112;++g_alignment_skips55;vetoed=true;}
        // R139: publish Henry's decision for the R125 root-motion exit (a refusal revokes at once). NPC managers
        // (the query is global) never touch it. read-only, no native write.
        void* actor=nullptr;void* player=nullptr;void* entity=nullptr;std::uintptr_t animated=0;NativeOwner104 owner{};
        if(manager&&PhysVft6698(manager,0x3A633B0)&&Read(static_cast<unsigned char*>(manager)+8,actor)&&
           PhysVft6698(actor,0x3A80610)&&Read(static_cast<unsigned char*>(actor)+0x1F8,player)&&
           PhysVft6698(player,0x3AA4B98)&&Read(static_cast<unsigned char*>(player)+0x38,entity)&&entity&&
           entity==g_last_player_entity.load()){
            g_root_lease_tick139.store(0,std::memory_order_release);
            g_counter_lease_tick160.store(vetoed?std::uint64_t(GetTickCount64()):0,std::memory_order_release); // R160: refusal revokes at once
            if(vetoed&&Read(static_cast<unsigned char*>(player)+0x268,animated)&&animated&&
               NativeResolve104(player,owner)&&owner.actor==actor&&owner.state){
                g_root_lease_animated139.store(animated,std::memory_order_relaxed);
                g_root_lease_state139.store(reinterpret_cast<std::uintptr_t>(owner.state),std::memory_order_relaxed);
                g_root_lease_tick139.store(GetTickCount64(),std::memory_order_release);
            }
        }
    }
    // Receipts observe the decision above; they never feed back into it.
    if(AlignVetoReceiptsActive25())AlignVetoReceipt25(manager,caller,original,vetoed,why);
    return vetoed?false:original;
}
static bool __fastcall NativeAlignmentDetour112(void* manager) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::Alignment};
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    // Query side effects and native exclusions run even when Henry owns motion.
    bool original=false;{kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::Alignment};original=g_alignment_original112?g_alignment_original112(manager):false;}
    kcd2vr::hookcost178r22::Scope policy{kcd2vr::hookcost178r22::AlignmentPolicy}; // R22C1: policy part separate from the game's query
    return NativeAlignmentResult112(manager,caller,original);
}
// R125 (Miki R124 6 Nov: "henry was still moving forward too ... it is recurring bug", VrCombatNoStep=true).
// R124 F3 log: the late alignment query WAS vetoed on Henry's slashes (C24 vetoed=49..128 per attack, for example
// key=5 18:00:27.987 vetoed=53/53 phaseSeen=0x1C) and the queued position command too (C36 count 1..16, one per
// attack), and Henry still travelled C38 forwardCm=45..126 (18:00:16.170 61.6, 18:00:27.672 66.8, 18:00:33.860
// 126.0 = 100 cm in the 200 ms hit window, 5 m/s - no walk is that fast). that is the attack clip's own root
// motion: the attack fragments run CAnimatedCharacter (vft 3A49468, C_Player+268) with an animation-driven
// movement control method, so CalculateWantedEntityMovement 839FF4 hands the clip's root translation (MergeMCM
// D73724, plus any ForceMovement +1B8/+1BC) to the physics request 8A3188. nothing of that passes the alignment
// or the command queue we veto.
// change: at 83A1A0 (the common exit, after merge/clamp/forced movement) for Henry ONLY, while the existing
// no-step owner proofs pass (R139: the alignment veto's own decision of this frame, see RootMotionOwned125: F4 contact mode, Henry manager
// identity, protected-body census, tracking) AND his live main action is an ordinary slash/thrust (or a punch
// with fists) in phase 4/8/16, the horizontal part of the wanted movement becomes the game's own requested
// entity movement (+67C, the movement controller / stick request). rotation and height stay native, so turning,
// slopes, feet and gravity are untouched; deliberate walking (stick, roomscale) is the requested movement and
// still moves him. kicks, clinch, masterstrike, paired and hit reactions fail CurrentOrdinaryMotion59c3 or the
// body census and keep the clip motion. "Attack step veto" OFF = native. evaluated every frame, no latch.
static bool RootMotionOwned125(std::uintptr_t animated,int& phase) noexcept {
    phase=0;const auto b=WhBase();
    if(!b||!animated||!g_no_step55.load(std::memory_order_relaxed))return false;
    std::uintptr_t vft=0;void* entity=nullptr;
    const auto a=reinterpret_cast<const unsigned char*>(animated);
    if(!Read(a,vft)||vft!=b+0x3A49468||!Read(a+0x38,entity)||!entity||
       entity!=g_last_player_entity.load(std::memory_order_acquire))return false;
    // R139: the owner proof is the R112 alignment veto of this frame (NativeAlignmentResult112): Henry's
    // NativeMotionContext112 (F4 contact mode, Henry manager identity, tracking, equipment, protected-body census,
    // current ordinary slash/thrust or tracked punch) plus the record check. a refusal there (kick, clinch,
    // masterstrike, paired, hit reaction, guard, step switch OFF, NPC) revokes the lease, so it keeps the clip.
    const auto tick=g_root_lease_tick139.load(std::memory_order_acquire);const auto now=std::uint64_t(GetTickCount64());
    if(!tick||now<tick||now-tick>kRootLeaseMs139||g_root_lease_animated139.load(std::memory_order_relaxed)!=animated)return false;
    const auto state=g_root_lease_state139.load(std::memory_order_relaxed);int live=0;
    if(!state||!Read(reinterpret_cast<const unsigned char*>(state)+0x38,live)||
       !kcd2vr::free_targeting_d42::attackPhase(live))return false;
    if(g_root_lease_tick139.load(std::memory_order_acquire)!=tick)return false; // republished while reading
    phase=live;return true;
}
static void RootMotionMid125(DetourModKit::hook::MidContext& c) noexcept {
    using namespace DetourModKit::hook;
    const auto b=g_root_motion_contract125.load(std::memory_order_acquire);
    const auto animated=gpr(c,Gpr::Rdi),out=gpr(c,Gpr::Rbx);
    void* entity=nullptr;
    // cheap NPC rejection first: this exit runs for every animated character every frame.
    if(!b||!animated||!out||!Read(reinterpret_cast<const unsigned char*>(animated)+0x38,entity)||
       !entity||entity!=g_last_player_entity.load(std::memory_order_relaxed))return;
    ReadScope101R2 scope;
    int phase=0;unsigned char noMovement=1;float wanted[2]{},request[2]{};
    const bool owned=Read(reinterpret_cast<const unsigned char*>(animated)+0x708,noMovement)&&!noMovement&&
        RootMotionOwned125(animated,phase)&&Read(reinterpret_cast<const unsigned char*>(out)+0x10,wanted)&&
        Read(reinterpret_cast<const unsigned char*>(animated)+0x68C,request)&&
        std::isfinite(wanted[0])&&std::isfinite(wanted[1])&&std::isfinite(request[0])&&std::isfinite(request[1]);
    if(owned){
        // `out` is the caller's stack QuatT (839FF4 rdx) about to go to 8A3188: q +0, t +10.
        std::memcpy(reinterpret_cast<unsigned char*>(out)+0x10,request,sizeof(request));
        g_root_motion_frames125.fetch_add(1,std::memory_order_relaxed);
    }
    // F3 recorder only: one row per attack window with the removed clip travel (proof for the next log).
    static std::uint64_t seen=0,frames=0,first=0;static unsigned lines=0,mask=0;static double removed=0,kept=0;
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){frames=0;return;}
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    if(session!=seen){seen=session;lines=0;frames=0;}
    if(owned){
        if(!frames){first=now;removed=0;kept=0;mask=0;}
        ++frames;mask|=unsigned(phase);
        const double dx=double(wanted[0])-request[0],dy=double(wanted[1])-request[1];
        removed+=100.0*std::sqrt(dx*dx+dy*dy);
        kept+=100.0*std::sqrt(double(request[0])*request[0]+double(request[1])*request[1]);
        return;
    }
    if(frames&&lines<64){
        ++lines;char m[240]{};std::snprintf(m,sizeof(m),
            "R125 ROOT_MOTION_VETO frames=%llu ms=%llu removedClipCm=%.1f keptRequestCm=%.1f phases=0x%X total=%llu owner=HENRY_ORDINARY_ATTACK",
            (unsigned long long)frames,(unsigned long long)(now-first),removed,kept,mask,
            (unsigned long long)g_root_motion_frames125.load(std::memory_order_relaxed));
        diagnostic_trace::Write(m);
    }
    frames=0;
}
// exact bytes of 839FF4 (CalculateWantedEntityMovement), its single caller 83A7A2, the requested-movement
// QuatT (+67C, reset at 83A72B), the MergeMCM call 83A0D6, the hooked exit 83A1A0 and ForceMovement 8447B0.
static bool NativeRootMotionContract125(std::uintptr_t b) noexcept {
    if(!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;
    const unsigned char entry[20]{0x48,0x89,0x5c,0x24,0x20,0x55,0x56,0x57,0x48,0x8d,0x6c,0x24,0xb9,0x48,0x81,0xec,0xe0,0x00,0x00,0x00};
    const unsigned char noMove[10]{0x80,0xb9,0x08,0x07,0x00,0x00,0x00,0x49,0x8b,0xf0};
    const unsigned char request[19]{0x4c,0x8d,0x81,0x7c,0x06,0x00,0x00,0x4c,0x8b,0xce,0x48,0x8d,0x55,0xb7,0xe8,0x46,0x01,0x00,0x00};
    const unsigned char merge[5]{0xe8,0x49,0x96,0x53,0x00};
    const unsigned char exitBytes[21]{0x0f,0x11,0x03,0xf2,0x0f,0x11,0x4b,0x10,0x89,0x4b,0x18,0x48,0x8b,0xc3,0x48,0x8b,0x4d,0x37,0x48,0x33,0xcc};
    const unsigned char caller[5]{0xe8,0x4d,0xf8,0xff,0xff};
    const unsigned char reset[21]{0x4c,0x89,0x89,0x8c,0x06,0x00,0x00,0x44,0x89,0x89,0x84,0x06,0x00,0x00,0x4c,0x89,0x89,0x7c,0x06,0x00,0x00};
    const auto pin=[b](std::uintptr_t rva,const auto& expected){
        unsigned char actual[sizeof(expected)]{};
        return Read(reinterpret_cast<void*>(b+rva),actual)&&!std::memcmp(actual,expected,sizeof(actual));
    };
    std::uintptr_t force=0;
    return pin(0x839FF4,entry)&&pin(0x83A016,noMove)&&pin(0x83A06B,request)&&pin(0x83A0D6,merge)&&
        pin(0x83A195,exitBytes)&&pin(0x83A7A2,caller)&&pin(0x83A72B,reset)&&
        Read(reinterpret_cast<void*>(b+0x3A49468+0x2F0),force)&&force==b+0x8447B0;
}
// optional like D40: a failed arm never revokes the alignment/position hooks and is retried on the next arm call.
static bool NativeRootMotionArm125(std::uintptr_t b) noexcept {
    if(!b||g_motion_contract112.load(std::memory_order_acquire)!=b)return false;
    if(g_root_motion_contract125.load(std::memory_order_acquire)==b)return true;
    if(!NativeRootMotionContract125(b))return false;
    if(!g_root_motion_hook125){
        using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
        auto h=mid_at(MidRequest{"kcd2vr_r125_henry_attack_root_motion",Target{DetourModKit::Address{b+0x83A1A0}},options},RootMotionMid125);
        if(!h)return false;
        g_root_motion_hook125=std::move(*h);
    }
    g_root_motion_contract125.store(b,std::memory_order_release);
    if(!g_root_motion_hook125->enable()){g_root_motion_contract125.store(0,std::memory_order_release);return false;}
    diagnostic_trace::Write("R125 ROOT_MOTION_VETO armed: Henry ordinary attack phases 4/8/16 keep the requested horizontal movement, clip root translation removed (Attack step veto)");
    return true;
}
// R125: service tick retry of the optional root-motion arm every 2 s until it holds (no session latch;
// returns at once when armed or while the no-step core is not armed).
static void ServiceRootMotionArm125() noexcept {
    static std::uint64_t retry=0;const auto now=std::uint64_t(GetTickCount64());
    if(now-retry<2000)return;
    retry=now;(void)NativeRootMotionArm125(WhBase());
}
// R160 (Miki R158, VrCombatNoStep=true: "henry is still dancing forward with the strikes"). R158 log 18:19-18:22 (F2+F3 on):
// every chained follow-up (combat_*_fz*_az*_attackspecial_lngsw) and many natk moved Henry's ENTITY forward (C38 ATTACK_STEP
// 18:19:45.843 phase 16 256.9 cm -> 46.699 357.3 -> special), while the R112 alignment veto vetoed every frame of the same
// attacks (C24 key=53 vetoed=79/79 phaseSeen=0x1C) and the C36 position veto fired once per attack. the R125 exit at 83A1A0
// never owned a frame (0 R125 rows although that row is Domain::Bug and F2 was on): it rests on four disassembly facts
// (rdi = this at the exit, C_Player+268 = that this, +38 = entity, +708 = 0) that no log can confirm without the DLL.
// so this does not depend on it: after Henry's native combat update (game thread, ActorUpdateDetour), while the R112 veto
// owns his ordinary slash/thrust (phase 4/8/16) and through its phase 32, the forward entity travel beyond 5 cm from where
// the sequence started is put back with the exact CEntity::SetPosRotScale (vft+168 = 509810, the same leaf the pickup /
// brush / weapon placement use) - position x/y only, z / rotation / scale re-written as read. if R125 ever works there is
// no excess and this writes nothing. kicks (NativeKickActive175 + kind 8 fails the ordinary-motion proof), punches,
// clinch, masterstrike, paired bodies and hit reactions are never touched; the stick moves him and the anchor; the moment
// the sequence ends nothing is written (no snap back). no latch: a failed write ends only this sequence.
// (private here; the exported PublishMoveStick160 / CounterStepAfterUpdate160 wrappers sit after combat_mode_controls.inl)
static void PublishMoveStickImpl160(float lx,float ly) noexcept {
    g_move_stick_lx160.store(std::isfinite(lx)?lx:0.f,std::memory_order_relaxed);
    g_move_stick_ly160.store(std::isfinite(ly)?ly:0.f,std::memory_order_relaxed);
    g_move_stick_tick160.store(std::uint64_t(GetTickCount64()),std::memory_order_release);
}
static bool MoveStickActive160(std::uint64_t now) noexcept {
    const auto t=g_move_stick_tick160.load(std::memory_order_acquire);
    if(!t||now<t||now-t>250)return false; // no fresh pad frame = nothing pushed
    return kcd2vr::counter_step160::stickActive(g_move_stick_lx160.load(std::memory_order_relaxed),
        g_move_stick_ly160.load(std::memory_order_relaxed));
}
// SEH leaf only (no RAII object crosses it), same call shape as PropSetLocal596 / CallPlaceR100.
static bool CallSetPosRotScale160(std::uintptr_t fn,void* entity,const float* t,const float* q,const float* s) noexcept {
    __try {reinterpret_cast<EntitySetPosRotScaleFn>(fn)(entity,t,q,s,0u);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return true;
}
static void CounterStepAfterUpdateImpl160(void* combatActor) noexcept {
    namespace cs=kcd2vr::counter_step160;
    // game thread only (ActorUpdateDetour, Henry's C_CombatPlayer); no other caller touches these.
    static cs::State state{};static bool faultWindow=false;
    static std::uint64_t frames=0,clamped=0,notApplied=0,first=0,seenSession=0,targetFrames=0;static double removedCm=0,maxForwardCm=0;
    static unsigned mask=0,lines=0;
    const auto now=std::uint64_t(GetTickCount64());const auto b=WhBase();
    cs::Frame f{};f.now=now;f.lease=g_counter_lease_tick160.load(std::memory_order_acquire);
    f.enabled=b&&combatActor&&g_no_step55.load(std::memory_order_relaxed)&&
        g_motion_contract112.load(std::memory_order_acquire)==b&&retail_build112::Ready(b);
    void* entity=nullptr;Matrix34V6550 tm{};std::uintptr_t setter=0;
    if(f.enabled){
        ReadScope101R2 scope;void* player=nullptr;NativeOwner104 owner{};nc104::Observed obs{};
        if(Read(static_cast<unsigned char*>(combatActor)+0x1F8,player)&&player&&NativeResolve104(player,owner)&&
           owner.actor==combatActor&&NativeObserve104(owner,obs)){
            f.phase=obs.phase;
            if(cs::holdPhase(f.phase)&&(state.armed||(cs::leasePhase(f.phase)&&cs::leaseFresh(f.lease,now)))&&!faultWindow){
                entity=owner.entity;std::uint32_t id=0;void* parent=nullptr;void* parentEntity=nullptr;
                f.kick=animation_filter::NativeKickActive175();
                f.ordinary=!f.kick&&CurrentOrdinaryMotion59c3(owner); // kind 0/1 only: kicks (8), punches, clinch rows fail
                f.context=g_gameplay_allowed.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
                    !g_ctx_menu.load()&&!animation_filter::nativeBodyOwned112.load()&&!diagnostics::lab::NativeReference()&&
                    !kcd2vr::vr_combat111::ClinchInput55c1(now)&&!animation_filter::PlayerNativeBodyRequired112();
                // exact CEntity + setter, not parented/mounted (entity+98 -> +18, getter 83A8F4), live world TM at +58
                const bool placeable=entity&&PhysVft6698(entity,kCEntityVftRva)&&
                    Read(reinterpret_cast<void*>(b+kCEntityVftRva+0x168),setter)&&setter==b+0x509810&&
                    Read(static_cast<unsigned char*>(entity)+0x0C,id)&&id&&Read(static_cast<unsigned char*>(entity)+0x98,parent)&&
                    (!parent||(Read(static_cast<unsigned char*>(parent)+0x18,parentEntity)&&!parentEntity))&&
                    GetEntityWorldTmV6552(entity,id,tm);
                if(!placeable)f.ordinary=false;
                f.entity=reinterpret_cast<std::uintptr_t>(entity);f.x=tm.m[3];f.y=tm.m[7];f.fx=tm.m[1];f.fy=tm.m[5];
                f.stick=MoveStickActive160(now);
                {   // R164 stand-off: the opponent position Henry's own update published this frame (PublishNpcDefense153, game thread)
                    namespace fbs=kcd2vr::npc_force_block153;const auto pub=fbs::publishedMs.load(std::memory_order_acquire);
                    if(pub&&now>=pub&&now-pub<=200&&fbs::opponent.load(std::memory_order_relaxed)&&fbs::opponentPos.load(std::memory_order_relaxed)){
                        f.target=true;f.tx=fbs::opponentX.load(std::memory_order_relaxed);f.ty=fbs::opponentY.load(std::memory_order_relaxed);}
                }
            }
        }
    }
    if(!cs::holdPhase(f.phase))faultWindow=false; // a failed write ends only that sequence
    const auto r=cs::step(state,f);
    bool applied=false;
    if(r.write&&entity){
        // keep z, rotation and scale exactly as the game has them now; only the forward excess goes.
        const V3 ax{tm.m[0],tm.m[4],tm.m[8]},ay{tm.m[1],tm.m[5],tm.m[9]},az{tm.m[2],tm.m[6],tm.m[10]};
        const float scale[3]{Len(ax),Len(ay),Len(az)};const Q4 q=QFromBasis(ax,ay,az);
        const float rot[4]{q.x,q.y,q.z,q.w},pos[3]{r.x,r.y,tm.m[11]};
        bool sane=true;for(float v:scale)sane=sane&&std::isfinite(v)&&v>.5f&&v<2.f;
        for(float v:rot)sane=sane&&std::isfinite(v);
        if(!sane||!CallSetPosRotScale160(setter,entity,pos,rot,scale)){faultWindow=true;state={};}
        else {
            Matrix34V6550 after{};std::uint32_t id=0;
            applied=Read(static_cast<unsigned char*>(entity)+0x0C,id)&&GetEntityWorldTmV6552(entity,id,after)&&
                std::fabs(after.m[3]-r.x)<.01f&&std::fabs(after.m[7]-r.y)<.01f;
        }
    }
    // F3 combat recorder only: one row per held sequence (proof for the next log, nothing on screen).
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){frames=0;return;}
    const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
    if(session!=seenSession){seenSession=session;lines=0;frames=0;}
    if(state.armed){
        if(!frames){first=now;clamped=0;notApplied=0;removedCm=0;maxForwardCm=0;mask=0;targetFrames=0;}
        ++frames;if(f.target)++targetFrames;mask|=unsigned(f.phase);maxForwardCm=std::max(maxForwardCm,100.0*r.forward);
        if(r.write){++clamped;removedCm+=100.0*r.removed;if(!applied)++notApplied;}
        return;
    }
    if(frames&&lines<64){
        ++lines;char m[360]{};std::snprintf(m,sizeof(m),
            "R160 COMBAT_COUNTER_STEP frames=%llu clampedFrames=%llu notApplied=%llu removedCm=%.1f maxForwardCm=%.1f phases=0x%X ms=%llu end=%s owner=HENRY_ORDINARY_ATTACK opponentFrames=%llu standOffM=%.2f",
            (unsigned long long)frames,(unsigned long long)clamped,(unsigned long long)notApplied,removedCm,maxForwardCm,mask,
            (unsigned long long)(now-first),faultWindow?"WRITE_FAULT":"RELEASED",(unsigned long long)targetFrames,double(cs::kStandOffM));
        diagnostic_trace::Write(m);
    }
    frames=0;
}
static bool NativeMotionContract112(std::uintptr_t b) noexcept {
    if(!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;
    const unsigned char query[16]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0x41,0x08,0x48,0x8b};
    const unsigned char alignmentCall[13]{0xe8,0x53,0x0a,0,0,0x84,0xc0,0x0f,0x84,0x3c,0x01,0,0};
    const unsigned char sweepCall[8]{0xe8,0xcc,0xb5,0xff,0xff,0x48,0x85,0xc0};
    const unsigned char position36[14]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0x41,0x08};
    const unsigned char positionCall36[7]{0xe8,0x40,0x86,0x99,0x00,0xeb,0x18};
    const unsigned char selfMover36[14]{0x45,0x33,0xc0,0x85,0xd2,0x75,0x08,0x4c,0x8b,0x41,0x08,0x49,0x8b,0xc0};
    unsigned char p36[14]{},pc36[7]{},self36[14]{};
    unsigned char a[16]{},c[13]{},s[8]{};std::uintptr_t method=0;
    return Read(reinterpret_cast<void*>(b+0x14095E8),p36)&&!std::memcmp(p36,position36,sizeof(p36))&&
        Read(reinterpret_cast<void*>(b+0xA70FA3),pc36)&&!std::memcmp(pc36,positionCall36,sizeof(pc36))&&
        Read(reinterpret_cast<void*>(b+0xA71B60),self36)&&!std::memcmp(self36,selfMover36,sizeof(self36))&&
        Read(reinterpret_cast<void*>(b+0x9E37A8),a)&&!std::memcmp(a,query,sizeof(a))&&
        Read(reinterpret_cast<void*>(b+0x9E2D50),c)&&!std::memcmp(c,alignmentCall,sizeof(c))&&
        Read(reinterpret_cast<void*>(b+0xA7EB53),s)&&!std::memcmp(s,sweepCall,sizeof(s))&&
        Read(reinterpret_cast<void*>(b+0x3A633B0+7*8),method)&&method==b+0x9E2D00;
}
// Verified against WHGame SHA256 bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7.
// Pin the exact queue dispatch, constructor and SELF mover route in addition
// to the existing full retail build verification. No guessed vtable call ABI.
static bool NativeRotationContractD40(std::uintptr_t b) noexcept {
    if(!SupportedWhBuild(b)||!retail_build112::Ready(b))return false;
    const unsigned char entry[15]{0x48,0x89,0x5c,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xec,0x60};
    const unsigned char wrapper[14]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0x41,0x08};
    const unsigned char eligibility[9]{0xe8,0x0d,0x7f,0x66,0xff,0x84,0xc0,0x74,0x0b};
    const unsigned char kind3[22]{0x0f,0xb6,0x0f,0x85,0xc9,0x74,0x6c,0x83,0xe9,0x01,0x74,0x5a,0x83,0xe9,0x01,0x74,0x48,0x83,0xe9,0x01,0x74,0x36};
    const unsigned char call[13]{0x48,0x8b,0xd7,0x48,0x8b,0xcb,0xe8,0x11,0x88,0x99,0x00,0xeb,0x25};
    const unsigned char build[11]{0x48,0x8b,0xd7,0x48,0x8b,0xcb,0xe8,0x0e,0x00,0x00,0x00};
    const unsigned char ctorCall[12]{0x48,0x8b,0x92,0xf8,0x01,0x00,0x00,0xe8,0x8e,0x00,0x00,0x00};
    const unsigned char constructor[22]{0x40,0x53,0x48,0x83,0xec,0x20,0x4c,0x8b,0xc2,0x48,0x8b,0xd9,0xba,0x01,0x00,0x00,0x00,0xe8,0xf6,0x72,0x66,0xff};
    const unsigned char moverCall[13]{0x4c,0x8d,0x44,0x24,0x20,0x48,0x8b,0xcf,0xe8,0x26,0x82,0x66,0xff};
    const unsigned char self[16]{0x33,0xd2,0x49,0x8b,0xf9,0x49,0x8b,0xe8,0x48,0x8b,0xf1,0xe8,0x84,0x00,0x00,0x00};
    const unsigned char start[31]{0x48,0x8b,0x88,0x50,0x02,0x00,0x00,0xe8,0xd4,0x18,0xf7,0xff,0x48,0x8b,0xd5,0x48,0x8b,0x08,0x4c,0x8b,0x81,0xa0,0x00,0x00,0x00,0x48,0x8b,0xc8,0x41,0xff,0xd0};
    const unsigned char moverTail[27]{0x48,0x8b,0x8b,0x50,0x02,0x00,0x00,0xe8,0x8a,0x18,0xf7,0xff,0x48,0x8b,0x08,0x48,0x8b,0x91,0xa8,0x00,0x00,0x00,0x48,0x8b,0xc8,0xff,0xd2};
    const unsigned char resolver[15]{0x45,0x33,0xc0,0x85,0xd2,0x75,0x08,0x4c,0x8b,0x41,0x08,0x49,0x8b,0xc0,0xc3};
    const auto pin=[b](std::uintptr_t rva,const auto& expected){
        unsigned char actual[sizeof(expected)]{};
        return Read(reinterpret_cast<void*>(b+rva),actual)&&!std::memcmp(actual,expected,sizeof(actual));
    };
    return pin(0x14097F0,entry)&&pin(0x14097AC,wrapper)&&pin(0x14097CE,eligibility)&&pin(0xA70F44,kind3)&&pin(0xA70F90,call)&&
        pin(0x14097D7,build)&&pin(0x1409816,ctorCall)&&pin(0x14098B0,constructor)&&
        pin(0x1409885,moverCall)&&pin(0xA71ACC,self)&&pin(0xA71AE4,start)&&
        pin(0xA71B2E,moverTail)&&pin(0xA71B60,resolver);
}
static bool NativeRotationArmD40(std::uintptr_t b) noexcept {
    if(!b||g_motion_contract112.load(std::memory_order_acquire)!=b)return false;
    if(g_rotation_contract_d40.load(std::memory_order_acquire)==b)return true;
    // A failed optional rotation arm must not revoke the already verified
    // position/late-query hooks. Retain partial trampolines, fail native-only.
    if(g_rotation_hook_d40||!NativeRotationContractD40(b))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_d40_henry_queued_rotation",DetourModKit::hook::Target{DetourModKit::Address{b+0x14097F0}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&NativeRotationDetourD40);
    if(!h)return false;
    g_rotation_hook_d40=std::move(*h);g_rotation_original_d40=g_rotation_hook_d40->original<NativeRotationCommandD40>();
    if(!g_rotation_original_d40||!g_rotation_hook_d40->enable())return false;
    g_rotation_contract_d40.store(b,std::memory_order_release);return true;
}

static bool NativeMotionArm112() noexcept {
    const auto b=WhBase();
    if(g_motion_contract112.load()==b&&b){(void)NativeRotationArmD40(b);(void)NativeRootMotionArm125(b);return true;}
    // Retain any partially installed trampoline for process lifetime. A failed
    // arm stays native-only (contract remains0), never hot-recreates a hook.
    if(g_alignment_hook112||g_position_hook36||!NativeMotionContract112(b))return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_v66112_henry_alignment_query",DetourModKit::hook::Target{DetourModKit::Address{b+0x9E37A8}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&NativeAlignmentDetour112);
    if(!h)return false;
    g_alignment_hook112=std::move(*h);g_alignment_original112=g_alignment_hook112->original<NativeAlignmentQuery112>();
    DetourModKit::hook::InlineRequest positionReq36{"kcd2vr_c36_henry_queued_position",DetourModKit::hook::Target{DetourModKit::Address{b+0x14095E8}},DetourModKit::hook::Options{}};
    auto position36=DetourModKit::hook::inline_at(std::move(positionReq36),&NativePositionDetour36);
    if(!position36)return false;
    g_position_hook36=std::move(*position36);g_position_original36=g_position_hook36->original<NativePositionCommand36>();
    if(!g_alignment_original112||!g_position_original36||!g_alignment_hook112->enable()||!g_position_hook36->enable())return false;
    g_motion_contract112.store(b,std::memory_order_release);(void)NativeRotationArmD40(b);(void)NativeRootMotionArm125(b);return true;
}

struct NativeTrajectoryLease112 {
    void* producer=nullptr;void* action=nullptr;void* actor=nullptr;
    std::uint64_t lab=0,epoch=0,key=0,generation=0,request=0,sample=0,tick=0;
    int bone=-1,hand=-1;unsigned correlated=0;
    QuatT28 seed{};ph::Pose last{};
};
static thread_local NativeTrajectoryLease112 g_trajectory_lease112;
static bool NativeTrajectoryPrepare112(std::uintptr_t caller,void* collisions,int hand,int bone,
    const QuatT28* previous,const QuatT28* current,float dt,QuatT28& vrPrevious,QuatT28& vrCurrent) noexcept {
    auto& lease=g_trajectory_lease112;
    // Prediction callers keep their authored subdivisions and time intervals.
    if(caller!=WhBase()+0xA7EB58){++g_trajectory_reject112[0];return false;}
    const auto reject=[&](unsigned reason){++g_trajectory_reject112[reason];lease={};return false;};
    ReadScope101R2 scope;PhysFrame6698 f{};NativeOwner104 owner{};
    if(!std::isfinite(dt)||dt<=0||dt>.05f||!NativeMotionContext112(f,owner)||
       !previous||reinterpret_cast<std::uintptr_t>(previous)<0x30)return reject(1);
    auto producer=reinterpret_cast<unsigned char*>(reinterpret_cast<std::uintptr_t>(previous)-0x30);
    void* actor=nullptr;void* collisionOwner=nullptr;void* collisionBack=nullptr;void* action=nullptr;int actionHand=-1;
    unsigned char havePrevious=0,hit=1;std::int16_t actionBone=-1;QuatT28 seed{},nativeCurrent{};
    if(!Read(producer+8,actor)||actor!=owner.actor||!Read(producer+0x18,action)||!action||
       !Read(static_cast<unsigned char*>(actor)+0x218,collisionOwner)||collisionOwner!=collisions||
       !Read(static_cast<unsigned char*>(collisions)+8,collisionBack)||collisionBack!=actor||
       !Read(producer+0x4C,havePrevious)||havePrevious!=1||
       !Read(static_cast<unsigned char*>(action)+0x308,hit)||hit||
       !Read(static_cast<unsigned char*>(action)+0x0C,actionBone)||bone<0||actionBone!=bone||
       !Read(static_cast<unsigned char*>(action)+0x40C,actionHand)||actionHand!=hand||
       !PhysCopySweep112(previous,seed)||!PhysCopySweep112(current,nativeCurrent))return reject(2);
    CommittedFrame6695 body{};
    if(!g_committed_frame6695.read(body)||body.stamp.epoch!=f.input.bodyEpoch||
       body.stamp.sample!=f.input.sample||!body.valid[0]||
       !std::isfinite(f.world[7])||f.world[7]<.5f||f.world[7]>2.f)return reject(3);
    const auto& profile=f.profiles[0];const auto& capture=f.correlation126[0];
    const auto* socket=&capture.socket;const auto now=std::uint64_t(GetTickCount64());
    const bool sameCapture=capture.stamp.epoch==body.stamp.epoch&&capture.stamp.sample==body.stamp.sample&&
        capture.stamp.actor==body.stamp.actor&&capture.stamp.tick==body.stamp.tick;
    // V66.135R1: `capture.socket` is the coherent socket used by this pose. The
    // global socket mailbox is asynchronous, so a NEWER attachment callback must
    // not invalidate it. If the mailbox still contains the SAME callback serial,
    // however, topology disagreement is real corruption/mutation and fails closed.
    bool sameCallbackTopology=true;PhysSocketBank101 liveSockets{};
    if(socket->callback&&g_phys_sockets101.read(liveSockets))for(const auto& v:liveSockets.sockets)
        if(v.valid&&v.key==socket->key&&v.generation==socket->generation&&v.joint==socket->joint&&
           v.character==socket->character&&v.callback==socket->callback){
            sameCallbackTopology=!std::memcmp(&v.relative,&socket->relative,sizeof(v.relative))&&
                                 !std::memcmp(v.scale,socket->scale,sizeof(v.scale));break;}
    if(!sameCapture||!socket->valid||socket->key!=profile.key||socket->generation!=profile.generation||socket->joint!=bone||
       reinterpret_cast<std::uintptr_t>(socket->character)!=body.stamp.actor||now<socket->tick||now-socket->tick>200||
       !sameCallbackTopology||!profile.nativeSeed)return reject(4);
    // Same evaluated pose proves weapon = socketJoint * relative. Invert that
    // relation; never transplant the weapon origin onto an unrelated hit bone.
    const auto toWorld=[&](ph::Pose weapon){
        auto joint=ph::compose(weapon,ph::inverse(PhysPose6698(socket->relative)));
        joint.p=joint.p*f.world[7];return ph::compose(PhysWorld6698(f.world),joint);
    };
    const auto expected=toWorld(profile.nativeWeapon),desired=toWorld(f.output.weapons[0]);
    const auto actual=PhysPose6698(nativeCurrent);
    if(!ph::finite(expected)||!ph::finite(desired)||pm::length(expected.p-actual.p)>.015||
       pm::angle(expected.q,actual.q)>.04)return reject(5);
    const auto lab=diagnostics::lab::Snapshot();
    const bool same=lease.producer==producer&&lease.action==action&&lease.actor==actor&&lease.lab==lab&&
        lease.epoch==body.stamp.epoch&&lease.key==profile.key&&lease.generation==profile.generation&&
        lease.request==f.request&&lease.bone==bone&&lease.hand==hand&&
        !std::memcmp(&lease.seed,&seed,sizeof(seed));
    if(!same){lease={};lease.producer=producer;lease.action=action;lease.actor=actor;lease.lab=lab;
        lease.epoch=body.stamp.epoch;lease.key=profile.key;lease.generation=profile.generation;
        lease.request=f.request;lease.bone=bone;lease.hand=hand;lease.seed=seed;
        lease.sample=f.input.sample;lease.tick=f.input.tick;lease.last=desired;lease.correlated=1;++g_trajectory_reject112[7];return false;}
    if(f.input.sample<lease.sample||f.input.tick<lease.tick||f.input.tick-lease.tick>50||
       pm::length(desired.p-lease.last.p)>.5||pm::angle(desired.q,lease.last.q)>1.2)return reject(6);
    const bool fresh=f.input.sample>lease.sample&&f.input.tick>lease.tick;
    if(fresh&&lease.correlated<3)++lease.correlated;
    const auto from=fresh?lease.last:desired;
    lease.sample=f.input.sample;lease.tick=f.input.tick;lease.last=desired;
    if(lease.correlated<3){++g_trajectory_reject112[7];return false;}
    // Stack-local arguments are borrowed synchronously. Native callers still
    // own eligibility, hit bookkeeping, return values and damage resolution.
    vrPrevious=PhysQuat6698(from);vrCurrent=PhysQuat6698(desired);
    ++g_trajectory_writes112;return true;
}
