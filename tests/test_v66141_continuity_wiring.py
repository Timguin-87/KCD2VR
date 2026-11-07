import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
R=Path(__file__).resolve().parents[1]
N=read_source(R/'src/vrik/physical_native.inl')
M=read_source(R/'src/mod.cpp')

def test_marker():
    assert_current_identity(R)

def test_exact_owner_requires_key_grip_tracking():
    assert 'handKnown&&live==r.key&&liveGrip>.55f&&TrackingHandUsable111(r.side,now)' in N

def test_exact_owner_gets_bounded_extension_only():
    assert 'const std::uint64_t maxAge=handKnown?80ull:60ull;' in N
    assert '250ull' not in N[N.find('bool PhysReplayLast135'):N.find('bool PhysReplayLast135')+5000]

def test_different_key_still_fails_immediately():
    assert 'if(handKnown&&live!=r.key)return false;' in N

def test_body_freshness_still_required():
    assert 'now-body.stamp.tick>80' in N
