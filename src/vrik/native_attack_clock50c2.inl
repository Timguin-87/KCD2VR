// Exact native 9E3B80 boundary, after its once-per-frame check and dt getter,
// before it advances elapsed time and publishes the three deadline properties.
namespace ac50=kcd2vr::attackclock50;
static std::optional<DetourModKit::hook::Hook> g_motion_clock_hook50;
static std::atomic<bool> g_motion_clock_ready50{false},g_motion_clock_fault50{false};
static std::atomic<std::uintptr_t> g_motion_clock_actor50{0};
static std::atomic<std::uint64_t> g_motion_clock_guard_tick19{0}; // bounds Unknown before any borrowed binding read
static kcd2vr::core::SnapshotMailbox<ac50::Binding> g_motion_clock_binding50;
static std::atomic<std::uint64_t> g_motion_clock_captured50{0},g_motion_clock_applied50{0},g_motion_clock_contacts50{0},g_motion_clock_rejected50{0};
static std::atomic<int> g_motion_clock_reason50{0},g_motion_clock_phase50{0};
static std::atomic<float> g_motion_clock_dt50{0},g_motion_clock_effective50{0};
static bool ClockRead50(std::uintptr_t p,void* dst,std::size_t n) noexcept {
    if(!p||!dst)return false;
    __try{std::memcpy(dst,reinterpret_cast<const void*>(p),n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool ClockWrite50(std::uintptr_t p,const void* src,std::size_t n) noexcept {
    if(!p||!src)return false;
    __try{std::memcpy(reinterpret_cast<void*>(p),src,n);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool MotionClockSelected50() noexcept {
    if(kcd2vr::motion59c4::selected.load(std::memory_order_acquire))return false;
    return ac50::enabled.load(std::memory_order_relaxed)&&g_motion_clock_ready50.load(std::memory_order_acquire)&&
        !g_motion_clock_fault50.load(std::memory_order_relaxed)&&g_combat_experiment175.load()&&g_physical_combat177.load()&&
        NativeTimingSelected22(); // R59C3: legacy F4 comparison adapts an admitted native timeline
}
static void InvalidateMotionClockStart50(void* action) noexcept {
    const auto expected=g_motion_clock_actor50.load(std::memory_order_acquire);
    std::uintptr_t actor=0;
    // A native restart may reuse the same action allocation. It needs a NEW
    // matched VR release; matching pointers alone must not revive the old lease.
    if(expected&&action&&ClockRead50(reinterpret_cast<std::uintptr_t>(action)+0x70,&actor,8)&&actor==expected)
        g_motion_clock_actor50.store(0,std::memory_order_release);
}
static void CaptureMotionClock50(void* action,const AttackTimingScope22& scope,bool started) noexcept {
    if(!scope.clock50)return;
    g_motion_clock_actor50.store(0,std::memory_order_release);
    if(!started||!MotionClockSelected50()||scope.intent.policy!=g_attack_timing_policy22.load())return;
    ac50::Binding b{};b.intent=scope.intent;b.action=reinterpret_cast<std::uintptr_t>(action);
    if(!ClockRead50(b.action+0xA0,&b.helper,8)||!ClockRead50(b.action+0x58,&b.row,8)||
       !ClockRead50(b.intent.actor+0x3C0,&b.clock,8)||!ClockRead50(b.intent.actor+0x200,&b.manager,8)||
       !ClockRead50(b.intent.player+0x38,&b.entity,8)||b.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()))return;
    // B27308 can run before its manager installs the slot. Capture the identity
    // here; inspect() requires CURRENT manager membership at the actual write.
    if(g_motion_clock_binding50.publish(b)){
        g_motion_clock_guard_tick19.store(b.intent.tick,std::memory_order_release);
        g_motion_clock_actor50.store(b.intent.actor,std::memory_order_release);++g_motion_clock_captured50;
    }
}
// -1 unknown: caller must defer a competing guard; 0 stable absence; 1 exact
// current admitted ordinary action. The caller supplies its freshly read frame.
static int NativeSynchronizedAttackOwnedC10R18(const NativeOwner104& owner,const PhysFrame6698& f,std::uint64_t now) noexcept {
    if(!CombatSynchronized59c10r9())return 0;
    const auto expected=g_motion_clock_actor50.load(std::memory_order_acquire);
    if(!expected)return 0;
    if(!MotionClockSelected50()||!kcd2vr::contact176::fresh(g_motion_clock_guard_tick19.load(std::memory_order_acquire),now,1500))return 0;
    ac50::Binding b{};
    if(!g_motion_clock_binding50.read(b))return -1;
    if(expected!=b.intent.actor||expected!=reinterpret_cast<std::uintptr_t>(owner.actor)||
       b.intent.player!=reinterpret_cast<std::uintptr_t>(owner.player)||b.intent.state!=reinterpret_cast<std::uintptr_t>(owner.state)||
       b.entity!=reinterpret_cast<std::uintptr_t>(owner.entity)||b.intent.policy!=g_attack_timing_policy22.load()||
       b.intent.epoch!=g_pose_epoch6689.load()||b.intent.epoch!=owner.epoch||b.intent.request!=g_weapon_request_epoch6694.load()||
       !kcd2vr::contact176::fresh(b.intent.tick,now,1500))return 0;
    // C10R19 (review): currency first. Once the admitted action is no longer the live main action in phase 4/8 the answer is a stable
    // absence (0); incomplete frame/tracking evidence afterwards must not release a block the player is holding (it returned -1 for
    // up to 1.5 s after every admitted attack). Unreadable live state is still "unknown".
    std::uintptr_t manager=0,slots=0,current=0;int phase=0;
    if(!ClockRead50(b.intent.actor+0x200,&manager,8)||!manager||
       !ClockRead50(manager+0x38,&slots,8)||!slots||!ClockRead50(slots,&current,8)||
       !ClockRead50(b.intent.state+0x38,&phase,4))return -1;
    if(manager!=b.manager||current!=b.action||(phase!=4&&phase!=8))return 0;
    if(!f.input.sample||f.input.sample!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!f.input.tracked[0]||!f.input.tracked[1]||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||f.player!=owner.player||f.manager!=reinterpret_cast<void*>(b.intent.manager)||
       f.input.bodyEpoch!=b.intent.epoch||f.request!=b.intent.request||!f.output.active[0]||!f.output.owned[0]||
       !f.profiles[0].melee()||f.profiles[0].key!=b.intent.key||f.profiles[0].generation!=b.intent.generation)return -1;
    TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||!kcd2vr::contact176::fresh(tracking.tick,now,80))return -1;
    std::uint64_t key=0;if(!NativeHandKey6697(f.manager,0,key))return -1;
    if(key!=b.intent.key)return 0;
    ac50::View v{};return ac50::inspect(b,WhBase(),ClockRead50,v)?1:-1;
}
// R11 (fists = weapons): R27 lets an admitted punch advance normally; catch-up is only at real
// VR fist contact. Only the established attack-clock addresses are written.
static std::atomic<std::uint64_t> g_fist_clock_generation_r11{0};
static double g_fist_clock_held_r11=0; // native game thread only (this mid hook)
static bool FistClockMidR11(std::uintptr_t clock,float dt) noexcept {
    namespace fc=kcd2vr::fist_clock_r11;namespace fh=kcd2vr::fist_action_h12;
    if(!fc::enabled.load(std::memory_order_relaxed)||g_weapon_control_mode.load()!=7||!FistSelected101()||
       !g_motion_clock_ready50.load(std::memory_order_acquire)||g_motion_clock_fault50.load(std::memory_order_relaxed)||
       diagnostics::lab::NativeReference()||!g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||
       g_ctx_cutscene.load()||g_ctx_scripted.load()||animation_filter::nativeBodyOwned112.load()||
       animation_filter::PlayerNativeBodyRequiredForMotion122()||!retail_build112::Ready(WhBase()))return false;
    const auto now=std::uint64_t(GetTickCount64());fh::Binding b{};
    if(!fh::current(b,now)||!fh::punch(b.nativeType)||b.intent.side<0||b.intent.side>1)return false;
    std::uintptr_t owner=0;if(!ClockRead50(clock+8,&owner,8)||owner!=b.intent.actor)return false; // another actor's clock
    if(b.intent.policy!=g_attack_timing_policy22.load()||b.intent.epoch!=g_pose_epoch6689.load()||
       b.intent.request!=g_fist_epoch101.load())return true;
    NativeOwner104 native{};FistOwner101 empty{};TrackingSnapshot tracking{};FistContactFrameH11 frame{};
    const int side=b.intent.side;
    if(!NativeResolve104(reinterpret_cast<void*>(b.intent.player),native)||reinterpret_cast<std::uintptr_t>(native.actor)!=b.intent.actor||
       reinterpret_cast<std::uintptr_t>(native.state)!=b.intent.state||native.epoch!=b.intent.epoch||
       !FistReadOwner101(empty)||!empty.empty||empty.player!=native.player||empty.request!=b.intent.request||
       !ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||tracking.head_held||
       !(side?tracking.left_valid:tracking.right_valid)||(side?tracking.left_held:tracking.right_held)||
       !tracking.tick||now<tracking.tick||now-tracking.tick>60||
       !((side?tracking.left_grip:tracking.right_grip)>.60f&&(side?tracking.left_trigger:tracking.right_trigger)>.60f)||
       !g_fist_contact_h11.read(frame)||!frame.completedH31||!frame.valid[side]||!frame.pairH31[side]||
       frame.player!=native.player||frame.request!=b.intent.request||frame.epoch!=b.intent.epoch||
       frame.gestureSampleH31[side]!=b.intent.sample||!frame.tick||now<frame.tick||now-frame.tick>60||
       HandClaimBlockedT24(side,frame.character,frame.epoch,g_weapon_request_epoch6694.load()))return true;
    if(g_fist_clock_generation_r11.load(std::memory_order_relaxed)!=b.generation){
        g_fist_clock_generation_r11.store(b.generation,std::memory_order_relaxed);g_fist_clock_held_r11=0;}
    fc::Ids id{};id.actor=b.intent.actor;id.state=b.intent.state;id.action=b.action;id.helper=b.helper;id.row=b.row;id.type=b.nativeType;id.hand=b.nativeHand;id.player=b.intent.player;id.entity=reinterpret_cast<std::uintptr_t>(native.entity);
    ac50::View v{};
    if(!fc::inspect(id,clock,WhBase(),ClockRead50,v)){fc::rejected.fetch_add(1,std::memory_order_relaxed);fc::lastWhy.store(40+v.check);return true;}
    const auto gesture=b.intent.sample;
    const bool touched=FistTouchedR11(b,frame.character,now);
    FistPeakR10 peak{};const bool havePeak=FistPeakMailboxR10(side).read(peak)&&peak.side==side&&
        peak.palms.player==native.player&&peak.palms.request==b.intent.request&&peak.palms.epoch==b.intent.epoch&&
        peak.palms.character==frame.character;
    const bool travelling=fc::advancing(havePeak&&peak.peak.valid,peak.peak.gesture,peak.peak.tick,gesture,b.intent.tick,now);
    const auto d=fc::decide(v.phase,ac50::remaining(v,0),ac50::remaining(v,1),ac50::remaining(v,2),dt,touched,travelling,
        g_fist_clock_held_r11,fc::maxHoldMs.load(std::memory_order_relaxed));
    fc::lastWhy.store(d.why,std::memory_order_relaxed);
    if(d.act==fc::Act::Native){if(g_fist_clock_held_r11>0&&!touched)fc::released.fetch_add(1,std::memory_order_relaxed);return true;}
    const auto p=d.act==fc::Act::Hold?fc::holdPlan(clock,v,dt):fc::catchUpPlan(clock,v,dt);
    const auto applied=ac50::apply(p,ClockRead50,ClockWrite50);
    if(applied==ac50::Applied::Fault||applied==ac50::Applied::RollbackFault){g_motion_clock_fault50=true;return true;}
    if(applied!=ac50::Applied::Changed)return true;
    if(d.act==fc::Act::Hold){g_fist_clock_held_r11+=double(dt)*1000.;fc::holds.fetch_add(1,std::memory_order_relaxed);}
    else{fc::catchups.fetch_add(1,std::memory_order_relaxed);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[240]{};
            std::snprintf(m,sizeof(m),"R11 FIST_CLOCK CATCH_UP side=%d gesture=%llu heldMs=%.0f phase=%d peakLeft=%.4f rate=%.2f",
                side,(unsigned long long)gesture,g_fist_clock_held_r11,v.phase,double(ac50::remaining(v,1)),double(v.rate));
            diagnostic_trace::Write(m);}}
    return true;
}
static bool ComboRecoveryHoldR14(std::uintptr_t clock,float dt) noexcept; // N2 (I12), defined after MotionClockMid50
static void MotionClockMid50(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    // I10/NF2: only Henry's own clock, only while his CURRENT skeleton anim has a live observed kill occurrence
    if(kcd2vr::native_kill_nf::liveSerial.load(std::memory_order_acquire)){
        const auto henry=g_motion_clock_actor50.load(std::memory_order_acquire);std::uintptr_t clockActor=0;
        if(henry&&ClockRead50(gpr(ctx,Gpr::Rbx)+8,&clockActor,8)&&clockActor==henry&&
           kcd2vr::native_kill_nf::StandDown(reinterpret_cast<std::uintptr_t>(animation_filter::PlayerSkeletonAnim()),std::uint64_t(GetTickCount64())))return;
    }
    if(FistClockMidR11(gpr(ctx,Gpr::Rbx),xmm(ctx,0).lane<float>(0)))return;
    if(ComboRecoveryHoldR14(gpr(ctx,Gpr::Rbx),xmm(ctx,0).lane<float>(0)))return; // N2 (I12)
    const auto expected=g_motion_clock_actor50.load(std::memory_order_acquire);
    if(!expected||!ac50::enabled.load(std::memory_order_relaxed)||!g_combat_experiment175.load()||
       !g_physical_combat177.load())return;
    const auto clock=gpr(ctx,Gpr::Rbx);std::uintptr_t actor=0;
    if(!ClockRead50(clock+8,&actor,8)||actor!=expected)return; // unrelated NPC clocks: no additional reads
    if(!MotionClockSelected50())return;
    const auto no=[](int why){g_motion_clock_reason50=why;++g_motion_clock_rejected50;};
    ac50::Binding b{};ac50::View v{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_motion_clock_binding50.read(b)||b.clock!=clock||b.intent.policy!=g_attack_timing_policy22.load()||
       b.intent.epoch!=g_pose_epoch6689.load()||b.intent.request!=g_weapon_request_epoch6694.load()||
       b.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
       !kcd2vr::contact176::fresh(b.intent.tick,now,1500)){no(1);return;}
    // The matched release belongs to this admitted action. A subsequent
    // PREPARE/release proposal cannot replace it until native action start.
    const auto& t=b.intent;
    PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!f.input.tracked[0]||!f.input.tracked[1]||!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||!f.output.active[0]||!f.output.owned[0]||
       f.input.bodyEpoch!=t.epoch||f.request!=t.request||reinterpret_cast<std::uintptr_t>(f.player)!=t.player||
       reinterpret_cast<std::uintptr_t>(f.manager)!=b.intent.manager||!f.profiles[0].melee()||
       f.profiles[0].key!=t.key||f.profiles[0].generation!=t.generation){no(3);return;}
    TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||!kcd2vr::contact176::fresh(tracking.tick,now,80)){no(3);return;}
    std::uint64_t key=0;
    if(!NativeHandKey6697(f.manager,0,key)||key!=t.key){no(4);return;}
    if(!ac50::inspect(b,WhBase(),ClockRead50,v)){no(40+v.check);return;}
    // R161 tutorial GAME TIMING: this swing's start decided "game's own speed" (practice weapon) -> no catch-up, native rate.
    if(kcd2vr::tutorial_native_r159::swingTiming.decided(b.intent.tick,kcd2vr::tutorial_native_r159::nativeTimingOnlyNow(now))){no(7);return;}
    ac50::Contact contact{};const bool touched=ac50::contact.read(contact)&&contact.actor==actor&&ac50::touches(contact,t)&& // R59.16C10R17: gesture match too
        kcd2vr::contact176::fresh(contact.tick,now,kcd2vr::contact59c3::maximumContactAge);
    const float dt=xmm(ctx,0).lane<float>(0);
    // Use deliberate release speed when the blade has stopped at a real contact;
    // otherwise use the current body-relative gesture speed from the same pose.
    const double live=b.intent.kind==at22::Kind::Thrust?f.output.melee118.handSpeed:f.output.melee118.tipSpeed;
    const double speed=touched?b.intent.speed:std::clamp(live,.35,std::max(.35,b.intent.speed));
    const auto p=ac50::contactPlan59c3(b,v,dt,speed,touched);
    const auto applied=ac50::apply(p,ClockRead50,ClockWrite50);
    if(applied==ac50::Applied::Fault||applied==ac50::Applied::RollbackFault){g_motion_clock_fault50=true;no(6);return;}
    if(applied==ac50::Applied::Stale){no(5);return;}
    g_motion_clock_reason50=0;g_motion_clock_phase50=v.phase;g_motion_clock_dt50=dt;g_motion_clock_effective50=p.effectiveDt;
    if(applied==ac50::Applied::Changed){++g_motion_clock_applied50;if(touched)++g_motion_clock_contacts50;}
    // General/SIMD registers and flags are untouched. Native resumes normally.
}
// N2 (I9, ported in I12): keep Henry's admitted attack alive in its last hit-phase frames while the native combo slot is
// open and the VR player has not pressed yet (combo_recovery_hold_r14.h). native game thread only (this mid hook).
// composes with R25 native_combo_grace: that owns the slot timer (9D6A34) and the 911250 deferral, this only holds the
// attack clock tail so the press finds the current attack alive. no timer, slot, phase or latch write here.
static kcd2vr::combo_hold_r14::Tracker g_combo_hold_r14{}; // keyed by the complete binding identity (game thread only)
static bool g_carrier_window_armed_cb=false; // D30-CMB: last published carrier window was armed (game thread only)
static bool ComboRecoveryHoldR14(std::uintptr_t clock,float dt) noexcept {
    namespace ch=kcd2vr::combo_hold_r14;
    const auto retire=[](){ch::revoke(g_combo_hold_r14);ch::holdingActor.store(0,std::memory_order_release); // R94
        if(g_carrier_window_armed_cb){g_carrier_window_armed_cb=false; // D30-CMB: disarm the carrier window at once
            namespace cc=kcd2vr::combo_carrier_cb;cc::Window w{};w.serial=cc::windowSerial.fetch_add(1,std::memory_order_relaxed)+1;(void)cc::window.publish(w);}
        return false;};
    const auto expected=g_motion_clock_actor50.load(std::memory_order_acquire);
    if(!ch::enabled.load(std::memory_order_relaxed)||!expected||!ac50::enabled.load(std::memory_order_relaxed)||
       !g_motion_clock_ready50.load(std::memory_order_acquire)||g_motion_clock_fault50.load(std::memory_order_relaxed)||
       !g_combat_experiment175.load()||!g_physical_combat177.load())return retire();
    if(!MotionClockSelected50())return retire(); // R26 existing native-body, retail, policy and experiment gates
    std::uintptr_t actor=0;if(!ClockRead50(clock+8,&actor,8)){if(clock==g_combo_hold_r14.key.clock)return retire();return false;}
    if(actor!=expected)return false; // NPC clocks untouched
    const auto now=std::uint64_t(GetTickCount64());
    const bool context=!diagnostics::lab::NativeReference()&&g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&!animation_filter::nativeBodyOwned112.load();
    ac50::Binding b{};
    if(!g_motion_clock_binding50.read(b)||b.clock!=clock)return retire();
    const auto key=ch::keyOf(b);
    if(!ch::valid(key))return retire();
    (void)ch::rebind(g_combo_hold_r14,key); // Only a new admitted occurrence restores eligibility.
    if(g_combo_hold_r14.ended)return false;
    if(b.intent.policy!=g_attack_timing_policy22.load()||
       b.intent.epoch!=g_pose_epoch6689.load()||b.intent.request!=g_weapon_request_epoch6694.load()||
       !kcd2vr::contact176::fresh(b.intent.tick,now,4000))return retire();
    // R26: a live native action is not evidence of current physical-hand ownership.
    // Reuse the existing MotionClockMid50 completed frame/key/generation admission.
    const auto& t=b.intent;PhysFrame6698 f{};
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
       !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!f.input.tracked[0]||!f.input.tracked[1]||
       !g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||!f.output.active[0]||!f.output.owned[0]||
       f.input.bodyEpoch!=t.epoch||f.request!=t.request||reinterpret_cast<std::uintptr_t>(f.player)!=t.player||
       reinterpret_cast<std::uintptr_t>(f.manager)!=t.manager||b.entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load())||
       !f.profiles[0].melee()||f.profiles[0].key!=t.key||f.profiles[0].generation!=t.generation)return retire();
    TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||!kcd2vr::contact176::fresh(tracking.tick,now,60))return retire();
    std::uint64_t currentKey=0;
    if(!NativeHandKey6697(f.manager,0,currentKey)||currentKey!=t.key||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire))return retire();
    int currentPhase=0;
    if(!ClockRead50(b.intent.state+0x38,&currentPhase,sizeof(currentPhase)))return retire();
    // R94: hold the tail of phase 16 or 32. A hold that began in one phase never continues in another
    // (the native clock moved on = released); the same VrComboRecoveryHoldMs budget covers both.
    if(!ch::holdPhase(currentPhase)||(g_combo_hold_r14.started&&g_combo_hold_r14.phase!=currentPhase)){
        if(g_combo_hold_r14.started)return retire();return false;}
    ac50::View v{};
    if(!ch::inspectRecovery(b,WhBase(),ClockRead50,v)){
        // Invalidation retires this exact stroke; a transient read/gate failure cannot renew its budget.
        if(g_combo_hold_r14.holding)ch::endOther.fetch_add(1,std::memory_order_relaxed);
        return retire();}
    ch::Stop st{};const bool pressedN2=ch::stop.read(st)&&ch::stopped(st,key);
    unsigned char slot=0;(void)ClockRead50(b.intent.state+0x868,&slot,1);
    ch::Input in{};in.phase=v.phase;in.total=ac50::remaining(v,2);in.dt=dt;in.slot=slot!=0;
    in.pressed=pressedN2;
    // R94: only a MANUAL guard (LT) ends a running hold. Our automatic posture guard is kept down by
    // NativeGuardTick112 while holdingNow(); before the hold starts the old rule (any guard) stays.
    const bool manualGuardR94=g_guard_manual112.load()&&g_guard_manual_owner112.load()==reinterpret_cast<void*>(b.intent.player);
    in.guard=manualGuardR94||(!g_combo_hold_r14.holding&&(g_native_guard112.held||g_native_guard112.pendingPress46));
    in.on=context;in.heldMs=ch::elapsed(g_combo_hold_r14,now);in.maxMs=ch::maxHoldMs.load(std::memory_order_relaxed);
    const auto why=ch::decide(in);ch::lastWhy.store(int(why),std::memory_order_relaxed);
    { // D30-CMB: arm the carrier keep-alive while this exact stroke may still be held in the open slot (Hold or before the tail)
        namespace cc=kcd2vr::combo_carrier_cb;cc::Window w{};
        if(why==ch::Why::Hold||why==ch::Why::NotEnd){
            w.actor=b.intent.actor;w.attack=b.action;w.manager=b.manager;w.key=b.intent.tick;w.tick=now;w.thread=GetCurrentThreadId();
            if(!ClockRead50(b.action+0x80,&w.fragment,8))w.fragment=0;}
        w.serial=cc::windowSerial.fetch_add(1,std::memory_order_relaxed)+1;(void)cc::window.publish(w);g_carrier_window_armed_cb=w.attack!=0;}
    if(why!=ch::Why::Hold){
        if(g_combo_hold_r14.holding&&ch::ends(why)){g_combo_hold_r14.holding=false;
            (why==ch::Why::Pressed?ch::endPress:why==ch::Why::Guard?ch::endGuard:why==ch::Why::NoSlot?ch::endSlot:
             why==ch::Why::Timeout?ch::endTimeout:ch::endOther).fetch_add(1,std::memory_order_relaxed);
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[220]{};
                std::snprintf(m,sizeof(m),"R14 COMBO_HOLD end=%s heldMs=%.0f stroke=%llu total=%.4f slot=%d",ch::name(why),g_combo_hold_r14.heldMs,
                    (unsigned long long)b.intent.tick,double(in.total),int(slot));diagnostic_trace::Write(m);}}
        // R94: before the hold started, a slot that is not open YET (the native phase-16/CB3 open can come after this
        // clock tick, or only in phase 32) or one long frame is not an end: keep this stroke eligible for its tail.
        const bool notYetR94=!g_combo_hold_r14.started&&(why==ch::Why::NoSlot||why==ch::Why::BadDt);
        if(ch::ends(why)&&!notYetR94){ch::revoke(g_combo_hold_r14);ch::holdingActor.store(0,std::memory_order_release);} // R94
        return false;}
    const auto applied=ac50::apply(ch::holdPlan(clock,v,dt),ClockRead50,ClockWrite50);
    if(applied==ac50::Applied::Fault||applied==ac50::Applied::RollbackFault){g_motion_clock_fault50=true;ch::faults.fetch_add(1,std::memory_order_relaxed);ch::revoke(g_combo_hold_r14);return true;}
    if(applied!=ac50::Applied::Changed)return retire();
    if(!g_combo_hold_r14.holding){g_combo_hold_r14.holding=true;ch::windows.fetch_add(1,std::memory_order_relaxed);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[200]{};
            std::snprintf(m,sizeof(m),"R14 COMBO_HOLD begin stroke=%llu total=%.4f maxMs=%u slot=%d",(unsigned long long)b.intent.tick,double(in.total),in.maxMs,int(slot));
            diagnostic_trace::Write(m);}}
    if(!g_combo_hold_r14.started){g_combo_hold_r14.started=now;g_combo_hold_r14.phase=v.phase;} // R94
    ch::holdingActor.store(b.intent.actor,std::memory_order_release);ch::holdingTick.store(now,std::memory_order_release); // R94
    g_combo_hold_r14.heldMs+=double(dt)*1000.;ch::frames.fetch_add(1,std::memory_order_relaxed);
    return true;
}
static bool ArmMotionClock50() noexcept {
    if(g_motion_clock_ready50.load())return true;
    const auto b=WhBase();if(!b||!retail_build112::Ready(b))return false;
    for(const auto& pin:ac50::pins){unsigned char bytes[16]{};
        if(!ClockRead50(b+pin.rva,bytes,sizeof(bytes))||std::memcmp(bytes,pin.bytes.data(),sizeof(bytes)))return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_r50c2_motion_clock",Target{DetourModKit::Address{b+0x9E3C16}},options},MotionClockMid50);
    if(!h)return false;g_motion_clock_hook50=std::move(*h);
    if(!g_motion_clock_hook50->enable()){g_motion_clock_hook50.reset();return false;}
    g_motion_clock_ready50.store(true,std::memory_order_release);return true;
}
static void ShutdownMotionClock50() noexcept {
    g_motion_clock_ready50=false;g_motion_clock_actor50=0;
    if(g_motion_clock_hook50){(void)g_motion_clock_hook50->disable();g_motion_clock_hook50.reset();}
}
static void MotionClockSummary50() noexcept {
    if(!g_combat_experiment175.load()||!g_physical_combat177.load())return;
    static std::uint64_t last=0;const auto now=std::uint64_t(GetTickCount64());if(now-last<1000)return;last=now;
    char line[500]{};std::snprintf(line,sizeof(line),
        "V66.178R50C2 MOTION_CLOCK ready=%d fault=%d captured=%llu applied=%llu contactCatchup=%llu rejected=%llu reason=%d phase=%d nativeDt=%.5f effectiveDt=%.5f scope=HENRY_CURRENT_ORDINARY_ACTION nativePhaseWrite=NONE acceptedHit=UNCLAIMED",
        int(g_motion_clock_ready50.load()),int(g_motion_clock_fault50.load()),(unsigned long long)g_motion_clock_captured50.load(),
        (unsigned long long)g_motion_clock_applied50.load(),(unsigned long long)g_motion_clock_contacts50.load(),
        (unsigned long long)g_motion_clock_rejected50.load(),g_motion_clock_reason50.load(),g_motion_clock_phase50.load(),
        double(g_motion_clock_dt50.load()),double(g_motion_clock_effective50.load()));diagnostic_trace::Write(line);
    namespace ch=kcd2vr::combo_hold_r14;char hold[380]{};std::snprintf(hold,sizeof(hold),
        "R14 COMBO_HOLD_SUMMARY enabled=%d maxMs=%u windows=%llu frames=%llu endPress=%llu endGuard=%llu endSlot=%llu endTimeout=%llu endOther=%llu faults=%llu lastWhy=%s stops=%llu unbound=%llu notConsumed=%llu",
        int(ch::enabled.load()),unsigned(ch::maxHoldMs.load()),(unsigned long long)ch::windows.load(),(unsigned long long)ch::frames.load(),
        (unsigned long long)ch::endPress.load(),(unsigned long long)ch::endGuard.load(),(unsigned long long)ch::endSlot.load(),
        (unsigned long long)ch::endTimeout.load(),(unsigned long long)ch::endOther.load(),(unsigned long long)ch::faults.load(),
        ch::name(ch::Why(ch::lastWhy.load())),(unsigned long long)ch::stopsPublished.load(),(unsigned long long)ch::stopsUnbound.load(),(unsigned long long)ch::stopsNotConsumed.load());diagnostic_trace::Write(hold);
}
