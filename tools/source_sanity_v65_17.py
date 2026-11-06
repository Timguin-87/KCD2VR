#!/usr/bin/env python3
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import re, sys
root=Path(__file__).resolve().parents[1]
files=[
 'src/mod.cpp','src/vr_menu.cpp','src/xr_manager.cpp','src/native_stereo_outer_lab.cpp',
 'src/native_vr_pad_premap.cpp','src/virtual_xinput.cpp','src/flashui_probe.cpp','src/frik_pose_probe.cpp'
]
fail=[]
print('KCD2VR V65.17 source sanity')
for rel in files:
    p=root/rel; s=read_source(p, errors='ignore')
    counts={c:s.count(c) for c in '{}()[]'}
    ok=counts['{']==counts['}'] and counts['(']==counts[')'] and counts['[']==counts[']']
    print(('PASS' if ok else 'FAIL')+f': {rel} braces={counts["{"]}/{counts["}"]} parens={counts["("]}/{counts[")"]} brackets={counts["["]}/{counts["]"]}')
    if not ok: fail.append(rel)
# Contract-level negative checks across current source/config only.
scan='\n'.join(read_source(root/p, errors='ignore') for p in ['src/mod.cpp','src/vr_menu.cpp','src/xr_manager.cpp','src/native_vr_pad_premap.cpp','src/virtual_xinput.cpp','KCD2VR.ini'])
neg={
 'no fixed old corrected-FOV constant':'kV6508CorrectedGameFov' not in scan,
 'no old Hermite center slope':'centerSlope' not in scan and 'float h10x=' not in scan and 'mappedAx' not in scan,
 'no current Corrected Mono owner':'Corrected Mono' not in scan,
 'no F12 global glyph forcing':'ApplyF7PromptPresentation(true' not in scan,
}
for n,ok in neg.items():
    print(('PASS' if ok else 'FAIL')+': '+n)
    if not ok: fail.append(n)
# V65.17a compile-order guard: the boundary hook is defined before legacy helpers.
stereo=read_source(root/'src/native_stereo_outer_lab.cpp', errors='ignore')
compile_order={
 'EnsureNexusHook declared before CSystem boundary': ('bool EnsureNexusHook() noexcept;', 'bool EnsureCSystemRenderHook() noexcept'),
 'EnsureGetOrCreateHook declared before CSystem boundary': ('bool EnsureGetOrCreateHook() noexcept;', 'bool EnsureCSystemRenderHook() noexcept'),
}
for n,(decl,use) in compile_order.items():
    ok=decl in stereo and use in stereo and stereo.index(decl) < stereo.index(use)
    print(('PASS' if ok else 'FAIL')+': '+n)
    if not ok: fail.append(n)
# No shipped retail profile override anywhere in package.
overrides=[p for p in root.rglob('defaultProfile.xml') if 'research' not in p.parts]
ok=not overrides
print(('PASS' if ok else 'FAIL')+': no shipped defaultProfile.xml override'+((' ['+', '.join(map(str,overrides))+']') if overrides else ''))
if not ok: fail.append('defaultProfile override')
print(f'RESULT: {len(files)+len(neg)+len(compile_order)+1-len(fail)}/{len(files)+len(neg)+len(compile_order)+1} passed')
sys.exit(1 if fail else 0)
