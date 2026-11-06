#pragma once
// R59.16C10R8 PHYSICS BODIES for the VR hands and the held blade (HIGGS-style "the engine owns the contact").
//
// The mod's existing ray sweeps let the WORLD stop the weapon/hands (static+terrain only). Nothing in the physics
// world knew the VR weapon or hands existed, so they could never push a prop, a corpse or be stopped by a body.
// This creates three PE_RIGID bodies (blade capsule, one sphere per palm) that follow the tracked poses by velocity.
//
// Modes (Immersion > EXPERIMENTS, INI VrPhysBodies):
//   0 OFF      nothing is created.
//   1 FOLLOW   bodies exist and follow, but collide with NOTHING (geom flags 0). Proves creation/following/cleanup.
//   2 COLLIDE  new proxy creation REFUSED until a full-volume spawn-clearance boundary is verified.
//              Existing proxies keep safe parking/cleanup. This does not disable the main weapon sweeps.
//              Intended collision mask excludes Henry/living NPCs; rays never see the bodies.
//
// Native layouts below are KCD2's own CryCommon (libKCD2) and were checked against WHGame 6A350E20:
//   CreatePrimitive(+8102E0) dispatch 4 sphere / 5 cylinder / 6 capsule (capsule init reads +0..+0x1C),
//   CRigidEntity::AddGeometry(+526194) reads density +4, mass +8, pos +0xC, q +0x18, scale +0x28,
//   pe_action_set_velocity / pe_status_dynamics already verified (CP03). Everything else is header-derived and
//   is read back after creation before the body is driven.
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <cstring>

