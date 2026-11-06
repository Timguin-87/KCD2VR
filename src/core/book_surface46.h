#pragma once
#include "snapshot_mailbox.h"
#include <cstdint>
namespace kcd2vr::book_surface46 {
// One copied native book plane and the SAME coherent pre-passenger requested
// wrist positions, all in world metres. Geometry producer owns axis/face proof.
// A generic bounding box without a proven page face must leave valid=false.
struct Surface {
    std::uintptr_t instance=0,action=0;std::uint64_t lifetime=0,tick=0,sequence=0,poseEpoch=0,sourceEpoch=0;
    std::uint32_t entity=0;bool valid=false,handValid[2]{};
    float origin[3]{},right[3]{},up[3]{},normal[3]{};
    float halfWidth=0,halfHeight=0,hand[2][3]{};
};
inline core::SnapshotMailbox<Surface> current;
}
