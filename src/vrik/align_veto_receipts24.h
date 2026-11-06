#pragma once
// C25 receipts for the VrCombatNoStep alignment veto (native_motion112.inl).
// Pure bookkeeping: no native reads, no I/O, no allocation, no policy. The veto
// decision is made before a call is recorded here; nothing here can change it.
#include <cstdint>

namespace kcd2vr::align24 {

// First failing condition of NativeMotionContext112 / NativeAlignmentOwned112,
// in evaluation order. Codes are stable log vocabulary; append only.
enum Refusal : std::uint8_t {
    None=0,ContactIdentity,Mode,LabReference,Contract,PhysFault,WeaponMode,GameContext,HandTracking,
    NativeBodyOwned,PoseStale,FrameUnread,InputStale,EpochMismatch,NotOwned,OutputInactive,Profile,
    Resolve,Observe,ActionType,NoCandidate,Phase,OrdinaryMotion,TrackingSnapshot,HandHeld,TrackingStale,
    Equipment,NoStepDisabled,ManagerIdentity,BodyRequired,HenryId,RecordVector,RecordCount,RecordType,
    RecordTarget,RecordVtable,RecordRace,NoTurnDisabled,Count
};
inline const char* RefusalName(unsigned r) noexcept {
    static const char* const names[]{"NONE","CONTACT_IDENTITY","MODE","LAB_REFERENCE","CONTRACT","PHYS_FAULT",
        "WEAPON_MODE","GAME_CONTEXT","HAND_TRACKING","NATIVE_BODY_OWNED","POSE_STALE","FRAME_UNREAD","INPUT_STALE",
        "EPOCH_MISMATCH","NOT_OWNED","OUTPUT_INACTIVE","PROFILE","RESOLVE","OBSERVE","ACTION_TYPE","NO_CANDIDATE",
        "PHASE","ORDINARY_MOTION","TRACKING_SNAPSHOT","HAND_HELD","TRACKING_STALE","EQUIPMENT","NO_STEP_DISABLED",
        "MANAGER_IDENTITY","BODY_REQUIRED","HENRY_ID","RECORD_VECTOR","RECORD_COUNT","RECORD_TYPE","RECORD_TARGET",
        "RECORD_VTABLE","RECORD_RACE","NO_TURN_DISABLED"};
    static_assert(sizeof(names)/sizeof(names[0])==Count,"refusal names");
    return r<Count?names[r]:"UNKNOWN";
}

// One Henry query (9E37A8) as seen after the unchanged veto decision.
struct Call {
    std::uint64_t key=0;        // hidden attack identity (consumed attack-timing serial)
    std::uint64_t action=0;     // Henry's current native main-channel action object
    std::uint64_t tick=0;
    int phase=-1;               // native action phase if observable, else -1
    bool original=false;        // native answer
    bool atVetoCaller=false;    // returned to 9E2D55
    bool vetoed=false;          // our answer was false while native was true
    std::uint8_t refusal=None;  // why a native true answer at 9E2D55 was kept
    std::uint32_t records=0;    // alignment records in the manager (+58..+60)
    std::uint32_t unexpected=0; // records whose type/vtable is outside the veto's exact set
    std::uint32_t unexpectedType=0;std::uint64_t unexpectedVft=0; // first unexpected record (type, vtable RVA)
};

struct Attack {
    std::uint64_t key=0,action=0,first=0,last=0,unexpectedVft=0;
    std::uint32_t calls=0,trueAnswers=0,vetoed=0,refused=0,foreignCaller=0;
    std::uint32_t unexpectedRecords=0,maxRecords=0,unexpectedType=0,phaseSeen=0;
    std::uint8_t firstRefusal=None;bool open=false;
};

struct Totals {
    std::uint64_t attacks=0,calls=0,trueAnswers=0,vetoed=0,refused=0,unexpectedCalls=0,droppedAttacks=0;
};

inline void Bump(std::uint32_t& v) noexcept {if(v!=0xFFFFFFFFu)++v;}

class Ledger {
public:
    static constexpr unsigned kPending=8;
    void Observe(const Call& c) noexcept {
        if(cur_.open&&(c.key!=cur_.key||c.action!=cur_.action))Close();
        if(!cur_.open){cur_={};cur_.open=true;cur_.key=c.key;cur_.action=c.action;cur_.first=c.tick;}
        cur_.last=c.tick;Bump(cur_.calls);++tot_.calls;
        if(c.phase>=0&&c.phase<=0x7ff)cur_.phaseSeen|=std::uint32_t(c.phase);
        if(c.records>cur_.maxRecords)cur_.maxRecords=c.records;
        if(c.unexpected){
            ++tot_.unexpectedCalls;
            if(!cur_.unexpectedRecords){cur_.unexpectedType=c.unexpectedType;cur_.unexpectedVft=c.unexpectedVft;}
            if(c.unexpected>cur_.unexpectedRecords)cur_.unexpectedRecords=c.unexpected;
        }
        if(!c.original)return;
        Bump(cur_.trueAnswers);++tot_.trueAnswers;
        if(!c.atVetoCaller){Bump(cur_.foreignCaller);return;}
        if(c.vetoed){Bump(cur_.vetoed);++tot_.vetoed;return;}
        Bump(cur_.refused);++tot_.refused;
        if(cur_.firstRefusal==None)cur_.firstRefusal=c.refusal==None?std::uint8_t(Count):c.refusal;
    }
    // An attack ends when another identity arrives (Observe) or Henry's query goes quiet.
    void Expire(std::uint64_t now,std::uint64_t idleMs) noexcept {
        if(cur_.open&&(now<cur_.last||now-cur_.last>idleMs))Close();
    }
    bool Pop(Attack& out) noexcept {
        if(!count_)return false;
        out=ring_[(head_+kPending-count_)%kPending];--count_;return true;
    }
    void Discard() noexcept {cur_={};count_=0;}
    const Totals& totals() const noexcept {return tot_;}
    const Attack& current() const noexcept {return cur_;}
    unsigned pending() const noexcept {return count_;}
private:
    void Close() noexcept {
        cur_.open=false;++tot_.attacks;
        if(count_==kPending){++tot_.droppedAttacks;cur_={};return;} // keep the oldest edges; count the loss
        ring_[head_]=cur_;head_=(head_+1)%kPending;++count_;cur_={};
    }
    Attack cur_{};Attack ring_[kPending]{};unsigned head_=0,count_=0;Totals tot_{};
};

// Line budget: at most `perSecond` emitted attack lines per rolling second.
struct LineBudget {
    std::uint64_t window=0;unsigned used=0;
    bool Take(std::uint64_t now,unsigned perSecond) noexcept {
        if(!window||now<window||now-window>=1000){window=now?now:1;used=0;}
        if(used>=perSecond)return false;
        ++used;return true;
    }
};

} // namespace kcd2vr::align24
