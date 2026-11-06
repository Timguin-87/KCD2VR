#!/usr/bin/env python3
"""Extract actual Safe/SEH scalar and array access helpers; no statement substitutions."""
from pathlib import Path
import sys
from source_text import read_source_bytes
root=Path(__file__).resolve().parents[1]
source=read_source_bytes(root/'src/frik_pose_probe.cpp').decode('utf-8').replace('\r\n','\n')
start=source.index('template<class T> bool Read(const void* p,T& out) noexcept {')
end=source.index('void Rtti(',start)
text=source[start:end]
assert text.count('bool Read(')==2 and text.count('bool SafeWrite(')==1
out=Path(sys.argv[1]) if len(sys.argv)>1 else root/'tests/generated_scalar_access_h21.inc'
out.write_text('// Generated from src/frik_pose_probe.cpp; do not edit.\n'+text,encoding='utf-8',newline='\n')
