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

static HANDLE g_mod_thread = nullptr;

static DWORD WINAPI ModThreadMain(LPVOID)
{
    kcd2vr::Mod::Instance().Run();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        g_mod_thread = CreateThread(nullptr, 0, ModThreadMain, nullptr, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
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
