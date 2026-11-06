#pragma once
// Portable UI-only input arbitration. No engine, Windows, or XR calls.
#include <array>
#include <cmath>
#include <cstdint>
namespace kcd2vr::ui_menu_policy {
enum class Owner { Outside, Blocked, Navigation, Left, Right };
struct Hand { bool tracked=false; float grip=0,trigger=0; };
struct Frame {
    bool menu=false,foreground=false,valid=true;
    bool pointer_allowed=true;
    std::uint64_t context=0;
    std::array<Hand,2> hands{};
    std::uint32_t buttons=0;
    std::array<float,4> axes{};
    int preferred=1;
};
struct Decision { Owner owner=Owner::Outside; bool changed=false,nav=false; int hand=-1; bool rearm=false; };
class OwnerState {
    Owner owner_=Owner::Outside;
    std::array<bool,2> grips_{};
    std::uint64_t context_=0;
    bool initialized_=false,armed_=false;
    int selected_=-1;
public:
    static bool ActionNeutral(const Frame& f) noexcept {
        return f.buttons==0 && f.hands[0].trigger<=.35f && f.hands[1].trigger<=.35f;
    }
    static bool Neutral(const Frame& f) noexcept {
        if(!ActionNeutral(f))return false;
        for(float a:f.axes)if(std::fabs(a)>.20f)return false;
        return true;
    }
    Decision Update(const Frame& f) noexcept {
        bool valid=f.valid;
        for(auto h:f.hands)valid=valid&&std::isfinite(h.grip)&&std::isfinite(h.trigger);
        for(float a:f.axes)valid=valid&&std::isfinite(a);
        const bool contextChanged=!initialized_||context_!=f.context;
        initialized_=true;context_=f.context;
        if(!f.menu||!f.pointer_allowed){grips_={};selected_=-1;}
        else if(valid){
            for(int i=0;i<2;++i){if(f.hands[i].grip>=.60f)grips_[i]=true;else if(f.hands[i].grip<=.40f)grips_[i]=false;}
            if(selected_<0||!grips_[selected_])selected_=grips_[0]&&grips_[1]?(f.preferred==0?0:1):grips_[0]?0:grips_[1]?1:-1;
        }
        Owner next=!f.menu?Owner::Outside:!f.foreground||!valid?Owner::Blocked:
            selected_<0?Owner::Navigation:!f.hands[selected_].tracked?Owner::Blocked:selected_==0?Owner::Left:Owner::Right;
        const bool oldTracked=owner_==Owner::Left?f.hands[0].tracked:owner_==Owner::Right?f.hands[1].tracked:true;
        if(f.menu&&!oldTracked)next=Owner::Blocked;
        // Do not synthesize release on a still-held native tap/hold superaction.
        // Likewise let an intentional mouse drag finish before transferring hands.
        if(!contextChanged && f.menu && f.foreground && valid && f.pointer_allowed && next!=Owner::Blocked &&
           owner_!=Owner::Outside && owner_!=Owner::Blocked && next!=owner_ && !ActionNeutral(f))next=owner_;
        Decision d{};d.changed=contextChanged||next!=owner_;d.rearm=d.changed;
        if(d.changed)armed_=false;
        owner_=next;d.owner=next;
        if(next==Owner::Navigation){if(Neutral(f))armed_=true;d.nav=armed_;}
        if(next==Owner::Left)d.hand=0;else if(next==Owner::Right)d.hand=1;
        return d;
    }
};
// UI focus consumes the semantic grip action, not hand tracking/finger curls.
// A grip held across UI->world must be released before it can start a world action.
// This latch is per hand: it never waits for sticks/triggers or blocks locomotion.
class GripRearm {
    std::uint8_t pending_=0;
public:
    std::uint8_t Update(bool consumed,const std::array<Hand,2>& hands) noexcept {
        for(unsigned i=0;i<2;++i){
            const auto bit=static_cast<std::uint8_t>(1u<<i); const auto& h=hands[i];
            const bool observedRelease=h.tracked&&std::isfinite(h.grip)&&h.grip<=.40f;
            if(observedRelease) pending_=static_cast<std::uint8_t>(pending_&~bit);
            else if(consumed) pending_=static_cast<std::uint8_t>(pending_|bit);
        }
        return pending_;
    }
    std::uint8_t Pending() const noexcept { return pending_; }
};
// Deliberate native navigation/selection owns focus over a jittering Grip ray.
// Include all four axes and every digital selection/D-pad bit. Trigger is the
// pointer click and therefore is deliberately not classified as native focus.
inline bool NativeFocusGesture175(const Frame& f) noexcept {
    if(f.buttons!=0)return true;
    for(float axis:f.axes)if(std::isfinite(axis)&&std::fabs(axis)>.30f)return true;
    return false;
}
// Xbox-compatible menu transport bits. Keep holds as holds: KCD2 owns superactions.
struct Pad {std::uint16_t buttons=0;std::uint8_t lt=0,rt=0;std::int16_t lx=0,ly=0,rx=0,ry=0;};
inline bool operator==(const Pad&a,const Pad&b) noexcept{return a.buttons==b.buttons&&a.lt==b.lt&&a.rt==b.rt&&a.lx==b.lx&&a.ly==b.ly&&a.rx==b.rx&&a.ry==b.ry;}
inline std::int16_t Axis(float f) noexcept {if(!std::isfinite(f))return 0;if(f>1)f=1;if(f< -1)f=-1;return static_cast<std::int16_t>(std::lround(f*32767.f));}
inline std::uint8_t Trigger(float f) noexcept {if(!std::isfinite(f)||f<0)f=0;if(f>1)f=1;return static_cast<std::uint8_t>(std::lround(f*255.f));}
inline Pad MakePad(const Frame& f,bool allowed) noexcept {
    Pad p{};if(!allowed)return p;
    p.buttons=static_cast<std::uint16_t>(f.buttons);
    p.lt=Trigger(f.hands[0].trigger);p.rt=Trigger(f.hands[1].trigger);
    p.lx=Axis(f.axes[0]);p.ly=Axis(f.axes[1]);p.ry=Axis(f.axes[3]);
    // Touch has no shoulder buttons: RS horizontal is previous/next tab.
    // LS retains native analog navigation; RS vertical retains native scrolling.
    if(f.axes[2]<-.55f)p.buttons|=0x100;
    if(f.axes[2]>.55f)p.buttons|=0x200;
    return p;
}
} // namespace
