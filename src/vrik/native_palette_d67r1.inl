// Included in frik_pose_probe.cpp's private namespace, after existing validated
// native readers. No hook, native call, palette write, pose sampling or wait.
namespace paletteD67R1=kcd2vr::diagnostics::native_palette_d67r1;
static bool NativePaletteAbiD67R1() noexcept {
    const auto base=WhBase();if(!kcd2vr::retail_build112::Ready(base)||!SupportedWhBuild(base))return false;
    // Pinned image plus the instructions which establish each read offset.
    // A modified/other build leaves the observer unqualified; gameplay proceeds.
    static std::uintptr_t checked=0;static bool valid=false; // existing pose mutex
    if(checked==base)return valid;
    checked=base;valid=false;
    const auto matches=[&](std::uintptr_t rva,const auto& expected){
        unsigned char actual[16]{};return sizeof(expected)<=sizeof(actual)&&
            Safe(reinterpret_cast<const void*>(base+rva),sizeof(expected))&&
            Read(reinterpret_cast<const void*>(base+rva),actual)&&
            std::memcmp(actual,expected,sizeof(expected))==0;
    };
    const unsigned char counter[]{0x42,0x39,0xac,0xf7,0x48,0x0a,0x00,0x00}; // 3D4EB3 [CI+slot*16+A48]
    const unsigned char slot[]{0x48,0x89,0x04,0xf7}; // 3D4EF4 [CI+(slot+A4)*16]
    const unsigned char previous[]{0x48,0x89,0x43,0x30}; // 3D4F24 data->previous
    const unsigned char output[]{0x48,0x8b,0x42,0x08}; // 5FE27B data->current output
    const unsigned char transfer[]{0x48,0x89,0x47,0x40}; // 3D5D01 data->bulk transfer entry
    const unsigned char previousOutput[]{0x48,0x8b,0x42,0x30,0x4d,0x89,0x43,0xd0,0x4c,0x8b,0x40,0x08};
    valid=matches(0x3D4EB3,counter)&&matches(0x3D4EF4,slot)&&matches(0x3D4F24,previous)&&
        matches(0x5FE27B,output)&&matches(0x5FE28F,previousOutput)&&matches(0x3D5D01,transfer);
    return valid;
}
struct NativePaletteSlotD67R1 {std::uintptr_t data=0;std::int32_t pool=-1;std::uint32_t unused=0;};
static_assert(sizeof(NativePaletteSlotD67R1)==16);
static bool SamePaletteSlotsD67R1(const NativePaletteSlotD67R1 (&a)[3],const NativePaletteSlotD67R1 (&b)[3]) noexcept {
    for(unsigned i=0;i<3;++i)if(a[i].data!=b[i].data||a[i].pool!=b[i].pool)return false;
    return true;
}
static bool ReadPaletteRingD67R1(const paletteD67R1::Record& r,paletteD67R1::Ring& out) noexcept {
    NativePaletteSlotD67R1 before[3]{},after[3]{};std::array<paletteD67R1::Slot,3> slots{};
    const auto ring=reinterpret_cast<const unsigned char*>(r.character)+0xA40;
    if(!Read(ring,before))return false;
    for(unsigned i=0;i<3;++i){const auto& b=before[i];auto& s=slots[i];s.data=b.data;s.pool=b.pool;
        if(!b.data)continue;
        const auto data=reinterpret_cast<const unsigned char*>(b.data);
        s.readable=Read(data+8,s.output)&&Read(data+0x30,s.previousData)&&Read(data+0x40,s.transfer);
    }
    // These are observations of native-owned fields, not ownership locks. The
    // exact descriptor/output match and repeated ring read reject visible reuse.
    if(!Read(ring,after)||!SamePaletteSlotsD67R1(before,after))return false;
    out=paletteD67R1::identify(slots,r.output,r.previousOutput);
    if(out.slot>=0){std::uintptr_t output=0,previous=0,transfer=0;
        const auto data=reinterpret_cast<const unsigned char*>(out.data);
        if(!Read(data+8,output)||!Read(data+0x30,previous)||!Read(data+0x40,transfer)||
           output!=r.output||previous!=out.previousData||transfer!=out.transfer)return false;
    }
    return true;
}
static bool PaletteIdentityD67R1(const paletteD67R1::Record& r,std::uint64_t now) noexcept {
    if(r.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       !PlayerIdentityAccepts37(reinterpret_cast<const void*>(r.character),now))return false;
    void* anim=nullptr;void* skeleton=nullptr;
    return ValidateCharInstance(reinterpret_cast<void*>(r.character),anim)&&
        ReadValidatedDefaultSkeletonFromPose(reinterpret_cast<void*>(r.pose),skeleton)&&
        reinterpret_cast<std::uintptr_t>(skeleton)==r.skeleton;
}
static void ReadNativePaletteD67R1(paletteD67R1::Record& r,unsigned phase,std::uint32_t count) noexcept {
    if(phase>1||!(r.flags&paletteD67R1::Abi))return;
    const auto now=std::uint64_t(GetTickCount64());if(!paletteD67R1::current(r.session))return;
    const auto start=paletteD67R1::clockNs();
    const auto finish=[&]{r.readNs+=paletteD67R1::clockNs()-start;};
    if(!PaletteIdentityD67R1(r,now)){finish();return;}
    r.flags|=phase?paletteD67R1::IdentityPost:paletteD67R1::IdentityPre;
    std::uintptr_t descriptor[6]{},again[6]{};
    if(!r.output||!Read(reinterpret_cast<const void*>(r.descriptor),descriptor)||descriptor[0]!=r.pose||
       descriptor[1]!=r.skeleton||descriptor[2]!=r.character+8||descriptor[4]!=r.output||
       !descriptor[3]||(phase&&descriptor[3]!=r.previousOutput)){finish();return;}
    if(!phase)r.previousOutput=descriptor[3];
    r.flags|=phase?paletteD67R1::DescriptorPost:paletteD67R1::DescriptorPre;
    paletteD67R1::Ring ringBefore{},ringAfter{};
    const bool ringKnown=ReadPaletteRingD67R1(r,ringBefore);
    // Four 32-byte reads per phase at already-validated joint IDs. No format or
    // quaternion convention is inferred from the bytes; GPU consumption is later.
    for(unsigned output=0;output<2;++output)for(unsigned side=0;side<2;++side){
        const int joint=r.wrists[side];if(joint<0||std::uint32_t(joint)>=count)continue;
        const auto address=output?r.previousOutput:r.output;
        if(!address||address>UINTPTR_MAX-std::uintptr_t(joint)*32u)continue;
        std::array<unsigned char,32> bytes{};
        if(Read(reinterpret_cast<const void*>(address+std::uintptr_t(joint)*32u),bytes)){
            r.hashes[phase][output][side]=paletteD67R1::hash32(bytes);r.masks[phase]|=1u<<(output*2+side);
        }
    }
    const bool descriptorSame=Read(reinterpret_cast<const void*>(r.descriptor),again)&&
        std::equal(std::begin(descriptor),std::end(descriptor),std::begin(again));
    if(!descriptorSame||!PaletteIdentityD67R1(r,std::uint64_t(GetTickCount64()))){
        r.masks[phase]=0;r.flags&=~unsigned(phase?paletteD67R1::DescriptorPost:paletteD67R1::DescriptorPre);finish();return;
    }
    if(ringKnown&&ReadPaletteRingD67R1(r,ringAfter)&&paletteD67R1::same(ringBefore,ringAfter)){
        (phase?r.after:r.before)=ringBefore;
        r.flags|=phase?paletteD67R1::RingPostStable:paletteD67R1::RingPreStable;
        if(phase&&r.before.data&&(r.flags&paletteD67R1::RingPreStable)&&paletteD67R1::same(r.before,r.after))r.flags|=paletteD67R1::RingUnchanged;
    }
    finish();
}
static std::optional<paletteD67R1::Record> BeginNativePaletteD67R1(void* desc,void* pose,void* skeleton,void* character,
    void* output,const RigMap& rig,std::uint32_t count,const TrackingSnapshot& tracking,
    const CoherentTransaction6689& tx,bool committed) noexcept {
    // Reuse this exact D68 core burst/cycle. No independent resampling can split
    // the pre/post group at the burst edge. A stopped session still retires it.
    if(!tx.motionSessionD68||!tx.motionCycleD68||!paletteD67R1::current(tx.motionSessionD68))return std::nullopt;
    const auto session=tx.motionSessionD68,now=std::uint64_t(GetTickCount64());
    const auto serial=paletteD67R1::ordinal.fetch_add(1,std::memory_order_relaxed)+1;
    static paletteD67R1::Budget budget; // caller holds the existing Henry pose mutex
    if(!budget.take(session,now)){paletteD67R1::suppressed.fetch_add(1,std::memory_order_relaxed);return std::nullopt;}
    paletteD67R1::Record r{};r.session=session;r.cycle=tx.motionCycleD68;r.serial=serial;r.tick=now;r.thread=GetCurrentThreadId();
    r.sample=tracking.sequence;r.epoch=tx.epoch;r.trackingEpoch=tracking.source_epoch;r.sourceNs=tracking.timeNs;r.locateTarget=tracking.poseTime;
    r.character=reinterpret_cast<std::uintptr_t>(character);r.pose=reinterpret_cast<std::uintptr_t>(pose);
    r.skeleton=reinterpret_cast<std::uintptr_t>(skeleton);r.descriptor=reinterpret_cast<std::uintptr_t>(desc);r.output=reinterpret_cast<std::uintptr_t>(output);
    r.wrists[0]=rig.hand_r;r.wrists[1]=rig.hand_l;
    if(committed)r.flags|=paletteD67R1::ScopedPose;
    if(NativePaletteAbiD67R1())r.flags|=paletteD67R1::Abi;
    ReadNativePaletteD67R1(r,0,count);return r;
}
static void FinishNativePaletteD67R1(paletteD67R1::Record& r,const CoherentTransaction6689& tx) noexcept {
    // Everything native was read BEFORE Restore. Only copied values and existing
    // publication atomics are used here or on the diagnostic worker.
    r.publication=tx.publicationD49;
    if(tx.pairedPublishedD49)r.flags|=paletteD67R1::PairedPublication;
    if(tx.pairedPublishedD49&&r.publication==ReadCompletedPublicationT23()&&
       g_committed_pose_valid6689.load(std::memory_order_acquire))r.flags|=paletteD67R1::PublicationCurrent;
    r.finishNs=paletteD67R1::clockNs();(void)paletteD67R1::push(r);
}

