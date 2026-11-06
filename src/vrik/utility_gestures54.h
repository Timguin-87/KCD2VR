#pragma once
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdio>
namespace kcd2vr::utility54 {
// Independent saved choices; disabling pointing Quick Talk never changes these.
inline std::atomic<bool> torchGestureD39{true},visorGestureD39{true};
inline void SetTorchGestureD39(bool on) noexcept {torchGestureD39.store(on,std::memory_order_release);}
inline void SetVisorGestureD39(bool on) noexcept {visorGestureD39.store(on,std::memory_order_release);}
enum class Action { None, VisorOpen, VisorClose, Torch };
struct Input {
    std::uint64_t tick=0;
    bool enabled=false,valid=false,empty=false,torch=false;
    double right=0,forward=0,up=0,trigger=0; // wrist relative to HMD, yaw frame; metres
    double grip=0; // utility owns Trigger-only; Grip+Trigger belongs to combat
    bool visorEnabled=true,torchEnabled=true; // D39 independent runtime choices
};
struct Output { Action action=Action::None;bool consumesTrigger=false; };
struct State {
    enum Zone { None,Temple,Shoulder } zone=None;
    bool blocked=true,down=false,ownedHold=false;
    bool lateStowR114=false; // torch held, press began outside the stow zone: may still arm for a short moment
    std::uint64_t last=0,began=0,pressR114=0;
    double startUp=0,startRight=0,startForward=0;
};
// R114 (Miki 6 Nov: "torch stowing is also a martyrium - if I press trigger behind my back - torch down"):
// taking the torch keeps the small over-the-shoulder zone (empty hand). STOWING it (the left hand holds the
// torch) uses a much bigger zone: anywhere behind the head / over either shoulder, or low behind the back.
// at head/shoulder height (up >= -45 cm) any point 2 cm behind the eyes counts, below that the hand must be
// 14 cm behind the eyes (= behind the body), so a torch held at the hip or in front for a block never stows.
// metres, wrist relative to the HMD in its yaw frame (right +, forward +, up +).
inline bool stowZoneR114(const Input& i) noexcept {
    if(!i.torchEnabled||!i.torch||i.empty)return false;
    if(!(i.right>=-.75&&i.right<=.25&&i.forward>=-.85&&i.up>=-1.05&&i.up<=.15))return false;
    return i.up>=-.45?i.forward<=-.02:i.forward<=-.14;
}
inline constexpr std::uint64_t kLateStowMsR114=350; // trigger squeezed a moment before the hand got there
inline Output update(State& s,const Input& i) noexcept {
    Output o{};
    const bool finite=std::isfinite(i.right)&&std::isfinite(i.forward)&&std::isfinite(i.up)&&std::isfinite(i.trigger)&&std::isfinite(i.grip);
    // Do not consume a fist's trigger, even if Trigger was pressed first at the
    // temple. Releasing Grip alone cannot turn the same hold into a visor move.
    // D38: a hand holding the native torch is no fist; its Grip on the torch must not block the stow gesture.
    if(std::isfinite(i.grip)&&i.grip>=.25&&!(i.torch&&!i.empty)){
        s.zone=State::None;s.blocked=true;s.down=i.trigger>.25;s.lateStowR114=false;
        s.ownedHold=false;s.last=i.tick;return o;
    }
    if(!i.enabled||!i.valid||!finite||!i.tick||
       (s.last&&(i.tick<s.last||i.tick-s.last>80))){
        s.zone=State::None;s.blocked=true;s.down=i.trigger>.25;s.lateStowR114=false;
        // A consumed physical press cannot reappear as native block/aim when
        // tracking or gameplay ownership returns. Only a known release retires it.
        if(i.valid&&std::isfinite(i.trigger)&&i.trigger<.25)s.ownedHold=false;
        s.last=i.tick;o.consumesTrigger=s.ownedHold;return o;
    }
    // GetTickCount64 can repeat across distinct XR frames. Equal clock values
    // do not prove tracking loss and must not cancel a real held gesture.
    // Holds still use elapsed time; a backwards clock or >80ms gap quarantines.
    s.last=i.tick;
    // A live OFF edge cancels the pending action but keeps its consumed press
    // quarantined. Re-enabling while Trigger is held must not replay it.
    if((s.zone==State::Temple&&!i.visorEnabled)||(s.zone==State::Shoulder&&!i.torchEnabled)){s.zone=State::None;s.blocked=true;s.lateStowR114=false;}
    if(i.trigger<.25){s.down=false;s.blocked=false;s.zone=State::None;s.ownedHold=false;s.lateStowR114=false;return o;}
    const bool press=i.trigger>=.70&&!s.down;
    if(i.trigger>=.70)s.down=true;
    if(s.blocked){
        // R114: only a torch-holding press that began outside the zone, still held, inside 350 ms, may arm the
        // stow when the hand arrives behind the head/back. no other blocked hold ever becomes a gesture.
        if(s.lateStowR114&&s.down&&i.tick>=s.pressR114&&i.tick-s.pressR114<=kLateStowMsR114&&stowZoneR114(i)){
            s.blocked=false;s.lateStowR114=false;s.zone=State::Shoulder;s.ownedHold=true;
            s.began=i.tick;s.startUp=i.up;s.startRight=i.right;s.startForward=i.forward;
        }else{
            if(s.lateStowR114&&(i.tick<s.pressR114||i.tick-s.pressR114>kLateStowMsR114||!i.torch))s.lateStowR114=false;
            o.consumesTrigger=s.ownedHold;return o;
        }
    }else if(press){
        const bool temple=i.visorEnabled&&i.empty&&i.right>=-.34&&i.right<=-.04&&i.forward>=-.16&&i.forward<=.26&&i.up>=-.20&&i.up<=.18;
        const bool take=i.torchEnabled&&i.empty&&i.right>=-.55&&i.right<=-.10&&i.forward>=-.50&&i.forward<=-.10&&i.up>=-.45&&i.up<=-.12;
        const bool shoulder=take||stowZoneR114(i);
        s.zone=temple?State::Temple:shoulder?State::Shoulder:State::None;
        s.ownedHold=s.zone!=State::None;
        s.began=i.tick;s.startUp=i.up;s.startRight=i.right;s.startForward=i.forward;
        if(s.zone==State::None){s.blocked=true; // a gameplay trigger cannot become a gesture later in the hold
            s.lateStowR114=i.torchEnabled&&i.torch&&!i.empty;s.pressR114=i.tick;} // R114 exception: torch stow only
    }
    if(s.zone==State::None)return o;
    o.consumesTrigger=true;
    // R114: a torch stow only has to stay inside the big stow zone (the hand may still travel behind the back);
    // take/visor keep the original drift limits.
    const bool stowHold=s.zone==State::Shoulder&&i.torch&&!i.empty;
    if(i.tick<s.began||i.tick-s.began>1200||(stowHold&&!stowZoneR114(i))||
       (!stowHold&&(std::abs(i.right-s.startRight)>.25||std::abs(i.forward-s.startForward)>.30))||
       (s.zone==State::Temple&&!i.empty)||(s.zone==State::Shoulder&&!i.empty&&!i.torch)){
        s.zone=State::None;s.blocked=true;return o;
    }
    if(s.zone==State::Temple){
        const double dy=i.up-s.startUp;
        if(dy>=.09)o.action=Action::VisorOpen;
        else if(dy<=-.09)o.action=Action::VisorClose;
    }else if(i.tick-s.began>=150)o.action=Action::Torch;
    if(o.action!=Action::None){s.zone=State::None;s.blocked=true;}
    return o;
}
// The retail console callbacks dispatch these exact C_Player wrappers. C54
// calls the full wrapper (including its native followup), never an SDK slot.
inline constexpr std::uintptr_t kVisorPlayerVft54=0x3AA4B98,kVisorOpen54=0x9F0D74,kVisorClose54=0x2AA1358;
inline constexpr unsigned char kVisorOpenEntry54[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0xE8,0x22,0xFC,0xEF,0xFF,0x48,0x8B,0xCB,0x48,0x83,0xC4,0x20,0x5B,0xE9,0xA1,0xF8,0x26,0x00};
inline constexpr unsigned char kVisorCloseEntry54[]{0x40,0x53,0x48,0x83,0xEC,0x20,0x48,0x8B,0xD9,0xE8,0xF2,0x9F,0xDB,0xFF,0x48,0x8B,0xCB,0xE8,0xC2,0xF2,0x1B,0xFE};
} // namespace kcd2vr::utility54
