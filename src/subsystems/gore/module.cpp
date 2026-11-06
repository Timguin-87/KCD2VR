#include "module.h"
namespace kcd2vr::subsystems::gore {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=14; d.name="Gore"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
