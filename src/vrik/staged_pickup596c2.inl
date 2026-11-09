// Exact retail, game-frame consumer. One world item, no inventory clone or fake
// success. Only the native normal-pickup request can acquire; steal stays native.
// R59.16C1 defaults F5=2 to a bounded native-physics velocity drive.
// Legacy kinematic comparison remains explicit (VrPropPhysicsDrive=false).
// No native constraint or active NPC ragdoll is claimed.
#include "pickup_input_d80.h"
#include "../core/haptics.h"
#include "../subsystems/physics/load_fault_recovery_r96.h"
#include "../core/load_rearm_r96.h" // R97: UpdateSmallPickupReaderR24 load re-arm (was only reached through mod.cpp)
#include "prop_parent_r153.h" // R153: SetPosRotScale is LOCAL, the instant-held item hangs on a parent
#include "instant_drop_r158.h" // R158: a dropped (not stowed) instant item falls like a scenery prop
namespace stage596 = kcd2vr::staged596c2;
namespace prop5916 = kcd2vr::prop5916c1;
struct StagedProp596 {
    void* entity=nullptr;void* controller=nullptr;void* item=nullptr;
    std::uint32_t id=0,actor=0;std::uint64_t key=0,epoch=0,sample=0;
    int side=-1;ph::Pose offset{};float scale[3]{};stage596::Hold hold{};
    void* body=nullptr;std::uintptr_t character=0;std::uint64_t bodyEpoch=0;
    bool physics=false;prop5916::Controller drive{};std::uint64_t reportTick=0;
    std::uint64_t sourceTickR96=0,loadEpisodeR96=0; // original qualified source; never refreshed during an observation gap
    ph::Pose observed{};bool observedValid=false;
    prop5916::KinematicTrailD85 kinematicD85{}; // D85 interact: legacy kinematic throw
    // R96: the R94R1 run (KCD2VR2.log 17:51:35-17:52:03, 13 instant holds) never
    // saw the native body move: 46 F5_PROP_BODY rows deltaPosition=0 while the
    // velocity was accepted. The displayed hand is pinned to that unmoving pose.
    // If the body does not follow for 100 ms, this hold carries the item with
    // the entity transform instead (same SetPosRotScale as the kinematic mode).
    bool kinematicR96=false,stuckRefValidR96=false;pm::Vec3 stuckRefR96{};std::uint64_t stuckSinceR96=0;
    // R153: the parent the entity hangs on, learned from a placement readback (prop_parent_r153.h). per hold.
    ph::Pose parentR153{};bool parentKnownR153=false;
    // Only populated by the exact native immediate transfer boundary. The
    // accepted request is retained once; no animation/action pointer is held.
    bool instantD72=false,noticeD72=false;
    std::uint64_t instantTokenD72=0;
    void* managerD72=nullptr;void* playerD72=nullptr;
    void* containerD72=nullptr;void* ownerD72=nullptr;void* inventoryD72=nullptr;
    std::uint32_t countD72=0;
    alignas(8) unsigned char acceptedD72[0xC8]{};
    kcd2vr::instant_pickup_d72::Transfer transferD72=nullptr;
};
static StagedProp596 g_prop596;
// R158 (instant_drop_r158.h): the drop waiting for static_grab_r89n::Tick (detach if parented, wake, hand velocity).
// only touched under PropLock596.
static kcd2vr::instant_drop_r158::Slot g_instant_drop_r158;
static bool PropCommitInstantD72(const StagedProp596&) noexcept;
static void PropRetireInstantD72(const StagedProp596&) noexcept;
static kcd2vr::load_fault_recovery_r96::Fault g_prop_fault5916c1;
static std::atomic<DWORD> g_stage_thread596{0};
static std::atomic<std::uint64_t> g_stage_tick596{0};
static std::atomic_flag g_prop_lock596=ATOMIC_FLAG_INIT;
static thread_local bool g_prop_replay596=false;
struct PropLock596 {bool owned=!g_prop_lock596.test_and_set(std::memory_order_acquire);
    ~PropLock596(){if(owned)g_prop_lock596.clear(std::memory_order_release);}};
