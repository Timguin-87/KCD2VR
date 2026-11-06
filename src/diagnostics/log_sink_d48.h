#pragma once
#include "capture168.h"
#include "../diagnostic_trace.h"
// Keep the auxiliary DetourModKit sink under the same finite recorder policy.
// The guard encloses argument evaluation too, so OFF does not format or fetch
// values solely for an auxiliary log. do/while preserves surrounding if/else.
#define KCD2VR_DIAG_LOG(domain, severity, ...) do { \
    if (::kcd2vr::diagnostic_trace::EnabledFor(::kcd2vr::diagnostics::capture168::Domain::domain)) \
        ::DetourModKit::log().severity(__VA_ARGS__); \
} while (false)
