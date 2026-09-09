#include "stereo_archaeology.h"
#include "debug_trace.h"

#include <windows.h>
#include <psapi.h>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace kcd2vr::stereo_archaeology
{
namespace
{
constexpr const char* kNeedles[] = {
    "sys_vr_support",
    "r_stereomode",
    "r_stereoMode",
    "r_stereodevice",
    "r_stereoDevice",
    "r_stereooutput",
    "r_stereoOutput",
    "r_stereoScaleCoefficient",
    "hmd_driver",
    "hmd_projection_screen_dist",
    "IHmdManager",
    "IHmdDevice",
    "SAsymmetricCameraSetupInfo",
    "CD3DStereoRenderer",
    "StereoRenderer",
    "StereoMode",
    "stereo mode",
    "Dual rendering",
    "dual rendering",
    "r_3MonHack",
    "r_3MonHackHUDFOVX",
    "r_3MonHackHUDFOVY",
    "r_3MonHackLeftCGFOffsetX",
    "r_3MonHackRightCGFOffsetX",
    "r_ShowRenderTarget",
    "r_ShowDynTextures",
    "r_ShowDynTexturesFilter",
    "r_ShowTexture",
    "r_D3D12EnableResourceTracking",
    "r_D3D12EnablePIXCaptureRuntime",
    "r_DuplicateLastFrameOnCameraChangeWH",
    "r_DuplicateLastFrameWH",
    "r_ReprojectOnlyStaticObjects",
    "r_MotionVectors",
    "e_ExecuteRenderAsJobMask",
    "e_SkipInFrustumOptimizationWH",
    "e_Recursion",
    "e_PortalsMaxRecursion",
};

bool IsReadableProtection(DWORD p)
{
    p &= 0xff;
    return p == PAGE_READONLY || p == PAGE_READWRITE ||
           p == PAGE_WRITECOPY || p == PAGE_EXECUTE_READ ||
           p == PAGE_EXECUTE_READWRITE || p == PAGE_EXECUTE_WRITECOPY;
}

void ScanModule(HMODULE module)
{
    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), module, &mi, sizeof(mi)) ||
        !mi.lpBaseOfDll || mi.SizeOfImage == 0)
        return;

    char path[MAX_PATH * 2]{};
    GetModuleFileNameA(module, path, static_cast<DWORD>(sizeof(path)));

    const auto base = reinterpret_cast<std::uintptr_t>(mi.lpBaseOfDll);
    const auto limit = base + static_cast<std::uintptr_t>(mi.SizeOfImage);
    std::uintptr_t cursor = base;

    while (cursor < limit)
    {
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(reinterpret_cast<void*>(cursor), &mbi, sizeof(mbi)))
            break;

        const auto region_base = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
        const auto region_end = region_base + mbi.RegionSize;
        const auto scan_begin = region_base < base ? base : region_base;
        const auto scan_end = region_end > limit ? limit : region_end;

        if (mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) &&
            !(mbi.Protect & PAGE_NOACCESS) && IsReadableProtection(mbi.Protect) &&
            scan_end > scan_begin)
        {
            const auto* bytes = reinterpret_cast<const unsigned char*>(scan_begin);
            const size_t size = static_cast<size_t>(scan_end - scan_begin);

            for (const char* needle : kNeedles)
            {
                const size_t n = std::strlen(needle);
                if (n == 0 || n > size) continue;

                for (size_t i = 0; i + n <= size; ++i)
                {
                    if (std::memcmp(bytes + i, needle, n) == 0)
                    {
                        char msg[1024]{};
                        std::snprintf(msg, sizeof(msg),
                            "STEREO-ARCH hit module='%s' rva=0x%llX string='%s'",
                            path,
                            static_cast<unsigned long long>((scan_begin + i) - base),
                            needle);
                        debug_trace::Write(msg);
                        break; // one hit per needle per region is enough for archaeology
                    }
                }
            }
        }

        if (region_end <= cursor) break;
        cursor = region_end;
    }
}
}

void ScanLoadedModules()
{
    debug_trace::Write("STEREO-ARCH scan begin");

    HMODULE modules[1024]{};
    DWORD needed = 0;
    if (!EnumProcessModules(GetCurrentProcess(), modules, sizeof(modules), &needed))
    {
        debug_trace::Write("STEREO-ARCH EnumProcessModules failed");
        return;
    }

    const DWORD count = (needed / sizeof(HMODULE) < 1024)
        ? needed / sizeof(HMODULE) : 1024;

    for (DWORD i = 0; i < count; ++i)
        ScanModule(modules[i]);

    debug_trace::Write("STEREO-ARCH scan end");
}
}
