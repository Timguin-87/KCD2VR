#pragma once
#include <atomic>
#include <cstdint>

namespace kcd2vr {
class XrManager;

class VrikBridge {
public:
    struct Target {
        bool active=false;
        float px=0,py=0,pz=0;
        float qx=0,qy=0,qz=0,qw=1;
        float trigger=0,grip=0;
    };
    struct ArmChain {
        bool active=false;
        // Diagnostic positions are in OpenXR LOCAL space, metres.
        float sx=0,sy=0,sz=0;
        float ex=0,ey=0,ez=0;
        float hx=0,hy=0,hz=0;
        float reach=0;
    };

    static void SetEnabled(bool enabled);
    static bool Enabled();
    static void Update(XrManager& xr);
    static Target Left();
    static Target Right();

private:
    static std::atomic<bool> enabled_;
    static Target left_,right_;
    static ArmChain left_arm_,right_arm_;
    static std::uint64_t samples_;
};
}
