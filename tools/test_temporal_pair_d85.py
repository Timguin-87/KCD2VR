#!/usr/bin/env python3
"""Replay real watchdog/native claim/transaction policy; no premature temporal publisher."""
from pathlib import Path
import argparse,json,subprocess
import render_host_compiler as host
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
policy=(root/'src/temporal_pair_watchdog_d85.h').read_text()
source=(root/'src/device_hook.cpp').read_text()
assert 'PublishTemporalStartFallbackR91' not in source
assert 'const bool d85_fallback=false,d85_fallback_sdk=false;' in source
assert 'if(!d82_temporal && g_v6615_active.load' in source
assert 'RecordTemporalFinalMainD85(list_this' in source
runs=[]
def run(name,text,negative=False):
 d=a.out/name;d.mkdir(parents=True,exist_ok=True)
 (d/'temporal_pair_watchdog_d85.h').write_text(text)
 exe=host.binary(d/'replay')
 cmd=host.command(['g++','-std=c++20','-O2','-Wall','-Wextra','-I'+str(d),'-I'+str(root/'src'),str(root/'tests/temporal_pair_watchdog_d85_test.cpp'),'-o',str(exe)])
 r=subprocess.run(cmd,capture_output=True,text=True);(d/'compile.log').write_text(r.stdout+r.stderr)
 if r.returncode:raise RuntimeError(str(d/'compile.log'))
 r=subprocess.run([str(exe)],capture_output=True,text=True);(d/'run.log').write_text(r.stdout+r.stderr)
 if (r.returncode==0)==negative:raise RuntimeError(str(d/'run.log'))
 runs.append({'name':name,'expected_failure':negative,'exit':r.returncode,'output':r.stdout.strip()})
run('production',policy)
mutants={
 'premature_sdk_fallback':policy.replace('if(temporal)return false;','if(temporal&&starvationFallback&&fallbackSdkReady){viaFallback=true;return !consumed.exchange(true);} if(temporal)return false;'),
 'temporal_stamp_authorizes':policy.replace('if(temporal)return false;','if(temporal)return temporal_compose_d82::ClaimRightCopy(consumed,true,nativeRightMain,composition,frame,currentOutput,samePairSdkReady);'),
 'native_disabled':policy.replace('if(temporal)return false;','return false;'),
 'timeout_no_attempts':policy.replace('if(attemptsSinceTemporal.load(std::memory_order_acquire)<kMinFailedAttempts)return false;',''),
}
for n,t in mutants.items():assert t!=policy;run(n,t,True)
(a.out/'RESULT.json').write_text(json.dumps({'kind':'production policy MSVC/host replay; no D3D12 execution','runs':runs},indent=2)+'\n')
print(runs[0]['output']);print('PASS: 4 negative controls rejected')
