#!/usr/bin/env python3
"""Host replay of the actual DLSS router -> aggregate -> native eye scheduler.
Run from an MSVC developer shell on Windows; never builds a mod DLL.
"""
import argparse, hashlib, json, subprocess
from pathlib import Path
import render_host_compiler as host

def function(text, prefix):
    start=text.index(prefix); opening=text.index('{',start); depth=1; i=opening+1
    while depth:
        depth += (text[i]=='{')-(text[i]=='}'); i+=1
    return text[start:i]

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
    root=Path(__file__).resolve().parents[1];out=args.out.resolve();out.mkdir(parents=True,exist_ok=True)
    files=['src/native_dlss_stereo_d82.cpp','src/native_dlss_stereo_d82_runtime.inl',
           'src/native_upscaler_stereo_d82.cpp','src/native_stereo_eye_control_v6604.inc']
    texts={p:(root/p).read_text() for p in files}
    cpp,runtime,aggregate,eye=(texts[p] for p in files)
    files+=['src/fsr_stereo_runtime_d82.h','src/native_fsr_stereo_d82.cpp','tests/dlss_recovery_r92_test.cpp']
    state=cpp[cpp.index('bool Installed() noexcept {'):cpp.index('void LogStatus(')]
    agg=aggregate[aggregate.index('namespace kcd2vr::native_upscaler_stereo_d82 { namespace {'):aggregate.index('void InstallEarly(')]
    for prefix in ['void SetMode(', 'bool PairReady(', 'bool RecoverableResetCut(', 'void BeginEpoch(', 'void EndEpoch(']:
        agg+=function(aggregate,prefix)+'\n'
    agg+='}\n'
    types=eye[eye.index('enum class ControlPass'):eye.index('thread_local EyeFlow*')]
    for prefix in ['bool IsStereoDesign(', 'bool CapturesStereoOutput(', 'bool RepeatsStereoFrames(']:types+=function(eye,prefix)+'\n'
    control=function(eye,'void MaybeRunEyeControl(')
    result={'kind':'actual source functions; OS/vendor/engine leaves substituted','dll_built':False,'headset_test':False,
            'hashes':{p:hashlib.sha256((root/p).read_bytes()).hexdigest() for p in files},'runs':[],'mutations':[]}
    def case(name,s=state,a=agg,c=control,opt='-O2',negative=False):
        folder=out/name;folder.mkdir(exist_ok=True)
        data={'native_dlss_state_d82_actual.inc':s,'native_dlss_stereo_d82_runtime.inl':runtime,
              'dlss_aggregate_r92_actual.inc':a,'dlss_eye_types_r92_actual.inc':types,'dlss_eye_control_r92_actual.inc':c}
        for fn,content in data.items():(folder/fn).write_text(content)
        exe=host.binary(folder/'replay')
        command=host.command(['g++','-std=c++20','-pthread','-Wall','-Wextra',opt,'-I'+str(folder),'-I'+str(root/'src'),
                              str(root/'tests/dlss_recovery_r92_test.cpp'),'-o',str(exe)])
        compile=subprocess.run(command,capture_output=True,text=True);(folder/'compile.log').write_text(compile.stdout+compile.stderr)
        if compile.returncode:raise RuntimeError('Compile failed: '+str(folder/'compile.log'))
        run=subprocess.run([str(exe)],capture_output=True,text=True,timeout=15);(folder/'run.log').write_text(run.stdout+run.stderr)
        row={'name':name,'exit_code':run.returncode,'output':run.stdout+run.stderr}
        if negative:
            row['rejected']=run.returncode!=0;result['mutations'].append(row);assert row['rejected'],name
        else:
            row['passed']=run.returncode==0;result['runs'].append(row);assert row['passed'],row
    case('debug',opt='-O0');case('optimized')
    changes=[('legacy_stop',state,agg,control.replace('if(retryCutR92) {','if(false) {')),
             ('fsr_cut_not_forwarded',state,agg.replace('(tech!=1&&tech!=2)','tech!=2'),control),
             ('ignore_native_completion',state,agg,control.replace('temporalCutR92 && outcome==3u && restored','temporalCutR92 && restored')),
             ('ignore_cut_frame',state.replace('p.recoverableCut && p.cutFrame==frame &&','p.recoverableCut &&'),agg,control),
             ('accept_discarded_pair',state,agg,control.replace('outcome==3u&&captureOk&&temporalPairD82','outcome==3u&&captureOk'))]
    for name,s,a,c in changes:
        assert (s,a,c)!=(state,agg,control);case(name,s,a,c,negative=True)
    (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
if __name__=='__main__':main()
