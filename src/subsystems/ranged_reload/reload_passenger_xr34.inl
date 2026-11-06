// D66C heavy crank chest gate: which tracked hand is in the head-relative chest box of
// THIS sample's tracking snapshot (head/right/left in one Y-up frame, metres). Shared by
// RangedSourceXR34 (before the lease) and CompletedReloadPoseSourceXR34 (passenger).
template<class Snapshot> static void ChestFlagsD66C(xcXR34::Source& out,const Snapshot& t) noexcept {
    out.chestKnown=false;out.handInChest[0]=out.handInChest[1]=false;
    out.quiverKnownD72=false;out.handInQuiverD72[0]=out.handInQuiverD72[1]=false;
    if(!t.allowed||!t.head_valid||!t.right_valid||!t.left_valid||t.head_held||t.right_held||t.left_held)return;
    const xcXR34::Vec head{t.head.x,t.head.y,t.head.z},hands[2]{{t.right.x,t.right.y,t.right.z},{t.left.x,t.left.y,t.left.z}};
    if(!xcXR34::finite(head)||!xcXR34::finite(hands[0])||!xcXR34::finite(hands[1]))return;
    for(int s=0;s<2;++s)out.handInChest[s]=xcXR34::handInChestD66C(head,hands[s]);
    out.chestKnown=true;
    // Same tracked sample, head-relative right shoulder quiver. This is an
    // input region; it does not manufacture ammunition or imply native stock.
    const float q[4]{t.hq.x,t.hq.y,t.hq.z,t.hq.w};
    double norm=0;for(float v:q){if(!std::isfinite(v))return;norm+=double(v)*v;}if(norm<.99||norm>1.01)return;
    const float inverseQ[4]{-q[0],-q[1],-q[2],q[3]};
    const auto forward=xcXR34::rotateInverse(inverseQ,{0,0,-1});
    const double horizontal=std::sqrt(forward.x*forward.x+forward.z*forward.z);if(horizontal<.1)return;
    const xcXR34::Vec front{forward.x/horizontal,0,forward.z/horizontal},right{-front.z,0,front.x};
    const xcXR34::Vec anchor{head.x+right.x*.20-front.x*.10,head.y-.10,head.z+right.z*.20-front.z*.10};
    for(int side=0;side<2;++side)out.handInQuiverD72[side]=xcXR34::length(hands[side]-anchor)<=.24;
    // R95R1: the right reload hand uses the accepted native/manual bow quiver
    // region (staged_reload596c2). Yaw-relative metres; pitch/roll do not move it.
    // This is acquisition intent only, never proof of an acquired native bolt.
    const auto delta=hands[0]-head;
    const xcXR34::Vec relative{delta.x*right.x+delta.z*right.z,delta.y,
        -(delta.x*front.x+delta.z*front.z)};
    out.handInQuiverD72[0]=relative.x>.06f&&relative.x<.55f&&relative.y>-.35f&&relative.y<.25f&&
        relative.z>.06f&&relative.z<.55f;
    out.quiverKnownD72=true;
}
// Narrow presentation/source acquisition for an already installed Reloading
// child. General physics, animation and input ownership remain native.
struct ReloadEquipmentXR34 {xcXR34::Owner owner{};PhysEquipment6698 equipment{};bool valid=false;};
struct ReloadEquipmentSeedXR34 {xcXR34::Source source{};PhysEquipment6698 equipment{};bool valid=false;};
static core::SnapshotMailbox<ReloadEquipmentXR34> g_reload_equipment_xr34;
static core::SnapshotMailbox<ReloadEquipmentSeedXR34> g_reload_equipment_seed_xr34;
static void SeedReloadEquipmentXR34(const xcXR34::Source& source) noexcept {
    ReloadEquipmentSeedXR34 seed{};PhysEquipment6698 metadata{};const auto now=std::uint64_t(GetTickCount64());
    if(xcXR34::fresh(source,now)&&g_phys_equipment6698.read(metadata)&&metadata.valid&&metadata.tick&&
       now>=metadata.tick&&now-metadata.tick<=100&&metadata.player==reinterpret_cast<void*>(source.player)&&
       metadata.manager==reinterpret_cast<void*>(source.manager)&&metadata.request==source.request&&metadata.epoch==source.body){
        for(int i=0;i<metadata.count&&i<int(metadata.items.size());++i){const auto& item=metadata.items[i];
            if(item.profile.key==source.key&&item.profile.generation==source.generation&&item.profile.classId==source.classId&&
               item.item==reinterpret_cast<void*>(source.weaponItem)){seed.source=source;seed.equipment=metadata;seed.valid=true;break;}}
    }
    (void)g_reload_equipment_seed_xr34.publish(seed);
}

