#pragma once
// R94: small standalone scenery uses the verified native rigid-body servo.
// Physics stays enabled throughout a hold. The tracked hand is the target;
// achieved body pose is the optional visible grip, and release retains native
// linear/angular momentum. This does not invent inventory items or move brushes.
// Native PickableItem pickup/theft/stow remains on its existing native route.
// VrStaticPropGrabR89 controls scenery grabs and current-contact prop reactions.
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "prop_drive5916c1.h"
#include "brush_swap_r100.h" // R100 loose scenery props (swap)
#include "prop_impact_sound_r124.h" // R124 impact sound of our loose props (VrPropImpactSoundR124)

namespace kcd2vr::static_grab_r89 {
inline std::atomic<bool> enabled{false};
// These are exclusions for other physics writers, never grants to move a body.
inline std::atomic<std::uint32_t> heldEntityR94[2]{};
// R103: the held prop's entity + rigid body per hand. The hand/weapon world query adds them to its
// skip list (npc_contact172.inl PrepareNpcQuery172), so the visible hand is never pushed off the
// object it holds. Pointers are only compared by the native query, never read through.
inline std::atomic<void*> heldEntityPtrR103[2]{},heldBodyR103[2]{};
inline bool ownsEntityR94(std::uint32_t id) noexcept {
    return id&&(heldEntityR94[0].load(std::memory_order_acquire)==id||
        heldEntityR94[1].load(std::memory_order_acquire)==id);
}
// last VR pickup Use pulse per hand (NoteVrPickupUse43, input thread); the native item pickup
// owns that squeeze, the scenery grab steps back.
inline std::atomic<std::uint64_t> pickupUseTick[2]{};

inline constexpr float kGripClose=.58f,kGripOpen=.30f,kGripRearm=.45f;
inline constexpr std::uint64_t kTouchMemoryMs=450;   // hand contact -> grip close
inline constexpr std::uint64_t kPickupYieldMs=350;   // native pickup Use near the same squeeze wins
inline constexpr double kMaxReach=.30;               // hand to object bounds centre
inline constexpr double kMaxDiameter=.65;            // same box limit as the small-pickup lab
inline constexpr double kDensity=350;                // kg/m3 of the bounding box (hollow mugs, wood, clay)
inline constexpr double kMinMass=.05,kMaxMass=3.0;

enum class ClassKind {Grab,Native,Other};
inline bool hasWord(const char* s,const char* w) noexcept {return s&&w&&std::strstr(s,w)!=nullptr;}
// Grab = loose scenery. Native = something another feature or the game owns (no message).
inline ClassKind classify(const char* cls) noexcept {
    if(!cls||!*cls)return ClassKind::Other;
    // An exact supported loose-body class must not be shadowed by "Light".
    if(!std::strcmp(cls,"RigidBodyLight"))return ClassKind::Grab;
    static const char* native[]{"Pickable","Door","Chest","Stash","Bed","Chair","Bench","Ladder","Dog","Horse",
        "Player","Human","AI","DeadBody","Shop","Torch","Fire","Minigame","Alchemy","Smith","Anvil","Grind",
        "Item","Weapon","Lamp","Light","Trigger","Area","Vehicle","Dialog","Cutscene"};
    for(const char* n:native)if(hasWord(cls,n))return ClassKind::Native;
    static const char* grab[]{"BasicEntity","RigidBodyEx","RigidBody","RigidBodyLight","DestroyableObject",
        "BreakableObject","Debris","GeomEntity"};
    for(const char* g:grab)if(!std::strcmp(cls,g))return ClassKind::Grab;
    return ClassKind::Other;
}
// local AABB (min xyz, max xyz) + world 3x4 row-major (m[0..2] row x ...): world diameter and mass
struct Size {double diameter=0,volume=0;double centre[3]{};bool ok=false;};
inline Size measure(const float (&local)[6],const float (&world)[12]) noexcept {
    Size s{};
    for(float v:local)if(!std::isfinite(v))return s;
    for(float v:world)if(!std::isfinite(v))return s;
    double half[3]{},mid[3]{};
    for(int a=0;a<3;++a){if(!(local[a+3]>local[a]))return s;half[a]=(double(local[a+3])-local[a])*.5;mid[a]=(double(local[a+3])+local[a])*.5;}
    double axis[3][3]{};double scale[3]{};
    for(int c=0;c<3;++c){for(int r=0;r<3;++r)axis[c][r]=world[r*4+c];
        scale[c]=std::sqrt(axis[c][0]*axis[c][0]+axis[c][1]*axis[c][1]+axis[c][2]*axis[c][2]);
        if(!std::isfinite(scale[c])||scale[c]<1e-4)return s;}
    double d2=0;for(int r=0;r<3;++r){double e=0;for(int c=0;c<3;++c)e+=std::fabs(axis[c][r])*half[c];d2+=e*e;}
    s.diameter=2*std::sqrt(d2);
    s.volume=8*half[0]*half[1]*half[2]*scale[0]*scale[1]*scale[2];
    for(int r=0;r<3;++r)s.centre[r]=world[r*4+3]+world[r*4]*mid[0]+world[r*4+1]*mid[1]+world[r*4+2]*mid[2];
    s.ok=std::isfinite(s.diameter)&&std::isfinite(s.volume)&&s.volume>0;
    return s;
}
inline double massKg(double volume) noexcept {
    if(!std::isfinite(volume)||volume<=0)return kMinMass;
    return std::clamp(volume*kDensity,kMinMass,kMaxMass);
}
// Game-thread entity reservation, separate from the per-hand input claim.
// Only its owner can retire it; two hands never run motors on the same body.
struct EntityClaims {
    std::uint32_t ids[2]{};
    bool retain(int side,std::uint32_t id) noexcept {
        if(side<0||side>1||!id||ids[side]||ids[1-side]==id)return false;
        ids[side]=id;return true;
    }
    void retire(int side,std::uint32_t id) noexcept {
        if(side>=0&&side<2&&ids[side]==id)ids[side]=0;
    }
};
// grip edge per hand: close after an open/rearm, never on a held squeeze
struct Edge {bool open=true;
    bool close(float grip) noexcept {
        if(!std::isfinite(grip)){open=false;return false;}
        if(grip<kGripRearm){open=true;return false;}
        if(open&&grip>=kGripClose){open=false;return true;}
        return false;}
};
// Retain the originating body/owner/pose identity, not just a reusable entity id.
struct Touch {
    std::uint32_t entity=0;std::uintptr_t owner=0,collider=0,actor=0;
    std::uint64_t tick=0,worldTick=0,epoch=0,request=0;bool rigid=false;
};
// Per-hand achieved scenery grip. It cannot replace the tracked control target
// and is retired before the hand claim, so a stale packet cannot revive a hold.
inline core::SnapshotMailbox<prop5916c1::Presentation> presentationR94[2];
inline void clearPresentationR94(int side) noexcept {
    if(side>=0&&side<2)(void)presentationR94[side].publish({});
}
inline bool selectPresentationR94(std::uintptr_t character,std::uint64_t epoch,
        std::uint64_t now,int side,std::uint64_t claim,physical::Pose& wrist) noexcept {
    if(side<0||side>1||!claim||!(enabled.load(std::memory_order_acquire)||brush_swap_r100::enabled.load(std::memory_order_acquire))||
       !heldEntityR94[side].load(std::memory_order_acquire))return false;
    prop5916c1::Presentation p{};
    if(!presentationR94[side].read(p)||p.side!=side||!p.body||p.key!=claim||
       p.instantTokenD78!=claim||p.character!=character||p.bodyEpoch!=epoch||
       !p.sourceTick||!p.observationTick||now<p.sourceTick||now<p.observationTick||
       now-p.sourceTick>80||now-p.observationTick>80||!physical::finite(p.wristWorld))return false;
    wrist=p.wristWorld;return true;
}
// Contact reactions require a fresh incoming movement. A resting contact or
// repeated source sample cannot keep adding energy. Real angular response is
// delegated to native AddImpulse at the contact point; this only bounds credit.
inline double contactImpulseR94(double speed,double mass) noexcept {
    if(!std::isfinite(speed)||!std::isfinite(mass)||speed<.10||speed>20||
       mass<.01||mass>kMaxMass)return 0;
    return std::clamp(mass*std::min(speed,4.)*.35,.005,2.0);
}

inline bool recent(std::uint64_t now,std::uint64_t tick,std::uint64_t ms) noexcept {return tick&&now>=tick&&now-tick<=ms;}
inline bool pickupYield(int side,std::uint64_t now) noexcept {
    return side>=0&&side<2&&recent(now,pickupUseTick[side].load(std::memory_order_acquire),kPickupYieldMs);
}
// keep a name safe for a Lua string and the log: letters, digits, _ - . only
inline void cleanName(const char* in,char* out,std::size_t n) noexcept {
    if(!out||!n)return;
    std::size_t k=0;
    for(std::size_t i=0;in&&in[i]&&k+1<n;++i){const char c=in[i];
        out[k++]=(c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'||c=='.'?c:'_';}
    out[k]=0;if(!k&&n>1){out[0]='?';out[1]=0;}
}
}
