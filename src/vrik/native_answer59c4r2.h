#pragma once
#include "contact_authority176.h"
#include <array>

namespace kcd2vr::answer59c4r2 {
// Copied before the native submit. These addresses are identity only after it
// returns; no answer consumer dereferences a borrowed action or native object.
struct Lineage { contact176::Ticket release{};std::uintptr_t action=0; };
// D568B0's sixth argument selects native application. Its zero/evaluation
// path also fills damage/block fields, but has not completed a contact.
enum class Effect { Unknown,Evaluation,Application };
struct Record {std::uintptr_t attacker=0,target=0;std::uint64_t tick=0;int kind=0;Lineage lineage{};Effect effect=Effect::Unknown;std::uint32_t attackerId=0,targetId=0;
    // C56 review (Claude): read-only copies of the native result for the
    // ANSWERED_BY_NPC receipt only - struck part (S_HitInfo+0x3C), surface
    // material (+0x38) and the producer caller RVA. They never decide a match,
    // never deny an application and never filter a record (a wood/shield clash
    // can be a legitimate completed block; the game decided, not the material).
    int part=-1,material=-1;std::uintptr_t callerRva=0;};
enum class Match {None,Same,Different,Unknown};
struct Result {Match match=Match::None;int kind=0;int part=-1,material=-1;std::uintptr_t callerRva=0;std::uint64_t tick=0;};
inline bool same(const contact176::Ticket& a,const contact176::Ticket& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.policy==b.policy&&a.player==b.player&&a.entity==b.entity&&
        a.tick==b.tick&&a.sample==b.sample&&a.epoch==b.epoch&&a.request==b.request&&a.key==b.key&&
        a.generation==b.generation&&a.gesture59c3==b.gesture59c3&&a.recovery59c3==b.recovery59c3;
}
inline bool sameMotion(const contact176::Ticket& a,const contact176::Ticket& b) noexcept {
    return a.gesture59c3&&a.gesture59c3==b.gesture59c3&&a.player==b.player&&a.entity==b.entity&&
        a.epoch==b.epoch&&a.request==b.request&&a.key==b.key&&a.generation==b.generation&&a.recovery59c3==b.recovery59c3;
}
class Ledger {
    struct Slot {core::SnapshotMailbox<Record> value;std::atomic<std::uint64_t> published{0};};
    std::array<Slot,16> slots_{};
    std::atomic<std::uint64_t> next_{0},ambiguousUntil_{0};
    void ambiguous(std::uint64_t until) noexcept {auto old=ambiguousUntil_.load();while(old<until&&!ambiguousUntil_.compare_exchange_weak(old,until)){} }
public:
    static constexpr std::uint64_t age=700;
    void note(const Record& r) noexcept {
        if(r.effect!=Effect::Application||!r.attacker||!r.target||!r.tick||r.kind<1||r.kind>5)return;
        auto& slot=slots_[next_.fetch_add(1,std::memory_order_relaxed)%slots_.size()];
        Record prior{};const auto old=slot.published.load(std::memory_order_acquire);
        if(old&&(!slot.value.read(prior)||prior.tick!=old))ambiguous(r.tick+age);
        else if(old&&(r.tick<old||r.tick-old<=age))ambiguous(old+age);
        // Mark the slot unpublished during replacement. A racing reader never
        // treats an incoherent or overwritten answer as permission for damage.
        slot.published.store(UINT64_MAX,std::memory_order_release);
        if(!slot.value.publish(r)){ambiguous(r.tick+age);slot.published.store(0,std::memory_order_release);return;}
        slot.published.store(r.tick,std::memory_order_release);
    }
    Result query(std::uintptr_t attacker,std::uintptr_t target,const contact176::Ticket& release,std::uint64_t now) const noexcept {
        Result result{};bool unknown=now<=ambiguousUntil_.load(std::memory_order_acquire);
        for(const auto& slot:slots_){
            const auto before=slot.published.load(std::memory_order_acquire);if(!before)continue;
            Record r{};
            if(before==UINT64_MAX||!slot.value.read(r)||slot.published.load(std::memory_order_acquire)!=before||r.tick!=before){unknown=true;continue;}
            if(!contact176::fresh(r.tick,now,age)||r.tick<release.tick||r.attacker!=attacker||r.target!=target)continue;
            if(!r.lineage.action||!r.lineage.release.serial){unknown=true;continue;}
            if(same(r.lineage.release,release))return {Match::Same,r.kind,r.part,r.material,r.callerRva,r.tick};
            // Different dispatch/sample metadata for the SAME physical gesture
            // is an ambiguous alias, never permission to resolve it twice.
            if(sameMotion(r.lineage.release,release)){unknown=true;continue;}
            result={Match::Different,0};
        }
        return unknown?Result{Match::Unknown,0}:result;
    }
};
} // namespace kcd2vr::answer59c4r2
