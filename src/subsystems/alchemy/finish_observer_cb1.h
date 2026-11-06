#pragma once
// Read-only recipe verdict, not inventory-delivery evidence or an alchemy exit.
#include <cstdint>
namespace kcd2vr::alchemy_finish_cb1 {
struct Context {std::uintptr_t instance=0,player=0,actor=0,soul=0;std::uint64_t lifetime=0;};
using CurrentReader=bool(*)(Context&) noexcept;
struct Result {
    std::uint64_t serial=0,tick=0; Context context{};
    std::uint8_t valid=0,success=0,dataRead=0;
    int recipeId=0,performedSteps=0,requiredSteps=0,qualityTier=0,requestedQty=0,outCount=0;
    unsigned char productClass[16]{}; // CLASS GUID; never a created item identifier.
};
void SetCurrentReader(CurrentReader) noexcept; // startup; current Henry/Alchemy/soul join, no native writes
bool Enabled() noexcept;
void SetEnabled(bool) noexcept; // capture only; installed original remains live exactly once
// Existing input/control thread: 0 stops F5 admission; a new finite F5 session
// attempts the qualified existing installation once. No capture-only popup.
void SetCaptureSessionD42(std::uint64_t) noexcept;
bool Install() noexcept;
void Uninstall() noexcept; // shutdown requires existing external hook quiescence
bool Installed() noexcept;
bool Last(Result&) noexcept;
std::uint64_t Count() noexcept;
}
