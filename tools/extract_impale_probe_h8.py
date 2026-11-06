from pathlib import Path
import sys
from source_text import read_source
r=Path(__file__).resolve().parents[1];s=read_source(r/"src/subsystems/gore/native_part_anchor.inl")
a=s.index("static bool ImpaleMainJoin72");b=s.index("// H7: bounded read-only observation",a)
Path(sys.argv[1]).write_text(s[a:b],encoding="utf-8")
