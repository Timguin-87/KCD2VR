"""Execute the delivered Lua 5.1 script with explicit native-link leaves.
No claim of game AI/animation/headset validation.
"""
from pathlib import Path
from lupa.lua51 import LuaRuntime
root=Path(__file__).resolve().parents[1]
lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute('''
t=10;adds=0;removes=0;scans=0;timers=0;failRemove=false;faultAdd=false
local function entity(id)
 return {id=id,this={id=id+100},human=true,
 soul={IsInCombatDanger=function()return false end},
 actor={IsDead=function()return false end,IsUnconscious=function()return false end},
 GetBonePos=function() return {x=0,y=2,z=0} end}
end
player=entity(1);npc=entity(2);otherNpc=entity(3);registry={[1]=player,[2]=npc,[3]=otherNpc};graph={}
Framework={WUIDToMsg=function(id)return tostring(id)end}
System={GetCurrTime=function()return t end,GetEntity=function(id)return registry[id]end,
 GetEntitiesInSphere=function()scans=scans+1;return {npc}end,LogAlways=function()end}
Script={SetTimer=function()timers=timers+1 end}
XGenAIModule={FindLinks=function(from,tag)return graph[tostring(from)..tag] or {} end,
 AddLink=function(from,to,tag) adds=adds+1;graph[tostring(from)..tag]={to};if faultAdd then error('after apply')end;return true end,
 RemoveLink=function(from,to,tag)removes=removes+1;if failRemove then return false end
 assert(graph[tostring(from)..tag][1]==to);graph[tostring(from)..tag]=nil;return true end}
function packet(g,d,s,mode,scene,body,members)
 return kcd2vr_life_d73('D76|1|'..(mode or 5)..'|'..g..'|'..d..'|'..s..'|0|0|0|0|1|0|'..(scene or (d==1 and '500' or '0'))..'|'..(body or 0)..'|'..(members or '102'))
end
function step(g,d,s) t=t+.25;packet(g or 0,d or 1,s or 0);KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick()end
function n()local c=0;for _ in pairs(KCD2VR_LifeD73.active)do c=c+1 end;return c end
''')
script=(root/'companion/source/Scripts/Startup/zzkcd2vr_life_d73.lua').read_text()
lua.execute(script)
lua.execute('''
KCD2VR_LifeD73.Tick();assert(scans==0 and adds==0 and timers==0)
for i=1,5 do packet(1,0,0);KCD2VR_LifeD73.Tick()end;assert(scans==0 and adds==0 and timers==0)
for i=1,5 do step() end;assert(adds==1 and n()==1)
failRemove=true;packet(0,0,0,4);assert(n()==1)
''')
lua.execute(script)  # same VM reload must retain debt and avoid a second timer
lua.execute('''
assert(n()==1 and timers==1);failRemove=false;packet(0,0,0,4);assert(n()==0)
faultAdd=true;for i=1,5 do step()end;assert(n()==0);faultAdd=false
graph['102switch_forceLook']={101};local old=adds
for i=1,5 do step()end;assert(adds==old and n()==0);graph['102switch_forceLook']=nil
for i=1,5 do step()end;assert(n()==1)
t=t+1;KCD2VR_LifeD73.Tick();assert(n()==0)
for i=1,5 do step()end;assert(n()==1)
npc.soul.IsInCombatDanger=function()return true end;step();assert(n()==0)
npc.soul.IsInCombatDanger=function()return false end
packet(0,1,1);KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick();assert(n()==2)
assert(KCD2VR_LifeD73.CanAnimate(npc,true));assert(not KCD2VR_LifeD73.CanGesture(npc))
packet(0,1,0);assert(n()==1);assert(not KCD2VR_LifeD73.CanAnimate(npc,true))
-- Native participant join rejects another person's simultaneous conversation.
packet(0,1,1);KCD2VR_LifeD73.Dialogue(otherNpc);KCD2VR_LifeD73.Tick();assert(n()==2)
assert(not KCD2VR_LifeD73.CanAnimate(otherNpc,true))
packet(0,1,1,7,'500',1);assert(n()==0) -- option changes retire the old lease
KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick();assert(n()==2 and KCD2VR_LifeD73.CanGesture(npc))
packet(0,1,1,7,'501',1);assert(n()==0 and not KCD2VR_LifeD73.CanGesture(npc))
KCD2VR_LifeD73.Tick();assert(n()==0) -- old runDialog witness cannot cross scene
KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick();assert(n()==2)
packet(0,1,1,7,'501',1,'103');assert(n()==0) -- same scene role removed
-- The real helper returns decimal strings, preserving WUIDs beyond 2^53.
Framework.WUIDToMsg=function(id)if id==102 then return '18364758544493064721' else return tostring(id)end end
packet(0,1,1,7,'501',1,'18364758544493064721');KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick();assert(n()==2)
packet(0,1,1,7,'501',1,'18364758544493064722');assert(n()==0)
Framework.WUIDToMsg=function(id)return tostring(id)end
packet(0,1,1,7,'501',1);KCD2VR_LifeD73.Dialogue(npc);KCD2VR_LifeD73.Tick();assert(n()==2)
t=t+.7;packet(0,1,1,7,'501',1);assert(n()==0) -- stale native participant subtree
packet(0,0,0)
for i=1,5 do step()end;assert(n()==1)
local oldPlayer=player;player={id=9,this={id=109},soul={IsInCombatDanger=function()return false end}}
packet(0,0,0);assert(n()==0) -- removal uses old 101 target, not new 109
assert(kcd2vr_life_d73('bad')=='D76|BAD_PACKET')
assert(kcd2vr_life_d73('D76|1|5|1|0|0|BAD|0|0|0|1|0|0|0|0')=='D76|BAD_NUMBER')
''')
print('PASS actual D80 dialogue-only NPC Lua 5.1: exact 64-bit native participants, scene/speech/mode changes, listening option, unrelated dialogue rejection, OFF/outdoor-no-scan, native-link precedence, failed cleanup, partial add, reload, lease expiry, combat, speech, player replacement')
