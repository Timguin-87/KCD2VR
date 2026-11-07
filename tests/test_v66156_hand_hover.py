import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
r=Path(__file__).resolve().parents[1]
m=read_source(r/'src/mod.cpp', errors='ignore')
i=read_source(r/'src/interaction_alignment.cpp', errors='ignore')
h=read_source(r/'src/interaction_alignment.h', errors='ignore')
checks=[
('source split API','SetVisualFallbackEnabled' in h and 'g_visual_fallback_enabled' in i),
('vrik backend enabled independently','interactionBackendEnabled=head_interaction_aim||vrikHandInteraction' in m),
('visual fallback only non-vrik or exact alchemy gaze owner',
 'SetVisualFallbackEnabled(head_interaction_aim||alchemyHeadSelection63)' in m and
 'alchemyHeadSelection63=alchemyAvailable63&&!alchemyPalmExperiment63' in m and
 'SetPalmRays59c9({})' in m),
('physical interaction not head gated','pi.enabled=live111&&xr_manager_&&interactionStatus114.enabled' in m),
('passive hover exists without grip dead zone','passiveHover156' in m and 'HoverOwner' in m and 'c.trigger<.15f&&h.grip<.35f' not in m),
('hover no auto use','po.fire_use=false;passiveHover156=true' in m),
('bounded hover reach','interaction57c1::HandReach' in m and 'interaction57c1::HandRadius' in m),
('diagnostic','V66.156-HAND-HOVER' in m),
]
for n,x in checks: print(('PASS' if x else 'FAIL'),n)
assert all(x for _,x in checks)
