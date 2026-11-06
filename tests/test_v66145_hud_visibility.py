import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
h=read_source(r/"src/mod.h"); m=read_source(r/"src/mod.cpp"); v=read_source(r/"src/vr_menu.h")
checks=[("requested status-hidden default; saved preference still loaded", "hud_status_icons_visible_{false}" in h and 'ReadIniBoolExplicit(explicit_ini,"Settings","VrHudStatusIconsVisible",false)' in m),
 ("optional contextual setting","VrContextualHud" in m),
 ("no status keepalive","visibility148_event" not in m),
 ("no automatic Buffs hide",'SetHudMovieClipVisible("Buffs"' not in m),
 ("explicit compass menu preference retained",'self->hud_compass_visible_.store(compass' in m and 'flashui_probe::UpdateHudVisibility170' in m)]
for n,x in checks: print(("PASS" if x else "FAIL"),n)
assert all(x for _,x in checks)
