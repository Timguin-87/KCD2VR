// The single-hand readers and paired paper snapshot share exactly the same
// admission and socket conversion. Callers choose when to sample the mailbox.
static bool HeldMapHandAdmittedD78R1(unsigned side,unsigned* refusal) noexcept {
    const auto reject=[&](unsigned reason) noexcept {if(refusal)*refusal=reason;return false;};
    // The FinalSkin and CombatCensus setters are mutually exclusive. The old
    // conjunction required an impossible tuple and refused every normal mode.
    // Only the calibrated pose-producing census owns a committed hand here.
    if(g_census_mode.load()<2)return reject(1);
    if(diagnostics::lab::NativeReference()||!CalibrationReady())return reject(2);
    if(animation_filter::nativeBodyOwned112.load())return reject(3);
    if(ExactHandItemState(side)!=0)return reject(4);
    if(NativeHeadAimEquipment120R3()==1)return reject(5);
    return true;
}
static bool HeldMapPoseFromPacketD78R1(unsigned side,const CommittedFrame6695& p,std::uint64_t now,
    float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,
    std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
    const auto reject=[&](unsigned reason) noexcept {if(refusal)*refusal=reason;return false;};
    if(!p.valid[side]||!TrackingHandUsable111(side,now))return reject(6);
    if(!p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80)return reject(7);
    // Post-Update native activity may precede the render-side body mirror.
    // Only this already-validated completed character/body may hide the paper.
    if(kcd2vr::native_passenger_a34::current(p.stamp.actor,p.stamp.epoch,now))return reject(3);
    if(HandClaimBlockedT24(side,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load()))return reject(10); // distinct from existing M34 presentation mask
    if(p.mapPresentationOwnerM34[side])return reject(9); // same completed selected hand owner, never a both-hand empty gate
    // Native held-item socket of THIS committed selected wrist, not a guessed
    // wrist-relative offset. Its parent/joint identity was validated at publish.
    if(!p.fistSlotValidH31[side])return reject(11);
    const auto pose=HandWorld6694(p.fistSlotH31[side],p.world);
    for(unsigned i=0;i<4;++i)out[i]=pose.q[i];for(unsigned i=0;i<3;++i)out[i+4]=pose.t[i];out[7]=1;
    for(float f:out)if(!std::isfinite(f))return reject(8);
    if(pair){pair->actor=p.stamp.actor;pair->epoch=p.stamp.epoch;pair->sample=p.stamp.sample;pair->tick=p.stamp.tick;pair->publicationR24=p.publicationR24;
        std::memcpy(pair->world,p.world,sizeof(pair->world));pair->valid=true;}
    // Optional bootstrap identity comes from THIS committed frame, with the
    // same publication/epoch join as the established door consumer. A pose-only
    // caller keeps its previous behavior; a missing identity grants no spawn.
    if(p.doorPlayer38&&p.stamp.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
       p.doorPlayer38==reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire))){
        if(sourceEntity)*sourceEntity=p.doorPlayer38;if(sourceEpoch)*sourceEpoch=p.stamp.epoch;if(sourceTick)*sourceTick=p.stamp.tick;}
    if(refusal)*refusal=0;return true;
}
static bool ReadHeldMapCommittedPoseM41(unsigned side,float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
    if(pair)*pair={};
    if(sourceEntity)*sourceEntity=0;if(sourceEpoch)*sourceEpoch=0;if(sourceTick)*sourceTick=0;
    if(!HeldMapHandAdmittedD78R1(side,refusal))return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)){if(refusal)*refusal=6;return false;}
    return HeldMapPoseFromPacketD78R1(side,p,now,out,refusal,sourceEntity,sourceEpoch,sourceTick,pair);
}

// M41 static parchment uses RIGHT; existing LEFT reader retains its old contract.
bool ReadHeldMapLeftPoseM1(float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
 return ReadHeldMapCommittedPoseM41(1,out,refusal,sourceEntity,sourceEpoch,sourceTick,pair);
}
bool ReadHeldMapRightPoseM41(float (&out)[8],unsigned* refusal,std::uintptr_t* sourceEntity,std::uint64_t* sourceEpoch,std::uint64_t* sourceTick,HeldMapPairM39* pair) noexcept {
 return ReadHeldMapCommittedPoseM41(0,out,refusal,sourceEntity,sourceEpoch,sourceTick,pair);
}

