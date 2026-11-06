import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
h=read_source(ROOT/'src/touch_bindings.h')
c=read_source(ROOT/'src/touch_bindings.cpp')
m=read_source(ROOT/'src/mod.cpp')

def test_explicit_right_grip_freelook_api():
    assert 'SetRightGripFreeLookEnabled' in h
    assert 'right_grip_free_look_enabled_' in h

def test_freelook_immediate_when_owned():
    assert 'right_grip_free_look_enabled_ &&' in c
    block=c[c.index('const bool directional_mode'):c.index('// V64.17 conservative', c.index('const bool directional_mode'))]
    assert '550ull' not in block
    assert 'right_grip_down' in block

def test_vrik_owns_grip_when_active():
    assert 'SetRightGripFreeLookEnabled(frik_pose_probe::CombatCensusMode()==0)' in m

def test_vertical_stick_only_suppressed_by_active_freelook():
    assert 'const Dir vertical_action_dir = directional_mode ? Dir::None : rd;' in c

def test_scope_marker():
    assert_current_identity(ROOT)
