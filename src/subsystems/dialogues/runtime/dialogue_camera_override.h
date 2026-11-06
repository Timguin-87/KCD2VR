#pragma once

namespace kcd2vr::dialogue_camera_override {
// Retail parser for esDialogueCameraOverrideType. The hook is read/write only at
// the function return: native parsing/cleanup still runs, then the selected enum
// is substituted while an actual dialogue is active.
bool Install() noexcept;
void Uninstall() noexcept;
// 0 = native/NoOverride, 1 = Medium, 2 = CloseShot, 3 = Closeup, 4 = Master.
void SetDialogueActive(bool active) noexcept;
void SetOverrideType(int type) noexcept;
// V65.03 F5 diagnostic: force parser result NoOverride (0) while dialogue is active.
void SetForceNoOverride(bool enabled) noexcept;
// Latest native parsed dialogue camera category before any override.
// 0=NoOverride, 1=Medium, 2=CloseShot, 3=Closeup, 4=Master.
int LastNativeType() noexcept;
}
