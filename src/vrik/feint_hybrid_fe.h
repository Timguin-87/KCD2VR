#pragma once
// D34 FE VrFeintHybrid: a VR feint of the COMMITTED HYBRID cut through the native feint function (pure decision).
//
// Static proof (retail WHGame.dll 6A350E20):
//  - the NPC block helper C53B30 copies Henry's announced row (+150, row query 12EDEF4 of the F0 zone) into the NPC +5D0
//    ONCE when its block starts (phase 128/256). Before that the NPC has no row of its own; after it a zone change of
//    Henry is not re-read (C37C2 serial 76). So a feint only matters while the NPC block has NOT started.
//  - native player input C5E043 -> C5D728 turns the live direction (+910) into a zone and calls 936A28(actor,zone,guard)
//    during the attack. 936A28 checks the weapon/rows (72E740 vt+130, 936D2C, 15FF380, 9386C0), writes 90D6D0 guard
//    and 90F824 zone, and ONLY when the committed zone (+180) changed and Henry's phase (+30) == 4 notifies Henry and the
//    opponent (+CF8)+148 through 90FAE8 (1/2). That notification is the native re-trigger of the NPC defence.
//  - in this build the HYBRID prepare and commit leave in one pose sample (J27/00:18 log: 42/68 cuts same sample,
//    median 18 ms), so the C62 pre-commit refinement (attackPrepared, 100 ms) never had a window: 0 feints.
// FE therefore asks for at most one native feint per stroke AFTER the commit, while Henry is still in phase 4 and the
// NPC block has not started, when the live C39 blade sector settled clearly in another cut zone.
#include <atomic>
#include <cstdint>
#include "dirblock39.h"
namespace kcd2vr::feint_fe {
inline std::atomic<bool> enabled{true}; // INI VrFeintHybrid (default ON)
inline std::atomic<std::uint64_t> asked{0},feints{0},refused{0};
constexpr unsigned kStableSamples=2;   // consecutive distinct pose samples in the same new zone
constexpr unsigned kMaxAsks=2;         // pose requests per stroke; the game thread spends ONE native feint per stroke
constexpr std::uint64_t kWindowMs=450; // after the commit; above the 0.29-0.43 s native NPC ETA. The live NPC state is the gate.
constexpr double kMarginDeg=15.;       // clear of a combat_zone.xml boundary (C39 near flag is 8)
inline bool cutZone(int z) noexcept {return z==0||z==1||z==2;}
// A sample is clear when C39 is confident and, for an ordinary sector, 15 deg inside its zone. An upward cut that C39
// remapped to its lateral side (angle in the low table) keeps C39's own lateral confidence test.
inline bool clear(bool nearBoundary,int zone,int angle) noexcept {
    if(nearBoundary||!cutZone(zone)||angle<0||angle>359)return false;
    if(kcd2vr::dirblock39::zoneFromAngle(angle)!=zone)return true;
    return kcd2vr::dirblock39::boundaryDistance(angle)>=kMarginDeg;
}
enum class Why : int { Ask=0, Disabled, NotCommitted, Late, Spent, NoSample, NotCutZone, Unclear, SameZone, Settling };
inline const char* name(Why w) noexcept {
    switch(w){case Why::Ask:return "ASK";case Why::Disabled:return "DISABLED";case Why::NotCommitted:return "NOT_COMMITTED";
        case Why::Late:return "LATE";case Why::Spent:return "SPENT";case Why::NoSample:return "NO_FRESH_SAMPLE";
        case Why::NotCutZone:return "NOT_CUT_ZONE";case Why::Unclear:return "SECTOR_UNCLEAR";case Why::SameZone:return "SAME_ZONE";
        case Why::Settling:return "SETTLING";}
    return "?";
}
struct Track {
    std::uint64_t gesture=0,lastSample=0;int announced=-1,candidate=-1;unsigned stable=0,asks=0;
};
struct Live { bool valid=false,nearBoundary=false;std::uint64_t gesture=0,tick=0;int zone=-1,angle=-1; };
// Pose thread, once per pose sample of the primary hand. committedTick = the commit of THIS gesture (0 = none yet),
// announced = the zone the commit carried. Returns Ask with `zone` once the new sector is clear and stable.
inline Why consider(Track& t,bool on,std::uint64_t gesture,bool cut,std::uint64_t committedTick,int announced,
                    const Live& s,std::uint64_t now,int& zone) noexcept {
    zone=-1;
    if(!on){t={};return Why::Disabled;}
    if(!gesture||!cut||!committedTick){t={};return Why::NotCommitted;}
    if(t.gesture!=gesture){t={};t.gesture=gesture;t.announced=announced;}
    if(t.asks>=kMaxAsks)return Why::Spent;
    if(now<committedTick||now-committedTick>kWindowMs)return Why::Late;
    if(!s.valid||s.gesture!=gesture||!s.tick||s.tick==t.lastSample)return Why::NoSample;
    t.lastSample=s.tick;
    if(!cutZone(s.zone)){t.candidate=-1;t.stable=0;return Why::NotCutZone;}
    if(!clear(s.nearBoundary,s.zone,s.angle)){t.candidate=-1;t.stable=0;return Why::Unclear;}
    if(s.zone==t.announced){t.candidate=-1;t.stable=0;return Why::SameZone;}
    if(s.zone!=t.candidate){t.candidate=s.zone;t.stable=1;}else ++t.stable;
    if(t.stable<kStableSamples)return Why::Settling;
    zone=s.zone;t.announced=s.zone;++t.asks;t.candidate=-1;t.stable=0;return Why::Ask;
}
// Game thread: the native state at the request. Same NPC predicate as DIRECT C39 NpcBlockStarted39.
inline bool npcBlockStarted(int npcPhase,int npc5D0) noexcept {return npc5D0>=0&&(npcPhase==128||npcPhase==256);}
enum class Gate : int { Pass=0, Disabled, NotHybrid, Owner, Spent, Late, HenryNotWindup, NoOpponent, NpcBlockStarted,
                        Unreadable, NoZoneChange, ComboSlot, NativeRefused };
inline const char* name(Gate g) noexcept {
    switch(g){case Gate::Pass:return "NATIVE_936A28";case Gate::Disabled:return "DISABLED";case Gate::NotHybrid:return "NOT_HYBRID";
        case Gate::Owner:return "OWNER_OR_STROKE_CHANGED";case Gate::Spent:return "ONE_FEINT_PER_STROKE_SPENT";case Gate::Late:return "LATE";
        case Gate::HenryNotWindup:return "HENRY_NOT_PHASE4";case Gate::NoOpponent:return "NO_PROVEN_OPPONENT";
        case Gate::NpcBlockStarted:return "NPC_BLOCK_STARTED_ROW_KEPT";case Gate::Unreadable:return "STATE_UNREADABLE";
        case Gate::NoZoneChange:return "NO_ZONE_CHANGE";case Gate::ComboSlot:return "COMBO_SLOT_OPEN";
        case Gate::NativeRefused:return "NATIVE_936A28_REFUSED_OR_UNCHANGED";}
    return "?";
}
struct GameState {
    bool on=false,hybrid=false,owner=false,used=false,opponent=false,readable=false,comboSlot=false;
    std::uint64_t sinceCommitMs=~0ull;int henryPhase=-1,npcPhase=-1,npc5D0=-1,oldZone=-1,target=-1;
};
inline Gate gate(const GameState& g) noexcept {
    if(!g.on)return Gate::Disabled;
    if(!g.hybrid)return Gate::NotHybrid;
    if(!g.owner||!cutZone(g.target))return Gate::Owner;
    if(g.used)return Gate::Spent;
    if(g.sinceCommitMs>kWindowMs)return Gate::Late;
    if(!g.readable)return Gate::Unreadable;
    if(g.henryPhase!=4)return Gate::HenryNotWindup; // 936A28 notifies the NPC only in phase 4
    if(!g.opponent||g.npcPhase<0)return Gate::NoOpponent;
    if(npcBlockStarted(g.npcPhase,g.npc5D0))return Gate::NpcBlockStarted;
    if(g.oldZone==g.target)return Gate::NoZoneChange;
    if(g.comboSlot)return Gate::ComboSlot;
    return Gate::Pass;
}
}
