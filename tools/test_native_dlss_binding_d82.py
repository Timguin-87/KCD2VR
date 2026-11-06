#!/usr/bin/env python3
"""Independent literal DLSS installation/interleaving and reentrancy review."""
from pathlib import Path
import argparse,hashlib,json,subprocess
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);p.add_argument('--baseline',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
cpp=(root/'src/native_dlss_stereo_d82.cpp').read_text();runtime=(root/'src/native_dlss_stereo_d82_runtime.inl').read_text()
prefix=(root/'tests/native_dlss_stereo_d82_test.cpp').read_text().split('int main() {',1)[0]
state=cpp[cpp.index('bool Installed() noexcept {'):cpp.index('void LogStatus(')]
def install(source):return source[source.index('bool Install() noexcept {'):source.index('bool Installed() noexcept {')]
def run(cmd,path,expected=0,marker=None):
    r=subprocess.run(cmd,capture_output=True,text=True,timeout=30);path.write_text(r.stdout+r.stderr)
    if r.returncode!=expected or (marker and marker not in r.stdout+r.stderr):raise RuntimeError(f'{path}: rc={r.returncode}\n{r.stdout}{r.stderr}')
    return {'command':cmd,'exit':r.returncode,'output':r.stdout.strip()}
def build(name,body):
    out=a.out/name;out.mkdir(exist_ok=True)
    (out/'native_dlss_runtime_host_prefix.inc').write_text(prefix)
    (out/'native_dlss_stereo_d82_runtime.inl').write_text(runtime)
    (out/'native_dlss_state_d82_actual.inc').write_text(state)
    (out/'native_dlss_install_d82_actual.inc').write_text(body)
    exe=out/'binding';run(['g++','-std=c++20','-O1','-Wall','-Wextra','-pthread','-I'+str(out),'-I'+str(root/'src'),str(root/'tests/native_dlss_binding_d82_test.cpp'),'-o',str(exe)],out/'build.log')
    return out,exe
hashes={str(p.relative_to(root)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/native_dlss_stereo_d82.cpp',root/'src/native_dlss_stereo_d82_runtime.inl']}
out,exe=build('actual',install(cpp));rows=[]
for case in ['success','late_module','wrong_pin','wrong_export','construction','enable0','enable1','enable2','enable3','enable4','race','reentrancy']:
    row=run([str(exe),case],out/(case+'.log'));rows.append(row);print(row['output'])
result={'runs':rows,'production_sha256':hashes,'limits':'Literal production Install, actual runtime/state accessors; native pin/module/DetourModKit/vendor API leaves controlled; Windows/GPU/headset NOT_RUN.'}
if a.baseline:
    out,exe=build('pre_install_publication_negative',install(a.baseline.read_text()))
    result['old_source_negative']=run([str(exe),'race'],out/'race.log',1,'!finishedDuringEnable')
    result['baseline_sha256']=hashlib.sha256(a.baseline.read_bytes()).hexdigest()
old='for(auto& hook:g_hooks)if(hook)(void)hook->disable();';assert old in install(cpp)
out,exe=build('discard_trampoline_negative',install(cpp).replace(old,old+'\n        for(auto& hook:g_hooks)hook.reset();g_create=nullptr;g_evaluate=nullptr;g_evaluate_c=nullptr;g_release=nullptr;g_shutdown=nullptr;',1))
# Fail the first enabled hook, so no synthetic callbacks can execute freed code.
result['discard_trampoline_negative']=run([str(exe),'enable3'],out/'run.log',1,'destroyedHooks==0')
assert hashes=={rel:hashlib.sha256((root/rel).read_bytes()).hexdigest() for rel in hashes},'Production changed during test; rerun'
(a.out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
print('PASS: 12 actual binding scenarios, retained-trampoline negative, optional real pre-fix creation race negative')
