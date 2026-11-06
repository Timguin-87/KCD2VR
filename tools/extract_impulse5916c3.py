import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1];full=read_source(r/'src/diagnostics/geometry_hit178r27.inl');s=full[full.index('namespace impulse596 {\n'):];s=s[:s.index('\nbool CtorsVerified()')]
pieces=[s[s.index('std::atomic<bool> g_enabled'):s.index('static void* EntityById')],s[s.index('constexpr std::uintptr_t kPhysicalProxyVft5916c3'):s.index('static bool Contract19c4r4')],s[s.index('static bool Deliver'):s.rindex('\n}')]]
(r/'tests/generated_impulse5916c3.inc').write_text('\n'.join(pieces))
(r/'tests/impulse5916c3_extraction.json').write_text(json.dumps({'source':'src/diagnostics/geometry_hit178r27.inl','sha256':[hashlib.sha256(p.encode()).hexdigest() for p in pieces],'functions':'Complete native-death receipt cache/admission, Request, OwnerCurrent, Reserve, common entity/parent gate, exact ImpulseBody, Deliver, Receipt, both-fault batch termination, Drain; only native leaves and fingerprint service mocked'},indent=2))
