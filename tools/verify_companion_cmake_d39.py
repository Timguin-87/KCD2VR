#!/usr/bin/env python3
"""Configure actual CMakeLists for both XR link modes with explicit host stubs.

Does not compile/link a DLL. Native dependencies are empty imported test targets;
current-source companion preparation and its Python code execute unmodified.
"""
import argparse,json,os,subprocess,sys,zipfile
from pathlib import Path
from companion_sources_d39 import SOURCE_PAIRS
ROOT=Path(__file__).resolve().parents[1]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--cmake',default='cmake');p.add_argument('--source',type=Path,default=ROOT);p.add_argument('--out',type=Path,required=True)
    args=p.parse_args();out=args.out.resolve();out.mkdir(parents=True,exist_ok=True);source=args.source.resolve()
    deps=out/'test_imports';deps.mkdir(exist_ok=True)
    (deps/'native_stub').write_bytes(b'')
    (deps/'DetourModKitConfig.cmake').write_text('''add_library(DetourModKit::DetourModKit INTERFACE IMPORTED)
add_library(OpenXR::openxr_loader ${KCD2VR_D39_TEST_XR_TYPE} IMPORTED)
set_target_properties(OpenXR::openxr_loader PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/native_stub")
''')
    results=[]
    for kind in ['STATIC','SHARED']:
        build=out/kind.lower()
        command=[args.cmake,'-S',str(source),'-B',str(build),'-G','Unix Makefiles',
            '-DDetourModKit_DIR='+str(deps),'-DCMAKE_BUILD_TYPE=Release','-DCMAKE_CXX_COMPILER=g++',
            '-DPython3_EXECUTABLE='+sys.executable,'-DKCD2VR_D39_TEST_XR_TYPE='+kind,
            '-DKCD2VR_SINGLE_BINARY='+('ON' if kind=='STATIC' else 'OFF')]
        run=subprocess.run(command,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True)
        (out/(kind.lower()+'.log')).write_text(run.stdout)
        result={'kind':kind,'configure_exit':run.returncode,'command':command,'native_build':'NOT_EXECUTED; imported dependencies are host-only stubs'}
        pak=build/'d39_companion/Mods/KCD2VR/Data/KCD2VR.pak'
        result['current_source_package_created']=pak.is_file()
        if pak.is_file():
            with zipfile.ZipFile(pak) as z:
                result['matching_current_source_members']=sum(z.read(member)==(source/name).read_bytes() for name,member in SOURCE_PAIRS.items())
                result['expected_current_source_members']=len(SOURCE_PAIRS)
        result['passed']=run.returncode==0 and result.get('matching_current_source_members')==len(SOURCE_PAIRS)
        results.append(result)
    (out/'results.json').write_text(json.dumps(results,indent=2)+'\n');print(json.dumps(results,indent=2))
    return 0 if all(r['passed'] for r in results) else 1
if __name__=='__main__':raise SystemExit(main())
