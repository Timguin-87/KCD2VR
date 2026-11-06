"""Compile the actual production startup FOV lambda with native leaves replaced."""
from pathlib import Path
import sys
r=Path(__file__).resolve().parents[1]
s=(r/'src/mod.cpp').read_text()
a=s.index('    kcd2vr::linked_fov_h26::Startup startupFov;')
b=s.index('    int f5_dialogue_lab_mode',a)
initial=s[s.index('"H28R1 FOV_START stage=INITIAL'):s.index('    // V46.2 in-backbuffer menu.',s.index('"H28R1 FOV_START stage=INITIAL'))]
assert 'log().info("{}",m)' in initial and 'diagnostic_trace::Write(m)' in initial
h=r"""
#include "subsystems/base/linked_fov_h26.h"
#include "diagnostics/capture168.h"
#include <atomic>
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
using namespace kcd2vr;
static unsigned long long now=0,samples=1;static float native=95;static bool setterOK=true;static int writes=0,reads=0;
unsigned long long GetTickCount64(){return now;}
namespace FrameSync {unsigned long long PublishedFovSampleCount(){return samples;}}
float ReadCurrentGameFovValue(){++reads;return native;}
bool ApplyNativeGameFov(float value){++writes;if(setterOK)native=value;return setterOK;}
namespace mocklog {inline std::vector<std::string> lines;struct Logger{void info(const char*,const char* p){lines.emplace_back(p);}};}
mocklog::Logger& log(){static mocklog::Logger out;return out;}
namespace kcd2vr::diagnostic_trace {inline std::vector<std::string> lines;void Write(const char* p){if(diagnostics::capture168::essential(p))lines.emplace_back(p);}}
struct XR {float GetSourceVerticalFov(){return 95;}float GetCorrectedViewVerticalFov(){return 95;}};
struct Mod {std::atomic<float> menu_game_fov_target_{95},vr_source_vertical_fov_deg_{95},vr_zoom_{1};XR xr;XR* xr_manager_=&xr;void run(int scenario){
"""
t=r"""
if(scenario==0){samples=0;now=100;refresh_game_fov_once();assert(mocklog::lines.empty());samples=1;refresh_game_fov_once();assert(writes==0&&reads==1&&mocklog::lines.size()==1);assert(mocklog::lines[0].find("stage=FIRST_RENDER")!=std::string::npos);for(now=101;now<11000;++now)refresh_game_fov_once();assert(writes==0&&mocklog::lines.size()==1);now=120000;refresh_game_fov_once(true);assert(mocklog::lines.size()==2&&mocklog::lines.back().find("FIRST_GAMEPLAY")!=std::string::npos);now=130000;refresh_game_fov_once(true);assert(mocklog::lines.size()==2);}
if(scenario==1){for(now=0;now<15000;++now){if(now==2000)native=110;refresh_game_fov_once();}assert(native==95&&menu_game_fov_target_.load()==95&&writes==1);assert(mocklog::lines.size()==2);assert(mocklog::lines.back().find("nativeBefore=110.000 nativeAfter=95.000")!=std::string::npos);assert(mocklog::lines.back().find("confirmed=1")!=std::string::npos);}
if(scenario==2){setterOK=false;native=110;for(now=0;now<30000;++now)refresh_game_fov_once();assert(writes==20&&mocklog::lines.size()==20&&native==110&&menu_game_fov_target_.load()==95);assert(mocklog::lines.back().find("confirmed=0")!=std::string::npos);}
if(scenario==3){menu_game_fov_target_=110;vr_source_vertical_fov_deg_=110;native=110;refresh_game_fov_once();assert(writes==0&&menu_game_fov_target_.load()==110);assert(mocklog::lines.back().find("selectedGame=110.000 selectedVR=110.000")!=std::string::npos);}
assert(mocklog::lines==diagnostic_trace::lines);
}};
int main(){assert(!diagnostics::capture168::gate.mask.load());for(int scenario=0;scenario<4;++scenario){now=0;samples=1;native=95;setterOK=true;writes=reads=0;mocklog::lines.clear();diagnostic_trace::lines.clear();Mod m;m.run(scenario);}std::puts("H28R1 actual production startup FOV lambda: quiet-mode receipts, late110 overwrite, bounded failure and editable110 selection PASS");}
"""
Path(sys.argv[1]).write_text(h+s[a:b]+t)
