// (door_pre_press66.h is included at FILE SCOPE in interaction_alignment.cpp -
//  it declares namespace kcd2vr and must never be included inside a namespace)
// The retail direct ray/AABB candidate path bypasses OnScreenDetour entirely.
// Observe its completed native descriptor immediately before +6404E5 appends
// it. This is still a candidate; the later native Use/action return is required.
// No register, descriptor, native ranking, eligibility or result is changed.
static HandCandidate g_direct_door54r1{}; // g_hand_candidate_mutex178r14
static std::atomic<bool> g_direct_door_retired54r1{false};
static void DirectDoorReceipt63(std::uint64_t now,std::uint32_t target,int side,const char* reason,
        float distance,std::uint32_t locked,bool descriptorKnown) noexcept {
    namespace dr63=kcd2vr::diagnostics::door_receipt63;
    // Keep native-owner/descriptor refusals separate from actual door offers,
    // and both separate from the general projection-enumeration receipt gate.
    static thread_local dr63::Gate early63,door63;
    if(!dr63::allow(descriptorKnown?door63:early63,now))return;
    char line63[384]{};std::snprintf(line63,sizeof(line63),
        "C63 F5_DOOR_DESCRIPTOR target=%u hand=%d reason=%s distance=%.3f lockedTarget=%u source=NATIVE_COMPLETED_DESCRIPTOR diagnosticOnly=1",
        target,side,reason,double(distance),locked);
    diagnostic_trace::Write(line63);
}
static void StopDirectDoor54r1() noexcept {
    if(!g_direct_door_hook54r1)return; // cold teardown cannot revoke first install
    g_direct_door_retired54r1.store(true,std::memory_order_release);
    // Disabling the patch is not callback rundown. Keep this handle/trampoline
    // until process exit, even after a failed disable, and never replace it.
    if(g_direct_door_hook54r1->is_enabled())(void)g_direct_door_hook54r1->disable();
}
// Caller holds g_hand_candidate_mutex178r14. Direct and projection observations
// keep separate provenance: a newer rejected direct observation must not leave
// its old witness in the projection cache, nor remove an independent witness.
static bool CopyDirectDoor54r1Locked(int side,std::uint64_t now,HandCandidate& out) noexcept {
    out={};
    if(side<0||side>1||!now)return false;
    const auto& c=g_direct_door54r1;
    if(g_direct_door_retired54r1.load(std::memory_order_acquire)||
       !g_enabled.load(std::memory_order_acquire)||!g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire)||
       c.sourceGeneration!=g_door_acquisition_generation65||!c.sourceGeneration||c.owner!=side||!c.entity||
       !c.tick||now<c.tick||now-c.tick>80||!c.sourceTick||now<c.sourceTick||now-c.sourceTick>80)return false;
    door_hold65::Frame hands65{};door_surface65::Witness surface65{};
    if(!door_hold65::frame.read(hands65)||!door_surface65::read(c.entity,hands65.hand[side],now,surface65)||
       hands65.hand[side].sourceGeneration!=c.sourceGeneration)return false;
    out=c;return true;
}
static bool ReadDirectDoor54r1(int side,std::uint64_t now,HandCandidate& out) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    return CopyDirectDoor54r1Locked(side,now,out);
}
static bool ReadExactDoor54r1(std::uint32_t target,int side,std::uint64_t now,HandCandidate& out) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    out={};
    if(!g_enabled.load(std::memory_order_acquire)||!g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire))return false;
    HandCandidate direct{};
    if(CopyDirectDoor54r1Locked(side,now,direct)&&direct.entity==target){out=direct;return true;}
    return HandRayActive()&&g_door_candidates38.find(target,side,now,out)&&out.sourceGeneration==g_door_generation38;
}
static bool PublishDirectDoor54r1(const HandCandidate& candidate,std::uint64_t generation,std::uint32_t observedTarget=0) noexcept {
    std::lock_guard<std::mutex> lk(g_hand_candidate_mutex178r14);
    if(g_direct_door_retired54r1.load(std::memory_order_acquire)||
       !generation||generation!=g_door_acquisition_generation65||!g_enabled.load(std::memory_order_acquire)||
       !g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire))return false;
    // C59: a native candidate pass visits MANY different objects. An unrelated
    // non-door must not erase a still-fresh admitted door before the Mod thread
    // can publish its held-Grip intent. Only a rejected observation of THAT
    // exact target withdraws it. Source revocation/freshness still wins above
    // and at the reader; native Use must independently accept the same target.
    if(candidate.entity)g_direct_door54r1=candidate;
    else if(observedTarget&&g_direct_door54r1.entity==observedTarget)g_direct_door54r1={};
    return true;
}
static void NativeDirectDoor54r1(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    if(g_direct_door_retired54r1.load(std::memory_order_acquire))return;
    const auto generation=DoorAcquisitionGeneration65();if(!generation)return;
    const auto now=std::uint64_t(GetTickCount64());
    const auto interactor=gpr(ctx,Gpr::R15),policy=gpr(ctx,Gpr::R12),payload=gpr(ctx,Gpr::Rdx);
    std::uint32_t target=0;std::uintptr_t nativePlayer=0,entity=0,descriptorEntityH12=0;float point[4]{};
    char cls[32]{};bool object=false;
    // RDX is the native sort record, not a CEntity. +0 is its priority key;
    // +10 is the constructed interaction descriptor's ID; +28 is the native
    // transformed bounds centre (not a claimed handle or a physics contact).
    __try {
        if(!policy||!payload||!interactor||
           *reinterpret_cast<const std::uintptr_t*>(policy)!=g_wh_base+kObjectPolicy||
           *reinterpret_cast<const std::uintptr_t*>(interactor)!=g_wh_base+0x3A6A0C8ull){DirectDoorReceipt63(now,0,-1,"POLICY_OR_INTERACTOR_REFUSED",0,0,false);return;}
        nativePlayer=*reinterpret_cast<const std::uintptr_t*>(interactor+8);
        if(!nativePlayer||*reinterpret_cast<const std::uintptr_t*>(policy+8)!=nativePlayer||
           *reinterpret_cast<const std::uintptr_t*>(nativePlayer)!=g_wh_base+0x3AA4B98ull){DirectDoorReceipt63(now,0,-1,"NATIVE_PLAYER_OR_POLICY_OWNER_REFUSED",0,0,false);return;}
        entity=*reinterpret_cast<const std::uintptr_t*>(nativePlayer+0x38);
        if(!entity||*reinterpret_cast<const std::uintptr_t*>(entity)!=g_wh_base+0x4072C78ull){DirectDoorReceipt63(now,0,-1,"NATIVE_ENTITY_REFUSED",0,0,false);return;}
        target=*reinterpret_cast<const std::uint32_t*>(payload+0x10);
        point[0]=*reinterpret_cast<const float*>(payload+0x28);
        point[1]=*reinterpret_cast<const float*>(payload+0x2c);
        point[2]=*reinterpret_cast<const float*>(payload+0x30);
        std::uintptr_t verifiedPlayer=0;
        object=CandidateKind57c1(interactor,target,verifiedPlayer,cls,&descriptorEntityH12)==interaction57c1::Kind::Object&&
            verifiedPlayer==nativePlayer&&!std::strcmp(cls,"AnimDoor");
    } __except(EXCEPTION_EXECUTE_HANDLER){DirectDoorReceipt63(now,target,-1,"NATIVE_DESCRIPTOR_READ_FAULT",0,0,false);return;}
    HandCandidate result{};
    palm59c9::Pair palms{};
    const char* receiptReason63="NO_FRESH_PALM_PACKET";
    if(object&&std::isfinite(point[0])&&std::isfinite(point[1])&&std::isfinite(point[2])&&g_palm_packets59c9.read(palms)){
        door_hold65::Frame frame65{};float closest65=1e9f;
        receiptReason63="NO_CURRENT_20CM_DOOR_SURFACE";
        if(door_hold65::frame.read(frame65))for(const auto& hand65:frame65.hand){
            if(!door_hold65::current(hand65,now)||hand65.player!=entity||hand65.sourceGeneration!=generation)continue;
            // H23: a completed native Door descriptor is semantic nomination,
            // not a generic hover winner. A previously transported Pickable may
            // still occupy palms.lockedEntity when KCD2 accepts THIS door. Using
            // that unrelated hover lock here prevented even requesting a surface
            // and made accepted-door recovery depend on evidence it could not get.
            // Observe the exact native nominee independently; the current same-
            // door 20cm physical query below and native accepted Use still decide
            // ownership. This does not mutate ranking, send Use, adopt a target,
            // or replace any held Pickable transaction.
            // DS-F16 stage 1: a committed near-door hand WITHOUT press warms the
            // pre-press descriptor cache (origin-joined, same target). The cache
            // grants nothing by itself; the press-scoped surface ray is issued
            // from the press edge (WarmDoorSurfaceOnPress66), the admission is
            // unchanged and still needs the witness + the native Use.
            if(!hand65.press){
                if(const auto* originH12=kcd2vr::door_origin_h12::find(target)){
                    if(!kcd2vr::door_origin_h12::joins(*originH12,target,descriptorEntityH12,policy,nativePlayer,
                        gpr(ctx,Gpr::Rbp),hand65,now)){receiptReason63="PRE_PRESS_ORIGIN_DESCRIPTOR_SOURCE_MISMATCH";continue;}
                }
                // DS-F47/F50 (Codex F43 review): NEAREST eligible door wins,
                // measured from the SAME current palm origin - the old entry's
                // cached distance came from an old pose and is never compared.
                // An unusable palm origin SKIPS the nomination entirely (the
                // prior entry survives only under its own freshness).
                const float px=palms.ray[hand65.side].ox,py=palms.ray[hand65.side].oy,pz=palms.ray[hand65.side].oz;
                const bool originOk=std::isfinite(px)&&std::isfinite(py)&&std::isfinite(pz);
                const float dx=point[0]-px,dy=point[1]-py,dz=point[2]-pz;
                const float d2=dx*dx+dy*dy+dz*dz;
                if(!originOk||!std::isfinite(d2)){receiptReason63="PALM_ORIGIN_UNUSABLE_NO_NOMINATION";continue;}
                kcd2vr::door_pre_press66::Entry old{};
                if(kcd2vr::door_pre_press66::read(hand65.side,now,generation,old)&&old.target!=target){
                    // DS-F53: a stale-owner/nonfinite nominee is INVALIDATED, not
                    // merely compared; a valid one beats the current candidate
                    // only when it is strictly nearer NOW (same current palm).
                    if(!kcd2vr::door_pre_press66::qualified(old,hand65.player,hand65.poseEpoch)){
                        kcd2vr::door_pre_press66::clear(hand65.side);
                    }else if(!kcd2vr::door_pre_press66::nearerNow(old,px,py,pz,d2)){
                        receiptReason63="FARTHER_DOOR_THAN_CACHED_NOMINEE";
                        continue;
                    }
                }
                kcd2vr::door_pre_press66::Entry e{};e.target=target;e.entity=entity;e.side=hand65.side;
                e.tick=now;e.sourceGeneration=generation;e.poseEpoch=hand65.poseEpoch;e.descriptorEntityH12=descriptorEntityH12;
                e.distance=d2;
                e.point[0]=point[0];e.point[1]=point[1];e.point[2]=point[2];
                std::memcpy(e.cls,cls,std::min(sizeof(e.cls),sizeof(cls)));
                kcd2vr::door_pre_press66::store(hand65.side,e);
                receiptReason63="PRE_PRESS_DESCRIPTOR_CACHED";
                continue;
            }
            // A pre-press observation above needs no closed Grip. Actual
            // surface acquisition still requires this current held press.
            if(hand65.grip<.30f)continue;
            // A substituted native inside-box branch belongs to the exact
            // descriptor and original committed hand, never the later centre.
            if(const auto* originH12=kcd2vr::door_origin_h12::find(target)){
                if(!kcd2vr::door_origin_h12::joins(*originH12,target,descriptorEntityH12,policy,nativePlayer,
                    gpr(ctx,Gpr::Rbp),hand65,now)){receiptReason63="PALM_ORIGIN_DESCRIPTOR_SOURCE_MISMATCH";continue;}
            }
            // This native descriptor remains semantic eligibility. Its centre
            // is a hint only; the actual pose-owner physics ray must prove the
            // SAME door surface within20cm of the current committed palm.
            (void)door_surface65::offer(door_surface65::request(target,hand65,now,{point[0],point[1],point[2]}),now);
            door_surface65::Witness surface65{};
            if(!door_surface65::read(target,hand65,now,surface65)||surface65.distance>=closest65)continue;
            closest65=surface65.distance;
            result={now,surface65.point.x,surface65.point.y,surface65.point.z,surface65.distance,hand65.side,true,target};
            std::memcpy(result.cls,cls,sizeof(result.cls));result.sourceGeneration=generation;
            result.sourceEpoch=hand65.poseEpoch;result.sourcePlayer=hand65.player;result.sourceTick=hand65.poseTick;
            receiptReason63="CURRENT_NATIVE_DOOR_20CM_SURFACE_OFFER";
        }
    }
    // A rejection withdraws only this same native target; later unrelated
    // candidates in the same iteration cannot erase its admitted door offer.
    const bool published63=PublishDirectDoor54r1(result,generation,target);
    if(object)DirectDoorReceipt63(now,target,result.owner,published63?receiptReason63:"SOURCE_REVOKED_BEFORE_PUBLICATION",result.distance,palms.lockedEntity,true);
    if(published63&&object)
        SelectionReceipt59c10(now,target,result.owner,result.entity?11:12,result.distance,0,palms.lockedEntity);
}
