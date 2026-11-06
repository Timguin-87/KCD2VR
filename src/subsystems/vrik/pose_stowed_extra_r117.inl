// R117: unnamed Henry BONE sockets (back polearm / two-hander). Same R112 rule:
// the socket keeps its rest offset to the first spine bone above it and follows
// the SOLVED spine. A socket directly on spine/spine1-3 uses that solved bone.
// Head/neck, pelvis/legs and arm/hand-held sockets are never touched.
static bool StowedExtraAnchorR117(const CoherentState6689& s,int joint,int& anchor) noexcept {
    anchor=-1;const RigMap& rig=s.rig;
    const auto spineBone=[&](int j){return j>=0&&(j==rig.spine||j==rig.spine1||j==rig.spine2||j==rig.spine3);};
    if(joint<0||joint>=int(s.count)||s.armR[joint]||s.armL[joint]||s.isFinger[joint])return false;
    if(s.trunk[joint]){if(!spineBone(joint))return false;anchor=joint;return true;}
    int parent=s.parent[joint];
    for(std::uint32_t guard=0;guard<s.count&&parent>=0&&parent<int(s.count);++guard){
        if(s.armR[parent]||s.armL[parent])return false;
        if(s.trunk[parent]){if(!spineBone(parent))return false;anchor=parent;return true;}
        parent=s.parent[parent];
    }
    return false;
}
// Publication side, same transaction as the named D63 slots. A new skeleton
// (InitCoherent6689) or character clears the table: the render leaf registers
// the joints again on its next draw, so loads/VRIK toggles never keep old ids.
static void PublishStowedExtraR117(const CoherentState6689& s,const std::array<bool,kPoseCapacity6689>& selected,
                                   bool full,CommittedFrame6695& packet) noexcept {
    auto& t=g_stowed_extraR117;
    const auto ch=reinterpret_cast<std::uintptr_t>(s.character);
    if(t.character.load(std::memory_order_acquire)!=ch||t.generation.load(std::memory_order_relaxed)!=s.initRuns137)
        t.Reset(ch,s.initRuns137);
    std::uint32_t torso=0,evaluated=0;
    for(std::size_t i=0;i<kStowedExtraR117;++i){
        const int j=t.joint[i].load(std::memory_order_acquire);
        int anchor=-1;
        if(j<0)continue;
        evaluated|=1u<<i;
        if(!StowedExtraAnchorR117(s,j,anchor))continue;
        torso|=1u<<i;
        if(!full||!selected[j]||!selected[anchor]||!ValidPose6689(s.work[anchor]))continue;
        QuatT28 model=s.work[j];
        if(anchor!=j){
            if(!ValidPose6689(s.rest[anchor])||!ValidPose6689(s.rest[j]))continue;
            const auto relative=RelativeFromAbsolute(s.rest[anchor],s.rest[j]);
            if(!ValidPose6689(relative))continue;
            model=AbsoluteFromRelative(s.work[anchor],relative);
        }
        if(!ValidPose6689(model))continue;
        packet.stowedExtraR117[i]={model,j,true};
    }
    t.torsoMask.store(torso,std::memory_order_release);
    t.evaluatedMask.store(evaluated,std::memory_order_release);
}
