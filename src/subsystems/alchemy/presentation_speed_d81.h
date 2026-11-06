#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::alchemy::presentation_d81 {
enum class Kind {None,Transfer,Book,Resource,Work};
// D85R5R1: this slider is hand pickup/return speed, not a station or work clock.
// Only exact pinned Take/ReturnObject and Take/Return/PlaceIngredient parents
// are transfers. Pouring, pot movement and mortar work retain their own time.
inline Kind transferKind(std::uintptr_t parentVftRva) noexcept {
    switch(parentVftRva){
    case 0x3A5C308:case 0x3A5C4C0:case 0x3B87A38:
    case 0x3A5CCD0:case 0x3C23010:return Kind::Transfer;
    default:return Kind::Work;
    }
}
// Speed the existing native action clock, which also propagates to its scope,
// procedural work and CAFs. A tracked work lease always wins over presentation.
// Bound a single native update to 80 ms of authored time: no normalized seek,
// fabricated event or replacement of the engine's Update argument.
inline float rate(float requested,float dt,float controllerScale) noexcept {
    if(!std::isfinite(requested)||!std::isfinite(dt)||!std::isfinite(controllerScale)||
       requested<=1.f||dt<=0.f||dt>.08f||controllerScale<=0.f||controllerScale>8.f)return 1.f;
    return std::max(1.f,std::min(std::clamp(requested,1.f,8.f),.08f/(dt*controllerScale)));
}
inline bool allowed(Kind kind,bool current,bool trackedWork,bool exactResourceWork) noexcept {
    (void)exactResourceWork;
    // Resource reach/return is admitted separately by the exact carafe hand
    // witness. Generic resources (including work) never inherit this rate.
    return current&&!trackedWork&&kind==Kind::Transfer;
}
// Native attachment requests can finish in a later callback for all ordinary
// props and special ingredients, not only the left flask. This is observation
// grace after one semantic call; it never grants another inventory operation.
inline bool attachmentAction(unsigned action,int source,int side) noexcept {
    const bool object=source==0||source==1||source==4||(source==5&&side==1);
    const bool takeOrReturnObject=action==1||action==2;
    const bool special=source>=10&&source<=12&&side==0;
    const bool takeOrReturnIngredient=action==3||action==4;
    return (object&&takeOrReturnObject)||(special&&takeOrReturnIngredient);
}
}
