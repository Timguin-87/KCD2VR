#!/usr/bin/env python3
"""Extract the real native adapter's POD types for portable contract tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
root=Path(__file__).resolve().parents[1]
s=read_source(root/'src/frik_pose_probe.cpp')
start=s.index('struct V3 {')
op=s.index('{',s.index('static QuatT28 AbsoluteFromRelative'))
level=1;k=op+1
while level:
    level+=(s[k]=='{')-(s[k]=='}');k+=1
rig=s[s.index('struct RigMap {'):s.index('\n};',s.index('struct RigMap {'))+3]
(root/'tests/generated_adapter_types.h').write_text('// Generated from current frik_pose_probe.cpp; do not hand-edit.\n'+rig+'\n'+s[start:k]+'\n')
