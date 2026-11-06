// Included inside the physical adapter's private namespace. No native calls.
// Only an authored socket in the primary wrist's own chain qualifies. The
// current animated transform remains the acquisition seed, never this reference.
static bool ShieldPrimaryReferenceB4(const QuatT28* rest,const int* parents,
    std::uint32_t count,int wrist,int joint,const QuatT28& relative,ph::Pose& out) noexcept {
    if(!rest||!parents||wrist<0||joint<0||wrist>=int(count)||joint>=int(count))return false;
    int at=joint;bool descendant=false;
    for(std::uint32_t n=0;n<count;++n){
        if(at==wrist){descendant=true;break;}
        if(at<0||at>=int(count))return false;
        at=parents[at];
    }
    if(!descendant)return false;
    const auto hand=PhysPose6698(rest[wrist]);
    const auto socket=PhysPose6698(rest[joint]);
    const auto local=PhysPose6698(relative);
    if(!ph::finite(hand)||!ph::finite(socket)||!ph::finite(local))return false;
    const auto candidate=ph::compose(ph::inverse(ph::compose(socket,local)),hand);
    if(!ph::finite(candidate)||pm::length(candidate.p)>.5)return false;
    out=candidate;return true;
}

// R26: a polearm's animated RightWeaponRoot is not a wrist socket. Six stable
// samples can therefore learn a point halfway up the shaft. The model's slt_0
// is its authored primary-hand helper; use the current rig's actual hand-slot
// relation, not a fixed human skeleton offset or the animated weapon root.
static bool PolePrimaryReferenceR26(const PhysEquipmentItem6698& item,
    const QuatT28* rest,const int* parents,std::uint32_t count,int wrist,int handSlot,
    const float* scale,ph::Pose& out) noexcept {
    if(item.profile.classId!=7||item.profile.side!=0||!item.profile.twoHanded||
       !item.slotValid34[0]||!item.boundsValid||!scale||!rest||!parents||
       wrist<0||handSlot<0||wrist>=int(count)||handSlot>=int(count))return false;
    int at=handSlot;bool descendant=false;
    for(std::uint32_t n=0;n<count;++n){
        if(at==wrist){descendant=true;break;}
        if(at<0||at>=int(count))return false;
        at=parents[at];
    }
    if(!descendant)return false;
    auto slot=item.slot34[0];
    for(int k=0;k<3;++k)if(!std::isfinite(scale[k])||scale[k]<=0||scale[k]>10||
        !std::isfinite(item.bounds[k])||!std::isfinite(item.bounds[k+3])||
        item.bounds[k]>item.bounds[k+3])return false;
    if(!ph::finite(slot))return false;
    const double v[3]{slot.p.x,slot.p.y,slot.p.z};
    for(int k=0;k<3;++k)if(v[k]<item.bounds[k]-.01||v[k]>item.bounds[k+3]+.01)return false;
    slot.p={slot.p.x*scale[0],slot.p.y*scale[1],slot.p.z*scale[2]};
    const auto hand=PhysPose6698(rest[wrist]),helper=PhysPose6698(rest[handSlot]);
    if(!ph::finite(hand)||!ph::finite(helper))return false;
    const auto handToSlot=ph::compose(ph::inverse(hand),helper);
    if(pm::length(handToSlot.p)>.20)return false;
    const auto candidate=ph::compose(slot,ph::inverse(handToSlot));
    if(!ph::finite(candidate))return false;
    out=candidate;return true;
}
static ph::Pose PoleCaptureCandidateR26(ph::Pose measured,ph::Pose nativePrimary,
    const ph::Pose* reference,bool primary,const ph::Pose* capturedPrimary) noexcept {
    if(!reference)return measured;
    const auto anchor=primary?*reference:(capturedPrimary?*capturedPrimary:*reference);
    // Preserve accepted native captures (including H3's 5 cm deviation). Only
    // a gross, hand-width-plus translation from the authored handle is repaired.
    // Rebase the native hand pair together; never invent a support separation.
    if(!ph::finite(nativePrimary)||pm::length(nativePrimary.p-reference->p)<=.10)return measured;
    return primary?anchor:ph::compose(anchor,ph::compose(ph::inverse(nativePrimary),measured));
}