static bool ReloadPlayerCurrentXR34(const xcXR34::Source& s) noexcept {
    // Source.player is C_Player; g_last_player_entity is its distinct CEntity.
    // Start from the current manager before dereferencing the captured player.
    void* manager=g_henry_item_attach_manager.load();void* entity=g_last_player_entity.load();
    void* player=nullptr;void* back=nullptr;void* playerEntity=nullptr;unsigned actor=0;
    return manager==reinterpret_cast<void*>(s.manager)&&PhysVft6698(manager,0x3EC9958)&&
        Read(static_cast<unsigned char*>(manager)+0x38,player)&&player==reinterpret_cast<void*>(s.player)&&
        PhysVft6698(player,0x3AA4B98)&&Read(static_cast<unsigned char*>(player)+0x9D0,back)&&back==manager&&
        Read(static_cast<unsigned char*>(player)+0x38,playerEntity)&&playerEntity==entity&&PhysVft6698(entity,0x4072C78)&&
        Read(static_cast<unsigned char*>(player)+0x30,actor)&&actor==s.actor&&
        manager==g_henry_item_attach_manager.load()&&entity==g_last_player_entity.load();
}
static bool ReloadContextXR34(const xcXR34::Owner& o,std::uint64_t now) noexcept {
    const auto& s=o.source;
    TrackingSnapshot tracking{};
    // Snapshot first: a concurrent tracking publication may be newer than the
    // caller's clock. Validate its original age against time read AFTER it.
    if(!ReadTrackingSnapshot(tracking))return false;
    now=std::uint64_t(GetTickCount64());
    return xcXR34::Registered()&&
        retail_build112::Ready(WhBase())&&(s.classId==10||s.classId==14||s.classId==15)&&
        !diagnostics::lab::NativeReference()&&!g_ctx_menu.load()&&!g_ctx_dialogue.load()&&
        !g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&!g_ctx_reading.load()&&
        g_gameplay_allowed.load()&&!NativePassengerCurrentA34(now)&&
        ReloadPlayerCurrentXR34(s)&&
        s.character==g_player_char37.load()&&s.manager==reinterpret_cast<std::uintptr_t>(g_henry_item_attach_manager.load())&&
        s.body==g_pose_epoch6689.load()&&s.request==g_weapon_request_epoch6694.load()&&
        s.tracking==kcd2vr::tracking59c6::epoch.load()&&
        g_head_valid.load()&&g_left_valid.load()&&g_right_valid.load()&&
        tracking.allowed&&tracking.sequence&&tracking.source_epoch==s.tracking&&tracking.tick&&now>=tracking.tick&&now-tracking.tick<=80&&
        tracking.head_valid&&tracking.right_valid&&tracking.left_valid&&!tracking.head_held&&!tracking.right_held&&!tracking.left_held;
}
static bool ReloadNativeIdentityXR34(const xcXR34::Owner& o,bool installed) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!ReloadContextXR34(o,now))return false;
    const xcXR34::Ops io{&RangedReadXR34};std::uintptr_t root=0;
    const auto member=xcXR34::membership(io,WhBase(),o,&root);
    if(member!=xcXR34::Member::Root&&(installed||member!=xcXR34::Member::Queued))return false;
    const auto& s=o.source;void* main=nullptr;void* parent=nullptr;void* child=nullptr;void* context=nullptr;
    void* character=nullptr;void* chars=nullptr;void* equipment=ManualEquipmentPtr59c9(reinterpret_cast<void*>(s.player));
    void* item=nullptr;void* ammo=nullptr;void* managerPlayer=nullptr;void* playerManager=nullptr;
    unsigned token=0,actor=0;int status=-1;std::uint64_t key=0;
    // Same exact read-only definition/class schema as PhysDescribeItem54.
    // A native conversion cannot relabel an old class recipe under one WUID.
    void* definition=nullptr;void* kind=nullptr;int classId=-1;unsigned char right=2;
    if(!Read(reinterpret_cast<unsigned char*>(s.weaponItem)+0x48,definition)||
       (!PhysVft6698(definition,0x3A4E390)&&!PhysVft6698(definition,0x3A4D890))||
       !Read(static_cast<unsigned char*>(definition)+0xE8,kind)||!kind||
       !Read(static_cast<unsigned char*>(kind)+8,classId)||classId!=s.classId||
       !Read(static_cast<unsigned char*>(kind)+0x25,right)||right>1||(right?0:1)!=s.primary)return false;
    return equipment==reinterpret_cast<void*>(s.equipment)&&ManualPlayer596(reinterpret_cast<void*>(s.player))&&
        Read(reinterpret_cast<unsigned char*>(s.player)+0x30,actor)&&actor==s.actor&&
        Read(reinterpret_cast<unsigned char*>(s.manager)+0x38,managerPlayer)&&reinterpret_cast<std::uintptr_t>(managerPlayer)==s.player&&
        Read(reinterpret_cast<unsigned char*>(s.player)+0x9D0,playerManager)&&reinterpret_cast<std::uintptr_t>(playerManager)==s.manager&&
        NativeHandKey6697(reinterpret_cast<void*>(s.manager),s.primary,key)&&key==s.key&&
        Read(static_cast<unsigned char*>(equipment)+0x40,item)&&reinterpret_cast<std::uintptr_t>(item)==s.weaponItem&&
        PhysVft6698(item,0x3A2DA88)&&Read(static_cast<unsigned char*>(item)+0x30,key)&&key==s.key&&
        Read(static_cast<unsigned char*>(equipment)+0x48,ammo)&&reinterpret_cast<std::uintptr_t>(ammo)==s.ammoItem&&
        PhysVft6698(ammo,0x3A2DA88)&&Read(static_cast<unsigned char*>(ammo)+0x30,key)&&key==s.ammoKey&&
        Read(static_cast<unsigned char*>(equipment)+0x50,main)&&reinterpret_cast<std::uintptr_t>(main)==o.main&&PhysVft6698(main,0x3E7B1B0)&&
        Read(static_cast<unsigned char*>(main)+0x98,parent)&&reinterpret_cast<std::uintptr_t>(parent)==o.parent&&PhysVft6698(parent,0x3E78CF8)&&
        Read(static_cast<unsigned char*>(parent)+0x80,main)&&reinterpret_cast<std::uintptr_t>(main)==o.main&&
        Read(static_cast<unsigned char*>(parent)+0x70,child)&&reinterpret_cast<std::uintptr_t>(child)==o.child&&PhysVft6698(child,0x3A450B0)&&
        Read(static_cast<unsigned char*>(child)+0x54,token)&&token==o.token&&
        Read(static_cast<unsigned char*>(child)+8,context)&&reinterpret_cast<std::uintptr_t>(context)==o.context&&
        Read(static_cast<unsigned char*>(child)+0x28,status)&&status==(member==xcXR34::Member::Root?2:1)&&
        Read(reinterpret_cast<unsigned char*>(o.controller)+0x20,context)&&reinterpret_cast<std::uintptr_t>(context)==o.context&&
        Read(reinterpret_cast<unsigned char*>(o.controller)+0x28,chars)&&chars&&
        Read(static_cast<unsigned char*>(chars)+0x10,character)&&reinterpret_cast<std::uintptr_t>(character)==s.character&&
        Read(static_cast<unsigned char*>(child)+0x30,context)&&(member!=xcXR34::Member::Root||reinterpret_cast<std::uintptr_t>(context)==root)&&
        Read(reinterpret_cast<unsigned char*>(g_henry_char_attachment_manager.load())+0x70,character)&&reinterpret_cast<std::uintptr_t>(character)==s.character;
}
static bool ReloadNativeIdentityReaderXR34(const xcXR34::Owner& o) noexcept {return ReloadNativeIdentityXR34(o,false);}
static bool ReloadNativeOwnerXR34(const xcXR34::Owner& o,bool installed) noexcept {
    xcXR34::Status state{};return xcXR34::ReadStatus(state)&&state.clockOwned&&
        xcXR34::sameOwner(o,state.owner)&&ReloadNativeIdentityXR34(o,installed);
}
static void RangedReloadEquipmentXR34() noexcept {
    ReloadEquipmentXR34 next{},old{};xcXR34::Status state{};const auto now=std::uint64_t(GetTickCount64());
    if(xcXR34::ReadStatus(state)&&state.clockOwned&&ReloadNativeOwnerXR34(state.owner,false)){
        PhysLive103 live{};PhysEquipment6698 metadata{};ReloadEquipmentSeedXR34 seed{};
        const bool retained=g_reload_equipment_xr34.read(old)&&old.valid&&xcXR34::sameOwner(old.owner,state.owner);
        if(retained)metadata=old.equipment;
        else if(g_reload_equipment_seed_xr34.read(seed)&&seed.valid&&xcXR34::sameSource(seed.source,state.owner.source))metadata=seed.equipment;
        else if(!g_phys_equipment6698.read(metadata)||!metadata.valid||now<metadata.tick||now-metadata.tick>100)metadata={};
        const auto& source=state.owner.source;
        if(metadata.valid&&metadata.player==reinterpret_cast<void*>(source.player)&&metadata.manager==reinterpret_cast<void*>(source.manager)&&
           metadata.request==source.request&&metadata.epoch==source.body&&PhysReadLive103(live)&&
           live.player==metadata.player&&live.manager==metadata.manager&&live.request==source.request&&live.epoch==source.body&&
           live.hand[source.primary]==source.key){
            bool itemCurrent=false;
            for(int i=0;i<metadata.count&&i<int(metadata.items.size());++i){const auto& item=metadata.items[i];
                if(item.profile.key==source.key&&item.profile.generation==source.generation&&item.profile.classId==source.classId&&
                   item.item==reinterpret_cast<void*>(source.weaponItem)&&PhysCachedItemMatches54(item,source.key))itemCurrent=true;}
            if(itemCurrent){next.owner=state.owner;next.equipment=metadata;next.equipment.tick=now;
                next.equipment.handKey[0]=live.hand[0];next.equipment.handKey[1]=live.hand[1];next.valid=true;}
        }
    }
    // This tick certifies a new native live-equipment read, never old pose age.
    (void)g_reload_equipment_xr34.publish(next);
}
static bool ReloadEquipmentCurrentXR34(const xcXR34::Owner& o,ReloadEquipmentXR34& out,std::uint64_t now) noexcept {
    return g_reload_equipment_xr34.read(out)&&out.valid&&xcXR34::sameOwner(out.owner,o)&&
        out.equipment.tick&&now>=out.equipment.tick&&now-out.equipment.tick<=80&&ReloadNativeOwnerXR34(o,true);
}
static bool RangedReloadSourceXR34(xcXR34::Source& out) noexcept {
    out=xcXR34::Source{};xcXR34::Source s{};xcXR34::Status state{};ReloadEquipmentXR34 equipment{};CommittedFrame6695 packet{};
    const auto now=std::uint64_t(GetTickCount64());
    // The source and wrists are one immutable completed packet. A separate
    // mailbox could expose a new body with an old/empty reload source.
    if(!ReadCommittedPacket6695(packet))return false;
    s=packet.reloadSourceXR34;
    if(!xcXR34::fresh(s,now)||s.auxiliaryUnavailableR95||
       !packet.publicationR24||s.bodyPublicationR95!=packet.publicationR24||
       !kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(s))return false;
    if(!xcXR34::ReadStatus(state)||
       !xcXR34::sameSource(s,state.owner.source)||!ReloadEquipmentCurrentXR34(state.owner,equipment,now)||
       packet.stamp.actor!=s.character||packet.stamp.epoch!=s.body||
       packet.stamp.sample!=s.sample||packet.stamp.tick!=s.tick||
       std::memcmp(packet.world,s.world,sizeof(s.world)))return false;
    const int hand=1-s.primary;
    if(!packet.valid[hand]||std::memcmp(packet.native[hand].q,s.nativeWrist[hand].q,sizeof(s.nativeWrist[hand].q))||
       std::memcmp(packet.native[hand].t,s.nativeWrist[hand].p,sizeof(s.nativeWrist[hand].p))||
       std::memcmp(packet.allowed[hand].q,s.allowedWrist[hand].q,sizeof(s.allowedWrist[hand].q))||
       std::memcmp(packet.allowed[hand].t,s.allowedWrist[hand].p,sizeof(s.allowedWrist[hand].p)))return false;
    if(xcXR34::handDrivenD81(s)){
        if(!s.intentKnownD78)return false;
        for(int side=0;side<2;++side)if(!packet.propRequestedValid5916c1[side]||
            std::memcmp(packet.propRequested5916c1[side].q,s.intentWristD78[side].q,sizeof(s.intentWristD78[side].q))||
            std::memcmp(packet.propRequested5916c1[side].t,s.intentWristD78[side].p,sizeof(s.intentWristD78[side].p)))return false;
    }
    if(!kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(s))return false;
    out=s;return true;
}
// One read-only validation of the exact post-controller native clip/clock.
// A mechanism observer may use it without inheriting Henry wrist ownership.
static bool ReloadNativeClockCurrentD86(const xcXR34::Passenger& p,std::uintptr_t character,bool presentationTransitionR95=false) noexcept {
    std::uintptr_t root=0,entry=0,context=0,characterNow=0;unsigned layer=0,layers=0;int status=-1;
    const xcXR34::Ops io{&RangedReadXR34};
    if(xcXR34::membership(io,WhBase(),p.owner,&root)!=xcXR34::Member::Root||root!=p.root||
       !Read(reinterpret_cast<unsigned char*>(p.owner.child)+0x28,status)||status!=2||
       !Read(reinterpret_cast<unsigned char*>(root)+0x20,context)||
       !Read(reinterpret_cast<unsigned char*>(context)+0x10,characterNow)||characterNow!=character||
       !Read(reinterpret_cast<unsigned char*>(root)+0x30,layer)||layer>8||
       !Read(reinterpret_cast<unsigned char*>(root)+0x34,layers)||
       !xcXR34::selectedScopeClockEntryD82(&RangedReadXR34,character,layer,layers,p.clip.id,entry,p.clip.nativeTransitionD80)||entry!=p.clock.entry||
       !animation_filter::ReloadPassengerOnlyXR34(reinterpret_cast<void*>(character+0x110),entry,p.clip.id,p.clip.nativeTransitionD80))return false;
    // Only an already-qualified, effect-free transition may advance while a
    // fresh tracked body is produced. This is presentation permission only:
    // mechanism readers keep exact equality and effectful clips stay exact.
    const bool advancing=presentationTransitionR95&&p.clip.nativeTransitionD80&&
        xcXR34::handDrivenD81(p.owner.source)&&!p.constraintD78.active&&
        !p.clip.cocking&&!p.clip.loading;
    // A real native spanner/lever may already be bound during this effect-free
    // transition. Its exact identity is still checked by the passenger consumer;
    // its mere presence cannot recreate the native-clock/body publication cycle.
    std::uintptr_t events=0,vft=0;unsigned eventCount=0;unsigned char segment=255;
    float normalized=-1,playback=0,active=-1,controllerScale=0,characterScale=0;
    if(p.clip.list!=p.clip.animation+0x48||
       !Read(reinterpret_cast<void*>(p.clip.list),vft)||vft!=WhBase()+0x3A7D9D0||
       !Read(reinterpret_cast<unsigned char*>(p.clip.list)+8,events)||events!=p.clip.entries||
       !Read(reinterpret_cast<unsigned char*>(events)-4,eventCount)||eventCount!=unsigned(p.clip.count)||
       !Read(reinterpret_cast<unsigned char*>(entry)+0x16,segment)||segment!=p.clock.segment||
       !Read(reinterpret_cast<unsigned char*>(entry)+0x44,normalized)||
        (advancing?(!std::isfinite(normalized)||normalized<p.clock.normalized||normalized>1.f):normalized!=p.clock.normalized)||
       !Read(reinterpret_cast<unsigned char*>(entry)+0x2C,playback)||playback!=p.clock.playbackScale||
       !Read(reinterpret_cast<unsigned char*>(p.owner.child)+0x10,active)||
        (advancing?(!std::isfinite(active)||active<p.clock.activeTime):active!=p.clock.activeTime)||
       !Read(reinterpret_cast<unsigned char*>(p.owner.controller)+0x9C,controllerScale)||controllerScale!=p.clock.controllerScale||
       !Read(reinterpret_cast<unsigned char*>(character)+0xA9C,characterScale)||characterScale!=p.clock.characterScale)return false;
    return true;
}
static bool ReloadPassengerCurrentXR34(const xcXR34::Passenger& p,std::uintptr_t character,std::uint64_t body,std::uint64_t now) noexcept {
    if(!kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(p.owner.source))return false;
    xcXR34::Passenger current{};ReloadEquipmentXR34 equipment{};
    if(p.bootstrap){xcXR34::Admission admission{};
        if(!xcXR34::ReadAdmission(admission,now))return false;
        current=xcXR34::PassengerFromAdmission(admission);
    }else if(!xcXR34::ReadPoseLeaseD87(current,now))return false;
    if(!xcXR34::sameOwner(current.owner,p.owner)||
       current.publication!=p.publication||current.frame.publication!=p.frame.publication||
       current.frame.sample!=p.frame.sample||current.frame.tick!=p.frame.tick||
       current.root!=p.root||current.clock.entry!=p.clock.entry||current.clip.id!=p.clip.id||
       character!=p.frame.character||body!=p.frame.body||!ReloadEquipmentCurrentXR34(p.owner,equipment,now))return false;
    if(!ReloadNativeClockCurrentD86(p,character,true))return false;
    const auto& b=p.binding;void* entity=nullptr;void* attachment=nullptr;int side=-1;
    if(b.toolBound){void* tool=nullptr;std::uint64_t key=0;std::uint32_t entityId=0;
        if(!Read(reinterpret_cast<unsigned char*>(p.owner.main)+0x80,tool)||reinterpret_cast<std::uintptr_t>(tool)!=b.toolItem||
           !Read(static_cast<unsigned char*>(tool)+0x30,key)||key!=b.toolKey||
           !RangedItemEntityXR34(tool,entityId,entity)||entityId!=b.toolId||reinterpret_cast<std::uintptr_t>(entity)!=b.toolEntity||
           !RangedBoundToolIdentityD72(p.owner,entityId,entity,side,attachment)||
           reinterpret_cast<std::uintptr_t>(attachment)!=b.toolAttachment||side!=p.workingHand)return false;
    }
    if(b.handBoltBound){unsigned idNow=0;
        if(b.handBoltPendingR95||!Read(reinterpret_cast<unsigned char*>(p.owner.parent)+0x88,idNow)||idNow!=b.boltId||b.boltKey!=p.frame.ammoKey||
           !(entity=EntityById6694(idNow))||reinterpret_cast<std::uintptr_t>(entity)!=b.boltEntity||
           !RangedBoundHandIdentityXR34(p.owner,idNow,entity,side,attachment)||
           reinterpret_cast<std::uintptr_t>(attachment)!=b.boltAttachment||side!=p.workingHand)return false;
    }
    // Before native Spawn/Ammo events, exact observed absence is a valid hand
    // presentation state. It grants no tool placement or native event credit.
    if(!b.toolBound){void* tool=nullptr;
        if(!Read(reinterpret_cast<unsigned char*>(p.owner.main)+0x80,tool)||b.toolId||b.toolEntity||b.toolAttachment)return false;
        if(b.toolPendingD72){void* pickable=nullptr;std::uint64_t key=0;
            if(!tool||reinterpret_cast<std::uintptr_t>(tool)!=b.toolItem||!PhysVft6698(tool,0x3A2DA88)||
               !Read(static_cast<unsigned char*>(tool)+0x30,key)||key!=b.toolKey||
               !Read(static_cast<unsigned char*>(tool)+0x88,pickable)||pickable)return false;
        }else if(tool||b.toolItem)return false;
    }
    if(!b.handBoltBound){unsigned idNow=~0u;
        if(!Read(reinterpret_cast<unsigned char*>(p.owner.parent)+0x88,idNow))return false;
        if(b.handBoltPendingR95){bool pending=false;
            // Native LoadReachedAmmo has supplied this exact entity, but the
            // right-hand slot has not finished binding. Keep fresh wrists only;
            // a complete binding needs a new native witness before placement.
            if(!idNow||idNow!=b.boltId||b.boltKey!=p.frame.ammoKey||!b.boltEntity||b.boltAttachment||
               !RangedReadBoltBindingR95(p.owner,idNow,entity,side,attachment,pending)||!pending||
               reinterpret_cast<std::uintptr_t>(entity)!=b.boltEntity||side!=p.workingHand||attachment)return false;
        }else if(idNow||b.boltId||b.boltKey||b.boltEntity||b.boltAttachment)return false;
    }
    if(!ReloadNativeOwnerXR34(p.owner,true)||p.publication!=xcXR34::passengerPublication.load(std::memory_order_acquire))return false;
    if(p.bootstrap){xcXR34::Admission after{};return xcXR34::ReadAdmission(after,now)&&after.publication==p.publication&&
        kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(p.owner.source);}
    xcXR34::Passenger after{};return xcXR34::ReadPoseLeaseD87(after,now)&&after.publication==p.publication&&
        kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(p.owner.source);
}
static bool ReloadMechanismCurrentD86(const xcXR34::Passenger& p,std::uint64_t now) noexcept {
    xcXR34::Passenger current{};
    if(!xcXR34::ReadMechanismWitnessD86(current,now)||!xcXR34::sameOwner(current.owner,p.owner)||
       current.publication!=p.publication||current.root!=p.root||current.clip.id!=p.clip.id||
       !xcXR34::sameMechanismClockD78(current.clock,p.clock)||
       !kcd2vr::reload_throw_claim_t25::workingHandUnclaimed(p.owner.source)||
       !ReloadNativeOwnerXR34(p.owner,true)||!ReloadNativeClockCurrentD86(p,p.frame.character))return false;
    xcXR34::Passenger after{};
    return xcXR34::ReadMechanismWitnessD86(after,now)&&after.publication==p.publication&&
        xcXR34::sameOwner(after.owner,p.owner)&&xcXR34::sameMechanismClockD78(after.clock,p.clock);
}
static bool ReadReloadPassengerXR34(xcXR34::Passenger& out,std::uintptr_t character,std::uint64_t body,std::uint64_t now) noexcept {
    out={};xcXR34::Passenger p{};
    if(!xcXR34::ReadPoseLeaseD87(p,now)){xcXR34::Admission admission{};
        if(!xcXR34::ReadAdmission(admission,now))return false;
        p=xcXR34::PassengerFromAdmission(admission);
    }
    if(!ReloadPassengerCurrentXR34(p,character,body,now))return false;
    out=p;return true;
}
static bool ReloadPoseWriterCurrentXR34(const xcXR34::Owner& o,std::uint64_t publication) noexcept {
    xcXR34::Passenger p{};return ReadReloadPassengerXR34(p,o.source.character,o.source.body,GetTickCount64())&&
        xcXR34::sameOwner(p.owner,o)&&(!publication||p.publication==publication);
}
// Called before the existing anatomical arm solve. Its output is a requested
// endpoint, never a bone write or a tracking replacement. Raw goals are const.
static bool ConstrainReloadWristD78(const xcXR34::Passenger& owner,const ph::Pose (&raw)[2],
    const bool (&valid)[2],const TrackingSnapshot& tracking,ph::Pose& out) noexcept {
    out={};const auto now=std::uint64_t(GetTickCount64());
    if(!owner.constraintD78.active||!tracking.allowed||!tracking.head_valid||!tracking.left_valid||!tracking.right_valid||
       tracking.head_held||tracking.left_held||tracking.right_held||
       !ReloadPassengerCurrentXR34(owner,owner.frame.character,owner.frame.body,now))return false;
    xcXR34::Passenger p=owner;
    if(p.constraintD78.mounted){xcXR34::Source source{};RangedPlacedXR34 placed{};
        if(!RangedSourceXR34(source)||!xcXR34::sameSource(source,p.frame)||
           !g_ranged_placed_xr34.read(placed)||!placed.valid||!xcXR34::sameOwner(placed.owner,p.owner)||
           placed.id!=p.binding.toolId||reinterpret_cast<std::uintptr_t>(placed.entity)!=p.binding.toolEntity||
           placed.toolKey!=p.binding.toolKey||placed.sample>source.sample||source.tick<placed.tick||
           source.tick-placed.tick>80||!RangedPlacementStillCurrentXR34(placed))return false;
        RangedMechanismSpatialD78(p.owner,source,p.binding,&placed);
    }else{xcXR34::Source source{};
        if(!RangedSourceXR34(source)||!xcXR34::sameSource(source,p.frame))return false;
        RangedMechanismSpatialD78(p.owner,source,p.binding,nullptr);
    }
    xcXR34::Wrist input[2]{},result{};
    for(int side=0;side<2;++side){if(!ph::finite(raw[side]))return false;
        input[side]={{float(raw[side].q.x),float(raw[side].q.y),float(raw[side].q.z),float(raw[side].q.w)},
            {float(raw[side].p.x),float(raw[side].p.y),float(raw[side].p.z)}};}
    if(!xcXR34::projectReloadWristD78(p,input,valid,tracking.sequence,tracking.tick,tracking.source_epoch,
       p.workingHand?tracking.left_grip:tracking.right_grip,result)||
       !ReloadPassengerCurrentXR34(owner,owner.frame.character,owner.frame.body,now))return false;
    out=RangedWristPoseD72(result);return ph::finite(out);
}
static xcXR34::Source CompletedReloadPoseSourceXR34(const xcXR34::Passenger& p,const CommittedFrame6695& packet,
    const QuatT28& weapon,const TrackingSnapshot& tracking,std::uint64_t now) noexcept {
    xcXR34::Source next{};
    if(p.workingHand<0||p.workingHand>1||p.frame.primary!=1-p.workingHand||packet.stamp.actor!=p.frame.character||packet.stamp.epoch!=p.frame.body||packet.stamp.sample!=tracking.sequence||
       packet.stamp.tick!=tracking.tick||tracking.source_epoch!=p.frame.tracking||!tracking.allowed||
       tracking.head_held||tracking.right_held||tracking.left_held||!packet.valid[p.workingHand]||!ValidPose6689(weapon))return next;
    next=p.owner.source;next.publication=0;next.sample=tracking.sequence;next.tick=tracking.tick;next.completedLease=p.owner.serial;
    next.nativePoseClockD78=p.clock.entry;next.nativePoseNormalizedD78=p.clock.normalized;next.nativePoseActiveTimeD78=p.clock.activeTime;
    ChestFlagsD66C(next,tracking);
    std::memcpy(next.world,packet.world,sizeof(next.world));std::memcpy(next.weaponModel.q,weapon.q,sizeof(weapon.q));
    std::memcpy(next.weaponModel.p,weapon.t,sizeof(weapon.t));
    next.nativeWeaponD72=next.weaponModel;
    if(xcXR34::handDrivenD81(p.owner.source)&&packet.valid[p.frame.primary]){
        const auto& g=p.owner.source.primaryGripD72;
        const ph::Pose grip{{g.q[0],g.q[1],g.q[2],g.q[3]},{g.p[0],g.p[1],g.p[2]}};
        const auto held=ph::compose(PhysPose6698(packet.allowed[p.frame.primary]),ph::inverse(grip));
        next.weaponModel={{float(held.q.x),float(held.q.y),float(held.q.z),float(held.q.w)},
            {float(held.p.x),float(held.p.y),float(held.p.z)}};
    }
    const auto& model=next.weaponModel;
    const auto inverse=ph::inverse(ph::Pose{{model.q[0],model.q[1],model.q[2],model.q[3]},
        {model.p[0],model.p[1],model.p[2]}});
    for(int side=0;side<2;++side){const auto& allowed=(side==p.workingHand||xcXR34::handDrivenD81(p.owner.source))?packet.allowed[side]:packet.native[side];
        std::memcpy(next.nativeWrist[side].q,packet.native[side].q,sizeof(packet.native[side].q));
        std::memcpy(next.nativeWrist[side].p,packet.native[side].t,sizeof(packet.native[side].t));
        std::memcpy(next.allowedWrist[side].q,allowed.q,sizeof(allowed.q));std::memcpy(next.allowedWrist[side].p,allowed.t,sizeof(allowed.t));
        const auto hand=ph::point(inverse,PhysPose6698(allowed).p);next.hand[side]={hand.x,hand.y,hand.z};
        next.grip[side]=side?tracking.left_grip:tracking.right_grip;}
    const auto grip=ph::point(inverse,PhysPose6698(packet.native[p.workingHand]).p);
    next.nativeGrip={grip.x,grip.y,grip.z};
    if(!RangedRawIntentD78(next,packet))return xcXR34::Source{};
    next.allowed=xcXR34::fresh(next,now);
    return next;
}
static bool PrepareReloadPoseExactR93R3(const xcXR34::Passenger& p,CommittedFrame6695& packet,
    const QuatT28* original,std::size_t count,const TrackingSnapshot& tracking) noexcept {
    xcXR34::Source next{};const auto now=std::uint64_t(GetTickCount64());
    if(!original||!ReloadPassengerCurrentXR34(p,packet.stamp.actor,packet.stamp.epoch,now)||
       packet.stamp.sample!=tracking.sequence||packet.stamp.tick!=tracking.tick||
       tracking.source_epoch!=p.frame.tracking||!tracking.allowed||tracking.head_held||tracking.right_held||tracking.left_held||
       !packet.valid[p.workingHand]){return false;}
    // Ordinary carried-item native attachment table, distinct from the ammo
    // table used by the working hand. No entity/descriptor is invented.
    void* model=nullptr;void* player=nullptr;const char* name=nullptr;void* attachment=nullptr;
    void* character=nullptr;void* manager=nullptr;std::uint32_t id=0;void* entity=nullptr;std::int16_t joint=-1;
    QuatT28 relative{},again{};unsigned char deferred=1;
    if(!PhysSocketContract101(WhBase())||!RangedItemEntityXR34(reinterpret_cast<void*>(p.frame.weaponItem),id,entity)||
       !Read(reinterpret_cast<unsigned char*>(p.frame.player)+0x1B0,model)||!model||
       !Read(static_cast<unsigned char*>(model)+0x20,player)||reinterpret_cast<std::uintptr_t>(player)!=p.frame.player||
       !Read(reinterpret_cast<void*>(WhBase()+0x40AC2A8+8*p.frame.primary),name)||!name||
       !(attachment=ManualFindHandAttachment19c4r4(model,name))||!ManualBoundEntity19c4r4(attachment,id,entity)||
       !Read(static_cast<unsigned char*>(attachment)+0x28,manager)||manager!=g_henry_char_attachment_manager.load()||
       !Read(static_cast<unsigned char*>(manager)+0x70,character)||reinterpret_cast<std::uintptr_t>(character)!=p.frame.character||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||std::size_t(joint)>=count||
       !Read(static_cast<unsigned char*>(attachment)+0x7A,deferred)||deferred||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,relative)||!ValidPose6689(relative)||!ValidPose6689(original[joint])){
        return false;}
    const auto weapon=AbsoluteFromRelative(original[joint],relative);
    if(id==p.binding.toolId||id==p.binding.boltId||!ValidPose6689(weapon)||!Read(static_cast<unsigned char*>(attachment)+0x9C,again)||
       std::memcmp(&relative,&again,sizeof(relative))||!ReloadPassengerCurrentXR34(p,packet.stamp.actor,packet.stamp.epoch,now)){
        return false;}
    next=CompletedReloadPoseSourceXR34(p,packet,weapon,tracking,now);
    if(!next.allowed)return false;
    next.auxiliaryUnavailableR95=false;next.auxiliaryReleasedR95=false;
    packet.reloadSourceXR34=next;
    return true;
}

