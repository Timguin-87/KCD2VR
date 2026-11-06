import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
s=(r/'src/subsystems/interactions/held_visual_policy.inl').read_text(encoding='utf-8-sig')
(r/'tests/generated_held_visual43.inc').write_text('// Actual production visual-only permission; regenerate via tools/extract_held_visual43.py.\n'+s.rstrip()+'\n')
