#include "../subsystems/throw_drop/legacy_layout_state.inl" // M2_OWNED_SOURCE
// V66.178 F4 rung 2 VR combat core: native rhythm state + strike tuning (INI).
static std::atomic<bool> g_native_busy178{false};
static std::atomic<std::uint64_t> g_native_busy_tick178{0};
// V66.178R4: last observation outcome + identity (a failed read invalidates; epoch-bound).
static std::atomic<bool> g_native_state_ok178{false};
static std::atomic<std::uint64_t> g_native_state_epoch178{0};
static std::atomic<int> g_native_phase178{-1},g_native_type178{-1};
static std::atomic<bool> g_strike_block_active178{true};
#include "../subsystems/physics/support_fallback_state.inl" // M2_OWNED_SOURCE
#include "../subsystems/vrik/wrist_policy_state.inl" // M2_OWNED_SOURCE
static std::atomic<float> g_strike_reference_j59c9{75.f}; // R59.16C9C: joules of a "normal full strike" = power 1.0 (INI VrStrikeReferenceJ)
#include "../subsystems/vrik/head_palm_policy_state.inl" // M2_OWNED_SOURCE
#include "../subsystems/physics/support_source_state.inl" // M2_OWNED_SOURCE
static std::atomic<float> g_strike_min_scale178{.35f};
#include "../subsystems/physics/weapon_feel_state.inl" // M2_OWNED_SOURCE
static std::atomic<bool> g_intent_pi59c16{false}; // R59.16C10R16: F4 press 1 VR INTENT, producer view (set by ApplyUserMode175)
#include "../subsystems/physics/support_axis_state.inl" // M2_OWNED_SOURCE
static std::atomic<unsigned> g_strike_min_interval178{0};
// V66.98 exact retail bridge. Included in frik_pose_probe's private namespace.
// Native getters/commands execute on CCryAction's outer frame callback only.
// Pose callbacks perform bounded shared world queries and transactional publication.
namespace ph = kcd2vr::physical;
namespace pm = kcd2vr::pose_core;
static ph::Pose PhysPose6698(const QuatT28& q) noexcept {return {{q.q[0],q.q[1],q.q[2],q.q[3]},{q.t[0],q.t[1],q.t[2]}};}
static QuatT28 PhysQuat6698(ph::Pose p) noexcept {return {{float(p.q.x),float(p.q.y),float(p.q.z),float(p.q.w)},{float(p.p.x),float(p.p.y),float(p.p.z)}};}
static ph::Pose PhysWorld6698(const float* q) noexcept {return {{q[0],q[1],q[2],q[3]},{q[4],q[5],q[6]}};}
static ph::Pose PhysModel6698(const float* q,const float* model) noexcept {
    auto p=ph::compose(ph::inverse(PhysWorld6698(model)),PhysWorld6698(q));p.p=p.p*(1.0/model[7]);return p;
}
static bool PhysVft6698(void* p,std::uintptr_t rva) noexcept {std::uintptr_t v=0;return p&&Read(p,v)&&v==WhBase()+rva;}
// R134: quest/tutorial copies of weapons are item ALIASES (log 7 Nov 02:27:42 line 33886: Henry's right hand holds
// "alias_prepadeni_longSword000357", census class=-1, no V66.98 ITEM row, MELEE_FLOW key=FFFF.. invalidMask=2 for the
// whole training fight). Their definition (item+0x48) is S_ItemClassAlias (RTTI vtable 0x3BAEBF0), not
// S_MeleeWeaponItemClass 0x3A4E390 / S_MissileWeaponItemClass 0x3A4D890. Every S_ItemClassAlias virtual is
// `mov rcx,[rcx+0x68]; mov rax,[rcx]; jmp [rax+N]` (e.g. WHGame+0x2A46A30), so +0x68 is the source item class.
// Returns the weapon definition that owns class/weight/requirements (the item itself, or the alias source), else null.
static void* PhysWeaponDefinitionR134(void* def) noexcept {
    for(int hop=0;hop<3&&def;++hop){
        // R135: + S_WeaponItemClass 0x3A4E620 itself (same +0xE8 weapon class as melee/missile: its vslot +0x50
        // 0x474A20 = `call 0x476358 (mov rax,[rcx+0xE8]); mov eax,[rax+8]`).
        if(PhysVft6698(def,0x3A4E390)||PhysVft6698(def,0x3A4D890)||PhysVft6698(def,0x3A4E620))return def;
        void* source=nullptr;
        if(!PhysVft6698(def,0x3BAEBF0)||!Read(static_cast<unsigned char*>(def)+0x68,source)||source==def)return nullptr;
        def=source;
    }
    return nullptr;
}
static bool PhysPutFloat6698(void* p,float v) noexcept {
    __try {std::memcpy(p,&v,4);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct PhysEquipmentItem6698 {
    ph::Profile profile{};void* item=nullptr;void* entity=nullptr;float bounds[6]{};
    void* worldLink54=nullptr; // observation only; a stored native item need not have a world model
    int set=-1,slot126=-1;bool boundsValid=false,held=false,dropUnitT1=false;
    std::uint64_t guardFaceRetryRequestH24=0;unsigned guardFaceRetryCountH24=0;bool guardFaceWasHeldH24=false;
    bool slotValid34[2]{};std::array<ph::Pose,2> slot34{};std::uint64_t slotRetry34=0; // C34b: the weapon model's own hand helpers slt_0 / slt_1 (model-local metres)
    std::uint32_t defVftR135=0;unsigned char defSourceR135=0;bool weightDefaultR136=false; // R135: definition vtable rva; 0 weapon def, 1 alias source. R136: default class weight used
};
struct PhysEquipment6698 {
    std::array<PhysEquipmentItem6698,8> items{};int count=0,selected=-1,heldIndex[2]{-1,-1};
    void* player=nullptr;void* manager=nullptr;std::uint64_t request=0,tick=0,epoch=0,handKey[2]{},leftNativeMembershipH36=0;
    bool valid=false,handKnown[2]{};
};
struct PhysSocket101 {std::uint64_t key=0,generation=0,tick=0,callback=0;void* character=nullptr;int joint=-1;QuatT28 relative{};float scale[3]{1,1,1};bool valid=false;};
struct PhysCorrelation126 {pm::Stamp stamp{};PhysSocket101 socket{};};
struct PhysGrip6698 {
    std::uint64_t key=0,generation=0,lastSample=0;ph::Pose hand[2],candidate[2],mount{},nativeWeapon{};
    pm::Stamp nativeStamp{},mountStamp126{};PhysSocket101 nativeSocket126{},captureBinding{};
    unsigned stable[2]{};bool ready[2]{},mountReady=false;float geometryScale[3]{1,1,1};
    // Preserve learned melee geometry across the native held-key -> stowed
    // socket -> hand socket transition. Reuse only the identical wrist binding;
    // this is not a pose, socket owner, or freshness cache.
    PhysSocket101 heldBindingR27{};ph::Pose heldHandR27[2]{};
    std::uint64_t heldBindingEpochR27=0;bool heldReadyR27[2]{};float heldScaleR27[3]{1,1,1};
    // R121: a crossbow grip reused from its last draw is owned at once and checked again by the
    // normal six-sample capture; a different settled grip replaces it (no latch, new body epoch = fresh learn).
    bool recheckR121[2]{};unsigned recheckSamplesR121[2]{};
    unsigned restSkipsR123[2]{}; // R123: crossbow learn samples skipped because they sat on the bind/rest relation
    unsigned restWindowR140[2]{}; // R140: bow/crossbow grip learned ON the rest relation: samples left in which a real hold replaces it
};
#include "../subsystems/physics/support_slot_state.inl" // M2_OWNED_SOURCE
static double m34angle(pm::Quat q) noexcept {const double w=std::clamp(std::abs(q.w),0.0,1.0);return 2*std::acos(w);}
// Qualified from THIS held bow's named rest string, optionally refined by a full draw.
// Pure copied geometry; no borrowed native pointer is dereferenced by consumers.
#include "../subsystems/ranged_reload/bow_geometry_state.inl" // M2_OWNED_SOURCE
struct PhysGripBank6698 {std::array<PhysGrip6698,8> grip{};};
struct PhysFrame6698 {
    ph::Input input{};ph::Output output{};std::array<ph::Profile,2> profiles{};
    std::array<PhysCorrelation126,2> correlation126{};
    double publishWorldDeltaM122=0,publishWorldDeltaRad122=0;
    void* player=nullptr;void* manager=nullptr;std::uint64_t request=0;float world[8]{};
    ph::Pose previousWeapon177{};
    std::uint64_t previousSample177=0,previousTick177=0,previousNs59c6=0;
    std::uint64_t holsterGripKeyR15[2]{},holsterGripGenerationR15[2]{}; // completed gesture priority only
    bool retainedSupport55[2]{}; // voice/utility veto only, never a pose/contact owner
    std::uint64_t trackingEpochT23=0,characterT23=0,publicationT23=0; // exact completed producer identities
};
static void PublishEnvironmentFx178r22c1(const PhysFrame6698&) noexcept;
// Form the interval only from the previous COMPLETED publication. Nothing here
// retains borrowed native requests, grants a native attack window or refreshes age.
static void PhysInterval177(const PhysFrame6698& before,PhysFrame6698& now,std::uint64_t ready) noexcept {
    now.previousSample177=now.previousTick177=now.previousNs59c6=0;
    if(!now.input.physicalCombat177||!before.input.physicalCombat177||!ready||before.input.sample!=ready||
       !before.input.allowed||!before.input.attacks||!now.input.allowed||!now.input.attacks||
       !before.input.tracked[0]||!before.input.tracked[1]||!now.input.tracked[0]||!now.input.tracked[1]||
       !before.output.active[0]||!before.output.owned[0]||!now.output.active[0]||!now.output.owned[0]||
       !before.output.contactKnown[0]||!now.output.contactKnown[0]||
       !before.player||before.player!=now.player||before.manager!=now.manager||before.request!=now.request||
       before.input.bodyEpoch!=now.input.bodyEpoch||before.input.epoch!=now.input.epoch||
       !before.input.tick||now.input.tick<before.input.tick||
       kcd2vr::motion_time::elapsed(before.input.tick,now.input.tick,before.input.timeNs,now.input.timeNs)<=0||
       kcd2vr::motion_time::elapsed(before.input.tick,now.input.tick,before.input.timeNs,now.input.timeNs)>.125||
       now.input.sample<=before.input.sample|| // counter gaps are not elapsed time
       std::abs(double(before.world[7])-now.world[7])>1e-5)return;
    const auto& a=before.profiles[0];const auto& b=now.profiles[0];
    const auto& x=before.correlation126[0];const auto& y=now.correlation126[0];
    if(!a.key||a.key!=b.key||a.generation!=b.generation||a.entity!=b.entity||a.classId!=b.classId||a.side!=0||b.side!=0||
       !a.primary||!b.primary||!a.melee()||!b.melee()||
       !x.socket.valid||!y.socket.valid||x.socket.character!=y.socket.character||x.socket.joint!=y.socket.joint||
       x.socket.key!=a.key||y.socket.key!=b.key||x.socket.generation!=a.generation||y.socket.generation!=b.generation||
       x.stamp.epoch!=before.input.bodyEpoch||y.stamp.epoch!=now.input.bodyEpoch||
       x.stamp.sample!=before.input.sample||y.stamp.sample!=now.input.sample||
       x.stamp.tick!=before.input.tick||y.stamp.tick!=now.input.tick||
       x.stamp.actor!=reinterpret_cast<std::uintptr_t>(x.socket.character)||
       y.stamp.actor!=reinterpret_cast<std::uintptr_t>(y.socket.character)||
       std::memcmp(&x.socket.relative,&y.socket.relative,sizeof(x.socket.relative))||
       std::memcmp(x.socket.scale,y.socket.scale,sizeof(x.socket.scale))||
       !ph::finite(before.output.weapons[0])||!ph::finite(now.output.weapons[0]))return;
    now.previousWeapon177=before.output.weapons[0];
    now.previousSample177=before.input.sample;now.previousTick177=before.input.tick;now.previousNs59c6=before.input.timeNs;
}
static bool NativePhysicalAction117(void*,const ph::Command&,const PhysFrame6698&,bool) noexcept;
static bool NativeFinalComboDeferredD72(const ph::Command&,const PhysFrame6698&,std::uint64_t) noexcept;
static bool NativeFinalComboPreparedD72(const ph::Command&,const PhysFrame6698&,std::uint64_t) noexcept;
static kcd2vr::final_combo_d72::QueueD82 NativeFinalContactCommandD82(ph::Command&,const PhysFrame6698&,std::uint64_t) noexcept;
static std::atomic<std::uint64_t> g_zone_feints57{0},g_cocked_prepares57{0};
static std::atomic<std::uint64_t> g_wall_blocked58{0},g_wall_slid58{0},g_wall_backout58{0},g_wall_stuck58{0},g_wall_scrape58{0};static std::uint64_t g_scrape_last58[2]{}; // V66.178R58 wall feel
static bool NativeZoneOnly57(void* player,const ph::Command&,const PhysFrame6698&) noexcept; // native_combat104.inl // V66.178R57: zone re-announcements of a held preparation
static void NativeDirectPrepareGuardHandoff19c4r2(const ph::Command&,const PhysFrame6698&,std::uint64_t) noexcept;
static bool NativePhysicalGuardHeld117(void*) noexcept;
static bool NativePreparedOwnerMatches117(void*) noexcept;
struct PhysSocketBank101 {std::array<PhysSocket101,8> sockets{};};
static PhysSocketBank101 g_phys_socket_bank101;
// Sparse producer counters: no per-frame I/O and no native polling from worker.
static std::atomic<std::uint64_t> g_socket_capture_counts101[5]{}; // eligible, stale packet, busy, matched, no socket
static std::atomic<std::uint64_t> g_socket_callback101{0}; // exact attachment-callback generation; topology is comparable only within one callback
static std::atomic<std::uint64_t> g_physical_gates101R2[10]{}; // see worker labels; observational only


static kcd2vr::core::SnapshotMailbox<PhysSocketBank101> g_phys_sockets101;
static std::atomic<std::uint32_t> g_phys_relevant_id101[8]{};
static bool PhysSocketContract101(std::uintptr_t base) noexcept {
    if(!base)return false;
    constexpr std::uintptr_t slot[]{0x60,0x80,0xC8};
    constexpr std::uintptr_t target[]{0x1A72860,0x1069A88,0x1AAF3C0};
    for(int i=0;i<3;++i){std::uintptr_t got=0;if(!Read(reinterpret_cast<void*>(base+0x3A657C0+slot[i]),got)||got!=base+target[i])return false;}
    const unsigned char expected[16]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x78,0x10,0x55,0x48,0x8D,0x68,0xA1};
    unsigned char actual[16]{};return Read(reinterpret_cast<void*>(base+0x1069A88),actual)&&!std::memcmp(actual,expected,16);
}
static void PhysRememberSocket101(const PhysEquipmentItem6698& item,void* self,void* attachment,std::uint64_t now) noexcept {
    const auto callback=g_socket_callback101.fetch_add(1,std::memory_order_acq_rel)+1;
    // Exact native CAttachmentBONE project routine (+1069A88): signed joint at
    // +78, character from manager+70, current joint * default-relative +9C.
    // No virtual SDK calls and no use of stale model/world attachment +30.
    if(!PhysVft6698(attachment,0x3A657C0))return;
    PhysSocket101 x{};void* manager=nullptr;std::int16_t joint=-1;unsigned char deferred=0;QuatT28 again{};
    if(!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||!manager||!Read(static_cast<unsigned char*>(manager)+0x70,x.character)||
       !PhysVft6698(x.character,0x40602F8)||!Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x7A,deferred)||deferred||!Read(static_cast<unsigned char*>(attachment)+0x9C,x.relative))return;
    double norm=0;for(float v:x.relative.q){if(!std::isfinite(v))return;norm+=double(v)*v;}
    if(norm<.98||norm>1.02)return;for(float v:x.relative.t)if(!std::isfinite(v)||std::fabs(v)>2)return;
    for(int i=0;i<3;++i)if(!Read(static_cast<unsigned char*>(self)+0x0C+i*4,x.scale[i])||!std::isfinite(x.scale[i])||x.scale[i]<=0||x.scale[i]>10)return;
    std::int16_t after=-1;if(!Read(static_cast<unsigned char*>(attachment)+0x78,after)||after!=joint||!Read(static_cast<unsigned char*>(attachment)+0x9C,again)||std::memcmp(&again,&x.relative,sizeof(again)))return;
    x.key=item.profile.key;x.generation=item.profile.generation;x.tick=now;x.callback=callback;x.joint=joint;x.valid=true;
    auto* dest=&g_phys_socket_bank101.sockets[item.profile.side];
    for(auto& s:g_phys_socket_bank101.sockets)if(!s.valid||s.key==x.key){dest=&s;break;}
    *dest=x;g_phys_sockets101.publish(g_phys_socket_bank101);
}
static kcd2vr::core::SnapshotMailbox<PhysEquipment6698> g_phys_equipment6698;
static kcd2vr::core::SnapshotMailbox<PhysGripBank6698> g_phys_grips6698;
static kcd2vr::core::SnapshotMailbox<PhysFrame6698> g_phys_frame6698;
static PhysGripBank6698 g_phys_grip_bank6698; // only attachment producer under try-lock
static std::atomic_flag g_phys_attachment_lock6698=ATOMIC_FLAG_INIT;
struct PhysTryLock6698 {bool owned=!g_phys_attachment_lock6698.test_and_set(std::memory_order_acquire);~PhysTryLock6698(){if(owned)g_phys_attachment_lock6698.clear(std::memory_order_release);}};
static std::array<kcd2vr::haptics101::Cue,2> g_phys_cues101{};
static ph::State g_phys_state6698; // only pose producer, already serialized by pose mutex
static void PhysRetireMotion30c1(const char* reason="UNSPECIFIED",unsigned sourceLine=0) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    kcd2vr::diagnostics::hit_chain_c54r3::recorder.retire(g_phys_state6698,reason,sourceLine,now);
    if(kcd2vr::diagnostics::vrik_h29::active(now)){kcd2vr::diagnostics::vrik_h29::Record r{};r.tick=now;r.sample=g_phys_state6698.sample;r.epoch=g_phys_state6698.bodyEpoch;r.line=int(sourceLine);std::strncpy(r.stage,reason,sizeof(r.stage)-1);kcd2vr::diagnostics::vrik_h29::push(r);}
    g_phys_state6698={};
}
static std::uint64_t g_latchedKey33=0;static bool g_latchedNative33=false; // V66.178R33/R36 engagement owner latch (consumer side)
static std::array<ph::HolsterState,8> g_phys_holsters6698;
static std::atomic<bool> g_phys_armed6698{false},g_phys_input6698{false};
static std::atomic<bool> g_phys_fault6698{false};
static std::atomic<bool> g_phys_abort6698{false};
static std::atomic<std::uint64_t> g_phys_input_tick6698{0};
static std::atomic<unsigned> g_phys_pending6698{0};
static std::atomic<void*> g_phys_pending_self164[2]{}; // identity-only early filter; never write authority
static std::atomic<std::uint64_t> g_phys_ready_sample6698{0};

// R28 rung 7: copied receipts under the existing pose mutex. No retained pose
// can become a motor seed; this permits only bounded, real extra angular coverage.
static kcd2vr::native_rejoin_r28::Tracker<PhysSocket101> g_native_rejoin_r28;
static kcd2vr::native_rejoin_r28::Context PhysRejoinContextR28(void* character,
        std::uint64_t body,std::uint64_t tracking) noexcept {
    return {kcd2vr::smooth_lab::NativeRejoinPolicyR28(),reinterpret_cast<std::uintptr_t>(character),body,tracking};
}
static bool PhysRejoinCorrelationR28(const PhysCorrelation126& c,const ph::Profile& p,
        const ph::Input& in,std::uint64_t actor) noexcept {
    return c.socket.valid&&c.socket.key==p.key&&c.socket.generation==p.generation&&
        reinterpret_cast<std::uintptr_t>(c.socket.character)==actor&&
        c.stamp.actor==actor&&c.stamp.epoch==in.bodyEpoch&&c.stamp.sample==in.sample&&c.stamp.tick==in.tick;
}
static void PhysObserveNativeRejoinR28(bool nativeBody,bool eligible,void* character,
        std::uint64_t body,const TrackingSnapshot& tracking,std::uint64_t now) noexcept {
    auto context=PhysRejoinContextR28(character,body,tracking.source_epoch);
    if(!context.policy){g_native_rejoin_r28.clear();return;}
    if(!eligible||!tracking.allowed||!tracking.head_valid||tracking.head_held||
       !kcd2vr::native_rejoin_r28::fresh(tracking.tick,now))context={};
    std::array<kcd2vr::native_rejoin_r28::Binding<PhysSocket101>,2> bindings{};
    if(g_native_rejoin_r28.entering(context,nativeBody)){
        const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
        PhysFrame6698 frame{};
        if(ready&&ready==g_phys_state6698.sample&&g_phys_frame6698.read(frame)&&
           ready==g_phys_ready_sample6698.load(std::memory_order_acquire)&&frame.input.sample==ready&&
           frame.characterT23==context.actor&&frame.input.bodyEpoch==context.bodyEpoch&&
           frame.trackingEpochT23==context.trackingEpoch&&frame.publicationT23&&frame.input.allowed&&
           kcd2vr::native_rejoin_r28::fresh(frame.input.tick,now)){
            for(int side=0;side<2;++side){
                const bool current=side?tracking.left_valid&&!tracking.left_held:tracking.right_valid&&!tracking.right_held;
                const auto& p=frame.profiles[side];const auto& c=frame.correlation126[side];
                if(current&&frame.input.tracked[side]&&frame.output.active[side]&&frame.output.owned[side]&&
                   !frame.output.support[side]&&PhysRejoinCorrelationR28(c,p,frame.input,context.actor))
                    bindings[side]={p,c.socket,true};
            }
        }
    }
    g_native_rejoin_r28.observe(context,nativeBody,bindings);
}
static void PhysClaimNativeRejoinR28(ph::Input& in,const ph::State& state,
        const std::array<ph::Profile,2>& profiles,const std::array<PhysCorrelation126,2>& correlation,
        void* character,const TrackingSnapshot& tracking,std::uint64_t now) noexcept {
    const auto context=PhysRejoinContextR28(character,in.bodyEpoch,tracking.source_epoch);
    if(!context.policy){g_native_rejoin_r28.clear();return;}
    for(int side=0;side<2;++side){
        const bool current=side?tracking.left_valid&&!tracking.left_held:tracking.right_valid&&!tracking.right_held;
        const bool eligible=!state.weapon[side].ready&&in.allowed&&in.contact&&!in.contactSuspended59c1&&
            !in.contactFaultH30&&in.controllerAnchor119&&in.tracked[side]&&in.intentTracked[side]&&
            tracking.head_valid&&!tracking.head_held&&current&&
            (!in.requirePrimaryGrip114||in.grip[side]>=.25)&&
            PhysRejoinCorrelationR28(correlation[side],profiles[side],in,context.actor);
        in.nativeRejoinR28[side]=g_native_rejoin_r28.claim(context,side,profiles[side],correlation[side].socket,
            in.sample,in.tick,now,eligible);
    }
}

struct PhysPlacement6698 {
    // Keep the historical first four fields/order for bridge/source compatibility.
    void* self=nullptr;void* attachment=nullptr;std::uint64_t key=0;bool changed=false;
    void* entity=nullptr;void* manager=nullptr;std::uint32_t entityId=0;int side=-1;
    std::uint64_t generation=0,verifiedAt=0,placedAt=0,sourceTick146=0,epoch146=0,request146=0;unsigned verifyFails=0;float wanted[8]{},localWanted[8]{};
    std::uint64_t visualPublicationR24=0,visualSampleR24=0,visualTrackingR24=0,visualCharacterR24=0; // copied completed pose identity; never refreshes source age
    bool initialPlacementVerifiedD72=false;
};
static std::atomic<std::uint64_t> g_phys_grace135{0},g_phys_grace_expired135{0},g_phys_verify_deferred135{0};
// V66.157: diagnose authority loss instead of hiding it with a longer grace or
// reviving the retired controller writer in full physical modes.  Reasons 1..10
// match PhysReplayLast135 call sites: equipment invalid/stale, attachment read,
// live hand, entity identity, missing/identity/inactive/stale frame, write fail.
static std::atomic<std::uint64_t> g_phys_authority_drop157[11]{};
static std::atomic<std::uint64_t> g_phys_native_restore157{0};
static inline void PhysAuthorityDrop157(unsigned reason) noexcept {
    if(reason<11)g_phys_authority_drop157[reason].fetch_add(1,std::memory_order_relaxed);
    g_phys_native_restore157.fetch_add(1,std::memory_order_relaxed);
}
static std::array<PhysPlacement6698,2> g_phys_placement6698;
#include "../subsystems/throw_drop/allowed_release_history.inl" // M2_OWNED_SOURCE
// Serialized pose producer / outer CCryAction consumer. No pointers in requests.
struct PhysCommands6698 {
    std::array<ph::Command,32> q{};std::atomic<unsigned> head{0},tail{0};
    bool push(ph::Command c) noexcept {unsigned h=head.load(std::memory_order_relaxed),n=(h+1)%q.size();if(n==tail.load(std::memory_order_acquire)){if(c.kind!=ph::CommandKind::Draw&&c.kind!=ph::CommandKind::Holster)g_phys_abort6698=true;return false;}q[h]=c;head.store(n,std::memory_order_release);return true;}
    bool peek(ph::Command& c) noexcept {unsigned t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))return false;c=q[t];return true;}
    bool pop(ph::Command& c) noexcept {unsigned t=tail.load(std::memory_order_relaxed);if(t==head.load(std::memory_order_acquire))return false;c=q[t];tail.store((t+1)%q.size(),std::memory_order_release);return true;}
};
static PhysCommands6698 g_phys_commands6698;
// Called only by the native consumer while its existing owner mutex is held.
// A later deliberate gesture or an enqueued cancellation must not sit behind
// an old final-intent wait and then lend that old intent fresh contact evidence.
static bool PhysFinalQueuedConflictD82(const ph::Command& original,const PhysFrame6698& f,std::uint64_t now) noexcept {
    const auto tail=g_phys_commands6698.tail.load(std::memory_order_relaxed);
    const auto head=g_phys_commands6698.head.load(std::memory_order_acquire);
    for(auto at=tail;at!=head;at=(at+1)%g_phys_commands6698.q.size()){
        const auto& c=g_phys_commands6698.q[at];
        if(c.side!=0||c.key!=original.key||c.generation!=original.generation||c.bodyEpoch!=original.bodyEpoch||
           c.epoch!=original.epoch||c.sample<original.sample||c.sample>f.input.sample||c.tick>now)continue;
        if(c.kind==ph::CommandKind::Abort&&c.gesture59c3==original.gesture59c3)return true;
        if(c.kind==ph::CommandKind::Prepare&&c.gesture59c3&&c.gesture59c3!=original.gesture59c3)return true;
    }
    return false;
}

