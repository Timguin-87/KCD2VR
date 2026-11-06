#pragma once
#include <cmath>
#include <cstdint>
// DS-F94: the CROSSBOW / RIFLE physical reload stage policy - the unfinished
// ranged feature (the bow's F11=2 stage machine exists; this generalizes the
// empty->loaded lifecycle to the two remaining weapon families). Pure logic,
// host-tested. The native animation/ammo calls are a later stage; nothing here
// writes natively or fabricates a loaded weapon.
namespace kcd2vr::crossbow_rifle_reload60 {
enum class Weapon : unsigned char {None,Crossbow,Rifle};
enum class Stage : unsigned char {Idle,Empty,Working,Loaded,Ready,Fault};
inline const char* stageName(Stage s) noexcept {
    switch(s){case Stage::Idle:return "IDLE";case Stage::Empty:return "EMPTY";case Stage::Working:return "WORKING";
    case Stage::Loaded:return "LOADED";case Stage::Ready:return "READY";default:return "FAULT";}
}
struct Step { // the per-stage action the player performs (host evidence)
    unsigned char stage=0; // 0 = the span/draw action, 1 = the ammo placement
    bool valid=false,placed=false,released=false;
};
struct Request {
    Weapon weapon=Weapon::None;
    bool weaponDrawn=false,current=true,ammoAvailable=false,handTracked=false,steady=false;
    std::uint64_t tick=0;
};
struct Machine {
    Stage stage=Stage::Idle;Weapon weapon=Weapon::None;
    std::uint64_t since=0,spanSince=0;unsigned spanMs=0;unsigned char spanStage=0;
    // crossbow: span (draw the string) then place the bolt; rifle: load then ram.
    static constexpr unsigned char kSpan=0,kPlace=1;
    static constexpr unsigned kMinSpanMs=250,kMaxSpanMs=4000;
    static constexpr unsigned kMaxStageMs=6000;
    void begin(Weapon w,std::uint64_t now) noexcept {
        weapon=(w==Weapon::Crossbow||w==Weapon::Rifle)?w:Weapon::None;
        stage=(weapon!=Weapon::None)?Stage::Empty:Stage::Idle;since=now;spanSince=0;spanMs=0;spanStage=kSpan;
    }
    void release() noexcept {stage=Stage::Idle;weapon=Weapon::None;since=0;spanMs=0;spanSince=0;spanStage=kSpan;}
    // per-frame step: the request must stay current+tracked; the span must be
    // HELD at least kMinSpanMs before the ammo placement is admitted; a lost
    // hand or an over-long stage faults (the caller shows the one visible line).
    // DS-F102 (Codex driver/touch review): the span dwell accumulates ONLY on
    // stage-matched valid controller evidence (s.stage==kSpan) and restarts on
    // a lost hold - an armed but untouched weapon never earns held-span credit;
    // a wrong-stage release/placement cannot advance the machine.
    Stage step(const Request& r,const Step& s,std::uint64_t now) noexcept {
        if(stage==Stage::Idle||stage==Stage::Fault)return stage;
        if(r.weapon!=weapon||!r.weaponDrawn||!r.current||!r.handTracked){release();return Stage::Idle;} // identity loss: abort
        const std::uint64_t elapsed=now>=since?now-since:0;
        if(elapsed>kMaxStageMs){stage=Stage::Fault;return stage;}
        switch(stage){
        case Stage::Empty:
            if(spanStage==kSpan){
                if(s.stage!=kSpan||!s.valid){spanMs=0;spanSince=0;return stage;} // wrong/missing stage evidence: restart the dwell
                if(!spanSince)spanSince=now; // DS-F102: the dwell starts on the FIRST valid span hold, not at begin()
                // Measure through THIS release/hold sample. Cached previous-frame
                // dwell undercounted a valid release and let an overlong release pass.
                const auto dwell=now>=spanSince?now-spanSince:0;
                if(dwell>kMaxSpanMs){spanMs=kMaxSpanMs+1;stage=Stage::Fault;return stage;}
                spanMs=unsigned(dwell);
                // the span/draw must be HELD long enough, then the ammo is placed
                if(s.released){
                    if(spanMs>=kMinSpanMs){spanStage=kPlace;stage=Stage::Working;since=now;spanMs=0;spanSince=0;}
                    else {spanMs=0;spanSince=0;spanStage=kSpan;stage=Stage::Fault;return stage;} // a tap is not a span
                } // an over-long span has already failed above, including release
                return stage;
            }
            break;
        case Stage::Working:
            if(spanStage==kPlace){
                if(s.stage!=kPlace||!s.valid)return stage; // wrong-stage evidence cannot place the ammo
                if(s.placed){if(!r.ammoAvailable){stage=Stage::Fault;return stage;}stage=Stage::Loaded;since=now;}
                return stage;
            }
            break;
        case Stage::Loaded:
            if(r.steady){stage=Stage::Ready;since=now;}
            return stage;
        case Stage::Ready:
            if(!r.ammoAvailable||!r.steady){release();return Stage::Idle;} // fired: back to idle (the caller re-arms Empty)
            return stage;
        default:return stage;
        }
        return stage;
    }
};
}
