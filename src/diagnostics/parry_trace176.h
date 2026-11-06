#pragma once
// Completed native block input edges, not accepted parry/perfect-block results.
// Consumers use copied identities only and must revalidate their native hit args.
#include "../core/snapshot_mailbox.h"
#include <atomic>
#include <cstdint>
namespace kcd2vr::diagnostics::parry176 {
enum class Cause : unsigned { Initial, Motion, Manual, Assisted, NativeWindow, AutoWindup, WeaponContact }; // R46: Assisted = automatic re-press on the opponent's attack phase, not player motion
struct Edge {
    std::uint64_t sequence=0,tick=0,epoch=0,publication=0;
    std::uintptr_t player=0,actor=0,state=0,entity=0;
    std::uintptr_t opponentActor=0,opponentState=0,opponentEntity=0;
    std::uint32_t henryId=0,opponentId=0;
    int incomingSector=-1;
    Cause cause=Cause::Initial;
    bool manualHeld=false,releaseReturned=false,pressReturned=false;
};
// Low-cost actual incoming-result counters. Only the live-pair native Submit
// observer writes these; input delivery alone never increments them.
inline std::atomic<std::uint64_t> results55c2{0},blocks55c2{0},perfect55c2{0};
inline core::SnapshotMailbox<Edge> completed;
inline std::atomic<std::uint64_t> sequence{0},publication{1};
// Invalidation cannot fail because a mailbox writer is busy, nor can an older
// in-flight publication become authoritative after a mode/owner transition.
inline void Clear() noexcept {publication.fetch_add(1,std::memory_order_acq_rel);}
inline bool Publish(Edge edge,std::uint64_t expectedPublication,Edge* published=nullptr) noexcept {
    if(!edge.pressReturned||!edge.tick||!edge.actor||!edge.state||!edge.entity||!edge.henryId||
       publication.load(std::memory_order_acquire)!=expectedPublication)return false;
    edge.publication=expectedPublication;
    edge.sequence=sequence.fetch_add(1,std::memory_order_relaxed)+1;
    if(!completed.publish(edge))return false;
    if(published)*published=edge;
    return true;
}
inline bool Read(Edge& edge) noexcept {
    const auto current=publication.load(std::memory_order_acquire);
    if(!completed.read(edge)||!edge.sequence||!edge.pressReturned||edge.publication!=current||
       publication.load(std::memory_order_acquire)!=current){edge={};return false;}
    return true;
}
inline const char* Name(Cause cause) noexcept {
    return cause==Cause::WeaponContact?"WEAPON_CONTACT":cause==Cause::AutoWindup?"C4R1_AUTO_WINDUP":cause==Cause::NativeWindow?"NATIVE_WINDOW":cause==Cause::Motion?"MOTION":cause==Cause::Manual?"MANUAL":cause==Cause::Assisted?"ASSISTED_PHASE":"INITIAL";
}
}
