from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
s=(root/'src/combat_diag_probe.cpp').read_text()
a=s.index('void SetContactImpulse596(')
b=s.index('void SetContactImpulseDiagnostics19c4r4',a)
Path(sys.argv[1]).write_text(s[a:b])
