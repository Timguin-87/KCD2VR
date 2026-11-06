// Current Henry identity from the existing XR34 manager/player/entity chain.
// Only read-only source joining: no item/actor lookup or native operation added.
struct HenryItemContextH41 {std::uintptr_t manager=0,player=0,actor=0;std::uint32_t id=0;};
static bool ReadHenryItemContextH41(HenryItemContextH41& out) noexcept {
    out={};const auto b=WhBase();if(!b||!retail_build112::Ready(b))return false;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    void* actor=g_last_player_entity.load(std::memory_order_acquire);void* player=nullptr;void* back=nullptr;void* entity=nullptr;
    std::uint32_t actorId=0,playerId=0;
    if(!manager||!actor||!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !PhysVft6698(player,0x3AA4B98)||!Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager||
       !Read(static_cast<unsigned char*>(player)+0x38,entity)||entity!=actor||!PhysVft6698(actor,0x4072C78)||
       !Read(static_cast<unsigned char*>(actor)+0x0C,actorId)||!actorId||
       !Read(static_cast<unsigned char*>(player)+0x30,playerId)||playerId!=actorId||
       manager!=g_henry_item_attach_manager.load(std::memory_order_acquire)||actor!=g_last_player_entity.load(std::memory_order_acquire))return false;
    out={reinterpret_cast<std::uintptr_t>(manager),reinterpret_cast<std::uintptr_t>(player),reinterpret_cast<std::uintptr_t>(actor),actorId};return true;
}
static bool AlchemyFinishCurrentH41(kcd2vr::alchemy_finish_cb1::Context& out) noexcept {
    out={};namespace a=kcd2vr::activity_input163;auto& w=a::witness175;
    if(!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire))return false;
    const auto lifetime=w.lifetime175.load(std::memory_order_acquire);
    const auto instance=w.active[unsigned(a::Kind175::Alchemy)].load(std::memory_order_acquire);
    HenryItemContextH41 h{};std::uintptr_t player=0,soul=0;unsigned char running=0;
    if(!instance||!ReadHenryItemContextH41(h)||!PhysVft6698(reinterpret_cast<void*>(instance),0x3F60B08)||
       !Read(reinterpret_cast<void*>(instance+0x18),player)||player!=h.player||
       !Read(reinterpret_cast<void*>(instance+0x2C),running)||running!=1||
       !Read(reinterpret_cast<void*>(h.player+0x668),soul)||!soul)return false;
    // +668 is the original soul argument loaded by the pinned native alchemy
    // finish caller. It is compared with the captured original arg, never used
    // to call an inventory/effect or determine whether a brew may finish.
    HenryItemContextH41 after{};std::uintptr_t playerAgain=0,soulAgain=0;
    if(!Read(reinterpret_cast<void*>(instance+0x18),playerAgain)||playerAgain!=h.player||
       !Read(reinterpret_cast<void*>(instance+0x2C),running)||running!=1||!Read(reinterpret_cast<void*>(h.player+0x668),soulAgain)||soulAgain!=soul||
       !ReadHenryItemContextH41(after)||after.manager!=h.manager||after.player!=h.player||after.actor!=h.actor||after.id!=h.id||
       !w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
       w.lifetime175.load(std::memory_order_acquire)!=lifetime||w.active[unsigned(a::Kind175::Alchemy)].load(std::memory_order_acquire)!=instance)return false;
    out={instance,h.player,h.actor,soul,lifetime};return true;
}
static bool GripStowCurrentH41(const kcd2vr::physics::grab_producer60::Owner& o,std::uint64_t now,kcd2vr::grip_stow_cb1::Identity& out) noexcept {
    out={};if(o.side<0||o.side>1||!now||!g_gameplay_allowed.load(std::memory_order_acquire)||g_cal_state.load()!=2||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||g_ctx_scripted.load())return false;
    HenryItemContextH41 h{};CommittedFrame6695 p{};TrackingSnapshot t{};
    const auto body=g_pose_epoch6689.load(std::memory_order_acquire),request=g_weapon_request_epoch6694.load(std::memory_order_acquire),policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const auto character=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire));
    if(!ReadHenryItemContextH41(h)||h.actor!=o.player||!character||character!=g_player_char37.load(std::memory_order_acquire)||
       !ReadCommittedPacket6695(p)||!p.valid[o.side]||p.stamp.actor!=character||p.stamp.epoch!=body||body!=o.poseEpoch||
       !p.stamp.tick||now<p.stamp.tick||now-p.stamp.tick>80||p.doorPlayer38!=h.actor||p.doorTrackingEpoch66!=o.trackingEpoch||
       !TrackingHandUsable111(o.side,now)||!ReadTrackingSnapshot(t)||!t.allowed||!t.head_valid||t.head_held||
       (o.side?(!t.left_valid||t.left_held):(!t.right_valid||t.right_held))||
       !t.tick||now<t.tick||now-t.tick>80||t.source_epoch!=o.trackingEpoch||(policy&1)||
       t.source_epoch!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire))return false;
    HenryItemContextH41 after{};
    if(!ReadHenryItemContextH41(after)||after.manager!=h.manager||after.player!=h.player||after.actor!=h.actor||after.id!=h.id||
       body!=g_pose_epoch6689.load(std::memory_order_acquire)||request!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       policy!=g_pose_policy_generation57.load(std::memory_order_acquire)||character!=g_player_char37.load(std::memory_order_acquire)||
       character!=reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))||
       t.source_epoch!=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)||!g_gameplay_allowed.load()||
       g_ctx_menu.load()||g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_reading.load()||g_ctx_scripted.load())return false;
    out={h.id,h.actor,character,h.manager,h.player,body,t.source_epoch,request,policy};return true;
}
