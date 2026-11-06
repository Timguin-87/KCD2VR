"""Execute the shipped supply Lua in Lua5.1, with documented native leaves.
The opaque WUIDs are table identities, deliberately not convertible to numbers.
No game process, installed PAK, save or inventory is touched.
"""
from pathlib import Path
import argparse,json,sys,zipfile
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--pak',type=Path,help='unified companion containing BOTH actual supply and starter modules');ap.add_argument('--asset-pak',type=Path);args=ap.parse_args()
source=(ROOT/'src/vrik/native_supply_t2.lua').read_text()
if args.pak:
 with zipfile.ZipFile(args.pak) as z:
  actual=z.read('Scripts/KCD2VR/native_supply_t2.lua').decode()
  assert actual.replace('\r\n','\n')==source.replace('\r\n','\n'),'shipped supply differs from source'
  source=actual
HARNESS=r'''
nativeItems={} inventoryValues={} bindings={} timers={} draws={} created=0 destroyed=0 lookups=0 inventoryScans=0 itemCreates=0 nextInventoryIndex=0
local gd='43286e47-cde0-50bd-ba4c-9747b777b16d' local ga='c0760688-f426-5341-95b9-482b87dbcb9b'
function add(kind,n,amount)
 for j=1,n do local key={opaque=kind..j} nativeItems[key]={id=key,class=kind==0 and gd or ga,amount=amount or 1} inventoryValues[nextInventoryIndex]=key nextInventoryIndex=nextInventoryIndex+1 end
end
add(0,5) add(1,3)
ItemManager={GetItem=function(key) lookups=lookups+1 return nativeItems[key] end}
Script={SetTimer=function(ms,f) timers[#timers+1]=f end}
actor={id=42,inventory={},human={}}
function actor.inventory:GetInventoryTable() inventoryScans=inventoryScans+1 return inventoryValues end
function actor.inventory:GetCountOfClass(guid)
 local n=0 for _,key in pairs(inventoryValues) do local q=nativeItems[key] if q and q.class==guid then n=n+q.amount end end return n
end
function actor.inventory:CreateItem(...) itemCreates=itemCreates+1 error('forbidden') end
function actor.human:DrawFromInventory(key,hand,animate) assert(nativeItems[key]) assert(hand==0) draws[#draws+1]=key end
function actor:GetAttachmentBone(slot,name) return bindings[name] and bindings[name].bone or nil end
function actor:GetAttachmentCGF(slot,name) return bindings[name] and bindings[name].path or nil end
function actor:CreateBoneAttachment(slot,bone,name) assert(bone=='Hips','retail male skeleton has Hips, not Pelvis') assert(not bindings[name]) created=created+1 bindings[name]={bone=bone} end
function actor:SetAttachmentCGF(slot,name,path) assert(bindings[name]) if not failModel then bindings[name].path=path end end
function actor:SetAttachmentPos(slot,name,p) bindings[name].pos=p end
function actor:SetAttachmentAngles(slot,name,p) bindings[name].angles=p end
function actor:DestroyAttachment(slot,name) destroyed=destroyed+1 bindings[name]=nil end
System={GetEntity=function(id) if id==42 then return actor end return nil end}
g_localActor=actor
KCD2VRThrowingAssets1={NativeReady=function() return 'DONE' end}
function liveCount() local n=0 for _ in pairs(bindings) do n=n+1 end return n end
function removeOwned(key) for j,k in pairs(inventoryValues) do if k==key then inventoryValues[j]=nil return true end end return false end
function invoke(serial,action,kind,body,request)
 return kcd2vr_supply_native_t2(serial..'|'..action..'|'..(kind or 0)..'|42|'..(body or 1)..'|'..(request or 1)..'|0.2|0.13|0.0|0.2|-0.13|0.0|0|0|0')
end
'''
def make():
 l=LuaRuntime(unpack_returned_tuples=True);l.execute(HARNESS);l.execute(source);return l
checks=[]
def ck(ok,label):
 assert ok,label;checks.append(label)
