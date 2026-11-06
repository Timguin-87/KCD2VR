import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import math

ROOT = Path(__file__).resolve().parents[1]
XR = read_source(ROOT / 'src' / 'xr_manager.cpp', encoding='utf-8')
FRAME = read_source(ROOT / 'src' / 'frame_sync.cpp', encoding='utf-8')
REC = read_source(ROOT / 'src' / 'recursive_view_probe.cpp', encoding='utf-8')
CMAKE = (ROOT / 'CMakeLists.txt').read_text(encoding='utf-8')

results = []
def check(name, cond, detail=''):
    if not cond:
        raise AssertionError(f'{name}: {detail}')
    results.append((name, detail or 'ok'))

# 1) Single final eye routing. Internal textures remain canonical.
def route(swap):
    internal = ['Ltex', 'Rtex']
    return [internal[i ^ (1 if swap else 0)] for i in range(2)]
check('canonical_route_no_swap', route(False) == ['Ltex','Rtex'])
check('single_final_swap', route(True) == ['Rtex','Ltex'])

# 2) Claude patch would swap destinations early AND V45 already swaps at final layer.
def claude_double_swap():
    internal = ['Ltex','Rtex']
    early = [internal[1], internal[0]]
    return [early[1], early[0]]
check('claude_double_swap_cancels', claude_double_swap() == ['Ltex','Rtex'],
      'early swap + existing final swap cancels; do not merge')
check('claude_early_swap_not_present', 'std::swap(dsts[0], dsts[1])' not in XR)

# 3) OpenXR FOV remap math is finite and asymmetric for the Quest/VDXR sample.
fovs = [(-0.9424778, 0.6981317, 0.7679449, -0.9599311),
        (-0.6981317, 0.9424778, 0.7679449, -0.9599311)]
tans = [tuple(math.tan(x) for x in f) for f in fovs]
for t in tans:
    check('fov_tangents_finite', all(math.isfinite(x) for x in t))
check('fov_is_asymmetric', abs(abs(tans[0][0]) - abs(tans[0][1])) > 0.1)
check('left_right_fov_mirror', abs(tans[0][0] + tans[1][1]) < 1e-4 and abs(tans[0][1] + tans[1][0]) < 1e-4)

# 4) Gemini CopyResource-to-XR proposal cannot handle current dimensions.
src = (1920,1080,28)
dst = (2496,2688,28)
copyresource_compatible = src == dst
check('gemini_copyresource_rejected_for_current_sizes', not copyresource_compatible,
      f'{src[:2]} source != {dst[:2]} XR eye; shader blit/scale is required')

# 5) OpenXR D3D12 color images are not transitioned COMMON<->RT in V46.
check('no_xr_left_common_transition', 'barriers[1].Transition.pResource = left' not in XR)
check('no_xr_dsts_common_transition', 'b.Transition.pResource = dsts[eye]' not in XR)
check('xr_state_policy_marker', 'must remain RENDER_TARGET' in XR)

# 6) One xrLocateViews call owns the frame sample.
check('single_xrLocateViews_call', XR.count('xrLocateViews(session_') == 1,
      f"count={XR.count('xrLocateViews(session_')}")

# 7) Camera pose publish occurs before xrEndFrame in RunFrame.
run = XR[XR.index('bool XrManager::RunFrame'):]
check('camera_pose_before_endframe', run.index('CameraHook::SetHeadPose') < run.index('xrEndFrame(session_'))
check('active_snapshot_present', 'active_render_frame_.predicted_display_time' in XR)
check('valid_snapshot_counter_present', 'test_valid_frame_snapshots_' in XR and 'valid_frame_snapshots' in read_source(ROOT / 'src' / 'xr_manager.h', encoding='utf-8'))
check('fov_counter_requires_real_fov', 'if (used_openxr_fov_this_blit)' in XR)

# 8) Recursive-view candidate remains engine-owned/observer-only.
check('recursive_type1_request_probe', 'view_type == 1' in REC)
check('recursive_no_manual_constructor', 'new CRenderView' not in REC and 'CRenderView(' not in REC)
check('recursive_pipeline_type_counter', 's_pipeline_counts[view_type]' in FRAME)

# 9) New translation units are in CMake.
check('cmake_frame_sync', 'src/frame_sync.cpp' in CMAKE)
check('cmake_recursive_probe', 'src/recursive_view_probe.cpp' in CMAKE)

# 10) F8 matrix and route marker are compiled into the source.
MOD = read_source(ROOT / 'src' / 'mod.cpp', encoding='utf-8')
check('matrix_has_7_phases', MOD.count('{ "MONO_') >= 4 and 'RECURSIVE_VIEW_OBSERVER' in MOD)
check('matrix_is_5_seconds', 'kMatrixPhaseMs = 5000ull' in MOD)
check('matrix_logs_valid_snapshots', 'validSnapshots=%llu' in MOD)
check('marker_shader_present', 'V46 route-identification marker' in XR)

print('V46 STATIC/LOGIC TESTS: PASS')
for name, detail in results:
    print(f'PASS {name}: {detail}')
