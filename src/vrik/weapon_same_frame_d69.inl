// OFF-default D69 experiment. This runs after BOTH body and physical packet
// publication on the same skinning callback. It is a CPU placement receipt,
// not a render/pixel-frame join and not proof that native setters are thread safe.
// No native cache/object lifetime is extended and no normal grace clock renewed.
static bool PhysSameFramePinsD69(void* self,void* attachment,void* entity) noexcept {
    const auto base=WhBase();
    if(!retail_build112::Ready(base)||!PhysVft6698(self,0x3A68F68)||
       !PhysVft6698(attachment,0x3A657C0)||!PhysVft6698(entity,kCEntityVftRva))return false;
    std::uintptr_t setter=0,getCharacter=0;
    if(!Read(reinterpret_cast<void*>(base+kCEntityVftRva+0x168),setter)||setter!=base+0x509810||
       !Read(reinterpret_cast<void*>(base+kCEntityVftRva+0x328),getCharacter)||getCharacter!=base+0x6411AC)return false;
    // Exact existing getter, with its exact child update slot checked BEFORE
    // any placement. PlaceBothConsumers retains its existing SEH failure leaf.
    void* child=CharacterForEntity6689(entity);std::uintptr_t update=0;
    return !child||(PhysVft6698(child,0x40602F8)&&
        Read(reinterpret_cast<void*>(base+0x40602F8+0x190),update)&&update==base+0x423C30);
}
static void PhysSameFrameD69(std::uint64_t publication,std::uint64_t sample,
        std::uint64_t motionSession,std::uint64_t motionCycle) noexcept {
    namespace wt=kcd2vr::weapon_trials_d69;
    if(!wt::sameFrame())return; // OFF performs no new reads, queries or calls.
    const auto revision=wt::sameFrameRevision.load(std::memory_order_acquire);
    const auto now=std::uint64_t(GetTickCount64());
    if(!retail_build112::Ready(WhBase())||!publication||(publication&1)||!sample||
       g_weapon_control_mode.load()<4||animation_filter::nativeBodyOwned112.load()||
       NativePassengerCurrentA34(now)||diagnostics::lab::NativeReference()||
       g_phys_fault6698.load()||!g_gameplay_allowed.load()||!TrackingHeadUsable111(now)||
       heldT4::HasUnretiredCollider())return; // native body has its own writer
    PhysTryLock6698 lock;if(!lock.owned)return; // existing attachment writer lock
    PhysFrame6698 f{};PhysEquipment6698 equipment{};CommittedFrame6695 body{};
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
    if((policy&1)||ready!=sample||!g_phys_frame6698.read(f)||!g_phys_equipment6698.read(equipment)||
       ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||f.input.sample!=sample||
       f.publicationT23!=publication||!f.input.allowed||!equipment.valid||
       f.request!=equipment.request||f.request!=g_weapon_request_epoch6694.load()||
       f.input.bodyEpoch!=equipment.epoch||f.input.bodyEpoch!=g_pose_epoch6689.load()||
       f.player!=equipment.player||f.manager!=equipment.manager||f.manager!=g_henry_item_attach_manager.load()||
       !f.input.tick||now<f.input.tick||now-f.input.tick>80||now<equipment.tick||now-equipment.tick>80||
       !PhysReadVisualBodyR24(publication,f.input.bodyEpoch,f.trackingEpochT23,f.characterT23,body)||
       body.stamp.sample!=sample||body.stamp.tick!=f.input.tick)return;
    for(int j=0;j<8;++j)if(!std::isfinite(f.world[j])||f.world[j]!=body.world[j])return;
    if(f.world[7]<.1f||f.world[7]>10.f)return;
    static std::uint64_t lastPublication[2]{},lastRevision[2]{};
    for(int side=0;side<2;++side){
        const auto& p=f.profiles[side];const auto& r=g_phys_placement6698[side];
        if(lastPublication[side]==publication&&lastRevision[side]==revision)continue;
        if(!body.valid[side]||!f.input.tracked[side]||!TrackingHandUsable111(side,now)||
           !f.output.active[side]||!f.output.owned[side]||f.output.solverOwnedT4[side]||
           !p.primary||p.side!=side||!p.key||!p.generation||
           f.correlation126[side].stamp.actor!=f.characterT23||
           !r.changed||r.side!=side||r.key!=p.key||r.generation!=p.generation||
           r.request146!=f.request||r.epoch146!=f.input.bodyEpoch||r.manager!=equipment.manager||
           !r.sourceTick146||now<r.sourceTick146||now-r.sourceTick146>80||
           r.entityId!=p.entity||!PhysEntityIdentity135(r.entity,r.entityId))continue;
        const auto* item=static_cast<const PhysEquipmentItem6698*>(nullptr);
        for(int i=0;i<equipment.count&&i<int(equipment.items.size());++i)
            if(equipment.items[i].held&&equipment.items[i].profile.side==side&&
               equipment.items[i].profile.key==p.key&&equipment.items[i].profile.generation==p.generation&&
               equipment.items[i].profile.entity==p.entity&&equipment.items[i].entity==r.entity){item=&equipment.items[i];break;}
        if(!item)continue;
        void* object=nullptr;void* manager=nullptr;void* character=nullptr;std::uint32_t id=0;
        std::uint64_t live=0;
        if(!Read(static_cast<unsigned char*>(r.attachment)+0x20,object)||object!=r.self||
           !Read(static_cast<unsigned char*>(r.self)+8,id)||id!=r.entityId||
           !Read(static_cast<unsigned char*>(r.attachment)+0x28,manager)||!manager||
           manager!=g_henry_char_attachment_manager.load()||
           !Read(static_cast<unsigned char*>(manager)+0x70,character)||
           reinterpret_cast<std::uintptr_t>(character)!=f.characterT23||
           !PhysVft6698(character,0x40602F8)||
           !NativeHandKey6697(equipment.manager,side,live)||live!=p.key||
           !PhysSameFramePinsD69(r.self,r.attachment,r.entity))continue;
        const auto local=(f.output.displayLatched[side]||f.output.displayRejoinR13[side])?
            f.output.displayWeapons[side]:f.output.weapons[side];
        if(!ph::finite(local)||!std::isfinite(r.wanted[7])||r.wanted[7]<.001f||r.wanted[7]>100.f)continue;
        ph::Pose scaled=local;scaled.p=scaled.p*double(f.world[7]);
        const auto target=ph::compose(PhysWorld6698(f.world),scaled);
        float wanted[8]{float(target.q.x),float(target.q.y),float(target.q.z),float(target.q.w),
            float(target.p.x),float(target.p.y),float(target.p.z),r.wanted[7]};
        bool finite=true;for(float v:wanted)finite=finite&&std::isfinite(v);if(!finite)continue;
        // Revalidate ownership after all native getters; never replay an old
        // target on OFF/menu/native/loss/swap. No restore is run on this thread.
        const auto finalNow=std::uint64_t(GetTickCount64());
        if(!wt::sameFrame()||wt::sameFrameRevision.load()!=revision||!retail_build112::Ready(WhBase())||
           g_weapon_control_mode.load()<4||g_phys_fault6698.load()||NativePassengerCurrentA34(finalNow)||
           heldT4::HasUnretiredCollider()||finalNow<f.input.tick||finalNow-f.input.tick>80||
           f.request!=g_weapon_request_epoch6694.load()||equipment.manager!=g_henry_item_attach_manager.load()||
           policy!=g_pose_policy_generation57.load()||publication!=ReadCompletedPublicationT23()||
           sample!=g_phys_ready_sample6698.load()||!g_committed_pose_valid6689.load()||
           f.input.bodyEpoch!=g_pose_epoch6689.load()||f.trackingEpochT23!=tracking59c6::epoch.load()||
           f.characterT23!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
           !g_gameplay_allowed.load()||animation_filter::nativeBodyOwned112.load()||
           diagnostics::lab::NativeReference()||!TrackingHeadUsable111(finalNow)||!TrackingHandUsable111(side,finalNow)||
           !NativeHandKey6697(equipment.manager,side,live)||live!=p.key||
           !Read(static_cast<unsigned char*>(r.attachment)+0x20,object)||object!=r.self||
           !Read(static_cast<unsigned char*>(r.attachment)+0x28,manager)||manager!=g_henry_char_attachment_manager.load()||
           !Read(static_cast<unsigned char*>(manager)+0x70,character)||
           reinterpret_cast<std::uintptr_t>(character)!=f.characterT23||!PhysEntityIdentity135(r.entity,r.entityId))return;
        bool child=false,checked=false;bool writeOk=false,readback=false;
        {PerfScope101R2 timing(2);InvalidateReadCache101R2();
            writeOk=PlaceBothConsumers6689(r.self,r.entity,wanted,child);InvalidateReadCache101R2();}
        lastPublication[side]=publication;lastRevision[side]=revision; // attempt once, including failure
        namespace md=kcd2vr::diagnostics::motion_d68;
        if(motionSession&&motionCycle&&md::requested()){
            md::Event e{};static thread_local md::Cadence cadence[2];
            if(md::begin(e,md::Attachment)&&e.session==motionSession&&md::inWindow(e)&&cadence[side].due(e.session,e.ns)){
                e.cycle=motionCycle;
                if(writeOk)readback=PlacementReadback6689(r.self,r.entity,r.entityId,wanted,checked);
                e.actor=f.characterT23;e.object=reinterpret_cast<std::uintptr_t>(r.attachment);e.sample=sample;
                e.sourceNs=f.input.timeNs;e.sourceTick=f.input.tick;e.poseEpoch=f.input.bodyEpoch;
                e.trackingEpoch=f.trackingEpochT23;e.publication=publication;e.request=f.request;
                e.side=side;e.thread=GetCurrentThreadId();e.flags=0x10000u|(writeOk?1u:0u)|(child?2u:0u)|
                    (writeOk?4u:0u)|(checked?8u:0u)|(readback?16u:0u);
                std::copy_n(wanted,8,e.v);std::copy_n(r.wanted,8,e.v+8);std::copy_n(f.world,8,e.v+16);
                std::copy_n(f.world,8,e.v+24);e.count=32;(void)md::push(e);
            }
        }
        // This experiment has no lease/restore state of its own. On failure
        // the regular attachment callback remains the sole normal fallback.
    }
}
