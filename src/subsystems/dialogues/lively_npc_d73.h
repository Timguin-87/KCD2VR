#pragma once
#include "core/snapshot_mailbox.h"
#include "dialogue_state59c10r2.h"
#include <atomic>
#include <cmath>
#include <cstdint>
namespace kcd2vr::lively_npc_d73 {
// Intent only. Native/VM work is done on the existing admitted callbacks.
inline std::atomic<std::uint64_t> mode{4}; // generation 1; NPC life and body gestures OFF
inline std::atomic<bool> ready{false},cleanup{false};
inline bool Enabled() noexcept {return (mode.load(std::memory_order_acquire)&1)!=0;}
inline bool ListeningGestures() noexcept {return (mode.load(std::memory_order_acquire)&2)!=0;}
inline void SetFlagD76(std::uint64_t bit,bool on) noexcept {
    auto old=mode.load(std::memory_order_acquire);
    for(;;){if(bool(old&bit)==on)return;
        const auto next=((old+4)&~std::uint64_t(3))|((old&3)&~bit)|(on?bit:0);
        if(mode.compare_exchange_weak(old,next,std::memory_order_acq_rel)){
            if(old&1)cleanup.store(true,std::memory_order_release);return;}}
}
inline void SetEnabled(bool on) noexcept {SetFlagD76(1,on);}
inline void SetListeningGestures(bool on) noexcept {SetFlagD76(2,on);}
// NPC presentation eligibility is separate from Henry's visible-body preference
// and from the native dialog input map. It never grants camera/player ownership.
inline bool DialogueScene(const dialogue_state59c10r2::State& s,std::uint64_t now) noexcept {
    using namespace dialogue_state59c10r2;
    return fresh(s,now)&&s.player&&s.instance&&s.membership.known&&s.membership.active&&
        s.membership.participant&&s.membership.listed&&s.membership.sceneD52&&
        s.membership.managerD52&&s.membership.playerSoulD52&&
        (s.camera==Camera::FirstPerson||s.camera==Camera::Dialogue);
}
struct Frame {
    std::uint64_t tick=0,generation=0;
    std::uintptr_t actor=0,scene=0;
    bool gameplay=false,dialogue=false,hmd=false;
    float x=0,y=0,z=0,fx=0,fy=0,fz=0;
    std::uintptr_t dialogueInstance=0,dialogueManager=0;std::uint64_t playerSoul=0;
};
inline core::SnapshotMailbox<Frame> frames;
inline bool Current(const Frame& f,std::uint64_t now,std::uint64_t expected) noexcept {
    return (expected&1)&&f.generation==expected&&f.actor&&f.tick&&now>=f.tick&&now-f.tick<=80&&
        f.hmd&&f.dialogue&&f.scene&&std::isfinite(f.x)&&std::isfinite(f.y)&&std::isfinite(f.z)&&
        std::isfinite(f.fx)&&std::isfinite(f.fy)&&std::isfinite(f.fz);
}
struct Speaker {std::uintptr_t hud=0,actor=0,scene=0;std::uint64_t generation=0,tick=0;bool henry=false,clearedD81=false;};
inline bool SpeakerCurrentD81(const Speaker& s,const Frame& f,std::uint64_t now) noexcept {
    return s.hud&&f.dialogue&&s.scene&&s.scene==f.scene&&s.actor==f.actor&&s.generation==f.generation&&
        s.tick&&now>=s.tick&&now-s.tick<=60000; // clear/next subtitle/scene change normally retires sooner
}
inline bool Speaking(const Speaker& s,const Frame& f,std::uint64_t now) noexcept {
    return SpeakerCurrentD81(s,f,now)&&s.henry&&!s.clearedD81;
}
// A current native NPC subtitle suppresses listening animations. A matched
// native Clear is positive quiet-phase evidence, unlike never seeing a speaker.
inline bool ListeningD81(const Speaker& s,const Frame& f,std::uint64_t now) noexcept {
    return SpeakerCurrentD81(s,f,now)&&(s.henry||s.clearedD81);
}
bool Prepare() noexcept; // Saved ON at qualified startup, or an explicit menu change.
void Publish(std::uintptr_t actor,std::uintptr_t scene,bool gameplay,bool dialogue,std::uint64_t now,
    const dialogue_state59c10r2::State* state=nullptr) noexcept;
}
