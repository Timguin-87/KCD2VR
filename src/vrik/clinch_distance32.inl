// C32: narrower initiation of the EXISTING native sync guard (clinch).
// Only72C97E native-true candidates are vetoed; active paired actions stay native.
static std::atomic<float> g_clinch_distance_meters32{.40f};
static void ClinchDistanceConfigure32(float meters) noexcept {
    g_clinch_distance_meters32.store(std::isfinite(meters)&&meters>=.05f&&meters<=2.f?meters:0.f,
        std::memory_order_release); // zero/invalid = native initiation distance
}
static bool ClinchEntityPosition32(void* entity,std::uint32_t& id,ph::Vec3& position) noexcept {
    // CEntity +100 getter1A74140 = lea[rcx+58];ret. ID getter1A76130
    // reads+C. Both exact leafs/vft entries pinned by ArmCloseCombat175.
    std::uintptr_t vf=0;Matrix34V6550 tm{};id=0;
    if(!entity||!Read(entity,vf)||vf!=WhBase()+0x4072C78||
       !Read(static_cast<unsigned char*>(entity)+0xC,id)||!id||
       !Read(static_cast<unsigned char*>(entity)+0x58,tm)||!FiniteTmV6552(tm))return false;
    position={tm.m[3],tm.m[7],tm.m[11]};return pm::finite(position);
}
// The committed ray belongs to this exact character/root. Root travel or
// turning after publication cannot turn an old hand contact into a new grab.
// Match the existing coherent_pose contact-world tolerances, not a new reach.
static bool ClinchRootCurrentD40(const CommittedFrame6695& frame) noexcept {
    const auto character=frame.stamp.actor;
    if(!character||character!=g_player_char37.load(std::memory_order_acquire)||
       character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))||
       character+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire)))return false;
    for(float value:frame.world)if(!std::isfinite(value))return false;
    ModelWorld6692 current{};
    if(!ReadModelWorld6692(reinterpret_cast<void*>(character),current))return false;
    return pm::length(pm::Vec3{current.t.x,current.t.y,current.t.z}-pm::Vec3{frame.world[4],frame.world[5],frame.world[6]})<=.002&&
        pm::angle(pm::Quat{current.q.x,current.q.y,current.q.z,current.q.w},pm::Quat{frame.world[0],frame.world[1],frame.world[2],frame.world[3]})<=.002&&
        std::abs(current.scale-frame.world[7])<1e-5;
}
static bool ClinchReadIntentD40(void* target,std::uint32_t entity,std::uint64_t now,
                              kcd2vr::clinch_intent_d40::Ticket& selected) noexcept {
    namespace intent=kcd2vr::clinch_intent_d40;
    const auto publication=ReadCompletedPublicationT23();
    CommittedFrame6695 frame{};TrackingSnapshot tracking{};
    if(!publication||(publication&1)||!ReadCommittedPacket6695(frame)||frame.publicationR24!=publication||
       !ClinchRootCurrentD40(frame)||!ReadTrackingSnapshot(tracking)||!tracking.allowed||!tracking.head_valid||tracking.head_held||
       !tracking.tick||now<tracking.tick||now-tracking.tick>80||
       !tracking.source_epoch||tracking.source_epoch!=frame.doorTrackingEpoch66||
       tracking.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire))return false;
    for(int side=0;side<2;++side){intent::Ticket t{};
        if(!intent::tickets[side].read(t)||!intent::current(t,now,frame.stamp.actor,frame.stamp.epoch,
            g_contact_control_epoch6695.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(target),entity,publication)||
           frame.stamp.sample!=t.sequence||frame.stamp.tick!=t.tick||!frame.valid[side]||
           frame.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
           frame.stamp.actor!=g_player_char37.load(std::memory_order_acquire)||
           frame.stamp.actor+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))||
           frame.stamp.epoch!=g_pose_epoch6689.load()||!TrackingHandUsable111(side,now)||
           (side?tracking.left_held:tracking.right_held)||ExactHandItemState(side)!=0||
           ReadPhysicalHandHeld55(side,now)!=0||now<g_native_use_until107[side].load())continue;
        const float grip=side?tracking.left_grip:tracking.right_grip;
        const float trigger=side?tracking.left_trigger:tracking.right_trigger;
        if(!std::isfinite(grip)||grip<.35f||!std::isfinite(trigger)||trigger>.55f||publication!=ReadCompletedPublicationT23())continue;
        // Re-read the pinned GetForeignData leaf's owner field; the borrowed
        // contact pointer is not a retained physics lease or a writable body.
        std::uintptr_t vf=0,foreign=0;void* bodyOwner=nullptr;
        if(!Read(reinterpret_cast<void*>(t.collider),vf)||vf<WhBase()||
           (npc172::classify(vf-WhBase())!=npc172::Body::Living&&npc172::classify(vf-WhBase())!=npc172::Body::Articulated)||
           !Read(reinterpret_cast<void*>(vf+0x50),foreign)||foreign!=WhBase()+0x5D6F44||
           !Read(reinterpret_cast<void*>(t.collider+0x20),bodyOwner)||bodyOwner!=target||
           !PhysEntityIdentity135(target,entity)||publication!=ReadCompletedPublicationT23()||
           frame.stamp.actor!=g_player_char37.load(std::memory_order_acquire)||
           frame.stamp.actor+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))||
           !ClinchRootCurrentD40(frame))continue;
        // The root/foreign-owner reads above can overlap a context or pose
        // takeover. Rejoin the original publication, epochs and live context
        // after the last native-memory leaf, before returning usable intent.
        const auto done=std::uint64_t(GetTickCount64());
        if(done<now||!intent::current(t,done,frame.stamp.actor,frame.stamp.epoch,
               g_contact_control_epoch6695.load(std::memory_order_acquire),reinterpret_cast<std::uintptr_t>(target),entity,publication)||
           publication!=ReadCompletedPublicationT23()||frame.stamp.epoch!=g_pose_epoch6689.load()||
           frame.stamp.actor!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())||
           frame.stamp.actor!=g_player_char37.load(std::memory_order_acquire)||
           frame.stamp.actor+8!=reinterpret_cast<std::uintptr_t>(g_henry_char_attachment_manager.load(std::memory_order_acquire))||
           tracking.source_epoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
           !tracking.tick||done<tracking.tick||done-tracking.tick>80||!TrackingHandUsable111(side,done)||
           !g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
           diagnostics::lab::NativeReference()||animation_filter::nativeBodyOwned112.load()||
           animation_filter::NativeKickActive175()||g_ctx_dialogue.load()||g_ctx_menu.load()||
           g_ctx_cutscene.load()||g_ctx_scripted.load())continue;
        selected=t;return true;
    }
    return false;
}
static bool ClinchDistanceReject32(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    namespace intent=kcd2vr::clinch_intent_d40;
    const int mode=intent::getMode();const auto policy=intent::generation.load(std::memory_order_acquire);
    const auto trackingPolicy=tracking59c6::epoch.load(std::memory_order_acquire);
    const bool trialD70=kcd2vr::claude_d70::UnarmedFix();
    const auto trialRevisionD70=kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire);
    float limit=g_clinch_distance_meters32.load(std::memory_order_acquire);
    bool distanceEnabled=limit>=.05f&&limit<=2.f;
    if(!g_close_combat_ready175.load(std::memory_order_acquire)||(!distanceEnabled&&mode==int(intent::Mode::Distance)&&!trialD70)||
       !(gpr(ctx,Gpr::Rax)&255)||F4Rung178r10()==0||!retail_build112::Ready(WhBase())||
       !g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load()||!g_left_valid.load()||
       diagnostics::lab::NativeReference()||animation_filter::nativeBodyOwned112.load()||
       animation_filter::NativeKickActive175()||g_ctx_dialogue.load()||g_ctx_menu.load()||
       g_ctx_cutscene.load()||g_ctx_scripted.load())return false;
    const auto now=std::uint64_t(GetTickCount64());void* player=nullptr;std::uintptr_t fistCharacter=0;
    std::uint64_t request=0,epoch=0,sample=0;bool fist=false;
    if(FistSelected101()&&FistModePermits178r22c2()){
        FistFrame101 f{};
        if(g_fist_fault101.load()||!g_fist_frame101.read(f)||!f.sample.valid||
           !f.sample.empty[0]||!f.sample.empty[1]||!f.sample.sequence||!f.sample.tick||
           now<f.sample.tick||now-f.sample.tick>80||f.request!=g_fist_epoch101.load()||
           f.sample.epoch!=g_pose_epoch6689.load())return false;
        player=f.player;request=f.request;epoch=f.sample.epoch;sample=f.sample.sequence;fistCharacter=f.sample.actor;fist=true;
        if(trialD70){limit=.30f;distanceEnabled=true;} // unarmed only, no saved setting mutation
        // The committed pose stamp owns a CCharInstance, NOT a CEntity.
        if(!fistCharacter||fistCharacter!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()))return false;
    }else{
        PhysFrame6698 f{};const auto ready=g_phys_ready_sample6698.load(std::memory_order_acquire);
        if(!ready||!g_phys_frame6698.read(f)||f.input.sample!=ready||
           ready!=g_phys_ready_sample6698.load(std::memory_order_acquire)||
           !f.input.allowed||!f.input.coherentMelee119||!f.input.tracked[0]||!f.input.tracked[1]||
           !f.output.active[0]||!f.output.owned[0]||!f.profiles[0].melee()||
           !f.input.tick||now<f.input.tick||now-f.input.tick>80||
           f.request!=g_weapon_request_epoch6694.load()||f.input.bodyEpoch!=g_pose_epoch6689.load())return false;
        player=f.player;request=f.request;epoch=f.input.bodyEpoch;sample=ready;
    }
    ReadScope101R2 reads;NativeOwner104 henry{};
    void *actor=nullptr,*state=nullptr,*opponent=nullptr,*opponentState=nullptr,*back=nullptr;
    void *owner=nullptr,*otherEntity=nullptr;
    const auto self=reinterpret_cast<const unsigned char*>(gpr(ctx,Gpr::Rbx));
    unsigned char inCombat=0,otherCombat=0;int syncStart=-1;
    if(!self||!player||!Read(self+8,actor)||!CurrentOpponent168(actor,state)||
       !Read(static_cast<unsigned char*>(actor)+0x3F0,back)||back!=self||
       !Read(static_cast<unsigned char*>(state)+0xCF8,opponent)||!opponent||opponent==actor||
       !CurrentOpponent168(opponent,opponentState)||
       !Read(static_cast<unsigned char*>(opponentState)+0xCF8,back)||back!=actor||
       !Read(static_cast<unsigned char*>(state)+8,inCombat)||inCombat!=1||
       !Read(static_cast<unsigned char*>(opponentState)+8,otherCombat)||otherCombat!=1||
       !Read(reinterpret_cast<void*>(WhBase()+0x53204E4),syncStart)||syncStart!=35||
       !NativeResolve104(player,henry)||henry.epoch!=epoch)return false;
    const bool enemyInitiatedD70=henry.actor==opponent&&henry.state==opponentState;
    if(henry.actor!=actor||henry.state!=state){
        if(!(trialD70&&fist&&enemyInitiatedD70))return false;
    }
    // D41: RBX+8 is this automatic component's initiating actor. The native
    // pair query asks its master row first (+937B35), then the opponent's slave
    // row. Player intent/distance settings may veto Henry's candidate only.
    // D70 opt-in unarmed trial additionally limits the reciprocal enemy candidate
    // to30cm. It never consumes Henry's pending physical grip ticket.
    void* other=enemyInitiatedD70?actor:opponent;
    if(!Read(static_cast<unsigned char*>(other)+0x1F8,owner)||!owner||
       !Read(static_cast<unsigned char*>(owner)+0x278,back)||back!=other||
       !Read(static_cast<unsigned char*>(owner)+0x38,otherEntity)||otherEntity==henry.entity)return false;
    std::uint32_t hid=0,oid=0;ph::Vec3 hp{},op{};
    if(!ClinchEntityPosition32(henry.entity,hid,hp)||!ClinchEntityPosition32(otherEntity,oid,op)||hid==oid)return false;
    const double distance=pm::length(hp-op);
    if(!std::isfinite(distance))return false;
    const bool distanceRejected=distanceEnabled&&distance>limit;
    intent::Ticket ticket{};
    const bool hasIntent=!enemyInitiatedD70&&mode==int(intent::Mode::ContactGrip)&&!distanceRejected&&
        ClinchReadIntentD40(otherEntity,oid,now,ticket);
    if((mode==int(intent::Mode::Distance)||enemyInitiatedD70)&&!distanceRejected)return false;
    // Revalidate the exact candidate after each intent read. A protected
    // owner/context takeover keeps the native result and consumes no ticket.
    const auto currentCandidate=[&]() noexcept {
        NativeOwner104 current{};void* currentBack=nullptr;
        if(!NativeResolve104(player,current)||current.entity!=henry.entity||current.actor!=henry.actor||
           current.state!=henry.state||current.epoch!=epoch||
           !Read(static_cast<unsigned char*>(state)+0xCF8,currentBack)||currentBack!=opponent||
           !Read(static_cast<unsigned char*>(opponentState)+0xCF8,currentBack)||currentBack!=actor||
           !Read(static_cast<unsigned char*>(owner)+0x38,currentBack)||currentBack!=otherEntity||
           !PhysEntityIdentity135(henry.entity,hid)||!PhysEntityIdentity135(otherEntity,oid)||
           (fist?(request!=g_fist_epoch101.load()||fistCharacter!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load())):
             (request!=g_weapon_request_epoch6694.load()||sample!=g_phys_ready_sample6698.load(std::memory_order_acquire))))return false;
        return trialRevisionD70==kcd2vr::claude_d70::combatRevision.load(std::memory_order_acquire)&&
            g_close_combat_ready175.load(std::memory_order_acquire)&&F4Rung178r10()!=0&&retail_build112::Ready(WhBase())&&
            g_gameplay_allowed.load()&&g_head_valid.load()&&g_right_valid.load()&&g_left_valid.load()&&
            !diagnostics::lab::NativeReference()&&!animation_filter::nativeBodyOwned112.load()&&!animation_filter::NativeKickActive175()&&
            !g_ctx_dialogue.load()&&!g_ctx_menu.load()&&!g_ctx_cutscene.load()&&!g_ctx_scripted.load()&&
            epoch==g_pose_epoch6689.load()&&policy==intent::generation.load(std::memory_order_acquire)&&mode==intent::getMode()&&
            (mode!=int(intent::Mode::ContactGrip)||trackingPolicy==tracking59c6::epoch.load(std::memory_order_acquire));
    };
    if(!currentCandidate())return false;
    if(!enemyInitiatedD70&&mode==int(intent::Mode::ContactGrip)&&hasIntent){
        intent::Ticket currentTicket{};
        const bool currentIntent=ClinchReadIntentD40(otherEntity,oid,std::uint64_t(GetTickCount64()),currentTicket);
        if(!currentCandidate())return false;
        if(currentIntent&&currentTicket.serial==ticket.serial&&
           intent::current(currentTicket,std::uint64_t(GetTickCount64()),g_player_char37.load(std::memory_order_acquire),
               g_pose_epoch6689.load(std::memory_order_acquire),g_contact_control_epoch6695.load(std::memory_order_acquire),
               reinterpret_cast<std::uintptr_t>(otherEntity),oid,ReadCompletedPublicationT23())&&
           intent::consume(currentTicket))return false;
    }
    return true; // distance, no current grip, consumed press, or automatic-clinch OFF: AL1 -> AL0 only
}
