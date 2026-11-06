#!/usr/bin/env python3
"""Check actual HUD consumers and finite map/receipt/F9 producers on a host.

Native/GFx/OS leaves are substituted. This is not a Windows DLL, game or headset
test. --baseline supplies immutable D83R1 for targeted regression controls.
"""
import argparse, hashlib, json, os, subprocess
from pathlib import Path

def function(text, signature):
    start=text.index(signature); pos=text.index('{',start)+1; depth=1
    while depth:
        depth+=(text[pos]=='{')-(text[pos]=='}'); pos+=1
    return text[start:pos]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
    ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--baseline',type=Path)
    a=ap.parse_args();root=a.source.resolve();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
    results=[];hashes={}
    def source(path):
        p=root/path;hashes[path]=hashlib.sha256(p.read_bytes()).hexdigest();return p.read_text()
    def run(name,body,variant='release',src=None,expect_fail=False):
        src=src or root;folder=out/variant;folder.mkdir(exist_ok=True)
        cpp=folder/(name+'.cpp');exe=folder/name
        body=body.replace('#include "../src/','#include "'+str(src/'src')+'/')
        cpp.write_text(body)
        inc=[src/'tests/ui175/mocks',src/'tests/syntax_shims',src/'tests/openxr_sdk',src/'src']
        cmd=[os.environ.get('CXX','g++'),'-std=c++23','-O2' if variant=='release' else '-O0','-g0','-UNDEBUG',
             '-fms-extensions','-ffunction-sections','-fdata-sections','-Wno-subobject-linkage',
             '-D_ReturnAddress=HostReturnAddress','-D__fastcall=',*['-I'+str(x) for x in inc]]
        if variant=='sanitized':cmd+=['-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-pie','-no-pie']
        cmd+=[str(cpp),'-pthread','-Wl,--gc-sections','-o',str(exe)]
        b=subprocess.run(cmd,capture_output=True,text=True);(folder/(name+'.build.log')).write_text(b.stdout+b.stderr)
        row={'name':name,'variant':variant,'source':str(src),'command':cmd,'build_exit':b.returncode,'expected_regression':expect_fail}
        if b.returncode==0:
            r=subprocess.run([str(exe)],capture_output=True,text=True,timeout=90,
                env=dict(os.environ,ASAN_OPTIONS='detect_leaks=0',UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1'))
            row['run_exit']=r.returncode;row['output']=r.stdout+r.stderr
            (folder/(name+'.run.log')).write_text(row['output'])
        row['passed']=b.returncode==0 and (row.get('run_exit')!=0 if expect_fail else row.get('run_exit')==0)
        results.append(row);print(name,variant,'PASS' if row['passed'] else 'FAIL',flush=True)

    cases=['contextual_hud162r1','hud_transition170','hud_semantic54','hud_policy59c7',
           'hud_experiment36c5','hud_repair175','palm_reveal_lifetime','hud_writer175_bridge',
           'map_performance_d72','capture_routes_d84','d48_diagnostic_policy']
    for name in cases:run(name,source('tests/'+name+'_test.cpp'))
    for variant in ('debug','sanitized'):
        for name in ('hud_writer175_bridge','hud_semantic54','map_performance_d72','capture_routes_d84'):
            run(name,source('tests/'+name+'_test.cpp'),variant)

    trace=source('src/diagnostic_trace.cpp')
    sink='''#include "diagnostics/capture168.h"
#include <vector>
#include <string>
#include <cassert>
#include <cstdio>
static std::uint64_t now=10000;static unsigned clockReads=0;
static std::uint64_t GetTickCount64(){++clockReads;return now;}
namespace kcd2vr::diagnostic_trace {
static bool enabled=true;static std::vector<std::string> rows;
bool Enabled()noexcept{return enabled;}
static void Enqueue(const char*s)noexcept{rows.emplace_back(s);}
'''
    for sig in ('bool EnabledFor(', 'void WriteFor(', 'void WriteReceiptFor(', 'void Write('):sink+=function(trace,sig)+'\n'
    sink+='''}
int main(){namespace c=kcd2vr::diagnostics::capture168;namespace d=kcd2vr::diagnostic_trace;
 d::Write("FAULT OFF");d::WriteFor(c::Domain::Physics,"OFF");assert(d::rows.empty()&&clockReads==0);
 c::gate.select(c::Domain::Interaction,true,now);const auto f5=c::gate.session[unsigned(c::Domain::Interaction)].load();
 d::WriteReceiptFor(c::Domain::Interaction,f5,"D48_F11 copied under F5");assert(d::rows.size()==1);
 c::gate.select(c::Domain::Vrik,true,now);c::gate.select(c::Domain::Interaction,false,now);
 d::WriteFor(c::Domain::Interaction,"OFF ORDINARY");assert(d::rows.size()==1);
 d::WriteReceiptFor(c::Domain::Interaction,f5,"original F5 drain");assert(d::rows.size()==2);
 now+=5001;d::WriteReceiptFor(c::Domain::Interaction,f5,"cannot borrow active F7");assert(d::rows.size()==2);
 c::gate.select(c::Domain::Interaction,true,now);d::WriteReceiptFor(c::Domain::Interaction,f5,"old F5 cannot borrow replacement");assert(d::rows.size()==2);
 d::WriteReceiptFor(c::Domain::Interaction,0,"zero cannot authorize");assert(d::rows.size()==2);
 c::gate.select(c::Domain::Physics,true,now);d::WriteFor(c::Domain::Physics,"payload HUD FAULT F3");assert(d::rows.size()==3);
 c::gate.select(c::Domain::Physics,false,now);d::WriteFor(c::Domain::Physics,"OFF despite other keys");assert(d::rows.size()==3);
 d::enabled=false;d::WriteReceiptFor(c::Domain::Vrik,1,"logger OFF");assert(d::rows.size()==3);
 std::puts("PASS literal diagnostic sink: typed original-domain receipts, OFF before clock, bounded drain, cross-domain serial isolation, restart rejection");}
'''
    for v in ('debug','release','sanitized'):run('actual_receipt_sink_d84',sink,v)

    # The key handler is taken literally from Mod; no physics setter is supplied
    # by the fixture, so accidental gameplay mutations cannot silently pass.
    mod=source('src/mod.cpp');start=mod.index('        // D84 F9 records physics');end=mod.index('\n        if(!matrix_active && f10_down',start)
    keys='''#include "diagnostics/capture_control168.h"
#include <cassert>
#include <cstdio>
static std::uint64_t now=10000;static std::uint64_t GetTickCount64(){return now;}
static bool ensure_vrik_lab(){return true;}static bool combat_diag_probe_installed=false;
static int cues=0;static void LabLadderCue(int n){cues=n;}
static constexpr int MB_ICONASTERISK=1;static void MessageBeep(int){}
namespace kcd2vr::combat_diag_probe {bool Install(){return true;}}
namespace kcd2vr::frik_pose_probe {static int mode=0;void SetWeaponUpstreamDiagMode(int n){mode=n;}void CycleWeaponUpstreamDiagMode(){mode=(mode+1)%6;}int WeaponUpstreamDiagMode(){return mode;}const char* WeaponUpstreamDiagModeName(int){return "UPSTREAM";}}
namespace kcd2vr::diagnostic_trace {void Write(const char*)noexcept{} void WriteFor(diagnostics::capture168::Domain,const char*)noexcept{}}
using namespace kcd2vr;
static void key(bool shift_down){const bool developerMode=true,matrix_active=false,f9_down=true,f9_down_prev=false;
'''+mod[start:end]+'''}
int main(){namespace c=diagnostics::capture168;
 c::gate.select(c::Domain::Combat,true,now);key(false);assert(c::active(c::Domain::Physics,now)&&c::active(c::Domain::Combat,now)&&cues==1);
 const auto lease=c::gate.session[unsigned(c::Domain::Physics)].load();key(true);assert(frik_pose_probe::mode==1&&c::gate.session[unsigned(c::Domain::Physics)].load()==lease);
 key(false);assert(!c::active(c::Domain::Physics,now)&&frik_pose_probe::mode==0&&c::active(c::Domain::Combat,now));
 key(true);assert(c::active(c::Domain::Physics,now)&&frik_pose_probe::mode==1);
 now+=c::Duration;c::gate.poll(now);assert(!c::active(c::Domain::Physics,now));
 key(true);assert(frik_pose_probe::mode==1&&c::active(c::Domain::Physics,now));
 std::puts("PASS literal Mod F9/Shift+F9: independent finite recorder, upstream ladder, no gameplay setter, existing other capture preserved");}
'''
    run('actual_f9_keys_d84',keys)
    if a.baseline:
        baseline=a.baseline.resolve()
        run('baseline_hud_semantic54',source('tests/hud_semantic54_test.cpp'),src=baseline,expect_fail=True)
        quiet='''#include <cstdint>
static unsigned calls=0;static std::uint64_t tick(){++calls;return 1000000000ull;}
#define KCD2VR_PERF_D61_TEST_CLOCK tick
#include "diagnostics/performance_metrics_d61.h"
#include <cstdio>
int main(){for(int i=0;i<1000;++i){kcd2vr::diagnostics::perf_d61::Scope frame{3};}
std::printf("OFF clock calls=%u\\n",calls);return calls?1:0;}
'''
        run('baseline_map_off',quiet,src=baseline,expect_fail=True)
        run('current_map_off',quiet)
    for p in ('src/diagnostics/capture168.h','src/diagnostics/map_performance_d72.h','src/ui_contextual_hud162r1.h',
              'src/ui_palm_alpha44c2.h','src/ui_hud_visibility175.h','src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp'):
        source(p)
    receipt={'passed':all(r['passed'] for r in results),'results':results,'source_sha256':hashes,
        'limitations':'Native/GFx/OS leaves substituted. ASAN+UBSAN used where labelled; leak detection disabled in this executor. No Windows DLL, game, render, GPU or headset execution.'}
    (out/'RESULTS.json').write_text(json.dumps(receipt,indent=2)+'\n')
    return 0 if receipt['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
