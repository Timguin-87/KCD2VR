#pragma once
// NF (Miki 26 Oct): "Native kill animations" switch. ON = while Henry's native paired kill / finisher / takedown clip
// (masterstrike death, combo death, combo finisher, takedown, execution) is actually playing on HIS skeleton, our
// clock/hold/impale hooks stand down for the observed native occurrence. D51 also recognizes exact
// authored nonlethal master/attack-combo aliases on Henry's active runtime layer 0 while this backup is ON.
// D51 native paired BACKUP: while selected, the extra C47 and V168 VR-only masterstrike vetoes yield to
// the native window/factory. Their saved modes remain unchanged and resume when OFF. The independent
// physical finisher filter still follows its own setting. No attack, window, perk or success is supplied.
// Playback stand-down still requires a qualified native clip that the game actually started. The observed clip is
// an OCCURRENCE (Henry's skeleton anim + FIFO layer + clip id), found by scanning every live protected FIFO entry (not
// only the first body owner), retired explicitly when it leaves the FIFO, the inspection stops, or the switch goes OFF.
// no timestamp grants authority by itself; seenTick is only a liveness guard for a stalled inspector.
// D63 attacker catalog is retained for evidence/diagnostics only. Paired/death/finisher
// body ownership is unchanged until paired alignment is proven; OFF is not that proof.
#include <atomic>
#include <cstdint>
#include <cstring>
#include "native_paired_aliases_d51.h"
#include "paired_attacker_aliases_d63.h"

