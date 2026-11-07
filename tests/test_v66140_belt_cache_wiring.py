import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
R=Path(__file__).resolve().parents[1]
H=read_source(R/'src/vrik/physical_weapon.h')
N=read_source(R/'src/vrik/physical_native.inl')
M=read_source(R/'src/mod.cpp')

def test_marker():
    assert_current_identity(R)

def test_cache_fields_exist():
    for x in ['beltMount140','beltKey140','beltGeneration140','beltMountReady140']:
        assert x in H

def test_only_native_belt_uses_cache():
    assert 'if(zone==ph::HolsterZone126::NativeBelt)' in N

def test_cache_written_only_when_holstered():
    assert 'if(!item.held&&liveMountKnown)' in N

def test_drawn_item_prefers_matching_cache():
    assert 'else if(item.held&&cacheMatch)' in N
    assert 'input.mount=hs.beltMount140;input.mountKnown=true;' in N
