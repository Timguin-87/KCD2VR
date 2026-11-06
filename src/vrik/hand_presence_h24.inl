namespace handPresenceH24=kcd2vr::physics::hand_proxy60;
namespace handPolicyH24=kcd2vr::native_hand_proxy_t4;
static std::atomic<bool> g_native_hand_physics_h24{false};
struct HandPresenceSourceH24 {handPolicyH24::Input input{};bool valid=false;};
static kcd2vr::core::SnapshotMailbox<HandPresenceSourceH24> g_hand_presence_source_h24[2];
static kcd2vr::core::SnapshotMailbox<kcd2vr::physics::dialogue_hand_h24::Witness> g_dialogue_hand_presence_h24;
static std::atomic<bool> g_dialogue_hand_presence_enabled_h24{false};
static bool HandPresenceContextH24(const handPolicyH24::Input& in) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(in.source.side<0||in.source.side>1)return false;
    kcd2vr::physics::hand_presence_h24::Context c{};
    c.enabled=g_native_hand_physics_h24.load(std::memory_order_acquire);
    c.epoch=g_pose_epoch6689.load(std::memory_order_acquire);c.character=g_player_char37.load(std::memory_order_acquire);
    c.entity=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire));
    c.cancel=g_pose_policy_generation57.load(std::memory_order_acquire);c.trackingEpoch=kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire);
    c.calibrated=g_cal_state.load(std::memory_order_acquire)==2;c.gameplay=g_gameplay_allowed.load(std::memory_order_acquire);
    c.menu=g_ctx_menu.load();c.dialogue=g_ctx_dialogue.load();c.cutscene=g_ctx_cutscene.load();c.reading=g_ctx_reading.load();c.scripted=g_ctx_scripted.load();
    kcd2vr::physics::dialogue_hand_h24::Witness dialogue{};
    c.dialoguePresence=c.dialogue&&g_dialogue_hand_presence_enabled_h24.load(std::memory_order_acquire)&&
        g_dialogue_hand_presence_h24.read(dialogue)&&kcd2vr::physics::dialogue_hand_h24::current(dialogue,in,c.cancel,now);
    // The native action is observed after Update, before the next skinning
    // callback mirrors it into nativeBodyOwned112. Protected hand motors must
    // revoke from the same copied current owner in that interval too.
    c.nativeBody=animation_filter::nativeBodyOwned112.load()||
        kcd2vr::native_passenger_a34::current(in.source.characterH24,in.source.sourceGeneration,now);
    c.nativeReference=diagnostics::lab::NativeReference();
    c.tracked=TrackingHeadUsable111(now)&&TrackingHandUsable111(in.source.side,now);
    c.nativeUseUntil=g_native_use_until107[in.source.side].load(std::memory_order_acquire);
    kcd2vr::physics::grab_producer60::Status grab{};
    c.grabBusy=kcd2vr::physics::grab_producer60::ReadStatus(in.source.side,grab)&&
        grab.state!=kcd2vr::physics::grab_producer60::State::Idle;
    return kcd2vr::physics::hand_presence_h24::current(in,now,c);
}
static bool HandPresenceCurrentH24(const handPolicyH24::Input& in) noexcept {
    HandPresenceSourceH24 current{};
    return in.source.side>=0&&in.source.side<2&&HandPresenceContextH24(in)&&
        g_hand_presence_source_h24[in.source.side].read(current)&&current.valid&&
        current.input.source==in.source&&current.input.playerEntityH24==in.playerEntityH24&&
        current.input.playerBodyH24==in.playerBodyH24&&current.input.playerBodyIdH24==in.playerBodyIdH24&&
        HandPresenceContextH24(current.input);
}
// This runs only in the existing qualified pose callback. Body identity is
// refreshed at 500ms, not polled by a worker; the physics phase revalidates it.
static bool HandPresenceIdentityH24(void* character,std::uint64_t epoch,handPolicyH24::Input& out) noexcept {
    static std::uintptr_t cachedEntity=0,cachedBody=0;static int cachedId=-1;
    static std::uint64_t cachedEpoch=0,next=0;
    const auto now=std::uint64_t(GetTickCount64());FistOwner101 owner{};
    if(!FistReadOwner101(owner)||!owner.empty)return false;
    auto entity=g_last_player_entity.load(std::memory_order_acquire);
    if(!entity)return false;
    if(cachedEpoch!=epoch||cachedEntity!=reinterpret_cast<std::uintptr_t>(entity)||now>=next){
        void* proxy=nullptr;void* body=nullptr;int id=-1;
        cachedEpoch=epoch;cachedEntity=reinterpret_cast<std::uintptr_t>(entity);next=now+500;
        cachedBody=0;cachedId=-1;
        if(NativeHeldBodyT4(entity,proxy,body,id,false)){cachedBody=reinterpret_cast<std::uintptr_t>(body);cachedId=id;}
    }
    if(!cachedBody||cachedId<0)return false;
    out.source.player=reinterpret_cast<std::uintptr_t>(owner.player);out.source.characterH24=reinterpret_cast<std::uintptr_t>(character);
    out.playerBodyH24=cachedBody;out.playerEntityH24=cachedEntity;out.playerBodyIdH24=cachedId;out.emptyBothHandsH24=true;
    return true;
}
