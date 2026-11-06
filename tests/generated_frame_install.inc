bool EnsureNexusHook(bool physicalOnly=false) noexcept;
bool EnsureNexusHook(bool physicalOnly) noexcept {
    if(g_nexus_hook) return g_nexus_original&&g_nexus_installed.load(std::memory_order_acquire);
    if(physicalOnly?!ExactPhysicalFrameReady6699():!ExactBuildReady()) return false;
    const auto b=WhBase();
    const auto target=b+kFrameNexusRva;
    if(!Executable(reinterpret_cast<void*>(target))) return false;
    DetourModKit::hook::InlineRequest req{
        "kcd2vr_v6504a_f8_ccryaction_frame_nexus",
        DetourModKit::hook::Target{DetourModKit::Address{target}},
        DetourModKit::hook::Options{}};
    auto h=DetourModKit::hook::inline_at(std::move(req),&FrameNexusDetour);
    if(!h) return false;
    g_nexus_hook=std::move(*h);
    g_nexus_original=g_nexus_hook->original<FrameNexusFn>();
    if(!g_nexus_original || !g_nexus_hook->enable()) {
        g_nexus_original=nullptr;
        g_nexus_hook.reset();
        return false;
    }
    g_nexus_installed.store(true,std::memory_order_release);
    Trace("V65.07-F8 rung1 installed exact CCryAction+0x669C88 frame-nexus observer; no DoFrame/GetView hooks, no writes");
    return true;
}
bool EnsurePhysicalFramePump6698() noexcept {return EnsureNexusHook(true);}