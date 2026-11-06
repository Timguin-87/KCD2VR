// Called only by native-frame owner resolution. This copied readiness does not
// grant native attack eligibility; it scopes Grip-only recognition to combat.
static bool FistCombatReadyH27(void* player) noexcept {
    const auto now=std::uint64_t(GetTickCount64());NativeOwner104 owner{};
    if(!retail_build112::Ready(WhBase())||!g_gameplay_allowed.load()||!g_head_valid.load()||!g_left_valid.load()||!g_right_valid.load()||
       diagnostics::lab::NativeReference()||animation_filter::nativeBodyOwned112.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_scripted.load()||g_ctx_reading.load()||
       kcd2vr::combat_rules178r10::useHeld.load()||kcd2vr::workstation178r20c1::ownsActivityWrist45(now)||
       kcd2vr::hands59c7f::grabs(now)!=0||!NativeResolve104(player,owner)||owner.epoch!=g_pose_epoch6689.load())return false;
    void* opponent=nullptr;void* state=nullptr;void* back=nullptr;
    return Read(static_cast<unsigned char*>(owner.state)+0xCF8,opponent)&&opponent&&opponent!=owner.actor&&
        (PhysVft6698(opponent,0x3E3FAC8)||PhysVft6698(opponent,0x3A80610))&&
        Read(static_cast<unsigned char*>(opponent)+0x210,state)&&state&&
        Read(static_cast<unsigned char*>(state)+0xCE0,back)&&back==opponent&&
        Read(static_cast<unsigned char*>(state)+0xCF8,back)&&back==owner.actor;
}
