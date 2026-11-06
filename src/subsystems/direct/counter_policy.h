#pragma once
// C39 (Miki: "finally good blocking and masterstrikes in the direct mode"). Pure policy + receipt helpers, no native calls.
// 1) GUARD OUT OF STAGGER (DIRECT): the C37C2 log had 101 native perfect-block openings (Henry state+0x778 0->1) in F4 DIRECT;
//    a block press inside the opening gave PB 40 / block 13 / hit 2, while 44 of the 46 hits had NO press at all. 26 of those
//    46 opened while Henry was in phase 64 (own hit reaction) or 32 (recovery) and the old whitelist (native_admission147::
//    guardInput) never presses there. The PB trigger gate 908BF4 only excludes Henry phase 8 (72EFDC(actor,8)) and 512, so the
//    game itself offers the opening in 32/64. Policy: at most ONE ordinary block press per observed opening, only with the VR
//    guard posture, an attacking linked opponent and Henry in 32/64; the game's own block input decides (CanStartBlock/PB).
//    The press is kept until the native block adopts it (phase 128/256 or +308) or the phase leaves 32/64, else released.
// 2) DIRECT COUNTER receipts. Native master strike = attack input at D4AD94 while +7D8>0 -> 22B780B -> 1483540 kind5 ->
//    1483580 -> factory 1483A70: group 1483AF0 true -> 1483B04 SYNC (paired) action, false -> empty (type 55 != PB 14).
//    There is no non-paired native master strike; the HYBRID run's one master strike played Henry's full-body clip and applied
//    TWO native damages (14:08:00.150 15.75, 14:08:00.845 61.64, caller D536BC which the DIRECT sweep veto (C52534) does not stop).
//    So DIRECT never requests it (refusal below) and keeps its single collision-owned BuildAndSubmitMeleeHit.
#include <atomic>
#include <cstdint>
namespace kcd2vr::direct_counter39 {
inline std::atomic<bool> staggerGuard{false};                 // INI VrDirectGuardOutOfStagger (COMBAT FLOW page)
// C44 (Miki: "Henry can also stagger and cannot attack while staggered (option: staggered attacks are slow)"). VrDirectStaggerRule:
// 0 = a DIRECT strike may start out of Henry's own hit reaction (phase 64, after 150 ms - the old VrStrikeDuringStagger=true),
// 1 = no DIRECT hit while Henry is in 64 (the old default: the phase publish yields NATIVE_PHASE_PROTECTED, nothing is submitted),
// 2 = allowed, but a hit of a strike that started in 64 goes in at half strength. The native 64 itself is never written.
inline std::atomic<int> staggerRule44{1};
inline int clampStaggerRule44(int v) noexcept {return v<0?0:v>2?2:v;}
inline bool staggerStrikeAllowed44(int rule) noexcept {return clampStaggerRule44(rule)!=1;}
inline float staggerStrength44(int rule,bool fromStagger,float strength) noexcept {return clampStaggerRule44(rule)==2&&fromStagger?strength*.5f:strength;}
inline constexpr unsigned kStaggerHoldMs=900;                // Henry phase 64 p90 954 ms, 32 p90 799 ms in the C37C2 DIRECT run
inline constexpr const char* kRefusal="NATIVE_MASTER_IS_PAIRED_SYNC_ACTION(1483A90->1483B04)";
inline bool staggered(int phase,int type) noexcept {return (phase==32||phase==64)&&type>=-1&&type<=1;}
inline bool adopted(int phase,bool blocking) noexcept {return blocking||phase==128||phase==256;}
// Native master-strike stance for the attacker's offered zone (+0x158, Henry's frame): combat_zone.xml
// master_strike_combat_zone_id as read by C566D8 (research_r17 B 1.3). Receipt only, never written.
inline int msZone(int offered) noexcept {
    switch(offered){case 0:return 5;case 1:return 2;case 2:return 1;case 3:return 4;case 4:return 3;case 5:return 0;default:return -1;}
}
inline bool counterSide(int origin,int offered) noexcept {
    const int z=msZone(offered);return z>=0&&origin==z&&(origin==0||origin==1||origin==2||origin==5);
}
// Own opening serial (the guard pump's window55c2 is reset with every guard release, so its serial repeats).
struct Opening {
    std::uintptr_t opponent=0;std::uint32_t id=0;std::uint64_t serial=0;bool open=false;
    bool observe(std::uintptr_t actor,std::uint32_t entity,bool known,int count) noexcept {
        if(!known||!actor||!entity||count<0||count>16)return false; // unreadable sample: no edge, no close
        if(opponent!=actor||id!=entity){opponent=actor;id=entity;open=false;}
        if(count==0){open=false;return false;}
        if(open)return false;
        open=true;++serial;return true;
    }
};
enum class Act : unsigned char {None,Press,Keep,Adopted,Release};
enum class Why : unsigned char {None,Disabled,Held,Manual,Phase,Posture,Window,Attacker,Blocking,Spent,Pressed,Holding,Accepted,PostureLost,PhaseLeft,Timeout,Foreign};
inline const char* name(Why w) noexcept {
    constexpr const char* n[]{"NONE","DISABLED","HELD","MANUAL","PHASE","POSTURE","WINDOW","ATTACKER","BLOCKING","SPENT","PRESSED","HOLDING","ADOPTED","POSTURE_LOST","PHASE_LEFT","TIMEOUT","FOREIGN_RELEASE"};
    return n[unsigned(w)];
}
struct Stagger {std::uint64_t tried=0,tick=0;bool pending=false;};
struct StaggerIn {
    bool enabled=false,posture=false,held=false,manual=false,blocking=false,windowOpen=false,attacking=false;
    int phase=-1,type=-99;std::uint64_t window=0,now=0;
};
inline Act step(Stagger& s,const StaggerIn& in,Why& why) noexcept {
    why=Why::None;
    if(s.pending){
        if(!in.held){s.pending=false;why=Why::Foreign;return Act::None;}
        if(adopted(in.phase,in.blocking)){s.pending=false;why=Why::Accepted;return Act::Adopted;}
        if(!in.posture||in.manual){s.pending=false;why=Why::PostureLost;return Act::Release;}
        if(!staggered(in.phase,in.type)){s.pending=false;why=Why::PhaseLeft;return Act::Release;}
        if(in.now<s.tick||in.now-s.tick>kStaggerHoldMs){s.pending=false;why=Why::Timeout;return Act::Release;}
        why=Why::Holding;return Act::Keep;
    }
    if(!in.enabled){why=Why::Disabled;return Act::None;}
    if(in.held){why=Why::Held;return Act::None;}
    if(in.manual){why=Why::Manual;return Act::None;}
    if(!staggered(in.phase,in.type)){why=Why::Phase;return Act::None;}
    if(!in.posture){why=Why::Posture;return Act::None;}
    if(!in.windowOpen||!in.window){why=Why::Window;return Act::None;}
    if(!in.attacking){why=Why::Attacker;return Act::None;}
    if(in.blocking){why=Why::Blocking;return Act::None;}
    if(in.window==s.tried){why=Why::Spent;return Act::None;}
    s.tried=in.window;s.tick=in.now;s.pending=true;why=Why::Pressed;return Act::Press;
}
// Enemy attack START tracker for the exact linked opponent (phase 4/8 entered from a known non-attack phase).
struct Attack {
    std::uintptr_t opponent=0,state=0;std::uint32_t id=0;std::uint64_t serial=0,openTick=0;int lastPhase=-1,offered=-1;bool open=false,known=false;
    enum class Edge : unsigned char {None,Open,Close,Lost};
    Edge observe(std::uintptr_t actor,std::uintptr_t st,std::uint32_t entity,bool readable,int phase,int offeredZone,std::uint64_t now) noexcept {
        if(!readable||!actor||!st||!entity||phase<0){
            if(open&&!actor){open=false;known=false;return Edge::Lost;} // opponent unlinked
            return Edge::None;                                        // unreadable: no edge
        }
        if(actor!=opponent||st!=state||entity!=id){
            const bool was=open;opponent=actor;state=st;id=entity;open=false;lastPhase=phase;known=true;
            return was?Edge::Lost:Edge::None;                         // fresh baseline, never adopt mid-attack
        }
        const bool attack=phase==4||phase==8;const bool before=known&&(lastPhase==4||lastPhase==8);
        lastPhase=phase;known=true;
        if(attack&&!open&&!before){open=true;++serial;openTick=now;offered=offeredZone;return Edge::Open;}
        if(attack&&open&&offered<0&&offeredZone>=0)offered=offeredZone;
        if(!attack&&open){open=false;return Edge::Close;}
        return Edge::None;
    }
};
// Pump -> contact-thread publication (receipt only). state is written last and read first.
inline std::atomic<std::uint64_t> openTick{0},openSerial{0};inline std::atomic<int> openOffered{-1};inline std::atomic<std::uintptr_t> openState{0};
inline void publish(const Attack& a) noexcept {
    if(!a.open){openState.store(0,std::memory_order_release);return;}
    openTick.store(a.openTick,std::memory_order_relaxed);openSerial.store(a.serial,std::memory_order_relaxed);
    openOffered.store(a.offered,std::memory_order_relaxed);openState.store(a.state,std::memory_order_release);
}
}
