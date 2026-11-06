#pragma once
#include <atomic>
namespace kcd2vr::alchemy_tool_contact_d55 {
// Opt-in bounds proxy only: no rigid-body/native action owner, no cached hold.
// Off returns before bounds/world queries; each enabled solve uses the current
// completed tool/socket plus the existing shared per-sample query budget.
inline std::atomic<bool> enabled{false};
}
