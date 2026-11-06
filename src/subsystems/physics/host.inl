// Repeated lexical contributions; deliberately no include guard.
#if KCD2VR_M2_HOST_SLOT == 18
// src/mod.cpp: fillmenustate_01.inl
// M2_SLOT_BEGIN 18
    out.weapon_grip_cm=weapon_grip_cm_.load(std::memory_order_acquire);out.support_sword_cm=support_sword_cm_.load(std::memory_order_acquire);out.support_slot34=frik_pose_probe::SupportSlot34();out.support_pole_cm=support_pole_cm_.load(std::memory_order_acquire); // R59.16C7F
// M2_SLOT_END 18
#elif KCD2VR_M2_HOST_SLOT == 31
// src/mod.cpp: fillmenustate_02.inl
// M2_SLOT_BEGIN 31
    out.ragdoll_push = g_ragdoll_push33.load(); // C33
    out.ragdoll_combat_ready57 = frik_pose_probe::PhysicalCombat177Enabled();
// M2_SLOT_END 31
#elif KCD2VR_M2_HOST_SLOT == 32
// src/mod.cpp: fillmenustate_03.inl
// M2_SLOT_BEGIN 32
    out.ragdoll_physics = RagdollPreference64(); // R59.16C10R7: F9 switch, menu-owned default
// M2_SLOT_END 32
#elif KCD2VR_M2_HOST_SLOT == 77
// src/mod.cpp: applymenustate_01.inl
// M2_SLOT_BEGIN 77
    const int f9PreferenceH6=g_physical_held_f9_t4.load();
    if(f9PreferenceH6>0)g_f9_saved_ragdoll64.store(state.ragdoll_physics?1:0);
    g_ragdoll_wanted59.store(f9PreferenceH6==0&&state.ragdoll_physics,std::memory_order_release);
// M2_SLOT_END 77
#elif KCD2VR_M2_HOST_SLOT == 98
// src/mod.cpp: savemenusettings_01.inl
// M2_SLOT_BEGIN 98
    ok = wf("VrWristFromGripCm", weapon_grip_cm_.load(std::memory_order_acquire)) && ok; ok = wf("VrSupportGripOffsetCm", support_sword_cm_.load(std::memory_order_acquire)) && ok; ok = wb("VrSupportFromWeaponSlot", frik_pose_probe::SupportSlot34()) && ok; ok = wf("VrSupportGripOffsetPoleCm", support_pole_cm_.load(std::memory_order_acquire)) && ok; // R59.16C7F
// M2_SLOT_END 98
#elif KCD2VR_M2_HOST_SLOT == 114
// src/mod.cpp: savemenusettings_02.inl
// M2_SLOT_BEGIN 114
    ok = wi("VrRagdollPush", g_ragdoll_push33.load()) && ok; // C33
// M2_SLOT_END 114
#elif KCD2VR_M2_HOST_SLOT == 115
// src/mod.cpp: savemenusettings_03.inl
// M2_SLOT_BEGIN 115
    ok = wb("VrRagdollF9Default", false) && ok; // Session-only; do not re-arm on restart.
// M2_SLOT_END 115
#elif KCD2VR_M2_HOST_SLOT == 145
// src/mod.cpp: loadconfig_01.inl
// M2_SLOT_BEGIN 145
    future.bind("EnablePhysicalGrabbing", enable_physical_grabbing_, false);
    future.bind("EnableNpcPhysicalReaction", enable_npc_physical_reaction_, false);
// M2_SLOT_END 145
#elif KCD2VR_M2_HOST_SLOT == 153
// src/mod.cpp: loadconfig_02.inl
// M2_SLOT_BEGIN 153
    g_ragdoll_wanted59.store(false);
// M2_SLOT_END 153
#elif KCD2VR_M2_HOST_SLOT == 183
// src/mod.cpp: loadconfig_03.inl
// M2_SLOT_BEGIN 183
    g_ragdoll_push33.store(std::clamp(ReadIniIntExplicit(explicit_ini,"Settings","VrRagdollPush",2),1,3)); // C33
