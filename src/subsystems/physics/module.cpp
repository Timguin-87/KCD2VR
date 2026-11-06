#include "module.h"
namespace kcd2vr::subsystems::physics {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=5; d.name="Physics"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
