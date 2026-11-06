"""Source-bound die marker geometry and exact camera-publication regression."""
from pathlib import Path
import argparse, hashlib, json, subprocess
from host_compiler import command, executable_path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--compiler',default='cl');a=p.parse_args()
out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
extract=out/'visual_ray_d75_actual.inl'
subprocess.run(['python',str(root/'tests/extract_visual_ray_d75.py'),'--out',str(extract)],check=True)
flash=(root/'src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp').read_text()
functions=[]
for signature in ['bool DiceMarkerLeaseR92()', 'void ServiceDiceMarkerR92()']:
 start=flash.index(signature);brace=flash.index('{',start);depth=1;end=brace+1
 while depth:
  depth+=(flash[end]=='{')-(flash[end]=='}');end+=1
 functions.append(flash[start:end])
service=out/'dice_marker_service_actual.inl';service.write_text('\n\n'.join(functions))
results=[]
for name in ['dice_marker_r92','dice_marker_handover_r92','visual_ray_d75']:
 for opt in ['/Od','/O2']:
  exe=executable_path(out/(name+opt[1:]));cmd=command(root/'tests'/f'{name}_test.cpp',exe,[root/'src',root/'tests'],a.compiler)
  if '/O2' in cmd:cmd[cmd.index('/O2')]=opt
  if name=='dice_marker_handover_r92':cmd.insert(1,('/D' if a.compiler.lower() in ('cl','cl.exe') else '-D')+'KCD2VR_DICE_HANDOVER_R92_EXTRACT="'+str(service).replace('\\','/')+'"')
  if name=='visual_ray_d75':cmd.insert(1,('/D' if a.compiler.lower() in ('cl','cl.exe') else '-D')+'KCD2VR_VISUAL_RAY_D75_EXTRACT="'+str(extract).replace('\\','/')+'"')
  b=subprocess.run(cmd,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.build.log').write_text(b.stdout)
  if b.returncode:raise RuntimeError(b.stdout)
  r=subprocess.run([str(exe)],stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);exe.with_suffix('.run.log').write_text(r.stdout);print(r.stdout,flush=True)
  if r.returncode:raise RuntimeError(r.stdout)
  results.append({'test':name,'optimization':opt,'exit':r.returncode,'output':r.stdout})
paths=['src/subsystems/hud_ui_controls/runtime/flashui_probe.cpp','tests/dice_marker_handover_r92_test.cpp','src/subsystems/dice/selection_marker_r92.h','src/vrik/visual_ray_d75.h','src/camera_hook.cpp','src/xr_manager.cpp','tests/dice_marker_r92_test.cpp','tests/visual_ray_d75_test.cpp','tools/run_dice_marker_r92.py']
(out/'RESULT.json').write_text(json.dumps({'passed':True,'results':results,'sha256':{x:hashlib.sha256((root/x).read_bytes()).hexdigest() for x in paths},'limits':'Actual marker header and extracted actual visual-camera publication/getter. Actual marker Service/lease executed with native Flash leaves substituted. No native die selector or OpenXR composition exercised; no DLL.'},indent=2))
