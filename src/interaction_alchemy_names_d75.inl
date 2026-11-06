// Included at interaction_alignment namespace scope. The existing N72 Lua/HUD
// endpoint still obtains the exact game's localized name. Native action
// selection is deliberately independent of this read-only display focus.
bool AlchemyHandNameWantedD72() noexcept {
    return g_enabled.load(std::memory_order_acquire)&&g_selection_ready68.load(std::memory_order_acquire)&&
        craft68::enabled.load(std::memory_order_acquire)&&NomineeAlchemyInstance68()!=0&&
        minigame_hands45::modeFor(minigame_hands45::Alchemy)==minigame_hands45::SixDofVrik;
}
static bool FinishAlchemyNameFocusD75(const selection68::Table& table,
    selection68::Ref ref,std::uint64_t sourceEpoch,std::uint64_t modeEpoch,
    int side,std::uint64_t now,AlchemyHandNameFocusD72& out,std::uint64_t visualGeneration=0) noexcept {
    selection68::Table again{};std::uint32_t entity=0,player=0;
    if(!subsystems::alchemy::near67::valid(ref)||!sourceEpoch||!modeEpoch||
       !AlchemyHandNameWantedD72()||!selection68::registered(table,ref.entity)||
       !subsystems::alchemy::near67::same(table.refs[unsigned(ref.slot)],ref)||
       !SelectionRead68(table.instance+0x150+4*ref.slot,entity)||entity!=ref.entity||
       !SelectionVft68(table.playerEntity,0x4072C78)||
       !SelectionRead68(table.playerEntity+0xC,player)||!player||
       sourceEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       modeEpoch!=minigame_hands45::modeEpoch67.load(std::memory_order_acquire)||
       !craft68::selectionTable68(again,now)||!selection68::sameTable(table,again)||
       !NomineeActivityCurrent68(again)||(visualGeneration&&
       visualGeneration!=CameraHook::CurrentFamilyGenerationR24()))return false;
    if(visualGeneration){
        // Recenter may become pending during the native reads above before
        // its new camera generation is committed. Revalidate the display ray
        // after those reads; a newer rendered frame in the same lifetime is OK.
        visual_ray_d75::Sample current{};
        if(!CameraHook::GetLatestVisualRayD75(current,80)||current.sourceEpoch!=sourceEpoch||
           current.cameraGeneration!=visualGeneration)return false;
    }
    out={table.instance,table.lifetime,sourceEpoch,modeEpoch,player,entity,ref.slot,side};
    return true;
}
bool ReadAlchemyHandNameFocusD72(std::uint64_t now,AlchemyHandNameFocusD72& out) noexcept {
    out={};selection68::Table table{};
    if(!AlchemyHandNameWantedD72()||!craft68::selectionTable68(table,now)||
       !NomineeActivityCurrent68(table))return false;
    const auto mode=minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    workstation_sample46::Frame frame{};
    if(frik_pose_probe::WorkstationFrame46(frame)&&frame.allowed&&
       workstation_sample46::fresh(frame,now)&&
       frame.sourceEpoch==tracking59c6::epoch.load(std::memory_order_acquire)){
        visual_ray_d75::Sample ray{};
        if(CameraHook::GetLatestVisualRayD75(ray,80)&&ray.sourceEpoch==frame.sourceEpoch){
            std::array<craft68::NativeOffer,subsystems::alchemy::near67::Capacity> offers{};
            for(unsigned i=0;i<offers.size();++i)(void)craft68::offers[i].read(offers[i]);
            // R116: with VrAlchemyViewSelectR116 the name shown is the item the selection picks
            // (same eye ray, same exact-hit-then-6-deg rule as HeadTargetR116).
            const auto ref=headR116::enabled.load(std::memory_order_acquire)?
                HeadPickR116(table,offers,{ray.origin[0],ray.origin[1],ray.origin[2]},
                    {ray.forward[0],ray.forward[1],ray.forward[2]},now):
                alchemy_head_name_d75::select(table,offers,
                {ray.origin[0],ray.origin[1],ray.origin[2]},
                {ray.forward[0],ray.forward[1],ray.forward[2]},now);
            if(subsystems::alchemy::near67::valid(ref)){
                // A recenter, station exit or mode change during the read must
                // not publish a name from the old tracking/native lifetime.
                workstation_sample46::Frame again{};
                if(!frik_pose_probe::WorkstationFrame46(again)||!again.allowed||
                   !workstation_sample46::fresh(again,now)||again.poseEpoch!=frame.poseEpoch||
                   again.sourceEpoch!=frame.sourceEpoch)return false;
                return FinishAlchemyNameFocusD75(table,ref,frame.sourceEpoch,mode,-1,now,out,ray.cameraGeneration);
            }
        }
    }
    // Preserve D74's useful nearby-hand label when the head is not directly
    // viewing a registered station object. This remains display-only too.
    craft68::Nomination nomination{};
    const int side=craft68::selectionOwner68(table,nomination,now);
    return side>=0&&FinishAlchemyNameFocusD75(table,nomination.decision.ref,
        nomination.context.sourceEpoch,nomination.context.generation,side,now,out);
}
