"""Extract the actual F11 automatic-request owner and callback; no native code runs."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/vrik/staged_reload596c2.inl')
c=read_source(r/'src/diagnostics/reload_lifecycle178r36c2.inl')
def span(text,a,b):return text[text.index(a):text.index(b,text.index(a))]
parts=[span(s,'static bool ManualEquipment596(','static bool ManualAction596('),
       span(s,'static bool ManualAutomaticReload19c3(void* equipment,','static void ManualBowTick596('),
       span(c,'// C37 automatic-request caller set.','static bool ManualAutomaticFault32() noexcept; // atomic'),
       span(c,'static void ReloadAutomaticRequest19c3(','static void ReloadDispatch36(')]
(r/'tests/generated_reload_auto19c3.inc').write_text('\n'.join(parts),encoding='utf-8')
(r/'tests/reload_auto19c3_provenance.json').write_text(json.dumps({'sources':['src/vrik/staged_reload596c2.inl','src/diagnostics/reload_lifecycle178r36c2.inl'],'sha256':[hashlib.sha256(p.encode()).hexdigest() for p in parts],'native_leaves':'native item lookup, player type, Henry actor and hand key providers substituted; production freshness/owner checks, C37 caller-context admission and actual callback unchanged'},indent=2)+'\n',encoding='utf-8')
