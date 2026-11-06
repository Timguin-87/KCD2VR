"""Extract the actual contact-submit observer/call ordering for the host fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import sys
source=Path(__file__).resolve().parents[1]/"src/diagnostics/geometry_hit178r27.inl"
text=read_source(source, encoding="utf-8")
start=text.index("    const bool serialEntered59=serialReservationC2.tryEnter();")
end=text.index("    if(telegraph34){TelegraphWrite34",start)
body=text[start:end]
assert body.count("frik_pose_probe::ObserveHitPartNamespace62(")==1
assert body.count("CallSubmit(")==1
assert body.index("if(serialEntered59)")<body.index("ObserveHitPartNamespace62")<body.index("nativeCallStarted()")<body.index("CallSubmit(")
Path(sys.argv[1]).write_text(body,encoding="utf-8")
