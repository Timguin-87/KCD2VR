#pragma once
// R138: who owns a LEFT trigger press that starts while Henry is out of combat (R122 native-LT rule).
// Miki 7 Nov: "lost ability to give punches, henry was kicking a lot, the block position was also strange".
// closing the left fist (grip + trigger) to start a fist fight IS such a press, R122 gave it to the game as LT
// for the whole press. LT = the game's block: henry stood in the game's own free block and every punch sent
// with LT held was heavy-in-free-guard = the game's unarmed KICK row (attack type 4).
// rule now: an empty left hand whose grip is closed (>= .60, the fist threshold) is a FIST, never native LT.
// the decision waits up to 100 ms for the grip (the trigger usually crosses first while the hand closes);
// an open-hand trigger press still reaches the game after those 100 ms (stealth kill / knock out / butcher -
// the game's own 0.25 s hold starts then). a native press whose grip closes later is cut for the rest of
// that press. state is per press only (no latch).
#include <cstdint>
namespace kcd2vr::fist_lt_r138 {
constexpr std::uint64_t kDecideMs=100;
constexpr float kFistGrip=.60f,kDown=.20f;
enum class Event{None,Native,Fist,Cut};
struct State{bool pass=false,down=false,pending=false;std::uint64_t pressTick=0;};
struct Input{
    float raw=0,grip=0;std::uint64_t now=0;
    bool leftHandFree=true; // no item in henry's left hand (weapon in the left hand keeps the old R122 rule)
    bool consume=false,menu=false,tracked=false,ltPublished=false;
    int inCombat=-1; // -1 unknown, 0 out of combat, 1 in combat
};
// returns the decision event of this sample (for one log line); s.pass = raw LT goes to the game this sample.
inline Event step(State& s,const Input& in) noexcept {
    Event ev=Event::None;
    const bool down=in.raw>=kDown;
    const bool fist=in.grip>=kFistGrip&&in.leftHandFree;
    if(!down){s.pass=false;s.pending=false;}
    else if(!s.down&&in.consume&&!in.menu&&in.tracked){
        s.pass=false;s.pressTick=in.now;s.pending=in.inCombat==0&&!in.ltPublished;
    }
    if(s.pending){
        const bool refuse=fist||in.menu||!in.tracked||in.ltPublished;
        if(refuse||in.now<s.pressTick||in.now-s.pressTick>=kDecideMs){
            s.pending=false;s.pass=!refuse;ev=refuse?Event::Fist:Event::Native;
        }
    }
    if(s.pass&&fist){s.pass=false;ev=Event::Cut;}
    if(s.pass&&in.ltPublished)s.pass=false; // mod block/kick owns LT now
    s.down=down;
    return ev;
}
}
