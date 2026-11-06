#pragma once
#include <cstdint>
#include "solver_command_h28.h"
#include "gore_cut_d56.h"
namespace kcd2vr::contact59c7g {
// Value copy attached to the queued hit itself; never a sample-only side channel.
struct Metadata {
 std::uintptr_t collider=0;int bodyMaterial=-1,feature=0;bool twoHands=false;int weaponClass=-1;bool shield=false;int originZone47=-1;
 kcd2vr::solver_command_h28::Stamp sourceSolverH28{};
 std::uint64_t occurrenceSerialH28=0;
 gore_cut_d56::Evidence goreCutD56{};
};
}
