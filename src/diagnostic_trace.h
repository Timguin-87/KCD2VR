#pragma once
#include "diagnostics/capture168.h"
namespace kcd2vr::diagnostic_trace {
void SetEnabled(bool enabled) noexcept;
bool Enabled() noexcept;
// Cheap mask test before clock/formatting work; finite five-recorder policy.
bool EnabledFor(diagnostics::capture168::Domain domain) noexcept;
void WriteFor(diagnostics::capture168::Domain domain,const char* message) noexcept;
// Queued records retain their original lease; only the bounded drain grace applies.
void WriteReceiptFor(diagnostics::capture168::Domain domain,std::uint64_t session,const char* message) noexcept;
void Write(const char* message) noexcept;
// Bounded lifecycle/fault receipts only. Callers rate-limit; no screen or extra file.
// High-rate measurements must continue through the F-key gated writers.
void WriteStatus(const char* message) noexcept;
// Independent Shift+F2 finite metrics whitelist; does not enable any recorder group.
void WritePerformanceD61(const char* message) noexcept;
// Worker-only copied Shift+F2 motion bursts; independent of F2/F7 capture bits.
void WritePerformanceD68(const char* message) noexcept;
// Worker-only F2/Shift+F2 map report; shared KCD2VR.log sink and finite gate.
void WriteMapPerformanceD72(const char* message) noexcept;
// D30: records accepted into the file queue since start (F2 FRAME logLinesPerS).
unsigned long long Accepted() noexcept;
// Call only from Mod::Run worker, not a native hook. Shutdown drains before close.
void Drain(bool all=false) noexcept;
}
