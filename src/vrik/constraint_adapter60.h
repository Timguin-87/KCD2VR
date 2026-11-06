#pragma once
#include "constraint_service60.h"
#include "constraint_op60.h"
#include <cstdint>
// DS-F45 (after Codex F38 review): adapter bridge v3.
// v3 corrections:
// - the pending retry RE-QUALIFIES against the current lifecycle: if the
//   service's world changed, the stale pending is cancelled, never published;
// - when the pending op already represents the current edge action, its
//   successful admission IS the edge admission: no second equivalent command
//   is built or admitted in the same feed (lastEdge advances from the
//   retained op);
// - pendingEdge tracks which machine action the pending op came from.
namespace kcd2vr::constraint_adapter60 {
struct Adapter {
    constraint60::Service service;
    constraint60::Action lastEdge=constraint60::Action::None;
    constraint_op60::Op pending{};
    constraint60::Action pendingEdge=constraint60::Action::None;
    std::uintptr_t sourceBody=0;std::uint64_t sourceGeneration=0; // the runtime owner's retained lease identity
    std::uint32_t nextSerial=1;
    std::uint64_t seenCreateTick=0; // DS-F51 (Codex): per-ACQUIRE command identity - edge state must reset on a new acquire
    static bool isEdge(constraint60::Action a) noexcept {
        return a==constraint60::Action::Create||a==constraint60::Action::Remove||a==constraint60::Action::Cleanup;
    }
    // DS-F48 (Codex 1429 fix): the pending must still belong to the CURRENT
    // lifecycle. Idle/Removed/Fault and a world mismatch cancel it. A
    // QUARANTINED machine still owns its resource: pending Remove/Cleanup of
    // the SAME world survive (the teardown must reach the executor) - only
    // pending Create/Drive are cancelled there.
    bool pendingCurrent() noexcept {
        if(pending.kind==constraint_op60::Kind::None)return false;
        const bool invalidated=service.stage==constraint60::Stage::Idle||service.stage==constraint60::Stage::Removed||
            service.stage==constraint60::Stage::Fault||pending.key.world!=service.world;
        if(invalidated){pending={};pendingEdge=constraint60::Action::None;return false;}
        if(service.stage==constraint60::Stage::Quarantined&&
           (pending.kind==constraint_op60::Kind::Create||pending.kind==constraint_op60::Kind::Drive)){
            pending={};pendingEdge=constraint60::Action::None;return false;
        }
        return true;
    }
    bool tryAdmit(const constraint_op60::Op& op) noexcept {
        if(!pendingCurrent()){ /* stale pending cancelled above */ }
        if(pending.kind!=constraint_op60::Kind::None){
            if(!constraint_op60::admit(pending))return false; // still busy/full: retry next step
            pending={};pendingEdge=constraint60::Action::None;
        }
        if(op.kind==constraint_op60::Kind::None)return true;
        if(!constraint_op60::admit(op)){pending=op;return false;}
        return true;
    }
    constraint60::Step feed(const constraint60::Snap& snap,std::uint64_t now) noexcept {
        // DS-F51: a NEW acquire (the machine's createTick moved) resets the
        // edge/pending state - a reused Adapter never leaks the previous
        // lifecycle's Remove into the next Create edge.
        if(service.stage==constraint60::Stage::Requested&&service.createTick&&service.createTick!=seenCreateTick){
            seenCreateTick=service.createTick;lastEdge=constraint60::Action::None;
            pending={};pendingEdge=constraint60::Action::None;
        }
        const auto st=service.step(snap,now);
        constraint_op60::Op op{};
        bool wantAdmit=false;
        if(st.action==constraint60::Action::Drive){
            op.kind=constraint_op60::Kind::Drive;op.tick=now;op.serial=nextSerial++;
            op.key={service.id,service.buddy,service.world,sourceBody,sourceGeneration};
            wantAdmit=true;
        }else if(isEdge(st.action)&&st.action!=lastEdge){
            // DS-F45: a retained op that already represents THIS edge advances
            // the edge on its own success - no second equivalent command.
            if(pending.kind!=constraint_op60::Kind::None&&pendingEdge==st.action&&
               pending.key.world==service.world){
                const bool ok=tryAdmit({});
                if(ok)lastEdge=st.action;
                return st;
            }
            switch(st.action){
            case constraint60::Action::Create:op.kind=constraint_op60::Kind::Create;break;
            case constraint60::Action::Remove:op.kind=constraint_op60::Kind::Remove;break;
            case constraint60::Action::Cleanup:op.kind=constraint_op60::Kind::Cleanup;break;
            default:op.kind=constraint_op60::Kind::None;break;}
            if(op.kind!=constraint_op60::Kind::None){
                op.tick=now;op.serial=nextSerial++;
                op.key={service.id,service.buddy,service.world,sourceBody,sourceGeneration};
                wantAdmit=true;
            }
        }
        const bool ok=(pending.kind!=constraint_op60::Kind::None||wantAdmit)?tryAdmit(op):true;
        if(wantAdmit&&ok&&op.kind!=constraint_op60::Kind::None&&pending.kind==constraint_op60::Kind::None){
            lastEdge=st.action;
        }else if(wantAdmit&&!ok&&pending.kind!=constraint_op60::Kind::None&&pendingEdge==constraint60::Action::None){
            pendingEdge=st.action; // the retained op came from THIS edge
        }
        return st;
    }
    // The executor reports an executed command. A refused Create feeds a
    // faulted status; the machine handles it per stage (Requested now
    // quarantines on a faulted status - v3.1).
    constraint60::Step executed(const constraint_op60::Op& op,bool ok,const constraint60::Snap& statusNow,std::uint64_t now) noexcept {
        if(op.kind==constraint_op60::Kind::Create&&!ok){
            constraint60::Snap faulted=statusNow;faulted.status.kind=constraint60::Status::Kind::Fault;
            faulted.status.world=statusNow.world; // DS-F45: the faulted status must qualify against OUR world
            return feed(faulted,now);
        }
        return feed(statusNow,now);
    }
};
}
