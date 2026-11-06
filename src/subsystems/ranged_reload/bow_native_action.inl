// Current context is rechecked only when the exact native firing action runs.
static bool PhysBowAimCurrentD65C(const kcd2vr::bow_physical_aim_d65c::Snapshot& s) noexcept {
    std::unique_lock<std::recursive_mutex> lock(g_phys_native_mutex6698,std::try_to_lock);if(!lock.owns_lock())return false;
    const auto now=std::uint64_t(GetTickCount64());const auto& n=g_phys_native6698;
    const auto& f=PhysLatestComplete175();
    return !diagnostics::lab::NativeReference()&&g_gameplay_allowed.load()&&!g_phys_fault6698.load()&&
        g_weapon_control_mode.load()>=4&&TrackingHeadUsable111(now)&&g_right_valid.load()&&g_left_valid.load()&&
        n.player==s.player&&n.key==s.key&&n.bowGeneration175==s.generation&&n.epoch==s.body&&n.request==s.request&&
        s.tracking==kcd2vr::tracking59c6::epoch.load(std::memory_order_acquire)&&
        f.player==s.player&&f.request==s.request&&f.input.bodyEpoch==s.body&&f.input.allowed&&
        f.input.tick&&now>=f.input.tick&&now-f.input.tick<=80&&f.input.tracked[0]&&f.input.tracked[1]&&
        f.profiles[1].bow()&&f.profiles[1].key==s.key&&f.profiles[1].generation==s.generation;
}
static bool PhysBowCall6698(void* player,int operation) noexcept {
    namespace aim=kcd2vr::bow_physical_aim_d65c;
    if(operation!=1)aim::retire(); // new draw or cancel cannot inherit a pending release
    aim::current.store(&PhysBowAimCurrentD65C,std::memory_order_release);
    if(!PhysVft6698(player,0x3AA4B98))return false;
    void* controller=static_cast<unsigned char*>(player)+0xD70;void* back=nullptr;void* context=nullptr;
    if(!PhysVft6698(controller,0x3A80E30)||!Read(static_cast<unsigned char*>(controller)+0x18,back)||back!=player||!Read(static_cast<unsigned char*>(player)+0x280,context)||!context)return false;
    __try {
        void* bow=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x7F3310)(context);if(!bow)return false;
        struct Functor{void* vft;void* controller;void* bow;} f{nullptr,controller,bow};
        if(operation==0){
            unsigned char active=0,latch=0;
            if(!Read(static_cast<unsigned char*>(controller)+0x20,active)||!Read(static_cast<unsigned char*>(controller)+0x21,latch)||active||latch)return false;
            reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x1F24800)(&f);
            // This latch acknowledges the input only: +1F24825 sets it BEFORE
            // the eligibility calls. A charge callback is separate evidence.
            if(!Read(static_cast<unsigned char*>(controller)+0x21,latch)||latch!=1)return false;
            g_phys_native6698.controller=controller;g_phys_native6698.bow=bow;
        }
        else if(operation==1){
            // Native Fire itself reads this exact equipment item at bow+0x40.
            // This read merely binds our token; failure leaves the native release unchanged.
            if(auto* draft=aim::releaseDraft){void* launcher=nullptr;std::uint64_t key=0;
                if(draft->player==player&&Read(static_cast<unsigned char*>(bow)+0x40,launcher)&&
                   PhysVft6698(launcher,0x3A2DA88)&&Read(static_cast<unsigned char*>(launcher)+0x30,key)&&key==draft->key)
                    draft->launcher=launcher;
            }
            reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x1F24600)(&f);
        }
        else {reinterpret_cast<void(__fastcall*)(void*,void*)>(WhBase()+0x2A9FF94)(controller,bow);*reinterpret_cast<unsigned char*>(static_cast<unsigned char*>(controller)+0x20)=0;*reinterpret_cast<unsigned char*>(static_cast<unsigned char*>(controller)+0x21)=0;}
        return true;
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT native bow request");return false;}
}