namespace kcd2vr::phys_bodies59c10r8 {

// ---------------------------------------------------------------- native layouts (x64, CryEngine 3 generation)
struct V3f {float x=0,y=0,z=0;};
struct Qf {float x=0,y=0,z=0,w=1;}; // Quat_tpl: Vec3 v; float w
inline constexpr std::uint32_t kUnusedF=0xFFBFFFFFu; // MARK_UNUSED float
inline constexpr std::int32_t kUnusedI=std::int32_t(0x80000000u);
inline float unusedFloat() noexcept {float f;const std::uint32_t u=kUnusedF;static_assert(sizeof f==sizeof u);std::memcpy(&f,&u,4);return f;}

struct PrimSphere {V3f center;float r=0;};                         // primitives::sphere   (type 4)
struct PrimCapsule {V3f center,axis;float r=0,hh=0;};              // primitives::capsule  (type 6)
static_assert(sizeof(PrimSphere)==16&&sizeof(PrimCapsule)==32);
static_assert(offsetof(PrimCapsule,r)==0x18&&offsetof(PrimCapsule,hh)==0x1C);

struct ParamsPos {int type=0;V3f pos;Qf q;float scale;void* pMtx3x4=nullptr;void* pMtx3x3=nullptr;int iSimClass;int bRecalcBounds=1;bool bEntGridUseOBB=false;};
static_assert(offsetof(ParamsPos,pos)==4&&offsetof(ParamsPos,q)==16&&offsetof(ParamsPos,scale)==32&&offsetof(ParamsPos,pMtx3x4)==40&&offsetof(ParamsPos,iSimClass)==56&&sizeof(ParamsPos)==72);

struct ParamsSimulation {int type=10;int iSimClass;float maxTimeStep,minEnergy,damping;V3f gravity;float dampingFreefall;V3f gravityFreefall;
    float maxRotVel,mass,density;int maxLoggedCollisions,disablePreCG;float maxFriction;int collTypes;};
static_assert(offsetof(ParamsSimulation,gravity)==20&&offsetof(ParamsSimulation,gravityFreefall)==36&&offsetof(ParamsSimulation,mass)==52&&offsetof(ParamsSimulation,maxLoggedCollisions)==60&&sizeof(ParamsSimulation)==76);

struct ParamsFlags {int type=15;std::uint32_t flags,flagsOR,flagsAND;};
struct CollisionClass {std::uint32_t type=0,ignore=0;};
struct ParamsCollisionClass {int type=26;CollisionClass orBits{},andBits{0xFFFFFFFFu,0xFFFFFFFFu};};
static_assert(sizeof(ParamsFlags)==16&&sizeof(ParamsCollisionClass)==20);

struct GeomParams {int type=0;float density=0,mass=0;V3f pos;Qf q;float scale=1;void* pMtx3x4=nullptr;void* pMtx3x3=nullptr;int surface_idx=0;
    std::uint32_t flags=0,flagsCollider=0;float minContactDist;int idmatBreakable=-1;void* pLattice=nullptr;int* pMatMapping=nullptr;int nMats=0;int bRecalcBBox=1;};
static_assert(offsetof(GeomParams,mass)==8&&offsetof(GeomParams,pos)==12&&offsetof(GeomParams,q)==24&&offsetof(GeomParams,scale)==40&&
              offsetof(GeomParams,pMtx3x4)==48&&offsetof(GeomParams,surface_idx)==64&&offsetof(GeomParams,flags)==68&&
              offsetof(GeomParams,flagsCollider)==72&&offsetof(GeomParams,pLattice)==88&&offsetof(GeomParams,bRecalcBBox)==108&&sizeof(GeomParams)==112);

struct ActionSetVelocity {int type=10;int ipart;int partid;V3f v,w;int bRotationAroundPivot=0;};
static_assert(offsetof(ActionSetVelocity,v)==12&&offsetof(ActionSetVelocity,w)==24&&sizeof(ActionSetVelocity)==40);

struct StatusPos {int type=1;int partid=-1,ipart=-1;std::uint32_t flags=0,flagsOR=0,flagsAND=0;V3f pos;V3f bbox[2];Qf q;float scale=1;int iSimClass=0;
    void* pMtx3x4=nullptr;void* pMtx3x3=nullptr;void* pGeom=nullptr;void* pGeomProxy=nullptr;float timeBack=0;};
static_assert(offsetof(StatusPos,pos)==24&&offsetof(StatusPos,q)==60&&offsetof(StatusPos,pMtx3x4)==88&&offsetof(StatusPos,timeBack)==120);

struct CollHistoryItem {V3f pt,n,v[2];float mass[2];float age;int idCollider;int partid[2];int idmat[2];};
static_assert(sizeof(CollHistoryItem)==80);
struct StatusCollisions {int type=9;CollHistoryItem* pHistory=nullptr;int len=0;float age=0;int bClearHistory=0;};
static_assert(offsetof(StatusCollisions,pHistory)==8&&offsetof(StatusCollisions,len)==16&&sizeof(StatusCollisions)==32);

inline constexpr int kPeRigid=2;
inline constexpr int kPrimSphere=4,kPrimCapsule=6;
inline constexpr std::uint32_t kGeomColltype0=0x1;
inline constexpr std::uint32_t kPefNeverAffectTriggers=0x8000,kPefIgnoreAreas=0x800000,kPefLogCollisions=0x2000000,kPefTraceable=0x400;
inline constexpr std::uint32_t kCollClassLiving=1u<<2,kCollClassGame=1u<<10;
inline constexpr int kForeignMagic=0x4B565242; // 'KVRB'

// ---------------------------------------------------------------- pure follow logic (host-tested)
enum class Kind : int {Blade=0,HandRight=1,HandLeft=2};
inline const char* Name(Kind k) noexcept {return k==Kind::Blade?"BLADE":k==Kind::HandRight?"HAND_R":"HAND_L";}
struct Target {bool valid=false;V3f p;Qf q;std::uint64_t tick=0;};
struct Tuning {float tau=.035f,maxSpeed=14.f,maxAngSpeed=40.f,teleportDistance=.60f;std::uint64_t maxAgeMs=80;};
struct Command {enum class Op {None,Velocity,Teleport,Park} op=Op::None;V3f v,w;V3f p;Qf q;float error=0;};

inline V3f sub(V3f a,V3f b) noexcept {return {a.x-b.x,a.y-b.y,a.z-b.z};}
inline float len(V3f a) noexcept {return std::sqrt(a.x*a.x+a.y*a.y+a.z*a.z);}
inline V3f mul(V3f a,float s) noexcept {return {a.x*s,a.y*s,a.z*s};}
inline bool finite(V3f a) noexcept {return std::isfinite(a.x)&&std::isfinite(a.y)&&std::isfinite(a.z);}
inline bool finite(Qf q) noexcept {return std::isfinite(q.x)&&std::isfinite(q.y)&&std::isfinite(q.z)&&std::isfinite(q.w);}
inline V3f clampLen(V3f a,float m) noexcept {const float l=len(a);return l>m&&l>0?mul(a,m/l):a;}
// angular velocity that rotates `from` to `to` within tau (shortest arc)
inline V3f angularTo(Qf from,Qf to,float tau) noexcept {
    // e = to * conj(from)
    const Qf c{-from.x,-from.y,-from.z,from.w};
    Qf e{to.w*c.x+to.x*c.w+to.y*c.z-to.z*c.y, to.w*c.y-to.x*c.z+to.y*c.w+to.z*c.x, to.w*c.z+to.x*c.y-to.y*c.x+to.z*c.w, to.w*c.w-to.x*c.x-to.y*c.y-to.z*c.z};
    if(e.w<0){e.x=-e.x;e.y=-e.y;e.z=-e.z;e.w=-e.w;}
    const float s=std::sqrt(e.x*e.x+e.y*e.y+e.z*e.z);if(s<1e-6f)return {};
    const float angle=2.f*std::atan2(s,e.w);return mul(V3f{e.x/s,e.y/s,e.z/s},angle/std::max(tau,1e-3f));
}
// One update: a stale/invalid target parks the body (zero velocity, no teleport); a large error teleports; otherwise
// the body is given the velocity that closes the error in tau, capped. Collisions in mode 2 then do the rest.
inline Command step(const Target& t,V3f current,Qf currentQ,bool haveCurrent,std::uint64_t now,const Tuning& u={},bool rotate=true) noexcept {
    Command c{};
    if(!t.valid||!finite(t.p)||!finite(t.q)||!t.tick||now<t.tick||now-t.tick>u.maxAgeMs){c.op=Command::Op::Park;return c;}
    if(!haveCurrent||!finite(current)){c.op=Command::Op::Teleport;c.p=t.p;c.q=t.q;return c;}
    const V3f e=sub(t.p,current);c.error=len(e);
    if(c.error>u.teleportDistance){c.op=Command::Op::Teleport;c.p=t.p;c.q=t.q;return c;}
    c.op=Command::Op::Velocity;c.v=clampLen(mul(e,1.f/u.tau),u.maxSpeed);
    c.w=rotate&&finite(currentQ)?clampLen(angularTo(currentQ,t.q,u.tau),u.maxAngSpeed):V3f{};
    return c;
}
// Capsule between two world points: entity pose at the midpoint, local axis +Y, half height = half length.
inline bool capsuleFromSegment(V3f a,V3f b,V3f& center,Qf& q,float& halfHeight) noexcept {
    const V3f d=sub(b,a);const float l=len(d);if(!finite(a)||!finite(b)||l<.05f||l>3.f)return false;
    center={(a.x+b.x)*.5f,(a.y+b.y)*.5f,(a.z+b.z)*.5f};halfHeight=l*.5f;
    const V3f n=mul(d,1.f/l);const V3f y{0,1,0};
    const float dot=std::clamp(n.y,-1.f,1.f);
    if(dot>.99999f){q={};return true;}
    if(dot<-.99999f){q={1,0,0,0};return true;} // 180 deg about X
    V3f axis{y.y*n.z-y.z*n.y,y.z*n.x-y.x*n.z,y.x*n.y-y.y*n.x};const float al=len(axis);axis=mul(axis,1.f/al);
    const float half=std::acos(dot)*.5f,s=std::sin(half);q={axis.x*s,axis.y*s,axis.z*s,std::cos(half)};return true;
}
}
