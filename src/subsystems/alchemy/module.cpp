#include "module.h"
namespace kcd2vr::subsystems::alchemy {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=7; d.name="Alchemy"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
