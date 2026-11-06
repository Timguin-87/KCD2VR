-- Optional native dialogue ForceLook/face-idle proposal, driven by the D73 DLL.
-- D81: current native participants; Henry speech and witnessed quiet phases.
-- No outdoor discovery, bone sampling or gameplay animation admission.
-- Private graph links never remove a quest/greeting's switch_forceLook link.
KCD2VR_LifeD73 = KCD2VR_LifeD73 or { active={}, gaze={}, dialogs={}, t=0 }
local L=KCD2VR_LifeD73
local LOOK='kcd2vr_lifeLookD73'
local LISTEN='kcd2vr_lifeListenD73'
local function member(e)
  if not e or not e.this or not L.members or not Framework or not Framework.WUIDToMsg then return false end
  -- Exact retail WUIDToMsg197C90C is decimal uint64; never round through a Lua number.
  local ok,id=pcall(Framework.WUIDToMsg,e.this.id)
  return ok and type(id)=='string' and L.members[id] or false
end
local function now() return System.GetCurrTime() end
local function fresh() return L.untilTime and now()<=L.untilTime and L.player==player end
-- Packet eligibility only: OFF does not ask the engine for time, actors or bones.
function L.Enabled() return L.dialogue and fresh() or false end
local function valid(e)
  if not e or e==player or not e.this or not e.human or not e.soul or not e.actor then return false end
  local ok,dead=pcall(e.actor.IsDead,e.actor)
  if not ok or dead then return false end
  local oku,uncon=pcall(e.actor.IsUnconscious,e.actor)
  if oku and uncon then return false end
  local okc,combat=pcall(e.soul.IsInCombatDanger,e.soul)
  return okc and not combat
end
local function same(a)
  local e=System.GetEntity(a.id)
  return e==a.entity and e.this and e.this.id==a.from
end
local function links(from,tag)
  local ok,result=pcall(XGenAIModule.FindLinks,from,tag)
  if ok and type(result)=='table' then return result end
end
local function release(key)
  local a=L.active[key];if not a then return true end
  -- The old exact WUIDs survive player changes, failed cleanup and script reload.
  -- FindLinks empty is positive retirement proof; a missing entity is not.
  local existing=links(a.from,a.tag)
  if existing and next(existing)==nil then L.active[key]=nil return true end
  if not same(a) then return false end
  local ok,result=pcall(XGenAIModule.RemoveLink,a.from,a.to,a.tag)
  if not ok or result==false then return false end
  local after=links(a.from,a.tag)
  if after and next(after)==nil then L.active[key]=nil return true end
  return false
end
local function acquire(e,tag,t)
  local key=tostring(e.id)..':'..tag
  local a=L.active[key]
  if a then
    if same(a) and a.to==player.this.id then a.seen=t return true end
    release(key);return false
  end
  -- Never adopt pre-existing private or native links; preserve native look tasks.
  local native=links(e.this.id,'switch_forceLook');local own=links(e.this.id,tag)
  if not native or next(native) or not own or next(own) then return false end
  a={id=e.id,entity=e,from=e.this.id,to=player.this.id,tag=tag,since=t,seen=t}
  L.active[key]=a -- retain even if AddLink applies then faults
  local ok,result=pcall(XGenAIModule.AddLink,a.from,a.to,tag)
  if not ok or result==false then release(key) return false end
  return true
end
local function count(tag)
  local n=0;for _,a in pairs(L.active) do if not tag or a.tag==tag then n=n+1 end end;return n
end
local function playerSafe()
  if not L.Enabled() or not player or not player.this or not player.soul then return false end
  local ok,combat=pcall(player.soul.IsInCombatDanger,player.soul)
  return ok and not combat
end
local function participant(e,t)
  local d=e and e.id and L.dialogs[e.id]
  return L.dialogue and L.scene and L.scene~='0' and member(e) and d and d.entity==e and
    d.scene==L.scene and t>=d.tick and t-d.tick<=.6
end
function L.Dialogue(e)
  if L.dialogue and L.Enabled() and member(e) and e.id then L.dialogs[e.id]={entity=e,tick=now(),scene=L.scene} end
end
function L.CanAnimate(e,listen)
  if not playerSafe() or not valid(e) then return false end
  if not participant(e,now()) or (listen and not L.listening) then return false end
  local a=L.active[tostring(e.id)..':'..(listen and LISTEN or LOOK)]
  return a and same(a) and a.to==player.this.id or false
