#pragma once
// R9 lane B: why ONE squeeze did not become a hand door claim. H40 used to fail silently before its
// claim (R8d2 log: 0 claims in 9 min of F5 squeezes, every receipt frozen at the 16:36 one), so the
// next run could not tell a busy hand from a dead pose. One code per press, first failure wins, the
// claim clears it. Pure, no native reads. Text is short enough for the one visible line (96 chars).
#include <atomic>
#include <cstdint>

namespace kcd2vr::door_press_why_r9 {
enum Code : unsigned {
    None=0,
    HandBusy=1,          // not eligible: menu, item in hand, trigger, thumbstick, hand not tracked
    OtherTarget=2,       // the game offers another object (pickup) under this hand
    Off=3,               // H40 transport/enable off
    Trigger=4,           // trigger pressed (fist) during the squeeze
    NoIdentity=5,        // player/equipment/tracking identity not ready at the press
    PressExpired=6,      // squeeze older than 100 ms when it could be admitted
    LeaseBusy=7,         // holster draw / old native Use holds this hand this frame
    SharedBusy=8,        // a throw/reload/door transaction holds this hand
    PoseMissing=9,       // no completed VR pose frame (or invalid/stale)
    PoseNotPermitted=10, // completed pose not permitted (menu/dialogue/native body/reload/policy)
    HandNotAvailable=11, // completed pose: this hand holds/supports/solver-owns something
    PoseNotCurrent=12,   // current reader refused (see currentWhy)
    IdentityChanged=13,  // press identity != completed pose identity
    ClaimRace=14,        // serial/claim/retain CAS lost
    Count
};
inline const char* name(unsigned c) noexcept {
    static constexpr const char* n[]{"NONE","HAND_BUSY","OTHER_TARGET","OFF","TRIGGER","NO_IDENTITY","PRESS_EXPIRED",
        "LEASE_BUSY","SHARED_BUSY","POSE_MISSING","POSE_NOT_PERMITTED","HAND_NOT_AVAILABLE","POSE_NOT_CURRENT",
        "IDENTITY_CHANGED","CLAIM_RACE"};
    return c<Count?n[c]:"UNKNOWN";
}
// what Miki sees. only the SYSTEM refusals (5+) get a line: 1..4 are the hand's own state.
inline const char* say(unsigned c) noexcept {
    switch(c){
    case NoIdentity:return "door: hand door not ready (player/equipment not reported yet)";
    case PressExpired:return "door: squeeze came too late, squeeze again";
    case LeaseBusy:return "door: this hand is busy with a holster/draw";
    case SharedBusy:return "door: this hand is busy with a knife/reload";
    case PoseMissing:return "door: no VR hand pose this frame";
    case PoseNotPermitted:return "door: game owns the body now (menu/dialogue/animation)";
    case HandNotAvailable:return "door: this hand holds or supports something";
    case PoseNotCurrent:return "door: hand pose not current, squeeze again";
    case IdentityChanged:return "door: body changed during the squeeze, squeeze again";
    default:return nullptr;
    }
}
inline bool visible(unsigned c) noexcept {return say(c)!=nullptr;}
// first failure of the current press per hand. note() is a no-op after the first code of a press.
struct Press {
    unsigned code=None;std::uint64_t press=0;bool open=false;
    void begin(std::uint64_t serial) noexcept {code=None;press=serial;open=true;}
    // returns true only for the first code of this press
    bool note(unsigned c) noexcept {if(!open||code!=None||c==None)return false;code=c;return true;}
    void claimed() noexcept {code=None;open=false;}
    void end() noexcept {open=false;}
};
inline std::atomic<unsigned> last[2]{};          // last press code per hand (0 = claimed / none)
inline std::atomic<std::uint64_t> lastTick[2]{}; // when it was set
inline std::atomic<std::uint64_t> refused[2]{},claimedCount[2]{};
}
