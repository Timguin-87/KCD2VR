#pragma once
#include <cmath>
#include <cstdint>
namespace kcd2vr::focus_zoom58c2 {
// The native named CVar is a maximum decrease in degrees, not cl_fov or a
// projection matrix. Native focus interpolation then has identical endpoints.
struct Lease {
    bool requested=false,owned=false;float baseline=0;
    unsigned attempts=0;std::uint64_t due=0;
    struct Result {bool attempted=false,ok=false;};
    template<class IO> Result step(bool want,std::uint64_t now,IO& io) noexcept {
        if(want!=requested){requested=want;attempts=3;due=now;}
        if(!attempts||now<due)return {};
        --attempts;due=now+500;
        if(!want&&!owned){attempts=0;return {true,true};}
        float current=0;
        if(!io.get(current)||!std::isfinite(current)||current<0||current>90)return {true,false};
        if(want){
            if(!owned){baseline=current;owned=true;}
            // A prior failed write is retried only against our captured value.
            if(std::abs(current)>.001f&&std::abs(current-baseline)>.001f){owned=false;attempts=0;return {true,false};}
            if(std::abs(current)>.001f&&!io.set(0))return {true,false};
            float read=0;if(!io.get(read)||!std::isfinite(read)||std::abs(read)>.001f)return {true,false};
        }else{
            // Respect a later console/mod writer; never restore over its value.
            if(std::abs(current)>.001f&&std::abs(current-baseline)>.001f){owned=false;attempts=0;return {true,true};}
            if(std::abs(current-baseline)>.001f&&!io.set(baseline))return {true,false};
            float read=0;if(!io.get(read)||!std::isfinite(read)||std::abs(read-baseline)>.001f)return {true,false};
            owned=false;
        }
        attempts=0;return {true,true};
    }
};
}
