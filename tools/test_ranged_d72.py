"""D72 actual ranged/knife adapters with explicit native leaves.
Normal/refusal flow only: portable Windows stubs do not test SEH or execute game code.
"""
import argparse, hashlib, json, re, subprocess
from pathlib import Path
from native_research112 import PE
from test_pending_reach_t23 import extract_function
ROOT=Path(__file__).resolve().parents[1]

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll',required=True,type=Path)
    ap.add_argument('--build-dir',required=True,type=Path)
    ap.add_argument('--compiler',default='g++')
    ap.add_argument('--extract-only',action='store_true')
    a=ap.parse_args();build=a.build_dir.resolve()
    assert build!=ROOT and ROOT not in build.parents
    build.mkdir(parents=True,exist_ok=True)
    dll=PE(a.dll);assert hashlib.sha256(dll.data).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
    assert dll.stamp==0x6A350E20
    source=ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl'
    text=source.read_text();pins=[]
    for name,rva in [('hideD72',0x87963C),('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
        m=re.search(r'char '+name+r'\[\]\{([^}]+)\}',text)
        value=bytes(int(x.strip(),0) for x in m[1].split(','));assert dll.read(rva,len(value))==value;pins.append((rva,value))
    (build/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(r,','.join('0x%02x'%x for x in v)) for r,v in pins))
    (build/'windows.h').write_text('''#pragma once
#include <cstdlib>
#include <cstdint>
#define __fastcall
#ifndef __try
#define __try try
#endif
#define __except(x) catch(...)
#define __finally catch(...) { throw; } if(true)
#define EXCEPTION_EXECUTE_HANDLER 1
constexpr unsigned MEM_RESERVE=1,MEM_COMMIT=2,PAGE_READWRITE=4;
inline void* VirtualAlloc(void*,std::size_t n,unsigned,unsigned){return std::calloc(1,n);}
[[noreturn]] inline void RaiseException(unsigned code,unsigned,unsigned,const void*){throw code;}
''')
    fixture=ROOT/'tests/subsystems/throw_drop/fixtures/pending_reach_t23/frozen_native_supply_publication.inl'
    (build/'frozen_native_supply_publication.inl').write_bytes(fixture.read_bytes())
    helpers=(ROOT/'src/subsystems/throw_drop/holster_gestures.inl').read_text()
    (build/'draw_candidate_t23.inc').write_text(''.join(extract_function(helpers,n) for n in ['PhysMainWeaponR27','PhysAnotherMainHeldR27','PhysDrawCandidatePolicyR38','PhysDrawCandidate126']))
    producer=(ROOT/'src/subsystems/throw_drop/native_supply_publication.inl').read_text()
    guarded='''const auto observed=t::ObservePending(side,pending);
        if(observed==t::PendingState::Unknown){(void)t::PublishGrant(side,{});continue;}
        if(observed==t::PendingState::Absent)'''
    assert producer.count(guarded)==1
    (build/'unknown_as_absent_t23.inc').write_text(producer.replace(guarded,'if(!t::ReadPending(side,pending))'))
    publication='''ReadCommittedPacket6695(packet)&&publicationT23&&!(publicationT23&1)&&publicationT23==frame.publicationT23&&
        publicationT23==ReadCompletedPublicationT23()&&'''
    assert producer.count(publication)==1
    (build/'publication_unjoined_t23.inc').write_text(producer.replace(publication,'ReadCommittedPacket6695(packet)&&'))
    if a.extract_only:
        (build/'windows.h').unlink() # MSVC fixtures use the real Windows headers and SEH.
        print('PASS exact retail pins and current producer slices prepared for MSVC');return 0
    tests=['tests/crossbow_native_xr34_test.cpp','tests/crossbow_start_leaf_d69_test.cpp','tests/crossbow_instant_d72_test.cpp',
        'tests/ranged_draw_d72_test.cpp','tests/subsystems/throw_drop/coherent_callback_d72_test.cpp','tests/subsystems/throw_drop/native_left_pouch_d72_test.cpp']
    results=[]
    for name in tests:
        test=ROOT/name;exe=build/test.stem
        command=[a.compiler,'-std=c++20','-O0','-g0','-pthread','-I'+str(ROOT/'src'),'-I'+str(build)]
        if 'crossbow_' in test.name:command+=['-D__finally=catch(...) { throw; } if(true)']
        command +=[str(test),'-o',str(exe)]
        compiled=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        run=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True) if compiled.returncode==0 else None
        log=compiled.stdout+(run.stdout if run else '')
        (build/(test.stem+'.log')).write_text(log)
        result={'test':name,'compile_exit':compiled.returncode,'run_exit':run.returncode if run else None,'passed':compiled.returncode==0 and run is not None and run.returncode==0,'log':str(build/(test.stem+'.log'))}
        results.append(result);print(name, 'PASS' if result['passed'] else 'FAIL',run.stdout.strip() if run else compiled.stdout)
    paths=[ROOT/name for name in tests]+[source,ROOT/'src/subsystems/throw_drop/pending_reach_t23.h',
        ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.h',ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.inl',
        ROOT/'src/subsystems/ranged_reload/crossbow_rifle_request_h21.inl',ROOT/'src/subsystems/ranged_reload/reload_passenger_xr34.inl',
        ROOT/'src/subsystems/ranged_reload/ranged_draw_policy_d72.h',ROOT/'src/subsystems/ranged_reload/ranged_draw_visibility_d72.inl',ROOT/'src/vrik/physical_native.inl']
    report={'all_passed':all(r['passed'] for r in results),'native_execution':False,'SEH_tested':False,
        'exact_dll_sha256':hashlib.sha256(dll.data).hexdigest(),'native_pins_verified':len(pins),
        'results':results,'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in paths},
        'limits':['Original native read/create/cost/Shoot leaves are explicit substitutes.','No headset, game, renderer, asset streaming or physical lever kinematics validation.','Legacy T23/T22 mains are fixture providers only; D72 executes the current left-chest route.','The actual crossbow attachment transport is checked by the separate Windows DLL compilation, not simulated native geometry.']}
    (build/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    return 0 if report['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
