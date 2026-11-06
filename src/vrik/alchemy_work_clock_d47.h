#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::alchemy_work_clock_d47 {
// Motion is a bounded request for native action time, not native work completed.
// Reconcile the previous write against the child's actual active-time field.
// Repeated controller callbacks before that field advances retain the request;
// they must not alternately write a nonzero rate and zero for one XR sample.
struct Ledger {
    double pending=0,advanced=0,discarded=0,lastActive=0;
    bool activeKnown=false;
    static constexpr double MaxPending=.12;
    static constexpr double MaxRate=2.5;

    void seed(double active) noexcept {lastActive=active;activeKnown=std::isfinite(active)&&active>=0;}
    void stop() noexcept {discarded+=pending;pending=0;}
    bool observe(double active,double savedRate) noexcept {
        if(!std::isfinite(active)||active<0||!std::isfinite(savedRate)||savedRate<=0)return false;
        if(activeKnown){
            const double delta=active-lastActive;
            if(delta<-.000001)return false; // the same token must not silently rewind
            const double used=std::max(0.,delta)/savedRate;
            advanced+=used;pending=std::max(0.,pending-used);
            if(pending<.000001)pending=0; // float active-time roundoff is not another stroke
        }
        lastActive=active;activeKnown=true;return true;
    }
    float rate(double credit,bool newSample,bool stopped,double effectiveDt,bool validDt) noexcept {
        if(stopped||!validDt||!std::isfinite(effectiveDt)||effectiveDt<=.000001){
            stop();if(newSample&&std::isfinite(credit)&&credit>0)discarded+=credit;return 0;
        }
        if(newSample){
            if(!std::isfinite(credit)||credit<0){stop();return 0;}
            const double requested=pending+credit;
            pending=std::min(MaxPending,requested);discarded+=requested-pending;
        }
        return float(std::min(MaxRate,pending/effectiveDt));
    }
};
}
