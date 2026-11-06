"""Extract the actual fixed-size SEH native-read leaf, not a test reimplementation."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import sys
from pathlib import Path
source=read_source(Path(__file__).resolve().parents[1]/'src/npc_lookat_hmd.cpp')
begin=source.index('template<class T> bool ReadTarget47c1(')
end=source.index('\nbool MatchEntry(',begin)
Path(sys.argv[1]).write_text('// Actual production leaf extracted from npc_lookat_hmd.cpp\n'+source[begin:end]+'\n')
