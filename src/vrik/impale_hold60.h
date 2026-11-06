#pragma once
// CL-IMPALE60 (Claude, 20 Oct): the constraint hold for a stab KILL - pure logic, host tested (tests/impale_hold60_test.cpp).
//
// Why a constraint and not the velocity servo: every native death is a ragdoll (combat_action_hit death rows: 141 of 141
// hasRagdoll=true), so the victim animation is NOT in the way. What we have today (C32/C33 HANG) drives one ragdoll part
// with pe_action_set_velocity every tick; it does not hold anything, it only chases the blade point. A type-5 add
// constraint (ball joint at the wound point) between the ragdoll part and the weapon body is what holds the corpse.
//
// Facts used (all from WHGame.dll, see CLAUDE_IMPALE_AND_KICK_PLAN_2026-10-20.md section D/E):
//  - type 5 on a ragdoll (vft 3A48B30, Action A99DF0) is a straight `call AC0D18`: same handler, same 0x88 packet, same
//    queue size as the weapon body. type 6 (remove) falls through to the same AC0D18 default call.   [PROVEN]
//  - frame bit0 is LOCAL; frame bit1 is WORLD (exact AC1443 transform branch).
//  - 0x200 = ignore buddy [LIKELY], 0x100 = INACTIVE, never set [LIKELY-STRONG, setFlags refuses].
//  - source = ragdoll articulated body + partid0 = the hit part; buddy = the weapon body handle. pt[0] = pt[1] = the
//    anchor in WORLD space; actual hold additionally requires typed source/buddy proof.               [packet layout PROVEN]
// NOT PROVEN: that [buddy+0x47] (type byte) of a ragdoll part / weapon passes the <=4 and within-3 check. If the add is
// refused the caller keeps the old velocity hang. Nothing here writes natively; the caller owns the call + the lease.
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include "constraint_packet60.h"
#include "impale47.h"
#include "../core/collab_test_h21.h"
#include "../subsystems/physics/impale_feel60.h"

