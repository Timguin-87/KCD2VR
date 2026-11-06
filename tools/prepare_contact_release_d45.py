#!/usr/bin/env python3
"""Prepare actual contact adapter host fixture outside source; no frozen evidence edits."""
import argparse,hashlib,json,runpy
from pathlib import Path
parser=argparse.ArgumentParser()
parser.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
parser.add_argument('--output',required=True,type=Path)
a=parser.parse_args();root=a.source.resolve();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
assert out!=root and root not in out.parents,'output must be outside source'
original=Path.write_text
# Existing extractors supply unchanged production functions, native MockRay
# leaf and monotonic host clock; redirect their generated test products only.
def redirected(path,text,*args,**kwargs):
    if path.parent==root/'tests' and path.name.startswith('generated_'):path=out/path.name
    return original(path,text,*args,**kwargs)
Path.write_text=redirected
try:
    runpy.run_path(str(root/'tools/extract_adapter_types.py'))
    runpy.run_path(str(root/'tools/extract_world_adapter_test.py'))
finally:Path.write_text=original
s=(root/'tests/world_adapter_test.cpp').read_text()
s=s.replace('"../src/','"'+(root/'src').as_posix()+'/')
# These includes live outside the extracted source region in the full DLL.
# Existing F2 fast-release option is a declared OFF leaf; this fixture exercises
# the actual ordinary contact release and native query adapter, not that option.
s='#include "vrik/contact_release_d45.h"\n#include "vrik/contact_hold_r11.h"\n#include "vrik/contact_glide_w5.h"\nnamespace kcd2vr::smooth_lab { inline bool StaleContactFastRelease(){return false;} }\n'+s
(out/'contact_release_world_d45.inc').write_text(s)
(out/'contact_release_d45_test.cpp').write_bytes((root/'tests/contact_release_d45_test.cpp').read_bytes())
paths=['src/frik_pose_probe.cpp','src/vrik/hand_contact_types.inl','src/vrik/contact_release_d45.h',
       'src/subsystems/physics/pose_free_hand_contact.inl','tests/world_adapter_test.cpp',
       'tests/contact_release_d45_test.cpp','tools/extract_adapter_types.py','tools/extract_world_adapter_test.py']
(out/'inputs.json').write_text(json.dumps({p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in paths},indent=2))
print('Prepared actual-contact host fixture:',out)
