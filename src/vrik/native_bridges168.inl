// V168: only already-proved OnAction calls execute on the native frame.
// The master experiment adds a REJECTION at a pinned native factory branch;
// it never supplies a successful native result or writes a damage structure.
namespace b168=kcd2vr::bridges168;
static void CombatBridgeTraceD84(const char* text) noexcept {
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))diagnostic_trace::Write(text);
}
static void ShutdownFreeKickD72() noexcept;
static b168::Queue g_bridge_events168;
static kcd2vr::core::SnapshotMailbox<b168::SpecialRequest> g_special_request168;
static std::atomic<bool> g_special_allowed168{false},g_special_pending168{false};
static std::atomic<std::uint64_t> g_special_context168{1},g_bridge_serial168{0};
static kcd2vr::core::SnapshotMailbox<b168::Ticket> g_master_ticket168;
static std::atomic<std::uint64_t> g_master_policy168{1},g_master_used168{0};
static std::optional<DetourModKit::hook::Hook> g_master_gate168;
static std::atomic<bool> g_master_gate_ready168{false};
static std::optional<DetourModKit::hook::Hook> g_close_combat_gate175;
static std::atomic<bool> g_close_combat_ready175{false};
// V66.178R59 master strike for every weapon (Henry only). Learned from nexus 3611's KCSE plugin, which patches the
// retail weapon-family predicate for everybody: +909B4C(weaponClassRow) answers "class 1, 2, 4 or 16" (the sword
// groups) and its caller +908DA0 (kind 5 = master strike) turns a false into "no master strike ability". The
// companion pak adds the blunt/halberd rows and fragments; this mid-hook at +908E09 (right after that call, AL =
// its result, R8 = the row, RSI = the actor's soul) flips the answer to true for Henry's blunt / axe / flail /
// halberd (3/5/6/7) only. NPCs keep the retail rule. INI VrMasterStrikeAllWeapons (default on).
static std::optional<DetourModKit::hook::Hook> g_family_gate59;
static std::atomic<bool> g_family_gate_ready59{false},g_master_all_weapons59{true};
static std::atomic<bool> g_master_veto59c14{false}; // R59.16C10R14: VR ticket veto of the native master-strike factory (default OFF: the game decides)
static std::atomic<std::uint64_t> g_family_gate_flips59{0},g_family_gate_seen59{0};
static void RevokeMasterIntent168() noexcept {g_master_policy168.fetch_add(1,std::memory_order_acq_rel);}
static void BridgeEvent168(b168::Event e) noexcept {
    if(e.kind==1){ // R3 transport and observed native type are separate, with or without F3
        const auto t=kcd2vr::kick_route_d85::onTerminal(e.reason,e.allowed,e.type==4);
        if(t.startCooldown)kcd2vr::kick_route_d85::cooldown.dispatched(e.tick?e.tick:std::uint64_t(GetTickCount64()));
        else if(t.text)(void)kcd2vr::visible_failure::publish(t.text,std::uint64_t(GetTickCount64()));
    }
    if(e.kind==2){++b168::factoryQueries49;if(e.nativeResult)++b168::factoryGroupTrue49;
        if(e.allowed)++b168::factoryPermitted49;b168::factoryDenial49=e.reason;}
    // V66.178R18 (review F03): the rung-4 coach cue is gameplay, not a diagnostic - a permitted factory
    // decision is queued whether or not F3 records (recording copies events; it does not own them).
    const bool coach=e.kind==2&&e.allowed&&F4Rung178r10()>=4;
    if(coach||diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))g_bridge_events168.push(e);
}
static void RecordMasterIntent168(const NativeOwner104& owner,const ph::Command& c,const PhysFrame6698& f) noexcept {
    b168::Ticket t{};t.serial=++g_bridge_serial168;t.tick=c.tick;t.epoch=owner.epoch;t.policy=g_master_policy168.load();
    t.player=reinterpret_cast<std::uintptr_t>(owner.player);t.actor=reinterpret_cast<std::uintptr_t>(owner.actor);
    t.state=reinterpret_cast<std::uintptr_t>(owner.state);t.key=c.key;t.generation=c.generation;t.request=f.request;t.sample=c.sample;
    t.origin=c.originZone167;t.thrust=CombatExperiment175Enabled()&&c.stroke117==ph::CommandKind::Thrust;
    (void)Read(static_cast<unsigned char*>(owner.state)+0xCF8,t.opponent);
    // Slash/Pommel preparation explicitly invalidates any previous thrust ticket.
    if(!g_master_ticket168.publish(t))g_master_policy168.fetch_add(1,std::memory_order_acq_rel);
}
static bool CurrentOpponent168(void* actor,void*& state) noexcept {
    state=nullptr;std::uintptr_t v=0;void* back=nullptr;
    if(!actor||!Read(actor,v)||(v!=WhBase()+0x3E3FAC8&&v!=WhBase()+0x3A80610)||
       !Read(static_cast<unsigned char*>(actor)+0x210,state)||!state||
       !Read(static_cast<unsigned char*>(state)+0xCE0,back)||back!=actor){state=nullptr;return false;}
    return true;
}
#include "clinch_distance32.inl"
static void CloseCombatAutoGate175(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    if(ClinchDistanceReject32(ctx)){
        gpr(ctx,Gpr::Rax)&=~std::uintptr_t(255);return;
    }
    // D40 Contact/Off modes own automatic admission. An accepted physical
    // grip must not be vetoed again by the historical closed-fist anti-clinch.
    if(kcd2vr::clinch_intent_d40::getMode()!=int(kcd2vr::clinch_intent_d40::Mode::Distance))return;
    // Exact +72C97E: AL is CanStartSyncGuard's result, RBX is the auto-trigger
    // component. A zero takes the existing early return, before native +1A is
    // set. We reject this initiation only; no phase, distance, or NPC pose write.
    const auto policyD41=kcd2vr::clinch_intent_d40::generation.load(std::memory_order_acquire);
    const auto contextCurrentD41=[&]() noexcept {
        return g_close_combat_ready175.load(std::memory_order_acquire)&&FistSelected101()&&FistModePermits178r22c2()&&
            F4Rung178r10()!=0&&retail_build112::Ready(WhBase())&&
            g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
            !g_fist_fault101.load()&&!diagnostics::lab::NativeReference()&&!animation_filter::nativeBodyOwned112.load()&&
            !animation_filter::NativeKickActive175()&&!g_ctx_dialogue.load()&&!g_ctx_menu.load()&&
            !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
            policyD41==kcd2vr::clinch_intent_d40::generation.load(std::memory_order_acquire)&&
            kcd2vr::clinch_intent_d40::getMode()==int(kcd2vr::clinch_intent_d40::Mode::Distance);
    };
    if(!(gpr(ctx,Gpr::Rax)&255)||!contextCurrentD41())return;
    const auto now=std::uint64_t(GetTickCount64());FistFrame101 f{};
    if(!g_fist_frame101.read(f)||!f.player||!f.sample.valid||!f.sample.empty[0]||!f.sample.empty[1]||
       f.request!=g_fist_epoch101.load()||f.sample.epoch!=g_pose_epoch6689.load()||
       !f.sample.actor||!f.sample.sequence||!f.sample.tick||now<f.sample.tick||now-f.sample.tick>80)return;
    const bool closed[2]{std::isfinite(f.sample.grip[0])&&std::isfinite(f.sample.trigger[0])&&f.sample.grip[0]>.70&&f.sample.trigger[0]>.70,
                         std::isfinite(f.sample.grip[1])&&std::isfinite(f.sample.trigger[1])&&f.sample.grip[1]>.70&&f.sample.trigger[1]>.70};
    if(!closed[0]&&!closed[1])return;
    ReadScope101R2 reads;NativeOwner104 henry{};FistOwner101 hands{};nc104::Observed native{};
    void* actor=nullptr;void* state=nullptr;void* opponent=nullptr;void* opponentState=nullptr;void* back=nullptr;
    const auto self=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::Rbx));unsigned char inCombat=0,otherCombat=0;
    int syncStart=-1;
    if(!self||!Read(self+8,actor)||!CurrentOpponent168(actor,state)||
       !Read(static_cast<unsigned char*>(actor)+0x3F0,back)||back!=self||
       !Read(static_cast<unsigned char*>(state)+0xCF8,opponent)||!opponent||opponent==actor||
       !CurrentOpponent168(opponent,opponentState)||
       !Read(static_cast<unsigned char*>(opponentState)+0xCF8,back)||back!=actor||
       !Read(static_cast<unsigned char*>(state)+8,inCombat)||inCombat!=1||
       !Read(static_cast<unsigned char*>(opponentState)+8,otherCombat)||otherCombat!=1||
       !Read(reinterpret_cast<void*>(WhBase()+0x53204E4),syncStart)||syncStart!=35||
       !NativeResolve104(f.player,henry)||henry.actor!=actor||henry.state!=state||
       !FistReadOwner101(hands)||!hands.empty||hands.player!=henry.player||hands.request!=f.request||hands.epoch!=f.sample.epoch||
       !NativeObserve104(henry,native)||!native_admission147::guardWritable(native))return;
    // D41: the legacy closed-fist veto is also player-initiated only. An NPC
    // may still clinch Henry while he holds a fist; retail owns that pair.
    // A declined modern gate must not fall through and veto a cutscene via
    // this legacy path. Rejoin after native reads as well as at entry.
    const auto finalNowD41=std::uint64_t(GetTickCount64());
    if(!contextCurrentD41()||f.request!=g_fist_epoch101.load()||f.sample.epoch!=g_pose_epoch6689.load()||
       finalNowD41<f.sample.tick||finalNowD41-f.sample.tick>80)return;
    gpr(ctx,Gpr::Rax)&=~std::uintptr_t(255); // the retail caller performs its own test/branch
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        static std::atomic<std::uint64_t> due{0};auto next=due.load();
        if(now>=next&&due.compare_exchange_strong(next,now+500)){
            b168::Event e{};e.kind=3;e.tick=now;e.sample=f.sample.sequence;e.actor=reinterpret_cast<std::uintptr_t>(actor);
            e.opponent=reinterpret_cast<std::uintptr_t>(opponent);e.type=syncStart;e.origin=closed[0]?0:1;
            e.nativeResult=true;e.allowed=false;BridgeEvent168(e);
        }
    }
}
static bool ArmCloseCombat175() noexcept {
    if(g_close_combat_ready175.load(std::memory_order_acquire))return true;
    const auto b=WhBase();if(!b||!retail_build112::Verify(b))return false;
    struct Pin{std::uintptr_t rva;std::array<unsigned char,16> bytes;};
    static constexpr Pin pins[]{
        {0x72C918,{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x78,0x10,0x55,0x48,0x8D,0x68,0xA1}},
        {0x72C94D,{0x0F,0x28,0x7C,0x24,0x70,0x4C,0x8D,0x9C,0x24,0x90,0x00,0x00,0x00,0x49,0x8B,0x5B}},
        {0x72C976,{0x48,0x8B,0xCB,0xE8,0x76,0xB1,0x20,0x00,0x84,0xC0,0x74,0xCB,0x48,0x8B,0x43,0x08}},
        {0x72CAB2,{0xF3,0x0F,0x58,0xFE,0xF3,0x41,0x0F,0x59,0xF8,0xF3,0x0F,0x59,0xB8,0x4C,0x03,0x00}},
        {0x72CAC3,{0xE8,0xCC,0x8E,0x2A,0x00,0xF3,0x0F,0x10,0x80,0x44,0x03,0x00,0x00,0x0F,0x2F,0xC7}},
        {0x72CAD5,{0xE8,0xBA,0x8E,0x2A,0x00,0xF3,0x0F,0x5D,0xB8,0x48,0x03,0x00,0x00,0x48,0x8B,0x43}},
        {0x72CAEE,{0x83,0xC1,0x18,0xE8,0x02,0x69,0x2A,0x00,0x48,0x8B,0x4B,0x08,0xF3,0x0F,0x10,0x70}},
        {0x72CB04,{0xF3,0x41,0x0F,0x59,0xF9,0xF3,0x0F,0x58,0xFE,0x0F,0x2F,0xF8,0x0F,0x82,0x37,0xFE}},
        {0x72CB16,{0x48,0x8B,0xCB,0xE8,0x82,0x2E,0x53,0x00,0x84,0xC0,0x0F,0x85,0x27,0xFE,0xFF,0xFF}},
        {0x937AF4,{0x40,0x53,0x48,0x83,0xEC,0x40,0x48,0x8B,0x49,0x08,0x48,0x8B,0x81,0x10,0x02,0x00}},
        {0x937B25,{0x33,0xC0,0x89,0x44,0x24,0x38,0x8B,0x05,0xB3,0x89,0x9E,0x04,0x48,0x8B,0x49,0x58}},
        {0x937B35,{0x0F,0x11,0x44,0x24,0x24,0x89,0x44,0x24,0x20,0xC6,0x44,0x24,0x34,0x01,0xE8,0x3C}},
        {0x937B64,{0x48,0x8B,0x8B,0xA8,0x03,0x00,0x00,0x48,0x8B,0x49,0x58,0xE8,0x10,0x00,0x00,0x00}},
        {0x1A76130,{0x8B,0x41,0x0C,0xC3,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}},
        {0x1A74140,{0x48,0x8D,0x41,0x58,0xC3,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC,0xCC}}
    };
    for(const auto& p:pins){unsigned char bytes[16]{};
        if(!Read(reinterpret_cast<void*>(b+p.rva),bytes)||std::memcmp(bytes,p.bytes.data(),16))return false;}
    std::uintptr_t idGetter=0,worldGetter=0;
    if(!Read(reinterpret_cast<void*>(b+0x4072C78+8),idGetter)||idGetter!=b+0x1A76130||
       !Read(reinterpret_cast<void*>(b+0x4072C78+0x100),worldGetter)||worldGetter!=b+0x1A74140)return false;
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_close_combat175",Target{DetourModKit::Address{b+0x72C97E}},options},CloseCombatAutoGate175);
    if(!h)return false;g_close_combat_gate175=std::move(*h);
    if(!g_close_combat_gate175->enable()){g_close_combat_gate175.reset();return false;}
    g_close_combat_ready175.store(true,std::memory_order_release);return true;
}
// C23b OUTCOME_CHAIN receipt: a factory query nested in OUR owned D4AD94 call (same stack, same actor).
// permit = the AL this gate leaves for the retail caller (native group result unless the opt-in VR veto rejected).
static void OutcomeFactory23(std::uintptr_t actor,bool nativeResult,bool permit) noexcept {
    if(!kcd2vr::outcome23::onStack(actor))return;
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::outcome23::with([&](kcd2vr::outcome23::Core& k){k.factory(nativeResult,nativeResult&&permit,now,kcd2vr::outcome23::counters);});
}
static void MasterFactoryGate168(DetourModKit::hook::MidContext& ctx) noexcept {
    kcd2vr::attack_xi178r8::HookTimer178r22 ht22;
    using namespace DetourModKit::hook;
    // +1483A70 has saved self in RSI and the request in RDI. +1483A8B
    // calls the action-group predicate; AL at this site is its boolean result.
    // False flows through retail fallback and constructs an EMPTY native output
    // for masterStrike. No guessed smart-pointer output writes are used here.
    if(!g_master_gate_ready168.load()||!CombatExperiment175Enabled()||!retail_build112::Ready(WhBase()))return;
    ReadScope101R2 reads;
    const auto query=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::Rdi));
    const auto self=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::Rsi));
    int type=-1,master=-1,fallbackType=-1;void* actor=nullptr;void* player=nullptr;NativeOwner104 owner{};
    if(!query||!self||!Read(query,type)||!Read(reinterpret_cast<void*>(WhBase()+0x53204B4),master)||master!=55||type!=master||
       !Read(reinterpret_cast<void*>(WhBase()+0x53204A8),fallbackType)||fallbackType==master||
       !Read(self+8,actor)||!actor||!Read(static_cast<unsigned char*>(actor)+0x1F8,player)||
       !NativeResolve104(player,owner)||owner.actor!=actor)return; // not proven Henry: original unchanged
    const bool nativeResult=(gpr(ctx,Gpr::Rax)&255)!=0;
    // D51 backup removes only our extra VR ticket veto, after the exact Henry
    // factory join. The native predicate result (including false) is untouched.
    if(kcd2vr::native_kill_nf::NativePairedBackupD51()){
        b168::Event e{};e.kind=2;e.tick=GetTickCount64();e.actor=reinterpret_cast<std::uintptr_t>(actor);
        e.type=type;e.reason=unsigned(b168::Denial::NativeBackupD51);e.nativeResult=e.allowed=nativeResult;
        OutcomeFactory23(e.actor,nativeResult,nativeResult);BridgeEvent168(e);return;}
    b168::Ticket ticket{};(void)g_master_ticket168.read(ticket);
    b168::Live live{};live.now=GetTickCount64();live.actor=reinterpret_cast<std::uintptr_t>(actor);
    live.state=reinterpret_cast<std::uintptr_t>(owner.state);live.epoch=owner.epoch;live.policy=g_master_policy168.load();
    void* opponent=nullptr;void* enemyState=nullptr;void* currentOpponent=nullptr;void* manager=nullptr;void* back=nullptr;
    PhysFrame6698 f{};TrackingSnapshot tracking{};
    const bool identity=Read(query+0x18,opponent)&&Read(static_cast<unsigned char*>(owner.state)+0xCF8,currentOpponent)&&
        opponent&&opponent==currentOpponent&&CurrentOpponent168(opponent,enemyState)&&
        Read(static_cast<unsigned char*>(player)+0x9D0,manager)&&manager==g_henry_item_attach_manager.load()&&
        PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==player&&
        NativeHandKey6697(manager,0,live.key)&&g_phys_frame6698.read(f)&&f.player==player&&f.manager==manager&&
        f.output.active[0]&&f.output.owned[0]&&f.profiles[0].key==live.key&&f.input.bodyEpoch==owner.epoch&&
        f.request==g_weapon_request_epoch6694.load()&&live.now>=f.input.tick&&live.now-f.input.tick<=80;
    live.opponent=reinterpret_cast<std::uintptr_t>(opponent);
    if(identity){
        live.generation=f.profiles[0].generation;live.request=f.request;
        live.tracked=g_gameplay_allowed.load()&&!animation_filter::nativeBodyOwned112.load()&&
            ReadTrackingSnapshot(tracking)&&tracking.allowed&&tracking.head_valid&&tracking.right_valid&&
            !tracking.head_held&&!tracking.right_held;
        // Native state property payloads (offset+8), as in the retained hit probe.
        nc104::Observed enemyAttack{};NativeOwner104 enemyOwner{};enemyOwner.actor=opponent;enemyOwner.state=enemyState;
        if(NativeObserve104(enemyOwner,enemyAttack)&&enemyAttack.type>=0&&enemyAttack.type<=9&&
           (enemyAttack.phase==4||enemyAttack.phase==8))live.enemyAttacking=true;
        // +2D8 is a transient request/candidate flag, not the attacking phase.
        // Native factory eligibility is still required; this cannot manufacture a master strike.
        (void)Read(static_cast<unsigned char*>(enemyState)+0x278,live.enemyZone); // executed attack zone; no heuristic fallback
    }
    // V66.178R18 (review F04): the VR permit rule is for OUR physical weapon. Unarmed Henry (no owned
    // physical weapon in the frame) has no ticket by design: the native rule decides (fists are native
    // prepare/zone already), the gate only observes - it never rejects a native unarmed master strike.
    PhysFrame6698 uf{};const bool armedVr=g_phys_frame6698.read(uf)&&uf.player==player&&uf.output.active[0]&&uf.output.owned[0];
    auto denial=!armedVr?b168::Denial::UnarmedNative:identity?b168::qualify(ticket,live,g_master_used168.load()):b168::Denial::Identity;
    bool permit=nativeResult&&(denial==b168::Denial::None||denial==b168::Denial::UnarmedNative);
    // R59.16C10R14 (Miki: "I could not make any masterstrikes - we probably need to expand the window"): with the VR
    // veto OFF (default) the game's own master-strike factory decides for every VR stroke; the ticket rule is only
    // logged (reason=) and never rejects. With the veto ON the R18 rule applies as before.
    const bool nativeBackupD51=kcd2vr::native_kill_nf::NativePairedBackupD51();
    const bool veto59c14=g_master_veto59c14.load(std::memory_order_relaxed)&&!nativeBackupD51;
    if(nativeBackupD51)denial=b168::Denial::NativeBackupD51;
    if(!veto59c14&&nativeResult&&armedVr&&denial!=b168::Denial::None)permit=true;
    if(!armedVr){b168::Event e{};e.kind=2;e.tick=live.now;e.serial=0;e.actor=live.actor;e.opponent=live.opponent;e.origin=-1;e.enemyZone=live.enemyZone;e.type=type;
        e.reason=unsigned(denial);e.nativeResult=nativeResult;e.allowed=permit;OutcomeFactory23(live.actor,nativeResult,permit);BridgeEvent168(e);return;}
    if(permit&&veto59c14){auto used=g_master_used168.load();permit=used!=ticket.serial&&g_master_used168.compare_exchange_strong(used,ticket.serial);
        if(!permit)denial=b168::Denial::AlreadyConsumed;}
    else if(permit&&denial==b168::Denial::None){auto used=g_master_used168.load();(void)g_master_used168.compare_exchange_strong(used,ticket.serial);} // veto off: the ledger only records
    if(nativeResult&&!permit)gpr(ctx,Gpr::Rax)&=~std::uintptr_t(255); // reject only; leave other registers/flags unchanged
    b168::Event e{};e.kind=2;e.tick=live.now;e.serial=ticket.serial;e.sample=ticket.sample;e.key=ticket.key;
    e.actor=live.actor;e.opponent=live.opponent;e.origin=ticket.origin;e.enemyZone=live.enemyZone;e.type=type;
    e.reason=unsigned(denial);e.nativeResult=nativeResult;e.allowed=permit;OutcomeFactory23(live.actor,nativeResult,permit);BridgeEvent168(e);
}
static void MasterFamilyGate59(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    if(!g_family_gate_ready59.load()||!g_master_all_weapons59.load()||!retail_build112::Ready(WhBase()))return;
    if((gpr(ctx,Gpr::Rax)&255)!=0)return; // retail already said yes
    ReadScope101R2 reads;
    const auto row=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::R8));
    const auto soul=reinterpret_cast<const void*>(gpr(ctx,Gpr::Rsi));
    int cls=-1;if(!row||!soul||!Read(row+8,cls))return;
    if(cls!=3&&cls!=5&&cls!=6&&cls!=7)return;            // axe, mace, flail, halberd: the classes the companion rows add
    ++g_family_gate_seen59;
    void* manager=g_henry_item_attach_manager.load();void* player=nullptr;NativeOwner104 owner{};
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !NativeResolve104(player,owner)||!owner.soul||owner.soul!=soul)return; // not proven Henry: retail answer stands
    gpr(ctx,Gpr::Rax)=(gpr(ctx,Gpr::Rax)&~std::uintptr_t(255))|1; // the caller re-tests AL right after this site
    const auto n=++g_family_gate_flips59;
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())&&(n<=20||n%50==0)){char m[200]{};std::snprintf(m,sizeof(m),"V66.178R59 MASTER_FAMILY_GATE actor=HENRY weaponClass=%d retail=NO -> allowed=YES flips=%llu seen=%llu",cls,(unsigned long long)n,(unsigned long long)g_family_gate_seen59.load());diagnostic_trace::Write(m);}
}
static bool ArmFamilyGate59() noexcept {
    if(g_family_gate_ready59.load())return true;
    const auto b=WhBase();if(!b||!retail_build112::Verify(b))return false;
    struct Pin{std::uintptr_t rva;std::array<unsigned char,16> bytes;};
    static constexpr Pin pins[]{
        {0x908DE6,{0x49,0x8B,0x01,0x49,0x8B,0xC9,0xFF,0x90,0x98,0x02,0x00,0x00,0x48,0x8B,0x08,0x48}},
        {0x908DF6,{0x8B,0x51,0x08,0x48,0x8B,0xC8,0xFF,0xD2,0x48,0x8B,0xC8,0x4C,0x8B,0xC0,0xE8,0x43}},
        {0x908E06,{0x0D,0x00,0x00,0x84,0xC0,0x74,0x3F,0xBB,0x49,0x00,0x00,0x00,0xEB,0x11,0x49,0x8B}},
        {0x909B4C,{0x8B,0x51,0x08,0xB8,0x01,0x00,0x00,0x00,0x3B,0xD0,0x74,0x0F,0x83,0xFA,0x10,0x77}},
        {0x909B5C,{0x0B,0xB9,0x14,0x00,0x01,0x00,0x0F,0xA3,0xD1,0x73,0x01,0xC3,0x32,0xC0,0xC3,0xCC}},
    };
    for(const auto& p:pins){unsigned char bytes[16]{};if(!Read(reinterpret_cast<void*>(b+p.rva),bytes)||std::memcmp(bytes,p.bytes.data(),16))return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_master_family59",Target{DetourModKit::Address{b+0x908E09}},options},MasterFamilyGate59);
    if(!h)return false;g_family_gate59=std::move(*h);
    if(!g_family_gate59->enable()){g_family_gate59.reset();return false;}
    g_family_gate_ready59=true;CombatBridgeTraceD84("V66.178R59 MASTER_FAMILY_GATE armed site=+908E09 scope=HENRY_ONLY classes=3,5,6,7 npc=RETAIL");return true;
}
static bool ArmMasterGate168() noexcept {
    if(g_master_gate_ready168.load())return true;
    const auto b=WhBase();if(!b||!retail_build112::Verify(b))return false;
    struct Pin{std::uintptr_t rva;std::array<unsigned char,16> bytes;};
    // Entire group predicate and the caller's tested native-false fallback are
    // additionally verified by tools/verify_native_bridges168.py against SHA256.
    static constexpr Pin pins[]{
        {0x1483A70,{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x20,0x48}},
        {0x1483A80,{0x8B,0xDA,0x49,0x8B,0xF8,0x41,0x8B,0x10,0x48,0x8B,0xF1,0xE8,0x60,0x00,0x00,0x00}},
        {0x1483A90,{0x84,0xC0,0x74,0x10,0x4C,0x8B,0xC7,0x48,0x8B,0xD3,0x48,0x8B,0xCE,0xE8,0x62,0x00}},
        {0x1483AA0,{0x00,0x00,0xEB,0x39,0x48,0x8B,0x4E,0x08,0x48,0x8B,0x57,0x18,0x48,0x8B,0x01,0xFF}},
        {0x1483AB0,{0x90,0xB8,0x05,0x00,0x00,0x84,0xC0,0x74,0x1A,0x8B,0x05,0xE9,0xC9,0xE9,0x03,0x39}},
        {0x1483AC0,{0x07,0x75,0x10,0x4C,0x8B,0xC7,0x48,0x8B,0xD3,0x48,0x8B,0xCE,0xE8,0x97,0xB2,0x1C}},
        {0x1483AD0,{0x00,0xEB,0x0A,0x33,0xD2,0x48,0x8B,0xCB,0xE8,0x17,0xDF,0xF9,0xFE,0x48,0x8B,0x74}},
        {0x1483AE0,{0x24,0x38,0x48,0x8B,0xC3,0x48,0x8B,0x5C,0x24,0x30,0x48,0x83,0xC4,0x20,0x5F,0xC3}},
        {0x1483AF0,{0x44,0x8B,0x05,0x8D,0xCA,0xE9,0x03,0x48,0x8D,0x0D,0x7A,0xC9,0xE9,0x03,0xE9,0x09}},
        {0x1483AF4,{0xCA,0xE9,0x03,0x48,0x8D,0x0D,0x7A,0xC9,0xE9,0x03,0xE9,0x09,0x79,0x54,0xFF,0xCC}}
    };
    for(const auto& p:pins){unsigned char bytes[16]{};if(!Read(reinterpret_cast<void*>(b+p.rva),bytes)||std::memcmp(bytes,p.bytes.data(),16))return false;}
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto h=mid_at(MidRequest{"kcd2vr_master_factory168",Target{DetourModKit::Address{b+0x1483A90}},options},MasterFactoryGate168);
    if(!h)return false;g_master_gate168=std::move(*h);
    if(!g_master_gate168->enable()){g_master_gate168.reset();return false;}
    g_master_gate_ready168=true;kcd2vr::outcome23::factoryArmed.store(true,std::memory_order_relaxed); // C23b receipt availability
    if(!ArmFamilyGate59())CombatBridgeTraceD84("V66.178R59 MASTER_FAMILY_GATE arm=FAILED (pins or hook) masterStrikes=RETAIL_FAMILIES_ONLY"); // R59: optional, never blocks F4
    if(!ArmMasterGate47())CombatBridgeTraceD84("C47 MS_GATE arm=FAILED (pins or hook) masterStrikes=GAME_DECIDES"); // C47: optional, never blocks F4
    if(!ArmMasterEligibilityH21())CombatBridgeTraceD84("H21 MS_ELIGIBILITY arm=FAILED reason=EXACT_RETAIL_PINS_OR_HOOK firstFailure=UNOBSERVED nativeWrites=NONE");
    if(!ArmPlayerWindowH26())CombatBridgeTraceD84("H26 PLAYER_WINDOW_ALLOWANCE arm=FAILED scope=NATIVE_UNCHANGED");
    return true;
}
static void ShutdownBridges168() noexcept {
    ShutdownFreeKickD72();
    g_special_allowed168=false;g_special_pending168=false;++g_special_context168;++g_master_policy168;
    g_master_gate_ready168=false;kcd2vr::outcome23::factoryArmed.store(false,std::memory_order_relaxed);if(g_master_gate168){(void)g_master_gate168->disable();g_master_gate168.reset();}
    g_family_gate_ready59=false;if(g_family_gate59){(void)g_family_gate59->disable();g_family_gate59.reset();} // R59
    ShutdownMasterGate47(); // C47
    ShutdownMasterEligibilityH21();
    ShutdownPlayerWindowH26();
    g_close_combat_ready175=false;if(g_close_combat_gate175){(void)g_close_combat_gate175->disable();g_close_combat_gate175.reset();}
}
struct NativeKickGuardWaitR19 {
    b168::SpecialRequest request{};std::uint64_t weaponEpoch=0,fistEpoch=0;
};
static NativeKickGuardWaitR19 g_kick_guard_wait_r19;
static std::atomic<void*> g_down_kick_dispatch_h24{nullptr};
static bool NativeFreeKickLoadoutD72(const NativeOwner104&,b168::FreeKickLoadoutD72&) noexcept;
static bool NativeDownKickLoadoutH24(const NativeOwner104& owner,std::uint64_t) noexcept {
    b168::FreeKickLoadoutD72 current{};return NativeFreeKickLoadoutD72(owner,current);
}
static bool NativeKickGuardFenceR19(void* player,std::uint64_t now) noexcept {
    // The original native handler may synchronously update guard consumers.
    // Keep our released LT up for this one balanced R3 transaction only.
    if(player&&g_down_kick_dispatch_h24.load(std::memory_order_acquire)==player)return true;
    const auto& w=g_kick_guard_wait_r19;b168::SpecialRequest live{};NativeOwner104 owner{};
    return player&&w.request.player==reinterpret_cast<std::uintptr_t>(player)&&
        g_special_pending168.load()&&g_special_allowed168.load()&&
        b168::fresh(w.request,now,g_special_context168.load(),g_pose_epoch6689.load())&&
        w.weaponEpoch==g_weapon_request_epoch6694.load()&&w.fistEpoch==g_fist_epoch101.load()&&
        !(g_guard_manual_owner112.load()==player&&g_guard_manual112.load())&&
        g_special_request168.read(live)&&live.serial==w.request.serial&&live.tick==w.request.tick&&
        live.player==w.request.player&&live.actor==w.request.actor&&live.state==w.request.state&&
        NativeResolve104(player,owner)&&reinterpret_cast<std::uintptr_t>(owner.actor)==live.actor&&
        reinterpret_cast<std::uintptr_t>(owner.state)==live.state&&owner.epoch==live.epoch;
}
// Exact retail free-unarmed/left-torch R3 input. The game owns its selector, queue,
// action, collision and RPG result. No context/MAX/type field is written here.
struct NativeFreeKickR24 {
    NativeOwner104 owner{};b168::SpecialRequest request{};
    b168::FreeKickLoadoutD72 loadout{};
    std::uint64_t manual=0,guard=0,source=0;
    bool ownsModifier=false,releaseQueued=false,dispatching=false,revoked=false;
    bool dedicatedD72=false;int inputD72=1;NativeInputScope117* scopeD72=nullptr;
    bool awaitingGuardD84=false;
};
static NativeFreeKickR24 g_native_free_kick_r24;
static void NativeFreeKickContinueD84(b168::Event&) noexcept;
static bool NativeFreeKickInputOwnerR24(void* actor) noexcept {
    return actor&&g_native_free_kick_r24.owner.actor==actor;
}
static int NativeFreeKickMaximumR24(const NativeOwner104& o) noexcept {
    const auto b=WhBase();std::uintptr_t table=0,getter=0;unsigned char value=255,code[5]{};
    static constexpr unsigned char expected[]{0x0F,0xB6,0x41,0x1D,0xC3};
    if(!retail_build112::Ready(b)||!o.state||
       !Read(static_cast<unsigned char*>(o.state)+0x630,table)||table!=b+0x3B0A968||
       !Read(reinterpret_cast<void*>(table+0x10),getter)||getter!=b+0x1A7DDF0||
       !Read(reinterpret_cast<void*>(getter),code)||std::memcmp(code,expected,sizeof code)||
       !Read(static_cast<unsigned char*>(o.state)+0x64D,value)||value>2)return -1;
    return value;
}
// 12EDF54 reads the current guard-type property at state+60 through +8,
// then compares it with the native FreeGuard enum at 5320D18. MAX at +64D
// changes earlier and is not that acknowledgement. Validate the exact reader
// and property's simple getter; copy its value without calling or writing it.
static bool NativeFreeKickGuardTypeD84(const NativeOwner104& o,int& value) noexcept {
    value=-1;const auto b=WhBase();std::uintptr_t table=0,getter=0,again=0;
    unsigned char code[4]{},pin[17]{};int freeGuard=-1;
    static constexpr unsigned char expected[]{0x8B,0x41,0x08,0xC3};
    static constexpr unsigned char use[]{0x48,0x8B,0x89,0x10,0x02,0,0,0x48,0x83,0xC1,0x60,0x48,0x8B,0x01,0xFF,0x50,0x08};
    if(!retail_build112::Ready(b)||!o.state||
       !Read(reinterpret_cast<void*>(b+0x12EDF58),pin)||std::memcmp(pin,use,sizeof pin)||
       !Read(reinterpret_cast<void*>(b+0x5320D18),freeGuard)||freeGuard!=23||
       !Read(static_cast<unsigned char*>(o.state)+0x60,table)||!table||
       !Read(reinterpret_cast<void*>(table+8),getter)||!getter||
       !Read(reinterpret_cast<void*>(getter),code)||std::memcmp(code,expected,sizeof code)||
       !Read(static_cast<unsigned char*>(o.state)+0x68,value)||value< -1||value>255||
       !Read(static_cast<unsigned char*>(o.state)+0x60,again)||again!=table)return false;
    return true;
}
static bool NativeFreeKickLoadoutD72(const NativeOwner104& o,b168::FreeKickLoadoutD72& out) noexcept {
    out={};FistOwner101 hands{};std::uint64_t right=0,left=0;
    if(!FistReadOwner101(hands)||hands.player!=o.player||hands.epoch!=o.epoch||
       !NativeHandKey6697(hands.manager,0,right)||!NativeHandKey6697(hands.manager,1,left))return false;
    out.manager=reinterpret_cast<std::uintptr_t>(hands.manager);out.right=right;out.left=left;
    out.weaponRequest=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    if(PhysEmpty6698(right)&&PhysEmpty6698(left))return true;
    const auto tick=std::uint64_t(GetTickCount64());PhysFrame6698 frame{};
    if(!g_phys_frame6698.read(frame)||frame.player!=o.player||frame.manager!=hands.manager||
       frame.input.bodyEpoch!=o.epoch||frame.request!=out.weaponRequest||
       !frame.input.tick||tick<frame.input.tick||tick-frame.input.tick>80)return false;
    for(int side=0;side<2;++side){
        const auto key=side?left:right;if(PhysEmpty6698(key))continue;
        const auto& p=frame.profiles[side];
        // Exact native equipment remains unchanged. The dedicated authored
        // rows and native database decide which legitimate pair is eligible.
        if(p.key!=key||!p.generation||p.classId<0||p.classId>17)return false;
        if(side){out.leftClass=p.classId;out.generation=p.generation;}
        else {out.rightClass=p.classId;out.rightGeneration=p.generation;}
    }
    return true;
}
// D86: 1315CB4 -> actor vf580 (1315CF8) -> actor+450 vf18.
// In the standard retail object (3A79C10 / 9D3514), a missing opponent
// returns false for EVERY current guard. D85 inferred a combat query from
// guard 0/1/2; that sent an actual FreeAttack query through the modifier-up
// path, asking for a punch/stab instead of the kick. Recognize only this exact
// read-only branch; no virtual invocation or action/context write is added.
static bool NativeFreeKickObjectD86(const NativeOwner104& o,void* combat) noexcept {
    const auto b=WhBase();std::uintptr_t actorVft=0,queryFn=0,delegateFn=0,objectVft=0,verdictFn=0;void* actor=nullptr;
    if(!combat||!retail_build112::Ready(b)||!Read(o.actor,actorVft)||
       !Read(reinterpret_cast<void*>(actorVft+0x5E0),queryFn)||queryFn!=b+0x1315CB4||
       !Read(reinterpret_cast<void*>(actorVft+0x580),delegateFn)||delegateFn!=b+0x1315CF8||
       !Read(combat,objectVft)||objectVft!=b+0x3A79C10||
       !Read(reinterpret_cast<void*>(objectVft+0x18),verdictFn)||verdictFn!=b+0x9D3514||
       !Read(static_cast<unsigned char*>(combat)+0x18,actor)||actor!=o.actor)return false;
    constexpr unsigned char query[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x01,0x8B,0xFA,0x48,0x8B,0xD9,0xFF,0x90,0x80,0x05,0x00,0x00,0x84,0xC0,0x74,0x0D,0xB0,0x01,0x48,0x8B,0x5C,0x24,0x30,0x48,0x83,0xC4,0x20,0x5F,0xC3,0x48,0x8B,0x83,0x10,0x02,0x00,0x00,0x48,0x83,0xB8,0xF8,0x0C,0x00,0x00,0x00,0x0F,0x85,0x08,0x1C,0x03,0x01,0x32,0xC0,0xEB,0xDC};
    constexpr unsigned char delegate[]{0x48,0x8B,0x89,0x50,0x04,0x00,0x00,0x48,0x85,0xC9,0x74,0x07,0x48,0x8B,0x01,0x48,0xFF,0x60,0x18,0x32,0xC0,0xC3};
    constexpr unsigned char verdict[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x41,0x18,0x48,0x8B,0xF9,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x08,0x33,0xDB,0x84,0xC0,0x75,0x0D,0x8A,0xC3,0x48,0x8B,0x5C,0x24,0x30,0x48,0x83,0xC4,0x20,0x5F,0xC3,0x48,0x8B,0x4F,0x18,0xE8,0x06,0xAA,0x91,0x00,0x84,0xC0,0x75,0xE6,0x48,0x8B,0x4F,0x18,0x48,0x8B,0x81,0x10,0x02,0x00,0x00,0x48,0x39,0x98,0xF8,0x0C,0x00,0x00,0x74,0xD2,0xE8,0x21,0x9E,0xA9,0xFF,0x84,0xC0,0x75,0xC9,0xB3,0x01,0xEB,0xC5};
    unsigned char queryBytes[sizeof(query)]{},delegateBytes[sizeof(delegate)]{},verdictBytes[sizeof(verdict)]{};
    return Read(reinterpret_cast<void*>(b+0x1315CB4),queryBytes)&&!std::memcmp(queryBytes,query,sizeof(query))&&
        Read(reinterpret_cast<void*>(b+0x1315CF8),delegateBytes)&&!std::memcmp(delegateBytes,delegate,sizeof(delegate))&&
        Read(reinterpret_cast<void*>(b+0x9D3514),verdictBytes)&&!std::memcmp(verdictBytes,verdict,sizeof(verdict));
}
static bool NativeFreeKickContextR24(const NativeOwner104& o,const b168::FreeKickLoadoutD72* expected=nullptr) noexcept {
    // Both legacy unarmed and D72 authored equipment rows use the retained
    // original block modifier. No actor context, equipment class or MAX value
    // is changed.
    // D86 recognizes the exact standard +450 object's native no-opponent
    // verdict first. D85's guard-only inference misrouted guards 0/1/2 through
    // the combat modifier-up path although the native query was FreeAttack.
    // Unknown object kinds retain the previous guard policy. The free-route
    // press still waits for observed FreeGuard 23; no equipment/state spoof.
    b168::FreeKickLoadoutD72 loadout{};void* combat=nullptr;void* opponent=nullptr;int guard=-1;
    if(kcd2vr::combat_rules178r10::kickTransport.load()!=3||
       !NativeFreeKickLoadoutD72(o,loadout)||(expected&&!b168::sameFreeKickLoadoutD72(*expected,loadout))||
       !Read(static_cast<unsigned char*>(o.actor)+0x450,combat)||
       !Read(static_cast<unsigned char*>(o.state)+0xCF8,opponent))return false;
    // No opponent is mandatory before using the byte-qualified standard
    // object's false verdict. Unknown object kinds retain the prior policy.
    if(!opponent&&combat&&NativeFreeKickObjectD86(o,combat))return true;
    const bool guardKnown=combat&&NativeFreeKickGuardTypeD84(o,guard);
    return kcd2vr::kick_route_d85::freeContext(opponent!=nullptr,combat!=nullptr,guardKnown,guard);
}
// 0 = unreadable, 1 = current actor/state, 2 = positively replaced. A temporary
// read failure revokes input but cannot forgive our outstanding native LT-up.
static int NativeFreeKickOwnerStatusD84(const NativeFreeKickR24& k,NativeOwner104& now) noexcept {
    if(!k.owner.player||!NativeResolve104(k.owner.player,now))return 0;
    return now.actor==k.owner.actor&&now.state==k.owner.state?1:2;
}
static bool NativeFreeKickSameR24(const NativeFreeKickR24& k,NativeOwner104& now) noexcept {
    return NativeFreeKickOwnerStatusD84(k,now)==1;
}
static bool NativeFreeKickTakeoverR24(const NativeFreeKickR24& k) noexcept {
    return k.manual!=g_guard_manual_occurrence_r24.load(std::memory_order_acquire)||
        k.guard!=g_guard_lease_h14.load(std::memory_order_acquire)||
        (g_guard_manual_owner112.load()==k.owner.player&&g_guard_manual112.load());
}
static bool NativeFreeKickGuardRetainedR24() noexcept {
    const auto& k=g_native_free_kick_r24;
    // NativeSpecialTick checks current context first each native pump. This
    // fence grants no posture/parry or attack permission to another producer.
    return k.owner.player&&!k.releaseQueued&&(k.dispatching||(k.ownsModifier&&(k.source||k.awaitingGuardD84||k.revoked)));
}
static void NativeFreeKickReleaseR24() noexcept {
    auto& k=g_native_free_kick_r24;NativeOwner104 now{};
    if(!k.owner.player)return;
    if(!k.ownsModifier||NativeFreeKickTakeoverR24(k)){k={};return;}
    const int identity=NativeFreeKickOwnerStatusD84(k,now);
    if(identity==0){k.revoked=true;k.awaitingGuardD84=false;return;}
    if(identity==2){k={};return;}
    if(NativeGuardReleaseAckH14(now)==0){k={};return;}
    if(!k.releaseQueued){
        // Direct original handler, including its queued Update ACK. A pose
        // epoch change revokes permission but does not orphan our native press.
        k.source=0;
        bool entered=false;(void)PhysCallAction6698(now.player,3,2,true,&entered);
        k.releaseQueued=entered;
        // Retry only a proven pre-call failure. An entered/uncertain original
        // return waits actual context0 ACK; neither timeout nor held pose is ACK.
    }
    if(NativeGuardReleaseAckH14(now)==0)k={};
}
static void NativeFreeKickTickR24() noexcept {
    auto& k=g_native_free_kick_r24;if(!k.owner.player||k.dispatching)return;
    NativeOwner104 now{};nc104::Observed state{};TrackingSnapshot tracking{};
    const int identity=NativeFreeKickOwnerStatusD84(k,now);
    if(identity==0){
        if(k.awaitingGuardD84){b168::Event event{};event.kind=1;event.serial=k.request.serial;
            event.tick=GetTickCount64();event.actor=k.request.actor;event.requestedTickD84=k.request.tick;
            event.reason=unsigned(b168::KickGateH31::OwnerResolve);BridgeEvent168(event);}
        k.revoked=true;k.awaitingGuardD84=false;return;
    }
    if(identity==2){k={};return;}
    if(k.releaseQueued){NativeFreeKickReleaseR24();return;}
    // A manual press/release occurrence or a new physical guard lease owns its
    // own cleanup. Never send our old LT-up into that later source.
    if(k.manual!=g_guard_manual_occurrence_r24.load()||k.guard!=g_guard_lease_h14.load()){
        k.ownsModifier=false;k.revoked=true;
    }
    if(k.awaitingGuardD84){
        b168::Event event{};event.kind=1;event.serial=k.request.serial;
        event.tick=GetTickCount64();event.actor=k.request.actor;
        NativeFreeKickContinueD84(event);
        // One wait edge and one terminal result, not a line each native frame.
        if(event.reason!=unsigned(b168::KickGateH31::FreeGuardPendingD84))BridgeEvent168(event);
        return;
    }
    if(!k.source||k.source!=NativePreparedSourceGenerationH26()){
        NativeFreeKickReleaseR24();return;
    }
    if(!NativeObserve104(now,state)){k.revoked=true;return;}
    if(!state.requested){NativeFreeKickReleaseR24();return;}
    const auto tick=std::uint64_t(GetTickCount64());
    const bool live=!k.revoked&&now.epoch==k.owner.epoch&&g_special_allowed168.load()&&g_gameplay_allowed.load()&&
        k.request.context==g_special_context168.load()&&!diagnostics::lab::NativeReference()&&
        !animation_filter::nativeBodyOwned112.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&NativeFreeKickContextR24(now,&k.loadout)&&
        ReadTrackingSnapshot(tracking)&&tracking.allowed&&tracking.head_valid&&!tracking.head_held&&
        tracking.tick&&tick>=tracking.tick&&tick-tracking.tick<=60&&
        tick>=k.request.tick&&tick-k.request.tick<=1500;
    if(live&&state.input==k.inputD72&&NativeFreeKickMaximumR24(now)>0)return;
    // The exact still-pending input source may be cancelled with the same
    // original balanced abort used by physical input cleanup. No admitted
    // action is retained/replayed and no later foreign source is cancelled.
    k.revoked=true;
    if(state.requested&&state.input==k.inputD72&&k.source==NativePreparedSourceGenerationH26()){
        const auto source=k.source,serial=k.request.serial;
        const bool aborted=PhysCallAction6698(now.player,2,1,true);
        if(k.request.serial!=serial||k.source!=source)return;
        if(source!=NativePreparedSourceGenerationH26()){NativeFreeKickReleaseR24();return;}
        if(aborted&&NativeFreeKickSameR24(k,now)){
            (void)PhysCallAction6698(now.player,1,2,true);
            if(k.request.serial!=serial||k.source!=source)return;
            if(source!=NativePreparedSourceGenerationH26()){NativeFreeKickReleaseR24();return;}
            if(NativeFreeKickSameR24(k,now))(void)PhysCallAction6698(now.player,2,2,true);
        }
        // Do not drop the selector modifier while our deferred request can
        // still reselect. A failed abort remains revoked, never authorized.
        if(k.source==NativePreparedSourceGenerationH26()&&
           (!NativeFreeKickSameR24(k,now)||!NativeObserve104(now,state)||state.requested))return;
    }
    NativeFreeKickReleaseR24();
}
static void NativeFreeKickGuardEdgeR24(void* player,int mode) noexcept {
    auto& k=g_native_free_kick_r24;if(!player||k.owner.player!=player||(mode!=1&&mode!=2))return;
    // The existing guard owner is about to issue its own edge. Cancel only
    // our still-pending input before it changes the modifier; never issue LT-up.
    k.ownsModifier=false;k.revoked=true;
    if(!k.dispatching)NativeFreeKickTickR24();
}
using NativeFreeTypeD72=int(__fastcall*)(void*);
static NativeFreeTypeD72 g_free_type_original_d72=nullptr;
static std::optional<DetourModKit::hook::Hook> g_free_type_hook_d72;
static std::atomic<std::uintptr_t> g_free_type_contract_d72{0};
static int NativeFreeKickTypeD72(void* factory,int original,std::uintptr_t caller) noexcept {
    const auto b=WhBase();const auto k=g_native_free_kick_r24;
    if(!factory||!k.owner.actor||!k.dedicatedD72||g_free_type_contract_d72.load()!=b||
       (caller!=b+0x687C7A&&caller!=b+0x12EDFF4&&caller!=b+0x12EDF4D))return original;
    void* actor=nullptr;void* actionFactoryOwner=nullptr;void* currentFactory=nullptr;
    if(!Read(static_cast<unsigned char*>(factory)+8,actor)||actor!=k.owner.actor||
       !Read(static_cast<unsigned char*>(actor)+0x3A8,actionFactoryOwner)||!actionFactoryOwner||
       !Read(static_cast<unsigned char*>(actionFactoryOwner)+0x50,currentFactory)||currentFactory!=factory)return original;
    const auto* edge=g_native_input_edge_d72;
    const bool dispatch=edge&&edge->owned&&edge->actor==actor&&edge->scope==k.scopeD72&&
        edge->scope&&edge->scope->kickSpecialD72&&edge->scope->kickSerialD72==k.request.serial;
    // Nested foreign input cannot borrow the outer R3. Between edges only the
    // same still-pending native source may continue the original request.
    if(edge&&!dispatch)return original;
    unsigned char pending=0;int input=-1;
    const bool deferred=!edge&&k.source&&k.source==NativePreparedSourceGenerationH26()&&
        Read(static_cast<unsigned char*>(k.owner.state)+0x2A8,pending)&&pending==1&&
        Read(static_cast<unsigned char*>(k.owner.state)+0x248,input)&&input==k.inputD72;
    if(!dispatch&&!deferred)return original;
    NativeOwner104 current{};TrackingSnapshot tracking{};const auto now=std::uint64_t(GetTickCount64());
    const bool valid=!k.revoked&&!k.releaseQueued&&retail_build112::Ready(b)&&
        (!dispatch||edge->source==NativePreparedSourceGenerationH26())&&
        NativeFreeKickSameR24(k,current)&&current.epoch==k.owner.epoch&&
        current.epoch==g_pose_epoch6689.load()&&NativeFreeKickContextR24(current,&k.loadout)&&
        NativeFreeKickMaximumR24(current)>0&&k.manual==g_guard_manual_occurrence_r24.load()&&
        k.guard==g_guard_lease_h14.load()&&g_special_allowed168.load()&&g_gameplay_allowed.load()&&
        k.request.context==g_special_context168.load()&&!diagnostics::lab::NativeReference()&&
        !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
        now>=k.request.tick&&now-k.request.tick<=1500&&
        ReadTrackingSnapshot(tracking)&&tracking.allowed&&tracking.head_valid&&!tracking.head_held&&
        tracking.tick&&now>=tracking.tick&&now-tracking.tick<=60;
    // This is the upstream REQUEST type copied by the original query builder,
    // never state+218, a selected row or an accepted result. Revocation asks
    // for no matching row, so a failed kick cannot become a stab or a shot.
    return valid?4:0x7fffffff;
}
static int __fastcall NativeFreeKickTypeHookD72(void* factory) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const int original=g_free_type_original_d72?g_free_type_original_d72(factory):-1;
    return NativeFreeKickTypeD72(factory,original,caller);
}
static bool ArmFreeKickD72() noexcept {
    const auto b=WhBase();if(b&&g_free_type_contract_d72.load()==b)return true;
    if(!retail_build112::Ready(b)||g_free_type_hook_d72)return false;
    struct Pin{std::uintptr_t rva;unsigned size;unsigned char bytes[16];};
    static constexpr Pin pins[]{
        {0x12EE05C,16,{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0x41,0x08,0x48,0x8b}},
        {0x687C69,16,{0x8b,0x05,0x09,0x89,0xc9,0x04,0x48,0x8b,0xcf,0x89,0x43,0x3c,0xe8,0xe2,0x63,0xc6}},
        {0x12EDFEF,16,{0xe8,0x68,0x00,0x00,0x00,0x48,0x8d,0x4d,0xc0,0x89,0x44,0x24,0x68,0xe8,0x93,0xa3}},
        {0x12EDF41,15,{0x48,0x8b,0x0b,0x48,0x8b,0x49,0x50,0xe8,0x0f,0x01,0x00,0x00,0x8b,0xf8,0xeb}}
    };
    for(const auto& p:pins){unsigned char actual[16]{};
        if(!Read(reinterpret_cast<void*>(b+p.rva),actual)||std::memcmp(actual,p.bytes,p.size))return false;}
    int freeFamily=-1,freeGuard=-1,special=-1;
    if(!Read(reinterpret_cast<void*>(b+0x5320578),freeFamily)||freeFamily!=26||
       !Read(reinterpret_cast<void*>(b+0x5320D18),freeGuard)||freeGuard!=23||
       !Read(reinterpret_cast<void*>(b+0x5321018),special)||special!=2)return false;
    DetourModKit::hook::InlineRequest request{"kcd2vr_d72_free_kick_request",DetourModKit::hook::Target{DetourModKit::Address{b+0x12EE05C}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&NativeFreeKickTypeHookD72);if(!hook)return false;
    g_free_type_hook_d72=std::move(*hook);g_free_type_original_d72=g_free_type_hook_d72->original<NativeFreeTypeD72>();
    if(!g_free_type_original_d72||!g_free_type_hook_d72->enable()){g_free_type_hook_d72.reset();g_free_type_original_d72=nullptr;return false;}
    g_free_type_contract_d72.store(b,std::memory_order_release);
    CombatBridgeTraceD84("D72 KICK_REQUEST ready=1 scope=OWNED_R3_SPECIAL nativeEquipment=UNCHANGED selectorAdmission=NATIVE");return true;
}
static void ShutdownFreeKickD72() noexcept {
    g_free_type_contract_d72=0;
    if(g_free_type_hook_d72){(void)g_free_type_hook_d72->disable();g_free_type_hook_d72.reset();}
    g_free_type_original_d72=nullptr;
}
static void NativeFreeKickDispatchR24(const b168::SpecialRequest& r,const NativeOwner104& owner,
    const nc104::Observed& before,b168::Event& e) noexcept {
    using Gate=b168::KickGateH31;
    const auto refuse=[&](Gate why){e.reason=unsigned(why);};
    TrackingSnapshot tracking{};const auto tick=std::uint64_t(GetTickCount64());
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||tracking.head_held||
       !tracking.tick||tick<tracking.tick||tick-tracking.tick>60){refuse(Gate::Tracking);return;}
    if(!NativeInputArm117()){refuse(Gate::InputHook);return;}
    if(before.requested||g_phys_native6698.attackPrepared){refuse(Gate::NativeRequestPending);return;}
    // R99: free block 256 / zone -1 (the state our own modifier press leaves) is ready too.
    const bool quiet=native_admission147::guardWritable(before)||native_admission147::kickInputReadyR99(before)||
        (nc104::blockMetadataH14(before)&&(before.phase==128||before.phase==256)&&NativeFreeKickMaximumR24(owner)>0);
    if(!quiet){refuse(Gate::InputNotWritable);return;}
    NativeFreeKickR24 k{};k.owner=owner;k.request=r;
    if(!NativeFreeKickLoadoutD72(owner,k.loadout)||!NativeFreeKickContextR24(owner,&k.loadout)){
        refuse(Gate::Loadout);return;
    }
    k.dedicatedD72=b168::dedicatedFreeKickD72(k.loadout);k.inputD72=k.dedicatedD72?2:1;
    if(k.dedicatedD72&&!ArmFreeKickD72()){refuse(Gate::InputHook);return;}
    k.manual=g_guard_manual_occurrence_r24.load(std::memory_order_acquire);k.guard=g_guard_lease_h14.load();
    const int maximum=NativeFreeKickMaximumR24(owner),context=NativeGuardReleaseAckH14(owner);
    if(maximum<0||context<0){refuse(Gate::GuardAckUnknown);return;}
    if(maximum==0&&(context!=0||(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load()))){
        refuse(Gate::ManualGuard);return;
    }
    diagnostics::intent168::Scope intent{{r.serial,r.actor,diagnostics::intent168::Source::R3}};
    g_native_free_kick_r24=k;g_native_free_kick_r24.dispatching=true;
    // Claim only a genuinely new original press. Existing secondary/manual
    // context is borrowed without an LT-up or a physical-posture claim.
    if(maximum==0){
        g_native_free_kick_r24.ownsModifier=true;
        if(!PhysCallAction6698(owner.player,3,1,true)){
            g_native_free_kick_r24.dispatching=false;NativeFreeKickReleaseR24();refuse(Gate::PressFailed);return;
        }
    }
    g_native_free_kick_r24.dispatching=false;
    NativeFreeKickContinueD84(e);
}
static void NativeFreeKickContinueD84(b168::Event& e) noexcept {
    using Gate=b168::KickGateH31;
    const auto k=g_native_free_kick_r24;const auto& r=k.request;const auto& owner=k.owner;
    e.requestedTickD84=r.tick;
    const auto refuse=[&](Gate why){
        e.reason=unsigned(why);
        auto& current=g_native_free_kick_r24;
        if(current.request.serial==r.serial&&current.request.tick==r.tick){
            current.awaitingGuardD84=false;current.dispatching=false;NativeFreeKickReleaseR24();
        }
    };
    const auto now=std::uint64_t(GetTickCount64());TrackingSnapshot tracking{};
    if(!b168::freshGuardWaitR99(r,now,g_special_context168.load(),g_pose_epoch6689.load())){
        (void)NativeFreeKickGuardTypeD84(owner,e.freeGuardD84); // F3 row: which guard the game had at expiry
        refuse(k.awaitingGuardD84?Gate::FreeGuardExpiredD84:Gate::RequestExpired);return;
    }
    if(k.revoked||NativeFreeKickTakeoverR24(k)){refuse(Gate::GuardWaitOwner);return;}
    if(!g_special_allowed168.load()||!g_gameplay_allowed.load()||diagnostics::lab::NativeReference()||
       animation_filter::nativeBodyOwned112.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||
       g_ctx_cutscene.load()||g_ctx_scripted.load()){
        refuse(Gate::Gameplay);return;
    }
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||tracking.head_held||
       !tracking.tick||now<tracking.tick||now-tracking.tick>60){refuse(Gate::Tracking);return;}
    if(!retail_build112::Ready(WhBase())||!OwnCombatInputHook101(WhBase())||!NativeInputArm117()){
        refuse(Gate::InputHook);return;
    }
    if(g_phys_native6698.attackPrepared){refuse(Gate::WeaponPrepared);return;}
    NativeOwner104 again{};nc104::Observed ready{};
    const bool same=NativeFreeKickSameR24(k,again)&&again.epoch==owner.epoch&&NativeFreeKickContextR24(again,&k.loadout)&&
        NativeObserve104(again,ready)&&!ready.requested&&NativeFreeKickMaximumR24(again)>0&&
        k.manual==g_guard_manual_occurrence_r24.load()&&k.guard==g_guard_lease_h14.load()&&
        g_special_allowed168.load()&&b168::freshGuardWaitR99(r,GetTickCount64(),g_special_context168.load(),g_pose_epoch6689.load());
    if(!same){refuse(Gate::AfterCancelOwner);return;}
    // R99: after our modifier press the game sits in free block 256 with zone -1
    // (R97 log). That is the expected guard, not a busy input.
    if(k.source||!(native_admission147::guardWritable(ready)||native_admission147::kickInputReadyR99(ready)||
       (nc104::blockMetadataH14(ready)&&(ready.phase==128||ready.phase==256)))){
        e.beforePhaseH31=ready.phase;e.beforeTypeH31=ready.type; // F3 row shows the state that refused
        refuse(Gate::InputNotWritable);return;
    }
    e.beforePhaseH31=ready.phase;e.beforeTypeH31=ready.type;
    e.modifierD84=NativeFreeKickMaximumR24(again);
    if(!NativeFreeKickGuardTypeD84(again,e.freeGuardD84)){refuse(Gate::GuardAckUnknown);return;}
    if(e.freeGuardD84!=23){
        // Native free-kick rows require CURRENT FreeGuard, not just the new
        // block modifier. Retain this one original R3 for its existing 120 ms
        // deadline while native Update acknowledges that guard; no input is
        // replayed and no attack/guard property is written by this adapter.
        g_native_free_kick_r24.awaitingGuardD84=true;
        e.reason=unsigned(Gate::FreeGuardPendingD84);return;
    }
    g_native_free_kick_r24.awaitingGuardD84=false;g_native_free_kick_r24.dispatching=true;
    diagnostics::intent168::Scope intent{{r.serial,r.actor,diagnostics::intent168::Source::R3}};
    if(!NativeZone104(owner.actor,5)){refuse(Gate::ZoneCall);return;}
    if(!NativeFreeKickSameR24(k,again)||again.epoch!=owner.epoch||!NativeFreeKickContextR24(again,&k.loadout)||
       NativeFreeKickMaximumR24(again)<=0||k.manual!=g_guard_manual_occurrence_r24.load()||k.guard!=g_guard_lease_h14.load()||
       !NativeFreeKickGuardTypeD84(again,e.freeGuardD84)||e.freeGuardD84!=23||
       !g_special_allowed168.load()||!b168::freshGuardWaitR99(r,GetTickCount64(),g_special_context168.load(),g_pose_epoch6689.load())){
        refuse(Gate::ZoneOwner);return;
    }
    NativeInputScope117 input{};input.owner=owner;input.stroke=ph::CommandKind::Slash;input.prepare=true;
    input.kickSpecialD72=k.dedicatedD72;input.kickSerialD72=r.serial;
    g_native_free_kick_r24.scopeD72=&input;
    auto* prior=g_input_scope117;g_input_scope117=&input;
    const auto t0=GetTickCount64();const bool press=PhysCallAction6698(owner.player,0,1,true);const auto t1=GetTickCount64();
    if(input.visited)e.inputWitnessH31|=1u;
    const bool preparation=input.prepareStableR18;const auto preparedSource=input.prepareSourceR18;
    input.prepare=false;input.visited=false;
    const bool release=press&&preparation&&preparedSource==NativePreparedSourceGenerationH26()&&
        NativeFreeKickSameR24(k,again)&&NativeFreeKickContextR24(again,&k.loadout)&&
        PhysCallAction6698(owner.player,1,2,true);const auto t2=GetTickCount64();
    if(input.visited)e.inputWitnessH31|=2u;g_input_scope117=prior;
    auto& current=g_native_free_kick_r24;
    current.scopeD72=nullptr;
    current.dispatching=false;
    if(preparation){
        if(input.releaseSourceR24&&input.releaseSourceR24==NativePreparedSourceGenerationH26())current.source=input.releaseSourceR24;
        else if(preparedSource==NativePreparedSourceGenerationH26())current.source=preparedSource;
    }
    if(!release||!input.releaseStableR24||e.inputWitnessH31!=3u)current.revoked=true;
    e.nativeResult=press;e.allowed=release;e.hold=k.dedicatedD72?-3:-2;e.msPress=unsigned(t1-t0);e.msRelease=unsigned(t2-t1);
    e.reason=!press?unsigned(Gate::PressFailed):!release?unsigned(Gate::ReleaseFailed):
        e.inputWitnessH31!=3u?unsigned(Gate::BeginNotObserved):0u;
    if(NativeFreeKickSameR24(current,again)&&NativeObserve104(again,ready)){e.type=ready.type;e.input=ready.input;e.origin=ready.zone;}
    NativeFreeKickTickR24();
}
static bool NativeFreeKickVisualCurrentR24() noexcept {
    // Called only after an active exact free-kick FIFO alias. Read CURRENT
    // membership first; never retain a native action pointer between calls.
    void* manager=g_henry_item_attach_manager.load();void* player=nullptr;NativeOwner104 owner{};
    if(!retail_build112::Ready(WhBase())||!g_gameplay_allowed.load()||diagnostics::lab::NativeReference()||
       !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!NativeResolve104(player,owner))return false;
    void* actions=nullptr;void* slots=nullptr;void* action=nullptr;void* helper=nullptr;void* row=nullptr;void* type=nullptr;
    void* back=nullptr;int channel=-1,kind=-1,stateType=-1,phase=0;unsigned char active=0;
    if(!Read(static_cast<unsigned char*>(owner.actor)+0x200,actions)||!actions||
       !Read(static_cast<unsigned char*>(actions)+0x38,slots)||!slots||!Read(slots,action)||!action||
       !PhysVft6698(action,0x3A61C58)||!Read(static_cast<unsigned char*>(action)+0x3C,channel)||channel!=0||
       !Read(static_cast<unsigned char*>(action)+0x28,active)||active!=1||
       !Read(static_cast<unsigned char*>(action)+0x40,back)||back!=actions||
       !Read(static_cast<unsigned char*>(action)+0x70,back)||back!=owner.actor||
       !Read(static_cast<unsigned char*>(action)+0xA0,helper)||!PhysVft6698(helper,0x3A61040)||
       !Read(static_cast<unsigned char*>(helper)+8,back)||back!=owner.actor||
       !Read(static_cast<unsigned char*>(helper)+0x20,back)||back!=static_cast<unsigned char*>(action)+0x88||
       !PhysVft6698(back,0x3A61440)||!Read(static_cast<unsigned char*>(action)+0x58,row)||!row||
       !Read(static_cast<unsigned char*>(row)+0x148,type)||!type||!Read(static_cast<unsigned char*>(type)+8,kind)||kind!=4||
       !Read(static_cast<unsigned char*>(owner.state)+0x218,stateType)||stateType!=4||
       !Read(static_cast<unsigned char*>(owner.state)+0x38,phase)||(phase!=4&&phase!=8&&phase!=16&&phase!=32))return false;
    NativeOwner104 current{};void* now=nullptr;
    return NativeResolve104(player,current)&&current.actor==owner.actor&&current.state==owner.state&&current.epoch==owner.epoch&&
        Read(static_cast<unsigned char*>(current.actor)+0x200,now)&&now==actions&&
        Read(static_cast<unsigned char*>(actions)+0x38,now)&&now==slots&&Read(slots,now)&&now==action&&
        Read(static_cast<unsigned char*>(action)+0x58,now)&&now==row&&
        Read(static_cast<unsigned char*>(action)+0x28,active)&&active==1;
}
static void NativeSpecialTick168() noexcept {
    NativeFreeKickTickR24(); // retire our original input even when ingress was revoked
    if(!g_special_pending168.load(std::memory_order_acquire))return;
    b168::SpecialRequest r{};if(!g_special_request168.read(r))return;
    g_special_pending168.store(false,std::memory_order_release);
    b168::Event e{};e.kind=1;e.tick=GetTickCount64();e.serial=r.serial;e.actor=r.actor;
    NativeOwner104 owner{};nc104::Observed before{};TrackingSnapshot tracking{};
    using Gate=b168::KickGateH31;
    // CL-E: the refusals a player can understand get one short in-game line (visible_failure limits it to 1 per 2 s).
    const auto sayCLE=[&](unsigned reason) noexcept {
        const char* text=nullptr;
        switch(Gate(reason)){
        case Gate::InputNotWritable:case Gate::NativeBody:case Gate::NativeRequestPending:case Gate::GuardNotQuiet:
            text=kcd2vr::kick26::RefusedTextCLE(e.tick);break;
        case Gate::WeaponPrepared:text="kick: finish or release the attack first";break;
        case Gate::ManualGuard:case Gate::FinalManualGuard:case Gate::GuardReasserted:text="kick: lower your guard first";break;
        case Gate::Gameplay:case Gate::Tracking:text="kick: not now (menu, dialogue or tracking lost)";break;
        default:break;}
        if(text)(void)kcd2vr::visible_failure::publish(text,e.tick);
    };
    const auto reject=[&](Gate reason) noexcept {e.reason=unsigned(reason);sayCLE(e.reason);BridgeEvent168(e);};
    // Throttle only a positively observed native kick; returned handlers alone are not admission.
    if(!kcd2vr::kick_route_d85::cooldown.ready(e.tick)){
        e.cooldownLeftD85=kcd2vr::kick_route_d85::cooldown.remaining(e.tick);
        CancelKickServiceForR3H24(r.serial,r.tick);reject(Gate::CooldownD85);return;
    }
    unsigned failure=0;bool observedH31=false;
    const auto require=[&](bool pass,Gate reason) noexcept {if(!pass)failure=unsigned(reason);return pass;};
    // Preserve original short-circuit order and original native calls exactly.
    const bool eligible=require(!CombatExperiment175Enabled()||kcd2vr::combat_rules178r10::kickTransport.load(std::memory_order_relaxed)==3,Gate::Mode)&&
        require(g_special_allowed168.load(),Gate::IngressDisabled)&&
        require(b168::fresh(r,e.tick,g_special_context168.load(),g_pose_epoch6689.load()),Gate::RequestExpired)&&
        require(retail_build112::Ready(WhBase()),Gate::Retail)&&require(OwnCombatInputHook101(WhBase()),Gate::InputHook)&&
        require(g_gameplay_allowed.load(),Gate::Gameplay)&&require(!diagnostics::lab::NativeReference(),Gate::Reference)&&
        require(!animation_filter::nativeBodyOwned112.load(),Gate::NativeBody)&&
        require(ReadTrackingSnapshot(tracking)&&tracking.allowed&&tracking.head_valid&&!tracking.head_held,Gate::Tracking)&&
        require(NativeResolve104(reinterpret_cast<void*>(r.player),owner),Gate::OwnerResolve)&&
        require(reinterpret_cast<std::uintptr_t>(owner.actor)==r.actor,Gate::ActorChanged)&&
        require(reinterpret_cast<std::uintptr_t>(owner.state)==r.state,Gate::StateChanged)&&
        require((observedH31=NativeObserve104(owner,before)),Gate::Observe)&&
        require(nc104::valid(before)||(kcd2vr::combat_rules178r10::kickTransport.load()==3&&nc104::blockMetadataH14(before)&&
             (NativeHeldGuardOwnerH14(owner)||g_kick_guard_wait_r19.request.serial==r.serial||
              (NativeFreeKickContextR24(owner)&&NativeFreeKickMaximumR24(owner)>0))),Gate::NativeMetadata);
    if(observedH31){e.beforePhaseH31=before.phase;e.beforeTypeH31=before.type;}
    if(owner.actor&&owner.state){ // D85 kick (F3 row): read-only no-target facts
        void* opponentD85=nullptr;void* combatD85=nullptr;std::uintptr_t vt=0,vf18=0;const auto b=WhBase();
        e.opponentD85=Read(static_cast<unsigned char*>(owner.state)+0xCF8,opponentD85)?(opponentD85?1:0):-1;
        if(Read(static_cast<unsigned char*>(owner.actor)+0x450,combatD85)&&combatD85){
            e.combatObjD85=true;
            if(Read(combatD85,vt)&&vt){e.combatObjVtD85=vt>=b&&vt-b<0x5600000?vt-b:vt;
                if(Read(reinterpret_cast<void*>(vt+0x18),vf18))e.combatObjVf18D85=vf18>=b&&vf18-b<0x5600000?vf18-b:vf18;}
        }
    }
    if(!eligible){e.reason=failure;sayCLE(e.reason);BridgeEvent168(e);return;}
    // The existing DOWN5/HEAVY route also selects the retail empty-hand kick.
    // A kick is proved by the native selected row, not by returning from the
    // SPECIAL input handler. Keep the same current loadout and native rules.
    const bool direct17=kcd2vr::combat_rules178r10::kickTransport.load(std::memory_order_relaxed)==3;
    b168::FreeKickLoadoutD72 kickLoadoutD72{};
    if(direct17&&!NativeFreeKickLoadoutD72(owner,kickLoadoutD72)){
        (void)kcd2vr::visible_failure::publish("Kick unavailable: this loadout has no verified native kick selector",e.tick);
        // This refused request must not later draw a fallback knee. Preserve
        // any replacement request published by a native/reentrant owner.
        CancelKickServiceForR3H24(r.serial,r.tick);
        reject(Gate::Loadout);return;
    }
    if(direct17&&!NativeInputArm117()){reject(Gate::InputHook);return;}
    const bool dedicatedD72=direct17&&b168::dedicatedCombatKickD72(kickLoadoutD72);
    // Prove the kick loadout BEFORE releasing any native prepare or guard.
    // A refused ranged R3 must leave bow/gun aim and its pending action intact.
    if(g_phys_native6698.attackPrepared&&g_phys_native6698.heldPrepare57){
        PhysCancelAttack117();
        NativeOwner104 afterCancelH24{};
        if(!NativeResolve104(owner.player,afterCancelH24)||afterCancelH24.actor!=owner.actor||
           afterCancelH24.state!=owner.state||afterCancelH24.epoch!=owner.epoch||!NativeObserve104(afterCancelH24,before)){
            reject(Gate::AfterCancelOwner);return;
        }
    }
    if(g_native_free_kick_r24.owner.player){reject(Gate::NativeRequestPending);return;}
    if(direct17){ // D85 kick: a free guard without opponent no longer goes to the R19 route (guard released -> punch)
        namespace kr=kcd2vr::kick_route_d85;
        const auto route=kr::choose(e.opponentD85!=0,NativeFreeKickContextR24(owner));e.routeD85=int(route);
        if(route==kr::Route::Free){NativeFreeKickDispatchR24(r,owner,before,e);BridgeEvent168(e);return;}
    }
    if(before.requested){reject(Gate::NativeRequestPending);return;}
    if(g_phys_native6698.attackPrepared){reject(Gate::WeaponPrepared);return;}
    // One R3 may hand off OUR raised posture, never manual/foreign guard.
    // Keep the original serial/tick/deadline while native Update clears LT.
    const bool waitingR19=g_kick_guard_wait_r19.request.serial==r.serial;
    const bool manualR19=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
    if(manualR19){reject(Gate::ManualGuard);return;}
    if(!waitingR19&&NativeHeldGuardOwnerH14(owner)){
        g_kick_guard_wait_r19={r,g_weapon_request_epoch6694.load(),g_fist_epoch101.load()};
        NativeGuardRelease112("R19_R3_WAIT_NATIVE_GUARD_RELEASE");
        if(b168::fresh(r,GetTickCount64(),g_special_context168.load(),g_pose_epoch6689.load()))g_special_pending168=true;
        return;
    }
    if(waitingR19){
        if(g_kick_guard_wait_r19.request.tick!=r.tick||
           g_kick_guard_wait_r19.request.context!=r.context||
           g_kick_guard_wait_r19.weaponEpoch!=g_weapon_request_epoch6694.load()||
           g_kick_guard_wait_r19.fistEpoch!=g_fist_epoch101.load()||NativePhysicalGuardHeld117(owner.player)){
            reject(Gate::GuardWaitOwner);return;
        }
        const int ack=NativeGuardReleaseAckH14(owner);
        const bool quietReady=(nc104::valid(before)||nc104::blockMetadataH14(before))&&
            (before.type==-1||before.type==1)&&!before.requested&&!before.requestCandidate&&
            (before.phase==1||before.phase==2||before.phase==128||before.phase==256);
        if(!quietReady){reject(Gate::GuardNotQuiet);return;}
        if(ack==1){g_special_pending168=true;return;}
        if(ack!=0){reject(Gate::GuardAckUnknown);return;}
    }
    // 90F824 changes DESIRED +F8/+188, not the live phase/block metadata.
    // After OUR exact guard's LT-up ACK the original selector may replace its
    // residual128/256 block. Waiting for neutral here deadlocks that input.
    // No foreign guard, protected action or renewed ingress gets this handoff.
    const auto inputReadyH19=[&](const nc104::Observed& s) noexcept {
        if(native_admission147::guardWritable(s))return true;
        // R99: a residual free block (phase 256, no request, no candidate) after an
        // earlier kick/block is not busy; held/manual guards were already handled above
        // and are re-checked after the zone write (GuardReasserted).
        if(native_admission147::kickInputReadyR99(s)&&!NativePhysicalGuardHeld117(owner.player))return true;
        return waitingR19&&(nc104::valid(s)||nc104::blockMetadataH14(s))&&
            (s.type==-1||s.type==1)&&!s.requested&&!s.requestCandidate&&
            (s.phase==1||s.phase==2||s.phase==128||s.phase==256)&&NativeGuardReleaseAckH14(owner)==0;
    };
    if(!inputReadyH19(before)){reject(Gate::InputNotWritable);return;}
    if(!NativeZone104(owner.actor,5)){reject(Gate::ZoneCall);return;}
    NativeOwner104 zoneOwnerR19{};nc104::Observed zoneStateR19{};int zoneR19=-1;
    if(!NativeResolve104(owner.player,zoneOwnerR19)||zoneOwnerR19.actor!=owner.actor||zoneOwnerR19.state!=owner.state||
       zoneOwnerR19.epoch!=owner.epoch||!NativeObserve104(zoneOwnerR19,zoneStateR19)){reject(Gate::ZoneOwner);return;}
    if(!inputReadyH19(zoneStateR19)){reject(Gate::InputNotWritable);return;}
    if(NativePhysicalGuardHeld117(owner.player)||
       (g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load())){reject(Gate::GuardReasserted);return;}
    if(!Read(static_cast<unsigned char*>(owner.state)+0xF8,zoneR19)){reject(Gate::ZoneRead);return;}
    if(zoneR19!=5){reject(Gate::ZoneMismatch);return;}
    g_kick_guard_wait_r19={};
    diagnostics::intent168::Scope scope{{r.serial,r.actor,diagnostics::intent168::Source::R3}};
    // Like the old key tap, balanced synchronous native edges. No key binding,
    // inventory/weapon-class spoof, automatic retry, or worker-thread game call.
    // Default transport3 now uses the native DOWN/HEAVY lifecycle below.
    // Legacy transport1/2 retains attack_special as a comparison. A returned
    // handler is transport evidence, never proof that a kick/hit was accepted.
    const auto t0=GetTickCount64();
    // H17: eligibility reads may overlap a raw tracking/style/context revoke.
    // Recheck the original ingress immediately before its first native edge;
    // after a press, retain the balanced release (never abort a native clip).
    if(!g_special_allowed168.load(std::memory_order_acquire)){reject(Gate::FinalIngress);return;}
    if(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load()){reject(Gate::FinalManualGuard);return;}
    if(!b168::fresh(r,t0,g_special_context168.load(std::memory_order_acquire),g_pose_epoch6689.load())){reject(Gate::FinalRequest);return;}
    b168::FreeKickLoadoutD72 currentLoadoutD72{};
    if(direct17&&(!NativeFreeKickLoadoutD72(owner,currentLoadoutD72)||
       !b168::sameFreeKickLoadoutD72(kickLoadoutD72,currentLoadoutD72))){reject(Gate::Loadout);return;}
    // H24: the installed armed kick rows, like the retail DOWN kick, are
    // input_class_id=1 (HEAVY), attack_zone_id=5. attack_special transports
    // input2 and is not the same selection. The original prepare handler
    // 27C08C8 calls D4AAC4 with the retail HEAVY constant; primary release
    // 27C08F8 passes that SAME constant to D4A99C. No short-press classifier,
    // inventory spoof, forced attack type, direct hit or extra stamina debit.
    struct DownScopeH24 {
        NativeInputScope117 input{};NativeInputScope117* prior=nullptr;void* priorGuard=nullptr;bool active=false;
        DownScopeH24(const NativeOwner104& owner,bool enabled,bool specialD72,std::uint64_t serial):active(enabled){
            if(!active)return;input.owner=owner;input.stroke=ph::CommandKind::Slash;input.prepare=true;
            input.kickSpecialD72=specialD72;input.kickSerialD72=serial;
            prior=g_input_scope117;g_input_scope117=&input;
            priorGuard=g_down_kick_dispatch_h24.exchange(owner.player,std::memory_order_acq_rel);
        }
        ~DownScopeH24(){if(active){g_input_scope117=prior;g_down_kick_dispatch_h24.store(priorGuard,std::memory_order_release);}}
    } downScopeH24(owner,direct17,dedicatedD72,r.serial);
    const bool pressed=PhysCallAction6698(owner.player,direct17?0:4,1,direct17);const auto t1=GetTickCount64();
    if(direct17&&downScopeH24.input.visited)e.inputWitnessH31|=1u;
    downScopeH24.input.prepare=false;downScopeH24.input.visited=false;
    const auto t2=t1;
    const bool sameLoadoutD72=!direct17||(NativeFreeKickLoadoutD72(owner,currentLoadoutD72)&&
        b168::sameFreeKickLoadoutD72(kickLoadoutD72,currentLoadoutD72));
    const bool released=pressed&&sameLoadoutD72&&PhysCallAction6698(owner.player,direct17?1:4,2,direct17);const auto t3=GetTickCount64();
    if(pressed&&!sameLoadoutD72&&downScopeH24.input.prepareStableR18&&
       downScopeH24.input.prepareSourceR18==NativePreparedSourceGenerationH26())PhysAbortAttack117(owner.player);
    if(direct17&&downScopeH24.input.visited)e.inputWitnessH31|=2u;
    e.nativeResult=pressed;e.allowed=released;e.hold=dedicatedD72?-3:direct17?-2:-1;e.msPress=unsigned(t1-t0);e.msHold=0;e.msRelease=unsigned(t3-t2);
    NativeOwner104 after{};nc104::Observed state{};
    if(NativeResolve104(owner.player,after)&&after.actor==owner.actor&&after.state==owner.state&&NativeObserve104(after,state)){
        e.type=state.type;e.input=state.input;e.origin=state.zone;
    }
    e.reason=pressed?(released?0u:3u):2u;
    if(!e.reason&&direct17&&e.inputWitnessH31!=3u)e.reason=unsigned((e.inputWitnessH31&1u)?Gate::ReleaseNotObserved:Gate::BeginNotObserved);
    BridgeEvent168(e);
}
static void ServiceBridges168() noexcept {
    const bool recording=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
    b168::Event e{};for(unsigned i=0;i<64&&g_bridge_events168.pop(e);++i){char text[1024]{};
        // V66.178R15 rung 4 coach: a permitted master-strike factory decision = one firm haptic on the weapon hand.
        if(e.kind==2&&e.allowed&&F4Rung178r10()>=4)(void)kcd2vr::haptics101::publish_event(0,0x4D415354ull|e.serial,.6f,60,std::uint64_t(GetTickCount64()));
        if(!recording||!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,e.tick))continue;
        if(e.kind==1)std::snprintf(text,sizeof(text),
            "V66.168 R3_NATIVE serial=%llu tick=%llu actor=%llX pressReturned=%d holdReturned=%d releaseReturned=%d msPress=%u msHold=%u msRelease=%u reason=%u gate=%s beforePhase=%d beforeType=%d inputEdges=%u observedAttackType=%d input=%d zone=%d transport=%s requestTick=%llu requestAgeMs=%llu freeGuard=%d modifierMax=%d opponent=%d combatObj=%d combatObjVt=%llX combatObjVf18=%llX route=%s cooldownLeftMs=%llu selection=NATIVE_CONTEXTUAL hit=UNCLAIMED",
            (unsigned long long)e.serial,(unsigned long long)e.tick,(unsigned long long)e.actor,e.nativeResult,e.hold,e.allowed,e.msPress,e.msHold,e.msRelease,e.reason,b168::kickGateNameH31(e.reason),e.beforePhaseH31,e.beforeTypeH31,e.inputWitnessH31,e.type,e.input,e.origin,e.hold==-3?"D72_SPECIAL_NATIVE":e.hold==-2?"DOWN5_HEAVY_NATIVE":e.hold>=0?"DIRECT_HOLD":"EXACT_ONACTION",
            (unsigned long long)e.requestedTickD84,(unsigned long long)(e.requestedTickD84&&e.tick>=e.requestedTickD84?e.tick-e.requestedTickD84:0),e.freeGuardD84,e.modifierD84,
            e.opponentD85,e.combatObjD85?1:0,(unsigned long long)e.combatObjVtD85,(unsigned long long)e.combatObjVf18D85,
            e.routeD85<0?"PRE_ROUTE":kcd2vr::kick_route_d85::name(kcd2vr::kick_route_d85::Route(e.routeD85)),(unsigned long long)e.cooldownLeftD85);
        else if(e.kind==3)std::snprintf(text,sizeof(text),
            "V66.175 CLOSE_COMBAT tick=%llu sample=%llu actor=%llX opponent=%llX emptyFistSide=%d nativeSyncStartType=%d autotrigger=REJECTED boundary=72C97E source=F10_FRESH_CLOSED_EMPTY_HANDS standOff=UNCHANGED ongoingNativeActions=UNCHANGED hit=UNCLAIMED",
            (unsigned long long)e.tick,(unsigned long long)e.sample,(unsigned long long)e.actor,(unsigned long long)e.opponent,e.origin,e.type);
        else std::snprintf(text,sizeof(text),
            "V66.168 MASTER_FACTORY serial=%llu tick=%llu sample=%llu actor=%llX opponent=%llX key=%016llX origin=%d enemyExecutedZone=%d actionType=%d nativeGroupMatch=%d vrPermit=%d reject=%s boundary=1483A90 scope=KNOWN_FACTORY_ONLY damage=UNCLAIMED",
            (unsigned long long)e.serial,(unsigned long long)e.tick,(unsigned long long)e.sample,(unsigned long long)e.actor,(unsigned long long)e.opponent,(unsigned long long)e.key,e.origin,e.enemyZone,e.type,e.nativeResult,e.allowed,b168::name(b168::Denial(e.reason)));
        diagnostic_trace::Write(text);
    }
    { // C23b OUTCOME_CHAIN: close idle chains; at most 4 closed-chain lines per pump, F3 combat capture only (edge-only, bounded)
        const auto now23=std::uint64_t(GetTickCount64());namespace oc=kcd2vr::outcome23;
        oc::with([&](oc::Core& k){k.pump(now23,oc::counters);});
        for(unsigned i=0;i<4;++i){oc::Chain c{};bool got=false;oc::with([&](oc::Core& k){got=k.pop(c);});if(!got)break;
            if(!recording)continue; // counters (COMBAT_FLOW) keep counting; the detail line is F3-only
            char line[1400]{};oc::format(c,now23,line,sizeof(line));diagnostic_trace::Write(line);oc::counters.lines.fetch_add(1,std::memory_order_relaxed);
            char feint59[2400]{};if(oc::formatFeint59(c,feint59,sizeof(feint59)))diagnostic_trace::Write(feint59);}
    }
    if(const auto dropped=g_bridge_events168.dropped.exchange(0);dropped&&recording){char m[160]{};std::snprintf(m,sizeof(m),"V66.168 BRIDGE_DROPPED count=%llu causalCompleteness=UNKNOWN",(unsigned long long)dropped);diagnostic_trace::Write(m);}
}
