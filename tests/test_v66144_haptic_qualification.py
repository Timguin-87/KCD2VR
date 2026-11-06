import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
s=read_source(r/"src/vrik/physical_weapon.h")
checks=[("deferred zero impact","contact.known&&contact.limited115&&allowed<.999" in s and "out.impactSpeed[side]=0.0" in s),("qualified plane retained","if(contact.plane&&contact.known)" in s),("no attempted fallback","std::min(attempted,3.0)" not in s)]
for n,v in checks: print(("PASS" if v else "FAIL"),n)
assert all(v for _,v in checks)
