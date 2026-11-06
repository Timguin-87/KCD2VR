#pragma once
#include <cstdint>

// No Windows or engine dependencies: phase/budget logic is directly unit-testable.
namespace kcd2vr::single_run {
enum class Phase : int { Off=0, Baseline=1, NativeCount2=2, ReplayDefault=3, ReplaySelector=4, Recovery=5 };
constexpr const char* Name(Phase p) noexcept {
    switch(p) {
        case Phase::Baseline: return "BASELINE";
        case Phase::NativeCount2: return "NATIVE-COUNT2";
        case Phase::ReplayDefault: return "REPLAY-DEFAULT";
        case Phase::ReplaySelector: return "REPLAY-SELECTOR1";
        case Phase::Recovery: return "RETURN-TO-BASELINE";
        default: return "OFF";
    }
}
constexpr bool Count2(Phase p) noexcept {
    return p==Phase::NativeCount2 || p==Phase::ReplayDefault || p==Phase::ReplaySelector;
}
constexpr bool Replay(Phase p) noexcept { return p==Phase::ReplayDefault || p==Phase::ReplaySelector; }
constexpr int Diag(Phase p) noexcept { return p==Phase::Off ? 0 : (Replay(p) ? 4 : 1); }
constexpr Phase Next(Phase p) noexcept {
    switch(p) {
        case Phase::Baseline: return Phase::NativeCount2;
        case Phase::NativeCount2: return Phase::ReplayDefault;
        case Phase::ReplayDefault: return Phase::ReplaySelector;
        case Phase::ReplaySelector: return Phase::Recovery;
        default: return Phase::Off;
    }
}
struct Progress {
    static constexpr unsigned kWarmup=8;
    static constexpr unsigned kSamples=120;
    static constexpr unsigned kMaxCommands=600;
    static constexpr std::uint64_t kTimeoutMs=30000;
    Phase phase{Phase::Off};
    unsigned commands{}, workCommands{};
    std::uint64_t startedMs{};
    void Reset(Phase p,std::uint64_t now) noexcept { phase=p;commands=0;workCommands=0;startedMs=now; }
    void Record(bool hadNormalExecute) noexcept { ++commands;if(hadNormalExecute)++workCommands; }
    bool Complete() const noexcept { return workCommands>=kWarmup+kSamples; }
    bool TimedOut(std::uint64_t now) const noexcept {
        return commands>=kMaxCommands || (now>=startedMs && now-startedMs>=kTimeoutMs);
    }
};
}
