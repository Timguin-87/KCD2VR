#!/usr/bin/env python3
"""Exact synchronous entry and public motion-owner fixture; no native ABI shim."""
from pathlib import Path
import argparse,hashlib,json

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--check',action='store_true',help='Verify committed extraction and input hashes without writing files.')
args=parser.parse_args()

root=Path(__file__).resolve().parents[1]
def function(path,signature):
    text=(root/path).read_text()
    start=text.index(signature);brace=text.index('{',start);depth=1;end=brace+1
    while depth:
        if text[end]=='{':depth+=1
        elif text[end]=='}':depth-=1
        end+=1
    return text[start:end]
hand='src/subsystems/doors/door_hand_native66.inl'
frik='src/frik_pose_probe.cpp'
service='src/subsystems/doors/native_door_service.inl'
parts=[
    '// Generated from current production source; run tools/extract_door_entry_h5.py.',
    'static thread_local door4::Request g_door_entry66;',
    function(hand,'static door4::Request DoorEntry66('),
    function(hand,'static bool OwnedDoorMotion66('),
    'using InteractiveActionFn6689=std::uintptr_t(__fastcall*)(void*,const char*,std::uint32_t,bool,float);',
    function(frik,'static std::uintptr_t DoorNativeCall66('),
    'namespace kcd2vr::frik_pose_probe {',
    function(service,'bool DoorNativeMotionOwned66('),
    '}',
    'namespace kcd2vr::door66 {',
    function(frik,'bool OwnedMotionFor('),
    '}',
]
out=root/'tests/generated_door_entry_h5.inc'
generated='\n'.join(parts)+'\n'
inputs={p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in [hand,frik,service,'src/subsystems/doors/native_door19c4.inl','src/core/native_door19c4.h']}
manifest=root/'tests/door_entry_h5_extraction.json'
if args.check:
    saved=json.loads(manifest.read_text())
    if out.read_text()!=generated or saved['inputs']!=inputs or saved['generatedSha256']!=hashlib.sha256(out.read_bytes()).hexdigest():
        raise SystemExit('Door-entry extraction is stale; regenerate from current production source before reviewing.')
    print('PASS: exact door-entry extraction and five production input hashes match; no files written.')
else:
    out.write_text(generated)
    manifest.write_text(json.dumps({'inputs':inputs,'generated':out.relative_to(root).as_posix(),'generatedSha256':hashlib.sha256(out.read_bytes()).hexdigest(),'scope':'Actual producer/capture/control65, exact DoorEntry66/OwnedDoorMotion66/DoorNativeCall66 and public wrappers. Native root-memory matcher has separate retained fixture; no engine action or body mutation is performed.'},indent=2)+'\n')
