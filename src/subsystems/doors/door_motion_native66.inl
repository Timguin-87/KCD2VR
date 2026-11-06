// Included inside frik_pose_probe's native implementation namespace; the core
// contract header is included at file scope. No shared entity setter is hooked.
namespace dmn66=kcd2vr::door_motion_native66;
using DoorMotionApply66=void(__fastcall*)(void*,void*,float,bool,void*);
static DoorMotionApply66 g_door_motion_original66=nullptr;
static std::optional<DetourModKit::hook::Hook> g_door_motion_apply_hook66,g_door_motion_animated_hook66,g_door_motion_entity_hook66;
static std::atomic<std::uintptr_t> g_door_motion_base66{0};
static std::atomic<bool> g_door_motion_enabled66{false},g_door_motion_retired66{false};
static std::atomic<unsigned> g_door_motion_receipts66{0};
struct DoorMotionTls66 {dmn66::Root root{};std::uint64_t serial=0;bool owned=false;};
static thread_local DoorMotionTls66 g_door_motion_tls66;
static bool DoorMotionCopy66(std::uintptr_t address,void* out,std::size_t size) noexcept {
    if(!address||!out||!size)return false;
    // Fixed reads of a clip already executing on the native thread. No broad
    // world polling or per-field VirtualQuery is introduced into this callback.
    __try {std::memcpy(out,reinterpret_cast<const void*>(address),size);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool DoorMotionRoot66(std::uintptr_t state,std::uintptr_t entity,dmn66::Root& root,
                             std::uint64_t& serial) noexcept {
    const auto base=g_door_motion_base66.load(std::memory_order_acquire);
    if(!g_door_motion_enabled66.load(std::memory_order_acquire)||g_door_motion_retired66.load(std::memory_order_acquire)||
       !base||entity!=reinterpret_cast<std::uintptr_t>(g_last_player_entity.load(std::memory_order_acquire)))return false;
    auto copy=DoorMotionCopy66;
    if(!dmn66::readRoot(copy,base,state,entity,root))return false;
    return kcd2vr::door66::OwnedMotionFor(entity,root.target,root.targetEntity,
        static_cast<std::uint64_t>(GetTickCount64()),&serial)&&serial&&
        g_door_motion_enabled66.load(std::memory_order_acquire)&&!g_door_motion_retired66.load(std::memory_order_acquire);
}
static bool DoorMotionCurrent66() noexcept {
    const auto& tls=g_door_motion_tls66;if(!tls.owned)return false;
    dmn66::Root current{};std::uint64_t serial=0;
    return DoorMotionRoot66(tls.root.clip+0x40,tls.root.entity,current,serial)&&
        serial==tls.serial&&dmn66::same(tls.root,current);
}
static void DoorMotionReceipt66(const char* endpoint) noexcept {
    {namespace h4=diagnostics::interaction_cycle_h4;const auto& tls=g_door_motion_tls66;h4::Event e{};e.stage=h4::Stage::Motion;e.tick=GetTickCount64();e.doorSerial=tls.serial;e.target=tls.root.target;e.player=tls.root.entity;h4::copy(e.reason,sizeof(e.reason),endpoint);h4::copy(e.label,sizeof(e.label),"EXISTING_EXACT_NATIVE_ROOT_ENDPOINT_NOT_OPERATION_COMPLETION");h4::observe(e,e.tick);}
    if(g_door_motion_receipts66.fetch_add(1,std::memory_order_relaxed)>=32)return;
    const auto& tls=g_door_motion_tls66;char line[256]{};
    std::snprintf(line,sizeof(line),"C66 DOOR_NATIVE_MOTION serial=%llu target=%u endpoint=%s decision=VR_ROOT_OWNS nativeClipAndInterpolation=ORIGINAL",
        static_cast<unsigned long long>(tls.serial),tls.root.target,endpoint);diagnostic_trace::Write(line);
}
static void DoorMotionAnimatedMid66(DetourModKit::hook::MidContext& c) noexcept {
    using namespace DetourModKit::hook;const auto base=g_door_motion_base66.load(std::memory_order_acquire);
    const auto& r=g_door_motion_tls66.root;
    // Gate before844528 loads its scratch RCX/RAX. RDI is the adjustment
    // state and RSI the same IEntity passed to the original helper. The
    // current root matcher independently verifies state+60/vtable/endpoint.
    if(!base||gpr(c,Gpr::Rdi)!=r.clip+0x40||gpr(c,Gpr::Rsi)!=r.entity||!DoorMotionCurrent66())return;
    instruction_pointer(c)=base+dmn66::AnimatedAfter;DoorMotionReceipt66("ANIMATED_CHARACTER_CORRECTION");
}
static void DoorMotionEntityMid66(DetourModKit::hook::MidContext& c) noexcept {
    using namespace DetourModKit::hook;const auto base=g_door_motion_base66.load(std::memory_order_acquire);
    const auto& r=g_door_motion_tls66.root;
    // At61C interpolation is complete and RSI/RBX are exact Entity/method.
    // RCX is NOT yet the Entity: its argument preparation begins at629. Skip
    // that scratch preparation and final call together. Native branch target
    //633 remains outside the hook patch and is never overwritten.
    if(!base||gpr(c,Gpr::Rsi)!=r.entity||
       gpr(c,Gpr::Rbx)!=base+0x509810||!DoorMotionCurrent66())return;
    instruction_pointer(c)=base+dmn66::EntityAfter;DoorMotionReceipt66("ENTITY_TRANSFORM");
}
static void DoorMotionApplyBody66(void* state,void* entity,float dt,bool flag,void* animation,
                                  std::uintptr_t caller) noexcept {
    const auto original=g_door_motion_original66;if(!original)return;
    const auto saved=g_door_motion_tls66;g_door_motion_tls66={};
    if(caller==g_door_motion_base66.load(std::memory_order_acquire)+dmn66::ApplyCaller){
        auto& tls=g_door_motion_tls66;
        tls.owned=DoorMotionRoot66(reinterpret_cast<std::uintptr_t>(state),reinterpret_cast<std::uintptr_t>(entity),tls.root,tls.serial);
    }
    // Never stop the procedural helper or alter dt. Native clip events,
    // interpolation clocks, target bookkeeping and cleanup all run normally.
    original(state,entity,dt,flag,animation);g_door_motion_tls66=saved;
}
static void __fastcall DoorMotionApplyDetour66(void* state,void* entity,float dt,bool flag,void* animation) noexcept {
    DoorMotionApplyBody66(state,entity,dt,flag,animation,reinterpret_cast<std::uintptr_t>(_ReturnAddress()));
}
static bool DoorMotionNativeContract66(std::uintptr_t base) noexcept {
    if(!base||!SupportedWhBuild(base)||!retail_build112::Ready(base))return false;
    unsigned char bytes[24]{};
    for(const auto& pin:dmn66::Pins)if(!DoorMotionCopy66(base+pin.rva,bytes,pin.size)||std::memcmp(bytes,pin.bytes,pin.size))return false;
    std::uintptr_t method=0;
    return DoorMotionCopy66(base+0x3A49468+0x2F0,&method,sizeof(method))&&method==base+0x8447B0;
}
static bool DoorMotionNativeArm66() noexcept {
    const auto base=WhBase();
    if(g_door_motion_enabled66.load(std::memory_order_acquire)&&g_door_motion_base66.load()==base)return true;
    // A partial install or shutdown is terminal. A callback may still be in
    // flight, or a failed disarm may leave it reachable: retain all trampolines.
    if(g_door_motion_retired66.load()||g_door_motion_apply_hook66||g_door_motion_animated_hook66||
       g_door_motion_entity_hook66||!DoorMotionNativeContract66(base))return false;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&DoorMotionApplyDetour66),&module)||!module)return false;
    using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
    auto apply=inline_at(InlineRequest{"kcd2vr_c66_door_adjustment_context",Target{DetourModKit::Address{base+dmn66::Apply}},options},DoorMotionApplyDetour66);
    if(!apply)return false;g_door_motion_apply_hook66=std::move(*apply);
    g_door_motion_original66=g_door_motion_apply_hook66->original<DoorMotionApply66>();
    auto animated=mid_at(MidRequest{"kcd2vr_c66_door_animated_correction",Target{DetourModKit::Address{base+dmn66::AnimatedCall}},options},DoorMotionAnimatedMid66);
    if(animated)g_door_motion_animated_hook66=std::move(*animated);
    auto entity=mid_at(MidRequest{"kcd2vr_c66_door_entity_correction",Target{DetourModKit::Address{base+dmn66::EntityCall}},options},DoorMotionEntityMid66);
    if(entity)g_door_motion_entity_hook66=std::move(*entity);
    if(!g_door_motion_original66||!animated||!entity){g_door_motion_retired66=true;return false;}
    g_door_motion_base66.store(base,std::memory_order_release);
    if(!g_door_motion_animated_hook66->enable()||!g_door_motion_entity_hook66->enable()||!g_door_motion_apply_hook66->enable()){
        g_door_motion_retired66.store(true,std::memory_order_release);return false;
    }
    g_door_motion_enabled66.store(true,std::memory_order_release);
    diagnostic_trace::Write("C66 DOOR_NATIVE_MOTION installed exact Henry door-root application veto; native procedural clocks and shared transform entries preserved");return true;
}
static void DoorMotionNativeShutdown66() noexcept {
    g_door_motion_enabled66.store(false,std::memory_order_release);
    // frik Install begins with Uninstall. A cold shutdown has no reachable
    // callback or trampoline to retire and must not poison the first install.
    if(!g_door_motion_apply_hook66&&!g_door_motion_animated_hook66&&!g_door_motion_entity_hook66&&!g_door_motion_original66)return;
    g_door_motion_retired66.store(true,std::memory_order_release);
    if(g_door_motion_apply_hook66)(void)g_door_motion_apply_hook66->disable();
    if(g_door_motion_animated_hook66)(void)g_door_motion_animated_hook66->disable();
    if(g_door_motion_entity_hook66)(void)g_door_motion_entity_hook66->disable();
    // Intentionally retain hook handles, base, and original for process life.
}
