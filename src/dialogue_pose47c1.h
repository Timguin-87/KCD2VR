#pragma once
#include "ui_menu_router.h"
namespace kcd2vr::dialogue_pose47c1 {
// Dialogue control loss blocks gameplay, but does not establish a cinematic
// camera. This permission is visual only. Native paired/body actions still win.
struct Context {
 bool enabled=false,vrik=false,dialogue=false;
 int cameraMode=0;
 bool theater=false,matrix=false,vrMenu=false,nativeReference=false,activity=false;
 ui_menu_router::Decision ui{};
};
// Camera selection is an explicit presentation choice, independent of whether
// a calibrated visual body is selected or dialogue hands are enabled.
// The selector still requires fresh native interactive-dialogue ownership.
inline bool CameraRequested28(const Context& c) noexcept {
 return c.dialogue&&c.cameraMode==2&&
  !c.theater&&!c.matrix&&!c.vrMenu&&!c.nativeReference&&!c.activity&&
  c.ui.owner==ui_menu_router::Owner::None&&c.ui.uncertain==0;
}
inline bool Allow(const Context& c) noexcept {
 return c.enabled&&c.vrik&&CameraRequested28(c);
}
}
