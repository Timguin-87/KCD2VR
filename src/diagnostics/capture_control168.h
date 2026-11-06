#pragma once
#include "capture168.h"
#include "coverage_d42.h"
#include "../diagnostic_trace.h"
#include "../build_identity.h"
#include <cstdio>
namespace kcd2vr::diagnostics::capture168 {
inline void select(Domain d,bool on,std::uint64_t now,const char* source) noexcept {
    d=canonical(d);gate.select(d,on,now);char m[320]{};std::snprintf(m,sizeof(m),
        "V66.168 CAPTURE domain=%s session=%llu state=%s durationMs=%llu source=%s gameplayModes=UNCHANGED",
        name(d),(unsigned long long)gate.session[unsigned(d)].load(),on?"BEGIN":"END",(unsigned long long)(on?Duration:0),source);
    diagnostic_trace::Write(m);
    if(on&&gate.active(d,now)){
        diagnostic_trace::WriteFor(d,"KCD2VR BUILD " KCD2VR_BUILD_MARKER);
        coverage_d42::begin(d,gate.session[unsigned(d)].load(std::memory_order_acquire),diagnostic_trace::Write);
    }
}
inline unsigned cycleVrik(std::uint64_t now,const char* source) noexcept {
    const auto before=gate.vrikRung(now);
    if(before!=1&&before!=2){select(Domain::Vrik,before==0,now,source);return before==0&&gate.active(Domain::Vrik,now)?1u:0u;}
    return gate.cycleVrik(now);
}
inline void poll(std::uint64_t now) noexcept {
    const auto ended=gate.poll(now);for(unsigned i=0;i<DomainCount;++i)if(ended&(1u<<i)){
        char m[256]{};std::snprintf(m,sizeof(m),"V66.168 CAPTURE domain=%s session=%llu state=EXPIRED gameplayModes=UNCHANGED",
            name(Domain(i)),(unsigned long long)gate.session[i].load());diagnostic_trace::Write(m);
    }
}
}
