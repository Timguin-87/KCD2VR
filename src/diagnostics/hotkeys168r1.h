#pragma once
// Key routing only; no renderer/gameplay setters or native calls.
#include <array>
namespace kcd2vr::diagnostics::keys168r1 {
enum class F10Route { HandsRecorder, NativeCapture, HandsMode };
inline F10Route f10Route(bool shift,bool control) noexcept {
    return shift?F10Route::HandsRecorder:control?F10Route::NativeCapture:F10Route::HandsMode;
}
struct Presses { bool hud=false, combat=false, interaction=false; };
struct Edges {
    std::array<bool,3> previous{};
    Presses poll(std::array<bool,3> down, bool blocked) noexcept {
        Presses out{!blocked&&down[0]&&!previous[0],
                    !blocked&&down[1]&&!previous[1],
                    !blocked&&down[2]&&!previous[2]};
        previous=down; // consume blocked edges; never replay after a modal test
        return out;
    }
};
}
