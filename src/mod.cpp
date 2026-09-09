// mod.cpp

#include "mod.h"
#include "debug_trace.h"
#include "diagnostic_trace.h"
#include "stereo_archaeology.h"
#include "frame_sync.h"
#include "recursive_view_probe.h"
#include "vrik_bridge.h"
#include "frik_pose_probe.h"
#include "flashui_probe.h"
#include "hud_xml_mod.h"
#include "depth_probe.h"

#include <DetourModKit/logger.hpp>
#include <DetourModKit/config.hpp>

#include <chrono>
#include <thread>
#include <windows.h>
#include <mmsystem.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cmath>
#include <utility>
#include <algorithm>

namespace kcd2vr {

using DetourModKit::log;
namespace config = DetourModKit::config;


namespace {

bool GetLoadedModuleSiblingPath(const char* filename, char* out, DWORD capacity) noexcept
{
    if (!filename || !out || capacity == 0)
        return false;

    HMODULE self = nullptr;
    if (!GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(&GetLoadedModuleSiblingPath),
            &self))
        return false;

    DWORD len = GetModuleFileNameA(self, out, capacity);
    if (len == 0 || len >= capacity)
        return false;

    char* slash = std::strrchr(out, '\\');
    if (!slash)
        slash = std::strrchr(out, '/');
    if (!slash)
        return false;

    *(slash + 1) = '\0';
    const size_t used = std::strlen(out);
    const size_t name_len = std::strlen(filename);
    if (used + name_len + 1 > capacity)
        return false;

    std::memcpy(out + used, filename, name_len + 1);
    return true;
}


bool ReadIniBoolExplicit(const char* path, const char* section, const char* key, bool fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;

    char raw[64]{};
    const char* fallback_text = fallback ? "true" : "false";
    GetPrivateProfileStringA(section, key, fallback_text, raw, static_cast<DWORD>(sizeof(raw)), path);

    // Trim leading/trailing ASCII whitespace.
    char* begin = raw;
    while (*begin == ' ' || *begin == '\t' || *begin == '\r' || *begin == '\n') ++begin;
    char* end = begin + std::strlen(begin);
    while (end > begin && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\r' || end[-1] == '\n')) --end;
    *end = '\0';

    for (char* p = begin; *p; ++p)
        if (*p >= 'A' && *p <= 'Z') *p = static_cast<char>(*p - 'A' + 'a');

    const bool value =
        std::strcmp(begin, "true") == 0 ||
        std::strcmp(begin, "1") == 0 ||
        std::strcmp(begin, "yes") == 0 ||
        std::strcmp(begin, "on") == 0;

    char trace[256]{};
    std::snprintf(trace, sizeof(trace),
        "explicit INI bool: [%s] %s raw='%s' parsed=%s",
        section, key, begin, value ? "true" : "false");
    debug_trace::Write(trace);
    return value;
}


int ReadIniIntExplicit(const char* path, const char* section, const char* key, int fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;
    char fb[64]{};
    std::snprintf(fb, sizeof(fb), "%d", fallback);
    char raw[64]{};
    GetPrivateProfileStringA(section, key, fb, raw, static_cast<DWORD>(sizeof(raw)), path);
    char* end = nullptr;
    const long parsed = std::strtol(raw, &end, 10);
    const int value = (end && end != raw) ? static_cast<int>(parsed) : fallback;
    char trace[256]{};
    std::snprintf(trace, sizeof(trace), "explicit INI int: [%s] %s raw='%s' parsed=%d", section, key, raw, value);
    debug_trace::Write(trace);
    return value;
}

float ReadIniFloatExplicit(const char* path, const char* section, const char* key, float fallback) noexcept
{
    if (!path || !section || !key)
        return fallback;
    char fb[64]{};
    std::snprintf(fb, sizeof(fb), "%.9g", static_cast<double>(fallback));
    char raw[64]{};
    GetPrivateProfileStringA(section, key, fb, raw, static_cast<DWORD>(sizeof(raw)), path);
    char* end = nullptr;
    const float parsed = std::strtof(raw, &end);
    const float value = (end && end != raw) ? parsed : fallback;
    char trace[256]{};
    std::snprintf(trace, sizeof(trace), "explicit INI float: [%s] %s raw='%s' parsed=%.6f", section, key, raw, value);
    debug_trace::Write(trace);
    return value;
}


// V46.1 direct CryEngine CVar access, recovered from WHGame.dll itself.
// WHGame code at 0x245F33D obtains r_Width/r_Height through IConsole::GetCVar
// (vslot +0xB8) and sets the integer value through ICVar vslot +0x38.
// Float readers/setters are independently observed at +0x20/+0x40.
// Keeping all SEH pointer touches in tiny leaf helpers avoids MSVC C2712.
constexpr std::uintptr_t kIConsoleGlobalRva = 0x492D8A8ull;
constexpr std::uint32_t kSupportedWhTimestampV6143 = 0x6A350E20u;

bool IsSupportedWhGameBuildV6143() noexcept
{
    const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!base) return false;
    __try {
        const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
        const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
        return nt->Signature == IMAGE_NT_SIGNATURE && nt->FileHeader.TimeDateStamp == kSupportedWhTimestampV6143;
    } __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

void* SafeGetCryConsole() noexcept
{
    const auto wh = reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if (!wh) return nullptr;
    __try { return *reinterpret_cast<void**>(wh + kIConsoleGlobalRva); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

void* SafeConsoleGetCVar(void* console, const char* name) noexcept
{
    if (!console || !name) return nullptr;
    __try {
        auto** vt = *reinterpret_cast<void***>(console);
        if (!vt || !vt[0xB8 / sizeof(void*)]) return nullptr;
        using Fn = void*(__fastcall*)(void*, const char*);
        return reinterpret_cast<Fn>(vt[0xB8 / sizeof(void*)])(console, name);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return nullptr; }
}

// V61.33 experimental semantic FlashUI command path. Public CryEngine IConsole
// documentation places ExecuteString after GetCVar in the same interface family;
// the current KCD2 GetCVar slot (+0xB8) is already WHGame-xref proven. The
// ExecuteString slot below (+0xD0) is therefore treated as a *candidate*, not
// a proven KCD2 ABI: executable-page validation + SEH fail closed, and the call
// is made only after the game itself has returned a recent live HUD object and
// the user explicitly releases the HUD-scale slider. No startup call is made.
bool SafeConsoleExecuteStringCandidate(void* console, const char* command) noexcept
{
    // V61.43: every hard-coded WHGame interface offset is build-gated. The
    // supplied WHGame.dll and KCD2Minimap.asi independently identify this as
    // Steam 1.5.6 / timestamp 0x6A350E20. Unknown builds fail closed.
    if (!IsSupportedWhGameBuildV6143() || !console || !command || !*command) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(console);
        if (!vt) return false;
        void* target = vt[0xD0 / sizeof(void*)];
        if (!target) return false;
        MEMORY_BASIC_INFORMATION mbi{};
        if (!VirtualQuery(target,&mbi,sizeof(mbi)) || mbi.State!=MEM_COMMIT ||
            (mbi.Protect & (PAGE_NOACCESS|PAGE_GUARD))) return false;
        const DWORD exec = mbi.Protect & 0xffu;
        if (!(exec==PAGE_EXECUTE || exec==PAGE_EXECUTE_READ || exec==PAGE_EXECUTE_READWRITE || exec==PAGE_EXECUTE_WRITECOPY)) return false;
        using Fn = void(__fastcall*)(void*, const char*, bool, bool);
        reinterpret_cast<Fn>(target)(console,command,false,false);
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool ExecuteUiVecCommand(void* console, const char* op, const char* movieClip,
                         float x, float y, float z) noexcept
{
    if (!console || !op || !movieClip) return false;
    char cmd[320]{};
    std::snprintf(cmd,sizeof(cmd),
        "#UIAction.%s(\"hud\",0,\"%s\",{x=%.4f,y=%.4f,z=%.4f})",
        op,movieClip,double(x),double(y),double(z));
    return SafeConsoleExecuteStringCandidate(console,cmd);
}

bool ApplyNativeVrUiLayout(bool enabled,
                           float subtitleScale, float subtitleY,
                           float dialogueScale, float dialogueY, float dialogueInsetPct,
                           float hintsScale, float hintsInsetPct, float hintsY) noexcept
{
    // V62.4: retail-native FlashUI path. V62.1 incorrectly sent #UIAction Lua
    // through IConsole; retail Pure-Game does not execute that developer-console
    // Lua bridge. flashui_probe now mirrors WHGame's own GetElement("hud")
    // call and modifies the declared MovieClips through IFlashVariableObject
    // GetDisplayInfo/SetDisplayInfo (+0xD0/+0xC8).
    return flashui_probe::ApplyVrLayout(enabled,subtitleScale,subtitleY,
        dialogueScale,dialogueY,dialogueInsetPct,hintsScale,hintsInsetPct,hintsY);
}

bool SyncLuaUiCompanion(const VrMenuState& s) noexcept
{
    // V62.9: C++ remains the master control surface. The companion is a tiny
    // lifecycle helper using the HUD functions exposed by the bundled test UI
    // adapter. Every command is optional/fail-soft; native FlashUI remains fallback.
    void* console=SafeGetCryConsole();
    if(!console) return false;
    char cmd[256]{};
    bool ok=true;
    auto run=[&](const char* name,double value){
        std::snprintf(cmd,sizeof(cmd),"%s %.4f",name,value);
        ok=SafeConsoleExecuteStringCandidate(console,cmd)&&ok;
    };
    run("kcd2vr_ui_enable",s.vr_ui_layout?1.0:0.0);
    run("kcd2vr_ui_sub_scale",s.vr_ui_subtitle_scale);
    run("kcd2vr_ui_sub_y",s.vr_ui_subtitle_y);
    run("kcd2vr_ui_dialog_scale",s.vr_ui_dialogue_scale);
    run("kcd2vr_ui_dialog_y",s.vr_ui_dialogue_y);
    run("kcd2vr_ui_dialog_inset",s.vr_ui_dialogue_inset_pct);
    run("kcd2vr_ui_hints_scale",s.vr_ui_hints_scale);
    run("kcd2vr_ui_hints_inset",s.vr_ui_hints_inset_pct);
    run("kcd2vr_ui_hints_y",s.vr_ui_hints_y);
    run("kcd2vr_ui_compass_scale",s.vr_ui_compass_scale);
    run("kcd2vr_ui_compass_y",s.vr_ui_compass_y);
    run("kcd2vr_ui_stats_scale",s.vr_ui_stats_scale);
    run("kcd2vr_ui_stats_x",s.vr_ui_stats_x);
    run("kcd2vr_ui_stats_y",s.vr_ui_stats_y);
    run("kcd2vr_ui_lower_scale",s.vr_ui_lower_scale);
    run("kcd2vr_ui_lower_y",s.vr_ui_lower_y);
    run("kcd2vr_ui_lower_spread",s.vr_ui_lower_spread);
    run("kcd2vr_ui_crosshair",s.hud_crosshair_visible?1.0:0.0);
    run("kcd2vr_ui_compass_visible",s.hud_compass_visible?1.0:0.0);
    run("kcd2vr_ui_lower_visible",s.hud_lower_icons_visible?1.0:0.0);
    run("kcd2vr_ui_letterbox",s.vr_ui_hide_letterbox?1.0:0.0);
    return ok;
}

bool ApplySemanticHudRootScale(float requested) noexcept
{
    // V61.38 release recovery: use the V61.33 semantics again.
    // CryEngine documents instanceID=-1 as "all instances"; V61.37 accidentally
    // changed this to instance 0 while also disabling the live HUD observer.
    // Never issue the candidate script bridge unless WHGame itself has returned
    // a recent live HUD object through the proven FlashUI lookup path.
    const float scale=std::clamp(requested,0.50f,1.20f);
    if(!flashui_probe::HasRecentHud()) {
        char m[192]{};
        std::snprintf(m,sizeof(m),"V61.38-HUD-SCALE WAIT scale=%.2f reason=no-recent-live-HUD",double(scale));
        debug_trace::Write(m);
        return false;
    }
    void* console=SafeGetCryConsole();
    if(!console) {
        debug_trace::Write("V61.38-HUD-SCALE FAILED reason=no-console");
        return false;
    }
    char cmd[256]{};
    std::snprintf(cmd,sizeof(cmd),
        "#UIAction.SetScale(\"HUD\",-1,\"MasterSwitch\",{x=%.4f,y=%.4f,z=1.0})",
        double(scale),double(scale));
    const bool ok=SafeConsoleExecuteStringCandidate(console,cmd);
    char m[384]{};
    std::snprintf(m,sizeof(m),
        "V61.38-HUD-SCALE %s scale=%.2f semantic=UIAction.SetScale element=HUD instance=-1 mc=MasterSwitch",
        ok?"REQUESTED":"FAILED",double(scale));
    debug_trace::Write(m);
    return ok;
}

bool ApplySemanticDialogueTextScale(float requested) noexcept
{
    // HUD.xml exposes SetSubtitleSize -> fc_setSubtitleSize. Use all live HUD
    // instances, matching CryEngine's documented instanceID=-1 semantics.
    const float scale=std::clamp(requested,0.50f,1.50f);
    const int size=static_cast<int>(std::lround(scale*100.0f));
    if(!flashui_probe::HasRecentHud()) {
        char m[192]{};
        std::snprintf(m,sizeof(m),"V61.38-DIALOG-TEXT WAIT size=%d reason=no-recent-live-HUD",size);
        debug_trace::Write(m);
        return false;
    }
    void* console=SafeGetCryConsole();
    if(!console) return false;
    char cmd[256]{};
    std::snprintf(cmd,sizeof(cmd),
        "#UIAction.CallFunction(\"HUD\",-1,\"SetSubtitleSize\",%d)", size);
    const bool ok=SafeConsoleExecuteStringCandidate(console,cmd);
    char m[256]{};
    std::snprintf(m,sizeof(m),"V61.38-DIALOG-TEXT %s size=%d instance=-1",ok?"REQUESTED":"FAILED",size);
    debug_trace::Write(m);
    return ok;
}

int SafeCVarGetInt(void* cvar, int fallback = 0) noexcept
{
    if (!cvar) return fallback;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x10 / sizeof(void*)]) return fallback;
        using Fn = int(__fastcall*)(void*);
        return reinterpret_cast<Fn>(vt[0x10 / sizeof(void*)])(cvar);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return fallback; }
}

float SafeCVarGetFloat(void* cvar, float fallback = 0.0f) noexcept
{
    if (!cvar) return fallback;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x20 / sizeof(void*)]) return fallback;
        using Fn = float(__fastcall*)(void*);
        return reinterpret_cast<Fn>(vt[0x20 / sizeof(void*)])(cvar);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return fallback; }
}

bool SafeCVarSetInt(void* cvar, int value) noexcept
{
    if (!cvar) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x38 / sizeof(void*)]) return false;
        using Fn = void(__fastcall*)(void*, int);
        reinterpret_cast<Fn>(vt[0x38 / sizeof(void*)])(cvar, value);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

bool SafeCVarSetFloat(void* cvar, float value) noexcept
{
    if (!cvar) return false;
    __try {
        auto** vt = *reinterpret_cast<void***>(cvar);
        if (!vt || !vt[0x40 / sizeof(void*)]) return false;
        using Fn = void(__fastcall*)(void*, float);
        reinterpret_cast<Fn>(vt[0x40 / sizeof(void*)])(cvar, value);
        return true;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// V58.3 regression repair: KCD2 can expose the saved cl_fov value while the
// active ViewCamera is still using the old/default cached projection until the
// graphics slider is touched. A same-value Set() may be short-circuited by the
// CVar implementation, so reproduce the user's proven slider workaround with a
// tiny reversible pulse and immediately restore the exact original value.
// This preserves the user's chosen FOV; it does not tie the MIS mono-remap
// calibration back to cl_fov.
float ReadCurrentGameFovValue() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "cl_fov");
    if (!f) return 0.0f;
    const float v = SafeCVarGetFloat(f, 0.0f);
    return (v > 1.0f && v < 200.0f) ? v : 0.0f;
}

float ReadCurrentHorizontalFovValue() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "cl_hfov");
    if (!f) return 0.0f;
    const float v = SafeCVarGetFloat(f, 0.0f);
    return (v > 1.0f && v < 220.0f) ? v : 0.0f;
}

bool ApplyNativeHorizontalFov(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* hf = console ? SafeConsoleGetCVar(console, "cl_hfov") : nullptr;
    void* vf = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    void* rw = console ? SafeConsoleGetCVar(console, "r_CustomResWidth") : nullptr;
    void* rh = console ? SafeConsoleGetCVar(console, "r_CustomResHeight") : nullptr;
    if (!hf) { debug_trace::Write("V60.9-HFOV cl_hfov CVar not found"); return false; }
    const float target = std::clamp(value, 70.0f, 170.0f);
    const float before_v = vf ? SafeCVarGetFloat(vf, -1.0f) : -1.0f;
    const int before_w = rw ? SafeCVarGetInt(rw, -1) : -1;
    const int before_h = rh ? SafeCVarGetInt(rh, -1) : -1;
    const bool ok = SafeCVarSetFloat(hf, target);
    const float got_h = SafeCVarGetFloat(hf, -1.0f);
    const float got_v = vf ? SafeCVarGetFloat(vf, -1.0f) : -1.0f;
    const int got_w = rw ? SafeCVarGetInt(rw, -1) : -1;
    const int got_hres = rh ? SafeCVarGetInt(rh, -1) : -1;
    char m[384]{};
    std::snprintf(m,sizeof(m),
        "V60.9-HFOV APPLY cl_hfov target=%.1f readback=%.3f cl_fov before/after=%.3f/%.3f customRes before=%dx%d after=%dx%d ok=%s",
        static_cast<double>(target), static_cast<double>(got_h),
        static_cast<double>(before_v), static_cast<double>(got_v),
        before_w,before_h,got_w,got_hres,ok?"true":"false");
    debug_trace::Write(m);
    return ok && std::fabs(got_h-target) < 0.25f;
}

float ReadCurrentGameFpsCap() noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return 0.0f;
    void* f = SafeConsoleGetCVar(console, "sys_MaxFPS");
    return f ? SafeCVarGetFloat(f, 0.0f) : 0.0f;
}

bool ApplyNativeGameFov(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    if (!f) return false;
    const float target = std::clamp(value, 70.0f, 130.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[256]{}; std::snprintf(m,sizeof(m),"V59.7-NATIVE-APPLY cl_fov target=%.1f readback=%.3f ok=%s; no pulse",target,got,ok?"true":"false"); debug_trace::Write(m);
    return ok && std::fabs(got-target) < 0.25f;
}

bool ApplyNativeGameFovV6128Test(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "cl_fov") : nullptr;
    if (!f) return false;
    // Test-only ceiling. If the CVar itself has a lower engine clamp, readback exposes it
    // and the closed loop stops rather than forcing memory.
    const float target = std::clamp(value, 70.0f, 150.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[288]{}; std::snprintf(m,sizeof(m),
        "V61.32-GAME-FOV request=%.2f readback=%.3f ok=%s ceiling=150 ordinary-CVar-only",
        static_cast<double>(target),static_cast<double>(got),ok?"true":"false"); debug_trace::Write(m);
    return ok && got > 1.0f;
}

bool ApplyNativeGameFps(float value) noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "sys_MaxFPS") : nullptr;
    if (!f) return false;
    const float target = std::clamp(value, 60.0f, 120.0f);
    const bool ok = SafeCVarSetFloat(f, target);
    const float got = SafeCVarGetFloat(f, -1.0f);
    char m[256]{}; std::snprintf(m,sizeof(m),"V59.7-NATIVE-APPLY sys_MaxFPS target=%.1f readback=%.3f ok=%s",target,got,ok?"true":"false"); debug_trace::Write(m);
    return ok && std::fabs(got-target) < 0.25f;
}

float ReadMergedMeshesDistance() noexcept
{
    void* console = SafeGetCryConsole();
    void* f = console ? SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio") : nullptr;
    return f ? SafeCVarGetFloat(f, 0.0f) : 0.0f;
}

bool ApplyMergedMeshesDistance(float value) noexcept
{
    // V61.32 release-facing "View Distance" control: keep the three ordinary
    // Warhorse distance ratios in lockstep so the slider is predictable.
    void* console = SafeGetCryConsole();
    void* view = console ? SafeConsoleGetCVar(console, "e_ViewDistRatio") : nullptr;
    void* veg = console ? SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation") : nullptr;
    void* merged = console ? SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio") : nullptr;
    if (!view || !veg || !merged) return false;
    const float target = std::clamp(value, 30.0f, 300.0f);
    bool ok=true;
    ok=SafeCVarSetFloat(view,target)&&ok;
    ok=SafeCVarSetFloat(veg,target)&&ok;
    ok=SafeCVarSetFloat(merged,target)&&ok;
    const float gv=SafeCVarGetFloat(view,-1.0f), gg=SafeCVarGetFloat(veg,-1.0f), gm=SafeCVarGetFloat(merged,-1.0f);
    ok=ok && std::fabs(gv-target)<0.25f && std::fabs(gg-target)<0.25f && std::fabs(gm-target)<0.25f;
    char m[360]{};
    std::snprintf(m,sizeof(m),"V61.32-VIEW-DIST APPLY target=%.1f readback={view:%.2f veg:%.2f merged:%.2f} ok=%s",
        static_cast<double>(target),static_cast<double>(gv),static_cast<double>(gg),static_cast<double>(gm),ok?"true":"false");
    debug_trace::Write(m);
    return ok;
}

bool ReadRendererColor(float& gamma,float& brightness,float& contrast) noexcept
{
    void* console=SafeGetCryConsole(); if(!console) return false;
    void* g=SafeConsoleGetCVar(console,"r_Gamma");
    void* b=SafeConsoleGetCVar(console,"r_Brightness");
    void* c=SafeConsoleGetCVar(console,"r_Contrast");
    if(!g||!b||!c) return false;
    gamma=SafeCVarGetFloat(g,1.0f); brightness=SafeCVarGetFloat(b,0.0f); contrast=SafeCVarGetFloat(c,1.0f);
    return std::isfinite(gamma)&&std::isfinite(brightness)&&std::isfinite(contrast);
}

bool ApplyRendererColor(float gamma,float brightness,float contrast) noexcept
{
    void* console=SafeGetCryConsole(); if(!console) return false;
    void* g=SafeConsoleGetCVar(console,"r_Gamma");
    void* b=SafeConsoleGetCVar(console,"r_Brightness");
    void* c=SafeConsoleGetCVar(console,"r_Contrast");
    if(!g||!b||!c) return false;
    gamma=std::clamp(gamma,0.50f,2.00f);
    brightness=std::clamp(brightness,-0.50f,0.50f);
    contrast=std::clamp(contrast,0.50f,2.00f);
    bool ok=true;
    ok=SafeCVarSetFloat(g,gamma)&&ok; ok=SafeCVarSetFloat(b,brightness)&&ok; ok=SafeCVarSetFloat(c,contrast)&&ok;
    const float rg=SafeCVarGetFloat(g,-99.0f), rb=SafeCVarGetFloat(b,-99.0f), rc=SafeCVarGetFloat(c,-99.0f);
    ok=ok&&std::fabs(rg-gamma)<0.02f&&std::fabs(rb-brightness)<0.02f&&std::fabs(rc-contrast)<0.02f;
    char m[320]{}; std::snprintf(m,sizeof(m),"V62.4-GRAPHICS-COLOR gamma=%.3f brightness=%.3f contrast=%.3f readback=%.3f/%.3f/%.3f ok=%s",
        double(gamma),double(brightness),double(contrast),double(rg),double(rb),double(rc),ok?"true":"false"); debug_trace::Write(m);
    return ok;
}


// V61.12: KCD2 retail WHGame contains the DOF parameter names
// Dof_User_FocusDistance / Dof_FocusDistance / FocusDistance. Try them only
// through the existing CryConsole read path; never write engine DOF state.
float ReadEngineDofFocusDistance(const char** source_name) noexcept
{
    static void* cached=nullptr;
    static const char* cached_name=nullptr;
    static std::uint64_t last_resolve_ms=0;
    const std::uint64_t now=static_cast<std::uint64_t>(GetTickCount64());
    if (!cached && (last_resolve_ms==0 || now-last_resolve_ms>=1000ull)) {
        last_resolve_ms=now;
        void* console=SafeGetCryConsole();
        if (console) {
            const char* names[]={"Dof_User_FocusDistance","Dof_FocusDistance","FocusDistance"};
            for (const char* name:names) {
                void* c=SafeConsoleGetCVar(console,name);
                if (c) {
                    cached=c; cached_name=name; char msg[192]{};
                    std::snprintf(msg,sizeof(msg),"V61.12-ENGINE-DOF source resolved CVar=%s (read-only)",name);
                    debug_trace::Write(msg); break;
                }
            }
        }
    }
    if (source_name) *source_name=cached_name;
    if (!cached) return 0.0f;
    const float v=SafeCVarGetFloat(cached,0.0f);
    return (std::isfinite(v) && v>0.01f && v<1000.0f) ? v : 0.0f;
}

float PseudoScaleForFocusDistance(float d) noexcept
{
    auto lerpf=[](float a,float b,float u) noexcept { return a+(b-a)*std::clamp(u,0.0f,1.0f); };
    if (d <= 0.60f) return 0.80f;
    if (d <= 1.50f) return lerpf(0.80f,0.85f,(d-0.60f)/0.90f);
    if (d <= 4.00f) return lerpf(0.85f,0.98f,(d-1.50f)/2.50f);
    return 0.98f;
}

bool ReapplyCurrentGameFov(float& before, float& pulse_readback, float& after) noexcept
{
    before = pulse_readback = after = 0.0f;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* f = SafeConsoleGetCVar(console, "cl_fov");
    if (!f) return false;
    before = SafeCVarGetFloat(f, 0.0f);
    if (!(before > 1.0f && before < 200.0f)) return false;
    // V59.3 recovery guard: user testing showed cl_fov below 110 can leave both
    // desktop and XR output black across restarts while the ASI is loaded. Never
    // pulse or write cl_fov in that unsafe range.
    if (before < 109.5f) return false;

    // One degree is large enough to avoid same-value/quantization short-cuts,
    // but is restored immediately before normal gameplay begins.
    const float pulse = (before <= 198.0f) ? (before + 1.0f) : (before - 1.0f);
    if (!SafeCVarSetFloat(f, pulse)) return false;
    pulse_readback = SafeCVarGetFloat(f, -1.0f);
    if (!SafeCVarSetFloat(f, before)) return false;
    after = SafeCVarGetFloat(f, -1.0f);
    return std::fabs(after - before) < 0.01f;
}

// V58 dialogue camera controls discovered as native WHGame CVars. Keep this
// reversible: capture the user's/game's original values once, apply only the
// two documented camera switches, and restore on mode Native/shutdown.
struct DialogueCVarSnapshot
{
    bool valid=false;
    int force_pov=0;
    int no_cameras=0;
};

bool CaptureDialogueCVars(DialogueCVarSnapshot& out) noexcept
{
    void* c=SafeGetCryConsole(); if(!c) return false;
    void* p=SafeConsoleGetCVar(c,"wh_dlg_ForcePOV");
    void* n=SafeConsoleGetCVar(c,"wh_dlg_NoCameras");
    if(!p || !n) return false;
    out.force_pov=SafeCVarGetInt(p,0);
    out.no_cameras=SafeCVarGetInt(n,0);
    out.valid=true;
    return true;
}

