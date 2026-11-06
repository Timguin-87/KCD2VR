// Copied-only scheduling credits for the existing shared18ray budget. This
// reserves no time and grants no Door selection/Use/animation authority.
struct DoorSurfaceCredits65 {std::uint64_t sample=0;unsigned count=0;};
static thread_local DoorSurfaceCredits65 g_door_surface_credits65;
static void ReserveDoorSurfaceCredits65(std::uint64_t sample,std::uint64_t now) noexcept {
    namespace surface65=kcd2vr::door_surface65;
    g_door_surface_credits65={sample,0};
    if(!sample||!now)return;
    const auto player=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire));
    const auto poseEpoch=g_pose_epoch6689.load(std::memory_order_acquire);
    std::array<surface65::Request,8> requested[2]{};unsigned count[2]{};
    count[0]=surface65::requests(now,player,poseEpoch,0,requested[0]);
    count[1]=surface65::requests(now,player,poseEpoch,1,requested[1]);
    if(!count[0]&&!count[1])return; // no new native/model/tracking reads when quiet
    if(!g_gameplay_allowed.load(std::memory_order_acquire)||!CalibrationReady()||diagnostics::lab::NativeReference()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||!kcd2vr::door_use46::enabled.load(std::memory_order_acquire))return;
    kcd2vr::door_hold65::Frame frame{};
    if(!kcd2vr::door_hold65::frame.read(frame))return;
    const auto generation=interaction_alignment::DoorAcquisitionGeneration65();
    const auto tracking=tracking59c6::epoch.load(std::memory_order_acquire);
    const auto settings=kcd2vr::door_use46::settingsEpoch.load(std::memory_order_acquire);
    const auto experiment=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire);
    for(int side=0;side<2;++side){
        // available is the producer's current empty/nonreserved hand witness.
        // Actual native item/tracking/world/target is rechecked at final query.
        const auto& h=frame.hand[side];
        if(!kcd2vr::door_hold65::current(h,now)||h.side!=side||h.player!=player||h.poseEpoch!=poseEpoch||
           h.grip<.30f||!h.press||h.sourceGeneration!=generation||h.trackingEpoch!=tracking||
           h.settingsEpoch!=settings||h.experimentEpoch!=experiment)continue;
        for(unsigned n=0;n<count[side];++n)if(surface65::same(requested[side][n],h)){
            const unsigned reserve=std::min(surface65::probeRayCount,g_contact_budget6695.remaining);
            g_contact_budget6695.remaining-=reserve;g_door_surface_credits65.count+=reserve;
            break; // bounded orientation probes for each actually held hand
        }
    }
}
static void ReturnDoorSurfaceCredits65(std::uint64_t sample) noexcept {
    if(!sample||g_door_surface_credits65.sample!=sample||!g_door_surface_credits65.count)return;
    // Return exactly what this same pass withheld, ONCE. No reset of used,
    // spent, started or inFlight; exhausting1000us still refuses the Door ray.
    const auto credits=g_door_surface_credits65.count;g_door_surface_credits65.count=0;
    const auto ceiling=g_contact_budget6695.capacity;
    if(g_contact_budget_sample65!=sample||g_contact_budget6695.used>ceiling||
       g_contact_budget6695.remaining>ceiling-g_contact_budget6695.used||
       credits>ceiling-g_contact_budget6695.used-g_contact_budget6695.remaining)return;
    g_contact_budget6695.remaining+=credits;
}
// The existing ContactPass and the final-query-only fallback share this exact
// begin boundary. Repeated eye/skin callbacks never replenish a sample.
static void BeginContactBudget65(std::uint64_t sample,std::uint64_t now,bool reserveDoor=true) noexcept {
    if(!sample||g_contact_budget_sample65==sample)return;
    g_contact_budget6695.begin_work();g_contact_budget_sample65=sample;
    g_door_surface_credits65={sample,0};
    if(reserveDoor)ReserveDoorSurfaceCredits65(sample,now);
}
