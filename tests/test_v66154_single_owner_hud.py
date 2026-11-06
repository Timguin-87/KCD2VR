import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
m=read_source(r/'src/mod.cpp', errors='ignore')
# V162R1 supersedes the historical MasterSwitch-only contract; a single
# coordinator owns parent placement and local text multipliers on the XR surface.
assert 'ApplyAutoVrLayout(true' not in m
assert 'ApplyCorrectedMonoGlobalUiOverlayExclusions(true' not in m
assert 'ApplyUniformReadableHudLayout(true,geometry' in m
assert 'vr_ui_top_boundary_.load' in m and 'vr_ui_bottom_boundary_.load' in m
assert 'visibility148_event' not in m
assert 'flashui_probe::ApplyReadablePromptScale(' not in m
print('PASS single readable-parent coordinator; field-effect and world-FOV owners excluded')
