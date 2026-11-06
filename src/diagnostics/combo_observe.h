#pragma once
#include "combo_snapshot.h"
namespace kcd2vr::diagnostics::combo {
// Copied Windows unwind frames, not guessed native stack offsets. 91496D is
// the verified return from C_CombatTriggerCombo's native close notification.
// Its immediate caller frame names the close site; missing/recursive frames
// remain unknown. This observation cannot authorize a combo or change a timer.
struct CloseStack {std::uint64_t callerRva=0;unsigned matches=0;};
inline CloseStack InspectCloseStack(const std::uintptr_t* frames,std::size_t count) noexcept {
    CloseStack out{};if(!frames)return out;
    for(std::size_t i=0;i<count;++i)if(frames[i]==0x91496D){
        ++out.matches;out.callerRva=i+1<count?frames[i+1]:0;
    }
    if(out.matches!=1)out.callerRva=0;
    return out;
}
inline const char* ClosePath(const CloseStack& s) noexcept {
    if(s.matches>1)return "AMBIGUOUS_CLOSE_STACK";
    if(!s.matches)return "NO_CLOSE_FRAME";
    switch(s.callerRva){
    case 0:return "CLOSE_CALLER_UNAVAILABLE";
    case 0x9087C5:return "UPDATE_TIMER_EXPIRED";
    case 0x2256E33:return "UPDATE_ELIGIBILITY_FALSE";
    case 0x9111B4:return "PHASE4_CLOSE";
    case 0x911248:return "PHASE1_OR64_CLOSE";
    default:return "OTHER_NATIVE_CLOSE";
    }
}
struct Episode {Snapshot before{},after{};bool captured=false,comparable=false,changed=false;};
// The actual hooks use this wrapper. No diagnostic gate may skip/repeat original.
// Capture sees the owning native callback before and after, never worker polling.
// A failed read still calls the native function exactly once.
template<class Original,class CaptureFn> Episode Around(bool record,Original original,CaptureFn capture) {
    Episode e{};
    if(record){e.before=capture();e.captured=true;}
    original();
    if(record){e.after=capture();e.comparable=Comparable(e.before,e.after);
        e.changed=e.comparable&&ObservedFieldsChanged(e.before,e.after);}
    return e;
}
}
