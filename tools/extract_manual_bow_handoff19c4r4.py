import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1];s=read_source(r/'src/vrik/staged_reload596c2.inl')
a=s.index('struct ManualReady596 {');b=s.index('static std::atomic<bool> g_manual_bow_fault596',a);part=s[a:b]
(r/'tests/manual_bow_handoff19c4r4.inc').write_text(part)
# Assert the real coherent producer passes the exact request AFTER pose/tracking fields are populated.
c=read_source(r/'src/vrik/coherent_pose.inl');needle='pi.epoch=preparedRequest169r2;\n            ManualNockInput19c4r4(pi,tx.physicalProfiles[1],g_phys_state6698.sample);';assert needle in c
assert c.index('pi.intentTracked[side]=')<c.index(needle)<c.index('tx.physicalOutput=ph::step(')
(r/'tests/manual_bow_handoff19c4r4_provenance.json').write_text(json.dumps({'source':'src/vrik/staged_reload596c2.inl','sha256':hashlib.sha256(part.encode()).hexdigest(),'actual_coherent_caller':needle,'caller_connected':True,'leaves':'none in readiness and one-use publication; exact native attachment producers covered separately'},indent=2)+'\n')
