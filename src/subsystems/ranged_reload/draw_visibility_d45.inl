// Called only under PhysTryLock6698, for a current equipment item. The receipt
// belongs to the existing entity-hide route; never adopt native-hidden objects.
// The first physical placement reveals the bow atomically with its grip pose.
// A timer is not placement readiness. Only a successful physical placement or
// an explicit native ownership/context takeover retires this owned hide receipt.
// Missing physical ownership stays diagnosable instead of flashing a native pose.
static void PhysRevealDrawnBowD45(const PhysEquipment6698& equipment,
        const PhysEquipmentItem6698& item,std::uint64_t now,bool placed,bool nativeFallback) noexcept {
    struct Pending {void* entity=nullptr;std::uint64_t key=0,generation=0,request=0,body=0,since=0,due=0;};
    static Pending pending[2]{};
    const int side=item.profile.side;
    if(side<0||side>1)return;
    auto& wait=pending[side];
    if(!item.held||!item.profile.bow()||!item.entity||g_hidden_bow_entity59c14.load()!=item.entity){wait={};return;}
    if(wait.entity!=item.entity||wait.key!=item.profile.key||wait.generation!=item.profile.generation||
       wait.request!=equipment.request||wait.body!=equipment.epoch||!wait.since||now<wait.since)
        wait={item.entity,item.profile.key,item.profile.generation,equipment.request,equipment.epoch,now};
    if(PhysHolsterCaptureD48(now)&&(placed||nativeFallback||now>=wait.due)){
        wait.due=now+500;char line[320]{};std::snprintf(line,sizeof(line),
            "D48 HOLSTER_REVEAL key=%016llX generation=%llu body=%llu request=%llu held=%d placed=%d nativeOwner=%d waitMs=%llu decision=%s",
            (unsigned long long)item.profile.key,(unsigned long long)item.profile.generation,
            (unsigned long long)equipment.epoch,(unsigned long long)equipment.request,item.held,placed,nativeFallback,
            (unsigned long long)(now-wait.since),placed?"PLACED":nativeFallback?"NATIVE_OWNER":"WAIT_CURRENT_PLACEMENT");
        diagnostic_trace::Write(line);
    }
    if(placed||nativeFallback)
        StowedBowShow59c14(item.entity,now);
    if(g_hidden_bow_entity59c14.load()!=item.entity)wait={};
}
