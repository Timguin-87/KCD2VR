#pragma once
#include "reading_pose26c1.h"
#include "../activity_input163.h"
#include "../core/snapshot_mailbox.h"
#include "../ui_scene_presentation.h"
#include "../diagnostics/reading_receipts32.h"
#include "dice_body_d81.h"

namespace kcd2vr::reading_pose32 {
// A native Reading lifecycle is a separate VISUAL owner. It is not a tool
// experiment, input map, attachment, collision or gameplay permission.
struct Lease {
    std::uint64_t tick=0,lifetime=0;
    std::uintptr_t reading=0,alchemy=0;
    bool allowed=false;
};
inline core::SnapshotMailbox<Lease> visual;
inline std::atomic<bool> blocked{true};
inline bool matches(const Lease& s) noexcept {
    using namespace activity_input163;
    const auto& w=witness175;
    return s.allowed&&s.reading&&w.ready.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire)&&
        s.lifetime==w.lifetime175.load(std::memory_order_acquire)&&
        s.reading==w.active[unsigned(Kind175::Reading)].load(std::memory_order_acquire)&&
        s.alchemy==w.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire)&&
        !w.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)&&
        !w.active[unsigned(Kind175::Sharpening)].load(std::memory_order_acquire);
}
inline Lease qualify(std::uint64_t now,bool selected,bool theater,bool matrix,bool vrMenu,
                     bool dialogue,bool cinematic,bool reference,
                     const ui_menu_router::Decision& ui) noexcept {
    using namespace activity_input163;
    const auto& w=witness175;
    Lease s{};s.tick=now;s.lifetime=w.lifetime175.load(std::memory_order_acquire);
    s.reading=w.active[unsigned(Kind175::Reading)].load(std::memory_order_acquire);
    s.alchemy=w.active[unsigned(Kind175::Alchemy)].load(std::memory_order_acquire);
    // Use the SAME semantic native-context presentation resolver as Mod. A
    // lingering cinematic bit inside an actual immersive minigame is not a
    // third-person cutscene. Owner=None plus that bit still refuses outright.
    const auto scene=ui_scene_presentation::Resolve(ui,cinematic);
    s.allowed=reading_pose26c1::allow(bool(s.reading),selected,theater||scene.theater,matrix,vrMenu,
        dialogue,scene.passive_fullscreen,reference,ui)&&now!=0;
    // Legacy VrReadingHands also governs tool experiments and was auto-saved
    // false in older builds. It must not remove C31's existing visual-body
    // intent or grant a tool owner. Native book fingers remain native below.
    s.allowed=matches(s)&&s.lifetime==w.lifetime175.load(std::memory_order_acquire)&&
        !w.transitionDepth175.load(std::memory_order_acquire);
    return s;
}
inline bool publish(std::uint64_t now,bool selected,bool theater,bool matrix,bool vrMenu,
                    bool dialogue,bool cinematic,bool reference,
                    const ui_menu_router::Decision& ui) noexcept {
    const auto s=qualify(now,selected,theater,matrix,vrMenu,dialogue,cinematic,reference,ui);
    if(!s.allowed)blocked.store(true,std::memory_order_release);
    const bool written=visual.publish(s);
    blocked.store(!written||!s.allowed,std::memory_order_release);
    return written&&s.allowed;
}
// The activity-eye policy is published earlier in Mod than the tracking/body
// packet. Qualify from current copied lifecycle/UI, not last frame's lease.
inline bool nativeContext(bool selected,bool cinematic,std::uint64_t now,
                          const ui_menu_router::Decision& ui) noexcept {
    return ui.owner==ui_menu_router::Owner::NativeContext&&
        qualify(now,selected,false,false,false,false,cinematic,false,ui).allowed;
}
inline bool valid(const Lease& s,std::uint64_t now) noexcept {
    if(blocked.load(std::memory_order_acquire)||!s.tick||now<s.tick||now-s.tick>80||!matches(s))return false;
    Lease current{};
    if(!visual.read(current)||!current.allowed||current.reading!=s.reading||
       current.alchemy!=s.alchemy||current.lifetime!=s.lifetime)return false;
    // A Begin/End racing the first reads cannot carry an older permission.
    return !blocked.load(std::memory_order_acquire)&&
        s.lifetime==activity_input163::witness175.lifetime175.load(std::memory_order_acquire)&&
        !activity_input163::witness175.transitionDepth175.load(std::memory_order_acquire);
}
inline bool read(Lease& s,std::uint64_t now) noexcept {
    return !blocked.load(std::memory_order_acquire)&&visual.read(s)&&valid(s,now);
}
inline bool active(std::uint64_t now) noexcept {Lease s{};return read(s,now);}
inline bool poseAllowed(bool gameplay,bool otherVisual,std::uint64_t now) noexcept {
    return gameplay||otherVisual||active(now)||dice_body_d81::active(now);
}
}
