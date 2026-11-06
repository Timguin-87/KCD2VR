#pragma once
#include <cstdint>

namespace kcd2vr::prompt_refresh_d56 {
// One event request, at most four native script callbacks. There is no periodic
// Flash keepalive and no input injection. A newer family/UI event replaces it.
inline constexpr int family(int style) noexcept { return style==4?4:2; }
struct State {
    std::uint64_t ticket=0,vm=0,due=0;
    unsigned attempts=0;
    bool begin(std::uint64_t request,std::uint64_t generation,std::uint64_t now) noexcept {
        if(!request||!generation)return false;
        if(request!=ticket||generation!=vm){ticket=request;vm=generation;due=0;attempts=0;}
        if(attempts>=4||now<due)return false;
        ++attempts;due=now+100;return true;
    }
    bool terminal(bool dispatched) const noexcept {return dispatched||attempts>=4;}
};
}
