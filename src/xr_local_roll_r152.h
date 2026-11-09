#pragma once
// R152 (SteamVR "Meta compatibility mode" start): from session start until the first recenter the runtime's LOCAL
// space is turned 180 deg about its forward axis - every head pose comes back upside down (upY -1, quaternion
// ~(0,0,-1,0), his log 8 Nov: 15 s of it, then his recenter fixed it). R108 drops such poses as a glitch, so the
// start/menu screen had no pose = not to be seen until you recenter. a head can't stay upside down, so when the
// tracked head stays upside down 0.5 s the mod switches to a second LOCAL space turned 180 deg about forward
// (undoes the runtime's turn), recenters and re-places the screen. same rule back (if the runtime fixes itself
// the turned space reads upside down -> back to plain LOCAL). no latch: it can switch any time. a headset lying
// upside down on the desk flips at most every 5 s after two quick flips. pure logic here (tests/local_roll_r152_test.cpp).
#include <cstdint>
namespace kcd2vr::local_roll_r152 {
inline double upY(float x,float y,float z,float w) noexcept {(void)y;(void)w;return 1.0-2.0*(double(x)*x+double(z)*z);}
struct Watch { std::uint64_t since=0, uprightSince=0; unsigned quickSwaps=0; };
inline constexpr std::uint64_t kHoldMs=500, kSlowHoldMs=5000, kUprightResetMs=1000;
// true = switch the LOCAL space now. nowMs must be > 0 (GetTickCount64).
inline bool watch(Watch& w,bool tracked,double up,std::uint64_t nowMs) noexcept {
    if(!tracked||!(up==up)){w.since=0;w.uprightSince=0;return false;}
    if(up>=-0.5){
        w.since=0;
        if(up>0.5){if(!w.uprightSince)w.uprightSince=nowMs;if(nowMs-w.uprightSince>=kUprightResetMs)w.quickSwaps=0;}
        else w.uprightSince=0;
        return false;
    }
    w.uprightSince=0;
    if(!w.since){w.since=nowMs;return false;}
    const std::uint64_t need=w.quickSwaps>=2?kSlowHoldMs:kHoldMs;
    if(nowMs-w.since>=need){w.since=0;++w.quickSwaps;return true;}
    return false;
}
} // namespace kcd2vr::local_roll_r152
