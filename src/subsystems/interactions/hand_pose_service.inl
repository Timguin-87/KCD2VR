bool ReadMiscGestureBodyD59(MiscGestureBodyD59& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    if(diagnostics::lab::NativeReference()||!CalibrationReady()||!retail_build112::Ready(WhBase())||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||
       animation_filter::nativeBodyOwned112.load()||animation_filter::NativeKickActive175()||
       kcd2vr::activity_input163::witness175.Active())return false;
    CommittedFrame6695 p{};TrackingSnapshot live{};
    if(!ReadCommittedPacket6695(p)||!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||live.head_held||
       !live.tick||now<live.tick||now-live.tick>60||!p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||
       !p.doorPlayer38||!p.doorTrackingEpoch66||p.doorTrackingEpoch66!=live.source_epoch||
       live.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)||NativePassengerCurrentA34(now)||
       p.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    // Same exact CEntity identity/read used by the D58 map bootstrap. No call
    // through a new native slot and no entity lookup on the input thread.
    std::uintptr_t vt=0;std::uint32_t id=0;
    if(!Read(reinterpret_cast<void*>(p.doorPlayer38),vt)||vt!=WhBase()+0x4072C78||
       !Read(reinterpret_cast<void*>(p.doorPlayer38+0xC),id)||!id)return false;
    MiscGestureBodyD59 next{};next.player=p.doorPlayer38;next.character=p.stamp.actor;next.playerId=id;
    next.tick=p.stamp.tick;next.body=p.stamp.epoch;next.tracking=p.doorTrackingEpoch66;
    const Q4 q{p.world[0],p.world[1],p.world[2],p.world[3]};
    for(int side=0;side<2;++side){
        if(!p.valid[side]||!p.palmValid59c9[side]||(side?!live.left_valid:!live.right_valid)||
           (side?live.left_held:live.right_held)||!TrackingHandUsable111(side,now)||
           HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))continue;
        const V3 origin=Add(QRotateV(q,Mul(p.palm59c9[side],p.world[7])),{p.world[4],p.world[5],p.world[6]});
        if(!pc::finite(PV(origin)))continue;
        next.palm[side][0]=origin.x;next.palm[side][1]=origin.y;next.palm[side][2]=origin.z;next.valid[side]=true;
    }
    if(!next.valid[0]&&!next.valid[1])return false;
    if(p.publicationR24!=ReadCompletedPublicationT23()||p.stamp.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       p.doorPlayer38!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    out=next;return true;
}
bool ReadInteractionHandWorld123R1(int side,float& x,float& y,float& z) noexcept {
    if(side<0||side>1||g_census_mode.load()<2||diagnostics::lab::NativeReference()||!CalibrationReady())return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.valid[side]||!TrackingHandUsable111(side,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return false;
    if(HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))return false;
    const auto position=FromT(HandWorld6694(p.allowed[side],p.world));
    if(!std::isfinite(position.x)||!std::isfinite(position.y)||!std::isfinite(position.z))return false;
    x=position.x;y=position.y;z=position.z;return true;
}
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
