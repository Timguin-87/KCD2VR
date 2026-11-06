#!/usr/bin/env python3
"""Compare protected view/stereo/calibration/bow source against the supplied R93R4 ZIP.

This proves source preservation, not runtime smoothness or binary equivalence.
The only allowed RunFrame difference is the read-only fresh-hand anchor copy.
"""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile
from test_fresh_pose_uncapped_menu import block


def sha(data):return hashlib.sha256(data).hexdigest()


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('baseline_zip',type=Path);p.add_argument('output',type=Path)
    a=p.parse_args();root=Path(__file__).resolve().parents[1]
    checks=[]
    with zipfile.ZipFile(a.baseline_zip) as z:
        prefixes=[n[:-len('src/xr_manager.cpp')] for n in z.namelist() if n.endswith('src/xr_manager.cpp')]
        if len(prefixes)!=1:raise ValueError('Baseline must contain exactly one source root')
        prefix=prefixes[0]
        def old(path):return z.read(prefix+path)
        paths={'src/camera_hook.cpp','src/camera_hook.h','src/frame_sync.cpp','src/frame_sync.h','src/mod.cpp',
               'src/interaction_alignment.h','src/interaction_alignment.cpp','src/vrik/bow_arm_aim_r9.h',
               'src/subsystems/ranged_reload/bow_physical_aim_d65c.h',
               'src/subsystems/ranged_reload/bow_physical_aim_shoot_d65c.inl'}
        for path in (root/'src').rglob('*'):
            if path.is_file() and any(x in path.name for x in ('native_stereo','native_dlss','native_fsr','native_xess','native_upscaler','stereo_upscaler','stereo_runtime','calibration')):
                paths.add(path.relative_to(root).as_posix())
        for path in sorted(paths):
            before=old(path);after=(root/path).read_bytes()
            equal=before==after
            allowed=None
            if path=='src/mod.cpp':
                old_default=b'"VrComboKeepSlotPhase2",true)'
                new_default=b'"VrComboKeepSlotPhase2",false)'
                allowed='Only the missing-key combo-slot experiment default is OFF; saved values and all other Mod code are unchanged.'
                equal=before.count(old_default)==1 and after.count(new_default)==1 and before.replace(old_default,new_default)==after
            checks.append({'file':path,'equal':equal,'allowed_change':allowed,'baseline_sha256':sha(before),'current_sha256':sha(after)})
        path='src/xr_manager.cpp';before=old(path).decode('utf-8-sig').replace('\r\n','\n');after=(root/path).read_text(encoding='utf-8-sig')
        names=('SampleHeadPoseAtTime','SampleHeadPoseNow','SampleLateHeadPose','LocateFamilyForGameFrameHJ1','NoteCameraApplyHJ1',
               'CopyLatestStereoViews','CopyLastGoodStereoViews','CopyActiveRenderFrameState','CopyRenderFrameStateByIndex',
               'BlitBridgePairStableToEyes','SubmitBridgeImagesOnRenderThread')
        for name in names:
            marker=('void' if name=='NoteCameraApplyHJ1' else 'bool')+' XrManager::'+name+'('
            left=block(before,marker);right=block(after,marker)
            checks.append({'file':path,'function':name,'equal':left==right,'baseline_sha256':sha(left.encode()),'current_sha256':sha(right.encode())})
        marker='bool XrManager::RunFrame(';left=block(before,marker);right=block(after,marker)
        observer='''        if(motion_trials_d67::UncappedSelectedAt(0)&&now>0&&frame_state.predictedDisplayPeriod>0)
            (void)fresh_hands_r94::anchors.publish({frame_state.predictedDisplayTime,now,
                frame_state.predictedDisplayPeriod,tracking59c6::epoch.load(std::memory_order_acquire)});
'''
        checks.append({'file':path,'function':'RunFrame','equal':right.count(observer)==1 and left==right.replace(observer,''),
                       'allowed_change':'one immutable prediction anchor observation; no camera/render output or target change'})
        path='src/tracking_sample59c6.h';before=old(path).decode('utf-8-sig').replace('\r\n','\n');after=(root/path).read_text(encoding='utf-8-sig')
        for marker in ('inline bool ReadHead(','inline void PublishHead(','inline void SetCameraSource('):
            checks.append({'file':path,'function':marker,'equal':block(before,marker)==block(after,marker)})
    result={'status':'PASS' if all(c['equal'] for c in checks) else 'FAIL','checks':checks,
            'scope':'Exact protected source/functions; permitted RunFrame observation isolated; runtime effects NOT_MEASURED',
            'baseline_zip_sha256':sha(a.baseline_zip.read_bytes())}
    a.output.mkdir(parents=True,exist_ok=True);(a.output/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(result['status'],len(checks),'protected-source checks')
    for c in checks:
        if not c['equal']:print('DIFF',c['file'],c.get('function',''))
    return 0 if result['status']=='PASS' else 1


if __name__=='__main__':raise SystemExit(main())