bool ApplyDialogueMode(const DialogueCVarSnapshot& original,int mode) noexcept
{
    if(!original.valid) return false;
    void* c=SafeGetCryConsole(); if(!c) return false;
    void* p=SafeConsoleGetCVar(c,"wh_dlg_ForcePOV");
    void* n=SafeConsoleGetCVar(c,"wh_dlg_NoCameras");
    if(!p || !n) return false;
    int target_p=original.force_pov, target_n=original.no_cameras;
    if(mode==1){ target_p=1; target_n=0; }
    else if(mode==2){ target_p=0; target_n=1; }
    bool ok=true; ok=SafeCVarSetInt(p,target_p)&&ok; ok=SafeCVarSetInt(n,target_n)&&ok;
    const int got_p=SafeCVarGetInt(p,-999), got_n=SafeCVarGetInt(n,-999);
    char m[256]{}; std::snprintf(m,sizeof(m),
        "V58-DIALOGUE mode=%d target={ForcePOV:%d NoCameras:%d} readback={%d,%d} ok=%s",
        mode,target_p,target_n,got_p,got_n,ok?"true":"false"); debug_trace::Write(m);
    return ok && got_p==target_p && got_n==target_n;
}


void V54PhaseBeeps(int phase) noexcept
{
    // Keep the test/game thread unblocked: emit the phase ID on a tiny detached sound thread.
    std::thread([phase](){ for(int i=0;i<phase;i++){ Beep(880,90); if(i+1<phase) Sleep(80); } }).detach();
}
void V54DoneBeep() noexcept { std::thread([](){ Beep(620,180); Beep(1040,260); }).detach(); }

struct V54StereoCVarSnapshot { bool valid=false; float strength=0, eye=0, screen=0, scale=0; int flip=0; };
V54StereoCVarSnapshot CaptureV54StereoCVars() noexcept {
    V54StereoCVarSnapshot o{}; void* c=SafeGetCryConsole(); if(!c) return o;
    auto F=[&](const char*n){return SafeCVarGetFloat(SafeConsoleGetCVar(c,n),0.0f);};
    auto I=[&](const char*n){return SafeCVarGetInt(SafeConsoleGetCVar(c,n),0);};
    o.strength=F("r_StereoStrength"); o.eye=F("r_StereoEyeDist"); o.screen=F("r_StereoScreenDist"); o.scale=F("r_stereoScaleCoefficient"); o.flip=I("r_StereoFlipEyes"); o.valid=true; return o;
}
bool ApplyV54StereoCVars(float ipd, bool flip) noexcept {
    void* c=SafeGetCryConsole(); if(!c) return false; bool ok=true;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoStrength"),1.0f)&&ok;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoEyeDist"),ipd)&&ok;
    ok=SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_stereoScaleCoefficient"),1.0f)&&ok;
    ok=SafeCVarSetInt(SafeConsoleGetCVar(c,"r_StereoFlipEyes"),flip?1:0)&&ok;
    char m[320]{};std::snprintf(m,sizeof(m),"V54-STEREO-CVARS target={strength:1 eye:%.6f scale:1 flip:%d} ok=%s",(double)ipd,flip?1:0,ok?"true":"false");debug_trace::Write(m);return ok;
}
void RestoreV54StereoCVars(const V54StereoCVarSnapshot&o) noexcept { if(!o.valid)return; void*c=SafeGetCryConsole(); if(!c)return; SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoStrength"),o.strength); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoEyeDist"),o.eye); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_StereoScreenDist"),o.screen); SafeCVarSetFloat(SafeConsoleGetCVar(c,"r_stereoScaleCoefficient"),o.scale); SafeCVarSetInt(SafeConsoleGetCVar(c,"r_StereoFlipEyes"),o.flip); }
static V54StereoCVarSnapshot g_v610_manual_stereo_original{};
static bool g_v610_manual_dual_active=false;

struct EngineCVarSnapshot
{
    bool valid = false;
    int custom_width = 0;
    int custom_height = 0;
    int custom_preview = 0;
    int custom_max_size = 0;
    float cl_fov = 0.0f;
};

bool CaptureEngineCVarSnapshot(EngineCVarSnapshot& out) noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* w = SafeConsoleGetCVar(console, "r_CustomResWidth");
    void* h = SafeConsoleGetCVar(console, "r_CustomResHeight");
    void* p = SafeConsoleGetCVar(console, "r_CustomResPreview");
    void* m = SafeConsoleGetCVar(console, "r_CustomResMaxSize");
    void* f = SafeConsoleGetCVar(console, "cl_fov");
    if (!w || !h || !p || !m || !f) return false;
    out.custom_width = SafeCVarGetInt(w, 0);
    out.custom_height = SafeCVarGetInt(h, 0);
    out.custom_preview = SafeCVarGetInt(p, 0);
    out.custom_max_size = SafeCVarGetInt(m, 0);
    out.cl_fov = SafeCVarGetFloat(f, 0.0f);
    out.valid = true;
    return true;
}

bool ApplyEngineCVarTest(const EngineCVarSnapshot& original,
                         int custom_width, int custom_height,
                         float requested_fov, bool use_custom_res,
                         char* log_buf, std::size_t log_cap) noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* w = SafeConsoleGetCVar(console, "r_CustomResWidth");
    void* h = SafeConsoleGetCVar(console, "r_CustomResHeight");
    void* p = SafeConsoleGetCVar(console, "r_CustomResPreview");
    void* f = SafeConsoleGetCVar(console, "cl_fov");
    if (!w || !h || !p || !f) return false;

    int target_w = use_custom_res ? custom_width : original.custom_width;
    int target_h = use_custom_res ? custom_height : original.custom_height;
    int target_preview = use_custom_res ? 1 : original.custom_preview;
    float target_fov = requested_fov > 1.0f ? requested_fov : original.cl_fov;

    // r_CustomResMaxSize defaults to 4096 in this build. Never request beyond
    // the engine's own current limit during the automatic diagnostic.
    if (use_custom_res && original.custom_max_size > 0 &&
        (target_w > original.custom_max_size || target_h > original.custom_max_size))
        return false;

    bool ok = true;
    // Width/height first, preview last: avoid briefly enabling preview with a
    // stale dimension pair. For disable/restore, all values are restored.
    ok = SafeCVarSetInt(w, target_w) && ok;
    ok = SafeCVarSetInt(h, target_h) && ok;
    ok = SafeCVarSetInt(p, target_preview) && ok;
    ok = SafeCVarSetFloat(f, target_fov) && ok;

    const int got_w = SafeCVarGetInt(w, -1);
    const int got_h = SafeCVarGetInt(h, -1);
    const int got_p = SafeCVarGetInt(p, -1);
    const float got_f = SafeCVarGetFloat(f, -1.0f);
    if (log_buf && log_cap) {
        std::snprintf(log_buf, log_cap,
            "V46.1-CVAR-APPLY requested={custom:%s %dx%d preview:%d cl_fov:%.3f} readback={%dx%d preview:%d cl_fov:%.3f} ok=%s",
            use_custom_res ? "true" : "false", target_w, target_h, target_preview,
            static_cast<double>(target_fov), got_w, got_h, got_p,
            static_cast<double>(got_f), ok ? "true" : "false");
    }
    return ok;
}

bool ApplyMenuCustomResolution(int width, int height) noexcept
{
    // V62.7 explicit SOURCE resolution Apply. V62.6 only changed the
    // r_CustomResPreview variables, which does not change the captured PRESENT
    // backbuffer. Static WHGame evidence proves r_Width/r_Height use the same
    // guarded ICVar integer setter (+0x38). This path runs only on the button.
    if (width <= 0 || height <= 0 || width > 4096 || height > 4096) return false;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* rw = SafeConsoleGetCVar(console, "r_Width");
    void* rh = SafeConsoleGetCVar(console, "r_Height");
    if (!rw || !rh) return false;

    bool ok = true;
    ok = SafeCVarSetInt(rw,width) && ok;
    ok = SafeCVarSetInt(rh,height) && ok;

    // Keep custom-res bookkeeping coherent when those CVars exist, but do not
    // enable Preview: Preview was the V62.6 no-visible-change path.
    if (void* cw = SafeConsoleGetCVar(console,"r_CustomResWidth")) (void)SafeCVarSetInt(cw,width);
    if (void* ch = SafeConsoleGetCVar(console,"r_CustomResHeight")) (void)SafeCVarSetInt(ch,height);
    if (void* cp = SafeConsoleGetCVar(console,"r_CustomResPreview")) (void)SafeCVarSetInt(cp,0);

    const int got_w = SafeCVarGetInt(rw,-1);
    const int got_h = SafeCVarGetInt(rh,-1);
    ok = ok && got_w==width && got_h==height;
    char msg[384]{};
    std::snprintf(msg,sizeof(msg),
        "V62.7-SOURCE-RES Apply requested=%dx%d r_Width/r_Height readback=%dx%d ok=%s; waiting for KCD2 PRESENT recreation (XR eye target unchanged)",
        width,height,got_w,got_h,ok?"true":"false");
    debug_trace::Write(msg);
    diagnostic_trace::Write(msg);
    return ok;
}

bool RestoreEngineCVars(const EngineCVarSnapshot& original, char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    return ApplyEngineCVarTest(original, original.custom_width, original.custom_height,
                               original.cl_fov, false, log_buf, log_cap);
}

struct FpsCVarSnapshot
{
    bool valid = false;
    float sys_max_fps = 0.0f;
    int r_vsync = -999;
    int duplicate_last_frame = -999;
};

bool CaptureFpsCVarSnapshot(FpsCVarSnapshot& out) noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* maxfps = SafeConsoleGetCVar(console, "sys_MaxFPS");
    void* vsync = SafeConsoleGetCVar(console, "r_Vsync");
    void* dup = SafeConsoleGetCVar(console, "r_DuplicateLastFrameWH");
    if (!maxfps) return false;
    out.sys_max_fps = SafeCVarGetFloat(maxfps, 0.0f);
    if (vsync) out.r_vsync = SafeCVarGetInt(vsync, -999);
    if (dup) out.duplicate_last_frame = SafeCVarGetInt(dup, -999);
    out.valid = true;
    return true;
}

bool ApplyFpsCap(const FpsCVarSnapshot& original, float cap, char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    void* maxfps = SafeConsoleGetCVar(console, "sys_MaxFPS");
    if (!console || !maxfps) return false;
    const float target = cap > 1.0f ? cap : original.sys_max_fps;
    const bool ok = SafeCVarSetFloat(maxfps, target);
    const float got = SafeCVarGetFloat(maxfps, -1.0f);
    if (log_buf && log_cap) std::snprintf(log_buf, log_cap,
        "V49-FPS-CAP target=%.3f readback=%.3f original=%.3f ok=%s",
        static_cast<double>(target), static_cast<double>(got), static_cast<double>(original.sys_max_fps), ok ? "true" : "false");
    return ok;
}



bool ApplyDuplicateLastFrame(int value, char* log_buf, std::size_t log_cap) noexcept
{
    void* console = SafeGetCryConsole();
    void* dup = console ? SafeConsoleGetCVar(console, "r_DuplicateLastFrameWH") : nullptr;
    if (!dup) { if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,"V50-DUPLICATE-LAST unavailable"); return false; }
    const bool ok = SafeCVarSetInt(dup, value);
    const int got = SafeCVarGetInt(dup, -999);
    if(log_buf&&log_cap) std::snprintf(log_buf,log_cap,"V50-DUPLICATE-LAST target=%d readback=%d ok=%s",value,got,ok?"true":"false");
    return ok && got==value;
}

// V46.3 safe VR graphics profile. This is intentionally conservative: it only
// applies ordinary CVars already exercised by Warhorse's shipped quality
// presets. It does NOT force stereo-frustum bypass, occlusion, shadow cascade,
// TAA/history, screen-space effect, or HLOD internals. Those remain diagnosis
// knobs until the native eye path is proven.
struct VrGraphicsSnapshot
{
    bool valid = false;
    float view_dist = 0.0f;
    float vegetation_dist = 0.0f;
    float merged_mesh_dist = 0.0f;
    float sharpening = 0.0f;
    int motion_blur = 0;
};

bool CaptureVrGraphicsSnapshot(VrGraphicsSnapshot& out) noexcept
{
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    if (!view || !veg || !merged || !sharp || !blur) return false;
    out.view_dist = SafeCVarGetFloat(view, 0.0f);
    out.vegetation_dist = SafeCVarGetFloat(veg, 0.0f);
    out.merged_mesh_dist = SafeCVarGetFloat(merged, 0.0f);
    out.sharpening = SafeCVarGetFloat(sharp, 0.0f);
    out.motion_blur = SafeCVarGetInt(blur, 0);
    // Reject a bad/exception fallback snapshot rather than ever replacing an
    // unknown renderer state with our defaults.
    if (out.view_dist <= 0.0f || out.vegetation_dist <= 0.0f || out.merged_mesh_dist <= 0.0f)
        return false;
    out.valid = true;
    return true;
}

bool ApplyVrBalancedGraphics(const VrGraphicsSnapshot& original, float clarity,
                             char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    if (!view || !veg || !merged || !sharp || !blur) return false;

    // V61.9: cull pose-sync fixed the edge-pop regression; return the profile to the cheaper 120/120/120 baseline.
    const float target_view = std::max(original.view_dist, 120.0f);
    const float target_veg = std::max(original.vegetation_dist, 120.0f);
    const float target_merged = std::max(original.merged_mesh_dist, 120.0f);
    const float requested_clarity = std::clamp(clarity, 0.0f, 0.75f);
    const float target_sharp = std::max(original.sharpening, requested_clarity);

    bool ok = true;
    ok = SafeCVarSetFloat(view, target_view) && ok;
    ok = SafeCVarSetFloat(veg, target_veg) && ok;
    ok = SafeCVarSetFloat(merged, target_merged) && ok;
    ok = SafeCVarSetInt(blur, 0) && ok; // no camera/object motion blur in VR baseline
    ok = SafeCVarSetFloat(sharp, target_sharp) && ok;

    const float got_view = SafeCVarGetFloat(view, -1.0f);
    const float got_veg = SafeCVarGetFloat(veg, -1.0f);
    const float got_merged = SafeCVarGetFloat(merged, -1.0f);
    const int got_blur = SafeCVarGetInt(blur, -1);
    const float got_sharp = SafeCVarGetFloat(sharp, -1.0f);
    if (log_buf && log_cap) {
        std::snprintf(log_buf, log_cap,
            "V46.3-GRAPHICS-APPLY original={view:%.2f veg:%.2f merged:%.2f blur:%d sharp:%.3f} target={view:%.2f veg:%.2f merged:%.2f blur:0 sharp:%.3f} readback={view:%.2f veg:%.2f merged:%.2f blur:%d sharp:%.3f} ok=%s",
            static_cast<double>(original.view_dist), static_cast<double>(original.vegetation_dist),
            static_cast<double>(original.merged_mesh_dist), original.motion_blur,
            static_cast<double>(original.sharpening), static_cast<double>(target_view),
            static_cast<double>(target_veg), static_cast<double>(target_merged),
            static_cast<double>(target_sharp), static_cast<double>(got_view), static_cast<double>(got_veg),
            static_cast<double>(got_merged), got_blur, static_cast<double>(got_sharp), ok ? "true" : "false");
    }
    return ok;
}

bool RestoreVrGraphics(const VrGraphicsSnapshot& original, char* log_buf, std::size_t log_cap) noexcept
{
    if (!original.valid) return false;
    void* console = SafeGetCryConsole();
    if (!console) return false;
    void* view = SafeConsoleGetCVar(console, "e_ViewDistRatio");
    void* veg = SafeConsoleGetCVar(console, "e_ViewDistRatioVegetation");
    void* merged = SafeConsoleGetCVar(console, "e_MergedMeshesViewDistRatio");
    void* sharp = SafeConsoleGetCVar(console, "r_Sharpening");
    void* blur = SafeConsoleGetCVar(console, "r_MotionBlur");
    if (!view || !veg || !merged || !sharp || !blur) return false;
    bool ok = true;
    ok = SafeCVarSetFloat(view, original.view_dist) && ok;
    ok = SafeCVarSetFloat(veg, original.vegetation_dist) && ok;
    ok = SafeCVarSetFloat(merged, original.merged_mesh_dist) && ok;
    ok = SafeCVarSetInt(blur, original.motion_blur) && ok;
    ok = SafeCVarSetFloat(sharp, original.sharpening) && ok;
    if (log_buf && log_cap) {
        std::snprintf(log_buf, log_cap,
            "V46.3-GRAPHICS-RESTORE view=%.2f veg=%.2f merged=%.2f blur=%d sharp=%.3f ok=%s",
            static_cast<double>(original.view_dist), static_cast<double>(original.vegetation_dist),
            static_cast<double>(original.merged_mesh_dist), original.motion_blur,
            static_cast<double>(original.sharpening), ok ? "true" : "false");
    }
    return ok;
}

struct WindowSearchContext
{
    DWORD pid = 0;
    HWND best = nullptr;
    long long best_area = 0;
};

BOOL CALLBACK FindProcessWindowProc(HWND hwnd, LPARAM param) noexcept
{
    auto* ctx = reinterpret_cast<WindowSearchContext*>(param);
    if (!ctx || !IsWindowVisible(hwnd) || GetWindow(hwnd, GW_OWNER) != nullptr) return TRUE;
    DWORD pid = 0; GetWindowThreadProcessId(hwnd, &pid);
    if (pid != ctx->pid) return TRUE;
    RECT r{}; if (!GetClientRect(hwnd, &r)) return TRUE;
    const long long w = static_cast<long long>(r.right - r.left);
    const long long h = static_cast<long long>(r.bottom - r.top);
    const long long area = w * h;
    if (w >= 640 && h >= 480 && area > ctx->best_area) {
        ctx->best = hwnd;
        ctx->best_area = area;
    }
    return TRUE;
}

HWND FindMainGameWindow() noexcept
{
    WindowSearchContext ctx{};
    ctx.pid = GetCurrentProcessId();
    EnumWindows(&FindProcessWindowProc, reinterpret_cast<LPARAM>(&ctx));
    return ctx.best;
}


struct FlatUiPointerInjector
{
    bool trigger_down=false;
    bool was_active=false;
    void Release() noexcept {
        if(trigger_down){ INPUT i{}; i.type=INPUT_MOUSE; i.mi.dwFlags=MOUSEEVENTF_LEFTUP; SendInput(1,&i,sizeof(i)); trigger_down=false; }
        was_active=false;
    }
    void Update(HWND hwnd,bool active,float u,float v,float trigger,bool a_pressed) noexcept {
        if(!active||!hwnd){ Release(); return; }
        RECT rc{}; if(!GetClientRect(hwnd,&rc)){ Release(); return; }
        const int w=rc.right-rc.left,h=rc.bottom-rc.top; if(w<=1||h<=1){ Release(); return; }
        POINT p{std::clamp(int(std::lround(u*double(w-1))),0,w-1),std::clamp(int(std::lround(v*double(h-1))),0,h-1)};
        if(ClientToScreen(hwnd,&p)) SetCursorPos(p.x,p.y);
        const bool want=trigger>0.55f;
        if(want!=trigger_down){ INPUT i{}; i.type=INPUT_MOUSE; i.mi.dwFlags=want?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP; SendInput(1,&i,sizeof(i)); trigger_down=want; }
        if(a_pressed){ INPUT ii[2]{}; ii[0].type=ii[1].type=INPUT_MOUSE; ii[0].mi.dwFlags=MOUSEEVENTF_LEFTDOWN; ii[1].mi.dwFlags=MOUSEEVENTF_LEFTUP; SendInput(2,ii,sizeof(INPUT)); }
        if(!was_active){ was_active=true; debug_trace::Write("V62.9-F6-POINTER Touch AIM -> game UI mouse active"); }
    }
};

} // namespace

struct V603ViewPreset {
    int presentation_mode;
    float fov_asymmetry;
    float uv_binocular;
    bool angular_correct;
    int pose_honesty;
    const char* name;
};

static V603ViewPreset ViewPresetForMode(int mode) noexcept
{
    // V61.39 release rollback: modes 0 and 1 intentionally share the exact
    // proven GOOD MONO source mapping.  Mode 1 differs only by enabling the
    // optical-center / Center-Y correction in the runtime loop.  Do NOT route
    // corrected mono through the matched-XR-FOV remapper (mapMode 4).
    switch (mode) {
    case 1: return {3, 0.00f, 0.000f, true, 1, "MONO_CORRECTED_GOODMAP"};
    case 2: return {3, 0.90f, 0.000f, true, 1, "PSEUDO_090"};
    case 3: return {3, 1.00f, 0.000f, true, 1, "PSEUDO_DEPTH_070_100"};
    default:return {3, 0.00f, 0.000f, true, 1, "MONO_OLD"};
    }
}

static int MonoMapModeForCleanMode(int mode) noexcept
{
    // 0/1 = the old stable GOOD MONO blit.  Only deliberate pseudo modes use
    // the matched submitted-FOV remapper introduced by the pseudo-stereo work.
    return mode >= 2 ? 4 : 0;
}

static float CyclopeanAsymmetryForMode(int mode) noexcept
{
    return ViewPresetForMode(mode).fov_asymmetry;
}

Mod& Mod::Instance()
{
    static Mod instance;
    return instance;
}

void Mod::FillMenuState(VrMenuState& out) const
{
    out.enabled = enabled_.load(std::memory_order_acquire);
    out.bridge_enabled = enable_vr_output_bridge_.load(std::memory_order_acquire);
    out.head_rotation = apply_head_rotation_.load(std::memory_order_acquire);
    out.head_position = apply_head_position_.load(std::memory_order_acquire);
    out.diagnostic_rendering = diagnostic_rendering_.load(std::memory_order_acquire);
    out.swap_eyes = vr_swap_eyes_.load(std::memory_order_acquire);
    out.rotation_order = vr_rotation_order_.load(std::memory_order_acquire);
    out.fake_dof_active = fake_dof_active_.load(std::memory_order_acquire);
    out.fake_dof_scale = fake_dof_scale_.load(std::memory_order_acquire);
    out.engine_dof_active = engine_dof_active_.load(std::memory_order_acquire);
    out.engine_dof_available = engine_dof_available_.load(std::memory_order_acquire);
    out.engine_dof_focus_m = engine_dof_focus_m_.load(std::memory_order_acquire);
    out.engine_dof_scale = engine_dof_scale_.load(std::memory_order_acquire);
    out.depth_stereo_strength = vr_depth_stereo_strength_.load(std::memory_order_acquire);
    out.head_pose_scale = head_pose_scale_.load(std::memory_order_acquire);
    out.camera_side_offset_m = camera_side_offset_m_.load(std::memory_order_acquire);
    out.camera_back_offset_m = camera_back_offset_m_.load(std::memory_order_acquire);
    out.camera_height_offset_m = camera_height_offset_m_.load(std::memory_order_acquire);
    out.game_fov_target = menu_game_fov_target_.load(std::memory_order_acquire);
    out.horizontal_fov_target = menu_horizontal_fov_target_.load(std::memory_order_acquire);
    out.fps_cap_target = menu_fps_cap_target_.load(std::memory_order_acquire);
    out.merged_mesh_target = menu_merged_mesh_target_.load(std::memory_order_acquire);
    out.auto_projection_active = vr_auto_projection_active_.load(std::memory_order_acquire);
    out.auto_fov_trim_deg = vr_auto_fov_trim_deg_.load(std::memory_order_acquire);
    out.auto_center_y_trim = vr_auto_center_y_trim_.load(std::memory_order_acquire);
    out.presentation_mode = vr_presentation_mode_.load(std::memory_order_acquire);
    out.presentation_zoom = vr_zoom_.load(std::memory_order_acquire);
    out.source_vertical_fov_deg = vr_source_vertical_fov_deg_.load(std::memory_order_acquire);
    out.presentation_center_y = vr_presentation_center_y_.load(std::memory_order_acquire);
    out.cull_frustum_scale = vr_cull_frustum_scale_.load(std::memory_order_acquire);
    out.native_ui_scale = vr_native_ui_scale_.load(std::memory_order_acquire);
    out.dialogue_text_scale = vr_dialogue_text_scale_.load(std::memory_order_acquire);
    out.hud_live_observed = flashui_probe::HasRecentHud();
    out.ui_runtime_build_supported = IsSupportedWhGameBuildV6143();
    out.flat_screen_scale = vr_flat_screen_scale_.load(std::memory_order_acquire);
    out.transient_zoom_retain = vr_transient_zoom_retain_.load(std::memory_order_acquire);
    out.hud_xml_scale = hud_xml_scale_.load(std::memory_order_acquire);
    out.hud_xml_offset_x = hud_xml_offset_x_.load(std::memory_order_acquire);
    out.hud_xml_offset_y = hud_xml_offset_y_.load(std::memory_order_acquire);
    out.flat_ui_ipd_mm = vr_flat_ui_ipd_mm_.load(std::memory_order_acquire);
    out.stereo_mode = vr_stereo_experiment_.load(std::memory_order_acquire);
    out.cyclopean_mode = vr_cyclopean_mode_.load(std::memory_order_acquire);
    out.afr_ipd_scale = vr_afr_ipd_scale_.load(std::memory_order_acquire);
    out.balanced_graphics = enable_vr_balanced_graphics_.load(std::memory_order_acquire);
    out.fix_head_sync = (vr_pose_source_mode_.load(std::memory_order_acquire) == 2);
    out.vr_ui_layout = enable_vr_ui_layout_.load(std::memory_order_acquire);
    out.vr_ui_subtitle_scale = vr_ui_subtitle_scale_.load(std::memory_order_acquire);
    out.vr_ui_subtitle_y = vr_ui_subtitle_y_.load(std::memory_order_acquire);
    out.vr_ui_dialogue_scale = vr_ui_dialogue_scale_.load(std::memory_order_acquire);
    out.vr_ui_dialogue_y = vr_ui_dialogue_y_.load(std::memory_order_acquire);
    out.vr_ui_dialogue_inset_pct = vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire);
    out.vr_ui_hints_scale = vr_ui_hints_scale_.load(std::memory_order_acquire);
    out.vr_ui_hints_inset_pct = vr_ui_hints_inset_pct_.load(std::memory_order_acquire);
    out.vr_ui_hints_y = vr_ui_hints_y_.load(std::memory_order_acquire);
    out.vr_ui_compass_scale = vr_ui_compass_scale_.load(std::memory_order_acquire);
    out.vr_ui_compass_y = vr_ui_compass_y_.load(std::memory_order_acquire);
    out.vr_ui_stats_scale = vr_ui_stats_scale_.load(std::memory_order_acquire);
    out.vr_ui_stats_x = vr_ui_stats_x_.load(std::memory_order_acquire);
    out.vr_ui_stats_y = vr_ui_stats_y_.load(std::memory_order_acquire);
    out.vr_ui_lower_scale = vr_ui_lower_scale_.load(std::memory_order_acquire);
    out.vr_ui_lower_y = vr_ui_lower_y_.load(std::memory_order_acquire);
    out.vr_ui_lower_spread = vr_ui_lower_spread_.load(std::memory_order_acquire);
    out.vr_ui_hide_letterbox = vr_ui_hide_letterbox_.load(std::memory_order_acquire);
    out.graphics_profile_applied = graphics_profile_applied_for_ui_.load(std::memory_order_acquire);
    out.render_scale = vr_render_scale_.load(std::memory_order_acquire);
    out.engine_clarity = vr_engine_clarity_.load(std::memory_order_acquire);
    out.gamma = vr_gamma_.load(std::memory_order_acquire);
    out.brightness = vr_brightness_.load(std::memory_order_acquire);
    out.contrast = vr_contrast_.load(std::memory_order_acquire);
    out.hud_crosshair_visible = hud_crosshair_visible_.load(std::memory_order_acquire);
    out.hud_lower_icons_visible = hud_lower_icons_visible_.load(std::memory_order_acquire);
    out.hud_compass_visible = hud_compass_visible_.load(std::memory_order_acquire);
    out.dialogue_camera_mode = vr_dialogue_camera_mode_.load(std::memory_order_acquire);
    out.touch_bindings = enable_touch_bindings_.load(std::memory_order_acquire);
    out.touch_attack_direction = enable_touch_attack_direction_.load(std::memory_order_acquire);
    out.matrix_running = matrix_running_for_ui_.load(std::memory_order_acquire);

    XrManager* xr = xr_manager_;
    if (xr) {
        out.xr_running = xr->IsRunning();
        xr->GetEyeSize(out.eye_width, out.eye_height);
        // V61.43 PERF: this used to lock/copy the render frame and run 8x tan +
        // atan every menu refresh. The XR coverage value is now calculated by
        // the existing 5 Hz auto-projection update and published atomically.
        out.required_source_vfov_deg = vr_required_source_vfov_deg_.load(std::memory_order_acquire);
        XrManager::RenderFrameState frame{};
        if (xr->CopyActiveRenderFrameState(frame)) out.raw_ipd_m = frame.raw_ipd;
        const auto tc = xr->GetTestCounters();
        out.xr_frames = tc.xr_frames;
        out.valid_snapshots = tc.valid_frame_snapshots;
    }
    out.vrik_probe_enabled = enable_vrik_ownership_test_.load(std::memory_order_acquire);
    {
        const auto vs = frik_pose_probe::GetStatus();
        out.vrik_mode = vs.mode;
        out.vrik_rig_valid = vs.rig_valid;
        out.vrik_joint_count = vs.joint_count;
        out.vrik_hand_r = vs.hand_r;
        out.vrik_correlated = vs.correlated_calls;
        out.vrik_final_writes = vs.final_output_writes;
        out.vrik_prebuild_writes = vs.prebuild_writes;
        out.vrik_rejects = vs.rejects;
    }
    out.present_captures = DeviceHook::PresentCaptureCount();
    out.recursive_requests = RecursiveViewProbe::RecursiveRequestCount();
    out.recursive_passes = RecursiveViewProbe::RecursivePassCount();
    out.recursive_pipeline = FrameSync::PipelineCount(1);
    out.recursive_rt = DeviceHook::RecursiveRenderTargetBarrierCount();
}

