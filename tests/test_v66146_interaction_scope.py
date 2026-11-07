import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]; h=read_source(r/"src/mod.h"); m=read_source(r/"src/mod.cpp")
checks=[("default off","vr_interaction_center_test_{false}" in h),("ini default off","settings.bind(\"VrInteractionCenterTest\", vr_interaction_center_test_, false)" in m),("startup hook disabled","interaction_alignment::SetEnabled(false)" in m),("per frame gate retained","const bool head_interaction_aim = interaction_alignment_installed" in m and "vr_interaction_center_test_.load" in m),("hand ray retained","SetHandRayWorld(true" in m)]
for n,x in checks: print(("PASS" if x else "FAIL"),n)
assert all(x for _,x in checks)
