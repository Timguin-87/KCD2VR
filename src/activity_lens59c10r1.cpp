#include "activity_lens59c10r1.h"
#include "activity_input163.h"
#include "dialogue_presentation19c2.h"
#include "frik_pose_probe.h"
#include <intrin.h>
#include "core/snapshot_mailbox.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "dialogue_participants_d52.h"
#include "lb_prompt_x_r153.h" // R156: hold-to-look prompt offered = its action map enabled (seen here, used by the X press)
#include "close_tutorial_r156.h" // R156: tutorial screen open = its close_tutorial action map enabled (seen here, used by the B press)
#include <DetourModKit/hook.hpp>
#include <windows.h>
#include <optional>
#include <cstring>
#include <cstdio>

namespace kcd2vr::activity_lens59c10r1 { namespace {
using Fn=void(__fastcall*)(void*,void*);
Fn original=nullptr;std::optional<DetourModKit::hook::Hook> hook;
std::uintptr_t base=0;core::SnapshotMailbox<Policy> policy;
core::SnapshotMailbox<dialogue_state59c10r2::State> dialogue;
core::SnapshotMailbox<dialogue_participants_d52::Observation> dialoguePeopleD52;
diagnostics::npc_dialogue_d52::Budget dialoguePeopleBudgetD52;
std::atomic<std::uint64_t> viewObservations19c2{0},selectorObservations19c2{0},selectorOverrides19c2{0},selectorNativeTrue19c2{0};
core::SnapshotMailbox<dialogue_presentation19c2::Request> dialogueRequest19c2;
std::atomic<bool> selectorReady19c2{false};
using SelectorFn19c2=bool(__fastcall*)(void*);
SelectorFn19c2 selectorOriginal19c2=nullptr;std::optional<DetourModKit::hook::Hook> selectorHook19c2;
core::SnapshotMailbox<activity_eye59c19::State> eyeState19;
core::SnapshotMailbox<EyeReceipt59c19> eyeReceipts19;
core::SnapshotMailbox<ForgeCamera> forgeCamera;
core::SnapshotMailbox<ForgeCamera> diceCameraD80;
std::atomic<bool> forgeCameraBlocked{true};
std::atomic<bool> forgeObserverReady{false};
ExitHoldR89 exitHoldR89; // R89 dice: game-thread View() only
std::atomic<std::uint64_t> exitHoldWritesR89{0};
enum class Result : unsigned { Applied, Unchanged, PolicyUnavailable, PolicyInactive, PolicyStale,
    OwnershipRefused, OutputUnavailable, InvalidNativeFov, WriteFailed };
constexpr const char* resultName[]{"APPLIED","UNCHANGED","POLICY_UNAVAILABLE","POLICY_INACTIVE","POLICY_STALE",
    "OWNERSHIP_REFUSED","OUTPUT_UNAVAILABLE","INVALID_NATIVE_FOV","WRITE_FAILED"};
struct Receipt {std::uint64_t tick=0,policyTick=0;float native=0,written=0,requested=0;
    Evidence evidence{};Result result=Result::OutputUnavailable;};
core::SnapshotMailbox<Receipt> receipts;
std::array<std::atomic<std::uint64_t>,9> observed{};
template<class T>bool Read(std::uintptr_t address,T& value) noexcept {
    __try {value=*reinterpret_cast<const T*>(address);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
bool WriteFov(void* output,float value) noexcept {
    __try {*reinterpret_cast<float*>(static_cast<unsigned char*>(output)+0x30)=value;return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
Evidence Ownership(void* self,const std::array<std::uintptr_t,4>& active,std::uintptr_t forgeBuilder,std::uintptr_t dice=0) noexcept {
    return qualify(base,reinterpret_cast<std::uintptr_t>(self),active,
        [](std::uintptr_t address,auto& value){return Read(address,value);},forgeBuilder,dice);
}
bool ForgeWriteCurrent(std::uintptr_t instance,std::uint64_t lifetime) noexcept {
    const auto& w=activity_input163::witness175;
    return instance&&forgeObserverReady.load(std::memory_order_acquire)&&w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire)&&
        lifetime==w.lifetime175.load(std::memory_order_acquire)&&instance==w.forgeBuilder.load(std::memory_order_acquire)&&
        forgeObserverReady.load(std::memory_order_acquire)&&w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire)&&lifetime==w.lifetime175.load(std::memory_order_acquire);
}
void __fastcall View(void* self,void* output) {
    original(self,output); // native pose, zoom and gameplay side effects run once
    forgeCameraBlocked.store(true,std::memory_order_release);
    Policy p{};const auto now=std::uint64_t(GetTickCount64());
    if(!retail_build112::Ready(base))return;
    using namespace diagnostics::capture168;
    const bool recording=active(Domain::Hud,now)||active(Domain::Interaction,now);
    // Observer is independent of activity zoom options. No worker-thread native
    // pointer reads, extra detour, or guessed dialogue-context CVar.
    const auto dialog=dialogue_state59c10r2::observe(base,reinterpret_cast<std::uintptr_t>(self),now,
        [](std::uintptr_t address,auto& value){return Read(address,value);},recording);
    dialogue.publish(dialog);
    {// R156: the hold-to-look / watch prompt is offered exactly while the game enables its "infinite_hold_interaction_init"
     // action map (REVIEW155_lbx2: enabled at 3 of 75 X presses, the three at the prompt). the same verified named-map walk
     // as the dialog input owner above, bounded SEH reads, no native call. lb_prompt_x_r153.h takes the tick as "listed"
     // BEFORE the first X tap (R155 needed one wasted tap to see the map through the OnAction detour).
     const auto hold=dialogue_input_owner19c3::inspectMap(base,"infinite_hold_interaction_init",
         [](std::uintptr_t address,auto& value){return Read(address,value);});
     if(hold.known&&hold.enabled)kcd2vr::lb_prompt_x_r153::holdMapEnabledMs.store(now,std::memory_order_release);}
    {// R156: a full-screen tutorial is open exactly while the game enables its "close_tutorial" action map (xi_back only,
     // no VR button sends it - Miki could not close those screens). the same verified named-map walk as the dialog input
     // owner, bounded SEH reads, no native call; the pad premap turns the right B into a held xi_back while this is fresh.
     const auto tut=dialogue_input_owner19c3::inspectMap(base,kcd2vr::close_tutorial_r156::kMapName,
         [](std::uintptr_t address,auto& value){return Read(address,value);});
     if(tut.known&&tut.enabled)kcd2vr::close_tutorial_r156::mapEnabledMs.store(now,std::memory_order_release);}
    (void)dialogue_participants_d52::capture(dialog,base,now,dialoguePeopleBudgetD52,
        [](std::uintptr_t address,auto& value){return Read(address,value);},
        [](const auto& people){return dialoguePeopleD52.publish(people);});
    // The native FirstPerson provider has just consumed this original player's
    // absolute pose. A culled/unskinned body must not be the camera's only head
    // producer. This copies that same native pose; no pose/visibility write.
    dialogue_presentation19c2::Request cameraIntent28{};
    if(dialog.active&&dialog.camera==dialogue_state59c10r2::Camera::FirstPerson&&
       dialogueRequest19c2.read(cameraIntent28)&&dialogue_presentation19c2::requested(cameraIntent28,now))
        frik_pose_probe::ObserveNativeDialogueHead28(dialog,now);
    viewObservations19c2.fetch_add(1,std::memory_order_relaxed);
    const bool havePolicy=policy.read(p);
    // Copy lifecycle state from existing native start/stop callbacks. Inactive
    // gameplay uses the already-observed first-person witness: no extra camera
    // pointer chain or calls. Activity ownership reuses the existing qualifier.
    const auto& w=activity_input163::witness175;
    activity_eye59c19::State view{};view.tick=now;view.entity=dialog.entity;
    const auto transition=w.transitionDepth175.load(std::memory_order_acquire);
    const auto life=w.lifetime175.load(std::memory_order_acquire);
    const bool ready=w.ready.load(std::memory_order_acquire);
    for(unsigned i=0;i<view.instances.size();++i){
        view.instances[i]=w.active[i].load(std::memory_order_acquire);
        if(view.instances[i]){
            view.kindMask|=1u<<i;const auto start=w.startTick175[i].load(std::memory_order_acquire);
            if(start&&(!view.enteredAt||start<view.enteredAt))view.enteredAt=start;
        }
    }
    const auto dice=w.dice.load(std::memory_order_acquire);
    if(dice){view.kindMask|=32u;const auto start=w.diceStart.load(std::memory_order_acquire);
        if(start&&(!view.enteredAt||start<view.enteredAt))view.enteredAt=start;}
    const auto forgeBuilder=w.forgeBuilder.load(std::memory_order_acquire);
    if(forgeBuilder){
        view.kindMask|=16u;const auto start=w.forgeBuilderStart.load(std::memory_order_acquire);
        if(start&&(!view.enteredAt||start<view.enteredAt))view.enteredAt=start;
    }
    view.lifetime=w.lifetime175.load(std::memory_order_acquire);
    view.known=ready&&dialog.known&&!transition&&!w.transitionDepth175.load(std::memory_order_acquire)&&life==view.lifetime;
    view.active=view.kindMask!=0;
    const bool eyePolicy=havePolicy&&p.eye&&dialogue_state59c10r2::age(p.tick,now,80)&&
        std::isfinite(p.radians)&&p.radians>=1.f&&p.radians<=2.45f;
    Evidence evidence{};
    // Construction camera observation must also run when Theater currently
    // suppresses lens policy, otherwise the native-owner exit is circular.
    if((forgeBuilder||dice||recording||eyePolicy||fresh(p,now))&&view.active)
        evidence=Ownership(self,view.instances,forgeBuilder,dice);
    if(forgeObserverReady.load(std::memory_order_acquire)&&forgeBuilder&&view.known&&!dialog.active&&evidence.kindMask==16u&&evidence.camera!=Owner::None&&
       dialog.player==reinterpret_cast<std::uintptr_t>(self)-0x40&&
       life==w.lifetime175.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire)&&
       w.ready.load(std::memory_order_acquire)&&forgeBuilder==w.forgeBuilder.load(std::memory_order_acquire)){
        const bool published=forgeCamera.publish({now,life,forgeBuilder,dialog.player,evidence.camera});
        forgeCameraBlocked.store(!published,std::memory_order_release);
    }
    if(dice&&view.known&&!dialog.active&&evidence.kindMask==32u&&evidence.camera!=Owner::None&&
       dialog.player==reinterpret_cast<std::uintptr_t>(self)-0x40&&life==w.lifetime175.load(std::memory_order_acquire)&&
       dice==w.dice.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire))
        (void)diceCameraD80.publish({now,life,dice,dialog.player,evidence.camera});
    view.neutral=view.known&&!view.active&&!dialog.active&&
        dialog.camera==dialogue_state59c10r2::Camera::FirstPerson&&eyePolicy;
    view.eligible=view.known&&view.active&&!dialog.active&&eyePolicy&&
        evidence.camera!=Owner::None&&activity_eye59c19::eligibleKindsD81(evidence.kindMask);
    eyeState19.publish(view);
    {   // Retain only this qualified dice visit, never any other minigame's lens.
        const bool exiting=!view.active||(view.kindMask==32u&&evidence.refusal==Refusal::ActivityInactive);
        bool otherCameraR95=false;
        const Owner camera=(exitHoldR89.armed&&exiting&&view.known&&eyePolicy&&!dialog.active)?
            playerCamera(base,[](std::uintptr_t address,auto& value){return Read(address,value);},&otherCameraR95):Owner::None;
        float native=0;
        const DiceExitFrameR91 diceExit{view.known,havePolicy&&p.eye,dice,dialog.player,
            view.lifetime,view.kindMask,evidence,camera,havePolicy,eyePolicy,dialog.active||otherCameraR95};
        if(exitHoldStep(exitHoldR89,diceExit)&&output&&
           Read(reinterpret_cast<std::uintptr_t>(output)+0x30,native)&&std::isfinite(native)&&native>.1f&&native<3.1f){
            // The retained visit is identity only. A fresh policy and unchanged
            // lifecycle must still authorize this final output write.
            // Reobserve the existing native dialogue/player and camera chains:
            // an output read can yield to a protected owner without changing dice.
            const auto finalDialogR95=dialogue_state59c10r2::observe(base,reinterpret_cast<std::uintptr_t>(self),
                std::uint64_t(GetTickCount64()),[](std::uintptr_t address,auto& value){return Read(address,value);});
            bool finalOtherCameraR95=false;
            const auto finalCameraR95=playerCamera(base,[](std::uintptr_t address,auto& value){return Read(address,value);},&finalOtherCameraR95);
            if((finalDialogR95.known&&(finalDialogR95.active||finalDialogR95.player!=dialog.player||finalDialogR95.entity!=dialog.entity))||
               finalOtherCameraR95||finalCameraR95==Owner::FirstPerson){exitHoldR89={};return;}
            if(!finalDialogR95.known||(finalDialogR95.membership.active&&!finalDialogR95.inputOwner.known)||finalCameraR95!=Owner::Minigame)return;
            Policy finalPolicyR95{};const bool finalPolicyKnownR95=policy.read(finalPolicyR95);
            if(finalPolicyKnownR95&&!finalPolicyR95.eye){exitHoldR89={};return;}
            if(!finalPolicyKnownR95||finalPolicyR95.tick!=p.tick||
               finalPolicyR95.radians!=p.radians||
               !dialogue_state59c10r2::age(finalPolicyR95.tick,std::uint64_t(GetTickCount64()),80)||
               !w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
               w.lifetime175.load(std::memory_order_acquire)!=view.lifetime||w.dice.load(std::memory_order_acquire)!=dice)return;
            if(native!=p.radians&&WriteFov(output,p.radians)){
                exitHoldWritesR89.fetch_add(1,std::memory_order_relaxed);
                if(recording){
                    observed[unsigned(Result::Applied)].fetch_add(1,std::memory_order_relaxed);
                    receipts.publish({now,p.tick,native,p.radians,p.radians,
                        Evidence{Owner::Minigame,0,Refusal::NoActivity,0,0x408EB60},Result::Applied});
                }
            }
            return;
        }
    }
    // Diagnose the formerly silent rejection paths only under F2/F5. Normal
    // disabled gameplay does not acquire additional activity/camera pointers.
    if(!recording&&(!output||!havePolicy||!fresh(p,now)))return;
    if(!view.active)evidence={Owner::None,0,Refusal::NoActivity};
    float native=0;
    const bool haveOutput=output&&Read(reinterpret_cast<std::uintptr_t>(output)+0x30,native);
    float written=native;Result result=Result::Unchanged;
    if(!haveOutput)result=Result::OutputUnavailable;
    else if(!havePolicy)result=Result::PolicyUnavailable;
    else if(!p.immersive)result=Result::PolicyInactive;
    else if(!fresh(p,now))result=Result::PolicyStale;
    else if(evidence.camera==Owner::None)result=Result::OwnershipRefused;
    else if(!std::isfinite(native)||native<=.1f||native>=3.1f)result=Result::InvalidNativeFov;
    else {
        const auto selected=select(p,now,native,evidence);
        if(std::isfinite(selected)&&selected!=native){
            // The native read qualifier is not a lifetime lease. A stop,
            // replacement or shutdown during those reads must also veto the
            // final Forge-only scalar write, while retaining original output.
            if(forgeBuilder&&(!view.known||!ForgeWriteCurrent(forgeBuilder,life))){
                evidence.camera=Owner::None;evidence.refusal=Refusal::ActivityInactive;
                forgeCameraBlocked.store(true,std::memory_order_release);result=Result::OwnershipRefused;
            }
            else if(dice&&(!view.known||!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
                    w.dice.load(std::memory_order_acquire)!=dice||w.lifetime175.load(std::memory_order_acquire)!=life)){
                evidence.refusal=Refusal::ActivityInactive;result=Result::OwnershipRefused;
            }
            else if(WriteFov(output,selected)){written=selected;result=Result::Applied;}
            else result=Result::WriteFailed;
        }
    }
    // No formatting, disk writes, native polling, XR queries or render locks here.
    if(recording){
        observed[unsigned(result)].fetch_add(1,std::memory_order_relaxed);
        receipts.publish({now,p.tick,native,written,p.radians,evidence,result});
    }
}
bool __fastcall DialogueCameraCandidate19c2(void* self) noexcept {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const bool native=selectorOriginal19c2(self); // exactly one native predicate call
    // C20 reviewed: a native false is already the desired result. UpdateView
    // publishes semantic dialogue state independently; no extra reads here.
    if(!native)return false;
    if(!selectorReady19c2.load(std::memory_order_acquire)||caller!=base+0x9E5991||!retail_build112::Ready(base)||
       !dialogue_presentation19c2::selectorOwner(base,reinterpret_cast<std::uintptr_t>(self),
           [](std::uintptr_t address,auto& value){return Read(address,value);}))return native;
    dialogue_state59c10r2::State seed{};
    if(!dialogue.read(seed)||!seed.player)return native;
    const auto now=std::uint64_t(GetTickCount64());
    const bool capturePresentation=diagnostics::capture168::active(diagnostics::capture168::Domain::Hud,now)||
        diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now);
    // Old sample is an address seed ONLY. Re-read exact player/backlinks, soul,
    // participants, liveness, current instance and camera at this native callback.
    const auto current=dialogue_state59c10r2::observe(base,seed.player+0x40,now,
        [](std::uintptr_t address,auto& value){return Read(address,value);},capturePresentation);
    dialogue.publish(current);selectorObservations19c2.fetch_add(1,std::memory_order_relaxed);
    if(native)selectorNativeTrue19c2.fetch_add(1,std::memory_order_relaxed);
    dialogue_presentation19c2::Request request{};(void)dialogueRequest19c2.read(request);
    if(!dialogue_presentation19c2::chooseFirstPerson(request,current,now)||
       !dialogue_presentation19c2::selectorOwner(base,reinterpret_cast<std::uintptr_t>(self),
           [](std::uintptr_t address,auto& value){return Read(address,value);}))return native;
    if(native)selectorOverrides19c2.fetch_add(1,std::memory_order_relaxed);
    // Decline this Dialog camera candidate. The native selector chooses its
    // remaining providers/actual FirstPerson camera; no manager/root field write.
    return false;
}
bool Pins() noexcept {
    __try {
        struct Pin {std::uintptr_t rva;const char* bytes;std::size_t count;};
        const Pin pins[]{
            {0x7F22FC,"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x48\x8B\xFA\x48\x8B\xD9",16},
            {0x7F2314,"\x48\x8B\x48\x38\xE8\x5F\x00\x00\x00",9},
            {0x7F2358,"\xE8\xDF\xEC\x86\x00",5},
            {0x7F236C,"\xE8\xA3\xAD\xED\xFF",5},
            {0x7F2C56,"\xF3\x0F\x11\x5E\x30",5},
            {0x10610CE,"\xF3\x0F\x11\x46\x30",5},
            {0x6CD15A,"\xF3\x0F\x11\x47\x30",5},
            {0x9155F1,"\x48\x8B\x05\x90\x7D\xB8\x04",7},
            {0x197CB94,"\x48\x8B\x81\x90\x00\x00\x00\x48\x8D\x48\x18\x48\xF7\xD8\x48\x1B\xC0\x48\x23\xC1\xC3",21},
            {0x311C9F6,"\x48\x8B\x88\x50\x01\x00\x00\x48\x8B\x49\x10\xE8\x8E\x01\x86\xFE",16},
            {0x3BDEE4,"\x83\xB9\x60\x02\x00\x00\x17\x0F\x94\xC0\xC3",11},
            {0x1435810,"\x48\x8B\x81\x40\x01\x00\x00\x8B\x80\xBC\x00\x00\x00\xC1\xE8\x03\x24\x01\xC3",19},
            {0x1A749E0,"\x48\x8D\x41\x30\xC3",5},
            {0xA10AF4,"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x30\x48\x8B\xF9\x48\x8D\x4C",16},
            {0x3F7C40,"\x48\x8B\x01\x4D\x8B\xD8\xC7\x42\x08\x00\x00\x00\x00",13},
            {0x3F7C5A,"\x48\x8B\x41\x20\x4D\x8B\x0B\x4C\x2B\xC8",10},
            {0x1A88910,"\x0F\xB6\x41\x09\xC3",5},
            {0x85AE44,"\x40\x88\x77\x09",4},
            {0x9E5F4C,"\x48\x89\x5C\x24\x08\x57\x48\x83\xEC\x20\x33\xDB\x48\x8B\xF9\x38",16},
            {0x9E5981,"\xE8\x22\x04\x00\x00\x48\x8B\x08\x48\x8B\x11\x48\x8B\xC8\xFF\xD2",16},
            {0x9E59C9,"\x48\x8D\xBB\xE8\x01\x00\x00",7},
            {0x7F128F,"\xF3\x0F\x10\x5F\x44",5}};
        for(const auto& pin:pins)if(std::memcmp(reinterpret_cast<void*>(base+pin.rva),pin.bytes,pin.count))return false;
        return *reinterpret_cast<const std::uintptr_t*>(base+0x4048018+0x98)==base+0xA10AF4&&
            *reinterpret_cast<const std::uintptr_t*>(base+0x4048530+0xD0)==base+0x1A88910&&
            *reinterpret_cast<const std::uintptr_t*>(base+0x4048530+0xC8)==base+0x85ADF4&&
            *reinterpret_cast<const std::uintptr_t*>(base+0x3E63CA8)==base+0x9E5F4C&&
            *reinterpret_cast<const std::uintptr_t*>(base+0x3A81030)==base+0x7F22FC&&
            *reinterpret_cast<const std::uintptr_t*>(base+0x3A81038)==base+0x3B6E80;
    }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
}
bool DiceCameraCurrentD80(std::uint64_t now) noexcept {
    ForgeCamera observed{};const auto& w=activity_input163::witness175;
    return diceCameraD80.read(observed)&&forgeCurrent(observed,now,w.dice.load(std::memory_order_acquire),
        w.lifetime175.load(std::memory_order_acquire),w.ready.load(std::memory_order_acquire),w.transitionDepth175.load(std::memory_order_acquire)!=0);
}
bool ForgeCameraCurrent(std::uint64_t now) noexcept {
    if(!forgeObserverReady.load(std::memory_order_acquire)||forgeCameraBlocked.load(std::memory_order_acquire))return false;
    ForgeCamera observed{};if(!forgeCamera.read(observed))return false;
    const auto& w=activity_input163::witness175;
    const auto life=w.lifetime175.load(std::memory_order_acquire);
    const bool current=forgeCurrent(observed,now,w.forgeBuilder.load(std::memory_order_acquire),life,
        w.ready.load(std::memory_order_acquire),w.transitionDepth175.load(std::memory_order_acquire)!=0);
    return current&&forgeObserverReady.load(std::memory_order_acquire)&&!forgeCameraBlocked.load(std::memory_order_acquire)&&
        life==w.lifetime175.load(std::memory_order_acquire)&&w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire);
}
void RequestDialogueFirstPerson19c2(bool wanted,std::uint64_t now) noexcept {
    dialogueRequest19c2.publish({now,wanted});
}
bool Install() noexcept {
    if(hook||selectorHook19c2)return selectorReady19c2.load(std::memory_order_acquire);
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));
    if(!base||!retail_build112::Verify(base)||!Pins())return false;
    DetourModKit::hook::Options options{};options.fail_if_already_hooked=true;
    auto h=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{
        "c10r1_activity_view_lens",DetourModKit::hook::Target{DetourModKit::Address{base+0x7F22FC}},options},&View);
    if(!h)return false;hook=std::move(*h);original=hook->original<Fn>();
    if(!original||!hook->enable()){
        if(!dialogue_presentation19c2::releaseHook(hook,original))
            diagnostic_trace::Write("C19C2 DIALOGUE_HOOK_FAULT stage=VIEW_ENABLE_ROLLBACK trampoline=RETAINED requested=OFF");
        return false;
    }
    auto candidate=DetourModKit::hook::inline_at(DetourModKit::hook::InlineRequest{
        "c19c2_dialogue_camera_candidate",DetourModKit::hook::Target{DetourModKit::Address{base+0x9E5F4C}},options},&DialogueCameraCandidate19c2);
    if(candidate){selectorHook19c2=std::move(*candidate);selectorOriginal19c2=selectorHook19c2->original<SelectorFn19c2>();}
    if(!selectorOriginal19c2||!selectorHook19c2->enable()){
        dialogueRequest19c2.publish({});
        const bool candidateOff=dialogue_presentation19c2::releaseHook(selectorHook19c2,selectorOriginal19c2);
        const bool viewOff=dialogue_presentation19c2::releaseHook(hook,original);
        if(!candidateOff||!viewOff)diagnostic_trace::Write("C19C2 DIALOGUE_HOOK_FAULT stage=SELECTOR_ENABLE_ROLLBACK trampoline=RETAINED requested=OFF");
        return false;
    }
    selectorReady19c2.store(true,std::memory_order_release);
    forgeObserverReady.store(true,std::memory_order_release);
    diagnostic_trace::Write("C19C2 HUD_DIALOGUE_SELECTOR installed predicate=9E5F4C caller=9E5991 cameraSelection=NATIVE_9E58CC defaultFirstPerson=1E8 writes=NO_MANAGER_OR_HEAD_FIELDS");
    diagnostic_trace::Write("KCD2VR BUILD ACTIVITY_LENS installed callback=7F22FC scope=OWNED_ALCHEMY_READING_SHARPENING_FORGEBUILDER dialogueWitness=GET_PLAYER_DIALOG_197CB94 frustumHook=NONE");
    return true;
}
void Uninstall() noexcept {
    forgeObserverReady.store(false,std::memory_order_release);
    forgeCameraBlocked.store(true,std::memory_order_release);
    selectorReady19c2.store(false,std::memory_order_release);dialogueRequest19c2.publish({});
    policy.publish({});dialogue.publish({});eyeState19.publish({});eyeReceipts19.publish({});
    const bool candidateOff=dialogue_presentation19c2::releaseHook(selectorHook19c2,selectorOriginal19c2);
    const bool viewOff=dialogue_presentation19c2::releaseHook(hook,original);
    if(!candidateOff||!viewOff)diagnostic_trace::Write("C19C2 DIALOGUE_HOOK_FAULT stage=UNINSTALL trampoline=RETAINED requested=OFF retryRequired=1");
}

