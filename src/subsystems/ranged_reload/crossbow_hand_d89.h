#pragma once
// R89 xbow: crossbow hand reload in the BOW pattern. The game's own reload
// action is never started for Henry's held crossbow while this is on; our
// state machine owns the hand steps and calls the same native leaves the
// game's reload event handler 289CCD0 calls, in the same order:
//   LoadReachedCocking -> 2895F30: 2895F74(player) [=1741C80(component,1)],
//                                  2ACD5D4(equipment,2) [=393524(eq+28,2); C5F484(eq)]
//   LoadReachedAmmo    -> 289CDA2: 289595C(create) [=28959D8(mgr,player,&id)],
//                                  393524(eq+28,0); C5F484(eq)
//   LoadReachedWeapon  -> 289C930: 289595C(remove) [=40B730(player,&id,side)],
//                                  289C988(player) [=2A8A0C0 bind -> 2A8A030 nock],
//                                  393524(eq+28,1); C5F484(eq)
// Pure policy + sequences only: no native reads, no logging, no allocation.
// The production glue (crossbow_hand_d89.inl) supplies the leaves; host tests
// supply controlled leaves (tests/crossbow_hand_d89_test.cpp).
#include <cmath>
#include <cstdint>
#include "crossbow_hand_owner_r91.h"
namespace kcd2vr::crossbow_hand_d89 {
struct Vec {double x=0,y=0,z=0;};
inline Vec operator-(Vec a,Vec b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline Vec operator+(Vec a,Vec b) noexcept {return {a.x+b.x,a.y+b.y,a.z+b.z};}
inline Vec operator*(Vec a,double s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline double dot(Vec a,Vec b) noexcept {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline double length(Vec a) noexcept {return std::sqrt(dot(a,a));}
inline bool finite(Vec a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
// Row-major 3x4 (CryEngine Matrix34): world = M * local.
inline Vec apply(const float (&m)[12],Vec p) noexcept {
    return {m[0]*p.x+m[1]*p.y+m[2]*p.z+m[3],m[4]*p.x+m[5]*p.y+m[6]*p.z+m[7],m[8]*p.x+m[9]*p.y+m[10]*p.z+m[11]};
}
// local = M^-1 * world (general 3x3 inverse; entity scale need not be 1).
inline bool unapply(const float (&m)[12],Vec w,Vec& out) noexcept {
    const double a=m[0],b=m[1],c=m[2],d=m[4],e=m[5],f=m[6],g=m[8],h=m[9],i=m[10];
    const double det=a*(e*i-f*h)-b*(d*i-f*g)+c*(d*h-e*g);
    if(!std::isfinite(det)||std::abs(det)<1e-9)return false;
    const Vec p{w.x-m[3],w.y-m[7],w.z-m[11]};
    out={((e*i-f*h)*p.x+(c*h-b*i)*p.y+(b*f-c*e)*p.z)/det,
         ((f*g-d*i)*p.x+(a*i-c*g)*p.y+(c*d-a*f)*p.z)/det,
         ((d*h-e*g)*p.x+(b*g-a*h)*p.y+(a*e-b*d)*p.z)/det};
    return finite(out);
}
inline constexpr double kGripOn=.65,kGripOff=.25;   // same edge rule as D72/H21
inline constexpr double kStringReach=.12;           // grab within 12 cm of the string rest
inline constexpr double kRailReach=.12;             // open the hand within 12 cm of the rail groove
inline constexpr double kLateralSlip=.15;           // hand left the rail line: string slips
inline constexpr double kPullShare=.90;             // 90 % of the native string travel cocks
inline bool lightClass(int c) noexcept {return c==10;}
inline bool toolClass(int c) noexcept {return c==14||c==15;}
inline bool crossbowClass(int c) noexcept {return lightClass(c)||toolClass(c);}
// R111: VrHeavyCrossbowHandPullR111 (default ON, Experiments "Heavy crossbow: pull
// string by hand"). The heavy crossbow (15, cranequin) is spanned like the light
// one: grip the string at its rest, pull it back to the nut ("Crossbow_arrow"),
// native cocked. No cranequin, no game reload action, no XR34. OFF = old route
// (chest Grip -> the game's cranequin action, XR34 crank push).
// R117: VrMediumCrossbowHandPullR117 (default ON, Experiments "Medium crossbow: pull
// string by hand"): the goat's-foot crossbow (14) the same way - string rest
// "Crossbow_medium_string" (y .2695) back to the nut, 90 % of 13.15 cm = 11.8 cm.
// No spanner, no game reload action, no XR34. OFF = the spanner route.
inline bool stringPullClass(int c,bool heavyHandPull,bool mediumHandPull=false) noexcept {return lightClass(c)||(c==15&&heavyHandPull)||(c==14&&mediumHandPull);}
inline bool toolCockingClass(int c,bool heavyHandPull,bool mediumHandPull=false) noexcept {return toolClass(c)&&!stringPullClass(c,heavyHandPull,mediumHandPull);}
// One frame of everything the policy needs, all in the crossbow's own local
// frame (native weapon entity, metres) so moving the crossbow is not a pull.
struct Input {
    bool context=false;       // Henry holds an owned crossbow in the LEFT hand, tracking fresh
    bool leftHand=true;       // crossbow is in the left hand (right hand works)
    int classId=-1;           // 10 light, 14 goat's foot, 15 cranequin
    int cocked=-1;            // native component+D8: 0/1, -1 unreadable
    bool loaded=false;        // native component+E0 holds the ammo key
    bool bolts=false;         // equipment+48 holds an ammo item
    bool handFree=false;      // working hand has no native item (or our bolt only)
    bool geometry=false;      // string rest + rail read from the native skeleton
    Vec hand{},rest{},rail{}; // weapon-local
    double grip=0;            // working hand grip 0..1
    bool inQuiver=false,inChest=false;
    bool boltInHand=false;    // our native-created bolt is bound to the right hand attachment
    double crankTravel=.08;   // VrCrankTravelD66C (class 15, old crank route only)
    bool heavyHandPull=false; // R111 VrHeavyCrossbowHandPullR111: class 15 = string pull like class 10
    bool mediumHandPull=false; // R117 VrMediumCrossbowHandPullR117: class 14 = string pull like class 10
};
enum class Command : unsigned {None,Cock,TakeBolt,DropBolt,Nock};
enum class Note : unsigned {None,LeftHandOnly,NoBolts,HandBusy,NoGeometry,CockFirst,ToolFirst,ToolTaken,
    PullFurther,StringSlipped,Cocked,BoltTaken,BoltDropped,Loaded,AlreadyLoaded,BoltLost,PullByHand,PullByHandNoTool};
inline const char* text(Note n) noexcept {
    switch(n){
    case Note::LeftHandOnly:return "crossbow: hand reload needs the crossbow in the left hand";
    case Note::NoBolts:return "crossbow: no bolts equipped";
    case Note::HandBusy:return "crossbow: right hand holds something - empty it to reload";
    case Note::NoGeometry:return "crossbow: string position not readable - hand reload cannot start";
    case Note::CockFirst:return "crossbow: pull the string back first";
    case Note::ToolFirst:return "crossbow: take the spanning tool from your chest first";
    case Note::ToolTaken:return "crossbow: tool in hand - put it on the string and pull";
    case Note::PullFurther:return "crossbow: pull the string all the way back";
    case Note::StringSlipped:return "crossbow: string slipped - pull straight along the rail";
    case Note::Cocked:return "crossbow: cocked - take a bolt from the right shoulder";
    case Note::BoltTaken:return "crossbow: bolt in hand - open the hand on the rail";
    case Note::BoltDropped:return "crossbow: bolt dropped - open the hand on the rail";
    case Note::Loaded:return "crossbow: loaded";
    case Note::AlreadyLoaded:return "crossbow: already loaded";
    case Note::BoltLost:return "crossbow: game removed the bolt from the hand - take a new one";
    case Note::PullByHand:return "crossbow: heavy crossbow - grip the string at the front and pull it back (no crank)";
    case Note::PullByHandNoTool:return "crossbow: no spanner needed - grip the string at the front and pull it back"; // R117
    default:return nullptr;
    }
}
enum class Stage : unsigned {Idle,Tool,Span,Bolt};
struct Step {Command command=Command::None;Note note=Note::None;double progress=0;Stage stage=Stage::Idle;};
// Distance from p to the segment a..b.
inline double segmentDistance(Vec p,Vec a,Vec b) noexcept {
    const Vec ab=b-a;const double l2=dot(ab,ab);double t=l2>1e-12?dot(p-a,ab)/l2:0;
    t=t<0?0:t>1?1:t;return length(p-(a+ab*t));
}
class Machine {
public:
    Stage stage() const noexcept {return stage_;}
    void reset() noexcept {stage_=Stage::Idle;neutral_=false;start_={};}
    // The glue reports the result of the native command it ran for this step.
    void commandResult(Command c,bool ok) noexcept {
        if(c==Command::TakeBolt)stage_=ok?Stage::Bolt:Stage::Idle;
        else if(c==Command::Nock||c==Command::DropBolt||c==Command::Cock)stage_=Stage::Idle;
        if(!ok)neutral_=false; // a refused native step needs a fresh grip
    }
    Step update(const Input& in) noexcept {
        Step s{};
        const bool held=in.grip>=kGripOff,down=in.grip>kGripOn;
        if(!in.context){
            // Never leave a native bolt behind in a hand we no longer own.
            if(stage_==Stage::Bolt){s.command=Command::DropBolt;}
            reset();s.stage=stage_;return s;
        }
        if(!in.leftHand){if(down&&neutral_&&(in.inQuiver||in.inChest))s.note=Note::LeftHandOnly;
            if(in.grip<kGripOff)neutral_=true;else if(down)neutral_=false;
            if(stage_==Stage::Bolt)s.command=Command::DropBolt;
            stage_=Stage::Idle;s.stage=stage_;return s;}
        const bool edge=down&&neutral_;
        if(in.grip<kGripOff)neutral_=true;else if(edge)neutral_=false;
        const Vec groove=in.rail-in.rest;const double travel=length(groove);
        const bool shape=in.geometry&&finite(in.hand)&&finite(in.rest)&&finite(in.rail)&&travel>.03&&travel<.5;
        switch(stage_){
        case Stage::Bolt:
            if(!in.boltInHand){stage_=Stage::Idle;s.note=Note::BoltLost;break;}
            if(!held){
                if(shape&&in.cocked==1&&!in.loaded&&segmentDistance(in.hand,in.rest,in.rail)<=kRailReach)s.command=Command::Nock;
                else {s.command=Command::DropBolt;s.note=Note::BoltDropped;}
            }
            break;
        case Stage::Tool:
            if(!held){stage_=Stage::Idle;break;} // tool put away
            if(in.cocked!=0){stage_=Stage::Idle;break;}
            if(shape&&length(in.hand-in.rest)<=kStringReach){stage_=Stage::Span;start_=in.hand;}
            break;
        case Stage::Span:{
            if(in.cocked!=0||!shape){stage_=Stage::Idle;break;}
            // Class 10/14 pull the string back along rest -> rail (native default
            // joints). Class 15 cranks forward (D66C contract, VrCrankTravelD66C),
            // R111: unless heavyHandPull - then it is pulled back like class 10
            // (heavy string travel 17.43 cm, 90 % = 15.7 cm cocks).
            Vec axis=groove*(1./travel);double need=travel*kPullShare;
            if(in.classId==15&&!in.heavyHandPull){axis=axis*-1.;need=in.crankTravel;}
            const Vec moved=in.hand-start_;const double along=dot(moved,axis);
            const double lateral=length(moved-axis*along);
            s.progress=need>0?(along<0?0:along/need):0;
            if(!held){stage_=Stage::Idle;s.note=Note::PullFurther;break;}
            if(lateral>kLateralSlip){stage_=Stage::Idle;s.note=Note::StringSlipped;break;}
            if(along>=need-1e-6){s.command=Command::Cock;s.progress=1;}
            break;}
        case Stage::Idle:default:
            if(!edge)break;
            if(in.cocked<0)break; // unknown native state: no claim either way
            if(in.loaded){if(in.inQuiver)s.note=Note::AlreadyLoaded;break;}
            if(in.cocked==1){
                if(!in.inQuiver)break;
                if(!in.bolts){s.note=Note::NoBolts;break;}
                if(!in.handFree){s.note=Note::HandBusy;break;}
                s.command=Command::TakeBolt;break;
            }
            // uncocked
            if(stringPullClass(in.classId,in.heavyHandPull,in.mediumHandPull)){
                if(!in.handFree){s.note=Note::HandBusy;break;}
                if(!in.geometry){if(in.inQuiver)s.note=Note::NoGeometry;break;}
                if(shape&&length(in.hand-in.rest)<=kStringReach){stage_=Stage::Span;start_=in.hand;break;}
                if(in.inQuiver)s.note=Note::CockFirst;
                else if(in.inChest&&in.classId==15)s.note=Note::PullByHand; // R111: old habit, chest = crank
                else if(in.inChest&&in.classId==14)s.note=Note::PullByHandNoTool; // R117: old habit, chest = spanner
            }else if(toolClass(in.classId)){
                if(in.inChest){
                    if(!in.handFree){s.note=Note::HandBusy;break;}
                    if(!in.geometry){s.note=Note::NoGeometry;break;}
                    stage_=Stage::Tool;s.note=Note::ToolTaken;break;}
                if(in.inQuiver)s.note=Note::ToolFirst;
            }
            break;
        }
        s.stage=stage_;return s;
    }
private:
    Stage stage_=Stage::Idle;bool neutral_=false;Vec start_{};
};
// Identity changes reset a half-finished pull even before a bolt exists.
inline bool resetForOwnerR91(Machine& machine,crossbow_hand_r91::Owner& prior,
                             const crossbow_hand_r91::Owner& next) noexcept {
    const bool changed=!crossbow_hand_r91::same(prior,next);
    if(changed)machine.reset();
    prior=next;return changed;
}
// ---------------------------------------------------------------------------
// Native sequences. N supplies the leaves; each returns false on a fault.
// N::cock()            2895F74(nullptr,player)      -> 1741C80(component,1)
// N::cockNotify()      2ACD5D4(equipment,2)         -> 393524(eq+28,2); C5F484(eq)
// N::create(side)      28959D8(189F3C0(),player,&id), side = native return
// N::remove()          40B730(nullptr,player,&id,side), id must become 0
// N::bind()            289C988(nullptr,player)      -> 2A8A0C0(component,ammo) -> 2A8A030
// N::notify(v)         393524(equipment+28,v)
// N::refresh()         C5F484(equipment)
// N::cocked()/loaded() readback of component+D8 / component+E0
// N::handId()          our bolt entity id (0 = none)
// ---------------------------------------------------------------------------
template<class N> bool cockSequence(N& n) noexcept {
    if(n.cocked())return true;                    // native already cocked: nothing to repeat
    if(!n.cock()||!n.cocked())return false;       // 2895F30 order: cock, then notify+tags
    return n.cockNotify();
}
template<class N> bool takeBoltSequence(N& n) noexcept {
    if(n.handId())return false;                   // 289595C create skips when +88 already holds one
    int side=-1;if(!n.create(side))return false;
    if(!n.handId())return false;
    if(side!=0){(void)n.remove();return false;}   // crossbow bolt must be the RIGHT hand (log: HAND_END side=0)
    return n.notify(0)&&n.refresh();              // 289CDA2 order after create
}
template<class N> bool dropBoltSequence(N& n) noexcept {
    if(!n.handId())return true;
    return n.remove()&&!n.handId();
}
template<class N> bool nockSequence(N& n) noexcept {
    if(!n.cocked()||n.loaded()||!n.handId())return false;
    if(!n.remove()||n.handId())return false;      // 289C930: remove the hand bolt FIRST
    if(!n.bind()||!n.loaded())return false;       // then 289C988 binds the ammo to the weapon
    return n.notify(1)&&n.refresh();              // then 393524(eq+28,1); C5F484(eq)
}
// R96: native give-back. Pure policy; the glue (crossbow_hand_d89.inl) owns
// the native reads/calls. A game request swallowed while our hands are idle is
// parked; idle hands for kNativeWindowMs give the SAME request back to the
// game, which then reloads natively (no "can't even natively reload").
inline constexpr std::uint64_t kNativeWindowMs=1500;     // idle hands -> the game reloads
// R111: an UNCOCKED heavy crossbow with the hand pull gets 3 s: the give-back
// plays the game's cranequin animation, the thing the hand pull replaces, and
// the string rest of the heavy one is 8 cm further out than the light one's.
inline constexpr std::uint64_t kNativeWindowHeavyPullMs=3000;
// R117: the same 3 s for an UNCOCKED goat's-foot crossbow with the hand pull
// (the give-back plays the game's spanner animation, the thing it replaces).
inline std::uint64_t giveBackWindowMs(int c,int cocked,bool heavyHandPull,bool mediumHandPull) noexcept {
    return cocked==0&&((c==15&&heavyHandPull)||(c==14&&mediumHandPull))?kNativeWindowHeavyPullMs:kNativeWindowMs;
}
inline constexpr std::uint64_t kNativeStartGraceMs=1500; // replayed request -> native action appears
enum class Park : unsigned {Keep,Drop,GiveBack};
inline Park parkDecision(std::uint64_t parkedAt,std::uint64_t now,bool runnable,bool handsAtWork,
                         bool nativeInFlight,bool xr34Owned,std::uint64_t windowMs=kNativeWindowMs) noexcept {
    if(!parkedAt)return Park::Keep;
    if(xr34Owned||nativeInFlight)return Park::Drop;  // never replay into a running action
    if(!runnable)return Park::GiveBack;              // route off/lost/not ours: the game gets it now
    if(handsAtWork)return Park::Drop;                // our hands complete this reload
    return now>=parkedAt&&now-parkedAt>=windowMs?Park::GiveBack:Park::Keep;
}
// While a game reload of this crossbow runs (given back, or started natively),
// every request passes. It ends when the game's action is over: after the
// start grace without an action, or when loaded. The game cocks and loads in
// two actions, so the first action ending unloaded gets one more grace for
// its own follow-up load request. Never longer: no latch.
struct NativePass {
    bool active=false,seen=false,chained=false;std::uint64_t since=0;
    void begin(std::uint64_t now,bool live) noexcept {active=true;seen=live;chained=false;since=now?now:1;}
    void end() noexcept {*this={};}
    bool update(bool live,bool loaded,std::uint64_t now) noexcept {
        if(!active)return false;
        if(live)seen=true;
        const bool grace=since&&now>=since&&now-since<=kNativeStartGraceMs;
        if(!loaded&&(grace||live))return true;
        if(!loaded&&seen&&!chained){chained=true;seen=false;since=now?now:1;return true;}
        end();return false;
    }
};
// Deferral of the game's reload request (18468BC entry) for Henry's crossbow.
struct DeferInput {
    bool enabled=false,fault=false,ownReplay=false;
    unsigned char a=0,b=0;      // 18468BC dl / r8b
    bool henryEquipment=false;  // the request's equipment is Henry's current equipment
    int weaponClass=-1;         // class of equipment+40
};
inline bool deferRequest(const DeferInput& in) noexcept {
    // r8b!=0 is the game's immediate cock+bind path (no reload action): untouched.
    return in.enabled&&!in.fault&&!in.ownReplay&&in.b==0&&in.henryEquipment&&crossbowClass(in.weaponClass);
}
}
