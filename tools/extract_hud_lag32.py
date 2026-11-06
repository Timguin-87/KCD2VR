"""Extract the actual compositor lag block and its quaternion math for host tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
R=Path(__file__).resolve().parents[1]
s=read_source(R/'src/xr_manager.cpp')
def function(signature):
 start=s.index(signature);brace=s.index('{',start);depth=1;i=brace+1
 while depth:
  depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[start:i]+'\n'
math='struct V3f { float x,y,z; };\n'+''.join(function(sig) for sig in (
 'static XrQuaternionf QConj(', 'static XrQuaternionf QMul(', 'static V3f QRotate(',
 'XrQuaternionf QNormalize20(', 'XrQuaternionf QNlerp20(', 'float QAngle20('))
start=s.index('            const float lag=hud_lag_.load(',s.index('const auto xr_hud ='))
end=s.index('hud_lag_last_ns_=nowNs;',start)+len('hud_lag_last_ns_=nowNs;')
(R/'tests/generated_hud_lag32_math.inl').write_text('// Actual xr_manager.cpp math; regenerate, do not hand edit.\n'+math)
(R/'tests/generated_hud_lag32.inl').write_text('// Actual xr_manager.cpp compositor block.\n'+s[start:end]+'\n')
print('Extracted actual HUD lag compositor and math')
