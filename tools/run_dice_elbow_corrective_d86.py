"""Host-only checks of actual menu observation/getter and finite F2/F5/F7 receipts.

No Windows mod, DLL, game, renderer or headset is executed.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

p = argparse.ArgumentParser()
p.add_argument('--out', type=Path)
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
out = a.out or Path(tempfile.mkdtemp(prefix='kcd2vr_dice_elbow_d86_'))
out.mkdir(parents=True, exist_ok=True)

def run(name, source, negative=False):
    exe = out / name
    build = subprocess.run(['g++', '-std=c++17', '-O2', '-I', str(root / 'src'),
                            '-I', str(root / 'src/vrik'), str(source), '-o', str(exe)],
                           capture_output=True, text=True)
    (out / (name + '.compile.txt')).write_text(build.stdout + build.stderr)
    if build.returncode:
        raise RuntimeError(name + ' failed to compile: ' + build.stderr)
    test = subprocess.run([str(exe)], capture_output=True, text=True)
    (out / (name + '.result.txt')).write_text(test.stdout + test.stderr)
    print(test.stdout.strip())
    if (test.returncode == 0) == negative:
        raise RuntimeError(name + ' unexpected exit ' + str(test.returncode))

run('dice_elbow_receipts_d86', root / 'tests/dice_elbow_receipts_d86_test.cpp')

# Compile the actual production apply block and actual XR getter. Native/GPU
# boundaries are stand-ins; the observation routing itself is not rewritten.
mod = (root / 'src/mod.cpp').read_text()
start = mod.index('    if(diagnostics::presentation_d86::selected()&&')
end = mod.index('\n    return true;\n}', start)
apply = mod[start:end]
xr = (root / 'src/xr_manager.h').read_text()
start = xr.index('    struct PresentationObservationD86 {')
end = xr.index('    const ControllerState& LeftController()', start)
getter = xr[start:end]
stub = r'''
#include "diagnostics/presentation_receipt_d86.h"
#include <atomic>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
namespace kcd2vr {
unsigned observationReads=0;
std::uint64_t GetTickCount64(){return 1002;}
struct XrManager {
 std::atomic<float> presentation_zoom_{.95f},dialogue_zoom_multiplier_{.8f},source_vertical_fov_deg_{95};
 std::atomic<int> presentation_mode_{3},mono_uv_reprojection_mode_{4};
''' + getter + r'''
};
struct DeviceHook {
 static std::uint64_t BridgeCompletePairCount(){++observationReads;return 12068;}
 static std::uint64_t V6634LastConsumedPairSerial(){++observationReads;return 63823214617012ull;}
 static std::uint64_t PresentCaptureCount(){++observationReads;return 15000;}
};
namespace native_upscaler_stereo_d82 {
struct Publication{std::uint64_t serial=24136,epoch=17;std::uint32_t frame=14928;int technology=2;};
Publication LastPublication(){++observationReads;return {};}
}
namespace native_dlss_stereo_d82 {
struct Status{bool enabled=true;unsigned fault=14;};
Status GetStatus(){++observationReads;return {};}
}
namespace diagnostic_trace {
std::vector<std::string> rows;
void Write(const char* line){rows.emplace_back(line);}
}
void call(float before,float request,float applied,int currentMode,int previousMode,bool haveXr=true){
 XrManager xr;XrManager* xr_manager_=haveXr?&xr:nullptr;
 struct State{float presentation_zoom=1;}state;state.presentation_zoom=request;
 const float previous_zoom_d86=before,zoom=applied;
 const int cyclopean=currentMode,old_cyclopean=previousMode,production_stereo=3;
''' + apply + r'''
}
}
int main(){
 using namespace kcd2vr;namespace cap=diagnostics::capture168;
 unsigned n=0,bad=0;auto ck=[&](bool ok,const char* s){++n;if(!ok){++bad;std::printf("FAIL %s\n",s);}};
 call(1,.95f,.95f,0,0);ck(observationReads==0&&diagnostic_trace::rows.empty(),"OFF actual caller performs no native or queue observation");
 cap::gate.select(cap::Domain::Interaction,true,1000);
 call(1,.95f,.95f,0,0);ck(observationReads==5&&diagnostic_trace::rows.size()==1,"actual caller observes one changed apply");
 auto row=diagnostic_trace::rows.back();ck(row.find("requestedZoom=0.949999988 storedZoom=0.949999988 xrKnown=1 xrZoom=0.949999988")!=std::string::npos,"actual XR getter joins actual requested/stored values");
 ck(row.find("combinedZoom=0.75999999")!=std::string::npos&&row.find("sourceFovDegrees=95")!=std::string::npos,"combined framing cannot be confused with source FOV");
 ck(row.find("publicationRevision=24136 publishedEpoch=17 publishedFrame=14928")!=std::string::npos&&row.find("dlssFault=14")!=std::string::npos,"queue identity and DLSS fault copied from actual call leaves");
 call(.95f,.95f,.95f,0,0);ck(diagnostic_trace::rows.size()==1&&observationReads==5,"unchanged zoom/mode does not repeat apply edge");
 call(1,.95f,1,1,1);ck(diagnostic_trace::rows.size()==2&&diagnostic_trace::rows.back().find("storedZoom=1")!=std::string::npos,"forced projective remap still receipts user's attempted scalar");
 call(1,.95f,.95f,0,0,false);ck(diagnostic_trace::rows.back().find("xrKnown=0")!=std::string::npos,"missing XR is unknown rather than inferred applied pixels");
 std::printf("production_presentation_d86=%u/%u actualApplyAndXrGetter=1\n",n-bad,n);return bad?1:0;
}
'''
source = out / 'production_presentation_d86.cpp'
source.write_text(stub)
run('production_presentation_d86', source)

# Wrongly reporting the 95-degree FOV as zoom must be caught by the same caller
# test. This reproduces the interpretation error without touching production.
mutant = out / 'presentation_fov_as_zoom_mutant.cpp'
needle = 'return {presentation_zoom_.load(std::memory_order_acquire)'
assert stub.count(needle) == 1
mutant.write_text(stub.replace(needle, 'return {source_vertical_fov_deg_.load(std::memory_order_acquire)', 1))
run('presentation_fov_as_zoom_mutant', mutant, negative=True)
print('Results: ' + str(out))
