#include "module.h"
namespace kcd2vr::subsystems::throw_drop {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=15; d.name="Throwing and Drop Weapons"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
