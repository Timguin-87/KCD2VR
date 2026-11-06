#!/usr/bin/env python3
"""Extract the exact production F12 setter and frame prerequisite/installer for host tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import re,sys
R=Path(__file__).resolve().parents[1]
S=Path(sys.argv[1]) if len(sys.argv)>1 else R
O=Path(sys.argv[2]) if len(sys.argv)>2 else R/'tests'
O.mkdir(parents=True,exist_ok=True)
def function(text,name):
    cursor=0
    while True:
        found=text.index(name,cursor);start=text.rfind('\n',0,found)+1
        brace=text.index('{',found);semi=text.find(';',found)
        if semi<0 or brace<semi:break
        cursor=semi+1
    depth=0
    tokens=re.compile(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*.*?\*/|[{}]',re.S)
    for m in tokens.finditer(text,brace):
        if m.group()=='{':depth+=1
        elif m.group()=='}':
            depth-=1
            if depth==0:return text[start:m.end()]
    raise ValueError(name)
f=read_source(S/'src/frik_pose_probe.cpp')
out='static const char* WeaponControlModeNameImpl(int mode) noexcept;\n'
for n in ['SetWeaponControlModeImpl(int','CycleWeaponControlModeImpl()','WeaponControlModeImpl() noexcept','WeaponControlModeNameImpl(int mode) noexcept {']:
    # Avoid the forward declaration for ModeName and names contained in Cycle.
    if n=='WeaponControlModeImpl() noexcept': n='static int WeaponControlModeImpl() noexcept'
    out+=function(f,n)+'\n'
(O/'generated_mode_adapter.inc').write_text(out)
s=read_source(S/'src/native_stereo_outer_lab.cpp')
functions=function(s,'bool ExactBuildReady()')+'\n'
new='ExactPhysicalFrameReady6699()' in s
if new:functions+=function(s,'static bool ExactPhysicalFrameReady6699()')+'\n'
# All exact constants used by the gates, including dependencies.
constants={m.group(1):m.group() for m in re.finditer(r'constexpr\s+(?:[\w:]+\s+)+(k\w+)\b[^;]*;',s,re.S)}
needed=set(re.findall(r'\bk[A-Z]\w+',functions));last=set()
while needed!=last:
    last=needed.copy()
    for n in list(needed):
        if n in constants:needed.update(re.findall(r'\bk[A-Z]\w+',constants[n]))
missing=needed-constants.keys()
if missing:raise ValueError(missing)
(O/'generated_frame_gate.inc').write_text('\n'.join(v for k,v in constants.items() if k in needed)+'\n'+functions)
# A second include is after the host hook definitions so the real installer runs.
installer=('bool EnsureNexusHook(bool physicalOnly=false) noexcept;\n' if new else '')+function(s,'bool EnsureNexusHook(')+'\n'+function(s,'bool EnsurePhysicalFramePump6698()')
(O/'generated_frame_install.inc').write_text(installer)
setup=[]
for m in re.finditer(r'MatchBytes\(b\+(k\w+),(k\w+),sizeof\(\2\)\)',functions):
    line='std::memcpy(image.data()+'+m[1]+','+m[2]+',sizeof('+m[2]+'));'
    if line not in setup:setup.append(line)
(O/'generated_frame_setup.inc').write_text('\n'.join(setup))
(O/'generated_frame_version.h').write_text('#define HAS_NARROW_PHYSICAL_GATE '+str(int(new))+'\n')
