#include "module.h"
namespace kcd2vr::subsystems::hybrid {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=2; d.name="Hybrid"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
