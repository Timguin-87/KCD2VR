#include "module.h"
namespace kcd2vr::subsystems::smithing {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=8; d.name="Smithing"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
