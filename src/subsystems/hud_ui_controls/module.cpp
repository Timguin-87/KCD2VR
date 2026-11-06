#include "module.h"
namespace kcd2vr::subsystems::hud_ui_controls {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=11; d.name="HUD UI Gestures and Controls"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
