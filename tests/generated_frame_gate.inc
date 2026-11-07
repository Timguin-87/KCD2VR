constexpr std::uint32_t kSupportedTimestamp = 0x6A350E20u;
constexpr std::uintptr_t kFrameNexusRva = 0x669C88ull;
constexpr std::uintptr_t kCCryActionVtableRva = 0x40472D0ull;
constexpr std::size_t kCCryActionFrameNexusSlot = 0x58 / sizeof(void*);
constexpr std::uintptr_t kPhase7e8CallsiteRva = 0x669D48ull;
constexpr std::uintptr_t kCameraHandoffCallsiteRva = 0x669DF3ull;
constexpr std::uintptr_t kRendererPhaseCallsiteRva = 0x669E60ull;
constexpr std::uintptr_t kPhase7f8CallsiteRva = 0x669FB9ull;
constexpr std::uintptr_t kGetOrCreateRenderViewRva = 0x43EB60ull;
constexpr std::uintptr_t kCRenderViewCameraBuildRva = 0x865550ull;
constexpr std::uintptr_t kCRenderViewCameraPairBuildRva = 0x866F6Cull;
constexpr std::uintptr_t kCSystemRenderRva = 0x43E464ull;
constexpr std::uintptr_t kCSystemVtableRva = 0x3DC2858ull;
constexpr std::uintptr_t kCSystemRenderBeginRva = 0x52EA10ull;
constexpr std::uintptr_t kCSystemRenderEndRva = 0x9A7AE0ull;
constexpr std::uintptr_t kCSystemSetViewCameraRva = 0x7F0AE0ull;
constexpr std::uintptr_t kCSystemGetViewCameraRva = 0x5D77F0ull;
constexpr std::uintptr_t kCSystemRenderBeginCallsiteRva = 0x6675C2ull;
constexpr unsigned char kFrameNexusPrologue[] = {
    0x41,0xF6,0xC0,0x20,0x0F,0x85,0x65,0x03,0x00,0x00,
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10
};
constexpr unsigned char kPhase7e8Callsite[] = {
    0x48,0x8B,0x0D,0xB9,0x3A,0x2C,0x04,0x48,0x8B,0x01,0xFF,0x90,0xE8,0x07,0x00,0x00
};
constexpr unsigned char kCameraHandoffCallsite[] = {
    0x48,0x8B,0x3D,0x0E,0x3A,0x2C,0x04,0x48,0x8B,0x4E,0x48,0x48,0x8B,0x07,
    0xC7,0x45,0xC0,0x00,0x00,0x00,0x00,0x48,0x8B,0x58,0x40
};
constexpr unsigned char kRendererPhaseCallsite[] = {
    0x48,0x8B,0x0D,0xA1,0x3A,0x2C,0x04,0x48,0x85,0xC9,0x74,0x09,0x48,0x8B,0x01,
    0xFF,0x90,0x80,0x03,0x00,0x00
};
constexpr unsigned char kPhase7f8Callsite[] = {
    0x48,0x8B,0x0D,0x48,0x38,0x2C,0x04,0x48,0x8B,0x01,0xFF,0x90,0xF8,0x07,0x00,0x00
};
constexpr unsigned char kGetOrCreatePrologue[] = {
    0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x30,
    0x48,0x63,0xC2,0x48,0x81,0xC1,0xD8,0x23,0x00,0x00
};
constexpr unsigned char kCRenderViewCameraBuildPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x68,0x10,
    0x48,0x89,0x70,0x18,0x48,0x89,0x78,0x20,0x41,0x56,0x48,0x83,0xEC,0x30
};
constexpr unsigned char kCRenderViewCameraPairBuildPrologue[] = {
    0x48,0x8B,0xC4,0x4C,0x89,0x48,0x20,0x55,0x53,0x56,0x57,0x41,0x54,0x41,0x56,0x41,0x57,
    0x48,0x8D,0xA8,0xB8,0xFA,0xFF,0xFF,0x48,0x81,0xEC,0x10,0x06,0x00,0x00
};
constexpr unsigned char kCSystemRenderPrologue[] = {
    0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,
    0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x68,0xA1,0x48,0x81,0xEC,0xC0,0x00,0x00,0x00
};
constexpr unsigned char kCSystemRenderBeginPrologue[] = {
    0x40,0x53,0x48,0x83,0xEC,0x20,0x80,0xB9,0xC2,0x05,0x00,0x00,0x00,
    0x48,0x8B,0xD9,0x0F,0x85,0x83,0x00,0x00,0x00,0x48,0x8B,0x41,0x20
};
constexpr unsigned char kCSystemRenderBeginCallsite[] = {
    0x48,0x8B,0x4F,0x48,
    0x4C,0x8B,0x84,0x24,0x90,0x00,0x00,0x00,
    0x48,0x8B,0x94,0x24,0x88,0x00,0x00,0x00,
    0x48,0x8B,0x01,0xFF,0x50,0x50
};
constexpr unsigned char kCSystemGetViewCameraPrologue[] = {
    0x48,0x8D,0x81,0x88,0x02,0x00,0x00,0xC3
};
constexpr unsigned char kCSystemSetViewCameraPrologue[] = {
    0x48,0x81,0xC1,0x88,0x02,0x00,0x00,0xE9,0x94,0x63,0xC3,0xFF
};
bool ExactBuildReady() noexcept {
    const auto b=WhBase();
    std::uint32_t ts=0;
    const bool pe=ReadPe(b,ts);
    bool vtable_slot=false;
    if(b) {
        __try {
            const auto target=*reinterpret_cast<const std::uintptr_t*>(b+kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*));
            vtable_slot=(target==b+kFrameNexusRva);
        } __except(EXCEPTION_EXECUTE_HANDLER) { vtable_slot=false; }
    }
    const bool nexus_bytes=b && MatchBytes(b+kFrameNexusRva,kFrameNexusPrologue,sizeof(kFrameNexusPrologue));
    const bool phase7e8_bytes=b && MatchBytes(b+kPhase7e8CallsiteRva,kPhase7e8Callsite,sizeof(kPhase7e8Callsite));
    const bool camera_bytes=b && MatchBytes(b+kCameraHandoffCallsiteRva,kCameraHandoffCallsite,sizeof(kCameraHandoffCallsite));
    const bool renderer_bytes=b && MatchBytes(b+kRendererPhaseCallsiteRva,kRendererPhaseCallsite,sizeof(kRendererPhaseCallsite));
    const bool phase7f8_bytes=b && MatchBytes(b+kPhase7f8CallsiteRva,kPhase7f8Callsite,sizeof(kPhase7f8Callsite));
    const bool render_view_bytes=b && MatchBytes(b+kGetOrCreateRenderViewRva,kGetOrCreatePrologue,sizeof(kGetOrCreatePrologue));
    const bool stereo_build_bytes=b && MatchBytes(b+kCRenderViewCameraBuildRva,kCRenderViewCameraBuildPrologue,sizeof(kCRenderViewCameraBuildPrologue));
    const bool pair_build_bytes=b && MatchBytes(b+kCRenderViewCameraPairBuildRva,kCRenderViewCameraPairBuildPrologue,sizeof(kCRenderViewCameraPairBuildPrologue));
    const bool csystem_render_bytes=b && MatchBytes(b+kCSystemRenderRva,kCSystemRenderPrologue,sizeof(kCSystemRenderPrologue));
    const bool csystem_begin_bytes=b && MatchBytes(b+kCSystemRenderBeginRva,kCSystemRenderBeginPrologue,sizeof(kCSystemRenderBeginPrologue));
    const bool csystem_begin_callsite=b && MatchBytes(b+kCSystemRenderBeginCallsiteRva,kCSystemRenderBeginCallsite,sizeof(kCSystemRenderBeginCallsite));
    const bool csystem_get_camera_bytes=b && MatchBytes(b+kCSystemGetViewCameraRva,kCSystemGetViewCameraPrologue,sizeof(kCSystemGetViewCameraPrologue));
    // FrameSync installs KCD2VR's normal PublishViewCamera observer on this exact
    // retail thunk at startup, so untouched entry bytes are not a valid F8
    // prerequisite once the mod is running. Accept either pristine retail bytes
    // or our known live self-hook; the CSystem vtable target is still validated
    // independently below.
    const bool csystem_set_camera_raw_bytes=b && MatchBytes(b+kCSystemSetViewCameraRva,kCSystemSetViewCameraPrologue,sizeof(kCSystemSetViewCameraPrologue));
    const bool csystem_set_camera_self_hook=FrameSync::PublishViewCameraHookInstalled();
    const bool csystem_set_camera_route=csystem_set_camera_raw_bytes || csystem_set_camera_self_hook;
    bool csystem_vtable=false;
    if(b) {
        __try {
            const auto* vt=reinterpret_cast<const std::uintptr_t*>(b+kCSystemVtableRva);
            csystem_vtable=(vt[0x48/sizeof(void*)]==b+kCSystemRenderRva &&
                            vt[0x50/sizeof(void*)]==b+kCSystemRenderBeginRva &&
                            vt[0x58/sizeof(void*)]==b+kCSystemRenderEndRva &&
                            vt[0x430/sizeof(void*)]==b+kCSystemSetViewCameraRva &&
                            vt[0x438/sizeof(void*)]==b+kCSystemGetViewCameraRva);
        } __except(EXCEPTION_EXECUTE_HANDLER) { csystem_vtable=false; }
    }
    const bool ok=pe && ts==kSupportedTimestamp && vtable_slot && nexus_bytes &&
        phase7e8_bytes && camera_bytes && renderer_bytes && phase7f8_bytes && render_view_bytes && stereo_build_bytes && pair_build_bytes && csystem_render_bytes &&
        csystem_begin_bytes && csystem_begin_callsite && csystem_get_camera_bytes && csystem_set_camera_route && csystem_vtable;
    g_supported.store(ok,std::memory_order_release);
    if(ok) {
        Tracef("V65.56-F8 STATIC-GATE PASS setViewCamRoute=1 setViewCamRaw=%d selfHook=%d cSystemVtable=1",
            csystem_set_camera_raw_bytes?1:0,csystem_set_camera_self_hook?1:0);
    }
    if(!ok) {
        Tracef("V65.56-F8 STATIC-GATE FAIL pe=%d timestamp=0x%08X expected=0x%08X ccrySlot=%d nexus=%d cSystemRender=%d cSystemBegin=%d beginCallsite=%d getViewCam=%d setViewCamRoute=%d setViewCamRaw=%d selfHook=%d cSystemVtable=%d; no hooks installed",
            pe?1:0,ts,kSupportedTimestamp,vtable_slot?1:0,nexus_bytes?1:0,csystem_render_bytes?1:0,csystem_begin_bytes?1:0,csystem_begin_callsite?1:0,csystem_get_camera_bytes?1:0,csystem_set_camera_route?1:0,csystem_set_camera_raw_bytes?1:0,csystem_set_camera_self_hook?1:0,csystem_vtable?1:0);
    }
    return ok;
}
static bool ExactPhysicalFrameReady6699() noexcept {
    const auto b=WhBase();std::uint32_t ts=0;
    const bool pe=ReadPe(b,ts);
    bool slot=false;
    if(b){__try {slot=*reinterpret_cast<const std::uintptr_t*>(
        b+kCCryActionVtableRva+kCCryActionFrameNexusSlot*sizeof(void*))==b+kFrameNexusRva;
    } __except(EXCEPTION_EXECUTE_HANDLER) {slot=false;}}
    const bool entry=b&&MatchBytes(b+kFrameNexusRva,kFrameNexusPrologue,sizeof(kFrameNexusPrologue));
    const bool phaseA=b&&MatchBytes(b+kPhase7e8CallsiteRva,kPhase7e8Callsite,sizeof(kPhase7e8Callsite));
    const bool camera=b&&MatchBytes(b+kCameraHandoffCallsiteRva,kCameraHandoffCallsite,sizeof(kCameraHandoffCallsite));
    const bool renderer=b&&MatchBytes(b+kRendererPhaseCallsiteRva,kRendererPhaseCallsite,sizeof(kRendererPhaseCallsite));
    const bool phaseB=b&&MatchBytes(b+kPhase7f8CallsiteRva,kPhase7f8Callsite,sizeof(kPhase7f8Callsite));
    const bool ok=pe&&ts==kSupportedTimestamp&&slot&&entry&&phaseA&&camera&&renderer&&phaseB;
    Tracef("V66.99-PHYSICAL FRAME_GATE %s pe=%d timestamp=0x%08X slot=%d entry=%d phaseA=%d cameraCallsite=%d rendererCallsite=%d phaseB=%d cameraEntryPrerequisite=NONE",
        ok?"PASS":"FAIL",pe?1:0,ts,slot?1:0,entry?1:0,phaseA?1:0,camera?1:0,renderer?1:0,phaseB?1:0);
    return ok;
}
