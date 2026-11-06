#pragma once
#include "ui_contextual_hud162r1.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <cmath>

namespace kcd2vr::ui_hud_visibility175 {
inline constexpr unsigned Group(unsigned i) noexcept {
    if(i>=ui_contextual_hud162r1::Clips.size()||i==20||i==21)return 0;
    // R106 (Miki): QAMWeapon/QAMFood (3,4) are NEVER owned, in every HUD mode - the game shows them when the
    // selection changes and fades them itself; the immersive HUD must not keep them, hide them or restore them.
    // CombatCursorCross (13) is native too: the game only draws it in a fight, where it marks the targeted enemy;
    // the centre-point preference keeps owning the ordinary cursors (14,15,19) - alchemy gaze pins 19 as before.
    if(i==3||i==4||i==13)return 0;
    // Retail HUD.xml: Cursor is actioncursor, including its unnamed central
    // artwork. InfoText, Bubbles and DiceCursor are separate siblings. Own the
    // cursor family, never the whole cccursor parent or interaction/name text.
    if(i==14||i==15||i==19)return ui_contextual_hud162r1::HideCrosshair163;
    auto group=ui_contextual_hud162r1::Clips[i].group;
    // These are status badges too. Retain their historical lower-group control,
    // but a user's status-off preference must also survive a right-palm reveal.
    if(i==5||i==6||i==7)group|=ui_contextual_hud162r1::HideStatus163;
    return group;
}
// Rung 3 additionally owns gameplay cursors, but never prompts/subtitles.
inline constexpr unsigned ExperimentGroup36(unsigned i,bool corrected,bool contextual) noexcept {
    if(!corrected)return Group(i);
    if(i==3||i==4||i==13)return 0; // native quick-selection UI and the fight cursor are functional, not decoration
    if(contextual&&(i==15||i==19))return ui_contextual_hud162r1::Other|ui_contextual_hud162r1::HideCrosshair163;
    return Group(i);
}
struct Key {
    std::uintptr_t wrapper=0,handle=0,object=0,hud=0;
    std::uint64_t generation=0;
};
inline bool Same(const Key&a,const Key&b) noexcept {
    return a.wrapper==b.wrapper&&a.handle==b.handle&&a.object==b.object&&
        a.hud==b.hud&&a.generation==b.generation;
}
struct Decision {bool qualified=false,visible=false,suppressed=false;unsigned index=0;};
// A fixed identity registry, populated only on qualified native child resolution.
// No retained engine ownership, pointer dereference, timer, or renderer callback.
// Native callbacks try once; a contended binding is unavailable, never waited on
// while an engine movie lock may be held. Every member shared across threads is
// either atomic or protected by the per-entry try gate.
class Registry {
    struct Entry {
        std::atomic_flag busy=ATOMIC_FLAG_INIT;
        std::atomic<std::uintptr_t> object{0},wrapper{0};
        Key key{};std::uint64_t epoch=0;int native=-1;float nativeAlpha54=-1;bool restricted=false;
    };
    std::array<Entry,ui_contextual_hud162r1::Clips.size()> entries_{};
    std::atomic<std::uint64_t> epoch_{1},revision54_{1};
public:
    std::uint64_t Epoch()const noexcept{return epoch_.load(std::memory_order_acquire);}
    std::uint64_t Revision54()const noexcept{return revision54_.load(std::memory_order_acquire);}
    void Invalidate()noexcept{epoch_.fetch_add(1,std::memory_order_acq_rel);}
    bool Bind(unsigned i,const Key& key,bool corrected=false,bool contextual=false) noexcept {
        if(i>=entries_.size()||!ExperimentGroup36(i,corrected,contextual)||!key.wrapper||!key.handle||!key.object||!key.hud)return false;
        auto&e=entries_[i];if(e.busy.test_and_set(std::memory_order_acquire))return false;
        const auto epoch=Epoch();
        if(e.epoch!=epoch||!Same(e.key,key)){
            e.object.store(0,std::memory_order_release);e.key=key;e.epoch=epoch;e.native=-1;e.nativeAlpha54=-1;e.restricted=false;
            e.wrapper.store(key.wrapper,std::memory_order_release);
            e.object.store(key.object,std::memory_order_release);++revision54_;
        }
        e.busy.clear(std::memory_order_release);return true;
    }
    void RemoveWrapper(std::uintptr_t wrapper) noexcept {
        if(!wrapper)return;
        for(auto&e:entries_)if(e.wrapper.load(std::memory_order_acquire)==wrapper){
            if(e.busy.test_and_set(std::memory_order_acquire)){
                // Destruction must invalidate even a contended binding. A later
                // native resolution may bind the new epoch; no stale retry.
                Invalidate();continue;
            }
            if(e.key.wrapper==wrapper){e.object.store(0,std::memory_order_release);
                e.wrapper.store(0,std::memory_order_release);e.key={};e.native=-1;e.nativeAlpha54=-1;++revision54_;}
            e.busy.clear(std::memory_order_release);
        }
    }
    bool NativeMeaning(unsigned i,const Key& key,bool current,bool& native,bool* restricted=nullptr) noexcept {
        if(i>=entries_.size())return false;
        auto&e=entries_[i];if(e.busy.test_and_set(std::memory_order_acquire))return false;
        const bool valid=e.epoch==Epoch()&&Same(e.key,key);
        if(valid){if(e.native<0)e.native=current?1:0;native=e.native!=0;if(restricted)*restricted=e.restricted;}
        e.busy.clear(std::memory_order_release);return valid;
    }
    bool NativeAlphaMeaning54(unsigned i,const Key& key,float current,float& native) noexcept {
        if(i>=entries_.size()||!std::isfinite(current)||current<0||current>100)return false;
        auto&e=entries_[i];if(e.busy.test_and_set(std::memory_order_acquire))return false;
        const bool valid=e.epoch==Epoch()&&Same(e.key,key);
        if(valid){if(e.nativeAlpha54<0)e.nativeAlpha54=current;native=e.nativeAlpha54;}
        e.busy.clear(std::memory_order_release);return valid;
    }
    struct Alpha54 {bool qualified=false;float value=0;unsigned index=0;};
    template<class Validate> Alpha54 NativeAlphaRequest54(const Key& key,float requested,unsigned hidden,
        bool enabled,bool palmActive,bool left,bool right,Validate validate,bool gazeCursor=false) noexcept {
        Alpha54 out{};out.value=requested;if(!std::isfinite(requested)||requested<0||requested>100)return out;
        for(unsigned i=0;i<entries_.size();++i){auto&e=entries_[i];
            if(e.object.load(std::memory_order_acquire)!=key.object)continue;
            if(e.busy.test_and_set(std::memory_order_acquire))continue;
            const bool valid=e.epoch==Epoch()&&Same(e.key,key)&&validate(e.key);
            if(valid){e.nativeAlpha54=requested;out.qualified=true;out.index=i;
                // Only semantic palm gates. Quick selectors, prompts, full-field
                // faders/vignettes and every position/scale property stay native.
                if(enabled&&gazeCursor&&i==19&&!(hidden&ui_contextual_hud162r1::HideCrosshair163))out.value=100;
                if(enabled&&palmActive&&(i==0||i==1||i==2||i==5||i==6||i==7)){
                    if(hidden&Group(i))out.value=0;
                    else if((i==0&&left)||(i==1&&right))out.value=100;
                }
            }
            e.busy.clear(std::memory_order_release);if(valid)return out;
        }
        return out;
    }
    template<class Validate> Decision NativeRequest(std::uintptr_t object,bool requested,
        std::uintptr_t hud,std::uint64_t generation,unsigned hidden,bool enabled,Validate validate,bool corrected=false,bool contextual=false) noexcept {
        Decision out{};out.visible=requested;
        if(!object||!hud)return out;
        for(unsigned i=0;i<entries_.size();++i){auto&e=entries_[i];
            if(e.object.load(std::memory_order_acquire)!=object)continue;
            if(e.busy.test_and_set(std::memory_order_acquire))continue;
            const bool valid=e.epoch==Epoch()&&e.key.object==object&&e.key.hud==hud&&
                e.key.generation==generation&&validate(e.key);
            if(valid){
                e.native=requested?1:0;out.qualified=true;out.index=i;
                out.visible=requested&&!(enabled&&(hidden&ExperimentGroup36(i,corrected,contextual)));
                out.suppressed=requested&&!out.visible;e.restricted=out.suppressed;
            }
            e.busy.clear(std::memory_order_release);
            if(valid)return out;
        }
        return out;
    }
};
} // namespace kcd2vr::ui_hud_visibility175
