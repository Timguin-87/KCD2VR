// Retail-only ordinary attack START and native RPG speed getter. The R22C2
// comparison scales the getter; R50C2 instead adapts the current native clock.
namespace at22=kcd2vr::attack_timing178r22c2;
using AttackStart22=bool(__fastcall*)(void*,void*);
using AttackSpeed22=float(__fastcall*)(void*);
static AttackStart22 g_attack_start22=nullptr;
static AttackSpeed22 g_attack_speed22=nullptr;
static std::optional<DetourModKit::hook::Hook> g_attack_start_hook22,g_attack_speed_hook22;
static std::atomic<std::uintptr_t> g_attack_timing_contract22{0};
static std::atomic<std::uint64_t> g_attack_timing_serial22{0},g_attack_timing_policy22{1},g_attack_timing_consumed22{0};
static std::atomic<std::uint64_t> g_attack_timing_published22{0},g_attack_timing_matched22{0},g_attack_timing_applied22{0};
static std::atomic<float> g_attack_timing_native22{1},g_attack_timing_result22{1};
static kcd2vr::core::SnapshotMailbox<at22::Intent> g_attack_timing_intent22;
struct AttackTimingScope22 {void* actor=nullptr;void* speedComponent=nullptr;float scale=1;bool used=false,clock50=false;at22::Intent intent{};std::uint64_t tick=0,policy=0,epoch=0;void* helper54=nullptr;void* row54=nullptr;int type54=-1;};
static thread_local AttackTimingScope22* g_attack_timing_scope22=nullptr;
static bool NativeTimingSelected22() noexcept;
using MainActionFactory54=void*(__fastcall*)(void*,void*);
static MainActionFactory54 g_main_action_factory54=nullptr;
static std::optional<DetourModKit::hook::Hook> g_main_action_factory_hook54;
static void* __fastcall MainActionFactory54Hook(void* out,void* actor) noexcept {
    void* result=g_main_action_factory54?g_main_action_factory54(out,actor):nullptr;void* action=nullptr;
    namespace se54=kcd2vr::swing_effort59c14;
    if(!out||result!=out||!Read(out,action)){se54::actionRegistry54.invalidate();return result;}
    kcd2vr::fist_action_h12::recycled(reinterpret_cast<std::uintptr_t>(action));
    if(action){void* actualActor=nullptr;
        const bool exact=PhysVft6698(action,0x3A61C58)&&Read(static_cast<unsigned char*>(action)+0x70,actualActor)&&actualActor==actor;
        (void)se54::actionRegistry54.construct(reinterpret_cast<std::uintptr_t>(action),reinterpret_cast<std::uintptr_t>(actor),exact,GetTickCount64());}
    return result; // native return, allocation, reference counts and lifetime are unchanged
}
static bool ArmMainActionFactory54() noexcept {
    const auto b=WhBase();namespace se54=kcd2vr::swing_effort59c14;
    if(!b||!retail_build112::Ready(b))return false;
    if(se54::actionContractBase54.load()==b)return true;
    const unsigned char entry[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0x48,0x8B,0xFA};
    const unsigned char call[]{0xE8,0xE6,0xFD,0xFF,0xFF}; // 915931 -> 91571C, sole ordinary constructor caller
    unsigned char actual[sizeof(entry)]{},site[sizeof(call)]{};
    if(!Read(reinterpret_cast<void*>(b+0x91590C),actual)||std::memcmp(actual,entry,sizeof(entry))||
       !Read(reinterpret_cast<void*>(b+0x915931),site)||std::memcmp(site,call,sizeof(call)))return false;
    using namespace DetourModKit::hook;Options opt{};opt.fail_if_already_hooked=true;
    auto h=inline_at(InlineRequest{"kcd2vr_c54_main_action_factory",Target{DetourModKit::Address{b+0x91590C}},opt},MainActionFactory54Hook);
    if(!h)return false;g_main_action_factory_hook54=std::move(*h);g_main_action_factory54=g_main_action_factory_hook54->original<MainActionFactory54>();
    if(!g_main_action_factory54||!g_main_action_factory_hook54->enable()){g_main_action_factory_hook54.reset();g_main_action_factory54=nullptr;return false;}
    se54::actionRegistry54.invalidate();se54::actionContractBase54.store(b,std::memory_order_release);return true;
}
static bool NativeCostActionMatch54(void* action,kcd2vr::swing_effort59c14::NativeCost54& cost) noexcept {
    namespace se54=kcd2vr::swing_effort59c14;se54::Action54 binding{};cost={};const auto now=GetTickCount64();
    if(!action||se54::actionContractBase54.load()!=WhBase()||!NativeTimingSelected22()||
       !se54::actionRegistry54.start(reinterpret_cast<std::uintptr_t>(action),now,binding))return false;
    struct Quarantine54 {const kcd2vr::motion59c4::Key& key;std::uint64_t tick;bool matched=false;
        ~Quarantine54(){if(!matched)kcd2vr::swing_effort59c14::ledger.nativeUncertain54(key,tick);}} quarantine54{binding.key,now};
    NativeOwner104 owner{};const auto& key=binding.key;void* helper=nullptr;void* row=nullptr;void* back=nullptr;void* provider=nullptr;void* type=nullptr;
    int channel=-1,nativeType=-1,hand=-1;TrackingSnapshot tracking{};
    if(!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||!tracking.right_valid||!tracking.left_valid||
       tracking.head_held||tracking.right_held||tracking.left_held||!at22::fresh(tracking.tick,now)||now-tracking.tick>80||
       key.body!=g_pose_epoch6689.load()||key.request!=g_weapon_request_epoch6694.load()||key.recovery!=kcd2vr::contact176::recoveryEpoch55c1.load()||
       !kcd2vr::solver_rpg_h29::costOwned(binding,frik_pose_probe::GeometryIntentOwnerH30,frik_pose_probe::GeometrySolverCostOwnerValidH28)||
       !NativeResolve104(reinterpret_cast<void*>(key.player),owner)||reinterpret_cast<std::uintptr_t>(owner.actor)!=binding.actor||
       reinterpret_cast<std::uintptr_t>(owner.state)!=binding.state||owner.epoch!=key.body||
       !PhysVft6698(action,0x3A61C58)||!Read(static_cast<unsigned char*>(action)+0x3C,channel)||channel!=0||
       !Read(static_cast<unsigned char*>(action)+0x70,back)||back!=owner.actor||
       !Read(static_cast<unsigned char*>(action)+0xA0,helper)||reinterpret_cast<std::uintptr_t>(helper)!=binding.helper||!PhysVft6698(helper,0x3A61040)||
       !Read(static_cast<unsigned char*>(helper)+8,back)||back!=owner.actor||
       !Read(static_cast<unsigned char*>(helper)+0x20,provider)||provider!=static_cast<unsigned char*>(action)+0x88||!PhysVft6698(provider,0x3A61440)||
       !Read(static_cast<unsigned char*>(action)+0x58,row)||reinterpret_cast<std::uintptr_t>(row)!=binding.row||
       !Read(static_cast<unsigned char*>(row)+0x148,type)||!type||!Read(static_cast<unsigned char*>(type)+8,nativeType)||(nativeType!=0&&nativeType!=1)||
       !Read(static_cast<unsigned char*>(row)+0x224,hand)||hand!=0)return false;
    cost.key=key;cost.actor=binding.actor;cost.state=binding.state;cost.action=binding.action;cost.helper=binding.helper;cost.row=binding.row;
    cost.type=nativeType;cost.hand=hand;cost.causal54=binding;cost.observedTick54=now;quarantine54.matched=true;return true;
}
static void NativeTimingCancel178r22c2() noexcept {g_attack_timing_policy22.fetch_add(1,std::memory_order_acq_rel);}
static void NativeTimingCancelPending59c3() noexcept {
    // A cancelled next preparation does not revoke the CURRENT admitted action.
    // Its manager membership/phase/lifetime still revoke it natively. Ownership
    // loss calls the separate policy cancellation as before.
    const auto pending=g_attack_timing_serial22.load();auto used=g_attack_timing_consumed22.load();
    while(used<pending&&!g_attack_timing_consumed22.compare_exchange_weak(used,pending)){}
}
static bool NativeTimingMode44c1() noexcept {
    return FistSelected101()||g_combat_integrated178r22c2.load(std::memory_order_acquire)||
        (g_combat_experiment175.load(std::memory_order_acquire)&&g_physical_combat177.load(std::memory_order_acquire));
}
static bool NativeTimingSelected22() noexcept {
    return NativeTimingMode44c1()&&g_gameplay_allowed.load()&&
        !diagnostics::lab::NativeReference()&&!g_ctx_dialogue.load()&&!g_ctx_cutscene.load()&&
        !g_ctx_scripted.load()&&!g_ctx_menu.load()&&!animation_filter::nativeBodyOwned112.load()&&
        !animation_filter::PlayerNativeBodyRequiredForMotion122()&&
        g_attack_timing_contract22.load()==WhBase()&&retail_build112::Ready(WhBase());
}
static void NativeTimingPublish22(at22::Intent i) noexcept {
    // Replace pending intent only. An admitted CURRENT action keeps its clock
    // until native replacement/cancel, not merely the player's next input.
    const auto retire=g_attack_timing_serial22.load();auto used=g_attack_timing_consumed22.load();
    while(used<retire&&!g_attack_timing_consumed22.compare_exchange_weak(used,retire)){}
    if(!NativeTimingSelected22())return;
    i.policy=g_attack_timing_policy22.load();i.serial=++g_attack_timing_serial22;
    if(at22::eligible(i,GetTickCount64(),i.policy,g_pose_epoch6689.load(),0)&&g_attack_timing_intent22.publish(i))
        ++g_attack_timing_published22;
}
static void NativeTimingWeapon178r22c2(const NativeOwner104& owner,const ph::Command& c,const PhysFrame6698& f) noexcept {
    at22::Intent i{};i.player=reinterpret_cast<std::uintptr_t>(owner.player);i.actor=reinterpret_cast<std::uintptr_t>(owner.actor);
    i.state=reinterpret_cast<std::uintptr_t>(owner.state);i.manager=reinterpret_cast<std::uintptr_t>(f.manager);
    i.epoch=c.bodyEpoch;i.request=c.epoch;i.sample=c.sample;i.tick=c.tick;i.key=c.key;i.generation=c.generation;i.side=c.side;
    i.kind=c.stroke117==ph::CommandKind::Slash?at22::Kind::Slash:c.stroke117==ph::CommandKind::Thrust?at22::Kind::Thrust:at22::Kind::None;
    i.speed=c.timingSpeed178r22c2;i.gesture59c3=c.gesture59c3;
    i.recovery54=c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load();NativeTimingPublish22(i);
}
static void NativeTimingFist178r22c2(void* player,std::uint64_t sample,std::uint64_t tick,int side,double speed) noexcept {
    NativeOwner104 o{};FistOwner101 fists{};
    if(!NativeTimingSelected22()||!FistSelected101()||!NativeResolve104(player,o)||!FistReadOwner101(fists)||
       !fists.empty||fists.player!=player){NativeTimingCancel178r22c2();return;}
    at22::Intent i{};i.player=reinterpret_cast<std::uintptr_t>(player);i.actor=reinterpret_cast<std::uintptr_t>(o.actor);
    i.state=reinterpret_cast<std::uintptr_t>(o.state);i.manager=reinterpret_cast<std::uintptr_t>(fists.manager);
    i.epoch=o.epoch;i.request=fists.request;i.sample=sample;i.tick=tick;i.side=side;i.speed=speed;i.kind=at22::Kind::Punch;
    NativeTimingPublish22(i);
}
static bool MotionClockSelected50() noexcept;
static bool FistPreparedStartH31(const at22::Intent& i,std::uint64_t now) noexcept {
    // A native START may happen on the next pump after our successful release.
    // Preserve the original gesture clock; only this exact prepared/released
    // punch has the existing150ms prepared lifecycle. Tracking stays60/80ms.
    std::unique_lock<std::recursive_mutex> lockH31(g_fist_native_mutex101,std::try_to_lock);if(!lockH31.owns_lock())return false;
    const auto& n=g_fist_native101;
    return i.kind==at22::Kind::Punch&&now>=i.tick&&now-i.tick<=150&&
        n.player==reinterpret_cast<void*>(i.player)&&n.request==i.request&&n.epoch==i.epoch&&n.side==i.side&&
        n.pendingSample==i.sample&&n.pendingTick>=i.tick&&n.pendingTick-i.tick<=100&&
        ((n.lastAttack>=n.pendingTick&&n.lastAttack<=now)||(n.prepared&&g_fist_release_call_h31));
}
static bool NativeStartIntentEligibleH31(const at22::Intent& i,std::uint64_t now,std::uint64_t consumed) noexcept {
    if(at22::eligible(i,now,g_attack_timing_policy22.load(),g_pose_epoch6689.load(),consumed))return true;
    return FistPreparedStartH31(i,now)&&at22::eligible(i,i.tick,g_attack_timing_policy22.load(),g_pose_epoch6689.load(),consumed);
}
static bool NativeTimingMatch22(void* action,AttackTimingScope22& scope) noexcept {
    if(!NativeTimingMode44c1())return false;
    const auto now=std::uint64_t(GetTickCount64());at22::Intent i{};TrackingSnapshot t{};NativeOwner104 o{};
    void* earlyActor=nullptr;
    if(!g_attack_timing_intent22.read(i)||!NativeStartIntentEligibleH31(i,now,g_attack_timing_consumed22.load())||
       !action||!Read(static_cast<unsigned char*>(action)+0x70,earlyActor)||reinterpret_cast<std::uintptr_t>(earlyActor)!=i.actor||
       !NativeTimingSelected22()||!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid||t.head_held||t.right_held||t.left_held||
       !at22::fresh(t.tick,now)||now-t.tick>80||!NativeResolve104(reinterpret_cast<void*>(i.player),o)||
       reinterpret_cast<std::uintptr_t>(o.actor)!=i.actor||reinterpret_cast<std::uintptr_t>(o.state)!=i.state)return false;
    // Exact main attack class, its provider/helper backlinks, table type and hand.
    // Other action subclasses (master strikes, finishers, clinches, bow) never qualify.
    void* back=nullptr;void* helper=nullptr;void* row=nullptr;void* type=nullptr;void* provider=nullptr;int attackType=-1,hand=-1,actionChannel=-1;
    auto* a=static_cast<unsigned char*>(action);
    if(!PhysVft6698(action,0x3A61C58)||!Read(a+0x3C,actionChannel)||actionChannel!=0||!Read(a+0x70,back)||back!=o.actor||
       !Read(a+0xA0,helper)||!PhysVft6698(helper,0x3A61040)||!Read(static_cast<unsigned char*>(helper)+8,back)||back!=o.actor||
       !Read(static_cast<unsigned char*>(helper)+0x20,provider)||provider!=a+0x88||!PhysVft6698(provider,0x3A61440)||
       !Read(a+0x58,row)||!row||!Read(static_cast<unsigned char*>(row)+0x148,type)||!type||
       !Read(static_cast<unsigned char*>(type)+8,attackType)||
       (i.kind==at22::Kind::Punch?!(attackType==5||attackType==6||attackType==7):(attackType!=0&&attackType!=1))||
       !Read(static_cast<unsigned char*>(row)+0x224,hand)||hand<0||hand>1||(i.kind!=at22::Kind::Punch&&hand!=i.side)||
       !Read(static_cast<unsigned char*>(o.actor)+0x388,scope.speedComponent)||!scope.speedComponent||
       !Read(static_cast<unsigned char*>(scope.speedComponent)+8,back)||back!=o.actor)return false;
    void* manager=nullptr;std::uint64_t key=0;
    if(!Read(static_cast<unsigned char*>(o.player)+0x9D0,manager)||reinterpret_cast<std::uintptr_t>(manager)!=i.manager||
       manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=o.player)return false;
    if(i.kind==at22::Kind::Punch){
        std::uint64_t other=0;
        if(!FistSelected101()||i.request!=g_fist_epoch101.load()||!NativeHandKey6697(manager,0,key)||!PhysEmpty6698(key)||
           !NativeHandKey6697(manager,1,other)||!PhysEmpty6698(other))return false;
    }else{
        PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
           !f.input.allowed||!f.input.attacks||!f.input.coherentMelee119||!at22::fresh(f.input.tick,now)||now-f.input.tick>80||
           f.player!=o.player||f.manager!=manager||f.input.bodyEpoch!=i.epoch||f.request!=i.request||i.request!=g_weapon_request_epoch6694.load()||
           !f.output.active[0]||!f.output.owned[0]||!f.profiles[0].melee()||f.profiles[0].key!=i.key||f.profiles[0].generation!=i.generation||
           !NativeHandKey6697(manager,0,key)||key!=i.key||!key||PhysEmpty6698(key))return false;
    }
    auto consumed=g_attack_timing_consumed22.load();
    if(i.policy!=g_attack_timing_policy22.load()||consumed>=i.serial||
       !g_attack_timing_consumed22.compare_exchange_strong(consumed,i.serial))return false;
    scope.intent=i;scope.helper54=helper;scope.row54=row;scope.type54=attackType;
    scope.clock50=i.kind!=at22::Kind::Punch&&i.side==0&&MotionClockSelected50();scope.actor=o.actor;scope.tick=i.tick;scope.policy=i.policy;scope.epoch=i.epoch;scope.scale=at22::multiplier(i.kind,i.speed);++g_attack_timing_matched22;return true;
}
#include "native_attack_clock50c2.inl"
static bool __fastcall NativeAttackStart22(void* action,void* context) noexcept {
    AttackTimingScope22 scope{};const bool match=NativeTimingMatch22(action,scope);
    // A restarted/replaced native action cannot retain the previous VR fist.
    // NPC starts do not cancel Henry; a nested same-Henry start revokes the outer token.
    void* startActorH12=nullptr;(void)(action&&Read(static_cast<unsigned char*>(action)+0x70,startActorH12));
    const auto fistTokenH12=kcd2vr::fist_action_h12::begin(reinterpret_cast<std::uintptr_t>(startActorH12),
        reinterpret_cast<std::uintptr_t>(action),match&&scope.intent.kind==at22::Kind::Punch);
    InvalidateMotionClockStart50(action);
    struct Restore {AttackTimingScope22* prior;~Restore(){g_attack_timing_scope22=prior;}} restore{g_attack_timing_scope22};
    g_attack_timing_scope22=match?&scope:nullptr;
    namespace se54=kcd2vr::swing_effort59c14;se54::NativeCost54 nativeCost54{};
    (void)NativeCostActionMatch54(action,nativeCost54); // actual constructor/release/dispatch ancestry, never latest Intent
    se54::NativeCostScope54 costScope54(nativeCost54);
    const bool started=g_attack_start22?g_attack_start22(action,context):false;
    if(match&&scope.intent.kind==at22::Kind::Punch){
        kcd2vr::fist_action_h12::Binding b{};b.intent=scope.intent;
        b.action=reinterpret_cast<std::uintptr_t>(action);b.helper=reinterpret_cast<std::uintptr_t>(scope.helper54);
        b.row=reinterpret_cast<std::uintptr_t>(scope.row54);b.nativeType=scope.type54;
        FistOwner101 emptyH12{};NativeOwner104 sameH12{};
        const bool unchangedH12=started&&Read(static_cast<unsigned char*>(scope.row54)+0x224,b.nativeHand)&&
            scope.intent.policy==g_attack_timing_policy22.load()&&NativeTimingSelected22()&&
            FistReadOwner101(emptyH12)&&emptyH12.empty&&emptyH12.request==scope.intent.request&&
            reinterpret_cast<std::uintptr_t>(emptyH12.player)==scope.intent.player&&
            NativeResolve104(emptyH12.player,sameH12)&&reinterpret_cast<std::uintptr_t>(sameH12.actor)==scope.intent.actor&&
            reinterpret_cast<std::uintptr_t>(sameH12.state)==scope.intent.state&&sameH12.epoch==scope.intent.epoch;
        (void)kcd2vr::fist_action_h12::finish(b,fistTokenH12,unchangedH12);
    }
    if(costScope54.value.valid()){
        NativeOwner104 same54{};void* helper54=nullptr;void* row54=nullptr;void* actor54=nullptr;
        const bool exact54=started&&NativeTimingSelected22()&&
            nativeCost54.key.recovery==kcd2vr::contact176::recoveryEpoch55c1.load()&&
            nativeCost54.key.request==g_weapon_request_epoch6694.load()&&
            kcd2vr::solver_rpg_h29::costOwned(nativeCost54.causal54,frik_pose_probe::GeometryIntentOwnerH30,frik_pose_probe::GeometrySolverCostOwnerValidH28)&&
            NativeResolve104(reinterpret_cast<void*>(nativeCost54.key.player),same54)&&reinterpret_cast<std::uintptr_t>(same54.actor)==nativeCost54.actor&&
            reinterpret_cast<std::uintptr_t>(same54.state)==nativeCost54.state&&same54.epoch==nativeCost54.key.body&&
            Read(static_cast<unsigned char*>(action)+0x70,actor54)&&reinterpret_cast<std::uintptr_t>(actor54)==nativeCost54.actor&&
            Read(static_cast<unsigned char*>(action)+0xA0,helper54)&&reinterpret_cast<std::uintptr_t>(helper54)==nativeCost54.helper&&
            Read(static_cast<unsigned char*>(action)+0x58,row54)&&reinterpret_cast<std::uintptr_t>(row54)==nativeCost54.row;
        const bool credited54=costScope54.finish(exact54,GetTickCount64());
        if(diagnostics::combat_capture::active(GetTickCount64())){char text54[448]{};std::snprintf(text54,sizeof(text54),
            "C54 NATIVE_COST gesture=%llu action=%p helper=%p row=%p nativeStart=%d sameOwner=%d preparation=%u inputPrep=%d strike=%u invalid=%d complete=%d deltaOverride=NONE stage=START_RETURN finalCredit=AFTER_BOTH_START_AND_DISPATCH",
            (unsigned long long)nativeCost54.key.gesture,action,helper54,row54,int(started),int(exact54),
            costScope54.value.raised,int(costScope54.value.inputProof54),costScope54.value.struck,int(costScope54.value.invalid),int(credited54));diagnostic_trace::Write(text54);}
    }
    if(started&&kcd2vr::motion59c4::selected.load()&&PhysVft6698(action,0x3A61C58)){
        void* actor=nullptr;void* owner=nullptr;void* row=nullptr;void* type=nullptr;int id=-1;NativeOwner104 exact{};
        if(Read(static_cast<unsigned char*>(action)+0x70,actor)&&PhysVft6698(actor,0x3A80610)&&
           Read(static_cast<unsigned char*>(actor)+0x1F8,owner)&&NativeResolve104(owner,exact)&&exact.actor==actor&&
           Read(static_cast<unsigned char*>(action)+0x58,row)&&row&&Read(static_cast<unsigned char*>(row)+0x148,type)&&type&&
           Read(static_cast<unsigned char*>(type)+8,id)&&(id==0||id==1)){
            ++kcd2vr::motion59c4::nativeStarts;
            diagnostic_trace::Write("R59C4 MOTION_OWNED unexpectedOrdinaryStart=1 source=UNRESOLVED previewAcceptance=FAILED");
        }
    }
    if(match)CaptureMotionClock50(action,scope,started);
    return started;
}
// R59.16C10R17 NPC REACTION TIME (Miki: "we need only adapt the enemies' reactions to our speed"). In F4 press 1/3 the hidden
// native attack starts at the VR commit and natively reaches its hit ~0.43 s later (D4D950: hit = (Th - e0s) / RPG speed,
// Henry ~1.16) while the real blade arrives 70-120 ms after the commit. Every NPC defence window (B275F8 at the start, the
// 914204 block offer at phase 4) and the NPC's block alignment (D4D4FC / follower 9E3D3D) are sized from that native
// time-to-hit. Once per admitted VR attack, at the exact D4DA56 call inside D4D950 (params already copied to TimeCop+0x18..),
// the RPG speed handed back is raised so the native hit falls VrNativeHitTime seconds after the start - never slower than the
// native speed, at most 4x. The contact catch-up (contactPlan59c3) then pulls the hit onto the real blade, and the NPC's
// aligned block follows it every frame. 0 = native timing.
static std::atomic<float> g_native_hit_time59c17{.30f};
static std::atomic<std::uint64_t> g_native_hit_applied59c17{0};static std::atomic<float> g_native_hit_native59c17{0},g_native_hit_scaled59c17{0};
static std::atomic<float> g_native_hit_before59c17{0},g_native_hit_after59c17{0};
static float NativeHitTimeSpeed59c17(AttackTimingScope22* scope,void* component,std::uintptr_t caller,float native) noexcept {
    // H11: verified ordinary unarmed actions use the same native coordinator;
    // their old timing branch was unreachable (types5/6/7 rejected above).
    const bool punchH11=scope&&scope->intent.kind==at22::Kind::Punch;
    const float target=punchH11?.12f:g_native_hit_time59c17.load(std::memory_order_relaxed);
    if(!scope||scope->used||component!=scope->speedComponent||caller!=WhBase()+0xD4DA56||!(target>=.10f&&target<=1.f)||
       (!punchH11&&!CombatSynchronized59c10r9())||scope->policy!=g_attack_timing_policy22.load()||
       scope->epoch!=g_pose_epoch6689.load()||
       !(at22::fresh(scope->tick,GetTickCount64())||(punchH11&&FistPreparedStartH31(scope->intent,GetTickCount64())))||
       !std::isfinite(native)||native<.1f||native>8.f)return -1.f;
    void* clock=nullptr;void* back=nullptr;unsigned char ignoreSpeed=1,resetElapsed=0;float th=0,e0s=0;
    if(!scope->actor||!Read(static_cast<unsigned char*>(scope->actor)+0x3C0,clock)||!clock||
       !Read(static_cast<unsigned char*>(clock)+8,back)||back!=scope->actor||
       !Read(static_cast<unsigned char*>(clock)+0x2C,ignoreSpeed)||ignoreSpeed||   // D4DA56: [+0x2C] set = the speed is ignored (1.0)
       !Read(static_cast<unsigned char*>(clock)+0x2E,resetElapsed)||!Read(static_cast<unsigned char*>(clock)+0x1C,th)||
       (!resetElapsed&&!Read(static_cast<unsigned char*>(clock)+0x50,e0s)))return -1.f;
    const float span=th-(resetElapsed?0.f:e0s);
    if(!std::isfinite(span)||span<.05f||span>5.f)return -1.f;
    const float scaled=std::clamp(span/target,native,native*4.f);
    scope->used=true;g_native_hit_native59c17.store(native,std::memory_order_relaxed);g_native_hit_scaled59c17.store(scaled,std::memory_order_relaxed);
    g_native_hit_before59c17.store(span/native,std::memory_order_relaxed);g_native_hit_after59c17.store(span/scaled,std::memory_order_relaxed);
    g_native_hit_applied59c17.fetch_add(1,std::memory_order_relaxed);
    return scaled;
}
static float __fastcall NativeAttackSpeed22(void* component) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const float native=g_attack_speed22?g_attack_speed22(component):1.f;
    auto* scope=g_attack_timing_scope22;
    {const float eta=NativeHitTimeSpeed59c17(scope,component,caller,native);if(eta>0)return eta;} // R59.16C10R17
    if(!scope||scope->clock50||scope->used||component!=scope->speedComponent||caller!=WhBase()+0xD4DA56||
       !NativeTimingMode44c1()||scope->policy!=g_attack_timing_policy22.load()||
       scope->epoch!=g_pose_epoch6689.load()||!at22::fresh(scope->tick,GetTickCount64()))return native;
    scope->used=true;const float scaled=at22::apply(native,scope->scale);
    g_attack_timing_native22=native;g_attack_timing_result22=scaled;++g_attack_timing_applied22;
    return scaled; // original coordinator owns ALL phase timers and animation updates
}
static bool ArmAttackTiming178r22c2() noexcept {
    const auto b=WhBase();if(!b||!retail_build112::Ready(b))return false;
    if(g_attack_timing_contract22.load()==b)return true;
    const unsigned char start[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x57,0x48,0x83,0xEC,0x40};
    const unsigned char speed[]{0x48,0x89,0x5C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57,0x48,0x83,0xEC,0x20};
    unsigned char bytes[15]{};std::uintptr_t entry=0;
    if(!Read(reinterpret_cast<void*>(b+0xB27308),bytes)||std::memcmp(bytes,start,sizeof(start))||
       !Read(reinterpret_cast<void*>(b+0xD4DD2C),bytes)||std::memcmp(bytes,speed,sizeof(speed))||
       !Read(reinterpret_cast<void*>(b+0x3A61C58+0x1C8),entry)||entry!=b+0xB27308)return false;
    DetourModKit::hook::InlineRequest speedReq{"kcd2vr_v66178r22c2_attack_speed",DetourModKit::hook::Target{DetourModKit::Address{b+0xD4DD2C}},DetourModKit::hook::Options{}};
    auto s=DetourModKit::hook::inline_at(std::move(speedReq),&NativeAttackSpeed22);if(!s)return false;
    g_attack_speed_hook22=std::move(*s);g_attack_speed22=g_attack_speed_hook22->original<AttackSpeed22>();
    if(!g_attack_speed22||!g_attack_speed_hook22->enable()){g_attack_speed_hook22.reset();g_attack_speed22=nullptr;return false;}
    DetourModKit::hook::InlineRequest startReq{"kcd2vr_v66178r22c2_attack_start",DetourModKit::hook::Target{DetourModKit::Address{b+0xB27308}},DetourModKit::hook::Options{}};
    auto a=DetourModKit::hook::inline_at(std::move(startReq),&NativeAttackStart22);
    if(a){g_attack_start_hook22=std::move(*a);g_attack_start22=g_attack_start_hook22->original<AttackStart22>();}
    if(!g_attack_start22||!g_attack_start_hook22->enable()){
        g_attack_start_hook22.reset();g_attack_start22=nullptr;g_attack_speed_hook22.reset();g_attack_speed22=nullptr;return false;
    }
    g_attack_timing_contract22.store(b,std::memory_order_release);
    diagnostic_trace::Write(ArmMainActionFactory54()?"C54 COST_ACTION_FACTORY ready=1 scope=CONSTRUCTOR_GENERATION_OWNED_RELEASE":"C54 COST_ACTION_FACTORY ready=0 credit=DISABLED nativeInput=UNCHANGED");
    diagnostic_trace::Write(ArmMotionClock50()?"V66.178R50C2 MOTION_CLOCK_HOOK ready=1 scope=ALL_F4_ORDINARY_MELEE":"V66.178R50C2 MOTION_CLOCK_HOOK ready=0 nativeClock=UNCHANGED");return true;
}
static void ShutdownAttackTiming178r22c2() noexcept {
    kcd2vr::swing_effort59c14::actionContractBase54=0;kcd2vr::swing_effort59c14::actionRegistry54.invalidate();
    g_main_action_factory_hook54.reset();g_main_action_factory54=nullptr;
    ShutdownMotionClock50();NativeTimingCancel178r22c2();g_attack_timing_contract22=0;
    g_attack_start_hook22.reset();g_attack_start22=nullptr;g_attack_speed_hook22.reset();g_attack_speed22=nullptr;
}
static void ServiceAttackTiming178r22c2() noexcept {
    AdmissionSummary51();
    MotionClockSummary50();
    if(!NativeTimingMode44c1()||!diagnostics::capture168::any(GetTickCount64()))return;
    char line[300]{};std::snprintf(line,sizeof(line),
        "V66.178R22C2 ATTACK_TIMING published=%llu matched=%llu applied=%llu lastNative=%.3f lastRequested=%.3f scope=HENRY_ORDINARY_START coordinator=NATIVE acceptedHit=UNCLAIMED",
        (unsigned long long)g_attack_timing_published22.load(),(unsigned long long)g_attack_timing_matched22.load(),
        (unsigned long long)g_attack_timing_applied22.load(),double(g_attack_timing_native22.load()),double(g_attack_timing_result22.load()));
    diagnostic_trace::Write(line);
    char r17[300]{};std::snprintf(r17,sizeof(r17),
        "R59.16C10R17 NPC_REACTION_TIME targetHitS=%.2f applied=%llu lastRpgSpeed=%.3f lastScaledSpeed=%.3f lastNativeHitS=%.3f lastNewHitS=%.3f catchUp=contactPlan59c3(to hit)",
        double(g_native_hit_time59c17.load()),(unsigned long long)g_native_hit_applied59c17.load(),double(g_native_hit_native59c17.load()),
        double(g_native_hit_scaled59c17.load()),double(g_native_hit_before59c17.load()),double(g_native_hit_after59c17.load()));
    diagnostic_trace::Write(r17);
}