#include "../subsystems/throw_drop/holster_commit_d48.inl"
struct PhysNativeOwner6698 {
    void* player=nullptr;void* controller=nullptr;void* bow=nullptr;std::uint64_t key=0,epoch=0,request=0;
    std::uint64_t bladeZoneGestureD84=0;int bladeZoneOriginalD84=-1; bool bowHeld=false,attackPrepared=false,heldPrepare57=false,cutFeintUsed62=false;std::uint64_t attackKey=0,dispatchFrame=0,prepareFrame=0,bowBeginFrame=0;float nativeChargeCap=0;std::uint64_t actionNames[5]{};
    ph::CommandKind stroke117=ph::CommandKind::None;int zone117=-1;bool natural117=false;
    std::uint64_t prepareSample117=0,attackGeneration117=0;
    std::uint64_t prepareTickC10R18=0,prepareGestureC10R18=0; // immutable onset of the current ordinary preparation
    std::uint64_t committedGestureFE=0,committedTickFE=0; // D34 FE: the last HYBRID cut whose native commit returned
    bool bowChargeObserved114=false,physicalBow175=false;
    std::uint64_t bowGeneration175=0,bowBeginSample175=0;
};
static bool NativeHybridFeintFE(void* player,const ph::Command& c,const PhysFrame6698& f,PhysNativeOwner6698& n,std::uint64_t now) noexcept; // native_input117.inl (D34 FE)
static void RememberPrepareC10R18(PhysNativeOwner6698& n,const ph::Command& c) noexcept {
    n.bladeZoneGestureD84=0;n.bladeZoneOriginalD84=-1;
    if(c.kick59||!c.gesture59c3){n.prepareTickC10R18=n.prepareGestureC10R18=0;n.cutFeintUsed62=false;return;}
    if(n.prepareGestureC10R18!=c.gesture59c3){n.prepareTickC10R18=c.tick;n.prepareGestureC10R18=c.gesture59c3;n.cutFeintUsed62=false;}
}
// The forward classifier may refine its first few lateral samples into a stab.
// Both use the same native heavy input only with VrThrustAsSwing. Preserve the
// one prepared input and its original onset; never reinterpret an unrelated
// gesture, a held feint, or a configured light-input stab this way.
static std::uint64_t NativePreparedSourceGenerationH26() noexcept;
static bool NativeForwardRefinement19c4r2(const PhysNativeOwner6698& n,const ph::Command& c,
    const PhysFrame6698& f,std::uint64_t now) noexcept {
    const bool heldH26=c.controlledHoldH26&&c.held57&&c.cutFeint62&&!n.cutFeintUsed62&&n.heldPrepare57&&
        n.attackPrepared&&n.natural117&&n.player==f.player&&n.epoch==c.bodyEpoch&&n.request==c.epoch&&
        n.attackKey==c.key&&n.attackGeneration117==c.generation&&n.prepareGestureC10R18==c.gesture59c3&&
        n.stroke117==ph::CommandKind::Slash&&c.stroke117==ph::CommandKind::Slash&&
        c.kind==ph::CommandKind::Zone57&&c.side==0&&!c.kick59&&
        (c.zone117==0||c.zone117==1||c.zone117==2)&&c.zone117!=n.zone117&&
        n.prepareTickC10R18&&c.tick>=n.prepareTickC10R18&&c.tick-n.prepareTickC10R18<=3000&&
        now>=c.tick&&now-c.tick<=100&&c.sample>=n.prepareSample117&&c.sample<=f.input.sample;
    if(heldH26)return true; // NativeZoneOnly57 additionally requires the exact observed original input source.
    const bool cut62=c.cutFeint62&&!n.cutFeintUsed62&&f.input.forwardOnly19&&f.input.coherentMelee119&&
        c.stroke117==ph::CommandKind::Slash&&
        (c.zone117==0||c.zone117==1||c.zone117==2)&&c.zone117!=n.zone117;
    return CombatSynchronized59c10r9()&&!CombatIntent59c16()&&(cut62||g_thrust_as_swing55.load(std::memory_order_relaxed))&&
        c.kind==ph::CommandKind::Zone57&&!c.kick59&&!c.held57&&c.side==0&&
        n.attackPrepared&&!n.heldPrepare57&&n.natural117&&n.stroke117==ph::CommandKind::Slash&&
        (cut62||(!c.cutFeint62&&c.stroke117==ph::CommandKind::Thrust))&&n.player==f.player&&n.epoch==c.bodyEpoch&&n.request==c.epoch&&
        n.attackKey==c.key&&n.attackGeneration117==c.generation&&n.prepareGestureC10R18&&
        c.gesture59c3==n.prepareGestureC10R18&&n.prepareSample117&&c.sample>=n.prepareSample117&&c.sample<=f.input.sample&&
        n.prepareTickC10R18&&c.tick>=n.prepareTickC10R18&&c.tick-n.prepareTickC10R18<=100&&
        now>=c.tick&&now-c.tick<=100;
}
static std::recursive_mutex g_phys_native_mutex6698;
enum class PhysQueuedCommitH8 {Absent,ExactQueued,Busy};
// Synchronize with the existing native consumer before reading plain ring slots.
// try_lock never blocks another game callback; the existing recursive mutex also
// permits a callback already inside that native owner. A Busy result reads no
// slot and grants no ownership, dispatch or renewed clock.
static PhysQueuedCommitH8 PhysQueuedOriginalCommitH8(const kcd2vr::original_contact_h8::Source& original,
        const PhysFrame6698& frame,std::uint64_t now) noexcept {
    std::unique_lock<std::recursive_mutex> lock(g_phys_native_mutex6698,std::try_to_lock);
    if(!lock.owns_lock())return PhysQueuedCommitH8::Busy;
    const auto tail=g_phys_commands6698.tail.load(std::memory_order_relaxed);
    const auto head=g_phys_commands6698.head.load(std::memory_order_acquire);bool found=false;
    for(auto slot=tail;slot!=head;slot=(slot+1)%g_phys_commands6698.q.size()){
        const auto& c=g_phys_commands6698.q[slot];
        if(c.side!=0||c.kick59||c.gesture59c3!=original.gesture||c.key!=original.key||c.generation!=original.generation||
           c.bodyEpoch!=original.body||c.epoch!=original.request||c.recoveryEpochH30!=original.recovery)continue;
        if(c.kind==ph::CommandKind::Abort)return PhysQueuedCommitH8::Absent;
        if((c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)&&!c.held57&&
           c.tick==frame.output.intentQuarantineH8[0].commit&&c.sample<=frame.output.intentQuarantineH8[0].afterSample&&
           now>=c.tick&&now-c.tick<=100)found=true;
    }
    return found&&tail==g_phys_commands6698.tail.load(std::memory_order_acquire)?PhysQueuedCommitH8::ExactQueued:PhysQueuedCommitH8::Absent;
}

static PhysNativeOwner6698 g_phys_native6698; // engine thread only
// Explicit physical charge lifetime; identity only, no borrowed state writes.
// Clear after native cancellation/fire. This also keeps lock contention from
// falling back to native timed charge for a known local physical bow action.
static std::atomic<void*> g_phys_charge_player175{nullptr},g_phys_charge_action175{nullptr};
static PhysFrame6698 g_phys_native_complete175; // accessed under native-owner mutex
static const PhysFrame6698& PhysLatestComplete175() noexcept {
    PhysFrame6698 candidate{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if(ready&&g_phys_frame6698.read(candidate)&&candidate.input.sample==ready&&
       ready==g_phys_ready_sample6698.load(std::memory_order_acquire))g_phys_native_complete175=candidate;
    return g_phys_native_complete175; // original producer times, no age refresh
}

// V66.142: exact, short-lived proof that KCD2VR successfully dispatched the
// RELEASE edge of this physical melee swing. The predictor can run after KCD2
// lowers its transient `attacking` flag, so native-attacking alone is too late.
struct PhysicalStrikeTicket142 {
    std::atomic<std::uint64_t> tick{0}, key{0}, generation{0}, sample{0};
};
static PhysicalStrikeTicket142 g_physical_strike142{};
static bool F4DefenseOwned176(void*) noexcept;
static void PublishPhysicalStrike176(const ph::Command& c,const PhysFrame6698& f) noexcept {
    kcd2vr::contact176::Ticket t{};
    const auto policy=kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel)+1;
    // This records a dispatched release, not accepted attack/damage permission.
    // Reacquiring guard after dispatch cannot erase its identity. The contact
    // consumer separately requires THIS release's admitted current action.
    if(CombatExperiment175Enabled()&&c.side==0&&
       (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust)){
        t.player=reinterpret_cast<std::uintptr_t>(f.player);t.entity=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());
        t.serial=kcd2vr::contact176::nextSerial.fetch_add(1,std::memory_order_relaxed)+1;
        t.gesture59c3=c.gesture59c3;t.recovery59c3=c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load();
        t.policy=policy;t.tick=c.tick;t.sample=c.sample;t.epoch=c.bodyEpoch;t.request=c.epoch;t.key=c.key;t.generation=c.generation;
    }
    // Preserve the legacy sweep gate: defense cannot grant latest-release
    // authority. The separate history records dispatch only; its consumer also
    // requires this release's admitted receiver before delivering contact.
    // The policy increment invalidates old releases even if publication is busy.
    (void)kcd2vr::contact176::released.publish(F4DefenseOwned176(f.player)?kcd2vr::contact176::Ticket{}:t);
    (void)kcd2vr::contact59c3::releases.publish(t);
}
// CB1: what this VR strike physically is, for the native combo finisher gate (physical_finisher_cb1.h).
static kcd2vr::finisher_cb1::ReleaseScope FinisherScopeCB1(ph::CommandKind k,std::uint64_t gesture) noexcept {
    namespace cb=kcd2vr::finisher_cb1;
    return cb::ReleaseScope(k==ph::CommandKind::Slash?cb::Kind::Cut:k==ph::CommandKind::Thrust?cb::Kind::Thrust:
        k==ph::CommandKind::Pommel?cb::Kind::Pommel:cb::Kind::None,gesture);
}
static void PublishPhysicalStrike142(const ph::Command& c) noexcept {
    g_physical_strike142.key.store(c.key,std::memory_order_relaxed);
    g_physical_strike142.generation.store(c.generation,std::memory_order_relaxed);
    g_physical_strike142.sample.store(c.sample,std::memory_order_relaxed);
    g_physical_strike142.tick.store(std::uint64_t(GetTickCount64()),std::memory_order_release);
}
static bool PhysicalStrikeWindow142(std::uint64_t key,std::uint64_t generation,std::uint64_t sample,std::uint64_t now) noexcept {
    const auto tick=g_physical_strike142.tick.load(std::memory_order_acquire);
    if(!tick||now<tick||now-tick>120)return false;
    if(g_physical_strike142.key.load(std::memory_order_relaxed)!=key||
       g_physical_strike142.generation.load(std::memory_order_relaxed)!=generation)return false;
    const auto committed=g_physical_strike142.sample.load(std::memory_order_relaxed);
    return committed&&sample>=committed; // original 120 ms age above, not a sample-count clock
}

using PhysChargeFn6698=std::uint32_t(__fastcall*)(void*,float);
static PhysChargeFn6698 g_phys_charge_original6698=nullptr;
static std::optional<DetourModKit::hook::Hook> g_phys_charge_hook6698;
// V66.110 read-only bridge probe: both KCD2 weapon solvers converge on
// WHGame+0xA7A124 with previous/current collision-bone poses.  This observer
// proves the local-player pivot relationship before any trajectory replacement.
using PhysNativeSweepFn110=void*(__fastcall*)(void*,int,int,const QuatT28*,const QuatT28*,float);
static PhysNativeSweepFn110 g_phys_native_sweep_original110=nullptr;
static std::optional<DetourModKit::hook::Hook> g_phys_native_sweep_hook110;
static std::atomic<void*> g_phys_collision_owner110{nullptr};
static std::atomic<std::uint64_t> g_phys_collision_tick112{0};
static std::atomic<std::uint64_t> g_phys_native_sweep_calls110{0},g_phys_native_sweep_local110{0},g_phys_native_sweep_hits110{0};
static bool NativeTrajectoryPrepare112(std::uintptr_t,void*,int,int,const QuatT28*,const QuatT28*,float,QuatT28&,QuatT28&) noexcept;
static kcd2vr::contact176::Payload PredictionAuthority176(std::uintptr_t,void*,int,int,float) noexcept;
static void ProximityCapture36(const PhysFrame6698&,std::uint64_t) noexcept;
static void PhysFault6698(const char* why) noexcept {
    if(!g_phys_fault6698.exchange(true)){diagnostic_trace::Write(why);g_phys_abort6698=true;}
    g_phys_input6698=false;
}
// Narrow SEH leaves keep native exceptions out of RAII scopes. No retry loop.
static void* PhysItemLookup6698(std::uint64_t key) noexcept {
    void* registry=nullptr;if(!Read(reinterpret_cast<void*>(WhBase()+0x5487A78),registry)||!registry)return nullptr;
    __try {return reinterpret_cast<void*(__fastcall*)(void*,const std::uint64_t*)>(WhBase()+0x1E3E090)(static_cast<unsigned char*>(registry)+0x168,&key);}
    __except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT item lookup; no further native requests");return nullptr;}
}
static bool PhysBounds6698(void* entity,float* bounds) noexcept {
    __try {
        void* proxy=reinterpret_cast<void*(__fastcall*)(void*,int)>(WhBase()+0x5D70C4)(entity,0);
        if(!PhysVft6698(proxy,0x3A4A268)||!PhysVft6698(static_cast<unsigned char*>(proxy)-0x50,0x3A49FD8))return false;
        reinterpret_cast<void(__fastcall*)(void*,float*)>(WhBase()+0x63F884)(proxy,bounds);return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT native bounds; no repeated query");return false;}
}
// C34b (Miki: "the game has a native position for the second hand on the sword!"). It does: every two-handed weapon
// model carries the helpers slt_0 (right hand) and slt_1 (left hand); WHGame's attach routine 0x1085234 picks the helper
// by hand from the table {"slt_0","slt_1"} at 0x40AC288, and the item is placed so that the helper sits on the hand's
// RightHandSlot / LeftHandSlot bone. 0xB309DC = GetHelperQuatT(entity,name,out,localOnly) reads it (the game itself calls it
// with "slt_0" at 0x1E9DB45). We call the ORIGINAL (prologue-validated hook trampoline), never the patched entry.
// C36b (review): localOnly=false adds the entity SLOT transform (e->vt[0x2D8]), i.e. the helper in the entity frame - the same
// frame as the attached weapon pose. Both are read and logged; the entity-frame one is used, and only after it reproduces the
// measured native RIGHT wrist (fail-closed; there is no bind-pose fallback any more). C37: 0x180536FC4 orthonormalises both
// the helper TM and the slot TM before 0x18053D960 converts them, so the result is a rigid QuatT in model metres.
static bool PhysHelper34(void* entity,const char* name,ph::Pose& out,bool localOnly) noexcept {
    static std::atomic<bool> fault34{false}; // optional feature: a fault turns off ONLY the slot read, never the physical weapons
    if(!entity||!g_attachment_helper_transform_original||fault34.load(std::memory_order_relaxed))return false;
    QuatT28 q{};bool ok=false;
    __try{ok=g_attachment_helper_transform_original(entity,name,&q,localOnly);}
    __except(EXCEPTION_EXECUTE_HANDLER){fault34.store(true);diagnostic_trace::Write("C34 WEAPON_SLOTS FAULT (helper read raised; slot grip off for this session, physical weapons unchanged)");return false;}
    if(!ok)return false;
    const double n=std::sqrt(double(q.q[0])*q.q[0]+double(q.q[1])*q.q[1]+double(q.q[2])*q.q[2]+double(q.q[3])*q.q[3]);
    if(!std::isfinite(n)||n<.5||n>2.)return false; // the long_sword_crazy_guy helper carries a 1.22 axis scale: normalise
    out.q={q.q[0]/n,q.q[1]/n,q.q[2]/n,q.q[3]/n};out.p={q.t[0],q.t[1],q.t[2]};
    return ph::finite(out)&&pm::length(out.p)<3.0;
}
static bool PhysEmpty6698(std::uint64_t key) noexcept {std::uint64_t empty=0;return !key||(Read(reinterpret_cast<void*>(WhBase()+0x5325B60),empty)&&key==empty);}
// C54: loadout identity/weapon class are independent of a spawned world model.
// A successfully read null C_Item world link means metadata-only, not an invalid
// equipped weapon. It grants a virtual draw zone only; geometry/attachment/collision
// ownership still requires the existing live entity/socket proofs after drawing.
#include "../subsystems/physics/native_equipment_snapshot.inl" // M2_OWNED_SOURCE
#include "../subsystems/physics/native_handling_profile.inl" // M2_OWNED_SOURCE
static bool PhysContactOpponent175(const npc172::Witness& w) noexcept {
    // Body class alone is not an actor: articulated doors must remain passive
    // contacts. Follow only Henry's current native opponent and require complete
    // reciprocal combat-actor/entity identity to match this synchronous ray.
    if(!retail_build112::Ready(WhBase())||!g_npc_query172.requested||!g_npc_query172.ready||
       !w.ownerRead||!w.owner||!w.entityId||!w.collider||w.part<0||
       (w.body!=npc172::Body::Living&&w.body!=npc172::Body::Articulated))return false;
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.player||e.request!=g_weapon_request_epoch6694.load()||
       e.epoch!=g_pose_epoch6689.load()||!e.tick||now<e.tick||now-e.tick>100||
       !PhysVft6698(e.player,0x3AA4B98))return false;
    void* actor=nullptr;void* state=nullptr;void* opponent=nullptr;void* enemyState=nullptr;
    void* targetActor=nullptr;void* entity=nullptr;void* back=nullptr;void* henry=nullptr;std::uint32_t id=0;
    if(!Read(static_cast<unsigned char*>(e.player)+0x38,henry)||henry!=g_last_player_entity.load()||!PhysVft6698(henry,0x4072C78)||
       !Read(static_cast<unsigned char*>(e.player)+0x278,actor)||!PhysVft6698(actor,0x3A80610)||
       !Read(static_cast<unsigned char*>(actor)+0x1F8,back)||back!=e.player||
       !Read(static_cast<unsigned char*>(actor)+0x210,state)||!state||
       !Read(static_cast<unsigned char*>(state)+0xCE0,back)||back!=actor||
       !Read(static_cast<unsigned char*>(state)+0xCF8,opponent)||!opponent||opponent==actor||
       !PhysVft6698(opponent,0x3E3FAC8)||!Read(static_cast<unsigned char*>(opponent)+0x210,enemyState)||!enemyState||
       !Read(static_cast<unsigned char*>(enemyState)+0xCE0,back)||back!=opponent||
       !Read(static_cast<unsigned char*>(opponent)+0x1F8,targetActor)||!targetActor||
       !Read(static_cast<unsigned char*>(targetActor)+0x278,back)||back!=opponent||
       !Read(static_cast<unsigned char*>(targetActor)+0x38,entity)||reinterpret_cast<std::uintptr_t>(entity)!=w.owner||
       entity==g_last_player_entity.load()||!PhysVft6698(entity,0x4072C78)||
       !Read(static_cast<unsigned char*>(entity)+0xC,id)||id!=w.entityId||
       !Read(static_cast<unsigned char*>(state)+0xCF8,back)||back!=opponent)return false;
    return true;
}
#include "door_surface_query65.inl"
static ph::Contact103 PhysSweep6698(void* character,const ph::Profile& p,ph::Pose from,ph::Pose to,bool acquire) noexcept {
    ModelWorld6692 model{};
    const auto unknown=[](){ph::Contact103 c{};c.known=false;c.fraction=0;c.coverage169r2.reason=kcd2vr::sweep169r2::Reason::InvalidQuery;return c;};
    const bool modelAndBinding=ReadModelWorld6692(character,model)&&CollisionBinding6692(WhBase());
    kcd2vr::contact::AcquisitionBurstD45 acquireBudgetD45(g_contact_budget6695,
        modelAndBinding&&g_contact_budget_active6695&&g_contact_pass6695&&!acquire&&
        p.initialAcquireSweepD44&&p.key&&p.generation&&p.entity&&p.nativeSeed&&!p.shield176());
    auto rayOwner169r1=kcd2vr::diagnostics::passive169r1::rayContext.sample;
    rayOwner169r1.source=kcd2vr::passive169r1::Source::WeaponWorld;rayOwner169r1.side=p.side;
    rayOwner169r1.actor=reinterpret_cast<std::uintptr_t>(character);rayOwner169r1.key=p.key;rayOwner169r1.generation=p.generation;
    kcd2vr::diagnostics::passive169r1::RayScope rayScope169r1(rayOwner169r1);
    const bool capture169r2=kcd2vr::diagnostics::bow169r2::recording(rayOwner169r1.tick);
    const auto before169r2=capture169r2?ContactMicros6695():0;
    const auto budgetBefore169r2=g_contact_budget6695.spent;
    const auto rayD70=[&](pm::Vec3 a,pm::Vec3 b,double margin){
        ph::Contact103 hit{};
        const V3 origin=ModelToWorld6692(model,{float(a.x),float(a.y),float(a.z)}),finish=ModelToWorld6692(model,{float(b.x),float(b.y),float(b.z)}),motion=Sub(finish,origin);
        const float length=Len(motion);if(length<.0001f)return hit;if(length>2.5f)return unknown();
        const auto h=WorldRayDistance6692(origin,motion);
        if(h.state!=wc::Result::Clear&&h.state!=wc::Result::Hit){auto c=unknown();c.coverage169r2.reason=h.reason169r2;return c;}
        if(h.state==wc::Result::Hit){
            // D63: a ray starting just inside a world surface can be EXITING it.
            // The generic facing normal intentionally points toward the origin;
            // using that flipped normal here made a1.5mm outward withdrawal a
            // new inward hit. Never clear the ray from direction alone: ONE
            // current reverse query must see this exact exit as its FIRST hit.
            // It checks the whole remaining path without skipping an epsilon,
            // and shares the same native count/time budget. A nearer obstacle,
            // changed body/part/plane or incomplete query refuses the recovery.
            if(!acquire&&p.melee()&&p.key&&p.generation&&!p.initialAcquireSweepD44&&!p.nativeRejoinSweepR28){
                namespace ex=kcd2vr::weapon_exit_d63;
                const auto surface=[](const WorldRayResult6693& r){return ex::Surface{
                    {r.point.x,r.point.y,r.point.z},{r.nativeNormalD63.x,r.nativeNormalD63.y,r.nativeNormalD63.z},
                    r.native172,r.distance,r.plane};};
                const pm::Vec3 start{origin.x,origin.y,origin.z},end{finish.x,finish.y,finish.z};
                const auto first=surface(h);
                if(ex::candidate(first,start,end)){
                    const auto reverse=WorldRayDistance6692(finish,Mul(motion,-1.f));
                    if(reverse.state!=wc::Result::Clear&&reverse.state!=wc::Result::Hit){
                        auto c=unknown();c.coverage169r2.reason=reverse.reason169r2;return c;}
                    if(reverse.state==wc::Result::Hit&&ex::confirmed(first,surface(reverse),start,end))return hit;
                }
            }
            hit.fraction=double(std::clamp((h.distance-float(margin)*model.scale)/length,0.f,1.f));
            hit.plane=h.plane;hit.witness169r1=h.witness169r1;hit.native172=h.native172;
            if(h.plane){const auto normal=QRotateV(QConj(model.q),h.normal),point=WorldToModel6692(model,h.point);
                hit.normal={normal.x,normal.y,normal.z};hit.point={point.x,point.y,point.z};}
        }
        return hit;
    };
    ph::Contact103 result169r2=unknown();
    if(modelAndBinding){
        if(!acquire&&ph::initial_target_clear_d70(p,to,rayD70)){
            result169r2={};result169r2.targetPlacementD70=true;
            result169r2.coverage169r2.planned=result169r2.coverage169r2.completed=1;
            result169r2.coverage169r2.attemptedRays=result169r2.coverage169r2.completedRays=ph::initial_target_ray_count_d72(p);
        }else result169r2=ph::sweep_weapon112(p,from,to,acquire,rayD70);
    }
    if(!modelAndBinding)result169r2.coverage169r2.reason=kcd2vr::sweep169r2::Reason::BindingUnavailable;
    if(!acquire&&result169r2.known&&!result169r2.limited115&&result169r2.plane&&result169r2.fraction<1)
        result169r2.currentOpponent175=PhysContactOpponent175(result169r2.native172);
    if(capture169r2){
        kcd2vr::diagnostics::bow169r2::Record r{};r.kind=1;r.side=p.side;r.acquire=acquire;
        r.tick=rayOwner169r1.tick;r.sample=rayOwner169r1.sequence;r.epoch=rayOwner169r1.epoch;r.request=rayOwner169r1.request;
        r.actor=rayOwner169r1.actor;r.key=p.key;r.generation=p.generation;r.sweep=result169r2.coverage169r2;
        r.known=result169r2.known;r.limited=result169r2.limited115;r.fraction=result169r2.fraction;
        r.raysUsed=g_contact_budget6695.used;r.raysRemaining=g_contact_budget6695.remaining;
        r.workUs=g_contact_budget6695.spent>=budgetBefore169r2?g_contact_budget6695.spent-budgetBefore169r2:0;
        r.totalUs=ContactMicros6695()-before169r2;kcd2vr::diagnostics::bow169r2::push(r);
    }
    return result169r2;
}

