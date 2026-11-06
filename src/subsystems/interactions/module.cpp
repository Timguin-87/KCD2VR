#include "module.h"
namespace kcd2vr::subsystems::interactions {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=6; d.name="Interactions"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
