"""Extract actual production menu cases for the H26 regression fixture."""
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
s=(root/'src/vr_menu.cpp').read_text()
a=s.index('    case DragControl::GameFov:')
b=s.index('    case DragControl::CenterX:',a)
Path(sys.argv[1]).write_text('#include <initializer_list>\n#include <cassert>\n#include <cmath>\n#include <cstdio>\nstruct State {float game_fov_target=95,source_vertical_fov_deg=95,corrected_center_scale_fov_deg=95;int cyclopean_mode=0;};\nenum class DragControl{GameFov,SourceFov};\nvoid drag(State& s,DragControl dc,float fraction){const int kV4GameFov=0,kV4SourceFov=1;auto t=[&](int){return fraction;};switch(dc){\n'+s[a:b]+'}}\nint main(){for(int mode:{0,1})for(auto control:{DragControl::GameFov,DragControl::SourceFov})for(float pos:{0.f,.25f,.5f,.75f,1.f}){\nState s;s.cyclopean_mode=mode;drag(s,control,pos);assert(s.game_fov_target==95&&s.source_vertical_fov_deg==s.game_fov_target&&s.corrected_center_scale_fov_deg==s.game_fov_target);\n}std::puts("H26 exact production Game/VR slider cases: both controls and both modes remain linked PASS");}\n')