#include "../subsystems/physics/shield_primary_b4.inl" // M2_OWNED_SOURCE
static void PhysCaptureGrip6698(const PhysEquipmentItem6698& item,const float* natural,void* attachmentSelf,const CommittedFrame6695& body,const float* coherentScale=nullptr,const PhysSocket101* coherentSocket=nullptr,const ph::Pose* primaryReferenceB4=nullptr,const ph::Pose* poleReferenceR26=nullptr) noexcept {
    auto* slot=&g_phys_grip_bank6698.grip[0];bool found=false;
    for(auto& x:g_phys_grip_bank6698.grip)if(x.key==item.profile.key){slot=&x;found=true;break;}
    if(!found){for(auto& x:g_phys_grip_bank6698.grip)if(!x.key){slot=&x;found=true;break;}if(!found)slot=&g_phys_grip_bank6698.grip[item.profile.side];*slot={};slot->key=item.profile.key;}
    auto& g=*slot;if(g.generation!=item.profile.generation){g={};g.key=item.profile.key;g.generation=item.profile.generation;}
    if(!body.stamp.sample||body.stamp.sample==g.lastSample)return;g.lastSample=body.stamp.sample;
    if(item.held){
        if(!coherentSocket||!coherentSocket->valid)return;
        // A held item key can arrive before its native draw attachment changes.
        // Learn each primary/support pair from one immutable socket binding.
        // Callback tick/serial updates are freshness, not a geometry change.
        // Never reset the checked physical collision state here.
        kcd2vr::grip_binding59c10r3::qualify(g,*coherentSocket);
    }
    auto weapon=PhysModel6698(natural,body.world);if(!ph::finite(weapon))return;g.nativeWeapon=weapon;g.nativeStamp=body.stamp;g.nativeSocket126=coherentSocket?*coherentSocket:PhysSocket101{};
    for(int k=0;k<3;++k){float scale=0;if(coherentScale)scale=coherentScale[k];else if(!Read(static_cast<unsigned char*>(attachmentSelf)+0x0C+k*4,scale))return;if(!std::isfinite(scale)||scale<=0||scale>10)return;g.geometryScale[k]=scale*natural[7]/body.world[7];}
    if(item.held&&item.profile.melee()&&primaryReferenceB4&&
       g.heldBindingEpochR27==body.stamp.epoch&&
       kcd2vr::grip_binding59c10r3::same(g.heldBindingR27,*coherentSocket)&&
       !std::memcmp(g.heldScaleR27,g.geometryScale,sizeof(g.geometryScale))){
        for(int side=0;side<2;++side)if(!g.ready[side]&&g.heldReadyR27[side]){
            g.hand[side]=g.candidate[side]=g.heldHandR27[side];g.ready[side]=true;g.stable[side]=6;
        }
    }
    // R123 fixed melee grip (melee_grip_r123.h): the primary hand of a sword/axe/mace/dagger uses the authored wrist
    // socket relation at once. A learned or cached grip that differs (the R118 longsword: 93 deg / 4.9 cm, a frozen clip
    // pose) is replaced on the next sample. Decided per sample, no latch; OFF = the old 6-frame learn.
    if(item.held&&primaryReferenceB4&&ph::finite(*primaryReferenceB4)&&kcd2vr::melee_grip_r123::enabled.load(std::memory_order_relaxed)&&
       kcd2vr::melee_grip_r123::eligible(item.profile.classId,item.profile.melee(),item.profile.shield176())&&
       item.profile.side>=0&&item.profile.side<2){
        const int ps=item.profile.side;const auto& ref=*primaryReferenceB4;
        const double oldCm=pm::length(g.hand[ps].p-ref.p)*100,oldDeg=pm::angle(g.hand[ps].q,ref.q)*57.29577951308232;
        if(!g.ready[ps]||oldCm>.01||oldDeg>.05){
            char m[256]{};std::snprintf(m,sizeof(m),"R123 MELEE_GRIP key=%016llX class=%d side=%d result=%s previousCm=%.1f previousDeg=%.1f local=(%.3f,%.3f,%.3f)",
                (unsigned long long)g.key,item.profile.classId,ps,g.ready[ps]?"AUTHORED_REPLACED_LEARNED":"AUTHORED_SOCKET",g.ready[ps]?oldCm:0.,g.ready[ps]?oldDeg:0.,ref.p.x,ref.p.y,ref.p.z);
            diagnostic_trace::Write(m);
            g.hand[ps]=g.candidate[ps]=ref;g.ready[ps]=true;g.stable[ps]=6;
        }
    }
    // R121 placement-first crossbow draw (Miki 6 Nov "is there any better solution"): every crossbow draw learned its grip
    // again from 6 stable native samples (log 10:54:29.457 drawn -> GRIP +85 ms -> owned +96 ms, 10:54:31.877 +57 ms,
    // the same local grip (-0.081,0.142,-0.000) every time), so the drawn crossbow sat 5-9 frames at Henry's animated
    // hand (or hidden by the R120 guard). The SAME wrist binding (key/generation/character/joint/relative/scale) in the
    // SAME body epoch reuses the settled grip on the first held sample, and the normal capture keeps checking it.
    if(item.held&&kcd2vr::ranged_kind_xr32::crossbow(item.profile.classId)&&coherentSocket&&
       g.heldBindingEpochR27==body.stamp.epoch&&kcd2vr::grip_binding59c10r3::same(g.heldBindingR27,*coherentSocket)&&
       !std::memcmp(g.heldScaleR27,g.geometryScale,sizeof(g.geometryScale))){
        for(int side=0;side<2;++side)if(!g.ready[side]&&g.heldReadyR27[side]){
            g.hand[side]=g.heldHandR27[side];g.candidate[side]={};g.ready[side]=true;g.stable[side]=0;g.recheckR121[side]=true;g.recheckSamplesR121[side]=0;
            char m[224]{};std::snprintf(m,sizeof(m),"R121 XBOW_GRIP_REUSE key=%016llX side=%d role=%s sample=%llu joint=%d",
                (unsigned long long)g.key,side,side==item.profile.side?"PRIMARY":"SUPPORT",(unsigned long long)body.stamp.sample,coherentSocket->joint);
            diagnostic_trace::Write(m);
        }
    }
    if(!item.held){
        // Cached model-space mount comes from THIS equipped item's real native
        // attachment. Reject detached loose/world items, not a generic body zone.
        if(pm::length(weapon.p)<2.0){g.mount=weapon;g.mountReady=true;g.mountStamp126=body.stamp;}
        // HS3 (Miki 25 Oct: second bow draw seated rotated to the left): the bow grip was learned once on the first draw and
        // reused forever. a stowed bow forgets it, so every draw learns the grip again from its own completed native pose.
        if(item.profile.bow()||kcd2vr::ranged_kind_xr32::crossbow(item.profile.classId)){for(int s=0;s<2;++s){g.ready[s]=false;g.stable[s]=0;g.recheckR121[s]=false;g.restSkipsR123[s]=0;g.restWindowR140[s]=0;}}
    }else for(int side=0;side<2;++side){
        if((g.ready[side]&&!g.recheckR121[side]&&!g.restWindowR140[side])||!body.valid[side])continue; // R121: a reused crossbow grip keeps being checked; R140: a rest-relation learn too
        // R121: the check only covers the draw itself (~0.33 s at 90 Hz). A later hand motion relative to the crossbow
        // (reload, aim) must never move the grip, so an unsettled check simply keeps the reused grip.
        if(g.ready[side]&&g.recheckR121[side]&&++g.recheckSamplesR121[side]>30){
            g.recheckR121[side]=false;char m[200]{};std::snprintf(m,sizeof(m),"R121 XBOW_GRIP_CHECK key=%016llX side=%d result=KEPT_REUSED_NOT_SETTLED",(unsigned long long)g.key,side);diagnostic_trace::Write(m);continue;}
        // R140: the provisional rest-relation grip found no other settled hold in its window: it IS the hold, keep it.
        if(g.ready[side]&&g.restWindowR140[side]&&!--g.restWindowR140[side]){
            char m[200]{};std::snprintf(m,sizeof(m),"R140 RANGED_GRIP_REST key=%016llX class=%d side=%d result=KEPT_NO_OTHER_HOLD",(unsigned long long)g.key,item.profile.classId,side);diagnostic_trace::Write(m);continue;}
        // Shields must not learn an animated blocking/idle offset as their permanent grip.
        // Other weapons retain the existing same-pose primary/support capture.
        const bool referenceShieldB4=item.profile.shield176()&&side==item.profile.side;
        if(referenceShieldB4&&!primaryReferenceB4)continue;
        auto candidate=referenceShieldB4?*primaryReferenceB4:ph::compose(ph::inverse(weapon),PhysPose6698(body.native[side]));
        if(poleReferenceR26){
            const auto nativePrimary=ph::compose(ph::inverse(weapon),PhysPose6698(body.native[item.profile.side]));
            candidate=PoleCaptureCandidateR26(candidate,nativePrimary,poleReferenceR26,side==item.profile.side,
                g.ready[item.profile.side]?&g.hand[item.profile.side]:nullptr);
        }
        const double span=pm::length(candidate.p);
        const bool isPrimary=side==item.profile.side;
        if(!ph::finite(candidate)||span>2.5||(!isPrimary&&!item.profile.twoHanded&&!item.profile.bow()))continue;
        if(!isPrimary){
            if(!g.ready[item.profile.side])continue;
            // R20C1: a synthetic fallback must never disable coherent native
            // support capture. Keep the same six-frame, bounds and hilt checks.
            const double separation=pm::length(candidate.p-g.hand[item.profile.side].p);
            if(separation<.045||separation>(item.profile.bow()?.85:.70))continue;
            if(!item.profile.bow()){
                if(!item.boundsValid)continue;
                const double v[3]{candidate.p.x,candidate.p.y,candidate.p.z};double outside2=0;
                for(int k=0;k<3;++k){const double a=item.bounds[k]*g.geometryScale[k],b=item.bounds[k+3]*g.geometryScale[k];const double d=std::max({a-v[k],v[k]-b,0.0});outside2+=d*d;}
                if(outside2>.10*.10)continue;
                if(item.profile.classId==4){
                    ph::Profile shape{};PhysFillProfile6698(item,g,shape);
                    if(!shape.geometry||!kcd2vr::support_grip166::OnHilt(g.hand[item.profile.side].p,shape.tip,candidate.p))continue;
                }
            }
        }
        // R123: a crossbow primary sample that sits on the authored rest relation is the bind frame before the draw clip
        // moved the crossbow (R122 log: 13:56:56 and 13:58:02 learned (-0.065,0.024,0.018) q~identity, 13:56:58 and 13:58:04
        // the real hold (-0.081,0.142,-0.000), 12 cm apart, D72). Skip it (bounded, kRestSkipCap) so the learn settles on the hold.
        // R140: the bow too (melee_grip_r123.h rangedRest), checked BEFORE the D70 bow reuse so a bind frame is never
        // reused either, and while a rest-relation learn is provisional (restWindowR140) every rest sample is skipped.
        const bool rangedRestR140=isPrimary&&primaryReferenceB4&&kcd2vr::melee_grip_r123::rangedRest(item.profile.classId);
        const double restMR140=rangedRestR140?pm::length(candidate.p-primaryReferenceB4->p):1.,restRadR140=rangedRestR140?pm::angle(candidate.q,primaryReferenceB4->q):1.;
        const bool onRestR140=rangedRestR140&&kcd2vr::melee_grip_r123::restRelation(restMR140,restRadR140);
        if(rangedRestR140&&kcd2vr::melee_grip_r123::skipRest(restMR140,restRadR140,g.restSkipsR123[side],g.restWindowR140[side]!=0)){
            if(!g.restWindowR140[side]&&!g.restSkipsR123[side]++){char m[200]{};std::snprintf(m,sizeof(m),"R123 RANGED_GRIP_REST_FRAME key=%016llX class=%d side=%d result=SKIPPED_BIND_FRAME",(unsigned long long)g.key,item.profile.classId,side);diagnostic_trace::Write(m);}
            g.stable[side]=0;continue;
        }
        // D70: a redraw may reuse a settled bow grip only after THIS completed
        // native pose proves the same local grip, binding, scale and actor epoch.
        // Never reuse the old rotated bow simply because its inventory key stayed.
        if(item.profile.bow()&&isPrimary&&kcd2vr::grip_binding59c10r3::reuseBowD70(
           g,*coherentSocket,body.stamp.epoch,g.geometryScale,side,
           pm::length(candidate.p-g.heldHandR27[side].p),pm::angle(candidate.q,g.heldHandR27[side].q))){
            g.hand[side]=g.candidate[side]=candidate;g.ready[side]=true;g.stable[side]=6;
            continue;
        }
        if(g.stable[side]&&pm::length(candidate.p-g.candidate[side].p)<.018&&pm::angle(candidate.q,g.candidate[side].q)<.10)++g.stable[side];else {g.candidate[side]=candidate;g.stable[side]=1;}
        // The current native wrist socket can prove the primary relation on
        // this first completed pose. Ordinary weapons still use the SAME-pose
        // candidate; only a float-precision match to the authored reference
        // skips the repeated-sample wait. Animated child offsets and support
        // candidates keep the original six-sample acquisition.
        // Bow draw can initially match the authored wrist while the attachment is
        // still moving. Keep its existing six-sample capture; other weapons retain
        // the authored-reference shortcut and shields retain their reference pose.
        // D72: same-item D71 capture proves the crossbow's first authored-match
        // sample can be 12 cm from its settled grip. Ranged draw must use its
        // coherent binding/pose stabilization, never the one-sample shortcut.
        const bool authoredPrimaryB4=!item.profile.bow()&&!kcd2vr::ranged_kind_xr32::crossbow(item.profile.classId)&&isPrimary&&primaryReferenceB4&&
            (referenceShieldB4||(pm::length(candidate.p-primaryReferenceB4->p)<1e-5&&
                pm::angle(candidate.q,primaryReferenceB4->q)<1e-4));
        if(authoredPrimaryB4||g.stable[side]>=6){
            // R121: a reused crossbow grip that the settled native pose confirms (same 1.8 cm / 0.1 rad as the stability
            // test) is kept as it is (no micro jump); a different settled grip replaces it and is remembered for the next draw.
            const bool replaceR121=!g.recheckR121[side]||!g.ready[side]||pm::length(candidate.p-g.hand[side].p)>=.018||pm::angle(candidate.q,g.hand[side].q)>=.10;
            if(g.recheckR121[side]){char m[200]{};std::snprintf(m,sizeof(m),"R121 XBOW_GRIP_CHECK key=%016llX side=%d result=%s",(unsigned long long)g.key,side,replaceR121?"REPLACED_BY_SETTLED_GRIP":"CONFIRMED");diagnostic_trace::Write(m);}
            g.recheckR121[side]=false;
            // R140: a provisional rest-relation grip is replaced by this settled hold (rest samples were skipped, so it is off
            // the rest relation); a learn that settles ON the rest relation (skip cap ran out) opens the provisional window.
            if(g.restWindowR140[side]){g.restWindowR140[side]=0;char m[256]{};std::snprintf(m,sizeof(m),"R140 RANGED_GRIP_REST key=%016llX class=%d side=%d result=REPLACED_BY_SETTLED_HOLD local=(%.3f,%.3f,%.3f)",(unsigned long long)g.key,item.profile.classId,side,candidate.p.x,candidate.p.y,candidate.p.z);diagnostic_trace::Write(m);}
            else if(onRestR140){g.restWindowR140[side]=kcd2vr::melee_grip_r123::kRestWindow;char m[200]{};std::snprintf(m,sizeof(m),"R140 RANGED_GRIP_REST key=%016llX class=%d side=%d result=PROVISIONAL_REST_LEARN",(unsigned long long)g.key,item.profile.classId,side);diagnostic_trace::Write(m);}
            if(replaceR121)g.hand[side]=candidate;g.ready[side]=true;if(PhysHolsterCaptureD48(body.stamp.tick)){char m[384]{};std::snprintf(m,sizeof(m),"V66.166 GRIP key=%016llX native=%c role=%s samples=%u local=(%.3f,%.3f,%.3f) localQ=(%.4f,%.4f,%.4f,%.4f) source=%s calibration=AUTOMATIC",(unsigned long long)g.key,side?'L':'R',isPrimary?"PRIMARY":"SUPPORT_CANDIDATE",g.stable[side],candidate.p.x,candidate.p.y,candidate.p.z,candidate.q.x,candidate.q.y,candidate.q.z,candidate.q.w,referenceShieldB4?"AUTHORED_WRIST_SOCKET_B4":authoredPrimaryB4?"SAME_POSE_AUTHORED_WRIST_MATCH":"SAME_ANIMATED_POSE");diagnostic_trace::Write(m);}}
    }
    if(item.held&&(item.profile.bow()||kcd2vr::ranged_kind_xr32::crossbow(item.profile.classId)||(item.profile.melee()&&primaryReferenceB4))&&g.ready[item.profile.side]){ // R121: + crossbow
        g.heldBindingR27=*coherentSocket;g.heldBindingEpochR27=body.stamp.epoch;
        std::memcpy(g.heldScaleR27,g.geometryScale,sizeof(g.geometryScale));
        for(int side=0;side<2;++side){g.heldHandR27[side]=g.hand[side];g.heldReadyR27[side]=g.ready[side];}
    }
    g_phys_grips6698.publish(g_phys_grip_bank6698);
}
static void PhysCaptureSamePose101(void* character,const QuatT28* original,std::uint32_t count,const RigMap& rig,const TrackingSnapshot& t,const QuatT28* restB4=nullptr,const int* parentsB4=nullptr,int rightHandSlotR26=-1) noexcept {
    if(g_weapon_control_mode.load()<4||!g_phys_armed6698.load()||!t.sequence||!original||rig.hand_r<0||rig.hand_l<0||rig.hand_r>=int(count)||rig.hand_l>=int(count))return;
    g_socket_capture_counts101[0].fetch_add(1,std::memory_order_relaxed);
    PhysEquipment6698 e{};PhysSocketBank101 sockets{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_phys_equipment6698.read(e)||!e.valid||e.request!=g_weapon_request_epoch6694.load()||e.epoch!=g_pose_epoch6689.load()||now<e.tick||now-e.tick>100||!g_phys_sockets101.read(sockets)){g_socket_capture_counts101[1].fetch_add(1,std::memory_order_relaxed);return;}
    PhysTryLock6698 lock;if(!lock.owned){g_socket_capture_counts101[2].fetch_add(1,std::memory_order_relaxed);return;}
    bool matched=false;CommittedFrame6695 native{};native.stamp={e.epoch,t.sequence,t.tick,reinterpret_cast<std::uintptr_t>(character)};native.world[3]=native.world[7]=1;
    native.valid[0]=native.valid[1]=true;native.native[0]=original[rig.hand_r];native.native[1]=original[rig.hand_l];
    for(int i=0;i<e.count;++i){const auto& item=e.items[i];if(!item.held)continue;
        for(const auto& socket:sockets.sockets)if(socket.valid&&socket.key==item.profile.key&&socket.generation==item.profile.generation&&socket.character==character&&socket.joint>=0&&socket.joint<int(count)&&now>=socket.tick&&now-socket.tick<=500){
            // Shared native pose evaluation: both hands and socket are from
            // 'original' before VRIK takes visual ownership of any joint.
            const auto weapon=AbsoluteFromRelative(original[socket.joint],socket.relative);
            const float qs[8]{weapon.q[0],weapon.q[1],weapon.q[2],weapon.q[3],weapon.t[0],weapon.t[1],weapon.t[2],1};
            ph::Pose referenceB4{};const ph::Pose* primaryB4=nullptr;
            const int primaryWristB4=item.profile.side?rig.hand_l:rig.hand_r;
            if(ShieldPrimaryReferenceB4(restB4,parentsB4,count,primaryWristB4,socket.joint,socket.relative,referenceB4))primaryB4=&referenceB4;
            matched=true;g_socket_capture_counts101[3].fetch_add(1,std::memory_order_relaxed);
            ph::Pose poleR26{};const ph::Pose* poleReferenceR26=nullptr;
            if(PolePrimaryReferenceR26(item,restB4,parentsB4,count,rig.hand_r,rightHandSlotR26,socket.scale,poleR26))
                poleReferenceR26=&poleR26;
            PhysCaptureGrip6698(item,qs,nullptr,native,socket.scale,&socket,primaryB4,poleReferenceR26);break;
        }
    }
    if(!matched)g_socket_capture_counts101[4].fetch_add(1,std::memory_order_relaxed);
}
// V66.135: a coherent physical owner must not be visually surrendered because one
// mailbox/native read misses a frame. Replay ONLY the last pose that was actually
// written for this exact item. True hand-key/lifecycle changes still restore at once.
static bool PhysEntityIdentity135(void* entity,std::uint32_t id) noexcept {
    std::uintptr_t vf=0;std::uint32_t actual=0;
    return entity&&id&&Read(entity,vf)&&vf==WhBase()+kCEntityVftRva&&
        Read(static_cast<unsigned char*>(entity)+0x0C,actual)&&actual==id;
}
// R59.16C10R14 STOWED BOW HIDE (Miki: "the bow is still visible if I move my head up, clipping through me - we
// wanted to remove it"). The stowed bow entity on Henry's back is hidden with the game's own CEntity::Hide
// (WHGame+0x87963C, the exact pin held_map_m1 uses) while the HMD looks up more than VrHideStowedBowPitchDeg
// (mode 1) or always while stowed (mode 2); it is shown again the moment it is drawn, when the mode is off or
// when the weapon owner stops. Render-only: the item, its slot and the native draw path are untouched.
#include "../subsystems/ranged_reload/stowed_bow_state.inl" // M2_OWNED_SOURCE
static bool EntityHiddenFlag59c14(void* entity,bool& hidden) noexcept {
    unsigned flags=0;if(!Read(static_cast<unsigned char*>(entity)+8,flags))return false;hidden=(flags&0x10)!=0;return true;
}
static bool EntityHidePinR125() noexcept {
    // R125: checked on every call, no session latch. The old one-time static saw the F2
    // recorder's D77 mid hook on 0x87963C (a jmp over the first instruction), refused, and
    // kept refusing for the whole session: R124 log, all R120 STOWED_ENTITY rows
    // HIDE_PIN_REFUSED. Identity = CEntity vtable slot 0x1F8 (.rdata) + original bytes or our
    // detour over the first instruction (entity_hide_pin_r125.h).
    namespace ep=::kcd2vr::entity_hide_pin_r125;
    const auto b=WhBase();std::uintptr_t slot=0;unsigned char actual[16]{};
    return b&&retail_build112::Ready(b)&&Read(reinterpret_cast<const void*>(b+ep::kEntityVftRva+ep::kHideSlot),slot)&&
        Read(reinterpret_cast<const void*>(b+ep::kHideRva),actual)&&ep::accept(b,slot,actual);
}
static bool EntityHide59c14(void* entity,bool hidden,bool* invoked=nullptr) noexcept {
    if(invoked)*invoked=false;
    if(!entity||!EntityHidePinR125())return false; // refused = asked again on the next call
    __try{if(invoked)*invoked=true;reinterpret_cast<void(__fastcall*)(void*,bool)>(WhBase()+0x87963C)(entity,hidden);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Called from the attachment callback (game thread) for the stowed bow; entity identity already verified.
#include "../subsystems/ranged_reload/stowed_bow_visibility.inl" // M2_OWNED_SOURCE
static void (*g_actionOwnerNativeTick59c15)() noexcept=nullptr;
#include "../subsystems/ranged_reload/stowed_bow_names.inl" // M2_OWNED_SOURCE
#include "../subsystems/ranged_reload/stowed_character_d44.inl"
#include "../subsystems/ranged_reload/stowed_bow_native_pins.inl" // M2_OWNED_SOURCE
static void* CharAttachment59c15(void* ch,const char* name) noexcept {
    const auto b=WhBase(); void* mgr=static_cast<unsigned char*>(ch)+8; std::uintptr_t vt=0,fn=0;
    if(!Read(mgr,vt)||vt!=b+0x4060548||!Read(reinterpret_cast<const void*>(vt+0x40),fn)||fn!=b+0x971B2C)return nullptr;
    void* att=nullptr; __try{att=reinterpret_cast<void*(__fastcall*)(void*,const char*)>(fn)(mgr,name);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return att;
}
static bool AttachmentFlags59c15(void* att,unsigned& flags) noexcept {return att&&Read(static_cast<unsigned char*>(att)+8,flags);}
static bool AttachmentHidden59c15(void* att,bool& hidden) noexcept {unsigned f=0;if(!AttachmentFlags59c15(att,f))return false;hidden=(f&0x10000u)!=0;return true;}
static bool AttachmentHide59c15(void* att,bool hide) noexcept {
    const auto b=WhBase(); std::uintptr_t vt=0,fn=0;
    if(!att||!Read(att,vt)||!Read(reinterpret_cast<const void*>(vt+0x88),fn)||fn!=b+0x5AF5E8)return false;
    __try{reinterpret_cast<void(__fastcall*)(void*,unsigned)>(fn)(att,hide?1u:0u);return true;}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
#include "../subsystems/ranged_reload/stowed_bow_attachment.inl" // M2_OWNED_SOURCE
static bool PhysAttachmentWorld164(void* attachment,std::uintptr_t expected,ModelWorld6692& world) noexcept {
    if(!expected||!retail_build112::Ready(WhBase())||!PhysVft6698(attachment,0x3A657C0))return false;
    void* manager=nullptr;void* character=nullptr;void* again=nullptr;
    return Read(static_cast<unsigned char*>(attachment)+0x28,manager)&&manager&&
        Read(static_cast<unsigned char*>(manager)+0x70,character)&&reinterpret_cast<std::uintptr_t>(character)==expected&&
        PhysVft6698(character,0x40602F8)&&ReadModelWorld6692(character,world)&&
        Read(static_cast<unsigned char*>(manager)+0x70,again)&&again==character;
}
static ph::Pose PhysCurrentWorld164(ph::Pose local,const ModelWorld6692& world) noexcept {
    local.p=local.p*world.scale;
    return ph::compose({{world.q.x,world.q.y,world.q.z,world.q.w},{world.t.x,world.t.y,world.t.z}},local);
}
#include "../subsystems/experimental/belt_attachment_experiment.inl" // M2_OWNED_SOURCE
static std::uint64_t ReadCompletedPublicationT23() noexcept;
static bool PhysReadVisualBodyR24(std::uint64_t publication,std::uint64_t epoch,std::uint64_t tracking,
    std::uint64_t character,CommittedFrame6695& body) noexcept {
    return publication&&!(publication&1)&&publication==ReadCompletedPublicationT23()&&
        tracking&&tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&character&&
        ReadCommittedPacket6695(body)&&body.publicationR24==publication&&
        body.stamp.actor==character&&body.stamp.epoch==epoch&&body.doorTrackingEpoch66==tracking&&
        publication==ReadCompletedPublicationT23()&&g_committed_pose_valid6689.load(std::memory_order_acquire)&&
        epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&tracking==tracking59c6::epoch.load(std::memory_order_acquire)&&
        character==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
}
static bool PhysReplayLast135(PhysPlacement6698& r,std::uint64_t now,unsigned reason) noexcept {
    if(!r.changed||!r.self||!r.attachment||!r.entity||r.side<0||r.side>1||
       reason==7||reason==8||!r.sourceTick146||now<r.sourceTick146||
       r.epoch146!=g_pose_epoch6689.load()||r.request146!=g_weapon_request_epoch6694.load()||
       !TrackingHeadUsable111(now)||!TrackingHandUsable111(r.side,now)||
       !r.placedAt||now<r.placedAt||
       g_weapon_control_mode.load()<4||animation_filter::nativeBodyOwned112.load()||NativePassengerCurrentA34(now)||
       g_phys_fault6698.load()||!g_gameplay_allowed.load()||
       g_henry_item_attach_manager.load()!=r.manager||!PhysEntityIdentity135(r.entity,r.entityId))return false;
    std::uint64_t live=0;const bool handKnown=NativeHandKey6697(r.manager,r.side,live);
    // A successful different key is authoritative sheath/swap evidence. An unreadable
    // hand chain receives only a very short 60-ms grace, never an indefinite cache.
    if(handKnown&&live!=r.key)return false;
    const float liveGrip=r.side?g_left_grip.load(std::memory_order_relaxed):g_right_grip.load(std::memory_order_relaxed);
    const bool exactHeld=handKnown&&live==r.key&&liveGrip>.55f&&TrackingHandUsable111(r.side,now);
    if(!std::isfinite(liveGrip)||liveGrip<.25f)return false;
    const std::uint64_t maxAge=handKnown?80ull:60ull;
    if(now-r.sourceTick146>maxAge)return false;
    // V66.135R1: never replay an old WORLD pose. Rebuild the last accepted weapon
    // pose from its body/model-space transform using Henry's CURRENT committed body
    // transform. If body motion itself is not current, skip grace instead of leaving
    // the sword behind in world space while Henry walks, turns or rides.
    const auto publicationR24=ReadCompletedPublicationT23();
    const auto readBodyR24=[&](std::uint64_t serial,CommittedFrame6695& body){
        return PhysReadVisualBodyR24(serial,r.epoch146,r.visualTrackingR24,r.visualCharacterR24,body)&&body.valid[r.side]&&
            body.stamp.tick&&now>=body.stamp.tick&&now-body.stamp.tick<=80&&
            std::isfinite(body.world[7])&&body.world[7]>=.5f&&body.world[7]<=2.f;
    };
    const auto pendingR24=[&](std::uint64_t serial){
        // A brief in-flight publication keeps this destination untouched, only
        // for the immediately preceding owner and the original80/60ms grace.
        if(serial&1)return g_committed_pose_valid6689.load(std::memory_order_acquire)&&
            r.visualPublicationR24&&serial==r.visualPublicationR24+1&&
            r.visualTrackingR24==tracking59c6::epoch.load(std::memory_order_acquire)&&
            r.visualCharacterR24==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
        CommittedFrame6695 current{};
        return r.visualPublicationR24&&serial>r.visualPublicationR24&&readBodyR24(serial,current);
    };
    // R98 xbow lane (weapon flicker): while that body is in flight/newer the
    // drawn hands are the last body skinned at Henry's CURRENT entity world, but
    // the untouched weapon kept the OLD world pose: walking/turning it lagged one
    // frame behind the hands and snapped back (flicker, "comes from behind").
    // Keep the same last accepted hand-relative (model-space) weapon pose and
    // only re-place it at the current world, exactly like the replay below.
    // Neither source time nor grace is refreshed; no new hand pose is taken.
    const auto rebaseR98=[&]() noexcept {
        ph::Pose local{{r.localWanted[0],r.localWanted[1],r.localWanted[2],r.localWanted[3]},
                       {r.localWanted[4],r.localWanted[5],r.localWanted[6]}};
        ModelWorld6692 now164{};
        if(!ph::finite(local)||!PhysAttachmentWorld164(r.attachment,r.visualCharacterR24,now164))return;
        const auto world=PhysCurrentWorld164(local,now164);if(!ph::finite(world))return;
        float wanted[8]{float(world.q.x),float(world.q.y),float(world.q.z),float(world.q.w),
                        float(world.p.x),float(world.p.y),float(world.p.z),r.localWanted[7]};
        bool child=false,placed=false;
        {PerfScope101R2 timing(2);InvalidateReadCache101R2();placed=PlaceBothConsumers6689(r.self,r.entity,wanted,child);InvalidateReadCache101R2();}
        if(placed){std::memcpy(r.wanted,wanted,sizeof(wanted));g_phys_rebase_r98.fetch_add(1,std::memory_order_relaxed);}
    };
    // A newer successful body packet can precede its physical packet. Consume
    // without taking a new hand pose. Neither source time nor its
    // original grace is refreshed; explicit retirement has no matching body.
    if((publicationR24&1)||publicationR24>r.visualPublicationR24){
        if(!pendingR24(publicationR24))return false;
        rebaseR98();return true;}
    CommittedFrame6695 body{};
    if(!readBodyR24(publicationR24,body)||publicationR24!=r.visualPublicationR24||
       body.stamp.sample!=r.visualSampleR24||body.stamp.tick!=r.sourceTick146)return false;
    ph::Pose local{{r.localWanted[0],r.localWanted[1],r.localWanted[2],r.localWanted[3]},
                   {r.localWanted[4],r.localWanted[5],r.localWanted[6]}};
    if(!ph::finite(local))return false;
    ModelWorld6692 current164{};if(!PhysAttachmentWorld164(r.attachment,body.stamp.actor,current164))return false;
    const auto world=PhysCurrentWorld164(local,current164);
    if(!ph::finite(world))return false;
    float wanted[8]{float(world.q.x),float(world.q.y),float(world.q.z),float(world.q.w),
                    float(world.p.x),float(world.p.y),float(world.p.z),r.localWanted[7]};
    bool child=false;bool placed=false;
    if(publicationR24!=ReadCompletedPublicationT23()||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
       r.visualTrackingR24!=tracking59c6::epoch.load(std::memory_order_acquire)){
        if(!pendingR24(ReadCompletedPublicationT23()))return false;
        rebaseR98();return true;}
    {PerfScope101R2 timing(2);InvalidateReadCache101R2();placed=PlaceBothConsumers6689(r.self,r.entity,wanted,child);InvalidateReadCache101R2();}
    if(!placed)return false;
    std::memcpy(r.wanted,wanted,sizeof(wanted));
    g_phys_grace135.fetch_add(1,std::memory_order_relaxed);
    static std::uint64_t last=0;if(now<last||now-last>=2000){last=now;char m[320]{};
        std::snprintf(m,sizeof(m),"V66.135R1 PHYSICAL_GRACE side=%c reason=%u ageMs=%llu liveHand=%s policy=BODY_RELATIVE_LAST_GOOD_POSE bodyAgeMs=%llu exactHeld=%d maxAgeMs=%llu",
            r.side?'L':'R',reason,(unsigned long long)(now-r.placedAt),handKnown?"SAME":"UNREADABLE",
            (unsigned long long)(now-body.stamp.tick),exactHeld?1:0,(unsigned long long)maxAge);diagnostic_trace::Write(m);}
    return true;
}
#include "../subsystems/throw_drop/draw_observer_host_d77.inl"
#include "../subsystems/ranged_reload/draw_visibility_d45.inl"
#include "../subsystems/ranged_reload/first_draw_binding_d75.inl"
#include "../subsystems/ranged_reload/ranged_draw_visibility_d72.inl"
#include "../subsystems/ranged_reload/bow_placement_lifetime.inl" // M2_OWNED_SOURCE
#include "native_held_owner_t4.inl"
#include "../subsystems/physics/held_attachment_owner.inl" // M2_OWNED_SOURCE
static PhysicalAttachmentResult101 PhysAttachmentResultImpl101(void* self,void* attachment) noexcept {
    using R=PhysicalAttachmentResult101;
    const auto native=[&](){if(g_entity_attachment_update_original)g_entity_attachment_update_original(self,attachment);};
    std::uint32_t id=0;if(!Read(static_cast<unsigned char*>(self)+8,id))return R::NotMine;
    bool relevant=false;for(auto& known:g_phys_relevant_id101)if(known.load(std::memory_order_relaxed)==id&&id){relevant=true;break;}
    const bool thisPending164=g_phys_pending_self164[0].load(std::memory_order_acquire)==self||
        g_phys_pending_self164[1].load(std::memory_order_acquire)==self;
    if(!relevant&&!thisPending164)return R::NotMine;
    PhysEquipment6698 equipment{};g_phys_equipment6698.read(equipment);int index=-1;
    for(int i=0;i<equipment.count;++i)if(equipment.items[i].profile.entity==id){index=i;break;}
    if(index<0&&!thisPending164)return R::NotMine;
    if(index>=0&&DrawNativeActiveD77()){
        namespace dd=kcd2vr::diagnostics::draw_native_d77;dd::Event ev{};
        ev.kind=dd::Kind::Attachment;ev.tick=GetTickCount64();ev.draw.key=equipment.items[index].profile.key;
        ev.draw.body=equipment.epoch;ev.draw.request=equipment.request;ev.draw.side=equipment.items[index].profile.side;
        ev.object=reinterpret_cast<std::uintptr_t>(self);ev.other=reinterpret_cast<std::uintptr_t>(attachment);
        ev.entity=reinterpret_cast<std::uintptr_t>(equipment.items[index].entity);ev.entityId=id;ev.value=-1;
        dd::recorder.attachment(ev); // copied lookup admission; not a native/render success
    }
    PhysTryLock6698 lock;if(!lock.owned)return index>=0?R::Busy:R::NotMine;
    PhysPlacement6698* existing=nullptr;
    for(auto& old:g_phys_placement6698)if(old.self==self&&old.attachment==attachment&&old.changed){existing=&old;break;}
    const bool pending=existing!=nullptr;if(index<0&&!pending)return R::NotMine;
    const auto restore=[&](unsigned why48=0,bool nativeOwnerD75=false){
        if(diagnostics::ownership48::active(GetTickCount64())){
            diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::Attachment;e.tick=GetTickCount64();e.thread=GetCurrentThreadId();e.code=int(why48);e.flags=1;
            e.actor=reinterpret_cast<std::uintptr_t>(equipment.player);e.object=reinterpret_cast<std::uintptr_t>(attachment);e.item=existing?existing->key:0;e.generation=existing?existing->generation:0;e.side=existing?existing->side:-1;
            e.epoch=g_pose_epoch6689.load();if(existing){e.v[31]=float(existing->sourceTick146?e.tick-existing->sourceTick146:0);std::copy_n(existing->wanted,8,e.v);}
            diagnostics::ownership48::name(e,"RESTORE_NATIVE_ATTACHMENT");(void)diagnostics::ownership48::push(e);
        }

        if((index>=0&&equipment.items[index].profile.bow())||(existing&&existing->key==g_bow_lifetimes59c10r2.previous.key))
            PhysRetireBowGeometryLifetime59c10r2();
        if(nativeOwnerD75)RangedDrawRetireAttachmentD72(self,attachment);
        else {
            // A missing/read-failed publication is not a native handoff. Keep
            // first-placement hides, and conceal a returning native callback
            // before relinquishing a previously verified physical destination.
            for(auto& accepted:g_weapon_draw_accepted_d75)if(accepted.binding.self==reinterpret_cast<std::uintptr_t>(self)&&
                accepted.binding.attachment==reinterpret_cast<std::uintptr_t>(attachment))accepted={};
            if(index>=0&&equipment.items[index].held)
                RangedDrawBeginD72(RangedDrawIdentityD72(equipment,equipment.items[index],self,attachment));
        }
        native();for(auto& old:g_phys_placement6698)if(old.self==self&&old.attachment==attachment)old={};unsigned mask=0;for(int s=0;s<2;++s){const auto& record=g_phys_placement6698[s];if(record.changed)mask|=1u<<s;g_phys_pending_self164[s].store(record.changed?record.self:nullptr,std::memory_order_release);}g_phys_pending6698=mask;};
    const auto now=std::uint64_t(GetTickCount64());
    // Hard lifecycle/ownership boundaries never use grace.
    const bool nativeFallbackD45=g_weapon_control_mode.load()<4||animation_filter::nativeBodyOwned112.load()||NativePassengerCurrentA34(now)||g_phys_fault6698.load()||!g_gameplay_allowed.load();
    if(nativeFallbackD45&&WeaponDrawReloadAttachmentD75(self,attachment,now))return R::Waiting;
    if(nativeFallbackD45){RangedDrawRetireAttachmentD72(self,attachment);if(index>=0)PhysRevealDrawnBowD45(equipment,equipment.items[index],now,false,true);}
    if(nativeFallbackD45){
        if(pending){restore(11,true);return R::Restored;}return R::Waiting;
    }
    // A transient publisher/read gap may leave the mailbox empty or >100 ms old.
    // Freeze at the last coherent physical pose briefly instead of displaying one
    // authored/native frame between two physical frames.
    if(!equipment.valid){if(existing&&PhysReplayLast135(*existing,now,1))return R::Placed;if(pending){++g_phys_grace_expired135;PhysAuthorityDrop157(1);restore(1);return R::Restored;}return R::Waiting;}
    if(equipment.request!=g_weapon_request_epoch6694.load()||equipment.epoch!=g_pose_epoch6689.load()||now<equipment.tick){if(pending){restore(12);return R::Restored;}return R::Waiting;}
    if(now-equipment.tick>100){if(existing&&PhysReplayLast135(*existing,now,2))return R::Placed;if(pending){++g_phys_grace_expired135;PhysAuthorityDrop157(2);restore(2);return R::Restored;}return R::Waiting;}
    if(index<0){if(pending){restore(13);return R::Restored;}return R::Waiting;}
    const auto& item=equipment.items[index];float natural[8]{};
    if(!ReadAttachmentQs6689(attachment,natural)){if(existing&&PhysReplayLast135(*existing,now,3))return R::Placed;if(pending)PhysAuthorityDrop157(3);restore(3);return R::Restored;}
    PhysRememberSocket101(item,self,attachment,now);
    if(!item.held&&!WeaponDrawMayStowD75(equipment,item))return R::Waiting;
    CommittedFrame6695 body{};if(!item.held&&g_committed_frame6695.read(body)&&body.stamp.epoch==g_pose_epoch6689.load()&&now>=body.stamp.tick&&now-body.stamp.tick<=100)PhysCaptureGrip6698(item,natural,self,body);
    if(!item.held){restore(0,true);
        if(RangedDrawStowedD73(item)&&item.profile.bow()&&PhysEntityIdentity135(item.entity,id))StowedBowHide59c14(item.entity,now); // retire draw hide before stow hide
        if(body.stamp.epoch==equipment.epoch&&body.stamp.tick&&now>=body.stamp.tick&&now-body.stamp.tick<=80&&
           PhysBeltExperiment164(equipment,index,self,attachment,natural,body))return R::Placed;
        return R::Restored;}
    const int side=item.profile.side;std::uint64_t live=equipment.handKey[side];bool liveKnown=equipment.handKnown[side];
    // The native-frame equipment snapshot already validated this hand chain twice.
    // Re-read only once it is older than half a frame budget; do not triple-query
    // the same manager/player/slots chain on every attachment callback.
    if(now-equipment.tick>50)liveKnown=NativeHandKey6697(equipment.manager,side,live);
    if(liveKnown&&live!=item.profile.key){restore(14,true);return R::Restored;}
    if(!liveKnown){if(existing&&PhysReplayLast135(*existing,now,4))return R::Placed;if(pending)PhysAuthorityDrop157(4);restore(4);return R::Restored;}
    // item.entity was resolved and generation-stamped by the metadata snapshot.
    // A global entity-system lookup on every attachment callback was redundant and
    // expensive; validate the exact pointer/id directly instead.
    if(!PhysEntityIdentity135(item.entity,id)){if(existing&&PhysReplayLast135(*existing,now,5))return R::Placed;if(pending)PhysAuthorityDrop157(5);restore(5);return R::Restored;}
    const bool drawWeaponD75=wdrawD75::weaponClass(item.profile.classId);
    const auto drawBindingD72=RangedDrawIdentityD72(equipment,item,self,attachment);
    // The old placement slot may have the SAME attachment but a NEW item/key.
    // Match its complete identity before deciding first transport was hidden.
    const bool samePlacementD72=existing&&rdrawD72::same(drawBindingD72,
        {reinterpret_cast<std::uintptr_t>(existing->entity),reinterpret_cast<std::uintptr_t>(existing->self),reinterpret_cast<std::uintptr_t>(existing->attachment),
         existing->key,existing->generation,existing->epoch146,existing->request146,existing->entityId,existing->side});
    if(drawWeaponD75&&(!samePlacementD72||RangedDrawPendingForD72(drawBindingD72)))RangedDrawBeginD72(drawBindingD72);
    PhysRevealDrawnBowD45(equipment,item,now,false,false);
    if(item.profile.bow())BowCaptureCharacter128(item.entity);
    PhysFrame6698 frame{};
    // Do not expose a new pose before its command/publication transaction has
    // completed. The existing native-frame consumer already uses this marker.
    const auto ready174=g_phys_ready_sample6698.load(std::memory_order_acquire);
    const bool haveFrame=g_phys_frame6698.read(frame)&&frame.input.sample==ready174&&
        ready174==g_phys_ready_sample6698.load(std::memory_order_acquire);
    const bool frameOwnerR24=haveFrame&&frame.request==equipment.request&&frame.input.bodyEpoch==g_pose_epoch6689.load()&&frame.player==equipment.player&&frame.profiles[side].key==live&&frame.profiles[side].generation==item.profile.generation;
    const bool frameAge=haveFrame&&now>=frame.input.tick&&now-frame.input.tick<=100;
    const auto publicationR24=ReadCompletedPublicationT23();
    CommittedFrame6695 visualBodyR24{};
    const bool currentBodyR24=frameOwnerR24&&PhysReadVisualBodyR24(publicationR24,frame.input.bodyEpoch,
        frame.trackingEpochT23,frame.characterT23,visualBodyR24)&&visualBodyR24.valid[side];
    if(((publicationR24&1)||(currentBodyR24&&publicationR24>frame.publicationT23))&&
       frameOwnerR24&&frame.output.active[side]&&existing&&existing->visualPublicationR24==frame.publicationT23&&
       PhysReplayLast135(*existing,now,6))return R::Busy;
    const bool frameIdentity=frameOwnerR24&&frame.characterT23==frame.correlation126[side].stamp.actor&&
        currentBodyR24&&frame.publicationT23==publicationR24&&visualBodyR24.stamp.sample==frame.input.sample&&
        visualBodyR24.stamp.tick==frame.input.tick;
    // INVESTIGATE_D67R1: this proves CPU attachment/body publication identity.
    // F2_PALETTE_D67R1 now joins that same publication to native current/previous
    // skinning output and its pool counter. It does NOT prove that this later
    // attachment callback was read by the same eye's draw as those bones. Follow
    // the attachment render-object submission through its existing native
    // callback and pair it with Henry renderObject+58 skinData (3D4C49); success
    // requires one body publication for both drawn wrists and held mesh in each
    // eye. Any future repair belongs to a proved visual-consumer scope here,
    // not a newer transform injected into native physics/gameplay state.
    if(!frameIdentity||!frame.output.active[side]||!frameAge){
        g_physical_gates101R2[!haveFrame?4:!frameIdentity?5:!frame.output.active[side]?6:7].fetch_add(1,std::memory_order_relaxed);
        const unsigned authorityReason=!haveFrame?6:!frameIdentity?7:!frame.output.active[side]?8:9;
        if(existing&&PhysReplayLast135(*existing,now,authorityReason))return R::Placed;
        if(pending){++g_phys_grace_expired135;PhysAuthorityDrop157(authorityReason);restore(authorityReason);return R::Restored;}return R::Waiting;
    }
    const auto localTarget=(frame.output.displayLatched[side]||frame.output.displayRejoinR13[side])?frame.output.displayWeapons[side]:frame.output.weapons[side]; // V66.178R19: the placement writer shows the display packet (pinned); physics/native keep weapons[]
    ModelWorld6692 current164{};
    // The captured socket proves Henry's character identity, not its old world transform.
    const auto character164=frame.correlation126[side].stamp.actor;
    if(!PhysAttachmentWorld164(attachment,character164,current164)){if(pending){PhysAuthorityDrop157(10);restore(10);return R::Restored;}return R::Waiting;}
    const auto target=PhysCurrentWorld164(localTarget,current164);
    float wanted[8]{float(target.q.x),float(target.q.y),float(target.q.z),float(target.q.w),float(target.p.x),float(target.p.y),float(target.p.z),natural[7]};bool child=false,checked=false;
    auto& record=g_phys_placement6698[side];
    const bool first=!record.changed||!samePlacementD72;
    const bool pendingRevealD72=drawWeaponD75&&(first||!record.initialPlacementVerifiedD72||RangedDrawPendingForD72(drawBindingD72)||
        (item.profile.bow()&&g_hidden_bow_entity59c14.load()==item.entity));
    const auto verifyPlanD72=rdrawD72::verification(first,pendingRevealD72,now,record.verifiedAt);
    const bool verify=verifyPlanD72.read;
    bool writeOk=false,verifyOk=true;
    // The native world read must not turn a replaced body receipt into a new
    // attachment placement. This is visual currentness, not attack admission.
    if(frame.publicationT23!=ReadCompletedPublicationT23()||!g_committed_pose_valid6689.load(std::memory_order_acquire)||
       frame.trackingEpochT23!=tracking59c6::epoch.load(std::memory_order_acquire)){
        if(existing&&ReadCompletedPublicationT23()>frame.publicationT23&&PhysReplayLast135(*existing,now,6))return R::Busy;
        if(pending){restore(7);return R::Restored;}return R::Waiting;
    }
    {PerfScope101R2 timing(2);InvalidateReadCache101R2();writeOk=PlaceBothConsumers6689(self,item.entity,wanted,child);InvalidateReadCache101R2();
     if(writeOk&&verify)verifyOk=PlacementReadback6689(self,item.entity,id,wanted,checked);}
    if(DrawNativeActiveD77()){
        namespace dd=kcd2vr::diagnostics::draw_native_d77;dd::Event ev{};
        ev.kind=dd::Kind::Placement;ev.tick=GetTickCount64();ev.draw.key=item.profile.key;
        ev.draw.body=equipment.epoch;ev.draw.request=equipment.request;ev.draw.side=side;
        ev.object=reinterpret_cast<std::uintptr_t>(self);ev.other=reinterpret_cast<std::uintptr_t>(attachment);
        ev.entity=reinterpret_cast<std::uintptr_t>(item.entity);ev.entityId=id;
        ev.value=(writeOk?1:0)|(child?2:0)|(verify?4:0)|(checked?8:0)|(verifyOk?16:0)|(first?32:0);
        ev.observedSample=frame.input.sample;ev.observedPublication=frame.publicationT23;ev.observedTracking=frame.trackingEpochT23;
        std::copy_n(wanted,8,ev.wanted);std::copy_n(natural,8,ev.natural);
        dd::recorder.attachment(ev); // write/readback receipt; pixels still unobserved
    }
    if(kcd2vr::diagnostics::motion_d68::requested()){
        namespace md=kcd2vr::diagnostics::motion_d68;md::Event e{};
        static thread_local md::Cadence cadence[2];
        if(md::begin(e,md::Attachment)&&md::inWindow(e)){
            if(cadence[side].due(e.session,e.ns)){
                e.actor=frame.characterT23;e.object=reinterpret_cast<std::uintptr_t>(attachment);
                e.sample=frame.input.sample;e.sourceNs=frame.input.timeNs;e.sourceTick=frame.input.tick;
                e.poseEpoch=frame.input.bodyEpoch;e.trackingEpoch=frame.trackingEpochT23;
                e.publication=frame.publicationT23;e.request=frame.request;e.side=side;e.thread=GetCurrentThreadId();
                // Actual placement return/readback status, not render/pixel proof.
                e.flags=(writeOk?1u:0u)|(child?2u:0u)|(verify?4u:0u)|(checked?8u:0u)|(verifyOk?16u:0u)|(first?32u:0u);
                std::copy_n(wanted,8,e.v);std::copy_n(natural,8,e.v+8);std::copy_n(frame.world,8,e.v+16);
                e.v[24]=current164.q.x;e.v[25]=current164.q.y;e.v[26]=current164.q.z;e.v[27]=current164.q.w;
                e.v[28]=current164.t.x;e.v[29]=current164.t.y;e.v[30]=current164.t.z;e.v[31]=current164.scale;
                e.count=32;(void)md::push(e);
            }else md::noteCadenceSkip(md::Attachment);
        }
    }
    if(!writeOk){
        if(!first&&PhysReplayLast135(record,now,10))return R::Placed;
        if(!first)PhysAuthorityDrop157(10);
        restore();PhysFault6698("V66.135 PHYSICAL PLACEMENT_FAULT repeated/current native destination restored");return R::Fault;
    }
    // A readback is diagnostic coherence, not a second visual owner. One failed
    // verification must not restore a successfully written pose. Require three
    // separated failures before latching a placement fault.
    unsigned verifyFails=first?0:record.verifyFails;
    if(verifyPlanD72.periodic&&!verifyOk){++verifyFails;g_phys_verify_deferred135.fetch_add(1,std::memory_order_relaxed);
        if(verifyFails>=3){restore();PhysFault6698("V66.135 PHYSICAL READBACK_FAULT three separated verification failures");return R::Fault;}}
    else if(verify&&verifyOk)verifyFails=0;
    const bool placementCurrentD75=frame.publicationT23==ReadCompletedPublicationT23()&&
        g_committed_pose_valid6689.load(std::memory_order_acquire)&&
        frame.trackingEpochT23==tracking59c6::epoch.load(std::memory_order_acquire);
    RangedDrawPlacedD72(drawBindingD72,verify,verifyOk&&placementCurrentD75);
    PhysRevealDrawnBowD45(equipment,item,now,rdrawD72::mayReveal(verify,verifyOk&&placementCurrentD75),false);
    TracePhysicalItem28(frame,side,self,attachment,item.entity,id,item.profile.generation,current164,wanted,verify,verifyOk);
    if(item.profile.bow())PhysPublishBowPlacement59c10r2(equipment,item,frame,current164,target,now);
    g_physical_gates101R2[8].fetch_add(1,std::memory_order_relaxed);
    record.self=self;record.attachment=attachment;record.entity=item.entity;record.manager=equipment.manager;record.entityId=id;record.side=side;
    if(diagnostics::ownership48::active(now)){
        diagnostics::ownership48::Event e{};e.stage=diagnostics::ownership48::Attachment;e.tick=now;e.thread=GetCurrentThreadId();e.side=side;e.code=0;e.flags=first?2u:0u;
        e.actor=frame.characterT23;e.object=reinterpret_cast<std::uintptr_t>(attachment);e.item=live;e.generation=item.profile.generation;e.sample=frame.input.sample;e.publication=frame.publicationT23;e.epoch=frame.input.bodyEpoch;e.sourceEpoch=frame.trackingEpochT23;e.sourceNs=frame.input.timeNs;
        std::copy_n(wanted,8,e.v);std::copy_n(natural,8,e.v+8);std::copy_n(frame.world,8,e.v+16);
        diagnostics::ownership48::name(e,"PLACED_VR_WORLD_QXYZW_PXYZ_SCALE_NATIVE_SAME_LAYOUT");(void)diagnostics::ownership48::push(e);
    }
    record.key=live;record.generation=item.profile.generation;record.changed=true;record.placedAt=now;record.sourceTick146=frame.input.tick;record.epoch146=frame.input.bodyEpoch;record.request146=frame.request;record.verifyFails=verifyFails;
    record.visualPublicationR24=frame.publicationT23;record.visualSampleR24=frame.input.sample;
    record.visualTrackingR24=frame.trackingEpochT23;record.visualCharacterR24=frame.characterT23;
    record.initialPlacementVerifiedD72=(!first&&record.initialPlacementVerifiedD72)||rdrawD72::mayReveal(verify,verifyOk&&placementCurrentD75);
    if(verifyPlanD72.periodic)record.verifiedAt=now;std::memcpy(record.wanted,wanted,sizeof(wanted));
    PhysRecordReleaseT1(equipment,item,frame,attachment,wanted);
    const float localWanted[8]{float(localTarget.q.x),float(localTarget.q.y),float(localTarget.q.z),float(localTarget.q.w),float(localTarget.p.x),float(localTarget.p.y),float(localTarget.p.z),natural[7]};
    std::memcpy(record.localWanted,localWanted,sizeof(localWanted));g_phys_pending_self164[side].store(self,std::memory_order_release);g_phys_pending6698.fetch_or(1u<<side);
    static std::uint64_t lastTrace104[2]{};
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&
       (first||now<lastTrace104[side]||now-lastTrace104[side]>=2000)){
        lastTrace104[side]=now;
        const auto requested=frame.input.hands[side],allowed=frame.output.hands[side];
        char msg[704]{};std::snprintf(msg,sizeof(msg),
            "V66.135 WEAPON_POSE rung=%d side=%c sample=%llu key=%016llX contactKnown=%d blocked=%d sweepDeferred=%d inputModel=(%.4f,%.4f,%.4f) allowedModel=(%.4f,%.4f,%.4f) controllerToAllowedDeg=%.3f writtenWorld=(%.4f,%.4f,%.4f) readback=%s child=%d graceTotal=%llu",
            g_weapon_control_mode.load(),side?'L':'R',(unsigned long long)frame.input.sample,(unsigned long long)live,
            frame.output.contactKnown[side]?1:0,frame.output.blocked[side]?1:0,frame.output.sweepDeferred115[side]?1:0,
            requested.p.x,requested.p.y,requested.p.z,allowed.p.x,allowed.p.y,allowed.p.z,
            pm::angle(requested.q,allowed.q)*180/3.141592653589793,target.p.x,target.p.y,target.p.z,!verify?"SKIPPED":verifyOk?"OK":"DEFERRED",child?1:0,(unsigned long long)g_phys_grace135.load());
        const auto oldWorld=ph::compose(PhysWorld6698(frame.world),ph::Pose{localTarget.q,localTarget.p*frame.world[7]});
        char timing164[320]{};std::snprintf(timing164,sizeof(timing164),"V66.164 ATTACHMENT_WORLD side=%c sample=%llu poseAgeMs=%llu rebaseM=%.5f rebaseRad=%.5f owner=EXACT_CURRENT_HENRY_CHARACTER",
            side?'L':'R',(unsigned long long)frame.input.sample,(unsigned long long)(now-frame.input.tick),pm::length(target.p-oldWorld.p),pm::angle(target.q,oldWorld.q));diagnostic_trace::Write(timing164);
        diagnostic_trace::Write(msg);
    }
    if(first&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){char m[192]{};std::snprintf(m,sizeof(m),"V66.135 PLACEMENT ACTIVE side=%c entity=%u sharedWeaponPose=1 antiFlickerGraceMs=80",side?'L':'R',id);diagnostic_trace::Write(m);}
    return R::Placed;
}
static bool PhysCallAction6698(void* player,int name,int mode,bool direct178r17=false,bool* enteredR24=nullptr) noexcept {
    if(enteredR24)*enteredR24=false;
    auto& n=g_phys_native6698;void* input=nullptr;void* back=nullptr;
    if(name<0||name>4||!PhysVft6698(player,0x3AA4B98)||!Read(static_cast<unsigned char*>(player)+0xB88,input)||!PhysVft6698(input,0x3EC99A8)||!Read(static_cast<unsigned char*>(input)+0x10,back)||back!=player)return false;
    // V66.178R8: the native XI transport replaces the synchronous handler call (see attack_xi178r8.h).
    // V66.178R17: direct178r17 = the handler itself, below the action map (no binding, no lock map, one ~69 ms call).
    if(!direct178r17&&kcd2vr::attack_xi178r8::enabled.load(std::memory_order_relaxed)){
        kcd2vr::attack_xi178r8::edges.fetch_add(1,std::memory_order_relaxed);
        return kcd2vr::attack_xi178r8::translate(name,mode,std::uint64_t(GetTickCount64()));
    }
    static const char* names[5]{"prepare_to_attack","attack_primary","attack_abort","block","attack_special"};
    __try {
        if(!n.actionNames[name])reinterpret_cast<void(__fastcall*)(std::uint64_t*,const char*)>(WhBase()+0x8E5AEC)(&n.actionNames[name],names[name]);
        if(!n.actionNames[name])return false;
        auto call=g_combat_bypass101.load(std::memory_order_acquire);
        if(!call)call=reinterpret_cast<CombatInputFn101>(WhBase()+0x8E5324);
        InvalidateReadCache101R2();if(enteredR24)*enteredR24=true;
        call(input,&n.actionNames[name],mode,mode==2?0.f:1.f);InvalidateReadCache101R2();return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT native action request");return false;}
}
static void NativeForgetSolverGestureH28(void* player,std::uint64_t gesture) noexcept;
static void PhysAbortAttack117(void* player) noexcept {
    NativeForgetSolverGestureH28(player,g_phys_native6698.prepareGestureC10R18);
    NativeTimingCancelPending59c3();
    // Retail D4A7EC sets controller latches +38/+3C/+40 to CANCELLED (2).
    // prepare_to_attack only starts from 0. Its release handler 27C08F8
    // commits ONLY from 1, then resets to 0: release AFTER abort cannot fire.
    // Abort release alone never resets this latch (the old lost-swing bug).
    if(PhysCallAction6698(player,2,1)){
        (void)PhysCallAction6698(player,1,2);
        if(g_phys_native6698.actionNames[4])(void)PhysCallAction6698(player,4,2);
        (void)PhysCallAction6698(player,2,2);
    }
}
static void PhysCancelAttack117() noexcept {
    kcd2vr::kick5916c3::delivery.cancel();
    kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel);
    auto& n=g_phys_native6698;
    if(n.attackPrepared&&n.player&&(!n.natural117||NativePreparedOwnerMatches117(n.player)))PhysAbortAttack117(n.player);
    n.attackPrepared=false;n.prepareTickC10R18=n.prepareGestureC10R18=0;
}
#include "../subsystems/ranged_reload/bow_native_action.inl" // M2_OWNED_SOURCE
static void PhysCancelNative6698(bool releaseTransport166=true) noexcept {
    kcd2vr::bow_physical_aim_d65c::retire(); // includes a released shot still awaiting its native action callback
    PerfScope101R2 cancelTiming178r6(16);
    NativeTimingCancel178r22c2(); // ownership loss also revokes an already-admitted clock
    auto& n=g_phys_native6698;
    if(n.bowHeld){n.bowHeld=false;PhysBowCall6698(n.player,2);diagnostic_trace::Write("V66.109 BOW CANCEL native uncharge; no release/fire synthesis");}
    PhysCancelAttack117();
    n.nativeChargeCap=0;n.bowChargeObserved114=false;n.physicalBow175=false;n.bowGeneration175=0;n.bowBeginSample175=0;
    g_phys_charge_action175.store(nullptr,std::memory_order_release);
    g_phys_charge_player175.store(nullptr,std::memory_order_release);
    // An uncharge is not an equipment/menu/tracking ownership transition.
    // Otherwise held RT leaks back to XI for one frame, then takeover uncharges
    // again. True lifecycle losses retain the default transport release.
    if(releaseTransport166)g_phys_input6698=false;
}
static bool PhysLeftEquipmentBeginH36(const PhysEquipment6698&,const ph::Command&,bool precise=false) noexcept;
static bool PhysLeftEquipmentEnterH36(const PhysEquipment6698&,const ph::Command&) noexcept;
template<class Invoke> static bool PhysSelectEquipmentInvokeD48(const PhysEquipment6698&,const ph::Command&,Invoke&&);
template<class Invoke> static bool PhysLeftEquipmentInvokeH36(const PhysEquipment6698&,const ph::Command&,Invoke&&);
#include "../subsystems/throw_drop/native_equipment_leaves.inl" // M2_OWNED_SOURCE
#include "../subsystems/throw_drop/holster_retirement_h36.inl"
static bool PhysDeferRelease103(const PhysNativeOwner6698& owner,const ph::Command& command,std::uint64_t nativeFrame) noexcept {
    return ((command.kind==ph::CommandKind::Slash||command.kind==ph::CommandKind::Thrust||command.kind==ph::CommandKind::Pommel)&&owner.attackPrepared&&owner.prepareFrame==nativeFrame)||
        (command.kind==ph::CommandKind::BowFire&&owner.bowHeld&&owner.bowBeginFrame==nativeFrame);
}
#include "../subsystems/ranged_reload/bow_pending_release.inl" // M2_OWNED_SOURCE
static std::uint64_t g_rung1_prepare30c1=0,g_rung1_commit30c1=0,g_rung1_reject30c1=0;
static std::uint64_t g_rung1_cuts32c1=0,g_rung1_thrusts32c1=0;
static const char* g_rung1_last_reject30c1="NONE";
static void PhysRung1Summary30c1(const PhysFrame6698& f,std::uint64_t now,bool active,bool have,bool abort) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now))return;
    if(!g_combat_integrated178r22c2.load(std::memory_order_relaxed)&&!PhysicalCombat177Enabled())return;
    static std::uint64_t due=0;if(now<due)return;due=now+1000;
    const auto& d=f.output.melee118;char msg[1450]{};
    std::snprintf(msg,sizeof(msg),"V66.178R49C1 MELEE_FLOW active=%d haveFresh=%d nativeBody=%d abort=%d sample=%llu ageMs=%llu weaponOwned=%d release=%u invalidMask=%u handlingOnly=%d key=%016llX primary=%d tracked=%d guard=%d stroke=%s kind=%u travel=%.3f fastTravel=%.3f speed=%.3f prepared=%d dispatchedPrepare=%llu dispatchedCommit=%llu inputRejects=%llu lastReject=%s handSpeed=%.3f axisKnown=%d axisCos=%.3f modelAxisCos=%.3f guardOrigin59=%d handNet59=%.3f guardCutPass59=%d dispatchedCuts=%llu dispatchedThrusts=%llu cockedPrepares57=%llu zoneFeints57=%llu wall58{blocked=%llu slid=%llu backout=%llu stuck=%llu scrape=%llu} nativeBeginObserved=%llu nativeReleaseObserved=%llu inputClassUnobserved=%llu masterFactoryQueries=%llu masterGroupTrue=%llu masterPermit=%llu masterDenial=%s admission=UNCLAIMED damage=UNCLAIMED",
        active,have,animation_filter::nativeBodyOwned112.load(),abort,(unsigned long long)f.input.sample,
        (unsigned long long)(f.input.tick&&now>=f.input.tick?now-f.input.tick:UINT64_MAX),f.output.owned[0],f.output.releaseReason121[0],f.output.invalidMask55c1[0],f.output.handlingOnly55c1[0],(unsigned long long)f.profiles[0].key,f.profiles[0].primary,f.input.tracked[0],
        f.output.guard117,melee118::name(d.reason),unsigned(d.kind),d.progress,d.fastTravel,d.tipSpeed,g_phys_native6698.attackPrepared,
        (unsigned long long)g_rung1_prepare30c1,(unsigned long long)g_rung1_commit30c1,(unsigned long long)g_rung1_reject30c1,g_rung1_last_reject30c1,
        d.handSpeed,int(d.axisKnown),d.axisCos,d.modelAxisCos,int(d.guardedOrigin59),d.guardHandTravel59,int(d.guardCutPass59),
        (unsigned long long)g_rung1_cuts32c1,(unsigned long long)g_rung1_thrusts32c1,(unsigned long long)g_cocked_prepares57.load(),(unsigned long long)g_zone_feints57.load(),
        (unsigned long long)g_wall_blocked58.load(),(unsigned long long)g_wall_slid58.load(),(unsigned long long)g_wall_backout58.load(),(unsigned long long)g_wall_stuck58.load(),(unsigned long long)g_wall_scrape58.load(),
        (unsigned long long)kcd2vr::attack_xi178r8::beginObserved49.load(),
        (unsigned long long)kcd2vr::attack_xi178r8::releaseObserved49.load(),
        (unsigned long long)kcd2vr::attack_xi178r8::classUnobserved49.load(),
        (unsigned long long)kcd2vr::bridges168::factoryQueries49.load(),
        (unsigned long long)kcd2vr::bridges168::factoryGroupTrue49.load(),
        (unsigned long long)kcd2vr::bridges168::factoryPermitted49.load(),
        kcd2vr::bridges168::name(kcd2vr::bridges168::Denial(kcd2vr::bridges168::factoryDenial49.load())));
    diagnostic_trace::Write(msg);
}
#include "../subsystems/throw_drop/native_supply_publication.inl" // M2_OWNED_SOURCE
#include "../subsystems/throw_drop/legacy_supply_publication_t25.inl"
static void PhysPublishSupplyDispatchT25(const PhysFrame6698& f,const PhysEquipment6698& e,bool allowed,std::uint64_t now) noexcept {
    if constexpr(kcd2vr::ammo_service_t22::integrated)PhysPublishSupplyT2(f,e,allowed,now);
    else PhysPublishSupplyLegacyT25(f,e,allowed,now);
}
#include "../subsystems/physics/native_admission_retirement.inl" // M2_OWNED_SOURCE
// Native transport evidence is a disposition, not proof of native admission.
static void TraceHitCommandC54R3(const ph::Command& c,const char* stage,const char* reason,std::uint64_t now,bool called=false) noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
       (c.kind!=ph::CommandKind::Prepare&&c.kind!=ph::CommandKind::Slash&&c.kind!=ph::CommandKind::Thrust&&
        c.kind!=ph::CommandKind::Pommel&&c.kind!=ph::CommandKind::Abort&&c.kind!=ph::CommandKind::Zone57))return;
    char line[720]{};std::snprintf(line,sizeof(line),
        "C54R3 HIT_CHAIN stage=%s why=%s gesture=%llu sample=%llu tick=%llu ageMs=%llu key=%016llX generation=%llu epoch=%llu request=%llu side=%d command=%u zone=%d origin=%d held=%d recognizerAbort=%d called=%d nativeAcceptance=NOT_INFERRED writes=EXISTING_GAMEPLAY_ONLY",
        stage,reason,(unsigned long long)c.gesture59c3,(unsigned long long)c.sample,(unsigned long long)c.tick,
        (unsigned long long)(now>=c.tick?now-c.tick:UINT64_MAX),(unsigned long long)c.key,(unsigned long long)c.generation,
        (unsigned long long)c.bodyEpoch,(unsigned long long)c.epoch,c.side,unsigned(c.kind),c.zone117,c.originZone167,int(c.held57),int(c.recognizerAbort18),int(called));
    diagnostic_trace::Write(line);
}
enum class PhysCommandDecisionH8 {Continue,Wait,Discard};
// Called only after the existing command deadline/item/recovery checks. Waiting
// retains the original queue entry; it never dispatches an uncertain sample.
static PhysCommandDecisionH8 PhysCommandAuthorityH8(const PhysFrame6698& frame,ph::Command& c,
        PhysNativeOwner6698& n,std::uint64_t now,bool melee,bool cancel,bool kick,bool heldBusy) noexcept {
    if(melee&&!cancel&&!heldBusy&&
       ph::quarantined_command_h8(frame.input,frame.output,frame.profiles[c.side],c,now)){
        TraceHitCommandC54R3(c,"COMMAND_WAIT","CURRENT_MOTION_UNCERTAIN_ORIGINAL_COMMIT_RETAINED_H8",now);
        return PhysCommandDecisionH8::Wait;
    }
    if(melee&&!cancel&&(heldBusy||(!kick&&!ph::command_authority_h30(frame.input,frame.output,frame.profiles[c.side],c.side)))){
        TraceHitCommandC54R3(c,"COMMAND_DISCARD","CURRENT_COMPLETED_FRAME_HAS_NO_ATTACK_AUTHORITY",now);
        g_phys_commands6698.pop(c);if(n.attackPrepared)PhysCancelAttack117();return PhysCommandDecisionH8::Discard;
    }
    return PhysCommandDecisionH8::Continue;
}
// R163: the practice / quest weapon detector (R156 npc_force_block153::practiceWeapon, read by the R156 / R159 / R163 gates)
// was published only by the active physical path below, so with native controls / native body / a dialogue it went stale and
// the gates saw "no training fight" (REVIEW162_tut: practiceWeapon=0 from 22:15:54 while he held the wooden sword). the same
// read-only equipment snapshot is refreshed here every 250 ms while the physical path is inactive. no write, no command.
static void PhysPracticeRefreshR163(std::uint64_t now) noexcept {
    static std::uint64_t due=0;if(due&&now<due&&due-now<=250)return;due=now+250;
    PhysEquipment6698 e{};(void)PhysRefreshEquipment103(e,now);}
