import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys,json,hashlib
root=Path(__file__).resolve().parents[1];dst=root/'tests'
path=root/'src/interaction_alignment.cpp';src=read_source(path)
def func(name,text=src):
 start=text.index(name);brace=text.index('{',start);depth=1;i=brace+1
 while depth:
  if text[i]=='{':depth+=1
  if text[i]=='}':depth-=1
  i+=1
 return text[start:i],text[:start].count('\n')+1,text[:i].count('\n')+1
names=['static void SelectionReceipt59c10(', 'static void PublishHandCandidate178r16(', 'static void DoorCandidateObservation38(', 'char __fastcall OnScreenDetour(', 'void SetEnabled(', 'void SetVisualFallbackEnabled(', 'void SetNativeActivityFallback5916c3(', 'void SetHandRayWorld(', 'void ClearLegacyHandRayWorldH5(', 'bool HandRayActive(', 'void SetPalmRays59c9(', 'bool LastHandCandidate(', 'bool ExactDoorCandidate38(', 'std::uint64_t DoorSourceGeneration38(', 'bool DoorSourceCurrent38(']
parts=[];meta=[]
# ExactDoorCandidate now shares the real direct/projection candidate lookup.
# Extract its actual helpers rather than supply an always-refusing stub.
direct=read_source(root/'src/interaction_direct_door54r1.inl')
for n in ['static bool CopyDirectDoor54r1Locked(', 'static bool ReadExactDoor54r1(']:
 code,a,b=func(n,direct);parts.append(code);meta.append({'function':n,'source':'src/interaction_direct_door54r1.inl','lines':[a,b],'sha256':hashlib.sha256(code.encode()).hexdigest()})
for n in names:
 code,a,b=func(n);parts.append(code);meta.append({'function':n,'source':'src/interaction_alignment.cpp','lines':[a,b],'sha256':hashlib.sha256(code.encode()).hexdigest()})
pre='''// Complete verbatim production functions. Native input and ray metadata are explicit fixtures.
#include <windows.h>
#include <intrin.h>
#include <atomic>
#include <cmath>
#include <mutex>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include "diagnostics/capture168.h"
#include "diagnostics/interaction_receipt59c10.h"
namespace kcd2vr::diagnostic_trace {inline std::vector<std::string> receipts;inline void Write(const char* line){receipts.emplace_back(line);}}
#include "vrik/physical_interaction.h"
#include "vrik/interaction_ray112.h"
#include "vrik/interaction_cone57c1.h"
#include "core/snapshot_mailbox.h"
#include "core/door_candidates38.h"
#include "interaction_alignment.h"
#include "subsystems/alchemy/native_selection68.h"
// This fixture exercises the ordinary/native-reference selector. It has no
// actual alchemy object-selector invocation or tracked crafting frame. The
// enabled native wrappers and source joins run in native_selection68/bridge67.
namespace kcd2vr::frik_pose_probe {
inline bool WorkstationFrame46(workstation_sample46::Frame& out) noexcept {out={};return false;}
}
namespace kcd2vr::interaction_alignment {
namespace selection68=kcd2vr::subsystems::alchemy::native_selection68;
namespace craft68=kcd2vr::subsystems::alchemy::bridge67;
static std::atomic<bool> g_selection_ready68{false};
static selection68::Scope g_selection_scope68{};
static bool SelectionScopeCurrent68(selection68::Table&,std::uint64_t) noexcept {return false;}
constexpr std::uintptr_t kScreenReturn=0x640363ull;
constexpr std::uintptr_t kObjectPolicy=0x3A69D90ull,kDogPolicy=0x3A69E48ull,kChatPolicy=0x3A69F00ull;
std::uintptr_t g_wh_base=0;
std::atomic<bool> g_split_ready57c1{false},g_enabled{true},g_visual_fallback_enabled{true},g_hand_ray_active{false};
std::atomic<bool> g_native_activity_fallback5916c3{false};
std::atomic<std::uint64_t> g_interaction_calls{1};
std::atomic<std::uint64_t> g_onscreen_checks{0},g_unknown_candidates57c1{0},g_head_candidates57c1{0},g_hand_candidates57c1{0},g_onscreen_forced{0};
std::atomic<float> g_radius_m{.21f},g_cone_half_angle_deg{10.f};
struct Candidate57c1 {std::uintptr_t player=0;std::uint64_t tick=0;interaction57c1::Kind kind=interaction57c1::Kind::Unknown;std::uint32_t id=0;char cls[32]{};}; // R59.16C10R10
thread_local Candidate57c1 g_candidate57c1{};
using OnScreenCheckFn=char(__fastcall*)(std::uintptr_t,std::uintptr_t,float*,std::uintptr_t);
OnScreenCheckFn g_onscreen_original=nullptr;
using RaySample=interaction112::Ray;
struct NativeRay112 { RaySample ray{}; float ox=0,oy=0,oz=0,reach=0;std::uint64_t tick=0; };
core::SnapshotMailbox<NativeRay112> g_native_packets112;
core::SnapshotMailbox<RaySample> g_hand_packets112;
core::SnapshotMailbox<palm59c9::Pair> g_palm_packets59c9;
thread_local palm59c9::Ranking g_palm_ranking59c9;
std::mutex g_hand_candidate_mutex178r14;HandCandidate g_hand_candidate178r14{};
door_candidates38::Records<HandCandidate> g_door_candidates38;
std::uint64_t g_door_generation38=1;
std::uint64_t g_door_acquisition_generation65=(std::uint64_t(1)<<63)|1;
static HandCandidate g_direct_door54r1{};
static std::atomic<bool> g_direct_door_retired54r1{false};
static void InvalidateVisualRay() noexcept {} // Not the branch under test; native projection is a spy.
'''
(dst/'generated_interactor5916c3.inc').write_text(pre+'\n\n'.join(parts)+'\n}\n');(dst/'interactor5916c3_extraction.json').write_text(json.dumps(meta,indent=2))

s=read_source(root/"tests/physical_contract175_test.cpp")
assert s.count("int main(")==1
(dst/"generated_melee5916c3.inc").write_text(s[:s.index("int main(")])
