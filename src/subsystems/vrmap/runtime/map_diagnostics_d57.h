#pragma once
#include <cstdint>

namespace kcd2vr::map_diagnostics_d57 {
// Owned native-script client only. Never renews map, actor, tracking or capture.
struct Budget {
    std::uint64_t bug=0,interaction=0,vm=0,due=0;
    unsigned used=0;
    bool unavailable=false;
    bool admit(bool owner,std::uint64_t now,std::uint64_t b,std::uint64_t i,std::uint64_t v) noexcept {
        if(!owner||!now||now>UINT64_MAX-500||!v||(!b&&!i))return false;
        if(b!=bug||i!=interaction||v!=vm){bug=b;interaction=i;vm=v;due=0;used=0;unavailable=false;}
        if(unavailable||used>=120||now<due)return false;
        due=now+500;++used;return true;
    }
};
}
