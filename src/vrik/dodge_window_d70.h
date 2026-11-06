#pragma once
// D70 opt-in GLOBAL native dodge-window CVar lease. This is not an accepted
// dodge/perfect result and does not change button duration or cooldown.
#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace kcd2vr::dodge_window_d70 {
inline std::atomic<float> requestedFactor{0.f};
inline float Bound(float v) noexcept {return !std::isfinite(v)||v<=0?0.f:std::clamp(v,1.f,3.f);}
inline float Factor() noexcept {return requestedFactor.load(std::memory_order_acquire);}
inline void SetFactor(float v) noexcept {requestedFactor.store(Bound(v),std::memory_order_release);}
inline bool Near(float a,float b) noexcept {return std::isfinite(a)&&std::isfinite(b)&&std::fabs(a-b)<.0001f;}
struct Lease {
    static constexpr const char* name="wh_cs_Dodge_WindowMod";
    float original=0,written=0,requested=0;
    std::uint64_t next=0;
    bool owned=false,settled=false,foreign=false,wanted=false;
    bool owns()const noexcept {return owned;}
    void rearmRestore()noexcept {next=0;settled=false;}
    template<class IO,class Log>void service(bool active,std::uint64_t now,IO& io,Log log) noexcept {
        const float factor=Factor();const bool want=active&&factor>0;
        if(factor!=requested){requested=factor;foreign=false;settled=false;next=0;}
        if(want!=wanted){wanted=want;settled=false;next=0;}
        // Idle/OFF performs no native reads. A foreign writer permanently owns
        // this setting until the user changes the factor, not merely closes UI.
        if((!want&&!owned)||foreign||(settled&&want))return;
        if(next&&now<next)return;next=now+250;
        if(owned){
            float current=0;if(!io.Read(name,current)||!std::isfinite(current))return;
            if(!Near(current,written)){
                if(Near(current,original)){owned=false;}
                else {owned=false;foreign=true;log(name,"OTHER_OWNER",current,current);return;}
            }else {
                const float desired=original*factor;
                if(want&&Near(written,desired)){settled=true;return;}
                // Retain cleanup debt until both setter and readback confirm;
                // false setter return is not proof that no side effect happened.
                if(!io.Write(name,original))return;
                float check=0;if(!io.Read(name,check)||!Near(check,original))return;
                log(name,"RESTORED",written,check);owned=false;
            }
        }
        if(!want)return;
        float current=0;if(!io.Read(name,current)||!std::isfinite(current)||current<=0||current>10)return;
        const float desired=current*factor;if(!std::isfinite(desired))return;
        if(Near(current,desired)){settled=true;return;}
        original=current;written=desired;owned=true;
        if(!io.Write(name,written))return;
        float check=0;if(!io.Read(name,check)||!Near(check,written))return;
        settled=true;log(name,"APPLIED_GLOBAL",original,check);
    }
};
}
