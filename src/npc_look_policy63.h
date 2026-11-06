#pragma once
#include <atomic>
#include "dialogue_state59c10r2.h"

namespace kcd2vr::npc_look_policy63 {
// D52: one atomic mode+generation prevents off/on during a callback from
// lending permission back to that older request. Native really means no write.
inline std::atomic<std::uint64_t> modeD52{3}; // generation 1, Follow HMD (legacy default)
inline std::atomic<bool> hookReadyD52{false},sceneAdmittedD52{false};
inline std::atomic<std::uint64_t> sceneTickD52{0};
inline bool followHmd() noexcept {return (modeD52.load(std::memory_order_acquire)&1)!=0;}
inline void setFollowHmd(bool enabled) noexcept {
    auto before=modeD52.load(std::memory_order_acquire);
    for(;;){if(bool(before&1)==enabled)return;
        const auto after=((before+2)&~std::uint64_t(1))|std::uint64_t(enabled);
        if(modeD52.compare_exchange_weak(before,after,std::memory_order_acq_rel))return;}
}
// This witness is independent of the preference and of F5 capture. It describes
// a confirmed conversation, never a worker-renewed timer or global gameplay flag.
struct Owner {
    std::uint64_t tick=0;
    std::uintptr_t entity=0,player=0,instance=0,scene=0;
    std::uint64_t playerSoul=0;
};
enum class Admission : unsigned {Unavailable,CameraProtected,InputUnknown,Unclassified,InputAdmitted,SpeechContinued};
inline const char* name(Admission r) noexcept {
    constexpr const char* names[]{"SCENE_UNAVAILABLE","CAMERA_PROTECTED","INPUT_UNKNOWN",
        "SCENE_NOT_ADMITTED","INPUT_ADMITTED","SPEECH_CONTINUED"};return names[unsigned(r)];
}
inline bool fresh(const Owner& o,std::uint64_t now) noexcept {
    return o.entity&&o.player&&o.instance&&o.scene&&o.playerSoul&&dialogue_state59c10r2::age(o.tick,now,80);
}
inline bool sameScene(const Owner& a,const Owner& b) noexcept {
    return a.entity==b.entity&&a.player==b.player&&a.instance==b.instance&&a.scene==b.scene&&a.playerSoul==b.playerSoul;
}
inline bool same(const Owner& a,const Owner& b) noexcept {return a.tick==b.tick&&sameScene(a,b);}
struct Result {Owner owner{};Admission reason=Admission::Unavailable;};
inline Result capture(const dialogue_state59c10r2::State& s,const Owner& previous,std::uint64_t now) noexcept {
    Result out{};
    if(!dialogue_state59c10r2::fresh(s,now)||!s.player||!s.instance||
       !s.membership.known||!s.membership.active||!s.membership.sceneD52||!s.membership.playerSoulD52)return out;
    if(s.camera!=dialogue_state59c10r2::Camera::FirstPerson&&s.camera!=dialogue_state59c10r2::Camera::Dialogue){
        out.reason=Admission::CameraProtected;return out;}
    if(!s.inputOwner.known){out.reason=Admission::InputUnknown;return out;}
    const Owner next{s.tick,s.entity,s.player,s.instance,s.membership.sceneD52,s.membership.playerSoulD52};
    // Only an input-admitted conversation can continue across a disabled speech
    // phase. Membership alone also covers barks; do not broaden their permission.
    if(s.inputOwner.enabled){out.owner=next;out.reason=Admission::InputAdmitted;return out;}
    if(fresh(previous,now)&&sameScene(previous,next)&&s.tick>=previous.tick){
        out.owner=next;out.reason=Admission::SpeechContinued;return out;}
    out.reason=Admission::Unclassified;return out;
}
}
