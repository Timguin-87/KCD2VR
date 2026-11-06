// Exact retail camera consumer, not a global RPG parameter or projection patch.
// 10610B6 calls C_RPGCombat+0xA8; 10610B9..CE applies only its result to FOV.
// Recovered signature: RCX=RPGCombat, RDX=soul, R8=item key; float in XMM0.
using BowZoomFn121=float(__fastcall*)(void*,void*,std::uint64_t);
static BowZoomFn121 g_bow_zoom_original121=nullptr;
static std::optional<DetourModKit::hook::Hook> g_bow_zoom_hook121;
static std::atomic<std::uintptr_t> g_bow_zoom_contract121{0};
static float BowZoomResult121(std::uintptr_t caller,void* combat,void* soul,std::uint64_t key,float native) noexcept {
    const auto b=WhBase(),now=std::uint64_t(GetTickCount64());
    if(caller!=b+0x10610B9||!b||g_bow_zoom_contract121.load()!=b||!retail_build112::Ready(b)||
       !BowHandlingMode59c9(g_weapon_control_mode.load())||diagnostics::lab::NativeReference()||
       !g_phys_armed6698.load()||g_phys_fault6698.load()||!g_gameplay_allowed.load()||
       !g_head_valid.load()||
       g_ctx_dialogue.load()||g_ctx_cutscene.load()||g_ctx_menu.load()||g_ctx_scripted.load()||
       !std::isfinite(native)||native<0||native>4)return native;
    ReadScope101R2 reads;PhysFrame6698 f{};NativeOwner104 owner{};
    if(!PhysVft6698(combat,0x3AE4410)||!g_phys_frame6698.read(f)||!f.input.allowed||
       !f.input.sample||now<f.input.tick||now-f.input.tick>80||f.request!=g_weapon_request_epoch6694.load()||
       f.input.bodyEpoch!=g_pose_epoch6689.load()||
       !key||!NativeResolve104(f.player,owner)||!soul||soul!=owner.soul)return native;
    int zoomSide=-1;bool crossbow32=false;
    if(f.profiles[1].bow()&&f.profiles[1].primary&&f.profiles[1].key==key)zoomSide=1;
    else if(kcd2vr::ranged_options_xr32::crossbowNoZoom.load(std::memory_order_acquire)){
        for(int side=0;side<2;++side)if(kcd2vr::ranged_kind_xr32::crossbow(f.profiles[side].classId)&&f.profiles[side].primary&&f.profiles[side].key==key){
            if(zoomSide!=-1)return native;zoomSide=side;crossbow32=true;
        }
    }
    if(zoomSide<0)return native;
    // Native hand membership also covers the brief release-to-holster transition;
    // requiring visual grip ownership here would reintroduce zoom on release.
    void* manager=nullptr;void* back=nullptr;std::uint64_t held=0;
    if(!Read(static_cast<unsigned char*>(owner.player)+0x9D0,manager)||manager!=f.manager||
       manager!=g_henry_item_attach_manager.load()||!PhysVft6698(manager,0x3EC9958)||
       !Read(static_cast<unsigned char*>(manager)+0x38,back)||back!=owner.player||
       !NativeHandKey6697(manager,zoomSide,held)||held!=key)return native;
    static std::atomic<std::uint64_t> due121{0};auto due=due121.load();
    static std::atomic<std::uint64_t> crossbowLoggedKey32{0};
    const bool logCrossbow32=!crossbow32||crossbowLoggedKey32.load(std::memory_order_acquire)!=key;
    if(logCrossbow32&&now>=due&&due121.compare_exchange_strong(due,now+1000)){
        if(crossbow32)crossbowLoggedKey32.store(key,std::memory_order_release);
        char msg[224]{};std::snprintf(msg,sizeof(msg),crossbow32?"XR32 CROSSBOW_ZOOM caller=10610B9 owner=HENRY key=%016llX native=%.5f cameraValue=0 rules=UNCHANGED":"V66.121 BOW_ZOOM caller=10610B9 owner=HENRY key=%016llX native=%.5f cameraValue=0 rules=UNCHANGED",
            (unsigned long long)key,double(native));diagnostic_trace::Write(msg);
    }
    return 0.f; // FOV / (1 + zoom * blend) becomes FOV; native blend is untouched.
}
static float __fastcall BowZoomDetour121(void* combat,void* soul,std::uint64_t key) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const float native=g_bow_zoom_original121?g_bow_zoom_original121(combat,soul,key):0.f;
    return BowZoomResult121(caller,combat,soul,key,native);
}
#include "bow_zoom_contract121.inl"
static bool ArmBowZoom121() noexcept {
    const auto b=WhBase();if(b&&g_bow_zoom_contract121.load()==b)return true;
    if(g_bow_zoom_hook121||!BowZoomContract121(b))return false;
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto hook=inline_at(InlineRequest{"kcd2vr_bow_camera_zoom121",Target{DetourModKit::Address{b+0x2CEEAAC}},options},&BowZoomDetour121);
    if(!hook)return false;
    g_bow_zoom_hook121=std::move(*hook);g_bow_zoom_original121=g_bow_zoom_hook121->original<BowZoomFn121>();
    if(!g_bow_zoom_original121||!g_bow_zoom_hook121->enable())return false;
    g_bow_zoom_contract121.store(b,std::memory_order_release);
    diagnostic_trace::Write("V66.121 BOW_ZOOM ARMED camera-only caller=10610B9 F12=6/7 Henry soul+held key required crossbow=OPTIONAL_CLASS10_14_15");return true;
}
