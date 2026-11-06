#include "module.h"
namespace kcd2vr::subsystems::experimental {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=17; d.name="Experimental"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
