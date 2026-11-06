// Generated from actual src/dllmain.cpp; do not edit.
// worker SHA256: 6855faeab1e30ad6fc021ee6ee15bafc868f7b12840d3afb3c14c039409d3b56
static DWORD WINAPI ModThreadMain(LPVOID)
{
    (void)kcd2vr::diagnostics::unified_log_d81::Initialize();
    // Capture vendor creation before companion/plugin/camera setup. The worker
    // runs after loader initialization; never install hooks from DllMain.
    HMODULE selfD82=nullptr;
    if(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&ModThreadMain),&selfD82)) {
        try {
            const auto ini=kcd2vr::startup_s15::ModulePath(selfD82).parent_path()/L"KCD2VR.ini";
            const bool wanted=kcd2vr::startup_s15::ReadBool(ini,L"Enabled",true)&&kcd2vr::startup_s15::ProductionMode(ini)==3;
            kcd2vr::native_upscaler_stereo_d82::InstallEarly(wanted);
        }catch(...){kcd2vr::diagnostic_trace::Write("D82 TEMPORAL_STEREO earlyCapture=UNAVAILABLE reason=profile-path");}
    }
#ifdef KCD2VR_PROXY_DINPUT8
    kcd2vr::proxy29::LoadAsiPlugins();
#endif
    (void)kcd2vr::companion_loader59c4::Install();
    kcd2vr::Mod::Instance().Run();
    return 0;
}
