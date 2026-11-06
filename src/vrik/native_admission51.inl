// R51C1: replace R51's global CVars with one exact-Henry angular admission
// tolerance. 72F8C5 calls 13D0938(actor,target); its float result is compared
// with the target angle at 72F8CA. NPCs, other classes and result logic survive.
using AdmissionAngle51=float(__fastcall*)(void*,void*);
using AdmissionGate51=bool(__fastcall*)(void*,int);
static AdmissionAngle51 g_admission_angle51=nullptr;
static AdmissionGate51 g_admission_gate51=nullptr;
static std::optional<DetourModKit::hook::Hook> g_admission_angle_hook51,g_admission_gate_hook51;
static std::atomic<bool> g_admission_enabled51{false},g_admission_ready51{false};
static std::atomic<std::uint64_t> g_admission_angles51{0},g_admission_veto51{0},g_admission_light_veto51{0};
static bool AdmissionOwner51(void* actor) noexcept {
    if(!actor||!NativeTimingSelected22()||!g_admission_enabled51.load())return false;
    PhysFrame6698 f{};NativeOwner104 owner{};nc104::Observed native{};void* player=nullptr;void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_frame6698.read(f)||!f.player||!f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||
       !kcd2vr::contact176::fresh(f.input.tick,now,80)||!f.output.active[0]||!f.output.owned[0]||
       !f.profiles[0].melee()||!Read(static_cast<unsigned char*>(actor)+0x1F8,player)||player!=f.player||
       !NativeResolve104(f.player,owner)||owner.actor!=actor||
       !NativeObserve104(owner,native)||
       !kcd2vr::native_admission147::guardInput(native)||
       f.input.bodyEpoch!=owner.epoch||f.request!=g_weapon_request_epoch6694.load()||
       !Read(static_cast<unsigned char*>(player)+0x9D0,manager)||manager!=f.manager||manager!=g_henry_item_attach_manager.load()||
       !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=player||
       !NativeHandKey6697(manager,0,key)||key!=f.profiles[0].key||!key||PhysEmpty6698(key))return false;
    TrackingSnapshot t{};
    if(!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid||
       t.head_held||t.right_held||t.left_held||!kcd2vr::contact176::fresh(t.tick,now,80))return false;
    // Deliberate preparation or the release's short input window. Resting and
    // unrelated native actions do not acquire the wider angle merely by F4.
    return f.output.strokeActive119||PhysicalStrikeWindow142(f.profiles[0].key,f.profiles[0].generation,f.input.sample,now);
}
static float __fastcall AdmissionAngleHook51(void* actor,void* target) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const float native=g_admission_angle51?g_admission_angle51(actor,target):0.f;
    // RDX is the native target OWNER, not a guessed attack-row pointer.
    if(caller!=WhBase()+0x72F8CA||!target||!g_admission_ready51.load()||
       !std::isfinite(native)||native<=0||native>=1.57079633f||!AdmissionOwner51(actor))return native;
    ++g_admission_angles51;return 1.57079633f;
}
// N2 (I9, ported in I12): same-stack flag the C47 masterstrike gate (inside this D4AD94 call) sets when it redirects to D4ADCC.
static thread_local bool* g_combo_route_redirect_n2=nullptr;
static bool __fastcall AdmissionGateHook51(void* self,int input) noexcept {
    auto* scope=g_input_scope117;void* actor=nullptr;
    const bool owned=scope&&scope->visited&&self&&Read(static_cast<unsigned char*>(self)+0x18,actor)&&actor==scope->owner.actor;
    // C23b OUTCOME_CHAIN (receipts only): payload pre-read in D4AD94's own branch order (+838 && heavy, +7D8, +868)
    // on OUR owned edge only, and a thread-local same-stack scope so the existing 1483580/1483A90/90C850 detours
    // can join this call. Arguments and the native result are unchanged.
    int ms23=-1,heavy23=-1;unsigned char r830=0,slot868=0;bool known23=false;
    const bool record23=diagnostics::combat_capture::active(std::uint64_t(GetTickCount64()));
    if(owned)known23=Read(static_cast<unsigned char*>(scope->owner.state)+0x7D8,ms23)&&
        Read(static_cast<unsigned char*>(scope->owner.state)+0x838,r830)&&Read(static_cast<unsigned char*>(scope->owner.state)+0x868,slot868)&&
        Read(reinterpret_cast<void*>(WhBase()+0x5321014),heavy23);
    kcd2vr::attackclock50::Binding heldBeforeN2{};
    const bool boundBeforeN2=kcd2vr::combo_hold_r14::enabled.load(std::memory_order_relaxed)&&owned&&scope->prepare&&g_motion_clock_binding50.read(heldBeforeN2)&&
        heldBeforeN2.intent.actor==reinterpret_cast<std::uintptr_t>(actor)&&
        heldBeforeN2.intent.state==reinterpret_cast<std::uintptr_t>(scope->owner.state)&&
        kcd2vr::combo_hold_r14::valid(kcd2vr::combo_hold_r14::keyOf(heldBeforeN2));
    bool blocked=true;
    // N2 (Codex N1 review): the combo-hold stop comes from THIS call's actual route, after the original returned. The C47 gate
    // (a mid hook inside this same D4AD94) sets the redirect flag when it sends a denied owned masterstrike to D4ADCC.
    bool redirectN2=false;bool* priorRedirectN2=g_combo_route_redirect_n2;g_combo_route_redirect_n2=&redirectN2;
    {kcd2vr::outcome23::StackScope stack23{record23&&owned,reinterpret_cast<std::uintptr_t>(actor)};
     blocked=g_admission_gate51?g_admission_gate51(self,input):true;}
    g_combo_route_redirect_n2=priorRedirectN2;
    if(boundBeforeN2&&kcd2vr::combo_hold_r14::enabled.load(std::memory_order_relaxed)){namespace ch=kcd2vr::combo_hold_r14;
        ch::Route r{};r.owned=true;r.prepare=true;r.known=known23;r.r830Heavy=r830&&input==heavy23;r.slot=slot868!=0;r.ms=ms23;
        r.redirected=redirectN2;r.consumed=blocked;
        const auto why=ch::route(r);
        if(why==ch::RouteWhy::NotConsumed)ch::stopsNotConsumed.fetch_add(1,std::memory_order_relaxed);
        if(why==ch::RouteWhy::Combo){kcd2vr::attackclock50::Binding held{};
            // bind the stop to the stroke the 9E3C16 hook is holding: same actor/state as this native caller
            if(boundBeforeN2&&g_motion_clock_binding50.read(held)&&ch::same(ch::keyOf(heldBeforeN2),ch::keyOf(held))&&
               held.intent.actor==reinterpret_cast<std::uintptr_t>(actor)&&
               held.intent.state==reinterpret_cast<std::uintptr_t>(scope->owner.state)&&ch::valid(ch::keyOf(held))){
                ch::Stop st{};st.key=ch::keyOf(held);st.serial=ch::stopSerial.fetch_add(1,std::memory_order_relaxed)+1;
                st.tick=std::uint64_t(GetTickCount64());st.redirected=redirectN2;
                if(ch::stop.publish(st))ch::stopsPublished.fetch_add(1,std::memory_order_relaxed);
            }else ch::stopsUnbound.fetch_add(1,std::memory_order_relaxed);}}
    if(owned)kcd2vr::diagnostics::flowH12::audit.input(known23,ms23,slot868!=0,blocked);
    if(record23&&owned){
        const auto route=kcd2vr::outcome23::classify(known23,r830!=0,input==heavy23,ms23,slot868!=0,blocked);
        const auto now=std::uint64_t(GetTickCount64());const bool prepare=scope->prepare;const int slot=known23?(slot868?1:0):-1;
        kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.admission(prepare,route,slot,now,kcd2vr::outcome23::counters);});
    }
    if(blocked&&owned){++g_admission_veto51;if(scope->stroke==ph::CommandKind::Thrust)++g_admission_light_veto51;}
    return blocked; // observation only: stamina, incapacitation and authored eligibility remain native
}
static bool ArmAdmission51() noexcept {
    if(g_admission_ready51.load())return true;
    const auto b=WhBase();if(!b||!retail_build112::Ready(b))return false;
    const unsigned char angle[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xDA,0x48,0x85,0xD2,0x74,0x1D,0x48,0x8B};
    const unsigned char gate[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x8B,0xFA,0x48};
    const unsigned char call[]{0xE8,0x6E,0x10,0xCA,0x00,0x0F,0x2F,0xC6};
    unsigned char a[16]{},g[16]{},c[8]{};
    if(!Read(reinterpret_cast<void*>(b+0x13D0938),a)||std::memcmp(a,angle,sizeof(a))||
       !Read(reinterpret_cast<void*>(b+0xD4AD94),g)||std::memcmp(g,gate,sizeof(g))||
       !Read(reinterpret_cast<void*>(b+0x72F8C5),c)||std::memcmp(c,call,sizeof(c)))return false;
    using namespace DetourModKit::hook;Options opt{};opt.fail_if_already_hooked=true;
    auto ah=inline_at(InlineRequest{"kcd2vr_r51c1_admission_angle",Target{DetourModKit::Address{b+0x13D0938}},opt},AdmissionAngleHook51);
    if(!ah)return false;g_admission_angle_hook51=std::move(*ah);g_admission_angle51=g_admission_angle_hook51->original<AdmissionAngle51>();
    auto gh=inline_at(InlineRequest{"kcd2vr_r51c1_admission_veto",Target{DetourModKit::Address{b+0xD4AD94}},opt},AdmissionGateHook51);
    if(gh){g_admission_gate_hook51=std::move(*gh);g_admission_gate51=g_admission_gate_hook51->original<AdmissionGate51>();}
    if(!g_admission_angle51||!g_admission_gate51||!g_admission_angle_hook51->enable()||!g_admission_gate_hook51->enable()){
        g_admission_angle_hook51.reset();g_admission_gate_hook51.reset();g_admission_angle51=nullptr;g_admission_gate51=nullptr;return false;}
    g_admission_ready51=true;return true;
}
static void ShutdownAdmission51() noexcept {
    g_admission_ready51=false;g_admission_angle_hook51.reset();g_admission_gate_hook51.reset();
    g_admission_angle51=nullptr;g_admission_gate51=nullptr;
}
static void AdmissionSummary51() noexcept {
    if(!NativeTimingMode44c1())return;
    char s[640]{};std::snprintf(s,sizeof(s),
        "V66.178R51C1 ADMISSION ready=%d enabled=%d angleAdjusted=%llu nativeInputVeto=%llu stabInputVeto=%llu matchedStarts=%llu published=%llu begin=%llu beginRequested=%llu release=%llu releaseWithoutRequest=%llu stabBegin=%llu stabRelease=%llu noStepSkips55=%llu globalCvarWrites=NONE results=NATIVE",
        int(g_admission_ready51.load()),int(g_admission_enabled51.load()),(unsigned long long)g_admission_angles51.load(),
        (unsigned long long)g_admission_veto51.load(),(unsigned long long)g_admission_light_veto51.load(),
        (unsigned long long)g_attack_timing_matched22.load(),(unsigned long long)g_attack_timing_published22.load(),
        (unsigned long long)g_input_begin51.load(),(unsigned long long)g_input_requested51.load(),
        (unsigned long long)g_input_release51.load(),(unsigned long long)g_input_empty_release51.load(),
        (unsigned long long)g_input_stab_begin51.load(),(unsigned long long)g_input_stab_release51.load(),(unsigned long long)g_alignment_skips55.load());
    diagnostic_trace::Write(s);
}
