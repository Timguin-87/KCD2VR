#include "module.h"
namespace kcd2vr::subsystems::vrmap {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=13; d.name="VRMap"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
