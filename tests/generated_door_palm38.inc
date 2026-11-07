// Generated exact committed-palm accessor; do not edit.
bool ReadInteractionPalm59c9(int side,float& x,float& y,float& z,float& dx,float& dy,float& dz,std::uint64_t& sourceTick,
    std::uint64_t* sourceEpoch,std::uintptr_t* sourcePlayer) noexcept {
    if(sourceEpoch)*sourceEpoch=0;if(sourcePlayer)*sourcePlayer=0;
    if(side<0||side>1||g_census_mode.load()<2||diagnostics::lab::NativeReference()||!CalibrationReady())return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.valid[side]||!p.palmValid59c9[side]||!TrackingHandUsable111(side,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return false;
    if(HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))return false;
    const Q4 worldQ{p.world[0],p.world[1],p.world[2],p.world[3]};
    const V3 origin=Add(QRotateV(worldQ,Mul(p.palm59c9[side],p.world[7])),{p.world[4],p.world[5],p.world[6]});
    const V3 direction=QRotateV(worldQ,p.palmDirection59c9[side]);
    if(!pc::finite(PV(origin))||!pc::finite(PV(direction)))return false;
    x=origin.x;y=origin.y;z=origin.z;dx=direction.x;dy=direction.y;dz=direction.z;sourceTick=p.stamp.tick;
    // Geometry/default callers retain their existing behavior. Optional door
    // identity comes from this packet, never from a newly sampled current owner.
    if(p.doorPlayer38&&p.stamp.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
       p.doorPlayer38==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))){
        if(sourceEpoch)*sourceEpoch=p.stamp.epoch;if(sourcePlayer)*sourcePlayer=p.doorPlayer38;}
    return true;
}
// DS-F88 (Claude's helper note): the narrow grab reader - the committed world
// wrist + quaternion + epochs, WITHOUT the g_census_mode>=2 diagnostics gate.
// Public namespace scope: the anonymous-namespace helpers are visible here via
// the using-directive.
bool ReadGrabHandWorld60(int side,float palm[3],float quat[4],std::uint64_t& poseEpoch,std::uint64_t& trackingEpoch,std::uintptr_t& player,std::uint64_t& tick) noexcept {
    palm[0]=palm[1]=palm[2]=0;quat[0]=quat[1]=quat[2]=0;quat[3]=1;
    poseEpoch=trackingEpoch=0;player=0;tick=0;
    if(side<0||side>1)return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.valid[side]||!TrackingHandUsable111(side,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return false;
    const auto wrist=HandWorld6694(p.allowed[side],p.world);
    const auto pos=FromT(wrist);
    if(!std::isfinite(pos.x)||!std::isfinite(pos.y)||!std::isfinite(pos.z))return false;
    palm[0]=pos.x;palm[1]=pos.y;palm[2]=pos.z;
    const auto q=QFromQuatT(wrist);
    quat[0]=q.x;quat[1]=q.y;quat[2]=q.z;quat[3]=q.w;
    poseEpoch=p.stamp.epoch;trackingEpoch=p.doorTrackingEpoch66;
    player=p.doorPlayer38;tick=p.stamp.tick;
    return true;
}
