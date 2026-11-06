import argparse,json,os,subprocess,sys,hashlib,re
from native_research112 import PE
from pathlib import Path
r=Path(__file__).resolve().parents[1];p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args();o=a.out.resolve();o.mkdir(parents=True,exist_ok=True)
for script in ['extract_prompt_callbacks_d50.py','extract_prompt_consumer_d53.py']:
 subprocess.run([sys.executable,str(r/'tools'/script),'--out',str(o)],check=True)
cases=['ammo_readiness_k32_test','prompt_callbacks_d50_test','prompt_consumer_d53_test','manual_arrow64_test','subsystems/throw_drop/native_ammo_service_t22_test']
# Select the retained world-left cases: production H41 intentionally rejects
# the retired right/native-attachment routes in the old combined fixture.
f=(r/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp').read_text()
a0=f.index('    for(int side=0;side<2;++side)for(int family=0;family<2;++family)for(int count:{1,3,5}){',f.index('int main()'))
a1=f.index('    // T26: actual service, separate world-held LEFT chest knife; RIGHT unchanged.',a0)
f=f[:a0]+f[a1:]
# The retained production fixture lacks the newer output-only haptic leaf.
# Substitute the device sink, not any admission/ownership decision.
if 'bool HoverHaptic178r14(' not in f:
 f=f.replace('namespace kcd2vr::frik_pose_probe {','namespace kcd2vr::frik_pose_probe {\nbool HoverHaptic178r14(int,std::uint64_t) noexcept {return true;}',1)
f=f.replace('static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){}',
    'static unsigned retireNotesD53=0; static const char* retiredAtD53=nullptr;\nstatic void AmmoNoteT18(const char* tag,int,std::uint64_t,std::uint64_t,const char* why){if(!std::strcmp(tag,"D53_DRAW_RETIRE")){++retireNotesD53;retiredAtD53=why;}}')
f=f.replace('static void Reset(int count0=5,int count1=3){','static void Reset(int count0=5,int count1=3){retireNotesD53=0;retiredAtD53=nullptr;')
receipt_test='''    // The production cleanup preserves the first failed native draw boundary.
    Reset(5,0);in.chestKnivesT26=true;previewUnknown=true;Draw(1);
    now+=20;Tick(0,0);Check(retireNotesD53==1&&retiredAtD53&&!std::strcmp(retiredAtD53,"NATIVE_PREVIEW"));
    now+=20;Tick(0,0);Check(retireNotesD53==1&&payCalls==0&&shootCalls==0);
'''
pos=f.rindex('    std::printf(');f=f[:pos]+receipt_test+f[pos:]
carrier=(r/'src/subsystems/throw_drop/runtime/native_ammo_carrier_t18.inl').read_text()
def body(signature):
 begin=carrier.index(signature);opening=carrier.index('{',begin);depth=1;end=opening+1
 while depth:
  if carrier[end]=='{':depth+=1
  elif carrier[end]=='}':depth-=1
  end+=1
 return carrier[begin:end]
# Restore the missing R11 source helper rather than replacing it with true.
extra='static void* HenryCharacter18(){return &character;}\n'+body('static bool AmmoCharacterCurrentK33(')+'\n'+body('static bool AmmoOwnerT18R11(')+'\n'
if 'static bool AmmoOwnerT18R11(' not in f:
 f=f.replace('#define KCD2VR_AMMO_SERVICE_T22_HOST_TEST',extra+'#define KCD2VR_AMMO_SERVICE_T22_HOST_TEST')
(o/'world_service_d53.cpp').write_text(f)
native=PE(Path('C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll'))
assert hashlib.sha256(native.data).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
rt=(r/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl').read_text();pins=[]
for name,rva in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
 m=re.search(r'char '+name+r'\[\]\{([^}]+)\}',rt);v=bytes(int(x.strip(),0) for x in m[1].split(','));assert native.read(rva,len(v))==v;pins.append((rva,v))
(o/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(v,','.join('0x%02x'%x for x in b)) for v,b in pins))
rows=[]
for case in cases:
 n=Path(case).name;src=(o/'world_service_d53.cpp') if '/' in case else r/'tests'/(case+'.cpp');exe=o/(n+'.exe');bat=o/(n+'.cmd');vc=Path(os.environ.get('ProgramFiles','C:/Program Files'))/'Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
 bat.write_text('@echo off\ncall "'+str(vc)+'" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /std:c++20 /EHsc /O1 /I"'+str(r/'src')+'" /I"'+str(o)+'" /I"'+str(r/'tests/openxr_sdk')+'" "'+str(src)+'" /Fe:"'+str(exe)+'" /Fo:"'+str(o/(n+'.obj'))+'"\nexit /b %errorlevel%\n')
 b=subprocess.run(['cmd','/d','/c',str(bat)],capture_output=True,text=True);(o/(n+'.compile.log')).write_text(b.stdout+b.stderr);row={'case':case,'compile_exit':b.returncode}
 if b.returncode==0:
  t=subprocess.run([str(exe)],capture_output=True,text=True);row.update(run_exit=t.returncode,summary=t.stdout[-1000:]);(o/(n+'.run.log')).write_text(t.stdout+t.stderr)
 row['passed']=row.get('run_exit')==0;rows.append(row);print(json.dumps(row),flush=True)
inputs=['src/vrik/physical_weapon.h','src/subsystems/hud_ui_controls/runtime/prompt_device_lock.cpp','src/subsystems/ranged_reload/manual_arrow64.inl','src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl']
(o/'RESULTS.json').write_text(json.dumps({'tests':rows,'passed':all(x['passed'] for x in rows),'native_game_execution':False,'headset_tested':False,'inputs':{x:hashlib.sha256((r/x).read_bytes()).hexdigest() for x in inputs}},indent=2))
raise SystemExit(0 if all(x['passed'] for x in rows) else 1)
