import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from source_contract5916c3 import assert_current_identity, assert_filtered_pad_route
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def text(p): return read_source(ROOT/p, errors='ignore')

def test_head_aim_kernel_retired_after_headset_regression():
    s=text('src/head_aim120r3.h')
    assert 'kEnabled = false' in s
    assert 'equipment==1 && trigger>0.55f' in s
    assert 'RangedHeadAim' in s

def test_production_path_uses_right_trigger_not_right_grip():
    s=text('src/mod.cpp')
    assert 'RangedHeadAim(aimEquipment,rrAim.trigger)' in s
    assert 'aimEquipment,rrAim.trigger);' in s
    block=s[s.index('const auto& rrAim=xr_manager_->RightController()'):s.index('view_marriage_visual_active=false;',s.index('const auto& rrAim=xr_manager_->RightController()'))]
    assert 'RangedHeadAim(aimEquipment,rrAim.grip)' not in block

def test_native_input_transition_reanchors():
    s=text('src/native_input_look_lab.cpp')
    assert 'if(old!=active){ ResetAnchor();' in s
    assert 'g_consumer_anchor.store(false' in s
    assert 'g_consumed_seq.store(0' in s

def test_old_ini_and_menu_cannot_restore_head_aim():
    m=text('src/mod.cpp'); v=text('src/vr_menu.cpp'); h=text('src/vr_menu.h')
    assert 'EnableNativeHeadAim", enable_native_head_aim_.load' in m
    assert 'VrHeadAimPreferenceVersion",4' in m
    assert 'EnableNativeHeadAim","true"' not in m
    assert 's.native_head_look=!s.native_head_look' not in v
    assert 'RANGED HEAD AIM: ON' not in v
    assert 'bool native_head_look = false' in h

def test_v115_fresh_sampling_migrates_old_cached_preference():
    m=text('src/mod.cpp');h=text('src/mod.h');ini=text('KCD2VR.ini')
    assert 'settings.bind("VrPoseSourceMode", vr_pose_source_mode_, 2)' in m
    assert '"VrPoseSourceMode", 2)' in m
    assert '"VrPoseSourceMode","2"' in m
    assert 'wi("VrHeadSmoothPreferenceVersion",2)' in m
    assert 'vr_pose_source_mode_{2}' in h
    assert 'VrPoseSourceMode = 2' in ini

def test_normal_camera_stays_openxr_when_native_head_aim_inactive():
    s=text('src/mod.cpp')
    assert 'CameraHook::SetNativeHeadRotationOwner(productionHeadAim&&native_head_rotation_owned);' in s
    assert '(native_head_rotation_owned ? false : apply_head_rotation_.load' in s

def test_build_marker():
    assert_current_identity(ROOT)
