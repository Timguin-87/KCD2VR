#pragma once
#include <atomic>
namespace kcd2vr::developer_mode {
// Access policy only: it never enables a recorder or changes gameplay preferences.
// The launcher's checkbox is read once per process; later INI reloads do not arm it.
inline std::atomic<int> startup{-1};
inline void ConfigureStartup(bool enabled) noexcept {
    int unconfigured=-1;
    (void)startup.compare_exchange_strong(unconfigured,enabled?1:0,std::memory_order_acq_rel);
}
inline bool Enabled() noexcept {return startup.load(std::memory_order_acquire)==1;}
constexpr bool IsDeveloperShortcut(unsigned functionKey,bool shift=false,bool control=false) noexcept {
    switch(functionKey){
    case 2:case 3:case 4:case 5:case 9:case 11:case 12:return true;
    case 7:return shift; // R124: plain F7 opens the ordinary VR menu, Shift+F7 is the VRIK recorder.
    case 10:return shift||control; // Plain F10 changes normal hand mode.
    default:return false; // F1 body follow, F6 Theater/Immersive toggle, Shift+F8 calibration.
    }
}
inline bool FunctionKeyAvailable(unsigned key,bool shift=false,bool control=false) noexcept {
    return Enabled()||!IsDeveloperShortcut(key,shift,control);
}
}
