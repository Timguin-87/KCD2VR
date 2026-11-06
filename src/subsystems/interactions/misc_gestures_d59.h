#pragma once
// One fixed native-script client for both opt-in misc gesture connections.
// Input owns recognition; the existing original-first script callback owns
// descriptor calls. Neither path owns camera, physics or native action results.
#include "pet_service_d59.h"
#include "quick_talk_service_d78.h"
#include "washing_d59.h"
#include "../../frik_pose_probe.h"
#include "../../interaction_alignment.h"
#include "../../ui_menu_input.h"
#include "../../diagnostics/capture168.h"
#include "../../diagnostic_trace.h"
#include <windows.h>
#include <atomic>
#include <cstring>

namespace kcd2vr::misc_gestures_d59 {
inline std::atomic<std::uint64_t> policy{1};
inline std::atomic<bool> alchemyTransfer{false};
struct Input {
    frik_pose_probe::MiscGestureBodyD59 body{};
    std::uint64_t tick=0,policy=0,inputTick=0;
    float grip[2]{},trigger[2]{};
    bool free[2]{},empty[2]{},permitted=false;
};
inline core::SnapshotMailbox<Input> current;
inline void SetPet(bool on) noexcept {
    if(petting_d59::Enabled()!=on)policy.fetch_add(1,std::memory_order_acq_rel);
    petting_d59::SetEnabled(on);
    interaction_alignment::SetDogGestureObservationD59R1(on);
}
inline void SetWash(bool on) noexcept {
    if(washing_d59::Enabled()!=on)policy.fetch_add(1,std::memory_order_acq_rel);
    washing_d59::SetEnabled(on);
}
inline bool Any() noexcept {return petting_d59::Enabled()||washing_d59::Enabled();}
inline bool CurrentBody(std::uintptr_t player,std::uint32_t id,std::uint64_t body,
    std::uint64_t tracking,std::uint64_t generation,unsigned mask,std::uint64_t now) noexcept {
    Input in{};frik_pose_probe::MiscGestureBodyD59 live{};
    if(!current.read(in)||!in.permitted||!petting_d59::Fresh(in.tick,now)||!petting_d59::Fresh(in.inputTick,now)||
       generation!=policy.load(std::memory_order_acquire)||in.policy!=generation||
       in.body.player!=player||in.body.playerId!=id||in.body.body!=body||in.body.tracking!=tracking||
       !frik_pose_probe::ReadMiscGestureBodyD59(live)||live.player!=player||live.playerId!=id||
       live.body!=body||live.tracking!=tracking)return false;
    const unsigned reserved=frik_pose_probe::PhysicalHandReservations114();
    const unsigned ui=ui_menu_input::WorldGripBlockedMask();
    for(unsigned side=0;side<2;++side)if(mask&(1u<<side)){
        if(!live.valid[side]||!in.free[side]||!in.empty[side]||!std::isfinite(in.grip[side])||
           !std::isfinite(in.trigger[side])||in.grip[side]<0||in.trigger[side]<0||in.grip[side]>=.35f||in.trigger[side]>=.35f||
           (reserved&(1u<<side))||(ui&(side?1u:2u))||frik_pose_probe::ExactHandItemState(side)!=0)return false;
    }
    return true;
}
inline bool PetCurrent(const petting_d59::Frame& f,int side,std::uint64_t now) noexcept {
    if(side<0||side>1||!CurrentBody(f.playerIdentity,f.player,f.body,f.tracking,f.policy,1u<<side,now))return false;
    interaction_alignment::DogHandCandidateD59R1 c{};
    return interaction_alignment::ReadDogHandCandidateD59R1(side,now,c)&&c.entity==f.hand[side].target&&
        c.owner==side&&c.sourcePlayer==f.playerIdentity&&c.sourceEpoch==f.body&&
        c.source.generation==f.hand[side].candidateGeneration&&c.source.context==f.hand[side].candidateContext&&
        petting_d59::Fresh(c.tick,now)&&petting_d59::Fresh(c.sourceTick,now)&&
        std::isfinite(c.distance)&&c.distance>=0&&c.distance<=.20f;
}
inline bool WashCurrent(const washing_d59::Frame& f,std::uint64_t now) noexcept {
    // Looking up at the face deliberately releases the selector. The service
    // retains/revalidates the exact native tub and smart-object descriptor.
    return CurrentBody(f.actor,f.player,f.body,f.tracking,f.policy,3u,now);
}
inline void Report(const char* message) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(diagnostics::capture168::gate.active(diagnostics::capture168::Domain::Interaction,now))
        diagnostic_trace::Write(message);
}
inline bool Wanted() noexcept {return petting_d59::Wanted()||washing_d59::Wanted()||quick_talk_target_d78::Wanted();}
inline void Service(const native_script_t2::Context& context) noexcept {
    petting_d59::Service(context);washing_d59::Service(context);quick_talk_target_d78::Service(context);
}
inline bool Ensure(std::uint64_t now) noexcept {
    // Called by the input owner only, with a selected gate. No new native hook
    // while OFF; retries are bounded rather than per-frame install attempts.
    static bool ready=false;static unsigned attempts=0;static std::uint64_t next=0;
    if(ready)return true;
    if(attempts>=3||now<next)return false;
    ++attempts;next=now+1000;
    petting_d59::SetCurrentReader(&PetCurrent);petting_d59::SetReporter(&Report);
    washing_d59::SetCurrentReader(&WashCurrent);
    ready=native_script_t2::Register(&Service,&Wanted)&&native_script_t2::Install();
    Report(ready?"D59 MISC script dispatcher READY gates=SEPARATE_DEFAULT_OFF":"D59 MISC script dispatcher UNAVAILABLE no native dispatch");
    return ready;
}
}
