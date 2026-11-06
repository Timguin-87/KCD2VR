// H23: accelerate only the authored pickup CLIP, never IAction time.
// The native event dispatcher still emits the real Attach. No normalized seek,
// synthetic event, inventory write or new native hook is involved.
struct PickupClipClockH23 {
    void* controller=nullptr;void* skeleton=nullptr;std::uintptr_t character=0;int layer=-1;
    float saved=1.f,written=8.f;bool active=false;
    std::uint64_t unownedUseD65=0; // zero preserves established held-pickup behavior
};
static PickupClipClockH23 g_pickup_clock_h23;
static std::atomic<bool> g_pickup_clock_live_h23{false};
// D44: the current controller callback may migrate between engine workers.
// Restore can also run before Queue on the original cached worker, outside the
// general owner transaction. Protect this separate lease without waiting on an
// animation job or borrowing unrelated native-owner state.
static std::atomic<void*> g_pickup_clock_controller_d44{nullptr};
static std::atomic_flag g_pickup_clock_lock_d44=ATOMIC_FLAG_INIT;
static thread_local unsigned g_pickup_clock_depth_d44=0;
struct PickupClipTransactionD44 {
    bool acquired=false;
    PickupClipTransactionD44() noexcept {
        if(g_pickup_clock_depth_d44){++g_pickup_clock_depth_d44;acquired=true;}
        else if(!g_pickup_clock_lock_d44.test_and_set(std::memory_order_acquire)){
            g_pickup_clock_depth_d44=1;acquired=true;
        }
    }
    ~PickupClipTransactionD44(){if(acquired&&!--g_pickup_clock_depth_d44)g_pickup_clock_lock_d44.clear(std::memory_order_release);}
    explicit operator bool() const noexcept {return acquired;}
};
static void ClearPickupClockH23() noexcept {
    g_pickup_clock_h23={};g_pickup_clock_controller_d44.store(nullptr,std::memory_order_release);
    g_pickup_clock_live_h23.store(false,std::memory_order_release);
}
static bool PickupClockControllerH23(void* ctrl) noexcept {
    return OnPickupOwnerD44()&&(ctrl==g_henry_ctrl.load(std::memory_order_acquire)||
        (g_pickup_clock_live_h23.load(std::memory_order_acquire)&&
         ctrl==g_pickup_clock_controller_d44.load(std::memory_order_acquire)));
}
constexpr std::uintptr_t kLayerScaleH23=0xAA8448;
constexpr unsigned char kLayerScalePinH23[]{0x83,0xFA,0x0F,0x77,0x1F,0x0F,0x28,0xC2,0x48,0x63,0xC2,0x0F,0x57,0xD2,0x48,0xC1,0xE0,0x06,0xF3,0x0F,0x10,0xD0,0x0F,0x57,0xC0,0xF3,0x0F,0x5F,0xC2,0xF3,0x0F,0x11,0x44,0x08,0x50,0xC3};
using LayerScaleH23=void(__fastcall*)(void*,int,float);
static bool PickupLayerSourceH23(void*& skel,std::uintptr_t& character) noexcept {
    const auto base=WhBase();skel=animation_filter::PlayerSkeletonAnim();
    character=reinterpret_cast<std::uintptr_t>(HenryChar());
    return base&&retail_build112::Ready(base)&&skel&&character&&
        Rd<std::uintptr_t>(skel,0,0)==base+kSkelVft&&
        Rd<std::uintptr_t>(reinterpret_cast<void*>(base+kSkelVft),0xB8,0)==base+kLayerScaleH23&&
        MatchBytes(base+kLayerScaleH23,kLayerScalePinH23,sizeof(kLayerScalePinH23));
}
static bool SetPickupLayerScaleH23(void* skel,int layer,float value) noexcept {
    if(!skel||layer<0||layer>15||!std::isfinite(value)||value<0||value>8)return false;
    __try {Native13<LayerScaleH23>(kLayerScaleH23)(skel,layer,value);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return Rd<float>(skel,0x50+0x40*layer,-1.f)==value;
}
// Called BEFORE original Update/Queue/Finish can change action membership.
// The layer value is shared by replacement clips: a changed FIFO must NOT skip
// restoration. Its current native scope cache, not an old action pointer, is
// the restore authority. An independent setter or native speed=8 wins.
// The optional controller is supplied only by the existing Update detour,
// before its original callback. It grants restoration of this clip lease only;
// no general owner state or native attachment may be touched through it.
static bool RestorePickupClipBoundaryD44(void* currentUpdate=nullptr) noexcept {
    const bool qualifiedUpdate=currentUpdate&&currentUpdate==g_henry_ctrl.load(std::memory_order_acquire)&&
        currentUpdate==g_pickup_clock_controller_d44.load(std::memory_order_acquire)&&
        IsHenryController(currentUpdate)&&retail_build112::Ready(WhBase());
    if(!qualifiedUpdate&&!OnPickupOwnerD44())return !g_pickup_clock_live_h23.load(std::memory_order_acquire);
    PickupClipTransactionD44 transaction;
    if(!transaction)return !g_pickup_clock_live_h23.load(std::memory_order_acquire);
    auto& lease=g_pickup_clock_h23;if(!lease.active)return true;
    if(currentUpdate&&(!qualifiedUpdate||lease.controller!=currentUpdate))return false;
    void* skel=nullptr;std::uintptr_t character=0;
    // An unavailable observer/pin is UNKNOWN, not proof the old live layer
    // disappeared. Retain the receipt and retry at the next owner boundary.
    if(!PickupLayerSourceH23(skel,character))return false;
    if(skel!=lease.skeleton||character!=lease.character){ClearPickupClockH23();return true;}
    const float current=Rd<float>(skel,0x50+0x40*lease.layer,-1.f);
    if(!std::isfinite(current)||current<0)return false;
    if(current!=lease.written){RetireUnownedPickupClockD65(lease.unownedUseD65);ClearPickupClockH23();return true;} // other writer owns it
    // A transient controller/scope observer loss cannot strand our 8x value.
    // Exact current skeleton+character+setter and readback8 still prove the
    // layer we changed; restore its captured native value in that case.
    float native=lease.saved;bool nativeCacheD65=false;
    void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    if(ctrl&&IsHenryController(ctrl)){
        void* scopes=Rd<void*>(ctrl,kCtrlScopes,nullptr);const unsigned count=Rd<unsigned>(ctrl,kCtrlScopeCount,0);
        if(scopes&&count&&count<=kMaxScopes&&Rd<int>(scopes,kScopeLayer,-1)==lease.layer){
            const float cache=Rd<float>(scopes,kScopeSpeedBias,-1.f);
            if(std::isfinite(cache)&&cache>=0&&cache<=8){native=cache;nativeCacheD65=true;}
        }
    }
    if(lease.unownedUseD65&&!nativeCacheD65)return false; // new fallback cannot borrow an unknown native scope
    if(native==lease.written){RetireUnownedPickupClockD65(lease.unownedUseD65);ClearPickupClockH23();return true;} // native current scope explicitly wants 8
    if(!SetPickupLayerScaleH23(skel,lease.layer,native))return false;
    if(native!=lease.saved)RetireUnownedPickupClockD65(lease.unownedUseD65);
    ClearPickupClockH23();return true;
}
static bool RestorePickupClipH23() noexcept {return RestorePickupClipBoundaryD44();}
static void TickPickupClipH23(std::uint64_t now) noexcept {
    if(!OnPickupOwnerD44())return;
    PickupClipTransactionD44 transaction;if(!transaction)return;
    if(g_pickup_clock_h23.active||!NativeGraspEnabled()||!g_context13.load()||
       g_owner.state!=Playing||g_owner.releaseUnclaimed55||g_pick13.attachStarted||kcd2vr::activity_input163::witness175.Active()||
       !CurrentOwnerInput55(now)||!StillPlaying())return;
    // FullBody root owns the actual pickup event-bearing base clip. Blend FIFO
    // entries stay native until only that one clip remains. Secondary layers,
    // unrelated Henry actions, NPCs and every workstation are untouched.
    void* skel=nullptr;std::uintptr_t character=0;
    if(!PickupLayerSourceH23(skel,character)||character!=g_owner.useInput55.character||
       g_owner.layer!=0||Rd<int>(g_owner.scope,kScopeLayer,-1)!=0||
       Rd<unsigned>(g_owner.scope,kScopeNumLayers,0)==0||NumAnims(skel,0)!=1)return;
    const float speed=Rd<float>(g_owner.action,kActSpeedBias,-1.f);
    const float cache=Rd<float>(g_owner.scope,kScopeSpeedBias,-1.f);
    const float layer=Rd<float>(skel,0x50,-1.f);
    const float time=AnimTime(skel,0,0);
    if(speed!=1.f||cache!=speed||layer!=speed||!std::isfinite(time)||time<0||time>=1.f)return;
    if(!StillPlaying()||!CurrentOwnerInput55(now))return;
    g_pickup_clock_h23={g_henry_ctrl.load(std::memory_order_acquire),skel,character,0,layer,8.f,true};
    g_pickup_clock_controller_d44.store(g_pickup_clock_h23.controller,std::memory_order_release);
    g_pickup_clock_live_h23.store(true,std::memory_order_release);
    if(!SetPickupLayerScaleH23(skel,0,8.f))(void)RestorePickupClipH23();
}
