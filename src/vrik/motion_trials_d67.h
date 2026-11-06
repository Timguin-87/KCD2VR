#pragma once
// D67/D72: finite F7 pose comparisons coexist with accepted persistent timing. No pose/filter writes.
#include "diagnostics/capture168.h"
#include <atomic>
#include <cstdint>
#include <algorithm>
#include <cmath>
namespace kcd2vr::motion_trials_d67 {
// D70: accepted fresh-publication timing is the missing-key/startup default.
// D72: explicit menu OFF remains available. F7 recording/elbow comparisons
// do not turn accepted timing off; timing is independent of capture lifetime.
// Mode4 is a persistent pose-only choice: F7 never enables token pacing in it.
inline std::atomic<unsigned> menu{2},sessionMode{0};
inline std::atomic<std::uint64_t> session{0},generation{1},elbowGeneration{1};
// Independent hand-source lifetime. F7 capture/rung and elbow settings must
// not invalidate fresh hands and select an older worker pose while mode 4 stays on.
inline std::atomic<std::uint64_t> handRefreshGenerationR94{1};
inline std::atomic<bool> continuityRepair{true};
// Saved elbow choice is independent of presentation pacing. Legacy mode 3 migrates once.
inline std::atomic<bool> elbowPreferenceR92{false};
inline bool ElbowPreferenceR92() noexcept{return elbowPreferenceR92.load(std::memory_order_acquire);}
inline void SetElbowPreferenceR92(bool on) noexcept {
    if(elbowPreferenceR92.exchange(on,std::memory_order_acq_rel)!=on){generation.fetch_add(1);elbowGeneration.fetch_add(1);}
}
inline bool ElbowSelectedForModeR92(unsigned mode) noexcept{return mode==3||ElbowPreferenceR92();}
inline std::atomic<float> elbowOutward{.35f},elbowBack{.25f};
inline float ElbowOutward() noexcept {return elbowOutward.load(std::memory_order_acquire);}
inline float ElbowBack() noexcept {return elbowBack.load(std::memory_order_acquire);}
inline void SetElbowOutward(float v) noexcept {v=std::isfinite(v)?std::clamp(v,.1f,.6f):.35f;if(elbowOutward.exchange(v)!=v){generation.fetch_add(1);elbowGeneration.fetch_add(1);}}
inline void SetElbowBack(float v) noexcept {v=std::isfinite(v)?std::clamp(v,.1f,.6f):.25f;if(elbowBack.exchange(v)!=v){generation.fetch_add(1);elbowGeneration.fetch_add(1);}}
inline unsigned valid(unsigned m) noexcept {return m<=3?m:0;}
inline unsigned MenuMode() noexcept {return menu.load(std::memory_order_acquire);}
inline unsigned MenuModeForChoice(unsigned choice) noexcept {return choice==1?2u:choice==2?4u:choice==3?3u:0u;}
inline unsigned MenuChoiceForMode(unsigned mode) noexcept {return mode==2?1u:mode==4?2u:mode==3?3u:0u;}
inline unsigned MenuModeFromIni(int stored) noexcept {return stored==4?4u:unsigned(std::clamp(stored,0,3));}
inline bool UncappedSelectedAt(std::uint64_t) noexcept {return MenuMode()==4;}
inline void SetMenuMode(unsigned m) noexcept {
    m=m==4?4u:valid(m);if(m==1)m=0;
    if(m==3){SetElbowPreferenceR92(true);m=2;} // old elbow+timing choice keeps its original effects
    const auto previous=menu.exchange(m,std::memory_order_acq_rel);
    if(previous!=m){
        generation.fetch_add(1,std::memory_order_acq_rel);
        if(previous==4||m==4)handRefreshGenerationR94.fetch_add(1,std::memory_order_acq_rel);
    }
}
inline void SetMode(unsigned m) noexcept {
    m=valid(m);session.store(m?diagnostics::capture168::gate.session[unsigned(diagnostics::capture168::Domain::Vrik)].load(std::memory_order_acquire):0,std::memory_order_release);
    if(sessionMode.exchange(m,std::memory_order_acq_rel)!=m)generation.fetch_add(1,std::memory_order_acq_rel);
}
inline unsigned ModeAt(std::uint64_t now) noexcept {
    namespace c=diagnostics::capture168;
    const auto serial=session.load(std::memory_order_acquire);
    const auto m=sessionMode.load(std::memory_order_acquire);
    if(m&&serial&&serial==c::gate.session[unsigned(c::Domain::Vrik)].load(std::memory_order_acquire)&&c::gate.active(c::Domain::Vrik,now))return m;
    return MenuMode();
}
inline bool TimingSelectedAt(std::uint64_t now) noexcept {
    const auto selected=MenuMode();
    return selected!=4&&(selected>=2||ModeAt(now)==2);
}
inline bool ElbowSelectedAt(std::uint64_t now) noexcept {return ElbowSelectedForModeR92(ModeAt(now));}
inline bool ContinuityRepair() noexcept {return continuityRepair.load(std::memory_order_acquire);}
inline void SetContinuityRepair(bool on) noexcept {
    if(continuityRepair.exchange(on,std::memory_order_acq_rel)!=on)generation.fetch_add(1,std::memory_order_acq_rel);
}
inline std::uint64_t Policy(unsigned mode) noexcept {
    // Timing/diagnostic selection cannot reset the arm solver. Only actual
    // arm policy and, while active, its tunable pole weights participate.
    return (ElbowSelectedForModeR92(mode)?((elbowGeneration.load(std::memory_order_acquire)<<4)|8ull):0ull)|(ContinuityRepair()?1ull:0ull);
}
}
