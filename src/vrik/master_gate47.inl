// C47 MASTER STRIKE GATE (VrMasterStrikeStabOnly, default 1). Included after native_bridges168.inl.
// Our own input edge (NativePhysicalAction117 prepare/commit, exact Henry owner) decides ONCE per attempt edge and publishes the
// verdict; a pinned mid-hook on the retail cold block 22B780B (D4AD94's "+7D8 > 0" branch) enforces it for the SAME actor/state
// within 300 ms: ALLOWED -> retail kind-5 request (native paired master strike), DENIED -> resume at D4ADCC, the retail path of a
// closed window (combo slot / ordinary attack). Henry only, VR strokes only (no fresh attempt = untouched native input).
namespace mg47=kcd2vr::master_gate47;
struct MasterAttempt47 {
    std::uint64_t serial=0,tick=0;std::uintptr_t actor=0,state=0,player=0;
    std::uint64_t keyH21=0,generationH21=0,bodyH21=0,requestH21=0;
    std::uint64_t bundleRevisionD70=0;
    std::uint64_t nativePolicyD51=0; // existing paired-backup generation; old verdicts cannot resume after a toggle
    // D62: the extra veto belongs to this input policy and opponent, not to
    // every input on Henry's otherwise persistent actor/state for 300 ms.
    std::uintptr_t targetD62=0;bool strictD62=false;
    int origin=-1,enemyZone=-1,window=-1,perk=-1,mode=1;float energy=0.f,minEnergy=0.f;
    mg47::Stroke stroke=mg47::Stroke::Unknown;mg47::Reason reason=mg47::Reason::Qualified;bool prepare=true,xi=false;
    std::uint64_t gesture=0; // C49 review: the VR gesture of this edge (prepare -> release verdict carry)
    std::uintptr_t targetStateR120=0; // R120 INSTANT: the enemy's combat state (targetD62+0x210) the blade grant belongs to
};
static std::atomic<std::uint64_t> g_master_grant_gesture120{0}; // R120: one INSTANT grant per VR gesture (prepare + release share it)
static kcd2vr::core::SnapshotMailbox<MasterAttempt47> g_master_attempt47;
static std::atomic<std::uint64_t> g_master_attempt_serial47{0},g_master_receipted47{0},g_master_allowed47{0},g_master_denied47{0};
static std::optional<DetourModKit::hook::Hook> g_master_gate47;
static std::atomic<bool> g_master_gate_ready47{false};
// Henry's soul ability 73 (C44 layout: exact C_Soul vtable, soul+0x320 rows, +0x328 count; the 9DCD80 all-abilities flag).
static int MasterPerk47(const NativeOwner104& owner) noexcept {
    const auto b=WhBase();std::uintptr_t vft=0,data=0;std::uint64_t n=0;
    if(!owner.soul||!Read(owner.soul,vft)||vft!=b+0x3F44F28||!Read(static_cast<unsigned char*>(owner.soul)+0x320,data)||
       !Read(static_cast<unsigned char*>(owner.soul)+0x328,n)||n>512||(n&&!data))return -1;
    void* g=nullptr;void* c=nullptr;int all=0;
    if(Read(reinterpret_cast<void*>(b+0x53322A0),g)&&g&&Read(static_cast<unsigned char*>(g)+0xA8,c)&&c&&Read(static_cast<unsigned char*>(c)+0x60,all)&&all)return 1;
    return mg47::hasAbility(std::size_t(n),mg47::kMasterStrikeAbility,[&](std::size_t i,mg47::AbilityRow& r){
        return Read(reinterpret_cast<void*>(data+i*sizeof(mg47::AbilityRow)),r);});
}
// The side the stroke comes FROM: the physical origin zone when the origin rule is on (C167/C176), else the announced zone.
static int attackZoneOrigin47(const ph::Command& c) noexcept {return mg47::attackZone(c.originZone167)?c.originZone167:mg47::attackZone(c.zone117)?c.zone117:-1;}
static mg47::Stroke MasterStroke47(ph::CommandKind k) noexcept {
    return k==ph::CommandKind::Thrust?mg47::Stroke::Thrust:k==ph::CommandKind::Slash?mg47::Stroke::Cut:k==ph::CommandKind::Pommel?mg47::Stroke::Pommel:mg47::Stroke::Unknown;
}
// Exact retail 90A68A..90A6A3 / C56663..C5667E read the current
// opponent's state+150 property (payload+158), then C566D8 compares that
// combat_zone row's master-strike zone with Henry's stance. Executed+278
// is not this offered zone. Reuse the existing reciprocal/current-pair
// reader and require an actual offered attack; unknown stays ordinary.
static int MasterIncomingZone55(const NativeOwner104& owner) noexcept {
    NativeParryOpponent176 incoming{};void* current=nullptr;
    if(!retail_build112::Ready(WhBase())||!NativeParryOpponentRead176(owner,incoming)||
       !incoming.incoming||!incoming.attacking||!mg47::attackZone(incoming.sector)||
       !Read(static_cast<unsigned char*>(owner.state)+0xCF8,current)||current!=incoming.actor||
       !Read(static_cast<unsigned char*>(incoming.state)+0xCF8,current)||current!=owner.actor)return -1;
    return incoming.sector;
}
// Returns true when this attempt edge must NOT become the native master strike (the caller then also keeps the VR stroke zone).
static bool MasterGateEdge47(const NativeOwner104& owner,const ph::Command& c,ph::CommandKind stroke,bool prepare) noexcept {
    const auto nativePolicyD51=kcd2vr::native_kill_nf::generation.load(std::memory_order_acquire);
    if(kcd2vr::native_kill_nf::NativePairedBackupD51())return false; // native window/selection, no mod veto
    const int mode=mg47::clampMode(mg47::mode.load(std::memory_order_relaxed));
    if(!mode||!g_master_gate_ready47.load(std::memory_order_acquire))return false;
    MasterAttempt47 a{};a.bundleRevisionD70=kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire);a.nativePolicyD51=nativePolicyD51;a.serial=g_master_attempt_serial47.fetch_add(1,std::memory_order_relaxed)+1;a.tick=GetTickCount64();
    // A kick / unresolved owner replaces the previous attempt with an empty one: an older verdict is never applied to it.
    if(!owner.actor||!owner.state||c.kick59){(void)g_master_attempt47.publish(a);return false;}
    a.actor=reinterpret_cast<std::uintptr_t>(owner.actor);a.state=reinterpret_cast<std::uintptr_t>(owner.state);a.mode=mode;a.prepare=prepare;
    a.strictD62=mg47::requireOpposite.load(std::memory_order_acquire);
    (void)Read(static_cast<unsigned char*>(owner.state)+0xCF8,a.targetD62);
    if(a.targetD62)(void)Read(reinterpret_cast<void*>(a.targetD62+0x210),a.targetStateR120); // R120: same actor->state link as the 22B780B hook (RAX=[RCX+210])
    a.player=reinterpret_cast<std::uintptr_t>(owner.player);a.keyH21=c.key;a.generationH21=c.generation;a.bodyH21=c.bodyEpoch;a.requestH21=c.epoch;
    a.xi=kcd2vr::attack_xi178r8::enabled.load(std::memory_order_relaxed);a.stroke=MasterStroke47(stroke);
    a.origin=attackZoneOrigin47(c);a.minEnergy=mg47::clampEnergy(mg47::minEnergy.load(std::memory_order_relaxed));
    a.energy=!prepare&&c.effortKnown19&&std::isfinite(c.effortScale19)?float(c.effortScale19):mg47::prepareEnergy(c.direction.x,c.direction.y,c.direction.z);
    a.enemyZone=MasterIncomingZone55(owner);
    // H11: the live native window may outlast the offered attack's phase4/8.
    // Reuse only the sector actually observed during this SAME open window;
    // never infer one from an executed animation or create a new opportunity.
    if(!mg47::attackZone(a.enemyZone)){
        kcd2vr::native_master23::Published openH11{};NativeParryOpponent176 pairH11{};int countH11=-1,currentOfferedH11=-1;
        if(NativeMasterCount23(owner,countH11)&&countH11>0&&g_master_published23.read(openH11)&&openH11.available&&
           openH11.count>0&&a.tick>=openH11.tick&&a.tick-openH11.tick<=80&&openH11.key==c.key&&
           openH11.generation==c.generation&&openH11.body==c.bodyEpoch&&openH11.request==c.epoch&&
           openH11.actorH11==reinterpret_cast<std::uintptr_t>(owner.actor)&&openH11.stateH11==reinterpret_cast<std::uintptr_t>(owner.state)&&
           NativeParryOpponentRead176(owner,pairH11)&&pairH11.id==openH11.opponentId&&
           openH11.opponentH11==reinterpret_cast<std::uintptr_t>(pairH11.actor)&&openH11.opponentStateH11==reinterpret_cast<std::uintptr_t>(pairH11.state)&&
           Read(static_cast<unsigned char*>(pairH11.state)+0x158,currentOfferedH11)&&currentOfferedH11==openH11.incomingZoneH11)
            a.enemyZone=openH11.incomingZoneH11;
    }
    int live=-1;a.window=NativeMasterCount23(owner,live)?live:-1;a.perk=MasterPerk47(owner);
    mg47::In in{};in.stabFamiliesD70=kcd2vr::claude_d70::MasterstrikeFix();in.mode=mode;in.stroke=a.stroke;in.energy=a.energy;in.minEnergy=a.minEnergy;in.origin=a.origin;in.enemyZone=a.enemyZone;in.perk=a.perk;
    a.gesture=c.gesture59c3;MasterAttempt47 prev{};
    // Preserve release of an already native-admitted matching prepare, even if
    // the experiment changed meanwhile. Reclassifying that in-flight release
    // as an ordinary attack would split the native transaction. The new policy
    // governs fresh prepares; revision checks still revoke stale hook verdicts.
    const bool carry=!prepare&&a.gesture&&g_master_attempt47.read(prev)&&prev.nativePolicyD51==nativePolicyD51&&
        prev.prepare&&prev.gesture==a.gesture&&prev.actor==a.actor&&
        prev.state==a.state&&prev.player==a.player&&prev.keyH21==a.keyH21&&prev.generationH21==a.generationH21&&
        prev.bodyH21==a.bodyH21&&prev.requestH21==a.requestH21&&prev.targetD62==a.targetD62&&
        prev.mode==a.mode&&prev.strictD62==a.strictD62&&mg47::allowed(prev.reason)&&a.tick>=prev.tick&&a.tick-prev.tick<=3000; // same gesture/target/policy, prepare ALLOWED
    a.reason=mg47::commitReason(mg47::sideRule(mg47::decide(in),a.strictD62),carry); // I10: side test opt-in
    if(a.bundleRevisionD70!=kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire)||
       kcd2vr::native_kill_nf::NativePairedBackupD51()||
       kcd2vr::native_kill_nf::generation.load(std::memory_order_acquire)!=nativePolicyD51||
       mg47::clampMode(mg47::mode.load(std::memory_order_acquire))!=a.mode||
       mg47::requireOpposite.load(std::memory_order_acquire)!=a.strictD62)return false;
    kcd2vr::diagnostics::flowH12::audit.attempt(prepare,int(a.stroke),a.window,a.perk,unsigned(a.reason));
    if(!g_master_attempt47.publish(a))return false; // not published = not enforced; the receipt would lie, so say nothing
    return !mg47::pairedClip(a.mode,a.reason); // R120: INSTANT never starts the paired clip, so the press keeps the VR stroke zone
}
// C47 is an extra WEAPON-gesture veto, never an unarmed/native-input policy.
// Henry's actor/state commonly survives equipping or sheathing. A recent weapon
// verdict must not leak to a punch (native HEAVY), another item or another epoch.
static bool MasterAttemptCurrentH21(const MasterAttempt47& a,std::uintptr_t actor,std::uintptr_t state,std::uint64_t now) noexcept {
    if(a.bundleRevisionD70!=kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire)||
       kcd2vr::native_kill_nf::NativePairedBackupD51()||
       a.nativePolicyD51!=kcd2vr::native_kill_nf::generation.load(std::memory_order_acquire)||
       !a.serial||!a.player||!a.keyH21||PhysEmpty6698(a.keyH21)||a.actor!=actor||a.state!=state||
       now<a.tick||now-a.tick>300||a.bodyH21!=g_pose_epoch6689.load()||a.requestH21!=g_weapon_request_epoch6694.load())return false;
    const auto* scope=g_input_scope117;
    // Both DIRECT and owned XI create this scope around their actual original
    // input handler. A nearby keyboard/gamepad or nested foreign input is not
    // this physical attempt and must retain the native decision.
    if(!scope||scope->fistPunchR19||scope->prepare!=a.prepare||MasterStroke47(scope->stroke)!=a.stroke||
       scope->owner.player!=reinterpret_cast<void*>(a.player)||
       reinterpret_cast<std::uintptr_t>(scope->owner.actor)!=actor||reinterpret_cast<std::uintptr_t>(scope->owner.state)!=state||
       scope->owner.epoch!=a.bodyH21)return false;
    const auto policyCurrent=[&]() noexcept {
        return a.bundleRevisionD70==kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire)&&!kcd2vr::native_kill_nf::NativePairedBackupD51()&&
            a.nativePolicyD51==kcd2vr::native_kill_nf::generation.load(std::memory_order_acquire)&&
            mg47::clampMode(mg47::mode.load(std::memory_order_acquire))==a.mode&&
            mg47::requireOpposite.load(std::memory_order_acquire)==a.strictD62;
    };
    PhysFrame6698 frame{};
    if(!policyCurrent()||!a.targetD62||!a.generationH21||!g_gameplay_allowed.load(std::memory_order_acquire)||
       animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)||
       !g_phys_frame6698.read(frame)||frame.player!=reinterpret_cast<void*>(a.player)||frame.request!=a.requestH21||
       !frame.input.allowed||frame.input.bodyEpoch!=a.bodyH21||!frame.input.tick||now<frame.input.tick||now-frame.input.tick>80||
       frame.profiles[0].key!=a.keyH21||frame.profiles[0].generation!=a.generationH21)return false;
    NativeOwner104 owner{};std::uint64_t key=0;std::uintptr_t target=0;
    return NativeResolve104(reinterpret_cast<void*>(a.player),owner)&&reinterpret_cast<std::uintptr_t>(owner.actor)==actor&&
        reinterpret_cast<std::uintptr_t>(owner.state)==state&&owner.epoch==a.bodyH21&&
        NativeHandKey6697(g_henry_item_attach_manager.load(),0,key)&&key==a.keyH21&&!PhysEmpty6698(key)&&
        Read(reinterpret_cast<void*>(state+0xCF8),target)&&target==a.targetD62&&policyCurrent();
}
static void MasterGateHook47(DetourModKit::hook::MidContext& ctx) noexcept {
    kcd2vr::attack_xi178r8::HookTimer178r22 ht47;
    using namespace DetourModKit::hook;
    if(kcd2vr::native_kill_nf::NativePairedBackupD51()||
       !g_master_gate_ready47.load(std::memory_order_acquire)||!mg47::clampMode(mg47::mode.load(std::memory_order_relaxed))||
       !CombatExperiment175Enabled()||!retail_build112::Ready(WhBase()))return;
    ReadScope101R2 reads;
    // 22B780B: RCX = actor ([self+18], D4ADB4), RAX = actor+210 state (D4ADB8), EDI = input class, RBX = self.
    const auto actor=gpr(ctx,Gpr::Rcx),state=gpr(ctx,Gpr::Rax);int count=-1;std::uintptr_t back=0;
    if(!actor||!state||!Read(reinterpret_cast<void*>(actor+0x210),back)||back!=state||
       !Read(reinterpret_cast<void*>(state+0x7D8),count)||count<=0||count>=64)return;
    MasterAttempt47 a{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_master_attempt47.read(a)||!MasterAttemptCurrentH21(a,actor,state,now))return; // not our VR attempt
    const bool deny=!mg47::pairedClip(a.mode,a.reason); // R120: INSTANT (mode 3) = admitted but no paired clip
    if(deny)instruction_pointer(ctx)=WhBase()+0xD4ADCC; // the retail closed-window path; the counter itself is untouched
    if(deny&&g_combo_route_redirect_n2)*g_combo_route_redirect_n2=true; // N2: this D4AD94 call now takes D4ADCC (D4AE70 combo route)
    auto last=g_master_receipted47.load(std::memory_order_relaxed);
    if(last==a.serial||!g_master_receipted47.compare_exchange_strong(last,a.serial))return; // one decision line per attempt edge
    // R120 INSTANT: the window is open here (count>0 checked above) and the press is this VR attempt = the game admitted the
    // master strike. hand it to the blade: the next contact on this enemy lands at full strike power. one grant per gesture.
    if(mg47::instantGrant(a.mode,a.reason)&&a.targetStateR120){
        auto g=g_master_grant_gesture120.load(std::memory_order_relaxed);
        if(!a.gesture||(g!=a.gesture&&g_master_grant_gesture120.compare_exchange_strong(g,a.gesture))){
            mg47::publishGrant(a.targetStateR120,a.stroke,now);
            char g120[260]{};std::snprintf(g120,sizeof(g120),"R120 MS_INSTANT grant=%llu stroke=%s edge=%s window=%d target=%llX -> paired clip skipped (D4ADCC), next blade contact on him within %llu ms = full-power master hit",
                (unsigned long long)mg47::grantSerial.load(),mg47::name(a.stroke),a.prepare?"PREPARE":"COMMIT",count,(unsigned long long)a.targetD62,(unsigned long long)mg47::kInstantGrantMs);
            diagnostic_trace::Write(g120);}
    }
    const auto denied=deny?g_master_denied47.fetch_add(1,std::memory_order_relaxed)+1:g_master_denied47.load(std::memory_order_relaxed);
    const auto allowedN=deny?g_master_allowed47.load(std::memory_order_relaxed):g_master_allowed47.fetch_add(1,std::memory_order_relaxed)+1;
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    mg47::Receipt r{};r.attempt=a.serial;r.tick=now;r.ageMs=now-a.tick;r.denied=denied;r.allowedN=allowedN;r.stroke=a.stroke;r.energy=a.energy;
    r.minEnergy=a.minEnergy;r.origin=a.origin;r.enemyZone=a.enemyZone;r.window=count;r.perk=a.perk;r.mode=a.mode;r.prepare=a.prepare;r.xi=a.xi;r.reason=a.reason;
    char m[420]{};mg47::format(r,m,sizeof(m));diagnostic_trace::Write(m);
}
static bool ArmMasterGate47() noexcept {
    if(g_master_gate_ready47.load())return true;
    const auto b=WhBase();if(!b||!retail_build112::Verify(b))return false;
    struct Pin{std::uintptr_t rva;std::array<unsigned char,13> bytes;};
    static constexpr Pin pins[]{
        {0xD4ADB4,{0x48,0x8B,0x4B,0x18,0x48,0x8B,0x81,0x10,0x02,0x00,0x00,0x83,0xB8}}, // mov rcx,[rbx+18]; mov rax,[rcx+210]; cmp [rax+7D8]..
        {0xD4ADC1,{0xD8,0x07,0x00,0x00,0x00,0x0F,0x8F,0x3F,0xCA,0x56,0x01,0xE8,0x9F}}, // ..,0; jg 22B780B; call D4AE70 (D4ADCC)
        {0xD4ADCE,{0x00,0x00,0x00,0x48,0x8B,0x4B,0x18,0x8B,0xD7,0x84,0xC0,0x75,0x6B}}, // D4ADCC: call D4AE70; mov rcx,[rbx+18]; mov edx,edi; test al; jne
        {0x22B780B,{0x8B,0xD7,0xE8,0x2E,0xBD,0x1C,0xFF,0x90,0xE9,0x3A,0x36,0xA9,0xFE}}, // mov edx,edi; call 1483540; nop; jmp D4AE52
        {0xD4AE46,{0x48,0x8B,0x89,0xB0,0x03,0x00,0x00,0xE8,0xDA,0xFD,0xF0,0xFF,0xB0}}, // combo route ...; D4AE52 mov al,1
    };
    for(const auto& p:pins){unsigned char bytes[13]{};if(!Read(reinterpret_cast<void*>(b+p.rva),bytes)||std::memcmp(bytes,p.bytes.data(),13))return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_master_gate47",Target{DetourModKit::Address{b+0x22B780B}},options},MasterGateHook47);
    if(!h)return false;g_master_gate47=std::move(*h);
    if(!g_master_gate47->enable()){g_master_gate47.reset();return false;}
    g_master_gate_ready47.store(true,std::memory_order_release);
    diagnostic_trace::Write("C47 MS_GATE armed site=+22B780B deny=+D4ADCC scope=HENRY_VR_ATTEMPT_300MS writes=NONE");return true;
}
static void ShutdownMasterGate47() noexcept {
    g_master_gate_ready47.store(false,std::memory_order_release);
    if(g_master_gate47){(void)g_master_gate47->disable();g_master_gate47.reset();}
}
