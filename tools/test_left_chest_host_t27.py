"""Actual delivery/config and enabled LEFT-world service host checks; no DLL/PAK."""
from pathlib import Path
import argparse,subprocess,json,hashlib,zipfile,re
from native_research112 import PE
ROOT=Path(__file__).resolve().parents[1]
VC='C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat'
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',required=True,type=Path);p.add_argument('--baseline-zip',required=True,type=Path);p.add_argument('--dll',required=True,type=Path);a=p.parse_args();out=a.out.resolve();assert ROOT not in out.parents and ROOT!=out;out.mkdir(parents=True,exist_ok=True)
 assert hashlib.sha256(a.baseline_zip.read_bytes()).hexdigest()=='a4082516f2821da01eea08a2d81eee0066d95e8f01beca0aa65773bf69a246f4'
 rows={}
 def run(name,file):
  cmd=out/(name+'.cmd');cmd.write_text('@echo off\ncall "'+VC+'" >nul\ncd /d "'+str(out)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address /I "'+str(ROOT/'src')+'" /I "'+str(out)+'" "'+str(file)+'" /Fe:'+name+'.exe\nif errorlevel 1 exit /b %errorlevel%\n'+name+'.exe\nexit /b %errorlevel%\n')
  r=subprocess.run(['cmd','/d','/c',str(cmd)],capture_output=True,text=True);log=r.stdout+r.stderr;(out/(name+'.log')).write_text(log);rows[name]={'exit':r.returncode,'log':str(out/(name+'.log'))};print(name,r.returncode,log[-600:]);return r.returncode,log
 assert run('config',ROOT/'tests/subsystems/throw_drop/left_chest_config_t27_test.cpp')[0]==0
 assert run('inventory',ROOT/'tests/inventory_delivery_t7_test.cpp')[0]==0
 # Production original gate must fail with IDENTICAL Mode0/F4OFF fixture.
 with zipfile.ZipFile(a.baseline_zip) as z:
  old=z.read('KCD2VR_SOURCE/src/subsystems/throw_drop/runtime/inventory_delivery_t7.inl')
 prior=out/'prior_delivery.inl';prior.write_bytes(old)
 test=(ROOT/'tests/inventory_delivery_t7_test.cpp').read_text()
 for oldinc,newinc in [('../src/subsystems/throw_drop/inventory_lifecycle_t8.h',(ROOT/'src/subsystems/throw_drop/inventory_lifecycle_t8.h').as_posix()),('../src/vrik/weapon_release_t1.h',(ROOT/'src/vrik/weapon_release_t1.h').as_posix()),('../src/subsystems/throw_drop/runtime/inventory_delivery_t7.inl',prior.as_posix())]:test=test.replace(oldinc,newinc)
 f=out/'causal_inventory.cpp';f.write_text(test);code,log=run('causal_inventory',f);assert code!=0 and 'Wanted()' in log
 # Exact enabled H41 header and actual service. Legacy attachments/axes deliberately not admitted by H41.
 fixture=(ROOT/'tests/subsystems/throw_drop/native_ammo_service_t22_test.cpp').read_text()
 start=fixture.index('    for(int side=0;side<2;++side)for(int family=0;family<2;++family)for(int count:{1,3,5}){',fixture.index('int main()'))
 end=fixture.index('    // T26: actual service, separate world-held LEFT chest knife; RIGHT unchanged.',start)
 (out/'world.cpp').write_text(fixture[:start]+fixture[end:])
 native=PE(a.dll);assert hashlib.sha256(native.data).hexdigest()=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
 runtime=(ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl').read_text();pins=[]
 for name,rva in [('slot',0x7F1E3C),('filename',0x1A81E20),('helper',0x51EF4C),('selected',0x2AAD1AD),('worldDisableT26',0xABDFF8),('proxyEnableT26',0xABE028),('getProxyT26',0x5D70C4)]:
  match=re.search(r'char '+name+r'\[\]\{([^}]+)\}',runtime);value=bytes(int(x.strip(),0) for x in match[1].split(','));assert native.read(rva,len(value))==value;pins.append((rva,value))
 (out/'pins_t22.inc').write_text('\n'.join('{0x%X,{%s}},'%(r,','.join('0x%02x'%x for x in v)) for r,v in pins))
 code,log=run('world',out/'world.cpp');assert code==0
 rows['world']['fixture']=json.loads(next(x for x in log.splitlines() if x.startswith('{"checks":')))
 rows['inventory']['feature_flags']='compile-time leaves match production integrated=true,leftChestKnivesT26=true; actual native reads/VM leaves substituted'
 rows['causal_inventory']['expected']='RED: exact frozen H41 F4-dependent Wanted gate'
 result={'passed':True,'runs':rows,'native_execution':False,'headset':False,'DLL_or_PAK_built':False,'source_sha256':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [ROOT/'src/subsystems/throw_drop/config.inl',ROOT/'src/subsystems/throw_drop/runtime/inventory_delivery_t7.inl',ROOT/'src/subsystems/throw_drop/runtime/native_ammo_service_t22.inl']}}
 (out/'RESULT.json').write_text(json.dumps(result,indent=2)+'\n')
if __name__=='__main__':main()
