#!/usr/bin/env python3
"""Extract the current native-frame hook and transport mask, without changing bodies."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/native_stereo_outer_lab.cpp')
a=s.index('void __fastcall FrameNexusDetour(');b=s.index('\nvoid __fastcall CameraHandoffDetour(',a)
frame=s[a:b]
(r/'tests/generated_physical_frame.inc').write_text(frame)
s=read_source(r/'src/mod.cpp')
a=s.index('const bool physical_rt_owned =')
b=s.index('static bool physical_rt_owned_prev',a)
mask=s[a:b].strip()
(r/'tests/generated_physical_input.inc').write_text(mask+'\n')
(r/'delivery/v66101r1').mkdir(parents=True,exist_ok=True)
(r/'delivery/v66101r1/frame_input_extract.json').write_text(json.dumps({'frame_hook_sha256':hashlib.sha256(frame.encode()).hexdigest(),'input_line_sha256':hashlib.sha256(mask.encode()).hexdigest(),'replacement':'None within extracted bodies. Win32/device/physical bridge calls supplied by host fixture. Surrounding full translation units not compiled by this test.'},indent=2)+'\n')
