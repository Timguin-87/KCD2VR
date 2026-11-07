#include "../diagnostics/hmd_dodge_context_h24.h"
// Included in the sole native pose TU after the old fist/weapon adapters.
// No guessed RPG hit structures, direct damage writes, or forced action IDs.
namespace nc104=kcd2vr::native_combat104;
static std::atomic<bool> g_guard_countermotion57c1{true};
// R59.16C10R16 (Miki: "block and move against the weapon, we emit perfect block"): in F4 press 1 (VR INTENT) the held guard is the
// native block, and the perfect-block re-press inside the native window needs the guard hand to move toward the incoming side.
// The other F4 modes keep the R45 automatic re-press on every window.
// R59.16C10R17: "parrying (from block against the weapon) = perfect block" in EVERY F4 mode (rung 3 SYNC too): with the menu switch
// PARRY NEEDS COUNTER-MOTION on, holding the guard alone never re-presses; the guard hand must move against the incoming blow.
static bool GuardMotionRequired59c3() noexcept {return g_guard_countermotion57c1.load();}
// R59.16C10R17 receipts (GUARD_R17 line every 2 s with GUARD_SUMMARY)
static std::atomic<std::uint64_t> g_guard_stale59c17{0},g_guard_against59c17{0},g_guard_lateral_toward59c17{0},g_guard_lateral_mirror59c17{0};
static std::atomic<std::uint64_t> g_guard_feints_total59c17{0},g_guard_forward_total59c17{0},g_guard_incoming_reads59c17{0},g_guard_perceived_match59c17{0};
static std::atomic<float> g_guard_against_last59c17{0.f};static std::atomic<int> g_guard_zone158_last59c17{-1};
static std::atomic<bool> g_guard_strict_seen59c17{false},g_block_cooldown_lease59c17{false};
static std::atomic<std::uint64_t> g_guard_against_total59c17{0},g_guard_toward_total59c17{0},g_guard_mirror_total59c17{0},g_guard_reads_total59c17{0}; // cumulative (normal-log line) // the latter: mod.cpp's wh_cs_BlockCooldown=.001 lease (SetBlockCooldownLease59c17)
static std::atomic<bool> g_ms_keep_stance59c17{true}; // shared guard/input stance policy
static std::atomic<bool> g_no_step55{true};                 // INI VrCombatNoStep (V66.178R55; used by native_motion112.inl)
static std::atomic<std::uint64_t> g_alignment_skips55{0};   // V66.178R55: native attack alignment queries answered "none" for Henry's VR attack
using NativeRequestFn104=void(__fastcall*)(void*,std::int32_t,bool);
using NativeZoneFn104=void(__fastcall*)(void*,std::int32_t,std::int64_t,bool);
using NativeDamageFn104=std::int64_t(__fastcall*)(void*);
static NativeRequestFn104 g_native_request104=nullptr;
static NativeZoneFn104 g_native_zone104=nullptr;
static NativeDamageFn104 g_native_damage_original104=nullptr;
static std::optional<DetourModKit::hook::Hook> g_native_damage_hook104;
static std::atomic<std::uintptr_t> g_native_contract104{0};
static std::mutex g_native_arm_mutex104;
static std::atomic<bool> g_native_fault104{false}; // Native exceptions require validated reset, never a timed retry.
static std::atomic<std::uint64_t> g_native_frame104{0};
struct NativeOwner104 {void* player=nullptr;void* entity=nullptr;void* actor=nullptr;void* state=nullptr;void* soul=nullptr;std::uint64_t epoch=0;};
static int NativeSynchronizedAttackOwnedC10R18(const NativeOwner104&,const PhysFrame6698&,std::uint64_t) noexcept;
static bool NativePendingBlockCommitR18(const NativeOwner104&,const ph::Command&,
    const PhysFrame6698&,const nc104::Observed&,std::uint64_t) noexcept;
