#pragma once
#include "subsystems/subsystem.h"
namespace kcd2vr::subsystems::doors {
const ModuleDescriptor& Descriptor() noexcept;

// The existing host worker evaluates its current admission chain once, in its
// original short-circuit order. This copied verdict grants no native lifetime.
struct ServiceAdmission { bool wanted=false; };
struct ServiceLeaves {
    bool (*ready)() noexcept=nullptr;
    bool (*set_enabled)(bool) noexcept=nullptr;
    void (*start)() noexcept=nullptr;
    bool (*stop)() noexcept=nullptr;
    void (*write)(const char*) noexcept=nullptr;
};
struct ServiceState {
    bool previous_wanted=false;
    std::uint64_t retry_after=0;
    int reported_bridge=-1;
};
// Existing worker scheduler only; no hook installation, engine pointers or VM
// calls are introduced here. Stop is a request to the retained native owners,
// not their cleanup ACK. A missing provider leaves the service state untouched.
bool TickService(ServiceState&,ServiceAdmission,std::uint64_t now,
                 const ServiceLeaves&) noexcept;
// Owns the one production worker-service state. Do not call concurrently or
// create a second service instance for the same native door owner.
bool TickService(ServiceAdmission,std::uint64_t now,const ServiceLeaves&) noexcept;
}
