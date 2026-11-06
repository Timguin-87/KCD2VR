// T23 native owner publication. No attachment creation or loadout-slot writer.
static bool PhysDrawCandidate126(const PhysEquipment6698&,int) noexcept;
static std::uint64_t ReadCompletedPublicationT23() noexcept;
extern std::atomic<float> g_holster_radius136;
struct PhysSupplyAdmissionT23 {
    kcd2vr::throw_admission_t23::Pending pending{};heldT4::Owner retiring{};
    std::uint64_t admission=0;bool captured=false,refused=false;
};
static PhysSupplyAdmissionT23 g_supply_admission_t23[2];
static bool PhysSupplyOwnerT23(const heldT4::Owner& owner,const kcd2vr::supply_t2::Input& in) noexcept {
    return owner.side==0&&owner.player==in.player&&owner.manager==in.manager&&owner.character==in.character&&
        owner.epoch==in.body&&owner.request==in.request;
}
static void PhysSupplyAdmissionTickT23(const kcd2vr::throw_admission_t23::Source& source) noexcept {
    namespace t=kcd2vr::throw_admission_t23;
    for(int side=0;side<2;++side){
        t::Pending pending{};auto& state=g_supply_admission_t23[side];
        const auto observed=t::ObservePending(side,pending);
        if(observed==t::PendingState::Unknown){(void)t::PublishGrant(side,{});continue;}
        if(observed==t::PendingState::Absent){
            state={};(void)t::PublishGrant(side,{});
            if(side==0)heldT4::SetThrowAdmissionSuspendedT23(false);
            continue;
        }
        (void)t::PublishGrant(side,{}); // no old completed-frame ACK survives a miss
        if(!t::SamePending(state.pending,pending)){state={};state.pending=pending;}
        if(!source.complete||!t::PublicationQualified(source)||!t::SameOwner(pending.owner,source.frame)||!source.handKnown[side]||!source.handEmpty[side]||
           !source.frame.pose18[side]||source.frame.poseTick18[side]!=source.frame.tick)continue;
        // Left does not cancel, suspend or wait for an unrelated right solver.
        if(side==1){(void)t::PublishGrant(side,{pending,source.frame,source.tracking,0,true,source.publication,source});continue;}
        if(state.refused)continue;
        const auto admission=heldT4::AdmissionSerial();
        if(!state.captured){
            if(heldT4::HasUnretiredCollider()){
                if(!heldT4::ReadRetainedOwner(state.retiring)||!PhysSupplyOwnerT23(state.retiring,pending.owner))continue;
            }
            state.admission=admission;state.captured=true;
        }
        if(admission!=state.admission){state.refused=true;continue;}
        // Independent bit8 blocks CAS1->3; never clears H36 equipment bit4.
        // The root's exact Retire(owner), not Cancel(), names this right owner.
        heldT4::SetThrowAdmissionSuspendedT23(true);
        if(!heldT4::ThrowAdmissionSuspendedT23())continue;
        if(heldT4::HasUnretiredCollider()){
            heldT4::Owner current{};
            if(!state.retiring.key||!heldT4::ReadRetainedOwner(current)||current!=state.retiring||
               !PhysSupplyOwnerT23(current,pending.owner)||heldT4::AdmissionSerial()!=state.admission){state.refused=true;continue;}
            heldT4::Retire(current);continue; // retirement request is NEVER ACK
        }
        if(heldT4::AdmissionSerial()!=state.admission)continue;
        (void)t::PublishGrant(side,{pending,source.frame,source.tracking,state.admission,true,source.publication,source});
    }
}
static void PhysPublishSupplyT2(const PhysFrame6698& frame,const PhysEquipment6698& e,bool allowed,std::uint64_t now) noexcept {
    namespace s=kcd2vr::supply_t2;namespace t=kcd2vr::throw_admission_t23;s::Input i{};t::Source source{};
    if(!kcd2vr::action_time_owner59c11::ThrowingSupplyT2()&&!s::nativeLease.load(std::memory_order_acquire)){
        (void)t::PublishLive({});(void)t::PublishSource(source);PhysSupplyAdmissionTickT23(source);
        kcd2vr::action_time_owner59c11::PublishThrowingSupplyT2(i);return;
    }
    i.manager=reinterpret_cast<std::uintptr_t>(e.manager);i.player=reinterpret_cast<std::uintptr_t>(e.player);
    i.body=frame.input.bodyEpoch;i.request=frame.request;i.sample=frame.input.sample;i.tick=frame.input.tick;
    i.hand[0]=e.handKey[0];i.hand[1]=e.handKey[1];i.beltKey=e.handKey[1]; // wire field16 now excludes the actual occupied left hand
    CommittedFrame6695 packet{};
    void* ci=allowed?HenryCharacter59c15():nullptr;void* entity=allowed?g_last_player_entity.load(std::memory_order_acquire):nullptr;
    std::uint32_t id=0;
    const auto publicationT23=ReadCompletedPublicationT23();
    const bool nativeExact=allowed&&frame.input.allowed&&e.valid&&frame.player==e.player&&frame.manager==e.manager&&
        e.epoch==i.body&&e.request==i.request&&frame.input.epoch==i.request&&e.count>=0&&e.count<=int(e.items.size())&&
        ci&&entity&&PlayerIdentityAccepts37(ci,now)&&PhysVft6698(entity,kCEntityVftRva)&&
        Read(static_cast<unsigned char*>(entity)+0xC,id)&&id&&
        static_cast<unsigned char*>(ci)+8==g_henry_char_attachment_manager.load(std::memory_order_acquire)&&
        frame.characterT23==reinterpret_cast<std::uintptr_t>(ci)&&frame.trackingEpochT23&&
        frame.trackingEpochT23==tracking59c6::epoch.load(std::memory_order_acquire);
    t::Live live{};live.allowed=nativeExact;live.observed=now;
    if(nativeExact){live.player=i.player;live.manager=i.manager;live.character=reinterpret_cast<std::uintptr_t>(ci);
        live.entity=id;live.body=i.body;live.request=i.request;live.tracking=frame.trackingEpochT23;}
    for(int side=0;side<2;++side){live.handKnown[side]=nativeExact&&e.handKnown[side];live.handEmpty[side]=live.handKnown[side]&&!e.handKey[side];
        live.tracked[side]=nativeExact&&frame.input.tracked[side]&&frame.input.intentTracked[side]&&TrackingHandUsable111(side,now);}
    (void)t::PublishLive(live);
    const bool exact=nativeExact&&ReadCommittedPacket6695(packet)&&publicationT23&&!(publicationT23&1)&&publicationT23==frame.publicationT23&&
        publicationT23==ReadCompletedPublicationT23()&&packet.stamp.sample==i.sample&&packet.stamp.epoch==i.body&&packet.stamp.tick==i.tick&&
        packet.stamp.actor==reinterpret_cast<std::uintptr_t>(ci)&&packet.stamp.actor==frame.characterT23&&
        packet.doorPlayer38==reinterpret_cast<std::uintptr_t>(entity)&&frame.trackingEpochT23&&
        packet.doorTrackingEpoch66==frame.trackingEpochT23&&frame.trackingEpochT23==tracking59c6::epoch.load(std::memory_order_acquire)&&
        s::fresh(now,i.tick,80)&&!std::memcmp(packet.world,frame.world,sizeof(frame.world));
    i.allowed=exact;i.chestKnivesT26=kcd2vr::ammo_service_t22::integrated&&kcd2vr::ammo_service_t22::leftChestKnivesT26;
    if(exact){i.character=reinterpret_cast<std::uintptr_t>(ci);i.entity=id;source.tracking=frame.trackingEpochT23;
        source.publication=publicationT23;}
    i.grip=exact?float(frame.input.grip[0]):0;i.tracked=exact&&frame.input.tracked[0]&&frame.input.intentTracked[0]&&packet.valid[0];
    for(int side=0;side<2;++side){
        i.grip18[side]=exact?float(frame.input.grip[side]):0;
        i.trigger18[side]=exact?float(frame.input.trigger[side]):0;
        i.gripRaw18[side]=std::isfinite(frame.input.grip[side])?float(frame.input.grip[side]):0; // R109: never gated by exact
    }
    const auto a=ph::body_axes(frame.input);
    i.rightHipPouchD66=false; // left hip only; retain packet field for compatibility
    // R109: the knife zone is the left hip OR the chest pouch (VrKnifeChestZoneR109, where the VrChestDagger
    // visual sits); offsets/cue measure from the centre nearer to the LEFT hand. D85R5 had hip only.
    const pm::Vec3 chest=ph::knife_pouch_centre_r109(frame.input,frame.input.hands[1].p);
    i.zoneRaw18=i.chestKnivesT26&&ph::knife_pouch_holds_r109(frame.input,frame.input.hands[1].p);
    const pm::Vec3 mount[2]{i.chestKnivesT26?chest:ph::body_shoulder_r38(frame.input,0)-a.up*.50+a.right*.03+a.forward*.13,
        ph::body_shoulder_r38(frame.input,0)-a.up*.25-a.forward*.25+a.right*.06};
    for(int k=0;k<2;++k){const auto offset=frame.input.hands[0].p-mount[k];
        i.offset[k][0]=float(offset.x);i.offset[k][1]=float(offset.y);i.offset[k][2]=float(offset.z);}
    // Same normal hip candidate, finite stock, ambiguity and neutral Grip
    // arbitration, evaluated before final F9 holster suppression zeros grants.
    static kcd2vr::hip_t10::State reachState{};
    kcd2vr::hip_t10::Input reach{};reach.body=i.body;reach.request=i.request;reach.sample=i.sample;reach.tick=i.tick;reach.grip=i.grip;
    reach.allowed=exact&&i.tracked;reach.stock=kcd2vr::weapon_release_t1::TestEnabledH21()&&kcd2vr::action_time_owner59c11::ThrowingSupplyT2();
    reach.equipmentRadius=g_holster_radius136.load();
    reach.available[0]=kcd2vr::inventory_lifecycle_t8::daggers.load()>0;reach.available[1]=kcd2vr::inventory_lifecycle_t8::axes.load()>0;
    const auto centre=ph::body_shoulder_r38(frame.input,0)-a.up*.50+a.right*.03;
    reach.distance[0]=pm::length(frame.input.hands[0].p-centre);reach.distance[1]=pm::length(frame.input.hands[0].p-mount[0]);
    reach.distance[2]=pm::length(frame.input.hands[0].p-mount[1]);
    if(exact)for(int n=0;n<e.count;++n)if(ph::holster_zone126(e.items[n].profile)==ph::HolsterZone126::RightHip45&&PhysDrawCandidate126(e,n)){
        reach.equipment=true;reach.key=e.items[n].profile.key;reach.generation=e.items[n].profile.generation;break;
    }
    // This selected-side chord alone owns RIGHT combat input. A LEFT fist
    // must not suppress an independent RIGHT Grip-only stock gesture.
    const bool boxing=s::QualifiedFistChordT25(i,0)&&!t::OwnAmmoClaim(0,i);
    const auto grant=t::Reach(reachState,reach,e.handKnown[0],!e.handKey[0],boxing);
    i.pouchNear[0]=exact&&grant.owner==kcd2vr::hip_t10::Owner::Dagger;
    i.pouchNear[1]=exact&&grant.owner==kcd2vr::hip_t10::Owner::Axe;
    for(int side=0;side<2;++side){
        const bool have=exact&&packet.valid[side]&&frame.input.tracked[side]&&frame.input.intentTracked[side]&&TrackingHandUsable111(side,now);
        if(have){const auto wrist=HandWorld6694(packet.allowed[side],packet.world);
            if(ph::finite(PhysPose6698(wrist))){std::copy_n(wrist.q,4,i.wrist18[side]);std::copy_n(wrist.t,3,i.wrist18[side]+4);
                i.pose18[side]=true;i.poseTick18[side]=packet.stamp.tick;}}
        for(int k=0;k<2;++k){
            const auto offset=frame.input.hands[side].p-mount[k];
            i.offset18[side][k][0]=float(offset.x);i.offset18[side][k][1]=float(offset.y);i.offset18[side][k][2]=float(offset.z);
            i.near18[side][k]=i.pose18[side]&&s::StockGestureAllowedT25(i,side)&&
                (i.chestKnivesT26?(side==1&&k==0&&e.handKnown[1]&&!e.handKey[1]&&
                    kcd2vr::inventory_lifecycle_t8::daggers.load()>0&&ph::knife_pouch_holds_r109(frame.input,frame.input.hands[side].p)):
                 (side==0?i.pouchNear[k]:e.handKnown[side]&&!e.handKey[side]&&pm::length(offset)<.18));
        }
        source.handKnown[side]=exact&&e.handKnown[side];source.handEmpty[side]=source.handKnown[side]&&!e.handKey[side];
    }
    if(i.chestKnivesT26)i.pouchNear[0]=i.pouchNear[1]=false; // no right-hip/axe supply route
    source.frame=i;source.complete=exact;
    (void)t::PublishSource(source);PhysSupplyAdmissionTickT23(source);
    (void)kcd2vr::action_time_owner59c11::PublishThrowingSupplyT2(i);
}