// INVESTIGATE_D67R1: native producer lineage is now known: 3D4E2C caches
// CCharInstance+A40/A48 in three 16-byte slots; 3D6054 queues DE7A18, which
// calls 5FE260 and this 5FE2F0 callback with current/previous output pointers.
// The receipt joins those bytes to body/attachment publication; a native pool
// counter is NOT predicted display time. Missing: the actual per-eye draw's
// current/previous skinning resource selection. 3D4ACC calls the allocator at
// 3D4C44, stores skinData at renderObject+58 (3D4C49), and stores character at
// renderObject+50 (3D4BE5). 4A34FC obtains a separately cached remapped skinData
// via renderer+8F8 (call 4A3844). Its allocator 3D5EC0 shares the parent's palette
// and transfer entry (+8/+40), but keeps wrapper/remap lineage. Actual bulk
// upload 505DA8 chooses renderer+63648 by CRenderView+8D40 modulo 3, waits the
// skinData+28 job at 505EC1, and copies data+8 (nBones*32) at 505EE7. The copied
// transfer entry in this receipt is that exact next join; still no upload/read
// or per-eye draw is observed by this producer. Follow its real callers and
// buffer finalization at 863B14, joining the SAME data/output
// pointer plus pool counter to both existing eye submission receipts. Success:
// each eye's Henry mesh and held attachment resolve to one body publication,
// and previous-frame history resolves to the preceding drawn palette. Do not
// infer this from a nearby camera token, pointer equality alone, or add future
// prediction to gameplay/collision input while the final join is unproved.
