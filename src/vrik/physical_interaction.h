#pragma once
#include <cstdint>
#include <algorithm>

namespace kcd2vr::physical_interaction106 {
inline float EffectiveReach(float native_reach,bool hand_active,float hand_limit) noexcept {
    if(!hand_active||hand_limit<=0.05f)return native_reach;
    return std::min(native_reach,std::clamp(hand_limit,0.08f,2.0f));
}
inline float EffectiveRadius(float normal_radius,bool hand_active,float hand_radius) noexcept {
    const float r=hand_active&&hand_radius>0.02f?hand_radius:normal_radius;
    return std::clamp(r,0.03f,0.30f);
}


struct HandInput {
    bool tracked=false;
    bool empty=false;
    bool neutral=true;
    float grip=0.0f;
    // Optional delayed-tap mode retained as a generic policy tool. V66.108 no longer
    // uses it for Left Grip; both grips are immediate physical interaction inputs.
    bool tap=false;
};
struct Input {
    bool enabled=false;
    bool menu=false;
    std::uint64_t now_ms=0;
    HandInput hand[2]{}; // 0 right, 1 left
    // V66.178R7: how long this hand's passive hover ray has already owned KCD2's
    // interactor (ms). The native look-ray query completes asynchronously; a Use
    // pulse 20 ms after a brand-new hand ray raced it (no accepted hand target in
    // any recorded run). With an established hover the target is already selected.
    std::uint64_t hover_age_ms[2]{};
};
struct HandState { bool down=false,rearm=true; std::uint64_t pressedAt=0; bool tapValid=false; };
struct State {
    HandState hand[2]{};
    int ray_owner=-1;
    std::uint64_t ray_until_ms=0;
    std::uint64_t use_at_ms=0;
    bool use_pending=false;
};
struct Output {
    int ray_owner=-1;
    bool ray_active=false;
    bool fire_use=false;
    bool simultaneous_reject=false;
};
// Retry only inside the existing ray lease if publication/transport rejected the
// use pulse. A render-frame gap must not silently consume the sole Grip edge.
inline void CompleteUse116(State& s,const Output& out,bool dispatched) noexcept {
    if(out.fire_use&&!dispatched&&out.ray_active&&s.ray_owner==out.ray_owner)s.use_pending=true;
}
inline Output Step(State& s,const Input& in) noexcept {
    constexpr float press=.58f, release=.30f;
    Output out{};
    bool candidate[2]{};
    for(int i=0;i<2;++i){
        const bool down=in.hand[i].grip>press;
        if(!in.hand[i].tap){
            if(in.hand[i].grip<release)s.hand[i].rearm=true;
            candidate[i]=in.enabled&&!in.menu&&in.hand[i].tracked&&in.hand[i].empty&&in.hand[i].neutral&&down&&!s.hand[i].down&&s.hand[i].rearm;
            s.hand[i].down=down;
        } else {
            const bool released=in.hand[i].grip<release;
            if(down&&!s.hand[i].down){s.hand[i].pressedAt=in.now_ms;s.hand[i].tapValid=true;s.hand[i].down=true;}
            if(s.hand[i].down&&(!in.hand[i].neutral||!in.hand[i].tracked||!in.hand[i].empty||in.now_ms-s.hand[i].pressedAt>400))s.hand[i].tapValid=false;
            candidate[i]=false;
            if(s.hand[i].down&&released){
                candidate[i]=in.enabled&&!in.menu&&s.hand[i].tapValid&&in.hand[i].tracked&&in.hand[i].empty&&in.hand[i].neutral;
                s.hand[i].down=false;s.hand[i].tapValid=false;
            }
        }
    }
    if(!in.enabled||in.menu){
        s.ray_owner=-1;s.ray_until_ms=0;s.use_at_ms=0;s.use_pending=false;
        for(int i=0;i<2;++i) if(s.hand[i].down) s.hand[i].rearm=false;
        return out;
    }
    if(candidate[0]&&candidate[1]){
        s.hand[0].rearm=s.hand[1].rearm=false;out.simultaneous_reject=true;
    } else {
        const int c=candidate[0]?0:(candidate[1]?1:-1);
        if(c>=0){
            if(!in.hand[c].tap)s.hand[c].rearm=false;s.ray_owner=c;
            const std::uint64_t settle=in.hover_age_ms[c]>=100ull?20ull:120ull; // V66.178R7
            s.ray_until_ms=in.now_ms+350ull+(settle-20ull);s.use_at_ms=in.now_ms+settle;s.use_pending=true;
        }
    }
    if(s.ray_owner>=0){
        const int o=s.ray_owner;
        // Grip release cancels an unsubmitted delayed Use. The optional tap
        // policy deliberately fires after release and retains its old contract.
        if(in.now_ms>=s.ray_until_ms||!in.hand[o].tracked||!in.hand[o].empty||
           (!in.hand[o].tap&&in.hand[o].grip<release)){s.ray_owner=-1;s.ray_until_ms=0;s.use_at_ms=0;s.use_pending=false;}
    }
    out.ray_owner=s.ray_owner;out.ray_active=s.ray_owner>=0&&in.now_ms<s.ray_until_ms;
    if(out.ray_active&&s.use_pending&&in.now_ms>=s.use_at_ms){out.fire_use=true;s.use_pending=false;}
    return out;
}
}
