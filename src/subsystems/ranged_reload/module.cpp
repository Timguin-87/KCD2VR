#include "module.h"
namespace kcd2vr::subsystems::ranged_reload {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=16; d.name="Ranged Reload"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