namespace kcd2vr::native_kill_nf {
inline std::atomic<bool> enabled{false}; // VrNativeKillAnimations; live native paired backup, default OFF
inline bool NativePairedBackupD51() noexcept {return enabled.load(std::memory_order_acquire);}
// The ADB supplies exact membership, not a runtime layer mapping. Only an
// active layer-0 entry on validated Henry may use this additional body owner;
// those identity/FIFO checks belong to InspectNativeBody112 and its readers.
inline bool BackupBodyAliasD51(const char* name,int runtimeLayer) noexcept {
    return runtimeLayer==0&&NativePairedBackupD51()&&native_paired_aliases_d51::contains(name);
}
inline std::atomic<std::uint64_t> clipsSeen{0},standDowns{0},retired{0};
inline constexpr std::uint64_t kLivenessMs=250;           // inspector stalled this long = the occurrence is unknown

// Henry's own (master side or self) kill / finisher / takedown clip names from the retail catalog families:
// combat_*_masterstrike*_master, combat_combodeath_*_master, combat_combo_c*_master, combat_takedown_*,
// plus anything named kill / execut / finish. victim-side (_slave) clips on Henry are native body already.
inline bool KillAlias(const char* n) noexcept {
    if(!n||std::strncmp(n,"combat_",7)!=0)return false;
    if(std::strstr(n,"takedown")||std::strstr(n,"kill")||std::strstr(n,"execut")||std::strstr(n,"finish"))return true;
    if(!std::strstr(n,"_master"))return false;
    return std::strstr(n,"masterstrike")||std::strstr(n,"combodeath")||std::strncmp(n,"combat_combo_c",14)==0;
}

// one observed kill occurrence on one skeleton anim (pure value, used by the host tests)
struct Occurrence {std::uintptr_t anim=0;int layer=-1,id=-1;std::uint64_t serial=0,seenTick=0;};
struct State {Occurrence live{};std::uint64_t nextSerial=1;};
// one complete inspection result: on=switch, found=a protected kill entry exists in this anim's FIFO (any position)
inline void observe(State& s,bool on,std::uintptr_t anim,bool found,int layer,int id,std::uint64_t now) noexcept {
    if(!on||!anim||!found){s.live=Occurrence{};return;}
    if(s.live.anim==anim&&s.live.layer==layer&&s.live.id==id&&s.live.serial){s.live.seenTick=now?now:1;return;}
    s.live=Occurrence{anim,layer,id,s.nextSerial++,now?now:1};
}
inline void retire(State& s) noexcept {s.live=Occurrence{};}
// stand down only for the CURRENT Henry anim's live occurrence, while the switch is on and the inspector is alive
inline bool standDown(const State& s,bool on,std::uintptr_t henryAnim,std::uint64_t now) noexcept {
    return on&&henryAnim&&s.live.serial&&s.live.anim==henryAnim&&s.live.seenTick&&now>=s.live.seenTick&&
        now-s.live.seenTick<=kLivenessMs;
}

// runtime. NF3 (Codex I6 review, generation race): the inspector captures the switch GENERATION before its FIFO scan
// and carries it to the publish/retire; SetEnabled bumps the generation under the same lock, so a scan that started
// before an OFF (or an OFF->ON) can never publish afterwards. every metadata commit and every StandDown read is
// serialized by one tiny spin lock (no torn anim/key/seen/serial). StandDown also requires the occurrence's generation
// to be the current one.
inline std::atomic<std::uintptr_t> liveAnim{0};
inline std::atomic<std::uint64_t> liveSerial{0},liveSeen{0},nextSerialRt{1},generation{1},liveGeneration{0},staleScans{0};
inline std::atomic<std::int64_t> liveKey{-1};
inline std::atomic_flag commitLock=ATOMIC_FLAG_INIT;
struct Locked {
    Locked() noexcept {while(commitLock.test_and_set(std::memory_order_acquire)){}}
    ~Locked() noexcept {commitLock.clear(std::memory_order_release);}
    Locked(const Locked&)=delete;Locked& operator=(const Locked&)=delete;
};
inline void RetireLocked() noexcept {
    if(liveSerial.exchange(0,std::memory_order_acq_rel))retired.fetch_add(1,std::memory_order_relaxed);
    liveAnim.store(0,std::memory_order_relaxed);liveSeen.store(0,std::memory_order_relaxed);
    liveKey.store(-1,std::memory_order_relaxed);liveGeneration.store(0,std::memory_order_relaxed);
}
inline void Retire() noexcept {Locked l;RetireLocked();}
// capture BEFORE the scan; 0 = the switch is off (nothing to scan)
inline std::uint64_t BeginScan() noexcept {
    Locked l;return enabled.load(std::memory_order_relaxed)?generation.load(std::memory_order_relaxed):0;
}
// the scan's outcome for the generation it started under. a stale scan publishes nothing and retires nothing that a
// newer generation owns (SetEnabled already cleared the old occurrence).
inline void Observe(std::uint64_t scanGeneration,std::uintptr_t anim,bool found,int layer,int id,std::uint64_t now) noexcept {
    Locked l;
    if(!scanGeneration||scanGeneration!=generation.load(std::memory_order_relaxed)||!enabled.load(std::memory_order_relaxed)){
        staleScans.fetch_add(1,std::memory_order_relaxed);return;}
    if(!anim||!found){RetireLocked();return;}
    const std::int64_t key=(std::int64_t(layer)<<32)|std::uint32_t(id);
    if(liveSerial.load(std::memory_order_relaxed)&&liveAnim.load(std::memory_order_relaxed)==anim&&
       liveKey.load(std::memory_order_relaxed)==key&&liveGeneration.load(std::memory_order_relaxed)==scanGeneration){
        liveSeen.store(now?now:1,std::memory_order_relaxed);return;}
    liveAnim.store(anim,std::memory_order_relaxed);liveKey.store(key,std::memory_order_relaxed);liveSeen.store(now?now:1,std::memory_order_relaxed);
    liveGeneration.store(scanGeneration,std::memory_order_relaxed);
    liveSerial.store(nextSerialRt.fetch_add(1,std::memory_order_relaxed),std::memory_order_release);
    clipsSeen.fetch_add(1,std::memory_order_relaxed);
}
inline bool StandDown(std::uintptr_t henryAnim,std::uint64_t now) noexcept {
    if(!henryAnim)return false;
    {Locked l;
     if(!enabled.load(std::memory_order_relaxed)||!liveSerial.load(std::memory_order_relaxed))return false;
     if(liveGeneration.load(std::memory_order_relaxed)!=generation.load(std::memory_order_relaxed))return false;
     const auto anim=liveAnim.load(std::memory_order_relaxed);const auto seen=liveSeen.load(std::memory_order_relaxed);
     if(anim!=henryAnim||!seen||now<seen||now-seen>kLivenessMs)return false;}
    standDowns.fetch_add(1,std::memory_order_relaxed);return true;
}
// Real switch edges drop the occurrence; applying another menu preference
// with this value unchanged must not interrupt a currently playing fallback.
inline void SetEnabled(bool on) noexcept {Locked l;if(enabled.load(std::memory_order_relaxed)==on)return;
    enabled.store(on,std::memory_order_release);generation.fetch_add(1,std::memory_order_acq_rel);RetireLocked();}
}
