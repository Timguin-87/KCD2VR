#pragma once
// Copied F5 evidence only. These fields never select a target, authorize a
// gesture, alter a transform, or certify a native recipe/heat result.
#include "../../diagnostics/capture168.h"
#include "../../diagnostics/interaction_receipt59c10.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>

namespace kcd2vr::subsystems::smithing::bellows_geometry_d51 {
struct Evidence {
    std::uintptr_t entity=0,character=0,skeleton=0,attachment=0;
    std::uintptr_t sourceModel=0,holder=0,nativeAction=0;
    std::uint64_t nativeKey=0;std::uint32_t host=0,handType[2]{};
    int slot=-1,side=-1,attached=-1;bool actionKnown=false,handTypesKnown=false;
    unsigned stage=0;int joint=-1;bool physicalHandleJointD53=false;
    bool entityKnown=false,modelKnown=false,jointKnown=false,relativeKnown=false,
         rootKnown=false,branchKnown=false,cachedKnown=false;
    float entityWorld[12]{},modelWorld[8]{},jointModel[7]{},relative[7]{},
          rootModel[7]{},branchModel[7]{},cachedModel[7]{},pointModel[3]{},pointWorld[3]{};
};
// Check the finite recorder before the caller performs ANY supplementary read.
// 500 ms cadence gives at most 360 ordinary samples in a 180 s capture.
struct Gate {
    diagnostics::interaction_receipt59c10::Gate gate;
    bool begin(std::uintptr_t instance,std::uint32_t entity,int kind,std::uint64_t now) noexcept {
        namespace capture=diagnostics::capture168;
        if(!capture::active(capture::Domain::Interaction,now))return false;
        const auto session=capture::gate.session[unsigned(capture::Domain::Interaction)].load(std::memory_order_acquire);
        return gate.observe(session,now,{instance,entity,kind,0},500,512,500);
    }
};
inline int formatTransform(char* line,std::size_t size,const char* kind,std::uint64_t session,
                           std::uint64_t tick,std::uintptr_t instance,std::uint64_t life,
                           std::uint32_t id,const Evidence& e) noexcept {
    return std::snprintf(line,size,
        "D51 F5_BELLOWS_TRANSFORM kind=%s session=%llu tick=%llu instance=%llX life=%llu entity=%u entityPtr=%llX character=%llX skeleton=%llX attachment=%llX stage=%u joint=%d known=%d,%d,%d,%d entityWorld=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g characterWorld=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g jointModel=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g socketRelative=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g pointModel=%.8g,%.8g,%.8g pointWorld=%.8g,%.8g,%.8g target=%s writes=NONE",
        kind,(unsigned long long)session,(unsigned long long)tick,(unsigned long long)instance,(unsigned long long)life,id,
        (unsigned long long)e.entity,(unsigned long long)e.character,(unsigned long long)e.skeleton,(unsigned long long)e.attachment,
        e.stage,e.joint,e.entityKnown,e.modelKnown,e.jointKnown,e.relativeKnown,
        e.entityWorld[0],e.entityWorld[1],e.entityWorld[2],e.entityWorld[3],e.entityWorld[4],e.entityWorld[5],e.entityWorld[6],e.entityWorld[7],e.entityWorld[8],e.entityWorld[9],e.entityWorld[10],e.entityWorld[11],
        e.modelWorld[0],e.modelWorld[1],e.modelWorld[2],e.modelWorld[3],e.modelWorld[4],e.modelWorld[5],e.modelWorld[6],e.modelWorld[7],
        e.jointModel[0],e.jointModel[1],e.jointModel[2],e.jointModel[3],e.jointModel[4],e.jointModel[5],e.jointModel[6],
        e.relative[0],e.relative[1],e.relative[2],e.relative[3],e.relative[4],e.relative[5],e.relative[6],
        e.pointModel[0],e.pointModel[1],e.pointModel[2],e.pointWorld[0],e.pointWorld[1],e.pointWorld[2],
        e.physicalHandleJointD53?"EVALUATED_HANDLE_JOINT":"AUTHORED_SOURCE");
}
inline int formatOwner(char* line,std::size_t size,const char* kind,std::uint64_t session,
                       std::uint64_t tick,std::uintptr_t instance,std::uint64_t life,
                       std::uint32_t id,const Evidence& e) noexcept {
    return std::snprintf(line,size,
        "D51 F5_BELLOWS_OWNER kind=%s session=%llu tick=%llu instance=%llX life=%llu entity=%u sourceModel=%llX holder=%llX nativeKey=%llX host=%u slot=%d side=%d attached=%d actionKnown=%d nativeAction=%llX handTypesKnown=%d handType=%u,%u writes=NONE",
        kind,(unsigned long long)session,(unsigned long long)tick,(unsigned long long)instance,(unsigned long long)life,id,
        (unsigned long long)e.sourceModel,(unsigned long long)e.holder,(unsigned long long)e.nativeKey,e.host,e.slot,e.side,e.attached,
        e.actionKnown,(unsigned long long)e.nativeAction,e.handTypesKnown,e.handType[0],e.handType[1]);
}
inline int formatRig(char* line,std::size_t size,const char* kind,std::uint64_t session,
                    std::uint64_t tick,std::uint32_t id,const Evidence& e) noexcept {
    return std::snprintf(line,size,
        "D51 F5_BELLOWS_RIG kind=%s session=%llu tick=%llu entity=%u knownRootBranchCached=%d,%d,%d rootJoint0=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g branchJoint1=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g cachedAttachmentModel=%.8g,%.8g,%.8g,%.8g,%.8g,%.8g,%.8g cachedPoseUse=DIAGNOSTIC_COMPARISON_ONLY writes=NONE",
        kind,(unsigned long long)session,(unsigned long long)tick,id,e.rootKnown,e.branchKnown,e.cachedKnown,
        e.rootModel[0],e.rootModel[1],e.rootModel[2],e.rootModel[3],e.rootModel[4],e.rootModel[5],e.rootModel[6],
        e.branchModel[0],e.branchModel[1],e.branchModel[2],e.branchModel[3],e.branchModel[4],e.branchModel[5],e.branchModel[6],
        e.cachedModel[0],e.cachedModel[1],e.cachedModel[2],e.cachedModel[3],e.cachedModel[4],e.cachedModel[5],e.cachedModel[6]);
}
}
