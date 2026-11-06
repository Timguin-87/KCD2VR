#!/usr/bin/env python3
"""Copy the actual mod R3 mode dispatcher verbatim into its host fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source_bytes
from pathlib import Path
import hashlib
import json

root = Path(__file__).resolve().parents[1]
raw = read_source_bytes(root / 'src/mod.cpp')
source = raw.decode().replace('\r\n', '\n')
start_anchor = '        if(touch_events.special_attack_requested){'
end_anchor = '\n\n        if(touch_events.back_cancel) {'
if source.count(start_anchor) != 1 or source.count(end_anchor) != 1:
    raise SystemExit('Ambiguous/missing production R3 dispatcher boundary')
start = source.index(start_anchor)
end = source.index(end_anchor, start)
body = source[start:end] + '\n'
if not body.endswith('        }\n'):
    raise SystemExit('Unexpected R3 dispatcher end')
(root / 'tests/generated_r3_route176.inc').write_text(
    '// Generated from mod.cpp by tools/extract_r3_route176.py; do not edit.\n' + body)
out = root / 'review/V66.176/r3_dispatch_extract.json'
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps({
    'source': 'src/mod.cpp', 'source_sha256': hashlib.sha256(raw).hexdigest(),
    'excerpt_sha256': hashlib.sha256(body.encode()).hexdigest(),
    'production_decisions_rewritten': False,
}, indent=2) + '\n')
print('Extracted current production R3 mode dispatcher verbatim')
