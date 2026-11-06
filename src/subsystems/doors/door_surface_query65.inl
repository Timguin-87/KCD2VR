// Called only after the same Henry pose was committed, restored and published.
// A native eligible Door nominates an identity; only this synchronous physical
// ray proves a surface. The completed descriptor WORLD centre is only a query
// direction hint, never a handle, surface, normal or action admission.
struct DoorSurfaceBudget65 {
    NpcQuery172 savedQuery{};bool savedActive=false,savedPass=false;
    explicit DoorSurfaceBudget65(std::uint64_t sample) noexcept {
        savedQuery=g_npc_query172;savedActive=g_contact_budget_active6695;savedPass=g_contact_pass6695;
        // The ordinary weapon/finger pass may already have consumed this
        // sample's budget. Never reset its remaining rays or actual query cost.
        BeginContactBudget65(sample,GetTickCount64(),false);
        ReturnDoorSurfaceCredits65(sample);
        g_contact_budget_active6695=true;g_contact_pass6695=true;
        // Reuse the exact retail body-owner decode and Henry/equipment skips.
        // Door geometry does not require the optional NPC/hand collision mode.
        g_npc_query172={};g_npc_query172.requested=true;
    }
    ~DoorSurfaceBudget65(){g_npc_query172=savedQuery;g_contact_budget_active6695=savedActive;g_contact_pass6695=savedPass;}
};
static void PublishDoorSurface65(const CommittedFrame6695& packet,std::uint64_t sourceEpoch) noexcept {
    namespace surface65=kcd2vr::door_surface65;
    namespace doorPose65=kcd2vr::pose_core;
    const auto now=std::uint64_t(GetTickCount64());
    if(!sourceEpoch||sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       !packet.doorPlayer38||!packet.stamp.sample||!surface65::fresh(packet.stamp.tick,now)||
       packet.stamp.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       packet.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))||
       !g_gameplay_allowed.load(std::memory_order_acquire)||!CalibrationReady()||diagnostics::lab::NativeReference()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||!door_use46::enabled.load(std::memory_order_acquire))return;
    std::array<surface65::Request,8> requestsBySide65[2]{};unsigned requestCount65[2]{};bool anyRequest65=false;
    for(int side=0;side<2;++side){
        const auto count=surface65::requests(now,packet.doorPlayer38,packet.stamp.epoch,side,requestsBySide65[side]);
        for(unsigned n=0;n<count;++n){const auto& r=requestsBySide65[side][n];
            if(r.trackingEpoch==sourceEpoch&&r.sourceGeneration==interaction_alignment::DoorAcquisitionGeneration65()&&
               r.settingsEpoch==door_use46::settingsEpoch.load(std::memory_order_acquire)&&
               r.experimentEpoch==workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire))
                requestsBySide65[side][requestCount65[side]++]=r;
        }
        anyRequest65=anyRequest65||requestCount65[side]!=0;
    }
    if(!anyRequest65)return; // quiet ordinary frames perform no new native/model reads
    CommittedFrame6695 current{};
    if(!ReadCommittedPacket6695(current)||current.stamp.actor!=packet.stamp.actor||
       current.stamp.epoch!=packet.stamp.epoch||current.stamp.sample!=packet.stamp.sample||
       current.stamp.tick!=packet.stamp.tick||current.doorPlayer38!=packet.doorPlayer38)return;
    for(unsigned n=0;n<8;++n)if(current.world[n]!=packet.world[n])return;
    ModelWorld6692 world{};
    const auto sameWorld65=[&]() noexcept {
        ModelWorld6692 actual{};
        return ReadModelWorld6692(reinterpret_cast<void*>(packet.stamp.actor),actual)&&
            doorPose65::length(doorPose65::Vec3{actual.t.x,actual.t.y,actual.t.z}-doorPose65::Vec3{packet.world[4],packet.world[5],packet.world[6]})<=.002&&
            doorPose65::angle(doorPose65::Quat{actual.q.x,actual.q.y,actual.q.z,actual.q.w},doorPose65::Quat{packet.world[0],packet.world[1],packet.world[2],packet.world[3]})<=.002&&
            std::fabs(actual.scale-packet.world[7])<1e-5;
    };
    if(!sameWorld65())return;
    world.q={packet.world[0],packet.world[1],packet.world[2],packet.world[3]};
    world.t={packet.world[4],packet.world[5],packet.world[6]};world.scale=packet.world[7];
    static thread_local std::uint64_t queriedSample65[2]{};
    for(int side=0;side<2;++side){
        if(!packet.valid[side]||!packet.palmValid59c9[side]||queriedSample65[side]==packet.stamp.sample)continue;
        const auto& requests65=requestsBySide65[side];const auto count=requestCount65[side];
        if(!count)continue;
        const auto& family65=requests65[0];
        const auto eligible65=[&](std::uint64_t at) noexcept {
            const float grip=side?g_left_grip.load(std::memory_order_acquire):g_right_grip.load(std::memory_order_acquire);
            return surface65::fresh(packet.stamp.tick,at)&&sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire)&&
                packet.stamp.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
                packet.doorPlayer38==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))&&
                g_gameplay_allowed.load(std::memory_order_acquire)&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
                !g_ctx_cutscene.load()&&!g_ctx_reading.load()&&!animation_filter::nativeBodyOwned112.load()&&
                door_use46::enabled.load(std::memory_order_acquire)&&
                interaction_alignment::DoorAcquisitionGeneration65()==family65.sourceGeneration&&
                door_use46::settingsEpoch.load(std::memory_order_acquire)==family65.settingsEpoch&&
                workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire)==family65.experimentEpoch&&
                std::isfinite(grip)&&grip>=.30f&&
                TrackingHeadUsable111(at)&&TrackingHandUsable111(side,at)&&ExactHandItemState(side)==0;
        };
        if(!eligible65(now))continue;
        const V3 origin=ModelToWorld6692(world,packet.palm59c9[side]);
        V3 direction=QRotateV(world.q,packet.palmDirection59c9[side]);const float length=Len(direction);
        if(!doorPose65::finite(doorPose65::Vec3{origin.x,origin.y,origin.z})||!std::isfinite(length)||length<.8f||length>1.2f)continue;
        direction=Mul(direction,surface65::maxDistance/length);
        const auto aimed65=surface65::queryDirectionH24(requests65,count,{origin.x,origin.y,origin.z},
            {direction.x,direction.y,direction.z});
        direction={aimed65.x,aimed65.y,aimed65.z};
        queriedSample65[side]=packet.stamp.sample;
        // A fresh clear/foreign/busy result must never inherit the previous
        // successful surface for this hand. Withdraw before the actual query;
        // a busy mailbox skips this sample instead of granting new evidence.
        if(!surface65::withdraw(side,packet.stamp.sample,now))continue;
        DoorSurfaceBudget65 queryBudget65(packet.stamp.sample);
        // H23: Grip is a local physical grasp, not index-ray pointing. A
        // tangential palm direction used to miss a door the hand was touching.
        // H24 first aims toward the nearest native WORLD bounds centre (hint
        // only); six world axes remain from the SAME palm, each exactly20cm.
        // These are actual occluded world queries, never bounds-centre contact
        // or a sphere-overlap claim. The shared ray/time budget stays authoritative.
        const V3 probes65[surface65::probeRayCount]{direction,
            {surface65::maxDistance,0,0},{-surface65::maxDistance,0,0},
            {0,surface65::maxDistance,0},{0,-surface65::maxDistance,0},
            {0,0,surface65::maxDistance},{0,0,-surface65::maxDistance}};
        for(const auto& probe65:probes65){
            const auto hit=WorldRayDistance6692(origin,probe65);
            const auto done=std::uint64_t(GetTickCount64());
            // Busy includes exhausted shared credits/time: do not retry it and
            // never reset the budget. A changed source/world ends this sample.
            if(hit.state==wc::Result::Busy||!eligible65(done)||!sameWorld65())break;
            if(hit.state!=wc::Result::Hit||!hit.plane||!hit.native172.ownerRead||!hit.native172.entityId||
               !hit.native172.owner||!hit.native172.collider)continue;
            CommittedFrame6695 verify{};
            if(!ReadCommittedPacket6695(verify)||verify.stamp.actor!=packet.stamp.actor||
               verify.stamp.epoch!=packet.stamp.epoch||verify.stamp.sample!=packet.stamp.sample||
               verify.stamp.tick!=packet.stamp.tick||verify.doorPlayer38!=packet.doorPlayer38)break;
            bool samePacketWorld65=true;for(unsigned n=0;n<8;++n)samePacketWorld65=samePacketWorld65&&verify.world[n]==packet.world[n];
            if(!samePacketWorld65)break;
            bool published65=false;
            for(unsigned n=0;n<count;++n){const auto& r=requests65[n];
                if(r.target!=hit.native172.entityId||!surface65::current(r,done)||r.trackingEpoch!=sourceEpoch||
                   r.sourceGeneration!=interaction_alignment::DoorAcquisitionGeneration65()||
                   r.settingsEpoch!=door_use46::settingsEpoch.load(std::memory_order_acquire)||
                   r.experimentEpoch!=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire))continue;
                surface65::Witness w{};w.source=r;w.queryTick=done;w.sample=packet.stamp.sample;
                w.actualPalm={origin.x,origin.y,origin.z};w.point={hit.point.x,hit.point.y,hit.point.z};
                w.normal={hit.normal.x,hit.normal.y,hit.normal.z};w.entityId=hit.native172.entityId;
                w.opaqueOwner=hit.native172.owner;w.opaqueCollider=hit.native172.collider;w.distance=hit.distance;w.valid=true;
                published65=surface65::publish(w)||published65;
            }
            if(published65)break;
        }
    }
}
