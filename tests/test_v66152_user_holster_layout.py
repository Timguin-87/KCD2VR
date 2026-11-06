import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
R=Path(__file__).resolve().parents[1]
P=read_source(R/'src/vrik/physical_weapon.h')
M=read_source(R/'src/mod.cpp')
V=read_source(R/'src/vr_menu.cpp')
def test_mapping_order():
    assert 'p.legacyHolsters147' in P and 'NativeBelt' in P  # V153: two-handed flag no longer forces a shoulder
    assert 'HolsterZone126::RightShoulder' in P
    assert 'p.classId==9' in P and 'HolsterZone126::LeftShoulder' in P
def test_ui_default_and_label():
    assert_current_identity(R)
    assert 'USER SHOULDER LAYOUT: ON' in V
