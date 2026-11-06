#!/usr/bin/env python3
"""Run actual extracted contact adapters without writing generated files into source.
Run inside an x64 MSVC developer environment. Native physics/memory are explicit
synthetic leaves; this does not establish game/headset behavior.
"""
import argparse,hashlib,json,os,pathlib,runpy,subprocess,sys
P=pathlib.Path
parser=argparse.ArgumentParser();parser.add_argument('out',type=P);parser.add_argument('--asan',action='store_true');parser.add_argument('--only',nargs='+')
a=parser.parse_args();src=P(__file__).resolve().parents[1];out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
write=P.write_text;replace=os.replace

def redirect(p):
 p=P(p)
 return out/p.name if p.parent==src/'tests' and p.name.startswith('generated_') else p

def write_redirect(self,*args,**kw):return write(redirect(self),*args,**kw)
try:
 P.write_text=write_redirect
 os.replace=lambda source,target:replace(redirect(source),redirect(target))
 for tool in ('extract_adapter_types.py','extract_world_adapter_test.py'):runpy.run_path(str(src/'tools'/tool),run_name='__main__')
finally:P.write_text=write;os.replace=replace
# The contact extractor spans an adjacent roomscale publication include. It is
# not called by these contact units and requires the unrelated live XR host.
# Omit that named host only; ReadModelWorld and its actual origin qualification
# remain compiled. Dialogue input is a real type from the production header.
p=out/'generated_world_adapter.inc';text=p.read_text()
leaf='#include "subsystems/roomscale/pose_producer_d76.inl"'
assert text.count(leaf)==1
p.write_text(text.replace(leaf,'// Explicit omitted host: roomscale pose publication (not a contact consumer).'))
p=out/'generated_adapter_types.h';p.write_text('#include "vrik/dialogue_fingers_d78.h"\n'+p.read_text())

# Actual optional finger presentation owner, with explicit fixture FK leaves.
finger=(src/'src/subsystems/vrik/pose_fingers.inl').read_text(encoding='utf-8')
a0=finger.index('static void ApplyFingerContactSpringH31(')
b0=finger.index('// Old helper retained',a0)
(out/'generated_finger_spring_d81.inc').write_text(finger[a0:b0])

# Exact production settings and policy adapter: no copied switch logic.
frik=(src/'src/frik_pose_probe.cpp').read_text(encoding='utf-8')
functions=[]
for signature in ('bool PreciseContacts44(', 'void SetPreciseContacts44(', 'int HandsModeDs2(', 'void SetHandsModeDs2('):
 start=frik.index(signature);opening=frik.index('{',start);depth=1;end=opening+1
 while depth:
  depth+=(frik[end]=='{')-(frik[end]=='}');end+=1
 functions.append(frik[start:end])
(out/'generated_contact_settings_d79.inc').write_text('\n'.join(functions))
coherent=(src/'src/vrik/coherent_pose.inl').read_text(encoding='utf-8')
start=coherent.index('    const bool balancedContactD79=');end=coherent.index('    const auto contactControl=',start)
(out/'generated_contact_policy_d79.inc').write_text(coherent[start:end])

def local_copy(p):
 text=p.read_text(encoding='utf-8').replace('"../src/','"'+(src/'src').as_posix()+'/')
 text=text.replace('"world_adapter_test.cpp"','"world_fixture_d79.inc"')
 if p.name=='world_adapter_test.cpp':
  leaf='// Explicit roomscale-origin host leaves: this fixture models an unchanged\n// stable synthetic character root. Production ReadModelWorld reads and serial\n// compares it unchanged; native step submission is not under test here.\n#include "subsystems/roomscale/native_roomscale_d76.h"\nnamespace kcd2vr::native_roomscale_d76 {\n bool ReadCharacterOrigin(std::uintptr_t p,Origin& o) noexcept {o={};o.serial=2;return p!=0;}\n bool SameSerial(Surface,std::uintptr_t p,std::uint64_t serial) noexcept {return p!=0&&serial==2;}\n}\n'
  text=leaf+text
 return text
(out/'world_fixture_d79.inc').write_text(local_copy(src/'tests/world_adapter_test.cpp'))
(out/'contact_release_world_d45.inc').write_text(local_copy(src/'tests/world_adapter_test.cpp'))
suite=['contact_balanced_d79_test.cpp','contact_continuity_d63_test.cpp','contact_locality_d64_test.cpp','contact_mixed_hit_d63_test.cpp','contact_continuity_d71_test.cpp','station_hand_contact_d75_test.cpp','contact_release_d45_test.cpp']
suite+=['contact_spatial_probe_d81_test.cpp','contact_finger_d81_test.cpp','contact_recovery_d81_test.cpp']
suite=a.only or suite
results=[]
for file in suite:
 p=src/'tests'/file;n=p.stem;copy=out/file;copy.write_text(local_copy(p));exe=out/(n+'.exe')
 flags=['/Zi','/fsanitize=address'] if a.asan else []
 cmd=['cl.exe','/nologo','/std:c++20','/EHsc','/Od','/W3','/UNDEBUG','/DNOMINMAX','/DWIN32_LEAN_AND_MEAN',*flags,'/I'+str(src/'src'),'/I'+str(out),'/I'+str(src/'tests'),str(copy),'/Fo'+str(out/(n+'.obj')),'/Fe'+str(exe),'/link','/OPT:REF','user32.lib']
 c=subprocess.run(cmd,cwd=out,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);(out/(n+'.build.log')).write_text(c.stdout)
 row={'test':file,'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'build_exit':c.returncode,'command':cmd}
 if c.returncode==0:
  r=subprocess.run([str(exe)],cwd=src,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True);row.update(run_exit=r.returncode,output=r.stdout);(out/(n+'.run.log')).write_text(r.stdout)
 else:row['output']=c.stdout[-6000:]
 print(n,'PASS' if row.get('run_exit')==0 else 'FAIL',row['output'][-2000:],flush=True);results.append(row)
inputs=['src/frik_pose_probe.cpp','src/vrik/coherent_pose.inl','src/vrik/contact_balanced_d79.h','src/vrik/contact_release_d45.h','src/vrik/hand_contact_types.inl','src/subsystems/physics/pose_free_hand_contact.inl','src/subsystems/vrik/pose_fingers.inl','src/subsystems/vrik/pose_hand_display_d62.inl','src/vrik/hand_present_d62.h','tests/world_adapter_test.cpp','tools/extract_world_adapter_test.py','tools/extract_adapter_types.py']
(out/'RESULTS.json').write_text(json.dumps({'asan':a.asan,'inputs':{p:hashlib.sha256((src/p).read_bytes()).hexdigest() for p in inputs},'results':results},indent=2)+'\n')
sys.exit(0 if all(r.get('run_exit')==0 for r in results) else 1)
