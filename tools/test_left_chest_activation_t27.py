"""T27 actual Lua/config/delivery regression checks; no retail execution."""
from pathlib import Path
import argparse,hashlib,json,sys,zipfile,subprocess
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from test_throw_reservation_t20 import fresh, EXTENSION
from test_ammo_supply_t18 import FIXTURE
from lupa.lua51 import LuaRuntime

def main():
 ap=argparse.ArgumentParser();ap.add_argument('--out',required=True,type=Path);ap.add_argument('--parent',required=True,type=Path);a=ap.parse_args()
 checks=[]
 def ck(ok,name):
  assert ok,name
  checks.append(name)
 def status(x):return x.split('|')[1]
 source=(ROOT/'src/subsystems/throw_drop/resources/native_throw_reservation_t20.lua').read_text()
 def opaque(extra=''):
  l=LuaRuntime(unpack_returned_tuples=True);l.execute(FIXTURE);l.execute(EXTENSION)
  l.execute('g_localActor.id={nativeHandle=7};'+extra);l.execute(source);return l,l.globals().F
 l,f=opaque();key=f.seed('old',0,5)
 ck(f.call20(1,'refresh',1,0,None).split('|')[1:] == ['READY','-','5','0'],'opaque EntityId refresh resolves exact actor through registry')
 r=f.call20(2,'prepare',1,0,None)
 ck(status(r)=='RESERVED' and r.split('|')[2]==f'[Item-{key}]','opaque EntityId LEFT prepare selects existing finite unit')
 ck(f.call20(2,'prepare',1,0,None)==r,'same opaque LEFT request idempotent')
 ck(f.creates==0 and f.removed==0 and f['items'][key].amount==5,'reservation cannot add, debit, equip or replace native inventory')
 ck(status(f.call20(2,'clear',1,0,key))=='CLEARED','exact original clear supports opaque handle')
 for leaf in ('GetCountOfClass','GetInventoryTable'):
  l,f=opaque();f.seed('old',0,5)
  l.execute('local old=g_localActor.inventory.'+leaf+';g_localActor.inventory.'+leaf+'=function(self,...) local r=old(self,...);g_localActor.id={nativeHandle=7};return r end')
  ck(status(f.call20(1,'prepare',1,0,None))=='WAIT','same actor changed opaque handle revoked during '+leaf)
 l,f=opaque();f.seed('old',0,5);l.execute('System.GetEntity=function() return {id=7} end')
 ck(status(f.call20(1,'prepare',1,0,None))=='WAIT','numeric id resolving different entity refused')
 l,f=opaque();f.seed('old',0,5)
 l.execute('local old=ItemManager.GetItem;ItemManager.GetItem=function(k) local r=old(k);g_localActor={id={nativeHandle=7},inventory=g_localActor.inventory};return r end')
 ck(status(f.call20(1,'prepare',1,0,None))=='WAIT','actor replacement during final item query refused')
 # Causal old source under IDENTICAL opaque fixture must refuse before counts.
 old=source.replace('not actor.id or System.GetEntity(id)~=actor','actor.id~=id or System.GetEntity(id)~=actor').replace('actor.id==actorHandle and System.GetEntity(id)==actor','actor.id==id and System.GetEntity(id)==actor')
 l=LuaRuntime(unpack_returned_tuples=True);l.execute(FIXTURE);l.execute(EXTENSION);l.execute('g_localActor.id={nativeHandle=7}');l.execute(old);f=l.globals().F;f.seed('old',0,5)
 ck(status(f.call20(1,'prepare',1,0,None))=='WAIT' and f.counts==0,'causal prior raw-handle comparison reproduces blocked draw')
 # Pair current PAK in memory, unchanged unrelated payloads, idempotence.
 import importlib.util
 path=ROOT/'src/subsystems/throw_drop/resources/companion_patch_t27.py';spec=importlib.util.spec_from_file_location('t27_companion',path);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 with zipfile.ZipFile(a.parent) as z:members={i.filename:(i,z.read(i)) for i in z.infolist()}
 result,changed=m.plan_members(members)
 ck(changed==set(m.ROWS),'exactly two current companion Lua payloads updated')
 ck(all(result[n][1]==v[1] for n,v in members.items() if n not in changed),'all map,shops,door,combat,localization and other PAK bytes preserved')
 ck(not m.plan_members(result)[1],'companion overlay idempotent')
 bad=dict(members);info,data=bad[next(iter(m.ROWS))];bad[next(iter(m.ROWS))]=(info,data+b'--foreign')
 refused=False
 try:m.plan_members(bad)
 except ValueError:refused=True
 ck(refused,'unknown companion revision refused instead of overwritten')
 report={'passed':True,'checks':len(checks),'details':checks,'actual_lua':'Lua5.1','parent_sha256':hashlib.sha256(a.parent.read_bytes()).hexdigest(),'native_execution':False,'headset':False}
 a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