namespace kcd2vr::impale_hold60 {
namespace pkt=kcd2vr::constraint_packet60;
enum class Why : int {None=0,BadInput,NoBuddy,NoPart,NoBody,OffBlade,TipFar,FlagsRefused,AlreadyHeld,NotHeld};
inline const char* name(Why w) noexcept {
    switch(w){case Why::None:return "OK";case Why::BadInput:return "BAD_INPUT";case Why::NoBuddy:return "NO_WEAPON_BODY";
    case Why::NoPart:return "NO_RAGDOLL_PART";case Why::NoBody:return "NO_RAGDOLL_BODY";case Why::OffBlade:return "WOUND_OFF_BLADE";
    case Why::TipFar:return "TIP_FAR_FROM_WOUND";case Why::FlagsRefused:return "FLAGS_REFUSED";
    case Why::AlreadyHeld:return "ALREADY_HELD";default:return "NOT_HELD";}
}
// short, readable line for the game (Game.SendInfoText) - one line, no log feature
inline const char* visible(Why w) noexcept {
    switch(w){case Why::NoBuddy:return "Impale: no weapon body, corpse only follows";case Why::NoPart:return "Impale: no body part to hold";
    case Why::NoBody:return "Impale: body not a ragdoll yet";case Why::OffBlade:case Why::TipFar:return "Impale: blade not in the wound";
    case Why::FlagsRefused:return "Impale: hold refused";default:return "";}
}
// Seams the T4/physics owner fills (null = constraint hold unavailable, the C32 velocity hang runs exactly as before):
//  weaponBodyProvider(side) = the weapon's physical body handle for that hand, game thread, only while the weapon is leased;
//  say(text)                = the one-line in-game message (visible-failure mailbox), called at most once per impale.
inline std::atomic<std::uintptr_t(*)(int) noexcept> weaponBodyProvider{nullptr};
inline std::atomic<void(*)(const char*) noexcept> say{nullptr};
inline constexpr std::uint32_t kFlagIgnoreBuddy=0x200u;
// Exact retail transform branch: explicit WORLD frame bit1, never INACTIVE.
inline constexpr std::uint32_t kFlagWorldFrame=0x002u;
inline constexpr std::uint32_t kHoldFlags=kFlagWorldFrame|kFlagIgnoreBuddy; // never 0x100

struct Request {
    std::uint64_t serial=0;
    std::uintptr_t ragdollBody=0;   // articulated body handle (vft 3A48B30) = the Action target, as PartVelocity uses it
    std::uintptr_t weaponBody=0;    // the weapon's physical body handle (buddy). 0 = not available -> refused
    int part=-1;                    // ragdoll part id (candidate.part)
    double grip[3]{},tip[3]{},wound[3]{}; // world metres: blade line now, native contact point of the killing thrust
};
struct Plan {
    Why why=Why::BadInput;bool ok=false;
    double anchor[3]{};double depth=0;
    pkt::Packet add{};
};
inline bool finite3(const double v[3]) noexcept {return std::isfinite(v[0])&&std::isfinite(v[1])&&std::isfinite(v[2]);}
// Builds the type-5 packet for ONE hold. The anchor is the blade point the existing pick logic (impale47) chooses; the
// wound must be on the blade (lenient: the receipt comes 60-80 ms after the impact, the blade has moved).
inline Plan plan(const Request& r) noexcept {
    Plan out{};
    if(!r.serial||!finite3(r.grip)||!finite3(r.tip)||!finite3(r.wound)){out.why=Why::BadInput;return out;}
    if(!r.ragdollBody){out.why=Why::NoBody;return out;}
    if(r.part<0){out.why=Why::NoPart;return out;}
    if(!r.weaponBody){out.why=Why::NoBuddy;return out;}
    const auto pick=impale47::pickStuckPoint(r.grip,r.tip,r.wound,true);
    if(!pick.ok()){out.why=pick.pick==impale47::Pick::OffBlade?Why::OffBlade:Why::TipFar;return out;}
    for(int i=0;i<3;++i)out.anchor[i]=pick.point[i];
    out.depth=pick.depth;
    auto p=pkt::defaults();
    pkt::setBuddy(p,r.weaponBody);
    pkt::setPoint(p,0,float(out.anchor[0]),float(out.anchor[1]),float(out.anchor[2])); // world frame (flag pending reconciliation)
    pkt::setPoint(p,1,float(out.anchor[0]),float(out.anchor[1]),float(out.anchor[2]));
    pkt::setPartIds(p,std::uint32_t(r.part),pkt::kIntUnset); // part of the SOURCE; the weapon body has one part
    if(!pkt::setFlags(p,kHoldFlags)){out.why=Why::FlagsRefused;return out;}
    std::uint32_t f=0;std::memcpy(&f,p.bytes+0x60,4);
    if(f!=kHoldFlags||(f&0x100u)){out.why=Why::FlagsRefused;return out;} // belt and braces: the bytes, not the return value
    out.add=p;out.ok=true;out.why=Why::None;return out;
}

// One hold per impale serial. The game thread owns it. The native id is the constraint id the Action returned
// (queued work, NOT an ack) - release always sends the remove, a refused remove is retried (bounded), never forgotten.
enum class Phase : unsigned char {Idle,AwaitingPresent,Held,Releasing,Quarantined};
struct Hold {
    Phase phase=Phase::Idle;std::uint64_t serial=0,since=0;std::uint32_t id=0;int removeTries=0;
    bool active() const noexcept {return phase!=Phase::Idle;}
    Why held(std::uint64_t ser,std::uint64_t now,std::uint32_t nativeId) noexcept {
        if(phase!=Phase::Idle)return Why::AlreadyHeld;
        if(!ser||!nativeId)return Why::BadInput;
        // Action returned an ID, not Present. A separately retained typed source
        // and buddy must prove that EXACT constraint before it owns the motor.
        phase=Phase::AwaitingPresent;serial=ser;since=now;id=nativeId;removeTries=0;return Why::None;
    }
    bool present(std::uint64_t ser,std::uint32_t exactId) noexcept {
        if(phase!=Phase::AwaitingPresent||ser!=serial||exactId!=id)return false;phase=Phase::Held;return true;
    }
    bool constraintDrives() const noexcept {return phase==Phase::Held;}
    bool motorBlocked() const noexcept {return phase!=Phase::Idle;}
    bool beginRelease() noexcept {if(phase!=Phase::Held&&phase!=Phase::AwaitingPresent)return false;phase=Phase::Releasing;return true;}
    bool removed(bool submitted) noexcept {
        if(phase!=Phase::Releasing)return false;
        ++removeTries;
        // A legacy bool cannot distinguish refusal-before-entry from an entered
        // uncertain action. Never retry it or forget the original identity.
        if(!submitted)phase=Phase::Quarantined;
        return false; // positive submission is NOT exact Absent
    }
    void quarantine() noexcept {if(active())phase=Phase::Quarantined;}
    bool absent(std::uint64_t ser,std::uint32_t exactId) noexcept {
        if((phase!=Phase::Releasing&&phase!=Phase::Quarantined)||ser!=serial||exactId!=id)return false;
        *this={};return true;
    }
};
// The remove packet is DeepSeek's constraint_packet60 removeDefaults()+setRemoveId(id) (retail: type 6, id, bRemove=1);
// the caller sends it with Action(...,0) on the same body it used for the add.
}
