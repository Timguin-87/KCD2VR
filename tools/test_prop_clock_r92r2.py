"""Exercise the real servo plus freshly extracted staged consumer, without a DLL."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
from check_interactions_d84 import extract

ROOT=Path(__file__).resolve().parents[1]
BASELINE='22c60c4e8fa890385683b7b269f2df6d1f3cbbe2'

def main():
    p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True)
    out=p.parse_args().out.resolve();out.mkdir(parents=True,exist_ok=True)
    source=ROOT/'src/vrik/staged_pickup596c2.inl'
    consumer=out/'current_consumer.inc';consumer.write_text(extract(source))
    sources=[ROOT/'src/vrik/prop_drive5916c1.h',source,
             ROOT/'tests/prop_clock_r92r2_test.cpp',ROOT/'tests/prop_drive5916c1_test.cpp']
    results=[]
    def run(name,test,flag='/O2',baseline=False):
        exe=executable_path(out/name)
        includes=([out/'baseline_include'] if baseline else [])+[ROOT/'src',ROOT/'src/vrik']
        cmd=command(ROOT/'tests'/test,exe,includes,'cl');cmd[cmd.index('/O2')]=flag
        cmd.insert(1,'/DPROP_ADAPTER_D78_INC="'+consumer.as_posix()+'"')
        built=subprocess.run(cmd,cwd=out,text=True,capture_output=True)
        (out/(name+'.build.log')).write_text(built.stdout+built.stderr)
        if built.returncode:raise RuntimeError(name+' compile failed: '+built.stdout+built.stderr)
        executed=subprocess.run([str(exe)],cwd=out,text=True,capture_output=True)
        (out/(name+'.run.log')).write_text(executed.stdout+executed.stderr)
        passed=(executed.returncode==1 if baseline else executed.returncode==0)
        results.append({'name':name,'passed':passed,'compile_exit':built.returncode,
                        'run_exit':executed.returncode,'expected_failure':baseline,'output':executed.stdout.strip()})
        print(name+': '+executed.stdout.strip())
        if not passed:raise RuntimeError(name+' unexpected result')
    for name,flag in [('debug','/Od'),('optimized','/O2')]:
        run('consumer_'+name,'prop_clock_r92r2_test.cpp',flag)
        run('servo_'+name,'prop_drive5916c1_test.cpp',flag)
    old=subprocess.check_output(['git','show',BASELINE+':src/vrik/prop_drive5916c1.h'],cwd=ROOT)
    shadow=out/'baseline_include/vrik/prop_drive5916c1.h';shadow.parent.mkdir(parents=True,exist_ok=True);shadow.write_bytes(old)
    run('baseline_expected_failure','prop_clock_r92r2_test.cpp',baseline=True)
    report={'passed':all(x['passed'] for x in results),'baseline':BASELINE,'results':results,
            'source_sha256':{str(x.relative_to(ROOT)):hashlib.sha256(x.read_bytes()).hexdigest() for x in sources},
            'consumer_sha256':hashlib.sha256(consumer.read_bytes()).hexdigest(),
            'scope':'Actual current Controller and literal StagedPropTick596; substituted native leaves. No DLL, engine physics or headset execution.'}
    (out/'RESULT.json').write_text(json.dumps(report,indent=2)+'\n')

if __name__=='__main__':main()
