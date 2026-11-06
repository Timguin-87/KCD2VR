#!/usr/bin/env python3
"""Replay the pinned retail no-opponent query verdict used by the D86 R3 route.

Only four original read-only functions execute. Actor/state/object vtables,
the state predicate and the FreeGuard enum are controlled fixture leaves.
No game process, animation, action dispatch, inventory or hit is executed.
The DLL remains read-only; extracted native bytes stay in --out.
"""
from pathlib import Path
import argparse
import hashlib
import json
import re
import subprocess
from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
SLICES = {'query': (0x1315CB4, 0x42), 'delegate': (0x1315CF8, 22),
          'verdict': (0x9D3514, 95), 'free_guard': (0x12EDF54, 35)}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--dll', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    out = a.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    dll = PE(a.dll)
    assert hashlib.sha256(dll.data).hexdigest() == SHA, 'unsupported native image'
    production = (ROOT / 'src/vrik/native_bridges168.inl').read_text()
    source = production[production.index('static bool NativeFreeKickObjectD86('):
                        production.index('static bool NativeFreeKickContextR24(')]
    report = {'dll_sha256': SHA, 'slices': {}, 'production_pin_checks': {}}
    generated = []
    for name, (rva, size) in SLICES.items():
        data = dll.read(rva, size)
        (out / (name + '.bin')).write_bytes(data)
        generated.append('static const unsigned char ' + name + '[]{' + ','.join(hex(b) for b in data) + '};')
        report['slices'][name] = {'rva': hex(rva), 'size': size, 'sha256': hashlib.sha256(data).hexdigest()}
        if name != 'free_guard':
            m = re.search(r'constexpr unsigned char ' + name + r'\[\]\{([^}]+)\}', source)
            assert m, name
            pin = bytes(int(v.strip(), 0) for v in m[1].split(','))
            assert pin == data, name + ': production pin differs from retail'
            report['production_pin_checks'][name] = True
    fixture = r'''
#include <sys/mman.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <vector>
#include <array>
static bool statePredicate=false;
static unsigned predicateCalls=0,checks=0;
static bool __attribute__((ms_abi)) stateLeaf(void*) {++predicateCalls;return statePredicate;}
static int __attribute__((ms_abi)) propertyLeaf(void* self) {int v=0;std::memcpy(&v,static_cast<char*>(self)+8,4);return v;}
template<class T>static void put(void* self,unsigned offset,T value){std::memcpy(static_cast<char*>(self)+offset,&value,sizeof value);}
static void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL_NATIVE_D86 %s\n",why);std::exit(1);}}
int main(){
 constexpr std::size_t size=0x5400000;
 auto* image=static_cast<unsigned char*>(mmap(nullptr,size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0));
 if(image==MAP_FAILED)return 2;
 std::memcpy(image+0x1315CB4,query,sizeof(query));
 std::memcpy(image+0x1315CF8,delegate,sizeof(delegate));
 std::memcpy(image+0x9D3514,verdict,sizeof(verdict));
 std::memcpy(image+0x12EDF54,free_guard,sizeof(free_guard));
 put(image,0x5320D18,23); // native enum registration is an explicit leaf
 if(mprotect(image,size,PROT_READ|PROT_EXEC))return 3;
 std::array<unsigned char,0x600> actor{},state{},object{},actorVft{},stateVft{},objectVft{},guardVft{};
 // state+CF8 requires a larger independently owned backing allocation.
 std::vector<unsigned char> bigState(0xD20,0);
 put(actor.data(),0,reinterpret_cast<std::uintptr_t>(actorVft.data()));
 put(actor.data(),0x210,reinterpret_cast<std::uintptr_t>(bigState.data()));
 put(actor.data(),0x450,reinterpret_cast<std::uintptr_t>(object.data()));
 put(actorVft.data(),0x580,reinterpret_cast<std::uintptr_t>(image+0x1315CF8));
 put(object.data(),0,reinterpret_cast<std::uintptr_t>(objectVft.data()));
 put(object.data(),0x18,reinterpret_cast<std::uintptr_t>(actor.data()));
 put(objectVft.data(),0x18,reinterpret_cast<std::uintptr_t>(image+0x9D3514));
 put(bigState.data(),0,reinterpret_cast<std::uintptr_t>(stateVft.data()));
 put(stateVft.data(),8,reinterpret_cast<std::uintptr_t>(&stateLeaf));
 put(bigState.data(),0x60,reinterpret_cast<std::uintptr_t>(guardVft.data()));
 put(guardVft.data(),8,reinterpret_cast<std::uintptr_t>(&propertyLeaf));
 using Query=bool(__attribute__((ms_abi)) *)(void*,int);
 auto call=reinterpret_cast<Query>(image+0x1315CB4);
 for(bool activity:{false,true})for(int guard:{-1,0,1,2,23,255}){
  statePredicate=activity;predicateCalls=0;put(bigState.data(),0x68,guard);
  check(!call(actor.data(),0),"standard actor+450 object with NO opponent must select FreeAttack for every tested guard");
  check(predicateCalls==1,"original object virtual predicate was actually executed");
 }
 put(actor.data(),0x450,std::uintptr_t(0));predicateCalls=0;
 check(!call(actor.data(),0),"missing object with no opponent selects FreeAttack");
 check(predicateCalls==0,"missing object never calls the object predicate");
 munmap(image,size);
 std::printf("D86 retail query replay: %u checks PASS; original query/delegate/object/guard functions; no game outcome\n",checks);
}
'''
    unit = out / 'native_kick_query_d86.cpp'
    unit.write_text('\n'.join(generated) + fixture)
    exe = out / 'native_kick_query_d86'
    command = ['g++', '-std=c++20', '-O2', '-Wall', '-Wextra', str(unit), '-o', str(exe)]
    build = subprocess.run(command, text=True, capture_output=True)
    (out / 'build.log').write_text(build.stdout + build.stderr)
    report.update(command=command, compile_rc=build.returncode)
    if build.returncode == 0:
        run = subprocess.run([str(exe)], text=True, capture_output=True, timeout=20)
        report.update(run_rc=run.returncode, output=run.stdout + run.stderr)
        (out / 'run.log').write_text(run.stdout + run.stderr)
        print(run.stdout + run.stderr)
    else:
        print(build.stderr[-5000:])
    exe.unlink(missing_ok=True)
    report['limits'] = 'Read-only native query-family verdict only; state property, actor/object memory and enum leaves controlled; no native action selection, animation, stamina or kick hit executed.'
    (out / 'RESULT.json').write_text(json.dumps(report, indent=2) + '\n')
    raise SystemExit(report.get('run_rc', build.returncode))


if __name__ == '__main__':
    main()
