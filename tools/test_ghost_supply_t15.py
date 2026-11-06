"""Actual Lua51 supply callback and independent bootstrap; engine leaves substituted."""
from pathlib import Path
import sys,zipfile,argparse,os
r=Path(__file__).resolve().parents[1]
a=argparse.ArgumentParser();a.add_argument('--pak',type=Path);a.add_argument('--lua-deps','--deps',type=Path,default=os.environ.get('KCD2VR_LUA_TEST_DEPS'));args=a.parse_args()
if args.lua_deps:sys.path.insert(0,str(args.lua_deps))
try:from lupa.lua51 import LuaRuntime
except ModuleNotFoundError as e:raise SystemExit('Lua51 test needs lupa.lua51; set --lua-deps or KCD2VR_LUA_TEST_DEPS to its existing dependency directory') from e
source=(r/'src/vrik/native_supply_t2.lua').read_text();boot=(r/'src/subsystems/throw_drop/resources/startup_supply_t15.lua').read_text()
if args.pak:
 with zipfile.ZipFile(args.pak) as z:
  assert z.read('Scripts/KCD2VR/native_supply_t2.lua')==(r/'src/vrik/native_supply_t2.lua').read_bytes()
  assert z.read('Scripts/Startup/zzkcd2vr_supply_t15.lua')==(r/'src/subsystems/throw_drop/resources/startup_supply_t15.lua').read_bytes()
h=r"""
items={} values={} draws={} scans=0 forbidden=0 logs={};local id=0
local guids={'43286e47-cde0-50bd-ba4c-9747b777b16d','c0760688-f426-5341-95b9-482b87dbcb9b'}
function add(k,n)for j=1,n do id=id+1;local key={token='[Item-'..id..']'};items[key]={id=key,class=guids[k],amount=1};values[id]=key end end
add(1,5);add(2,3)
ItemManager={GetItem=function(k)return items[k] end}
actor={id=42,inventory={},human={}}
function actor.inventory:GetInventoryTable()scans=scans+1;if invalidScan then return nil end;return values end
function actor.inventory:GetCountOfClass(g)local n=0;for _,k in pairs(values)do if items[k].class==g then n=n+1 end end;return n end
function actor.human:DrawFromInventory(k,side,anim)assert(side==0 and anim and items[k]);draws[#draws+1]=k;if nativeError then error('native return lost') end end
function actor.inventory:CreateItem()forbidden=forbidden+1;error('no clones')end
-- No bone/attachment APIs or timer are provided: invisible pockets must work.
System={GetEntity=function(id)if id==42 then return actor end end,LogAlways=function(m)logs[#logs+1]=m end};g_localActor=actor
Framework={WUIDToString=function(key)return key.token end}
KCD2VRThrowingAssets1={NativeReady=function()error('starter is separate; owned stock must stay usable')end}
function call(serial,action,k,excluded)return kcd2vr_supply_native_t2(serial..'|'..action..'|'..(k or 0)..'|42|5|6|0|0|0|0|0|0|0|0|0|'..(excluded or '-')..'|0|0|0|0|0|0|0|0')end
function remove(k)for j,v in pairs(values)do if v==k then values[j]=nil;return true end end end
"""
l=LuaRuntime(unpack_returned_tuples=True);l.execute(h);l.execute(source);g=l.globals()
assert g.call(1,'refresh',0)=='READY|1|5|3';n=g.scans
for j in range(2,12):assert g.call(j,'refresh',0).startswith('READY|')
assert g.scans==n
assert g.call(12,'draw',0)=='REQUESTED|12|4|3';first=g.draws[1]
assert g.call(12,'draw',0).startswith('RESERVED|') and len(g.draws)==1
assert g.call(13,'draw',1).startswith('RESERVED|') and len(g.draws)==1
assert g.remove(first);assert g.call(14,'release',0)=='READY|14|4|3'
serial=15
for kind,n in [(0,4),(1,3)]:
 for _ in range(n):
  assert g.call(serial,'draw',kind).startswith('REQUESTED|');serial+=1
  assert g.remove(g.draws[len(g.draws)])
  assert g.call(serial,'release',kind).startswith(('READY|','EMPTY_STOCK|'));serial+=1
