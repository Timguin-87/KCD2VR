#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::comfort_vignette51 {

inline constexpr int ClampLevel(int level) noexcept {
    return level < 0 ? 0 : level > 3 ? 3 : level;
}

struct Frame {
    int level = 0;
    bool gameplay = false;
    bool headValid = false;
    bool inputFresh = false;
    bool smoothTurn = false;
    bool leftActive = false, rightActive = false;
    float leftX = 0.0f, leftY = 0.0f, rightX = 0.0f;
    std::uint64_t nowNs = 0, epoch = 0;
};

// XR-thread envelope. The returned value belongs to ONE immutable presentation
// frame and is used by both eyes. It never changes camera/head/hand transforms.
class Envelope {
    float strength_ = 0.0f;
    std::uint64_t lastNs_ = 0, epoch_ = 0;

    static float Deadzone(float value) noexcept {
        return std::clamp((value - 0.15f) / 0.85f, 0.0f, 1.0f);
    }

public:
    void Reset() noexcept {
        strength_ = 0.0f;
        lastNs_ = epoch_ = 0;
    }

    float Step(const Frame& f) noexcept {
        const int level = ClampLevel(f.level);
        const bool finite = (!f.leftActive || (std::isfinite(f.leftX) && std::isfinite(f.leftY))) &&
            (!f.rightActive || std::isfinite(f.rightX));
        if (!level || !f.gameplay || !f.headValid || !f.inputFresh ||
            (!f.leftActive && !f.rightActive) || !finite || !f.nowNs || !f.epoch) {
            Reset();
            return 0.0f;
        }
        // A menu, lost input/focus, recenter or suspended XR frame cannot leave
        // a hidden vignette waiting to appear when gameplay resumes. The first
        // newly eligible sample establishes a clock; later samples ramp in.
        if (!lastNs_ || epoch_ != f.epoch || f.nowNs <= lastNs_ ||
            f.nowNs - lastNs_ > 120000000ull) {
            strength_ = 0.0f;
            lastNs_ = f.nowNs;
            epoch_ = f.epoch;
            return 0.0f;
        }
        const float dt = static_cast<float>(f.nowNs - lastNs_) * 1.0e-9f;
        lastNs_ = f.nowNs;
        const float x = f.leftActive ? std::clamp(f.leftX, -1.0f, 1.0f) : 0.0f;
        const float y = f.leftActive ? std::clamp(f.leftY, -1.0f, 1.0f) : 0.0f;
        const float turn = f.smoothTurn && f.rightActive ? std::min(std::fabs(f.rightX), 1.0f) : 0.0f;
        const float motion = std::max(Deadzone(std::sqrt(x*x + y*y)), Deadzone(turn));
        const float scale = level == 1 ? 0.45f : level == 2 ? 0.70f : 0.95f;
        const float target = scale * motion;
        const float tau = target > strength_ ? 0.12f : 0.35f;
        strength_ += (target - strength_) * (1.0f - std::exp(-dt / tau));
        strength_ = std::clamp(strength_, 0.0f, scale);
        return strength_;
    }
};

} // namespace kcd2vr::comfort_vignette51
