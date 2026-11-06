import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
MOD=read_source(ROOT/'src/mod.cpp', errors='ignore')
PROBE=read_source(ROOT/'src/frik_pose_probe.cpp', errors='ignore')
HDR=read_source(ROOT/'src/frik_pose_probe.h', errors='ignore')

def test_marker():
    assert_current_identity(ROOT)

def test_filter_before_native_xi():
    f='frik_pose_probe::PhysicalWeaponConsumesRightTrigger139()'
    assert f in MOD
    assert MOD.index(f) < MOD.index('if(native_xi_full)')

def test_no_old_blanket_melee_suppression():
    assert 'PhysicalWeaponInputOwned6698())routeR.trigger=0' not in MOD

def test_same_filtered_route_goes_to_both_transports():
    assert_filtered_pad_route(read_source(ROOT/"src/mod.cpp"))

def test_consumer_declared():
    assert 'bool PhysicalWeaponConsumesRightTrigger139() noexcept;' in HDR

def test_consumer_is_bow_only_and_reference_safe():
    assert 'diagnostics::lab::NativeReference()' in PROBE
    # V168 restores the R4 contract. The actual function is also executed in
    # physical_bridge_test: Grip-string must NOT steal RT; Trigger-string must.
    assert 'f.output.active[1]&&f.profiles[1].bow()&&(f.input.bowTrigger||(f.input.quiverGesture114&&f.input.bowStringTrigger160))' in PROBE
