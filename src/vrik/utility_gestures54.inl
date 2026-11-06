#include "native_torch62.inl"
// Existing native game-frame owner. Never execute Lua on the XR worker.
struct VisorRequest54 {std::uint64_t serial=0,tick=0,epoch=0;std::uintptr_t character=0;bool close=false;};
static kcd2vr::core::SnapshotMailbox<VisorRequest54> g_visor_request54;
static std::atomic<std::uint64_t> g_visor_serial54{0};
bool RequestVisor54(bool close,std::uint64_t tick) noexcept {
    VisorRequest54 q{};q.tick=tick;q.close=close;q.epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    q.character=static_cast<std::uintptr_t>(g_player_char37.load(std::memory_order_acquire));
    if(!q.tick||!q.epoch||!q.character||q.epoch!=g_pose_epoch6689.load(std::memory_order_acquire)||q.character!=g_player_char37.load(std::memory_order_acquire))return false;
    q.serial=g_visor_serial54.fetch_add(1,std::memory_order_relaxed)+1;return g_visor_request54.publish(q);
}
// Exact retail wrappers used by closeVisorOn/raiseVisorOn. Only the full
// this-only wrapper is invoked, on the existing native game-frame thread.
static bool VisorCall54(void* player,bool close) noexcept {
    namespace u=kcd2vr::utility54;const auto b=WhBase();
    if(!player||!retail_build112::Ready(b)||!PhysVft6698(player,u::kVisorPlayerVft54))return false;
    __try {
        if(*reinterpret_cast<const std::uintptr_t*>(b+u::kVisorPlayerVft54+0x5B0)!=b+u::kVisorOpen54||
           *reinterpret_cast<const std::uintptr_t*>(b+u::kVisorPlayerVft54+0x5B8)!=b+u::kVisorClose54||
           std::memcmp(reinterpret_cast<const void*>(b+u::kVisorOpen54),u::kVisorOpenEntry54,sizeof(u::kVisorOpenEntry54))||
           std::memcmp(reinterpret_cast<const void*>(b+u::kVisorClose54),u::kVisorCloseEntry54,sizeof(u::kVisorCloseEntry54)))return false;
        InvalidateReadCache101R2();
        reinterpret_cast<void(__fastcall*)(void*)>(b+(close?u::kVisorClose54:u::kVisorOpen54))(player);
        InvalidateReadCache101R2();return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){InvalidateReadCache101R2();return false;}
}
static void UtilityNativeTick54(std::uint64_t now) noexcept {
    TorchNativeTick62(now);
    static std::uint64_t seen=0;VisorRequest54 q{};
    if(!g_visor_request54.read(q)||!q.serial||q.serial==seen)return;
    seen=q.serial; // refused/stale requests cannot replay after a menu or save load
    const bool scope=q.tick<=now&&now-q.tick<=120&&q.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        q.character==static_cast<std::uintptr_t>(g_player_char37.load(std::memory_order_acquire))&&
        CalibrationReady()&&!g_ctx_menu.load()&&!g_ctx_cutscene.load()&&!g_ctx_dialogue.load()&&!g_ctx_scripted.load()&&
        !animation_filter::nativeBodyOwned112.load()&&TrackingHeadUsable111()&&TrackingHandUsable111(1,now);
    // Revalidate on the native consumer too: the empty native off-hand slot
    // can become a committed physical support after the gesture was queued.
    TrackingSnapshot current54{};
    if(!scope||!ReadTrackingSnapshot(current54)||!current54.allowed||!current54.left_valid||
       now<current54.tick||now-current54.tick>80||!std::isfinite(current54.left_grip)||current54.left_grip>=.25f||
       !retail_build112::Ready(WhBase())||ExactHandItemState(1)!=0||ReadPhysicalHandHeld55(1,now)!=0)return;
    void* ci=HenryCharacter59c15();void* entity=g_last_player_entity.load(std::memory_order_acquire);std::uint32_t id=0;
    if(!ci||reinterpret_cast<std::uintptr_t>(ci)!=q.character||!PlayerIdentityAccepts37(ci,now)||
       !entity||!PhysVft6698(entity,kCEntityVftRva)||!Read(static_cast<unsigned char*>(entity)+0x0C,id)||!id)return;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);void* player=nullptr;NativeOwner104 owner{};
    if(!PhysVft6698(manager,0x3EC9958)||!Read(static_cast<unsigned char*>(manager)+0x38,player)||
       !NativeResolve104(player,owner)||owner.entity!=entity||owner.epoch!=q.epoch)return;
    const bool returned=VisorCall54(owner.player,q.close);char line[210]{};
    if(!returned)(void)kcd2vr::visible_failure::publish("visor: game visor call refused (retail pins did not match)",now); // D38
    std::snprintf(line,sizeof(line),"C54 VISOR54 direction=%s entity=%u nativeMethodReturned=%d nativeOutcome=UNOBSERVED",q.close?"CLOSE":"OPEN",id,returned?1:0);
    diagnostic_trace::Write(line);
}
int UtilityLeftItem54() noexcept {
    PhysEquipment6698 e{};const auto now=std::uint64_t(GetTickCount64());
    if(g_weapon_control_mode.load()<4)return ExactHandItemState(1)==0?0:-1;
    if(!g_phys_equipment6698.read(e)||!e.valid||!e.handKnown[1]||!e.tick||now<e.tick||now-e.tick>80||
       e.epoch!=g_pose_epoch6689.load()||e.request!=g_weapon_request_epoch6694.load()||
       !e.manager||e.manager!=g_henry_item_attach_manager.load())return -1;
    if(!e.handKey[1])return 0;
    const int held=e.heldIndex[1];
    const bool torch=held>=0&&held<e.count&&e.count<=int(e.items.size())&&e.items[held].held&&
        e.items[held].profile.key==e.handKey[1]&&e.items[held].profile.torch40();
    if(torch)g_torch_seen_r114.store(now,std::memory_order_release);
    return torch?11:-1;
}
// R114: the equipment copy is only accepted for 80 ms, so one late copy at the trigger press turned a held torch
// into "unknown" and the stow press was dropped for the whole hold. a torch read as held in the last 400 ms still
// counts as held while the copy is unknown (-1). a KNOWN empty hand (0) always wins at once.
int UtilityLeftItemR114(std::uint64_t now) noexcept {
    const int held=UtilityLeftItem54();if(held!=-1)return held;
    const auto seen=g_torch_seen_r114.load(std::memory_order_acquire);
    return seen&&now>=seen&&now-seen<=400&&ExactHandItemState(1)!=0?11:-1;
}
