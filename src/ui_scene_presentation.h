#pragma once
#include "ui_menu_router.h"
#include "dialogue_state59c10r2.h"
namespace kcd2vr::ui_scene_presentation {
struct Effective { bool theater=false; bool passive_fullscreen=false; bool pause_overlay=false; bool death_overlay=false; };
// A witnessed local smithing instance may keep its FIRST-PERSON entry view
// immersive. This does not classify a bone/third-person cinematic as gameplay.
inline bool SmithingImmersive43(bool enabled,bool witnessReady,bool smithing,
        const dialogue_state59c10r2::State& view,std::uint64_t now) noexcept {
    return enabled&&witnessReady&&smithing&&dialogue_state59c10r2::fresh(view,now)&&
        !view.active&&view.camera==dialogue_state59c10r2::Camera::FirstPerson;
}
inline bool AuthoredScene(bool cutscene,bool dialogue,int dialogueMode) noexcept {
    return dialogue?dialogueMode==0:cutscene;
}
inline Effective Resolve(const ui_menu_router::Decision& d,bool cutscene) noexcept {
    Effective e{};
    e.pause_overlay=d.owner==ui_menu_router::Owner::Pause;
    e.death_overlay=d.owner==ui_menu_router::Owner::GameOver;
    // A lingering cinematic bit cannot turn an active immersive minigame or
    // overlay into Theater. Genuine cutscenes with no UI owner remain authored.
    const bool authored=cutscene&&d.owner==ui_menu_router::Owner::None;
    e.passive_fullscreen=authored||d.owner==ui_menu_router::Owner::LoadingScreen;
    e.theater=!e.pause_overlay&&!e.death_overlay&&(d.FullFrame()||authored);
    return e;
}
inline float PassiveScale(float requested) noexcept {
    if(requested<1.0f)return 1.0f;
    if(requested>2.0f)return 2.0f; // R107: cutscene / dialogue / loading up to 2x
    return requested;
}
} // namespace kcd2vr::ui_scene_presentation
