#pragma once
#include <array>
#include "dialogue_state59c10r2.h"
#include "diagnostics/npc_dialogue_capture_d52.h"

namespace kcd2vr::dialogue_participants_d52 {
enum class Reason : unsigned {NoScene,Unavailable,Changed,Observed};
inline const char* name(Reason r) noexcept {
    constexpr const char* names[]{"NO_NATIVE_SCENE","READ_UNAVAILABLE","OWNER_CHANGED","OBSERVED_SOUL_IDS"};return names[unsigned(r)];
}
struct Participant {std::uint64_t soul=0;unsigned char participating=0;};
struct Observation {
    std::uint64_t tick=0,session=0,playerSoul=0;
    std::uintptr_t instance=0,scene=0;unsigned total=0,count=0;
    Reason reason=Reason::NoScene;std::array<Participant,8> people{};
};
// Existing native view callback only, after exact-retail qualification. Read-only
// soul IDs are not engine entities. No NPC/twin pointer or controller is guessed.
template<class Read> Observation observe(const dialogue_state59c10r2::State& state,
    std::uintptr_t base,std::uint64_t session,Read&& read) noexcept {
    Observation out{};out.tick=state.tick;out.session=session;out.instance=state.instance;
    out.scene=state.membership.sceneD52;out.playerSoul=state.membership.playerSoulD52;
    if(!state.known||!state.membership.active||!state.player||!out.scene||!state.membership.managerD52)return out;
    out.reason=Reason::Unavailable;
    std::uintptr_t begin=0,end=0;
    if(!dialogue_membership19c2::range(out.scene,0x20,read,begin,end))return out;
    out.total=unsigned((end-begin)/0x20);out.count=out.total<unsigned(out.people.size())?out.total:unsigned(out.people.size());
    for(unsigned i=0;i<out.count;++i){const auto at=begin+0x20*i;
        if(!read(at,out.people[i].soul)||!read(at+0x18,out.people[i].participating))return out;}
    // Revalidate the proven native membership, selected entries and container
    // after sampling. Failure is evidence of unavailable/changed state, not a
    // stale participant list that can grant behavior.
    const auto current=dialogue_membership19c2::inspect(base,state.player,state.membership.managerD52,state.instance,read);
    out.reason=Reason::Changed;std::uintptr_t currentBegin=0,currentEnd=0;
    if(!current.known||!current.active||current.sceneD52!=out.scene||current.playerSoulD52!=out.playerSoul||
       !read(out.scene,currentBegin)||currentBegin!=begin||!read(out.scene+8,currentEnd)||currentEnd!=end)return out;
    for(unsigned i=0;i<out.count;++i){Participant p{};const auto at=begin+0x20*i;
        if(!read(at,p.soul)||!read(at+0x18,p.participating)||p.soul!=out.people[i].soul||p.participating!=out.people[i].participating)return out;}
    out.reason=Reason::Observed;return out;
}
// Includes the gate before any native participant read. Host fixtures execute
// this same producer adapter, not a separately reconstructed gate condition.
template<class Read,class Publish> bool capture(const dialogue_state59c10r2::State& state,
    std::uintptr_t base,std::uint64_t now,diagnostics::npc_dialogue_d52::Budget& budget,Read&& read,Publish&& publish) noexcept {
    using namespace diagnostics::capture168;
    if(!budget.allow(Domain::Interaction,now,250,1024))return false;
    const auto serial=gate.session[unsigned(Domain::Interaction)].load(std::memory_order_acquire);
    const auto sample=observe(state,base,serial,read);
    if(!active(Domain::Interaction,now)||serial!=gate.session[unsigned(Domain::Interaction)].load(std::memory_order_acquire))return false;
    return publish(sample);
}
}