// The native input offer is not yet an admitted action. Protect it only while
// the exact same forward candidate remains live, and never extend its onset.
static bool NativePrimaryFrameOwned19c2(const NativeOwner104& owner,const PhysFrame6698& f,std::uint64_t now) noexcept {
    std::uint64_t liveKey=0;
    return CombatIntegrated178r22c2Enabled()&&!f.input.motionOwned59c5&&
        owner.player&&owner.actor&&owner.state&&owner.epoch==f.input.bodyEpoch&&f.player==owner.player&&
        f.input.epoch==f.request&&f.request==g_weapon_request_epoch6694.load()&&f.input.bodyEpoch==g_pose_epoch6689.load()&&
        f.input.sample&&f.input.sample==g_phys_ready_sample6698.load(std::memory_order_acquire)&&
        f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&f.input.tracked[1]&&
        ph::attack_authority55c1(f.input,f.output,0)&&f.profiles[0].melee()&&f.profiles[0].primary&&f.profiles[0].side==0&&
        f.profiles[0].key&&f.profiles[0].generation&&
        !f.output.guardBySide30c1[0]&&
        f.manager==g_henry_item_attach_manager.load()&&NativeHandKey6697(f.manager,0,liveKey)&&liveKey==f.profiles[0].key&&
        f.input.sample==g_phys_ready_sample6698.load(std::memory_order_acquire);
}
static bool NativePrimaryForwardStroke19c2(const NativeOwner104& owner,const PhysFrame6698& f,std::uint64_t now) noexcept {
    return f.output.strokeActive119&&f.output.primaryStrokeGesture19&&NativePrimaryFrameOwned19c2(owner,f,now);
}
// A newer XR publication can finish the recognizer before the original queued
// release crosses the next native frame. The exact still-queued release, not a
// remembered gesture or an added grace interval, proves this pending handoff.
static bool NativeQueuedForwardRelease19c2(const PhysNativeOwner6698& n,const PhysFrame6698& f,std::uint64_t now) noexcept {
    ph::Command c{};
    return n.prepareSample117&&g_phys_commands6698.peek(c)&&!c.kick59&&c.side==0&&
        ((c.kind==ph::CommandKind::Slash&&c.stroke117==ph::CommandKind::Slash)||
         (c.kind==ph::CommandKind::Thrust&&c.stroke117==ph::CommandKind::Thrust))&&
        c.stroke117==n.stroke117&&c.gesture59c3==n.prepareGestureC10R18&&c.key==n.attackKey&&c.generation==n.attackGeneration117&&
        c.bodyEpoch==n.epoch&&c.epoch==n.request&&c.sample>=n.prepareSample117&&c.sample<=f.input.sample&&
        c.tick>=n.prepareTickC10R18&&now>=c.tick&&now-c.tick<=100;
}
static bool NativeForwardPrepareOwned19c2(const NativeOwner104& owner,const PhysFrame6698& f,
    const nc104::Observed& state,std::uint64_t now) noexcept {
    const auto& n=g_phys_native6698;
    // H17: GuardTick precedes the actual release consumer. The native pending
    // block row is not guard-input/zone authority, but it must not let a raised
    // offhand shield cancel this SAME original release after XR ended its stroke.
    // Reuse its full R18 source/gesture/item proof and original 100ms deadline;
    // no active recognizer, remembered gesture or pending row alone qualifies.
    if(nc104::pendingBlockInputR18(state)){
        ph::Command queued{};
        return NativePrimaryFrameOwned19c2(owner,f,now)&&NativeQueuedForwardRelease19c2(n,f,now)&&
            g_phys_commands6698.peek(queued)&&NativePendingBlockCommitR18(owner,queued,f,state,now);
    }
    return NativePrimaryFrameOwned19c2(owner,f,now)&&
        n.attackPrepared&&n.prepareGestureC10R18&&n.prepareTickC10R18&&
        now>=n.prepareTickC10R18&&now-n.prepareTickC10R18<=600&&
        n.player==owner.player&&n.epoch==owner.epoch&&n.epoch==f.input.bodyEpoch&&n.request==f.request&&
        n.attackKey==f.profiles[0].key&&n.attackGeneration117==f.profiles[0].generation&&
        ((f.output.strokeActive119&&f.output.primaryStrokeGesture19==n.prepareGestureC10R18)||NativeQueuedForwardRelease19c2(n,f,now))&&
        (native_admission147::guardInput(state)||native_admission147::freeGuardHeld44c2(state,g_guard_physical112.load()))&&
        NativePreparedOwnerMatches117(owner.player);
}
static bool NativeSynchronizedPrepareOwnedC10R18(const NativeOwner104& owner,const PhysFrame6698& f,
    const nc104::Observed& state,std::uint64_t now) noexcept {
    return CombatSynchronized59c10r9()&&NativeForwardPrepareOwned19c2(owner,f,state,now);
}
struct NativePending104 {
    NativeOwner104 owner{};nc104::Intent intent{};nc104::Observed before{};
    std::uint64_t serial=0,tick=0,frame=0,key=0;int input=-1;
    bool held=false,watching=false,entered=false;
};
static NativePending104 g_native_pending104;
static std::uint64_t g_native_serial104=0;
struct NativeResult104 {
    std::uint64_t tick=0,serial=0,key=0;std::uintptr_t actor=0,target=0;
    unsigned kind=0,flags=0;int gesture=0,phase=0,type=-1,zone=-1,guardZone=-1,hand=-1,input=-1,expectedType=-1;
    float health=0,stamina=0;
};
// Bounded MPSC try-lock: copying one POD event only. No formatting or file I/O
// in game callbacks; readers never retain borrowed native event pointers.
static std::array<NativeResult104,128> g_native_events104{};
static unsigned g_native_head104=0,g_native_tail104=0;
static std::atomic_flag g_native_events_lock104=ATOMIC_FLAG_INIT;
static std::atomic<std::uint64_t> g_native_dropped104{0};
static kcd2vr::core::SnapshotMailbox<NativeOwner104> g_native_owner104;
static bool NativePush104(NativeResult104 e) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return false;
    if(g_native_events_lock104.test_and_set(std::memory_order_acquire)){++g_native_dropped104;return false;}
    const auto next=(g_native_head104+1)%g_native_events104.size();
    if(next==g_native_tail104){g_native_events_lock104.clear(std::memory_order_release);++g_native_dropped104;return false;}
    g_native_events104[g_native_head104]=e;g_native_head104=next;g_native_events_lock104.clear(std::memory_order_release);return true;
}
static bool NativeResolve104(void* player,NativeOwner104& o) noexcept {
    o={};void* back=nullptr;std::uintptr_t vft=0;const auto b=WhBase();
    if(!b||!player||!PhysVft6698(player,0x3AA4B98)||
       !Read(static_cast<unsigned char*>(player)+0x38,o.entity)||o.entity!=g_last_player_entity.load()||!PhysVft6698(o.entity,0x4072C78)||
       !Read(static_cast<unsigned char*>(player)+0x278,o.actor)||!Read(o.actor,vft)||vft!=b+0x3A80610||
       !Read(static_cast<unsigned char*>(o.actor)+0x1F8,back)||back!=player||
       !Read(static_cast<unsigned char*>(o.actor)+0x210,o.state)||!o.state||
       !Read(static_cast<unsigned char*>(o.state)+0xCE0,back)||back!=o.actor)return false;
    // Existing runtime-proved combo identity uses the C_Player link at +1F8.
    // Do not substitute the donor's tentative IEntity label for that live proof.
    (void)Read(static_cast<unsigned char*>(player)+0x668,o.soul);
    o.player=player;o.epoch=g_pose_epoch6689.load();return true;
}
#include "fist_combat_ready_h27.inl"
static bool NativeObserve104(const NativeOwner104& o,nc104::Observed& s,nc104::ReadReceipt59* receipt59=nullptr) noexcept {
    if(receipt59)return nc104::observe59(reinterpret_cast<std::uintptr_t>(o.actor),reinterpret_cast<std::uintptr_t>(o.state),s,
        [](const void* at,auto& value){return Read(at,value);},*receipt59);
    // Noncapture callers retain the original reads/short-circuit fast path.
    s={};void* current=nullptr;void* back=nullptr;
    if(!o.state||!Read(static_cast<unsigned char*>(o.actor)+0x210,current)||current!=o.state||
       !Read(static_cast<unsigned char*>(o.state)+0xCE0,back)||back!=o.actor)return false;
    const auto at=static_cast<unsigned char*>(o.state);unsigned char requested=0,candidate=0,blocking=0;
    if(!Read(at+0x38,s.phase)||!Read(at+0x218,s.type)||!Read(at+0x248,s.input)||!Read(at+0x278,s.zone)||!Read(at+0x1B8,s.hand)||
       !Read(at+0x2A8,requested)||!Read(at+0x2D8,candidate)||!Read(at+0x308,blocking)||requested>1||candidate>1||blocking>1)return false;
    s.requested=requested!=0;s.requestCandidate=candidate!=0;s.blocking=blocking!=0;return nc104::observedContextValidH14(s);
}
static void ObserveF11Native167(void* player,const char* edge,int side,std::uint64_t sample,int originZone,
    pm::Vec3 origin,int gesture,std::uint64_t key,bool returned,bool handler,int passed) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return;
    // Once per deliberate input edge, not a new per-frame native readback.
    NativeOwner104 owner{};nc104::Observed state{};cc::Event e{};
    e.kind=cc::Kind::F11Native167;e.tick=GetTickCount64();e.sequence=sample;e.mode=side;e.id=originZone;e.key[0]=key;
    e.aliasHash=gesture;e.layer=-1;e.value=float(passed);e.flags=(returned?1u:0u)|(handler?4u:0u);
    for(float& v:e.current)v=-1;
    if(NativeResolve104(player,owner)&&NativeObserve104(owner,state)){
        e.flags|=2;e.actor=reinterpret_cast<std::uintptr_t>(owner.actor);
        (void)Read(static_cast<unsigned char*>(owner.state)+0xF8,e.layer);
        e.current[0]=float(state.zone);e.current[1]=float(state.type);e.current[2]=float(state.phase);
        e.current[3]=float(state.hand);e.current[4]=float(state.input);
    }
    if(edge&&!std::strncmp(edge,"BOXING_",7)){
        fp3::Record r{};r.join=FistJoinC54R3();r.join.sample=sample;r.join.side=side;r.tick=e.tick;r.stage=fp3::Stage::NativeSnapshot;
        r.why=(e.flags&2)?fp3::NativeStateObserved:fp3::NativeStateUnreadable;r.mask=e.flags;r.nativeActor=reinterpret_cast<std::uintptr_t>(owner.actor);
        if(e.flags&2){r.nativePhase=state.phase;r.nativeType=state.type;r.nativeInput=state.input;r.nativeHand=state.hand;r.nativeZone=state.zone;
            r.nativeRequested=state.requested;r.nativeCandidate=state.requestCandidate;r.nativeBlocking=state.blocking;
            fp3::NativeContext context{};if(fp3::readContext(context,e.tick))(void)fp3::publishContext(context.join,context.tick,context.stage,r.nativeActor);}(void)fp3::push(r);
        e.poseEpoch=g_pose_epoch6689.load();e.key[1]=g_fist_epoch101.load();
    }
    e.hand[0]=float(origin.x);e.hand[1]=float(origin.y);e.hand[2]=float(origin.z);cc::name(e,edge);cc::push(e);
}
static bool NativeInvoke104(void* actor,int input,bool on) noexcept {
    if(!g_native_request104||g_native_fault104.load())return false;
    InvalidateReadCache101R2();
    __try{g_native_request104(actor,input,on);InvalidateReadCache101R2();return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){InvalidateReadCache101R2();g_native_fault104=true;return false;}
}
// MSVC C2712: the SEH frame lives in its own function; the timing scope (a destructor) stays outside it.
static bool NativeZoneCall104(void* actor,int zone) noexcept {
    InvalidateReadCache101R2();
    __try{g_native_zone104(actor,zone,0,true);InvalidateReadCache101R2();return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){InvalidateReadCache101R2();g_native_fault104=true;return false;}
}
static bool NativeZone104(void* actor,int zone) noexcept {
    if(zone<0)return true;if(zone>5||!g_native_zone104||g_native_fault104.load())return false;
    PerfScope101R2 zoneTiming178r8(17);
    return NativeZoneCall104(actor,zone);
}
static bool NativeSelected104() noexcept {
    return (FistSelected101()||FullWeaponMode120(g_weapon_control_mode.load()))&&!diagnostics::lab::NativeReference()&&
        g_gameplay_allowed.load()&&g_head_valid.load();
}
static void NativeDamageObserve104(void* ctx) noexcept {
    const bool geometry=combat_diag_probe::GeometryHitsEnabled178r27(); // V66.178R29: rung 2 always logs Henry's damage results (essential line)
    if(!geometry&&!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return;
    NativeOwner104 owner{};if(!ctx||!g_native_owner104.read(owner)||!owner.player||!owner.soul||owner.epoch!=g_pose_epoch6689.load())return;
    ReadScope101R2 scope;void* attacker=nullptr;void* victim=nullptr;void* entity=nullptr;
    auto p=static_cast<unsigned char*>(ctx);
    if(!Read(p+0x10,attacker)||!Read(p+8,victim)||(attacker!=owner.soul&&victim!=owner.soul))return;
    if(geometry){float stamina=0,health=0;int type=-1,zone=-1;
        if(Read(p+0x138,stamina)&&Read(p+0x13C,health)&&Read(p+0xA4,type)&&Read(p+0xA8,zone)&&std::isfinite(stamina)&&std::isfinite(health)){
            char m[220]{};std::snprintf(m,sizeof(m),"V66.178R29 GEOM_DAMAGE role=%s health=%.2f stamina=%.2f type=%d zone=%d source=NATIVE_DAMAGE_RESULT(0x72704C)",attacker==owner.soul?"HENRY_ATTACKS":"HENRY_HIT",health,stamina,type,zone);diagnostic_trace::Write(m);}}
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64()))return;
    NativeResult104 e{};e.kind=4;e.tick=GetTickCount64();e.actor=reinterpret_cast<std::uintptr_t>(owner.actor);e.flags=attacker==owner.soul?1:2;
    if(!Read(p+0x18,entity)||!Read(p+0x20,e.key)||!Read(p+0xA4,e.type)||!Read(p+0xA8,e.zone)||
       !Read(p+0x138,e.stamina)||!Read(p+0x13C,e.health)||!std::isfinite(e.stamina)||!std::isfinite(e.health)||
       e.stamina<0||e.health<0||e.stamina>1.e7f||e.health>1.e7f)return;
    e.target=reinterpret_cast<std::uintptr_t>(entity);
    // No serial attribution is invented: this result can originate from ordinary
    // native combat too. Request/state and result events remain distinguishable.
    NativePush104(e);
}
static std::int64_t __fastcall NativeDamageDetour104(void* ctx) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::Damage};
    const auto value=g_native_damage_original104?g_native_damage_original104(ctx):0;
    NativeDamageObserve104(ctx); // recorder can observe native-only gameplay too
    return value;
}
static bool NativeContract104(std::uintptr_t b) noexcept {
    if(!SupportedWhBuild(b))return false;
    const unsigned char req[24]{0x48,0x89,0x5c,0x24,0x08,0x57,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0x41,0x8a,0xf8,0x48,0x8b,0x89,0x10,0x02,0x00,0x00,0x48};
    const unsigned char zone[24]{0x83,0xfa,0xff,0x74,0x60,0x48,0x89,0x5c,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xec,0x30,0x8b,0xda,0x41,0x8a};
    unsigned char actual[24]{};std::uintptr_t method=0;
    return Read(reinterpret_cast<void*>(b+0x93A28C),actual)&&!std::memcmp(actual,req,24)&&
        Read(reinterpret_cast<void*>(b+0x90F824),actual)&&!std::memcmp(actual,zone,24)&&
        Read(reinterpret_cast<void*>(b+0x3A80610+74*8),method)&&method==b+0x93A28C&&
        Read(reinterpret_cast<void*>(b+0x3A80610+73*8),method)&&method==b+0x2756D58;
}
static bool NativeArm104() noexcept {
    const auto b=WhBase();if(!b||g_native_fault104.load())return false;
    if(g_native_contract104.load(std::memory_order_acquire)==b)return true;
    std::lock_guard<std::mutex> lock(g_native_arm_mutex104);
    if(!NativeContract104(b))return false;
    g_native_request104=reinterpret_cast<NativeRequestFn104>(b+0x93A28C);
    g_native_zone104=reinterpret_cast<NativeZoneFn104>(b+0x90F824);
    if(!g_native_damage_original104){
        const unsigned char expected[16]{0x40,0x53,0x48,0x83,0xec,0x20,0x48,0x8b,0xd9,0xe8,0xde,0,0,0,0x48,0x8b};unsigned char actual[16]{};
        if(Read(reinterpret_cast<void*>(b+0x72704C),actual)&&!std::memcmp(actual,expected,16)){
            DetourModKit::hook::InlineRequest req{"kcd2vr_v66104_native_damage_result",DetourModKit::hook::Target{DetourModKit::Address{b+0x72704C}},DetourModKit::hook::Options{}};
            auto h=DetourModKit::hook::inline_at(std::move(req),&NativeDamageDetour104);
            if(h){g_native_damage_hook104=std::move(*h);g_native_damage_original104=g_native_damage_hook104->original<NativeDamageFn104>();
                if(!g_native_damage_original104||!g_native_damage_hook104->enable()){g_native_damage_hook104.reset();g_native_damage_original104=nullptr;}}
        }
    }
    g_native_contract104.store(b,std::memory_order_release);return true;
}
static bool FistSelectPunchH11(void* player,int side,std::uint64_t epoch,bool kickH35=false) noexcept {
    NativeOwner104 owner{},again{};FistOwner101 hands{};
    if(side<0||side>1||!NativeArm104()||!NativeResolve104(player,owner)||owner.epoch!=epoch||
       !FistReadOwner101(hands)||!hands.empty||hands.player!=player||hands.epoch!=epoch)return false;
    // Authored native hook rows: right hand0 uses upper-left1, left hand1
    // uses upper-right2. Lower5 selects kicks. This is a desired direction
    // through the original setter, not an attack-type/current-phase write.
    // H35: the unarmed R3 kick is the same request with the LOWER zone (5): the only unarmed zone-5 rows are the kick rows
    // (combat_action_attack r_noweapon eZ2/eZ5+aZ5+kick+attack_heavy); the 14 Oct native kick clip came from exactly this chain.
    const int wanted=kickH35?5:(side==0?1:2);
    int zone=-1;
    if(!NativeZone104(owner.actor,wanted)||!Read(static_cast<unsigned char*>(owner.state)+0xF8,zone)||zone!=wanted)return false;
    return NativeResolve104(player,again)&&again.actor==owner.actor&&again.state==owner.state&&again.epoch==epoch&&
        FistReadOwner101(hands)&&hands.empty&&hands.player==player&&hands.epoch==epoch;
}
// F4 sector-qualified block input. The current opponent and its state are read
// only for a deliberate motion candidate or a completed edge, not every frame.
struct NativeParryOpponent176 {
    void* actor=nullptr;void* state=nullptr;void* entity=nullptr;
    std::uint32_t id=0;int sector=-1;bool incoming=false;
    bool attacking=false;int phase=0;int zone=-1; // V66.178R45: any observed native attack phase of the proven opponent (no perceived-sector agreement needed)
    int observedPhase19c4r1=-1; // -1 means unreadable; never rearm an optional windup edge from a failed read.
    int perceived=-1; // R59.16C10R17: Henry's +0x5D8 (receipt only); sector = the attacker's +0x158 (Henry's frame)
};
// V66.178R49: the 19:05 log held the guard the whole fight (phase 256) and never produced one assisted edge
// (incomingEdges45=0 in 85 summaries) while the enemy was attacking. Nothing in that log says WHY the opponent read
// or the attacking test failed, so the stage and the opponent's raw phase/type are published to GUARD_SUMMARY.
static std::atomic<int> g_opp_stage49{0},g_opp_phase49{-1},g_opp_type49{-99},g_opp_zone49{-1};
static bool NativeParryOpponentRead176(const NativeOwner104& owner,NativeParryOpponent176& out) noexcept {
    out={};void* opponent=nullptr;void* state=nullptr;void* back=nullptr;void* link=nullptr;void* entity=nullptr;
    std::uintptr_t vft=0;unsigned char combat=0,otherCombat=0;const auto base=WhBase();
    const auto fail=[](int stage){g_opp_stage49.store(stage,std::memory_order_relaxed);g_opp_phase49=-1;g_opp_type49=-99;g_opp_zone49=-1;return false;};
    if(!owner.actor||!owner.state||!Read(static_cast<unsigned char*>(owner.state)+0xCF8,opponent)||!opponent||opponent==owner.actor)return fail(1);
    if(!Read(opponent,vft)||(vft!=base+0x3E3FAC8&&vft!=base+0x3A80610))return fail(2);
    if(!Read(static_cast<unsigned char*>(opponent)+0x210,state)||!state||
       !Read(static_cast<unsigned char*>(state)+0xCE0,back)||back!=opponent)return fail(3);
    if(!Read(static_cast<unsigned char*>(state)+0xCF8,back)||back!=owner.actor)return fail(4);
    if(!Read(static_cast<unsigned char*>(owner.state)+8,combat)||combat>1||
       !Read(static_cast<unsigned char*>(state)+8,otherCombat)||otherCombat>1||
       !Read(static_cast<unsigned char*>(opponent)+0x1F8,link)||!link)return fail(5);
    // V66.178R45: the +8 inCombat byte is a per-frame transient (HENRY_COMBAT_EDGE toggles it every ~10 ms; 2632
    // edges in the 15:07 log) so requiring ==1 on both actors dropped this read on roughly every other tick
    // (every F4_BLOCK_EDGE of that log says opponent=0). The mutual +CE0/+CF8 back-links above are the identity proof.
    if(vft==base+0x3A80610){
        if(!PhysVft6698(link,0x3AA4B98)||!Read(static_cast<unsigned char*>(link)+0x278,back)||back!=opponent||
           !Read(static_cast<unsigned char*>(link)+0x38,entity))return fail(6);
    }else if(PhysVft6698(link,0x4072C78))entity=link;
    else if(!Read(static_cast<unsigned char*>(link)+0x38,entity))return fail(6);
    std::uint32_t id=0;
    if(!PhysVft6698(entity,0x4072C78)||entity==owner.entity||
       !Read(static_cast<unsigned char*>(entity)+0xC,id)||!id)return fail(7);
    out.actor=opponent;out.state=state;out.entity=entity;out.id=id;
    NativeOwner104 enemy{};enemy.actor=opponent;enemy.state=state;nc104::Observed attack{};int perceived=-1;
    {nc104::Observed raw{};if(NativeObserve104(enemy,raw)){g_opp_stage49.store(8,std::memory_order_relaxed);g_opp_phase49.store(raw.phase,std::memory_order_relaxed);g_opp_type49.store(raw.type,std::memory_order_relaxed);g_opp_zone49.store(raw.zone,std::memory_order_relaxed);}
     else g_opp_stage49.store(9,std::memory_order_relaxed);}
    // These are property PAYLOADS (+8), matching the retained retail hit probe.
    // +2D8 alone is a request/candidate flag, not a continuous strike window.
    // Require an observed native attack phase/type and agreeing current perceived
    // and executed sectors. No predicted blade direction/contact is invented.
    const bool attackKnown19c4r1=NativeObserve104(enemy,attack);
    if(attackKnown19c4r1)out.observedPhase19c4r1=attack.phase;
    if(attackKnown19c4r1&&attack.type>=0&&attack.type<=9){
        out.phase=attack.phase;out.zone=attack.zone;
        out.attacking=attack.phase==4||attack.phase==8; // 16 is spent; next prepared phase is a new attack
        // R59.16C10R17: the incoming sector is the attacker's +0x158 (the zone it offers to Henry's defence triggers, 914204),
        // in Henry's frame. The R16 rule (Henry +0x5D8 == attacker +0x278) was never true: +0x278 reads upper-left for almost
        // every attack, so not one press-1 parry qualified in any log. +0x5D8 stays a receipt (it matches in every native block).
        int offered=-1;
        if((attack.phase==4||attack.phase==8)&&Read(static_cast<unsigned char*>(state)+0x158,offered)&&kcd2vr::guard_parry176::sector(offered)){
            out.sector=offered;out.incoming=true;
            if(Read(static_cast<unsigned char*>(owner.state)+0x5D8,perceived))out.perceived=perceived;
        }
    }
    return true;
}
#include "weapon_parry_contact_h24.inl"
#include "proximity_capture36.inl"
static void NativeParryTrace176(const NativeOwner104& owner,const NativeParryOpponent176* expected,
    diagnostics::parry176::Cause cause,std::uint64_t tick,bool manual,bool released,std::uint64_t publication) noexcept {
    if(!CombatExperiment175Enabled()||!FullWeaponMode120(g_weapon_control_mode.load())||diagnostics::lab::NativeReference())return;
    NativeOwner104 current{};
    if(!NativeResolve104(owner.player,current)||current.actor!=owner.actor||current.state!=owner.state||current.epoch!=owner.epoch){
        diagnostics::parry176::Clear();return;
    }
    diagnostics::parry176::Edge edge{};edge.tick=tick;edge.epoch=current.epoch;
    edge.player=reinterpret_cast<std::uintptr_t>(current.player);edge.actor=reinterpret_cast<std::uintptr_t>(current.actor);
    edge.state=reinterpret_cast<std::uintptr_t>(current.state);edge.entity=reinterpret_cast<std::uintptr_t>(current.entity);
    if(!Read(static_cast<unsigned char*>(current.entity)+0xC,edge.henryId)||!edge.henryId)return;
    NativeParryOpponent176 opponent{};
    if(NativeParryOpponentRead176(current,opponent)&&(!expected||
       (opponent.actor==expected->actor&&opponent.state==expected->state&&opponent.entity==expected->entity&&opponent.id==expected->id))){
        edge.opponentActor=reinterpret_cast<std::uintptr_t>(opponent.actor);
        edge.opponentState=reinterpret_cast<std::uintptr_t>(opponent.state);
        edge.opponentEntity=reinterpret_cast<std::uintptr_t>(opponent.entity);edge.opponentId=opponent.id;
        edge.incomingSector=expected?expected->sector:opponent.sector;
    }
    edge.cause=cause;edge.manualHeld=manual;edge.releaseReturned=released;edge.pressReturned=true;
    diagnostics::parry176::Edge published{};
    if(diagnostics::parry176::Publish(edge,publication,&published)&&
       diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,tick)){
        char line[448]{};std::snprintf(line,sizeof(line),
            "V66.176 F4_BLOCK_EDGE sequence=%llu tick=%llu epoch=%llu cause=%s Henry=%u opponent=%u sector=%d manualHeld=%d releaseReturned=%d pressReturned=1 nativeOutcome=UNCLAIMED timing=NATIVE",
            (unsigned long long)published.sequence,(unsigned long long)tick,(unsigned long long)published.epoch,
            diagnostics::parry176::Name(cause),published.henryId,published.opponentId,published.incomingSector,manual?1:0,released?1:0);
        diagnostic_trace::Write(line);
    }
}
static void NativeManualBlockEdge176(void* player,std::uint64_t tick,std::uint64_t publication) noexcept {
    if(!F4GestureInputOwned176()||!g_gameplay_allowed.load()||!FullWeaponMode120(g_weapon_control_mode.load())||
       diagnostics::lab::NativeReference()||!retail_build112::Ready(WhBase()))return;
    NativeOwner104 owner{};
    if(NativeResolve104(player,owner))NativeParryTrace176(owner,nullptr,diagnostics::parry176::Cause::Manual,tick,true,false,publication);
}
// C4R1 optional assistance: a known non-windup -> windup edge, never a
// native perfect-block result. Unknown samples retain the previous observation;
// a long gap or any identity change establishes a new baseline without an edge.
struct GuardWindupKey19c4r1 {
    void* player=nullptr;void* actor=nullptr;void* state=nullptr;void* entity=nullptr;
    std::uint64_t epoch=0,key=0,generation=0,request=0;int side=0;
    void* opponent=nullptr;void* opponentState=nullptr;void* opponentEntity=nullptr;std::uint32_t opponentId=0;
    bool same(const GuardWindupKey19c4r1& b) const noexcept {
        return player==b.player&&actor==b.actor&&state==b.state&&entity==b.entity&&epoch==b.epoch&&
            key==b.key&&generation==b.generation&&request==b.request&&side==b.side&&
            opponent==b.opponent&&opponentState==b.opponentState&&opponentEntity==b.opponentEntity&&opponentId==b.opponentId;
    }
    bool valid() const noexcept {return player&&actor&&state&&entity&&epoch&&key&&generation&&request&&
        (side==0||side==1)&&opponent&&opponentState&&opponentEntity&&opponentId;}
};
struct GuardWindupEdge19c4r1 {
    GuardWindupKey19c4r1 key{};std::uint64_t tick=0;int phase=-1;bool known=false;
    bool observe(const GuardWindupKey19c4r1& current,int currentPhase,std::uint64_t now,bool enabled,bool readable) noexcept {
        if(!enabled){*this={};return false;}
        if(!readable||!current.valid()||currentPhase<0||!now)return false;
        const bool edge=known&&key.same(current)&&now>=tick&&now-tick<=80&&phase!=4&&currentPhase==4;
        key=current;phase=currentPhase;tick=now;known=true;return edge;
    }
};
struct NativeGuard112 {
    guard112::State policy{};guard_motion168::State motion168{};NativeOwner104 owner{};
    kcd2vr::guard_parry176::State parry176{};
    kcd2vr::fist_guard175::State fists175{};
    std::uint64_t key=0,generation=0,lastHold=0;int side=0;bool held=false;
    // One edge per native PerfectBlock opening; phase changes cannot rearm it.
    kcd2vr::native_parry55c2::Window window55c2{};
    std::uint64_t incomingEdgeTick45=0,pendingWindow55c2=0;
    // R46: the re-press half of a release+press transaction that could not be issued on the release tick
    bool pendingPress46=false;std::uint64_t pendingSince46=0;
    bool pendingBlockMetadataH14=false;
    bool pendingLegacyMotionH14=false;
    // An expired parry lease must not turn its unacknowledged LT-up into a new
    // initial LT-down. This fence grants no input or timing permission.
    bool releaseWaitH14=false;
    std::uint64_t releaseRequestH14=0,releaseSampleH14=0,releaseLeaseH14=0;
    GuardWindupEdge19c4r1 windup19c4r1{};GuardWindupKey19c4r1 pendingWindupKey19c4r1{};bool pendingAutoWindup19c4r1=false;
    kcd2vr::guardH5::Beat<GuardWindupKey19c4r1> beatH5{};
    bool pendingMotionH5=false;std::uint64_t pendingBeatH5=0,lastMotionTickH5=0;
    std::uint64_t leaseH14=0;
    NativeParryOpponent176 pendingOpponent46c1{};std::uint64_t pendingPublication46c1=0;
};
static NativeGuard112 g_native_guard112;
#include "torch_offhand_d60.inl"
static std::atomic<std::uint64_t> g_guard_lease_h14{0};
static void NativeGuardMarkHeldH14() noexcept {
    g_native_guard112.held=true;g_native_guard112.releaseWaitH14=false;
    g_native_guard112.leaseH14=g_guard_lease_h14.fetch_add(1,std::memory_order_relaxed)+1;
    const auto& owner=g_native_guard112.owner;
    kcd2vr::hmd_dodge_h24::publishGuard(reinterpret_cast<std::uintptr_t>(owner.entity),reinterpret_cast<std::uintptr_t>(owner.actor),
        reinterpret_cast<std::uintptr_t>(owner.state),owner.epoch,GetTickCount64(),true);
}
static std::atomic<bool> g_guard_incoming_repress45{true};
static std::atomic<bool> g_guard_auto_windup19c4r1{false};
static std::atomic<unsigned> g_guard_incoming_cooldown45{120};
std::atomic<std::uint64_t> g_guard_incoming_edges45{0},g_guard_incoming_dropped45{0};
// Exact retail fingerprint plus immutable instruction pins, checked once per module.
// This reader runs inside the existing owner-validated guard pump, with no new hook.
static std::uintptr_t g_parry_checked55c2=0;static bool g_parry_contract55c2=false;
static std::atomic<int> g_parry_count55c2{-1};
static std::atomic<std::uint64_t> g_parry_motion57c1{0},g_parry_toward57c1{0},g_parry_open57c1{0};
static bool NativeParryWindow55c2(const NativeOwner104& owner,int& count) noexcept {
    count=-1;const auto base=WhBase();
    if(!retail_build112::Ready(base))return false;
    if(g_parry_checked55c2!=base){
        g_parry_contract55c2=kcd2vr::native_parry55c2::contract(base,[](std::uintptr_t at,unsigned char* out,std::size_t n){
            for(std::size_t i=0;i<n;++i)if(!Read(reinterpret_cast<void*>(at+i),out[i]))return false;return true;});
        g_parry_checked55c2=base;
    }
    return g_parry_contract55c2&&owner.state&&
        Read(static_cast<unsigned char*>(owner.state)+kcd2vr::native_parry55c2::countOffset,count)&&
        kcd2vr::native_parry55c2::validCount(count);
}
// C23 (Codex C4R2 assignment item 1): EDGE-ONLY observation of the live native master-strike counted window,
// Henry state+0x7D8, inside this same exact-owner guard pump. No hook, no call into the trigger, no write.
// OPEN/CLOSE/UNKNOWN/RESUMED/OWNER_CHANGED stay distinct; a failed read is UNKNOWN and never creates a serial.
// The published copy only lets the pose thread hand a guard to the SAME recognizer; native D4AD94 decides.
static bool NativeStrokeTowardOpponent117(const NativeOwner104& owner,const char*& why,float& distance,float& cosine,NativeParryOpponent176* observed59=nullptr) noexcept;
static bool NativeMeasureTowardOpponent50(const NativeOwner104& owner,const char*& why,float& distance,float& cosine,NativeParryOpponent176* observed59=nullptr) noexcept;
static std::uintptr_t g_master_checked23=0;static bool g_master_contract23=false;
static kcd2vr::native_master23::Window g_master_window23; // guard-pump thread only
static kcd2vr::core::SnapshotMailbox<kcd2vr::native_master23::Published> g_master_published23;
static std::atomic<std::uint64_t> g_master_handoff_spent23{0},g_master_handoff_logged23{0};
static std::atomic<bool> g_master_handoff_enabled23{true};static std::atomic<float> g_master_handoff_speed23{-1.f};
static bool NativeMasterCount23(const NativeOwner104& owner,int& count) noexcept {
    count=-1;const auto base=WhBase();
    if(!retail_build112::Ready(base))return false;
    if(g_master_checked23!=base){
        g_master_contract23=kcd2vr::native_master23::contract(base,[](std::uintptr_t at,unsigned char* out,std::size_t n){
            for(std::size_t i=0;i<n;++i)if(!Read(reinterpret_cast<void*>(at+i),out[i]))return false;return true;});
        g_master_checked23=base;
    }
    return g_master_contract23&&owner.state&&
        Read(static_cast<unsigned char*>(owner.state)+kcd2vr::native_master23::countOffset,count)&&
        kcd2vr::native_master23::validCount(count);
}
static void NativeMasterObserve23(const NativeOwner104& owner,const PhysFrame6698& f,std::uint64_t now) noexcept {
    namespace nm=kcd2vr::native_master23;
    NativeParryOpponent176 opp{};int count=-1;
    const bool oppKnown=NativeParryOpponentRead176(owner,opp);
    const bool known=oppKnown&&NativeMasterCount23(owner,count);
    nm::Identity id{};
    id.player=reinterpret_cast<std::uintptr_t>(owner.player);id.actor=reinterpret_cast<std::uintptr_t>(owner.actor);
    id.state=reinterpret_cast<std::uintptr_t>(owner.state);id.opponent=reinterpret_cast<std::uintptr_t>(opp.actor);
    id.opponentState=reinterpret_cast<std::uintptr_t>(opp.state);id.opponentId=opp.id;
    id.key=f.profiles[0].key;id.generation=f.profiles[0].generation;id.body=f.input.bodyEpoch;id.request=f.request;
    id.gesture=f.output.primaryStrokeGesture19;
    auto& w=g_master_window23;const bool wasOpen=w.open;
    const auto edge=w.observe(id,known,count,now);
    kcd2vr::diagnostics::flowH12::audit.window(known,count,edge==nm::Edge::Open); // copied existing weapon observer; no new native read
    nm::Published pub{};pub.tick=now;pub.count=known?count:-1;
    static nm::SectorWitnessH11 sectorH11;
    pub.incomingZoneH11=sectorH11.observe(w,id,opp.incoming?opp.sector:-1);
    if(w.available(id)){
        const char* why=nullptr;float distance=-1.f,cosine=-2.f;
        pub.toward=NativeMeasureTowardOpponent50(owner,why,distance,cosine)&&!why&&distance>=0.f;
        pub.available=true;pub.serial=w.serial;pub.key=w.id.key;pub.generation=w.id.generation;pub.body=w.id.body;
        pub.request=w.id.request;pub.opponentId=w.id.opponentId;
        pub.actorH11=w.id.actor;pub.stateH11=w.id.state;pub.opponentH11=w.id.opponent;pub.opponentStateH11=w.id.opponentState;
    }
    g_master_published23.publish(pub);
    { // D30-CMB: one strong pulse on the weapon hand when the native master-strike window opens (once per window serial)
        static std::uint64_t cued=0;
        if(kcd2vr::master_gate47::cueOnOpen(kcd2vr::master_gate47::cue.load(std::memory_order_relaxed),edge==nm::Edge::Open,w.serial,cued)){
            cued=w.serial;(void)kcd2vr::haptics101::publish_event(0,0xC0D3A570ull^w.serial,.85f,90,now);}
    }
    if(edge!=nm::Edge::None&&(edge!=nm::Edge::Unknown||wasOpen)){
        const auto& e=w.id;char m[520]{};std::snprintf(m,sizeof(m),
            "C23 MS7D8_EDGE edge=%s serial=%llu count=%d tick=%llu openTick=%llu player=%p actor=%p state=%p opponent=%p oppState=%p oppId=%u key=%016llX generation=%llu body=%llu request=%llu gesture=%llu masterStrike=NATIVE_D4AD94_DECIDES",
            nm::name(edge),(unsigned long long)w.serial,known?count:-1,(unsigned long long)now,(unsigned long long)w.openTick,
            reinterpret_cast<void*>(e.player),reinterpret_cast<void*>(e.actor),reinterpret_cast<void*>(e.state),
            reinterpret_cast<void*>(e.opponent),reinterpret_cast<void*>(e.opponentState),e.opponentId,
            (unsigned long long)e.key,(unsigned long long)e.generation,(unsigned long long)e.body,(unsigned long long)e.request,(unsigned long long)e.gesture);
        diagnostic_trace::Write(m);
    }
    {   // C54 restores C50: both DIRECT and HYBRID need the linked attack-start receipt.
        namespace dc=kcd2vr::direct_counter39;static dc::Attack attack39{};static int mode50=0;
        const int current50=kcd2vr::contact58c1::experiment.load(std::memory_order_relaxed)?1:CombatIntegrated178r22c2Enabled()?2:0;
        if(current50!=mode50){const auto serial50=attack39.serial;attack39={};attack39.serial=serial50;dc::publish(attack39);mode50=current50;}
        if(current50){
            const auto e39=attack39.observe(reinterpret_cast<std::uintptr_t>(opp.actor),reinterpret_cast<std::uintptr_t>(opp.state),opp.id,oppKnown,
                opp.observedPhase19c4r1,opp.incoming?opp.sector:-1,now);
            dc::publish(attack39);
            if(e39!=dc::Attack::Edge::None&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
                int stance=-1,pb=-1;(void)Read(static_cast<unsigned char*>(owner.state)+0xF8,stance);(void)NativeParryWindow55c2(owner,pb);
                const char* en=e39==dc::Attack::Edge::Open?"WINDOW_OPEN":e39==dc::Attack::Edge::Close?"WINDOW_CLOSE":"WINDOW_LOST";
                char m[420]{};std::snprintf(m,sizeof(m),
                    "C39 DIRECT_COUNTER edge=%s serial=%llu opponent=%u oppPhase=%d offered158=%d msStance=%d henryStance=%d stanceMatch=%d ms7D8=%d pb778=%d sinceStartMs=%llu nativeMaster=NOT_REQUESTED refusal=%s",
                    en,(unsigned long long)attack39.serial,attack39.id,opp.observedPhase19c4r1,attack39.offered,dc::msZone(attack39.offered),stance,
                    int(stance>=0&&stance==dc::msZone(attack39.offered)),known?count:-1,pb,
                    (unsigned long long)(attack39.openTick&&now>=attack39.openTick?now-attack39.openTick:0),dc::kRefusal);diagnostic_trace::Write(m);}
        }else if(attack39.open||attack39.opponent){const auto s39=attack39.serial;attack39={};attack39.serial=s39;dc::publish(attack39);}
    }
    const auto& h=f.output.masterHandoff23;
    if(h.serial&&h.tick&&g_master_handoff_logged23.exchange(h.tick,std::memory_order_relaxed)!=h.tick){
        char m[360]{};std::snprintf(m,sizeof(m),
            "C23 MS_GUARD_HANDOFF serial=%llu sample=%llu tick=%llu kind=%d heldGuard=%d forward=%.2f tipSpeed=%.2f threshold=%.2f liveSerial=%llu ms7D8=%d sameWindow=%d path=GUARD_EXIT->SAME_RECOGNIZER extraPrepare=0",
            (unsigned long long)h.serial,(unsigned long long)h.sample,(unsigned long long)h.tick,h.kind,int(h.heldGuard),h.forward,h.tipSpeed,h.threshold,
            (unsigned long long)(pub.available?pub.serial:0),pub.count,int(pub.available&&pub.serial==h.serial));
        diagnostic_trace::Write(m);
    }
}
static bool NativePhysicalGuardHeld117(void* player) noexcept {
    return g_guard_physical112.load()&&g_guard_physical_owner112.load()==player;
}
// Unlike GuardOwnedByUs59c7e this grants no250ms released-state grace. It is
// used only to retain/release our present input while native block metadata is
// in +278. An observable block context cannot acquire a new guard owner.
static bool NativeHeldGuardOwnerH14(const NativeOwner104& owner) noexcept {
    const auto& g=g_native_guard112;
    return owner.player&&owner.actor&&owner.state&&owner.epoch&&
        owner.epoch==g_pose_epoch6689.load()&&g.held&&NativePhysicalGuardHeld117(owner.player)&&
        g.owner.player==owner.player&&g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch&&
        !(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load());
}
// LT-up queues controller+88. Native Update clears context0 later; +308 is
// animation/block outcome state and is not an ACK of that original input.
static int NativeGuardReleaseAckH14(const NativeOwner104& owner) noexcept {
    const auto b=WhBase();std::uintptr_t vft=0,getter=0;unsigned char value=0xFF,bytes[9]{};
    static constexpr unsigned char expected[]{0x0F,0xB6,0xC2,0x0F,0xB6,0x44,0x08,0x18,0xC3};
    if(!b||!owner.state||!retail_build112::Ready(b)||
       !Read(static_cast<unsigned char*>(owner.state)+0x630,vft)||vft!=b+0x3B0A968||
       !Read(reinterpret_cast<void*>(vft+8),getter)||getter!=b+0x1A7DDE0||
       !Read(reinterpret_cast<void*>(getter),bytes)||std::memcmp(bytes,expected,sizeof(bytes))||
       !Read(static_cast<unsigned char*>(owner.state)+0x648,value)||value>1)return -1;
    return value==0?0:1;
}
static bool NativeGuardAwaitReleaseH14(const PhysFrame6698& frame,std::uint64_t originalLease) noexcept {
    auto& g=g_native_guard112;
    if(!originalLease||g.leaseH14!=originalLease||g_guard_lease_h14.load()!=originalLease)return false;
    g.releaseWaitH14=true;g.releaseRequestH14=frame.request;
    g.releaseSampleH14=frame.input.sample;g.releaseLeaseH14=originalLease;return true;
}
static bool NativeGuardReleaseFenceH14(const NativeOwner104& owner,const PhysFrame6698& frame) noexcept {
    auto& g=g_native_guard112;if(!g.releaseWaitH14)return false;
    NativeOwner104 current{};
    const bool exact=NativeResolve104(owner.player,current)&&current.actor==owner.actor&&current.state==owner.state&&current.epoch==owner.epoch&&
        g.owner.player==owner.player&&g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch&&
        frame.player==owner.player&&frame.input.bodyEpoch==owner.epoch&&frame.request==g.releaseRequestH14&&
        frame.request==g_weapon_request_epoch6694.load()&&frame.input.sample>=g.releaseSampleH14&&
        g.side>=0&&g.side<=1&&frame.output.active[g.side]&&frame.output.owned[g.side]&&
        frame.profiles[g.side].key==g.key&&frame.profiles[g.side].generation==g.generation&&
        g.releaseLeaseH14&&g.releaseLeaseH14==g_guard_lease_h14.load();
    if(!exact){g.releaseWaitH14=false;return false;}
    const int ack=NativeGuardReleaseAckH14(current);
    if(ack<0){g.pendingPress46=g.pendingBlockMetadataH14=g.pendingLegacyMotionH14=false;return true;}
    if(ack!=0)return true;
    g.releaseWaitH14=false;return false;
}
static bool NativePendingBlockMetadataH14(const NativeOwner104& owner,std::uint64_t now) noexcept {
    const auto& g=g_native_guard112;const auto& k=g.pendingWindupKey19c4r1;
    return g.pendingPress46&&g.pendingBlockMetadataH14&&!g.held&&g.pendingSince46&&
        now>=g.pendingSince46&&now-g.pendingSince46<=150&&
        g.owner.player==owner.player&&g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch&&
        k.player==owner.player&&k.actor==owner.actor&&k.state==owner.state&&k.epoch==owner.epoch&&
        k.key==g.key&&k.generation==g.generation&&k.side==g.side&&k.request==g_weapon_request_epoch6694.load();
}
static bool NativeBlockMetadataInputH14(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto& g=g_native_guard112;
    return NativeHeldGuardOwnerH14(owner)&&g.side>=0&&g.side<=1&&g.key&&g.generation&&
        g.key==f.profiles[g.side].key&&g.generation==f.profiles[g.side].generation&&
        f.output.active[g.side]&&f.output.owned[g.side]&&f.input.allowed&&f.input.tracked[1]&&
        f.input.sample&&c.sample&&c.sample<=f.input.sample&&c.tick&&now>=c.tick&&now-c.tick<=100&&
        f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&
        f.player==owner.player&&f.input.bodyEpoch==owner.epoch&&c.bodyEpoch==owner.epoch&&
        c.epoch==f.request&&f.request==f.input.epoch&&f.request==g_weapon_request_epoch6694.load();
}
static bool NativeBlockMetadataReleasedH14(const NativeOwner104& owner,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now) noexcept {
    NativeOwner104 current{};nc104::Observed state{};void* manager=nullptr;void* back=nullptr;std::uint64_t key=0;
    return g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
        !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!g_phys_fault6698.load()&&
        c.tick&&now>=c.tick&&now-c.tick<=100&&f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&
        c.epoch==g_weapon_request_epoch6694.load()&&owner.epoch==g_pose_epoch6689.load()&&
        NativeResolve104(owner.player,current)&&current.actor==owner.actor&&current.state==owner.state&&current.epoch==owner.epoch&&
        Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)&&manager==f.manager&&manager==g_henry_item_attach_manager.load()&&
        PhysVft6698(manager,0x3EC9958)&&Read(static_cast<unsigned char*>(manager)+0x38,back)&&back==owner.player&&
        NativeHandKey6697(manager,0,key)&&key==c.key&&NativeObserve104(current,state)&&
        (nc104::blockMetadataH14(state)||!native_admission147::preparationBlockedOwnedGuard56(state,true));
}
static std::atomic<std::uint64_t> g_guard_release_tick59c7e{0}; // R59.16C7E-DECAP1: last time our block input was released
static std::atomic<const void*> g_guard_release_state59c7e{nullptr}; // R59.16C7F: ... and on which state object
// R40C1: bounded readiness evidence without enabling the heavyweight F3 capture.
// Counters observe existing decisions; there are no additional native reads.
static std::array<std::atomic<std::uint64_t>,8> g_guard_counts40c1{};
static std::atomic<const char*> g_guard_reason40c1{"NONE"};
static std::atomic<int> g_guard_phase40c1{0},g_guard_type40c1{-1};
static void NativeGuardSummary40c1(std::uint64_t now) noexcept {
    static std::uint64_t last=0;
    if(!CombatExperiment175Enabled()||(last&&now>=last&&now-last<2000))return;
    last=now;std::uint64_t c[8]{};for(unsigned i=0;i<8;++i)c[i]=g_guard_counts40c1[i].exchange(0,std::memory_order_relaxed);
    char line[1100]{};std::snprintf(line,sizeof(line),
        "V66.178R40C1 GUARD_SUMMARY intervalMs=2000 press=%llu postureRelease=%llu stateRelease=%llu safetyRelease=%llu motionRepress=%llu manualJoin=%llu observedSamples=%llu postureSamples=%llu held=%d side=%d lastReason=%s phase=%d type=%d perfectBlock=NOT_FORCED incomingEdges45=%llu incomingDropped45=%llu opponentStage49=%d oppPhase=%d oppType=%d oppZone=%d pbSlotCount=%d assist=NATIVE_WINDOW motionRequired=%d nativeIncomingResults=%llu nativeBlocks=%llu nativePerfectBlocks=%llu motionCandidates57=%llu towardSector57=%llu windowOpenings57=%llu",
        c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7],g_native_guard112.held?1:0,g_native_guard112.side,
        g_guard_reason40c1.load(),g_guard_phase40c1.load(),g_guard_type40c1.load(),
        (unsigned long long)g_guard_incoming_edges45.exchange(0,std::memory_order_relaxed),(unsigned long long)g_guard_incoming_dropped45.exchange(0,std::memory_order_relaxed),
        g_opp_stage49.load(std::memory_order_relaxed),g_opp_phase49.load(std::memory_order_relaxed),g_opp_type49.load(std::memory_order_relaxed),g_opp_zone49.load(std::memory_order_relaxed),g_parry_count55c2.load(std::memory_order_relaxed),int(GuardMotionRequired59c3()),
        (unsigned long long)diagnostics::parry176::results55c2.exchange(0,std::memory_order_relaxed),
        (unsigned long long)diagnostics::parry176::blocks55c2.exchange(0,std::memory_order_relaxed),
        (unsigned long long)diagnostics::parry176::perfect55c2.exchange(0,std::memory_order_relaxed),
        (unsigned long long)g_parry_motion57c1.exchange(0,std::memory_order_relaxed),
        (unsigned long long)g_parry_toward57c1.exchange(0,std::memory_order_relaxed),
        (unsigned long long)g_parry_open57c1.exchange(0,std::memory_order_relaxed));
    diagnostic_trace::Write(line);
    {   static std::uint64_t feints=0,forward=0;const auto ft=g_guard_feints_total59c17.load(std::memory_order_relaxed),fw=g_guard_forward_total59c17.load(std::memory_order_relaxed);
        char r17[520]{};std::snprintf(r17,sizeof(r17),
            "R59.16C10R17 GUARD_R17 intervalMs=2000 strictEntry=%d feintFrames=%llu forwardHandFrames=%llu incomingReads=%llu perceivedMatch=%llu againstEdges=%llu lateralToward=%llu lateralMirror=%llu lastAgainstCm=%.1f lastZone158=%d staleTrackingSkips=%llu blockCooldownLease=%d motionRequired=%d",
            g_guard_strict_seen59c17.load(std::memory_order_relaxed)?1:0,
            (unsigned long long)(ft>=feints?ft-feints:ft),(unsigned long long)(fw>=forward?fw-forward:fw),
            (unsigned long long)g_guard_incoming_reads59c17.exchange(0,std::memory_order_relaxed),(unsigned long long)g_guard_perceived_match59c17.exchange(0,std::memory_order_relaxed),
            (unsigned long long)g_guard_against59c17.exchange(0,std::memory_order_relaxed),(unsigned long long)g_guard_lateral_toward59c17.exchange(0,std::memory_order_relaxed),
            (unsigned long long)g_guard_lateral_mirror59c17.exchange(0,std::memory_order_relaxed),double(g_guard_against_last59c17.load(std::memory_order_relaxed))*100.,
            g_guard_zone158_last59c17.load(std::memory_order_relaxed),(unsigned long long)g_guard_stale59c17.exchange(0,std::memory_order_relaxed),
            g_block_cooldown_lease59c17.load(std::memory_order_relaxed)?1:0,int(GuardMotionRequired59c3()));
        feints=ft;forward=fw;diagnostic_trace::Write(r17);}
}
// C39: edge-only receipt of WHY an owned held guard dropped (F3 only; the summary only kept the last reason per 2 s).
static std::atomic<const char*> g_guard_last_release39{"NONE"};static std::atomic<std::uint64_t> g_guard_last_release_tick39{0};
static void GuardReleaseNote39(const char* reason,int phase) noexcept {
    const auto now=std::uint64_t(GetTickCount64());g_guard_last_release39.store(reason?reason:"UNNAMED",std::memory_order_relaxed);g_guard_last_release_tick39.store(now,std::memory_order_relaxed);
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    char m[200]{};std::snprintf(m,sizeof(m),"C39 GUARD_RELEASE reason=%s henryPhase=%d direct=%d writes=BLOCK_INPUT_RELEASE_ONLY",reason?reason:"UNNAMED",phase,
        kcd2vr::contact58c1::experiment.load(std::memory_order_relaxed)?1:0);diagnostic_trace::Write(m);
}
static void NativeGuardRelease112(const char* reason) noexcept {
    // R59.16C8 (Codex k review item 2): this path is context loss (tracking/body/owner/parry-pending), not a guard-to-strike
    // handoff. REVOKE the 250 ms strike authorization instead of starting one; only the posture-exit release below grants it.
    auto& g=g_native_guard112;
    g_guard_release_tick59c7e.store(0,std::memory_order_relaxed);g_guard_release_state59c7e.store(nullptr,std::memory_order_relaxed);
    if(g.held&&CombatExperiment175Enabled()){++g_guard_counts40c1[3];g_guard_reason40c1=reason;}
    if(g.held)GuardReleaseNote39(reason,g_guard_phase40c1.load(std::memory_order_relaxed));
    if(g.owner.player){
        NativeOwner104 current{};
        const bool sameOwner=NativeResolve104(g.owner.player,current)&&current.actor==g.owner.actor&&
            current.state==g.owner.state;
        const bool same=sameOwner&&current.epoch==g.owner.epoch;
        const bool manual=g_guard_manual_owner112.load()==g.owner.player&&g_guard_manual112.load();
        if(!same)diagnostics::parry176::Clear();
        // C19C3 / C20 review: recenter changes our pose epoch, not the native
        // block input's player. Release only our held press on the same exact
        // native owner; the old epoch grants no parry/strike witness.
        if(g.held&&!manual&&sameOwner){NativeFreeKickGuardEdgeR24(current.player,2);(void)PhysCallAction6698(current.player,3,2);}
    }
    g={};g_guard_physical112.store(false,std::memory_order_release);g_guard_physical_owner112=nullptr;
    kcd2vr::shield_block_r120::indicatorShield.store(false,std::memory_order_relaxed); // R120
}
// NativeGuardTick precedes the actual command consumer. Keep the CURRENT input
// owner across only that same stack frame when its exact forward Prepare is
// already queued. No released-state witness or timeout grants input authority.
struct NativeBlockGuardFrameH14;
static thread_local NativeBlockGuardFrameH14* g_block_guard_frame_h14=nullptr;
struct NativeBlockGuardFrameH14 {
    NativeBlockGuardFrameH14* previous=g_block_guard_frame_h14;
    NativeOwner104 owner{};std::uint64_t key=0,generation=0,lease=0;bool deferred=false;
    NativeBlockGuardFrameH14() noexcept {g_block_guard_frame_h14=this;}
    ~NativeBlockGuardFrameH14(){
        g_block_guard_frame_h14=previous;
        const auto& g=g_native_guard112;
        if(deferred&&g.held&&g.leaseH14==lease&&g.key==key&&g.generation==generation&&
           g.owner.player==owner.player&&g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch)
            NativeGuardRelease112("BLOCK_METADATA_PREPARE_NOT_CONSUMED_SAME_TICK");
    }
};
static bool NativeDeferBlockPrepareH14(const NativeOwner104& owner,const PhysFrame6698& f,
    const nc104::Observed& native,std::uint64_t now) noexcept {
    auto* scope=g_block_guard_frame_h14;ph::Command c{};
    if(!scope||scope->deferred||!nc104::blockMetadataH14(native)||!NativeHeldGuardOwnerH14(owner)||
       !NativePrimaryForwardStroke19c2(owner,f,now)||!g_phys_commands6698.peek(c)||
       c.side!=0||c.kick59||c.held57||!c.gesture59c3||c.gesture59c3!=f.output.primaryStrokeGesture19||
       (c.stroke117!=ph::CommandKind::Slash&&c.stroke117!=ph::CommandKind::Thrust)||
       !(c.kind==ph::CommandKind::Prepare||
         ((c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)&&!g_phys_native6698.attackPrepared))||
       c.key!=f.profiles[0].key||c.generation!=f.profiles[0].generation||
       !NativeBlockMetadataInputH14(owner,c,f,now))return false;
    scope->owner=owner;scope->key=g_native_guard112.key;scope->generation=g_native_guard112.generation;
    scope->lease=g_native_guard112.leaseH14;scope->deferred=true;return true;
}
// A deliberate strike is an input-owner handoff, not tracking/menu/context loss.
// The native block animation can remain in 128/256 after its input is released.
// Preserve ONLY the existing exact-state 250 ms witness after a synchronous,
// owner-checked release; do not renew it on subsequent stroke frames. This is
// not permission to replace a foreign block, a reaction, a clinch or an attack.
static void NativeGuardStrikeHandoff19c2(const NativeOwner104& owner,const char* reason) noexcept {
    auto& g=g_native_guard112;
    if(!g.held)return;
    const bool owned=g_guard_physical112.load()&&g_guard_physical_owner112.load()==owner.player&&
        g.owner.player==owner.player&&g.owner.actor==owner.actor&&g.owner.state==owner.state&&g.owner.epoch==owner.epoch;
    const bool manual=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
    if(!owned||manual){NativeGuardRelease112(reason);return;}
    g_guard_release_tick59c7e=0;g_guard_release_state59c7e=nullptr;
    const bool synchronous=!kcd2vr::attack_xi178r8::enabled.load(std::memory_order_relaxed);
    const bool released=PhysCallAction6698(owner.player,3,2);
    NativeOwner104 current{};nc104::Observed checked{};bool observed=false;
    const bool same=released&&synchronous&&g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&!animation_filter::nativeBodyOwned112.load()&&!g_phys_fault6698.load()&&
        g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
        NativeResolve104(owner.player,current)&&current.actor==owner.actor&&current.state==owner.state&&current.epoch==owner.epoch&&
        (observed=NativeObserve104(current,checked))&&(checked.type==-1||checked.type==0||checked.type==1)&&
        (checked.phase==0||checked.phase==1||checked.phase==2||checked.phase==4||checked.phase==8||checked.phase==32||checked.phase==128||checked.phase==256);
    const auto now=std::uint64_t(GetTickCount64());
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        char m[400]{};std::snprintf(m,sizeof(m),
            "C19C2 GUARD_STRIKE_HANDOFF state=%p key=%llu generation=%llu body=%llu witness=%s reason=%s releaseReturned=%d synchronous=%d livePhase=%d liveKnown=%d witnessMs=250 acceptedAttack=NOT_INFERRED",
            owner.state,(unsigned long long)g.key,(unsigned long long)g.generation,(unsigned long long)owner.epoch,
            same?"GRANTED":"REFUSED",!released?"RELEASE_NOT_RETURNED":!synchronous?"ASYNC_DELIVERY":!observed?"OWNER_OR_CONTEXT_CHANGED":same?reason:"PROTECTED_NATIVE_STATE",
            int(released),int(synchronous),observed?checked.phase:-1,int(observed));diagnostic_trace::Write(m);
    }
    if(CombatExperiment175Enabled()){++g_guard_counts40c1[3];g_guard_reason40c1=reason;}
    GuardReleaseNote39(reason,observed?checked.phase:-1);
    g={};g_guard_physical112.store(false,std::memory_order_release);g_guard_physical_owner112=nullptr;
    if(same){g_guard_release_state59c7e.store(owner.state,std::memory_order_relaxed);
        g_guard_release_tick59c7e.store(now,std::memory_order_relaxed);}
}
// GuardTick and the command consumer can observe different completed XR frames.
// Before a newly arrived DIRECT forward Prepare is offered, release only our
// exact still-held guard. This closes that publication race; it does not grant
// a phase override or recreate/renew a witness after a prior context loss.
static void NativeDirectPrepareGuardHandoff19c4r2(const ph::Command& c,const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto& g=g_native_guard112;
    if(!g.held||!kcd2vr::motion59c4::selected.load()||!f.input.motionOwned59c5||
       !CombatExperiment175Enabled()||c.kind!=ph::CommandKind::Prepare||c.kick59||c.held57||c.side!=0||
       (c.stroke117!=ph::CommandKind::Slash&&c.stroke117!=ph::CommandKind::Thrust)||
       !c.gesture59c3||f.output.primaryStrokeGesture19!=c.gesture59c3||!f.output.strokeActive119||
       f.output.guardBySide30c1[0]||!ph::attack_authority55c1(f.input,f.output,0)||
       !f.profiles[0].melee()||c.key!=f.profiles[0].key||c.generation!=f.profiles[0].generation||
       c.bodyEpoch!=f.input.bodyEpoch||c.epoch!=f.request||c.epoch!=f.input.epoch||
       c.sample>f.input.sample||now<c.tick||now-c.tick>100||now<f.input.tick||now-f.input.tick>80||
       c.bodyEpoch!=g_pose_epoch6689.load()||c.epoch!=g_weapon_request_epoch6694.load()||
       !retail_build112::Ready(WhBase())||!g_phys_armed6698.load()||diagnostics::lab::NativeReference()||
       !g_gameplay_allowed.load()||g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||
       animation_filter::nativeBodyOwned112.load()||g_phys_fault6698.load()||
       !g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       !g_guard_physical112.load()||g_guard_physical_owner112.load()!=f.player||g.side<0||g.side>1||
       g.key!=f.profiles[g.side].key||g.generation!=f.profiles[g.side].generation||
       !f.output.active[g.side]||!f.output.owned[g.side]||
       (g_guard_manual_owner112.load()==f.player&&g_guard_manual112.load()))return;
    const auto ready=g_phys_ready_sample6698.load();PhysFrame6698 current{};
    if(!ready||ready!=f.input.sample||!g_phys_frame6698.read(current)||current.input.sample!=ready||
       current.input.tick!=f.input.tick||current.player!=f.player||current.manager!=f.manager||
       current.request!=f.request||current.input.bodyEpoch!=f.input.bodyEpoch||
       current.profiles[0].key!=c.key||current.profiles[0].generation!=c.generation||
       ready!=g_phys_ready_sample6698.load())return;
    NativeOwner104 owner{};void* manager=nullptr;void* back=nullptr;std::uint64_t heldKey=0;
    if(!NativeResolve104(f.player,owner)||g.owner.player!=owner.player||g.owner.actor!=owner.actor||
       g.owner.state!=owner.state||g.owner.epoch!=owner.epoch||
       !Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||
       manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
       !NativeHandKey6697(manager,g.side,heldKey)||heldKey!=g.key)return;
    NativeGuardStrikeHandoff19c2(owner,"DIRECT_FORWARD_PREPARE");
}
static bool FistBeginGuardWaitH19(void* player,std::uint64_t epoch) noexcept {
    NativeOwner104 owner{};
    if(!NativeResolve104(player,owner)||owner.epoch!=epoch||!NativeHeldGuardOwnerH14(owner))return false;
    g_fist_native101.guardActorH19=owner.actor;g_fist_native101.guardStateH19=owner.state;
    return true;
}
static int FistGuardReleaseAckU16(void* player,std::uint64_t epoch) noexcept {
    NativeOwner104 owner{};FistOwner101 hands{};
    if(!NativeResolve104(player,owner)||owner.epoch!=epoch||!FistReadOwner101(hands)||
       !hands.empty||hands.player!=player||hands.epoch!=epoch||
       owner.actor!=g_fist_native101.guardActorH19||owner.state!=g_fist_native101.guardStateH19||
       NativePhysicalGuardHeld117(player)||
       (g_guard_manual_owner112.load()==player&&g_guard_manual112.load()))return -1;
    nc104::Observed native{};
    if(!NativeObserve104(owner,native))return -1;
    // The original input selector may replace OUR released block while its
    // phase128/256 and metadata7 are still present. 90F824 changes the desired
    // direction (+F8/+188), NOT that block metadata (+278). Requiring a neutral
    // guardWritable state here can wait for the very replacement we must offer.
    // Preserve the original100ms gesture; cancel a new request/protected action.
    if((native.type!=-1&&native.type!=1)||native.requested||native.requestCandidate||
       (native.phase!=1&&native.phase!=2&&native.phase!=128&&native.phase!=256))return -1;
    return NativeGuardReleaseAckH14(owner);
}
static bool NativeKickGuardFenceR19(void*,std::uint64_t) noexcept;
static bool NativeFreeKickGuardRetainedR24() noexcept;
#include "weapon_parry_delivery_h24.inl"
static void NativeFistGuardTick175() noexcept {
    std::unique_lock<std::recursive_mutex> fistLock(g_fist_native_mutex101,std::try_to_lock);if(!fistLock.owns_lock())return;
    const auto now=std::uint64_t(GetTickCount64());auto& g=g_native_guard112;
    FistFrame101 f{};
    const bool selected=FistSelected101()&&FistModePermits178r22c2()&&!diagnostics::lab::NativeReference()&&
        !g_fist_fault101.load()&&!animation_filter::nativeBodyOwned112.load()&&
        g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load();
    if(!selected||!g_fist_frame101.read(f)||f.request!=g_fist_epoch101.load()||
       f.sample.epoch!=g_pose_epoch6689.load()||!f.sample.valid||now<f.sample.tick||now-f.sample.tick>80){NativeGuardRelease112();return;}
    // Current claim observation only: never renew the original fist sample.
    for(int sideT24=0;sideT24<2;++sideT24)f.sample.claimedT24[sideT24]=f.sample.claimedT24[sideT24]||HandClaimBlockedT24(sideT24,f.sample.actor,f.sample.epoch,g_weapon_request_epoch6694.load());
    NativeOwner104 dodgeOwnerH24{};
    if(NativeResolve104(f.player,dodgeOwnerH24)&&NativeHmdGuardYieldH24(dodgeOwnerH24,now))return;
    if(NativeKickGuardFenceR19(f.player,now)){NativeGuardRelease112("R19_KICK_WAIT");return;}
    // Moving is only a candidate at 0.4 m/s, before punch travel/intent qualifies.
    // Repositioning a raised guard must not drop block for up to 500ms.
    const bool attacking=g_fist_native101.prepared||g_fist_native101.waitingGuardU16||f.punch.valid;
    const auto decision=kcd2vr::fist_guard175::update(g.fists175,f.sample,attacking);
    if(fp3::active(now)){static fp3::Rate rate;const auto squeezeR114=[&](int i){return f.sample.grip[i]>double(.60f)&&f.sample.trigger[i]>double(.60f);};const unsigned gate=(!squeezeR114(0)?1u:0u)|(!squeezeR114(1)?2u:0u)|(attacking?4u:0u)|(decision.block?8u:0u);if(rate.permit(now,gate)){
        char line[650]{};std::snprintf(line,sizeof(line),"H29 FIST_BLOCK sample=%llu tick=%llu epoch=%llu request=%llu analogGate=%u gripR=%.3f triggerR=%.3f gripL=%.3f triggerL=%.3f threshold=GT_0_60 sensitivity=%.3f raisedSince=%llu dwellMs=%.2f gestureBlocking=%d nativeGuardHeld=%d zone=%d why=%s",(unsigned long long)f.sample.sequence,(unsigned long long)now,(unsigned long long)f.sample.epoch,(unsigned long long)f.request,gate,f.sample.grip[0],f.sample.trigger[0],f.sample.grip[1],f.sample.trigger[1],double(melee_tuning_c54::blockH29()),(unsigned long long)g.fists175.raisedSince,80./melee_tuning_c54::blockH29(),decision.block,g.held,decision.zone,(f.sample.claimedT24[0]||f.sample.claimedT24[1])&&!f.sample.responsiveCombatH27?"COMPETING_HAND_CLAIM_T24":(gate&3)?"TRIGGER_OR_GRIP_NOT_ABOVE_60":attacking?"QUALIFIED_PUNCH_OWNS_INPUT":decision.block?"POSTURE_QUALIFIED_SEE_NATIVE_GUARD_RESULT":"POSTURE_OR_DWELL_NOT_QUALIFIED");diagnostic_trace::Write(line);
    }}

    if(!decision.block){if(g.held)NativeGuardRelease112();return;}
    NativeOwner104 owner{};nc104::Observed native{};FistOwner101 hands{};
    if(!retail_build112::Ready(WhBase())||!NativeArm104()||!FistReadOwner101(hands)||!hands.empty||hands.player!=f.player||
       hands.epoch!=f.sample.epoch||!NativeResolve104(f.player,owner)||!NativeObserve104(owner,native)){NativeGuardRelease112();return;}
    if(g.owner.player&&(g.owner.actor!=owner.actor||g.owner.state!=owner.state||g.owner.epoch!=owner.epoch||g.key!=0||g.generation!=f.request)){
        NativeGuardRelease112();return; // fresh posture dwell after an ownership change
    }
    g.owner=owner;g.key=0;g.generation=f.request;
    const bool heldFistMetadataH14=nc104::blockMetadataH14(native)&&NativeHeldGuardOwnerH14(owner)&&
        g.key==0&&g.generation==f.request&&hands.empty;
    if(!heldFistMetadataH14&&!native_admission147::guardWritable(native)&&!native_admission147::freeGuardHeld44c2(native,g.held)){NativeGuardRelease112();return;}
    int zone=-1;
    if(native_admission147::guardWritable(native)&&
       (!Read(static_cast<unsigned char*>(owner.state)+0xF8,zone)||zone< -1||zone>5||
        (zone!=decision.zone&&!NativeZone104(owner.actor,decision.zone)))){NativeGuardRelease112();return;}
    NativeOwner104 after{};
    if(!NativeResolve104(owner.player,after)||after.actor!=owner.actor||after.state!=owner.state||after.epoch!=owner.epoch||
       !NativeObserve104(after,native)||(!(nc104::blockMetadataH14(native)&&heldFistMetadataH14&&NativeHeldGuardOwnerH14(after))&&
       !native_admission147::guardWritable(native)&&!native_admission147::freeGuardHeld44c2(native,g.held))){NativeGuardRelease112();return;}
    const bool manual=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
    if(!g.held){
        if(native.blocking&&!manual)return; // foreign native block has no physical release owner
        if(manual||PhysCallAction6698(owner.player,3,1)){
            NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))
                diagnostic_trace::Write(manual?"V66.175 FIST_GUARD edge=JOIN_MANUAL nativeOutcome=UNCLAIMED":"V66.175 FIST_GUARD edge=PRESS source=RAISED_EMPTY_HANDS nativeOutcome=UNCLAIMED");
        }
    }else if(!manual&&now>=g.lastHold&&now-g.lastHold>=50){
        (void)PhysCallAction6698(owner.player,3,4);g.lastHold=now;
    }
}
static void NativeGuardTick112() noexcept {
    std::unique_lock<std::recursive_mutex> ownerLock(g_phys_native_mutex6698,std::try_to_lock);if(!ownerLock.owns_lock())return;
    if(NativeFreeKickGuardRetainedR24())return; // exact pending R3 modifier, no physical posture claim
    NativeGuardSummary40c1(GetTickCount64());
    if(FistSelected101()&&FistModePermits178r22c2()){NativeFistGuardTick175();return;}
    // F4 gestures replace native Trigger block. Drain a pre-existing manual
    // lease once, including a cold/missing weapon frame; never borrow that lease
    // as positional permission or leave its native latch held after suppression.
    if(g_guard_manual112.load()&&g_gameplay_allowed.load()&&F4GestureInputOwned176()&&
       retail_build112::Ready(WhBase())){
        void* manualOwner=g_guard_manual_owner112.load();NativeOwner104 manualNative{};
        if(manualOwner&&NativeResolve104(manualOwner,manualNative)){
            // An exact owner's native latch is still pending if input validation
            // prevents the release call. Do not claim cleanup or start a new
            // positional lease; a native physical fault disables further calls.
            if(g_phys_fault6698.load()||!PhysCallAction6698(manualOwner,3,2))return;
        }
        g_guard_manual112=false;g_guard_manual_owner112=nullptr;
        g_native_guard112={};g_guard_physical112=false;g_guard_physical_owner112=nullptr;
    }
    const auto now=std::uint64_t(GetTickCount64());const auto poseTick=animation_filter::combatPoseTick112.load();
    auto& g=g_native_guard112;PhysFrame6698 f{};NativeOwner104 owner{};nc104::Observed native{};
    const auto f3=diagnostics::lab::Get(diagnostics::lab::Key::F3);
    const bool selected=retail_build112::Ready(WhBase())&&g_phys_armed6698.load()&&FullWeaponMode120(g_weapon_control_mode.load())&&
        !diagnostics::lab::NativeReference()&&!FistSelected101()&&!animation_filter::nativeBodyOwned112.load()&&
        g_gameplay_allowed.load()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
        g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
        !g_phys_fault6698.load()&&poseTick&&now>=poseTick&&now-poseTick<=100;
    if(!selected||!g_phys_frame6698.read(f)||((f3!=2&&f3!=3)&&!f.input.coherentMelee119)||!f.input.allowed||now<f.input.tick||now-f.input.tick>80||
       f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       !NativeArm104()||
       !NativeResolve104(f.player,owner)||!NativeObserve104(owner,native)) {
        g_master_published23.publish({}); // no exact owner/frame: nothing to hand off (the window record itself is untouched)
        NativeGuardRelease112(!selected?"CONTEXT_TRACKING_POSE_OR_NATIVE_BODY":"FRAME_OWNER_OR_NATIVE_READ");return;}
    ObserveTorchOffhandD60(f,owner,native,now);
    if(NativeHmdGuardYieldH24(owner,now))return;
    if(NativeKickGuardFenceR19(owner.player,now)){NativeGuardRelease112("R19_KICK_WAIT");return;}
    // Actual interception owns defense before an outgoing gesture may claim the
    // same frame. Neither ordinary frontal posture nor stroke state is proof.
    NativeParryOpponent176 contactIncomingH24{};
    (void)NativeParryOpponentRead176(owner,contactIncomingH24);
    if(CombatExperiment175Enabled()&&NativeWeaponContactParryH24(owner,f,contactIncomingH24,native,now))return;
    NativeMasterObserve23(owner,f,now); // C23: edge-only +7D8 observation for every exact-owner pump tick
    if(NativeDeferBlockPrepareH14(owner,f,native,now))return;
    // C54: the exact completed sample already chose a forward counter handoff.
    // Do not press ordinary block before its queued Prepare is consumed. This
    // reserves only that sample/gesture, never an arbitrary open window; native
    // input still rechecks the count, perk, stance and opponent at consumption.
    const auto& handoff54=f.output.masterHandoff23;
    if(handoff54.serial&&handoff54.tick==f.input.tick&&handoff54.sample==f.input.sample&&
       f.input.masterWindowSerial23==handoff54.serial&&f.input.masterWindowKey23==f.profiles[0].key&&
       f.input.masterWindowGeneration23==f.profiles[0].generation&&
       f.input.masterWindowBody23==f.input.bodyEpoch&&f.input.masterWindowRequest23==f.request&&
       f.output.primaryStrokeGesture19&&f.output.strokeActive119){
        int count54=-1;
        if(NativeMasterCount23(owner,count54)&&count54>0){
            NativeGuardStrikeHandoff19c2(owner,"C54_SAME_SAMPLE_MASTER_HANDOFF");return;
        }
    }
        const kcd2vr::kick5916c3::Owner kickOwner{reinterpret_cast<std::uintptr_t>(f.player),f.profiles[0].key,
        f.profiles[0].generation,f.input.bodyEpoch,f.request};
    if(kcd2vr::kick5916c3::delivery.active(kickOwner,now)){
        NativeGuardRelease112("OWNED_KICK_PREPARE");return;
    }
    const bool parryMode176=CombatExperiment175Enabled();
    if(parryMode176&&!f.input.positionGuard176){NativeGuardRelease112("MODE_FRAME_MISMATCH");return;}
    if(f.input.motionOwned59c5){
        // Posture is not a second phase owner. The stroke recognizer can finish
        // before its checked contact arrives; shield posture must not steal that
        // same actor's phase either. Cancel/contact-done/yield/expiry clears the
        // actual driver lease, so this is not an extra fixed guard cooldown.
        const kcd2vr::motion59c4::Key motionContext{0,f.profiles[0].key,f.profiles[0].generation,
            f.input.bodyEpoch,f.input.epoch,kcd2vr::contact176::recoveryEpoch55c1.load(),
            reinterpret_cast<std::uintptr_t>(owner.player)};
          const auto motionOwnership=combat_diag_probe::VrStateAttackOwnership59c10(motionContext,now);
          if(motionOwnership!=kcd2vr::motion59c4::AttackOwnership::None){
              // A concurrent snapshot update is not proof that the attack ended.
              // Defer this guard tick rather than stealing a still-owned phase.
              if(motionOwnership==kcd2vr::motion59c4::AttackOwnership::Owned)
                  NativeGuardStrikeHandoff19c2(owner,"MOTION_PREPARE_OR_STRIKE_OWNS_PHASE");
              else NativeGuardRelease112("MOTION_OWNERSHIP_PUBLICATION_UNSETTLED");return;
        }
    }
    if(NativeForwardPrepareOwned19c2(owner,f,native,now)){
        NativeGuardStrikeHandoff19c2(owner,"F4_FORWARD_PREPARATION");return;
    }
    // R94: COMBO HELP: HOLD keeps Henry's attack tail alive in an open combo slot. A guard press would end that
    // attack (and the combo). The automatic posture guard waits; a manual LT guard is not touched here.
    if(kcd2vr::combo_hold_r14::holdingNow(reinterpret_cast<std::uintptr_t>(owner.actor),now)&&
       !(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load())){
        unsigned char slotR94=0;
        if(Read(static_cast<unsigned char*>(owner.state)+0x868,slotR94)&&slotR94==1){
            kcd2vr::combo_hold_r14::autoGuardDeferred.fetch_add(1,std::memory_order_relaxed);
            NativeGuardRelease112("R94_COMBO_HOLD_OPEN_SLOT");return;}
    }
    if(CombatSynchronized59c10r9()){
        // A raised shield is not permission to cancel our primary preparation.
        // Once admitted, exact native main-action ownership outlives the gesture
        // recognizer and ends with native recovery/replacement, not a cooldown.
        const int ownership=NativeSynchronizedAttackOwnedC10R18(owner,f,now);
        if(ownership!=0){NativeGuardRelease112(ownership>0?
            "SYNCHRONIZED_ADMITTED_MAIN_ACTION":"SYNCHRONIZED_ACTION_EVIDENCE_UNSETTLED");return;}
    }
    // Use the exact owned shield profile's grip position, never infer a shield
    // face normal from the weapon bounds axis. A raised shield takes precedence.
    const bool shield176=parryMode176&&f.input.positionGuard176&&f.output.active[1]&&f.output.owned[1]&&
        f.profiles[1].side==1&&f.profiles[1].shield176()&&
        ph::natural_guard117(f.input,f.profiles[1],f.output.weapons[1],g.held&&g.side==1);
    const int side=shield176?1:0;
    if(!f.output.active[side]||!f.output.owned[side]||
       !(side==1?f.profiles[side].shield176():f.profiles[side].melee())){NativeGuardRelease112("WEAPON_NOT_OWNED");return;}
    std::uint64_t key=0;void* manager=nullptr;void* back=nullptr;
    if(!Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||manager!=g_henry_item_attach_manager.load()||
       !PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
       !NativeHandKey6697(manager,side,key)||key!=f.profiles[side].key){NativeGuardRelease112("EXACT_ITEM_OWNER");return;}
    if(g.owner.player&&(g.owner.actor!=owner.actor||g.owner.state!=owner.state||g.owner.epoch!=owner.epoch||
        g.key!=key||g.side!=side||g.generation!=f.profiles[side].generation))NativeGuardRelease112("OWNER_ITEM_OR_SIDE_CHANGED");
    g.owner=owner;g.key=key;g.side=side;g.generation=f.profiles[side].generation;
    const bool manual=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
    g_guard_feints_total59c17.store(f.output.guardFeints59c17,std::memory_order_relaxed);g_guard_forward_total59c17.store(f.output.guardForwardVetoes59c17,std::memory_order_relaxed);
    g_guard_strict_seen59c17.store(f.input.guardStrict59c17,std::memory_order_relaxed);
    auto decision=guard112::update(g.policy,f.input,f.profiles[side],f.output.weapons[side],true);
    if(f.input.positionGuard176){
        // R30C1: do not independently press block or cancel an attack before the
        // completed physical sample has passed its speed/dwell arbitration.
        decision.block=f.output.guardBySide30c1[side];g.policy.block=decision.block;
    }
    kcd2vr::shield_block_r120::indicatorShield.store(side==1&&decision.block,std::memory_order_relaxed); // R120: BLOCK indicator text
    // F4 position owns an already-held defense during counter-motion. A stroke
    // that began outside guard keeps its native attack arbitration; LT cannot
    // override posture or create a separate block owner in this mode.
    // C19C2 user contract: a completed primary strike wins over a secondary
    // shield's posture, even before its queued Prepare reaches the consumer.
    // Pure parry motion retains guard because its PRIMARY guard decision is true.
    if(NativePrimaryForwardStroke19c2(owner,f,now)){
        NativeGuardStrikeHandoff19c2(owner,"F4_PRIMARY_FORWARD_STROKE");return;
    }
    if(f.input.coherentMelee119&&f.output.strokeActive119&&!(parryMode176&&decision.block&&(g.held||side==1))){
        if(f.input.motionOwned59c5)NativeGuardStrikeHandoff19c2(owner,"ACTIVE_PHYSICAL_STROKE");
        else NativeGuardRelease112("ACTIVE_PHYSICAL_STROKE");return;
    }
    auto& prepared=g_phys_native6698;
    if(decision.block&&native.type==-1&&prepared.attackPrepared&&prepared.player==owner.player&&
       prepared.epoch==owner.epoch&&prepared.request==f.request&&(prepared.attackKey==key||(parryMode176&&side==1))){
        PhysCancelAttack117();
        if(!NativeObserve104(owner,native)){NativeGuardRelease112();return;}
    }
    // Never force a direction through an attack or native reaction. The current
    // guard state is +F8, not +278 (the latter is the attack's zone snapshot).
    int currentZone=-1,masterCountH11=0;
    const bool keepMasterStanceH11=decision.block&&g_ms_keep_stance59c17.load(std::memory_order_relaxed)&&
        NativeMasterCount23(owner,masterCountH11)&&masterCountH11>0;
    if(!keepMasterStanceH11&&native_admission147::guardWritable(native)&&decision.zone>=0&&
       Read(static_cast<unsigned char*>(owner.state)+0xF8,currentZone)&&currentZone>=-1&&currentZone<=5&&
       currentZone!=decision.zone&&!NativeZone104(owner.actor,decision.zone)){NativeGuardRelease112();return;}
    // The setter can notify native listeners. Revalidate after crossing that boundary.
    NativeOwner104 after{};
    if(!NativeResolve104(owner.player,after)||after.actor!=owner.actor||after.state!=owner.state||after.epoch!=owner.epoch||
       !NativeObserve104(after,native)){NativeGuardRelease112();return;}
    const bool heldBlockMetadataH14=nc104::blockMetadataH14(native)&&NativeHeldGuardOwnerH14(owner);
    const bool pendingBlockMetadataH14=parryMode176&&nc104::blockMetadataH14(native)&&NativePendingBlockMetadataH14(owner,now);
    const bool want=decision.block&&(heldBlockMetadataH14||pendingBlockMetadataH14||
        (parryMode176?native_admission147::guardHeldInput44c2(native,g.held||g.pendingPress46):native_admission147::guardWritable(native)));
    if(parryMode176){++g_guard_counts40c1[6];if(decision.block)++g_guard_counts40c1[7];
        g_guard_phase40c1=native.phase;g_guard_type40c1=native.type;}
    TrackingSnapshot raw168{};
    // Holding an input during an ordinary action is permitted, but a deliberate
    // release/re-press still requires the original neutral/guard state. Otherwise
    // its post-release revalidation would reject and unnecessarily drop defense.
    // C54: collect defensive motion while the completed posture is present,
    // BEFORE the initial native block press. All71 captured PB openings had
    // guardHeld0, so recording only after press erased the interception history.
    // This records no input/result; re-press below still requires our held guard.
    const bool needTracking176=parryMode176?decision.block&&!manual&&
        (!g.held||(GuardMotionRequired59c3()?(heldBlockMetadataH14||native_admission147::guardHeldInput44c2(native,true)):native_admission147::guardWritable(native))):
        f3==3&&want&&g.held&&!manual;
    const bool tracking168=needTracking176&&ReadTrackingSnapshot(raw168)&&raw168.allowed&&raw168.head_valid&&raw168.right_valid&&
        !raw168.head_held&&!raw168.right_held;
    const auto motionHand176=side==1?raw168.left:raw168.right;
    const bool fresh168=!parryMode176&&f3==3&&want&&g.held&&!manual&&tracking168;
    const bool motionEdge168=g.motion168.update({raw168.tick,raw168.sequence,owner.epoch,key,
        {raw168.right.x,raw168.right.y,raw168.right.z},{raw168.head.x,raw168.head.y,raw168.head.z},fresh168});
    const bool fresh176=parryMode176&&decision.block&&!manual&&tracking168&&
        raw168.left_valid&&!raw168.left_held&&now>=raw168.tick&&now-raw168.tick<=60;
    const auto motion176=g.parry176.update({raw168.tick,raw168.sequence,owner.epoch,key,
        {motionHand176.x,motionHand176.y,motionHand176.z},{raw168.head.x,raw168.head.y,raw168.head.z},fresh176});
    NativeParryOpponent176 incoming176{};
    const bool opponentRead45=parryMode176&&NativeParryOpponentRead176(owner,incoming176);

    const GuardWindupKey19c4r1 windupKey19c4r1{owner.player,owner.actor,owner.state,owner.entity,owner.epoch,
        key,f.profiles[side].generation,f.request,side,incoming176.actor,incoming176.state,incoming176.entity,incoming176.id};
    g.beatH5.observe(windupKey19c4r1,incoming176.observedPhase19c4r1,now,opponentRead45&&fresh176);
    const bool windupEdge19c4r1=g.windup19c4r1.observe(windupKey19c4r1,incoming176.observedPhase19c4r1,now,
        parryMode176&&g_guard_auto_windup19c4r1.load(std::memory_order_relaxed),opponentRead45);
    // R55C2: native trigger window, not phase 4/8 (which can precede the opening).
    // A guard raised during wind-up keeps the later opening available. Initial
    // press during an already-open window consumes it below, without a double tap.
    int pbCount55c2=-1;
    const bool windowKnown55c2=parryMode176&&NativeParryWindow55c2(owner,pbCount55c2);
    g_parry_count55c2.store(windowKnown55c2?pbCount55c2:-1,std::memory_order_relaxed);
    const auto windowSerial57c1=g.window55c2.serial;
    const bool windowReady55c2=g.window55c2.observe(reinterpret_cast<std::uintptr_t>(incoming176.actor),incoming176.id,
        windowKnown55c2&&opponentRead45,pbCount55c2);
    // R59.4: the native-window press may qualify from the trailing-window displacement when the
    // settle-gated edge detector has nothing (a guard hand mid-combo is never still for 120 ms).
    const auto window594=motion176.valid?motion176:(fresh176?g.parry176.recent():kcd2vr::guard_parry176::Movement{});
    // R59.16C10R17: against the attacker's offered zone (see againstAttack). F4 press 1 (VR INTENT, "easier") needs 3 cm, the
    // other F4 modes 4 cm; 50 % of the HMD-relative travel either way, inside the trailing <= 160 ms window.
    kcd2vr::guard_parry176::Against against59c17{};
    if(fresh176&&g_cal_state.load(std::memory_order_acquire)==2&&opponentRead45&&incoming176.incoming){
        g_guard_incoming_reads59c17.fetch_add(1,std::memory_order_relaxed);g_guard_reads_total59c17.fetch_add(1,std::memory_order_relaxed);
        if(incoming176.perceived==incoming176.sector)g_guard_perceived_match59c17.fetch_add(1,std::memory_order_relaxed);
        against59c17=kcd2vr::guard_parry176::againstRecent(g.parry176,incoming176.sector,
            CalFrameR133(),ph::body_axes(f.input).right,
            CombatIntent59c16()?.03:.04,.50,std::max<std::uint64_t>(g.lastMotionTickH5,g.beatH5.active?g.beatH5.onset:0));
        if(!GuardMotionRequired59c3()&&!against59c17.ok&&motion176.valid)against59c17=kcd2vr::guard_parry176::againstAttack(motion176,incoming176.sector,
            CalFrameR133(),ph::body_axes(f.input).right,
            CombatIntent59c16()?.03:.04,.50);
    }
    const bool toward57c1=against59c17.ok;
    if(toward57c1){g_guard_against59c17.fetch_add(1,std::memory_order_relaxed);g_guard_against_total59c17.fetch_add(1,std::memory_order_relaxed);
        if(against59c17.lateral)(against59c17.lateral>0?g_guard_toward_total59c17:g_guard_mirror_total59c17).fetch_add(1,std::memory_order_relaxed);
        (against59c17.lateral>=0?g_guard_lateral_toward59c17:g_guard_lateral_mirror59c17).fetch_add(against59c17.lateral?1:0,std::memory_order_relaxed);
        g_guard_against_last59c17.store(float(against59c17.along),std::memory_order_relaxed);g_guard_zone158_last59c17.store(incoming176.sector,std::memory_order_relaxed);}
    if(windowSerial57c1!=g.window55c2.serial){++g_parry_open57c1; // C26 (Miki, F3): say when the native perfect-block window opens
        if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Combat,now)){char m26[200]{};
            std::snprintf(m26,sizeof(m26),"C26 WINDOW kind=PERFECT_BLOCK edge=OPEN serial=%llu count=%d guardHeld=%d attackerZone=%d read=NATIVE_STATE_778 effect=NONE",
                (unsigned long long)g.window55c2.serial,pbCount55c2,g.held?1:0,incoming176.sector);diagnostic_trace::Write(m26);}}
    if(window594.valid)++g_parry_motion57c1;
    if(toward57c1)++g_parry_toward57c1;
    // H5: motion can request the native block input even when the old held
    // guard has not exposed a +778 opening. This never awards a PB. Spend one
    // deliberate beat per actually observed same-opponent attack, not every
    // frame of overlapping 160ms motion and not automatic attack-onset refresh.
    const bool motionBeatH5=parryMode176&&GuardMotionRequired59c3()&&toward57c1&&g.beatH5.ready();
    // H20: an early counter-motion may have delivered an ordinary block before
    // +778 opened. A NEW deliberate motion inside the real native opening must
    // remain offerable. againstRecent's lastMotionTickH5 fence excludes the
    // consumed translation; the opening is still consumed once below. Neither
    // this edge nor its trace awards a native perfect-block result.
    const bool nativeBeatH5=windowReady55c2&&toward57c1&&
        ((!g.beatH5.active&&!g.beatH5.serial)||g.beatH5.spent);
    const bool incomingEdge45=false&&(GuardMotionRequired59c3()?(motionBeatH5||nativeBeatH5):windowReady55c2)&&(parryMode176||g_guard_incoming_repress45.load(std::memory_order_relaxed))&&g.held&&want&&
        !manual&&!g.pendingPress46&&
        (!g.incomingEdgeTick45||now<g.incomingEdgeTick45||now-g.incomingEdgeTick45>=g_guard_incoming_cooldown45.load(std::memory_order_relaxed));
    const bool autoWindup19c4r1=false&&!incomingEdge45&&windupEdge19c4r1&&incoming176.attacking&&incoming176.phase==4&&
        g.held&&want&&!manual&&!g.pendingPress46&&native_admission147::guardHeldInput44c2(native,true);
    if(incomingEdge45||autoWindup19c4r1){
        const bool metadataMotionH14=heldBlockMetadataH14&&incomingEdge45&&GuardMotionRequired59c3()&&toward57c1&&
            (motionBeatH5||nativeBeatH5);
        // No input edge from grace/replayed tracking, even before the release.
        TrackingSnapshot fresh55c2{};
        if(!ReadTrackingSnapshot(fresh55c2)||!fresh55c2.allowed||!fresh55c2.head_valid||!fresh55c2.right_valid||!fresh55c2.left_valid||
           fresh55c2.head_held||fresh55c2.right_held||fresh55c2.left_held||now<fresh55c2.tick||now-fresh55c2.tick>60){
            g_guard_stale59c17.fetch_add(1,std::memory_order_relaxed);return;} // R59.16C10R17: was a silent return (no receipt)
        // Auto refresh does not consume a native PB window or its cooldown.
        if(!autoWindup19c4r1){g.incomingEdgeTick45=now;g.window55c2.consume();g.pendingWindow55c2=g.window55c2.serial;}
        g.pendingAutoWindup19c4r1=false;g.pendingBlockMetadataH14=false;g.pendingLegacyMotionH14=false;
        const auto originalLeaseH14=g.leaseH14;
        const bool released=PhysCallAction6698(owner.player,3,2);
        if(!released){++g_guard_incoming_dropped45;return;} // keep ownership of an input we did not release
        if(!NativeGuardAwaitReleaseH14(f,originalLeaseH14))return; // callback replaced the input owner
        if(motionBeatH5)g.beatH5.consume();
        if(GuardMotionRequired59c3()&&incomingEdge45)g.lastMotionTickH5=raw168.tick;
        g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;
        NativeOwner104 again{};nc104::Observed checked{};
        const bool nativeSafe=released&&g_gameplay_allowed.load()&&!g_phys_fault6698.load()&&
            NativeResolve104(owner.player,again)&&again.actor==owner.actor&&again.state==owner.state&&again.epoch==owner.epoch&&
            NativeObserve104(again,checked)&&(native_admission147::guardHeldInput44c2(checked,true)||
                (metadataMotionH14&&nc104::blockMetadataH14(checked)));
        PhysFrame6698 current46c1{};TrackingSnapshot tracking46c1{};std::uint64_t held46c1=0;
        void* manager46c1=nullptr;void* back46c1=nullptr;
        const bool same46c1=nativeSafe&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
            !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
            !animation_filter::nativeBodyOwned112.load()&&g_phys_armed6698.load()&&
            FullWeaponMode120(g_weapon_control_mode.load())&&!diagnostics::lab::NativeReference()&&CombatExperiment175Enabled()&&
            Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager46c1)&&manager46c1==manager&&
            manager46c1==g_henry_item_attach_manager.load()&&PhysVft6698(manager46c1,0x3EC9958)&&
            Read(static_cast<unsigned char*>(manager46c1)+0x38,back46c1)&&back46c1==owner.player&&
            NativeHandKey6697(manager46c1,side,held46c1)&&held46c1==key&&f.request==g_weapon_request_epoch6694.load()&&
            g_phys_frame6698.read(current46c1)&&current46c1.player==owner.player&&current46c1.manager==manager&&
            current46c1.request==f.request&&current46c1.input.allowed&&current46c1.input.bodyEpoch==owner.epoch&&
            now>=current46c1.input.tick&&now-current46c1.input.tick<=80&&
            current46c1.output.active[side]&&current46c1.output.owned[side]&&current46c1.profiles[side].key==key&&
            current46c1.profiles[side].generation==f.profiles[side].generation&&current46c1.input.positionGuard176&&
            current46c1.output.guardBySide30c1[side]&&
            ReadTrackingSnapshot(tracking46c1)&&tracking46c1.allowed&&tracking46c1.head_valid&&tracking46c1.right_valid&&tracking46c1.left_valid&&
            !tracking46c1.head_held&&!tracking46c1.right_held&&!tracking46c1.left_held&&
            now>=tracking46c1.tick&&now-tracking46c1.tick<=60;
        const bool manualNow=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
        // R46 (ChatGPT R45 review P1): the release may not have been acknowledged by the game yet (XI transport queues it;
        // +308 can still read 1). Then the press is NOT dropped: it is the pending half of this transaction and is issued
        // by the next ticks (deadline 150 ms), once the native state no longer reports the old block.
        NativeParryOpponent176 pair55c2{};int afterCount55c2=-1;
        const bool samePair19c4r1=same46c1&&NativeParryOpponentRead176(again,pair55c2)&&
            pair55c2.actor==incoming176.actor&&pair55c2.state==incoming176.state&&pair55c2.entity==incoming176.entity&&pair55c2.id==incoming176.id;
        const bool timed55c2=!autoWindup19c4r1&&samePair19c4r1&&NativeParryWindow55c2(again,afterCount55c2)&&afterCount55c2>0;
        const bool autoQualified19c4r1=autoWindup19c4r1&&samePair19c4r1;
        const bool motionQualifiedH5=motionBeatH5&&samePair19c4r1&&pair55c2.incoming&&
            g.beatH5.active&&g.beatH5.key.same(windupKey19c4r1);
        const auto cause19c4r1=autoQualified19c4r1?diagnostics::parry176::Cause::AutoWindup:
            motionQualifiedH5?diagnostics::parry176::Cause::Motion:
            timed55c2?diagnostics::parry176::Cause::NativeWindow:diagnostics::parry176::Cause::Initial;
        const bool metadataRepressH14=metadataMotionH14&&samePair19c4r1&&pair55c2.incoming&&
            pair55c2.sector==incoming176.sector&&(motionQualifiedH5||timed55c2);
        const int releaseAckH14=same46c1?NativeGuardReleaseAckH14(again):-1;
        const bool pressed=same46c1&&(native_admission147::guardInput(checked)||metadataRepressH14)&&
            releaseAckH14==0&&!manualNow&&!checked.blocking&&PhysCallAction6698(owner.player,3,1);
        if(pressed){NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;++g_guard_counts40c1[4];
            g_guard_incoming_edges45.fetch_add(1,std::memory_order_relaxed);
            NativeParryTrace176(again,(timed55c2||autoQualified19c4r1||motionQualifiedH5)?&incoming176:nullptr,cause19c4r1,now,manualNow,true,diagnostics::parry176::publication.load(std::memory_order_acquire));}
        else if(same46c1&&!manualNow&&releaseAckH14>=0){g.pendingPress46=true;g.pendingSince46=now;
            g.pendingBlockMetadataH14=metadataRepressH14;
            g.pendingAutoWindup19c4r1=autoWindup19c4r1;g.pendingWindupKey19c4r1=windupKey19c4r1;
            g.pendingMotionH5=motionQualifiedH5;g.pendingBeatH5=g.beatH5.serial;
            g.pendingOpponent46c1=(timed55c2||autoQualified19c4r1||motionQualifiedH5)?incoming176:NativeParryOpponent176{};g.pendingPublication46c1=diagnostics::parry176::publication.load(std::memory_order_acquire);}
        else g_guard_incoming_dropped45.fetch_add(1,std::memory_order_relaxed);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char line[352]{};
            std::snprintf(line,sizeof(line),"V66.178R45 GUARD_INCOMING edge=%s source=%s slot=%d zone=%d sector=%d opponent=%u ourPhase=%d nativeBlocking=%d acceptedBlock=UNCLAIMED timing=NATIVE againstCm=%.1f travelCm=%.1f lateral=%d perceived=%d releaseAckContext=%d metadata=%d fence=%d pendingAgeMs=%llu",
                pressed?"RELEASE_PRESS":g.pendingPress46?"RELEASE_PRESS_PENDING":"RELEASE_ONLY",autoWindup19c4r1?"C4R1_AUTO_WINDUP":motionBeatH5?"H5_COUNTER_MOTION":"NATIVE_WINDOW",pbCount55c2,incoming176.zone,incoming176.sector,incoming176.id,checked.phase,checked.blocking?1:0,
                against59c17.along*100.,against59c17.travel*100.,against59c17.lateral,incoming176.perceived,
                releaseAckH14,int(metadataMotionH14),int(g.releaseWaitH14),(unsigned long long)(g.pendingPress46?now-g.pendingSince46:0));
            diagnostic_trace::Write(line);}
        return;
    }
    const bool motionEdge176=false&&!g_guard_countermotion57c1.load()&&!g_guard_incoming_repress45.load(std::memory_order_relaxed)&&motion176.valid&&g_cal_state.load(std::memory_order_acquire)==2&&
        opponentRead45&&incoming176.incoming&&
        kcd2vr::guard_parry176::againstAttack(motion176,incoming176.sector,
            CalFrameR133(),ph::body_axes(f.input).right,.06,.65).ok;
    if(motionEdge176){
        const auto publication=diagnostics::parry176::publication.load(std::memory_order_acquire);
        // This is an INPUT edge through the real block action. The native timing
        // window and all acceptance/perfect-block/masterstrike flags stay native.
        // The held positional guard owns this edge; Trigger is not an input.
        // Native eligibility and the original timing window decide its outcome.
        const auto originalLeaseH14=g.leaseH14;
        const bool released=PhysCallAction6698(owner.player,3,2);
        if(released&&!NativeGuardAwaitReleaseH14(f,originalLeaseH14))return;
        g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;
        NativeOwner104 again{};nc104::Observed checked{};std::uint64_t heldKey=0;
        // Revalidate after the native release before restoring positional guard.
        // No stale hand, changed owner or mode transition can emit the new press.
        const bool nativeSafe=released&&g_gameplay_allowed.load()&&!g_phys_fault6698.load()&&
            NativeResolve104(owner.player,again)&&again.actor==owner.actor&&again.state==owner.state&&again.epoch==owner.epoch&&
            NativeObserve104(again,checked)&&native_admission147::guardWritable(checked);
        PhysFrame6698 currentFrame{};TrackingSnapshot currentTracking{};void* currentManager=nullptr;void* currentBack=nullptr;
        const bool same=nativeSafe&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
            !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
            !animation_filter::nativeBodyOwned112.load()&&g_phys_armed6698.load()&&
            FullWeaponMode120(g_weapon_control_mode.load())&&!diagnostics::lab::NativeReference()&&CombatExperiment175Enabled()&&
            Read(static_cast<unsigned char*>(owner.player)+0x9D0,currentManager)&&currentManager==manager&&
            currentManager==g_henry_item_attach_manager.load()&&PhysVft6698(manager,0x3EC9958)&&
            Read(static_cast<unsigned char*>(manager)+0x38,currentBack)&&currentBack==owner.player&&
            NativeHandKey6697(manager,side,heldKey)&&heldKey==key&&f.request==g_weapon_request_epoch6694.load()&&
            g_phys_frame6698.read(currentFrame)&&currentFrame.player==owner.player&&currentFrame.manager==manager&&
            currentFrame.request==f.request&&currentFrame.input.allowed&&currentFrame.input.bodyEpoch==owner.epoch&&
            currentFrame.output.active[side]&&currentFrame.output.owned[side]&&currentFrame.profiles[side].key==key&&
            currentFrame.profiles[side].generation==f.profiles[side].generation&&
            currentFrame.input.positionGuard176&&ph::natural_guard117(currentFrame.input,currentFrame.profiles[side],currentFrame.output.weapons[side],true)&&
            ReadTrackingSnapshot(currentTracking)&&currentTracking.sequence==raw168.sequence&&
            currentTracking.allowed&&currentTracking.head_valid&&currentTracking.right_valid&&currentTracking.left_valid&&
            !currentTracking.head_held&&!currentTracking.right_held&&!currentTracking.left_held;
        const bool manualNow=g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load();
        NativeParryOpponent176 rechecked176{};
        const bool incomingSame=same&&NativeParryOpponentRead176(again,rechecked176)&&rechecked176.incoming&&
            rechecked176.actor==incoming176.actor&&rechecked176.state==incoming176.state&&
            rechecked176.entity==incoming176.entity&&rechecked176.id==incoming176.id&&rechecked176.sector==incoming176.sector;
        const bool ownerAllows=!manualNow;
        // A re-entrant native/manual press already restored block. Do not emit
        // a second press or mislabel that other press as this motion's result.
        const bool releaseAck176=nativeSafe&&NativeGuardReleaseAckH14(again)==0;
        if(nativeSafe&&checked.blocking){
            // An existing block has no new positional press owner to adopt.
        }else if(same&&incomingSame&&ownerAllows&&releaseAck176&&PhysCallAction6698(owner.player,3,1)){
            NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
            ++g_guard_counts40c1[4];
            NativeParryTrace176(again,&incoming176,diagnostics::parry176::Cause::Motion,now,manualNow,true,publication);
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write(
                "V66.176 GUARD_MOTION edge=RELEASE_PRESS source=F4_NATIVE_ATTACK_SECTOR guardOwner=POSITION acceptedParry=UNCLAIMED timing=NATIVE");
        }else {
            // A changed incoming attack does not drop an otherwise valid held
            // positional guard. Reacquire ordinary block, without motion/parry
            // attribution. Mode/tracking/posture/owner loss forbids restoration.
            if(same&&!manualNow&&!checked.blocking&&releaseAck176&&PhysCallAction6698(owner.player,3,1)){
                NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
                NativeParryTrace176(again,nullptr,diagnostics::parry176::Cause::Initial,now,false,true,publication);
            }else if(same&&!manualNow&&!checked.blocking&&NativeGuardReleaseAckH14(again)==1){
                g.pendingPress46=true;g.pendingSince46=now;g.pendingBlockMetadataH14=false;
                g.pendingLegacyMotionH14=incomingSame;g.pendingAutoWindup19c4r1=false;g.pendingMotionH5=false;
                g.pendingWindupKey19c4r1=windupKey19c4r1;
                g.pendingOpponent46c1=incomingSame?incoming176:NativeParryOpponent176{};g.pendingPublication46c1=publication;
            }
            g.motion168.reset();g.parry176.reset();
            if(!same)diagnostics::parry176::Clear();
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))
                diagnostic_trace::Write("V66.176 GUARD_MOTION edge=UNQUALIFIED_AFTER_RELEASE reason=REVALIDATION_OR_NATIVE_CALL acceptedParry=UNCLAIMED");
        }
        return;
    }
    if(motionEdge168){
        // Release OUR physical block, then revalidate the live actor/item before
        // pressing. This asks native timing again; it cannot grant a perfect block.
        const auto originalLeaseH14=g.leaseH14;
        const bool released=PhysCallAction6698(owner.player,3,2);
        if(released&&!NativeGuardAwaitReleaseH14(f,originalLeaseH14))return;
        g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;
        NativeOwner104 again{};nc104::Observed checked{};std::uint64_t heldKey=0;
        PhysFrame6698 current168{};TrackingSnapshot tracking168{};void* manager168=nullptr;void* back168=nullptr;
        const bool same168=released&&g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
           !g_ctx_menu.load()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
           !animation_filter::nativeBodyOwned112.load()&&!g_phys_fault6698.load()&&g_phys_armed6698.load()&&
           FullWeaponMode120(g_weapon_control_mode.load())&&!diagnostics::lab::NativeReference()&&
           NativeResolve104(owner.player,again)&&again.actor==owner.actor&&again.state==owner.state&&again.epoch==owner.epoch&&
           NativeObserve104(again,checked)&&native_admission147::guardWritable(checked)&&
           Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager168)&&manager168==manager&&manager168==g_henry_item_attach_manager.load()&&
           PhysVft6698(manager168,0x3EC9958)&&Read(static_cast<unsigned char*>(manager168)+0x38,back168)&&back168==owner.player&&
           NativeHandKey6697(manager,0,heldKey)&&heldKey==key&&
           f.request==g_weapon_request_epoch6694.load()&&g_phys_frame6698.read(current168)&&current168.player==owner.player&&
           current168.manager==manager&&current168.request==f.request&&current168.input.allowed&&current168.input.bodyEpoch==owner.epoch&&
           now>=current168.input.tick&&now-current168.input.tick<=80&&current168.output.active[side]&&current168.output.owned[side]&&
           current168.profiles[side].key==key&&current168.profiles[side].generation==f.profiles[side].generation&&
           ph::natural_guard117(current168.input,current168.profiles[side],current168.output.weapons[side],true)&&
           (!current168.input.positionGuard176||current168.output.guardBySide30c1[side])&&
           ReadTrackingSnapshot(tracking168)&&tracking168.allowed&&tracking168.head_valid&&tracking168.right_valid&&tracking168.left_valid&&
           !tracking168.head_held&&!tracking168.right_held&&!tracking168.left_held&&now>=tracking168.tick&&now-tracking168.tick<=60&&
           !(g_guard_manual_owner112.load()==owner.player&&g_guard_manual112.load());
        const int ack168=same168?NativeGuardReleaseAckH14(again):-1;
        if(same168&&ack168==0&&PhysCallAction6698(owner.player,3,1)){
            NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write("V66.168 GUARD_MOTION edge=RELEASE_PRESS source=TRACKED_TRANSLATION acceptedParry=UNCLAIMED timing=NATIVE");
        }else {if(same168&&ack168==1){g.pendingPress46=true;g.pendingSince46=now;
                g.pendingBlockMetadataH14=g.pendingLegacyMotionH14=g.pendingAutoWindup19c4r1=g.pendingMotionH5=false;
                g.pendingWindupKey19c4r1=windupKey19c4r1;g.pendingOpponent46c1={};
                g.pendingPublication46c1=diagnostics::parry176::publication.load(std::memory_order_acquire);}
            g.motion168.reset();if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write("V66.168 GUARD_MOTION edge=RELEASE_ONLY reason=REVALIDATION_OR_NATIVE_CALL");}
        return;
    }
    // C39 DIRECT (VrDirectGuardOutOfStagger): ONE ordinary block press per native perfect-block opening while Henry is in his own
    // hit reaction (64) or recovery (32) with the VR guard posture held; guardInput() never pressed there (26 of 46 unanswered
    // openings in the C37C2 DIRECT run). The native block input decides; the press is released if the game does not adopt it.
    {   namespace dc=kcd2vr::direct_counter39;static dc::Stagger stagger39{};static dc::Opening opening39{};
        const bool direct39=parryMode176&&kcd2vr::contact58c1::experiment.load(std::memory_order_relaxed);
        const bool opened39=opening39.observe(reinterpret_cast<std::uintptr_t>(incoming176.actor),incoming176.id,windowKnown55c2&&opponentRead45,pbCount55c2);
        dc::StaggerIn in39{};in39.enabled=direct39&&dc::staggerGuard.load(std::memory_order_relaxed);in39.posture=decision.block;in39.held=g.held;
        in39.manual=manual;in39.blocking=native.blocking;in39.windowOpen=opening39.open&&windowKnown55c2&&opponentRead45&&pbCount55c2>0;
        in39.attacking=opponentRead45&&incoming176.attacking;in39.phase=native.phase;in39.type=native.type;in39.window=opening39.serial;in39.now=now;
        const bool f339=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now);
        if(opened39&&f339){const auto rt=g_guard_last_release_tick39.load(std::memory_order_relaxed);char m[420]{};std::snprintf(m,sizeof(m),
            "C39 PB_WINDOW edge=OPEN serial=%llu count=%d henryPhase=%d type=%d blocking308=%d guardHeld=%d posture=%d lastRelease=%s lastReleaseAgeMs=%lld opponent=%u oppPhase=%d offered158=%d staggerEligible=%d direct=%d",
            (unsigned long long)opening39.serial,pbCount55c2,native.phase,native.type,native.blocking?1:0,g.held?1:0,decision.block?1:0,g_guard_last_release39.load(std::memory_order_relaxed),
            rt&&now>=rt?(long long)(now-rt):-1LL,incoming176.id,incoming176.phase,incoming176.sector,int(dc::staggered(native.phase,native.type)),int(direct39));diagnostic_trace::Write(m);}
        dc::Why why39=dc::Why::None;const auto act39=dc::step(stagger39,in39,why39);
        const auto note39=[&](const char* edge,int returned){if(!f339)return;const auto held39=stagger39.tick&&now>=stagger39.tick?now-stagger39.tick:0;char m[360]{};std::snprintf(m,sizeof(m),
            "C39 GUARD_STAGGER edge=%s why=%s window=%llu henryPhase=%d type=%d pb778=%d opponent=%u oppPhase=%d heldMs=%llu returned=%d nativeOutcome=UNCLAIMED timing=NATIVE",
            edge,dc::name(why39),(unsigned long long)opening39.serial,native.phase,native.type,pbCount55c2,incoming176.id,incoming176.phase,(unsigned long long)held39,returned);diagnostic_trace::Write(m);};
        if(act39==dc::Act::Press){
            if(NativeGuardReleaseFenceH14(owner,f)){stagger39.pending=false;return;}
            const bool pressed39=PhysCallAction6698(owner.player,3,1);
            if(pressed39){NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;g.window55c2.consume();++g_guard_counts40c1[0];g_guard_reason40c1="C39_STAGGER_PRESS";}
            else stagger39.pending=false;
            note39("PRESS",pressed39?1:0);return;
        }
        if(act39==dc::Act::Keep){if(now<g.lastHold||now-g.lastHold>=50){(void)PhysCallAction6698(owner.player,3,4);g.lastHold=now;}return;}
        if(act39==dc::Act::Release){
            const bool released39=PhysCallAction6698(owner.player,3,2);
            g.held=false;g_guard_physical112=false;g_guard_physical_owner112=nullptr;GuardReleaseNote39("C39_STAGGER_NOT_ADOPTED",native.phase);
            note39("RELEASE",released39?1:0);return;
        }
        if(act39==dc::Act::Adopted)note39("ADOPTED",1);
    }
    if(g.pendingPress46&&(!want||g.held||manual||now<g.pendingSince46||now-g.pendingSince46>150)){
        if(!g.held)g_guard_incoming_dropped45.fetch_add(1,std::memory_order_relaxed);g.pendingPress46=false;g.pendingBlockMetadataH14=false;g.pendingLegacyMotionH14=false;}
    if(want&&!g.held){
        if(NativeGuardReleaseFenceH14(owner,f))return;
        // A foreign native block is not ours to release. A known manual LT can
        // share ownership, and its release will be balanced by our lease.
        if((native.blocking&&!manual)||(!native_admission147::guardInput(native)&&
           !(nc104::blockMetadataH14(native)&&NativePendingBlockMetadataH14(owner,now))))return;
        const auto publication=diagnostics::parry176::publication.load(std::memory_order_acquire);
        const bool pendingPress46=g.pendingPress46;
        const bool pendingAuto19c4r1=pendingPress46&&g.pendingAutoWindup19c4r1;
        const bool pendingMotionH5=pendingPress46&&g.pendingMotionH5;
        if(pendingPress46){TrackingSnapshot fresh55c2{};
            if(!ReadTrackingSnapshot(fresh55c2)||!fresh55c2.allowed||!fresh55c2.head_valid||!fresh55c2.right_valid||!fresh55c2.left_valid||
               fresh55c2.head_held||fresh55c2.right_held||fresh55c2.left_held||now<fresh55c2.tick||now-fresh55c2.tick>60){
                NativeGuardRelease112("PARRY_PENDING_TRACKING");return;}}
        if(pendingPress46){const int ackH14=NativeGuardReleaseAckH14(owner);
            if(ackH14<0){g.pendingPress46=g.pendingBlockMetadataH14=g.pendingLegacyMotionH14=false;return;}
            if(ackH14!=0)return;}
        const bool wasBlockMetadataH14=pendingPress46&&g.pendingBlockMetadataH14;
        const bool pendingLegacyMotionH14=pendingPress46&&g.pendingLegacyMotionH14;
        g.pendingPress46=false;g.pendingAutoWindup19c4r1=false;g.pendingMotionH5=false;g.pendingBlockMetadataH14=false;g.pendingLegacyMotionH14=false;
        bool motionQualifiedH5=(pendingMotionH5&&opponentRead45&&fresh176&&incoming176.incoming&&
            windupKey19c4r1.same(g.pendingWindupKey19c4r1)&&g.beatH5.active&&g.beatH5.serial==g.pendingBeatH5&&
            incoming176.actor==g.pendingOpponent46c1.actor&&incoming176.state==g.pendingOpponent46c1.state&&
            incoming176.entity==g.pendingOpponent46c1.entity&&incoming176.id==g.pendingOpponent46c1.id)||
            (pendingLegacyMotionH14&&opponentRead45&&fresh176&&incoming176.incoming&&
             windupKey19c4r1.same(g.pendingWindupKey19c4r1)&&incoming176.sector==g.pendingOpponent46c1.sector);
        const bool autoQualified19c4r1=pendingAuto19c4r1&&opponentRead45&&windupKey19c4r1.same(g.pendingWindupKey19c4r1)&&
            g_guard_auto_windup19c4r1.load(std::memory_order_relaxed)&&incoming176.actor==g.pendingOpponent46c1.actor&&
            incoming176.state==g.pendingOpponent46c1.state&&incoming176.entity==g.pendingOpponent46c1.entity&&incoming176.id==g.pendingOpponent46c1.id;
        bool timed55c2=!pendingAuto19c4r1&&windowKnown55c2&&pbCount55c2>0&&opponentRead45&&
            (!pendingPress46||(g.pendingWindow55c2==g.window55c2.serial&&
                incoming176.actor==g.pendingOpponent46c1.actor&&incoming176.state==g.pendingOpponent46c1.state&&
                incoming176.entity==g.pendingOpponent46c1.entity&&incoming176.id==g.pendingOpponent46c1.id));
        // The motion attribution expires with its original pair/side/window.
        // Same current owned guard may restore once after ACK as ordinary block.
        if(wasBlockMetadataH14&&(!GuardMotionRequired59c3()||pendingAuto19c4r1||
           (!motionQualifiedH5&&!timed55c2)||incoming176.sector!=g.pendingOpponent46c1.sector)){
            motionQualifiedH5=false;timed55c2=false;}
        int masterBefore54=-1;
        const bool traceMaster54=!manual&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&
            NativeMasterCount23(owner,masterBefore54);
        const bool pressed54=manual||PhysCallAction6698(owner.player,3,1);
        if(traceMaster54&&masterBefore54>0){int masterAfter54=-1;(void)NativeMasterCount23(owner,masterAfter54);
            char m[280]{};std::snprintf(m,sizeof(m),
                "C54 GUARD_MASTER_ARBITRATION sample=%llu gesture=%llu before7D8=%d after7D8=%d blockPress=%d handoff=%llu outcome=NATIVE_UNCLAIMED",
                (unsigned long long)f.input.sample,(unsigned long long)f.output.primaryStrokeGesture19,
                masterBefore54,masterAfter54,int(pressed54),(unsigned long long)handoff54.serial);diagnostic_trace::Write(m);}
        if(pressed54){NativeGuardMarkHeldH14();g.lastHold=now;g_guard_physical_owner112=owner.player;g_guard_physical112=true;
            if(!pendingAuto19c4r1&&!pendingMotionH5)g.window55c2.consume();
            if(pendingPress46){++g_guard_counts40c1[4];g_guard_incoming_edges45.fetch_add(1,std::memory_order_relaxed);
                if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write(pendingAuto19c4r1?
                    "V66.178R46 GUARD_INCOMING edge=PRESS_AFTER_RELEASE_ACK source=C4R1_AUTO_WINDUP nativeOutcome=UNCLAIMED timing=NATIVE":
                    motionQualifiedH5?"H5 GUARD_INCOMING edge=PRESS_AFTER_RELEASE_ACK source=COUNTER_MOTION nativeOutcome=UNCLAIMED timing=NATIVE":
                    "V66.178R46 GUARD_INCOMING edge=PRESS_AFTER_RELEASE_ACK source=NATIVE_WINDOW_OR_ORDINARY_RESTORE timing=NATIVE");}
            if(parryMode176){++g_guard_counts40c1[manual?5:0];g_guard_reason40c1="POSTURE_ENTRY";}
            if(parryMode176&&!manual)NativeParryTrace176(owner,(timed55c2||autoQualified19c4r1||motionQualifiedH5)?&incoming176:nullptr,
                autoQualified19c4r1?diagnostics::parry176::Cause::AutoWindup:motionQualifiedH5?diagnostics::parry176::Cause::Motion:timed55c2?diagnostics::parry176::Cause::NativeWindow:diagnostics::parry176::Cause::Initial,
                now,false,pendingPress46,pendingPress46?g.pendingPublication46c1:publication);
            if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write(manual?"V66.117 GUARD edge=JOIN_MANUAL timing=NATIVE":"V66.117 GUARD edge=PRESS reason=POSTURE_ENTRY timing=NATIVE postureOnlyRepress=0 motionRepress=GAMEPLAY_CAPABILITY");}
    }else if(!want&&g.held){
        if(parryMode176){++g_guard_counts40c1[decision.block?2:1];g_guard_reason40c1=decision.block?"NATIVE_INPUT_STATE":"POSTURE_EXIT";}
        if(!manual)(void)PhysCallAction6698(owner.player,3,2);
        GuardReleaseNote39(decision.block?"NATIVE_INPUT_STATE":"POSTURE_EXIT",native.phase);
        g.held=false;g_guard_physical112=false;g_guard_release_tick59c7e.store(std::uint64_t(GetTickCount64()),std::memory_order_relaxed);g_guard_release_state59c7e.store(g.owner.state,std::memory_order_relaxed);
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))diagnostic_trace::Write("V66.117 GUARD edge=RELEASE reason=POSTURE_OR_NATIVE_STATE");
    }else if(want&&g.held&&!manual&&now-g.lastHold>=50){
        (void)PhysCallAction6698(owner.player,3,4);g.lastHold=now;
    }
}
static void NativeRelease104(const char* reason) noexcept {
    auto& p=g_native_pending104;if(!p.held)return;
    NativeOwner104 now{};nc104::Observed observed{};
    // Release only OUR matching request in the same still-live native owner.
    if(NativeResolve104(p.owner.player,now)&&now.actor==p.owner.actor&&now.state==p.owner.state&&
       NativeObserve104(now,observed)&&observed.requested&&observed.input==p.input)NativeInvoke104(now.actor,p.input,false);
    p.held=false;(void)reason;
}
// These diagnostics report adapter rejection, NOT a fabricated native reason.
enum class NativeReject104 : unsigned { None,Context,Tracking,Contract,Owner,State,Equipment,HandRole,RequestBusy,InputId,ForeignRequest,ZoneCall,OwnerChanged,NativeCall };
static bool NativeRejectEvent104(NativeReject104 why,nc104::Gesture g,int side,std::uint64_t key,void* actor=nullptr) noexcept {
    NativeResult104 e{};e.kind=7;e.tick=GetTickCount64();e.flags=unsigned(why);e.gesture=int(g);e.hand=side;e.key=key;e.actor=reinterpret_cast<std::uintptr_t>(actor);NativePush104(e);return false;
}
static bool NativeSubmit104(void* player,nc104::Gesture gesture,int side,std::uint64_t key,pm::Vec3 velocity) noexcept {
    using R=NativeReject104;
    const auto reject=[&](R why,void* actor=nullptr){return NativeRejectEvent104(why,gesture,side,key,actor);};
    if(!NativeSelected104())return reject(R::Context);
    if(side<0||side>1||!(side?g_left_valid.load():g_right_valid.load())||!pm::finite(velocity))return reject(R::Tracking);
    if(!NativeArm104())return reject(R::Contract);
    NativeOwner104 owner{};nc104::Observed before{};
    if(!NativeResolve104(player,owner))return reject(R::Owner);
    if(!NativeObserve104(owner,before)||!nc104::valid(before))return reject(R::State,owner.actor);
    void* manager=nullptr;void* back=nullptr;std::uint64_t handKey=0,otherKey=0;
    if(!Read(static_cast<unsigned char*>(player)+0x9D0,manager)||manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=player||!NativeHandKey6697(manager,side,handKey))return reject(R::Equipment,owner.actor);
    if(key){if(handKey!=key)return reject(R::HandRole,owner.actor);}
    else if(!PhysEmpty6698(handKey)||!NativeHandKey6697(manager,1-side,otherKey)||!PhysEmpty6698(otherKey))return reject(R::HandRole,owner.actor);
    const auto now=std::uint64_t(GetTickCount64());auto& p=g_native_pending104;
    if(p.held||(p.watching&&now>=p.tick&&now-p.tick<160))return reject(R::RequestBusy,owner.actor);
    const auto in=nc104::intent(gesture,side,velocity);int nativeInput=-1;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x5321010+4*in.inputIndex),nativeInput)||nativeInput<0||nativeInput>=256)return reject(R::InputId,owner.actor);
    // Do not take over an unrelated request or force the native action state.
    if(before.requested)return reject(R::ForeignRequest,owner.actor);
    if(!NativeZone104(owner.actor,in.zone))return reject(R::ZoneCall,owner.actor);
    NativeOwner104 afterZone{};
    if(!NativeResolve104(player,afterZone)||afterZone.actor!=owner.actor||afterZone.state!=owner.state||afterZone.epoch!=owner.epoch)return reject(R::OwnerChanged,owner.actor);
    if(!NativeInvoke104(owner.actor,nativeInput,true))return reject(R::NativeCall,owner.actor);
    p={};p.owner=owner;p.intent=in;p.before=before;p.serial=++g_native_serial104;p.tick=now;p.frame=g_native_frame104.load();p.input=nativeInput;p.key=key;p.held=true;p.watching=true;
    g_native_owner104.publish(owner);
    NativeResult104 e{};e.kind=1;e.tick=now;e.serial=p.serial;e.key=key;e.actor=reinterpret_cast<std::uintptr_t>(owner.actor);e.gesture=int(gesture);e.input=nativeInput;e.zone=in.zone;e.hand=side;e.expectedType=in.expectedType;NativePush104(e);return true;
}
static void NativeFrameBegin104() noexcept {
    ++g_native_frame104;
    auto& p=g_native_pending104;
    if(!NativeSelected104()){NativeRelease104("MODE_OR_CONTEXT_EXIT");p.watching=false;}
    // A full native frame must see the asserted request before the release edge.
    // The engine's action state, not a fabricated button hold, owns any continuation.
    if(p.held&&g_native_frame104.load()>p.frame)NativeRelease104("NEXT_NATIVE_FRAME");
}
static void NativeFrameEnd104() noexcept {
    ReadScope101R2 readScope;
    const bool ref=diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64());
    auto& p=g_native_pending104;
    // V66.109: V66.104 direct requests are no longer the production fist/sword
    // transport. Do not pay the ~2.9 ms native-state readback every F10/F12 frame.
    // Poll only during a bounded direct-request acknowledgement window or F3 reference.
    if(!p.watching&&!ref){g_native_owner104.publish(NativeOwner104{});return;}
    if(!NativeSelected104()&&!ref){g_native_owner104.publish(NativeOwner104{});return;}
    // Diagnostic snapshots need not run once per rendered/native frame. Live
    // write gates resolve their own current owner; request acknowledgement stays
    // unthrottled. A fresh contact prefilter has an independent 80 ms expiry.
    static std::uint64_t observedAt114=0;
    const auto now=std::uint64_t(GetTickCount64());
    if(!p.watching&&observedAt114&&now>=observedAt114&&now-observedAt114<20)return;
    observedAt114=now;
    void* player=nullptr;void* manager=g_henry_item_attach_manager.load();NativeOwner104 owner{};nc104::Observed state{};
    if(!manager||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!NativeResolve104(player,owner)||!NativeObserve104(owner,state)){g_native_owner104.publish(NativeOwner104{});return;}
    g_native_owner104.publish(owner);
    PublishContactIdentity114(owner.player,owner.actor,owner.entity);
    static nc104::Observed prior{};static void* priorOwner=nullptr;static int priorGuard=-1;
    int guardZone=-1;(void)Read(static_cast<unsigned char*>(owner.state)+0xF8,guardZone);
    const bool pending=p.watching&&owner.actor==p.owner.actor&&owner.state==p.owner.state&&owner.epoch==p.owner.epoch;
    const bool entry=pending&&!p.entered&&nc104::entered(p.before,state);
    if(entry)p.entered=true;
    if(entry||(ref&&(priorOwner!=owner.actor||priorGuard!=guardZone||nc104::changed(prior,state)))){
        NativeResult104 e{};e.kind=entry?2:3;e.tick=now;e.serial=pending?p.serial:0;e.actor=reinterpret_cast<std::uintptr_t>(owner.actor);
        e.gesture=pending?int(p.intent.gesture):-1;e.expectedType=pending?p.intent.expectedType:-1;e.key=pending?p.key:0;
        e.phase=state.phase;e.type=state.type;e.input=state.input;e.zone=state.zone;e.guardZone=guardZone;e.hand=state.hand;e.flags=(state.requestCandidate?1:0)|(state.blocking?2:0)|(state.requested?4:0);NativePush104(e);
    }
    if(pending&&now>=p.tick&&now-p.tick>500){
        NativeResult104 e{};e.kind=p.entered?6:5;e.tick=now;e.serial=p.serial;e.actor=reinterpret_cast<std::uintptr_t>(owner.actor);e.type=state.type;e.phase=state.phase;e.input=state.input;NativePush104(e);p.watching=false;
    }
    prior=state;priorOwner=owner.actor;priorGuard=guardZone;
}
static void NativeService104() noexcept {
    for(unsigned n=0;n<64;++n){NativeResult104 e{};
        if(g_native_events_lock104.test_and_set(std::memory_order_acquire))break;
        const bool have=g_native_tail104!=g_native_head104;
        if(have){e=g_native_events104[g_native_tail104];g_native_tail104=(g_native_tail104+1)%g_native_events104.size();}
        g_native_events_lock104.clear(std::memory_order_release);if(!have)break;
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,GetTickCount64())||
           !diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,e.tick))continue;
        static const char* names[]{"NONE","REQUEST_DISPATCHED","NATIVE_ACTION_ENTERED","NATIVE_STATE","NATIVE_DAMAGE_OBSERVED","NO_ENTRY_OBSERVED","ACTION_WINDOW_ENDED","ADAPTER_REQUEST_REJECTED"};
        char text[640]{};std::snprintf(text,sizeof(text),"V66.104 NATIVE_COMBAT status=%s tick=%llu serial=%llu actor=%p target=%p key=%016llX gesture=%d phase=%d type=%d expectedType=%d input=%d zone=%d guardZone=%d hand=%d flags=%u healthDamage=%.4f staminaDamage=%.4f geometry=INHERITED_NATIVE serialRelation=OWNER_TIME_NOT_HIT_PROOF damageAttribution=NOT_AUTOMATICALLY_VR",
            names[e.kind<8?e.kind:0],(unsigned long long)e.tick,(unsigned long long)e.serial,reinterpret_cast<void*>(e.actor),reinterpret_cast<void*>(e.target),(unsigned long long)e.key,e.gesture,e.phase,e.type,e.expectedType,e.input,e.zone,e.guardZone,e.hand,e.flags,e.health,e.stamina);diagnostic_trace::Write(text);
    }
}
