#include "module.h"
namespace kcd2vr::subsystems::dialogues {
const ModuleDescriptor& Descriptor() noexcept {
 static const ModuleDescriptor descriptor=[] { ModuleDescriptor d{}; d.id=10; d.name="Dialogues"; d.lifecycle=Lifecycle::HostOwned; return d; }();
 return descriptor;
}
}
