#pragma once
#include "ui_native_catalogue.h"
#include "core/snapshot_mailbox.h"
#include "vrik/alchemy_semi46.h"
#include <array>
#include <atomic>
#include <cstdint>
namespace kcd2vr::activity_input163 {
// Input ownership only. World textures stay visible after activity exit in the
// supplied log, so they must NEVER activate controls. The retired action-map
// observer is not resurrected. Unclassified workstations have an explicit mode.
// The native witness below is published only by the exact supported minigame
// start/stop/destructor event boundaries. Read() never dereferences a game object.
enum class Kind175 : unsigned { Alchemy,Smithing,Reading,Sharpening,Unknown,ForgeBuilder,Dice };
struct Witness175 {
    std::array<std::atomic<std::uintptr_t>,4> active{};
    // M1: ForgeBuilder uses the same qualified C_Minigame lifecycle, but is
    // deliberately separate from the four tracked-hand activity preferences.
    std::atomic<std::uintptr_t> forgeBuilder{0};
    std::atomic<std::uintptr_t> dice{0};
    std::atomic<std::uint64_t> diceStart{0};
    std::atomic<std::uint64_t> forgeBuilderStart{0};
    std::atomic<std::uint64_t> forgeBuilderEpoch{1};
    std::atomic<bool> ready{false};
    // C10R19 actual native lifecycle changes, including same-address reuse.
    std::atomic<std::uint64_t> lifetime175{1};
    std::atomic<unsigned> transitionDepth175{0};
    std::array<std::atomic<std::uint64_t>,4> startTick175{};
    // Per-activity continuity survives nested Reading; global lifetime still revokes packets.
    std::array<std::atomic<std::uint64_t>,4> activityEpochH46{};
    // R20C1: event-only smithing phase, tagged with the minigame lifetime.
    // A late callback from an old instance can never authorize a new instance.
    std::atomic<std::uint64_t> smithEpochC1{1},smithSampleC1{0};
    void Begin(Kind175 kind,std::uintptr_t instance,std::uint64_t enteredAt=0)noexcept{
        if(kind==Kind175::Dice){
            if(ready.load(std::memory_order_acquire)&&instance&&dice.exchange(instance,std::memory_order_acq_rel)!=instance){
                diceStart.store(enteredAt,std::memory_order_release);lifetime175.fetch_add(1,std::memory_order_acq_rel);
            }
            return;
        }
        if(kind==Kind175::ForgeBuilder){
            if(ready.load(std::memory_order_acquire)&&instance&&
               forgeBuilder.exchange(instance,std::memory_order_acq_rel)!=instance){
                forgeBuilderStart.store(enteredAt,std::memory_order_release);
                forgeBuilderEpoch.fetch_add(1,std::memory_order_acq_rel);
                lifetime175.fetch_add(1,std::memory_order_acq_rel);
            }
            return;
        }
        if(ready.load(std::memory_order_acquire)&&unsigned(kind)<active.size()&&instance){
            if(kind==Kind175::Smithing)smithEpochC1.fetch_add(1,std::memory_order_acq_rel);
            if(active[unsigned(kind)].exchange(instance,std::memory_order_acq_rel)!=instance){
                startTick175[unsigned(kind)].store(enteredAt,std::memory_order_release);
                activityEpochH46[unsigned(kind)].fetch_add(1,std::memory_order_acq_rel);
                lifetime175.fetch_add(1,std::memory_order_acq_rel);
            }
        }
    }
    Kind175 End(std::uintptr_t instance)noexcept{
        Kind175 released=Kind175::Unknown;if(!instance)return released;
        auto die=instance;
        if(dice.compare_exchange_strong(die,0,std::memory_order_acq_rel)){
            diceStart.store(0,std::memory_order_release);lifetime175.fetch_add(1,std::memory_order_acq_rel);released=Kind175::Dice;
        }
        auto forge=instance;
        if(forgeBuilder.compare_exchange_strong(forge,0,std::memory_order_acq_rel)){
            forgeBuilderStart.store(0,std::memory_order_release);
            forgeBuilderEpoch.fetch_add(1,std::memory_order_acq_rel);
            lifetime175.fetch_add(1,std::memory_order_acq_rel);
            released=Kind175::ForgeBuilder;
        }
        for(unsigned i=0;i<active.size();++i){auto expected=instance;
            if(active[i].compare_exchange_strong(expected,0,std::memory_order_acq_rel)){
                startTick175[i].store(0,std::memory_order_release);
                activityEpochH46[i].fetch_add(1,std::memory_order_acq_rel);
                lifetime175.fetch_add(1,std::memory_order_acq_rel);
                released=Kind175(i);if(released==Kind175::Smithing)smithEpochC1.fetch_add(1,std::memory_order_acq_rel);}}
        return released;
    }
    void Clear()noexcept{dice.store(0,std::memory_order_release);diceStart.store(0,std::memory_order_release);for(auto&e:activityEpochH46)e.fetch_add(1,std::memory_order_acq_rel);for(auto&entry:active)entry.store(0,std::memory_order_release);for(auto&t:startTick175)t.store(0,std::memory_order_release);forgeBuilder.store(0,std::memory_order_release);forgeBuilderStart.store(0,std::memory_order_release);forgeBuilderEpoch.fetch_add(1,std::memory_order_acq_rel);smithEpochC1.fetch_add(1,std::memory_order_acq_rel);lifetime175.fetch_add(1,std::memory_order_acq_rel);}
    int SmithPhaseC1()const noexcept{
        if(!ready.load(std::memory_order_acquire)||!active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire))return -1;
        const auto sample=smithSampleC1.load(std::memory_order_acquire);
        return (sample>>8)==smithEpochC1.load(std::memory_order_acquire)&&(sample&255)<=20?int(sample&255):-1;
    }
    bool Active()const noexcept{
        if(!ready.load(std::memory_order_acquire))return false;
        if(forgeBuilder.load(std::memory_order_acquire)||dice.load(std::memory_order_acquire))return true;
        for(const auto&entry:active)if(entry.load(std::memory_order_acquire))return true;
        return false;
    }
};
inline Witness175 witness175;
inline bool ForgeBuilderActive() noexcept {
    return witness175.ready.load(std::memory_order_acquire)&&
        !witness175.transitionDepth175.load(std::memory_order_acquire)&&
        witness175.forgeBuilder.load(std::memory_order_acquire)!=0;
}
// R59.16C10 (Codex HUD handoff 5c: "give the held-B rearm a REAL native stage epoch"): the witnessed minigame
// lifetime boundaries (start/stop instances per kind + the event-only smithing phase epoch). It changes exactly
// when a native activity instance begins or ends (alchemy -> alchemy reading, smithing stage change), never on a
// rendered texture or pointer. Zero while the native witness is not installed.
inline std::uint64_t StageEpoch59c10() noexcept {
    if(!witness175.ready.load(std::memory_order_acquire))return 0;
    std::uint64_t h=1469598103934665603ull;
    for(unsigned i=0;i<witness175.active.size();++i){h^=std::uint64_t(witness175.active[i].load(std::memory_order_acquire))+i;h*=1099511628211ull;}
    h^=std::uint64_t(witness175.forgeBuilder.load(std::memory_order_acquire));h*=1099511628211ull;
    h^=std::uint64_t(witness175.dice.load(std::memory_order_acquire));h*=1099511628211ull;
    h^=witness175.forgeBuilderEpoch.load(std::memory_order_acquire);h*=1099511628211ull;
    h^=witness175.smithEpochC1.load(std::memory_order_acquire);h*=1099511628211ull;
    return h?h:1;
}
// Per-activity pose preference, independent of native input/combat exclusion.
// Unknown activities retain native hands. No polling or guessed class identity.
inline std::array<std::atomic<bool>,4> trackedHands59c7f{true,true,false,true};
inline bool TrackedHands59c7f() noexcept {
    if(!witness175.ready.load(std::memory_order_acquire))return false;
    if(witness175.forgeBuilder.load(std::memory_order_acquire)||witness175.dice.load(std::memory_order_acquire))return false;
    bool any=false;
    for(unsigned i=0;i<witness175.active.size();++i)if(witness175.active[i].load(std::memory_order_acquire)){
        if(!trackedHands59c7f[i].load(std::memory_order_relaxed))return false;
        any=true;
    }
    return any;
}
inline constexpr bool AnvilPhaseC1(int phase) noexcept {
    // 0x858188 selects inspection first for 12/13, before IsAnvil(0x8591F0).
    return phase==11||phase==14||phase==16||phase==17;
}
bool InstallNative175() noexcept;
void UninstallNative175() noexcept;
// C38: called only after the existing original outer native gameplay frame
// returns. Bounded reads of the exact active alchemy/interactor; no native call,
// new hook or worker polling. It publishes the completed frame's copied target.
void AlchemySelectionNativeFrameEnd38() noexcept;
// C45: the alchemy recipe book as the game itself holds it: C_OpenBookAction (primary vft 3A5BD40, C37I1 RTTI) live
// in the alchemy director's slot 1 with started=1 (the exact read behind the C37 F5_ALCHEMY_STAGE class=OpenBook
// receipts, diag.log 84118..85042). Copied once per completed native frame beside the C38 selection; read-only.
// C46 also joins the exact per-action read-enable callback generation. A live
// OpenBook entry/exit animation alone never authorizes a page pulse.
struct AlchemyBook45 {std::uintptr_t instance=0,action=0;std::uint64_t lifetime=0,tick=0,generation=0;std::uint32_t entity=0;};
inline kcd2vr::core::SnapshotMailbox<AlchemyBook45> alchemyBook45;
inline bool AlchemyBookOpen45(std::uint64_t now,AlchemyBook45& out) noexcept {
    out={};AlchemyBook45 a{};kcd2vr::alchemy_semi46::BookEnable enabled{};
    if(!kcd2vr::alchemy_semi46::hooksReady.load(std::memory_order_acquire)||!alchemyBook45.read(a)||!a.instance||!a.action||!a.tick||now<a.tick||now-a.tick>120||
       !witness175.ready.load(std::memory_order_acquire)||witness175.transitionDepth175.load(std::memory_order_acquire)||
       a.lifetime!=witness175.lifetime175.load(std::memory_order_acquire)||
       a.instance!=witness175.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)||
       !kcd2vr::alchemy_semi46::bookEnable.read(enabled)||!enabled.enabled||enabled.instance!=a.instance||
       enabled.action!=a.action||enabled.lifetime!=a.lifetime||enabled.generation!=a.generation||
       a.generation!=kcd2vr::alchemy_semi46::bookGeneration.load(std::memory_order_acquire))return false;
    out=a;return true;
}
struct State { bool nativeControls=false; };
inline State Read(const ui_native_state::Snapshot& s,bool explicitControls=false) noexcept {
    using namespace ui_native_state;
    const auto live=s.known&s.visible;
    return {explicitControls||witness175.Active()||(live&ui_native_catalogue::NativeContextMask)!=0};
}
}
