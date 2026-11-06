static bool CurrentGraspMapping55(const CoherentState6689& s,int side,kcd2vr::grasp13::FingerMapping55& out) noexcept {
    out={};if(side<0||side>1||!s.ready||!s.fingerMap[side]||!s.owner||!s.graspMappingGeneration55)return false;
    const unsigned n=unsigned(s.fingerCount[side]);if(!n||n>out.ids.size())return false;
    kcd2vr::grasp13::FingerMapping55 m{};m.skeleton=reinterpret_cast<std::uintptr_t>(s.owner);
    m.generation=s.graspMappingGeneration55;m.joints=s.count;m.wrist=side?s.rig.hand_l:s.rig.hand_r;m.count=n;
    for(unsigned i=0;i<n;++i){const int id=s.fingerIds[side][i];if(id<0||unsigned(id)>=s.count)return false;
        m.ids[i]=id;m.parents[i]=s.parent[id];}
    if(!kcd2vr::grasp13::validFingerMapping55(m))return false;out=m;return true;
}
static bool OriginalGraspRotations55(const CoherentState6689& s,int side,kcd2vr::grasp13::FingerRotations54& out,unsigned& count) noexcept {
    kcd2vr::grasp13::FingerMapping55 mapping{};if(!CurrentGraspMapping55(s,side,mapping))return false;
    const unsigned n=mapping.count;if(n>out.size())return false;
    kcd2vr::grasp13::FingerRotations54 native{};
    for(unsigned i=0;i<n;++i){const int id=mapping.ids[i],parent=mapping.parents[i];
        if(parent<0||unsigned(parent)>=s.count||!ValidPose6689(s.original[id])||!ValidPose6689(s.original[parent]))return false;
        const auto local=RelativeFromAbsolute(s.original[parent],s.original[id]);
        if(!ValidPose6689(local)||std::fabs(Len(FromT(local))-Len(FromT(s.restLocal[id])))>.004f)return false;
        for(unsigned q=0;q<4;++q)native[i][q]=local.q[q];
    }
    out=native;count=n;return true;
}
static bool OriginalGraspFingers55(const CoherentState6689& s,int side,const kcd2vr::grasp13::Binding& binding,
                                 std::uint64_t epoch,std::uint64_t sample,std::uint64_t tick,std::uint64_t now,
                                 kcd2vr::grasp13::FingerReference55& out,
                                 const kcd2vr::grasp13::Acquisition54* originalAction55=nullptr) noexcept {
    out={};if(side<0||side>1||!sample||!tick)return false;
    const unsigned n=unsigned(s.fingerCount[side]);
    if(!s.fingerMap[side]||!n||n>32||binding.side!=side||binding.character!=reinterpret_cast<std::uintptr_t>(s.character)||
       binding.epoch!=epoch||(!(binding.attached&&binding.place)&&!binding.socketOnly28))return false;
    kcd2vr::grasp13::Acquisition54 acquisition{};
    if(!g_graspAcquisition54||!g_graspAcquisition54(acquisition)||
       !kcd2vr::grasp13::freshAcquisition54(acquisition,now)||!kcd2vr::grasp13::acquisitionOwns55(binding,acquisition)||
       (originalAction55&&!kcd2vr::grasp13::sameAcquisition55(acquisition,*originalAction55)))return false;
    kcd2vr::grasp13::FingerReference55 reference{};reference.acquisition=acquisition;
    reference.sample=sample;reference.tick=tick;
    if(!CurrentGraspMapping55(s,side,reference.mapping)||!OriginalGraspRotations55(s,side,reference.rotations,reference.count))return false;
    if(!kcd2vr::grasp13::validFingerReference55(binding,reference))return false;
    out=reference;return true;
}
static bool ApplyGraspFingers54(CoherentState6689& s,int side,const kcd2vr::grasp13::Binding& binding,
                                std::uint64_t epoch,std::uint64_t now) noexcept {
    (void)now;if(side<0||side>1||HandClaimBlockedT24(side,reinterpret_cast<std::uintptr_t>(s.character),epoch,g_weapon_request_epoch6694.load()))return false;auto& cache=s.graspFingers54[side];
    const unsigned n=unsigned(s.fingerCount[side]);
    if(!s.fingerMap[side]||!n||n>32||binding.side!=side||binding.character!=reinterpret_cast<std::uintptr_t>(s.character)||
       binding.epoch!=epoch){cache={};return false;}
    // Do not freeze the first attached frame: its animation phase can precede
    // the transaction which supplies the canonical prop grip. Until that exact
    // paired reference returns, the native locals remain the presentation owner.
    kcd2vr::grasp13::FingerMapping55 mapping{};
    if(!CurrentGraspMapping55(s,side,mapping)){cache={};return false;}
    if(!cache.currentCanonical55(binding,mapping)&&!cache.captureCanonical55(binding,n,&mapping))return false;
    for(unsigned i=0;i<n;++i){const int id=s.fingerIds[side][i];s.dirty[id]=true;
        for(unsigned q=0;q<4;++q)s.local[id].q[q]=cache.rotations[i][q];
        ToT(s.local[id],FromT(s.restLocal[id]));}
    return true;
}
// C55: native cleanup temporarily withholds the public attachment binding.
// Keep only the inert native finger sample across that gap; it cannot drive
// fingers until the SAME token/item/hand/body epoch is positively revalidated.
// Clearing it on a failed read lost the actual grasp before Finish returned.
static bool ApplyPublishedGraspFingers55(CoherentState6689& s,int side,std::uint64_t epoch,
                                        std::uint64_t now) noexcept {
    if(side<0||side>1)return false;
    auto& cache=s.graspFingers54[side];
    if(cache.ready&&(cache.owner.character!=reinterpret_cast<std::uintptr_t>(s.character)||cache.owner.epoch!=epoch))cache={};
    kcd2vr::grasp13::Binding binding{};
    if(!g_graspBinding13||!g_graspBinding13(binding))return false;
    if(!((binding.attached&&binding.place)||binding.socketOnly28)||binding.side!=side||
       binding.character!=reinterpret_cast<std::uintptr_t>(s.character)||binding.epoch!=epoch){cache={};return false;}
    if(!ApplyGraspFingers54(s,side,binding,epoch,now)){
        // The actual held owner also wins before its paired reference arrives.
        // Restore this transaction's native locals explicitly: an earlier
        // physical-item finger path may already have written another cache.
        kcd2vr::grasp13::FingerRotations54 native{};unsigned n=0;
        if(OriginalGraspRotations55(s,side,native,n))for(unsigned i=0;i<n;++i){const int id=s.fingerIds[side][i];
            s.dirty[id]=true;for(unsigned q=0;q<4;++q)s.local[id].q[q]=native[i][q];ToT(s.local[id],FromT(s.restLocal[id]));}
    }
    return true; // actual held owner keeps native locals if a canonical sample is unavailable
}
// Even revisions are complete semantic-policy publications. A menu/modal ABA
// invalidates an in-flight acquisition even when its final flags look equal.
extern std::atomic<std::uint64_t> g_pose_policy_generation57;