bool Mod::ApplyMenuState(const VrMenuState& state)
{
    if (matrix_running_for_ui_.load(std::memory_order_acquire)) return false;

    const int presentation = std::clamp(state.presentation_mode, 0, 3);
    const int stereo = 0; // V61.39: AFR/DUAL release activation hard-disabled
    const int cyclopean = std::clamp(state.cyclopean_mode, 0, 3);
    const float head_scale = std::clamp(state.head_pose_scale, 0.80f, 1.20f);
    const float camera_side = std::clamp(state.camera_side_offset_m, -0.30f, 0.30f);
    const float camera_back = std::clamp(state.camera_back_offset_m, -0.30f, 0.30f);
    const float camera_height = std::clamp(state.camera_height_offset_m, -0.30f, 0.30f);
    const float zoom = std::clamp(state.presentation_zoom, 0.85f, 1.20f);
    const float game_fov_target = std::clamp(state.game_fov_target,70.0f,130.0f);
    const float horizontal_fov_target = std::clamp(state.horizontal_fov_target,70.0f,170.0f);
    const float fps_cap_target = std::clamp(state.fps_cap_target,60.0f,120.0f);
    const float merged_mesh_target = std::clamp(state.merged_mesh_target,30.0f,300.0f);
    const float auto_fov_trim = std::clamp(state.auto_fov_trim_deg,-10.0f,10.0f);
    const float auto_center_trim = std::clamp(state.auto_center_y_trim,-0.12f,0.12f);
    const float source_fov = std::clamp(state.source_vertical_fov_deg, 70.0f, 140.0f);
    const float center_y = std::clamp(state.presentation_center_y, -0.20f, 0.20f);
    const float cull_scale = std::clamp(state.cull_frustum_scale, 1.0f, 1.60f);
    const float native_ui_scale = std::clamp(state.native_ui_scale, 0.50f, 1.20f);
    const float dialogue_text_scale = std::clamp(state.dialogue_text_scale, 0.50f, 1.50f);
    const float flat_screen_scale = std::clamp(state.flat_screen_scale, 0.45f, 3.00f);
    const float transient_zoom_retain = std::clamp(state.transient_zoom_retain, 0.0f, 1.0f);
    const float hud_xml_scale = std::clamp(state.hud_xml_scale, 0.50f, 1.10f);
    const int hud_xml_x = std::clamp(state.hud_xml_offset_x, -700, 700);
    const int hud_xml_y = std::clamp(state.hud_xml_offset_y, -700, 700);
    const float flat_ui_ipd_mm = std::clamp(state.flat_ui_ipd_mm, -80.0f, 80.0f);
    const float ipd_scale = 0.0f; // V61.39: retired AFR/DUAL path cannot be re-armed by stale menu state
    const float render_scale = std::clamp(state.render_scale, 0.50f, 2.0f);
    const float engine_clarity = std::clamp(state.engine_clarity, 0.0f, 0.75f);
    const float gamma = std::clamp(state.gamma,0.50f,2.00f);
    const float brightness = std::clamp(state.brightness,-0.50f,0.50f);
    const float contrast = std::clamp(state.contrast,0.50f,2.00f);
    const float depth_stereo_strength = std::clamp(state.depth_stereo_strength, 0.0f, 1.0f);
    const int dialogue_mode = std::clamp(state.dialogue_camera_mode, 0, 2);
    const int rotation_order = state.rotation_order ? 1 : 0;
    const int pose_source_mode = state.fix_head_sync ? 2 : 0; // V62.4 checkbox owns only the proven A/B modes
    const float ui_sub_scale = std::clamp(state.vr_ui_subtitle_scale,0.30f,1.25f);
    const float ui_sub_y = std::clamp(state.vr_ui_subtitle_y,-600.0f,600.0f);
    const float ui_dialog_scale = std::clamp(state.vr_ui_dialogue_scale,0.30f,1.50f);
    const float ui_dialog_y = std::clamp(state.vr_ui_dialogue_y,-720.0f,720.0f);
    const float ui_dialog_inset = std::clamp(state.vr_ui_dialogue_inset_pct,0.0f,80.0f);
    const float ui_hints_scale = std::clamp(state.vr_ui_hints_scale,0.50f,1.50f);
    const float ui_hints_inset = std::clamp(state.vr_ui_hints_inset_pct,0.0f,75.0f);
    const float ui_hints_y = std::clamp(state.vr_ui_hints_y,-600.0f,600.0f);
    const float ui_compass_scale=std::clamp(state.vr_ui_compass_scale,0.45f,1.50f);
    const float ui_compass_y=std::clamp(state.vr_ui_compass_y,-20.0f,100.0f);
    const float ui_stats_scale=std::clamp(state.vr_ui_stats_scale,0.45f,1.50f);
    const float ui_stats_x=std::clamp(state.vr_ui_stats_x,-700.0f,700.0f);
    const float ui_stats_y=std::clamp(state.vr_ui_stats_y,-20.0f,100.0f);
    const float ui_lower_scale=std::clamp(state.vr_ui_lower_scale,0.45f,1.50f);
    const float ui_lower_y=std::clamp(state.vr_ui_lower_y,-20.0f,60.0f);
    const float ui_lower_spread=std::clamp(state.vr_ui_lower_spread,-20.0f,50.0f);

    // Enabled is persisted by the menu but remains a restart-only master gate.
    enabled_.store(state.enabled, std::memory_order_release);
    enable_vr_output_bridge_.store(state.bridge_enabled, std::memory_order_release);
    apply_head_rotation_.store(state.head_rotation, std::memory_order_release);
    apply_head_position_.store(state.head_position, std::memory_order_release);
    diagnostic_rendering_.store(state.diagnostic_rendering, std::memory_order_release);
    vr_swap_eyes_.store(state.swap_eyes, std::memory_order_release);
    vr_rotation_order_.store(rotation_order, std::memory_order_release);
    vr_pose_source_mode_.store(pose_source_mode, std::memory_order_release);
    enable_vr_ui_layout_.store(state.vr_ui_layout,std::memory_order_release);
    vr_ui_subtitle_scale_.store(ui_sub_scale,std::memory_order_release);
    vr_ui_subtitle_y_.store(ui_sub_y,std::memory_order_release);
    vr_ui_dialogue_scale_.store(ui_dialog_scale,std::memory_order_release);
    vr_ui_dialogue_y_.store(ui_dialog_y,std::memory_order_release);
    vr_ui_dialogue_inset_pct_.store(ui_dialog_inset,std::memory_order_release);
    vr_ui_hints_scale_.store(ui_hints_scale,std::memory_order_release);
    vr_ui_hints_inset_pct_.store(ui_hints_inset,std::memory_order_release);
    vr_ui_hints_y_.store(ui_hints_y,std::memory_order_release);
    vr_ui_compass_scale_.store(ui_compass_scale,std::memory_order_release);
    vr_ui_compass_y_.store(ui_compass_y,std::memory_order_release);
    vr_ui_stats_scale_.store(ui_stats_scale,std::memory_order_release);
    vr_ui_stats_x_.store(ui_stats_x,std::memory_order_release);
    vr_ui_stats_y_.store(ui_stats_y,std::memory_order_release);
    vr_ui_lower_scale_.store(ui_lower_scale,std::memory_order_release);
    vr_ui_lower_y_.store(ui_lower_y,std::memory_order_release);
    vr_ui_lower_spread_.store(ui_lower_spread,std::memory_order_release);
    vr_ui_hide_letterbox_.store(state.vr_ui_hide_letterbox,std::memory_order_release);
    head_pose_scale_.store(head_scale, std::memory_order_release);
    camera_side_offset_m_.store(camera_side, std::memory_order_release);
    camera_back_offset_m_.store(camera_back, std::memory_order_release);
    camera_height_offset_m_.store(camera_height, std::memory_order_release);
    menu_game_fov_target_.store(game_fov_target,std::memory_order_release);
    menu_horizontal_fov_target_.store(horizontal_fov_target,std::memory_order_release);
    menu_fps_cap_target_.store(fps_cap_target,std::memory_order_release);
    menu_merged_mesh_target_.store(merged_mesh_target,std::memory_order_release);
    vr_auto_fov_trim_deg_.store(auto_fov_trim,std::memory_order_release);
    vr_auto_center_y_trim_.store(auto_center_trim,std::memory_order_release);
    vr_presentation_mode_.store(presentation, std::memory_order_release);
    vr_zoom_.store(zoom, std::memory_order_release);
    vr_source_vertical_fov_deg_.store(source_fov, std::memory_order_release);
    vr_presentation_center_y_.store(center_y, std::memory_order_release);
    vr_cull_frustum_scale_.store(cull_scale, std::memory_order_release);
    CameraHook::SetCullFrustumScale(1.0f); // V61.5: TP/cull widening retired
    vr_native_ui_scale_.store(native_ui_scale, std::memory_order_release);
    vr_dialogue_text_scale_.store(dialogue_text_scale, std::memory_order_release);
    vr_flat_screen_scale_.store(flat_screen_scale, std::memory_order_release);
    vr_transient_zoom_retain_.store(transient_zoom_retain, std::memory_order_release);
    hud_xml_scale_.store(hud_xml_scale, std::memory_order_release);
    hud_xml_offset_x_.store(hud_xml_x, std::memory_order_release);
    hud_xml_offset_y_.store(hud_xml_y, std::memory_order_release);
    vr_flat_ui_ipd_mm_.store(flat_ui_ipd_mm, std::memory_order_release);
    // V58: this value is now ONLY the mono fallback angular-coverage calibration.
    // Do not write it into KCD2 cl_fov. The game's own graphics FOV remains native.
    vr_stereo_experiment_.store(stereo, std::memory_order_release);
    vr_cyclopean_mode_.store(cyclopean, std::memory_order_release);
    vr_afr_ipd_scale_.store(ipd_scale, std::memory_order_release);
    const bool old_balanced = enable_vr_balanced_graphics_.exchange(state.balanced_graphics, std::memory_order_acq_rel);
    const float old_clarity = vr_engine_clarity_.exchange(engine_clarity, std::memory_order_acq_rel);
    vr_render_scale_.store(render_scale, std::memory_order_release);
    vr_gamma_.store(gamma,std::memory_order_release);
    vr_brightness_.store(brightness,std::memory_order_release);
    vr_contrast_.store(contrast,std::memory_order_release);
    hud_crosshair_visible_.store(state.hud_crosshair_visible,std::memory_order_release);
    hud_lower_icons_visible_.store(state.hud_lower_icons_visible,std::memory_order_release);
    hud_compass_visible_.store(state.hud_compass_visible,std::memory_order_release);
    vr_depth_stereo_strength_.store(depth_stereo_strength, std::memory_order_release);
    vr_dialogue_camera_mode_.store(dialogue_mode, std::memory_order_release);
    enable_touch_bindings_.store(state.touch_bindings, std::memory_order_release);
    enable_touch_attack_direction_.store(state.touch_attack_direction, std::memory_order_release);
    touch_bindings_.SetEnabled(state.touch_bindings);
    touch_bindings_.SetAttackDirectionMouseEmulation(state.touch_attack_direction);
    if (old_balanced != state.balanced_graphics || std::fabs(old_clarity - engine_clarity) > 0.0001f)
        graphics_profile_refresh_requested_.store(true, std::memory_order_release);

    CameraHook::SetTrackingOptions(state.head_rotation, state.head_position);
    CameraHook::SetPoseSourceMode(pose_source_mode, 0.0f);
    CameraHook::SetRotationOrder(rotation_order);
    CameraHook::SetCameraCalibrationOffset(camera_side, camera_back, camera_height);
    CameraHook::SetAfrIpd(0.064f, ipd_scale);
    if (xr_manager_) {
        xr_manager_->SetDiagnosticRendering(state.diagnostic_rendering);
        xr_manager_->SetFlatUiScale(flat_screen_scale);
        xr_manager_->SetFlatUiIpdMm(flat_ui_ipd_mm);
        xr_manager_->SetActiveVrBridge(state.bridge_enabled);
        // V61.0: pseudo presets remain available when stereo_mode==0.
        // Explicit AFR/DUAL tests bypass pseudo UV disparity and use the matched-B tangent map.
        const V603ViewPreset preset = ViewPresetForMode(cyclopean);
        int active_presentation = preset.presentation_mode;
        float active_uv = preset.uv_binocular;
        float active_asym = preset.fov_asymmetry;
        int active_map_mode = MonoMapModeForCleanMode(cyclopean);
        if (stereo == 1 || stereo == 2) {
            active_presentation = 3;
            active_uv = 0.0f;
            active_asym = 1.0f;
            active_map_mode = 4; // source mapping uses the same XrFovf submitted to OpenXR
        }
        vr_presentation_mode_.store(active_presentation, std::memory_order_release);
        vr_zoom_.store(zoom, std::memory_order_release);
        xr_manager_->SetPresentationSettings(
            render_scale, active_presentation, zoom,
            0.0f, 0.0f, 0.0f, 0.0f, source_fov);
        xr_manager_->SetAngularCorrectMono(stereo == 0 ? preset.angular_correct : true);
        xr_manager_->SetPoseHonestyOverride(stereo == 0 ? preset.pose_honesty : 1);
        xr_manager_->SetMonoFovAsymmetryScale(active_asym);
        xr_manager_->SetMonoUvReprojectionMode(active_map_mode);
        xr_manager_->SetMonoUvBinocularOffset(active_uv);
        xr_manager_->SetDepthPushWeight(depth_stereo_strength);
        xr_manager_->SetDepthPushEnabled(cyclopean == 3);
        if (DeviceHook::GenericDepthProbeEnabled() != (cyclopean == 3)) DeviceHook::SetGenericDepthProbe(cyclopean == 3);
        const float applied_center_y = cyclopean == 0 ? 0.0f : center_y;
        vr_presentation_center_y_.store(applied_center_y, std::memory_order_release);
        xr_manager_->SetPresentationCenterY(applied_center_y);
        xr_manager_->SetSwapEyes(state.swap_eyes);
        if (stereo == 1)
            xr_manager_->SetStereoExperiment(1, ipd_scale);
        else
            xr_manager_->SetStereoExperiment(0, 0.0f);
        if (stereo == 0 && cyclopean == 0)
            debug_trace::Write("V61.0-MONO invariant requested: stereo/AFR/native-dual off; matched pseudo shifts cleared");
        xr_manager_->SetSharpening(engine_clarity);
    }
    // V60.5: MONO is a hard state gate, not just a one-shot reset.  V60.4
    // reset the XR manager above and then immediately re-applied the stale
    // persisted stereo mode here, which could bring the doubled view back.
    // V61.0 explicit renderer-test modes. AFR is frame-locked at ViewCamera publication.
    // DUAL is a diagnostic CryEngine/native CRenderView path and leaves the PRESENT bridge mono.
    if (stereo == 1) {
        if (g_v610_manual_dual_active) { RestoreV54StereoCVars(g_v610_manual_stereo_original); g_v610_manual_dual_active=false; }
        FrameSync::SetNativeStereoReentryMode(0, 0.064f);
        DeviceHook::SetAfrStereoCapture(true);
        FrameSync::SetStereoMode(1);
    } else if (stereo == 2) {
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        float runtime_ipd=0.064f; XrManager::StereoViewSnapshot sv{};
        if (xr_manager_ && xr_manager_->CopyLatestStereoViews(sv) && sv.raw_ipd>0.03f && sv.raw_ipd<0.10f) runtime_ipd=sv.raw_ipd;
        const float test_ipd=std::clamp(runtime_ipd*std::max(0.05f,std::fabs(ipd_scale)),0.004f,0.095f);
        if (!g_v610_manual_stereo_original.valid) g_v610_manual_stereo_original=CaptureV54StereoCVars();
        ApplyV54StereoCVars(test_ipd, state.swap_eyes);
        g_v610_manual_dual_active=true;
        FrameSync::SetNativeStereoReentryMode(2, test_ipd);
        char ds[256]{}; std::snprintf(ds,sizeof(ds),"V61.0-DUAL-MENU engineStereo=ON reentry=selector01 testIpd=%.4fm scale=%.2f",(double)test_ipd,(double)ipd_scale); debug_trace::Write(ds);
    } else {
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        FrameSync::SetNativeStereoReentryMode(0, 0.064f);
        CameraHook::SetAfrIpd(0.064f, 0.0f);
        if (g_v610_manual_dual_active) { RestoreV54StereoCVars(g_v610_manual_stereo_original); g_v610_manual_dual_active=false; }
    }

    char m[320]{};
    std::snprintf(m, sizeof(m),
        "V62.4-MENU-APPLY bridge=%s tracking=%s/%s camSide=%.3fm camBack=%.3fm camHeight=%.3fm present=%d zoom=%.2f sourceVFov=%.1f cullScale=%.2f legacyHudScale=%.2f flatScreenScale=%.2f zoomRetain=%.2f stereo=%d cyclo=%d asym=%.3f afrIpd=%.2f swap=%s balanced=%s clarity=%.2f renderScale=%.2f dialogueMode=%d touch=%s",
        state.bridge_enabled?"true":"false", state.head_rotation?"rot":"-", state.head_position?"pos":"-",
        static_cast<double>(camera_side), static_cast<double>(camera_back), static_cast<double>(camera_height), presentation, static_cast<double>(zoom), static_cast<double>(source_fov), static_cast<double>(cull_scale), static_cast<double>(native_ui_scale), static_cast<double>(flat_screen_scale), static_cast<double>(transient_zoom_retain), stereo, cyclopean, static_cast<double>(CyclopeanAsymmetryForMode(cyclopean)),
        static_cast<double>(ipd_scale), state.swap_eyes?"true":"false", state.balanced_graphics?"true":"false",
        static_cast<double>(engine_clarity), static_cast<double>(render_scale), dialogue_mode, state.touch_bindings?"true":"false");
    debug_trace::Write(m);
    return true;
}

