#pragma once
#include "../graphics_owner31.h"
#include <cstdio>
namespace kcd2vr::diagnostics {
// Local POD comparisons only. Formatting occurs on option/fault edges or F2
// state changes; this observer never looks up a native CVar or changes a lease.
struct GraphicsReceipts32 {
    bool initialized=false,captured=false;unsigned options=0,shadow=0,eye=0,faults=0;int sun=-1;
    // C49: shadowLevel = VrShadowQuality 0 LOW / 1 MEDIUM / 2 HIGH (a bool still reads as 0/1).
    template<class Sink> void Observe(int shadowLevel,bool eyeOption,bool capture,
        const graphics_owner31::Shadows& sh,const graphics_owner31::Eye& ex,Sink&& sink) noexcept {
        shadowLevel=graphics_owner31::clampShadowLevel(shadowLevel);
        const unsigned opt=unsigned(shadowLevel)|(eyeOption?4u:0u);
        const int sunNow=sh.lease[3].owned?int(sh.lease[3].last+.5f):-1; // observed after our own write (POD copy, no CVar lookup)
        const unsigned sk=(sh.wanted?1u:0u)|(sh.applied?2u:0u)|(sh.pending?4u:0u)|
            (sh.rollback?8u:0u)|(sh.foreign?16u:0u)|(sh.Owned()?32u:0u)|(sh.gaveUp?64u:0u);
        const unsigned ek=unsigned(ex.requested)|(unsigned(ex.handled)<<2)|(ex.pending?16u:0u)|
            (ex.lease.owned?32u:0u)|(ex.lease.uncertain?64u:0u)|(ex.lease.blocked?128u:0u)|(ex.gaveUp?256u:0u);
        const unsigned failure=(sh.pending&&sh.retry?1u:0u)|(sh.foreign?2u:0u)|
            (ex.pending&&ex.retry?4u:0u)|(ex.lease.blocked?8u:0u)|(sh.gaveUp?16u:0u)|(ex.gaveUp?32u:0u);
        char line[288]{};
        if(!initialized||opt!=options){std::snprintf(line,sizeof(line),
            "C32 GRAPHICS_OPTION shadows=%d shadowQuality=%s sunResTarget=%d eyeSteady=%d nativeIO=EDGE_ONLY shadowGpuCost=UNMEASURED",
            shadowLevel,graphics_owner31::shadowLevelName(shadowLevel),shadowLevel?int(graphics_owner31::sunResForLevel(shadowLevel)):0,eyeOption?1:0);sink(line);}
        if(failure!=faults){std::snprintf(line,sizeof(line),
            "C32 GRAPHICS_%s issues=%u shadows=%u eye=%u retryMs=500 foreignWrites=PRESERVED",
            failure?"FAULT":"RECOVERED",failure,sk,ek);sink(line);}
        if(capture&&(!captured||!initialized||sk!=shadow||ek!=eye||sunNow!=sun)){std::snprintf(line,sizeof(line),
            "C32 GRAPHICS_STATE shadows=%u eye=%u requested=%s handled=%s owned=%d uncertain=%d shadowQuality=%s sunRes=%d sunResOriginal=%d sunResClamped=%d",
            sk,ek,exposure26::name(ex.requested),exposure26::name(ex.handled),ex.lease.owned?1:0,ex.lease.uncertain?1:0,
            graphics_owner31::shadowLevelName(sh.level),sunNow,sh.lease[3].owned?int(sh.lease[3].original+.5f):-1,sh.lease[3].clamped?1:0);sink(line);}
        initialized=true;captured=capture;options=opt;shadow=sk;eye=ek;faults=failure;sun=sunNow;
    }
};
}
