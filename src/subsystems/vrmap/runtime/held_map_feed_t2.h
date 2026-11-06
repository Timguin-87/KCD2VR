#pragma once
#include "held_map_snapshot_t2.h"
namespace kcd2vr::held_map_feed_t2 {
bool Install() noexcept;
// Copied current native Reading witness; no UI manager lookup or native calls.
void NativeBookTick(std::uint64_t now,std::uintptr_t reading,std::uint64_t lifetime,bool allowed) noexcept;
// Native presentation publishes desire only. All VM/native data service occurs
// on the sole shared original-first script-update scheduler, at most2Hz.
void Tick(bool visible,std::uint64_t now) noexcept;
// Before a helper is visible, a fresh current committed LEFT frame may ask the
// native script owner for bounded helper readiness. id=0 revokes this lease.
void RequestReadiness(std::uint32_t actor,std::uint64_t epoch,std::uint64_t sourceTick,std::uint64_t now,std::uint32_t helperId=0) noexcept;
// Native presenter only, after an exact ABSENT lookup qualified by the same
// current fresh pose actor/epoch/activation. Invalidates one successful ACK once;
// missing/failed/terminal attempts and UI/tracking hides do not rearm it. A NEW
// RequestReadiness with original <=80ms source age is still required. No VM call.
bool ReportHelperMissing(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation) noexcept;
// Exact frozen-open preparation receipt, joined to current helper/actor/VM.
// M41: exact current bootstrap ACK -> expected native static slot filename.
bool StaticModelM41(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation,std::uint32_t helperId,char (&path)[160]) noexcept;
bool LetterReadyD44(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation,std::uint64_t now) noexcept;
bool Read(Snapshot&,std::uint64_t now) noexcept;
}
