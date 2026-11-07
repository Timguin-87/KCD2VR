"""Execute both shipped Lua files in Lua 5.1. Native physics is an explicit spy.
The native admission itself is exercised separately by small_pickup_d81_test.
"""
from pathlib import Path
from lupa.lua51 import LuaRuntime

root = Path(__file__).resolve().parents[1]
lua = LuaRuntime(unpack_returned_tuples=True)
lua.execute((root / 'companion/Mods/kcd2vr_lab/Scripts/Startup/zzkcd2vr_lab.lua').read_text())
lua.execute((root / 'companion/source/Scripts/Entities/Items/PickableItem.lua').read_text())
lua.execute(r'''
checks=0
local function ck(v) checks=checks+1; assert(v,'small pickup Lua check '..checks) end
local milk='51555071-7c55-4da1-9b61-ee3c14fde18b'
local plate='c039a209-0175-4b6a-95dc-c5a6b0b28e6f'
local user,e,item,definition,calls,mass,grams,lastNative
local function setup(class,g)
 calls={physical=0,used=0,admission=0};mass=0;grams=g or 1000;lastNative=nil
 user={id=42};g_localActor=user
 item={}
 function item:CanUse(id) ck(id==42);lastNative='canuse';return true end
 function item:CanSteal(id) ck(id==42);lastNative='cansteal';return false end
 function item:BelongsToDeadBody() lastNative='deadbody';return false end
 function item:GetId() lastNative='itemid';return 7 end
 function item:OnUsed(id) ck(id==42);calls.used=calls.used+1;lastNative='onused';return 'NATIVE_USED' end
 e={id=51,class='PickableItem',npcOnly=false,user=user,item=item,Properties={}}
 function e:GetSlotCount() lastNative='slots';return 1 end
 function e:IsSlotValid(slot) ck(slot==0);lastNative='slotvalid';return true end
 function e:GetMass() lastNative='mass';return mass end
 function e:Physicalize(slot,kind,p)
  ck(lastNative=='admission' and slot==0 and kind==1 and p.density==-1 and p.mass==grams/1000)
  calls.physical=calls.physical+1;mass=p.mass;lastNative='physicalize'
 end
 definition={id=7,class=class or milk}
 PE_RIGID=1
 Framework={WUIDToString=function(id) lastNative='wuid';return '[Item-'..id..']' end}
 ItemManager={GetItem=function(id) ck(id==7);lastNative='definition';return definition end}
 System={GetEntity=function(id) lastNative='entity';if id==42 then return user elseif id==51 then return e end end,
  GetCVar=function(key) ck(key=='kcd2vr_small_pickup_v1:[Item-7]');calls.admission=calls.admission+1;lastNative='admission';return grams end}
end
for _,p in ipairs({{milk,1000},{plate,300}}) do
 setup(p[1],p[2]);ck(PickableItem.Use(e,user)=='NATIVE_USED')
 ck(calls.physical==1 and calls.admission==1 and calls.used==1 and mass==p[2]/1000)
 -- A second normal Use observes the existing rigid mass and never recreates it.
 ck(PickableItem.Use(e,user)=='NATIVE_USED');ck(calls.physical==1 and calls.used==2)
end
local faults={
 function() g_localActor={} end,
 function() e.class='BasicEntity' end,
 function() e.npcOnly=true end,
 function() e.Properties.bMissionCritical=1 end,
 function() e.Properties.bQuestItem=true end,
 function() e.Properties.bLocked=1 end,
 function() e.user={} end,
 function() item.CanUse=function()return false end end,
 function() item.CanSteal=function()return true end end,
 function() item.BelongsToDeadBody=function()return true end end,
 function() System.GetEntity=function()return {}end end,
 function() definition.class='unknown' end,
 function() definition.id=8 end,
 function() e.GetSlotCount=function()return 2 end end,
 function() e.IsSlotValid=function()return false end end,
 function() mass=1 end,
 function() mass=0/0 end,
 function() e.Physicalize=nil end,
 function() Framework.WUIDToString=function()return 'bad' end end,
 function() System.GetCVar=function()return 0 end end,
 function() System.GetCVar=function()return grams+1 end end,
 function() System.GetCVar=function()error('native read unavailable')end end,
 function() System.GetCVar=function()g_localActor={};return grams end end,
 function() System.GetCVar=function()e.id=52;return grams end end,
 function() System.GetCVar=function()e.user={};return grams end end,
 function() System.GetCVar=function()e.Properties.bQuestItem=1;return grams end end,
 function() local old=System.GetEntity;System.GetEntity=function(id)if id==42 then return {}else return old(id)end end end,
}
for _,fault in ipairs(faults) do
 setup();fault();ck(PickableItem.Use(e,user)=='NATIVE_USED')
 ck(calls.physical==0 and calls.used==1) -- original native Use fallback survives refusal/error
end
-- A native call that reports no physicalized mass must not report success.
setup();e.Physicalize=function()calls.physical=calls.physical+1 end
ck(KCD2VR_Lab.TryPreparePickupR24(e,user)==false and calls.physical==1 and calls.used==0)
-- Input absence cannot invent either a preparation or an inventory operation.
setup();ck(PickableItem.Use(e,nil)==false and calls.physical==0 and calls.used==0)
print('D81 actual packaged small pickup Lua: '..checks..' checks PASS; Lua 5.1; native physics mocked')
''')
