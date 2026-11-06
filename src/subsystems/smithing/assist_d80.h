#pragma once
#include <cmath>
namespace kcd2vr::subsystems::smithing::assist_d80 {
// Current physical LT, never a latch or native button echo. The caller proves
// fresh tracked input and the exact left tongs/workpiece binding separately.
inline bool held(bool enabled,int mode,int phase,bool current,float trigger) noexcept {
    // D83R1: observed passive phase15 retains the same attached workpiece.
    return enabled&&mode==3&&current&&(phase==11||phase==14||phase==15||phase==16||phase==17)&&
        std::isfinite(trigger)&&trigger>.55f&&trigger<=1.f;
}
}