bool Mod::SaveMenuSettings() const
{
    char path[MAX_PATH * 2]{};
    if (!GetLoadedModuleSiblingPath("KCD2VR.ini", path, static_cast<DWORD>(sizeof(path)))) return false;
    auto wb = [&](const char* key, bool v) { return WritePrivateProfileStringA("Settings", key, v?"true":"false", path) != FALSE; };
    auto wi = [&](const char* key, int v) { char b[64]{}; std::snprintf(b,sizeof(b),"%d",v); return WritePrivateProfileStringA("Settings",key,b,path) != FALSE; };
    auto wf = [&](const char* key, float v) { char b[64]{}; std::snprintf(b,sizeof(b),"%.6g",static_cast<double>(v)); return WritePrivateProfileStringA("Settings",key,b,path) != FALSE; };
    bool ok = true;
    ok = wb("Enabled", enabled_.load(std::memory_order_acquire)) && ok;
    ok = wf("HeadPoseScale", head_pose_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraSideOffsetMeters", camera_side_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraBackOffsetMeters", camera_back_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCameraHeightOffsetMeters", camera_height_offset_m_.load(std::memory_order_acquire)) && ok;
    ok = wb("ApplyHeadRotation", apply_head_rotation_.load(std::memory_order_acquire)) && ok;
    ok = wb("ApplyHeadPosition", apply_head_position_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrPoseSourceMode", vr_pose_source_mode_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableVrUiLayout", enable_vr_ui_layout_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiSubtitleScale", vr_ui_subtitle_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiSubtitleY", vr_ui_subtitle_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiDialogueScale", vr_ui_dialogue_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiDialogueY", vr_ui_dialogue_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiDialogueInsetPct", vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiHintsScale", vr_ui_hints_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiHintsInsetPct", vr_ui_hints_inset_pct_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiHintsY", vr_ui_hints_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCompassScale", vr_ui_compass_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiCompassY", vr_ui_compass_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsScale", vr_ui_stats_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsX", vr_ui_stats_x_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiStatsY", vr_ui_stats_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerScale", vr_ui_lower_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerY", vr_ui_lower_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrUiLowerSpread", vr_ui_lower_spread_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrUiHideLetterbox", vr_ui_hide_letterbox_.load(std::memory_order_acquire)) && ok;
    ok = wb("DiagnosticRendering", diagnostic_rendering_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableVrOutputBridge", enable_vr_output_bridge_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrRenderScale", vr_render_scale_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableVrBalancedGraphics", enable_vr_balanced_graphics_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrEngineClarity", vr_engine_clarity_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrDialogueCameraMode", vr_dialogue_camera_mode_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableTouchBindings", enable_touch_bindings_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableTouchAttackDirectionMouse", enable_touch_attack_direction_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrPresentationMode", vr_presentation_mode_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrPresentationZoom", vr_zoom_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAutoFovTrimDegrees", vr_auto_fov_trim_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAutoCenterYTrim", vr_auto_center_y_trim_.load(std::memory_order_acquire)) && ok;
    ok = wb("EnableBowZoomGuard", enable_bow_zoom_guard_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrSourceVerticalFovDegrees", vr_source_vertical_fov_deg_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrCullFrustumScale", vr_cull_frustum_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrNativeUiScale", vr_native_ui_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrFlatScreenScale", vr_flat_screen_scale_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrTransientZoomRetain", vr_transient_zoom_retain_.load(std::memory_order_acquire)) && ok;
    ok = wf("HudXmlScale", hud_xml_scale_.load(std::memory_order_acquire)) && ok;
    ok = wi("HudXmlOffsetX", hud_xml_offset_x_.load(std::memory_order_acquire)) && ok;
    ok = wi("HudXmlOffsetY", hud_xml_offset_y_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrFlatUiIpdMm", vr_flat_ui_ipd_mm_.load(std::memory_order_acquire)) && ok;
    ok = wb("MirrorMisMenuToDesktop", mirror_mis_menu_to_desktop_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrStereoExperiment", vr_stereo_experiment_.load(std::memory_order_acquire)) && ok;
    ok = wi("VrCyclopeanMode", vr_cyclopean_mode_.load(std::memory_order_acquire)) && ok;
    ok = wf("VrAfrIpdScale", vr_afr_ipd_scale_.load(std::memory_order_acquire)) && ok;
    ok = wb("VrSwapEyes", vr_swap_eyes_.load(std::memory_order_acquire)) && ok;
    return ok;
}

void Mod::OnRenderTick(ID3D12CommandQueue* queue)
{
    Mod& mod = Instance();
    // V58 hybrid ordering: refresh the dedicated menu texture first, let XR
    // copy the CLEAN game backbuffer + separate menu layer, then composite the
    // same book onto the desktop/monitor after the XR capture is complete.
    if (mod.vr_menu_) mod.vr_menu_->OnRenderThreadTick(queue);
    XrManager* xr = mod.xr_manager_;
    if (xr) {
        ID3D12Resource* menu_tex = (mod.vr_menu_ && mod.vr_menu_->IsVisible()) ? mod.vr_menu_->GetQuadTexture() : nullptr;
        xr->SetMenuQuadSource(menu_tex, menu_tex != nullptr);
        xr->OnRenderThreadTick(queue);
    }
    // V58.3: hybrid desktop mirroring is OFF by default. V58.1 could feed a
    // previously composited desktop book back into a later captured backbuffer,
    // producing the user's "monitor view flashing behind the XR menu" symptom.
    if (mod.vr_menu_ && mod.mirror_mis_menu_to_desktop_.load(std::memory_order_acquire))
        mod.vr_menu_->OnDesktopOverlayTick(queue);
}

bool Mod::LoadConfig()
{
    // Logger::configure() sets up the process-default sink; log() returns it.
    // File name resolves relative to the runtime (game binary) directory.
    DetourModKit::Logger::configure("KCD2VR", "KCD2VR.log");
    debug_trace::Write("logger configured; fallback trace active");

    char ini_path[MAX_PATH * 2]{};
    const bool have_ini_path =
        GetLoadedModuleSiblingPath("KCD2VR.ini", ini_path, static_cast<DWORD>(sizeof(ini_path)));

    config::Ini ini;
    if (have_ini_path)
    {
        ini.load(ini_path);
        char trace[768]{};
        std::snprintf(trace, sizeof(trace), "loading config from ASI directory: %s", ini_path);
        debug_trace::Write(trace);
    }
    else
    {
        ini.load("KCD2VR.ini");
        debug_trace::Write("WARNING: could not resolve ASI-relative INI path; using process-relative KCD2VR.ini");
    }

    auto settings = ini.section("Settings");
    settings.bind_log_level("LogLevel", "WARN"); // V61.36 CLEAN default: sparse logger output
    settings.bind("Enabled", enabled_, true);
    settings.bind("HeadPoseScale", head_pose_scale_, 1.0f);
    settings.bind("VrCameraSideOffsetMeters", camera_side_offset_m_, 0.0f);
    settings.bind("VrCameraBackOffsetMeters", camera_back_offset_m_, 0.0f);
    settings.bind("VrCameraHeightOffsetMeters", camera_height_offset_m_, 0.0f);
    settings.bind("ApplyHeadRotation", apply_head_rotation_, true);
    settings.bind("ApplyHeadPosition", apply_head_position_, true);
    settings.bind("VrPoseSourceMode", vr_pose_source_mode_, 2);
    settings.bind("EnableVrUiLayout", enable_vr_ui_layout_, true);
    settings.bind("VrUiSubtitleScale", vr_ui_subtitle_scale_, 0.55f);
    settings.bind("VrUiSubtitleY", vr_ui_subtitle_y_, 0.0f);
    settings.bind("VrUiDialogueScale", vr_ui_dialogue_scale_, 0.70f);
    settings.bind("VrUiDialogueY", vr_ui_dialogue_y_, 0.0f);
    settings.bind("VrUiDialogueInsetPct", vr_ui_dialogue_inset_pct_, 14.0f);
    settings.bind("VrUiHintsScale", vr_ui_hints_scale_, 1.00f);
    settings.bind("VrUiHintsInsetPct", vr_ui_hints_inset_pct_, 35.0f);
    settings.bind("VrUiHintsY", vr_ui_hints_y_, 0.0f);
    settings.bind("VrUiCompassScale", vr_ui_compass_scale_, 1.0f);
    settings.bind("VrUiCompassY", vr_ui_compass_y_, 0.0f);
    settings.bind("VrUiStatsScale", vr_ui_stats_scale_, 1.0f);
    settings.bind("VrUiStatsX", vr_ui_stats_x_, 0.0f);
    settings.bind("VrUiStatsY", vr_ui_stats_y_, 0.0f);
    settings.bind("VrUiLowerScale", vr_ui_lower_scale_, 1.0f);
    settings.bind("VrUiLowerY", vr_ui_lower_y_, 0.0f);
    settings.bind("VrUiLowerSpread", vr_ui_lower_spread_, 0.0f);
    settings.bind("VrUiHideLetterbox", vr_ui_hide_letterbox_, false);
    settings.bind("VrPosePredictionOffsetMs", vr_pose_prediction_offset_ms_, 0.0f);
    settings.bind("DiagnosticRendering", diagnostic_rendering_, false);
    settings.bind("EnableVrOutputBridge", enable_vr_output_bridge_, true);
    settings.bind("VrRenderScale", vr_render_scale_, 1.0f);
    settings.bind("EnableVrBalancedGraphics", enable_vr_balanced_graphics_, true);
    settings.bind("VrEngineClarity", vr_engine_clarity_, 0.18f);
    settings.bind("VrDialogueCameraMode", vr_dialogue_camera_mode_, 1);
    settings.bind("EnableTouchBindings", enable_touch_bindings_, true);
    settings.bind("EnableTouchAttackDirectionMouse", enable_touch_attack_direction_, true);
    settings.bind("VrPresentationMode", vr_presentation_mode_, 3);
    settings.bind("VrPresentationZoom", vr_zoom_, 1.00f);
    settings.bind("VrAutoFovTrimDegrees", vr_auto_fov_trim_deg_, 0.0f);
    settings.bind("VrAutoCenterYTrim", vr_auto_center_y_trim_, 0.0f);
    settings.bind("EnableBowZoomGuard", enable_bow_zoom_guard_, false);
    settings.bind("VrSourceVerticalFovDegrees", vr_source_vertical_fov_deg_, 95.0f);
    settings.bind("VrCullFrustumScale", vr_cull_frustum_scale_, 1.0f);
    settings.bind("VrNativeUiScale", vr_native_ui_scale_, 1.00f);
    settings.bind("VrDialogueTextScale", vr_dialogue_text_scale_, 1.00f);
    settings.bind("VrFlatScreenScale", vr_flat_screen_scale_, 1.70f);
    settings.bind("VrTransientZoomRetain", vr_transient_zoom_retain_, 0.00f);
    settings.bind("VrDepthStereoStrength", vr_depth_stereo_strength_, 0.30f);
    settings.bind("HudXmlScale", hud_xml_scale_, 0.85f);
    settings.bind("HudXmlOffsetX", hud_xml_offset_x_, 0);
    settings.bind("HudXmlOffsetY", hud_xml_offset_y_, 0);
    settings.bind("VrFlatUiIpdMm", vr_flat_ui_ipd_mm_, 0.0f);
    settings.bind("MirrorMisMenuToDesktop", mirror_mis_menu_to_desktop_, false);
    settings.bind("VrLeftOffsetX", vr_left_offset_x_, 0.0f);
    settings.bind("VrLeftOffsetY", vr_left_offset_y_, 0.0f);
    settings.bind("VrRightOffsetX", vr_right_offset_x_, 0.0f);
    settings.bind("VrRightOffsetY", vr_right_offset_y_, 0.0f);
    settings.bind("VrStereoExperiment", vr_stereo_experiment_, 0);
    settings.bind("VrCleanPresentationMode", vr_cyclopean_mode_, 1);
    settings.bind("VrRotationOrder", vr_rotation_order_, 0);
    settings.bind("VrAfrIpdScale", vr_afr_ipd_scale_, 0.0f);
    settings.bind("VrSwapEyes", vr_swap_eyes_, false);
    settings.bind("EnableLegacyStereoProbes", enable_legacy_probes_, false);
    settings.bind("EnableNativeEyeWrite", enable_native_eye_write_, false);


    auto future = ini.section("FutureFeatures");
    future.bind("EnableBodyIK", enable_body_ik_, false);
    future.bind("EnableVirtualHolsters", enable_virtual_holsters_, false);
    future.bind("EnableMotionControls", enable_motion_controls_, false);
    future.bind("EnableNativeMelee", enable_native_melee_, false);
    future.bind("EnableStaminaWeaponHandling", enable_stamina_weapon_handling_, false);
    future.bind("EnablePhysicalGrabbing", enable_physical_grabbing_, false);
    future.bind("EnableNpcPhysicalReaction", enable_npc_physical_reaction_, false);
    future.bind("EnablePhysicalBow", enable_physical_bow_, false);
    future.bind("EnableAlchemyVR", enable_alchemy_vr_, false);
    future.bind("EnableSmithingVR", enable_smithing_vr_, false);
    future.bind("DisableTargetLockSteering", disable_target_lock_steering_, false);
    future.bind("HideCenterCursor", hide_center_cursor_, false);
    future.bind("EnableWristBodyUI", enable_wrist_body_ui_, false);

    auto experimental = ini.section("Experimental");
    experimental.bind("EnableVrikSemanticOwnershipTest", enable_vrik_ownership_test_, false);

    const char* explicit_ini = have_ini_path ? ini_path : "KCD2VR.ini";

    // V40.2: DetourModKit config can report Loaded 0 items for this host build.
    // Explicitly parse every active VR setting through Win32 profile APIs so the
    // ASI-side KCD2VR.ini is authoritative and runtime logs show the real values.
    enabled_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "Enabled", true), std::memory_order_relaxed);
    head_pose_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "HeadPoseScale", 1.0f), std::memory_order_relaxed);
    camera_side_offset_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraSideOffsetMeters", 0.0f), -0.30f, 0.30f), std::memory_order_relaxed);
    camera_back_offset_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraBackOffsetMeters", 0.0f), -0.30f, 0.30f), std::memory_order_relaxed);
    camera_height_offset_m_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCameraHeightOffsetMeters", 0.0f), -0.30f, 0.30f), std::memory_order_relaxed);
    apply_head_rotation_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "ApplyHeadRotation", true), std::memory_order_relaxed);
    apply_head_position_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "ApplyHeadPosition", true), std::memory_order_relaxed);
    vr_pose_source_mode_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrPoseSourceMode", 2),0,3), std::memory_order_relaxed);
    enable_vr_ui_layout_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrUiLayout", true), std::memory_order_relaxed);
    vr_ui_subtitle_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSubtitleScale", 0.55f),0.30f,1.25f),std::memory_order_relaxed);
    vr_ui_subtitle_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiSubtitleY", 0.0f),-600.0f,600.0f),std::memory_order_relaxed);
    vr_ui_dialogue_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueScale", 0.70f),0.30f,1.50f),std::memory_order_relaxed);
    vr_ui_dialogue_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueY", 0.0f),-720.0f,720.0f),std::memory_order_relaxed);
    vr_ui_dialogue_inset_pct_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiDialogueInsetPct", 14.0f),0.0f,80.0f),std::memory_order_relaxed);
    vr_ui_hints_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsScale", 1.00f),0.50f,1.50f),std::memory_order_relaxed);
    vr_ui_hints_inset_pct_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsInsetPct", 35.0f),0.0f,75.0f),std::memory_order_relaxed);
    vr_ui_hints_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrUiHintsY", 0.0f),-600.0f,600.0f),std::memory_order_relaxed);
    vr_ui_compass_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiCompassScale",1.0f),0.45f,1.50f),std::memory_order_relaxed);
    vr_ui_compass_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiCompassY",0.0f),-20.0f,100.0f),std::memory_order_relaxed);
    vr_ui_stats_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsScale",1.0f),0.45f,1.50f),std::memory_order_relaxed);
    vr_ui_stats_x_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsX",0.0f),-700.0f,700.0f),std::memory_order_relaxed);
    vr_ui_stats_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiStatsY",0.0f),-20.0f,100.0f),std::memory_order_relaxed);
    vr_ui_lower_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerScale",1.0f),0.45f,1.50f),std::memory_order_relaxed);
    vr_ui_lower_y_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerY",0.0f),-20.0f,60.0f),std::memory_order_relaxed);
    vr_ui_lower_spread_.store(std::clamp(ReadIniFloatExplicit(explicit_ini,"Settings","VrUiLowerSpread",0.0f),-20.0f,50.0f),std::memory_order_relaxed);
    vr_ui_hide_letterbox_.store(ReadIniBoolExplicit(explicit_ini,"Settings","VrUiHideLetterbox",false),std::memory_order_relaxed);
    vr_pose_prediction_offset_ms_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPosePredictionOffsetMs", 0.0f),-50.0f,50.0f), std::memory_order_relaxed);
    diagnostic_rendering_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "DiagnosticRendering", false), std::memory_order_relaxed);
    enable_vr_output_bridge_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrOutputBridge", true), std::memory_order_relaxed);
    vr_render_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRenderScale", 1.0f), std::memory_order_relaxed);
    enable_vr_balanced_graphics_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableVrBalancedGraphics", true), std::memory_order_relaxed);
    vr_engine_clarity_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrEngineClarity", 0.18f), std::memory_order_relaxed);
    vr_dialogue_camera_mode_.store(ReadIniIntExplicit(explicit_ini, "Settings", "VrDialogueCameraMode", 1), std::memory_order_relaxed);
    enable_touch_bindings_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableTouchBindings", true), std::memory_order_relaxed);
    enable_touch_attack_direction_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableTouchAttackDirectionMouse", true), std::memory_order_relaxed);
    // V62.9: release-facing PC key targets for the fixed Touch action layout.
    // These are ordinary Win32 virtual-key codes; changing them never patches KCD2 input objects.
    {
        TouchBindings::Config tc{};
        tc.move_forward=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveForwardVk",'W'),1,255));
        tc.move_left=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveLeftVk",'A'),1,255));
        tc.move_back=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveBackVk",'S'),1,255));
        tc.move_right=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","MoveRightVk",'D'),1,255));
        tc.sprint=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","SprintVk",VK_LSHIFT),1,255));
        tc.crouch=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","CrouchVk",'C'),1,255));
        tc.jump=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","JumpVk",VK_SPACE),1,255));
        tc.interact=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","InteractVk",'E'),1,255));
        tc.torch=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","TorchVk",'R'),1,255));
        tc.visor=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","VisorVk",'Z'),1,255));
        tc.horse_dog=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","HorseDogVk",'X'),1,255));
        tc.inventory=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","InventoryVk",'I'),1,255));
        tc.pause=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","PauseVk",VK_ESCAPE),1,255));
        tc.quick_left=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","QuickLeftVk",'5'),1,255));
        tc.quick_right=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","QuickRightVk",'6'),1,255));
        tc.weapon_slot1=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","WeaponSlot1Vk",'1'),1,255));
        tc.weapon_slot2=static_cast<unsigned short>(std::clamp(ReadIniIntExplicit(explicit_ini,"Controls","WeaponSlot2Vk",'2'),1,255));
        touch_bindings_.SetConfig(tc);
    }
    vr_presentation_mode_.store(ReadIniIntExplicit(explicit_ini, "Settings", "VrPresentationMode", 3), std::memory_order_relaxed);
    vr_zoom_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrPresentationZoom", 1.00f), std::memory_order_relaxed);
    vr_auto_fov_trim_deg_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrAutoFovTrimDegrees", 0.0f),-10.0f,10.0f), std::memory_order_relaxed);
    vr_auto_center_y_trim_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrAutoCenterYTrim", 0.0f),-0.12f,0.12f), std::memory_order_relaxed);
    enable_bow_zoom_guard_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableBowZoomGuard", false), std::memory_order_relaxed);
    vr_source_vertical_fov_deg_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrSourceVerticalFovDegrees", 95.0f), std::memory_order_relaxed);
    vr_cull_frustum_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrCullFrustumScale", 1.0f),1.0f,1.60f), std::memory_order_relaxed);
    vr_native_ui_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrNativeUiScale", 1.00f),0.50f,1.20f), std::memory_order_relaxed);
    vr_dialogue_text_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrDialogueTextScale", 1.00f),0.50f,1.50f), std::memory_order_relaxed);
    vr_flat_screen_scale_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrFlatScreenScale", 1.70f),0.45f,3.00f), std::memory_order_relaxed);
    vr_transient_zoom_retain_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrTransientZoomRetain", 0.00f),0.0f,1.0f), std::memory_order_relaxed);
    vr_depth_stereo_strength_.store(std::clamp(ReadIniFloatExplicit(explicit_ini, "Settings", "VrDepthStereoStrength", 0.30f),0.0f,1.0f), std::memory_order_relaxed);
    hud_xml_scale_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "HudXmlScale", 0.85f), std::memory_order_relaxed);
    hud_xml_offset_x_.store(ReadIniIntExplicit(explicit_ini, "Settings", "HudXmlOffsetX", 0), std::memory_order_relaxed);
    hud_xml_offset_y_.store(ReadIniIntExplicit(explicit_ini, "Settings", "HudXmlOffsetY", 0), std::memory_order_relaxed);
    vr_flat_ui_ipd_mm_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrFlatUiIpdMm", 0.0f), std::memory_order_relaxed);
    mirror_mis_menu_to_desktop_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "MirrorMisMenuToDesktop", false), std::memory_order_relaxed);
    vr_left_offset_x_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrLeftOffsetX", 0.0f), std::memory_order_relaxed);
    vr_left_offset_y_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrLeftOffsetY", 0.0f), std::memory_order_relaxed);
    vr_right_offset_x_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRightOffsetX", 0.0f), std::memory_order_relaxed);
    vr_right_offset_y_.store(ReadIniFloatExplicit(explicit_ini, "Settings", "VrRightOffsetY", 0.0f), std::memory_order_relaxed);
    vr_stereo_experiment_.store(0, std::memory_order_relaxed); // V61.39: stale AFR/DUAL INI values ignored
    vr_cyclopean_mode_.store(std::clamp(ReadIniIntExplicit(explicit_ini, "Settings", "VrCleanPresentationMode", 1),0,3), std::memory_order_relaxed);
    vr_rotation_order_.store(0, std::memory_order_relaxed); // V61.20 D*B diagnostic retired
    vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed); // V61.39: retired
    vr_swap_eyes_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "VrSwapEyes", false), std::memory_order_relaxed);
    enable_legacy_probes_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableLegacyStereoProbes", false), std::memory_order_relaxed);
    enable_native_eye_write_.store(ReadIniBoolExplicit(explicit_ini, "Settings", "EnableNativeEyeWrite", false), std::memory_order_relaxed);
    enable_vrik_ownership_test_.store(ReadIniBoolExplicit(explicit_ini, "Experimental", "EnableVrikSemanticOwnershipTest", false), std::memory_order_relaxed);

    {
        char trace[320]{};
        std::snprintf(
            trace, sizeof(trace),
            "config parsed: Enabled=%s ApplyHeadRotation=%s ApplyHeadPosition=%s DiagnosticRendering=%s EnableVrOutputBridge=%s EnableNativeEyeWrite=%s HeadPoseScale=%.3f BalancedGraphics=%s Clarity=%.2f",
            enabled_.load(std::memory_order_relaxed) ? "true" : "false",
            apply_head_rotation_.load(std::memory_order_relaxed) ? "true" : "false",
            apply_head_position_.load(std::memory_order_relaxed) ? "true" : "false",
            diagnostic_rendering_.load(std::memory_order_relaxed) ? "true" : "false",
            enable_vr_output_bridge_.load(std::memory_order_relaxed) ? "true" : "false",
            enable_native_eye_write_.load(std::memory_order_relaxed) ? "true" : "false",
            head_pose_scale_.load(std::memory_order_relaxed),
            enable_vr_balanced_graphics_.load(std::memory_order_relaxed) ? "true" : "false",
            vr_engine_clarity_.load(std::memory_order_relaxed));
        debug_trace::Write(trace);
    }

    {
        char trace[768]{};
        std::snprintf(trace, sizeof(trace),
            "future gates (inert until implemented): bodyIK=%s holsters=%s motion=%s melee=%s staminaHandling=%s grabbing=%s npcPhysical=%s bow=%s alchemy=%s smithing=%s noLockSteering=%s hideCursor=%s wristUI=%s",
            enable_body_ik_.load() ? "true" : "false",
            enable_virtual_holsters_.load() ? "true" : "false",
            enable_motion_controls_.load() ? "true" : "false",
            enable_native_melee_.load() ? "true" : "false",
            enable_stamina_weapon_handling_.load() ? "true" : "false",
            enable_physical_grabbing_.load() ? "true" : "false",
            enable_npc_physical_reaction_.load() ? "true" : "false",
            enable_physical_bow_.load() ? "true" : "false",
            enable_alchemy_vr_.load() ? "true" : "false",
            enable_smithing_vr_.load() ? "true" : "false",
            disable_target_lock_steering_.load() ? "true" : "false",
            hide_center_cursor_.load() ? "true" : "false",
            enable_wrist_body_ui_.load() ? "true" : "false");
        debug_trace::Write(trace);
    }

    return true;
}

