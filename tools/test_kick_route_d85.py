#!/usr/bin/env python3
"""D86 kick: run the unchanged production no-target route predicate and the R3
outcome funnel (cooldown + one reason line) against explicit leaves.

Extracted from src/vrik/native_bridges168.inl: NativeFreeKickGuardTypeD84,
NativeFreeKickObjectD86, NativeFreeKickContextR24 and BridgeEvent168. Native memory is a byte map with
the exact retail reader pins; loadout, capture and the on-screen line are
leaves. The pinned retail query-family verdict is independently exercised by
test_native_kick_query_d86.py. Two causal negative controls must fail here.
This does not run the game, action manager, animation or a native kick hit.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess

p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--out', type=Path, required=True)
p.add_argument('--source', type=Path)
p.add_argument('--sanitize', action='store_true')
a = p.parse_args()
root = Path(__file__).resolve().parents[1]
source_root = (a.source or root).resolve()
out = a.out.resolve()
out.mkdir(parents=True, exist_ok=True)
source = source_root / 'src/vrik/native_bridges168.inl'
text = source.read_text()


def block(pattern):
    m = re.search(pattern, text)
    assert m, pattern
    begin = m.start()
    end = text.index('{', begin) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[begin:end]


definitions = '\n'.join(block(r'static bool ' + n + r'\([^;{]*\) noexcept \{') for n in
                        ['NativeFreeKickGuardTypeD84', 'NativeFreeKickObjectD86', 'NativeFreeKickContextR24'])
definitions += '\n' + block(r'static void BridgeEvent168\(b168::Event e\) noexcept \{')

host = r'''
#include "vrik/native_bridges168.h"
#include "vrik/kick_route_d85.h"
#include <map>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
namespace b168=kcd2vr::bridges168;
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
static std::map<std::uintptr_t,unsigned char> memory;
static constexpr std::uintptr_t imageBase=0x180000000ull,propertyTable=0x190000000ull,getter=0x190001000ull;
template<class T>static void put(std::uintptr_t at,const T& value){const auto* p=reinterpret_cast<const unsigned char*>(&value);for(unsigned i=0;i<sizeof value;++i)memory[at+i]=p[i];}
template<class T>static bool Read(const void* raw,T& value){auto at=reinterpret_cast<std::uintptr_t>(raw);auto* p=reinterpret_cast<unsigned char*>(&value);for(unsigned i=0;i<sizeof value;++i){auto it=memory.find(at+i);if(it==memory.end())return false;p[i]=it->second;}return true;}
static std::uintptr_t WhBase(){return imageBase;}
static std::uint64_t now=5000;static std::uint64_t GetTickCount64(){return now;}
namespace retail_build112 {static bool Ready(std::uintptr_t b){return b==WhBase();}}
namespace kcd2vr::combat_rules178r10 {inline std::atomic<int> kickTransport{3};}
static bool loadoutOk=true;
static bool NativeFreeKickLoadoutD72(const NativeOwner104&,b168::FreeKickLoadoutD72& out){out={};out.manager=0x4000;return loadoutOk;}
namespace diagnostics::capture168 {enum class Domain{Combat};static bool active(Domain,std::uint64_t){return false;}}
static int F4Rung178r10(){return 0;}
static b168::Queue g_bridge_events168;
static std::vector<std::string> lines;
namespace kcd2vr::visible_failure {static bool publish(const char* t,std::uint64_t){lines.emplace_back(t);return true;}}
'''

cases = r'''
static unsigned checks=0;
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL: %s (check %u)\n",why,checks);std::exit(1);}}
static constexpr std::uintptr_t actor=0x2000,state=0x3000,combatObj=0x5000;
static NativeOwner104 owner(){return {reinterpret_cast<void*>(0x1000),reinterpret_cast<void*>(actor),reinterpret_cast<void*>(state),9};}
static void world(bool opponent,bool combat,int guard){
 memory.clear();loadoutOk=true;kcd2vr::combat_rules178r10::kickTransport=3;
 const unsigned char use[]{0x48,0x8B,0x89,0x10,0x02,0,0,0x48,0x83,0xC1,0x60,0x48,0x8B,0x01,0xFF,0x50,0x08};
 const unsigned char get[]{0x8B,0x41,0x08,0xC3};
 put(imageBase+0x12EDF58,use);put(imageBase+0x5320D18,23);put(state+0x60,propertyTable);put(propertyTable+8,getter);put(getter,get);
 put(state+0x68,guard);put(state+0xCF8,opponent?std::uintptr_t(0x7000):std::uintptr_t(0));
 put(actor+0x450,combat?combatObj:std::uintptr_t(0));
}
static void standardObject(){
 put(actor,std::uintptr_t(imageBase+0x3E3FAC8));
 put(imageBase+0x3E3FAC8+0x5E0,std::uintptr_t(imageBase+0x1315CB4));
 put(imageBase+0x3E3FAC8+0x580,std::uintptr_t(imageBase+0x1315CF8));
 put(combatObj,std::uintptr_t(imageBase+0x3A79C10));
 put(imageBase+0x3A79C10+0x18,std::uintptr_t(imageBase+0x9D3514));
 put(combatObj+0x18,std::uintptr_t(actor));
 const unsigned char query[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x01,0x8B,0xFA,0x48,0x8B,0xD9,0xFF,0x90,0x80,0x05,0x00,0x00,0x84,0xC0,0x74,0x0D,0xB0,0x01,0x48,0x8B,0x5C,0x24,0x30,0x48,0x83,0xC4,0x20,0x5F,0xC3,0x48,0x8B,0x83,0x10,0x02,0x00,0x00,0x48,0x83,0xB8,0xF8,0x0C,0x00,0x00,0x00,0x0F,0x85,0x08,0x1C,0x03,0x01,0x32,0xC0,0xEB,0xDC};put(imageBase+0x1315CB4,query);
 const unsigned char delegate[]{0x48,0x8B,0x89,0x50,0x04,0x00,0x00,0x48,0x85,0xC9,0x74,0x07,0x48,0x8B,0x01,0x48,0xFF,0x60,0x18,0x32,0xC0,0xC3};put(imageBase+0x1315CF8,delegate);
 const unsigned char verdict[]{0x48,0x89,0x5C,0x24,0x08,0x57,0x48,0x83,0xEC,0x20,0x48,0x8B,0x41,0x18,0x48,0x8B,0xF9,0x48,0x8B,0x88,0x10,0x02,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x08,0x33,0xDB,0x84,0xC0,0x75,0x0D,0x8A,0xC3,0x48,0x8B,0x5C,0x24,0x30,0x48,0x83,0xC4,0x20,0x5F,0xC3,0x48,0x8B,0x4F,0x18,0xE8,0x06,0xAA,0x91,0x00,0x84,0xC0,0x75,0xE6,0x48,0x8B,0x4F,0x18,0x48,0x8B,0x81,0x10,0x02,0x00,0x00,0x48,0x39,0x98,0xF8,0x0C,0x00,0x00,0x74,0xD2,0xE8,0x21,0x9E,0xA9,0xFF,0x84,0xC0,0x75,0xC9,0xB3,0x01,0xEB,0xC5};put(imageBase+0x9D3514,verdict);
}
static b168::Event outcome(unsigned reason,bool released){b168::Event e{};e.kind=1;e.tick=now;e.reason=reason;e.allowed=released;e.type=4;return e;}
int main(){
 namespace kr=kcd2vr::kick_route_d85;
 // D80 22:01:54.084-.116: our fist guard up (free guard fragment), no opponent.
 world(false,true,23);
 check(NativeFreeKickContextR24(owner()),"guard FreeGuard23 + actor+450 object + no opponent keeps the free route (old: R19 -> punch)");
 world(false,false,-1);check(NativeFreeKickContextR24(owner()),"no +450 object, no opponent: free route (unchanged)");
 world(false,true,-1);check(NativeFreeKickContextR24(owner()),"+450 object, guard down, no opponent: free route, press waits for 23");
 world(false,true,0);check(!NativeFreeKickContextR24(owner()),"+450 object with combat guard 0 keeps the combat DOWN5 route");
 world(false,true,2);check(!NativeFreeKickContextR24(owner()),"+450 object with attackReady guard keeps the combat route");
 world(true,false,23);check(!NativeFreeKickContextR24(owner()),"opponent present never uses the free route");
 world(false,true,23);memory[getter]=0;check(!NativeFreeKickContextR24(owner()),"unknown guard getter keeps the old combat route");
 world(false,true,23);memory.erase(actor+0x450);check(!NativeFreeKickContextR24(owner()),"unreadable +450 refuses the free route");
 world(false,false,23);loadoutOk=false;check(!NativeFreeKickContextR24(owner()),"unknown loadout refuses");
 world(false,false,23);kcd2vr::combat_rules178r10::kickTransport=1;check(!NativeFreeKickContextR24(owner()),"legacy transport unchanged");
 // Standard retail actor+450 says FREE with no opponent for all guards.
 for(int guard:{-1,0,1,2,23}){
  world(false,true,guard);standardObject();
  check(NativeFreeKickContextR24(owner()),"pinned standard object with no opponent uses native free route regardless of guard");
 }
 world(true,true,0);standardObject();check(!NativeFreeKickContextR24(owner()),"opponent keeps native combat route");
 world(false,true,0);standardObject();memory[imageBase+0x9D3514]=0;
 check(!NativeFreeKickContextR24(owner()),"changed native object code does not acquire the new route");
 world(false,true,0);standardObject();put(combatObj+0x18,std::uintptr_t(actor+8));
 check(!NativeFreeKickContextR24(owner()),"foreign combat object does not acquire route");
 // R3 outcome funnel: cooldown and one reason line, with F3 OFF.
 kr::cooldown.reset();lines.clear();now=5000;
 BridgeEvent168(outcome(unsigned(b168::KickGateH31::FreeGuardPendingD84),false));
 check(kr::cooldown.ready(now)&&lines.empty(),"wait edge is not an outcome: no cooldown, no line");
 auto transported=outcome(0,true);transported.type=0;BridgeEvent168(transported);
 check(kr::cooldown.ready(now+1),"returned press/release with observed attack type0 is not a kick and earns no cooldown");
 BridgeEvent168(outcome(0,true));
 check(!kr::cooldown.ready(now+1999)&&kr::cooldown.ready(now+2000),"observed native kick starts exactly 2 s cooldown");
 check(lines.empty(),"accepted kick shows no line");
 check(kr::cooldown.remaining(now+500)==1500,"remaining cooldown is reported");
 kr::cooldown.reset();
 BridgeEvent168(outcome(unsigned(b168::KickGateH31::FreeGuardExpiredD84),false));
 check(kr::cooldown.ready(now)&&lines.size()==1&&lines[0]=="kick: guard did not come up in time","free guard timeout: one short line, no cooldown");
 BridgeEvent168(outcome(unsigned(b168::KickGateH31::CooldownD85),false));
 check(lines.size()==2&&lines[1]=="kick: wait 2 s between kicks","cooldown refusal says why");
 BridgeEvent168(outcome(unsigned(b168::KickGateH31::InputNotWritable),false));
 check(lines.size()==2,"gates already explained at their own site are not repeated");
 BridgeEvent168(outcome(0,false));
 check(kr::cooldown.ready(now),"press without returned release does not start the cooldown");
 std::printf("D86 kick route/cooldown production: %u checks PASS; native selector tested separately; headset NOT_RUN\n",checks);
}
'''

mutations = {
    'production': definitions,
    'old_guard_route': definitions.replace('if(!opponent&&combat&&NativeFreeKickObjectD86(o,combat))return true;', ''),
    'cooldown_without_native_kick': definitions.replace('onTerminal(e.reason,e.allowed,e.type==4)', 'onTerminal(e.reason,e.allowed,true)'),
}
assert mutations['old_guard_route'] != definitions
assert mutations['cooldown_without_native_kick'] != definitions
result = {'source': str(source), 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
          'extracted_sha256': hashlib.sha256(definitions.encode()).hexdigest(),
          'native_game_outcome': 'NOT_RUN', 'runs': []}
for name, actual in mutations.items():
    unit = out / (name + '.cpp')
    unit.write_text(host + actual + cases)
    exe = out / name
    cmd = ['g++', '-std=c++20', '-O1' if a.sanitize else '-O2', '-I' + str(root / 'src'), str(unit), '-o', str(exe)]
    if a.sanitize:
        cmd[1:1] = ['-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie']
    build = subprocess.run(cmd, capture_output=True, text=True)
    (out / (name + '_build.log')).write_text(build.stdout + build.stderr)
    entry = {'name': name, 'command': cmd, 'compile_rc': build.returncode, 'expected_failure': name != 'production'}
    if build.returncode == 0:
        run = subprocess.run([str(exe)], capture_output=True, text=True,
                             env=dict(os.environ, ASAN_OPTIONS='detect_leaks=0'), timeout=30)
        entry.update(run_rc=run.returncode, output=run.stdout + run.stderr)
        entry['passed'] = run.returncode == 0 if name == 'production' else run.returncode == 1 and 'FAIL:' in run.stderr
        (out / (name + '.log')).write_text(run.stdout + run.stderr)
        print(name + ': ' + run.stdout + run.stderr)
    else:
        print(build.stderr[-5000:])
        entry['passed'] = False
    exe.unlink(missing_ok=True)
    result['runs'].append(entry)
result['passed'] = all(r['passed'] for r in result['runs'])
(out / 'RESULT.json').write_text(json.dumps(result, indent=2) + '\n')
raise SystemExit(0 if result['passed'] else 1)
