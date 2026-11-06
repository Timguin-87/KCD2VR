# Production native Map request Lua51; retail UI leaves are substituted.
from pathlib import Path
import sys,json,zipfile,xml.etree.ElementTree as ET
sys.path.insert(0,'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
ROOT=Path(__file__).resolve().parents[1]
source=(ROOT/'mods/KCD2VRMap/map_native_read_h25.lua').read_text()
if len(sys.argv)>1:
 with zipfile.ZipFile(sys.argv[1]) as z:
  native_bytes=z.read('Scripts/KCD2VR/map_native_read_h25.lua')
  assert native_bytes==(ROOT/'mods/KCD2VRMap/map_native_read_h25.lua').read_bytes()
  source=native_bytes.decode()
  row=ET.fromstring(z.read('Libs/Tables/item/item__kcdvr_map.xml')).find('ItemClasses/Document')
  assert row.get('Model')=='characters/assets/parchment_folded/parchment_folded.cdf' and row.get('EntityScript')=='Book'
  images=row.findall('DocumentContent/Image');assert [x.get('Page') for x in images]==['0','1']
  for image in images:assert 'Libs/UI/Textures/Books/Maps/'+image.get('Image')+'_ui.dds' in z.namelist()

lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute(r'''
ITEM='5f3c2a71-8e4d-4b6a-9c1e-2d7f0b9a4e63';token='[Item-1234]';count=1;creates=0;uses=0;calls=0
focusEnabled=true;wrongFocus=false;wrongClient=false;failUse=false;wrongClass=false;badToken=false;missingMetadata=false
KCD2VRMap={};listener=nil;logs={}
g_localActor={id=42,inventory={}}
function g_localActor.inventory:GetCountOfClass(id)assert(id==ITEM);return count end
function g_localActor.inventory:CreateItem(id,health,n)assert(id==ITEM and health==1 and n==1);creates=creates+1;count=count+1 end
function g_localActor.inventory:FindItem(id)assert(id==ITEM);return count>0 and token or nil end
System={GetEntity=function(id)return id==g_localActor.id and g_localActor or nil end,LogAlways=function(s)logs[#logs+1]=s end}
ItemManager={GetItem=function(id)assert(id==token);if missingMetadata then return nil end;return {id=badToken and 'userdata:0x1' or token,class=wrongClass and 'other' or ITEM} end}
UIAction={RegisterElementListener=function(t,element,instance,event,callback)
 assert(element=='ApseInventoryList' and instance==0 and event=='OnFocusChanged');listener=t[callback];return true end,
 CallFunction=function(element,instance,func,arg)
 assert(element=='ApseInventoryList' and instance==0);calls=calls+1
 if func=='FocusNext' then return true end
 if func=='FocusItem' then assert(arg==token)
  if focusEnabled then listener(element,instance,'OnFocusChanged',{wrongFocus and '[Item-9999]' or token,false,false,wrongClient and 1 or 0}) end
  return true
 end
 assert(func=='LeftDoubleClick' and arg==0);uses=uses+1;if failUse then return false end;return true
end}
''')
lua.execute(source)
lua.execute(r'''
local function run(serial,status)local r=kcd2vr_map_native_read_h25('N5|'..serial);assert(r=='N5|'..serial..'|'..status,r) end
run(1,'NATIVE_READ_REQUESTED');assert(uses==1 and creates==0)
run(1,'SPENT_NO_RETRY');assert(uses==1)
wrongFocus=true;run(2,'WAIT_FOCUS');assert(uses==1);wrongFocus=false
listener('ApseInventoryList',0,'OnFocusChanged',{token,false,false,0});assert(uses==1)
wrongClient=true;listener('ApseInventoryList',0,'OnFocusChanged',{token,false,false,1});run(2,'WAIT_FOCUS');assert(uses==1);wrongClient=false
run(2,'NATIVE_READ_REQUESTED');assert(uses==2)
focusEnabled=false;run(3,'WAIT_FOCUS');run(3,'WAIT_FOCUS');run(3,'WAIT_FOCUS');run(3,'CONTEXT_CHANGED');assert(uses==2);focusEnabled=true
wrongClass=true;run(4,'ITEM_IDENTITY_REFUSED');wrongClass=false;assert(uses==2)
badToken=true;run(5,'ITEM_TOKEN_REFUSED');badToken=false;assert(uses==2)
missingMetadata=true;run(6,'ITEM_IDENTITY_REFUSED');missingMetadata=false;assert(uses==2)
failUse=true;run(7,'NATIVE_READ_REFUSED');run(7,'SPENT_NO_RETRY');assert(uses==3);failUse=false
count=0;run(8,'NATIVE_READ_REQUESTED');assert(creates==1 and count==1 and uses==4)
run(7,'STALE');assert(uses==4)
focusEnabled=false;run(9,'WAIT_FOCUS');g_localActor={id=43,inventory=g_localActor.inventory};run(9,'CONTEXT_CHANGED');assert(uses==4)
focusEnabled=false;run(10,'WAIT_FOCUS');KCD2VRMap.WorldEnded();run(10,'SPENT_NO_RETRY');assert(uses==4)
UIAction.CallFunction=function(element,instance,func,arg)
 if func=='FocusNext' then return end
 if func=='FocusItem' then listener(KCD2VRMap,element,instance,'OnFocusChanged',{token,false,false,0});return end
 uses=uses+1;error('native reply lost after dispatch')
end
run(11,'FAULT_NO_RETRY');run(11,'SPENT_NO_RETRY');assert(uses==5)
UIAction.CallFunction=function(element,instance,func,arg)
 if func=='LeftDoubleClick' then uses=uses+1 end
 return true
end
run(12,'WAIT_FOCUS');listener('ApseInventoryList',0,'OnFocusChanged',{token,false,false,0});assert(uses==5)
run(12,'NATIVE_READ_REQUESTED');run(12,'SPENT_NO_RETRY');assert(uses==6)
run(13,'WAIT_FOCUS');listener('ApseInventoryList',0,'OnFocusChanged',{token,false,false,0})
listener('ApseInventoryList',0,'OnFocusChanged',{'[Item-9999]',false,false,0});run(13,'WAIT_FOCUS');assert(uses==6)
listener('ApseInventoryList',0,'OnFocusChanged',{token,false,false,0});run(13,'NATIVE_READ_REQUESTED');assert(uses==7)

''')
with zipfile.ZipFile('C:/Steam/steamapps/common/KingdomComeDeliverance2/Data/IPL_GameData.pak') as z:
 t=ET.fromstring(z.read('Libs/UI/UIElements/ApseInventoryList.xml'))
 funcs={e.get('name') for e in t.findall('.//functions/function')};assert {'FocusItem','FocusNext','LeftDoubleClick'}<=funcs
 e=t.find('.//events/event[@name="OnFocusChanged"]');assert [p.get('name') for p in e]==['Ids','IsExpandable','IsExpand','ClientId']
print(json.dumps({'result':'PASS','runtime':'Lua 5.1','production_lua':True,'native_contract_xml':True,'exact_focus_and_one_shot':True,'native_animation':'NOT_RUN'},indent=2))
