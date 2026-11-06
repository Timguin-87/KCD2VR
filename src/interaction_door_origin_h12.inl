// Exact native selector point-input boundary. No native predicate/result override.
namespace originH12=kcd2vr::door_origin_h12;
using DoorPointFnH12=char(__fastcall*)(std::uintptr_t,const float*,const float*,float*);
static DoorPointFnH12 g_door_point_originalH12=nullptr;
static std::optional<DetourModKit::hook::Hook> g_door_point_hookH12,g_door_point_midH12;
static std::atomic<bool> g_door_point_readyH12{false},g_door_point_retiredH12{false};
struct DoorPointInvocationH12 {
    originH12::Receipt source{};std::uintptr_t point=0,bounds=0,distance=0;
    float local[3]{};kcd2vr::door_surface65::Witness surface{};bool usesSurface=false,pending=false;
};
static thread_local DoorPointInvocationH12 g_door_point_callH12;
static thread_local bool g_door_point_runningH12=false;
static bool DoorPointSourceH12(const originH12::Receipt& r,std::uint64_t now) noexcept {
    std::uintptr_t back=0,henry=0,policy=0;std::uint32_t id=0;
    const auto& h=r.hand;door_hold65::Frame frame{};
    if(!g_door_point_readyH12.load(std::memory_order_acquire)||g_door_point_retiredH12.load(std::memory_order_acquire)||
       !g_enabled.load(std::memory_order_acquire)||!g_split_ready57c1.load(std::memory_order_acquire)||
       g_visual_fallback_enabled.load(std::memory_order_acquire)||!door_use46::enabled.load(std::memory_order_acquire)||
       !door_use46::inputMode(combat_rules178r10::f5Rung.load(std::memory_order_acquire))||
       !door_hold65::current(h,now)||h.grip<.30f||!h.press||h.sourceGeneration!=DoorAcquisitionGeneration65()||
       h.trackingEpoch!=tracking59c6::epoch.load(std::memory_order_acquire)||
       h.settingsEpoch!=door_use46::settingsEpoch.load(std::memory_order_acquire)||
       h.experimentEpoch!=workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire)||
       !SelectionVft68(r.nativePlayer,0x3AA4B98)||!SelectionRead68(r.nativePlayer+0x38,henry)||henry!=h.player||
       !SelectionVft68(henry,0x4072C78)||!SelectionRead68(r.nativePlayer+0xB08,back)||
       !SelectionPolicy68(back,r.nativePlayer,policy)||policy!=r.policy||
       !SelectionVft68(r.entity,0x4072C78)||!SelectionRead68(r.entity+0xC,id)||id!=r.target)return false;
    if(!door_hold65::frame.read(frame))return false;
    const auto& current=frame.hand[h.side];
    // Revalidate current committed-player/pose epoch as well as the immutable
    // raw source. A newer pose in the SAME family may exist; it cannot relabel
    // the original point or extend its80ms age.
    float x=0,y=0,z=0,dx=0,dy=0,dz=0;std::uint64_t tick=0,epoch=0;std::uintptr_t player=0;
    if(!frik_pose_probe::ReadInteractionPalm59c9(h.side,x,y,z,dx,dy,dz,tick,&epoch,&player)||
       epoch!=h.poseEpoch||player!=h.player||tick<h.poseTick||!door_hold65::fresh(tick,now))return false;
    return door_hold65::current(current,now)&&door_hold65::sameSource(current,h)&&current.press==h.press&&
        current.poseTick==h.poseTick&&current.point.x==h.point.x&&current.point.y==h.point.y&&current.point.z==h.point.z&&
        !(frik_pose_probe::PhysicalHandReservations114()&(1u<<h.side))&&frik_pose_probe::ExactHandItemState(h.side)==0;
}
static bool DoorPointSurfaceH26(const DoorPointInvocationH12& call,std::uint64_t now) noexcept {
    const auto& w=call.surface;const auto& h=call.source.hand;
    if(!call.usesSurface)return true;
    // This witness was produced by the existing occluded native physics query,
    // not a clamp to the interaction bounds. Never accept a child/foreign owner.
    if(!door_use46::spatialMode(combat_rules178r10::f5Rung.load(std::memory_order_acquire))||
       !kcd2vr::door_surface65::current(w,now)||!kcd2vr::door_surface65::same(w.source,h)||
       w.entityId!=call.source.target||w.opaqueOwner!=call.source.entity)return false;
    kcd2vr::door_surface65::Witness latest{};
    if(!kcd2vr::door_surface65::read(call.source.target,h,now,latest)||latest.sample!=w.sample||latest.queryTick!=w.queryTick||
       latest.opaqueOwner!=w.opaqueOwner||latest.opaqueCollider!=w.opaqueCollider||
       latest.point.x!=w.point.x||latest.point.y!=w.point.y||latest.point.z!=w.point.z)return false;
    return true;
}
static void DoorPointPrepareH12(DetourModKit::hook::MidContext& ctx) noexcept {
    using namespace DetourModKit::hook;
    // The point-leaf hook executes immediately after this callback. Reentrant
    // leaf work never inherits or overwrites the outer one-shot transaction.
    if(g_door_point_runningH12)return;
    g_door_point_callH12.pending=false;
    if(!g_door_point_readyH12.load(std::memory_order_acquire)||g_door_point_retiredH12.load(std::memory_order_acquire))return;
    // Quiet frames add only copied mailbox checks, no native/model reads.
    const auto now=std::uint64_t(GetTickCount64());door_hold65::Frame hands{};
    if(!door_hold65::frame.read(hands))return;bool held=false;
    for(const auto& h:hands.hand)held=held||(door_hold65::current(h,now)&&h.grip>=.30f&&h.press);
    if(!held)return;
    const auto frame=gpr(ctx,Gpr::Rbp),entity=gpr(ctx,Gpr::Rsi),policy=gpr(ctx,Gpr::R12),interactor=gpr(ctx,Gpr::R15);
    const auto point=gpr(ctx,Gpr::Rdx),bounds=gpr(ctx,Gpr::R8),distance=gpr(ctx,Gpr::R9);
    std::uint32_t target=0,id=0;std::uintptr_t nativePlayer=0,actualPolicy=0,klass=0,name=0;
    if(!frame||point!=frame-0x48||bounds!=frame+0x18||distance!=stack_pointer(ctx)+0x40||
       !SelectionRead68(frame+0x48,target)||!target||target>16777215u)return;
    originH12::withdraw(target);
    if(!SelectionVft68(entity,0x4072C78)||!SelectionRead68(entity+0xC,id)||id!=target||
       !SelectionRead68(entity+0x20,klass)||!SelectionVft68(klass,0x3B282A0)||!SelectionRead68(klass+0x10,name))return;
    constexpr char door[]="AnimDoor";for(unsigned n=0;n<sizeof(door);++n){char c=0;if(!SelectionRead68(name+n,c)||c!=door[n])return;}
    if(!SelectionRead68(interactor+8,nativePlayer)||!SelectionPolicy68(interactor,nativePlayer,actualPolicy)||actualPolicy!=policy)return;
    originH12::Receipt receipt{};receipt.target=target;receipt.entity=entity;receipt.policy=policy;receipt.nativePlayer=nativePlayer;receipt.frame=frame;
    bool selected=false;
    for(const auto& h:hands.hand){receipt.hand=h;if(!DoorPointSourceH12(receipt,now))continue;if(selected)return;selected=true;g_door_point_callH12.source=receipt;}
    if(!selected)return;
    auto& call=g_door_point_callH12;call.usesSurface=false;call.surface={};float inverse[12]{},original[3]{},box[6]{};
    const float world[3]{xmm(ctx,15).lane<float>(0),xmm(ctx,14).lane<float>(0),xmm(ctx,13).lane<float>(0)};
    if(!SelectionCopy68(frame-0x20,inverse,sizeof(inverse))||!SelectionCopy68(point,original,sizeof(original))||
       !SelectionCopy68(bounds,box,sizeof(box))||!originH12::prepare(inverse,world,original,box,call.source.hand,call.local))return;
    if(door_use46::spatialMode(combat_rules178r10::f5Rung.load(std::memory_order_acquire))){
        // Nominate at the verified native AnimDoor candidate entry. Waiting for
        // descriptor append was circular: outside palm -> no append -> no query.
        float centre[3]{};if(originH12::worldCenter(inverse,box,centre))
            (void)kcd2vr::door_surface65::offer(kcd2vr::door_surface65::request(target,call.source.hand,now,{centre[0],centre[1],centre[2]}),now);
        kcd2vr::door_surface65::Witness w{};
        if(kcd2vr::door_surface65::read(target,call.source.hand,now,w)){
            call.surface=w;call.usesSurface=true;
            const float actual[3]{w.point.x,w.point.y,w.point.z};
            if(!DoorPointSurfaceH26(call,now)||!originH12::localPoint(inverse,actual,call.local)){
                call.usesSurface=false;call.surface={};
                if(!originH12::prepare(inverse,world,original,box,call.source.hand,call.local))return;
            }
        }
    }
    call.point=point;call.bounds=bounds;call.distance=distance;call.pending=true;
}
static char __fastcall DoorPointLeafH12(std::uintptr_t ignored,const float* point,const float* bounds,float* distance) {
    if(!g_door_point_callH12.pending||g_door_point_runningH12)return g_door_point_originalH12(ignored,point,bounds,distance);
    const auto call=g_door_point_callH12;g_door_point_callH12.pending=false;
    const bool scoped=call.pending&&!g_door_point_runningH12&&call.point==reinterpret_cast<std::uintptr_t>(point)&&
        call.bounds==reinterpret_cast<std::uintptr_t>(bounds)&&call.distance==reinterpret_cast<std::uintptr_t>(distance)&&
        DoorPointSourceH12(call.source,GetTickCount64())&&DoorPointSurfaceH26(call,GetTickCount64());
    if(!scoped)return g_door_point_originalH12(ignored,point,bounds,distance);
    char result=1;g_door_point_runningH12=true;
    // Do not key on ReturnAddress: the MID hook relocates the native CALL.
    // Exact MID source + all three original argument addresses bind this call.
    __try {result=g_door_point_originalH12(ignored,call.local,bounds,distance);}
    __finally {g_door_point_runningH12=false;}
    const auto now=std::uint64_t(GetTickCount64());float achieved=0;
    if(!result&&DoorPointSourceH12(call.source,now)&&DoorPointSurfaceH26(call,now)&&SelectionCopy68(reinterpret_cast<std::uintptr_t>(distance),&achieved,sizeof(achieved))&&
       std::isfinite(achieved)&&achieved>=0){auto receipt=call.source;receipt.tick=now;receipt.distance=achieved;
        std::copy(std::begin(call.local),std::end(call.local),std::begin(receipt.local));originH12::publish(receipt);}
    // Existing finite F5 recorder/gate only; no steady per-frame log or readback.
    static thread_local diagnostics::door_receipt63::Gate receiptGateH12;
    if(diagnostics::door_receipt63::allow(receiptGateH12,now)){
        char line[360]{};std::snprintf(line,sizeof(line),
            "H12 F5_DOOR_ORIGIN target=%u hand=%d nativeAL=%u sourceTick=%llu press=%llu localPoint=(%.4f,%.4f,%.4f) source=%s leaf=ORIGINAL_ONCE insideReceipt=%d nativePolicyAndSurface=STILL_REQUIRED",
            call.source.target,call.source.hand.side,unsigned(static_cast<unsigned char>(result)),
            (unsigned long long)call.source.hand.poseTick,(unsigned long long)call.source.hand.press,
            double(call.local[0]),double(call.local[1]),double(call.local[2]),call.usesSurface?"EXACT_PHYSICS_SURFACE":"PALM",originH12::find(call.source.target)!=nullptr);
        diagnostic_trace::Write(line);
    }
    return result; // original native AL and original distance output, exactly once
}
static void StopDoorPointH12() noexcept {
    g_door_point_readyH12.store(false,std::memory_order_release);
    if(g_door_point_midH12||g_door_point_hookH12)g_door_point_retiredH12.store(true,std::memory_order_release);
    // Disable prepare first; retain both executable handles and original until
    // process exit. A failed disable can only forward the original leaf.
    for(auto* h:{&g_door_point_midH12,&g_door_point_hookH12})if(*h){try{(void)(*h)->disable();}catch(...){}}
}
static bool InstallDoorPointH12(std::uintptr_t base) noexcept {
    if(g_door_point_readyH12.load(std::memory_order_acquire))return true;
    if(g_door_point_retiredH12.load(std::memory_order_acquire)||g_door_point_midH12||g_door_point_hookH12||
       !base||base!=g_wh_base||!retail_build112::Ready(base)||!g_split_ready57c1.load(std::memory_order_acquire)||
       !g_direct_door_hook54r1||!g_direct_door_hook54r1->is_enabled())return false;
    constexpr unsigned char call[]{0xE8,0x44,0xB7,0x97,0x00,0x84,0xC0,0x0F,0x85,0x4A,0x01,0x00,0x00,0x49,0x8B,0x04,0x24};
    constexpr unsigned char leaf[]{0x48,0x83,0xEC,0x48,0x49,0x8B,0xC8,0x4D,0x8B,0xD1,0xE8,0x3D,0x7B,0x44,0xFF,0x33,0xC9,0x84,0xC0};
    constexpr unsigned char contains[]{0xF3,0x0F,0x10,0x1A,0xF3,0x0F,0x10,0x01,0x0F,0x2F,0xC3,0x76,0x03,0x32,0xC0,0xC3};
    constexpr unsigned char inverse[]{0xE8,0x7E,0x43,0x13,0x00,0x41,0x0F,0x28,0xD6};
    if(!MatchEntry(base+0x640117,call,sizeof(call))||!MatchEntry(base+0xFBB860,leaf,sizeof(leaf))||
       !MatchEntry(base+0x4033AC,contains,sizeof(contains))||!MatchEntry(base+0x63FFDD,inverse,sizeof(inverse)))return false;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&DoorPointPrepareH12),&module)||!module)return false;
    try {using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
        auto leafHook=inline_at(InlineRequest{"kcd2vr_door_point_leaf_h12",Target{DetourModKit::Address{base+0xFBB860}},options},&DoorPointLeafH12);
        if(!leafHook)return false;g_door_point_hookH12=std::move(*leafHook);g_door_point_originalH12=g_door_point_hookH12->original<DoorPointFnH12>();
        auto mid=mid_at(MidRequest{"kcd2vr_door_point_source_h12",Target{DetourModKit::Address{base+0x640117}},options},&DoorPointPrepareH12);
        if(!mid){StopDoorPointH12();return false;}g_door_point_midH12=std::move(*mid);
        if(!g_door_point_originalH12||!g_door_point_hookH12->enable()||!g_door_point_hookH12->is_enabled()||
           !g_door_point_midH12->enable()||!g_door_point_midH12->is_enabled()){StopDoorPointH12();return false;}
        g_door_point_readyH12.store(true,std::memory_order_release);return true;
    }catch(...){StopDoorPointH12();return false;}
}