static bool __fastcall DeferredPickup596(void*,std::uint32_t) noexcept { return false; }
static void PropNote596(const char* phase,const StagedProp596& p,bool ok,const char* reason="NONE") noexcept {
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return;
    char line[480]{};std::snprintf(line,sizeof(line),
        "C10 F5_PROP stage=%s reason=%s actor=%u entity=%u controller=%p wuid=%016llX body=%p side=%d epoch=%llu sample=%llu nativeResult=%d inventory=DEFERRED_UNTIL_STOW",
        phase,reason,p.actor,p.id,p.controller,(unsigned long long)p.key,p.body,p.side,
        (unsigned long long)p.epoch,(unsigned long long)p.sample,ok);diagnostic_trace::Write(line);
}
static ph::Pose PropWorldHand596(const CommittedFrame6695& f,int side) noexcept {
    auto h=PhysPose6698(f.allowed[side]);h.p=h.p*f.world[7];return ph::compose(PhysWorld6698(f.world),h);
}
// Direct leaf calls are from the retail CEntity vtable and SetVelocityEx binding;
// vtable slots and the 40-byte action layout are pinned in the static audit.
static void* PropBody596(void* entity,std::uintptr_t* observedBody=nullptr,
    std::uintptr_t* observedVft=nullptr) noexcept {
    if(observedBody)*observedBody=0;
    if(observedVft)*observedVft=0;
    if(!PhysVft6698(entity,kCEntityVftRva))return nullptr;
    void* body=nullptr;
    __try {body=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x8A49FC)(entity);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    if(observedBody)*observedBody=reinterpret_cast<std::uintptr_t>(body);
    if(observedVft&&body)(void)Read(body,*observedVft);
    return PhysVft6698(body,0x3A48DC0)?body:nullptr; // RTTI CRigidEntity only
}
static bool PropSetVelocity5916C1(void* body,pm::Vec3 linear={},pm::Vec3 angular={}) noexcept {
    if(!PhysVft6698(body,0x3A48DC0)||!pm::finite(linear)||!pm::finite(angular))return false;
    std::uintptr_t vf=0,fn=0;
    if(!Read(body,vf)||!Read(reinterpret_cast<void*>(vf+0x38),fn)||fn!=WhBase()+0xAC0D18)return false;
    prop5916::Velocity action;
    action.linear[0]=float(linear.x);action.linear[1]=float(linear.y);action.linear[2]=float(linear.z);
    action.angular[0]=float(angular.x);action.angular[1]=float(angular.y);action.angular[2]=float(angular.z);
    __try {return reinterpret_cast<int(__fastcall*)(void*,const void*,int)>(fn)(body,&action,0)!=0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool PropVelocity596(void* entity) noexcept {return PropSetVelocity5916C1(PropBody596(entity));}
static bool PropDynamics5916C1(void* body,prop5916::Dynamics& d) noexcept {
    if(!PhysVft6698(body,0x3A48DC0))return false;
    std::uintptr_t vf=0,fn=0;
    if(!Read(body,vf)||!Read(reinterpret_cast<void*>(vf+0x30),fn)||fn!=WhBase()+0x8A6C90)return false;
    prop5916::Status status{};int ok=0;
    __try {ok=reinterpret_cast<int(__fastcall*)(void*,void*)>(fn)(body,&status);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!ok)return false;
    d.linear={status.linear[0],status.linear[1],status.linear[2]};
    d.angular={status.angular[0],status.angular[1],status.angular[2]};
    d.centre={status.centre[0],status.centre[1],status.centre[2]};d.mass=status.mass;
    return d.valid();
}
static bool PropObjectPose5916C1(const StagedProp596& p,ph::Pose& out) noexcept {
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(p.entity,p.id,tm))return false;
    const V3 x{tm.m[0],tm.m[4],tm.m[8]},y{tm.m[1],tm.m[5],tm.m[9]},z{tm.m[2],tm.m[6],tm.m[10]};
    if(std::fabs(Len(x)-p.scale[0])>.001f||std::fabs(Len(y)-p.scale[1])>.001f||std::fabs(Len(z)-p.scale[2])>.001f)return false;
    const Q4 q=QFromBasis(Normalize(x),Normalize(y),Normalize(z));
    out={{q.x,q.y,q.z,q.w},{tm.m[3],tm.m[7],tm.m[11]}};return ph::finite(out);
}
static ph::Pose PropRequestedWorld5916C1(const CommittedFrame6695& f,int side) noexcept {
    auto h=PhysPose6698(f.propRequested5916c1[side]);h.p=h.p*f.world[7];return ph::compose(PhysWorld6698(f.world),h);
}
static bool PropPickupHandD81(const CommittedFrame6695& f,int side,bool physics,ph::Pose& out) noexcept {
    if(side<0||side>1||!f.valid[side]||(physics&&!f.propRequestedValid5916c1[side]))return false;
    out=physics?PropRequestedWorld5916C1(f,side):PropWorldHand596(f,side);
    return ph::finite(out);
}
static void PropPublish5916C1(const StagedProp596& p,const CommittedFrame6695& f,ph::Pose actual,std::uint64_t now) noexcept {
    prop5916::Presentation v{};v.character=p.character;v.body=reinterpret_cast<std::uintptr_t>(p.body);
    v.bodyEpoch=p.bodyEpoch;v.experimentEpoch=p.epoch;v.key=p.key;v.observationTick=now;
    v.instantTokenD78=p.instantD72?p.instantTokenD72:0;
    v.sourceTick=f.stamp.tick;v.sequence=f.stamp.sample;v.side=p.side;
    v.wristWorld=ph::compose(actual,ph::inverse(p.offset));
    (void)prop5916::presentation.publish(v);
}
static bool PropIdentity596(const StagedProp596& p) noexcept {
    if(!p.id||EntityById6694(p.id)!=p.entity||!LifecycleHenry165(p.actor)||
       !PhysVft6698(p.controller,0x3EC85E0)||!PhysVft6698(p.item,0x3A2DA88))return false;
    void* ent=nullptr;void* item=nullptr;std::uint64_t key=0;
    return Read(static_cast<unsigned char*>(p.controller)+0x38,ent)&&ent==p.entity&&
        Read(static_cast<unsigned char*>(p.controller)+0x58,item)&&item==p.item&&
        Read(static_cast<unsigned char*>(item)+0x30,key)&&key==p.key&&PropBody596(p.entity)==p.body;
}
static bool PropHandFree596(int side) noexcept {
    std::uint64_t key=0;
    return NativeHandKey6697(g_henry_item_attach_manager.load(),side,key)&&PhysEmpty6698(key);
}
// D78R1: ordinary instant pickup has its own native admission. F5 only owns
// the older staged experiment. Existing accepted occurrences keep their debt
// after a preference change; current gameplay/body/tracking still must qualify.
static bool PropInstantContextD78(std::uint64_t now) noexcept {
    return g_cal_state.load(std::memory_order_acquire)==2&&g_gameplay_allowed.load(std::memory_order_acquire)&&
        !kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire)&&
        !diagnostics::lab::NativeReference()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!g_ctx_scripted.load()&&
        !animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
        WorkstationContextRefusal19c4r2(now,true)==nullptr;
}
static bool BuildStagedPickup596(void* controller,std::uint32_t actor,bool prepareInstantD72,StagedProp596* preparedD72=nullptr) noexcept {
    // Native grasp must receive its own Attach event; do not intercept the same
    // pickup first with the older kinematic/rigid-body F5 experiment.
    if(!prepareInstantD72&&g_graspEnabled13&&g_graspEnabled13()&&g_grab_mode60.load(std::memory_order_acquire)!=2)return false; // C61 staged hold owns this pickup in explicit PHYSICAL mode
    auto now=std::uint64_t(GetTickCount64());const auto frameTick=g_stage_tick596.load();
    const auto loadEpisodeR96=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire);
    StagedProp596 p{};p.controller=controller;p.actor=actor;
    CommittedFrame6695 f{};TrackingSnapshot t{};Matrix34V6550 tm{};
    std::uintptr_t observedBody=0,observedVft=0;
    unsigned validMask=0,trackingMask=0,freeMask=0,gripMask=0;
    double reach[2]{-1,-1};
    const auto reject=[&](const char* reason){
        if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))return false;
        using namespace kcd2vr::diagnostics::interaction_receipt59c10;
        static thread_local Gate receipt;
        const auto session=diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Interaction)].load();
        // Literal refusal reasons and the copied controller are diagnostic keys;
        // never dereference a failed identity just to give a refusal a target ID.
        if(!receipt.observe(session,now,{reinterpret_cast<std::uintptr_t>(controller)^reinterpret_cast<std::uintptr_t>(reason),p.id,p.side,0},0))return false;
        const auto base=WhBase();
        const auto rva=observedVft>=base&&observedVft-base<0x6000000?observedVft-base:0;
        char line[880]{};std::snprintf(line,sizeof(line),
            "C10 F5_PICKUP_REFUSED session=%llu reason=%s actor=%u target=%u controller=%p wuid=%016llX side=%d epoch=%llu thread=%lu stageThread=%lu frameAge=%llu committedAge=%llu trackingAge=%llu body=%p bodyVftRva=%llX validMask=%u trackingMask=%u freeMask=%u gripMask=%u reachR=%.3f reachL=%.3f observed=%llu changes=%llu coalesced=%llu",
            (unsigned long long)session,reason,actor,p.id,controller,(unsigned long long)p.key,p.side,
            (unsigned long long)kcd2vr::workstation178r20c1::experimentEpoch36.load(),
            static_cast<unsigned long>(GetCurrentThreadId()),static_cast<unsigned long>(g_stage_thread596.load()),
            (unsigned long long)age(now,frameTick),(unsigned long long)age(now,f.stamp.tick),(unsigned long long)age(now,t.tick),
            reinterpret_cast<void*>(observedBody),(unsigned long long)rva,validMask,trackingMask,freeMask,gripMask,reach[0],reach[1],
            (unsigned long long)receipt.seen,(unsigned long long)receipt.changes,(unsigned long long)receipt.coalesced);
        diagnostic_trace::Write(line);return false;
    };
    if(g_prop_replay596)return reject("NATIVE_STOW_REENTRY");
    // R96: a previous BODY_DRIVE_FAULT no longer refuses every later pickup
    // until restart. This new squeeze is a new attempt (never a timer retry):
    // the failed request itself is not replayed, the body is re-verified below.
    // R97: the fault is the load-scoped R96R1 Fault (a completed load also
    // clears it, and the lab prop reads it); a real new squeeze clears it too.
    if(prop5916::enabled.load()&&g_prop_fault5916c1.load()){
        g_prop_fault5916c1.clear();
        if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now))
            diagnostic_trace::Write("R96 F5_PROP previous BODY_DRIVE_FAULT cleared: new squeeze = new attempt");
    }
    if(prepareInstantD72?!PropInstantContextD78(now):!kcd2vr::workstation178r20c1::ownsWrist(now))
        return reject(prepareInstantD72?"INSTANT_GAMEPLAY_OWNER_NOT_ADMITTED":"WRIST_OWNER_NOT_ADMITTED");
    if(!retail_build112::Ready(WhBase()))return reject("RETAIL_FINGERPRINT");
    if(!LifecycleHenry165(actor))return reject("ACTOR_NOT_EXACT_HENRY");
    if(!prepareInstantD72&&g_stage_thread596.load()!=GetCurrentThreadId())return reject("WRONG_GAME_FRAME_THREAD");
    if(!frameTick)return reject("GAME_FRAME_NOT_PUBLISHED");
    if(now<frameTick)return reject("GAME_FRAME_FUTURE");
    if(now-frameTick>80)return reject("GAME_FRAME_STALE");
    PropLock596 lock;if(!lock.owned)return reject("LOCK_BUSY");
    // Repeated Use must not commit the held item, or pick a second one.
    if(g_prop596.id)return !prepareInstantD72;
    if(!PhysVft6698(controller,0x3EC85E0)||
       !Read(static_cast<unsigned char*>(controller)+0x38,p.entity)||
       !LifecycleEntity165(reinterpret_cast<std::uintptr_t>(p.entity),p.id)||
       !Read(static_cast<unsigned char*>(controller)+0x58,p.item)||!PhysVft6698(p.item,0x3A2DA88)||
       !Read(static_cast<unsigned char*>(p.item)+0x30,p.key)||PhysEmpty6698(p.key))return reject("NATIVE_ITEM_IDENTITY");
    // These are distinct contracts. The C10 test produced four ambiguous
    // POSE_OR_RIGID_BODY refusals: another pickup cannot distinguish a missing
    // committed wrist from a static/non-rigid object without this receipt.
    if(!ReadCommittedPacket6695(f))return reject("COMMITTED_WRIST_UNAVAILABLE");
    if(prepareInstantD72){
        // Grip, body, hand poses and head validity are one completed semantic
        // publication. A newer live tracking read cannot complete this packet.
        if(!f.mapFoldTrackedM39||!f.headEvidence122)
            return reject("INSTANT_COMPLETED_TRACKING_UNAVAILABLE");
        t.tick=f.stamp.tick;t.allowed=true;t.head_valid=true;
        t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
    }else if(!ReadTrackingSnapshot(t))return reject("TRACKING_SNAPSHOT_UNAVAILABLE");
    if(!t.allowed||!t.head_valid)return reject("TRACKING_CONTEXT_DISALLOWED");
    if(!GetEntityWorldTmV6552(p.entity,p.id,tm))return reject("OBJECT_WORLD_POSE_UNAVAILABLE");
    if(!PropBody596(p.entity,&observedBody,&observedVft))return reject("OBJECT_NOT_VERIFIED_RIGID_BODY");
    now=std::uint64_t(GetTickCount64()); // sample AFTER these exact copied sources/native reads
    if(!f.stamp.tick||now<f.stamp.tick||now-f.stamp.tick>80)return reject("COMMITTED_POSE_AGE");
    if(!t.tick||now<t.tick||now-t.tick>80)return reject("TRACKING_AGE");
    ph::Pose object{};const V3 x{tm.m[0],tm.m[4],tm.m[8]},y{tm.m[1],tm.m[5],tm.m[9]},z{tm.m[2],tm.m[6],tm.m[10]};
    p.scale[0]=Len(x);p.scale[1]=Len(y);p.scale[2]=Len(z);
    for(float v:p.scale)if(!std::isfinite(v)||v<.001f||v>100.f)return reject("OBJECT_SCALE");
    const Q4 q=QFromBasis(Normalize(x),Normalize(y),Normalize(z));
    object={{q.x,q.y,q.z,q.w},{tm.m[3],tm.m[7],tm.m[11]}};if(!ph::finite(object))return reject("OBJECT_POSE");
    double best=.25; // local reach, even when a distant ray offered native pickup
    p.physics=prop5916::enabled.load(std::memory_order_acquire);
    ph::Pose pickupHand{};
    const bool selectedFistsD80=FistInputOwned101();const char* inputRefusalD80=nullptr;
    // The transported Use identifies the acquiring hand. A second closed hand
    // being closer must not steal that item from the hand that requested it.
    // Older native/F5 acquisitions without that receipt retain nearest-hand
    // selection; a qualified request never falls across to the other hand.
    int requestedSideD85R5R2=-1;
    const bool requestedHandD85R5R2=prepareInstantD72&&
        action_time_owner59c11::RequestedPickupSideD85(p.id,requestedSideD85R5R2);
    for(int side=0;side<2;++side){
        if(requestedHandD85R5R2&&side!=requestedSideD85R5R2)continue;
        const double grip=side?t.left_grip:t.right_grip;
        const unsigned bit=1u<<side;
        if(!f.valid[side])continue;validMask|=bit;
        if(!TrackingHandUsable111(side))continue;trackingMask|=bit;
        if(!PropHandFree596(side))continue;freeMask|=bit;
        if(grip<.58)continue;gripMask|=bit;
        ph::Pose hand{};if(!PropPickupHandD81(f,side,p.physics,hand))continue;
        const double distance=pm::length(hand.p-object.p);reach[side]=distance;
        if(distance<best){
            if(const char* why=pickup_input_d80::refusal(f,side,selectedFistsD80)){inputRefusalD80=why;continue;}
            best=distance;p.side=side;pickupHand=hand;
        }
    }
    if(p.side<0)return reject(inputRefusalD80?inputRefusalD80:
        requestedHandD85R5R2?"REQUESTING_HAND_NOT_QUALIFIED":"NO_LOCAL_GRIP_WITHIN_25CM");
    if(!StageLeafPins596(kPropLeaves596))return reject("NATIVE_LEAF_PINS");
    // Physics drive consumes the requested wrist. Using a collision-corrected
    // visible wrist here would add that correction a second time at acquisition
    // and move the object even though the controller had not moved.
    p.offset=ph::compose(ph::inverse(pickupHand),object);
    p.epoch=prepareInstantD72?f.stamp.epoch:kcd2vr::workstation178r20c1::experimentEpoch36.load();p.sample=f.stamp.sample;
    p.body=PropBody596(p.entity);
    p.character=f.stamp.actor;p.bodyEpoch=f.stamp.epoch;
    if(p.physics){
        prop5916::Dynamics d{};
        if(!f.propRequestedValid5916c1[p.side])return reject("CONTROLLER_PROP_POSE_UNAVAILABLE");
        if(!PropPhysicsPins5916C1())return reject("PHYSICS_LEAF_PINS");
        if(!PropDynamics5916C1(p.body,d))return reject("RIGID_DYNAMICS_UNAVAILABLE");
        // A world item retains its actual momentum on acquisition. No SetWorldTM,
        // rigid-body replacement or unconditional zero-velocity write.
        p.drive.seed(ph::compose(PropRequestedWorld5916C1(f,p.side),p.offset),f.stamp.sample,f.stamp.tick);
        p.observed=object;p.observedValid=true;
    }
    now=std::uint64_t(GetTickCount64());
    if(now<f.stamp.tick||now-f.stamp.tick>80||now<t.tick||now-t.tick>80||
       loadEpisodeR96!=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)||
       kcd2vr::inventory_lifecycle_t8::loadingObserved18.load(std::memory_order_acquire)||
       (prepareInstantD72?!PropInstantContextD78(now):!kcd2vr::workstation178r20c1::ownsWrist(now)))
        return reject("FINAL_SOURCE_OR_OWNER_CHANGED");
    p.hold.begin(p.epoch,now);p.sourceTickR96=f.stamp.tick;p.loadEpisodeR96=loadEpisodeR96;
    if(p.physics&&!prepareInstantD72)PropPublish5916C1(p,f,object,now);
    else if(!p.physics&&!prepareInstantD72&&!PropVelocity596(p.entity))return reject("NATIVE_VELOCITY_CLEAR");
    if(prepareInstantD72){if(preparedD72)*preparedD72=p;return preparedD72!=nullptr;}
    // The original pickup is deferred: do not pretend an authored native grip
    // was accepted. Existing calibrated free-hand fingers follow the real Grip.
    g_prop596=p;PropNote596("ACQUIRED_WORLD_ITEM",p,true);return true;
}
static bool TryStagedPickup596(void* controller,std::uint32_t actor) noexcept {
    return BuildStagedPickup596(controller,actor,false);
}
static bool PropSetLocal596(std::uintptr_t fn,void* entity,const ph::Pose& local,const float* scale) noexcept {
    const auto q=PhysQuat6698(local);
    __try {reinterpret_cast<EntitySetPosRotScaleFn>(fn)(entity,q.t,q.q,scale,0x10u);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return true;
}
static bool PropPlace596(StagedProp596& p,ph::Pose pose) noexcept {
    if(!ph::finite(pose))return false;
    std::uintptr_t vf=0,fn=0;if(!Read(p.entity,vf)||vf!=WhBase()+kCEntityVftRva||
        !Read(reinterpret_cast<void*>(vf+0x168),fn)||fn!=WhBase()+0x509810)return false;
    // R153 (prop_parent_r153.h): the setter is LOCAL. an entity without a parent reads back where we asked and
    // nothing else happens (the old behaviour). one that hangs on a parent (the instant-held pickup item, R152 run
    // 11:18:47: placed at 2x its world position, 3 km away = "the item disappears") gets the parent learned from
    // this readback and is placed again in the parent's space. no readback (scale/identity) = old behaviour.
    namespace pp=kcd2vr::prop_parent_r153;
    ph::Pose before{};const bool beforeKnown=PropObjectPose5916C1(p,before);
    const ph::Pose local=p.parentKnownR153?pp::LocalFor(p.parentR153,pose):pose;
    if(!ph::finite(local)||!PropSetLocal596(fn,p.entity,local,p.scale))return false;
    ph::Pose world{};
    if(PropObjectPose5916C1(p,world)&&!pp::Matches(world,pose)){
        const auto parent=pp::Parent(world,local);const auto again=pp::LocalFor(parent,pose);
        ph::Pose check{};
        const bool placed=ph::finite(parent)&&ph::finite(again)&&PropSetLocal596(fn,p.entity,again,p.scale)&&
            PropObjectPose5916C1(p,check)&&pp::Matches(check,pose);
        if(!placed){
            // put it back where it was (under the parent we just saw), never leave it at the doubled position
            if(beforeKnown&&ph::finite(parent)){const auto back=pp::LocalFor(parent,before);if(ph::finite(back))(void)PropSetLocal596(fn,p.entity,back,p.scale);}
            char line[220]{};std::snprintf(line,sizeof(line),"R153 PICKUP_PLACE_REFUSED entity=%u key=%016llX reason=PARENT_SPACE_READBACK_WRONG -> hold ends, item stays",
                p.id,(unsigned long long)p.key);diagnostic_trace::Write(line);
            return false;
        }
        if(!p.parentKnownR153){
            char line[260]{};std::snprintf(line,sizeof(line),"R153 PICKUP_PARENT_SPACE entity=%u key=%016llX parent=(%.2f,%.2f,%.2f) wanted=(%.2f,%.2f,%.2f) (item placed in its parent's space)",
                p.id,(unsigned long long)p.key,parent.p.x,parent.p.y,parent.p.z,pose.p.x,pose.p.y,pose.p.z);diagnostic_trace::Write(line);
        }
        p.parentR153=parent;p.parentKnownR153=true;
    }
    return PropVelocity596(p.entity);
}
// R155 (Miki 8 Nov, R153 run: "the objects are pushing me back like those small objects did before"): the instant-held
// item is a real rigid body placed into the hand every frame (KINEMATIC_CARRY / R153 firm grip), and the hand is
// often inside Henry's living capsule - the physics step then pushes HIM away, exactly the R101 scenery-prop problem
// R103 fixed with the collision-class lease (self_collision_r103.h: ignore |= living, type |= living, restored after
// the release once the item is clear of Henry or after 4 s). The scenery path (static_grab_r89.inl HELD/DRIVE) has had
// it since R103; this hold never asked. Same lease, own owner bit per hand, re-applied every tick (250 ms rate).
static void PropSelfCollisionR155(const StagedProp596& p,std::uint64_t now) noexcept {
    if(!p.instantD72||!p.id||!p.entity||!p.body||p.side<0||p.side>1)return;
    self_collision_r103n::Exclude(kcd2vr::self_collision_r103::instantOwner(p.side),kcd2vr::self_collision_r103::Kind::Prop,p.id,
        reinterpret_cast<std::uintptr_t>(p.entity),reinterpret_cast<std::uintptr_t>(p.body),-1,now?now:1);
}
// R158: an instant item that leaves the hand WITHOUT the chest stow gets the scenery-prop gravity rules: one request,
// serviced at the next native rhythm frame (static_grab_r89.inl InstantDropR158). caller holds PropLock596.
static void PropDropR158(const StagedProp596& ended,pm::Vec3 v,bool handVelocity,std::uint64_t now) noexcept {
    if(!ended.instantD72||!ended.id||!ended.entity)return;
    kcd2vr::instant_drop_r158::Request r{};r.id=ended.id;r.entity=reinterpret_cast<std::uintptr_t>(ended.entity);
    r.handVelocity=handVelocity&&pm::finite(v);if(r.handVelocity){r.v[0]=v.x;r.v[1]=v.y;r.v[2]=v.z;}
    r.tick=now?now:1;g_instant_drop_r158.post(r);
}
static bool PropCommit596(void* controller,std::uint32_t actor) noexcept {
    bool ok=false;g_prop_replay596=true;
    __try {ok=reinterpret_cast<bool(__fastcall*)(void*,std::uint32_t)>(WhBase()+0x2A8A838)(controller,actor);}
    __except(EXCEPTION_EXECUTE_HANDLER){ok=false;}
    g_prop_replay596=false;return ok;
}
static void StagedPropTick596() noexcept {
    PropLock596 lock;if(!lock.owned||!g_prop596.id)return;
    auto& p=g_prop596;auto now=std::uint64_t(GetTickCount64());
    // This world-item request owns no constraint. A positively observed load
    // retires its private claim, without dereferencing/replaying the old item.
    if(p.loadEpisodeR96&&p.loadEpisodeR96!=kcd2vr::inventory_lifecycle_t8::loadEpisode18.load(std::memory_order_acquire)){
        PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    if(!PropIdentity596(p)){PropNote596("IDENTITY_ENDED",p,false);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;}
    CommittedFrame6695 f{};TrackingSnapshot t{};
    // Keep the exact short-circuit order of the ownership contract. These
    // receipts use the observations already required by the consumer.
    const auto eligibility=[&]() -> const char* {
        if(p.instantD72&&kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire))return "INSTANT_SHUTDOWN";
        if(p.instantD72){
            if(!PropInstantContextD78(now))return "INSTANT_GAMEPLAY_OWNER_ENDED";
            if(p.side<0||p.side>1||!p.instantTokenD72||
               kcd2vr::mutual_admission_t25::pickupClaimsD72[p.side].load(std::memory_order_acquire)!=p.instantTokenD72)
                return "INSTANT_HAND_CLAIM_ENDED";
        }else if(!kcd2vr::workstation178r20c1::ownsWrist(now))return "WRIST_OWNER_ENDED";
        const bool haveFrameR96=ReadCommittedPacket6695(f);now=std::uint64_t(GetTickCount64());
        if(!haveFrameR96)return "COMMITTED_WRIST_UNAVAILABLE";
        if(!f.valid[p.side])return "COMMITTED_HAND_INVALID";
        if(f.stamp.epoch!=p.bodyEpoch||f.stamp.actor!=p.character)return "BODY_IDENTITY_CHANGED";
        if(const char* why=pickup_input_d80::refusal(f,p.side,FistInputOwned101()))return why;
        if(p.instantD72){
            if(!f.mapFoldTrackedM39||!f.headEvidence122)return "INSTANT_COMPLETED_TRACKING_UNAVAILABLE";
            t.tick=f.stamp.tick;t.allowed=true;t.head_valid=true;
            t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
        }else{const bool haveTrackingR96=ReadTrackingSnapshot(t);now=std::uint64_t(GetTickCount64());
            if(!haveTrackingR96)return "TRACKING_SNAPSHOT_UNAVAILABLE";}
        if(!t.allowed||!t.head_valid)return "TRACKING_CONTEXT_DISALLOWED";
        if(!TrackingHandUsable111(p.side))return "HAND_TRACKING_UNUSABLE";

        if(t.head_held||(p.side?t.left_held:t.right_held))return "TRACKING_GRACE_NOT_FRESH";
        if(!PropHandFree596(p.side))return "NATIVE_HAND_ITEM_CHANGED";
        if(!p.kinematicR96&&p.physics!=prop5916::enabled.load())return "PHYSICS_MODE_CHANGED";
        if(p.physics&&!f.propRequestedValid5916c1[p.side])return "CONTROLLER_PROP_POSE_UNAVAILABLE";
        now=std::uint64_t(GetTickCount64());
        if(!t.tick||now<t.tick||now-t.tick>80)return "TRACKING_AGE";
        if(!f.stamp.tick||now<f.stamp.tick||now-f.stamp.tick>80)return "COMMITTED_POSE_AGE";
        if(p.instantD72&&!PropInstantContextD78(now))return "INSTANT_GAMEPLAY_OWNER_ENDED";
        return nullptr;
    };
    const char* endReason=eligibility();const bool eligible=endReason==nullptr;
    // A missing publication/temporarily untracked hand is not a native owner
    // replacement. Keep this exact claim only inside its ORIGINAL source age.
    // No drive, stow progress, presentation publication or lease renewal occurs.
    const bool observationGapR96=endReason&&(
        std::strcmp(endReason,"COMMITTED_WRIST_UNAVAILABLE")==0||std::strcmp(endReason,"COMMITTED_HAND_INVALID")==0||
        std::strcmp(endReason,"HAND_TRACKING_UNUSABLE")==0||std::strcmp(endReason,"TRACKING_SNAPSHOT_UNAVAILABLE")==0||
        std::strcmp(endReason,"INSTANT_COMPLETED_TRACKING_UNAVAILABLE")==0||std::strcmp(endReason,"TRACKING_GRACE_NOT_FRESH")==0||
        std::strcmp(endReason,"CONTROLLER_PROP_POSE_UNAVAILABLE")==0);
    if(observationGapR96&&p.sourceTickR96&&now>=p.sourceTickR96&&now-p.sourceTickR96<=80)return;
    // Stow is a gesture of the tracked hand. Collision/prop presentation may
    // keep the displayed wrist outside the chest; that correction must not
    // veto the same deliberate gesture in a different collision mode. Both
    // poses and the chest come from this one completed publication. Legacy
    // kinematic packets without requested-pose evidence keep their old source.
    const pm::Vec3 chest{f.solvedHead122.x,f.solvedHead122.y,f.solvedHead122.z-.40};
    const double distance=eligible&&f.headEvidence122?pm::length(PhysPose6698(
        f.propRequestedValid5916c1[p.side]?f.propRequested5916c1[p.side]:f.allowed[p.side]).p-chest):100.;
    const auto decision=p.hold.update(p.instantD72?p.bodyEpoch:kcd2vr::workstation178r20c1::experimentEpoch36.load(),now,eligible,
        p.side?t.left_grip:t.right_grip,distance);
    if(decision!=stage596::Decision::Hold){
        const auto ended=p;p={};prop5916::clearPresentation(); // clear BEFORE native removal/reentry
        const bool stow=decision==stage596::Decision::Stow;
        const bool intentionalRelease=eligible&&std::isfinite(ended.side?t.left_grip:t.right_grip)&&
            (ended.side?t.left_grip:t.right_grip)<.30;
        // Intentional let-go keeps achieved body momentum. Loss/menu/epoch exits
        // stop our drive safely; no new controller-velocity throw is synthesized.
        // D85 interact: the legacy kinematic hold keeps its hand-carried velocity
        // on an intentional open-hand release; the same native SetVelocity.
        pm::Vec3 throwD85{};
        const bool kinematicThrowD85=!ended.physics&&intentionalRelease&&!stow&&ended.kinematicD85.release(now,throwD85);
        const bool settled=ended.physics&&intentionalRelease&&!stow?true:
            kinematicThrowD85?PropSetVelocity5916C1(PropBody596(ended.entity),throwD85):PropVelocity596(ended.entity);
        const bool ok=stow?(settled&&(ended.instantD72?PropCommitInstantD72(ended):PropCommit596(ended.controller,ended.actor))):settled;
        // The immediate commit re-reads the item's real inventory membership.
        // Only that successful transfer confirms "taken". The older animated
        // request merely admits an action and is not a completion receipt.
        // This user cue is independent of capture, and follows the held hand.
        const bool confirmedD84=stow&&ok&&ended.instantD72;
        const bool hapticD84=confirmedD84&&kcd2vr::haptics101::publish_event(
            ended.side,ended.key,.65f,65,now);
        if(stow&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)){
            char line[320]{};std::snprintf(line,sizeof(line),
                "D84 F5_STOW_CONFIRM side=%d entity=%u key=%016llX token=%llu nativeInventoryVerified=%d hapticQueued=%d gesture=COMPLETED_REQUESTED_WRIST",
                ended.side,ended.id,(unsigned long long)ended.key,(unsigned long long)ended.instantTokenD72,
                confirmedD84,hapticD84);diagnostic_trace::Write(line);
        }
        PropRetireInstantD72(ended);
        if(!stow)PropDropR158(ended,throwD85,kinematicThrowD85,now); // R158: not stowed -> it must fall (no-op for non-instant)
        PropNote596(stow?"NATIVE_STOW_RETURN":(intentionalRelease?"DROP_RETAIN_BODY_VELOCITY":"CANCEL_TO_NATIVE_PHYSICS"),ended,ok,
            endReason?endReason:stow?"CHEST_STOW":intentionalRelease?"GRIP_RELEASE":"EXPERIMENT_EPOCH_OR_HOLD_TIMEOUT");return;
    }
    p.sourceTickR96=f.stamp.tick;
    PropSelfCollisionR155(p,now); // R155: the held item does not collide with Henry (idempotent, see above)
    if(p.sample==f.stamp.sample&&!p.physics)return;
    if(!p.physics){p.sample=f.stamp.sample;
        // R96: a hold that switched from the physics drive keeps its offset
        // from the requested wrist it was acquired with.
        const auto placedD85=ph::compose(p.kinematicR96&&f.propRequestedValid5916c1[p.side]?
            PropRequestedWorld5916C1(f,p.side):PropWorldHand596(f,p.side),p.offset);
        if(!PropPlace596(p,placedD85)){PropNote596("PLACEMENT_STOPPED",p,false);PropRetireInstantD72(p);PropDropR158(p,{},false,now);p={};return;}
        p.kinematicD85.add(placedD85.p,f.stamp.tick);
        return;
    }
    prop5916::Dynamics d{};ph::Pose actual{};
    if(!PropDynamics5916C1(p.body,d)||!PropObjectPose5916C1(p,actual)){
        PropNote596("BODY_STATUS_ENDED",p,false);(void)PropSetVelocity5916C1(p.body);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    const auto target=ph::compose(PropRequestedWorld5916C1(f,p.side),p.offset);
    now=std::uint64_t(GetTickCount64()); // R97: one post-read clock for the stuck check and the drive
    {   // R96: does the native body follow at all? 0.5 mm in 100 ms while the
        // hand is >=5 mm away. If not, carry by entity transform from now on.
        const bool movedR96=p.stuckRefValidR96&&pm::length(actual.p-p.stuckRefR96)>.0005;
        if(!p.stuckRefValidR96||movedR96){p.stuckRefR96=actual.p;p.stuckRefValidR96=true;p.stuckSinceR96=0;}
        else if(pm::length(target.p-actual.p)>=.005){
            if(!p.stuckSinceR96)p.stuckSinceR96=now;
            else if(now>=p.stuckSinceR96&&now-p.stuckSinceR96>=100){
                (void)PropSetVelocity5916C1(p.body);prop5916::clearPresentation();
                p.physics=false;p.kinematicR96=true;p.sample=f.stamp.sample;
                const bool placedR96=PropPlace596(p,target);
                PropNote596("KINEMATIC_CARRY_R96",p,placedR96,"NATIVE_BODY_NOT_FOLLOWING_100MS");
                if(!placedR96){PropRetireInstantD72(p);PropDropR158(p,{},false,now);p={};return;}
                p.kinematicD85.add(target.p,f.stamp.tick);
                return;
            }
        }else p.stuckSinceR96=0;
    }
    auto staged=p.drive;const auto command=staged.update(target,actual,d,f.stamp.sample,f.stamp.tick,now);
    if(command.decision==prop5916::Decision::End){
        // Existing F5 end receipt only: distinguish the incoming source from
        // the last successful drive instead of logging its sample as the cause.
        if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)){
            char reason[224]{};std::snprintf(reason,sizeof(reason),
                "CONTINUITY inSample=%llu priorSample=%llu inTick=%llu priorTick=%llu ageMs=%llu separation=%.4f",
                (unsigned long long)f.stamp.sample,(unsigned long long)p.drive.sequence,
                (unsigned long long)f.stamp.tick,(unsigned long long)p.drive.tick,
                (unsigned long long)(now>=f.stamp.tick?now-f.stamp.tick:~std::uint64_t{}),command.separation);
            PropNote596("DRIVE_CONTINUITY_ENDED",p,false,reason);
        }
        (void)PropSetVelocity5916C1(p.body);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    if(command.decision==prop5916::Decision::Duplicate){
        // The native body was already observed this callback. Publish that
        // result even when its controller clock has no new integration time;
        // PropPublish keeps the original source tick and never invents motion.
        PropPublish5916C1(p,f,actual,now);return;
    }
    const auto driveLoadR96=kcd2vr::load_fault_recovery_r96::Fault::capture();
    if(!PropSetVelocity5916C1(p.body,command.linear,command.angular)){
        // May have partially applied. Never retry it on a timer.
        g_prop_fault5916c1.record(driveLoadR96);PropNote596("BODY_DRIVE_FAULT",p,false);PropRetireInstantD72(p);p={};prop5916::clearPresentation();return;
    }
    p.drive=staged;p.sample=f.stamp.sample;
    PropPublish5916C1(p,f,actual,now); // observed pose, NOT extrapolated commanded position
    if(kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Interaction,now)&&
       (!p.reportTick||now-p.reportTick>=200)){
        const auto moved=p.observedValid?pm::length(actual.p-p.observed.p):0.;
        char line[512]{};std::snprintf(line,sizeof(line),
            "R59.16C1 F5_PROP_BODY entity=%u body=%p sample=%llu sourceAgeMs=%llu mass=%.3f observedV=%.3f observedW=%.3f deltaPosition=%.5f separation=%.4f commandV=%.3f nativeActionReturned=1 motion=CURRENT_BODY_OBSERVATION constraint=NONE damage=NONE",
            p.id,p.body,(unsigned long long)f.stamp.sample,(unsigned long long)(now-f.stamp.tick),d.mass,
            pm::length(d.linear),pm::length(d.angular),moved,command.separation,pm::length(command.linear));
        diagnostic_trace::Write(line);p.reportTick=now;p.observed=actual;p.observedValid=true;
    }
}

