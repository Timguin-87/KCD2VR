#include "native_stereo_probe.h"
#include "debug_trace.h"
#include <windows.h>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <thread>

namespace kcd2vr { namespace {
constexpr std::uintptr_t kStrengthRva = 0x50DBDE8ull;
constexpr std::uintptr_t kEyeDistRva  = 0x50DBDCCull;
constexpr std::uintptr_t kScreenRva   = 0x50DBDC8ull;
constexpr std::uintptr_t kHudRva      = 0x50DBDC0ull;
std::atomic<bool> g_installed{false}, g_stop{false}, g_active{false};
std::thread g_worker;
std::uintptr_t g_base=0;
float g_origStrength=0, g_origEye=0, g_origScreen=0, g_origHud=0;

float ReadF(std::uintptr_t r) noexcept { __try { return *reinterpret_cast<volatile float*>(g_base+r); } __except(EXCEPTION_EXECUTE_HANDLER) { return 0.0f; } }
bool WriteF(std::uintptr_t r,float v) noexcept { __try { *reinterpret_cast<volatile float*>(g_base+r)=v; return true; } __except(EXCEPTION_EXECUTE_HANDLER) { return false; } }
void Restore(){ WriteF(kStrengthRva,g_origStrength); WriteF(kEyeDistRva,g_origEye); WriteF(kScreenRva,g_origScreen); WriteF(kHudRva,g_origHud); }
void LogStage(const char* name){ char m[384]{}; std::snprintf(m,sizeof(m),
    "V61.4-NATIVE-STEREO stage=%s readback={strength:%.6f eye:%.6f screen:%.6f hud:%.6f}",name,
    (double)ReadF(kStrengthRva),(double)ReadF(kEyeDistRva),(double)ReadF(kScreenRva),(double)ReadF(kHudRva)); debug_trace::Write(m); }

void Worker(){
    while(!g_stop.load(std::memory_order_acquire)){
        if(!g_active.exchange(false)){ Sleep(25); continue; }
        g_origStrength=ReadF(kStrengthRva); g_origEye=ReadF(kEyeDistRva); g_origScreen=ReadF(kScreenRva); g_origHud=ReadF(kHudRva);
        LogStage("BASELINE_2S"); Sleep(2000); if(g_stop.load()) break;
        // First wake-up attempt: strength only. Leave all convergence parameters untouched.
        WriteF(kStrengthRva,0.05f); LogStage("STRENGTH_0.05_4S"); Sleep(4000); if(g_stop.load()) break;
        WriteF(kStrengthRva,0.15f); LogStage("STRENGTH_0.15_4S"); Sleep(4000); if(g_stop.load()) break;
        // Final conservative activation: only fill zero/near-zero distances; preserve plausible live defaults.
        float eye=ReadF(kEyeDistRva), screen=ReadF(kScreenRva), hud=ReadF(kHudRva);
        if(!std::isfinite(eye) || std::fabs(eye)<0.00001f) eye=0.032f;
        if(!std::isfinite(screen) || std::fabs(screen)<0.00001f) screen=2.0f;
        if(!std::isfinite(hud) || std::fabs(hud)<0.00001f) hud=1.0f;
        WriteF(kStrengthRva,0.15f); WriteF(kEyeDistRva,eye); WriteF(kScreenRva,screen); WriteF(kHudRva,hud);
        LogStage("STRENGTH_PLUS_DISTANCES_4S"); Sleep(4000);
        Restore(); LogStage("RESTORED");
        debug_trace::Write("V61.4-NATIVE-STEREO-DONE: no hardware breakpoints, no thread suspension; report any visual eye separation / second-view behavior");
    }
    Restore();
}
} // namespace

bool NativeStereoProbe::Install(){
    if(g_installed.exchange(true)) return true;
    g_base=reinterpret_cast<std::uintptr_t>(GetModuleHandleA("WHGame.dll"));
    if(!g_base){g_installed=false;return false;}
    g_stop=false; g_worker=std::thread(Worker);
    debug_trace::Write("V61.4-NATIVE-STEREO installed: safe F8 activation ladder; direct retained-CVar writes only; NO HW watchpoints/thread suspension");
    return true;
}
void NativeStereoProbe::Uninstall(){ if(!g_installed.exchange(false))return; g_stop=true; g_active=false; if(g_worker.joinable())g_worker.join(); Restore(); debug_trace::Write("V61.4-NATIVE-STEREO uninstalled/restored"); }
void NativeStereoProbe::StartWindow(std::uint32_t){ if(!g_installed.load()||g_active.load())return; g_active=true; debug_trace::Write("V61.4-NATIVE-STEREO-TEST requested: 2s baseline + 3x4s activation ladder, then automatic restore"); }
bool NativeStereoProbe::Active(){ return g_active.load(); }
} // namespace kcd2vr
