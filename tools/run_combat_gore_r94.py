"""Focused current-source adapters; explicit native leaves, no game/PAK build."""
from pathlib import Path
import argparse,hashlib,json,os,struct,subprocess,sys
from host_compiler import command,executable_path
ROOT=Path(__file__).resolve().parents[1]
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',type=Path,required=True);ap.add_argument('--compiler',default='cl');ap.add_argument('--lua-deps',type=Path,required=True);a=ap.parse_args();out=a.out.resolve();out.mkdir(parents=True,exist_ok=True)
 rows=[];hashes={}
 def source(rel):
  p=ROOT/rel;data=p.read_bytes();hashes[rel]=hashlib.sha256(data).hexdigest();return data.decode('utf-8-sig').replace('\r\n','\n')
 s=source('src/diagnostics/native_hit_probe134.inl');start=s.index('std::uint32_t EntityId(');end=s.index('\nint I(',start)
 (out/'actual_combat_actor_r94.inc').write_text(s[start:end])
 assert s.count('SubmitObservedNativeKillR94(rpg,hitInfo,strength,post,request,applyNative,attackType,details)')==2
 s=source('src/subsystems/gore/lethal_contact.inl');end=s.index('    const bool decap=');(out/'actual_lethal_gore_r94.inc').write_text(s[:end]+'}\n}\n')
 s=source('src/vrik/native_input117.inl');assert s.index('if(dispatched&&prepare)')<s.index('if(comboZonePickedR94)NativeRememberComboZoneR94(')
 assert 'NativePreserveComboZoneR94(owner,c)' in s and 'if(prepare)g_combo_zone_owner_r94={}' in s
 s=source('src/subsystems/gore/native_gore.inl');start=s.index('}else if(c.nonLethalLimb60)');end=s.index('}else if(!c.nativeLethal42',start)
 assert 'StartLimb50' not in s[start:end] and 'return;' in s[start:end]
 assert 'VrComboHoldR94Migrated' not in (ROOT/'src/mod.cpp').read_text()
 assert 'holdingActor' not in (ROOT/'src/subsystems/masterstrike_combos/combo_recovery_hold_r14.h').read_text()
 for rel in ['src/subsystems/gore/native_kill_observer_r94.inl','src/subsystems/masterstrike_combos/combo_zone_owner_r94.inl','src/vrik/combat_r9.h','src/subsystems/gore/host.inl','mods/KCD2VRGore/r93r3/zzkcd2vr_gore.lua']:
  source(rel)
 for fixture in ['native_kill_observer_r94_test','combo_zone_owner_r94_test','lethal_gore_r94_test','combat_r9_test']:
  for mode in (['debug','optimized','asan'] if fixture!='combat_r9_test' else ['optimized']):
   name=fixture+'_'+mode;exe=executable_path(out/name);cmd=command(ROOT/'tests'/f'{fixture}.cpp',exe,[ROOT/'src',ROOT/'tests',out],a.compiler)
   if mode=='debug':cmd=[('/Od' if x=='/O2' else x) for x in cmd]
   if mode=='asan':cmd.insert(1,'/fsanitize=address')
   p=subprocess.run(cmd,capture_output=True,text=True);(out/f'{name}.build.log').write_text(p.stdout+p.stderr);row={'name':name,'command':cmd,'compile_rc':p.returncode,'passed':False}
   if p.returncode==0:
    p=subprocess.run([str(exe)],capture_output=True,text=True);(out/f'{name}.run.log').write_text(p.stdout+p.stderr);row.update(run_rc=p.returncode,summary=(p.stdout+p.stderr).strip(),passed=p.returncode==0)
   rows.append(row);print(name,row['passed'],flush=True)
 env=os.environ.copy();env['PYTHONPATH']=str(a.lua_deps)
 for fixture in ['test_gore_f2_r94','test_gore_d52_lua','test_gore_eligibility_d56','test_gore_d73','test_optional_gore_r93']:
  p=subprocess.run([sys.executable,str(ROOT/'tests'/f'{fixture}.py')],env=env,capture_output=True,text=True);(out/f'{fixture}.log').write_text(p.stdout+p.stderr);rows.append({'name':fixture,'run_rc':p.returncode,'passed':p.returncode==0,'summary':(p.stdout+p.stderr).strip()});print(fixture,p.returncode==0,flush=True)
 dll=Path(r'C:/Steam/steamapps/common/KingdomComeDeliverance2/Bin/Win64MasterMasterSteamPGO/WHGame.dll');data=dll.read_bytes();stamp=struct.unpack_from('<I',data,struct.unpack_from('<I',data,0x3c)[0]+8)[0];digest=hashlib.sha256(data).hexdigest()
 assert stamp==0x6A350E20 and digest=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 result={'passed':all(x['passed'] for x in rows),'results':rows,'production_sha256':hashes,'retail':{'sha256':digest,'timestamp':hex(stamp)},'limits':['Native submit/OS reads are explicit leaves; actual adapter and actor resolver execute.','No game/headset or paired blade authority claim.','No DLL/PAK build or installation by this runner.']}
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n');return 0 if result['passed'] else 1
if __name__=='__main__':raise SystemExit(main())
