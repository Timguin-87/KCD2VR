#pragma once
// D85 xbow: one short line for every crossbow hand-reload refusal. Pure text
// and decision helpers only: no native reads, no logging, no allocation. The
// callers publish the line through visible_failure (KCD2VR.log always, on
// screen while F5/F7/F11 records).
#include <cstddef>
#include <cstdio>
namespace kcd2vr::crossbow_refusal_d85 {
inline constexpr std::size_t kLine=96; // visible_failure::kMaxText
// visible_failure::safe() refuses quotes, backslashes and non-ASCII. Keep the
// line instead of losing it: replace such characters.
inline void sanitize(char* text) noexcept {
    if(!text)return;
    for(char* p=text;*p;++p){const unsigned char c=static_cast<unsigned char>(*p);
        if(c<32||c>126||c=='"'||c=='\\')*p='_';}
}
enum class LineKind : unsigned {Stopped,NotTaken};
// "crossbow: hand reload stopped (INPUT_LOST: NATIVE_CLOCK_NOT_QUALIFIED) - game finishes it"
inline void line(char (&out)[kLine+1],LineKind kind,const char* why,const char* detail) noexcept {
    const char* w=why&&*why?why:"UNKNOWN";const bool d=detail&&*detail;
    if(kind==LineKind::NotTaken)
        std::snprintf(out,sizeof(out),"crossbow: hand reload not taken (%s%s%s) - game reloads",w,d?": ":"",d?detail:"");
    else
        std::snprintf(out,sizeof(out),"crossbow: hand reload stopped (%s%s%s) - game finishes it",w,d?": ":"",d?detail:"");
    sanitize(out);
}
// A tracked right-hand grip edge while the crossbow is held in the LEFT hand.
// kind 1 string (light, uncocked), 2 chest tool (14/15, uncocked), 3 bolt
// (cocked, right-shoulder quiver). The zone flags come from the same source
// sample. Refusals are reported only for grips inside a reload zone, so an
// ordinary right-hand grip (aiming, doors, items) never prints a line.
struct GripInput {
    bool atString=false,inChest=false,inQuiver=false; // reload zones of this sample
    bool handFree=false;  // native right-hand item key is empty
    bool itemsRead=false; // ManualItemsRead596 for the same weapon key
    bool loaded=false;    // native weapon already holds a bolt
    int cocked=-1;        // native component+D8: 0/1, -1 unreadable
    int classId=-1;       // 10 light, 14 goat's foot, 15 heavy
    bool callerPinned=false; // exact retail caller 2AAA4FB bytes
    bool heavyHandPull=false; // R111: HAND ONLY + VrHeavyCrossbowHandPullR111 - class 15 is a string class
    bool mediumHandPull=false; // R117: HAND ONLY + VrMediumCrossbowHandPullR117 - class 14 is a string class
};
enum class Grip : unsigned {None,HandBusy,Loaded,SpanFirst,ToolFirst,CodeDiffers};
struct GripDecision {unsigned kind=0;Grip refusal=Grip::None;};
inline GripDecision decideGrip(const GripInput& in) noexcept {
    GripDecision d{};
    // R111: the heavy crossbow with the hand pull has the light zones (string,
    // then quiver) and never the chest/spanner zone.
    // R117: the goat's foot with the hand pull too (no spanner from the chest).
    const bool light=in.classId==10||(in.classId==15&&in.heavyHandPull)||(in.classId==14&&in.mediumHandPull);
    // Zone that would start a reload for this weapon state (same rule as D72).
    const bool cocked=in.cocked==1,uncocked=in.cocked==0;
    const bool stringZone=light&&uncocked&&in.atString;
    const bool chestZone=!light&&uncocked&&in.inChest;
    const bool quiverZone=in.inQuiver;
    if(!stringZone&&!chestZone&&!quiverZone)return d; // ordinary grip: silent
    if(!in.handFree){d.refusal=Grip::HandBusy;return d;}
    if(!in.itemsRead||in.cocked<0)return d; // unknown native state: no claim either way
    if(in.loaded){if(quiverZone)d.refusal=Grip::Loaded;return d;}
    if(cocked){if(quiverZone)d.kind=3;}
    else if(stringZone)d.kind=1;
    else if(chestZone)d.kind=2;
    else if(quiverZone){d.refusal=light?Grip::SpanFirst:Grip::ToolFirst;return d;}
    if(d.kind&&!in.callerPinned){d.kind=0;d.refusal=Grip::CodeDiffers;}
    return d;
}
inline const char* gripText(Grip g) noexcept {
    switch(g){
    case Grip::HandBusy:return "crossbow: right hand holds something - empty it to reload";
    case Grip::Loaded:return "crossbow: already loaded";
    case Grip::SpanFirst:return "crossbow: span the string first - grip it at the rest and pull back";
    case Grip::ToolFirst:return "crossbow: take the spanning tool from the chest first";
    case Grip::CodeDiffers:return "crossbow: game code differs - hand reload off, game reload only";
    default:return nullptr;
    }
}
}
