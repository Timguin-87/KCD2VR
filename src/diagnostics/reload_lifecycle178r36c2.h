#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstring>
#include "../core/snapshot_mailbox.h"

namespace kcd2vr::diagnostics::reload36c2 {
// T30: exact installed HAND_REMOVE observer is compatible with native callers.
// No callable-original claim: this is a MID hook, owned by the existing reload module.
inline constexpr std::uintptr_t kHandRemoveEntryT30=0x40B730;
inline constexpr unsigned char kHandRemovePristineT30[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x6C,0x24,0x10,0x48,0x89,0x74,0x24,0x18,0x57};
class HandRemoveEntryT30 {
    struct Receipt {std::uintptr_t base=0,owner=0;std::uint64_t serial=0;unsigned char patch[16]{};};
    core::SnapshotMailbox<Receipt> receipt_;
    std::atomic<std::uint64_t> active_{0},next_{0};
public:
    void clear() noexcept {active_.store(0,std::memory_order_release);}
    template<class Copy,class Owns> bool publish(std::uintptr_t base,std::uintptr_t owner,bool nativeVerified,Copy copy,Owns owns) noexcept {
        clear();Receipt r{};r.base=base;r.owner=owner;unsigned char after[16]{};
        if(!base||!owner||!nativeVerified||!copy(base+kHandRemoveEntryT30,r.patch,sizeof(r.patch))||
           !std::memcmp(r.patch,kHandRemovePristineT30,sizeof(r.patch)))return false;
        // Owner's idempotent enable verifies its EXACT backend patch, not any JMP.
        // Re-read ties the copied witness to that positive same-owner verification.
        if(!owns()||!copy(base+kHandRemoveEntryT30,after,sizeof(after))||
           std::memcmp(after,r.patch,sizeof(after)))return false;
        r.serial=next_.fetch_add(1,std::memory_order_acq_rel)+1;if(!r.serial||!receipt_.publish(r))return false;
        active_.store(r.serial,std::memory_order_release);return true;
    }
    template<class Copy> bool qualifies(std::uintptr_t base,std::uintptr_t owner,Copy copy) const noexcept {
        const auto active=active_.load(std::memory_order_acquire);Receipt r{};unsigned char current[16]{};
        return active&&base&&owner&&receipt_.read(r)&&r.serial==active&&r.base==base&&r.owner==owner&&
            copy(base+kHandRemoveEntryT30,current,sizeof(current))&&!std::memcmp(current,r.patch,sizeof(current))&&
            active_.load(std::memory_order_acquire)==active;
    }
};
enum class Kind { Dispatch, Construct, DestroyBegin, HandBegin, HandEnd,
    HandItems, Transfer, AmmoBind, NockBegin, NockEnd, Cocking, Finalize, HandCensus };
inline const char* name(Kind k) noexcept {
    constexpr const char* names[]{"DISPATCH", "ACTION_CONSTRUCT", "ACTION_DESTROY_BEGIN",
        "HAND_BEGIN", "HAND_END", "HAND_ITEMS", "TRANSFER", "AMMO_BIND",
        "NOCK_BEGIN", "NOCK_END", "COCKING", "ACTION_FINALIZE", "HAND_CENSUS"};
    return names[static_cast<unsigned>(k)];
}
inline const char* event_name(std::uint32_t hash) noexcept {
    switch(hash) {
    case 0x34857AFA:return "LoadReachedAmmo";
    case 0xEF667DEA:return "LoadReachedWeapon";
    case 0x8ABB129A:return "LoadReachedCocking";
    case 0xAFE0B3FF:return "SpawnLever";
    case 0x3D6870EB:return "DespawnLever";
    case 0xC14030E2:return "DetachAmmo";
    default:return "UNNAMED_HASH";
    }
}
// C37: return address of the native 18468BC(equipment,...) call that allocated
// an action. Names exact retail call sites only; any other value is OTHER.
inline const char* origin_name(std::uint32_t rva) noexcept {
    switch(rva) {
    case 0:return "UNJOINED";
    case 0x2AAA012:return "UPDATE_RETRY";
    case 0x1F24836:return "BOW_PRIMARY_PRESS";
    case 0x1F2469E:return "BOW_SECONDARY_PRESS";
    case 0x224A38C:return "TICK_RETRY";
    case 0x737A74:return "WEAPON_RETRY_LATCH";
    case 0x222CB40:return "WEAPON_LATCH_COLD";
    case 0x1F28766:return "WEAPON_AUTOLOAD";
    case 0x33A144B:return "RESULT_READER";
    case 0x33A2B6E:return "ONE_SHOT_CALLBACK";
    case 0x33A33A4:return "ATTEMPT_COUNTER";
    default:return "OTHER";
    }
}
struct Event {
    Kind kind{};
    std::uint64_t session=0,call=0,tick=0,weaponKey=0,ammoKey=0;
    std::uintptr_t action=0,actor=0,weapon=0,ammo=0,receiver=0,receiverVft=0,lever=0;
    std::uint32_t rva=0,hash=0,henry=0,handId=0,nockId=0;
    int side=-1,create=-1;
    std::uint8_t abort=255,finishNock=255,finishCock=255;
    bool handRead=false,nockRead=false,weaponRead=false,ammoRead=false;
    bool poseRead59c1=false;float matrix59c1[12]{};
    std::uint32_t origin=0;std::uintptr_t originStack=0; // C37 exact construct->18468BC stack join, 0=UNJOINED
};
struct Call59c1 {
    std::uint64_t session=0,call=0,weapon=0,ammo=0,first=0,last=0,mask=0;
    std::uintptr_t action=0,actor=0;std::uint32_t henry=0,handBefore=0,handAfter=0,nockBefore=0,nockAfter=0;
    bool removed=false,created=false,nockRead=false,identityConflict=false;
};
struct Ledger59c1 {
    std::array<Call59c1,128> calls{};std::uint64_t overflow=0;
    Call59c1* accept(const Event& e) noexcept {
        if(!e.session||!e.call||!e.action||!e.henry)return nullptr;
        Call59c1* row=nullptr;
        for(auto& x:calls)if(x.session==e.session&&x.call==e.call){row=&x;break;}
        if(!row)for(auto& x:calls)if(!x.session){row=&x;break;}
        if(!row){++overflow;return nullptr;}
        auto& x=*row;
        if(!x.session){x.session=e.session;x.call=e.call;x.action=e.action;x.actor=e.actor;x.henry=e.henry;x.first=e.tick;}
        if(x.action!=e.action||x.actor!=e.actor||x.henry!=e.henry||
           (e.weaponRead&&x.weapon&&x.weapon!=e.weaponKey)||(e.ammoRead&&x.ammo&&x.ammo!=e.ammoKey)){
            x.identityConflict=true;return nullptr;
        }
        x.last=e.tick;x.mask|=1ull<<unsigned(e.kind);
        if(e.weaponRead)x.weapon=e.weaponKey;if(e.ammoRead)x.ammo=e.ammoKey;
        if(e.kind==Kind::HandBegin&&e.handRead){x.handBefore=e.handId;x.created=e.create==1;x.removed=e.create==0;}
        if(e.kind==Kind::HandEnd&&e.handRead)x.handAfter=e.handId;
        if(e.kind==Kind::NockBegin&&e.nockRead)x.nockBefore=e.nockId;
        if(e.kind==Kind::NockEnd&&e.nockRead){x.nockRead=true;x.nockAfter=e.nockId;}
        return row;
    }
};
// Recorder-only action coverage. Exact native-call joins above remain the proof
// for transfers. This wider ledger names missing stages; it never confers item
// ownership or treats an animation/finalize/destructor event as a loaded weapon.
struct ActionCoverage59c10r3 {
    struct Row {
        std::uint64_t session=0,serial=0,first=0,last=0,mask=0,weapon=0,ammo=0;
        std::uintptr_t action=0,actor=0,lever=0,originStack=0;std::uint32_t origin=0;
        std::uint32_t henry=0,hand=0,nock=0;
        unsigned dispatchMask=0,qualifiedCalls=0,createRequests=0,removeRequests=0;
        bool constructed=false,ended=false,identityConflict=false,handRead=false,nockRead=false;
    };
    std::array<Row,32> rows{};std::uint64_t serial=0,overflow=0;
    Row* accept(const Event& e) noexcept {
        if(!e.session||!e.action||e.kind==Kind::HandCensus)return nullptr;
        Row* row=nullptr;
        for(auto& r:rows)if(r.session==e.session&&r.action==e.action&&!r.ended){row=&r;break;}
        if(e.kind==Kind::Construct&&row){row->ended=true;row=nullptr;}
        // Unknown candidate dispatches cannot open a Henry lifetime. Constructor
        // or a proven Henry call may; missing constructor stays visible.
        if(!row&&e.kind!=Kind::Construct&&!e.henry)return nullptr;
        if(!row){for(auto& r:rows)if(!r.session){row=&r;break;}}
        if(!row){++overflow;return nullptr;}
        auto& r=*row;
        if(!r.session){r.session=e.session;r.serial=++serial;r.action=e.action;r.first=e.tick;r.constructed=e.kind==Kind::Construct;
            if(r.constructed){r.origin=e.origin;r.originStack=e.originStack;}} // origin only from this lifetime's constructor
        if(e.henry){
            if((r.henry&&(r.henry!=e.henry||r.actor!=e.actor))||
               (e.weaponRead&&r.weapon&&r.weapon!=e.weaponKey)){
                r.identityConflict=true;r.ended=true;return row;
            }
            r.henry=e.henry;r.actor=e.actor;++r.qualifiedCalls;
            if(e.weaponRead)r.weapon=e.weaponKey;
            if(e.ammoRead)r.ammo=e.ammoKey;
            if(e.handRead){r.handRead=true;r.hand=e.handId;}
            if(e.nockRead){r.nockRead=true;r.nock=e.nockId;}
            if(e.kind==Kind::HandBegin){r.createRequests+=e.create==1;r.removeRequests+=e.create==0;}
        }
        r.last=e.tick;r.mask|=1ull<<unsigned(e.kind);
        if(e.lever)r.lever=e.lever;
        if(e.kind==Kind::Dispatch){
            switch(e.hash){case 0x34857AFA:r.dispatchMask|=1;break;
                case 0xEF667DEA:r.dispatchMask|=2;break;case 0x8ABB129A:r.dispatchMask|=4;break;
                case 0xAFE0B3FF:r.dispatchMask|=8;break;case 0x3D6870EB:r.dispatchMask|=16;break;
                case 0xC14030E2:r.dispatchMask|=32;break;default:r.dispatchMask|=64;break;}
        }
        if(e.kind==Kind::DestroyBegin)r.ended=true;
        return row;
    }
};
// No blocking or allocation in a native callback. Loss invalidates completeness.
template<std::size_t N=256> struct Queue {
    std::atomic_flag lock=ATOMIC_FLAG_INIT;
    std::atomic<std::uint64_t> dropped{0};
    std::array<Event,N> data{};
    std::size_t head=0,tail=0;
    bool push(const Event& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire)){++dropped;return false;}
        const auto next=(head+1)%N;const bool ok=next!=tail;
        if(ok){data[head]=e;head=next;}else ++dropped;
        lock.clear(std::memory_order_release);return ok;
    }
    bool pop(Event& e) noexcept {
        if(lock.test_and_set(std::memory_order_acquire))return false;
        const bool ok=head!=tail;if(ok){e=data[tail];tail=(tail+1)%N;}
        lock.clear(std::memory_order_release);return ok;
    }
};
// A call is joined only through its exact native stack frame and capture
// generation. No time-window identity, pointer chasing on the worker, or
// persistent object ownership follows from these diagnostic tokens.
template<std::size_t N=8> struct Scopes {
    struct Slot {std::uintptr_t stack=0;Event event{};};
    std::array<Slot,N> slots{};
    std::uint64_t overflow=0;
    void erase(std::uintptr_t stack) noexcept {
        for(auto& s:slots)if(s.stack==stack)s={};
    }
    bool put(std::uintptr_t stack,const Event& e) noexcept {
        erase(stack);
        for(auto& s:slots)if(!s.stack||s.event.session!=e.session){s={stack,e};return true;}
        ++overflow;return false;
    }
    Event* find(std::uintptr_t stack,std::uint64_t session) noexcept {
        for(auto& s:slots)if(s.stack==stack&&s.event.session==session)return &s.event;
        return nullptr;
    }
};
} // namespace
