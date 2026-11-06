#!/usr/bin/env python3
"""Compile only the focused host receipt/consumer test; never build the DLL."""
from pathlib import Path
import argparse, hashlib, json, subprocess, sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from check_crossbow_d82 import function
from host_compiler import command, executable_path

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default=None)
    args=ap.parse_args();out=args.out.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
    body=ROOT/'src/vrik/interaction_body122.inl';text=body.read_text()
    declarations=text[text.index('using InteractionBodyFn122='):text.index('static std::atomic<std::uint64_t> g_interaction_body_seen135')]
    contract=ROOT/'src/vrik/interaction_body_contract122.inl';pins=ROOT/'src/vrik/prop_native_pins5916c1.inl';pickup=ROOT/'src/vrik/staged_pickup596c2.inl'
    getter=function(pickup.read_text(),'static bool SmallPickupBodyR24(')
    native='reinterpret_cast<void*(__fastcall*)(void*)>(WhBase()+0x8A49FC)(entity)'
    assert native in getter
    getter=getter.replace(native,'originalR93(entity)').replace('__try{','try{').replace('__except(EXCEPTION_EXECUTE_HANDLER)','catch(...)')
    actual=declarations+contract.read_text()+function(text,'static bool ArmInteractionBody122(')+function(text,'static void ShutdownInteractionBody122(')+pins.read_text()+getter
    (out/'interaction_body_entry_r93_actual.inc').write_text(actual)
    test=ROOT/'tests/interaction_body_entry_r93_test.cpp';fixture=test.read_text().replace('"../src/','"'+(ROOT/'src').as_posix()+'/')
    generated=out/test.name;generated.write_text(fixture)
    exe=executable_path(out/'owned_entry_test')
    cmd=command(generated,exe,[ROOT/'src',out],args.compiler)
    build=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/'compile.log').write_text(build.stdout)
    if build.returncode:print(build.stdout);return build.returncode
    run=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/'run.log').write_text(run.stdout);print(run.stdout,end='')
    sources=[body,contract,pins,pickup,test,Path(__file__),ROOT/'src/vrik/owned_interaction_body_entry_r93.h']
    (out/'RESULT.json').write_text(json.dumps({'passed':run.returncode==0,'command':cmd,'run_output':run.stdout,'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},'limits':['Actual production receipt/lifecycle/pin/static-body getter functions','Native memory, hook installation and native getter are explicit host leaves','No DLL build or headset test']},indent=2)+'\n')
    return run.returncode
if __name__=='__main__':raise SystemExit(main())