// M2_SLOT_END 183
#elif KCD2VR_M2_HOST_SLOT == 208
// src/mod.cpp: loadconfig_04.inl
// M2_SLOT_BEGIN 208
    frik_pose_probe::SetWristFromGrip59c7f(weapon_grip_cm_.load(std::memory_order_relaxed));
    frik_pose_probe::SetSupportSocket178r9(g_support_socket_synthetic59c7f,support_sword_cm_.load(std::memory_order_relaxed),support_pole_cm_.load(std::memory_order_relaxed));
// M2_SLOT_END 208
#elif KCD2VR_M2_HOST_SLOT == 225
// src/mod.h: mod_state_01.inl
// M2_SLOT_BEGIN 225
    std::atomic<float> weapon_grip_cm_{0.f},support_sword_cm_{10.f},support_pole_cm_{32.f}; // R59.16C7F live hand/weapon placement (menu + INI)
// M2_SLOT_END 225
#elif KCD2VR_M2_HOST_SLOT == 246
// src/mod.h: mod_state_02.inl
// M2_SLOT_BEGIN 246
    std::atomic<bool> enable_physical_grabbing_{false};
    std::atomic<bool> enable_npc_physical_reaction_{false};
// M2_SLOT_END 246
#elif KCD2VR_M2_HOST_SLOT == 275
// src/vr_menu.h: menu_state_01.inl
// M2_SLOT_BEGIN 275
    bool ragdoll_combat_ready57 = false; // Read-only prerequisite; never enables combat.
// M2_SLOT_END 275
#elif KCD2VR_M2_HOST_SLOT == 276
// src/vr_menu.h: menu_state_02.inl
// M2_SLOT_BEGIN 276
    bool ragdoll_physics = false;          // F9 physics is an explicit session opt-in.
    int   ragdoll_push = 2;                 // C33 VrRagdollPush: 1 low, 2 normal, 3 strong (F9 rung 2 push on the struck body part)
// M2_SLOT_END 276
#elif KCD2VR_M2_HOST_SLOT == 278
// src/vr_menu.h: menu_state_03.inl
// M2_SLOT_BEGIN 278
    float weapon_grip_cm = 0.f;             // R59.16C7F: weapon in hand - IK goal moved down the controller handle (-15..15 cm), INI VrWristFromGripCm
    bool  support_slot34 = true;            // C34b/C37 VrSupportFromWeaponSlot: slt_1 replaces the synthetic fallback when no native left frame exists
    float support_sword_cm = 10.f;          // R59.16C7F: left hand on two-handed swords (6..60 cm down the grip), INI VrSupportGripOffsetCm
    float support_pole_cm = 32.f;           // R59.16C7F: left hand on halberds/poles (6..60 cm), INI VrSupportGripOffsetPoleCm
// M2_SLOT_END 278
#elif KCD2VR_M2_HOST_SLOT == 280
// src/vr_menu.h: menu_state_04.inl
// M2_SLOT_BEGIN 280
    bool exp_wall_slide = false;           // H18: explicit tangential scrape opt-in; normal impact/back-out stays active
// M2_SLOT_END 280
#elif KCD2VR_M2_HOST_SLOT == 289
// src/vr_menu.h: menu_state_05.inl
// M2_SLOT_BEGIN 289
    float exp_weight_feel = 0.85f;          // VrWeaponWeightFeel 0.2..2.0 (authority scale; lower = heavier)
    float exp_fatigue_ramp = 0.6f;          // VrWeaponFatigueRamp 0..1: drag grows as stamina falls from full
    float exp_pole_one_hand = 0.70f;        // VrPoleOneHandFeel 0.15..1: one-handed polearm authority fraction
// M2_SLOT_END 289
#elif KCD2VR_M2_HOST_SLOT == 292
// src/vr_menu.h: menu_state_06.inl
// M2_SLOT_BEGIN 292
    bool  exp_two_hand_axis = true;         // VrTwoHandAxisGrip (R59.16C10R16)
// M2_SLOT_END 292
#elif KCD2VR_M2_HOST_SLOT == 305
// src/vr_menu.h: menu_state_07.inl
// M2_SLOT_BEGIN 305
    int   stat_sim26 = 0;                   // C26c VrSimulateHenry: 0 real stats, 1 low, 2 medium, 3 high (weapon feel only)