static void PhysNativeTickImpl6698() noexcept {
    // Copied F3/F7 payloads are formatted by the sole mod-worker consumer.
    // Native input, equipment and combat processing retain this callback.
    if(!g_phys_armed6698.load())return;
    std::unique_lock<std::recursive_mutex> ownerLock(g_phys_native_mutex6698,std::try_to_lock);if(!ownerLock.owns_lock())return;
    static thread_local bool busy=false;if(busy)return;busy=true;struct End{bool& b;~End(){b=false;}} end{busy};
    auto& n=g_phys_native6698;const auto nativeFrame=++n.dispatchFrame;const auto now=std::uint64_t(GetTickCount64());
    (void)NativeHeldObserveAdmissionsT4(now);
    const bool active=g_weapon_control_mode.load()>=4&&!animation_filter::nativeBodyOwned112.load()&&!g_phys_fault6698.load()&&g_gameplay_allowed.load()&&TrackingHeadUsable111(now)&&!diagnostics::lab::NativeReference();
    // Publish frame + command tail as one logical transaction. A transient
    // mailbox writer must not look like tracking loss or cancel a release before
    // its command has been published. Retain original capture age, never refresh.
    const PhysFrame6698 frame=PhysLatestComplete175(); // native calls may reenter charge and update the retained mailbox
    const bool have=frame.input.sample&&frame.request==g_weapon_request_epoch6694.load()&&frame.input.bodyEpoch==g_pose_epoch6689.load()&&frame.input.allowed&&now>=frame.input.tick&&now-frame.input.tick<=100;
    kcd2vr::action_time_owner59c11::TickWeaponDropT1(active&&have,reinterpret_cast<std::uintptr_t>(frame.player),frame.input.bodyEpoch,frame.request);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
        static std::uint64_t last=0,session=0;const auto current=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        if(session!=current||now<last||now-last>=1000){session=current;last=now;char line[700]{};
            std::snprintf(line,sizeof(line),"C54R3 HIT_CHAIN stage=NATIVE_FRAME session=%llu tick=%llu sample=%llu sourceTick=%llu ageMs=%llu active=%d completedFresh=%d mode=%d nativeBody=%d fault=%d gameplayAllowed=%d headUsable=%d nativeReference=%d request=%llu currentRequest=%llu epoch=%llu currentEpoch=%llu source=EXISTING_NATIVE_PUMP missingSourceSamples=NOT_INTERPRETED_AS_MISSED_HITS",
                (unsigned long long)current,(unsigned long long)now,(unsigned long long)frame.input.sample,(unsigned long long)frame.input.tick,
                (unsigned long long)(frame.input.tick&&now>=frame.input.tick?now-frame.input.tick:UINT64_MAX),int(active),int(have),g_weapon_control_mode.load(),
                int(animation_filter::nativeBodyOwned112.load()),int(g_phys_fault6698.load()),int(g_gameplay_allowed.load()),int(TrackingHeadUsable111(now)),int(diagnostics::lab::NativeReference()),
                (unsigned long long)frame.request,(unsigned long long)g_weapon_request_epoch6694.load(),(unsigned long long)frame.input.bodyEpoch,(unsigned long long)g_pose_epoch6689.load());diagnostic_trace::Write(line);}
    }
    // Consume cancellation even while native body ownership makes active=false.
    // Short-circuiting exchange left a stale abort for the first recovered frame.
    const bool abortRequested30c1=g_phys_abort6698.exchange(false);
    if(!active||abortRequested30c1||!have){PhysCancelNative6698();ph::Command c{};while(g_phys_commands6698.pop(c)){
        TraceHitCommandC54R3(c,"COMMAND_DISCARD",abortRequested30c1?"EXPLICIT_CONTEXT_ABORT":!have?"NO_CURRENT_COMPLETED_FRAME":"NATIVE_OWNER_OR_GAMEPLAY_UNAVAILABLE",now);}
        for(auto& key:g_weapon_release_pending_t1)key.store(0,std::memory_order_release);}
    PhysRung1Summary30c1(frame,now,active,have,abortRequested30c1);
    kcd2vr::vr_combat111::nativeEngageClipVisible178r33.store(combat_diag_probe::NativeOwned178r33(now),std::memory_order_relaxed); // V66.178R33: clip ownership follows the engagement latch
    if(!active){PhysPracticeRefreshR163(now);NativeHeldInvalidateT4();g_native_held_reason_t4.store(heldT4::Enabled()?HeldPhysicsSourceT4::Context:HeldPhysicsSourceT4::Disabled,std::memory_order_release);PhysPublishSupplyDispatchT25(frame,PhysEquipment6698{},false,now);g_phys_collision_owner110.store(nullptr,std::memory_order_release);return;}
    PhysEquipment6698 e{};const auto leftMembershipSnapshotH36=g_left_native_membership_h36.load(std::memory_order_acquire);
    static std::uint64_t equipmentGapSince135=0;
    {PerfScope101R2 equipmentTiming(4);
        if(!PhysRefreshEquipment103(e,now)){
            NativeHeldInvalidateT4();
            if(!equipmentGapSince135)equipmentGapSince135=now;
            // Do not turn one unreadable native snapshot into a visible ownership
            // transition. Commands are not advanced while the live snapshot is absent.
            if(now>=equipmentGapSince135&&now-equipmentGapSince135<=140){return;}
            g_phys_collision_owner110.store(nullptr,std::memory_order_release);g_phys_equipment6698.publish(PhysEquipment6698{});PhysCancelNative6698();return;
        }
        equipmentGapSince135=0;
    }
    {static std::uint64_t due121=0;
     if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&now>=due121){due121=now+5000;char msg[192]{};
      std::snprintf(msg,sizeof(msg),"V66.121 EQUIPMENT_COST liveSnapshots=%llu metadataRefreshes=%llu metadataPeriodMs=500 eligibility=LIVE",
       (unsigned long long)g_equipment_reads121,(unsigned long long)g_equipment_refresh121);diagnostic_trace::Write(msg);}}
    e.leftNativeMembershipH36=leftMembershipSnapshotH36; // sampled BEFORE the native equipment read; never retag an old empty-hand snapshot
    PhysPublishSupplyDispatchT25(frame,e,active&&have,now);
    PhysLeftEquipmentRefreshH36(e,now);
    NativeHeldCaptureT4(e,now);
    // T15 retirement recovery: F9 ON must not strand ordinary holsters after
    // its exact collider has retired and the native right hand is now empty.
    // Keep the original F9-OFF close-Grip rearm independent of pose availability.
    if(!heldT4::Enabled()&&!NativeHeldBusyT4()&&g_right_grip.load(std::memory_order_acquire)>=.55f)
        g_native_held_release_inhibit_t4.store(false,std::memory_order_release);
    if(heldT4::Enabled()&&g_native_held_release_inhibit_t4.load(std::memory_order_acquire)){
        const bool busyT15=NativeHeldBusyT4();const float gripT15=g_right_grip.load(std::memory_order_acquire);
        const bool currentT15=active&&have&&frame.input.tick&&now>=frame.input.tick&&now-frame.input.tick<=80&&
            frame.input.allowed&&frame.input.tracked[0]&&frame.input.intentTracked[0]&&TrackingHandUsable111(0,now)&&
            e.valid&&e.player&&e.manager&&e.player==frame.player&&e.manager==frame.manager&&
            e.epoch==frame.input.bodyEpoch&&e.request==frame.request&&frame.input.epoch==frame.request&&
            frame.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
            frame.input.bodyEpoch==g_pose_epoch6689.load(std::memory_order_acquire);
        heldT4::Achieved retiredT15{};
        const bool retiredProofT15=currentT15&&!busyT15&&e.handKnown[0]&&!e.handKey[0]&&gripT15<.25f&&
            heldT4::Read(retiredT15)&&retiredT15.colliderRetired;
        if(kcd2vr::release_rearm_t15::allow(busyT15,heldT4::Enabled(),e.handKnown[0],!e.handKey[0],
            currentT15,gripT15,retiredProofT15))
            g_native_held_release_inhibit_t4.store(false,std::memory_order_release);
    }
    // Cancel old prepared native combat even if coherent publication has paused
    // during creation/retirement. No duplicate native attack owner for F9.
    {static bool ownedT4=false;const bool currentT4=NativeHeldBusyT4();
     if(currentT4&&!ownedT4)PhysCancelNative6698();ownedT4=currentT4;}
    g_phys_equipment6698.publish(e);for(int i=0;i<8;++i)g_phys_relevant_id101[i].store(i<e.count?e.items[i].profile.entity:0,std::memory_order_relaxed);
    if(n.player&&(n.player!=e.player||n.request!=e.request||n.epoch!=e.epoch||(n.bowHeld&&e.handKey[1]!=n.key))){PhysCancelNative6698();}
    { // R1: combat components can be replaced without replacing the player.
        void* actor=nullptr;void* collisions=nullptr;void* back=nullptr;
        if(e.player&&Read(static_cast<unsigned char*>(e.player)+0x278,actor)&&PhysVft6698(actor,0x3A80610)&&
           Read(static_cast<unsigned char*>(actor)+0x1F8,back)&&back==e.player&&
           Read(static_cast<unsigned char*>(actor)+0x218,collisions)&&collisions&&
           Read(static_cast<unsigned char*>(collisions)+8,back)&&back==actor){
            g_phys_collision_tick112.store(now,std::memory_order_relaxed);
            g_phys_collision_owner110.store(collisions,std::memory_order_release);
        }
        else g_phys_collision_owner110.store(nullptr,std::memory_order_release);
    }
    n.player=e.player;n.request=e.request;n.epoch=e.epoch;
    if(n.attackPrepared&&e.handKey[0]!=n.attackKey)PhysCancelNative6698();
    if(kcd2vr::motion59c4::selected.load()){
        const auto misses=kcd2vr::motion59c4::attempts.expire(now);
        if(misses){char line[150]{};std::snprintf(line,sizeof(line),"R59C4 MOTION_OWNED misses=%u damage=NONE effort=UNIMPLEMENTED preview=1",misses);diagnostic_trace::Write(line);}
    }
    if(!have)return;
    ProximityCapture36(frame,now);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&FullWeaponMode120(g_weapon_control_mode.load())){
        static std::uint64_t last119=0;
        if(now-last119>=1000){last119=now;char msg[640]{};
            std::snprintf(msg,sizeof(msg),
                "V66.120 HANDLING F12=%d F3=%u policy=%s gripRequired=%d anchor=%d R={class:%d active:%d grip:%.2f contactKnown:%d blocked:%d} L={class:%d active:%d grip:%.2f contactKnown:%d blocked:%d} support=%d guard=%d bowHeld=%d bowDraw=%.3f releaseReasonRL=%u/%u physicalReload=XR34_SEPARATE_NATIVE_CLOCK_OWNER",
                g_weapon_control_mode.load(),diagnostics::lab::Get(diagnostics::lab::Key::F3),frame.input.coherentMelee119?"DELIBERATE_50CM":"UNAVAILABLE",frame.input.requirePrimaryGrip114,frame.input.controllerAnchor119,
                frame.profiles[0].classId,frame.output.active[0],frame.input.grip[0],frame.output.contactKnown[0],frame.output.blocked[0],
                frame.profiles[1].classId,frame.output.active[1],frame.input.grip[1],frame.output.contactKnown[1],frame.output.blocked[1],
                frame.output.support[0]||frame.output.support[1],frame.output.guard117,frame.output.bowHeld,frame.output.bowFraction,frame.output.releaseReason121[0],frame.output.releaseReason121[1]);
            diagnostic_trace::Write(msg);
            char acquisition[256]{};std::snprintf(acquisition,sizeof(acquisition),
                "V66.124 WEAPON_ACQUISITION sample=%llu seedReachExcessRL=%.5f/%.5f releaseReasonRL=%u/%u contactOrder=WEAPON_THEN_FREE_HANDS budget=18_RAYS_1000US",
                (unsigned long long)frame.input.sample,frame.output.acquisitionReachError124[0],frame.output.acquisitionReachError124[1],
                frame.output.releaseReason121[0],frame.output.releaseReason121[1]);
            diagnostic_trace::Write(acquisition);
        }
        {   // R59.16C10 SUPPORT_LATCH (Miki: two-hand grip "not possible outside combat"; 19:07 log: support=0 while the LEFT
            // hand held the shield 19:16-19:20, then one SUPPORT_CANDIDATE capture at 19:17:56 - no latch attempt was observable).
            // One line per second while the off hand is near the socket (< 35 cm) or squeezing, and one line on every
            // latch/release transition. Observation only; no rule changes. F3 capture (same gate as HANDLING).
            static std::uint64_t lastProbe59c10=0;static bool lastLatched59c10=false;static int lastReason59c10=-1;
            const auto& pr=frame.output.supportProbe59c10;
            if(pr.side>=0){
                const bool latched=pr.reason==0;
                const bool handNear=pr.gap<.35||pr.grip>.55;
                const bool transition=latched!=lastLatched59c10||(handNear&&pr.reason!=lastReason59c10);
                static const char* const kReason[]={"LATCHED","NO_SUPPORT_SOCKET","OTHER_HAND_HOLDS_ITEM","OTHER_HAND_UNTRACKED","REARM_OPEN_GRIP_FIRST","GRIP_BELOW_0.55","GAP_OVER_14CM_OR_OFF_AXIS_10CM","TWIST_REGION_OR_REACH","DWELL_OR_SWEEP","AUTO_SUPPORT_PATH"};
                if(transition||(handNear&&!latched&&now-lastProbe59c10>=1000)){
                    lastProbe59c10=now;lastLatched59c10=latched;lastReason59c10=pr.reason;
                    char sm[400]{};std::snprintf(sm,sizeof(sm),"R59.16C10 SUPPORT_LATCH side=%d state=%s reason=%s gapCm=%.1f grip=%.2f otherKeyed=%d otherTracked=%d socket=%d rearm=%d v120=%d positionOnly=%d reachExcessCm=%.1f guard=%d otherClass=%d axis=%d alongCm=%.1f",
                        pr.side,latched?"ON":"OFF",kReason[pr.reason<10?pr.reason:1],pr.gap*100,pr.grip,pr.otherKeyed?1:0,pr.otherTracked?1:0,pr.profileSupport?1:0,pr.rearm?1:0,pr.v120?1:0,pr.positionOnly?1:0,
                        pr.reachExcess<500?pr.reachExcess*100:999.0,frame.output.guard117?1:0,frame.profiles[pr.side>=0?1-pr.side:0].classId,pr.axis59c16?1:0,pr.along59c16*100);
                    diagnostic_trace::Write(sm);
                }
                // C39 SUPPORT_GRIP: F3 edge-only, one line when the support hand's exact refusal / latch kind changes while the hand
                // is near or squeezing. Names the geometric reason (shaft centre line, butt/max point, socket gap); no rule input.
                if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)){
                    static int lastDetail39=-1;static std::uint64_t lastKey39=0;
                    int d=0;
                    if(pr.reason==0)d=pr.mode39==3?3:pr.mode39==2?2:1;
                    else if(pr.reason==6){
                        if(!pr.axis59c16)d=10;
                        else if(!(pr.perp39<pr.perpLimit39))d=11;
                        else if(pr.along59c16<pr.tMin39-.05)d=12;
                        else if(pr.along59c16>pr.tMax39+.05)d=13;
                        else d=14;
                    }else d=20+pr.reason;
                    const auto key39=frame.profiles[pr.side].key;
                    if((handNear||pr.reason==0)&&(d!=lastDetail39||key39!=lastKey39)){
                        lastDetail39=d;lastKey39=key39;
                        static const char* const kKind[]={"NONE","LATCHED_SOCKET","LATCHED_AXIS","LATCHED_SHAFT_SLIDE"};
                        static const char* const kGeo[]={"SOCKET_GAP_OVER_14CM","OFF_SHAFT_CENTRE","BEYOND_BUTT_END","BEYOND_MAX_POINT","ON_PRIMARY_HAND_6CM"};
                        const char* name=d<4?kKind[d]:d<20?kGeo[d-10]:kReason[(d-20)<10?(d-20):1];
                        char gm[420]{};std::snprintf(gm,sizeof(gm),"C39 SUPPORT_GRIP key=%016llX class=%d reason=%s grip=%.2f mode=%d centreLine=%d perpCm=%.1f perpLimitCm=%.1f alongCm=%.1f butt..maxCm=%.1f..%.1f wristRadiusCm=%.1f socketGapCm=%.1f socketFirst=%d nativeSource=%d",
                            (unsigned long long)key39,frame.profiles[pr.side].classId,name,pr.grip,int(pr.mode39),pr.centre39?1:0,pr.perp39<500?pr.perp39*100:999.0,pr.perpLimit39*100,pr.along59c16*100,
                            pr.tMin39*100,pr.tMax39*100,pr.radius39*100,pr.socketGap39<500?pr.socketGap39*100:999.0,pr.socketNear39?1:0,int(frame.profiles[pr.side].supportSource37));
                        diagnostic_trace::Write(gm);
                    }
                }
            }
        }
    }
    if(n.attackPrepared&&n.natural117!=frame.input.naturalCombat117)PhysCancelNative6698();
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&g_weapon_control_mode.load()>=5){
        static std::uint64_t seen119[2]{},due119[2]{};
        for(int side=0;side<2;++side){
            const auto& c=frame.output.contact119[side];
            if(frame.output.active[side]&&frame.output.blocked[side]&&frame.input.sample!=seen119[side]&&now>=due119[side]){
                seen119[side]=frame.input.sample;due119[side]=now+120;char msg[512]{};
                const bool geometry=c.known&&c.plane&&!c.limited115;
                const double unknown=std::numeric_limits<double>::quiet_NaN();
                const auto p=geometry?ph::point(PhysWorld6698(frame.world),c.point*frame.world[7]):pm::Vec3{unknown,unknown,unknown};
                const auto n=geometry?pm::rotate(PhysWorld6698(frame.world).q,c.normal):pm::Vec3{unknown,unknown,unknown};
                std::snprintf(msg,sizeof(msg),
                    "V66.119 WEAPON_CONTACT side=%d sample=%llu known=%d plane=%d deferred=%d fraction=%.4f incoming=%.3f pointWorld={%.3f,%.3f,%.3f} normalWorld={%.3f,%.3f,%.3f} geometry=%s response=%s material=UNRESOLVED nativeHit=NOT_INFERRED",
                    side,(unsigned long long)frame.input.sample,c.known,c.plane,c.limited115,c.fraction,frame.output.impactSpeed[side],p.x,p.y,p.z,n.x,n.y,n.z,geometry?"VALIDATED_RAY":"UNAVAILABLE",c.limited115?"DEFERRED_ROTATION":!c.known?"QUERY_UNAVAILABLE":"GENERIC_STATIC");
                diagnostic_trace::Write(msg);
            }
        }
    }
    if(n.attackPrepared&&n.natural117&&frame.profiles[0].generation!=n.attackGeneration117)PhysCancelNative6698();
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&frame.input.naturalCombat117){
        static std::uint64_t sample118=0,logged118=0;static melee118::Reason reason118=melee118::Reason::Idle;
        const auto& d=frame.output.melee118;
        const bool edge=d.reason==melee118::Reason::Prepared||d.reason==melee118::Reason::Committed;
        if(frame.input.sample!=sample118&&(edge||(d.reason!=reason118&&now-logged118>=250)||now-logged118>=1000)){
            sample118=frame.input.sample;logged118=now;reason118=d.reason;
            char msg[512]{};std::snprintf(msg,sizeof(msg),
                "V66.119 MELEE_INTENT sample=%llu reason=%s kind=%u handSpeed=%.3f tipSpeed=%.3f peak=%.3f net=%.3f fastTravel=%.3f coherence=%.3f ageMs=%.1f axisKnown=%d axisCos=%.3f modelAxisCos=%.3f guardOrigin59=%d handNet59=%.3f guardCutPass59=%d nativeHit=NOT_INFERRED",
                (unsigned long long)frame.input.sample,melee118::name(d.reason),unsigned(d.kind),d.handSpeed,d.tipSpeed,d.peak,d.progress,d.fastTravel,d.coherence,d.age*1000,
                int(d.axisKnown),d.axisCos,d.modelAxisCos,int(d.guardedOrigin59),d.guardHandTravel59,int(d.guardCutPass59));
            diagnostic_trace::Write(msg);
        }
    }
    if((diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now))&&frame.profiles[1].bow()){
        static std::uint64_t lastSample116=0,lastLog116=0;
        if(frame.input.sample!=lastSample116){
            lastSample116=frame.input.sample;
            const auto& b=frame.output.bow116;
            if(frame.output.commandCount||now<lastLog116||now-lastLog116>=1000){
                lastLog116=now;
                static constexpr const char* reasons[]{"NONE","WAITING","ACQUIRED","DRAWING","PRIMARY_RELEASED","UNREACHABLE","HAND_UNAVAILABLE","DEVIATION","HARD_SEPARATION","SHORT_RELEASE","FIRE_REQUESTED","GEOMETRY_UNAVAILABLE"};
                char msg[640]{};std::snprintf(msg,sizeof(msg),
                    "V66.116 BOW_STATE sample=%llu reason=%s gap=%.3f along=%.3f lateral=%.3f maxDraw=%.3f gripRL=%.3f/%.3f needsQuiver=%d rearm=%d physicalHeld=%d nativeHeld=%d chargeObserved=%d nativeCap=%.3f arrowSocket=UNVERIFIED triggerR=%.3f nativeTrigger=%d",
                    (unsigned long long)frame.input.sample,reasons[unsigned(b.reason)],b.gap,b.along,b.lateral,b.maxDraw,
                    frame.input.grip[0],frame.input.grip[1],b.needsQuiver,b.rearm,frame.output.bowHeld,n.bowHeld,n.bowChargeObserved114,double(n.nativeChargeCap),frame.input.trigger[0],frame.input.nativeBowTrigger126);
                diagnostic_trace::Write(msg);
            }
        }
    }
    if(n.bowHeld&&(!frame.input.tracked[0]||!frame.input.tracked[1]||!g_right_valid.load()||!g_left_valid.load()))PhysCancelNative6698();
    bool owns=false;for(int side=0;side<2;++side)owns=owns||(frame.output.active[side]&&((FullWeaponMode120(g_weapon_control_mode.load())&&frame.profiles[side].melee())||(BowHandlingMode59c9(g_weapon_control_mode.load())&&frame.profiles[side].bow())));
    if(owns&&!g_phys_input6698.load()){
        // Cancel any previous stock held-button action BEFORE the transport sees
        // ownership and releases its old trigger. Never turn takeover into fire.
        if(frame.profiles[1].bow())PhysBowCall6698(e.player,2);
        else if(!(frame.output.guard117&&NativePhysicalGuardHeld117(e.player)))PhysAbortAttack117(e.player);
    }
    if(g_phys_fault6698.load())return;
    g_phys_input_tick6698=now;g_phys_input6698=owns;
    auto& kickDelivery=kcd2vr::kick5916c3::delivery;
    const kcd2vr::kick5916c3::Owner liveKickOwner{reinterpret_cast<std::uintptr_t>(e.player),frame.profiles[0].key,
        frame.profiles[0].generation,frame.input.bodyEpoch,frame.request};
    if(kickDelivery.prepared&&!kickDelivery.active(liveKickOwner,now))PhysCancelAttack117();
    {const auto& a39=frame.output.arrival39;static std::uint64_t last39=0; // C39 DIRBLOCK: before this sample's commands become events
     if(!a39.gesture){last39=0;kcd2vr::dirblock39::mailbox.store(0,std::memory_order_release);} // C40: no stale prediction across aborted/new-owner gestures
     else if(a39.tick!=last39){last39=a39.tick;kcd2vr::dirblock39::mailbox.store(kcd2vr::dirblock39::pack(a39.gesture,a39.zone,a39.angle,a39.nearBoundary,a39.tick),std::memory_order_release);}}
    ph::Command c{};int budget=8;
    while(budget--&&g_phys_commands6698.peek(c)&&c.sample<=frame.input.sample){
        // A primary release cancels this item's older attack/string commands.
        // In particular a simultaneous string release must not fire a bow first.
        if(c.side>=0&&c.side<2&&c.kind!=ph::CommandKind::DropT1&&
           g_weapon_release_pending_t1[c.side].load(std::memory_order_acquire)==c.key){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","ITEM_DROP_OWNS_THIS_KEY",now);
            g_phys_commands6698.pop(c);continue;
        }
        // Let the original native frame observe prepare/begin before its release.
        // Do not drain both edges into one pre-frame callback, even after a delayed pose.
        if(PhysDeferRelease103(n,c,nativeFrame)&&!(kcd2vr::motion59c4::selected.load()&&!c.kick59&&
            (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel))){
            TraceHitCommandC54R3(c,"COMMAND_WAIT","PREPARE_AND_RELEASE_REQUIRE_DISTINCT_NATIVE_FRAMES",now);break;}
        if(g_phys_fault6698.load())break;
        // D82_FINAL_CONTACT_QUEUE_BEGIN: c is a local copy; the ring keeps the
        // original occurrence. Ordinary command expiry below is unchanged.
        const auto finalContactD82=c.epoch==e.request&&c.bodyEpoch==e.epoch&&c.side==0?
            NativeFinalContactCommandD82(c,frame,now):kcd2vr::final_combo_d72::QueueD82::Ordinary;
        if(finalContactD82==kcd2vr::final_combo_d72::QueueD82::Wait){
            TraceHitCommandC54R3(c,"COMMAND_WAIT","D82_FINAL_LIVE_SLOT_AWAITING_OWN_CONTACT",now);break;
        }
        if(finalContactD82==kcd2vr::final_combo_d72::QueueD82::Refused){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","D82_FINAL_CONTACT_OWNER_SLOT_OR_ONCE_ONLY",now);
            if(n.attackPrepared&&n.natural117&&n.prepareGestureC10R18==c.gesture59c3&&
               n.attackKey==c.key&&n.attackGeneration117==c.generation&&
               (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust))PhysCancelAttack117();
            g_phys_commands6698.pop(c);continue;
        }
        // D82_FINAL_CONTACT_QUEUE_END
        if(c.epoch!=e.request||c.bodyEpoch!=e.epoch||now<c.tick||now-c.tick>100||c.side<0||c.side>1){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD",c.epoch!=e.request?"REQUEST_CHANGED":c.bodyEpoch!=e.epoch?"POSE_EPOCH_CHANGED":now<c.tick?"FUTURE_COMMAND":now-c.tick>100?"COMMAND_EXPIRED_100MS":"INVALID_SIDE",now);
            if(n.attackPrepared&&n.natural117&&c.key==n.attackKey&&c.generation==n.attackGeneration117&&
               (c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel))PhysCancelAttack117();
            if(PhysEquipmentIntentD48(c)&&PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            g_phys_commands6698.pop(c);if(c.kind==ph::CommandKind::DropT1&&c.side>=0&&c.side<2)
                g_weapon_release_pending_t1[c.side].store(0,std::memory_order_release);continue;
        }
        // D48_DRAW_INTENT_CONSUMER_BEGIN
        // Release ends a queued, never-entered Draw. An entered H36 obligation
        // remains independent and continues its original completion observation.
        if(!PhysCurrentDrawIntentD48(frame.input,c)){
            if(PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);
            g_phys_commands6698.pop(c);continue;
        }
        // D48_DRAW_INTENT_CONSUMER_END
        // T24: native-empty preview still owns this same hand.
        // Recheck queued equipment at its existing native consumer too.
        if((c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&
           HandClaimBlockedT24(c.side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),e.epoch,e.request)){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
        if(c.side==0&&(c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&
           (NativeHeldBusyT4(0)||g_native_held_release_inhibit_t4.load(std::memory_order_acquire))){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
        // Draw/Holster reaches this consumer only after an actual ring enqueue.
        // Its producer already arbitrated Quick Talk before creating the request.
        // A later pointing pose cannot steal that admitted occurrence while the
        // native hand is still empty. Current item/epoch/deadline/native gates remain.
        kcd2vr::hand_only_door_h40::Admission doorDrawAdmissionH40;
        // CG1_DRAW_CONSUMER_BEGIN: Draw and its HS2 prerequisite stows share
        // one immutable both-hand occurrence, independent of current draw mode.
        const bool acquisitionCg1=c.kind==ph::CommandKind::Draw||
            (c.kind==ph::CommandKind::Holster&&c.corpseClaimCg1.mask);
        if(acquisitionCg1){
            if(c.corpseClaimCg1.mask!=3u){PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
            const unsigned doorPolicyCg1=c.kind==ph::CommandKind::Draw?(1u<<c.side):0u;
            if(kcd2vr::hand_only_door_h40::Reserved()&doorPolicyCg1)break;
            if(!doorDrawAdmissionH40.take(3u,true))break;
            if(kcd2vr::hand_only_door_h40::Reserved()&doorPolicyCg1)break;
            if(!kcd2vr::corpse_grip_cg1::claims.current(c.corpseClaimCg1)){
                PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue; // permanently obsolete; no re-stamp/retry
            }
        }
        // CG1_DRAW_CONSUMER_END
        const auto equipmentFenceH36=PhysLeftEquipmentH36(frame,e,c);
        if(equipmentFenceH36==HolsterNativeH36::Wait)break;
        if(equipmentFenceH36==HolsterNativeH36::Discard){if(PhysCancelUnenteredEquipmentH36(e,c))PhysRejectUnenteredEquipmentD48(c);g_phys_commands6698.pop(c);continue;}
#include "../subsystems/throw_drop/native_equipment_commands.inl" // M2_OWNED_SOURCE
        if(e.handKey[c.side]!=c.key||e.heldIndex[c.side]<0||e.items[e.heldIndex[c.side]].profile.generation!=c.generation){g_phys_commands6698.pop(c);continue;}
        bool called=false;
        const bool cancel=c.kind==ph::CommandKind::Abort||c.kind==ph::CommandKind::BowCancel;
        // R59C4: comparison still requests native ordinary actions. Only the
        // explicitly selected no-damage scaffold records VR events instead.
        // Neither path is evidence of complete action-independent combat.
        // V66.178R33 (R31 review 1): ONE owner per gesture. The decision is taken ONCE at the Prepare (Henry out of
        // combat or unknown -> native engagement) and latched on the gesture key; the commit/abort of the same key
        // follows the latch instead of re-reading the combat flag (a prepare at inCombat=0 followed by a commit at
        // inCombat=1 used to be cancelled half-way). Geometry does not submit while the latch is live.
        const bool geometryRung33=combat_diag_probe::GeometryHitsEnabled178r27();
        if(!geometryRung33){g_latchedNative33=false;g_latchedKey33=0;}
        const bool meleeKind33=c.kind==ph::CommandKind::Prepare||c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel||c.kind==ph::CommandKind::Abort||c.kind==ph::CommandKind::Zone57;
        if(meleeKind33&&!cancel&&!c.kick59&&
           ((c.recoveryEpochH30&&c.recoveryEpochH30!=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire))||
            (!c.recoveryEpochH30&&frame.output.normalIntentH30[c.side].current))){
            // Ownership at enqueue is immutable. A native-frame gap across
            // real tracking/context loss must not retag an old input with the
            // new recovery epoch. Cancellation still drains normally.
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","ORIGINAL_COMMAND_OWNER_RETIRED_H30",now);
            g_phys_commands6698.pop(c);
            if(n.attackPrepared&&n.prepareGestureC10R18==c.gesture59c3&&n.attackKey==c.key)PhysCancelAttack117();
            continue;
        }
        // R59.16C4: an R3 kick needs a live, allowed, tracked player, not an owned (gripped) sword pose.
        const bool kickAuthority59c4=c.kick59&&c.side==0&&frame.input.allowed&&frame.input.attacks&&frame.input.tracked[0];
        const auto motionDecisionH8=PhysCommandAuthorityH8(frame,c,n,now,meleeKind33,cancel,kickAuthority59c4,NativeHeldBusyT4(c.side));
        if(motionDecisionH8==PhysCommandDecisionH8::Wait)break;
        if(motionDecisionH8==PhysCommandDecisionH8::Discard)continue;
        // D34 FE: a post-commit HYBRID feint request. It owns no preparation, so it never reaches the prepared-input
        // lifecycle below and a refusal never cancels the committed native attack.
        if(c.feintFE){g_phys_commands6698.pop(c);(void)NativeHybridFeintFE(e.player,c,frame,n,now);continue;}
        // C10R19C1: copied command energy joins its full owner identity only after all native-side command filters.
        const kcd2vr::motion59c4::Key effortKey19{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,
            c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(e.player)};
        if(!c.kick59&&meleeKind33){
            if(c.kind==ph::CommandKind::Abort&&!c.recognizerAbort18)kcd2vr::swing_effort59c14::ledger.cancel(effortKey19);
            else if(c.effortKnown19&&(c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust))
                (void)kcd2vr::swing_effort59c14::ledger.publish(effortKey19,c.effortScale19,c.effortMass19,c.effortSupport19,c.tick);
        }
        if(meleeKind33&&!c.kick59&&!cancel&&!c.held57&&kcd2vr::routerH3::mode.load(std::memory_order_relaxed)){
            static kcd2vr::routerH3::Observer observerH3;
            const bool knownH3=g_native_state_ok178.load(std::memory_order_acquire)&&
                g_native_state_epoch178.load(std::memory_order_acquire)==c.bodyEpoch&&
                kcd2vr::motion59c4::fresh(g_native_busy_tick178.load(std::memory_order_acquire),now,100);
            const kcd2vr::routerH3::Observation observationH3{effortKey19,
                kcd2vr::motion59c4::selected.load(std::memory_order_acquire),knownH3,
                g_native_busy178.load(std::memory_order_acquire),g_native_phase178.load(),g_native_type178.load(),
                int(c.kind),kcd2vr::routerH3::mode.load(),c.timingSpeed178r22c2,c.effortScale19,c.effortKnown19};
            if(observerH3.take(observationH3)){char line[384]{};std::snprintf(line,sizeof(line),
                "H3 ROUTER_OBSERVE gesture=%llu weapon=%llu generation=%llu body=%llu request=%llu actual=%s stateKnown=%d busy=%d phase=%d type=%d firstKind=%d speed=%.3f scale=%.3f effortKnown=%d liveRouter=UNQUALIFIED admission=NATIVE noSecondProducer=1",
                (unsigned long long)c.gesture59c3,(unsigned long long)c.key,(unsigned long long)c.generation,
                (unsigned long long)c.bodyEpoch,(unsigned long long)c.epoch,observationH3.direct?"DIRECT":"HYBRID",
                knownH3,observationH3.busy,observationH3.phase,observationH3.type,int(c.kind),
                observationH3.speed,observationH3.scale,observationH3.effortKnown);diagnostic_trace::Write(line);}
        }
        if(kcd2vr::motion59c4::selected.load(std::memory_order_acquire)&&meleeKind33&&!c.kick59){
            namespace mo=kcd2vr::motion59c4;
            const mo::Key key{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,
                c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load(),reinterpret_cast<std::uintptr_t>(e.player)};
            // C35: a legacy rear/held cue is not a forward-motion attempt.
            // Refuse before ledger admission, not only after native publication.
            const bool ordinary=(c.kind==ph::CommandKind::Prepare||c.kind==ph::CommandKind::Slash||
                c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Zone57||c.kind==ph::CommandKind::Abort)&&
                !c.held57;
            const auto event=c.kind==ph::CommandKind::Prepare?mo::Event::Prepare:
                c.kind==ph::CommandKind::Abort?mo::Event::Cancel:c.kind==ph::CommandKind::Zone57?mo::Event::Feint:mo::Event::Commit;
            // C62's same cut has a newly qualified travel sector. Keep the
            // original spatial origin for other consumers, but do not replace
            // its actual Feint/Commit zone with the old starting-hand sector.
            const int origin=c.cutFeint62&&c.stroke117==ph::CommandKind::Slash?c.zone117:
                c.originZone167>=0?c.originZone167:c.zone117;
            const bool accepted=ordinary&&mo::attempts.event(key,event,c.tick,now,origin,c.stroke117==ph::CommandKind::Thrust?1:0,
                event==mo::Event::Cancel&&c.recognizerAbort18); // atomic provenance; tracking/owner/native cancels never revive
            TraceHitCommandC54R3(c,"DIRECT_LEDGER",!ordinary?"NOT_ORDINARY_FORWARD_GESTURE":accepted?"ACCEPTED_EXISTING_VR_ATTEMPT_LEDGER":"LEDGER_REFUSED_IDENTITY_ORDER_LIFETIME_OR_DUPLICATE",now,accepted);
            g_phys_commands6698.pop(c);
            if(n.attackPrepared)PhysCancelAttack117();
            n.attackPrepared=false;
            if(accepted&&event==mo::Event::Prepare)NativeDirectPrepareGuardHandoff19c4r2(c,frame,now);
            if(accepted){char line[260]{};std::snprintf(line,sizeof(line),
                "R59C4 MOTION_OWNED attempt=%llu event=%u origin=%d technique=%d timing=VR nativeAttackRequest=NONE consequence=%s",
                (unsigned long long)key.gesture,unsigned(event),origin,c.stroke117==ph::CommandKind::Thrust?1:0,combat_diag_probe::VrStateConsequence596()?"VR_STATE_DRIVER":"NOT_YET_QUALIFIED");diagnostic_trace::Write(line);
                // R59.6: tell the native state machine (NPC AI block trigger) what the blade is doing - no action, no animation.
                const int technique=c.stroke117==ph::CommandKind::Thrust?1:0;const int zone=technique?5:origin;
                const int kind596=event==mo::Event::Prepare?1:event==mo::Event::Commit?2:event==mo::Event::Feint?(c.cutFeint62?6:3):4;
                if(!combat_diag_probe::VrStateEvent5912(key,c.tick,kind596,zone,technique))
                    diagnostic_trace::Write("R59.12C1 VR_STATE_EVENT_DROPPED reason=QUEUE_FULL_OR_IDENTITY noNativeAttackRequested=1");}
            continue;
        }
        bool nativeOwned33=c.kick59;
        if(!c.kick59&&geometryRung33&&meleeKind33&&CombatExperiment175Enabled()&&combat_diag_probe::GeometryLiveAction178r35()){
            // R59C3: every contact-owned gesture uses the upstream native input
            // lifecycle. A restored/orphaned commit must not fall through the
            // retired R33 telegraph-only path because its old latch was cleared.
            nativeOwned33=true;
        }else if(!c.kick59&&geometryRung33&&meleeKind33){
            auto& latchedKey33=g_latchedKey33;auto& latchedNative33=g_latchedNative33; // V66.178R36 (F04): file-scope, reset outside the rung
            if(c.kind==ph::CommandKind::Prepare){
                latchedKey33=c.key;latchedNative33=combat_diag_probe::GeometryEngageNative178r31()&&(combat_diag_probe::GeometryLiveAction178r35()||combat_diag_probe::HenryInCombat178r31(now)!=1); // V66.178R35: with the live action route every stroke is a native attack
                if(latchedNative33)combat_diag_probe::NativeOwnerLatch178r33(c.key,now+1500);
            }
            nativeOwned33=latchedNative33&&latchedKey33==c.key;
            if(nativeOwned33&&c.kind!=ph::CommandKind::Prepare&&c.kind!=ph::CommandKind::Zone57){latchedNative33=false;combat_diag_probe::NativeOwnerLatch178r33(c.key,now+(c.kind==ph::CommandKind::Abort?0:600));}
        }
        if(geometryRung33&&meleeKind33&&!nativeOwned33){
            // A feint is meaningful only while this consumer owns a native
            // preparation. It must never fall through as a geometry commit.
            if(c.kind==ph::CommandKind::Zone57){g_phys_commands6698.pop(c);continue;}
            // V66.178R34: the suppressed native lifecycle becomes the TELEGRAPH of the geometry swing (pending at prepare,
            // committed at the release, idle at abort) so the enemy AI can read and block it.
            if(c.kind==ph::CommandKind::Prepare)combat_diag_probe::GeometryTelegraph178r34(1,c.zone117,now);
            else if(c.kind==ph::CommandKind::Abort)combat_diag_probe::GeometryTelegraph178r34(0,-1,now);
            else combat_diag_probe::GeometryTelegraph178r34(2,c.stroke117==ph::CommandKind::Thrust?5:c.zone117,now);
            g_phys_commands6698.pop(c);if(n.attackPrepared)PhysCancelAttack117();n.attackPrepared=false;
            static std::uint64_t suppressed27=0;if((suppressed27++%40)==0){char m[160]{};std::snprintf(m,sizeof(m),"V66.178R27 NATIVE_ATTACK_SUPPRESSED kind=%d count=%llu (rung 6: geometry hits own damage)",int(c.kind),(unsigned long long)suppressed27);diagnostic_trace::Write(m);}
            continue;}
        if(geometryRung33&&nativeOwned33&&!c.kick59&&c.kind!=ph::CommandKind::Abort){
            static std::uint64_t last31=0;if(!last31||now-last31>=2000){last31=now;char m[260]{};std::snprintf(m,sizeof(m),"V66.178R33 GEOM_ENGAGE_NATIVE kind=%d henryInCombat=%d owner=NATIVE_LATCHED key=%016llX (rung 2: native attack lifecycle requested; live-route contact still requires native phase and handler admission)",int(c.kind),combat_diag_probe::HenryInCombat178r31(now),(unsigned long long)c.key);diagnostic_trace::Write(m);}
        }
        if(!cancel&&(!(FullWeaponMode120(g_weapon_control_mode.load())||(!meleeKind33&&frame.profiles[c.side].bow()&&BowHandlingMode59c9(g_weapon_control_mode.load())))||(!frame.output.active[c.side]&&!c.kick59)||!frame.input.tracked[c.side]||
            (frame.input.requirePrimaryGrip114&&!c.kick59&&frame.input.grip[c.side]<.25))){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","CURRENT_MODE_TRACKING_ACTIVE_OR_PRIMARY_GRIP_GATE",now);g_phys_commands6698.pop(c);PhysCancelNative6698();continue;} // R59.16C4: R3 needs no sword grip
        const bool meleeCommit=c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust||c.kind==ph::CommandKind::Pommel;
        if(c.cutFeint62&&meleeCommit&&CombatSynchronized59c10r9()&&
           !kcd2vr::motion59c4::hybridAttempts54.publicationAllowed(effortKey19,kcd2vr::motion59c4::Event::Commit,c.tick,now)){
            g_phys_commands6698.pop(c);PhysCancelAttack117();continue;
        }
        // The completed physical decision belongs to the attacking hand. A raised
        // offhand shield contributes to aggregate guard117, but cannot discard
        // the primary hand's already-qualified attack. Native guard ownership is
        // still released by the existing typed prepare/guard interlock.
        if((c.kind==ph::CommandKind::Prepare||meleeCommit)&&
            kcd2vr::kick5916c3::guardReject(frame.input.naturalCombat117,frame.output.guardBySide30c1[c.side],c.kick59)){
            TraceHitCommandC54R3(c,"COMMAND_DISCARD","ATTACKING_HAND_STILL_OWNS_NATIVE_GUARD",now);
            g_phys_commands6698.pop(c);PhysCancelAttack117();continue;
        }
        // A kick has its own once-only request, not a repaired/orphan sword commit.
        // R59.16C4: the request field is the weapon request epoch on BOTH sides (ticket: frame.request at R3).
        const kcd2vr::kick5916c3::Owner kickOwner{reinterpret_cast<std::uintptr_t>(e.player),c.key,c.generation,c.bodyEpoch,frame.request};
        if(c.kick59){
            auto& delivery=kcd2vr::kick5916c3::delivery;
            if(c.kind==ph::CommandKind::Prepare&&n.attackPrepared)PhysCancelAttack117();
            const bool accepted=c.kind==ph::CommandKind::Prepare?delivery.begin(c.kickTicket59c3,kickOwner,now):
                (meleeCommit&&n.attackPrepared&&delivery.release(c.kickTicket59c3,kickOwner,now));
            if(!accepted){g_phys_commands6698.pop(c);PhysCancelAttack117();delivery.cancel();
                char rejected[216]{};std::snprintf(rejected,sizeof(rejected),"R59.16C3 R3_KICK59 stage=CONSUMER_REJECT request=%llu reason=REQUEST_OWNER_ORDER_OR_DEADLINE nativeKick=NOT_INFERRED",(unsigned long long)c.kickTicket59c3.id);
                diagnostic_trace::Write(rejected);continue;}
        }
        if(meleeCommit&&!n.attackPrepared){
            // A prior prepare can have been cancelled/rejected independently of
            // this still-fresh intent. Preserve the commit across a native frame;
            // the old consumer popped it here and silently lost the whole swing.
            if([&]{PerfScope101R2 t(14);auto finisherScope=FinisherScopeCB1(c.stroke117!=ph::CommandKind::None?c.stroke117:c.kind,c.gesture59c3);return NativePhysicalAction117(e.player,c,frame,true);}()){
                n.attackPrepared=true;n.attackKey=c.key;n.prepareFrame=nativeFrame;
                n.stroke117=c.stroke117;n.zone117=c.zone117;n.natural117=frame.input.naturalCombat117;
                n.prepareSample117=c.sample;n.attackGeneration117=c.generation;RememberPrepareC10R18(n,c);
                TraceHitCommandC54R3(c,"COMMAND_WAIT","ORPHAN_COMMIT_PREPARE_RESTORED_ORIGINAL_DEADLINE",now,true);
                diagnostic_trace::Write("V66.116 MELEE_COMMIT deferred=PREPARE_RESTORED expires=ORIGINAL_100MS");
            }else if(NativeFinalComboDeferredD72(c,frame,now)){
                TraceHitCommandC54R3(c,"COMMAND_WAIT","FINAL_CONTACT_ORIGINAL_100MS",now);
            }else {TraceHitCommandC54R3(c,"COMMAND_DISCARD","ORPHAN_COMMIT_PREPARE_RESTORE_REFUSED",now);g_phys_commands6698.pop(c);}
            break;
        }
        const bool prepareQueueD72=c.kind==ph::CommandKind::Prepare;
        const bool deferredBeforeD72=prepareQueueD72&&NativeFinalComboDeferredD72(c,frame,now);
        // A D82 contact transaction has its own measured tick/sample. pop()
        // copies the OLD queued occurrence out, so never pop back into c here.
        if(!prepareQueueD72){ph::Command originalD82{};g_phys_commands6698.pop(originalD82);}
        switch(c.kind){
        case ph::CommandKind::Prepare:
            // Count successful publication below; receipt is still a separate counter.
            if(!deferredBeforeD72)kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel); // one policy for the unchanged final-contact wait
            // V66.109: physical intent enters the same upstream native PLAYER
            // OnAction lifecycle as ordinary controls.  This replaces V66.104's
            // direct C_CombatActor::RequestAttack-only path, which dispatched but
            // never produced NATIVE_ACTION_ENTERED in the latest headset run.
            if(n.attackPrepared&&frame.input.naturalCombat117&&
               (n.prepareSample117!=c.sample||n.stroke117!=c.stroke117||n.zone117!=c.zone117))PhysCancelAttack117();
            if(!n.attackPrepared){PerfScope101R2 t(14);auto finisherScope=FinisherScopeCB1(c.stroke117,c.gesture59c3);called=NativePhysicalAction117(e.player,c,frame,true);}
            if(called){if(c.held57)++g_cocked_prepares57;n.heldPrepare57=c.held57;n.attackPrepared=true;n.attackKey=c.key;n.prepareFrame=nativeFrame;n.stroke117=c.stroke117;n.zone117=c.zone117;n.natural117=frame.input.naturalCombat117;
                n.prepareSample117=c.sample;n.attackGeneration117=c.generation;RememberPrepareC10R18(n,c);}
            break;
        case ph::CommandKind::Slash:case ph::CommandKind::Thrust:case ph::CommandKind::Pommel:{
            // Retail controller profile: prepare_to_attack is PRESS-only and
            // attack_primary is RELEASE-only.  The queue/defer policy guarantees
            // the release crosses a native frame after prepare.  Never synthesize
            // an attack_primary press or a prepare release.
            // A successful optional refinement belongs only to this still-owned preparation.
            if(kcd2vr::blade_zone_d84::ownsRefinedRelease(n,c)) c.zone117=n.zone117;
            const bool heldPrimary57=c.held57&&n.heldPrepare57&&g_thrust_as_swing55.load()&&
                n.stroke117==ph::CommandKind::Slash&&
                (c.stroke117==ph::CommandKind::Slash||c.stroke117==ph::CommandKind::Thrust);
            if(frame.input.naturalCombat117&&((n.stroke117!=c.stroke117&&!heldPrimary57)||n.zone117!=c.zone117)&&
               !NativeFinalComboPreparedD72(c,frame,now)){
                TraceHitCommandC54R3(c,"COMMAND_DISCARD","PREPARED_TECHNIQUE_OR_ZONE_NO_LONGER_MATCHES",now);
                PhysCancelAttack117();break;
            }
            {PerfScope101R2 t(15);auto finisherScope=FinisherScopeCB1(c.kind,c.gesture59c3);called=NativePhysicalAction117(e.player,c,frame,false);}
            if(called)kcd2vr::zone_d85::noteSent(c.gesture59c3,c.zone117); // D85 zone: the zone this release carried (F3 ZONE row)
            if(called){PublishPhysicalStrike142(c);PublishPhysicalStrike176(c,frame);
                if(c.kind==ph::CommandKind::Slash&&!c.kick59&&!c.held57){n.committedGestureFE=c.gesture59c3;n.committedTickFE=now;}}
            else PhysCancelAttack117();
            n.attackPrepared=false;break;}
        case ph::CommandKind::Abort:
            PhysCancelAttack117();called=true;break;
        case ph::CommandKind::Zone57:{
            if(NativeFinalComboPreparedD72(c,frame,now))break; // authored final endpoint already announced; consume same-gesture refinement
            // V66.178R57 feint: the HELD preparation moved to another side. Re-announce the zone through the same exact
            // native zone call the prepare uses (NativeZone104 + observed state+0xF8); no press, no release, no new action.
            const bool refinement19c4r2=NativeForwardRefinement19c4r2(n,c,frame,now);
            // Native redirect notifications are synchronous and may replace
            // the prepared input. Retire only this source after a refusal,
            // never a new preparation acquired inside the native callback.
            const auto feintOwnerH20=n;
            const auto heldSourceH26=NativePreparedSourceGenerationH26();
            const auto sameFeintOwnerH20=[&](){return (!c.controlledHoldH26||heldSourceH26==NativePreparedSourceGenerationH26())&&n.attackPrepared&&feintOwnerH20.attackPrepared&&
                n.player==feintOwnerH20.player&&n.epoch==feintOwnerH20.epoch&&n.request==feintOwnerH20.request&&
                n.attackKey==feintOwnerH20.attackKey&&n.attackGeneration117==feintOwnerH20.attackGeneration117&&
                n.prepareGestureC10R18==feintOwnerH20.prepareGestureC10R18&&
                n.prepareSample117==feintOwnerH20.prepareSample117&&n.prepareTickC10R18==feintOwnerH20.prepareTickC10R18;};
            if(n.attackPrepared&&n.attackKey==c.key&&(c.zone117==0||c.zone117==1||c.zone117==2||c.zone117==5)&&
               NativeZoneOnly57(e.player,c,frame)){
                if(c.optionalBladeZoneD84&&!sameFeintOwnerH20())break; // never adopt a replacement native action
                if(c.optionalBladeZoneD84){n.bladeZoneGestureD84=c.gesture59c3;n.bladeZoneOriginalD84=n.zone117;}
                n.zone117=c.zone117;
                if(refinement19c4r2){n.stroke117=c.stroke117;if(c.cutFeint62)n.cutFeintUsed62=true;}
                called=true;++g_zone_feints57;}
            else{
                if(c.cutFeint62&&!c.optionalBladeZoneD84)(void)kcd2vr::motion59c4::hybridAttempts54.event(effortKey19,
                    kcd2vr::motion59c4::Event::Cancel,c.tick,now,c.zone117,0,false);
                if(kcd2vr::blade_zone_d84::cancelRefusedZone(c.optionalBladeZoneD84,sameFeintOwnerH20()))PhysCancelAttack117();
                if(c.optionalBladeZoneD84)TraceHitCommandC54R3(c,"ZONE_REFINEMENT_REFUSED","ORIGINAL_PREPARATION_NOT_CANCELLED",now);
            }
            break;}
#include "../subsystems/ranged_reload/native_bow_commands.inl" // M2_OWNED_SOURCE
        default:break;}
        if(prepareQueueD72){
            if(!called&&NativeFinalComboDeferredD72(c,frame,now)){
                TraceHitCommandC54R3(c,"COMMAND_WAIT","FINAL_CONTACT_ORIGINAL_100MS",now);break;
            }
            ph::Command originalD82{};g_phys_commands6698.pop(originalD82);
        }
        TraceHitCommandC54R3(c,"COMMAND_NATIVE_RETURN",called?"EXISTING_ACTION_CALL_RETURNED":"EXISTING_ACTION_REFUSED_OR_NOT_CALLED",now,called);
        if(c.kick59){
            if(!called)kcd2vr::kick5916c3::delivery.cancel();
            char line[256]{};std::snprintf(line,sizeof(line),
                "R59.16C3 R3_KICK59 stage=CONSUMER_%s request=%llu returned=%d selectedKick=UNVERIFIED route=COMPANION_ZONE3",
                c.kind==ph::CommandKind::Prepare?"PREPARE":"RELEASE",(unsigned long long)c.kickTicket59c3.id,called?1:0);
            diagnostic_trace::Write(line);
        }
        if(c.kind==ph::CommandKind::BowFire&&!called&&!g_phys_fault6698.load())
            g_bow_release_rejected166.publish(BowReleaseRejected166{c.key,c.generation,c.epoch,c.bodyEpoch,c.sample});
        if(called){char m[224]{};std::snprintf(m,sizeof(m),"V66.110 PLAYER-ACTION REQUEST kind=%u key=%016llX fraction=%.3f dispatch=RETURNED route=player+0xB88/OnAction physicalHitInjection=NONE",unsigned(c.kind),(unsigned long long)c.key,c.fraction);diagnostic_trace::Write(m);}
    }
    ph::Command queuedRelease{};
    const bool pendingBowRelease=g_phys_commands6698.peek(queuedRelease)&&PhysPendingBowRelease103(n,queuedRelease,frame,now);
    if(n.bowHeld&&!frame.output.bowHeld&&!pendingBowRelease)PhysCancelNative6698(false);
    // R3 is a once-only button request, independent of the primary weapon Grip.
    // Keep its preparation across the required native-frame boundary, only for
    // the same live ticket/owner and original deadline. Tracking and all outer
    // gameplay/body-ownership guards still revoke it. Ordinary strokes retain
    // the existing weapon-release cancellation policy.
    const bool liveKickPreparation=kickDelivery.active(liveKickOwner,now);
    if(n.attackPrepared&&(!frame.input.tracked[0]||(!liveKickPreparation&&
        (!frame.output.active[0]||(!frame.input.heldMotionAttacks&&frame.input.grip[0]<.25)))))PhysCancelNative6698();
}
#include "../subsystems/ranged_reload/bow_charge_prepare.inl" // M2_OWNED_SOURCE
static bool PhysCopySweep112(const QuatT28* source,QuatT28& out) noexcept {
    __try {return source&&Read(source,out)&&ph::finite(PhysPose6698(out));}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
// Reference identity is resolved only from an actual sweep callback. No F12
// arming, frame polling, native writes or cached NPC-as-player approximation.
static bool ReferenceSweepOwner113(void* collisions,void** observedActorC54R3=nullptr) noexcept {
    void* actor=nullptr;void* player=nullptr;void* manager=nullptr;void* back=nullptr;void* entity=nullptr;
    if(!collisions||!Read(static_cast<unsigned char*>(collisions)+8,actor)||!PhysVft6698(actor,0x3A80610)||
       !Read(static_cast<unsigned char*>(actor)+0x218,back)||back!=collisions||
       !Read(static_cast<unsigned char*>(actor)+0x1F8,player)||!PhysVft6698(player,0x3AA4B98)||
       !Read(static_cast<unsigned char*>(player)+0x278,back)||back!=actor||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||entity!=g_last_player_entity.load()||!PhysVft6698(entity,0x4072C78)||
       !Read(static_cast<unsigned char*>(player)+0x9D0,manager)||manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=player)return false;
    if(observedActorC54R3)*observedActorC54R3=actor;return true;
}
static std::atomic<std::uint64_t> g_contact_kept114{0},g_contact_limited114{0};
static std::atomic<std::uint64_t> g_contact_until114{0};
struct ContactIdentity114 {void* collisions=nullptr;void* entity=nullptr;std::uint32_t id=0;std::uint64_t tick=0,epoch=0;};
static kcd2vr::core::SnapshotMailbox<ContactIdentity114> g_contact_identity114;
static void PublishContactIdentity114(void* player,void* actor,void* entity) noexcept {
    ContactIdentity114 identity{};void* back=nullptr;
    if(player&&actor&&entity==g_last_player_entity.load()&&
       Read(static_cast<unsigned char*>(player)+0x30,identity.id)&&identity.id&&
       Read(static_cast<unsigned char*>(actor)+0x218,identity.collisions)&&PhysVft6698(identity.collisions,0x3B91300)&&
       Read(static_cast<unsigned char*>(identity.collisions)+8,back)&&back==actor){
        identity.entity=entity;identity.tick=GetTickCount64();identity.epoch=g_pose_epoch6689.load();
    }else identity={};
    g_contact_identity114.publish(identity);
}
// No native calls, allocation or formatting under CryPhysics' collision lock.
// Record Henry's outgoing contacts and contacts whose native target ID is Henry.
// Unrelated NPC battles take only the early identity path; no actor enumeration.
static void ObserveNativeContact114(void* owner,const void* source,const void* target,const void* raw) noexcept {
    namespace capture=kcd2vr::diagnostics::combat_capture;
    const auto mode=diagnostics::lab::Get(diagnostics::lab::Key::F3);
    const auto now=std::uint64_t(GetTickCount64());
    if((mode!=1&&mode!=3&&!PredictionCapture120()&&!F11Capture167(1)&&!capture::captureOnly168r1(now))||!retail_build112::Ready(WhBase())||(mode==1&&!capture::active(now))||
       (mode==3&&now>g_contact_until114.load())||
       !owner||!raw||reinterpret_cast<std::uintptr_t>(owner)<0x10)return;
    auto collisions=static_cast<unsigned char*>(owner)-0x10;
    ContactIdentity114 identity{};std::uint32_t targetId=0;
    if(!g_contact_identity114.read(identity)||!identity.id||!identity.tick||now<identity.tick||now-identity.tick>80||
       identity.epoch!=g_pose_epoch6689.load()||identity.entity!=g_last_player_entity.load())return;
    ReadScope101R2 readScope;
    if(!Read(static_cast<const unsigned char*>(raw)+0x24,targetId))return;
    // One bounded target-ID read rejects unrelated NPC combat before the costly
    // live identity chain. Cached identity is a prefilter, never write authority.
    if(targetId!=identity.id&&collisions!=identity.collisions)return;
    if(!PhysVft6698(owner,0x3B912E8)||!PhysVft6698(collisions,0x3B91300))return;
    void* manager=g_henry_item_attach_manager.load();void* player=nullptr;void* entity=nullptr;void* back=nullptr;
    std::uint32_t henry=0;
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||!PhysVft6698(player,0x3AA4B98)||
       !Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||entity!=g_last_player_entity.load()||!PhysVft6698(entity,0x4072C78)||
       !Read(static_cast<unsigned char*>(player)+0x30,henry)||henry!=identity.id)return;
    const bool incoming=targetId==henry;
    void* nativeActorC54R3=nullptr;
    if(!incoming&&!ReferenceSweepOwner113(collisions,&nativeActorC54R3))return;
    // Up to 50 incoming + 50 outgoing records/sec, aggregate across opponents.
    static std::atomic<std::uint64_t> due[2]{};auto next=due[incoming?1:0].load();
    if(now<next||!due[incoming?1:0].compare_exchange_strong(next,now+20)){++g_contact_limited114;return;}
    capture::Event e{};e.kind=capture::Kind::Contact;e.tick=now;e.mode=int(mode);e.id=int(targetId);
    e.actor=reinterpret_cast<std::uintptr_t>(collisions);e.key[0]=reinterpret_cast<std::uintptr_t>(source);e.key[1]=reinterpret_cast<std::uintptr_t>(target);
    e.flags=incoming?2u:1u;
    if(!incoming){kcd2vr::diagnostics::punch_c54r3::NativeContext context{};
        if(kcd2vr::diagnostics::punch_c54r3::readContext(context,now)){
            e.sequence=context.join.sample;e.poseEpoch=context.join.epoch;e.parent164[0]=context.join.onset;
            std::memcpy(e.current,&context.session,sizeof(context.session)); // unused Contact pose payload; exact F3 session
            e.parent164[1]=context.join.request;e.parent164[2]=context.join.player;e.aliasHash=context.nativeActor;e.layer=context.join.side;
            e.trackingFlags=context.nativeActor&&reinterpret_cast<std::uintptr_t>(nativeActorC54R3)==context.nativeActor&&context.join.player==reinterpret_cast<std::uintptr_t>(player)?1u:2u;
            capture::name(e,"C54R3_PUNCH_TEMPORAL");}}
    if(!Read(raw,e.contact114))return;
    if(capture::push(e,mode==3||PredictionCapture120()))++g_contact_kept114;
}
// V115R1: register with the existing exact-build callback instead of installing
// a second hook over its detour. Legacy enrichment/combo evidence remains alive.
static bool ArmNativeContact114() noexcept {
    return retail_build112::Ready(WhBase())&&combat_diag_probe::RegisterContactObserver115(&ObserveNativeContact114);
}
static void* __fastcall PhysNativeSweepProbe110(void* collisions,int handSlot,int boneId,const QuatT28* previous,const QuatT28* current,float dt) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::SweepProbe};
    ++g_phys_native_sweep_calls110;
    const auto now=std::uint64_t(GetTickCount64());
    const auto ownerTick=g_phys_collision_tick112.load();
    const bool local=collisions&&collisions==g_phys_collision_owner110.load(std::memory_order_acquire)&&
        ownerTick&&now>=ownerTick&&now-ownerTick<=100&&g_gameplay_allowed.load()&&g_weapon_control_mode.load()>=4;
    static std::atomic<std::uint64_t> nextLog{0};auto due=nextLog.load();
    const bool log=local&&diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,now)&&now>=due&&nextLog.compare_exchange_strong(due,now+500);
    QuatT28 prev{},cur{};
    const bool copied=log&&PhysCopySweep112(previous,prev)&&PhysCopySweep112(current,cur);
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    namespace capture=kcd2vr::diagnostics::combat_capture;
    capture::Event reference{};
    bool haveReference=false;
    if((diagnostics::lab::Get(diagnostics::lab::Key::F3)==1||F11Capture167(1)||capture::captureOnly168r1(now))&&capture::active(now)&&
       retail_build112::Ready(WhBase())){
        // Per caller and hand: a frequently-called prediction sweep must not
        // consume every observation slot before the live/recoil solver arrives.
        // 25 Hz per bucket, bounded at the old aggregate 200 records/sec ceiling.
        static std::atomic<std::uint64_t> nextReference[4][2]{};
        const auto rva=caller>=WhBase()?caller-WhBase():0;
        const int bucket=rva==0xA7C9C1?0:rva==0xA7EB58?1:rva==0x27C1695?2:3;
        const int side=handSlot==0?0:handSlot==1?1:-1;
        auto next=side>=0?nextReference[bucket][side].load():UINT64_MAX;
        void* nativeActorC54R3=nullptr;
        if(side>=0&&now>=next&&ReferenceSweepOwner113(collisions,&nativeActorC54R3)&&
           nextReference[bucket][side].compare_exchange_strong(next,now+40)){
            QuatT28 rp{},rc{};
            if(std::isfinite(dt)&&dt>0&&PhysCopySweep112(previous,rp)&&PhysCopySweep112(current,rc)){
                reference.kind=capture::Kind::Sweep;reference.tick=now;reference.id=boneId;reference.layer=handSlot;
                reference.actor=reinterpret_cast<std::uintptr_t>(collisions);reference.key[0]=caller>=WhBase()?caller-WhBase():0;reference.value=dt;
                for(int i=0;i<3;++i){reference.previous[i]=rp.t[i];reference.current[i]=rc.t[i];}
                for(int i=0;i<4;++i){reference.previous[i+3]=rp.q[i];reference.current[i+3]=rc.q[i];}
                kcd2vr::diagnostics::punch_c54r3::NativeContext context{};
                if(kcd2vr::diagnostics::punch_c54r3::readContext(context,now)){
                    reference.sequence=context.join.sample;reference.poseEpoch=context.join.epoch;reference.parent164[0]=context.join.onset;
                    reference.key[1]=context.session; // unused Sweep key; prevents a later F3 capture inheriting this context
                    reference.parent164[1]=context.join.request;reference.parent164[2]=context.join.player;reference.aliasHash=context.nativeActor;reference.mode=context.join.side;
                    reference.trackingFlags=context.nativeActor&&reinterpret_cast<std::uintptr_t>(nativeActorC54R3)==context.nativeActor?1u:2u;
                    capture::name(reference,"C54R3_PUNCH_TEMPORAL");}
                haveReference=true;
            }
        }
    }
    QuatT28 vrPrevious{},vrCurrent{};
    const bool prediction120=PredictionSweep120(caller,collisions,handSlot,boneId);
    auto context164=diagnostics::trace164::for_owner(reinterpret_cast<std::uintptr_t>(collisions));
    if(prediction120&&context164.prediction)context164.sweep=diagnostics::trace164::serial();else context164={};
    diagnostics::trace164::Scope sweepScope164(context164);
    const bool replaced=local&&NativeTrajectoryPrepare112(caller,collisions,handSlot,boneId,previous,current,dt,vrPrevious,vrCurrent);
    // Every A7 call masks any enclosing gameplay scope, including unrelated
    // NPC/recoil/live-bone calls. Only the already-redirected predictor can grant
    // native contact authority; no recorder IDs or borrowed pointer cache.
    kcd2vr::contact176::SweepScope authority176(PredictionAuthority176(caller,collisions,handSlot,boneId,dt));
    void* result=g_phys_native_sweep_original110?g_phys_native_sweep_original110(collisions,handSlot,boneId,
        replaced?&vrPrevious:previous,replaced?&vrCurrent:current,dt):nullptr;
    if(haveReference){reference.flags=result?1u:0u;capture::push(reference);}
    if(!local)return result;
    ++g_phys_native_sweep_local110;if(result)++g_phys_native_sweep_hits110;
    if(!copied)return result;
    PhysFrame6698 frame{};ph::Pose vr{},nativeWeapon{},offset{};bool vrValid=false;int socketJoint=-1;
    std::uint64_t key=0,generation=0,age=UINT64_MAX;
    const int side=handSlot==0?0:handSlot==1?1:-1;
    if(side>=0&&g_phys_frame6698.read(frame)&&frame.request==g_weapon_request_epoch6694.load()&&
       frame.input.bodyEpoch==g_pose_epoch6689.load()&&now>=frame.input.tick&&now-frame.input.tick<=80&&
       frame.input.allowed&&frame.output.active[side]&&frame.output.owned[side]&&frame.profiles[side].melee()){
        key=frame.profiles[side].key;generation=frame.profiles[side].generation;age=now-frame.input.tick;
        vr=frame.output.weapons[side];vr.p=vr.p*frame.world[7];vr=ph::compose(PhysWorld6698(frame.world),vr);vrValid=ph::finite(vr);
        nativeWeapon=frame.profiles[side].nativeWeapon;nativeWeapon.p=nativeWeapon.p*frame.world[7];
        nativeWeapon=ph::compose(PhysWorld6698(frame.world),nativeWeapon);
        if(ph::finite(nativeWeapon))offset=ph::compose(ph::inverse(nativeWeapon),PhysPose6698(cur));
        PhysSocketBank101 sockets{};
        if(g_phys_sockets101.read(sockets))for(const auto& socket:sockets.sockets)
            if(socket.valid&&socket.key==key&&socket.generation==generation&&now>=socket.tick&&now-socket.tick<=100)socketJoint=socket.joint;
    }
    char m[1536]{};std::snprintf(m,sizeof(m),
        "V66.112-NATIVE-WEAPON-SWEEP callerRva=%llX owner=HENRY handSlot=%d sideMapping=DIAGNOSTIC_ONLY bone=%d socketJoint=%d key=%016llX generation=%llu sample=%llu ageMs=%llu dt=%.5f prev=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) cur=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) vrValid=%d vr=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) nativeWeaponToBone=(%.5f,%.5f,%.5f;%.6f,%.6f,%.6f,%.6f) resultNonzero=%d acceptedDamage=UNCLAIMED mutation=%s",
        (unsigned long long)(caller>=WhBase()?caller-WhBase():0),handSlot,boneId,socketJoint,(unsigned long long)key,(unsigned long long)generation,(unsigned long long)frame.input.sample,(unsigned long long)age,double(dt),
        double(prev.t[0]),double(prev.t[1]),double(prev.t[2]),double(prev.q[0]),double(prev.q[1]),double(prev.q[2]),double(prev.q[3]),
        double(cur.t[0]),double(cur.t[1]),double(cur.t[2]),double(cur.q[0]),double(cur.q[1]),double(cur.q[2]),double(cur.q[3]),
        vrValid?1:0,vr.p.x,vr.p.y,vr.p.z,vr.q.x,vr.q.y,vr.q.z,vr.q.w,offset.p.x,offset.p.y,offset.p.z,offset.q.x,offset.q.y,offset.q.z,offset.q.w,result?1:0,prediction120?"PREDICTION_SOCKET_CORRELATED":replaced?"LIVE_BONE_SOCKET_CORRELATED":"NONE");
    diagnostic_trace::Write(m);return result;
}
#include "../subsystems/ranged_reload/bow_charge_observer.inl" // M2_OWNED_SOURCE
static bool ArmNativeSweep113() noexcept {
    const auto base=WhBase();
    if(!retail_build112::Verify(base)||!SupportedWhBuild(base))return false;
    if(!g_phys_native_sweep_original110){
        const unsigned char expectedSweep[16]{0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x20,0x44};
        unsigned char actualSweep[16]{};if(!Read(reinterpret_cast<void*>(base+0xA7A124),actualSweep)||std::memcmp(actualSweep,expectedSweep,16)){diagnostic_trace::Write("V66.110 native weapon sweep fingerprint mismatch; physical mode fail-closed");return false;}
        DetourModKit::hook::InlineRequest sweepReq{"kcd2vr_v66110_native_weapon_sweep_probe",DetourModKit::hook::Target{DetourModKit::Address{base+0xA7A124}},DetourModKit::hook::Options{}};
        auto sh=DetourModKit::hook::inline_at(std::move(sweepReq),&PhysNativeSweepProbe110);if(!sh)return false;
        g_phys_native_sweep_hook110=std::move(*sh);g_phys_native_sweep_original110=g_phys_native_sweep_hook110->original<PhysNativeSweepFn110>();
        if(!g_phys_native_sweep_original110||!g_phys_native_sweep_hook110->enable()){g_phys_native_sweep_hook110.reset();g_phys_native_sweep_original110=nullptr;return false;}
        diagnostic_trace::Write("V66.112 native sweep bridge installed WHGame+0xA7A124; default=READ_ONLY; F3=3 may replace correlated live-bone arguments");
    }
    return g_phys_native_sweep_original110!=nullptr;
}
static bool PhysArmImpl6698() noexcept {
    if(g_phys_fault6698.load())return false;if(g_phys_armed6698.load())return true;
    const auto base=WhBase();if(!SupportedWhBuild(base)||!g_entity_attachment_update_original||!PhysSocketContract101(base)){diagnostic_trace::Write("V66.101R1 PHYSICAL socket contract unavailable; no layout guessing");return false;}
    for(const auto& f:ph::nativeFingerprint){if(f.rva==0x8E5324&&OwnCombatInputHook101(base))continue;unsigned char bytes[16]{};if(!Read(reinterpret_cast<void*>(base+f.rva),bytes)||std::memcmp(bytes,f.bytes.data(),16)){diagnostic_trace::Write("V66.98 PHYSICAL native fingerprint mismatch; mode not enabled");return false;}}
    if(!native_stereo_outer_lab::EnsurePhysicalFramePump6698())return false;
    if(!ArmNativeSweep113())return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_v6698_native_bow_charge",DetourModKit::hook::Target{DetourModKit::Address{base+0x28A1044}},DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&PhysChargeDetour6698);if(!h)return false;
    g_phys_charge_hook6698=std::move(*h);g_phys_charge_original6698=g_phys_charge_hook6698->original<PhysChargeFn6698>();
    if(!g_phys_charge_original6698||!g_phys_charge_hook6698->enable()){g_phys_charge_hook6698.reset();g_phys_charge_original6698=nullptr;return false;}
    g_phys_armed6698=true;diagnostic_trace::Write("V66.110 PHYSICAL ARMED staged-native-actions + body-relative intent + bounded support; contact=STATIC_FEATURE_RAYS; nativeSweep=A7A124-DEFAULT_READ_ONLY_F3_3_EXPERIMENT");return true;
}
// Capture command edges at publication: a native-frame mailbox reader can miss
// a one-frame abort, and the old 250 ms intent logger suppresses terminal reasons.
// Bounded POD queue only; no formatting, queries or file I/O on the pose thread.
static void ObserveMeleeCommand174r2(const ph::Command& c,const ph::Output& out,bool queued) noexcept {
    namespace cc=kcd2vr::diagnostics::combat_capture;
    if(!cc::active(c.tick)||c.side<0||c.side>1)return;
    if(c.kind!=ph::CommandKind::Prepare&&c.kind!=ph::CommandKind::Slash&&
       c.kind!=ph::CommandKind::Thrust&&c.kind!=ph::CommandKind::Pommel&&c.kind!=ph::CommandKind::Abort)return;
    cc::Event e{};e.kind=cc::Kind::MeleeCommand174r2;e.tick=c.tick;e.sequence=c.sample;
    e.poseEpoch=c.bodyEpoch;e.key[0]=c.key;e.key[1]=c.epoch;e.aliasHash=c.generation;
    e.mode=int(c.kind);e.id=c.side;e.layer=int(out.releaseReason121[c.side]);
    e.flags=(out.contactKnown[c.side]?1u:0u)|(out.blocked[c.side]?2u:0u)|
        (out.guard117?4u:0u)|(out.active[c.side]?8u:0u)|(queued?16u:0u);
    // The current melee observation describes the right-hand detector only.
    // Left-hand/context aborts must not inherit an unrelated right-hand reason.
    if(c.side==0){const auto& d=out.melee118;cc::name(e,melee118::name(d.reason));
        e.value=float(d.progress);e.phase=float(d.fastTravel);e.hand[0]=float(d.handSpeed);
        e.hand[1]=float(d.tipSpeed);e.hand[2]=float(d.age*1000);e.hand[3]=float(d.coherence);
        e.current[0]=float(d.axisCos);e.current[1]=float(d.modelAxisCos);e.current[2]=d.axisKnown?1.f:0.f;
        e.current[3]=float(d.guardHandTravel59);e.current[4]=d.guardedOrigin59?1.f:0.f;e.current[5]=d.guardCutPass59?1.f:0.f;}
    else cc::name(e,"LEFT_CONTEXT");
    cc::push(e);
}
static void ObservePhysicalImpact177(const PhysFrame6698& f) noexcept {
    namespace cc=kcd2vr::diagnostics::combat_capture;
    if(!f.input.physicalCombat177||!diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,f.input.tick))return;
    for(int side=0;side<2;++side){
        const auto& impact=f.output.impact177[side];const auto& contact=f.output.contact119[side];
        if(impact.impulse<=0||!contact.known||!contact.plane||contact.limited115||!f.output.owned[side])continue;
        cc::Event e{};e.kind=cc::Kind::PhysicalImpact177;e.tick=f.input.tick;e.sequence=f.input.sample;
        e.poseEpoch=f.input.bodyEpoch;e.key[0]=f.profiles[side].key;e.key[1]=f.request;e.aliasHash=f.profiles[side].generation;
        e.mode=side;e.id=int(contact.native172.entityId);e.layer=contact.native172.part;e.flags=f.output.support[side]?1:0;
        e.value=float(impact.energy);e.phase=float(impact.effectiveMass);e.hand[0]=float(impact.speed);e.hand[1]=float(impact.impulse);
        const auto point=ph::point(f.input.world,contact.point*f.input.worldScale);
        e.current[0]=float(point.x);e.current[1]=float(point.y);e.current[2]=float(point.z);
        cc::push(e); // bounded POD queue; formatting/file I/O only in worker
    }
}
// Recording energy is not a debit or a hit grant. Contact-led attempts can be
// consumed before recognizer Commit, so their immutable cost must precede frame
// publication. Settlement still requires the existing Committed/Consumed ledger
// and exact native raise/lower transaction, independently of contact admission.
// C54: consume the completed physical producer's POD events before publishing
// its contact frame. Native admission/input filtering can neither create nor
// erase this identity. No native calls, pointer readback or I/O on this path.
static void PhysPublishHybrid54(const PhysFrame6698& f) noexcept {
    namespace mo=kcd2vr::motion59c4;
    if(mo::selected.load(std::memory_order_acquire)||!CombatSynchronized59c10r9()||
       !f.input.physicalCombat177||f.input.motionOwned59c5||!f.player||!f.request||
       f.request!=f.input.epoch||!f.input.bodyEpoch||!f.input.tick||!f.input.sample)return;
    for(unsigned i=0;i<f.output.commandCount&&i<f.output.commands.size();++i){const auto& c=f.output.commands[i];
        if(c.kick59||c.side!=0||!c.gesture59c3||c.key!=f.profiles[0].key||c.generation!=f.profiles[0].generation||
           c.epoch!=f.request||c.bodyEpoch!=f.input.bodyEpoch||c.tick!=f.input.tick||c.sample!=f.input.sample)continue;
        const bool cancel=c.kind==ph::CommandKind::Abort;
        const bool commit=c.kind==ph::CommandKind::Slash||c.kind==ph::CommandKind::Thrust;
        if(!cancel&&!commit&&c.kind!=ph::CommandKind::Prepare&&c.kind!=ph::CommandKind::Zone57)continue;
        if(!cancel&&!ph::command_authority_h30(f.input,f.output,f.profiles[c.side],c.side))continue;
        if(c.held57&&!commit)continue; // rear preparation is not forward physical onset
        const mo::Key key{c.gesture59c3,c.key,c.generation,c.bodyEpoch,c.epoch,
            c.recoveryEpochH30?c.recoveryEpochH30:kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(f.player)};
        const auto event=c.kind==ph::CommandKind::Prepare?mo::Event::Prepare:
            cancel?mo::Event::Cancel:c.kind==ph::CommandKind::Zone57?mo::Event::Feint:mo::Event::Commit;
        (void)mo::hybridAttempts54.event(key,event,c.tick,f.input.tick,
            c.cutFeint62&&c.stroke117==ph::CommandKind::Slash?c.zone117:c.originZone167>=0?c.originZone167:c.zone117,
            c.stroke117==ph::CommandKind::Thrust?1:0,cancel&&c.recognizerAbort18);
    }
}
static void PhysPublishImpactEffort19(const PhysFrame6698& f,const PhysEquipment6698& equipment) noexcept {
    namespace mo=kcd2vr::motion59c4;
    const bool direct=mo::selected.load(std::memory_order_acquire)&&f.input.motionOwned59c5;
    const bool normal=!mo::selected.load(std::memory_order_acquire)&&CombatSynchronized59c10r9()&&
        ph::normal_intent_enabled_h30(f.input)&&!f.input.motionOwned59c5&&f.output.normalIntentH30[0].current;
    // A soft-recovery commit has no achieved-pose effort receipt. The original
    // checked impact may supply it later, under the SAME gesture/native cost
    // key. Never substitute controller speed or create another native debit.
    if((!direct&&!normal)||!f.input.physicalCombat177||
       !equipment.valid||!equipment.player||f.player!=equipment.player||f.request!=equipment.request||
       f.input.epoch!=equipment.request||f.input.bodyEpoch!=equipment.epoch||!f.input.tick||!f.input.sample||
       !equipment.tick||(f.input.tick>=equipment.tick?f.input.tick-equipment.tick:equipment.tick-f.input.tick)>100||
       equipment.count<0||equipment.count>int(equipment.items.size())||
       (f.input.haveWorld&&(f.publishWorldDeltaM122>.002||f.publishWorldDeltaRad122>.002||
           !std::isfinite(f.publishWorldDeltaM122)||!std::isfinite(f.publishWorldDeltaRad122)||
           !std::isfinite(f.world[7])||!std::isfinite(f.input.worldScale)||std::abs(double(f.world[7])-f.input.worldScale)>1e-5)))return;
    for(int side=0;side<2;++side){const auto& r=f.output.impactRecord178r30[side];const auto& p=f.profiles[side];
        const int item=equipment.heldIndex[side];
        if(!p.melee()||p.side!=side||!equipment.handKnown[side]||equipment.handKey[side]!=p.key||item<0||item>=equipment.count||
           !equipment.items[item].held||equipment.items[item].profile.key!=p.key||equipment.items[item].profile.generation!=p.generation)continue;
        if(!r.valid||!r.effortKnown19||!r.gesture59c3||!r.entityId||!r.collider||r.part<0||
           !std::isfinite(r.speed)||r.speed<.8||r.tick!=f.input.tick||r.sample!=f.input.sample||
           r.key!=p.key||r.generation!=p.generation||r.epoch!=f.input.bodyEpoch||r.request!=f.request||
           !f.output.contactKnown[side]||f.output.sweepDeferred115[side]||!ph::attack_authority55c1(f.input,f.output,side))continue;
        const mo::Key key{r.gesture59c3,r.key,r.generation,r.epoch,r.request,
            kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(f.player)};
        (void)kcd2vr::swing_effort59c14::ledger.publish(key,r.effortScale19,r.effortMass19,r.twoHands59c7g,r.tick);
    }
}
static void PhysPublish6698(const ph::Input& input,const ph::Output& output,const std::array<ph::Profile,2>& profiles,const PhysEquipment6698& equipment,const float* world,ph::State& state,std::array<ph::HolsterState,8>& holsters,const std::array<PhysCorrelation126,2>* correlation=nullptr,std::uint64_t trackingEpochT23=0,std::uint64_t characterT23=0,std::uint64_t publicationT23=0) noexcept {
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,input.tick)){
        namespace hc=kcd2vr::diagnostics::hit_chain_c54r3;
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Combat)].load();
        // The primary armed recognizer is right-side only. Do not attach its
        // observation to an offhand torch/shield or to the punch recognizer.
        hc::recorder.observe(hc::snapshot(input,output,profiles,state,0,session),true);
    }
    if(g_weapon_control_mode.load()<4||input.epoch!=g_weapon_request_epoch6694.load()||input.bodyEpoch!=g_pose_epoch6689.load()){
        if(PhysicalCombat177Enabled())g_phys_ready_sample6698.store(0,std::memory_order_release);
        return;
    }
    PhysApplyHolsterRejectionsD48(input,equipment,holsters);
    PhysFrame6698 f{};f.input=input;f.output=output;f.profiles=profiles;f.player=equipment.player;f.manager=equipment.manager;f.request=equipment.request;std::copy_n(world,8,f.world);
    // R15: publish gesture priority with the SAME completed transaction, even
    // when a second skin callback reuses one XR sample and skips FSM advance.
    if(input.requirePrimaryGrip114&&input.allowed&&equipment.valid&&equipment.count>=0&&equipment.count<=int(equipment.items.size()))
        for(int i=0;i<equipment.count;++i){const auto& item=equipment.items[i];const int hand=ph::holster_hand126(item.profile);
            if(item.held||item.set<0||hand<0||hand>1||!input.tracked[hand]||!input.intentTracked[hand]||
               !equipment.handKnown[hand]||!std::isfinite(input.grip[hand])||input.grip[hand]<=.55)continue;
            const auto& sw=holsters[0].switchHS2;
            const bool switching=sw.active&&sw.hand==hand&&sw.key==item.profile.key&&sw.generation==item.profile.generation&&
                sw.body==input.bodyEpoch&&sw.tick&&input.tick>=sw.tick&&input.tick-sw.tick<=3000;
            if(switching||equipment114::HeldAcquisitionR15(holsters[i].comparison114,item.profile.key,item.profile.generation,input.epoch,input.tick,input.grip[hand])){
                f.holsterGripKeyR15[hand]=item.profile.key;f.holsterGripGenerationR15[hand]=item.profile.generation;
            }
        }
    if(correlation)f.correlation126=*correlation;
    f.trackingEpochT23=trackingEpochT23;f.characterT23=characterT23;f.publicationT23=publicationT23;
    for(int owner=0;owner<2;++owner)f.retainedSupport55[owner]=kcd2vr::physical_hand_owner55::retained(input,output,profiles[owner],state.weapon[owner],owner);
    bool recovering55c1=false;ph::Output::IntentQuarantineH8 previousQuarantineH8[2]{};
    std::uint64_t previousRecoveryH8=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    {
        PhysFrame6698 previous{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(ready&&g_phys_frame6698.read(previous)&&ready==g_phys_ready_sample6698.load(std::memory_order_acquire)){
            for(int side=0;side<2;++side)previousQuarantineH8[side]=previous.output.intentQuarantineH8[side];
            if(input.physicalCombat177)PhysInterval177(previous,f,ready);
            // A real tracking/menu/mode loss can turn physicalCombat OFF in
            // this very frame. Retire the previous owner before re-entry too.
            if(input.physicalCombat177||previous.input.physicalCombat177)
                recovering55c1=kcd2vr::gesture_recovery_h30::Retires(previous,f,ph::command_authority_h30);
        }
    }
    if(input.haveWorld){const auto committedWorld=PhysWorld6698(world);f.publishWorldDeltaM122=pm::length(input.world.p-committedWorld.p);f.publishWorldDeltaRad122=pm::angle(input.world.q,committedWorld.q);}
    if(recovering55c1){
        // Retire before publishing new gesture/effort keys. Otherwise the same
        // frame would create them under an epoch that it immediately destroys.
        kcd2vr::contact176::policy.fetch_add(1,std::memory_order_acq_rel);
        const auto recovery56=kcd2vr::contact176::recoveryEpoch55c1.fetch_add(1,std::memory_order_acq_rel)+1;
        kcd2vr::recovery65h::edge(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,input.tick)); // edge only; NEVER infer queued/in-flight drops
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,input.tick)){
            for(int side=0;side<2;++side){const auto& q=previousQuarantineH8[side];if(!q.gesture)continue;
                char line[700]{};std::snprintf(line,sizeof(line),
                    "H8 INTENT_RETIRED side=%d gesture=%llu originalCommit=%llu originalRecovery=%llu currentRecovery=%llu previousFailedMask=%u uncertaintySince=%llu currentTick=%llu currentAllowed=%d attacks=%d tracked=%d intentTracked=%d contactFault=%d profileMatch=%d bodyMatch=%d requestMatch=%d currentCommandAuthority=%d currentQuarantine=%d semanticRetirement=1 cause=OBSERVED_CURRENT_SOURCE_OR_OWNER_UNAVAILABLE clocks=ORIGINAL_NOT_EXTENDED",
                    side,(unsigned long long)q.gesture,(unsigned long long)q.commit,(unsigned long long)previousRecoveryH8,
                    (unsigned long long)recovery56,q.failed,(unsigned long long)q.since,(unsigned long long)input.tick,
                    int(input.allowed),int(input.attacks),int(input.tracked[side]),int(input.intentTracked[side]),int(input.contactFaultH30),
                    int(q.owner.key==profiles[side].key&&q.owner.generation==profiles[side].generation),
                    int(q.owner.bodyEpoch==input.bodyEpoch),int(q.owner.request==input.epoch),
                    int(ph::command_authority_h30(input,output,profiles[side],side)),
                    int(ph::quarantined_intent_h8(input,output,profiles[side],side)));diagnostic_trace::Write(line);
            }
        }
        // Existing F3 receipt, now limited to genuine owner/solver/shield
        // retirement. Ordinary collision/reach display recovery is not an edge.
        static kcd2vr::receipt56::Site recoverySite57;
        static std::atomic<std::uint64_t> recoveryCount57{0};
        const auto edges57=recoveryCount57.fetch_add(1,std::memory_order_relaxed)+1;
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,input.tick)&&
           kcd2vr::receipt56::allow(recoverySite57,4,input.tick)){
            char m[300]{};std::snprintf(m,sizeof(m),"C56 HANDLING_RECOVERY_EDGE sample=%llu tick=%llu recoveryEpoch=%llu policy=%llu side=%s releaseReason=%u/%u strokeActive=%d totalEdges=%llu (queued contacts of this swing are now refused by design)",
                (unsigned long long)input.sample,(unsigned long long)input.tick,(unsigned long long)recovery56,(unsigned long long)kcd2vr::contact176::policy.load(std::memory_order_acquire),
                output.handlingOnly55c1[0]&&output.handlingOnly55c1[1]?"BOTH":output.handlingOnly55c1[0]?"R":output.handlingOnly55c1[1]?"L":"NONE",output.releaseReason121[0],output.releaseReason121[1],output.strokeActive119?1:0,(unsigned long long)edges57);
            diagnostic_trace::Write(m);}
    }
    const auto commandRecoveryH30=kcd2vr::contact176::recoveryEpoch55c1.load(std::memory_order_acquire);
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Combat,input.tick)){
        static kcd2vr::receipt56::Site quarantineSiteH8;
        for(int side=0;side<2;++side){const auto& q=output.intentQuarantineH8[side];
            if(ph::quarantined_intent_h8(input,output,profiles[side],side)&&
               kcd2vr::receipt56::allow(quarantineSiteH8,12,input.tick)){
                char line[700]{};std::snprintf(line,sizeof(line),
                    "H8 INTENT_QUARANTINE side=%d sample=%llu tick=%llu gesture=%llu originalCommit=%llu originalRecovery=%llu currentRecovery=%llu since=%llu failedMask=%u sourceDt=%.6f modelTip=%.3f trackedTip=%.3f modelHand=%.3f trackedHand=%.3f tipCap=40 handCap=20 currentCommands=REFUSED currentContact=REFUSED ancestry=%s originalClocks=UNCHANGED",
                    side,(unsigned long long)input.sample,(unsigned long long)input.tick,(unsigned long long)q.gesture,
                    (unsigned long long)q.commit,(unsigned long long)previousRecoveryH8,(unsigned long long)commandRecoveryH30,(unsigned long long)q.since,
                    q.failed,q.sourceDt,q.modelTip,q.trackingTip,q.modelHand,q.trackingHand,
                    recovering55c1?"RETIRED_BY_OTHER_OWNER_EDGE":"PRESERVED");diagnostic_trace::Write(line);
            }
        }
    }
    for(unsigned i=0;i<f.output.commandCount&&i<f.output.commands.size();++i)
        f.output.commands[i].recoveryEpochH30=commandRecoveryH30;
    PhysPublishHybrid54(f);
    PhysPublishImpactEffort19(f,equipment);
    if(!g_phys_frame6698.publish(f)){
        if(PhysicalCombat177Enabled())g_phys_ready_sample6698.store(0,std::memory_order_release);
        g_physical_gates101R2[9].fetch_add(1,std::memory_order_relaxed);return;
    }g_physical_gates101R2[2].fetch_add(1,std::memory_order_relaxed);g_phys_state6698=state;
    for(int side=0;side<2;++side)kcd2vr::haptics101::observe(g_phys_cues101[side],side,output.cueKey[side],output.cueGap[side],input.allowed&&input.tracked[side],input.sample,input.tick,output.cueRadiusD48[side]);
    // V66.178R58 wall feel: counters for the MELEE_FLOW line and a light scrape pulse while the blade drags along a
    // known surface (0.3-2 m/s slip -> 0.12-0.35 amplitude, 30 ms, at most every 70 ms per hand). Impacts keep their pulse.
    for(int side=0;side<2;++side){
        if(output.wallBlocked58[side])++g_wall_blocked58;if(output.wallSlid58[side])++g_wall_slid58;if(output.wallBackout58[side])++g_wall_backout58;if(output.wallStuck58[side])++g_wall_stuck58;
        namespace hp=kcd2vr::haptics101;
        if(input.wallSlide58&&output.wallBlocked58[side]&&output.scrapeSpeed58[side]>.3&&output.impactSpeed[side]<=.25&&output.active[side]&&output.owned[side]&&input.allowed&&input.tracked[side]&&
           hp::enabled.load()&&hp::allowed.load()&&input.tick>=g_scrape_last58[side]&&input.tick-g_scrape_last58[side]>=70){
            g_scrape_last58[side]=input.tick;++g_wall_scrape58;
            const float amp=float(std::clamp(.12+(output.scrapeSpeed58[side]-.3)/1.7*.23,.12,.35));
            hp::queue[side].publish(hp::Pulse{hp::epoch.load(),input.sample,input.tick,profiles[side].key,amp,30});
        }
    }
    for(int side=0;side<2;++side)if(output.impactSpeed[side]>.25&&output.active[side]&&output.owned[side]&&input.allowed&&input.tracked[side]){
        namespace hp=kcd2vr::haptics101;
        if(!hp::enabled.load()||!hp::allowed.load())continue;
        const int other=1-side;
        const bool support=output.support[side]&&output.owned[other]&&input.tracked[other];
        const auto feedback=kcd2vr::impact_haptics133::map(output.impactSpeed[side],profiles[side].weight,support);
        if(feedback.primary>0.0f)hp::queue[side].publish(hp::Pulse{hp::epoch.load(),input.sample,input.tick,
            profiles[side].key,feedback.primary,feedback.durationMs});
        if(support&&feedback.support>0.0f)hp::queue[other].publish(hp::Pulse{hp::epoch.load(),input.sample,input.tick,
            profiles[side].key,feedback.support,feedback.durationMs});
    }
    for(int side=0;side<2;++side){
        namespace passive=kcd2vr::passive169r1;
        const auto& c=output.contact119[side];passive::Sample sample{};
        sample.source=passive::Source::WeaponWorld;sample.side=side;sample.actor=reinterpret_cast<std::uintptr_t>(equipment.player);
        sample.key=profiles[side].key;sample.generation=profiles[side].generation;sample.epoch=input.bodyEpoch;
        sample.request=input.epoch;sample.sequence=input.sample;sample.tick=input.tick;sample.witness=c.witness169r1;
        sample.eligible=input.allowed&&input.contact&&input.tracked[side]&&input.intentTracked[side]&&output.active[side]&&output.owned[side];
        sample.worldCoherent=input.haveWorld&&f.publishWorldDeltaM122<=.002&&f.publishWorldDeltaRad122<=.002&&std::abs(double(world[7])-input.worldScale)<1e-5;
        sample.status=!sample.eligible?passive::Status::Unavailable:(!c.known||c.limited115||!sample.worldCoherent)?passive::Status::Unknown:
            output.blocked[side]?(c.plane?passive::Status::Contact:passive::Status::Unknown):passive::Status::Clear;
        sample.point=ph::point(input.world,c.point*input.worldScale);sample.normal=pm::rotate(input.world.q,c.normal);
        sample.allowed=ph::point(input.world,ph::point(output.weapons[side],c.feature)*input.worldScale);
        sample.requested=ph::point(input.world,input.hands[side].p*input.worldScale);
        sample.measuredMotion=true;sample.motion=output.contactMotion169r1[side];
        sample.flags=(output.support[side]?1u:0u)|(input.attacks?2u:0u);
        // Native/qualified impact cue above wins over the softer passive cue.
        kcd2vr::diagnostics::passive169r1::observe(sample,output.impactSpeed[side]<=.25&&
            (input.wallSlide58||sample.motion.incoming>.025), // H18: preserve inward touch, make tangential texture opt-in
            output.support[side]&&output.owned[1-side]&&input.tracked[1-side]);
    }
    ObservePhysicalImpact177(f);
    for(unsigned i=0;i<f.output.commandCount;++i){const auto& c=f.output.commands[i];
        const bool queued=g_phys_commands6698.push(c);
        TraceHitCommandC54R3(c,"COMMAND_ENQUEUE",queued?"QUEUED":"COMMAND_RING_FULL_OR_CONTENDED",input.tick,queued);
        if(queued&&c.kind==ph::CommandKind::DropT1&&c.side>=0&&c.side<2)
            g_weapon_release_pending_t1[c.side].store(c.key,std::memory_order_release);
        if(queued)PhysHolsterEnqueuedD48(input,equipment,holsters,c);
        else (void)PhysRejectHolsterLocalD48(c,input,equipment,holsters);
        ObserveMeleeCommand174r2(c,output,queued);}
    g_phys_holsters6698=holsters;
    g_phys_ready_sample6698.store(input.sample,std::memory_order_release);
    PhysTraceAcquisitionD48(input,output,profiles);
    PublishEnvironmentFx178r22c1(f);
}
#include "../subsystems/throw_drop/holster_gestures.inl" // M2_OWNED_SOURCE

#include "weapon_same_frame_d69.inl" // explicit OFF-default completed-pose placement experiment
