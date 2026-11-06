#!/usr/bin/env python3
"""Execute exact InspectNativeBody112 against bounded fixture-owned FIFOs.

The native alias/body rules are direct production includes; memory, retail
fingerprint, alias lookup and diagnostics are explicit host leaves. This checks
the standing-vs-mounted bow hypothesis without changing protected ownership.
"""
from pathlib import Path
import argparse
import subprocess
import tempfile

p = argparse.ArgumentParser()
p.add_argument("--source", type=Path, default=Path(__file__).resolve().parents[1])
p.add_argument("--output", type=Path)
args = p.parse_args()
source = args.source.resolve()
output = args.output or Path(tempfile.mkdtemp(prefix="kcd2vr_bow_body_d39_"))
output.mkdir(parents=True, exist_ok=True)
text = (source / "src/animation_filter.cpp").read_text()
start = text.index("static bool InspectNativeBody112(void* anim) noexcept {")
body = text[start:text.index("\nbool NativeBodyRequired112(", start)]
code = r'''
#include "vrik/vr_combat_ownership111.h"
#include "vrik/body_context115.h"
#include "vrik/body_context126.h"
#include "vrik/left_holding_b2.h"
#include <array>
#include <atomic>
#include <cstdio>
#include <cstring>
static std::uint64_t GetTickCount64(){return 2000;}
namespace kcd2vr {
namespace native_kill_nf {void Retire(){}}
namespace retail_build112 {bool verified=true;bool Ready(std::uintptr_t){return verified;}}
namespace frik_pose_probe {bool NativeFreeKickVisualR24(){return false;}}
namespace kick26 {std::atomic<std::uint64_t> lastHitReactionCLE{0},nativeClipStart{0};std::atomic<int> reachMs{200};}
namespace visible_failure {bool publish(const char*,std::uint64_t){return true;}}
namespace diagnostic_trace {void Write(const char*){}}
namespace diagnostics::combat_capture {enum class Kind {State};struct Event {Kind kind{};std::uint64_t tick=0,actor=0;int id=0,layer=0;unsigned flags=0;};
 bool active(std::uint64_t){return false;}void name(Event&,const char*){}void push(const Event&){}
}
}
using namespace kcd2vr;
namespace hookcost178r22 {enum {AnimationBody};struct Scope {explicit Scope(int){}};}
static constexpr std::uintptr_t kCSkeletonAnimVftRva=0x1234;
static std::uintptr_t WhBase(){return 0x1000;}
static std::atomic<void*> g_player_skeleton_anim{nullptr};
static std::atomic<bool> g_playback_layout_ready6690{true},nativeKickActive175{false};
static bool passenger=false;static const char* currentAlias=nullptr;static const char* decision=nullptr;
static bool MatchVft(void*,std::uintptr_t){return true;}
static bool NativePassengerBodyA34(void*){return passenger;}
static void ObserveKillNF2(void*){}
template<class T>static bool Read(const void* p,T& v){if(!p)return false;std::memcpy(&v,p,sizeof(v));return true;}
static bool NameForId6689(void*,int id,char* name,std::size_t cap){if(id!=17||!currentAlias)return false;std::snprintf(name,cap,"%s",currentAlias);return true;}
static bool BodyDecision115(bool native,const char* why,int=-1,int=-1,const char* = nullptr);
static bool BodyDecision115(bool native,const char* why,int,int,const char*){decision=why;return native;}
ACTUAL_NATIVE_BODY_FUNCTION
static std::array<unsigned char,0x600> actor{};
static std::array<unsigned char,4+0x78> queue{};
template<class T>static void put(unsigned char* p,T v){std::memcpy(p,&v,sizeof(v));}
static bool inspect(const char* alias,int layer=6,bool active=true,int id=17){
 actor={};queue={};currentAlias=alias;g_player_skeleton_anim=actor.data();
 put(actor.data()+layer*0x40+0x48,queue.data()+4);put(queue.data(),std::uint32_t(1));
 put(queue.data()+4+0x50,std::uint16_t(active?1:0));put(queue.data()+4+0x10,id);
 return InspectNativeBody112(actor.data());
}
static int checks=0,failures=0;static void ck(bool b,const char* m){++checks;if(!b){++failures;std::printf("FAIL %s reason=%s\n",m,decision?decision:"none");}}
int main(){
 for(const char* alias:{"bow_aimposes_pulled_upperbody","bow_aimposes_pulled_upperbody.caf","bow_aimposes_pulled_upperbody_player","bow_crouched_aimposes_pulled_upperbody"})
  ck(!inspect(alias,6),"standing full-draw upper-body aliases do not request native body");
 ck(!inspect("bow_aimposes_pulled_upperbody.caf",14),"recorded J27 layer14 full draw does not request native body");
 ck(inspect("bow_aimposes_pulled",6),"exact mounted bow aim remains protected");
 ck(inspect("bow_idle_loaded_handsup_over",6),"shared bow HitDeath alias remains protected");
 ck(inspect("combat_finish_lngsw_01",0),"finisher stays native during bow audit");
 ck(inspect("combat_clinch1_sz0_az0_ripostesync_step0_lngsw_master",0),"paired clinch stays native");
 ck(inspect("bow_aimposes_pulled_upperbody",6,true,999),"unresolved active alias remains fail closed");
 ck(!inspect("bow_aimposes_pulled",6,false),"inactive mounted alias does not create a timer lease");
 g_playback_layout_ready6690=false;ck(inspect("bow_aimposes_pulled_upperbody",6),"unqualified FIFO layout remains native");g_playback_layout_ready6690=true;
 passenger=true;ck(inspect("bow_aimposes_pulled_upperbody",6),"native passenger remains protected");passenger=false;
 retail_build112::verified=false;ck(inspect("bow_aimposes_pulled_upperbody",6),"unverified retail build cannot borrow bow exception");
 std::printf("actual InspectNativeBody112 bow audit: %d checks, %d failures; protected rules unchanged, native runtime NOT_EXECUTED\n",checks,failures);
 return failures?1:0;
}
'''.replace("ACTUAL_NATIVE_BODY_FUNCTION", body)
fixture = output / "production_bow_body_admission_d39.cpp"
fixture.write_text(code)
binary = output / "production_bow_body_admission_d39"
subprocess.run(["c++", "-std=c++17", "-O2", "-I", str(source / "src"), str(fixture), "-o", str(binary)], check=True)
result = subprocess.run([str(binary)], text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
print(result.stdout, end="")
(output / "bow_body_admission_result.txt").write_text(result.stdout)
raise SystemExit(result.returncode)
