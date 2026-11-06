#pragma once
// C44 GORE (Miki 17 Oct): "decapitations ONLY on the kill move, and somehow we need to see the head flying off ... stretch
// his neck, let the head be ragdolly". Pure part of the LOOSE NECK style: the retail pe_params_joint layout (read off
// CArticulatedEntity::SetParams 6F2FE8 type-5 branch at 6F4957 and GetParams 3D50CC type-5 branch at 21E2E1C, WHGame
// 6A350E20), the "unused" markers the engine tests for, the loose-neck parameter build and the head impulse direction.
// The native calls (GetParams/SetParams/Action on the death ragdoll) live in gore32.inl. Nothing here touches the game.
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "gore32.h"

namespace kcd2vr::decap44 {
// CryPhysics "unused" markers as this build checks them: a float is unused when (bits & 0xFFA00000) == 0xFFA00000
// (SetParams 6F39E2/6F4D45: and eax,0xFFA00000; cmp eax,0xFFA00000), an int when == 0x80000000 (6F4D00, op[0] at 6F4A0A),
// a pointer when == -1 (pMtx0 at 6F589B: cmp rax,-1).
constexpr std::uint32_t kUnusedFloatBits=0xFFBFFFFFu;
constexpr int kUnusedInt=int(0x80000000u);
// This is a native bit marker (a signalling-NaN payload), not a numeric float.
// Never send it through a float return/conversion that may quiet/canonicalize it.
inline bool isUnused(const float& f) noexcept {std::uint32_t b=0;std::memcpy(&b,&f,4);return (b&0xFFA00000u)==0xFFA00000u;}
inline bool isUnused(int i) noexcept {return i==kUnusedInt;}

// pe_params_joint as the retail articulated entity reads it (offsets proven in the disassembly, see gore32.inl Pins44):
//  +0x04 flags (joint flags; bits 0..2 = angle locked per axis), +0x08 flagsPivot, +0x0C pivot, +0x18 q0, +0x28 pMtx0,
//  +0x30 limits[0], +0x3C limits[1], +0x48 bounciness, +0x54 ks, +0x60 kd, +0x6C qdashpot, +0x78 kdashpot, +0x84 q,
//  +0x90 qext, +0x9C qtarget, +0xA8 op[0] (parent part id, out of GetParams), +0xAC op[1] (THE part id = the joint),
//  +0xB0 nSelfCollidingParts, +0xB8 pSelfCollidingParts, +0xC0 bNoUpdate, +0xC4 animationTimeScale, +0xC8 ranimationTimeScale.
struct JointParams {
    int type=5;
    std::uint32_t flags=0,flagsPivot=0;
    float pivot[3]{};float q0[4]{};
    void* pMtx0=nullptr;
    float limits[2][3]{};float bounciness[3]{};float ks[3]{},kd[3]{};float qdashpot[3]{},kdashpot[3]{};
    float q[3]{},qext[3]{},qtarget[3]{};
    int op[2]{};
    int nSelfCollidingParts=0;int pad=0;int* pSelfCollidingParts=nullptr;
    int bNoUpdate=0;float animationTimeScale=0,ranimationTimeScale=0;
};
static_assert(sizeof(JointParams)==0xD0,"retail pe_params_joint: last field ends at 0xCC, 8-byte aligned (pointers) = 0xD0");
static_assert(offsetof(JointParams,flags)==0x4&&offsetof(JointParams,flagsPivot)==0x8&&offsetof(JointParams,pivot)==0xC&&
    offsetof(JointParams,q0)==0x18&&offsetof(JointParams,pMtx0)==0x28&&offsetof(JointParams,limits)==0x30&&
    offsetof(JointParams,bounciness)==0x48&&offsetof(JointParams,ks)==0x54&&offsetof(JointParams,kd)==0x60&&
    offsetof(JointParams,qdashpot)==0x6C&&offsetof(JointParams,kdashpot)==0x78&&offsetof(JointParams,q)==0x84&&
    offsetof(JointParams,qext)==0x90&&offsetof(JointParams,qtarget)==0x9C&&offsetof(JointParams,op)==0xA8&&
    offsetof(JointParams,nSelfCollidingParts)==0xB0&&offsetof(JointParams,pSelfCollidingParts)==0xB8&&
    offsetof(JointParams,bNoUpdate)==0xC0&&offsetof(JointParams,animationTimeScale)==0xC4&&
    offsetof(JointParams,ranimationTimeScale)==0xC8,"retail pe_params_joint offsets");
// Everything unused except type and op[1] = the part whose joint we mean. GetParams fills it, SetParams skips it.
inline JointParams query(int part) noexcept {
    JointParams p{};unsigned char* b=reinterpret_cast<unsigned char*>(&p);
    for(std::size_t off=0xC;off<0x28;off+=4)std::memcpy(b+off,&kUnusedFloatBits,4);      // pivot, q0
    for(std::size_t off=0x30;off<0xA8;off+=4)std::memcpy(b+off,&kUnusedFloatBits,4);     // limits .. qtarget
    p.flags=std::uint32_t(kUnusedInt);p.flagsPivot=0;p.pMtx0=reinterpret_cast<void*>(~std::uintptr_t(0));
    p.op[0]=kUnusedInt;p.op[1]=part;p.nSelfCollidingParts=kUnusedInt;p.pSelfCollidingParts=nullptr;
    p.bNoUpdate=1;
    std::memcpy(b+0xC4,&kUnusedFloatBits,4);std::memcpy(b+0xC8,&kUnusedFloatBits,4);
    return p;
}
constexpr std::uint32_t kAllAnglesLocked=7u; // angle0_locked(i) = 1<<i
struct LooseTuning {double limitRad=3.0;double kd=0.05;};
// The write: unlock all three angles of the head joint, open the limits wide, no spring back to the pose (ks 0), a
// touch of damping, no bounce. Everything else stays unused, so the engine keeps the joint's own pivot/frame/pMtx0.
// bNoUpdate 0 = the engine refreshes the joint after the write (6F6214 skips the refresh when it is non-zero).
inline JointParams loose(const JointParams& before,int part,const LooseTuning& t={}) noexcept {
    JointParams p=query(part);
    const double lim=std::isfinite(t.limitRad)?std::fmin(std::fmax(t.limitRad,.5),3.1):3.0;
    const float kd=float(std::isfinite(t.kd)?std::fmin(std::fmax(t.kd,0.),5.):.05);
    p.flags=isUnused(int(before.flags))?std::uint32_t(kUnusedInt):(before.flags&~kAllAnglesLocked);
    for(int i=0;i<3;++i){p.limits[0][i]=float(-lim);p.limits[1][i]=float(lim);p.bounciness[i]=0;p.ks[i]=0;p.kd[i]=kd;}
    p.bNoUpdate=0;
    return p;
}
// Head impulse: the blade went in at the hit point and travelled through the neck, so the head leaves to the OTHER side
// of the neck axis, a bit upward. Degenerate geometry (hit on the axis) falls back to "away from Henry" or straight up.
inline gore32::V3 headImpulse(gore32::V3 hitWorld,gore32::V3 neckWorld,gore32::V3 up,gore32::V3 fromHenry,double magnitude) noexcept {
    using gore32::V3;if(!std::isfinite(magnitude)||magnitude<=0||!gore32::finite(hitWorld)||!gore32::finite(neckWorld)||!gore32::finite(up))return {};
    const double ul=gore32::len(up);const V3 u=ul>1e-6?up*(1/ul):V3{0,0,1};
    V3 d=neckWorld-hitWorld;d=d-u*gore32::dot(d,u);
    if(gore32::len(d)<.03){d=gore32::finite(fromHenry)?fromHenry-u*gore32::dot(fromHenry,u):V3{};}
    const double dl=gore32::len(d);d=dl>1e-6?d*(1/dl):V3{};
    V3 v=d+u*.6;const double vl=gore32::len(v);if(vl<1e-6)return u*magnitude;
    return v*(magnitude/vl);
}
// Bounded wait for the death ragdoll: the kill receipt comes first, the game ragdolls the body a little later (death
// animation / manager transition). Poll the entity's root body every pollMs until the joint answers or the window ends.
struct Wait {
    std::uint64_t start=0,next=0,deadline=0;unsigned polls=0;bool active=false;
    void begin(std::uint64_t now,std::uint64_t windowMs) noexcept {*this={};active=true;start=now;next=now;deadline=now+windowMs;}
    bool due(std::uint64_t now,std::uint64_t pollMs=50) noexcept {
        if(!active||now<next)return false;next=now+pollMs;++polls;return true;}
    bool expired(std::uint64_t now) const noexcept {return active&&now>deadline;}
    std::uint64_t waitedMs(std::uint64_t now) const noexcept {return start&&now>=start?now-start:0;}
};
// C46: 2 = STUMP + FLYING HEAD (C43D): hides the head parts like Hide, then the Lua stump + head (gore32.inl StumpAndHead43).
enum class Style {Hide=0,LooseNeck=1,StumpHead=2};
inline const char* name(Style s) noexcept {return s==Style::LooseNeck?"LOOSE_NECK":s==Style::StumpHead?"STUMP_HEAD":"HIDE";}
inline Style style(int v) noexcept {return v==1?Style::LooseNeck:v==2?Style::StumpHead:Style::Hide;}
inline bool hidesHead(Style s) noexcept {return s!=Style::LooseNeck;} // Hide and StumpHead hide the head parts at once
}
