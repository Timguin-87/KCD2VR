#include <atomic>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>
#include "activity_input163.h"
static std::uint64_t GetTickCount64(){return 1000;}
namespace diagnostic_trace {static void Write(const char*){}}
namespace kcd2vr::frik_pose_probe {
std::atomic<bool> g_ctx_dialogue{false},g_ctx_cutscene{false},g_ctx_menu{false},g_ctx_reading{false};
std::atomic<bool> g_ctx_agency_known{false},g_ctx_scripted{false};
std::atomic<int> g_ctx_agency_count{-1},g_f11_theory_phase{0};
std::atomic<std::uint64_t> g_pose_policy_generation57{2},g_pose_policy_flags57{0};
std::atomic<std::uint64_t> g_tool_context19c4r2{0},g_ctx_packed_last_logged{~0ull};
#include "pose_policy57.inc"
}
int main(){
 using namespace kcd2vr;using namespace frik_pose_probe;
 auto& w=activity_input163::witness175;w.Clear();w.ready=true;
 GameStateContext s{};ProduceContext57(s,false);PublishGameStateContext(s);
 const auto initial=g_pose_policy_generation57.load();
 assert(!g_ctx_reading.load()&&!s.hud_hidden);
 ProduceContext57(s,true);PublishGameStateContext(s);
 assert(s.hud_hidden&&!s.native_reading&&!g_ctx_reading.load());
 assert(g_pose_policy_generation57==initial);
 ProduceContext57(s,false);PublishGameStateContext(s);assert(g_pose_policy_generation57==initial);
 w.Begin(activity_input163::Kind175::Reading,0x123,1000);
 ProduceContext57(s,true);PublishGameStateContext(s);
 assert(s.native_reading&&g_ctx_reading.load()&&g_pose_policy_generation57==initial+2);
 w.End(0x123);ProduceContext57(s,true);PublishGameStateContext(s);
 assert(!g_ctx_reading.load()&&g_pose_policy_generation57==initial+4);
 const auto beforeUi=g_pose_policy_generation57.load();
 s.menu=true;PublishGameStateContext(s);assert(g_ctx_menu.load());
 s.menu=false;PublishGameStateContext(s);
 assert(!g_ctx_menu.load()&&g_pose_policy_generation57==beforeUi+4);
 const auto beforeModal=g_pose_policy_generation57.load();
 s.tool_pose_ui_blocked=true;PublishGameStateContext(s);
 s.tool_pose_ui_blocked=false;PublishGameStateContext(s);
 assert(g_pose_policy_generation57==beforeModal+4);
 PublishGameStateContext(s);assert(g_pose_policy_generation57==beforeModal+4);
 assert(!(g_pose_policy_generation57.load()&1));
 std::puts("actual producer/publisher: HUD split, native Reading and UI revocation PASS");
}
