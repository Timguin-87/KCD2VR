from pathlib import Path
import argparse,subprocess,json,hashlib,os
from host_compiler import command, executable_path
ROOT=Path(__file__).resolve().parents[1]
a=argparse.ArgumentParser();a.add_argument('--out',type=Path,required=True);a.add_argument('--compiler',default='cl');args=a.parse_args()
out=args.out.resolve();assert ROOT not in out.parents;out.mkdir(parents=True,exist_ok=True)
adpath=ROOT/'src/subsystems/ranged_reload/crossbow_native_xr34.inl';ad=adpath.read_text()
start=ad.index('    __try{started=g_crossbow_xr34.startNewAfterRestore(');endtxt='__finally{(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}';end=ad.index(endtxt,start)+len(endtxt)
body=ad[start:end].replace('    __try{','    {struct HostFinishD82 {ma::Token& token;bool& started;~HostFinishD82(){(void)ma::Finish(token,&XR34FinishAdmissionT25,&started);}} finish{token,started};',1).replace('}\n    '+endtxt,'}')
ad=ad[:start]+body+ad[end:]
fixture=(ROOT/'tests/crossbow_native_xr34_test.cpp').read_text().replace('"../src/','"'+(ROOT/'src').as_posix()+'/').replace('"'+adpath.as_posix()+'"','"native_adapter_host.inc"')
runs=[]
for name,opt,mutant in [('debug','/Od',False),('optimized','/O2',False),('lost_raw_reach','/Od',True)]:
 d=out/name;d.mkdir(exist_ok=True)
 (d/'crossbow_native_xr34_test.cpp').write_text(fixture)
 (d/'native_adapter_host.inc').write_text(ad.replace('return reader&&reader(owner,input);','return false;') if mutant else ad)
 test=d/'crossbow_bolt_intent_r95r1_test.cpp';test.write_text((ROOT/'tests/crossbow_bolt_intent_r95r1_test.cpp').read_text())
 exe=executable_path(d/'test');cmd=command(test,exe,[ROOT/'src',ROOT/'tests/syntax_shims',d],args.compiler)
 cmd=[opt if p=='/O2' else ('-O0' if opt=='/Od' else '-O2') if p=='-O2' else p for p in cmd]
 p=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(d/'compile.log').write_text(p.stdout)
 if p.returncode:raise RuntimeError(p.stdout[-5000:])
 p=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(d/'run.log').write_text(p.stdout);exe.unlink()
 ok=(p.returncode==1 and 'FAIL_BOLT_R95R1' in p.stdout) if mutant else p.returncode==0
 runs.append(dict(name=name,passed=ok,expected_failure=mutant,exit_code=p.returncode,output=p.stdout,command=cmd));print(name,p.stdout,flush=True)
 if not ok:raise RuntimeError(name+' failed')
sources=[adpath,adpath.with_suffix('.h'),ROOT/'tests/crossbow_native_xr34_test.cpp',ROOT/'tests/crossbow_bolt_intent_r95r1_test.cpp',Path(__file__)]
(out/'RESULT.json').write_text(json.dumps(dict(passed=True,runs=runs,sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},limitations=['Native memory, inventory and pose are fixture leaves. No DLL, link, headset or game execution.']),indent=2))
