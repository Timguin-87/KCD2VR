#include "module.h"
namespace kcd2vr::subsystems::masterstrike_combos {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=4; d.name="Masterstrike and Combos"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