// Optional second hand drives ONLY own parchment fold fraction. It never
// places Henry's right hand, occupies equipment or bypasses another hand owner.
bool ReadHeldMapFreeRightPoseM39(float (&out)[8],float& grip,const HeldMapPairM39* pair,bool* pairMismatch) noexcept {
    grip=0;if(pairMismatch)*pairMismatch=false;
    if(!pair||!pair->valid)return false; // M39P: never an unpaired fold end
    if(g_census_mode.load()<2||diagnostics::lab::NativeReference()||!CalibrationReady()||
       animation_filter::nativeBodyOwned112.load()||ExactHandItemState(0)!=0||
       NativeHeadAimEquipment120R3()==1)return false;
    CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
    if(!ReadCommittedPacket6695(p)||!p.mapFoldTrackedM39||!p.valid[0]||!TrackingHandUsable111(0,now)||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||p.stamp.epoch!=g_pose_epoch6689.load()||
       p.mapPresentationOwnerM34[0]||HandClaimBlockedT24(0,p.stamp.actor,p.stamp.epoch,g_weapon_request_epoch6694.load())||
       kcd2vr::native_passenger_a34::current(p.stamp.actor,p.stamp.epoch,now))return false;
    // M39P: same completed publication as the LEFT end (full stamp and world frame),
    // otherwise common movement between two packets reads as a false fold.
    if(p.stamp.actor!=pair->actor||p.stamp.epoch!=pair->epoch||p.stamp.sample!=pair->sample||p.stamp.tick!=pair->tick||
       std::memcmp(p.world,pair->world,sizeof(pair->world))!=0){if(pairMismatch)*pairMismatch=true;return false;}
    // Display grace cannot earn fold authority. The LEFT hand supplies the
    // other end of the distance, so both current hands and head must be tracked.
    TrackingSnapshot live{};
    if(!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||
       !live.right_valid||!live.left_valid||live.head_held||live.right_held||
       live.left_held||!live.tick||now<live.tick||now-live.tick>80)return false;
    const auto pose=HandWorld6694(p.allowed[0],p.world);
    for(unsigned i=0;i<4;++i)out[i]=pose.q[i];for(unsigned i=0;i<3;++i)out[i+4]=pose.t[i];out[7]=1;
    for(float f:out)if(!std::isfinite(f))return false;
    grip=p.mapGripM39[0];return std::isfinite(grip)&&grip>=0&&grip<=1;
}

// D64 second-hand paper support uses the exact same committed frame and yields
// to all existing hand owners. This is presentation only, never item ownership.
bool ReadHeldMapSupportLeftD64(float (&out)[8],float& grip,const HeldMapPairM39& primary) noexcept {
 grip=0;HeldMapPairM39 pair{};std::uintptr_t who=0;std::uint64_t body=0,when=0;
 if(!primary.valid||!primary.publicationR24||!ReadHeldMapCommittedPoseM41(1,out,nullptr,&who,&body,&when,&pair)||
    pair.actor!=primary.actor||pair.epoch!=primary.epoch||pair.sample!=primary.sample||pair.tick!=primary.tick||pair.publicationR24!=primary.publicationR24||
    std::memcmp(pair.world,primary.world,sizeof(pair.world))!=0)return false;
 CommittedFrame6695 p{};const auto now=std::uint64_t(GetTickCount64());
 if(!ReadCommittedPacket6695(p)||!p.mapFoldTrackedM39||p.stamp.actor!=pair.actor||p.stamp.epoch!=pair.epoch||
    p.stamp.sample!=pair.sample||p.stamp.tick!=pair.tick||p.publicationR24!=pair.publicationR24||std::memcmp(p.world,pair.world,sizeof(p.world))!=0)return false;
 TrackingSnapshot live{};
 if(!ReadTrackingSnapshot(live)||!live.allowed||!live.head_valid||!live.left_valid||!live.right_valid||
    live.head_held||live.left_held||live.right_held||!live.tick||now<live.tick||now-live.tick>80)return false;
 grip=p.mapGripM39[1];return std::isfinite(grip)&&grip>=0&&grip<=1;
}
