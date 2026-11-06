#!/usr/bin/env python3
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/diagnostics/geometry_hit178r27.inl')
a=s.index('static bool ChargeEffort5916(')
# Extract the function itself, not every later helper before Forget(). Native
# clock/ownership helpers are separate boundaries and must not enter this mock.
b=s.index('\n}\n',a)+3
text=s[a:b]
assert text.count('static bool ')==1 and 'diagnostic_trace::Write(m);return paid&&recorded;' in text
(r/'tests/effort_transaction5916c1.inc').write_text(text)
(r/'tests/effort_transaction5916c1_provenance.json').write_text(json.dumps({'production_sha256':hashlib.sha256(text.encode()).hexdigest(),'native_leaves':'EffortNative5916 + clock/output spies only; no native game execution'},indent=2))
s=read_source(r/'src/vrik/physical_native.inl')
a=s.index('static void PhysPublishImpactEffort19(')
b=s.index('\n}\n',a)+3
text=s[a:b]
assert text.count('static void ')==1 and 'ledger.publish' in text
(r/'tests/effort_impact_publisher19.inc').write_text(text)
