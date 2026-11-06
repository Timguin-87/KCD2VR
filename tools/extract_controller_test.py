#!/usr/bin/env python3
"""Extract CURRENT production F12 adapter, replacing only the native entity call leaf."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib,json
R=Path(__file__).resolve().parents[1]
a=read_source(R/'src/vrik/weapon_tracking.inl');start=a.index('static void* CallEntityLookup6694(');end=a.index('static void* EntityById6694(',start)
a=a[:start]+'''static void* CallEntityLookup6694(std::uintptr_t fn,void* sys,std::uint32_t id) noexcept {return MockNativeEntityLookup6694(fn,sys,id);}
'''+a[end:]
b=read_source(R/'src/frik_pose_probe.cpp');start=b.index('struct AttachmentState6689 {');end=b.index('static void WeaponControlTickImpl()',start)
b=b[start:end]
start=b.index('static void BowCaptureCharacter128(');end=b.index('\n}',start)+2
b=b[:start]+"static void BowCaptureCharacter128(void*) noexcept {} // host native leaf"+b[end:]
(R/'tests/generated_controller_adapter.inc').write_text(a+'\n'+b)
(R/'delivery/v66101r1').mkdir(parents=True,exist_ok=True)
(R/'delivery/v66101r1/controller_extract.json').write_text(json.dumps({'inputs':{n:hashlib.sha256(read_source_bytes(R/n)).hexdigest() for n in ['src/vrik/weapon_tracking.inl','src/frik_pose_probe.cpp']},'replacement':'CallEntityLookup6694 native invocation and BowCaptureCharacter128 native skeleton leaf; consumer methods are provided by fixture vtables; all ownership/mode/transform/restore code is current'},indent=2))
