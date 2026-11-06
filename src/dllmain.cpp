// dllmain.cpp
//
// Standard ASI plugin entry point. Ultimate ASI Loader loads this like any
// other DLL, calling DllMain with DLL_PROCESS_ATTACH exactly like the game
// itself would for a normal dependency. We do the real work on a separate
// thread so we never block the loader or the game's own DllMain chain (a
// classic source of load-order deadlocks in this style of mod).
//
// IMPORTANT: a prior revision of this file removed the CreateThread/Run()
// call entirely, based on an overcautious reading of "don't do heavy work
// in DllMain" - but without spawning the worker thread SOMEWHERE, the mod
// never runs at all (no log file, nothing). The thread-based pattern below
// (do real work on a spawned thread, not inline in DllMain itself) is the
// correct way to honor that caution while still actually starting the mod.

#include <windows.h>
#include "mod.h"
#include "diagnostic_trace.h"
#include "native_upscaler_stereo_d82.h"
#include "companion_loader59c4.h"
#include "diagnostics/unified_log_d81.h"
#include "subsystems/base/startup_setup_s15.inl"
#include <cwchar>
static bool g_setupOnlyS15=false;

static HANDLE g_mod_thread = nullptr;
#ifdef KCD2VR_PROXY_DINPUT8
namespace kcd2vr::proxy29 { void LoadAsiPlugins() noexcept; } // C29: we are dinput8.dll, load the other asi mods
#endif

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

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        {wchar_t host[MAX_PATH]{};GetModuleFileNameW(nullptr,host,MAX_PATH);const auto base=std::wcsrchr(host,L'\\');g_setupOnlyS15=_wcsicmp(base?base+1:host,L"rundll32.exe")==0;}
        if(g_setupOnlyS15)break;
        g_mod_thread = CreateThread(nullptr, 0, ModThreadMain, nullptr, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        if(g_setupOnlyS15)break;
        kcd2vr::Mod::Instance().Shutdown();
        if (g_mod_thread != nullptr)
        {
            // NEVER call WaitForSingleObject(g_mod_thread) inside DllMain
            // during process termination - risks loader-lock deadlocks.
            CloseHandle(g_mod_thread);
            g_mod_thread = nullptr;
        }
        break;
    }
    return TRUE;
}
