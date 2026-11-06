#pragma once
#include "../subsystems/smithing/native_floor_d56.h"
// Pose-owner-only history for the last published smith wrist. The coherent
// transaction copies this before solving and commits it only after publication.
// Rejected/native-restore-failed poses discard their copy without restoring old
// global state over a newer publisher or refunding consumed world-query budget.
// V3, Q4 and ToolSocket59c10 are supplied by the production adapter context.
struct SmithToolContactJ27State {
    std::uint32_t id=0,boundsId=0,ironId=0;std::uint64_t epoch=0;
    ToolSocket59c10 owner{};std::uint64_t lastTick=0;
    bool have=false,boundsOk=false,ironOk=false;V3 lastGoal{};Q4 lastQ{0,0,0,1};float bounds[6]{},ironBounds[6]{};
};
struct SmithAnvilContactA2State {
    std::uint32_t id=0,boundsId=0,ironId=0;std::uint64_t epoch=0;
    bool have=false,boundsOk=false,ironOk=false,corrected=false;
    ToolSocket59c10 owner{};std::uint64_t smithEpoch=0,lastTick=0;
    V3 lastGoal{};Q4 lastQ{0,0,0,1};float bounds[6]{},ironBounds[6]{};
};
struct SmithContactHistoryR38 {
    SmithToolContactJ27State world[2]{};
    SmithAnvilContactA2State anvil[2]{};
    kcd2vr::subsystems::smithing::native_floor_d56::State nativeFloorD56{};
};
static SmithContactHistoryR38 g_smith_contact_history_r38{};
static auto& g_smith_tool_contact_j27=g_smith_contact_history_r38.world;
static auto& g_smith_anvil_contact_a2=g_smith_contact_history_r38.anvil;
static SmithContactHistoryR38 ReadSmithContactHistoryR38() noexcept {return g_smith_contact_history_r38;}
static void CommitSmithContactHistoryR38(const SmithContactHistoryR38& staged) noexcept {
    g_smith_contact_history_r38=staged;
}
// A rejected pose did still perform its reads/logs. Keep attempt throttles out
// of the staged wrist history so rejection cannot turn failed reads into an
// every-frame retry or undo the diagnostic rate limit. These are pose-thread
// bookkeeping only; they never authorize a tool, contact, or native action.
struct SmithContactAttemptsR38 {
    ToolSocket59c10 owner{};
    std::uint32_t boundsId=0,ironId=0;
    std::uint64_t boundsFailedAt=0,ironFailedAt=0,lastLog=0;
};
static SmithContactAttemptsR38 g_smith_world_attempts_r38[2]{},g_smith_anvil_attempts_r38[2]{};
static SmithContactAttemptsR38& SmithAttemptsForOwnerR38(SmithContactAttemptsR38& a,const ToolSocket59c10& socket) noexcept {
    if(a.owner.character!=socket.character||a.owner.epoch!=socket.epoch||a.owner.id!=socket.id||
       a.owner.request!=socket.request||a.owner.modeEpoch67!=socket.modeEpoch67||
       !kcd2vr::smith_tool36::sameGripIdentity67(a.owner.smith36,socket.smith36)){
        const auto lastLog=a.lastLog;a=SmithContactAttemptsR38{};a.owner=socket;a.lastLog=lastLog;
    }
    return a;
}
static bool SmithBoundsRetryDueR38(std::uint32_t id,std::uint64_t now,std::uint32_t failedId,std::uint64_t failedAt) noexcept {
    return id!=failedId||!failedAt||now<failedAt||now-failedAt>=100;
}
