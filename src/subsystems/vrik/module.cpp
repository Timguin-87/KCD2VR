#include "module.h"
namespace kcd2vr::subsystems::vrik {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=12; d.name="VRIK"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
