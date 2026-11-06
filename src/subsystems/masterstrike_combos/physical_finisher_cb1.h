#pragma once
// Optional physical FINISHER candidate filter. Native lead-in recipes remain
// directional. Current weapon-point height is only an approximation, not a
// certified body-part contact or NPC paired-animation alignment; default ON.
// Candidate eligibility, learned combos, costs and final selection stay native.
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "finisher_rows_cb1.h"
#include "finisher_diagnostics_d42.h"
namespace kcd2vr::finisher_cb1 {
namespace bc=kcd2vr::bodycombo59c7g;
inline std::atomic<bool> enabled{true}; // VrPhysicalComboFinisher (0 = native finisher rule only)
// D61 session-only comparison: remove ONLY our predicted-region/motion candidate
// veto; native learned recipe, target, sequence, costs and outcome remain intact.
inline std::atomic<bool> nativeValidationTestD61{false};
inline bool PhysicalFilterEnabledD61() noexcept {
    return enabled.load(std::memory_order_acquire)&&!nativeValidationTestD61.load(std::memory_order_acquire);
}
// what the CURRENT VR release physically is. published by the physical command consumer right before
// the native attack release (physical_native.inl), read by the 9397CC gate on the game thread.
enum class Kind : unsigned { None=0,Cut=1,Thrust=2,Pommel=3 };
// The native selector runs inside this exact input dispatch. A failed/replaced
// request leaves no timed global gesture capable of affecting a later action.
inline thread_local Kind activeRelease=Kind::None;
// D60: diagnostic lineage only. Never grants eligibility or survives the native
// input stack; nested native-only scopes clear the outer gesture as well as kind.
inline thread_local std::uint64_t activeGestureD60=0;
struct ReleaseScope {Kind previous;std::uint64_t previousGesture;
 explicit ReleaseScope(Kind k,std::uint64_t gesture=0) noexcept:previous(activeRelease),previousGesture(activeGestureD60){
     activeRelease=k;activeGestureD60=k==Kind::None?0:gesture;}
 ~ReleaseScope(){activeRelease=previous;activeGestureD60=previousGesture;}ReleaseScope(const ReleaseScope&)=delete;};

inline std::atomic<std::uint64_t> release{0}; // tick<<8 | kind
inline void publishRelease(Kind k,std::uint64_t tick) noexcept {release.store((tick<<8)|unsigned(k),std::memory_order_release);}
inline Kind currentRelease(std::uint64_t now,std::uint64_t maxAgeMs=250) noexcept {
    const auto v=release.load(std::memory_order_acquire);const auto tick=v>>8;
    if(!v||!tick||now<tick||now-tick>maxAgeMs)return Kind::None;
    const auto k=unsigned(v&0xFF);return k<=3?Kind(k):Kind::None;
}
inline const char* name(Kind k) noexcept {return k==Kind::Cut?"CUT":k==Kind::Thrust?"THRUST":k==Kind::Pommel?"POMMEL":"NONE";}
inline int hex(char c) noexcept {return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:c>='A'&&c<='F'?c-'A'+10:-1;}
// "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx" -> 16 bytes as the native stores a GUID (first 3 fields LE)
inline bool parseGuid(const char* s,std::uint8_t out[16]) noexcept {
    if(!s)return false;
    std::uint8_t b[16]{};int n=0;
    for(const char* p=s;*p;++p){
        if(*p=='-')continue;
        const int h=hex(*p),l=p[1]?hex(p[1]):-1;
        if(h<0||l<0||n>=16)return false;
        b[n++]=std::uint8_t(h*16+l);++p;
    }
    if(n!=16)return false;
    const int order[16]{3,2,1,0,5,4,7,6,8,9,10,11,12,13,14,15};
    for(int i=0;i<16;++i)out[i]=b[order[i]];
    return true;
}
inline const Row* findRow(const std::uint8_t tech[16]) noexcept {
    for(const auto& r:rows){std::uint8_t g[16]{};if(parseGuid(r.technique,g)&&!std::memcmp(g,tech,16))return &r;}
    return nullptr;
}
// the weapon part and the motion of the release. VR has no shield bash command yet. a trip is the low
// hooking cut of the weapon head, so it is a cut that has to land on the leg.
inline bool motionOk(const Row& s,Kind k) noexcept {
    using F=bc::Feature;using M=bc::Motion;
    const bool fAny=s.feature==F::Any,mAny=s.motion==M::Any;
    switch(k){
    case Kind::Cut:   return (fAny||s.feature==F::Blade||s.feature==F::WeaponHead)&&
                             (mAny||s.motion==M::Cut||s.motion==M::CutAfterDeflection||s.motion==M::Strike||s.motion==M::Trip||
                              // D62: a mace/axe head swung into the face is a
                              // blunt strike of its HEAD, not a pommel/handle
                              // thrust. C09's authored physical row otherwise
                              // had no possible physical Kind that could pass.
                              (s.feature==F::WeaponHead&&s.motion==M::BluntStrike));
    case Kind::Thrust:return (fAny||s.feature==F::Tip||s.feature==F::WeaponHead)&&(mAny||s.motion==M::Thrust);
    case Kind::Pommel:return (fAny||s.feature==F::Pommel||s.feature==F::Handle)&&(mAny||s.motion==M::BluntStrike);
    default:return false;}
}
inline bool supported(const Row& s) noexcept {
    using F=bc::Feature;using M=bc::Motion;
    return s.feature!=F::Shield&&s.feature!=F::ShieldEdge&&s.motion!=M::Bash;
}
// height of the blade point above the target's feet (m, human ~1.8 m), with overlap for the ~16-60 ms
// between release and contact. Unknown regions (arm) are not demanded.
inline bool regionOk(bc::Region want,double h) noexcept {
    using R=bc::Region;
    if(want==R::Any||want==R::Arm)return true;
    if(!std::isfinite(h))return false;
    switch(want){
    case R::Head:case R::Face:case R::Neck:return h>=1.38;
    case R::Chest:case R::Ribs:case R::Torso:return h>=1.05&&h<1.55;
    case R::Abdomen:return h>=0.80&&h<1.22;
    case R::Leg:return h<1.00;
    case R::Knee:return h>=0.22&&h<0.78;
    default:return false;}
}
enum class Verdict : unsigned { NativeOnly=0,Ok=1,WrongMotion=2,WrongRegion=3,Unsupported=4,NoRelease=5 };
inline const char* name(Verdict v) noexcept {
    switch(v){case Verdict::Ok:return "OK";case Verdict::WrongMotion:return "WRONG_MOTION";case Verdict::WrongRegion:return "WRONG_REGION";
    case Verdict::Unsupported:return "NO_VR_MOTION";case Verdict::NoRelease:return "NO_VR_RELEASE";default:return "NATIVE_ONLY";}
}
// regionKnown=false -> a region demand cannot pass.
inline Verdict judge(const Row* r,Kind k,bool regionKnown,double h) noexcept {
    if(!r)return Verdict::NativeOnly;
    const auto& s=*r;
    if(s.feature==bc::Feature::Fist)return Verdict::NativeOnly; // fist combos: native rule (separate fist owner)
    if(s.region==bc::Region::Any&&s.feature==bc::Feature::Any&&s.motion==bc::Motion::Any)return Verdict::NativeOnly;
    if(!supported(s))return Verdict::Unsupported;
    if(k==Kind::None)return Verdict::NoRelease;
    if(!motionOk(s,k))return Verdict::WrongMotion;
    if(s.region!=bc::Region::Any&&s.region!=bc::Region::Arm&&(!regionKnown||!regionOk(s.region,h)))return Verdict::WrongRegion;
    return Verdict::Ok;
}
inline bool needsRegion(const Row* r) noexcept {return r&&r->region!=bc::Region::Any&&r->region!=bc::Region::Arm;}
// CB4 (Claude, 25 Oct; Miki 25 Oct log: C06 knee finisher refused 3x at 2.00/1.39/1.40 m = the wind-up height).
// the finisher is chosen at the release, before any contact exists. the region is judged on the stroke's own line:
// from the measured striking point along its last measured frame motion until it reaches the opponent's body
// (vertical capsule around his root, radius r) - the actual contact geometry of THIS stroke, not a fixed time ahead.
// the line is bounded by maxTravel; a stroke that does not come towards the body keeps the release height.
// p = striking point now, q = same point one physics frame earlier, o = opponent root (x,y), feet = his root z.
// D41: a preliminary region can only come from an actual intersection of
// this measured approach with the approximate body cylinder. A closest point
// on a line that misses the cylinder is not predicted contact. This remains
// a candidate filter: native hit/contact acceptance is a separate boundary.
inline bool ProjectContactCB4(const double p[3],const double q[3],double ox,double oy,double feet,
    double& height,double radius=.30,double maxTravel=1.0) noexcept {
    height=0;
    if(!p||!q||!std::isfinite(ox)||!std::isfinite(oy)||!std::isfinite(feet)||
       !std::isfinite(radius)||!(radius>0)||!std::isfinite(maxTravel)||!(maxTravel>0))return false;
    for(int i=0;i<3;++i)if(!std::isfinite(p[i])||!std::isfinite(q[i]))return false;
    const double h0=p[2]-feet,rx=p[0]-ox,ry=p[1]-oy;
    if(rx*rx+ry*ry<=radius*radius){height=h0;return std::isfinite(height);}
    const double d[3]{p[0]-q[0],p[1]-q[1],p[2]-q[2]};
    const double len=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
    if(!std::isfinite(len)||len<1e-4)return false;
    const double a=d[0]*d[0]+d[1]*d[1],b=2*(rx*d[0]+ry*d[1]),c=rx*rx+ry*ry-radius*radius;
    if(a<1e-10||b>=0)return false;
    const double disc=b*b-4*a*c;
    if(!std::isfinite(disc)||disc<0)return false;
    const double t=(-b-std::sqrt(disc))/(2*a);
    if(!std::isfinite(t)||t<0||t>maxTravel/len)return false;
    height=p[2]+d[2]*t-feet;return std::isfinite(height);
}
// Retained numeric helper for old callers/diagnostics. The production adapter
// uses ProjectContactCB4's boolean and never grants a region from this fallback.
inline double ContactHeightCB4(const double p[3],const double q[3],double ox,double oy,double feet,
    double radius=.30,double maxTravel=1.0) noexcept {
    double height=0;return ProjectContactCB4(p,q,ox,oy,feet,height,radius,maxTravel)?height:(p?p[2]-feet:0.);
}
// D41: crossing a height band in empty air during the approach cannot qualify
// a different body location. Judge the predicted first-contact height only.
inline bool regionOkPath(bc::Region want,double h0,double h1) noexcept {
    (void)h0;return std::isfinite(h1)&&regionOk(want,h1);
}
inline Verdict judgePath(const Row* r,Kind k,bool regionKnown,double h0,double h1) noexcept {
    (void)h0;return judge(r,k,regionKnown,h1);
}
inline const char* want(const Row* r,char* buf,std::size_t n) noexcept {
    if(!r){if(n)buf[0]=0;return buf;}
    const auto& s=*r;
    static constexpr const char* f[]{"?","ANY","BLADE","TIP","POMMEL","SHIELD","FIST","SHIELD_EDGE","HANDLE","WEAPON_HEAD"};
    static constexpr const char* m[]{"ANY","CUT","THRUST","STRIKE","BLUNT_STRIKE","CUT","BASH","TRIP"};
    if(s.region==bc::Region::Any){std::snprintf(buf,n,"%s %s",unsigned(s.feature)<10?f[unsigned(s.feature)]:"?",unsigned(s.motion)<8?m[unsigned(s.motion)]:"?");return buf;}
    std::snprintf(buf,n,"%s %s to %s",unsigned(s.feature)<10?f[unsigned(s.feature)]:"?",unsigned(s.motion)<8?m[unsigned(s.motion)]:"?",bc::name(s.region));
    return buf;
}
}

#include "final_combo_contact_d72.h" // D72 copied actual final contact and exact native input permit