#include "instant_pickup_native_d72.inl"
// R155 (Miki 8 Nov, R153 run: instant interactions work "except for the grab hand posture - they lose the 'grab look'
// and play idle"): the hold retains pickupClaimsD72[side] (RetainPickupD72), and hand_claim_t24::observe turns any
// pickup claim into SameActorBusy -> HandClaimBlockedT24 -> coherent_pose claimedFingerT24 keeps the NATIVE finger
// locals. For a native pickup those are the game's grab animation; for an item the mod carries itself there is no
// animation (forceImmediate), so the native locals are the idle hand. When the claim is THIS instant hold's token the
// fingers follow Grip/Trigger like a free hand instead - the hold needs Grip >= .30 so they stay closed on the item.
// Every other claim (throw, reload, ammo, native tool, and the scenery prop of static_grab_r89, which he called
// working) keeps the native fingers exactly as before. Read from the pose thread: atomics only.
static bool PropHoldOwnsHandR155(int side) noexcept {
    if(side<0||side>1)return false;
    const auto claim=kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire);
    return claim&&kcd2vr::instant_pickup_d72::active.load(std::memory_order_acquire)!=0&&
        claim==g_instant_pickup_hold_d72.load(std::memory_order_acquire);
}

// R24 read-only admission, entered synchronously by the exact registered Lua
// GetCVar callback. Normal Use retains its native semantics; the separate contact
// opcode requires the synchronous contact nomination and never invokes OnUsed.
// No native script execution, physicalization, inventory mutation or pose write here.
static void* SmallPickupItemR24(std::uint64_t key) noexcept {
    static constexpr StageLeafPin596 pins[]{{0x23D09E0,{0x48,0x89,0x54,0x24,0x10,0x48,0x83,0xEC}}};
    if(!StageLeafPins596(pins))return nullptr;
    void* system=nullptr;void* item=nullptr;std::uint64_t current=0;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x5487A78),system)||!system)return nullptr;
    __try{item=reinterpret_cast<void*(__fastcall*)(void*,std::uint64_t)>(WhBase()+0x23D09E0)(system,key);}
    __except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
    return PhysVft6698(item,0x3A2DA88)&&Read(static_cast<unsigned char*>(item)+0x30,current)&&current==key?item:nullptr;
}
static bool SmallPickupBodyR24(void* entity,void*& body) noexcept {
    body=nullptr;
    // The F5 observer legitimately hooks this getter. Qualify its exact owned
    // entry (or the untouched retail entry), not pristine bytes after our hook
    // is enabled; otherwise recording pickups disables their preparation.
    if(!PhysVft6698(entity,kCEntityVftRva)||!InteractionBodyEntryQualifiedR93(WhBase()))return false;
    __try{body=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x8A49FC)(entity);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){body=nullptr;return false;}
}
// Same qualified render-proxy reader as existing PhysBounds6698, with local refusal:
// a failed small-item query must not fault unrelated physical weapon handling.
static bool SmallPickupBoundsR24(void* entity,void*& proxy,float (&bounds)[6]) noexcept {
    proxy=nullptr;
    static constexpr StageLeafPin596 pins[]{
        {0x5D70C4,{0x4C,0x8B,0x81,0xA8,0,0,0,0x45}},
        {0x63F884,{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83}}};
    if(!retail_build112::Ready(WhBase())||!PhysVft6698(entity,kCEntityVftRva)||!StageLeafPins596(pins))return false;
    __try {
        // 0 selects the entity render-proxy type, not slot0.
        proxy=reinterpret_cast<void*(__fastcall*)(void*,int)>(WhBase()+0x5D70C4)(entity,0);
        if(!PhysVft6698(proxy,0x3A4A268)||!PhysVft6698(static_cast<unsigned char*>(proxy)-0x50,0x3A49FD8))return false;
        reinterpret_cast<void(__fastcall*)(void*,float*)>(WhBase()+0x63F884)(proxy,bounds);
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){proxy=nullptr;return false;}
}
static std::uint32_t SmallContactAdmissionR24(std::uint64_t key) noexcept;
static bool SmallInstantPickupAllowedD81(std::uint64_t now) noexcept {
    return kcd2vr::instant_d72::Interactions()&&g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&
        PropInstantContextD78(now)&&!action_time_owner59c11::NativePickupOutstandingD72();
}
static std::uint32_t SmallPickupAdmissionR24(std::uint64_t key,bool contact) noexcept {
    if(contact)return SmallContactAdmissionR24(key);
    namespace L=kcd2vr::lab58;namespace P=kcd2vr::lab_prop58;
    auto now=std::uint64_t(GetTickCount64());const auto tick=g_stage_tick596.load();
    const bool captureD85R5R1=kcd2vr::diagnostics::capture168::active(
        kcd2vr::diagnostics::capture168::Domain::Interaction,now);
    const char* phaseD85R5R1="READINESS_OR_CONTEXT";
    std::uint32_t returnedGramsD85R5R1=0;
    const auto noteD85R5R1=[&]() noexcept {
        // R120: switch ON but this pickup is not prepared for the instant route -> one short line, always (not F5-gated),
        // once per item/phase, so the next log says WHY a pickup with "Instant interactions" ON played the game animation.
        if(kcd2vr::instant_d72::Interactions()&&!returnedGramsD85R5R1){
            static thread_local std::uint64_t lastKeyR120=0;static thread_local const char* lastPhaseR120=nullptr;
            if(key!=lastKeyR120||phaseD85R5R1!=lastPhaseR120){lastKeyR120=key;lastPhaseR120=phaseD85R5R1;
                char r120[200]{};std::snprintf(r120,sizeof(r120),"R120 INSTANT_PICKUP_NOT_PREPARED key=%016llX phase=%s (switch ON; this pickup falls back to the game animation)",
                    (unsigned long long)key,phaseD85R5R1);diagnostic_trace::Write(r120);}
        }
        namespace cap=kcd2vr::diagnostics::capture168;
        if(!captureD85R5R1||!cap::active(cap::Domain::Interaction,now))return;
        static thread_local kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
        const auto session=cap::gate.session[unsigned(cap::Domain::Interaction)].load(std::memory_order_acquire);
        if(!gate.observe(session,now,{std::uintptr_t(key),0,-1,int(returnedGramsD85R5R1)},100,128,1000))return;
        char line[320]{};
        std::snprintf(line,sizeof(line),
            "D85R5R1 F5_PICKUP_PREPARE_NATIVE key=%016llX phase=%s grams=%u prepared=NOT_EXECUTED_BY_NATIVE_READER",
            (unsigned long long)key,phaseD85R5R1,returnedGramsD85R5R1);
        diagnostic_trace::Write(line);
    };
    struct ReceiptD85R5R1 {const decltype(noteD85R5R1)& note;~ReceiptD85R5R1(){note();}} receiptD85R5R1{noteD85R5R1};
    const bool instant=SmallInstantPickupAllowedD81(now);
    const bool physicsDriveD85R5R2=prop5916::enabled.load(std::memory_order_acquire);
    const auto allowed=[&](std::uint64_t at){return instant?SmallInstantPickupAllowedD81(at):
        L::active(L::Sub::PropWake)&&L::f9Rung.load()==3&&!L::uiBlocked.load()&&g_grab_mode60.load()==2&&
        kcd2vr::workstation178r20c1::ownsWrist(at);};
    // Static inventory items need a native rigid body in both hold modes.
    // The kinematic mode already uses PropWorldHand596 in BuildStagedPickup596;
    // requiring the velocity-drive option here prevented its preparation.
    // Keep the separate legacy F9 experiment's existing physics requirement.
    // R96: an old drive fault is cleared by the next real acquisition
    // (BuildStagedPickup596); it must not block item preparation forever.
    if(!allowed(now)||!retail_build112::Ready(WhBase())||g_prop_replay596||
       (!instant&&!physicsDriveD85R5R2)||!(g_lifecycle_coverage175.load()&1u))return 0;
    phaseD85R5R1="CALLBACK_THREAD_OR_FRAME";
    if(g_stage_thread596.load()!=GetCurrentThreadId()||!tick||now<tick||now-tick>80||
       (instant&&!g_committed_pose_valid6689.load()))return 0;
    phaseD85R5R1="PLAYER_IDENTITY";
    std::uint32_t actor=0;auto* henry=g_last_player_entity.load();
    if(!henry||!Read(static_cast<unsigned char*>(henry)+0xC,actor)||!LifecycleHenry165(actor))return 0;
    // Station/carry presentation has its own actual tool/resource owner.
    phaseD85R5R1="STATION_OR_EXISTING_HOLD";
    if(HeldInteractionVisual43())return 0;
    PropLock596 lock;if(!lock.owned||g_prop596.id)return 0;
    phaseD85R5R1="ITEM_INSTANCE";
    void* item=SmallPickupItemR24(key);void* definition=nullptr;void* link=nullptr;std::uint32_t id=0;
    unsigned char guid[16]{};
    if(!item||!Read(static_cast<unsigned char*>(item)+0x48,definition)||!definition||
       !Read(static_cast<unsigned char*>(definition)+8,guid)||
       !Read(static_cast<unsigned char*>(item)+0x88,link)||!link||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return 0;
    // D85 interact: native definition Weight (S_PickableItemClass+0x58) for the
    // allowed pickable classes; the retained R24 rows stay as a fallback only.
    phaseD85R5R1="NATIVE_ITEM_CLASS_OR_WEIGHT";
    std::uintptr_t definitionVftD85=0;float weightD85=0;
    const auto base=WhBase();
    if(!Read(definition,definitionVftD85)||!Read(static_cast<unsigned char*>(definition)+0x58,weightD85))return 0;
    auto grams=P::pickupNativeGramsD85(definitionVftD85>=base?definitionVftD85-base:0,weightD85);
    if(!grams)grams=P::pickupMassGramsR24(guid);
    if(!grams)return 0;
    phaseD85R5R1="ENTITY_OR_STATIC_BODY";
    void* entity=EntityById6694(id);std::uint32_t liveId=0;
    if(!LifecycleEntity165(reinterpret_cast<std::uintptr_t>(entity),liveId)||liveId!=id||entity==henry)return 0;
    void* body=nullptr;std::uintptr_t vft=0;
    if(!SmallPickupBodyR24(entity,body)||(body&&!Read(body,vft)))return 0;
    // Existing rigid bodies stay on their original path. Never replace living,
    // articulated, unknown physics types or a different native proxy contract.
    if(body&&vft!=WhBase()+0x3A578C0)return 0;
    phaseD85R5R1="CURRENT_COMMITTED_HAND_FRAME";
    CommittedFrame6695 f{};TrackingSnapshot t{};Matrix34V6550 tm{};
    if(!ReadCommittedPacket6695(f)||!f.stamp.tick||
       f.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
       f.stamp.epoch!=g_pose_epoch6689.load())return 0;
    if(instant){
        if(!f.mapFoldTrackedM39||!f.headEvidence122)return 0;
        t.tick=f.stamp.tick;t.allowed=t.head_valid=true;t.right_grip=f.mapGripM39[0];t.left_grip=f.mapGripM39[1];
    }else if(!ReadTrackingSnapshot(t))return 0;
    now=std::uint64_t(GetTickCount64());
    if(now<f.stamp.tick||now-f.stamp.tick>80)return 0;
    phaseD85R5R1="TRACKING_OR_RENDER_BOUNDS";
    if(!t.allowed||!t.head_valid||t.head_held||!t.tick||now<t.tick||now-t.tick>80||!GetEntityWorldTmV6552(entity,id,tm))return 0;
    void* renderProxy=nullptr;float localBounds[6]{};
    if(!SmallPickupBoundsR24(entity,renderProxy,localBounds)||!P::pickupBoundsSmallR24(localBounds,tm.m))return 0;
    const pm::Vec3 point{tm.m[3],tm.m[7],tm.m[11]};if(!pm::finite(point))return 0;
    phaseD85R5R1="CURRENT_HAND_ADMISSION";
    int requestedSideD85R5R2=-1;
    const bool requestedHandD85R5R2=instant&&
        action_time_owner59c11::RequestedPickupSideD85(id,requestedSideD85R5R2);
    unsigned pickupHandMask=0;for(int side=0;side<2;++side){ph::Pose hand{};
        if(requestedHandD85R5R2&&side!=requestedSideD85R5R2)continue;
        if(f.valid[side]&&(!physicsDriveD85R5R2||f.propRequestedValid5916c1[side])&&TrackingHandUsable111(side)&&
           !(side?t.left_held:t.right_held)&&PropHandFree596(side)&&(side?t.left_grip:t.right_grip)>=.58&&
           !pickup_input_d80::refusal(f,side,FistInputOwned101())&&
           PropPickupHandD81(f,side,physicsDriveD85R5R2,hand)&&pm::length(hand.p-point)<.25)pickupHandMask|=1u<<side;
    }
    phaseD85R5R1="CURRENT_IDENTITY_RECHECK";
    void* currentDefinition=nullptr;void* currentLink=nullptr;unsigned char currentGuid[16]{};
    if(!pickupHandMask||!allowed(now)||
       g_last_player_entity.load()!=henry||!LifecycleHenry165(actor)||SmallPickupItemR24(key)!=item||
       EntityById6694(id)!=entity||!Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
       !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(currentGuid,guid,16)||
       !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link)return 0;
    void* finalBody=nullptr;std::uintptr_t finalVft=0;
    if(!SmallPickupBodyR24(entity,finalBody)||finalBody!=body||
       (finalBody&&(!Read(finalBody,finalVft)||finalVft!=vft)))return 0;
    // Requalify the same current render box/world after all lookup/body leaves.
    // This is a bounded synchronous observation, not a retained slot/geometry lease.
    void* currentRenderProxy=nullptr;float currentBounds[6]{};Matrix34V6550 currentWorld{};
    if(!SmallPickupBoundsR24(entity,currentRenderProxy,currentBounds)||currentRenderProxy!=renderProxy||
        std::memcmp(currentBounds,localBounds,sizeof(localBounds))||
        !GetEntityWorldTmV6552(entity,id,currentWorld)||std::memcmp(currentWorld.m,tm.m,sizeof(tm.m))||
        EntityById6694(id)!=entity)return 0;
    // The new native render reads cannot authenticate an earlier item/body chain.
    if(SmallPickupItemR24(key)!=item||
        !Read(static_cast<unsigned char*>(item)+0x48,currentDefinition)||currentDefinition!=definition||
        !Read(static_cast<unsigned char*>(definition)+8,currentGuid)||std::memcmp(currentGuid,guid,16)||
        !Read(static_cast<unsigned char*>(item)+0x88,currentLink)||currentLink!=link||
        !SmallPickupBodyR24(entity,finalBody)||finalBody!=body||
        (finalBody&&(!Read(finalBody,finalVft)||finalVft!=vft))||
        !GetEntityWorldTmV6552(entity,id,currentWorld)||std::memcmp(currentWorld.m,tm.m,sizeof(tm.m))||
        EntityById6694(id)!=entity)return 0;
    phaseD85R5R1="LATEST_FRAME_AND_HAND_FRESHNESS";
    const auto finishNow=std::uint64_t(GetTickCount64());
    if(finishNow<now||finishNow-tick>80||finishNow-f.stamp.tick>80||finishNow-t.tick>80||
        prop5916::enabled.load(std::memory_order_acquire)!=physicsDriveD85R5R2||
        g_stage_thread596.load()!=GetCurrentThreadId()||g_stage_tick596.load()!=tick||
        !TrackingHeadUsable111(finishNow)||
        (t.source_epoch&&t.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)))return 0;
    for(int side=0;side<2;++side)if((pickupHandMask&(1u<<side))&&
        (!TrackingHandUsable111(side,finishNow)||!PropHandFree596(side)))pickupHandMask&=~(1u<<side);
    if(!pickupHandMask)return 0;
    phaseD85R5R1="FINAL_NATIVE_ADMISSION";
    std::uint32_t currentId=0;
    returnedGramsD85R5R1=Read(static_cast<unsigned char*>(link)+0x30,currentId)&&currentId==id&&
        allowed(finishNow)&&
        g_last_player_entity.load()==henry&&LifecycleHenry165(actor)&&g_committed_pose_valid6689.load()&&
        f.stamp.actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())&&
        f.stamp.epoch==g_pose_epoch6689.load()?grams:0;
    return returnedGramsD85R5R1;
}
// Ordinary instant pickup needs the same narrow static-item preparation as
// the F9 experiment. Prepare the native interception on the current native
// frame BEFORE Lua PickableItem.Use: its preparation precedes item:OnUsed,
// so installing only from OnUsed made the first static pickup fail by design.
// Exact pin/partial-install failures remain terminal for that installation.
static void UpdateSmallPickupReaderR24(bool active) noexcept {
    const bool requested=kcd2vr::instant_d72::Interactions()&&
        !kcd2vr::instant_pickup_d72::stopping.load(std::memory_order_acquire);
    static bool nativeAttemptedD85R5R1=false;
    if(!requested&&!g_instant_pickup_ready_d72.load(std::memory_order_acquire))nativeAttemptedD85R5R1=false;
    if(requested&&!g_instant_pickup_ready_d72.load(std::memory_order_acquire)&&
       !nativeAttemptedD85R5R1&&retail_build112::Ready(WhBase())&&action_time_owner59c11::Installed()){
        nativeAttemptedD85R5R1=true;
        (void)InstallInstantPickupD72();
    }
    active=active||(requested&&g_instant_pickup_ready_d72.load(std::memory_order_acquire));
    static bool registered=false;
    static unsigned attempts=0;
    static std::uint64_t nextAttempt=0;
    if(active==registered){attempts=0;nextAttempt=0;return;}
    const auto now=std::uint64_t(GetTickCount64());
    // R96: four refused registrations no longer end small pickups until restart: a save/level load starts a
    // fresh set of four, and after that one slow retry every 5 s (one native installer call, nothing per frame).
    {static kcd2vr::load_rearm_r96::Seen loadR96;if(loadR96.take()){attempts=0;nextAttempt=0;}}
    if(active&&attempts>=4&&nextAttempt&&now>=nextAttempt)attempts=3;
    // A busy installer may refuse a registration. Commit the edge only after
    // success; four bounded attempts avoid a silent permanent lost edge while
    // preserving the door installer's retained-failure policy. No frame scan.
    // R96: after 4 refused registrations wait 5 s and try again instead of
    // never registering until restart (old attempts>=4 session latch).
    if(active&&nextAttempt&&now<nextAttempt)return;
    if(kcd2vr::door_signal19c4r4::SetSmallPickupReaderR24(active?&SmallPickupAdmissionR24:nullptr)){
        registered=active;attempts=0;nextAttempt=0;
    }else{++attempts;nextAttempt=now+(attempts>=4?5000:500);if(attempts>=4)attempts=0;}
}
