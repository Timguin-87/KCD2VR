#include "module.h"
namespace kcd2vr::subsystems::direct {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=3; d.name="Direct"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
