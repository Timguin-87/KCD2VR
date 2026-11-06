// R112: stowed equipment follows the completed tracked torso, including when
// a native attack animates a socket or an intermediate equipment helper.
// Cache only validated companion anatomy. No native attachment state, inventory,
// animation playback, world transform or additional render transform is changed.
static void BuildStowedReferenceR112(CoherentState6689& s,const RigMap& rig) noexcept {
    s.stowedAnchorsR112.fill(-1);
    s.stowedReferenceR112={};
    for(std::size_t i=0;i<s.stowedJointsD63.size();++i){
        const int joint=s.stowedJointsD63[i];
        if(joint<0||joint>=int(s.count)||s.trunk[joint]||s.armR[joint]||s.armL[joint])continue;
        int parent=s.parent[joint];
        for(std::uint32_t guard=0;guard<s.count&&parent>=0&&parent<int(s.count);++guard){
            if(s.armR[parent]||s.armL[parent])break;
            if(s.trunk[parent]){
                // The first trunk ancestor must actually be a torso/hip bone.
                // Never borrow a head/neck pose or skip it to reach the chest.
                if(parent==rig.pelvis||parent==rig.spine||parent==rig.spine1||
                   parent==rig.spine2||parent==rig.spine3){
                    const auto relative=RelativeFromAbsolute(s.rest[parent],s.rest[joint]);
                    if(ValidPose6689(s.rest[parent])&&ValidPose6689(s.rest[joint])&&ValidPose6689(relative)){
                        s.stowedAnchorsR112[i]=parent;s.stowedReferenceR112[i]=relative;
                    }
                }
                break;
            }
            parent=s.parent[parent];
        }
    }
}
static bool StowedReferencePoseR112(const CoherentState6689& s,std::size_t slot,
    const std::array<bool,kPoseCapacity6689>& selected,QuatT28& out) noexcept {
    if(slot>=s.stowedAnchorsR112.size())return false;
    const int joint=s.stowedJointsD63[slot],anchor=s.stowedAnchorsR112[slot];
    if(joint<0||joint>=int(s.count)||anchor<0||anchor>=int(s.count)||
       !selected[joint]||!selected[anchor]||!ValidPose6689(s.work[anchor])||
       !ValidPose6689(s.stowedReferenceR112[slot]))return false;
    const auto model=AbsoluteFromRelative(s.work[anchor],s.stowedReferenceR112[slot]);
    if(!ValidPose6689(model))return false;
    out=model;return true;
}
