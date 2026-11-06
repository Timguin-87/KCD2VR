#pragma once
#include <atomic>
#include <cmath>
#include <cstdint>
// C39 DIRBLOCK: directional NPC block for VR DIRECT (pure logic, host tested; used by physical_weapon.h (pose cadence)
// and geometry_hit178r27.inl vrstate596 (game thread)).
// WHGame 6A350E20 static proof (see C39 dirblock report):
//  - Henry's announced row +150 is the payload row+138 of the native row query 12EDEF4(actor, F0 zone) -> 90FD50/90FF10
//    (per row: 15FF9CC(state+910 vf8(row+158), row+140/148/B0, combat_zone table 185321770) must equal row+138, or the row
//    carries -1). VR DIRECT already writes F0 then calls this query (Publish -> ResolveDefense -> SetDefense 90D6D0).
//  - the NPC's block direction +5D0 is written by the NPC block helper start C53B30 (C_CombatActionHelperBlock slot6):
//    [[helper+20]vf8()]vf8()+8 -> 93A2D0(npc+5D0), then phase 0x80. It is cleared (-1) at C55022 (helper end) and 939D9A.
//    No proven native path re-reads Henry's +150 into a started block (939CD8's block-mode cold path 225928E is the only
//    other writer: Henry +150 if Henry +2D0 else 934074(Henry F0)). C37C2: 37/37 block starts had 5D0 == Henry 150 of that
//    instant; serial 76 re-announced 150 2->5 after the block started and was not blocked (5D0 stayed 2).
// So: (1) the FIRST announcement must name the real approach sector, (2) a later change is only useful before the NPC
// block helper starts (5D0 == -1), at most kMaxUpdates times, and (3) once the block started the row is kept.
// Sector -> zone id uses the angle ranges of Tables combat_zone.xml (the side the attack comes FROM, cursor angle,
// 0 deg = Henry's right, counter-clockwise): 0 head 45..135, 1 upper_left 135..225, 5 lower 225..315, 2 upper_right
// 315..45 (zones 3/4 have no angle and are never produced). Zone id -> native row stays the native query above.
namespace kcd2vr::dirblock39 {
inline std::atomic<bool> enabled{true}; // INI VrDirectNpcDirectionalBlock (default ON)
inline std::atomic<std::uint64_t> mailbox{0}; // pose thread stores pack(), game thread loads (one word, no lock)
struct V { double x=0,y=0,z=0; }; // Henry body frame: x right, y forward, z up (physical_weapon body_vector117)
constexpr double kMinPlanarSpeed=1.0;   // m/s of tip motion across the body (a cut, not a push)
constexpr double kMinPlanarShare=.35;   // of the full tip speed; a forward push is a thrust, not a sector
constexpr double kLookahead=.05;        // s: tip velocity turned by the blade's own angular velocity (arc curvature)
constexpr double kMaxTurn=.7853981633974483; // rad, at most 45 deg of extrapolated turn
constexpr double kBoundaryDeg=8.;       // closer than this to a table boundary = not confident
constexpr unsigned kMaxUpdates=2,kStableSamples=2,kMinGapMs=40,kFreshMs=60,kContactFreshMs=150;
inline bool finite(V v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
inline double length(V v) noexcept {return std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z);}
// Rodrigues: rotate v about the axis of w by |w|*t (clamped). A zero/invalid w leaves v unchanged.
inline V turn(V v,V w,double t) noexcept {
    const double n=length(w);if(!finite(v)||!finite(w)||!std::isfinite(t)||t<=0||n<1e-9)return v;
    double a=n*t;if(a>kMaxTurn)a=kMaxTurn;
    const V k{w.x/n,w.y/n,w.z/n};const double c=std::cos(a),s=std::sin(a),d=k.x*v.x+k.y*v.y+k.z*v.z;
    const V kv{k.y*v.z-k.z*v.y,k.z*v.x-k.x*v.z,k.x*v.y-k.y*v.x};
    return {v.x*c+kv.x*s+k.x*d*(1-c),v.y*c+kv.y*s+k.y*d*(1-c),v.z*c+kv.z*s+k.z*d*(1-c)};
}
// combat_zone.xml ranges; angle in [0,360). Boundaries belong to the counter-clockwise zone.
inline int zoneFromAngle(double deg) noexcept {
    if(!std::isfinite(deg))return -1;
    deg=std::fmod(deg,360.);if(deg<0)deg+=360.;
    if(deg>=45.&&deg<135.)return 0;
    if(deg>=135.&&deg<225.)return 1;
    if(deg>=225.&&deg<315.)return 5;
    return 2;
}
inline double boundaryDistance(double deg) noexcept {
    deg=std::fmod(deg,360.);if(deg<0)deg+=360.;double best=360.;
    static constexpr double kB[4]{45.,135.,225.,315.};for(const double b:kB){double d=std::fabs(deg-b);if(d>180.)d=360.-d;if(d<best)best=d;}
    return best;
}
struct Prediction { bool valid=false,nearBoundary=false;int zone=-1,angle=-1;double planarSpeed=0; };
// Cut only. vTip/w in the body frame. The sector is where the blade comes FROM (-velocity). An upward cut (table 5,
// a thrust/low row) keeps its lateral side like stroke_zone117 does; a nearly vertical upward cut is not confident.
inline Prediction predictCut(V vTip,V w,double lookahead=kLookahead) noexcept {
    Prediction p{};if(!finite(vTip))return p;
    const V v=turn(vTip,w,lookahead);
    const double planar=std::hypot(v.x,v.z),full=length(v);
    p.planarSpeed=planar;
    if(!(planar>=kMinPlanarSpeed)||!(planar>=kMinPlanarShare*full))return p;
    double deg=std::atan2(-v.z,-v.x)*57.29577951308232;if(deg<0)deg+=360.;
    const int a=int(deg+.5)%360;
    int zone=zoneFromAngle(deg);bool nearB=boundaryDistance(deg)<kBoundaryDeg;
    if(zone==5){const double lateral=-v.x;zone=lateral<0?1:2;nearB=std::fabs(lateral)<.34*planar;} // within ~20 deg of straight up
    p.valid=true;p.zone=zone;p.angle=a;p.nearBoundary=nearB;return p;
}
// One-word pose->game mailbox: gesture low 32 | zone+1 (3) | near (1) | angle (9) | tick low 19 bits. 0 = empty.
constexpr std::uint64_t kTickMask=(1ull<<19)-1;
inline std::uint64_t pack(std::uint64_t gesture,int zone,int angle,bool nearB,std::uint64_t tick) noexcept {
    if(!gesture||(zone!=0&&zone!=1&&zone!=2&&zone!=5)||angle<0||angle>359)return 0;
    return (gesture&0xFFFFFFFFull)|(std::uint64_t(zone+1)<<32)|(std::uint64_t(nearB?1:0)<<35)|(std::uint64_t(angle)<<36)|((tick&kTickMask)<<45);
}
struct Sample { bool valid=false,nearBoundary=false;int zone=-1,angle=-1;std::uint64_t ageMs=0,tickLow=0; };
inline Sample unpack(std::uint64_t word,std::uint64_t gesture,std::uint64_t now,unsigned maxAgeMs) noexcept {
    Sample s{};if(!word||!gesture||(word&0xFFFFFFFFull)!=(gesture&0xFFFFFFFFull))return s;
    const int zone=int((word>>32)&7)-1;const int angle=int((word>>36)&511);const std::uint64_t t=(word>>45)&kTickMask;
    const std::uint64_t age=((now&kTickMask)-t)&kTickMask;
    if((zone!=0&&zone!=1&&zone!=2&&zone!=5)||angle>359||age>maxAgeMs)return s;
    s.valid=true;s.zone=zone;s.angle=angle;s.nearBoundary=((word>>35)&1)!=0;s.ageMs=age;s.tickLow=t;return s;
}
enum class Choice : int { Predicted=0, SameAsOrigin, NearBoundary, NoPrediction, NotCut, NotWaiting, Disabled };
inline const char* name(Choice c) noexcept {
    switch(c){case Choice::Predicted:return "PREDICTED_SECTOR";case Choice::SameAsOrigin:return "PREDICTED_EQUALS_ORIGIN";
        case Choice::NearBoundary:return "ORIGIN_KEPT_SECTOR_ON_BOUNDARY";case Choice::NoPrediction:return "ORIGIN_KEPT_NO_FRESH_PREDICTION";
        case Choice::NotCut:return "ORIGIN_KEPT_NOT_A_CUT";case Choice::NotWaiting:return "NPC_NOT_WAITING";case Choice::Disabled:return "DISABLED";}
    return "?";
}
// The initial zone: the predicted sector only for a cut against a waiting opponent with a fresh, confident sample.
inline int chooseInitial(bool on,bool waiting,int technique,int origin,const Sample& s,Choice& why) noexcept {
    if(!on){why=Choice::Disabled;return origin;}
    if(!waiting){why=Choice::NotWaiting;return origin;}
    if(technique!=0){why=Choice::NotCut;return origin;}
    if(!s.valid){why=Choice::NoPrediction;return origin;}
    if(s.nearBoundary){why=Choice::NearBoundary;return origin;}
    why=s.zone==origin?Choice::SameAsOrigin:Choice::Predicted;return s.zone;
}
enum class Step : int { None=0, Wait, Update, Frozen, Budget };
inline const char* name(Step s) noexcept {
    switch(s){case Step::None:return "NONE";case Step::Wait:return "WAIT";case Step::Update:return "UPDATE";case Step::Frozen:return "NPC_BLOCK_STARTED_ROW_KEPT";case Step::Budget:return "UPDATE_BUDGET_SPENT";}
    return "?";
}
// One plan per approach (motion attempt key is held by the caller). Updates only before the NPC block helper started.
struct Plan {
    bool active=false,frozen=false,budgetReported=false,frozenReported=false,cutRefined62=false;
    int zone=-1,origin=-1,technique=0,candidate=-1;unsigned updates=0,stable=0;
    std::uint64_t requestTick=0,lastUpdate=0,lastSample=~0ull;
    void begin(int chosen,int originZone,int tech,std::uint64_t now) noexcept {*this=Plan{};active=true;zone=chosen;origin=originZone;technique=tech;requestTick=lastUpdate=now;}
    Step consider(const Sample& s,bool npcBlockStarted,std::uint64_t now) noexcept {
        if(!active)return Step::None;
        if(frozen)return Step::Frozen;
        if(npcBlockStarted){frozen=true;return Step::Frozen;}
        if(technique!=0||!s.valid||s.tickLow==lastSample)return Step::None;
        lastSample=s.tickLow;
        if(s.nearBoundary||s.zone==zone){candidate=-1;stable=0;return Step::None;}
        if(s.zone!=candidate){candidate=s.zone;stable=1;}else ++stable;
        if(stable<kStableSamples)return Step::Wait;
        if(updates>=kMaxUpdates)return Step::Budget;
        if(now<lastUpdate||now-lastUpdate<kMinGapMs)return Step::Wait;
        return Step::Update;
    }
    void committed(int newZone,std::uint64_t now) noexcept {zone=newZone;++updates;lastUpdate=now;candidate=-1;stable=0;}
    bool canRefineCut62(int proposed,std::uint64_t now) const noexcept {
        return active&&!frozen&&!cutRefined62&&technique==0&&updates<kMaxUpdates&&
            (proposed==0||proposed==1||proposed==2)&&proposed!=zone&&requestTick&&now>=requestTick&&now-requestTick<=100;
    }
    // Called only AFTER the existing same-stage native row/setter readback.
    // Automatic prediction cannot subsequently overwrite this deliberate turn.
    void refinedCut62(int proposed,std::uint64_t now) noexcept {committed(proposed,now);cutRefined62=true;frozen=frozenReported=true;}
    // Zone for a later publish of the SAME attempt (commit, feint, contact): keep our row; a technique change after the
    // NPC block started keeps the row the NPC took (native attacks never change +150 inside one attack).
    int keep(int technique2,int proposed,bool npcBlockStarted,bool& kept) noexcept {
        kept=false;if(!active)return proposed;
        if(technique2==technique)return zone;
        if(npcBlockStarted){kept=true;frozen=true;return zone;}
        technique=technique2;zone=proposed;frozen=true;return proposed;
    }
};
}
