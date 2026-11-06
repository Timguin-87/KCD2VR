#pragma once
#include "dialogue_state59c10r2.h"

namespace kcd2vr::npc_lookat_hmd {

bool Install() noexcept;
void Uninstall() noexcept;
void SetEnabled(bool enabled) noexcept;
void SetDialogueActive(bool active) noexcept;
bool Enabled() noexcept;
// Current native conversation witness, independent of gameplay/body permission.
// An admitted scene survives disabled option input; source time is never renewed.
void PublishDialogueOwner63(const dialogue_state59c10r2::State&,bool embodied) noexcept;

} // namespace kcd2vr::npc_lookat_hmd
