// Source adapter for the new Ammo owner, not the old DropWeapon path. Include
// only after native_weapon_drop_t1.inl in action_time_owner59c11's namespace.
// T18 connects this through its finite native Ammo carrier/hand owner only.
// In particular, never pass the old equipped melee item as projectile payload.
using CreateAmmoFnT17=void*(__fastcall*)(void*,void*,void*,void*,bool,bool);
static CreateAmmoFnT17 g_create_ammo_original_t17=nullptr;
static std::optional<DetourModKit::hook::Hook> g_create_ammo_hook_t17;
struct AmmoLaunchCaptureT17 {
    void* manager=nullptr;void* player=nullptr;void* launcher=nullptr;void* ammo=nullptr;
    void* projectile=nullptr;unsigned calls=0;
    void* returnedState=nullptr;
    void (*returnedObserver)(void*,void*) noexcept=nullptr;
};
static thread_local AmmoLaunchCaptureT17* g_ammo_capture_t17=nullptr;
static void* __fastcall CreateAmmoDetourT17(void* manager,void* launcher,void* ammo,void* player,bool fired,bool transfer) noexcept {
    auto* capture=g_ammo_capture_t17;
    const auto caller=KCD2VR_GRASP_CALLER();
    const bool own=capture&&capture->manager==manager&&capture->player==player&&
        capture->launcher==launcher&&capture->ammo==ammo&&fired&&
        caller==WhBase()+0x289F821&&AmmoOwnPayloadT20(ammo);
    // A recovered dedicated payload can already be selected/restored in a
    // native ranged weapon before any throw/readiness scan. Refuse only the
    // three qualified ordinary preview/loaded-visual/shoot callers. Their
    // original NULL branches skip cloning, attachment and projectile launch.
    // Unknown callers and standard Ammo retain the complete original call.
    // T20's own unpaid preview calls the captured full original directly.
    const bool ordinary=caller==WhBase()+0x2895A5A||caller==WhBase()+0x2A8A083||caller==WhBase()+0x289F821;
    if(!own&&ordinary&&AmmoOwnPayloadT20(ammo))return nullptr;
    // Spend this original launch's factory admission BEFORE native code can
    // reenter. A failed/unknown original entry never grants a replay.
    if(own){if(capture->calls)return nullptr;++capture->calls;}
    // Dedicated Ammo is finite even in an archery quest with infinite arrows.
    // Request one REAL native transfer for our invocation only. Ordinary shots
    // retain their original transfer flag. No flags/counters are fabricated.
    void* result=g_create_ammo_original_t17?g_create_ammo_original_t17(manager,launcher,ammo,player,fired,own?true:transfer):nullptr;
    if(own){
        capture->projectile=result;
        // Optional observation of THIS original return, before native Shoot can
        // deliver contact or replace the payload. No observer runs for foreign,
        // ordinary or unknown calls; the one-entry budget is already spent.
        if(capture->returnedObserver)capture->returnedObserver(capture->returnedState,result);
    }
    return result;
}
static bool InstallAmmoLaunchT17() noexcept {
    if(g_create_ammo_hook_t17)return g_create_ammo_original_t17&&g_create_ammo_hook_t17->is_enabled();
    const auto b=WhBase();
    constexpr unsigned char create[]{0x48,0x89,0x5c,0x24,8,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7c,0x24,0x18,0x55};
    constexpr unsigned char shoot[]{0x48,0x8b,0xc4,0x48,0x89,0x58,8,0x48,0x89,0x68,0x10,0x48,0x89,0x70,0x18,0x57};
    constexpr unsigned char thunk[]{0xe9,0x23,0xbd,0xfd,0xff};
    constexpr unsigned char previewNull[]{0xe8,0x32,0x9a,0,0,0x48,0x85,0xc0,0x74,0x35};
    constexpr unsigned char shootNull[]{0xe8,0x6b,0xfc,0xff,0xff,0x48,0x8b,0xf8,0x48,0x85,0xc0,0x0f,0x84,0xb3,1,0,0};
    constexpr unsigned char loadedNull[]{0xe8,9,0x54,0xe1,0xff,0x48,0x8b,0xf8,0x48,0x85,0xc0,0x74,0x22};
    if(!retail_build112::Ready(b)||!MatchBytes(b+0x289F48C,create,sizeof(create))||
       !ShootPrologueMatchesD65C(shoot,sizeof(shoot))||!MatchBytes(b+0x2ACC880,thunk,sizeof(thunk))||
       !MatchBytes(b+0x2895A55,previewNull,sizeof(previewNull))||
       !MatchBytes(b+0x289F81C,shootNull,sizeof(shootNull))||!MatchBytes(b+0x2A8A07E,loadedNull,sizeof(loadedNull))||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3ECE6F0),0x118,0)!=b+0x2ACC880)return false;
    DetourModKit::hook::InlineRequest req{"kcd2vr_t17_native_ammo_factory",DetourModKit::hook::Target{DetourModKit::Address{b+0x289F48C}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(req),&CreateAmmoDetourT17);if(!hook)return false;
    g_create_ammo_hook_t17=std::move(*hook);g_create_ammo_original_t17=g_create_ammo_hook_t17->original<CreateAmmoFnT17>();
    return g_create_ammo_original_t17&&PinOwnCallbackT2(reinterpret_cast<const void*>(&CreateAmmoDetourT17))&&g_create_ammo_hook_t17->enable();
}
struct PreparedAmmoT17 {
    wrt1::Request release{};
    std::uint64_t launcher=0;
    // These must be joined by the Ammo carrier's native hand/entity/attachment
    // receipt, not a bool inferred from a Lua function returning successfully.
    std::uint64_t heldSerial=0;
    bool (*currentHand)(const PreparedAmmoT17&) noexcept=nullptr;
};
struct AmmoLaunchResultT17 {
    bool invoked=false,nativeReturned=false,unitTransferred=false;
    void* projectile=nullptr;std::uint32_t entity=0;std::uint64_t payloadKey=0;
    // Passing the launch request is not proof a later collision inflicted damage.
};
static bool AmmoKindsT17(void* launcher,void* ammo) noexcept {
    if(!Vft13(launcher,0x3A2DA88)||!Vft13(ammo,0x3A2DA88))return false;
    __try {
        auto kind=Native13<bool(__fastcall*)(void*,int)>(0x469590);
        // Retail S_AmmoItemClass::IsKind (96F66C) accepts Ammo=3. 1B is
        // the weapon base (469674), which Ammo deliberately does not inherit.
        return kind(launcher,2)&&kind(ammo,3);
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
