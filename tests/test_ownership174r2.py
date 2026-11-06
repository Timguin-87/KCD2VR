import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def test_trigger_arbitration_precedes_both_transports():
    assert_filtered_pad_route(read_source(ROOT/"src/mod.cpp"))
