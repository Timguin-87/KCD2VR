#pragma once
// DS-F18 (v3, after Codex 153105 review): typed add/remove-constraint service.
// v3 changes:
// - Every failure while a native ID is latched QUARANTINES it (Created stage
//   faults, remove timeout, lost constraint); fault() is reserved for clean
//   (no-resource) failures and clears nothing it does not own.
// - The status readback's OWN world must match (no wildcard): a Present/Absent/
//   Fault for another world never drives/releases this owner.
// - Cancelling before the first Create retires cleanly without a native request;
//   once createIssued, the create's completion stays bound to that request (a
//   timeout with an outstanding create quarantines and still admits the late ID).
// - resolveQuarantine needs the exact Absent(id, world) proof (world pointer
//   change alone is not proof the old constraint vanished); acquire(world=0)
//   is refused.
#include <cstdint>
#include <atomic>
#include <limits>
namespace kcd2vr::constraint60 {
// Process-wide, non-reused request identity; saturation refuses new requests.
inline std::atomic<std::uint64_t> nextAcquireToken60{0};
inline std::uint64_t mintAcquireToken60() noexcept {
    auto previous=nextAcquireToken60.load(std::memory_order_relaxed);
    for(unsigned retry=0;retry<32;++retry){
        if(previous==std::numeric_limits<std::uint64_t>::max())return 0;
        if(nextAcquireToken60.compare_exchange_weak(previous,previous+1,std::memory_order_relaxed))return previous+1;
    }
    return 0; // busy: no lifecycle or native ownership was changed
}
enum class Stage : unsigned char {Idle,Requested,Created,Present,Driving,Removing,Removed,Fault,Quarantined};
inline const char* stageName(Stage s) noexcept {
    switch(s){case Stage::Idle:return "IDLE";case Stage::Requested:return "REQUESTED";case Stage::Created:return "CREATED";
              case Stage::Present:return "PRESENT";case Stage::Driving:return "DRIVING";case Stage::Removing:return "REMOVING";
              case Stage::Removed:return "REMOVED";case Stage::Fault:return "FAULT";default:return "QUARANTINED";}
}
enum class Action : unsigned char {None,Create,Drive,Remove,Cleanup,Fault};
inline const char* actionName(Action a) noexcept {
    switch(a){case Action::None:return "NONE";case Action::Create:return "CREATE";case Action::Drive:return "DRIVE";
              case Action::Remove:return "REMOVE";case Action::Cleanup:return "CLEANUP";default:return "FAULT";}
}
struct Status { // typed status26-equivalent; world MUST match the owner's world for Present/Absent/Fault
    enum class Kind : unsigned char {Unknown,Present,Absent,Fault};
    Kind kind=Kind::Unknown;std::uint32_t id=0;std::uintptr_t world=0;
};
struct Snap {
    Status status{};          // current readback (Unknown when unreadable/unqualified)
    bool acceptedIdValid=false;std::uint32_t acceptedId=0; // the create call's returned id this frame
    std::uint64_t acceptedToken=0; // DS-F54 (Codex F46): the acquire token the native reply carries - a stale reply never latches
    std::uintptr_t world=0;   // the world generation the status was read from
    bool worldReplaced=false; // generation changed under us
};
struct Step {Stage stage=Stage::Idle;Action action=Action::None;const char* reason="NONE";};
struct Service {
    Stage stage=Stage::Idle;
    std::uint32_t id=0;std::uintptr_t buddy=0,world=0;
    std::uint64_t stageTick=0,createTick=0,acquireToken=0; // DS-F54: monotonic per-acquire identity - reply joins must carry it
    bool createIssued=false,cancel=false;
    static constexpr std::uint64_t kCreateTimeoutMs=500,kRemoveTimeoutMs=1000;
    bool acquire(std::uintptr_t newBuddy,std::uintptr_t newWorld,std::uint64_t now) noexcept {
        if(!newBuddy||!newWorld||stage==Stage::Quarantined)return false; // world 0 refused: unqualified ownership
        if(stage!=Stage::Idle&&stage!=Stage::Removed&&stage!=Stage::Fault)return false;
        const auto token=mintAcquireToken60();if(!token)return false;
        *this={};buddy=newBuddy;world=newWorld;stage=Stage::Requested;stageTick=createTick=now;createIssued=false;cancel=false;
        acquireToken=token; // minted independently of the reset, unique across Services
        return true;
    }
    // Cancel/release intent. During Requested/Created it only sets cancel (a late
    // accepted id drains into Removing); during Present/Driving it queues removal.
    bool release(std::uint64_t now) noexcept {
        switch(stage){
        case Stage::Requested:case Stage::Created:cancel=true;return true;
        case Stage::Present:case Stage::Driving:stage=Stage::Removing;stageTick=now;return true;
        default:return false;}
    }
    // The adapter proves the quarantined resource is gone: exact typed Absent for
    // the retained id in OUR world. worldReplaced alone is NOT proof. A quarantined
    // pending-create (no id yet) resolves by draining a late accepted id.
    bool resolveQuarantine(const Snap& s,std::uint64_t now) noexcept {
        if(stage!=Stage::Quarantined)return false;
        if(!id){
            if(s.acceptedIdValid&&s.acceptedId&&s.acceptedToken==acquireToken){id=s.acceptedId;stage=Stage::Removing;stageTick=now;return true;} // DS-F54: token-joined late drain
            return false;
        }
        const bool gone=s.status.kind==Status::Kind::Absent&&s.status.id==id&&s.status.world==world;
        if(!gone)return false;
        id=0;buddy=0;world=0;stage=Stage::Fault;return true;
    }
    bool worldMatch(const Snap& s) const noexcept {return !s.world||s.world==world;}
    bool statusWorldOk(const Status& st) const noexcept {return st.world==world;} // no wildcard on the readback
    Step step(const Snap& s,std::uint64_t now) noexcept {
        Step out{};out.stage=stage;out.reason=stageName(stage);
        switch(stage){
        case Stage::Idle:break;
        case Stage::Requested:
            if(s.worldReplaced||!worldMatch(s)){quarantine(out,"WORLD_REPLACED_DURING_CREATE");break;} // a pending create may exist
            if(s.status.kind==Status::Kind::Fault&&statusWorldOk(s.status)){quarantine(out,"CREATE_STATUS_FAULT");break;} // DS-F45: a faulted create leaves no clean fault (a native queue entry may exist)
            if(s.acceptedIdValid&&s.acceptedId&&createIssued&&s.acceptedToken==acquireToken){id=s.acceptedId;stage=Stage::Created;stageTick=now; // DS-F54: a stale reply (wrong token) never latches
                if(cancel){stage=Stage::Removing;stageTick=now;out.action=Action::Remove;out.reason="LATE_ACCEPTED_ID_DRAINING";break;}
                out.reason="ID_ACCEPTED_WAITING_STATUS";break;}
            if(cancel&&!createIssued){fault(out,"CANCELLED_BEFORE_CREATE");break;} // clean: no native request was made
            if(now<stageTick||now-stageTick>kCreateTimeoutMs){
                if(createIssued)quarantine(out,"CREATE_ACCEPT_TIMEOUT_PENDING_NATIVE"); // outstanding native create may complete later
                else fault(out,"CREATE_TIMEOUT_OR_CLOCK");
                break;}
            if(!createIssued){createIssued=true;out.action=Action::Create;out.reason="ISSUING_CREATE";}
            else out.reason="WAITING_ACCEPTED_ID";break;
        case Stage::Created:
            if(s.worldReplaced||!worldMatch(s)){quarantine(out,"WORLD_REPLACED_DURING_CREATE");break;}
            if(cancel){stage=Stage::Removing;stageTick=now;out.action=Action::Remove;out.reason="CANCEL_DRAINING_ACCEPTED_ID";break;}
            if(s.status.kind==Status::Kind::Fault&&statusWorldOk(s.status)){quarantine(out,"CREATE_STATUS_FAULT");break;}
            if(s.status.kind==Status::Kind::Absent&&s.status.id==id&&statusWorldOk(s.status)){quarantine(out,"CREATE_RESULT_ABSENT");break;}
            if(s.status.kind==Status::Kind::Present&&s.status.id==id&&statusWorldOk(s.status)){stage=Stage::Present;stageTick=now;out.action=Action::Drive;out.reason="PRESENT_FIRST_DRIVE";break;}
            if(now<createTick||now-createTick>kCreateTimeoutMs){quarantine(out,"CREATE_STATUS_TIMEOUT");break;} // latched id: never a clean fault
            out.reason="WAITING_PRESENT_STATUS";break;
        case Stage::Present:
            if(s.worldReplaced||!worldMatch(s)){quarantine(out,"WORLD_REPLACED_DURING_PRESENT");break;}
            if(s.status.kind==Status::Kind::Fault&&statusWorldOk(s.status)){quarantine(out,"PRESENT_STATUS_FAULT");break;}
            if(s.status.kind==Status::Kind::Absent&&s.status.id==id&&statusWorldOk(s.status)){quarantine(out,"CONSTRAINT_LOST_BEFORE_DRIVE");break;}
            if(s.status.kind==Status::Kind::Present&&s.status.id==id&&statusWorldOk(s.status)){stage=Stage::Driving;stageTick=now;out.action=Action::Drive;out.reason="DRIVING_START";break;}
            out.reason="WAITING_PRESENT_STATUS";break; // Unknown or wrong world: never drive
        case Stage::Driving:
            if(s.worldReplaced||!worldMatch(s)){quarantine(out,"WORLD_REPLACED_DURING_DRIVE");break;}
            if(s.status.kind==Status::Kind::Fault&&statusWorldOk(s.status)){quarantine(out,"DRIVE_STATUS_FAULT");break;}
            if(s.status.kind==Status::Kind::Absent&&s.status.id==id&&statusWorldOk(s.status)){quarantine(out,"CONSTRAINT_LOST_DURING_DRIVE");break;}
            if(s.status.kind==Status::Kind::Present&&s.status.id==id&&statusWorldOk(s.status)){out.action=Action::Drive;out.reason="DRIVING";break;}
            out.reason="DRIVING_STATUS_UNKNOWN_HOLD";break; // Unknown or wrong world: hold, no drive action
        case Stage::Removing:
            if(s.worldReplaced||!worldMatch(s)){quarantine(out,"WORLD_REPLACED_DURING_REMOVE");break;}
            if(s.status.kind==Status::Kind::Absent&&s.status.id==id&&statusWorldOk(s.status)){stage=Stage::Removed;out.stage=Stage::Removed;out.reason="REMOVED_CLEAN";break;}
            if(s.status.kind==Status::Kind::Fault&&statusWorldOk(s.status)){quarantine(out,"REMOVE_STATUS_FAULT");break;}
            if(now<stageTick||now-stageTick>kRemoveTimeoutMs){quarantine(out,"REMOVE_TIMEOUT");break;}
            out.action=Action::Remove;out.reason="WAITING_REMOVE";break;
        case Stage::Removed:
            stage=Stage::Idle;id=0;buddy=0;world=0;out.reason="RELEASED_TO_IDLE";break;
        case Stage::Fault:out.reason="FAULT_NEEDS_EXPLICIT_REACQUIRE";break;
        case Stage::Quarantined:
            if(!id&&s.acceptedIdValid&&s.acceptedId&&s.acceptedToken==acquireToken){id=s.acceptedId;stage=Stage::Removing;stageTick=now;out.action=Action::Remove;out.reason="QUARANTINE_LATE_ACCEPTED_ID_DRAINING";break;} // DS-F54: token-joined
            out.action=Action::Cleanup;out.reason="QUARANTINED_NEEDS_RESOLVE_OR_NATIVE_CLEANUP";break;
        }
        out.stage=stage;return out;
    }
    void fault(Step& out,const char* why) noexcept {stage=Stage::Fault;id=0;buddy=0;world=0;out.action=Action::Fault;out.reason=why;out.stage=Stage::Fault;} // clean only
    void quarantine(Step& out,const char* why) noexcept {stage=Stage::Quarantined;out.action=Action::Fault;out.reason=why;out.stage=Stage::Quarantined;} // retains id/buddy/world
};
}
