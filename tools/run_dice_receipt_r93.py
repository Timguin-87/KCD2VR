"""Run the actual bounded F5 marker receipt without a DLL or native memory access."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--compiler',default='cl');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
source=root/'src/subsystems/dice/hand_select_native_d85.inl'
text=source.read_text();start=text.index('static void DiceMarkerReceiptR93(');brace=text.index('{',start);end=brace+1;depth=1
while depth:
    depth+=(text[end]=='{')-(text[end]=='}');end+=1
extract=out/'dice_receipt_actual.inl';extract.write_text(text[start:end])
results=[]
for name in ['dice_receipt_r93','dice_hud_depth_r92r3']:
    for opt in ['/Od','/O2']:
        exe=executable_path(out/(name+'_'+opt[1:]))
        cmd=command(root/('tests/'+name+'_test.cpp'),exe,[root/'src',root/'tests'],a.compiler)
        if '/O2' in cmd:cmd[cmd.index('/O2')]=opt
        if name=='dice_receipt_r93':cmd.insert(1,('/D' if a.compiler.lower() in ('cl','cl.exe') else '-D')+'KCD2VR_DICE_RECEIPT_R93_EXTRACT="'+str(extract).replace('\\','/')+'"')
        build=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.build.log').write_text(build.stdout)
        if build.returncode:raise RuntimeError(build.stdout)
        run=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.run.log').write_text(run.stdout)
        print(run.stdout,flush=True)
        if run.returncode:raise RuntimeError(run.stdout)
        results.append({'test':name,'optimization':opt,'exit':run.returncode,'output':run.stdout})
paths=[source,root/'src/subsystems/dice/diagnostic_d84.inl',root/'tests/dice_receipt_r93_test.cpp',root/'src/subsystems/dice/hud_depth_r92r3.h',root/'tests/dice_hud_depth_r92r3_test.cpp',Path(__file__)]
(out/'RESULT.json').write_text(json.dumps({'passed':True,'results':results,'source_sha256':{str(x.relative_to(root)):hashlib.sha256(x.read_bytes()).hexdigest() for x in paths},'limits':'Actual diagnostic function with Write replaced by a string sink plus actual depth geometry helper. No native cursor read/write, renderer, DLL or headset validation.'},indent=2))