l=make();g=l.globals()
ck(g.invoke(1,'refresh',0)=='READY|1|5|3','initial real5/3 count')
ck(g.liveCount()==8 and g.created==8 and len(g.draws)==0,'visible own CGFs only, no native draw')
scans=g.inventoryScans
for n in range(2,6):g.invoke(n,'refresh',0)
ck(g.inventoryScans==scans and g.created==8,'idle refresh does not enumerate inventory or recreate models')
ck(g.invoke(6,'draw',0)=='REQUESTED|6|4|3' and len(g.draws)==1,'one explicit right native draw and reserved count')
first=g.draws[1]
ck(g.invoke(6,'draw',0).startswith('RESERVED|6|') and len(g.draws)==1,'duplicate serial never repeats draw')
ck(g.invoke(7,'draw',1).startswith('RESERVED|7|') and len(g.draws)==1,'only one unit reservation at a time')
ck(g.removeOwned(first),'simulated real native transfer removed actual owned WUID')
ck(g.invoke(8,'release',0)=='READY|8|4|3','commit selects/counts remaining real stock')
serial=9
for kind,count in ((0,4),(1,3)):
 for _ in range(count):
  result=g.invoke(serial,'draw',kind);serial+=1
  ck(result.startswith('REQUESTED|'),'following draw selects actual next unit')
  key=g.draws[len(g.draws)]
  ck(key!=first and g.removeOwned(key),'following WUID is independently owned and consumed once')
  g.invoke(serial,'release',kind);serial+=1
ck(g.liveCount()==0,'all supply models disappear when actual units exhausted')
ck(g.invoke(serial,'draw',0).startswith('EMPTY|') and len(g.draws)==8,'zero stock cannot refill, clone or throw infinitely')
ck(g.itemCreates==0,'runtime supply never creates items')
ck(len(g.inventoryValues)==0,'no fake or double inventory consumption')
l=make();g=l.globals();l.execute('KCD2VRThrowingAssets1=nil')
ck(g.invoke(1,'refresh',0)=='MISSING_ASSETS|1|5|3' and g.liveCount()==8,'absent starter is explicit while already-owned units remain visible')
ck(g.invoke(2,'draw',0)=='REQUESTED|2|4|3' and len(g.draws)==1 and g.itemCreates==0,'missing starter never blocks an actual owned draw or creates stock')
l=make();g=l.globals();l.execute('KCD2VRThrowingAssets1=nil;inventoryValues={}')
ck(g.invoke(1,'refresh',0)=='MISSING_ASSETS|1|0|0' and g.liveCount()==0 and g.itemCreates==0,'missing assets with empty stock is honest and cannot invent inventory')
l=make();g=l.globals();l.execute('inventoryValues={}')
ck(g.invoke(1,'refresh',0)=='EMPTY_STOCK|1|0|0' and g.itemCreates==0,'valid starter with depleted stock is distinct from missing assets')
for status,expected in [('WAIT','STARTER_WAIT'),('WAIT_LIMIT','STARTER_WAIT_LIMIT'),('UNCERTAIN','STARTER_UNCERTAIN'),('unexpected','STARTER_UNKNOWN')]:
 l=make();g=l.globals();l.execute("KCD2VRThrowingAssets1.NativeReady=function() return '"+status+"' end")
 ck(g.invoke(1,'refresh',0)==expected+'|1|5|3' and g.liveCount()==8 and g.itemCreates==0,'starter '+status+' readiness is explicit without changing owned inventory')
