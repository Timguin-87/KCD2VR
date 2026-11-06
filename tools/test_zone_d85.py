#!/usr/bin/env python3
"""D85 zone: run the unchanged production direct-context write and the strike-receipt zone expressions.

The direct route's attack-context block is copied verbatim out of
src/subsystems/direct/native_state_context.inl and the receipt zone expressions out of
geometry_hit178r27.inl / live_contact_executor.inl, then compiled against explicit
leaves (SetInt/SetFloat recorders and a fake S_CombatActorState reader).

Retail facts (WHGame 0x6A350E20): +0x270 is the running attack row's input_class_id
(0x938FD0 params slot 11) and the executed zone in Henry's frame is
symmetric(+0x150) (0x911250 phase 8). Run with --source <unpatched tree> as the
negative control: the D84R1 code writes a zone into +0x270 and reports +0x270 as the
strike zone, so this test must fail there.
"""
import argparse
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--source', type=Path)
p.add_argument('--sanitize', action='store_true')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
src = (a.source or root).resolve()
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)


def read(rel):
    return (src / rel).read_bytes().decode('latin1').replace('\r\n', '\n')


probe = read('src/diagnostics/native_hit_probe134.inl')
enum = re.search(r'enum : std::uint32_t \{ kInCombat=0x000,[^}]*\};', probe)
assert enum, 'state property enum'
ctx = read('src/subsystems/direct/native_state_context.inl')
start = ctx.index('        const std::array<int,5> expected{')
end_marker = 'SetFailure(hs,"CONTEXT_SETTER_OR_READBACK");return false;}'
end = ctx.index(end_marker, start) + len(end_marker)
block = ctx[start:end]
geom = read('src/diagnostics/geometry_hit178r27.inl')
m = re.search(r'const int contactOrigin59c7g=([^;]*);', geom)
assert m, 'geometry receipt zone'
geom_expr = m.group(1)
live = read('src/subsystems/base/live_contact_executor.inl')
m = re.search(r'PublishContact59c7g\(p,r,completedNowC2,te,(.*?),false,r\.process==1', live)
assert m, 'live receipt zone'
live_expr = m.group(1)

host = r'''
#include "diagnostics/combat_state_zone_d85.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <map>
#include <vector>
''' + enum.group(0) + r'''
constexpr std::uint32_t kActionPhaseProp39=0x30;
static std::map<std::uint32_t,int> g_state;
static std::vector<std::pair<std::uint32_t,int>> g_writes;
static const char* g_failure=nullptr;
static int I(const void*,std::uint32_t prop){auto it=g_state.find(prop);return it==g_state.end()?-99:it->second;}
static bool SetInt(const void*,std::uint32_t prop,int v){g_writes.push_back({prop,v});g_state[prop]=v;return true;}
static bool SetFloat(const void*,std::uint32_t,float){return true;}
static void SetFailure(const void*,const char* why){g_failure=why;}
static bool ContextWrite(const void* hs,int stage,int z,int technique,bool sameStageZone62,
                         const std::array<int,5>& before,std::array<int,5>& expectedOut){
''' + block + r'''
    expectedOut=expected;return true;
}
static int GeometryZone(const void* hs){return ''' + geom_expr + r''';}
static int LiveZone(const void* hs){return ''' + live_expr + r''';}
int main(){
    int fail=0;const void* hs=&g_state;
    for(int stage:{4,8})for(int z:{0,1,2,5}){
        g_state.clear();g_writes.clear();g_failure=nullptr;
        g_state[0x270]=1; // native input class: heavy
        std::array<int,5> before{1,-1,1,1,1},expected{};
        const bool ok=ContextWrite(hs,stage,z,0,false,before,expected);
        bool wroteInput=false;for(const auto& w:g_writes)wroteInput|=w.first==0x270;
        if(!ok||g_failure){std::printf("FAIL stage=%d zone=%d context write refused\n",stage,z);++fail;}
        if(wroteInput||g_state[0x270]!=1){std::printf("FAIL stage=%d zone=%d wrote zone into +0x270 (input class now %d)\n",stage,z,g_state[0x270]);++fail;}
        if(expected[4]!=before[4]){std::printf("FAIL stage=%d zone=%d expected input class %d\n",stage,z,expected[4]);++fail;}
        if(g_state[0x180]!=z||g_state[0xF0]!=z||g_state[0x120]!=z){std::printf("FAIL stage=%d zone=%d zone fields not written\n",stage,z);++fail;}
    }
    // strike receipt zone: running row upper_left (+0x150=1), input class heavy (+0x270=1) -> Henry zone 2;
    // a head row (+0x150=0) must report 0, not the input class.
    const int rows[3][2]{{1,2},{0,0},{5,5}};
    for(const auto& r:rows){g_state.clear();g_state[0x150]=r[0];g_state[0x270]=1;
        if(GeometryZone(hs)!=r[1]){std::printf("FAIL geometry receipt zone row=%d got %d want %d\n",r[0],GeometryZone(hs),r[1]);++fail;}
        if(LiveZone(hs)!=r[1]){std::printf("FAIL live receipt zone row=%d got %d want %d\n",r[0],LiveZone(hs),r[1]);++fail;}}
    std::printf("%s test_zone_d85 (production context block + receipt expressions)\n",fail?"FAILED":"PASSED");
    return fail?1:0;
}
'''
cpp = out / 'zone_d85_host.cpp'
cpp.write_text(host)
exe = out / 'zone_d85_host'
cmd = ['g++', '-std=c++20', '-w', '-I', str(root / 'src'), str(cpp), '-o', str(exe)]
if a.sanitize:
    cmd[1:1] = ['-fsanitize=address,undefined']
subprocess.run(cmd, check=True)
r = subprocess.run([str(exe)], capture_output=True, text=True)
print(r.stdout, end='')
raise SystemExit(r.returncode)
