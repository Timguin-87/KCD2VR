import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
R=Path(__file__).resolve().parents[1]
m=read_source(R/'src/mod.cpp'); h=read_source(R/'src/mod.h'); v=read_source(R/'src/vr_menu.h'); ui=read_source(R/'src/vr_menu.cpp'); pn=read_source(R/'src/vrik/physical_native.inl'); ini=(R/'KCD2VR.ini').read_text()

def test_user_layout_default_and_compat():
    assert 'user_shoulder_holsters_{true}' in h
    assert 'UserShoulderHolsters' in m and 'LegacyShoulderHolsters' in m
    assert 'UserShoulderHolsters = 1' in ini and 'LegacyShoulderHolsters = 1' in ini

def test_immersion_menu_toggles():
    assert 'user_shoulder_holsters = true' in v and 'bow_manual_reload = false' in v
    assert 'USER SHOULDER LAYOUT: ON' in ui
    assert 'MANUAL BOW / QUIVER RELOAD: ON' in ui
    assert 'SetLegacyHolsterLayout147(state.user_shoulder_holsters)' in m
    assert 'SetBowReload146(state.bow_manual_reload' in m

def test_holster_cue_is_combat_quiet_only():
    assert 'cueMotionQuiet149' in pn
    assert 'melee118::Reason::Candidate' in pn and 'melee118::Reason::Prepared' in pn and 'melee118::Reason::Committed' in pn
    assert 'std::max(out.melee118.handSpeed,out.melee118.tipSpeed)<0.65' in pn
    assert 'HolsterConfirmPulse136' in pn