assert len(g.draws)==8 and g.call(serial,'draw',1).startswith('EMPTY|') and g.forbidden==0
# Independent bootstrap succeeds with no Map script and repeated load is idempotent.
l2=LuaRuntime(unpack_returned_tuples=True);l2.execute(h);l2.globals().moduleSource=source
l2.execute("Script={ReloadScript=function(name)assert(name=='Scripts/KCD2VR/native_supply_t2.lua');assert(loadstring(moduleSource))() end}")
l2.execute(boot);assert l2.eval("type(kcd2vr_supply_native_t2)=='function' and KCD2VRGhostSupplyT15==15")
l2.globals().call(1,'draw',1);l2.execute(boot);assert l2.globals().call(2,'draw',0).startswith('RESERVED|')
# Native call ambiguity remains spent; another serial cannot duplicate it.
l3=LuaRuntime(unpack_returned_tuples=True);l3.execute(h);l3.execute(source);l3.execute('nativeError=true')
try:l3.globals().call(1,'draw',0)
except Exception:pass
assert len(l3.globals().draws)==1 and l3.globals().call(2,'draw',0).startswith('RESERVED|')
# Exclude actual occupied left WUID without touching any loadout slots.
l4=LuaRuntime(unpack_returned_tuples=True);l4.execute(h);l4.execute(source);g4=l4.globals();left=l4.eval('(function()for _,k in pairs(values)do if items[k].class:find("c076") then return k end end end)()')
assert g4.call(1,'draw',1,left.token).startswith('REQUESTED|');assert g4.draws[1].token!=left.token
# Same-count identity replacement recovers on the existing bounded refresh.
l5=LuaRuntime(unpack_returned_tuples=True);l5.execute(h);l5.execute(source);g5=l5.globals()
assert g5.call(1,'refresh',0)=='READY|1|5|3' and g5.scans==1
l5.execute('items={} values={} add(1,5);add(2,3)')
assert g5.call(2,'refresh',0)=='READY|2|5|3' and g5.scans==2
for j in range(3,8):assert g5.call(j,'refresh',0)=='READY|'+str(j)+'|5|3'
assert g5.scans==2
# A failed initial census must retry even while class counts remain unchanged.
l6=LuaRuntime(unpack_returned_tuples=True);l6.execute(h);l6.execute(source);g6=l6.globals();l6.execute('invalidScan=true')
assert g6.call(1,'refresh',0)=='INVENTORY_CAP|1|0|0' and g6.scans==1
l6.execute('invalidScan=false')
assert g6.call(2,'refresh',0)=='READY|2|5|3' and g6.scans==2
assert g6.call(3,'refresh',0)=='READY|3|5|3' and g6.scans==2
# Invalidated OTHER keys rescan without dropping an ambiguous native reservation.
assert g6.call(4,'draw',0)=='REQUESTED|4|4|3'
l6.execute('for key in pairs(items) do if items[key].class:find("c076") then items[key]=nil end end;for n,key in pairs(values) do if not items[key] then values[n]=nil end end;add(2,3)')
assert g6.call(5,'refresh',0)=='READY|5|4|3'
assert g6.call(6,'draw',1).startswith('RESERVED|') and len(g6.draws)==1
assert g6.call(7,'release',0)=='READY|7|5|3'
new_left=l6.eval('(function()for _,key in pairs(values)do if items[key].class:find("c076") then return key end end end)()')
assert g6.call(8,'draw',1,new_left.token).startswith('REQUESTED|') and g6.draws[2].token!=new_left.token
print('T15 actual Lua51:8 finite draws, no attachment/timer/creation APIs, no map dependency, idempotent boot, ambiguous native call spent, left-hand exclusion, same-count WUID replacement and failed census recovery PASS')
