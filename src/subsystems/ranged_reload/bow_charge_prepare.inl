static bool PhysChargePrepareLeaf6698(void* action,float dt) noexcept {
    auto& n=g_phys_native6698;if(!n.bowHeld||g_phys_fault6698.load()||!BowHandlingMode59c9(g_weapon_control_mode.load())||!g_gameplay_allowed.load()||!g_head_valid.load()||!g_left_valid.load()||!g_right_valid.load()||diagnostics::lab::NativeReference()||!PhysVft6698(action,0x47083C0)||!std::isfinite(dt)||dt<0||dt>.25f)return false;
    const PhysFrame6698 f=PhysLatestComplete175();const auto now=std::uint64_t(GetTickCount64());
    ph::Command release{};
    const bool pendingRelease=g_phys_commands6698.peek(release)&&PhysPendingBowRelease103(n,release,f,now)&&
        release.generation==n.bowGeneration175;
    if(!f.input.sample||f.request!=n.request||f.input.bodyEpoch!=n.epoch||n.epoch!=g_pose_epoch6689.load()||
       f.profiles[1].key!=n.key||f.profiles[1].generation!=n.bowGeneration175||
       (!f.output.bowHeld&&!pendingRelease)||!f.input.allowed||!f.input.bowActions||
       !f.input.tracked[0]||!f.input.tracked[1]||now<f.input.tick||now-f.input.tick>100)return false;
    if(n.physicalBow175&&(f.input.nativeBowTrigger126||!ph::manual_bow_geometry146(f.profiles[1])))return false;
    __try {
        void* actor=reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x9115E0)(static_cast<unsigned char*>(action)+0x58);if(actor!=n.player)return false;
        unsigned char abort=0,active=0;if(!Read(static_cast<unsigned char*>(action)+0x98,abort)||abort||!Read(static_cast<unsigned char*>(action)+0x99,active)||!active)return false;
        n.bowChargeObserved114=true;
        {float c128=0;if(Read(static_cast<unsigned char*>(action)+0xA8,c128)&&std::isfinite(c128)){g_bow_charge128.store(c128,std::memory_order_relaxed);
            g_bow_charge_tick128.store(std::uint64_t(GetTickCount64()),std::memory_order_release);}} // V66.128 read-only native ChargeFraction
        if(f.input.nativeBowTrigger126){
            float charge=0;if(Read(static_cast<unsigned char*>(action)+0xA8,charge)&&std::isfinite(charge))n.nativeChargeCap=charge;
            return false; // original runs with original dt; no ChargeFraction write
        }
        float first=0,second=0;if(!Read(static_cast<unsigned char*>(action)+0xAC,first)||!Read(static_cast<unsigned char*>(action)+0xB0,second)||!std::isfinite(first)||!std::isfinite(second))return false;
        const bool selectSecond=second>first||reinterpret_cast<bool(__fastcall*)(void*)>(WhBase()+0x289C734)(action);
        const float rate=selectSecond?second:first;if(rate<=0||rate>100)return false;
        n.nativeChargeCap=std::min(1.f,n.nativeChargeCap+dt*rate);
        const float fraction=float(f.output.bowHeld?f.output.bowFraction:release.fraction);
        if(!std::isfinite(fraction)||fraction<0||fraction>1)return false;
        const float target=quiver114::chargeTarget(fraction,n.nativeChargeCap);
        // Full disassembly: this consumer uses dt only to increment +A8.
        // Its manual update below runs with zero increment, keeping all native
        // parameter publication/state/eligibility calls. Native time caps draw.
        // No caller/frame dt, native stamina or weapon statistic is overwritten.
        return PhysPutFloat6698(static_cast<unsigned char*>(action)+0xA8,target);
    }__except(EXCEPTION_EXECUTE_HANDLER){PhysFault6698("V66.98 PHYSICAL FAULT native ChargeFraction adapter");return false;}
}
