#pragma once
#include <cstdint>
namespace kcd2vr::ui_back_escape_m1 {
// An explicit recovery gesture, independent of menu classification and native
// B semantics. UI/router Reset cannot turn the same physical hold into a new
// request. Focus/tracking loss requires a current neutral before rearming.
class State {
    bool quarantined_=true,pressed_=false,fired_=false;
    std::uint64_t start_=0,last_=0,generation_=0;
public:
    void Cancel() noexcept {quarantined_=true;pressed_=false;start_=last_=0;}
    bool Consumed() const noexcept {return fired_;}
    bool Update(bool active,bool allowed,bool down,std::uint64_t now,std::uint64_t generation) noexcept {
        if(!active||!allowed||!now||!generation){Cancel();return false;}
        // Keep the recovery's consumption through its release frame, so the
        // ordinary keyboard router cannot add a short-tap action afterward.
        if(!down){quarantined_=false;pressed_=false;start_=0;last_=now;generation_=generation;return false;}
        if(generation_!=generation){Cancel();return false;}
        if(quarantined_)return false;
        if(!pressed_){pressed_=true;fired_=false;start_=last_=now;return false;}
        if(fired_)return false;
        // This is discontinuity cancellation, not extra tracking grace: the
        // caller independently qualifies the current original input sample.
        if(now<last_||now-last_>250){Cancel();return false;}
        last_=now;
        if(now-start_<2000)return false;
        fired_=true;return true;
    }
};
}