end
function L.CanGesture(e)
  return L.bodyGestures and L.CanAnimate(e,true) or false
end
local function tick()
  local t=now();L.t=t
  if not playerSafe() then
    for key in pairs(L.active) do release(key) end;L.gaze={};L.dialogs={};return
  end
  L.gaze={} -- retire legacy outdoor dwell state; never discover world NPCs
  -- Only current native scene members executing their own runDialog subtree.
  -- No nearest-person guess, scene-pointer inference or forced dialogue action.
  if L.dialogue then
    for id,d in pairs(L.dialogs) do
      if participant(d.entity,t) and valid(d.entity) then
        acquire(d.entity,LOOK,t)
        if L.listening then acquire(d.entity,LISTEN,t) end
      else L.dialogs[id]=nil end
    end
  end
  for key,a in pairs(L.active) do
    local native=links(a.from,'switch_forceLook')
    local retire=not same(a) or not valid(a.entity) or a.to~=player.this.id or not native or next(native)
    if a.tag==LISTEN then retire=retire or not L.listening or not participant(a.entity,t)
    else retire=retire or not participant(a.entity,t) end
    if retire then release(key) end
  end
end
function kcd2vr_life_d73(packet)
  local f={};for v in string.gmatch(packet or '', '[^|]+') do f[#f+1]=v end
  if #f~=15 or f[1]~='D76' then return 'D76|BAD_PACKET' end
  if not string.match(f[13],'^%d+$') then return 'D76|BAD_SCENE' end
  local members={}
  for id in string.gmatch(f[15],'[^,]+') do
    if not string.match(id,'^%d+$') then return 'D76|BAD_MEMBER' end
    if id~='0' then members[id]=true end
  end
  local pos={x=tonumber(f[7]),y=tonumber(f[8]),z=tonumber(f[9])}
  local dir={x=tonumber(f[10]),y=tonumber(f[11]),z=tonumber(f[12])}
  for _,v in pairs({pos.x,pos.y,pos.z,dir.x,dir.y,dir.z}) do if v~=v or math.abs(v)>1e7 then return 'D76|NONFINITE' end end
  if not pos.x or not pos.y or not pos.z or not dir.x or not dir.y or not dir.z then return 'D76|BAD_NUMBER' end
  if L.vm~=f[2] or L.mode~=f[3] or L.player~=player or L.scene~=f[13] then
    for key in pairs(L.active) do release(key) end;L.gaze={};L.dialogs={}
  end
  L.vm=f[2];L.mode=f[3];L.player=player;L.pos=pos;L.dir=dir
  L.gameplay=false -- legacy packet field4 is deliberately not an outdoor opt-in
  L.dialogue=f[5]=='1' and f[13]~='0';L.listening=f[6]=='1'
  L.scene=f[13];L.members=members;L.bodyGestures=f[14]=='1'
  L.untilTime=now()+.35
  -- Disabling or an NPC subtitle retires LISTEN immediately; no timer delay.
  -- Packet field 6 means Henry speech or a positively witnessed native quiet phase.
  if not L.dialogue then for key in pairs(L.active) do release(key) end end
  if not L.listening then for key,a in pairs(L.active) do if a.tag==LISTEN then release(key) end end end
  if L.dialogue then for key,a in pairs(L.active) do if not participant(a.entity,now()) then release(key) end end end
  L.Wake() -- valid ON packet, or exact owned-link retirement debt, is the only wake
  return 'D76|'..tostring(count())
end
-- No startup polling. A valid packet wakes one retained timer; explicit OFF or
-- lease expiry stops it after owned links have positively retired. One callback
-- already queued in the engine may still drain after OFF, without any NPC query.
-- A reload of this scheduler swaps functions while retaining its pending ticket.
L.Tick=tick
function L.NeedsTimer() return next(L.active)~=nil or L.Enabled() end
function L.Wake()
  if L.timerPending or not L.NeedsTimer() then return end
  L.timerPending=true
  Script.SetTimer(250,function()
    L.timerPending=false
    if not L.NeedsTimer() then L.gaze={};L.dialogs={};return end
    local ok,err=pcall(function() L.Tick() end)
    if not ok then
      L.untilTime=0
      if not L.errorLogged then L.errorLogged=true;System.LogAlways('KCD2VR_LIFE D73 error '..tostring(err)) end
    end
    L.Wake() -- current function after reload; keep only exact cleanup debt alive
  end)
end
L.Wake() -- reload may retain a current lease/debt; fresh startup has neither
