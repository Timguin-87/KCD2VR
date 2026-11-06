#pragma once
#include <cstdint>
#include <algorithm>
namespace kcd2vr::contact {
// Budget is per query pass, NOT per eye or per ray. Elapsed time is supplied
// by the native adapter; tests use a deterministic clock. Cannot preempt a
// native call already in flight, only stop further calls in the pass.
struct Budget {
    unsigned remaining=0,used=0;std::uint64_t started=0,limit=1000;
    unsigned capacity=0;bool acquisitionBurstUsedD45=false;
    std::uint64_t spent=0;bool workOnly=false,inFlight=false;
    void begin_work(unsigned rays=18,std::uint64_t us=1000) noexcept {
        capacity=remaining=rays;used=0;started=spent=0;limit=us;workOnly=true;inFlight=false;acquisitionBurstUsedD45=false;
    }
    void finish(std::uint64_t now) noexcept {
        if(!workOnly||!inFlight)return;
        if(now<started)spent=limit;
        else spent+=std::min(limit-spent,now-started);
        inFlight=false;
    }
    void begin(std::uint64_t now,unsigned rays=18,std::uint64_t us=1000) noexcept {capacity=remaining=rays;used=0;started=now;limit=us;spent=0;workOnly=false;inFlight=false;acquisitionBurstUsedD45=false;}
    bool take(std::uint64_t now) noexcept {
        if(workOnly){
            if(!remaining||inFlight||spent>=limit)return false;
            started=now;inFlight=true;
        }else if(!remaining||now<started||now-started>=limit)return false;
        --remaining;++used;return true;
    }
};
// One fresh-binding query may use up to72 total rays in this pass, retaining
// the same work clock/1000us ceiling. Unused extra credits vanish on exit;
// ordinary contacts and reserved Door credits never inherit the allowance.
// Used rays remain truthful accounting, including those above the normal18.
struct AcquisitionBurstD45 {
    Budget& budget;unsigned beforeRemaining=0,beforeCapacity=0,beforeUsed=0;bool granted=false;
    AcquisitionBurstD45(Budget& b,bool qualified) noexcept : budget(b) {
        if(!qualified||!b.workOnly||b.inFlight||b.acquisitionBurstUsedD45||
           !b.capacity||b.capacity>72||b.spent>=b.limit)return;
        b.acquisitionBurstUsedD45=true;
        beforeRemaining=b.remaining;beforeCapacity=b.capacity;beforeUsed=b.used;granted=true;
        b.remaining+=72-b.capacity;b.capacity=72;
    }
    ~AcquisitionBurstD45(){
        if(!granted)return;
        const auto consumed=budget.used-beforeUsed;
        const auto additional=consumed>beforeRemaining?consumed-beforeRemaining:0;
        budget.remaining=consumed<beforeRemaining?beforeRemaining-consumed:0;
        budget.capacity=beforeCapacity+additional;
    }
};
}