// A post-controller callback may republish the same paused native pose while
// this body is being prepared. Retry only that exact geometry/clock/ownership;
// a progressed clock or changed tool/bolt needs its own native pose callback.
static bool SameReloadPoseRetryR93R3(const xcXR34::Passenger& before,const xcXR34::Passenger& after) noexcept {
    return before.publication!=after.publication&&xcXR34::sameOwner(before.owner,after.owner)&&
        xcXR34::sameSource(before.frame,after.frame)&&before.frame.sample==after.frame.sample&&before.frame.tick==after.frame.tick&&
        before.root==after.root&&before.childStatus==after.childStatus&&before.workingHand==after.workingHand&&
        before.bootstrap==after.bootstrap&&before.valid&&after.valid&&
        !std::memcmp(&before.clip,&after.clip,sizeof(before.clip))&&
        !std::memcmp(&before.clock,&after.clock,sizeof(before.clock))&&
        !std::memcmp(&before.binding,&after.binding,sizeof(before.binding))&&
        !std::memcmp(&before.constraintD78,&after.constraintD78,sizeof(before.constraintD78));
}
static bool PrepareReloadPoseXR34(xcXR34::Passenger& p,CommittedFrame6695& packet,
    const QuatT28* original,std::size_t count,const TrackingSnapshot& tracking) noexcept {
    const xcXR34::Source emptySourceR95{};packet.reloadSourceXR34=emptySourceR95;
    if(PrepareReloadPoseExactR93R3(p,packet,original,count,tracking))return true;
    xcXR34::Passenger current{};
    if(!ReadReloadPassengerXR34(current,packet.stamp.actor,packet.stamp.epoch,GetTickCount64())||
       !SameReloadPoseRetryR93R3(p,current))return false;
    if(!PrepareReloadPoseExactR93R3(current,packet,original,count,tracking))return false;
    p=current; // carry the qualified identical-pose receipt into final owner checks
    return true;
}
