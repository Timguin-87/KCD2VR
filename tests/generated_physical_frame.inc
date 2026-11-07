void __fastcall FrameNexusDetour(void* self,std::uintptr_t unused_arg,std::uint32_t flags) noexcept {
    const auto serial=g_nexus_calls.fetch_add(1,std::memory_order_relaxed)+1;
    const auto present_before=DeviceHook::PresentCaptureCount();
    const auto old_serial=t_nexus_serial;
    const auto old_depth=t_nexus_depth;
    const auto old_flags=t_nexus_flags;
    t_nexus_serial=serial;
    t_nexus_depth=old_depth+1;
    t_nexus_flags=flags;
    g_last_nexus_serial.store(serial,std::memory_order_release);
    g_last_flags.store(flags,std::memory_order_release);

    // One gameplay pump per outer native frame, never a second-eye render.
    // Receipt eligibility cannot survive an exceptional native return. Restore
    // only metadata in finally; the existing physics End remains normal-path.
    std::uint64_t present_after=present_before;
    __try {
        // F2/2 only. Before native/physics/pose locks and before the original
        // gameplay update can sample or schedule this frame's hand pose.
        if(old_depth==0 && !(flags&0x20u)) FrameSync::PaceGameplayBeforeUpdateD45();
        if(old_depth==0 && !(flags&0x20u)) frik_pose_probe::PhysicalNativeFrameTick6698();
        if(g_nexus_original) g_nexus_original(self,unused_arg,flags);
        if(old_depth==0 && !(flags&0x20u)) frik_pose_probe::PhysicalNativeFrameEnd104();
        present_after=DeviceHook::PresentCaptureCount();
        if(present_after>present_before) g_nexus_present_advances.fetch_add(present_after-present_before,std::memory_order_relaxed);
    } __finally {
        t_nexus_serial=old_serial;
        t_nexus_depth=old_depth;
        t_nexus_flags=old_flags;
    }

    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    auto last=g_last_nexus_log_ms.load(std::memory_order_relaxed);
    if(g_mode.load()!=0 && (serial<=6 || (now>=last+1000ull && g_last_nexus_log_ms.compare_exchange_strong(last,now,std::memory_order_acq_rel)))) {
        Tracef("V65.07-F8-NEXUS serial=%llu self=%p arg=0x%llX flags=0x%X bits={20:%d 10:%d 04:%d} presentBefore=%llu presentAfter=%llu presentDelta=%llu tid=%lu READ-ONLY",
            static_cast<unsigned long long>(serial),self,static_cast<unsigned long long>(unused_arg),unsigned(flags),
            (flags&0x20u)?1:0,(flags&0x10u)?1:0,(flags&0x04u)?1:0,
            static_cast<unsigned long long>(present_before),static_cast<unsigned long long>(present_after),
            static_cast<unsigned long long>(present_after-present_before),static_cast<unsigned long>(GetCurrentThreadId()));
    }
}
