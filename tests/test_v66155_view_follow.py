import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
m=read_source(r/'src/mod.cpp', errors='ignore')
f=read_source(r/'src/frame_sync.cpp', errors='ignore')
i=read_source(r/'src/interaction_alignment.cpp', errors='ignore')
checks=[
('head follow forced corrected','SetHeadOwnershipFixEnabled(true)' in m and 'SetHeadOwnershipProbeLevel(3)' in m),
('non-vrik visual interaction default','const bool nonVrikVisualInteraction=(viewFollowVrikMode==0);' in m),
('no head aim required','head_interaction_aim = interaction_alignment_installed && nonVrikVisualInteraction' in m),
('visual ray uses final camera forward','GetLatestVisualForwardWorld' in i),
('vrik head ray disabled by mode','viewFollowVrikMode==0' in m),
('diagnostic present','V66.155 VIEW_FOLLOW' in m and 'headFollow=RENDERER_MATCHED_PRESENT' in m),
]
for n,x in checks: print(('PASS' if x else 'FAIL'),n)
assert all(x for _,x in checks)
