// Included after committed_pose_service.inl. A paper placement needs one
// committed packet, not a new mailbox read for each hand and its grip value.
// Native-boundary ownership rechecks stay in NativeFrameTick.
bool ReadHeldMapSnapshotD78R1(HeldMapSnapshotD78R1& out,bool includeLeft,unsigned* refusal) noexcept {
    out={};
    if(!HeldMapHandAdmittedD78R1(0,refusal))return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)){if(refusal)*refusal=6;return false;}
    HeldMapSnapshotD78R1 next{};
    if(!HeldMapPoseFromPacketD78R1(0,p,now,next.right,refusal,&next.sourceEntity,
        &next.sourceEpoch,&next.sourceTick,&next.pair))return false;
    if(includeLeft&&p.mapFoldTrackedM39&&p.publicationR24&&HeldMapHandAdmittedD78R1(1,nullptr)){
        float left[8]{};
        if(HeldMapPoseFromPacketD78R1(1,p,now,left,nullptr,nullptr,nullptr,nullptr,nullptr)){
            // Display grace may keep paper visible, but cannot grant support.
            // This is the same strict live head/both-hand check as D64.
            TrackingSnapshot live{};
            const float grip=p.mapGripM39[1];
            if(ReadTrackingSnapshot(live)&&live.allowed&&live.head_valid&&live.left_valid&&live.right_valid&&
                !live.head_held&&!live.left_held&&!live.right_held&&live.tick&&now>=live.tick&&now-live.tick<=80&&
                std::isfinite(grip)&&grip>=0&&grip<=1){
                std::memcpy(next.left,left,sizeof(left));next.leftGrip=grip;next.haveLeft=true;
            }
        }
    }
    out=next;
    return true;
}