l=make();g=l.globals();l.execute("inventoryValues={nativeItems[inventoryValues[1]]}")
ck(g.invoke(1,'draw',0).startswith('EMPTY|') and len(g.draws)==0,'malformed enumerator record-values shape cannot draw')
l=make();g=l.globals();l.execute("for _,key in pairs(inventoryValues) do nativeItems[key].amount=5 end")
ck(g.invoke(1,'draw',0)=='EMPTY|1|0|0' and len(g.draws)==0,'stacked melee units rejected rather than dropped as one')
l=make();g=l.globals();l.execute("bindings.kcd2vr_supply_t2_1_1={bone='Pelvis',path='foreign.cgf'}")
g.invoke(1,'refresh',0);g.invoke(2,'clear',0)
ck(g.liveCount()==1 and g.bindings['kcd2vr_supply_t2_1_1']['path']=='foreign.cgf','foreign binding survives create/cleanup')
l=make();g=l.globals();l.execute('failModel=true');g.invoke(1,'refresh',0);g.invoke(2,'clear',0)
ck(g.liveCount()==0,'own empty creation cleaned if CGF binding fails')
l=make();g=l.globals();g.invoke(1,'refresh',0);l.execute('for _,f in ipairs(timers) do f() end')
ck(g.liveCount()==0,'renderer expires after native/VM renewal stops')
l=make();g=l.globals();g.invoke(1,'refresh',0)
ck(g.kcd2vr_supply_native_t2('2|clear|0|0|0|0|0|0|0|0|0|0|0|0|0')=='CLEARED|2|0|0' and g.liveCount()==0,'pause/stale actor clear works immediately without new native identity')
l=make();g=l.globals();l.execute("local real=inventoryValues inventoryValues={} for j=1,1000 do local key={other=j} nativeItems[key]={id=key,class='other-guid',amount=1} inventoryValues[#inventoryValues+1]=key end for _,key in pairs(real) do inventoryValues[#inventoryValues+1]=key end")
ck(g.invoke(1,'refresh',0)=='READY|1|5|3' and g.liveCount()==8,'large inventory above512 still finds actual loadout')
l=make();g=l.globals();l.execute("local real=inventoryValues inventoryValues={} for j=1,4100 do local key={other=j} nativeItems[key]={id=key,class='other-guid',amount=1} inventoryValues[#inventoryValues+1]=key end for _,key in pairs(real) do inventoryValues[#inventoryValues+1]=key end")
ck(g.invoke(1,'draw',0).startswith('INVENTORY_CAP|') and len(g.draws)==0,'bounded4096 inventory refusal never draws unknown unit')
l=make();g=l.globals();g.invoke(1,'refresh',0);l.execute('g_localActor={id=43}')
ck(g.invoke(2,'draw',0).startswith('UNAVAILABLE|') and len(g.draws)==0,'foreign/noncurrent actor cannot draw')
ck(g.kcd2vr_supply_native_t2('bad')=='BAD|0|0|0','malformed packet rejected')
# Execute BOTH actual shipped Lua modules with every startup timer/event lost.
# Native leaves create independent opaque quantity1 units; no fabricated equip.
assetPak=ROOT/'companion/optional_throwing_assets1/Mods/KCD2VRThrowingAssets/Data/KCD2VR_ThrowingAssets1.pak'
starterPak=args.asset_pak or args.pak or assetPak
with zipfile.ZipFile(starterPak) as z:starter=z.read('Scripts/Startup/zzkcd2vr_throwing_assets1.lua').decode()
def withStarter():
 l=make()
 l.execute(r'''
 inventoryValues={} nativeItems={} nextInventoryIndex=0 starterCreates=0 starterPerks={}
 actor.soul={}
 function actor.soul:HasPerk(key,include) return starterPerks[key]==true end
 function actor.soul:AddPerk(key) starterPerks[key]=true end
 function actor.inventory:CreateItem(guid,health,amount)
  assert(health==1 and amount==1) starterCreates=starterCreates+1
  add(guid=='43286e47-cde0-50bd-ba4c-9747b777b16d' and 0 or 1,1,1) return true
 end
 ''')
 l.execute(starter);l.execute('timers={};UIAction=nil');return l
l=withStarter();g=l.globals()
ck(g.invoke(1,'refresh',0)=='READY|1|5|3' and g.starterCreates==8,'actual optional starter recovers inside qualified supply with every timer/event lost')
ck(g.invoke(2,'refresh',0,1,2)=='READY|2|5|3' and g.starterCreates==8,'native readiness runs once per body without repeated item grants')
l.execute('inventoryValues={}')
ck(g.invoke(3,'release',0,1,2)=='EMPTY_STOCK|3|0|0' and g.starterCreates==8 and g.liveCount()==0,'consumed starter stock remains empty and holster disappears')
l.execute('KCD2VRThrowingAssets1.WorldEnded();starterPerks={}')
ck(g.invoke(4,'refresh',0,1,3)=='STARTER_ENDED_BODY|4|0|0' and g.starterCreates==8,'same ended native body reports explicit refusal and cannot grant through supply')
ck(g.invoke(5,'refresh',0,2,4)=='READY|5|5|3' and g.starterCreates==16,'new native body recovers starter without a gameplay-start event')
l=withStarter();g=l.globals();l.execute('g_localActor={id=43}')
ck(g.invoke(1,'refresh',0).startswith('UNAVAILABLE|') and g.starterCreates==0,'foreign actor cannot reach optional starter through supply')
l=withStarter();g=l.globals()
ck(g.invoke(1,'unknown',0).startswith('BAD|') and g.starterCreates==0,'unknown supply action cannot trigger starter side effects')
report={'checks':len(checks),'lua':l.eval('_VERSION'),'native_leaves':'documented schema substitutions','source':str(ROOT/'src/vrik/native_supply_t2.lua'),'result':'PASS','headset':'NOT_RUN','details':checks}
report['packaged_supply']=str(args.pak) if args.pak else 'SOURCE_ONLY';report['packaged_optional_starter']=str(starterPak)
out=ROOT/'review/THROW2_SUPPLY';out.mkdir(parents=True,exist_ok=True);(out/'LUA51_TEST.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
