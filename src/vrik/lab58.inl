// C58 PHYSICS LAB (F9 rung 3) native side. included by frik_pose_probe.cpp after impale_part59c7g.inl (needs
// touch43::PoseOf/CallAddImpact, gore32n::PartVelocity, ImpaleRoot47/PartStatus47/PickRagdollPart47, FlinchEntityById178r11,
// CharacterForEntity6689, ReadParentDirect/ReadJointCount). runs ONLY from NativeRhythmTick178 (the native frame tick
// that already drives the impale hang / gore corpse routes). no new hook, no per-frame native work unless a sub switch is on.
//
// pins (all already in the tree, none new):
//   living impact: CSkeletonPose vt 0x3A71028 slot +0x58 AddImpact 0x37C66A4 (touch_react43)
//   corpse drag:   CArticulatedEntity vt 0x3A48B30, GetStatus +0x30 = 0x8A9348 pe_status_pos type 1 by partid
//                  (part_anchor59c7g pins), Action +0x38 = 0xA99DF0 pe_action_set_velocity type 10 with partid
//                  (gore32 Pins), foreign data +0x28 type 1/2 / +0x20 exact current entity (corpse-qualified ImpaleRoot47 policy)
#include "../subsystems/physics/saved_corpse_drag_d72.h"
namespace lab58n {
namespace L=kcd2vr::lab58;
// ------------------------------------------------------------ B. living hit impact
struct LivingRec {std::uint32_t entity=0;int part=-1;std::uint64_t sample=0,tick=0;double point[3]{},normal[3]{},speed=0;};
static LivingRec g_living[8]{};static unsigned g_livingAt=0;
static L::ImpactBudget g_budget;static std::uint64_t g_impactCalls=0,g_impactSkips=0;
// every fresh Living impact record (both hands) is remembered for <= 1 s so the game's answer can be matched to its
// point/normal/speed by the impact sample id. the records are the ones the hit path already reads; nothing new is polled.
static void RememberLiving(const PhysFrame6698& f) noexcept {
    for(int s=0;s<2;++s){const auto& r=f.output.impactRecord178r30[s];
        if(!r.valid||!r.sample||!r.entityId||r.body!=int(kcd2vr::npc_contact172::Body::Living))continue;
        bool have=false;for(const auto& x:g_living)if(x.sample==r.sample){have=true;break;}if(have)continue;
        LivingRec& x=g_living[g_livingAt++%8];x={};x.entity=r.entityId;x.part=r.part;x.sample=r.sample;x.tick=r.tick;x.speed=r.speed;
        x.point[0]=r.point.x;x.point[1]=r.point.y;x.point[2]=r.point.z;x.normal[0]=r.normal.x;x.normal[1]=r.normal.y;x.normal[2]=r.normal.z;}
}
static const LivingRec* FindLiving(std::uint64_t sample,std::uint32_t entity) noexcept {
    for(const auto& x:g_living)if(x.sample==sample&&x.entity==entity)return &x;return nullptr;}
// D44: this answered-hit record contains only a physics part number, not a
// current character/shape-to-joint witness. The native RPG hit has already run.
// Preserve that gameplay answer and explicitly refuse the unsupported extra
// skeletal impulse. D51 also suspends the separate free-hand H9 joint/capsule
// actuators until animation/physics ownership is established per living actor.
static void LivingReceipt(std::uint64_t now,const char* text) noexcept {
    if(!L::captureActiveD84(now))return;diagnostic_trace::WriteFor(L::captureDomainD84(now),text);
}
static void LivingTick(std::uint64_t now) noexcept {
    combat_diag_probe::LabAnsweredLiving58 hits[8]{};const unsigned n=combat_diag_probe::LabTakeAnsweredLiving58(hits,8);
    if(!n||!L::active(L::Sub::LivingHitImpact))return;
    for(unsigned i=0;i<n;++i){const auto& h=hits[i];const auto* rec=FindLiving(h.sample,h.entity);const char* why=nullptr;
        if(!rec)why="NO_IMPACT_RECORD";
        else if(now<h.tick||now-h.tick>250||now<rec->tick||now-rec->tick>1000)why="STALE";
        else if(touch43::g_latched)why="FAULT_LATCHED_OFF";
        else if(!L::impactLeaseOk.load(std::memory_order_relaxed))why="LEASE_UNAVAILABLE";
        else why="NO_CURRENT_JOINT_WITNESS";
        ++g_impactSkips;char m[280]{};std::snprintf(m,sizeof(m),
            "C58 LAB_LIVING_IMPACT target=%u part=%d serial=%u result=SKIPPED reason=%s skips=%llu nativeAnswer=%d extraNativeCall=NONE damage=NONE",
            h.entity,h.part,h.serial,why,(unsigned long long)g_impactSkips,h.reaction);LivingReceipt(now,m);
    }
}
// ------------------------------------------------------------ C. corpse limb drag (W5: both hands, 0 RIGHT 1 LEFT)
// Miki 25 Oct: "grabbing and pulling their corpses with our HANDS". was LEFT only, and a refused squeeze said nothing.
// each hand has its own grab state, candidate, CG1 publisher and worker-consumed squeeze; the two hands may hold two
// different parts (or two corpses), never the same part. D40 retains the exact
// acquisition hand point in the current part's local frame; the part origin
// must never snap to the wrist. This remains a bounded linear velocity motor.
namespace corpseMotorD40=kcd2vr::corpse_body_owner_d40;
static L::DragState g_drag[2];static std::uintptr_t g_dragRoot[2]{};static std::uint64_t g_dragSearch[2]{};static std::uint64_t g_dragGrabs=0;
// One fault source: duplicating this as a bool used to keep dragging off
// forever even after the native Action capability could be requalified.
static auto& g_dragFault=gore32n::g_drive_fault33;
static std::uint64_t g_dragLoadEpisodeR96[2]{};
static std::uint64_t g_dragIdleTickD85R5R2[2]{};
static kcd2vr::corpse_grip_cg1::Publisher g_dragInputCg1[2];
static L::DragCandidate g_dragPendingCg1[2]{};static std::uintptr_t g_dragPendingRootCg1[2]{};
static std::uint64_t g_dragPressCg1[2]{};
static std::uint64_t g_dragPhysicsEpochD40[2]{};
static bool g_labServiceDirtyD80=false; // native-frame owner only; candidate cleanup owed after use
struct DragPartD40 {ph::Pose pose{};double scale=0;std::uintptr_t geometry=0,proxyGeometry=0;};
static kcd2vr::partanchor59c7g::Anchor g_dragAnchorD40[2];
static DragPartD40 g_dragPartD40[2];
static corpseMotorD40::Lease g_dragMotorD40[2];
// R101: whole-body follow + drive shaper per hand (corpse_follow_r101.h); cleared with the owner.
static kcd2vr::corpse_follow_r101::Rest g_dragFollowR101[2];static kcd2vr::corpse_follow_r101::Shaper g_dragShapeR101[2];
static int OtherHandPartR101(int side,std::uintptr_t body) noexcept; // saved_corpse_drag_d72.inl
struct DragDiagW5 {double nearM=-1;unsigned qualified=0,unqualified=0;const char* why="";std::uint64_t tick=0;bool gripWas=false,bodyOwned=false;std::uint64_t told=0;};
static DragDiagW5 g_dragDiag[2];
static bool DragInputCurrentCg1(int side,std::uint64_t now) noexcept {
    TrackingSnapshot t{};
    if(!ReadTrackingSnapshot(t))return false;
    now=std::uint64_t(GetTickCount64());
    return t.head_valid&&!t.head_held&&
        (side?(t.left_valid&&!t.left_held):(t.right_valid&&!t.right_held))&&
        t.tick&&now>=t.tick&&now-t.tick<=80&&
        t.input_tick&&now>=t.input_tick&&now-t.input_tick<=80&&
        t.source_epoch&&t.source_epoch==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
}
static bool DragInputCurrentCg1(std::uint64_t now) noexcept {return DragInputCurrentCg1(1,now);}

static bool DragUiOk() noexcept {
    return g_gameplay_allowed.load(std::memory_order_acquire)&&!g_ctx_menu.load(std::memory_order_relaxed)&&
        !g_ctx_dialogue.load(std::memory_order_relaxed)&&!g_ctx_cutscene.load(std::memory_order_relaxed)&&!L::uiBlocked.load(std::memory_order_relaxed);
}
// hand EMPTY: no exact held item, no native grasp (F5 physical grip) holding, no prop servo presenting on that side
static bool HandEmptyW5(int side) noexcept {
    if(side<0||side>1)return false;
    if(ExactHandItemState(side)!=0)return false;
    if(HeldInteractionVisual43(side))return false;
    kcd2vr::prop5916c1::Presentation pr{};if(kcd2vr::prop5916c1::presentation.read(pr)&&pr.body&&pr.side==side)return false;
    return true;
}
static bool GripHeldW5(int side) noexcept {
    PhysFrame6698 f{};if(!g_phys_frame6698.read(f))return false;
    const auto now=std::uint64_t(GetTickCount64());
    if(!f.input.tick||now<f.input.tick||now-f.input.tick>150||!f.input.tracked[side])return false;
    // The worker already owns the deliberate >.58 acquisition. Keep the same
    // <.30 release hysteresis here; relaxing to .4 must not silently drop it.
    return std::isfinite(f.input.grip[side])&&f.input.grip[side]>=kcd2vr::corpse_grip_cg1::kReleaseGrip&&f.input.grip[side]<=1;
}
// R89 corpse (R90 option): the current raw grip of one hand from the same input frame GripHeldW5 uses.
static bool GripValueR89(int side,std::uint64_t now,float& grip) noexcept {
    grip=0;PhysFrame6698 f{};
    if(side<0||side>1||!g_phys_frame6698.read(f))return false;
    now=std::uint64_t(GetTickCount64());
    if(!f.input.tick||now<f.input.tick||now-f.input.tick>150||!f.input.tracked[side])return false;
    grip=f.input.grip[side];return std::isfinite(grip);
}
static kcd2vr::corpse_grip_cg1::SearchWindowR89 g_labSearchR89[2];
// the articulated root of a corpse the game killed: identity + death receipt + parent proof (combat_diag_probe, same rules as
// the rung 2 Deliver) and the foreign-data proof root==this entity (ImpaleRoot47). read-only.
static bool DragRoot(std::uint32_t id,std::uintptr_t identity,std::uint64_t now,std::uintptr_t& root,const char*& why) noexcept {
    root=0;why="";
    if(!L::active(L::Sub::CorpseDrag)){why="CORPSE_DRAG_OFF";return false;}
    if(!combat_diag_probe::LabCorpseQualified58(id,identity,g_pose_epoch6689.load(std::memory_order_acquire),now,why))return false;
    void* entity=FlinchEntityById178r11(id);if(!entity||(identity&&reinterpret_cast<std::uintptr_t>(entity)!=identity)){why="ENTITY_IDENTITY_CHANGED";return false;}
    kcd2vr::partanchor59c7g::Identity pid{};pid.serial=1;pid.target=id;pid.entity=reinterpret_cast<std::uintptr_t>(entity);
    if(ImpaleRoot47(pid,root,why,true)!=kcd2vr::impale47::Poll::Ready||!root)return false;
    if(!L::active(L::Sub::CorpseDrag)){root=0;why="CORPSE_DRAG_OFF";return false;}
    return true;
}
// A native status call may reenter entity/physics work. Join the original
// entity/root again after it; an old raw part ID is not a transferable limb.
static bool ReadDragPartD40(std::uint32_t id,std::uintptr_t identity,std::uintptr_t expectedRoot,int part,
    std::uint64_t now,DragPartD40& out,const char*& why) noexcept {
    out={};std::uintptr_t root=0;
    if(!DragRoot(id,identity,now,root,why)||root!=expectedRoot){why="CURRENT_BODY_CHANGED";return false;}
    kcd2vr::partanchor59c7g::Status st(part);
    if(!PartStatus47(root,st)||st.index!=-1||!kcd2vr::partanchor59c7g::decode(st,part,out.pose,out.scale)){
        out={};why="CURRENT_PART_POSE_UNAVAILABLE";return false;}
    out.geometry=st.geometry;out.proxyGeometry=st.proxyGeometry;
    if(!DragRoot(id,identity,now,root,why)||root!=expectedRoot){out={};why="BODY_CHANGED_DURING_PART_READ";return false;}
    return true;
}
static bool UpdateDragPointD40(int side,const DragPartD40& part,ph::Vec3& point) noexcept {
    const auto& original=g_dragPartD40[side];
    return part.geometry==original.geometry&&part.proxyGeometry==original.proxyGeometry&&
        g_dragAnchorD40[side].update(g_dragAnchorD40[side].owner,part.pose,part.scale,point);
}
// Native release is acknowledgment of this velocity owner's retirement, not
// a write to the body. In particular, do not zero its existing momentum.
static void ClearDragOwnerD40(int side) noexcept {
    // R103: the corpse collides with Henry again once it is clear of him (or after 4 s)
    self_collision_r103n::Release(kcd2vr::self_collision_r103::corpseLabOwner(side),std::uint64_t(GetTickCount64()));
    corpseMotorD40::claims.release(g_dragMotorD40[side]);g_dragAnchorD40[side].clear();g_dragPartD40[side]={};
    g_dragFollowR101[side].clear();g_dragShapeR101[side].reset();
    g_dragRoot[side]=0;g_dragPressCg1[side]=0;g_dragPhysicsEpochD40[side]=0;g_dragPendingCg1[side]={};g_dragPendingRootCg1[side]=0;
    g_dragInputCg1[side].clear();kcd2vr::corpse_grip_cg1::claims.held(side,g_drag[side].held);
}
// D84: physics receipts require the finite F9 or F3 recorder; feature selection is separate.
static void DragReceipt(std::uint64_t now,const char* text) noexcept {
    if(!L::captureActiveD84(now))return;diagnostic_trace::WriteFor(L::captureDomainD84(now),text);}
static bool OtherHandHoldsW5(int side,std::uint32_t entity,int part) noexcept {
    const auto& o=g_drag[1-side];return o.held&&o.entity==entity&&o.part==part;
}
// candidate search for one hand: at most every 100 ms, <= 16 game-killed entities, root part first (far bodies skip the part scan).
// fills the diagnostics (nearest qualified part, how many corpses failed their proof and why) for the visible line.
static void DragSearchW5(int side,std::uint64_t now,const L::DragTuning& tune,float hx,float hy,float hz) noexcept {
    if(g_dragSearch[side]&&now>=g_dragSearch[side]&&now-g_dragSearch[side]<100)return;
    g_dragSearch[side]=now;
    auto& diag=g_dragDiag[side];diag.nearM=-1;diag.qualified=0;diag.unqualified=0;diag.why="";diag.tick=now;diag.bodyOwned=false;
    g_dragPendingRootCg1[side]=0;
    std::uint32_t ids[16]{};const unsigned n=combat_diag_probe::LabDeathCandidates58(ids,16,now);
    L::DragCandidate best{};double bestD=tune.grabRadius;double nearAny=-1;
    for(unsigned i=0;i<n;++i){const char* why="";std::uintptr_t root=0;
        void* e=FlinchEntityById178r11(ids[i]);if(!e){++diag.unqualified;diag.why="ENTITY_GONE";continue;}
        const auto ident=reinterpret_cast<std::uintptr_t>(e);
        if(!DragRoot(ids[i],ident,now,root,why)){++diag.unqualified;diag.why=why&&*why?why:"NO_ROOT";continue;}
        kcd2vr::partanchor59c7g::Status rs(-1);rs.index=0;ph::Pose rp{};double rsc=0;
        if(!PartStatus47(root,rs)||!kcd2vr::partanchor59c7g::decode(rs,rs.part,rp,rsc)){++diag.unqualified;diag.why="ROOT_POSE_UNAVAILABLE";continue;}
        ++diag.qualified;
        if(kcd2vr::pose_core::length(rp.p-ph::Vec3{hx,hy,hz})>2.5)continue;
        // D85R5R2: being within 2.5 m of the root is not a prospective limb
        // grab. The existing exact render-proxy bounds leaf can reject a hand
        // outside the whole current corpse before enumerating all its parts.
        // Unknown bounds fall through; this read never grants acquisition.
        void* boundsProxy=nullptr;float bounds[6]{};Matrix34V6550 boundsWorld{};
        if(SmallPickupBoundsR24(e,boundsProxy,bounds)&&GetEntityWorldTmV6552(e,ids[i],boundsWorld)&&
           kcd2vr::corpse_search_d85r5r2::outsideWorldBounds(bounds,boundsWorld.m,hx,hy,hz))continue;
        // POLICY (S4): this is the part of the PROVEN corpse root nearest to the HAND for a grab (there is no struck part);
        // it is NOT the C52 hit-to-corpse nearest-part mapping (which stays unapproved). every part comes from GetStatus on
        // this root only, and DragRoot above proved root's foreign data (+0x28 type 1/2, +0x20 entity) is this current entity.
        ph::Pose pose{};double sc=0;AnchorPart47 pick{};
        if(!PickRagdollPart47(root,-1,ph::Vec3{hx,hy,hz},pose,sc,pick)||pick.part<0)continue;
        const double d=kcd2vr::pose_core::length(pose.p-ph::Vec3{hx,hy,hz});
        if(nearAny<0||d<nearAny)nearAny=d;
        if(OtherHandHoldsW5(side,ids[i],pick.part))continue;
        if(!corpseMotorD40::claims.handAvailable(side,ident,root,pick.part,g_pose_epoch6689.load(std::memory_order_acquire))){
            diag.bodyOwned=true;diag.why="BODY_OWNED_BY_IMPALE_OR_OTHER_GRIP";continue;}
        if(d<bestD){bestD=d;best.valid=true;best.entity=ids[i];best.identity=ident;best.part=pick.part;best.pos={pose.p.x,pose.p.y,pose.p.z};g_dragPendingRootCg1[side]=root;}}
    diag.nearM=nearAny;
    g_dragPendingCg1[side]=best;
}
// D72 declarations: alternate provider owns only its explicitly tagged candidate.
// D78 raw hand intent is shared by saved and articulated corpse routes.
// Definition below retains the completed-pose, tracking and claim checks.
static bool SavedHandIntentD72(int side,float& x,float& y,float& z) noexcept;
static bool SavedDragOwnsD72(int side) noexcept;
static void InstallSavedDragD72() noexcept;
static void SavedWatchdogD72(std::uint64_t now) noexcept;
static void DragTickSide(int side,std::uint64_t now) noexcept {
    if(SavedDragOwnsD72(side))return; // native-script owner services/releases its own CG1 claim
    auto& drag=g_drag[side];auto& publisher=g_dragInputCg1[side];publisher.side=side;
    const bool on=L::active(L::Sub::CorpseDrag);
    const auto physicsEpochD40=kcd2vr::collab_h21::epoch.load(std::memory_order_acquire);
    const auto visitR96=kcd2vr::observed_load_recovery_r96::observe();
    if(visitR96.loading||(g_dragLoadEpisodeR96[side]&&g_dragLoadEpisodeR96[side]!=visitR96.episode)){
        // Same-address save reload retires only our velocity/input ownership.
        // Release has no native write and cannot discard a constraint debt.
        const bool oldOwner=drag.held||g_dragPendingCg1[side].valid||publisher.current.tick;
        if(drag.held){L::DragInput release{};release.now=now;(void)L::stepDrag(drag,release);}
        ClearDragOwnerD40(side);g_dragLoadEpisodeR96[side]=visitR96.episode;
        if(visitR96.loading||oldOwner)return;
    }
    g_dragLoadEpisodeR96[side]=visitR96.episode;
    if(!on&&!drag.held){publisher.clear();g_dragPendingCg1[side]={};g_dragDiag[side].gripWas=false;return;}
    // R90 'Corpse search' = ONLY ON SQUEEZE (R89): no hand/body/native part reads
    // unless this hand is starting a squeeze (or the worker already consumed one
    // for a published candidate). ALWAYS (R2) skips this block.
    if(kcd2vr::lab58::corpseSqueezeSearchR90.load(std::memory_order_acquire)){
     float grip=0;const bool fresh=GripValueR89(side,now,grip);
     const bool searching=g_labSearchR89[side].update(fresh,grip,now);
     if(!drag.held&&!searching&&!kcd2vr::corpse_grip_cg1::claims.consumed[side].load(std::memory_order_acquire)){
        publisher.clear();g_dragPendingCg1[side]={};g_dragPendingRootCg1[side]=0;g_dragDiag[side].gripWas=false;
        corpseMotorD40::claims.release(g_dragMotorD40[side]);kcd2vr::corpse_grip_cg1::claims.held(side,false);return;}}
    // An empty discovery result is not a native owner. Previously the 100 ms
    // search guard ran AFTER per-frame tracking, committed-pose and native hand
    // reads. Preserve active candidate/held validation and release immediately.
    if(!drag.held&&!g_dragPendingCg1[side].valid&&!publisher.current.tick){
        if(!kcd2vr::corpse_search_d85r5r2::due(g_dragIdleTickD85R5R2[side],now))return;
        g_dragIdleTickD85R5R2[side]=now;
    }
    L::DragInput in{};in.now=now;in.fault=g_dragFault.load(std::memory_order_relaxed)||gore32n::g_drive_fault33.load(std::memory_order_relaxed);
    const bool current=on&&DragInputCurrentCg1(side,now);const bool rawGrip=current&&GripHeldW5(side);
    in.gripHeld=rawGrip;in.uiOk=on&&DragUiOk();in.handEmpty=on&&HandEmptyW5(side);
    float hx=0,hy=0,hz=0;in.handValid=current&&!animation_filter::nativeBodyOwned112.load()&&SavedHandIntentD72(side,hx,hy,hz);in.hand={hx,hy,hz};
    const L::DragTuning tune{};DragPartD40 candidatePart{};
    if(drag.held){
        const char* why="";DragPartD40 part{};ph::Vec3 point{};
        in.entityQualified=kcd2vr::collab_h21::current(g_dragPhysicsEpochD40[side])&&corpseMotorD40::claims.current(g_dragMotorD40[side])&&
            ReadDragPartD40(drag.entity,drag.identity,g_dragRoot[side],drag.part,now,part,why);
        if(in.entityQualified&&UpdateDragPointD40(side,part,point)){in.partValid=true;in.part={point.x,point.y,point.z};}
    } else if(in.uiOk&&in.handEmpty&&in.handValid&&!in.fault&&(!drag.lastRelease||(now>=drag.lastRelease&&now-drag.lastRelease>=tune.regrabMs))){
        DragSearchW5(side,now,tune,hx,hy,hz);
        if(g_dragPendingCg1[side].valid){
            const char* why="";const auto& c=g_dragPendingCg1[side];
            if(!OtherHandHoldsW5(side,c.entity,c.part)&&
               ReadDragPartD40(c.entity,c.identity,g_dragPendingRootCg1[side],c.part,now,candidatePart,why)&&
               corpseMotorD40::claims.handAvailable(side,c.identity,g_dragPendingRootCg1[side],c.part,g_pose_epoch6689.load(std::memory_order_acquire))&&
               kcd2vr::pose_core::length(candidatePart.pose.p-ph::Vec3{hx,hy,hz})<=tune.grabRadius){
                in.candidate=c;in.candidate.pos={candidatePart.pose.p.x,candidatePart.pose.p.y,candidatePart.pose.p.z};
            }else g_dragPendingCg1[side]={};
        }
    }
    // Native candidate availability precedes Grip. A candidate grants NO drive;
    // only the exact worker-consumed squeeze of THIS hand may start/continue this grab.
    kcd2vr::corpse_grip_cg1::Candidate available{};
    if(in.uiOk&&in.handEmpty&&in.handValid&&!in.fault&&
       (drag.held?(in.entityQualified&&in.partValid):in.candidate.valid)){
        kcd2vr::corpse_grip_cg1::Key key{};key.actor=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load());
        key.epoch=g_pose_epoch6689.load();key.entity=drag.held?drag.entity:in.candidate.entity;
        key.identity=drag.held?drag.identity:in.candidate.identity;
        key.root=drag.held?g_dragRoot[side]:g_dragPendingRootCg1[side];key.part=drag.held?drag.part:in.candidate.part;
        key.physicsEpoch=drag.held?g_dragPhysicsEpochD40[side]:physicsEpochD40;
        if(key.actor&&key.epoch)available=publisher.publish(key,now,drag.held);
        else publisher.clear();
    }else publisher.clear();
    // CG1_NATIVE_CLAIM_BEGIN: only new acquisition takes the shared lease.
    // An existing drag must always run native release cleanup, even on contention.
    kcd2vr::hand_only_door_h40::Admission corpseAdmissionCg1;
    kcd2vr::corpse_grip_cg1::Request request{};
    const bool requestedCg1=kcd2vr::corpse_grip_cg1::handRequests[side].read(request)&&
        kcd2vr::corpse_grip_cg1::claims.authority(side,request)&&
        kcd2vr::corpse_grip_cg1::permits(request,available,now)&&(!drag.held||request.press==g_dragPressCg1[side]);
    const bool entryCg1=drag.held||(requestedCg1&&in.gripHeld&&corpseAdmissionCg1.take(1u<<side));
    const bool consumed=entryCg1&&requestedCg1&&kcd2vr::corpse_grip_cg1::claims.authority(side,request);
    in.gripHeld=in.gripHeld&&consumed;
    kcd2vr::partanchor59c7g::Anchor newAnchor{};
    if(!drag.held&&in.gripHeld&&in.candidate.valid){
        const auto& key=request.key;
        const kcd2vr::partanchor59c7g::Identity owner{request.press,key.epoch,key.serial,key.entity,key.identity,key.root,key.part};
        const corpseMotorD40::Key motor{side?corpseMotorD40::Slot::LeftHand:corpseMotorD40::Slot::RightHand,
            key.identity,key.root,request.press,key.epoch,key.part};
        if(!kcd2vr::collab_h21::current(key.physicsEpoch)||!newAnchor.acquire(owner,candidatePart.pose,candidatePart.scale,{hx,hy,hz})||
           !corpseMotorD40::claims.acquire(g_dragMotorD40[side],motor)){
            in.candidate.valid=false;g_dragDiag[side].bodyOwned=true;g_dragDiag[side].why="BODY_OWNER_OR_LOCAL_GRIP_REFUSED";
        }
    }
    const auto before=drag;const L::DragStep step=L::stepDrag(drag,in,tune);
    if(step.event==L::DragEvent::Grab){g_dragAnchorD40[side]=newAnchor;g_dragPartD40[side]=candidatePart;g_dragPhysicsEpochD40[side]=request.key.physicsEpoch;}
    else if(!before.held)corpseMotorD40::claims.release(g_dragMotorD40[side]);
    kcd2vr::corpse_grip_cg1::claims.held(side,drag.held); // actual post-step state, never candidate.active
    // CG1_NATIVE_CLAIM_END
    const bool recording=L::captureActiveD84(now);
    // W5: a new squeeze of an empty hand near a corpse that did not grab says why, once per 3 s per hand.
    auto& diag=g_dragDiag[side];
    const bool edge=rawGrip&&!diag.gripWas;diag.gripWas=rawGrip;
    if(edge&&!drag.held&&step.event!=L::DragEvent::Grab&&(diag.qualified||diag.unqualified)&&
       (diag.nearM<0||diag.nearM<.75)&&(!diag.told||now-diag.told>=3000)){
        L::GrabWhyInput g{};g.handEmpty=in.handEmpty;g.uiOk=in.uiOk;g.inputCurrent=in.handValid;g.fault=in.fault;
        g.consumed=consumed;g.nearM=diag.nearM;g.grabRadius=tune.grabRadius;g.qualified=diag.qualified;g.unqualified=diag.unqualified;g.why=diag.why;g.bodyOwned=diag.bodyOwned;
        g.otherHand=g_drag[1-side].held&&in.candidate.valid==false&&diag.nearM>=0&&diag.nearM<=tune.grabRadius;
        char t[96]{};L::grabWhy(g,t,sizeof(t));
        if(t[0]){diag.told=now;(void)kcd2vr::visible_failure::publish(t,now);
            if(recording){char m[300]{};
                std::snprintf(m,sizeof(m),"C58 LAB_DRAG refused side=%s reason=\"%s\" nearM=%.2f qualified=%u unqualified=%u proof=%s consumed=%d empty=%d current=%d",
                    side?"L":"R",t,diag.nearM,diag.qualified,diag.unqualified,diag.why,consumed?1:0,in.handEmpty?1:0,in.handValid?1:0);DragReceipt(now,m);}}
    }
    if(step.event==L::DragEvent::Grab){++g_dragGrabs;g_dragRoot[side]=request.key.root;g_dragPressCg1[side]=request.press;g_dragPendingCg1[side]={};
        if(recording){char m[300]{};std::snprintf(m,sizeof(m),"C58 LAB_DRAG grab side=%s entity=%u part=%d dist=%.2f grabs=%llu anchor=PART_LOCAL route=ARTICULATED_SET_VELOCITY(A99DF0,partid) constraint=NONE",side?"L":"R",drag.entity,drag.part,L::length(in.hand-in.candidate.pos),(unsigned long long)g_dragGrabs);DragReceipt(now,m);}
    } else if(step.event==L::DragEvent::Release){ClearDragOwnerD40(side);
        if(recording){char m[300]{};
            std::snprintf(m,sizeof(m),"C58 LAB_DRAG release side=%s entity=%u part=%d reason=%s heldMs=%llu finalWrite=NONE",side?"L":"R",before.entity,before.part,step.reason,(unsigned long long)(before.start&&now>=before.start?now-before.start:0));
            DragReceipt(now,m);}
    } else if(step.event==L::DragEvent::Drive){
        // Rejoin after all native reads/admission and use the latest achieved
        // local point. A callback changing the root/part cannot receive a write.
        const char* why="";DragPartD40 currentPart{};ph::Vec3 point{};
        const bool currentOwner=!kcd2vr::observed_load_recovery_r96::observe().loading&&
            g_dragLoadEpisodeR96[side]==kcd2vr::observed_load_recovery_r96::observe().episode&&
            kcd2vr::collab_h21::current(g_dragPhysicsEpochD40[side])&&corpseMotorD40::claims.current(g_dragMotorD40[side])&&
            kcd2vr::corpse_grip_cg1::claims.authority(side,request)&&DragInputCurrentCg1(side,std::uint64_t(GetTickCount64()))&&
            ReadDragPartD40(drag.entity,drag.identity,g_dragRoot[side],drag.part,now,currentPart,why)&&
            UpdateDragPointD40(side,currentPart,point)&&kcd2vr::corpse_grip_cg1::claims.authority(side,request)&&
            kcd2vr::collab_h21::current(g_dragPhysicsEpochD40[side])&&
            DragUiOk()&&HandEmptyW5(side)&&!animation_filter::nativeBodyOwned112.load()&&
            !kcd2vr::observed_load_recovery_r96::observe().loading&&
            g_dragLoadEpisodeR96[side]==kcd2vr::observed_load_recovery_r96::observe().episode;
        L::DragInput verified=in;verified.partValid=currentOwner;verified.part={point.x,point.y,point.z};
        const auto command=L::stepDrag(drag,verified,tune);
        const bool attempted=command.event==L::DragEvent::Drive;
        // R101: rate-limited drive + the rest of the body follows (no latex limbs). Same per-part Action.
        gore32n::g::V3 driveR101{};
        if(attempted){const auto shaped=g_dragShapeR101[side].step({command.v.x,command.v.y,command.v.z},now,{tune.maxSpeed,30});
            driveR101={shaped.x,shaped.y,shaped.z};
            const auto f=gore32n::BodyFollowR101(g_dragFollowR101[side],g_dragRoot[side],drag.part,g_dragPressCg1[side],
                OtherHandPartR101(side,g_dragRoot[side]),now,driveR101);
            static std::uint64_t followLogR101[2]{};
            if(recording&&(!followLogR101[side]||now<followLogR101[side]||now-followLogR101[side]>=500)){followLogR101[side]=now;char m[300]{};
                std::snprintf(m,sizeof(m),"R101 LAB_DRAG follow side=%s entity=%u part=%d ok=%d parts=%d stretched=%d writes=%d refused=%d worstM=%.3f guarded=%d pair=%d v=%.2f",
                    side?"L":"R",drag.entity,drag.part,f.ok?1:0,f.parts,f.plan.stretched,f.writes,f.refused,f.plan.worst,f.plan.guarded?1:0,f.plan.pairLimited?1:0,gore32n::g::len(driveR101));
                DragReceipt(now,m);}}
        if(!attempted||!gore32n::PartVelocity(g_dragRoot[side],drag.part,driveR101)){
            const bool fault=gore32n::g_drive_fault33.load(std::memory_order_relaxed);
            if(drag.held){L::DragInput rel=in;rel.fault=fault;rel.partValid=false;(void)L::stepDrag(drag,rel,tune);}
            ClearDragOwnerD40(side);
            if(recording){char m[300]{};std::snprintf(m,sizeof(m),"C58 LAB_DRAG release side=%s entity=%u part=%d reason=%s finalWrite=NONE",side?"L":"R",before.entity,before.part,
                !attempted?"CURRENT_LOCAL_GRIP_OR_OWNER_CHANGED":fault?"ACTION_FAULT":"ACTION_REFUSED");DragReceipt(now,m);}
            if(attempted)(void)kcd2vr::visible_failure::publish("corpse grab: the game refused the body push (Action), released",now);
        }else{
            // R103 (the dragged body pushed Henry away): while held, the ragdoll ignores living capsules
            self_collision_r103n::Exclude(kcd2vr::self_collision_r103::corpseLabOwner(side),kcd2vr::self_collision_r103::Kind::Corpse,
                drag.entity,drag.identity,g_dragRoot[side],drag.part,std::uint64_t(GetTickCount64()));
        }
    }
}
// Existing native owner only. When its service is disabled, retire the
// actual drag state before returning to the Draw consumer. No native write,
// no candidate-derived ACK and no lease needed for release cleanup.
static void RetireUnservicedCg1(std::uint64_t now) noexcept {
    if(!g_labServiceDirtyD80&&!g_drag[0].held&&!g_drag[1].held&&!SavedDragOwnsD72(0)&&!SavedDragOwnsD72(1))return;
    SavedWatchdogD72(now);g_labServiceDirtyD80=false;
    for(int side=0;side<2;++side){
        if(SavedDragOwnsD72(side))continue;
        auto& drag=g_drag[side];
        const auto& claims=kcd2vr::corpse_grip_cg1::claims;
        if(!drag.held&&!g_dragRoot[side]&&!g_dragPressCg1[side]&&!g_dragPendingCg1[side].valid&&!g_dragMotorD40[side].incarnation&&!g_dragAnchorD40[side].active&&
           !g_dragInputCg1[side].current.tick&&!claims.livePress[side].load(std::memory_order_acquire)&&
           !claims.nativeHeld[side].load(std::memory_order_acquire))continue;
        if(drag.held){L::DragInput release{};release.now=now;(void)L::stepDrag(drag,release);}
        kcd2vr::corpse_grip_cg1::claims.livePress[side].store(0,std::memory_order_release);
        kcd2vr::corpse_grip_cg1::claims.held(side,drag.held);
        g_dragInputCg1[side].side=side;ClearDragOwnerD40(side);
    }
}
static void DragTick(std::uint64_t now) noexcept {DragTickSide(0,now);DragTickSide(1,now);}
// D51: F9 selects the physics mode (historical extraction boundary).
// D84: the finite F9/Physics OR F3/Combat recorder owns this optional observer.
// Check it before hand/native candidate reads. Corpse ownership and release
// remain in Tick/DragTick independently of diagnostic recording.
static void SavedProviderDiagnosticsD84(std::uint64_t now) noexcept;
static void PhysicsDiagnosticsW5(std::uint64_t now,int f9) noexcept {
    if(!L::captureActiveD84(now))return;
    static std::uint64_t last=0;if(last&&now>=last&&now-last<1000)return;last=now;
    char m[700]{};int at=std::snprintf(m,sizeof(m),"D42_F3 PHYS_DIAG f9=%d labRung=%d corpseDrag=%d selectedLivingTouch=%d selectedLivingHit=%d propWake=%d impactLease=%d livingActuation=PAUSED touchLatched=%d dragFault=%d goreFault=%d ui=%d",
        f9,L::rung.load(),L::corpseDrag.load()?1:0,L::livingTouch.load()?1:0,L::livingHitImpact.load()?1:0,L::propWake.load()?1:0,
        L::impactLeaseOk.load()?1:0,touch43::g_latched?1:0,g_dragFault.load()?1:0,gore32n::g_drive_fault33.load()?1:0,DragUiOk()?1:0);
    const L::DragTuning tune{};
    for(int side=0;side<2&&at>0&&at<int(sizeof(m))-200;++side){
        float hx=0,hy=0,hz=0;const bool current=DragInputCurrentCg1(side,now);
        const bool hand=current&&SavedHandIntentD72(side,hx,hy,hz);
        if(hand&&!g_drag[side].held&&!L::active(L::Sub::CorpseDrag))DragSearchW5(side,now,tune,hx,hy,hz); // selected gameplay already searches in DragTick
        const auto& d=g_dragDiag[side];
        at+=std::snprintf(m+at,sizeof(m)-std::size_t(at)," %s={current=%d empty=%d grip=%d held=%d entity=%u part=%d nearM=%.2f qualified=%u unqualified=%u proof=%s}",
            side?"L":"R",current?1:0,HandEmptyW5(side)?1:0,GripHeldW5(side)?1:0,g_drag[side].held?1:0,g_drag[side].entity,g_drag[side].part,d.nearM,d.qualified,d.unqualified,d.why);
    }
    diagnostic_trace::WriteFor(L::captureDomainD84(now),m);
    SavedProviderDiagnosticsD84(now);
}
// Original-first script update; all native writer calls remain inside that proven owner.
#include "../subsystems/physics/saved_corpse_drag_d72.inl"
static void Tick(std::uint64_t now) noexcept {
    const int f9=L::f9Rung.load(std::memory_order_relaxed);
    if(L::rung.load(std::memory_order_relaxed)!=L::kRungLab&&!L::active(L::Sub::CorpseDrag)){
        RetireUnservicedCg1(now); // empty OFF returns before candidate/hand/native reads
        PhysicsDiagnosticsW5(now,f9); // returns before observer reads unless F9 or F3 records
        return;
    }
    g_labServiceDirtyD80=true;
    SavedWatchdogD72(now);
    if(L::active(L::Sub::CorpseDrag))InstallSavedDragD72();
    PhysicsDiagnosticsW5(now,f9);
    if(L::active(L::Sub::LivingHitImpact)){PhysFrame6698 f{};if(g_phys_frame6698.read(f))RememberLiving(f);LivingTick(now);}
    DragTick(now);
}
} // namespace lab58n
bool ReadCorpseGrabCandidateSideCg1(int side,kcd2vr::corpse_grip_cg1::Candidate& out,std::uint64_t now,bool& inputCurrent) noexcept {
    out={};inputCurrent=false;
    if(side<0||side>1||!kcd2vr::lab58::active(kcd2vr::lab58::Sub::CorpseDrag))return false;
    inputCurrent=lab58n::DragInputCurrentCg1(side,now);
    if(!inputCurrent||
       !lab58n::DragUiOk()||!lab58n::HandEmptyW5(side)||animation_filter::nativeBodyOwned112.load()||
       lab58n::g_dragFault.load()||gore32n::g_drive_fault33.load())return false;
    // Corpse Grip owns its input independently of ordinary Use/F5.
    // Rejoin the existing current committed hand owner before granting a
    // cached candidate: calibration/reference/hand claims may have changed.
    float x=0,y=0,z=0;if(!lab58n::SavedHandIntentD72(side,x,y,z))return false;
    kcd2vr::corpse_grip_cg1::Candidate c{};
    if(!kcd2vr::corpse_grip_cg1::handCandidates[side].read(c)||!kcd2vr::corpse_grip_cg1::valid(c.key)||
       !kcd2vr::corpse_grip_cg1::fresh(c.tick,now)||c.key.epoch!=g_pose_epoch6689.load()||
       (c.pendingOnlyD78&&(c.requestEpochD78!=g_weapon_request_epoch6694.load()||
        c.trackingEpochD78!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)))||
       !kcd2vr::collab_h21::current(c.key.physicsEpoch)||
       c.key.actor!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load()))return false;
    out=c;return true;
}
bool ReadCorpseGrabCandidateCg1(kcd2vr::corpse_grip_cg1::Candidate& out,std::uint64_t now,bool& inputCurrent) noexcept {
    return ReadCorpseGrabCandidateSideCg1(1,out,now,inputCurrent);
}
void SetLab58LivingTouch(bool on) noexcept {
    kcd2vr::lab58::livingTouch.store(on,std::memory_order_relaxed);
    if(kcd2vr::lab58::captureActiveD84(std::uint64_t(GetTickCount64()))){char m[200]{};std::snprintf(m,sizeof(m),"D51 LAB LIVING_TOUCH requested=%d active=0 reason=LIVING_ANIMATION_PHYSICS_OWNER_UNAVAILABLE nativeReactions=UNCHANGED",on?1:0);diagnostic_trace::WriteFor(kcd2vr::lab58::captureDomainD84(std::uint64_t(GetTickCount64())),m);}
}
void SetLab58(bool livingHitImpact,bool corpseDrag,bool propWake,float livingK) noexcept {
    namespace L=kcd2vr::lab58;
    const bool a=L::livingHitImpact.exchange(livingHitImpact),b=L::corpseDrag.exchange(corpseDrag),c=L::propWake.exchange(propWake);
    L::livingHitK.store(std::isfinite(livingK)?std::clamp(livingK,.5f,40.f):6.f);
    // No observer formatting outside an explicitly enabled finite recorder.
    if(L::captureActiveD84(std::uint64_t(GetTickCount64()))&&(a!=livingHitImpact||b!=corpseDrag||c!=propWake)){char m[280]{};
        std::snprintf(m,sizeof(m),"D51 LAB selectedLivingHit=%d corpseDrag=%d propWake=%d k=%.1f corpseGate=SELECTED_GAMEPLAY livingActuation=PAUSED nativeReactions=UNCHANGED",livingHitImpact?1:0,corpseDrag?1:0,propWake?1:0,double(L::livingHitK.load()));diagnostic_trace::WriteFor(L::captureDomainD84(std::uint64_t(GetTickCount64())),m);}
}
