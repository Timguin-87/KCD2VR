// Metadata-refresh-only helper retry. A model can exist while its helpers are
// still loading; caching that initial failure must not disable a valid sword.
static bool PhysReadGuardFaceH24(PhysEquipmentItem6698& x) noexcept {
    if(!x.entity||x.profile.classId!=4)return false;
    ph::Pose helper{};pm::Vec3 localNormal{1,0,0};
    if(!PhysHelper34(x.entity,"sharpening_center",helper,false)){
        // Exact retail swords_long proof: all30 available slt_0 helpers map
        // local -Y to the visual blade's thin X. No wrist/bounds axis guess.
        if(!PhysHelper34(x.entity,"slt_0",helper,false))return false;
        localNormal={0,-1,0};
    }
    const auto face=pm::unit(pm::rotate(helper.q,localNormal));
    if(!face)return false;
    x.profile.guardFaceNormalH24=*face;x.profile.guardFaceReadyH24=true;return true;
}
static void PhysRetryGuardFaceH24(PhysEquipmentItem6698& x,std::uint64_t request) noexcept {
    if(!x.held){x.guardFaceWasHeldH24=false;x.guardFaceRetryCountH24=0;return;}
    if(!x.guardFaceWasHeldH24||x.guardFaceRetryRequestH24!=request){
        x.guardFaceWasHeldH24=true;x.guardFaceRetryRequestH24=request;x.guardFaceRetryCountH24=0;
    }
    if(x.profile.guardFaceReadyH24||!x.entity||x.profile.classId!=4||x.guardFaceRetryCountH24>=4)return;
    ++x.guardFaceRetryCountH24;(void)PhysReadGuardFaceH24(x);
}