dialogue_state59c10r2::State Dialogue(std::uint64_t now) noexcept {
    dialogue_state59c10r2::State out{};
    if(!dialogue.read(out)||!dialogue_state59c10r2::fresh(out,now))return {};
    return out;
}
void Update(bool immersive,float degrees,std::uint64_t now) noexcept {
    const auto& w=activity_input163::witness175;
    const bool active=w.ready.load(std::memory_order_acquire)&&
        (w.active[0].load()||w.active[2].load()||w.active[3].load()||w.forgeBuilder.load()||w.dice.load());
    const float radians=std::isfinite(degrees)&&degrees>=70&&degrees<=140?degrees*.01745329251994329577f:0;
    policy.publish({now,radians,immersive&&active,immersive});
}
activity_eye59c19::State EyeState(std::uint64_t now) noexcept {
    activity_eye59c19::State out{};
    if(!eyeState19.read(out)||!activity_eye59c19::fresh(out,now))return {};
    return out;
}
void NoteEye59c19(const EyeReceipt59c19& r) noexcept {
    using namespace diagnostics::capture168;
    if(r.state.active&&(active(Domain::Hud,r.tick)||active(Domain::Interaction,r.tick)))eyeReceipts19.publish(r);
}
void Report(std::uint64_t now) noexcept {
    using namespace diagnostics::capture168;
    const bool interaction=active(Domain::Interaction,now),hud=active(Domain::Hud,now);
    {   // R89 dice: one row when the exit hold wrote (edge + every 5 s while it holds).
        static std::uint64_t lastHoldWrites=0,lastHoldRow=0;
        const auto writes=exitHoldWritesR89.load(std::memory_order_relaxed);
        if(writes!=lastHoldWrites&&(!lastHoldRow||now-lastHoldRow>=5000)){
            char m[200]{};std::snprintf(m,sizeof(m),"R89 DICE_EXIT_LENS hold=MINIGAME_CAMERA_AFTER_EXIT framesWritten=%llu wrote=GAMEPLAY_FOV ends=FIRST_NON_MINIGAME_CAMERA",
                (unsigned long long)(writes-lastHoldWrites));
            diagnostic_trace::Write(m);lastHoldWrites=writes;lastHoldRow=now;
        }
    }
    if(!interaction&&!hud)return;
    if(interaction){
        static std::uint64_t lastPeopleTickD52=0,lastPeopleSessionD52=0;
        dialogue_participants_d52::Observation people{};
        const auto serial=gate.session[unsigned(Domain::Interaction)].load(std::memory_order_acquire);
        if(dialoguePeopleD52.read(people)&&people.session==serial&&
           dialogue_state59c10r2::age(people.tick,now,500)&&
           (people.tick!=lastPeopleTickD52||serial!=lastPeopleSessionD52)){
            lastPeopleTickD52=people.tick;lastPeopleSessionD52=serial;
            char m[1000]{};int n=std::snprintf(m,sizeof(m),
                "D52 F5_DIALOGUE_PARTICIPANTS sourceTick=%llu session=%llu instance=%p scene=%p playerSoul=%llu result=%s total=%u sampled=%u entityTwinController=UNRESOLVED finalNpcPose=NOT_OBSERVED souls=",
                (unsigned long long)people.tick,(unsigned long long)serial,reinterpret_cast<void*>(people.instance),reinterpret_cast<void*>(people.scene),
                (unsigned long long)people.playerSoul,dialogue_participants_d52::name(people.reason),people.total,people.count);
            if(people.reason==dialogue_participants_d52::Reason::Observed)
                for(unsigned i=0;i<people.count&&n>0&&n<int(sizeof(m))-50;++i)
                    n+=std::snprintf(m+n,sizeof(m)-n,"%s%llu/%u",i?",":"",(unsigned long long)people.people[i].soul,unsigned(people.people[i].participating));
            diagnostic_trace::Write(m);
        }
    }
    { // Bounded POD diagnostics, including unavailable observations. No native reads.
        static std::uint64_t last=0,lastView=0,lastSelector=0,lastOverrides=0,lastNativeTrue=0;static unsigned lastReason=~0u;
        dialogue_state59c10r2::State d{};const bool have=dialogue.read(d);
        const unsigned reason=have?unsigned(d.membership.reason):~0u;
        if(!last||now<last||now-last>=(reason!=lastReason?250u:1000u)){
            const auto viewCount=viewObservations19c2.load(std::memory_order_relaxed);
            const auto selectorCount=selectorObservations19c2.load(std::memory_order_relaxed);
            const auto overrideCount=selectorOverrides19c2.load(std::memory_order_relaxed);
            const auto nativeTrue=selectorNativeTrue19c2.load(std::memory_order_relaxed);
            dialogue_presentation19c2::Request intent{};(void)dialogueRequest19c2.read(intent);
            char msg[640]{};std::snprintf(msg,sizeof(msg),
                "C19C2 HUD_DIALOGUE_STATE known=%d active=%d rawInstance=%p native=%s state=%u flags=%X participant=%d listed=%d camera=%u ageMs=%llu callbacks{view=%llu selector=%llu nativeEligible=%llu overrides=%llu} requestedFP=%d inputMapKnown=%d inputMapEnabled=%d inputMapReason=%s inputMapManager=%p inputMap=%p stalePermission=REFUSED",
                have&&d.known,have&&d.active,reinterpret_cast<void*>(d.instance),have?dialogue_membership19c2::name(d.membership.reason):"NO_OBSERVATION",
                d.membership.state,d.membership.flags,d.membership.participant,d.membership.listed,unsigned(d.camera),
                (unsigned long long)(have&&now>=d.tick?now-d.tick:~0ull),
                (unsigned long long)(viewCount-lastView),(unsigned long long)(selectorCount-lastSelector),(unsigned long long)(nativeTrue-lastNativeTrue),
                (unsigned long long)(overrideCount-lastOverrides),dialogue_presentation19c2::requested(intent,now),
                d.inputOwner.known,d.inputOwner.enabled,dialogue_input_owner19c3::name(d.inputOwner.reason),reinterpret_cast<void*>(d.inputOwner.manager),reinterpret_cast<void*>(d.inputOwner.map));
            diagnostic_trace::Write(msg);last=now;lastReason=reason;lastView=viewCount;lastSelector=selectorCount;lastOverrides=overrideCount;lastNativeTrue=nativeTrue;
            const auto& head=d.presentation;
            std::snprintf(msg,sizeof(msg),
                "C27 HUD_DIALOGUE_PRESENTATION active=%d known=%d nativeCamera=%u manager=%p selected=%p selectedVftRva=%llX flags38_39_3B=%u/%u/%u playerHeadHiddenA38=%u groupFlagsA39=%u requestedFP=%d observeOnly=1",
                have&&d.active,have&&head.known,unsigned(d.camera),reinterpret_cast<void*>(head.manager),
                reinterpret_cast<void*>(head.selected),(unsigned long long)(head.selectedVft>=base?head.selectedVft-base:0),
                unsigned(head.blendFromThirdPerson),unsigned(head.forceVisible),unsigned(head.bypassCompose),unsigned(head.headHidden),
                unsigned(head.groupFlags),dialogue_presentation19c2::requested(intent,now));
            diagnostic_trace::Write(msg);
        }
    }
    { // POD receipt only; no native reads, disk I/O or formatting on render.
        static std::uint64_t lastEye=0,lastLife=0;static unsigned lastReason=~0u;
        EyeReceipt59c19 e{};
        if(eyeReceipts19.read(e)&&e.state.active&&dialogue_state59c10r2::age(e.tick,now,80)){
            const bool changed=lastLife!=e.state.lifetime||lastReason!=unsigned(e.reason);
            if(!lastEye||now-lastEye>=(changed?250u:1000u)){
                lastEye=now;lastLife=e.state.lifetime;lastReason=unsigned(e.reason);
                constexpr const char* names[]{"APPLIED","INACTIVE","OWNER_UNAVAILABLE","OWNER_REFUSED","NO_FRESH_BODY",
                    "NO_NEUTRAL_EYE","LIFETIME_CHANGED","BODY_CHANGED","INVALID_EYE","NATIVE_ALCHEMY","NATIVE_SHARPENING"};
                const auto delta=dialogue_state59c10r2::sub(e.authored,e.anchored);
                char line[440]{};std::snprintf(line,sizeof(line),
                    "C10R19 ACTIVITY_EYE result=%s kindMask=%u lifetime=%llu head=%d neutral=%d authoredToAnchorM=%.4f entity=%p sourceAgeMs=%llu source=%s attempt=%s hmdAppliedAfter=ONCE",
                    names[unsigned(e.reason)],e.state.kindMask,(unsigned long long)e.state.lifetime,e.haveHead,e.neutral,
                    double(std::sqrt(dialogue_state59c10r2::dot(delta,delta))),reinterpret_cast<void*>(e.state.entity),(unsigned long long)(now-e.tick),e.reason==activity_eye59c19::Reason::NativeAlchemy?"CURRENT_NATIVE_ALCHEMY_EYE":e.reason==activity_eye59c19::Reason::NativeSharpening?"CURRENT_NATIVE_SHARPENING_EYE":"PRE_ACTIVITY_MODEL_LOCAL",e.fromCull?"CULL":"PUBLISH");
                diagnostic_trace::Write(line);
            }
        }
    }
    static std::uint64_t last=0;static Receipt previous{};
    static std::array<std::uint64_t,9> prior{};
    Receipt r{};if(!receipts.read(r)||!r.tick||now<r.tick||now-r.tick>80)return;
    const bool changed=r.result!=previous.result||r.evidence.refusal!=previous.evidence.refusal||
        r.evidence.cameraRva!=previous.evidence.cameraRva||r.evidence.instance!=previous.evidence.instance||
        std::fabs(r.native-previous.native)>.02f;
    if(last&&now>=last&&now-last<(changed?250u:1000u))return;
    std::array<std::uint64_t,9> count{};
    for(unsigned i=0;i<count.size();++i){const auto current=observed[i].load(std::memory_order_relaxed);count[i]=current-prior[i];prior[i]=current;}
    last=now;previous=r;char msg[760]{};
    std::snprintf(msg,sizeof(msg),"C10R3 %s_LENS result=%s refusal=%s nativeDeg=%.3f writtenDeg=%.3f requestedDeg=%.3f cameraOwner=%u cameraVftRva=%llX kindMask=%u instance=%p sourceAgeMs=%llu policyAgeMs=%llu samples{applied=%llu unchanged=%llu missingPolicy=%llu inactive=%llu stale=%llu owner=%llu output=%llu fov=%llu write=%llu} scope=FINAL_PLAYER_VIEW onlyFov=1",
        interaction?"ACTIVITY":"HUD",resultName[unsigned(r.result)],name(r.evidence.refusal),double(r.native*57.29577951308232f),double(r.written*57.29577951308232f),double(r.requested*57.29577951308232f),unsigned(r.evidence.camera),(unsigned long long)r.evidence.cameraRva,r.evidence.kindMask,reinterpret_cast<void*>(r.evidence.instance),
        (unsigned long long)(now-r.tick),(unsigned long long)(r.policyTick&&now>=r.policyTick?now-r.policyTick:~0ull),
        (unsigned long long)count[0],(unsigned long long)count[1],(unsigned long long)count[2],(unsigned long long)count[3],(unsigned long long)count[4],(unsigned long long)count[5],(unsigned long long)count[6],(unsigned long long)count[7],(unsigned long long)count[8]);
    diagnostic_trace::Write(msg);
}
}
