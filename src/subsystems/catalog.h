#pragma once
#include "subsystem.h"
namespace kcd2vr::subsystems {
inline constexpr const char* kBaseContract="KCD2VR-M2-C66-STATIC-2";
Registry& Catalog() noexcept;
Status StartCatalog(const HostServices&) noexcept;
Status StopCatalog() noexcept;
}
