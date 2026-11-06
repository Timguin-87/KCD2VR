"""Execute actual supply Lua5.1 with opaque WUIDs and owned-attachment lifecycle."""
from pathlib import Path
import argparse, ast, json, sys, zipfile
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--pak',type=Path);args=ap.parse_args()
raw=(ROOT/'src/vrik/native_supply_t2.lua').read_bytes()
if args.pak:
    with zipfile.ZipFile(args.pak) as z:
        assert z.read('Scripts/KCD2VR/native_supply_t2.lua')==raw
tree=ast.parse(Path(__file__).with_name('test_native_supply_t2.py').read_text())
HARNESS=next(ast.literal_eval(n.value) for n in tree.body if isinstance(n,ast.Assign) and
    any(isinstance(t,ast.Name) and t.id=='HARNESS' for t in n.targets))
def make():
    l=LuaRuntime(unpack_returned_tuples=True);l.execute(HARNESS);l.execute(raw.decode());return l
checks=[]
def ck(ok,label):
    assert ok,label
    checks.append(label)
def fixture():
    l=make()
    l.execute(r'''
Framework={WUIDToString=function(key) return key.token or '[Item-0]' end}
mainKey={token='[Item-72057594037927935]'}
nativeItems[mainKey]={id=mainKey,class='c0760688-f426-5341-95b9-482b87dbcb9b',amount=1}
inventoryValues[100]=mainKey
bindings.weapon_bow={bone='Bip01 Spine2',path='native-bow'}
bindings.weapon_shield={bone='Bip01 L Forearm',path='native-shield'}
function extended(serial,show,token)
  return kcd2vr_supply_native_t2(serial..'|refresh|0|42|1|1|0.2|0.13|0|0.2|-0.13|0|0|0|0|'..
    (token or mainKey.token)..'|'..(show or 0)..'|3|0.2|0|0|0|0|0')
end
''')
    return l,l.globals()
l,g=fixture()
ck(g.extended(1,1).startswith('READY|'),'extended protocol accepted')
b=g.bindings['kcd2vr_belt_t12_main']
ck(b is not None and b.path=='objects/manmade/weapons/axes/workaxe01.cgf','exact opaque inventory WUID maps to actual native axe CGF')
ck(g.itemCreates==0 and len(g.draws)==0,'belt rendering cannot create items or equip weapons')
ck(g.bindings.weapon_bow.path=='native-bow' and g.bindings.weapon_shield.path=='native-shield','native bow and shield bindings survive')
n=g.created;g.extended(2,1)
ck(g.created==n,'idle renewal does not recreate belt model')
g.extended(3,0)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'grab/held display edge removes own belt copy')
g.extended(4,1);g.removeOwned(g.mainKey);g.extended(5,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'real native inventory transfer cannot leave a belt ghost')
ck(all(g.bindings['kcd2vr_supply_t2_2_'+str(j)] is not None for j in range(1,4)) and g.itemCreates==0,'after main axe transfer the remaining real axes replenish the rear pouch')
l,g=fixture()
models=json.loads((ROOT/'review/T12/BELT_MODELS.json').read_text())['models']
mace=next((key,path) for key,path in models.items() if '/maces/' in path)
g.nativeItems[g.mainKey]['class']=mace[0];g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'].path==mace[1],'ordinary native mace resolves its own authored model')
l,g=fixture()
l.execute("local other={token=mainKey.token};nativeItems[other]=nativeItems[mainKey];inventoryValues[101]=other")
g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'ambiguous formatter identity cannot select an arbitrary item')
l,g=fixture();g.extended(1,1)
l.execute("nativeItems[mainKey].class='unsupported-guid'")
g.extended(2,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'unsupported definition fails closed')
l,g=fixture();g.extended(1,1)
l.execute("bindings.kcd2vr_belt_t12_main.path='foreign-replacement'")
g.extended(2,0)
ck(g.bindings['kcd2vr_belt_t12_main'].path=='foreign-replacement','cleanup does not destroy foreign replacement')
l,g=fixture();l.execute("bindings.kcd2vr_belt_t12_main={bone='Pelvis',path='foreign'}")
g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'].path=='foreign','existing foreign attachment name is never replaced')
l,g=fixture();g.extended(1,1);g.timers[len(g.timers)]()
ck(g.bindings['kcd2vr_belt_t12_main'] is None and g.bindings.weapon_bow.path=='native-bow','VM expiry removes only owned display')
l,g=fixture();g.extended(1,1);g.invoke(2,'clear',0)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'mode/menu clear removes belt immediately')
l,g=fixture();l.execute('Framework=nil');g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'missing registered formatter cannot guess WUID')
l,g=fixture();l.execute("Framework.WUIDToString=function() error('unavailable') end");g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'formatter error fails closed')
l,g=fixture();g.extended(1,1)
ck(g.extended(2,1,'72057594037927935')=='BAD|0|0|0','numeric WUID reconstruction prohibited')
l,g=fixture();l.execute('nativeItems[mainKey].amount=2');g.extended(1,1)
ck(g.bindings['kcd2vr_belt_t12_main'] is None,'stacked melee identity is not treated as one equipped weapon')
l,g=fixture();l.execute('inventoryValues={[0]=mainKey}')
g.extended(1,1)
l.execute("function onlyMainDraw() return kcd2vr_supply_native_t2('2|draw|1|42|1|1|0|0|0|0|0|0|0|0|0|'..mainKey.token..'|1|3|0|0|0|0|0|0') end")
ck(g.onlyMainDraw().startswith('EMPTY|') and len(g.draws)==0,'pouch draw cannot steal the only equipped main belt unit')
l,g=fixture()
l.execute('inventoryValues={[0]=mainKey,[1]=inventoryValues[5],[2]=inventoryValues[6],[3]=inventoryValues[7]}')
ck(g.extended(1,1).endswith('|0|3'),'main unit first in enumeration does not hide the three other finite axes')
print(json.dumps({'result':'PASS','lua':'Lua5.1','belt_checks':len(checks),'details':checks,'packaged':bool(args.pak),'headset':'NOT_RUN'},indent=2))