void Mod::Run()
{
    if (!LoadConfig())
        return;

    log().info("KCD2VR starting up");
    debug_trace::Write("KCD2VR-V62.9-LUA-UI-VRIK-LADDER");
    log().warning("KCD2VR BUILD KCD2VR-V62.9-LUA-UI-VRIK-LADDER");
    diagnostic_trace::Write("KCD2VR-V62.6 diagnostics channel online; file is next to KCD2VR.asi");
    // V61.18 retired diagnostics: fixed composition order and fixed 1:1 head translation.
    vr_rotation_order_.store(0, std::memory_order_relaxed);
    head_pose_scale_.store(1.0f, std::memory_order_relaxed);
    debug_trace::Write("KCD2VR starting up; debug path anchored to loaded ASI directory");

    if (!enabled_.load(std::memory_order_relaxed))
    {
        log().info("KCD2VR disabled via config, exiting");
        return;
    }

    running_ = true;

    camera_hook_ = std::make_unique<CameraHook>();
    CameraHook::SetTrackingOptions(apply_head_rotation_.load(std::memory_order_relaxed),
                                   apply_head_position_.load(std::memory_order_relaxed));
    CameraHook::SetCameraCalibrationOffset(camera_side_offset_m_.load(std::memory_order_relaxed),
                                           camera_back_offset_m_.load(std::memory_order_relaxed),
                                           camera_height_offset_m_.load(std::memory_order_relaxed));
    debug_trace::Write("installing camera hook");
    if (!camera_hook_->Install())
    {
        log().error("Camera hook failed to install - candidate ladder not resolved yet. See ROADMAP.md.");
        running_ = false;
        return;
    }

    debug_trace::Write("camera hook installed");
    CameraHook::SetCullFrustumScale(1.0f); // V61.5: ignore stale INI cull scale
    device_hook_ = std::make_unique<DeviceHook>();
    debug_trace::Write("installing D3D12 device hook");
    if (!device_hook_->Install())
    {
        log().error("Device hook failed to install - cannot capture the game's D3D12 device");
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    log().info("Waiting for the game's D3D12 device/queue to be captured (Proceed to main menu or load a save)...");
    debug_trace::Write("waiting for D3D12 device + DIRECT queue capture");
    ID3D12Device* game_device = nullptr;
    ID3D12CommandQueue* game_queue = nullptr;
    bool captured = false;
    
    // Polling window extended up to 60 seconds to allow time to reach the main menu / load game
    for (int i = 0; i < 600 && running_; ++i) 
    {
        if (device_hook_->TryGetDevice(&game_device, &game_queue))
        {
            captured = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!running_)
    {
        log().info("Shutdown requested while waiting for device capture");
        device_hook_->Uninstall();
        device_hook_.reset();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        return;
    }

    if (!captured)
    {
        log().error("Timed out waiting for the game's D3D12 device to be captured. Make sure you reach the main menu.");
        device_hook_->Uninstall();
        device_hook_.reset();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    debug_trace::Write("D3D12 device + DIRECT queue captured");
    if (enable_legacy_probes_.load(std::memory_order_relaxed)) {
        debug_trace::Write("V45-RECURSIVE legacy probes requested but suppressed: they overlap the ViewCamera publish hook used for frame identity");
    } else {
        debug_trace::Write("V45-LEAN legacy V29-V44 archaeology probes disabled; recursive-view proof only");
    }
    stereo_archaeology::detail::g_native_eye_write_enabled.store(
        enable_native_eye_write_.load(std::memory_order_relaxed), std::memory_order_release);

    if (!FrameSync::Install()) {
        debug_trace::Write("V45-PIPELINE install FAILED; forcing mono bridge for safety");
        vr_stereo_experiment_.store(0, std::memory_order_relaxed);
    }
    FrameSync::SetStereoMode(vr_stereo_experiment_.load(std::memory_order_relaxed));

    if (!RecursiveViewProbe::Install()) {
        debug_trace::Write("V45-RECURSIVE probe install FAILED; VR bridge continues without recursive-view correlation");
    }

    xr_manager_ = &XrManager::Instance();
    xr_manager_->SetDiagnosticRendering(diagnostic_rendering_.load(std::memory_order_relaxed));
    xr_manager_->SetActiveVrBridge(enable_vr_output_bridge_.load(std::memory_order_relaxed));
    xr_manager_->SetPresentationSettings(
        vr_render_scale_.load(std::memory_order_relaxed),
        vr_presentation_mode_.load(std::memory_order_relaxed),
        vr_zoom_.load(std::memory_order_relaxed),
        vr_left_offset_x_.load(std::memory_order_relaxed), vr_left_offset_y_.load(std::memory_order_relaxed),
        vr_right_offset_x_.load(std::memory_order_relaxed), vr_right_offset_y_.load(std::memory_order_relaxed),
        vr_source_vertical_fov_deg_.load(std::memory_order_relaxed));
    xr_manager_->SetStereoExperiment(vr_stereo_experiment_.load(std::memory_order_relaxed),
                                     vr_afr_ipd_scale_.load(std::memory_order_relaxed));
    xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
    xr_manager_->SetFlatUiIpdMm(vr_flat_ui_ipd_mm_.load(std::memory_order_relaxed));
    // V58: this is only the temporary MONO bridge angular-coverage calibration.
    // It no longer writes KCD2 cl_fov; the game's own graphics FOV is untouched.
    const float startup_vr_fov = std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed),70.0f,140.0f);
    const int startup_preset = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed),0,5);
    const V603ViewPreset startup_view = ViewPresetForMode(startup_preset);
    const int startup_present = startup_view.presentation_mode;
    const float startup_framing = std::clamp(vr_zoom_.load(std::memory_order_relaxed), 0.85f, 1.20f);
    const float startup_uv = startup_view.uv_binocular;
    vr_stereo_experiment_.store(0, std::memory_order_relaxed);
    vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed);
    vr_presentation_mode_.store(startup_present, std::memory_order_relaxed);
    vr_zoom_.store(startup_framing, std::memory_order_relaxed);
    vr_source_vertical_fov_deg_.store(startup_vr_fov, std::memory_order_relaxed);
    vr_swap_eyes_.store(false, std::memory_order_relaxed);
    xr_manager_->SetStereoExperiment(0, 0.0f);
    xr_manager_->SetAngularCorrectMono(startup_view.angular_correct);
    xr_manager_->SetPoseHonestyOverride(startup_view.pose_honesty);
    xr_manager_->SetMonoFovAsymmetryScale(startup_view.fov_asymmetry);
    xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(startup_preset));
    xr_manager_->SetMonoUvBinocularOffset(startup_uv);
    xr_manager_->SetPresentationCenterY(vr_presentation_center_y_.load(std::memory_order_relaxed));
    xr_manager_->SetPresentationSettings(vr_render_scale_.load(std::memory_order_relaxed), startup_present, startup_framing,
        0.0f,0.0f,0.0f,0.0f,startup_vr_fov);
    xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
    vr_zoom_.store(startup_framing, std::memory_order_relaxed);
    xr_manager_->SetSwapEyes(false);
    xr_manager_->SetFlatUiScale(vr_flat_screen_scale_.load(std::memory_order_relaxed));
    vr_flat_ui_ipd_mm_.store(0.0f, std::memory_order_relaxed);
    xr_manager_->SetFlatUiIpdMm(0.0f);
    xr_manager_->SetPresentPosePairingMode(1);
    DeviceHook::SetAfrStereoCapture(false);
    FrameSync::SetStereoMode(0);
    CameraHook::SetAfrIpd(0.064f, 0.0f);
    CameraHook::SetRotationFilterMode(5); // V58.7: fixed 8 ms retained; adaptive was decisively worse in headset A/B
    CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed));
    // V61.30 baseline: DOF-focus pseudo is no longer a startup mode. It is a
    // deliberate F4 hypothesis test so GOOD MONO remains the uncontaminated baseline.
    fake_dof_active_.store(false, std::memory_order_release);
    engine_dof_active_.store(false, std::memory_order_release);
    engine_dof_available_.store(false, std::memory_order_release);
    engine_dof_focus_m_.store(0.0f, std::memory_order_release);
    fake_dof_started_ms_.store(0, std::memory_order_release);
    fake_dof_scale_.store(0.95f, std::memory_order_release);
    engine_dof_scale_.store(0.95f, std::memory_order_release);
    debug_trace::Write("V61.32-F4 focus-driven pseudo OFF by default; failed GBuffer wrapper scan retired; fixed 8ms rotation baseline retained");
    { char msg[256]{}; std::snprintf(msg, sizeof(msg),
        "V40-STEREO config experiment=%d afrIpdScale=%.3f swapEyes=%s hotkeys=Insert(mode)/Home(swap)",
        vr_stereo_experiment_.load(), vr_afr_ipd_scale_.load(), vr_swap_eyes_.load()?"true":"false"); debug_trace::Write(msg); }
    { char msg[320]{}; std::snprintf(msg, sizeof(msg),
        "V44-PRESENT config renderScale=%.3f mode=%d zoom=%.3f sourceVFov=%.2f Loff=(%.3f,%.3f) Roff=(%.3f,%.3f)",
        vr_render_scale_.load(), vr_presentation_mode_.load(), vr_zoom_.load(), vr_source_vertical_fov_deg_.load(),
        vr_left_offset_x_.load(), vr_left_offset_y_.load(), vr_right_offset_x_.load(), vr_right_offset_y_.load());
      debug_trace::Write(msg); }
    
    debug_trace::Write("XR-OWNER-V20 single-source=XrManager::Instance; last-good stereo cache isolated from raw locate failures");
    debug_trace::Write("starting OpenXR initialization");
    if (!xr_manager_->EnsureInitialized(game_device, game_queue))
    {
        log().error("OpenXR session initialization failed");
        // Stereo observers were installed immediately before OpenXR init.
        // Tear them down on this failure path as well, otherwise an XR init
        // failure leaves WHGame detours active after the worker exits.
        RecursiveViewProbe::Uninstall();
    FrameSync::Uninstall();
        stereo_archaeology::Shutdown();
        xr_manager_->Shutdown();
        xr_manager_ = nullptr; // singleton - clear the pointer, do not delete
        device_hook_->Uninstall();
        device_hook_.reset();
        camera_hook_->Uninstall();
        camera_hook_.reset();
        running_ = false;
        return;
    }

    // V61.38 RELEASE RECOVERY:
    // - Game FOV and Source FOV must never start mismatched.
    // - Defaults remain 95/95, but an explicitly configured Game FOV is allowed.
    // - FPS is NOT forced at startup; the menu slider is seeded from the game's
    //   current sys_MaxFPS because the correct cap depends on the headset refresh.
    {
      float startup_game_fov=std::clamp(menu_game_fov_target_.load(std::memory_order_acquire),70.0f,130.0f);
      const bool fov_ok=ApplyNativeGameFov(startup_game_fov);
      const float fov_readback=ReadCurrentGameFovValue();
      if(fov_readback>1.0f) startup_game_fov=std::clamp(fov_readback,70.0f,130.0f);
      menu_game_fov_target_.store(startup_game_fov,std::memory_order_release);
      vr_source_vertical_fov_deg_.store(startup_game_fov,std::memory_order_release);
      xr_manager_->SetPresentationSettings(
          vr_render_scale_.load(std::memory_order_relaxed),
          vr_presentation_mode_.load(std::memory_order_relaxed),
          vr_zoom_.load(std::memory_order_relaxed),
          0.0f,0.0f,0.0f,0.0f,startup_game_fov);

      const float hf=ReadCurrentHorizontalFovValue();
      if(hf>1.0f) menu_horizontal_fov_target_.store(std::clamp(hf,70.0f,170.0f),std::memory_order_release);

      const float current_fps=ReadCurrentGameFpsCap();
      if(current_fps>1.0f) menu_fps_cap_target_.store(std::clamp(current_fps,60.0f,120.0f),std::memory_order_release);

      const float merged=ReadMergedMeshesDistance();
      if(merged>1.0f) menu_merged_mesh_target_.store(enable_vr_balanced_graphics_.load(std::memory_order_relaxed)?std::max(120.0f,merged):merged,std::memory_order_release);
      // V62.6: color controls are compositor-side only. Never read/write the
      // crash-prone retail r_Gamma/r_Brightness/r_Contrast setter path.
      vr_gamma_.store(1.0f,std::memory_order_release);
      vr_brightness_.store(0.0f,std::memory_order_release);
      vr_contrast_.store(1.0f,std::memory_order_release);
      if(xr_manager_) xr_manager_->SetColorAdjustment(1.0f,0.0f,1.0f);

      char m[512]{};
      std::snprintf(m,sizeof(m),
          "V61.38-STARTUP BASELINE gameFov=%.2f sourceFov=%.2f matched=true fovWrite=%s fpsCurrent=%.2f fpsForced=false cyclo=%d",
          double(startup_game_fov),double(startup_game_fov),fov_ok?"ok":"failed/readback-used",
          double(current_fps),vr_cyclopean_mode_.load(std::memory_order_relaxed));
      debug_trace::Write(m);
    }

    // V46.2 in-backbuffer menu. It shares the game's captured D3D12 device and
    // never creates a second graphics device. Input is chained through the
    // game's HWND using SetWindowSubclass so other overlays can coexist.
    if (HWND game_window = FindMainGameWindow()) {
        vr_menu_ = std::make_unique<VrMenu>();
        VrMenu::Callbacks menu_callbacks{};
        menu_callbacks.context = this;
        menu_callbacks.read_state = [](void* ctx, VrMenuState* out) {
            if (ctx && out) static_cast<Mod*>(ctx)->FillMenuState(*out);
        };
        menu_callbacks.apply_state = [](void* ctx, const VrMenuState* state) -> bool {
            return ctx && state && static_cast<Mod*>(ctx)->ApplyMenuState(*state);
        };
        menu_callbacks.request_matrix = [](void* ctx) {
            if (ctx) static_cast<Mod*>(ctx)->matrix_test_requested_.store(true, std::memory_order_release);
        };
        menu_callbacks.request_safe_mono = [](void* ctx) {
            if (ctx) static_cast<Mod*>(ctx)->safe_mono_requested_.store(true, std::memory_order_release);
        };
        menu_callbacks.save_settings = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            const bool ok=self->SaveMenuSettings();
            debug_trace::Write(ok?"V62.9-CONFIG saved KCD2VR.ini beside ASI":"V62.9-CONFIG SAVE FAILED");
            if(ok) MessageBeep(MB_OK);
            return ok;
        };

        menu_callbacks.request_record = [](void* ctx, int category) {
            if (ctx) static_cast<Mod*>(ctx)->record_request_.store(category, std::memory_order_release);
        };
        menu_callbacks.request_resolution_recreate = [](void* ctx, float render_scale) {
            auto* self = static_cast<Mod*>(ctx);
            if (self && self->xr_manager_) self->xr_manager_->RequestSwapchainRecreate(render_scale);
        };
        menu_callbacks.apply_custom_resolution = [](void*, int width, int height) -> bool {
            return ApplyMenuCustomResolution(width,height);
        };
        // V62.4: old generic MasterSwitch/SetSubtitleSize experiments stay retired.
        // The visible UI page drives declared KCD2 MovieClips directly.
        menu_callbacks.apply_hud_scale = nullptr;
        menu_callbacks.apply_dialogue_text_scale = nullptr;
        menu_callbacks.apply_vr_ui_layout = [](void*, const VrMenuState* state) -> bool {
            if(!state) return false;
            const bool ok=ApplyNativeVrUiLayout(
                state->vr_ui_layout,
                state->vr_ui_subtitle_scale,state->vr_ui_subtitle_y,
                state->vr_ui_dialogue_scale,state->vr_ui_dialogue_y,state->vr_ui_dialogue_inset_pct,
                state->vr_ui_hints_scale,state->vr_ui_hints_inset_pct,state->vr_ui_hints_y);
            static std::uint64_t last_ui_log_ms=0;
            const std::uint64_t ui_now=static_cast<std::uint64_t>(GetTickCount64());
            if(ui_now-last_ui_log_ms>=500ull || !ok) {
                last_ui_log_ms=ui_now;
                log().warning(
                    "V62.9 native FlashUI apply={} enabled={} subScale={:.2f} subY={:.0f} dialogScale={:.2f} dialogY={:.0f} dialogInset={:.1f}% hintsScale={:.2f} hintsInset={:.1f}% hintsY={:.0f}",
                    ok?"applied":"failed",state->vr_ui_layout,
                    state->vr_ui_subtitle_scale,state->vr_ui_subtitle_y,
                    state->vr_ui_dialogue_scale,state->vr_ui_dialogue_y,state->vr_ui_dialogue_inset_pct,
                    state->vr_ui_hints_scale,state->vr_ui_hints_inset_pct,state->vr_ui_hints_y);
                char ui_diag[512]{};
                std::snprintf(ui_diag,sizeof(ui_diag),"V62.9-UI applied=%s enabled=%d sub=%.2f y=%.0f dlg=%.2f dlgY=%.0f inset=%.1f hint=%.2f inset=%.1f y=%.0f",
                    ok?"yes":"no",state->vr_ui_layout?1:0,state->vr_ui_subtitle_scale,state->vr_ui_subtitle_y,
                    state->vr_ui_dialogue_scale,state->vr_ui_dialogue_y,state->vr_ui_dialogue_inset_pct,state->vr_ui_hints_scale,state->vr_ui_hints_inset_pct,state->vr_ui_hints_y);
                diagnostic_trace::Write(ui_diag);
            }
            return ok;
        };

        menu_callbacks.sync_ui_companion = [](void*, const VrMenuState* state) { if(state) (void)SyncLuaUiCompanion(*state); };
        menu_callbacks.set_vrik_mode = [](void*, bool enabled) {
            frik_pose_probe::SetFinalSkinTestMode(enabled ? 1 : 0);
            const int mode=frik_pose_probe::FinalSkinTestMode();
            char vm[256]{};
            std::snprintf(vm,sizeof(vm),"V62.9-VRIK menu mode=%s; F9 cycles READ -> HAND DELTA -> TWO-BONE POSITIONS -> OFF",frik_pose_probe::ModeName(mode));
            diagnostic_trace::Write(vm);
            debug_trace::Write(vm);
            if(!PlaySoundW(enabled?L"SystemAsterisk":L"SystemExclamation",nullptr,SND_ALIAS|SND_ASYNC|SND_NODEFAULT)) MessageBeep(enabled?MB_ICONASTERISK:MB_OK);
        };

        menu_callbacks.apply_game_fov = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyNativeGameFov(value)) {
                const float confirmed_fov = ReadCurrentGameFovValue();
                self->menu_game_fov_target_.store(confirmed_fov,std::memory_order_release);
                // V60.4: do NOT overwrite compositor source-FOV calibration here.
                // KCD2's logical cl_fov, published camera FOV, and actual render
                // projection have proven state-dependent.  The Source FOV slider
                // is now intentionally independent and live.
                char sync_msg[256]{};
                std::snprintf(sync_msg,sizeof(sync_msg),
                    "V60.4-GAME-FOV APPLY gameFov=%.3f sourceCalibration=%.3f independent=true",
                    static_cast<double>(confirmed_fov),
                    static_cast<double>(self->vr_source_vertical_fov_deg_.load(std::memory_order_acquire)));
                debug_trace::Write(sync_msg);
                self->pending_fov_calibration_not_before_ms_.store(0, std::memory_order_release);
                self->pending_fov_calibration_deadline_ms_.store(0, std::memory_order_release);
            }
        };
        menu_callbacks.apply_horizontal_fov = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyNativeHorizontalFov(value)) {
                const float confirmed=ReadCurrentHorizontalFovValue();
                if(confirmed>1.0f) self->menu_horizontal_fov_target_.store(confirmed,std::memory_order_release);
            }
        };
        menu_callbacks.apply_fps_cap = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyNativeGameFps(value)) self->menu_fps_cap_target_.store(ReadCurrentGameFpsCap(),std::memory_order_release);
        };
        menu_callbacks.apply_merged_mesh = [](void* ctx, float value) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            if(ApplyMergedMeshesDistance(value)) self->menu_merged_mesh_target_.store(ReadMergedMeshesDistance(),std::memory_order_release);
        };
        menu_callbacks.apply_graphics_color = [](void* ctx, float gamma, float brightness, float contrast) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self || !self->xr_manager_) return false;
            gamma=std::clamp(gamma,0.50f,2.00f);
            brightness=std::clamp(brightness,-0.50f,0.50f);
            contrast=std::clamp(contrast,0.50f,2.00f);
            self->vr_gamma_.store(gamma,std::memory_order_release);
            self->vr_brightness_.store(brightness,std::memory_order_release);
            self->vr_contrast_.store(contrast,std::memory_order_release);
            self->xr_manager_->SetColorAdjustment(gamma,brightness,contrast);
            char m[256]{}; std::snprintf(m,sizeof(m),
                "V62.9-XR-COLOR gamma=%.2f brightness=%+.2f contrast=%.2f compositorOnly=true",
                double(gamma),double(brightness),double(contrast));
            diagnostic_trace::Write(m);
            return true;
        };
        menu_callbacks.apply_hud_visibility = [](void* ctx, bool crosshair, bool lower_icons, bool compass) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            bool ok=true;
            bool cursor_ok=flashui_probe::SetHudMovieClipVisible("Cursor",crosshair);
            if(!cursor_ok){
                cursor_ok=true;
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CursorOutline",crosshair)&&cursor_ok;
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CursorCross",crosshair)&&cursor_ok;
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CombatCursorCross",crosshair)&&cursor_ok;
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CursorCrossPBSlot",crosshair)&&cursor_ok;
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CursorCrossOpponentLock",crosshair)&&cursor_ok;
            }
            ok=cursor_ok&&ok;
            ok=flashui_probe::SetHudMovieClipVisible("Stats",lower_icons)&&ok;
            ok=flashui_probe::SetHudMovieClipVisible("Compass",compass)&&ok;
            if(ok){
                self->hud_crosshair_visible_.store(crosshair,std::memory_order_release);
                self->hud_lower_icons_visible_.store(lower_icons,std::memory_order_release);
                self->hud_compass_visible_.store(compass,std::memory_order_release);
            }
            return ok;
        };
        menu_callbacks.request_recenter = [](void* ctx) {
            (void)ctx;
            CameraHook::RequestRecenter();
            debug_trace::Write("V61.0-RECENTER requested: full mod-local HMD position+yaw/pitch/roll origin will be recaptured on next pose");
        };
        menu_callbacks.toggle_fake_dof = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            const bool on=!self->fake_dof_active_.load(std::memory_order_acquire);
            if(on) self->engine_dof_active_.store(false,std::memory_order_release);
            self->fake_dof_active_.store(on,std::memory_order_release);
            self->fake_dof_started_ms_.store(on?static_cast<std::uint64_t>(GetTickCount64()):0ull,std::memory_order_release);
            self->fake_dof_scale_.store(on?0.80f:CyclopeanAsymmetryForMode(self->vr_cyclopean_mode_.load(std::memory_order_acquire)),std::memory_order_release);
            if(on) self->vr_cyclopean_mode_.store(3,std::memory_order_release);
            debug_trace::Write(on?"V61.16-FAKE-DOF ON exaggerated visibility test .70 -> .85 -> 1.00":"V61.16-FAKE-DOF OFF restored selected pseudo preset");
        };
        menu_callbacks.toggle_engine_dof = [](void* ctx) {
            auto* self=static_cast<Mod*>(ctx); if(!self) return;
            const bool on=!self->engine_dof_active_.load(std::memory_order_acquire);
            if(on) {
                self->fake_dof_active_.store(false,std::memory_order_release);
                self->fake_dof_started_ms_.store(0,std::memory_order_release);
                self->vr_cyclopean_mode_.store(3,std::memory_order_release);
            }
            self->engine_dof_active_.store(on,std::memory_order_release);
            debug_trace::Write(on?"V61.12-ENGINE-DOF ON; read-only KCD2 focus-distance adaptive pseudo requested":"V61.12-ENGINE-DOF OFF; restored selected pseudo preset");
        };
        menu_callbacks.write_hud_xml_mod = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            hud_xml_mod::Settings hs{};
            hs.scale=self->hud_xml_scale_.load(std::memory_order_acquire);
            hs.offset_x=self->hud_xml_offset_x_.load(std::memory_order_acquire);
            hs.offset_y=self->hud_xml_offset_y_.load(std::memory_order_acquire);
            return hud_xml_mod::WriteRuntimeHudMod(hs);
        };
        menu_callbacks.reset_hud_xml_mod = [](void* ctx) -> bool {
            auto* self=static_cast<Mod*>(ctx); if(!self) return false;
            self->hud_xml_scale_.store(1.0f,std::memory_order_release);
            self->hud_xml_offset_x_.store(0,std::memory_order_release);
            self->hud_xml_offset_y_.store(0,std::memory_order_release);
            return hud_xml_mod::ResetRuntimeHudMod();
        };
        if (!vr_menu_->Initialize(game_device, game_window, menu_callbacks)) {
            debug_trace::Write("V46.2-MENU initialization failed; VR continues with hotkeys only");
            vr_menu_.reset();
        }
    } else {
        debug_trace::Write("V46.2-MENU game HWND not found; VR continues with hotkeys only");
    }

    // Install the callback only after XrManager and the menu are initialized.
    // Menu rendering deliberately runs before the XR blit in OnRenderTick.
    DeviceHook::SetRenderTickCallback(&Mod::OnRenderTick);

    log().info("KCD2VR running - head tracking active; render-thread handoff armed");
    debug_trace::Write("OpenXR initialized; render-thread handoff armed; entering tracking loop");
    debug_trace::Write("V61.38 BUILD ID RELEASE-RECOVERY | FPS=USER | FOV=95/95 MATCHED START | 0=OLD-MONO | 1=CORRECTED-MONO | 2=PSEUDO-.90 | 3=DEPTH-PSEUDO-.70..1.00 | DIALOG-ZOOM-RETAIN=.30 | HUD/SUBTITLE=SESSION-PROBE | CULL=V61.12");
    debug_trace::Write("V61.27 combined: Touch-ray menu cursor fixed; menu 20% smaller; HUD XML viewport sliders retired; final-skinning probe read-only; no native stereo writes");

    struct MatrixPhase {
        const char* name; int kind; int pose_source; int rotation_order; int filter_mode; float prediction_offset_ms; std::uint64_t duration_ms;
        // V58.9 mono-focused F8 ladder. We intentionally retired the long
        // tracking/native-stereo ladder after V58.8 showed no useful visual
        // improvement and proved the tagged PRESENT path collapses to one eye.
        int crop_mode = -1;      // -1 unchanged, 0 legacy safety clamp, 1 angular/no-clamp
        int pose_mode = -1;      // -1 unchanged, 0 old fake-separated, 1 honest common origin
        int presentation_mode = -1; // -1 unchanged, 3 OpenXR tangent remap, 2 direct aspect-fill copy
        float source_vfov = 0.0f;   // >0 fixed test VFOV; 0=current published/game source
        float zoom = 1.0f;
        float mono_fov_asym = 1.0f; // submitted XrFovf asymmetry scale
        int pairing_mode = 0;      // 0=current XR pose, 1=PRESENT/render-paired pose
        int uv_reproj_mode = 0;    // 0=direct copy, 1=rotation-only ray reprojection
        float uv_binocular = 0.0f; // constant opposite-sign source UV x shift
        int bottom_edge_mode = 0; // V59.2: 0 black ref, 1 shift, 2 Y compress, 3/4 tail remap, 5 edge hold
        int stereo_test = 0;        // V61.0: 0 mono baseline, 1 AFR, 2 native dual-renderer diagnostic
        float stereo_ipd_scale = 0.0f;
    };
    const MatrixPhase matrix_phases[] = {
        // V61.0 stereo ladder at the current game/source FOV and current high-quality source raster.
        // A-D: frame-locked AFR with one shared base head pose and only +/- eye translation.
        // E-F: CryEngine stereo CVars + Normal CRenderView dual-pass selector diagnostic.
        // All phases use matched-B tangent mapping, no pseudo UV disparity, no temporal reprojection.
        { "A610_AFR_IPD025", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.25f },
        { "B610_AFR_IPD050", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.50f },
        { "C610_AFR_IPD075", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,0.75f },
        { "D610_AFR_IPD100", 0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 1,1.00f },
        { "E610_DUAL_IPD050",0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 2,0.50f },
        { "F610_DUAL_IPD100",0,0,0,5,0.0f,8000ull, 1,1,3,0.0f, 1.00f,1.00f,1,4,0.0f,0, 2,1.00f }
    };
    constexpr int kMatrixPhaseCount = static_cast<int>(sizeof(matrix_phases) / sizeof(matrix_phases[0]));

    struct MatrixCounters {
        XrManager::TestCounters xr{};
        std::uint64_t present = 0;
        std::uint64_t recursive_requests = 0;
        std::uint64_t recursive_passes = 0;
        std::uint64_t normal_pipeline = 0;
        std::uint64_t recursive_pipeline = 0;
        std::uint64_t recursive_barriers = 0;
        std::uint64_t recursive_rt = 0;
        std::uint64_t customres_barriers = 0;
        std::uint64_t customres_rt = 0;
        std::uint64_t published_fov_samples = 0;
        std::uint64_t scene_rt = 0;
        std::uint64_t scene_left = 0;
        std::uint64_t scene_right = 0;
    };
    auto capture_matrix_counters = [&]() -> MatrixCounters {
        MatrixCounters c{};
        c.xr = xr_manager_->GetTestCounters();
        c.present = DeviceHook::PresentCaptureCount();
        c.recursive_requests = RecursiveViewProbe::RecursiveRequestCount();
        c.recursive_passes = RecursiveViewProbe::RecursivePassCount();
        c.normal_pipeline = FrameSync::PipelineCount(0);
        c.recursive_pipeline = FrameSync::PipelineCount(1);
        c.recursive_barriers = DeviceHook::RecursiveBarrierCount();
        c.recursive_rt = DeviceHook::RecursiveRenderTargetBarrierCount();
        c.customres_barriers = DeviceHook::CustomResolutionBarrierCount();
        c.customres_rt = DeviceHook::CustomResolutionRenderTargetBarrierCount();
        c.published_fov_samples = FrameSync::PublishedFovSampleCount();
        c.scene_rt = DeviceHook::SceneColorProbeBarrierCount();
        c.scene_left = DeviceHook::SceneColorProbeLeftCount();
        c.scene_right = DeviceHook::SceneColorProbeRightCount();
        return c;
    };
    auto delta_u64 = [](std::uint64_t a, std::uint64_t b) -> std::uint64_t { return a >= b ? (a - b) : 0; };

    auto latest_present_size = [&]() -> std::pair<std::uint64_t, std::uint32_t> {
        ID3D12Resource* r = nullptr;
        if (!DeviceHook::TryGetLatestPresentResource(&r) || !r) return {0, 0};
        const auto d = r->GetDesc();
        r->Release();
        return { static_cast<std::uint64_t>(d.Width), static_cast<std::uint32_t>(d.Height) };
    };

    bool matrix_active = false;
    int matrix_phase = -1;
    std::uint64_t matrix_phase_start_ms = 0;
    MatrixCounters matrix_phase_start{};
    MatrixCounters matrix_cycle_start{};
    EngineCVarSnapshot matrix_original_cvars{};
    FpsCVarSnapshot matrix_original_fps{};
    bool matrix_phase_cvar_ok = false;
    V54StereoCVarSnapshot v54_stereo_original{};
    std::uint64_t v54_reentry_start=0;
    bool matrix_phase_fps_ok = false;
    float matrix_phase_target_fps = 0.0f;
    float matrix_restore_fov = 100.0f;
    float matrix_restore_framing = 1.00f;
    float matrix_restore_cull_scale = 1.0f;
    std::uint64_t boxing_last_sample_ms = 0;
    std::uint32_t boxing_sample_index = 0;
    int boxing_countdown_last_second = -1;
    bool boxing_mouse_l_prev = false;
    bool boxing_mouse_r_prev = false;
    std::uint64_t vrik_last_status_ms = 0; // V61.50: control-thread-only sparse ownership telemetry

    auto apply_matrix_phase = [&](int index) {
        const MatrixPhase& ph = matrix_phases[index];
        matrix_phase_start = capture_matrix_counters();
        CameraHook::ResetWorstCaseAngular();
        xr_manager_->SetFlatUiMode(false); xr_manager_->SetLatePoseEnabled(false);
        CameraHook::SetInjectionMode(1); CameraHook::SetTrackingOptions(true,true);
        CameraHook::SetPoseSourceMode(ph.pose_source,ph.prediction_offset_ms); CameraHook::SetRotationOrder(ph.rotation_order); CameraHook::SetRotationFilterMode(ph.filter_mode);
        xr_manager_->SetPresentPosePairingMode(ph.pairing_mode); xr_manager_->SetProjectionPoseMode(0); xr_manager_->SetSwapEyes(false); xr_manager_->SetPresentationTestMarker(false);
        xr_manager_->SetStereoExperiment(0,0.0f); DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); CameraHook::SetAfrIpd(0.064f,0.0f);
        FrameSync::SetNativeStereoReentryMode(0,0.064f); FrameSync::SetDenseRotationProbe(false);
        DeviceHook::SetSceneColorProbe(false,0,0);
        // F8 ladder addition: crop_mode/pose_mode, -1 = leave whatever was
        // already configured (so the existing 6 tracking-only phases above
        // are completely unaffected by this addition).
        if (ph.crop_mode >= 0) xr_manager_->SetAngularCorrectMono(ph.crop_mode == 1);
        xr_manager_->SetPoseHonestyOverride(ph.pose_mode);
        xr_manager_->SetMonoFovAsymmetryScale(ph.mono_fov_asym);
        xr_manager_->SetMonoUvReprojectionMode(ph.uv_reproj_mode);
        xr_manager_->SetMonoUvBinocularOffset(ph.uv_binocular);
        xr_manager_->SetMonoBottomEdgeMode(ph.bottom_edge_mode);
        xr_manager_->ResetClampStats();
        // V58.9 phase-local presentation override. Mode 3 is the historical
        // tangent remap; mode 2 is an identical aspect-fill mono copy.
        if (ph.presentation_mode >= 0) {
            float phase_vfov = ph.source_vfov;
            if (ph.source_vfov > 0.0f) {
                const bool fov_ok = ApplyNativeGameFov(ph.source_vfov);
                const float confirmed = ReadCurrentGameFovValue();
                if (confirmed > 1.0f) {
                    phase_vfov = confirmed;
                    vr_source_vertical_fov_deg_.store(confirmed, std::memory_order_release);
                    menu_game_fov_target_.store(confirmed, std::memory_order_release);
                }
                char fm[320]{}; std::snprintf(fm,sizeof(fm),
                    "V60.7-FIXED-FOV target=%.1f readback=%.3f source=%.3f ok=%s XR_FOV=UNCHANGED",
                    ph.source_vfov, confirmed, phase_vfov, fov_ok?"true":"false");
                debug_trace::Write(fm);
            }
            if (phase_vfov <= 0.0f) {
                // V59.8: F8 deliberately uses the confirmed GAME_FOV/SOURCE_FOV
                // lock, not a transient published camera FOV (dialogue/inventory
                // can publish their own temporary values).
                phase_vfov = vr_source_vertical_fov_deg_.load(std::memory_order_relaxed);
            }
            xr_manager_->SetPresentationSettings(
                vr_render_scale_.load(std::memory_order_relaxed), ph.presentation_mode, ph.zoom,
                0.0f,0.0f,0.0f,0.0f,phase_vfov);
        }
        float runtime_ipd=0.064f; XrManager::StereoViewSnapshot sv{}; if(xr_manager_->CopyLatestStereoViews(sv)&&sv.raw_ipd>0.03f&&sv.raw_ipd<0.10f) runtime_ipd=sv.raw_ipd;
        if(index==0 && !v54_stereo_original.valid) v54_stereo_original=CaptureV54StereoCVars();
        // V61.0 phase-local stereo mode. Use current source raster/resolution; no custom-res rebuild.
        if (ph.stereo_test == 1) {
            RestoreV54StereoCVars(v54_stereo_original);
            FrameSync::SetNativeStereoReentryMode(0,0.064f);
            CameraHook::SetAfrIpd(runtime_ipd, ph.stereo_ipd_scale);
            FrameSync::SetStereoMode(1);
            DeviceHook::SetAfrStereoCapture(true);
            xr_manager_->SetStereoExperiment(1, ph.stereo_ipd_scale);
        } else if (ph.stereo_test == 2) {
            DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); xr_manager_->SetStereoExperiment(0,0.0f);
            const float test_ipd=std::clamp(runtime_ipd*ph.stereo_ipd_scale,0.004f,0.095f);
            ApplyV54StereoCVars(test_ipd,false);
            FrameSync::SetNativeStereoReentryMode(2,test_ipd);
            CameraHook::SetAfrIpd(runtime_ipd,0.0f);
        }
        const auto probe_size = latest_present_size();
        const std::uint32_t probe_w = probe_size.first ? static_cast<std::uint32_t>(probe_size.first) : 1920u;
        const std::uint32_t probe_h = probe_size.second ? probe_size.second : 1920u;
        if(ph.stereo_test==2) {
            DeviceHook::SetSceneColorProbe(true,probe_w,probe_h);
            char sp[320]{}; std::snprintf(sp,sizeof(sp),
                "V61.0-DUAL-PROBE phase=%s reentryMode=%d ipdScale=%.2f rawIpd=%.4f target=%ux%u PRESENT_BRIDGE=MONO",
                ph.name,FrameSync::NativeStereoReentryMode(),(double)ph.stereo_ipd_scale,(double)runtime_ipd,probe_w,probe_h);
            debug_trace::Write(sp);
        }
        v54_reentry_start=FrameSync::NativeStereoReentryPasses();
        matrix_phase=index; matrix_phase_start_ms=static_cast<std::uint64_t>(GetTickCount64());
        V54PhaseBeeps(index+1);
        char m[720]{}; std::snprintf(m,sizeof(m),
          "V61.0-F8-BEGIN BUILD=AFR-DUAL-TPV-MENU-20260906-N phase=%d/%d name=%s durationMs=%llu GAME_FOV=%.3f SOURCE_FOV=%.3f MATCHED=%s kind=%d presentMode=%d fovAsym=%.3f zoom=%.2f mapMode=%d uvBinoc=%.4f pairing=%d stereoTest=%d ipdScale=%.2f",
          index+1,kMatrixPhaseCount,ph.name,(unsigned long long)ph.duration_ms,
          (double)ReadCurrentGameFovValue(), (double)vr_source_vertical_fov_deg_.load(std::memory_order_relaxed),
          (std::fabs(ReadCurrentGameFovValue()-vr_source_vertical_fov_deg_.load(std::memory_order_relaxed))<0.01f)?"true":"false",
          ph.kind, ph.presentation_mode, (double)ph.mono_fov_asym,(double)ph.zoom,ph.uv_reproj_mode,(double)ph.uv_binocular,ph.pairing_mode,ph.stereo_test,(double)ph.stereo_ipd_scale);
        debug_trace::Write(m);
    };

    auto finish_matrix_phase = [&]() {
        if(matrix_phase<0 || matrix_phase>=kMatrixPhaseCount) return;
        const MatrixPhase& ph=matrix_phases[matrix_phase]; const MatrixCounters endc=capture_matrix_counters();
        const auto psz=latest_present_size();
        const char* src = ph.pose_source==0?"CACHED_XR":ph.pose_source==1?"SYNC_PREDICTED":ph.pose_source==2?"SYNC_NOW":"SYNC_PREDICTED_OFFSET";
        char m[800]{};
        std::snprintf(m,sizeof(m),
            "V56-F8-END phase=%d/%d name=%s xrFrames=%llu present=%llu presentSize=%llux%u lastPoseFrame=%llu cameraPoseApplyAgeMs=%llu poseSource=%s rotationOrder=%s filterMode=%d offsetMs=%.1f monoBlits=%llu afrBlits=%llu",
            matrix_phase+1,kMatrixPhaseCount,ph.name,
            static_cast<unsigned long long>(delta_u64(endc.xr.xr_frames,matrix_phase_start.xr.xr_frames)),
            static_cast<unsigned long long>(delta_u64(endc.present,matrix_phase_start.present)),
            static_cast<unsigned long long>(psz.first),psz.second,
            static_cast<unsigned long long>(CameraHook::LastPoseFrameIndex()),
            static_cast<unsigned long long>(CameraHook::LastPoseApplyTickMs()?GetTickCount64()-CameraHook::LastPoseApplyTickMs():0),
            src,ph.rotation_order==0?"BASE_X_HMD_LOCAL":"HMD_X_BASE_DIAGNOSTIC",ph.filter_mode,ph.prediction_offset_ms,
            static_cast<unsigned long long>(delta_u64(endc.xr.mono_blits,matrix_phase_start.xr.mono_blits)),
            static_cast<unsigned long long>(delta_u64(endc.xr.afr_blits,matrix_phase_start.xr.afr_blits)));
        debug_trace::Write(m);
        char x[320]{}; std::snprintf(x,sizeof(x),"V56-REENTRY-DIAG phase=%d reentryMode=%d dualPassExecutions=%llu",matrix_phase+1,FrameSync::NativeStereoReentryMode(),(unsigned long long)(FrameSync::NativeStereoReentryPasses()-v54_reentry_start)); debug_trace::Write(x);
        { char sc[384]{}; std::snprintf(sc,sizeof(sc),
            "V59.6-SCENE-SUMMARY phase=%d name=%s rtTouches=%llu left=%llu right=%llu",
            matrix_phase+1,ph.name,
            (unsigned long long)DeviceHook::SceneColorProbeBarrierCount(),
            (unsigned long long)DeviceHook::SceneColorProbeLeftCount(),
            (unsigned long long)DeviceHook::SceneColorProbeRightCount()); debug_trace::Write(sc); }
        float wg=0,wh=0,wf=0; std::uint64_t wn=0; CameraHook::GetWorstCaseAngular(wg,wh,wf,wn); char a[320]{}; std::snprintf(a,sizeof(a),"V56-ANGULAR-WORST phase=%d name=%s worstGapDeg=%.4f hmdDeg=%.4f finalDeg=%.4f sample=%llu",matrix_phase+1,ph.name,wg,wh,wf,(unsigned long long)wn); debug_trace::Write(a);
        // F8 ladder addition: directly reports what fraction of blits this
        // phase had the legacy coverage-clamp actively reducing scale below
        // what was requested - the exact, empirical measurement of the
        // musings doc's 0.697-vs-0.995 finding, per phase.
        {
            std::uint64_t clamp_active=0, clamp_total=0; xr_manager_->GetClampStats(clamp_active, clamp_total);
            const double clamp_pct = clamp_total > 0 ? (100.0 * static_cast<double>(clamp_active) / static_cast<double>(clamp_total)) : 0.0;
            char c[256]{}; std::snprintf(c,sizeof(c),"V56-CLAMP-STATS phase=%d name=%s clampActive=%llu clampTotal=%llu clampActivePct=%.1f",
                matrix_phase+1,ph.name,(unsigned long long)clamp_active,(unsigned long long)clamp_total,clamp_pct);
            debug_trace::Write(c);
        }
    };

    auto log_boxing_sample = [&]() {
        if (!xr_manager_) return;
        const auto& l = xr_manager_->LeftController();
        const auto& r = xr_manager_->RightController();
        void* console = SafeGetCryConsole();
        auto cvar_i = [&](const char* name, int fallback) -> int {
            return SafeCVarGetInt(SafeConsoleGetCVar(console, name), fallback);
        };
        const int player_no_ik = cvar_i("player_NoIK", -999);
        const int player_draw_ik = cvar_i("player_DrawIK", -999);
        const int debug_limbs = cvar_i("wh_pl_DebugIKLimbs", -999);
        const int adik_targets = cvar_i("ca_useADIKTargets", -999);
        const int show_head = cvar_i("wh_pl_ShowHead", -999);
        const bool mouse_l = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        const bool mouse_r = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        const bool key_w = (GetAsyncKeyState('W') & 0x8000) != 0;
        const bool key_a = (GetAsyncKeyState('A') & 0x8000) != 0;
        const bool key_s = (GetAsyncKeyState('S') & 0x8000) != 0;
        const bool key_d = (GetAsyncKeyState('D') & 0x8000) != 0;
        const bool key_f = (GetAsyncKeyState('F') & 0x8000) != 0;
        const bool key_e = (GetAsyncKeyState('E') & 0x8000) != 0;
        const bool key_q = (GetAsyncKeyState('Q') & 0x8000) != 0;
        const bool mouse_m = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        char m[1400]{};
        std::snprintf(m, sizeof(m),
            "V47.0-COMBAT sample=%u input={LMB:%s RMB:%s MMB:%s F:%s E:%s Q:%s W:%s A:%s S:%s D:%s} Lctrl={active:%s pos:(%.4f,%.4f,%.4f)} Rctrl={active:%s pos:(%.4f,%.4f,%.4f)} cvars={player_NoIK:%d player_DrawIK:%d wh_pl_DebugIKLimbs:%d ca_useADIKTargets:%d wh_pl_ShowHead:%d} counters={normalPipe:%llu recursivePipe:%llu present:%llu}",
            ++boxing_sample_index, mouse_l?"down":"up", mouse_r?"down":"up", mouse_m?"down":"up", key_f?"down":"up", key_e?"down":"up", key_q?"down":"up", key_w?"down":"up", key_a?"down":"up", key_s?"down":"up", key_d?"down":"up",
            l.active?"true":"false", l.pose.pos_x,l.pose.pos_y,l.pose.pos_z,
            r.active?"true":"false", r.pose.pos_x,r.pose.pos_y,r.pose.pos_z,
            player_no_ik,player_draw_ik,debug_limbs,adik_targets,show_head,
            static_cast<unsigned long long>(FrameSync::PipelineCount(0)),
            static_cast<unsigned long long>(FrameSync::PipelineCount(1)),
            static_cast<unsigned long long>(DeviceHook::PresentCaptureCount()));
        debug_trace::Write(m);
    };

    auto restore_safe_mono = [&]() {
        if (vr_menu_) vr_menu_->SetCaptureCountdown(false, 0);
        DeviceHook::SetCustomResolutionProbe(false, 0, 0);
        const float safe_fov = std::clamp(matrix_active ? matrix_restore_fov : vr_source_vertical_fov_deg_.load(std::memory_order_relaxed), 70.0f, 140.0f);
        const float safe_framing = std::clamp(matrix_active ? matrix_restore_framing : vr_zoom_.load(std::memory_order_relaxed), 0.80f, 2.00f);
        // V58 safe state is Mono + OpenXR-FOV fallback calibration. Do not
        // touch KCD2 cl_fov or desktop/custom resolution here.
        vr_stereo_experiment_.store(0, std::memory_order_relaxed);
        vr_afr_ipd_scale_.store(0.0f, std::memory_order_relaxed);
        const int restore_preset = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed),0,5);
        const V603ViewPreset restore_view = ViewPresetForMode(restore_preset);
        const int restore_present = restore_view.presentation_mode;
        const float restore_zoom = safe_framing;
        const float restore_uv = restore_view.uv_binocular;
        vr_presentation_mode_.store(restore_present, std::memory_order_relaxed);
        vr_source_vertical_fov_deg_.store(safe_fov, std::memory_order_relaxed);
        vr_zoom_.store(restore_zoom, std::memory_order_relaxed);
        vr_left_offset_x_.store(0.0f, std::memory_order_relaxed);
        vr_left_offset_y_.store(0.0f, std::memory_order_relaxed);
        vr_right_offset_x_.store(0.0f, std::memory_order_relaxed);
        vr_right_offset_y_.store(0.0f, std::memory_order_relaxed);
        vr_swap_eyes_.store(false, std::memory_order_relaxed);
        xr_manager_->SetStereoExperiment(0, 0.0f);
        xr_manager_->SetPresentationSettings(
            vr_render_scale_.load(std::memory_order_relaxed), restore_present, restore_zoom,
            0.0f, 0.0f, 0.0f, 0.0f, safe_fov);
        xr_manager_->SetAngularCorrectMono(restore_view.angular_correct);
        xr_manager_->SetPoseHonestyOverride(restore_view.pose_honesty);
        xr_manager_->SetMonoUvReprojectionMode(0);
        xr_manager_->SetMonoUvBinocularOffset(restore_uv);
        xr_manager_->SetSwapEyes(false);
        xr_manager_->SetMonoFovAsymmetryScale(restore_view.fov_asymmetry);
        xr_manager_->SetPresentationTestMarker(false);
        xr_manager_->SetPresentPosePairingMode(1);
        DeviceHook::SetAfrStereoCapture(false);
        FrameSync::SetStereoMode(0);
        CameraHook::SetAfrIpd(0.064f, 0.0f);
    };

    bool vr_projection_logged = false;
    auto ensure_good_mono_projection = [&]() {
        if(vr_projection_logged) return;
        vr_projection_logged=true;
        debug_trace::Write("V58-GOOD-MONO: OpenXR tangent remap active; mono coverage/framing are compositor-only; KCD2 cl_fov and desktop resolution untouched");
    debug_trace::Write("V61.26-RESOLUTION preferred source baseline=1920x1800; KCD2 source/desktop resolution is NOT overridden by this build");
    };

    VrGraphicsSnapshot vr_graphics_original{};
    bool vr_graphics_applied = false;
    DialogueCVarSnapshot dialogue_original{};
    int dialogue_mode_applied = -1;
    std::uint64_t vr_graphics_last_try_ms = 0;
    graphics_profile_refresh_requested_.store(true, std::memory_order_release);
    graphics_profile_applied_for_ui_.store(false, std::memory_order_release);

    auto refresh_vr_graphics_profile = [&](bool force) {
        const bool wanted = enable_vr_balanced_graphics_.load(std::memory_order_acquire);
        const std::uint64_t now = static_cast<std::uint64_t>(GetTickCount64());
        if (wanted) {
            if (vr_graphics_applied && !force) return;
            if (!vr_graphics_original.valid) {
                if (!force && now - vr_graphics_last_try_ms < 1000) return;
                vr_graphics_last_try_ms = now;
                if (!CaptureVrGraphicsSnapshot(vr_graphics_original)) return;
                char msg[384]{};
                std::snprintf(msg, sizeof(msg),
                    "V46.3-GRAPHICS-SNAPSHOT view=%.2f veg=%.2f merged=%.2f blur=%d sharp=%.3f",
                    static_cast<double>(vr_graphics_original.view_dist),
                    static_cast<double>(vr_graphics_original.vegetation_dist),
                    static_cast<double>(vr_graphics_original.merged_mesh_dist),
                    vr_graphics_original.motion_blur, static_cast<double>(vr_graphics_original.sharpening));
                debug_trace::Write(msg);
            } else if (!force && now - vr_graphics_last_try_ms < 1000) {
                return;
            }
            vr_graphics_last_try_ms = now;
            char msg[768]{};
            const bool ok = ApplyVrBalancedGraphics(vr_graphics_original,
                vr_engine_clarity_.load(std::memory_order_acquire), msg, sizeof(msg));
            if (msg[0]) debug_trace::Write(msg);
            if (!ok) {
                // A setter can fail after earlier setters succeeded. Roll back
                // immediately so a partial profile can never be left behind.
                char rollback[512]{};
                (void)RestoreVrGraphics(vr_graphics_original, rollback, sizeof(rollback));
                if (rollback[0]) debug_trace::Write(rollback);
            }
            vr_graphics_applied = ok;
            graphics_profile_applied_for_ui_.store(ok, std::memory_order_release);
        } else if (vr_graphics_applied && vr_graphics_original.valid) {
            char msg[512]{};
            const bool ok = RestoreVrGraphics(vr_graphics_original, msg, sizeof(msg));
            if (msg[0]) debug_trace::Write(msg);
            if (ok) {
                vr_graphics_applied = false;
                graphics_profile_applied_for_ui_.store(false, std::memory_order_release);
            }
        }
    };

    bool game_fov_reapplied = false;
    auto refresh_game_fov_once = [&]() {
        if (game_fov_reapplied || FrameSync::PublishedFovSampleCount() == 0) return;
        game_fov_reapplied = true;
        const float game_fov_now=ReadCurrentGameFovValue();
        if (game_fov_now > 1.0f)
            menu_game_fov_target_.store(game_fov_now, std::memory_order_release);
        char m[320]{}; std::snprintf(m,sizeof(m),
            "V60.4-FOV-INDEPENDENT GAME_FOV=%.3f SOURCE_CAL=%.3f startup=true no-auto-sync",
            static_cast<double>(game_fov_now),
            static_cast<double>(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed)));
        debug_trace::Write(m);
    };

    auto refresh_dialogue_camera_mode = [&]() {
        const int wanted=std::clamp(vr_dialogue_camera_mode_.load(std::memory_order_relaxed),0,2);
        if(!dialogue_original.valid) {
            if(!CaptureDialogueCVars(dialogue_original)) return;
            char m[192]{}; std::snprintf(m,sizeof(m),"V58-DIALOGUE original={ForcePOV:%d NoCameras:%d}",dialogue_original.force_pov,dialogue_original.no_cameras); debug_trace::Write(m);
        }
        if(wanted!=dialogue_mode_applied && ApplyDialogueMode(dialogue_original,wanted)) dialogue_mode_applied=wanted;
    };
    refresh_game_fov_once();
    refresh_dialogue_camera_mode();

    // V51 default runtime tracking: immutable predictedDisplayTime pose, applied once
    // at PublishViewCamera instead of repeatedly mutating frustum cameras.
    debug_trace::Write(mirror_mis_menu_to_desktop_.load(std::memory_order_relaxed)
        ? "V58.3-MENU desktop mirror ENABLED by INI (diagnostic; may reintroduce hybrid leakage)"
        : "V58.3-MENU desktop mirror DISABLED; dedicated XR menu layer only");
    touch_bindings_.SetEnabled(enable_touch_bindings_.load(std::memory_order_relaxed));
    touch_bindings_.SetAttackDirectionMouseEmulation(enable_touch_attack_direction_.load(std::memory_order_relaxed));
    xr_manager_->SetSharpening(vr_engine_clarity_.load(std::memory_order_relaxed));
    xr_manager_->SetLatePoseEnabled(false);
    xr_manager_->SetPresentPosePairingMode(1);
    CameraHook::SetInjectionMode(1);
    CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),
                                  vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed));
    {
        char pm[192]{};
        std::snprintf(pm,sizeof(pm),"V61.45-POSE-SOURCE normal mode=%d offsetMs=%.2f (0=cached 1=predicted 2=now 3=predicted+offset)",
            vr_pose_source_mode_.load(std::memory_order_relaxed),
            double(vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed)));
        debug_trace::Write(pm);
    }

    // V61.32 presentation architecture: start in proven GOOD MONO immersive mode,
    // but restore F6 as the manual world-locked virtual screen. F8 now owns the
    // automatic cyclopean/source-FOV + optical-center correction experiment.
    bool crysis_crop_mono = true;
    bool adaptive_rotation_filter = false;
    debug_trace::Write("V61.30-ROT fixed 8ms baseline; old F4 adaptive-rotation diagnostic retired");
    std::uint64_t angular_sync_last_log_ms = 0;
    xr_manager_->SetAngularCorrectMono(true);
    debug_trace::Write("V62.9-MONO corrected projection baseline retained; V60.1 dialog/inventory guard; Fix head sync=mode2; Lua/native HUD persistence; VRIK ladder starts OFF");

    bool f6_down_prev = false;
    bool f7_down_prev = false;
    bool f9_down_prev = false;
    bool key_i_prev = false, key_m_prev = false, key_esc_prev = false;
    // V62.4 F6 state: AUTO when both flags are false; F6 cycles
    // AUTO -> FORCE SCREEN -> FORCE IMMERSIVE -> AUTO.
    bool flat_ui_manual = false;          // FORCE SCREEN
    bool flat_ui_force_immersive = false; // FORCE IMMERSIVE
    bool inventory_flat_ui = false;
    bool map_flat_ui = false;
    bool pause_flat_ui = false;
    bool startup_flat_ui_auto = true;
    std::uint64_t startup_ready_since_ms = 0;
    bool fov_safety_logged = false;
    bool tracking_suspended_for_flat_ui = true;
    void* flat_state_console = SafeGetCryConsole();
    void* cutscene_context_cvar = flat_state_console ? SafeConsoleGetCVar(flat_state_console, "e_SkipInFrustumOptimizationWH") : nullptr;
    // V62.4: retail WHGame exposes the active dialogue context id. Native
    // dialogue cameras are presented on the F6 comfort screen; forced 1st
    // person dialogue remains immersive with the fixed source-FOV guard.
    void* dialogue_context_cvar = flat_state_console ? SafeConsoleGetCVar(flat_state_console, "iDialogueContextId") : nullptr;
    bool dialogue_ratio_hidden = false;
    // V62.6: Inventory can recreate the HUD MovieClips. Reapply the desired
    // semantic layout with a short bounded retry sequence after close.
    bool inventory_was_open_for_hud = false;
    int hud_reapply_attempts = 0;
    std::uint64_t hud_reapply_next_ms = 0;
    // V62.9: bounded startup handoff to the optional Lua HUD lifecycle companion.
    // Slider changes sync immediately; these four retries cover script/gameplay startup order
    // without turning unknown-command handling into a permanent polling loop.
    const std::uint64_t lua_sync_epoch_ms = static_cast<std::uint64_t>(GetTickCount64());
    int lua_sync_stage = 0;
    constexpr std::uint64_t lua_sync_delays_ms[4] = { 2000ull, 5000ull, 10000ull, 20000ull };
    CameraHook::SetTrackingOptions(false,false);
    xr_manager_->SetFlatUiMode(true);
    debug_trace::Write("V62.9-STARTUP corrected culling retained; head-sync cached<->SYNC_NOW; F6 AUTO/override restored; Lua/native HUD persistence; VRIK ladder starts OFF");
    bool matrix_restore_flat_ui_manual = false;
    bool matrix_restore_inventory_flat_ui = false;
    // V61.18: run HUD FlashUI RTTI discovery directly; V61.17 accidentally only reached this
    // through the optional stereo graphics-pipeline observer, so normal runs never scanned.
    stereo_archaeology::DiscoverHudFlashUiOnce();
    stereo_archaeology::StartFocusedStereoChaseV61_20();
    // V62.4: restore the low-frequency FlashUI GetElement observer. The native
    // HUD sliders reuse the same game-returned HUD object path that previously
    // proved CombatCursorCross visibility, avoiding retail console-Lua entirely.
    const bool flashui_live_installed=flashui_probe::Install();
    if(flashui_live_installed) log().info("V62.4 native FlashUI HUD observer installed");
    else log().warning("V62.4 native FlashUI HUD observer unavailable; UI apply will use build-gated direct GetElement fallback");
    // V61.43: normal play remains completely free of VRIK hooks. The semantic
    // ownership ladder is an explicit opt-in experiment and installs only the
    // read-only WeaponSolver correlation + exact +0x5FE2F0 transform-builder hook.
    // V62.9 experimental branch: install the exact semantic observer/late builder hook.
    // READ is zero-write. F9 modes 2/3 use only validated selector-1 setter +0x53F4D0,
    // bounded controller-relative deltas, and immediate post-builder restore.
    enable_vrik_ownership_test_.store(true,std::memory_order_release);
    const bool vrik_probe_installed=frik_pose_probe::Install();
    if(vrik_probe_installed){
        log().warning("V62.9 VRIK ladder ready; F9 cycles OFF/READ/PREBUILD-HAND/PREBUILD-TWOBONE");
        diagnostic_trace::Write("V62.9-VRIK ladder installed; starts OFF. READ is zero-write; modes 2/3 are bounded PREBUILD experiments");
    } else {
        log().error("V62.9 VRIK ladder hook failed build/ABI validation; normal VR continues");
        diagnostic_trace::Write("V62.9-VRIK ERROR hook install failed; F9 unavailable");
    }

    // V61.32 combined-test state. F6 is no longer a hypothesis key; it is the
    // restored LOCAL virtual screen. F8 owns the corrected projection proof.
    bool f4_focus_pseudo_active=false;
    bool f4_restore_engine_dof=false, f4_restore_fake_dof=false;
    std::uint64_t f4_restore_fake_started=0;
    float f4_restore_engine_scale=0.95f, f4_restore_fake_scale=0.95f;
    int f4_restore_cyclopean_mode=0;
    bool f8_auto_projection_active=true;
    vr_auto_projection_active_.store(true,std::memory_order_release);
    float f8_restore_center_y=vr_presentation_center_y_.load(std::memory_order_acquire);
    std::uint64_t f8_last_adjust_ms=0, f8_last_status_ms=0;

    // V61.38: restore the successful V61.33 transient-zoom concept, but drive
    // it from the exact gameplay CView FOV recovered by the V61.12 identity gate.
    // This keeps dialogue/inventory/bow camera zoom from contaminating the
    // compositor while retaining a user-selectable fraction of native zoom.
    // V61.45: V61.38 transient dialogue/inventory cl_fov pumping is RETIRED.
    // The solved V60.1/V61.36 behavior is the fixed source-FOV guard below: gameplay
    // FOV remains untouched and transient dialogue/inventory camera publications are
    // never allowed to rescale the VR compositor source model.

    auto compute_auto_projection=[&](float actual_vfov_deg,float& out_center_y,float& out_required_centered,float& out_required_no_shift)->bool {
        out_center_y=0.0f; out_required_centered=0.0f; out_required_no_shift=0.0f;
        if(!xr_manager_ || !(actual_vfov_deg>30.0f && actual_vfov_deg<150.0f)) return false;
        XrManager::RenderFrameState rf{}; if(!xr_manager_->CopyActiveRenderFrameState(rf)||!rf.stereo_views_valid) return false;
        std::uint32_t sw=1920,sh=1800; xr_manager_->GetLastCapturedSourceResolution(sw,sh);
        const float aspect=sh?static_cast<float>(sw)/static_cast<float>(sh):(1920.0f/1800.0f);
        float maxx=0.0f, top=0.0f, down=0.0f; bool first=true;
        for(int e=0;e<2;++e){
            const auto& f=rf.eye_fov[e];
            const float l=std::tan(f.angleLeft),r=std::tan(f.angleRight),u=std::tan(f.angleUp),d=std::tan(f.angleDown);
            maxx=std::max(maxx,std::max(std::fabs(l),std::fabs(r)));
            top=std::max(top,u); if(first||d<down) down=d; first=false;
        }
        const float vertical_half=0.5f*(top-down);
        const float needed_tan=std::max(vertical_half,maxx/std::max(aspect,0.5f));
        out_required_centered=2.0f*std::atan(needed_tan)*57.29577951308232f;
        // Same no-shift coverage diagnostic formerly calculated separately by
        // GetRequiredMonoSourceVFovDeg() on every frame.
        const float maxy=std::max(std::fabs(top),std::fabs(down));
        const float no_shift_tan=std::max(maxy,maxx/std::max(aspect,0.5f));
        out_required_no_shift=2.0f*std::atan(no_shift_tan)*57.29577951308232f;
        const float source_tan=std::tan(actual_vfov_deg*0.5f*0.01745329251994329577f);
        if(!(source_tan>0.05f)) return false;
        // Center asymmetric top/down rays in the symmetric source texture.
        out_center_y=std::clamp((top+down)/(4.0f*source_tan),-0.20f,0.20f);
        return true;
    };

    while (running_)
    {
        const bool f6_down = (GetAsyncKeyState(VK_F6) & 0x8000) != 0;
        const bool f7_down = (GetAsyncKeyState(VK_F7) & 0x8000) != 0;
        const bool f9_down = vrik_probe_installed && ((GetAsyncKeyState(VK_F9) & 0x8000) != 0);
        // V62.9 developer ladder. The release-facing menu still has only NATIVE TOUCH / VRIK.
        // F9 is the explicit headset-test switch: OFF -> READ -> HAND DELTA -> TWO-BONE POSITIONS -> OFF.
        if(!matrix_active && f9_down && !f9_down_prev) {
            frik_pose_probe::CycleWriteMode();
            const auto vs=frik_pose_probe::GetStatus();
            char fm[320]{};
            std::snprintf(fm,sizeof(fm),"V62.9-VRIK F9 mode=%d %s calibrated=%d. Start empty-handed; if READ stays rig=false/setIK=0, do one normal interaction or draw/sheath once to seed player correlation.",
                vs.mode,frik_pose_probe::ModeName(vs.mode),vs.calibrated?1:0);
            log().warning("{}",fm);
            diagnostic_trace::Write(fm);
            if(!PlaySoundW(vs.mode==0?L"SystemExclamation":L"SystemAsterisk",nullptr,SND_ALIAS|SND_ASYNC|SND_NODEFAULT)) MessageBeep(vs.mode==0?MB_ICONHAND:MB_ICONASTERISK);
        }
        // V61.36 CLEAN: other diagnostic hotkeys and recorders remain retired.
        if (!matrix_active && f6_down && !f6_down_prev) {
            // V62.4: AUTO -> FORCE SCREEN -> FORCE IMMERSIVE -> AUTO.
            // AUTO state is never destroyed by a manual F6 press, so returning to
            // AUTO always restores menu/inventory/cutscene ownership correctly.
            if(!flat_ui_manual && !flat_ui_force_immersive) {
                flat_ui_manual=true;
                debug_trace::Write("V62.4-F6 override=FORCE_SCREEN -> LOCAL world-locked quad");
                std::thread([](){Beep(660,110);}).detach();
            } else if(flat_ui_manual) {
                flat_ui_manual=false; flat_ui_force_immersive=true;
                debug_trace::Write("V62.4-F6 override=FORCE_IMMERSIVE -> projection forced even if AUTO requests screen");
                std::thread([](){Beep(480,110);}).detach();
            } else {
                flat_ui_force_immersive=false;
                debug_trace::Write("V62.4-F6 override=AUTO -> state-driven virtual screen restored");
                std::thread([](){Beep(360,110);}).detach();
            }
            startup_ready_since_ms=0;
        }
        // Projection correction is selected from the menu; no F8 test toggle in clean build.
        if (!matrix_active && f7_down && !f7_down_prev && vr_menu_) {
            vr_menu_->Toggle();
            debug_trace::Write(vr_menu_->IsVisible() ? "V58.3-F7 menu opened: dedicated XR layer; desktop mirror gated OFF by default; LEFT-stick nav" : "V58-F7 menu closed; gameplay projection unchanged");
        }
        if (vr_menu_ && vr_menu_->IsVisible()) {
            vr_menu_->PumpInput();
            // V58.2: corrected user layout. The LEFT stick is the menu D-pad,
            // while the right controller keeps the pointer ray, trigger, A and B.
            // Either controller may keep navigation alive if the other hand is
            // temporarily untracked; right-hand selection simply remains idle.
            const auto& lc=xr_manager_->LeftController();
            const auto& rc=xr_manager_->RightController();
            int px=0,py=0; const bool pv=xr_manager_->GetMenuPointer(px,py);
            const float lmag=lc.thumb_x*lc.thumb_x+lc.thumb_y*lc.thumb_y;
            const float rmag=rc.thumb_x*rc.thumb_x+rc.thumb_y*rc.thumb_y;
            const float navx=(rmag>lmag)?rc.thumb_x:lc.thumb_x;
            const float navy=(rmag>lmag)?rc.thumb_y:lc.thumb_y;
            vr_menu_->PumpVrInput(lc.active || rc.active,px,py,pv,rc.trigger,navx,navy,rc.a_pressed,rc.b_pressed);
        }
        // V62.4 AUTO F6 default-key fallback for keyboard-opened APSE screens.
        // These are intentionally only fallbacks (custom key remaps are not inferred).
        const bool key_i=(GetAsyncKeyState('I')&0x8000)!=0;
        const bool key_m=(GetAsyncKeyState('M')&0x8000)!=0;
        const bool key_esc=(GetAsyncKeyState(VK_ESCAPE)&0x8000)!=0;
        if(!(vr_menu_&&vr_menu_->IsVisible())) {
            if(key_i&&!key_i_prev) { inventory_flat_ui=!inventory_flat_ui; map_flat_ui=false; pause_flat_ui=false; }
            if(key_m&&!key_m_prev) { map_flat_ui=!map_flat_ui; inventory_flat_ui=false; pause_flat_ui=false; }
            if(key_esc&&!key_esc_prev) {
                if(inventory_flat_ui||map_flat_ui) { inventory_flat_ui=false; map_flat_ui=false; }
                else pause_flat_ui=!pause_flat_ui;
            }
        }
        key_i_prev=key_i; key_m_prev=key_m; key_esc_prev=key_esc;

        ensure_good_mono_projection();
        xr_manager_->SetFlatUiScale(vr_flat_screen_scale_.load(std::memory_order_relaxed));
        xr_manager_->SetFlatUiIpdMm(0.0f);

        const float published_fov_rad = FrameSync::LatestPublishedFovRadians();
        const float published_fov_deg = published_fov_rad * 57.29577951308232f;
        const bool have_published_fov = FrameSync::PublishedFovSampleCount() > 0 && published_fov_rad > 0.50f && published_fov_rad < 2.70f;
        const std::uint64_t now_ms = static_cast<std::uint64_t>(GetTickCount64());
        const bool cutscene_flat_ui = cutscene_context_cvar && SafeCVarGetInt(cutscene_context_cvar,0)==1;
        const bool dialogue_active = dialogue_context_cvar && SafeCVarGetInt(dialogue_context_cvar,0)>0;
        const int dialogue_mode_now = std::clamp(vr_dialogue_camera_mode_.load(std::memory_order_relaxed),0,2);
        const bool native_dialogue_flat_ui = dialogue_active && dialogue_mode_now==0;

        if(lua_sync_stage<4 && now_ms-lua_sync_epoch_ms>=lua_sync_delays_ms[lua_sync_stage]) {
            VrMenuState lua_state{}; FillMenuState(lua_state);
            const bool lua_sent=SyncLuaUiCompanion(lua_state);
            if(lua_sync_stage==0 || lua_sync_stage==3) {
                char lm[192]{}; std::snprintf(lm,sizeof(lm),"V62.9-LUA startup sync stage=%d sent=%s",lua_sync_stage+1,lua_sent?"yes":"no");
                diagnostic_trace::Write(lm);
            }
            ++lua_sync_stage;
        }

        if(inventory_flat_ui) inventory_was_open_for_hud = true;
        if(inventory_was_open_for_hud && !inventory_flat_ui && hud_reapply_attempts==0) {
            inventory_was_open_for_hud = false;
            hud_reapply_attempts = 6;
            hud_reapply_next_ms = now_ms + 120ull;
            diagnostic_trace::Write("V62.9-HUD inventory closed; scheduled bounded HUD reapply");
        }
        if(hud_reapply_attempts>0 && now_ms>=hud_reapply_next_ms) {
            const bool ok = ApplyNativeVrUiLayout(
                enable_vr_ui_layout_.load(std::memory_order_acquire),
                vr_ui_subtitle_scale_.load(std::memory_order_acquire),vr_ui_subtitle_y_.load(std::memory_order_acquire),
                vr_ui_dialogue_scale_.load(std::memory_order_acquire),vr_ui_dialogue_y_.load(std::memory_order_acquire),vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire),
                vr_ui_hints_scale_.load(std::memory_order_acquire),vr_ui_hints_inset_pct_.load(std::memory_order_acquire),vr_ui_hints_y_.load(std::memory_order_acquire));
            char hm[192]{}; std::snprintf(hm,sizeof(hm),"V62.9-HUD reapply after inventory ok=%s remaining=%d",ok?"yes":"no",hud_reapply_attempts-1);
            diagnostic_trace::Write(hm);
            --hud_reapply_attempts;
            hud_reapply_next_ms = now_ms + (ok ? 350ull : 180ull);
            if(ok && hud_reapply_attempts>2) hud_reapply_attempts=2;
        }
        // V62.7: KCD2 recreates/rewrites HUD MovieClips in more contexts than
        // Inventory. Reassert the desired semantic layout + visibility at low
        // frequency. This is control-thread work, not a render/skinning hot hook.
        static std::uint64_t hud_keepalive_ms=0;
        if(now_ms-hud_keepalive_ms>=1000ull){
            hud_keepalive_ms=now_ms;
            (void)ApplyNativeVrUiLayout(
                enable_vr_ui_layout_.load(std::memory_order_acquire),
                vr_ui_subtitle_scale_.load(std::memory_order_acquire),vr_ui_subtitle_y_.load(std::memory_order_acquire),
                vr_ui_dialogue_scale_.load(std::memory_order_acquire),vr_ui_dialogue_y_.load(std::memory_order_acquire),vr_ui_dialogue_inset_pct_.load(std::memory_order_acquire),
                vr_ui_hints_scale_.load(std::memory_order_acquire),vr_ui_hints_inset_pct_.load(std::memory_order_acquire),vr_ui_hints_y_.load(std::memory_order_acquire));
            const bool cursor=hud_crosshair_visible_.load(std::memory_order_acquire);
            bool cursor_ok=flashui_probe::SetHudMovieClipVisible("Cursor",cursor);
            if(!cursor_ok){
                cursor_ok=flashui_probe::SetHudMovieClipVisible("CursorOutline",cursor);
                (void)flashui_probe::SetHudMovieClipVisible("CursorCross",cursor);
                (void)flashui_probe::SetHudMovieClipVisible("CombatCursorCross",cursor);
                (void)flashui_probe::SetHudMovieClipVisible("CursorCrossPBSlot",cursor);
                (void)flashui_probe::SetHudMovieClipVisible("CursorCrossOpponentLock",cursor);
            }
            (void)flashui_probe::SetHudMovieClipVisible("Stats",hud_lower_icons_visible_.load(std::memory_order_acquire));
            (void)flashui_probe::SetHudMovieClipVisible("Compass",hud_compass_visible_.load(std::memory_order_acquire));
        }

        // In forced first-person mode, remove the HUD ratio strips if the live
        // retail FlashUI object is available. This does not touch cinematic
        // camera geometry and is reversed as soon as the dialogue/mode changes.
        const bool want_ratio_hidden = dialogue_active && dialogue_mode_now==1;
        if(want_ratio_hidden!=dialogue_ratio_hidden) {
            if(flashui_probe::SetHudMovieClipVisible("RatioStrips",!want_ratio_hidden)) {
                dialogue_ratio_hidden=want_ratio_hidden;
                debug_trace::Write(want_ratio_hidden?"V62.4-DIALOGUE RatioStrips hidden for forced 1st person":"V62.4-DIALOGUE RatioStrips restored");
            }
        }
        const bool auto_flat_ui_pre = startup_flat_ui_auto || inventory_flat_ui || map_flat_ui || pause_flat_ui || cutscene_flat_ui || native_dialogue_flat_ui;
        const bool effective_flat_ui_pre = flat_ui_manual || (!flat_ui_force_immersive && auto_flat_ui_pre);
        if (vrik_probe_installed && frik_pose_probe::LiveRightArmEnabled() && now_ms-vrik_last_status_ms>=1000ull) {
            vrik_last_status_ms=now_ms;
            const auto vs=frik_pose_probe::GetStatus();
            log().warning("V62.9 VRIK mode={}({}) rig={} cal={} tracking={} allowed={} joints={} handR={} b1Len={:.3f}/{:.3f} refLen={:.3f}/{:.3f} ctrlDelta={:.3f} preWrites={} setIK={} jC/jE={}/{} rejects={} failStage={}",
                vs.mode,frik_pose_probe::ModeName(vs.mode),vs.rig_valid,vs.calibrated,vs.tracking_valid,vs.gameplay_allowed,vs.joint_count,vs.hand_r,
                vs.upper_length,vs.fore_length,vs.ref_upper_length,vs.ref_fore_length,vs.controller_delta,vs.prebuild_writes,vs.setik_calls,vs.solver_joint_c,vs.solver_joint_e,vs.rejects,vs.fail_stage);
            frik_pose_probe::VrikDiagnostics vd{};
            if (frik_pose_probe::GetDiagnostics(vd)) {
                char dl[4600]{};
                std::snprintf(dl,sizeof(dl),
                    "V62.9-VRIK sample=%llu mode=%d ids U/F/E/H=%d/%d/%d/%d parents=%d/%d/%d/%d solverC/E=%d/%d "
                    "b1Len=%.4f/%.4f refLen=%.4f/%.4f ctrlCry=(%.4f,%.4f,%.4f) "
                    "b0 U=(%.4f,%.4f,%.4f) F=(%.4f,%.4f,%.4f) E=(%.4f,%.4f,%.4f) H=(%.4f,%.4f,%.4f) "
                    "b1 U=(%.4f,%.4f,%.4f) F=(%.4f,%.4f,%.4f) E=(%.4f,%.4f,%.4f) H=(%.4f,%.4f,%.4f) "
                    "ref U=(%.4f,%.4f,%.4f) F=(%.4f,%.4f,%.4f) E=(%.4f,%.4f,%.4f) H=(%.4f,%.4f,%.4f) "
                    "DQ(U-F-H) c0=%.4f/%.4f c1=%.4f/%.4f c2=%.4f/%.4f c3=%.4f/%.4f "
                    "outU=[%.5f %.5f %.5f %.5f | %.5f %.5f %.5f %.5f] "
                    "outF=[%.5f %.5f %.5f %.5f | %.5f %.5f %.5f %.5f] "
                    "outE=[%.5f %.5f %.5f %.5f | %.5f %.5f %.5f %.5f] "
                    "outH=[%.5f %.5f %.5f %.5f | %.5f %.5f %.5f %.5f]",
                    static_cast<unsigned long long>(vd.sample),vs.mode,vd.upper,vd.fore,vd.elbow,vd.hand,
                    vd.parent_upper,vd.parent_fore,vd.parent_elbow,vd.parent_hand,vd.solver_joint_c,vd.solver_joint_e,
                    vd.b1_l1,vd.b1_l2,vd.ref_l1,vd.ref_l2,vd.controller_delta_cry[0],vd.controller_delta_cry[1],vd.controller_delta_cry[2],
                    vd.b0_upper[0],vd.b0_upper[1],vd.b0_upper[2],vd.b0_fore[0],vd.b0_fore[1],vd.b0_fore[2],vd.b0_elbow[0],vd.b0_elbow[1],vd.b0_elbow[2],vd.b0_hand[0],vd.b0_hand[1],vd.b0_hand[2],
                    vd.b1_upper[0],vd.b1_upper[1],vd.b1_upper[2],vd.b1_fore[0],vd.b1_fore[1],vd.b1_fore[2],vd.b1_elbow[0],vd.b1_elbow[1],vd.b1_elbow[2],vd.b1_hand[0],vd.b1_hand[1],vd.b1_hand[2],
                    vd.ref_upper[0],vd.ref_upper[1],vd.ref_upper[2],vd.ref_fore[0],vd.ref_fore[1],vd.ref_fore[2],vd.ref_elbow[0],vd.ref_elbow[1],vd.ref_elbow[2],vd.ref_hand[0],vd.ref_hand[1],vd.ref_hand[2],
                    vd.cand_l1[0],vd.cand_l2[0],vd.cand_l1[1],vd.cand_l2[1],vd.cand_l1[2],vd.cand_l2[2],vd.cand_l1[3],vd.cand_l2[3],
                    vd.out_upper[0],vd.out_upper[1],vd.out_upper[2],vd.out_upper[3],vd.out_upper[4],vd.out_upper[5],vd.out_upper[6],vd.out_upper[7],
                    vd.out_fore[0],vd.out_fore[1],vd.out_fore[2],vd.out_fore[3],vd.out_fore[4],vd.out_fore[5],vd.out_fore[6],vd.out_fore[7],
                    vd.out_elbow[0],vd.out_elbow[1],vd.out_elbow[2],vd.out_elbow[3],vd.out_elbow[4],vd.out_elbow[5],vd.out_elbow[6],vd.out_elbow[7],
                    vd.out_hand[0],vd.out_hand[1],vd.out_hand[2],vd.out_hand[3],vd.out_hand[4],vd.out_hand[5],vd.out_hand[6],vd.out_hand[7]);
                diagnostic_trace::Write(dl);
            }
        }
        // V61.43 PERF: coverage trig/mutex work is no longer done every XR
        // frame. It is refreshed inside the 200 ms auto-projection block below.
        float required_crop_vfov_deg = vr_required_source_vfov_deg_.load(std::memory_order_relaxed);

        // V61.45: transient dialogue/inventory zoom writer retired.
        // Keep gameplay cl_fov and the compositor source-FOV model unchanged here.
        // The V60.1 UI/DIALOG FOV GUARD below intentionally ignores transient
        // dialogue/inventory camera publications for VR source mapping.

        // V61.38: depth is live only while the DEPTH PSEUDO mode is selected.
        // Leaving that button disables candidate scoring and shader depth use
        // immediately; the installed descriptor hooks remain cheap pass-throughs.
        f8_auto_projection_active = vr_cyclopean_mode_.load(std::memory_order_acquire) != 0;
        vr_auto_projection_active_.store(f8_auto_projection_active, std::memory_order_release);
        if (xr_manager_) {
            const int clean_mode = std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire), 0, 3);
            const bool depth_mode = clean_mode == 3;
            xr_manager_->SetDepthPushWeight(vr_depth_stereo_strength_.load(std::memory_order_acquire));
            xr_manager_->SetDepthPushEnabled(depth_mode);
            if (DeviceHook::GenericDepthProbeEnabled() != depth_mode) DeviceHook::SetGenericDepthProbe(depth_mode);
        }
        if (!f8_auto_projection_active && xr_manager_) {
            vr_presentation_center_y_.store(0.0f, std::memory_order_release);
            xr_manager_->SetPresentationCenterY(0.0f);
        }
        if(!matrix_active && f8_auto_projection_active && xr_manager_ && !effective_flat_ui_pre
           && (now_ms - f8_last_adjust_ms >= 200ull)) {
            // V61.40/V61.43: this whole block - eye-FOV trig/coverage math plus
            // presentation setter calls - was running
            // unconditionally every single main-loop tick for any mode other than
            // OLD MONO, unthrottled, even though its inputs (game FOV, source
            // mapper FOV, trims, zoom) only actually change at menu-slider speed.
            // The status log right below was already throttled to 1/sec; this
            // reuses the same pattern (f8_last_adjust_ms, declared but never wired
            // to anything) at 200ms - still feels instant on a slider drag, but
            // cuts the real per-tick cost this introduced versus OLD MONO by ~18x
            // at a typical 90Hz tick rate. No visual/behavioral change intended.
            f8_last_adjust_ms=now_ms;
            const float actual=CameraHook::LatestGameplayFovDegrees();
            const auto samples=CameraHook::GameplayFovSampleCount();
            if(samples>0 && actual>30.0f && actual<150.0f){
                // V61.34 calibration: game FOV and compositor/view FOV are deliberately
                // independent again. The old menu exposes both. F8 only derives the
                // headset optical-center correction from the *selected view mapper FOV*.
                const float mapperFov=std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_acquire),70.0f,140.0f);
                float autoCenter=0.0f,requiredCentered=0.0f,requiredNoShift=0.0f;
                if(compute_auto_projection(mapperFov,autoCenter,requiredCentered,requiredNoShift)){
                    required_crop_vfov_deg=requiredNoShift;
                    vr_required_source_vfov_deg_.store(requiredNoShift,std::memory_order_release);
                    const float fovTrim=vr_auto_fov_trim_deg_.load(std::memory_order_acquire);
                    const float centerTrim=vr_auto_center_y_trim_.load(std::memory_order_acquire);
                    const float target=std::clamp(requiredCentered + 1.0f + fovTrim,70.0f,145.0f); // diagnostic suggestion only
                    const float effectiveCenter=std::clamp(autoCenter + centerTrim,-0.20f,0.20f);
                    vr_presentation_center_y_.store(effectiveCenter,std::memory_order_release);
                    xr_manager_->SetPresentationCenterY(effectiveCenter);
                    const int activeCyclo=std::clamp(vr_cyclopean_mode_.load(std::memory_order_acquire),0,3);
                    const V603ViewPreset activePreset=ViewPresetForMode(activeCyclo);
                    xr_manager_->SetPresentationSettings(
                        vr_render_scale_.load(std::memory_order_relaxed), activePreset.presentation_mode,
                        vr_zoom_.load(std::memory_order_relaxed), 0.0f,0.0f,0.0f,0.0f, mapperFov);
                    xr_manager_->SetAngularCorrectMono(activePreset.angular_correct);
                    xr_manager_->SetPoseHonestyOverride(activePreset.pose_honesty);
                    xr_manager_->SetMonoFovAsymmetryScale(activePreset.fov_asymmetry);
                    xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(activeCyclo));
                    xr_manager_->SetMonoUvBinocularOffset(activePreset.uv_binocular);
                    const float gameNow=ReadCurrentGameFovValue();
                    // V61.34 calibration build: do not chase/overwrite cl_fov here.
                    // The old menu Apply button owns game FOV so headset tuning is possible.
                    if(now_ms-f8_last_status_ms>=1000ull){
                        f8_last_status_ms=now_ms;
                        char fm[640]{}; std::snprintf(fm,sizeof(fm),
                            "V61.34-F8 STATUS actualGameplayVFOV=%.3f suggestedCoverageVFOV=%.3f requiredCentered=%.3f requiredNoShift=%.3f gameCVar=%.3f sourceMapper=%.3f centerY={auto:%.5f trim:%+.5f effective:%.5f} fovTrim=%+.2f zoom=%.3f samples=%llu coverage=%s",
                            double(actual),double(target),double(requiredCentered),double(required_crop_vfov_deg),double(ReadCurrentGameFovValue()),double(mapperFov),
                            double(autoCenter),double(centerTrim),double(effectiveCenter),double(fovTrim),double(vr_zoom_.load(std::memory_order_relaxed)),
                            (unsigned long long)samples,mapperFov+0.15f>=requiredCentered?"YES":"NO"); debug_trace::Write(fm);
                    }
                }
            } else if(now_ms-f8_last_status_ms>=1000ull){
                f8_last_status_ms=now_ms; debug_trace::Write("V61.32-F8 AUTO WAIT exact gameplay CView FOV sample not available yet");
            }
        }

        // V59.2 startup F6 exit: the headset test proved that the ~81.69deg
        // published camera produced by game FOV 110 has the correct world scale
        // and head motion even though it does NOT cover the Quest lower ray.
        // Therefore coverage is no longer a prerequisite. Keep the ~75deg
        // startup/menu camera on the comfort quad, but accept a stable >=80deg
        // gameplay camera and let the bottom-edge treatment handle only the gap.
        if (!matrix_active && startup_flat_ui_auto) {
            const float game_fov_now = ReadCurrentGameFovValue();
            const bool source_covers = game_fov_now > 1.0f && have_published_fov && published_fov_deg >= 80.0f &&
                CameraHook::LastPoseFrameIndex() > 0 && DeviceHook::PresentCaptureCount() > 8;
            if (source_covers) {
                if (startup_ready_since_ms == 0) {
                    startup_ready_since_ms = now_ms;
                    char m[256]{};
                    std::snprintf(m,sizeof(m),"V59.5-STARTUP-F6 gameplay-ready publishedVFOV=%.2f requiredFullCover=%.2f margin=%.2f; 100ms guard started",
                        published_fov_deg,required_crop_vfov_deg,published_fov_deg-required_crop_vfov_deg);
                    debug_trace::Write(m);
                } else if (now_ms - startup_ready_since_ms >= 100ull) {
                    startup_flat_ui_auto = false;
                    startup_ready_since_ms = 0;
                    debug_trace::Write("V61.10-STARTUP-F6 AUTO EXIT -> gameplay camera >=80deg confirmed; immersive projection");
                }
            } else {
                startup_ready_since_ms = 0;
            }
        }

        const bool auto_flat_ui = startup_flat_ui_auto || inventory_flat_ui || map_flat_ui || pause_flat_ui || cutscene_flat_ui || native_dialogue_flat_ui;
        const bool safe_ui_fit = flat_ui_manual || (!flat_ui_force_immersive && auto_flat_ui);
        xr_manager_->SetFlatUiMode(safe_ui_fit);
        if (safe_ui_fit != tracking_suspended_for_flat_ui) {
            tracking_suspended_for_flat_ui = safe_ui_fit;
            CameraHook::SetTrackingOptions(
                safe_ui_fit ? false : apply_head_rotation_.load(std::memory_order_relaxed),
                safe_ui_fit ? false : apply_head_position_.load(std::memory_order_relaxed));
            debug_trace::Write(safe_ui_fit
                ? "V58.7-F6-CAMERA HMD injection SUSPENDED while world quad is active"
                : "V58.7-F6-CAMERA HMD injection RESTORED for immersive view");
        }

        // V62.9 dialogue/source-FOV restore. The known-good V60.1/V61.36 rule was
        // that transient dialogue/inventory camera publications must never become
        // the compositor's source-FOV calibration. Keep the user-confirmed source
        // VFOV authoritative while immersive; F6 theater has its own flat geometry.
        if(!matrix_active && !safe_ui_fit && xr_manager_) {
            static std::uint64_t last_source_guard_ms=0;
            if(now_ms-last_source_guard_ms>=250ull) {
                last_source_guard_ms=now_ms;
                xr_manager_->SetSourceVerticalFov(vr_source_vertical_fov_deg_.load(std::memory_order_acquire));
            }
        }

        if (false && !matrix_active) {
            // V60.3 retired in V60.4: manual source-FOV calibration is independent.
            {
                const std::uint64_t not_before = pending_fov_calibration_not_before_ms_.load(std::memory_order_acquire);
                const std::uint64_t deadline = pending_fov_calibration_deadline_ms_.load(std::memory_order_acquire);
                if (deadline != 0 && now_ms >= not_before) {
                    bool committed = false;
                    if (have_published_fov && published_fov_deg >= 30.0f && published_fov_deg <= 140.0f) {
                        const float last = fov_calibration_last_sample_deg_.load(std::memory_order_relaxed);
                        std::uint64_t stable_since = fov_calibration_stable_since_ms_.load(std::memory_order_relaxed);
                        if (last > 1.0f && std::fabs(published_fov_deg - last) <= 0.20f) {
                            if (stable_since == 0) stable_since = now_ms;
                        } else {
                            stable_since = now_ms;
                        }
                        fov_calibration_last_sample_deg_.store(published_fov_deg, std::memory_order_release);
                        fov_calibration_stable_since_ms_.store(stable_since, std::memory_order_release);
                        if (now_ms - stable_since >= 300ull) {
                            const float prior = vr_source_vertical_fov_deg_.load(std::memory_order_relaxed);
                            vr_source_vertical_fov_deg_.store(published_fov_deg, std::memory_order_release);
                            char cal_msg[320]{};
                            std::snprintf(cal_msg,sizeof(cal_msg),
                                "V60.3-FOV-STABLECAL priorSourceVFOV=%.2f realPublishedVFOV=%.2f delta=%.2f stableMs=%llu",
                                static_cast<double>(prior), static_cast<double>(published_fov_deg), static_cast<double>(published_fov_deg-prior),
                                static_cast<unsigned long long>(now_ms-stable_since));
                            debug_trace::Write(cal_msg);
                            committed = true;
                        }
                    }
                    if (!committed && now_ms >= deadline) {
                        const float latest = fov_calibration_last_sample_deg_.load(std::memory_order_relaxed);
                        if (latest >= 30.0f && latest <= 140.0f) {
                            const float prior = vr_source_vertical_fov_deg_.load(std::memory_order_relaxed);
                            vr_source_vertical_fov_deg_.store(latest, std::memory_order_release);
                            char cal_msg[320]{};
                            std::snprintf(cal_msg,sizeof(cal_msg),
                                "V60.3-FOV-STABLECAL timeout priorSourceVFOV=%.2f latestPublishedVFOV=%.2f delta=%.2f",
                                static_cast<double>(prior), static_cast<double>(latest), static_cast<double>(latest-prior));
                            debug_trace::Write(cal_msg);
                        } else {
                            debug_trace::Write("V60.3-FOV-STABLECAL timeout: no valid published FOV; keeping CVar readback model");
                        }
                        committed = true;
                    }
                    if (committed) {
                        pending_fov_calibration_not_before_ms_.store(0, std::memory_order_release);
                        pending_fov_calibration_deadline_ms_.store(0, std::memory_order_release);
                        fov_calibration_stable_since_ms_.store(0, std::memory_order_release);
                    }
                }
            }
            const int active_view_mode = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed),0,5);
            const V603ViewPreset active_view = ViewPresetForMode(active_view_mode);
            float active_asymmetry = active_view.fov_asymmetry;
            // V61.37 CLEAN: no focus-driven sweep; mode 2 owns fixed .90, mode 3 owns depth-varying .70..1.00.
            xr_manager_->SetAngularCorrectMono(active_view.angular_correct);
            xr_manager_->SetPoseHonestyOverride(active_view.pose_honesty);
            xr_manager_->SetMonoFovAsymmetryScale(active_asymmetry);
            // V60.8: V60.7 user test showed phase B (source mapping matched to submitted pseudo FOV)
            // removed the visible warp. Keep MONO on the plain path; pseudo modes use matched mapping.
            xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(active_view_mode));
            xr_manager_->SetMonoUvBinocularOffset(active_view.uv_binocular);
            vr_presentation_mode_.store(active_view.presentation_mode, std::memory_order_relaxed);
            float source_vfov_deg = std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed), 70.0f, 140.0f);
            // V60.1 UI/DIALOG FOV GUARD: do not let transient dialogue,
            // inventory, inspection, or cinematic camera publications rescale
            // the VR compositor. Keep the source model locked to the confirmed
            // KCD2 render FOV selected by the user / F8 overscan ladder.
            // We still log published_fov_deg below for diagnosis.
            (void)have_published_fov;
            xr_manager_->SetPresentationSettings(vr_render_scale_.load(std::memory_order_relaxed), active_view.presentation_mode, vr_zoom_.load(std::memory_order_relaxed),
                0.0f,0.0f,0.0f,0.0f, source_vfov_deg);
            if (now_ms - angular_sync_last_log_ms >= 2000ull) {
                angular_sync_last_log_ms = now_ms;
                char am[512]{};
                std::snprintf(am,sizeof(am),
                    "V58.7-MONO-SYNC mode=%s publishedVFOV=%.2f requiredCropVFOV=%.2f margin=%.2f legacyVFOV=%.1f usedSourceVFOV=%.2f framing=%.3f f6=%s",
                    crysis_crop_mono ? "CRYSIS_CROP" : "LEGACY_CLAMP",
                    static_cast<double>(published_fov_deg),
                    static_cast<double>(required_crop_vfov_deg),
                    static_cast<double>(published_fov_deg-required_crop_vfov_deg),
                    static_cast<double>(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed)),
                    static_cast<double>(source_vfov_deg),
                    static_cast<double>(vr_zoom_.load(std::memory_order_relaxed)),
                    safe_ui_fit ? "ON" : "off");
                debug_trace::Write(am);
            }
        }
        // V61.34 F4 is observation-only generic depth discovery. It never
        // changes pseudo strength, DOF state, presentation, or resource contents.

        const bool graphics_refresh = graphics_profile_refresh_requested_.exchange(false, std::memory_order_acq_rel);
        refresh_vr_graphics_profile(graphics_refresh);
        refresh_game_fov_once();
        refresh_dialogue_camera_mode();

        // V62.4: generic MasterSwitch/percentage text controls remain retired.
        // Native semantic UI is issued directly from the menu/control thread against
        // declared HUD MovieClips. The old passive FlashUI observer/retry loop is
        // intentionally gone; do not retain dead references here.

        const bool matrix_requested = matrix_test_requested_.exchange(false, std::memory_order_acq_rel);
        if (matrix_requested) { // V61.15: keyboard F8 path retired
            if(fake_dof_active_.exchange(false,std::memory_order_acq_rel)) {
                fake_dof_started_ms_.store(0,std::memory_order_release);
                debug_trace::Write("V61.12-FAKE-DOF cancelled by manual F8 pseudo preset selection");
            }
            if(engine_dof_active_.exchange(false,std::memory_order_acq_rel))
                debug_trace::Write("V61.12-ENGINE-DOF cancelled by manual F8 pseudo preset selection");
            // V61.5: native stereo branch retired. F8/menu Stereo Test is now a manual
            // single-image pseudo-stereo preset selector. MONO remains the V61.4
            // LEGACY_GOOD_MONO hard gate and is never routed through cyclopean processing.
            static constexpr int kPseudoCycle[4] = {1, 2, 3, 0}; // V61.10: .80 -> .85 -> .95 -> MONO
            const int cur = std::clamp(vr_cyclopean_mode_.load(std::memory_order_relaxed), 0, 3);
            int next = 1;
            for (int i = 0; i < 4; ++i) {
                if (kPseudoCycle[i] == cur) { next = kPseudoCycle[(i + 1) % 4]; break; }
            }
            // From MONO, start at PS1 rather than skipping to PS2.
            if (cur == 0) next = 1;

            vr_stereo_experiment_.store(0, std::memory_order_release);
            vr_cyclopean_mode_.store(next, std::memory_order_release);
            vr_afr_ipd_scale_.store(0.0f, std::memory_order_release);
            DeviceHook::SetAfrStereoCapture(false);
            FrameSync::SetStereoMode(0);
            FrameSync::SetNativeStereoReentryMode(0, 0.064f);

            if (xr_manager_) {
                const V603ViewPreset preset = ViewPresetForMode(next);
                xr_manager_->SetStereoExperiment(0, 0.0f);
                xr_manager_->SetPresentationSettings(
                    vr_render_scale_.load(std::memory_order_relaxed),
                    preset.presentation_mode,
                    vr_zoom_.load(std::memory_order_relaxed),
                    0.0f, 0.0f, 0.0f, 0.0f,
                    vr_source_vertical_fov_deg_.load(std::memory_order_relaxed));
                xr_manager_->SetAngularCorrectMono(preset.angular_correct);
                xr_manager_->SetPoseHonestyOverride(preset.pose_honesty);
                xr_manager_->SetMonoFovAsymmetryScale(preset.fov_asymmetry);
                xr_manager_->SetMonoUvReprojectionMode(MonoMapModeForCleanMode(next));
                xr_manager_->SetMonoUvBinocularOffset(preset.uv_binocular);
            }

            const char* name = "MONO_OLD";
            switch (next) {
                case 1: name = "MONO_CORRECTED_GOODMAP"; break;
                case 2: name = "PSEUDO_090"; break;
                case 3: name = "PSEUDO_DEPTH_070_100"; break;
                default: break;
            }
            char msg[256]{};
            std::snprintf(msg, sizeof(msg),
                "V61.9-PSEUDO-SELECT mode=%d name=%s asym=%.3f uv=%.4f; 0.95 preferred; native/AFR/DUAL off",
                next, name, static_cast<double>(CyclopeanAsymmetryForMode(next)),
                static_cast<double>(ViewPresetForMode(next).uv_binocular));
            debug_trace::Write(msg);
            MessageBeep(MB_OK);
        }
        // Retired automated AFR/DUAL ladder remains unreachable.
        if (false && !matrix_active)
        {
            if (vr_menu_ && vr_menu_->IsVisible()) vr_menu_->SetVisible(false);
            matrix_original_cvars = EngineCVarSnapshot{};
            const bool have_engine_cvars = CaptureEngineCVarSnapshot(matrix_original_cvars);
            matrix_original_fps = FpsCVarSnapshot{};
            const bool have_fps_cvars = CaptureFpsCVarSnapshot(matrix_original_fps);
            matrix_restore_fov = std::clamp(vr_source_vertical_fov_deg_.load(std::memory_order_relaxed), 70.0f, 140.0f);
            matrix_restore_framing = std::clamp(vr_zoom_.load(std::memory_order_relaxed), 0.80f, 2.00f);
            matrix_restore_cull_scale = std::clamp(vr_cull_frustum_scale_.load(std::memory_order_relaxed), 1.0f, 1.60f);
            CameraHook::SetCullFrustumScale(1.0f); // isolate projection test
            matrix_restore_flat_ui_manual = flat_ui_manual;
            matrix_restore_inventory_flat_ui = inventory_flat_ui;
            // V60.6: F8 must be an immersive-world test, never the LOCAL/F6 quad.
            // Preserve the exact pre-test UI state and restore it on completion/cancel.
            flat_ui_manual = false;
            inventory_flat_ui = false;
            startup_flat_ui_auto = false;
            xr_manager_->SetFlatUiMode(false);
            CameraHook::SetTrackingOptions(
                apply_head_rotation_.load(std::memory_order_relaxed),
                apply_head_position_.load(std::memory_order_relaxed));
            tracking_suspended_for_flat_ui = false;
            debug_trace::Write("V60.6-F8-IMMERSIVE forced: F6/local quad OFF for ladder; prior F6 state saved for restore");
            matrix_active = true;
            matrix_running_for_ui_.store(true, std::memory_order_release);
            matrix_cycle_start = capture_matrix_counters();
            // V59.8 hard neutralization: start the cyclopean ladder from a known
            // mono state with no reprojection or binocular UV shift.
            xr_manager_->SetMonoFovAsymmetryScale(1.0f);
            xr_manager_->SetMonoUvReprojectionMode(0);
            xr_manager_->SetMonoUvBinocularOffset(0.0f);
            xr_manager_->SetPresentPosePairingMode(0);
            {
                const float gf = ReadCurrentGameFovValue();
                if (gf > 1.0f) vr_source_vertical_fov_deg_.store(gf, std::memory_order_release);
                char sync[320]{}; std::snprintf(sync,sizeof(sync),
                    "V60.6-F8-SYNC GAME_FOV=%.3f SOURCE_FOV=%.3f MATCHED=%s before-normal-tangent-ladder",
                    gf, vr_source_vertical_fov_deg_.load(std::memory_order_relaxed),
                    (gf>1.0f && std::fabs(gf-vr_source_vertical_fov_deg_.load(std::memory_order_relaxed))<0.01f)?"true":"false");
                debug_trace::Write(sync);
            }
            debug_trace::Write("V61.0-F8-NEUTRAL current GAME/SOURCE FOV preserved; zoom=1 uv=0; F6 forced off; matched-B mapping; stereo mode/IPD varies only");
            char start_msg[512]{};
            std::snprintf(start_msg, sizeof(start_msg),
                "V61.0-F8-START BUILD=AFR-DUAL-TPV-MENU-20260906-N 48-second stereo ladder: AFR IPD scales .25/.50/.75/1.00 then DUAL .50/1.00; current GAME/SOURCE FOV and current source resolution preserved; matched-B tangent map; pseudo UV/reprojection OFF; F6 forced OFF; originals={sys_MaxFPS:%.3f r_Vsync:%d duplicateLast:%d}",
                static_cast<double>(matrix_original_fps.sys_max_fps), matrix_original_fps.r_vsync, matrix_original_fps.duplicate_last_frame);
            debug_trace::Write(start_msg);
            apply_matrix_phase(0);
        }

        if (safe_mono_requested_.exchange(false, std::memory_order_acq_rel))
        {
            vr_cyclopean_mode_.store(0, std::memory_order_relaxed);
            if (matrix_active) {
                debug_trace::Write("V49-MENU Good Mono requested: cancelling active F8 ladder");
                char fps_restore_msg[256]{};
                (void)ApplyFpsCap(matrix_original_fps, 0.0f, fps_restore_msg, sizeof(fps_restore_msg)); char dup_restore[192]{}; (void)ApplyDuplicateLastFrame(matrix_original_fps.duplicate_last_frame,dup_restore,sizeof(dup_restore)); if(dup_restore[0]) debug_trace::Write(dup_restore); FrameSync::SetDenseRotationProbe(false);
                if (fps_restore_msg[0]) debug_trace::Write(fps_restore_msg);
                // Restore while matrix_active is still true so restore_safe_mono
                // uses the pre-test FOV/framing snapshot rather than the current phase.
                DeviceHook::SetSceneColorProbe(false,0,0); FrameSync::SetNativeStereoReentryMode(0,0.064f); RestoreV54StereoCVars(v54_stereo_original);
                xr_manager_->SetLatePoseEnabled(false); xr_manager_->SetPresentPosePairingMode(1); xr_manager_->SetProjectionPoseMode(0);
                xr_manager_->SetStereoExperiment(0,0.0f); DeviceHook::SetAfrStereoCapture(false); FrameSync::SetStereoMode(0); CameraHook::SetAfrIpd(0.064f,0.0f);
                CameraHook::SetInjectionMode(1); CameraHook::SetTrackingOptions(true,true); CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed)); CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed)); CameraHook::SetRotationFilterMode(5); adaptive_rotation_filter=false;
                // F8 ladder addition: same restore as normal completion -
                // without this, cancelling mid-test would leave these stuck
                // at whatever the interrupted phase had set.
                xr_manager_->SetAngularCorrectMono(crysis_crop_mono);
                xr_manager_->SetPoseHonestyOverride(-1);
                xr_manager_->SetMonoUvReprojectionMode(0); xr_manager_->SetMonoUvBinocularOffset(0.0f);
                xr_manager_->SetMonoBottomEdgeMode(0); xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
                restore_safe_mono();
                flat_ui_manual = matrix_restore_flat_ui_manual;
                inventory_flat_ui = matrix_restore_inventory_flat_ui;
                CameraHook::SetCullFrustumScale(matrix_restore_cull_scale);
                matrix_active = false;
                matrix_phase = -1;
                matrix_running_for_ui_.store(false, std::memory_order_release);
            } else {
                restore_safe_mono();
            }
            debug_trace::Write("V47.0-GOOD-MONO applied: Mono + OpenXR FOV + safe capture FOV + separate framing + swap off");
        }

        if (matrix_active)
        {
            const auto now_ms = static_cast<std::uint64_t>(GetTickCount64());
            const std::uint64_t phase_duration_ms = (matrix_phase >= 0 && matrix_phase < kMatrixPhaseCount)
                ? matrix_phases[matrix_phase].duration_ms : 10000ull;
            if (now_ms - matrix_phase_start_ms >= phase_duration_ms)
            {
                finish_matrix_phase();
                if (matrix_phase + 1 < kMatrixPhaseCount)
                {
                    apply_matrix_phase(matrix_phase + 1);
                }
                else
                {
                    const MatrixCounters total = capture_matrix_counters();
                    const auto total_req = delta_u64(total.recursive_requests, matrix_cycle_start.recursive_requests);
                    const auto total_pass = delta_u64(total.recursive_passes, matrix_cycle_start.recursive_passes);
                    const auto total_pipe = delta_u64(total.recursive_pipeline, matrix_cycle_start.recursive_pipeline);
                    const auto total_rt = delta_u64(total.recursive_rt, matrix_cycle_start.recursive_rt);
                    char fps_restore_msg[256]{};
                    (void)ApplyFpsCap(matrix_original_fps, 0.0f, fps_restore_msg, sizeof(fps_restore_msg)); char dup_restore[192]{}; (void)ApplyDuplicateLastFrame(matrix_original_fps.duplicate_last_frame,dup_restore,sizeof(dup_restore)); if(dup_restore[0]) debug_trace::Write(dup_restore); FrameSync::SetDenseRotationProbe(false);
                    if (fps_restore_msg[0]) debug_trace::Write(fps_restore_msg);
                    xr_manager_->SetLatePoseEnabled(false);
                    xr_manager_->SetPresentPosePairingMode(1);
                    xr_manager_->SetProjectionPoseMode(0);
                    xr_manager_->SetStereoExperiment(0,0.0f);
                    DeviceHook::SetAfrStereoCapture(false);
                    FrameSync::SetStereoMode(0);
                    CameraHook::SetAfrIpd(0.064f,0.0f);
                    CameraHook::SetInjectionMode(1);
                    CameraHook::SetTrackingOptions(true,true);
                    CameraHook::SetPoseSourceMode(vr_pose_source_mode_.load(std::memory_order_relaxed),
                                                  vr_pose_prediction_offset_ms_.load(std::memory_order_relaxed));
                    CameraHook::SetRotationOrder(vr_rotation_order_.load(std::memory_order_relaxed));
                    CameraHook::SetRotationFilterMode(5); adaptive_rotation_filter=false;
                    FrameSync::SetNativeStereoReentryMode(0,0.064f);
                    DeviceHook::SetSceneColorProbe(false,0,0);
                    RestoreV54StereoCVars(v54_stereo_original);
                    // F8 ladder addition: restore both overrides back to
                    // normal-gameplay state - crysis_crop_mono is the user's
                    // own toggled preference (same variable normal gameplay
                    // uses), pose_honesty_override_=-1 returns to the default
                    // stereo_experiment_==0 gating. Without this, gameplay
                    // after the F8 test would be stuck on whatever the LAST
                    // ladder phase happened to set.
                    xr_manager_->SetAngularCorrectMono(crysis_crop_mono);
                    xr_manager_->SetPoseHonestyOverride(-1);
                    xr_manager_->SetMonoUvReprojectionMode(0); xr_manager_->SetMonoUvBinocularOffset(0.0f);
                xr_manager_->SetMonoBottomEdgeMode(0); xr_manager_->SetMonoFovAsymmetryScale(CyclopeanAsymmetryForMode(vr_cyclopean_mode_.load(std::memory_order_relaxed)));
                    V54DoneBeep();
                    (void)ApplyNativeGameFov(matrix_restore_fov);
                    vr_source_vertical_fov_deg_.store(matrix_restore_fov, std::memory_order_release);
                    menu_game_fov_target_.store(matrix_restore_fov, std::memory_order_release);
                    restore_safe_mono();
                    flat_ui_manual = matrix_restore_flat_ui_manual;
                    inventory_flat_ui = matrix_restore_inventory_flat_ui;
                    CameraHook::SetCullFrustumScale(matrix_restore_cull_scale);
                    matrix_active = false;
                    matrix_running_for_ui_.store(false, std::memory_order_release);
                    matrix_phase = -1;
                    char done[640]{};
                    std::snprintf(done, sizeof(done),
                        "V61.0-F8-COMPLETE BUILD=AFR-DUAL-TPV-MENU-20260906-N restored pre-test FOV/preset; recursiveTotals={requests:%llu passes:%llu pipeline:%llu rt:%llu}",
                        static_cast<unsigned long long>(total_req), static_cast<unsigned long long>(total_pass),
                        static_cast<unsigned long long>(total_pipe), static_cast<unsigned long long>(total_rt));
                    debug_trace::Write(done);
                }
            }
        }

        // V61.32: physical Insert/Home legacy stereo/eye-swap hotkeys remain retired.
        // F8 is now XR-derived Auto Projection and never arms the old producer/AFR paths.
        const bool menu_open = vr_menu_ && vr_menu_->IsVisible();
        (void)menu_open;
        f6_down_prev = f6_down; f7_down_prev = f7_down; f9_down_prev = f9_down;
        CameraHook::SetTrackingOptions(
            safe_ui_fit ? false : apply_head_rotation_.load(std::memory_order_relaxed),
            safe_ui_fit ? false : apply_head_position_.load(std::memory_order_relaxed));
        XrPose head_pose{};
        // V46: RunFrame blocks on xrWaitFrame(), samples pose/views once at
        // predictedDisplayTime, immediately publishes that head pose to
        // CameraHook, then uses the same cached RenderFrameState for XR
        // composition. No post-xrEndFrame camera update.
        (void)xr_manager_->RunFrame(head_pose, head_pose_scale_.load(std::memory_order_relaxed));
        if(vrik_probe_installed) {
            const auto& rc=xr_manager_->RightController();
            const frik_pose_probe::TrackingPose hp{head_pose.pos_x,head_pose.pos_y,head_pose.pos_z,head_pose.qx,head_pose.qy,head_pose.qz,head_pose.qw};
            const frik_pose_probe::TrackingPose rp{rc.pose.pos_x,rc.pose.pos_y,rc.pose.pos_z,rc.pose.qx,rc.pose.qy,rc.pose.qz,rc.pose.qw};
            const bool vrik_gameplay_allowed=!safe_ui_fit && !(vr_menu_&&vr_menu_->IsVisible()) && !matrix_active;
            frik_pose_probe::PublishTrackingSnapshot(hp,true,rp,rc.active,vrik_gameplay_allowed);
        }

        // V61.32 bow-zoom guard. We do not yet have a verified semantic "bow
        // equipped" action owner, so this deliberately keys off the already-used
        // Right-Grip directional/aim gesture and only reacts when KCD2's exact
        // gameplay VFOV actually collapses by >2 degrees. Melee grip use with a
        // stable FOV performs zero writes. F8's own projection feedback supersedes
        // this guard when F8 AUTO is active.
        {
            static bool guard_grip_prev=false;
            static bool guard_adjusted=false;
            static float guard_baseline_actual=0.0f;
            static float guard_baseline_game=0.0f;
            static std::uint64_t guard_last_adjust_ms=0;
            const auto& r=xr_manager_->RightController();
            const bool guard_grip=r.active && r.grip>0.55f && !safe_ui_fit && !(vr_menu_ && vr_menu_->IsVisible());
            const bool guard_enabled=false; // V61.33 superseded by generic immersive transient-zoom reducer
            if(guard_enabled && guard_grip && !guard_grip_prev){
                guard_baseline_actual=CameraHook::LatestGameplayFovDegrees();
                guard_baseline_game=ReadCurrentGameFovValue();
                guard_adjusted=false; guard_last_adjust_ms=0;
                if(guard_baseline_actual>30.0f && guard_baseline_actual<150.0f){
                    char bm[240]{}; std::snprintf(bm,sizeof(bm),
                        "V61.32-BOW-ZOOM-GUARD-LEGACY armed grip=true baselineActual=%.3f baselineGame=%.3f heuristic=right-grip+FOV-drop",
                        double(guard_baseline_actual),double(guard_baseline_game)); debug_trace::Write(bm);
                }
            }
            if(guard_enabled && guard_grip && !f8_auto_projection_active && guard_baseline_actual>30.0f){
                const float actual=CameraHook::LatestGameplayFovDegrees();
                const std::uint64_t gnow=static_cast<std::uint64_t>(GetTickCount64());
                if(actual>30.0f && actual < guard_baseline_actual-2.0f && gnow-guard_last_adjust_ms>=100ull){
                    guard_last_adjust_ms=gnow;
                    const float gameNow=ReadCurrentGameFovValue();
                    const float correction=std::clamp(guard_baseline_actual-actual,1.0f,6.0f);
                    if(gameNow>1.0f && gameNow<149.5f){
                        const float request=std::min(150.0f,gameNow+correction);
                        if(ApplyNativeGameFovV6128Test(request)){
                            guard_adjusted=true;
                            char bm[280]{}; std::snprintf(bm,sizeof(bm),
                                "V61.32-BOW-ZOOM-GUARD-LEGACY suppress actual=%.3f baseline=%.3f game=%.3f request=%.3f",
                                double(actual),double(guard_baseline_actual),double(gameNow),double(request)); debug_trace::Write(bm);
                        }
                    }
                }
            }
            if((!guard_grip || !guard_enabled) && guard_grip_prev){
                if(guard_adjusted && guard_baseline_game>1.0f && !f8_auto_projection_active){
                    (void)ApplyNativeGameFovV6128Test(guard_baseline_game);
                    char bm[224]{}; std::snprintf(bm,sizeof(bm),
                        "V61.32-BOW-ZOOM-GUARD-LEGACY release restoreGame=%.3f",double(guard_baseline_game)); debug_trace::Write(bm);
                }
                guard_adjusted=false; guard_baseline_actual=0.0f; guard_baseline_game=0.0f;
            }
            guard_grip_prev=guard_grip;
        }

        // V61.8 THEATER-CAL: while an F6/game-theater screen is active, hold RIGHT GRIP
        // and use RIGHT STICK. Y changes physical screen distance; X changes screen size/framing.
        // This never writes cl_fov or the gameplay camera. Right grip remains normal bow-aim outside F6.
        if (safe_ui_fit && xr_manager_) {
            const auto& r = xr_manager_->RightController();
            if (r.active && r.grip > 0.55f) {
                static std::uint64_t last_ms = 0;
                const std::uint64_t now = static_cast<std::uint64_t>(GetTickCount64());
                const float dt = last_ms ? std::min<float>(0.05f, float(now-last_ms)/1000.0f) : 0.0f;
                last_ms = now;
                if (dt > 0.0f) {
                    float dist = xr_manager_->GetFlatUiDistance();
                    float scale = xr_manager_->GetFlatUiScale();
                    if (std::fabs(r.thumb_y) > 0.18f) dist = std::clamp(dist + r.thumb_y * dt * 0.85f, 0.80f, 3.20f);
                    if (std::fabs(r.thumb_x) > 0.18f) scale = std::clamp(scale + r.thumb_x * dt * 0.90f, 0.45f, 3.00f);
                    xr_manager_->SetFlatUiDistance(dist);
                    xr_manager_->SetFlatUiScale(scale);
                    vr_flat_screen_scale_.store(scale, std::memory_order_release);
                    static std::uint64_t last_log=0;
                    if (now-last_log>=250ull) {
                        last_log=now; char tm[256]{}; std::snprintf(tm,sizeof(tm),
                            "V61.8-THEATER-CAL distance=%.2fm scale=%.2f grip=%.2f stick=(%.2f,%.2f)",
                            double(dist),double(scale),double(r.grip),double(r.thumb_x),double(r.thumb_y)); debug_trace::Write(tm);
                    }
                }
            }
        }

        // V61.32 provisional Oculus/Quest Touch compatibility bindings. They use
        // ordinary Win32 input injection and never patch KCD2 input objects. Right
        // grip is now the contextual directional melee/bow modifier; future physical
        // grabbing can reclaim it when a real interaction/weapon-state classifier exists.
        const bool mis_menu_after_frame = vr_menu_ && vr_menu_->IsVisible();
        static FlatUiPointerInjector flat_pointer_injector{};
        if(safe_ui_fit && !mis_menu_after_frame) {
            float pu=0.5f,pv=0.5f; const bool hit=xr_manager_->GetFlatUiPointer(pu,pv);
            const auto& rr=xr_manager_->RightController();
            flat_pointer_injector.Update(FindMainGameWindow(),hit,pu,pv,rr.trigger,rr.a_pressed);
        } else flat_pointer_injector.Release();
        auto touch_events = touch_bindings_.Update(xr_manager_->LeftController(), xr_manager_->RightController(), mis_menu_after_frame, safe_ui_fit && !mis_menu_after_frame);
        if (touch_events.toggle_mis_menu && vr_menu_) {
            vr_menu_->Toggle();
            debug_trace::Write(vr_menu_->IsVisible() ? "V58-TOUCH MIS menu opened by left X" : "V58-TOUCH MIS menu closed by left X");
        }
        if (!matrix_active && touch_events.toggle_inventory_comfort) {
            inventory_flat_ui = !inventory_flat_ui; map_flat_ui=false; pause_flat_ui=false;
            debug_trace::Write(inventory_flat_ui
                ? "V62.4-INVENTORY AUTO SCREEN ON -> LOCAL quad"
                : "V62.4-INVENTORY AUTO SCREEN OFF -> immersive when no other AUTO owner");
        }
        if (!matrix_active && touch_events.toggle_pause_comfort) {
            pause_flat_ui = !pause_flat_ui;
            if(pause_flat_ui) { inventory_flat_ui=false; map_flat_ui=false; }
            debug_trace::Write(pause_flat_ui
                ? "V61.10-PAUSE-THEATER ON -> LOCAL quad"
                : "V61.10-PAUSE-THEATER OFF -> immersive projection restored");
        }
    }

    // Restore any V61.32 temporary test state if the worker stops while a test is ON.
    if(f4_focus_pseudo_active){
        engine_dof_active_.store(f4_restore_engine_dof,std::memory_order_release);
        fake_dof_active_.store(f4_restore_fake_dof,std::memory_order_release);
        fake_dof_started_ms_.store(f4_restore_fake_started,std::memory_order_release);
        engine_dof_scale_.store(f4_restore_engine_scale,std::memory_order_release);
        fake_dof_scale_.store(f4_restore_fake_scale,std::memory_order_release);
        vr_cyclopean_mode_.store(f4_restore_cyclopean_mode,std::memory_order_release);
    }
    if(f8_auto_projection_active){
        // V61.34+ calibration contract: F8 does not own or restore game/source FOV.
        // It only restores the pre-F8 presentation Center-Y on shutdown.
        vr_presentation_center_y_.store(f8_restore_center_y,std::memory_order_release);
        if(xr_manager_) xr_manager_->SetPresentationCenterY(f8_restore_center_y);
        vr_auto_projection_active_.store(false,std::memory_order_release);
    }


    // If shutdown interrupts the 40-second stereo ladder, restore temporary CryEngine
    // CVars before renderer/OpenXR teardown whenever the console is still alive.
    if (matrix_active || matrix_original_cvars.valid) {
        restore_safe_mono();
        matrix_active = false;
        matrix_running_for_ui_.store(false, std::memory_order_release);
    }

    if(dialogue_original.valid) {
        (void)ApplyDialogueMode(dialogue_original,0);
        debug_trace::Write("V58-DIALOGUE shutdown: original native dialogue camera CVars restored");
    }

    // V46.3 quality profile is runtime-only. Restore exactly what was present
    // before MIS touched the ordinary CryEngine graphics CVars.
    if (vr_graphics_applied && vr_graphics_original.valid) {
        char msg[512]{};
        const bool restored = RestoreVrGraphics(vr_graphics_original, msg, sizeof(msg));
        if (msg[0]) debug_trace::Write(msg);
        if (!restored) debug_trace::Write("V46.3-GRAPHICS restore failed during shutdown");
        graphics_profile_applied_for_ui_.store(false, std::memory_order_release);
    }

    touch_bindings_.Reset();

    // Stop new render-thread entries and wait for any callback already inside
    // XrManager before destroying OpenXR/D3D12 state.
    DeviceHook::SetRenderTickCallback(nullptr);
    DeviceHook::WaitForRenderCallbacksIdle();
    if (vr_menu_) {
        vr_menu_->Shutdown();
        vr_menu_.reset();
    }

    // Order matters: XrManager must be fully torn down (it holds its own
    // AddRef'd reference to the game's device/queue) BEFORE DeviceHook
    // releases its own reference.
    xr_manager_->Shutdown();
    xr_manager_ = nullptr; // singleton - clear the pointer, do not delete


    if(vrik_probe_installed) frik_pose_probe::Uninstall();
    flashui_probe::Uninstall();
    RecursiveViewProbe::Uninstall();
    FrameSync::Uninstall();
    stereo_archaeology::Shutdown();

    device_hook_->Uninstall();
    device_hook_.reset();

    camera_hook_->Uninstall();
    camera_hook_.reset();

    log().info("KCD2VR shut down cleanly");
    debug_trace::Write("KCD2VR shut down cleanly");
    log().shutdown();
}

void Mod::Shutdown()
{
    running_ = false;
}

} // namespace kcd2vr