// M2_SLOT_END 305
#elif KCD2VR_M2_HOST_SLOT == 311
// src/vr_menu.h: menu_state_08.inl
// M2_SLOT_BEGIN 311
    float exp_weapon_stat_effect = 1.0f;    // VrWeaponStatEffect (0 = R16 handling)
// M2_SLOT_END 311
#elif KCD2VR_M2_HOST_SLOT == 5007
// src/mod.cpp: ragdoll_backend
// M2_SLOT_BEGIN 5007
struct RagdollConsole59 {
    bool Read(const char* name,float& out) noexcept {void* v=SafeConsoleGetCVar(SafeGetCryConsole(),name);if(!v)return false;out=SafeCVarGetFloat(v,-12345.f);return out!=-12345.f;}
    bool Write(const char* name,float value) noexcept {void* v=SafeConsoleGetCVar(SafeGetCryConsole(),name);return v&&SafeCVarSetFloat(v,value);}
};
static kcd2vr::ragdoll59::Lease g_ragdoll59;
// M2_SLOT_END 5007
#elif KCD2VR_M2_HOST_SLOT == 5009
// src/mod.cpp: physical_reaction_leases
// M2_SLOT_BEGIN 5009
static std::atomic<bool> g_touch43{false},g_fallplay43{false};static std::atomic<float> g_touch_mass43{6.f},g_fallplay_threshold43{15.f};
// D51: restore-only compatibility state; global living-reaction forcing is retired.
using CvarLease43=kcd2vr::ragdoll59::RetiredGlobalLeaseD51;
static CvarLease43 g_lease_impact43{"ca_physicsProcessImpact"},g_lease_recoil43{"ca_UseRecoil"},g_lease_fall43{"g_fallAndPlayThreshold"};
static std::atomic<bool> g_ragdoll_wanted59{false};
static std::atomic<int> g_ragdoll_rung19c4r4{0};
static std::atomic<float> g_ragdoll_scale59{3.f};
static std::atomic<bool> g_contact_impulse596{true};static std::atomic<float> g_contact_impulse_scale596{1.f},g_contact_impulse_cap596{40.f};
static std::atomic<int> g_ragdoll_push33{2}; // C33 menu RAGDOLL PUSH: 1 low, 2 normal, 3 strong (VrRagdollPush)
static float RagdollPushScale33() noexcept {constexpr float s[]{1.f,.6f,1.f,1.8f};return s[std::clamp(g_ragdoll_push33.load(),1,3)];}
static float RagdollPushCap33() noexcept {return g_ragdoll_push33.load()>=3?80.f:std::clamp(g_contact_impulse_cap596.load(),12.f,60.f);}
// D84: diagnostic recording does not acquire or mutate a gameplay owner.
static auto PhysicsStatusCaptureD84() noexcept {
    using D=diagnostics::capture168::Domain;
    return diagnostics::capture_ticket_d84::select<D::Physics,D::Combat,D::Bug>([]{return GetTickCount64();});
}
static void ApplyRagdollRung19c4r4(int rung) noexcept {
    rung=std::clamp(rung,0,2);
    if(rung&&!combat_diag_probe::Install()){
        diagnostic_trace::Write("C4R4 F9 refused: exact native contact observer unavailable");rung=0;
    }
    g_ragdoll_rung19c4r4.store(rung);g_ragdoll_wanted59.store(rung==2);
    RagdollConsole59 io;(void)g_ragdoll59.Apply(io,rung==2,g_ragdoll_scale59.load());
    combat_diag_probe::SetContactImpulse596(rung==2&&g_contact_impulse596.load(),g_contact_impulse_scale596.load()*RagdollPushScale33(),RagdollPushCap33(),4.f); // C33: limb-sized mass, menu strength
    combat_diag_probe::SetContactImpulseDiagnostics19c4r4(rung==1);
    if(const auto capture=PhysicsStatusCaptureD84()){
        char m[280]{};std::snprintf(m,sizeof(m),
            "C4R4 RAGDOLL_F9 rung=%d mode=%s contactEnabled=%d globalCVarWrites=NONE currentArticulatedOnly=1 nativePointImpulse=0 capRequestedNs=%.0f push=%d cooldownMs=500",
            rung,rung==2?"BOUNDED_NATIVE_CONTACT":rung==1?"OBSERVE_ONLY":"OFF",rung==2&&g_contact_impulse596.load()?1:0,double(RagdollPushCap33()),g_ragdoll_push33.load());
        diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);
    }
}
static void ApplyRagdoll59(bool want) noexcept {ApplyRagdollRung19c4r4(want?2:0);}
// Read-only baseline of historical physics experiment CVars and comparison values,
// once per session, so tuning starts from what this game really runs with. MISSING = not registered in this build.
static bool LogPhysicsBaseline59c10r7() noexcept {
    const auto capture=PhysicsStatusCaptureD84();if(!capture)return false;
    RagdollConsole59 io;char m[1400]{};int at=std::snprintf(m,sizeof(m),"R59.16C10R7 PHYSICS_BASELINE (read-only, no F9 global acquisition)");
    auto add=[&](const char* n){if(at<=0||std::size_t(at)>=sizeof(m))return;float v=0;
        at+=io.Read(n,v)?std::snprintf(m+at,sizeof(m)-std::size_t(at)," %s=%.4g",n,double(v)):std::snprintf(m+at,sizeof(m)-std::size_t(at)," %s=MISSING",n);};
    for(const auto& e:kcd2vr::ragdoll59::kEntries)add(e.name);
    for(const char* n:kcd2vr::ragdoll59::kBaselineOnly)add(n);
    diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);return true;
}
// M2_SLOT_END 5009
#elif KCD2VR_M2_HOST_SLOT == 5011
// src/mod.cpp: physical_held_lifecycle
// M2_SLOT_BEGIN 5011
static std::atomic<int> g_phys_bodies59c10r8{0}; // R59.16C10R8: 0 off, 1 follow, 2 collide (Immersion > Experiments, INI VrPhysBodies)
static std::atomic<float> g_phys_hand_mass59c10r8{3.f},g_phys_blade_mass59c10r8{4.f};
// The legacy physical-held ladder is a menu-selected gameplay experiment.
// D84 F9 records only. Saved proxies must not compete with the rung-2 body.
static std::atomic<int> g_physical_held_f9_t4{0};
static std::atomic<int> g_f9_saved_ragdoll64{-1};
static std::atomic<bool> g_f9_failed_request_h6{false};
static int NextPhysicalHeldF9H6() noexcept {return g_f9_failed_request_h6.load()?0:(g_physical_held_f9_t4.load()+1)%4;}
static bool RagdollPreference64() noexcept {const int saved=g_f9_saved_ragdoll64.load();return saved>=0?saved==1:g_ragdoll_wanted59.load();}
static std::atomic<bool> g_legacy_phys_suspended_t4{false};
static void ApplyLegacyPhysBodiesT4() noexcept {
    frik_pose_probe::SetPhysBodies59c10r8(g_physical_held_f9_t4.load()==3&&!g_legacy_phys_suspended_t4.load()?g_phys_bodies59c10r8.load():0,
        g_phys_hand_mass59c10r8.load(),g_phys_blade_mass59c10r8.load());
}
static void ApplyPhysicalHeldF9T4(int rung) noexcept {
    rung=std::clamp(rung,0,3);
    // Installation is admission, not an ownership transfer. A refusal must not
    // turn off the previously selected hand/blade proxies or ragdoll preference.
    // Optional physical-hand preparation must never veto an independent F9 owner.
    if(rung>=2)(void)frik_pose_probe::PrepareNativeHandPhysicsH24();
    const bool installed=rung!=2||native_weapon_body_t4::Install();
    if(!installed){
        g_f9_failed_request_h6.store(true,std::memory_order_release);
        diagnostic_trace::Write("B3 F9 requested=PHYSICAL_HELD status=NATIVE_UNAVAILABLE previousMode=RETAINED ownershipTransfer=NONE");
        // D56: installation can refuse before any rung-change status is sent.
        // Use the existing bounded native-thread message mailbox; keep ownership.
        (void)kcd2vr::visible_failure::publish(kcd2vr::f9_status_fd::installRefusalText(),
            std::uint64_t(GetTickCount64()));
        return;
    }
    g_f9_failed_request_h6.store(false,std::memory_order_release);
    const int previous=g_physical_held_f9_t4.exchange(rung);
    kcd2vr::collab_h21::setPhysicsMode(rung);
    // H26: rung2 owns a physical held weapon; rung3 restores the tracked
    // weapon/contact producer for the proven C46 corpse-push route. The two
    // owners cannot coexist; wait for retirement before enabling impulses.
    // Save the preference while the explicit F9 ladder owns runtime. Observe
    // never activates physics; NPC/corpse requests require rung3 below.
    // Visiting 2 -> 1 must not consume the saved preference before 1 -> 0.
    if(previous==0&&rung>0)g_f9_saved_ragdoll64.store(g_ragdoll_wanted59.load()?1:0);
    if(rung==2){g_ragdoll_wanted59.store(false,std::memory_order_release);if(g_ragdoll_rung19c4r4.load()!=0)ApplyRagdollRung19c4r4(0);}
    else if(rung==3)g_ragdoll_wanted59.store(true,std::memory_order_release);
    else if(rung==1)g_ragdoll_wanted59.store(false,std::memory_order_release);
    else if(previous>0){const int saved=g_f9_saved_ragdoll64.exchange(-1);if(saved>=0)g_ragdoll_wanted59.store(saved==1,std::memory_order_release);}
    kcd2vr::lab58::rung.store(rung==3?3:0,std::memory_order_release);
    kcd2vr::lab58::f9Rung.store(rung,std::memory_order_release); // W5: rung 1/3 = physics diagnostics without F3
    if(const auto capture=PhysicsStatusCaptureD84()){
        char lm[200]{};std::snprintf(lm,sizeof(lm),"C58 LAB RUNG_NOW=%d corpseDrag=%d propWake=%d corpseSelectionIndependent=1",
            rung==3?3:0,kcd2vr::lab58::corpseDrag.load()?1:0,kcd2vr::lab58::propWake.load()?1:0);
        diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,lm);
    }
    combat_diag_probe::SetContactImpulse596(rung==3&&g_ragdoll_wanted59.load()&&!g_legacy_phys_suspended_t4.load()&&g_contact_impulse596.load(),g_contact_impulse_scale596.load()*RagdollPushScale33(),RagdollPushCap33(),4.f);
    combat_diag_probe::SetContactImpulseDiagnostics19c4r4(rung==1);
    if(rung==2){
        g_legacy_phys_suspended_t4.store(true);
        ApplyLegacyPhysBodiesT4();
        native_weapon_body_t4::SetEnabled(true);
        diagnostic_trace::Write("C61 F9 rung=2 PHYSICAL_HELD nativeRpgResult=UNCHANGED");
    }else{
        native_weapon_body_t4::SetEnabled(false);
        native_weapon_body_t4::Cancel();
        ApplyLegacyPhysBodiesT4(); // F9 OFF/observe also retires experimental follower bodies.
        diagnostic_trace::Write(rung==3?"H26 F9 rung=3 mode=TRACKED_WEAPON_CORPSE_PUSH retirement=REQUIRED nativeDamage=UNCHANGED":rung==1?"H21 F9 rung=1 mode=OBSERVE_ONLY":"T4 F9 rung=0 mode=OFF retirement=PENDING_ACK");
    }
    // D30: one visible line per rung (what is active). before this the ladder only beeped.
    (void)kcd2vr::visible_failure::publish(rung==3?kcd2vr::f9_status_fd::rung3Text(kcd2vr::lab58::corpseDrag.load(), // D32: all four listed
        kcd2vr::lab58::livingTouch.load(),kcd2vr::lab58::livingHitImpact.load(),kcd2vr::lab58::propWake.load()):
        kcd2vr::f9_status_fd::rungText(rung,kcd2vr::lab58::corpseDrag.load(),
        kcd2vr::lab58::livingTouch.load(),kcd2vr::lab58::livingHitImpact.load()),std::uint64_t(GetTickCount64()));
}
static void ServiceRagdollPushH6(bool consoleReady) noexcept {
    if(!consoleReady)return;
    const bool suspended=g_legacy_phys_suspended_t4.load();
    const bool wanted=g_ragdoll_wanted59.load();
    const bool selected=g_physical_held_f9_t4.load()==3;
    const bool run=selected&&wanted&&!suspended; // F9 owns runtime; the saved menu preference is not admission.
    if(run!=g_ragdoll59.On()){
        ApplyRagdoll59(run);
        combat_diag_probe::SetContactImpulseDiagnostics19c4r4(g_physical_held_f9_t4.load()==1);
        // Keep the saved preference when F9 is inactive or retirement is pending.
        // An actual rung3 install refusal keeps OFF and is not retried every frame.
        if(suspended||!selected)g_ragdoll_wanted59.store(wanted,std::memory_order_release);
    }
}
static void ServicePhysicalHeldF9T4() noexcept {
    native_weapon_body_t4::Achieved a{};
    const bool receiptRead=native_weapon_body_t4::Read(a);
    if(!receiptRead){
        a.state=native_weapon_body_t4::State::Waiting;
        a.reason=native_weapon_body_t4::Reason::NoInput;
        a.safeNativeRestore=false;a.colliderRetired=false;a.poseValid=false;
    }
    if(g_physical_held_f9_t4.load()!=2&&a.safeNativeRestore&&g_legacy_phys_suspended_t4.exchange(false)){
        ApplyLegacyPhysBodiesT4();
        diagnostic_trace::Write("T4 F9 retired=ACK legacyProxyPreference=PRESERVED runtime=F9_GATED savedSettings=UNCHANGED");
    }
    // Only transitions are formatted on the mod thread, never native callbacks.
    static unsigned priorState=~0u,priorReason=~0u,priorSource=~0u;
    static int priorRung=-1;
    const auto source=frik_pose_probe::HeldPhysicsSourceReasonT4();
    const auto state=unsigned(a.state),reason=unsigned(a.reason);const int rung=g_physical_held_f9_t4.load();
    const bool changed=state!=priorState||reason!=priorReason||rung!=priorRung||unsigned(source)!=priorSource;
    if(changed){
        // D30: rung 2 says why the held weapon is not physical (J27: silent NO_HELD_SWORD). edge only, rung 2 only;
        // the rung-change line from ApplyPhysicalHeldF9T4 wins the 2 s limiter, a later source change is shown.
        if(rung==2&&(unsigned(source)!=priorSource||rung!=priorRung)){
            if(const char* why=kcd2vr::f9_status_fd::heldRefusalText(unsigned(source)))
                (void)kcd2vr::visible_failure::publish(why,std::uint64_t(GetTickCount64()));
        }
        priorState=state;priorReason=reason;priorRung=rung;priorSource=unsigned(source);
    }
    // The gameplay lifecycle above still runs with every recorder off. The
    // optional phase read and formatting only run for a new/changed capture.
    static std::uint64_t reportedSessionD84=0;static unsigned reportedDomainD84=~0u;
    const auto capture=PhysicsStatusCaptureD84();
    if(capture&&(changed||capture.session!=reportedSessionD84||unsigned(capture.domain)!=reportedDomainD84)){
        reportedSessionD84=capture.session;reportedDomainD84=unsigned(capture.domain);
        const auto phase=native_physics_phase_t4::ReadReceipt();
        char m[768]{};std::snprintf(m,sizeof(m),
            "T4 F9 rung=%d bodyState=%u reason=%s source=%s bodyId=%d bodyGen=%llu key=%llu epoch=%llu sample=%llu sourceTick=%llu achieved=%d world=%llu legacyRestoreSafe=%d receiptRead=%d phaseSteps=%llu phaseRejected=%llu modMeleeContact=DISABLED_WHILE_OWNED nativeRpgResult=UNCHANGED",
            rung,state,native_weapon_body_t4::ReasonName(a.reason),frik_pose_probe::HeldPhysicsSourceNameT4(source),a.bodyId,
            static_cast<unsigned long long>(a.bodyGeneration),static_cast<unsigned long long>(a.owner.key),
            static_cast<unsigned long long>(a.owner.epoch),static_cast<unsigned long long>(a.sample),
            static_cast<unsigned long long>(a.sourceTick),a.poseValid?1:0,
            static_cast<unsigned long long>(a.worldGeneration),a.safeNativeRestore?1:0,receiptRead?1:0,
            static_cast<unsigned long long>(phase.steps),static_cast<unsigned long long>(phase.rejectedContexts));
        diagnostic_trace::WriteReceiptFor(capture.domain,capture.session,m);
    }
}
// M2_SLOT_END 5011
#else
#error Unknown M2 host contribution slot
#endif
