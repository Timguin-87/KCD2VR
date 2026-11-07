#pragma once
// R137 (Miki 7 Nov, R135 log): "what the view ray? it is still not centered or following my view (the old good
// interaction ray, that it moves with my head, where I look or point with the index finger)".
// R136 log /tmp/r136/in (03:15-03:36): vrikMode=4 the whole session, so KCD2's object selection ran the VRIK split
// (interaction_alignment.cpp OnScreenDetour): an OBJECT was offered only by a palm ray (both hands empty + open =
// a palm ray every frame, rayActive=1 in 107 of 109 INTERACTOR_CANDIDATE rows), every object outside that palm cone
// was refused (F5 receipts: 359 PALM_OUTSIDE_OR_STALE, 86 OFFERED, all at 0.08-0.60 m from the hand) and with no palm
// it fell to the game's own screen test (19 HAND_RAY_STALE). the headset ray (R121 eye ray, already built every
// look ray) was used only for people/dogs. so the object prompt followed the hands, not where you look.
// now (switch VrInteractionEyeRayR121 ON) every object candidate goes to ONE source:
//   1. a palm transaction that is already locked (door held, grabbed item)       -> the palm path (unchanged)
//   2. an index-finger point (grip closed, index lifted off the trigger, hand out in front of the head, 120 ms)
//      -> the finger ray from the finger tip along the hand. an object touching that palm (<=16 cm) stays palm.
//   3. an object within 60 cm of an open palm that the palm already selects    -> the palm path (hand hover/grab)
//   4. everything else -> the headset ray: rendered eye + rendered forward (R121), base radius/cone from the INI
// OFF = R136 behaviour (palm only, else the game's screen test). people/dogs stay on the headset ray as before.
// pure logic, no native reads, host tested (tests/view_select_r137_test.cpp). no latch: decided per candidate.
#include <cmath>
#include <cstdint>

namespace kcd2vr::view_select_r137 {
constexpr float kTouchM=.16f;     // palm59c9::choose near-touch radius: an object in the hand stays the hand's
constexpr float kPalmReachM=.60f; // the hover feedback cap (mod.cpp cand.distance<=.60): farther = your view decides
constexpr float kTipM=.11f;       // palm point (mid metacarpals) -> index finger tip
constexpr float kPointRadiusM=.06f,kPointHalfDeg=5.f,kPointReachM=8.f;
constexpr std::uint64_t kPointHoldMs=120;

enum class Source {Palm,Finger,Head};
// palmSide/palmDistance = palm59c9::choose result for this candidate (-1 = no palm takes it).
inline Source decide(bool lockedPalm,int pointSide,int palmSide,float palmDistance) noexcept {
    if(lockedPalm)return Source::Palm;
    const bool palm=palmSide>=0&&std::isfinite(palmDistance);
    if(pointSide>=0){
        if(palm&&palmSide==pointSide&&palmDistance<=kTouchM)return Source::Palm;
        return Source::Finger;
    }
    if(palm&&palmDistance<=kPalmReachM)return Source::Palm;
    return Source::Head;
}
// native ranking = squared distance of (x,y) from (50,50). the palm path gives 50+10*score (<=56 at 60 cm), so a
// head pick ranks after any palm pick in the same pass, a finger pick after a palm touch (<=51.6).
inline void rank(Source s,float px,float py,float& x,float& y) noexcept {
    const float d=std::sqrt((px-50.f)*(px-50.f)+(py-50.f)*(py-50.f));
    const float r=std::isfinite(d)?d:30.f;
    x=(s==Source::Finger?52.f:57.f)+.5f*r;y=50.f;
}

// ---- index-finger point pose (mod thread) ----
struct Hand {
    bool valid=false,empty=false;float grip=0,trigger=0;bool touchValid=false,touch=true;
    float hand[3]{},dir[3]{};      // palm point + hand direction, world metres (frik ReadInteractionPalm59c9)
};
struct Head {bool valid=false;float eye[3]{},fwd[3]{};};
inline bool pose(const Hand& h,const Head& head) noexcept {
    if(!h.valid||!h.empty||!head.valid||!std::isfinite(h.grip)||!std::isfinite(h.trigger))return false;
    // index extended = lifted OFF the trigger (capacitive); a controller without trigger touch never points
    if(h.grip<.60f||h.trigger>=.10f||!h.touchValid||h.touch)return false;
    double r[3],r2=0,a2=0,f2=0,ra=0,af=0,rf=0;
    for(int i=0;i<3;++i){
        if(!std::isfinite(h.hand[i])||!std::isfinite(h.dir[i])||!std::isfinite(head.eye[i])||!std::isfinite(head.fwd[i]))return false;
        r[i]=double(h.hand[i])-head.eye[i];r2+=r[i]*r[i];a2+=double(h.dir[i])*h.dir[i];ra+=r[i]*h.dir[i];
    }
    for(int i=0;i<2;++i){f2+=double(head.fwd[i])*head.fwd[i];af+=double(h.dir[i])*head.fwd[i];rf+=r[i]*head.fwd[i];} // Cry z up: horizontal x,y
    if(r2<.01||a2<.01||f2<.01)return false;
    const double fl=std::sqrt(f2);
    return rf/fl>=.20&&ra/std::sqrt(r2*a2)>.35&&af/std::sqrt(a2*f2)>.25; // in front, pointing away, not back at you
}
struct State {std::uint64_t since[2]{};int owner=-1;};
// returns the pointing side (-1 none). the pose must hold kPointHoldMs; losing it ends the point at once.
inline int update(State& s,const bool posed[2],std::uint64_t now) noexcept {
    for(int i=0;i<2;++i){
        if(!posed[i]){s.since[i]=0;if(s.owner==i)s.owner=-1;continue;}
        if(!s.since[i]||now<s.since[i])s.since[i]=now;
    }
    if(s.owner>=0)return s.owner;
    int best=-1;
    for(int i=0;i<2;++i)if(s.since[i]&&now-s.since[i]>=kPointHoldMs&&(best<0||s.since[i]<s.since[best]))best=i;
    s.owner=best;return best;
}
// finger ray: from the finger tip along the hand. false = bad numbers.
inline bool fingerRay(const Hand& h,float o[3],float d[3]) noexcept {
    double l=0;for(int i=0;i<3;++i)l+=double(h.dir[i])*h.dir[i];
    if(!std::isfinite(l)||l<1e-6)return false;
    l=std::sqrt(l);
    for(int i=0;i<3;++i){d[i]=float(h.dir[i]/l);o[i]=h.hand[i]+d[i]*kTipM;if(!std::isfinite(o[i]))return false;}
    return true;
}
}
