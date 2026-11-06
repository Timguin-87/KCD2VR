-- KCD2VR Physics Lab (optional add-on for the KCD2VR VR mod). Own script, protocol 1.
-- Does nothing by itself: the KCD2VR ASI calls KCD2VR_Lab.Wake{...} once per weapon/hand contact on a loose prop
-- (F9 rung 3 + VrLabPropWake58). the prop gets AwakePhysics + AddImpulse. only a pickable loose item (PHYSICALIZE
-- classes) without any physics gets Physicalize PE_RIGID first (that one is not undone, see WOKEN below); everything
-- else (BasicEntity dressing, rigid bodies) is WAKE_ONLY: without a rigid body (finite native GetMass > 0) it is refused,
-- a PE_STATIC bench / cart / ladder must never become a 1 kg rigid body.
-- Everything here is pcall-protected; decision diagnostics require the finite F5 capture.
KCD2VR_Lab = KCD2VR_Lab or {}
local G = KCD2VR_Lab
G.protocol = 1
G.version = '58.1-R94'
G.woken = G.woken or {}        -- [id] = true for props we Physicalized ourselves (never un-physicalized)
G.last = G.last or {}          -- [id] = time of the last wake (per entity cooldown)
G.stats = G.stats or { calls = 0, wakes = 0, physicalized = 0, refused = 0, errors = 0 }

-- which entity classes may be woken. everything else is refused (doors, chests, lights, npcs ...). edit here.
-- WAKE_ONLY: AwakePhysics + AddImpulse only when the entity already has a rigid body (finite native GetMass > 0)
local WAKE_ONLY = { 'BasicEntity', 'RigidBody', 'RigidBodyEx', 'DestroyableObject' }
-- PHYSICALIZE: pickable loose items; may get Physicalize PE_RIGID when they have no physics at all
local PHYSICALIZE = { 'PickableItem', 'Pickable', 'Item' }
-- class name fragments that are never touched even if the class is somehow allowed
local DENY = { 'Door', 'Chest', 'Anim', 'Light', 'Area', 'Trigger', 'Spawn', 'Tag', 'Vehicle', 'Torch', 'Fire' }
local PER_SECOND = 8           -- wakes per second (the ASI has the same limit, this is the second line)
local PER_MINUTE = 40
local COOLDOWN = 0.5           -- s per entity
local MIN_K, MAX_K = 0.2, 2.0  -- impulse k = clamp(0.3 * speed, 0.2, 2.0)
local MAX_DIST = 0.6           -- contact point to entity origin (the ASI already checks 0.3; this only catches a wrong id)
local DEFAULT_MASS = 1         -- for a prop we have to Physicalize ourselves

-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s) if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,'KCD2VR_LAB ' .. tostring(s)) end end
local function num(v, d) if type(v) == 'number' and v == v and v ~= math.huge and v ~= -math.huge then return v end return d end
local function vec(t) t = t or {} return { x = num(t.x, 0), y = num(t.y, 0), z = num(t.z, 0) } end
local function now()
  local t = nil
  pcall(function() if System.GetCurrTime then t = System.GetCurrTime() end end)
  if type(t) ~= 'number' then t = os.clock() end
  return t
end

local stamps = {}              -- times of the accepted wakes, trimmed to the last minute
local function budgetOk(t)
  local keep, sec, min = {}, 0, 0
  for _, s in ipairs(stamps) do
    if t - s < 60 then keep[#keep + 1] = s min = min + 1 if t - s < 1 then sec = sec + 1 end end
  end
  stamps = keep
  if sec >= PER_SECOND then return false, 'budget ' .. sec .. '/s' end
  if min >= PER_MINUTE then return false, 'budget ' .. min .. '/min' end
  return true
end

-- returns true, mayPhysicalize | false, why
local function classOk(cls)
  if type(cls) ~= 'string' or cls == '' then return false, 'no class' end
  for _, d in ipairs(DENY) do if cls:find(d, 1, true) then return false, 'denied class ' .. cls end end
  for _, a in ipairs(PHYSICALIZE) do if cls == a then return true, true end end
  for _, a in ipairs(WAKE_ONLY) do if cls == a then return true, false end end
  return false, 'class not allowed ' .. cls
end

local function flagged(e)
  local p = e.Properties
  if type(p) ~= 'table' then return nil end
  if p.bMissionCritical == true or p.bMissionCritical == 1 then return 'bMissionCritical' end
  if p.bQuestItem == true or p.bQuestItem == 1 then return 'bQuestItem' end
  if p.bLocked == true or p.bLocked == 1 then return 'bLocked' end
  return nil
end

-- Native mass is the observation. BasicEntity:IsRigidBody reads legacy
-- top-level Properties fields even though retail defines Properties.Physics;
-- its true result can describe a static body and must never override mass=0.
-- true / false / nil (= unknown; never wake or recreate an unknown body).
local function hasPhysics(e)
  if type(e.GetMass) ~= 'function' then return nil end
  local ok, mass = pcall(function() return e:GetMass() end)
  if not ok or type(mass) ~= 'number' or mass ~= mass or mass == math.huge or mass == -math.huge or mass < 0 then return nil end
  return mass > 0
end

local function refuse(id, why) G.stats.refused = G.stats.refused + 1 log('refused id=' .. tostring(id) .. ' ' .. why) end

function G.Wake(ev)
  G.stats.calls = G.stats.calls + 1
  if type(ev) ~= 'table' or type(ev.id) ~= 'number' then log('bad event') return false end
  local id = ev.id
  local e = System.GetEntity(id)
  if not e then return refuse(id, 'entity not found') end
  if e.actor ~= nil then return refuse(id, 'actor') end
  local ok, why = classOk(e.class)
  if not ok then return refuse(id, why) end
  local mayPhysicalize = why == true
  local f = flagged(e)
  if f then return refuse(id, f) end
  local t = now()
  local last = G.last[id]
  if last and t - last < COOLDOWN then return refuse(id, string.format('cooldown %.0f ms', (t - last) * 1000)) end
  local okB, whyB = budgetOk(t)
  if not okB then return refuse(id, whyB) end
  local d = vec(ev.d)
  local dl = math.sqrt(d.x * d.x + d.y * d.y + d.z * d.z)
  if dl < 0.01 then return refuse(id, 'no direction') end
  d = { x = d.x / dl, y = d.y / dl, z = d.z / dl }
  local speed = num(ev.speed, 0)
  if speed <= 0 then return refuse(id, 'no speed') end
  local pos = nil
  pcall(function() pos = e:GetWorldPos() end)
  if type(pos) ~= 'table' then return refuse(id, 'no world pos') end
  local p = ev.p and vec(ev.p) or pos
  local dist = math.sqrt((p.x - pos.x) ^ 2 + (p.y - pos.y) ^ 2 + (p.z - pos.z) ^ 2)
  if dist > MAX_DIST then return refuse(id, string.format('contact %.2f m from entity', dist)) end
  local k = math.max(MIN_K, math.min(MAX_K, 0.3 * speed))
  local phys = hasPhysics(e)
  if phys == nil then return refuse(id, 'native rigid state unknown') end
  if phys ~= true and not mayPhysicalize then
    -- WAKE_ONLY class without a rigid body: a static piece of dressing, never physicalized (S5)
    return refuse(id, 'no rigid body (' .. tostring(e.class) .. ' is wake-only, phys=' .. tostring(phys) .. ')')
  end
  -- admitted
  G.last[id] = t
  stamps[#stamps + 1] = t
  local how = 'awake'
  if phys == false then
    -- no physics at all: make it a rigid body. this is NOT undone (an un-physicalize of a game prop is more dangerous
    -- than leaving it rigid); logged once per prop
    if System.GetEntity(id) ~= e or flagged(e) or e.actor ~= nil then return refuse(id, 'owner changed before Physicalize') end
    local okP, eP = pcall(function() return e:Physicalize(0, PE_RIGID, { mass = DEFAULT_MASS, density = -1 }) end)
    if not okP or eP == false then G.stats.errors = G.stats.errors + 1 return refuse(id, 'Physicalize failed ' .. tostring(eP)) end
    -- Physicalize normally has no Boolean result. A nonthrowing call is not
    -- completion; require a current positive mass before awake/impulse.
    if System.GetEntity(id) ~= e or hasPhysics(e) ~= true then return refuse(id, 'Physicalize awaiting native mass readback') end
    G.stats.physicalized = G.stats.physicalized + 1
    if not G.woken[id] then G.woken[id] = true log('physicalized id=' .. tostring(id) .. ' class=' .. tostring(e.class) .. ' PE_RIGID mass=' .. DEFAULT_MASS .. ' (stays rigid, not undone)') end
    how = 'physicalized'
  end
  if System.GetEntity(id) ~= e or flagged(e) or e.actor ~= nil then return refuse(id, 'owner changed before wake') end
  local okA, eA = pcall(function() return e:AwakePhysics(1) end)
  if not okA or eA == false then G.stats.errors = G.stats.errors + 1 return refuse(id, 'AwakePhysics failed ' .. tostring(eA)) end
  if System.GetEntity(id) ~= e or hasPhysics(e) ~= true or flagged(e) or e.actor ~= nil then return refuse(id, 'owner or body changed before impulse') end
  local okI, eI = pcall(function() return e:AddImpulse(-1, p, d, k, 1) end)
  local submitted = okI and eI ~= false
  if not submitted then G.stats.errors = G.stats.errors + 1 end
  if submitted then G.stats.wakes = G.stats.wakes + 1 end
  log(string.format('WAKE id=%s class=%s hand=%s speed=%.2f k=%.2f dist=%.2f phys=%s how=%s impulseCall=%s result=%s motion=UNVERIFIED wakes=%d',
    tostring(id), tostring(e.class), tostring(ev.hand), speed, k, dist, tostring(phys), how, tostring(submitted), tostring(eI), G.stats.wakes))
  return submitted -- submission receipt only; current native rigid/pose owner proves motion

end

-- R94 normal small-prop interaction. The ASI supplies a current entity/body,
-- bounds and contact proof; these helpers recheck script ownership and native
-- mass on that exact entity. They never invoke Use/OnUsed or inventory methods.
-- A brush without a script entity/slot cannot reach this path.
local SCENERY_R94 = {BasicEntity=true, RigidBody=true, RigidBodyEx=true,
 RigidBodyLight=true, GeomEntity=true, Debris=true, DestroyableObject=true, BreakableObject=true}
local PROMOTE_R94 = {BasicEntity=true, RigidBody=true, RigidBodyEx=true,
 RigidBodyLight=true, GeomEntity=true, Debris=true}
local function setR94(v) return v == true or v == 1 end
local function massR94(e)
 if type(e.GetMass) ~= 'function' then return nil end
 local ok,m=pcall(e.GetMass,e)
 if not ok or num(m,nil) == nil or m < 0 then return nil end
 return m
end
-- R100: in KCD2 e.id is a userdata handle, not a number (D80 log, corpse_d72 entityId), so
-- e.id ~= id with the numeric id from the ASI was ALWAYS true and every scenery prepare and
-- push was refused. Compare the handle's numeric value, proven by System.GetEntity(n) == e.
local function numIdR100(e)
 if type(e)~='table' or not System or type(System.GetEntity)~='function' then return nil end
 local h,n=e.id,nil
 if type(h)=='number' then n=h
 elseif type(h)=='userdata' then
  local ok,text=pcall(tostring,h)
  local hex=ok and type(text)=='string' and string.match(text,'(%x+)$') or nil
  if hex and #hex<=16 then n=tonumber(hex,16) end
 end
 if num(n,nil)==nil or n<1 or n>4294967295 or n~=math.floor(n) then return nil end
 if System.GetEntity(n)~=e then return nil end
 return n
end
local function sameIdR100(e,id) return e~=nil and (e.id==id or numIdR100(e)==id) end
local function sceneryCurrentR94(e,id,cls)
 if not System or type(System.GetEntity) ~= 'function' or System.GetEntity(id) ~= e or
    not sameIdR100(e,id) or e.class ~= cls or not SCENERY_R94[cls] or e.actor ~= nil or e.item ~= nil or
    e.user ~= nil or e.npcOnly or flagged(e) then return false end
 local p=e.Properties
 -- Authored interaction belongs to its original native events. A method
 -- named OnUsed exists on generic classes too; only active instance flags
 -- are ownership evidence, not merely the inherited method's existence.
 if type(p)=='table' and (setR94(p.bUsable) or setR94(p.bPickable) or
    setR94(p.bImmovable) or setR94(p.bFixed)) then return false end
 return true
end
function G.PrepareSceneryR94(id,cls,mass,promote)
 if type(id)~='number' or id<=0 or type(cls)~='string' or not SCENERY_R94[cls] or
    num(mass,nil)==nil or mass<.05 or mass>3 or type(promote)~='boolean' or
    not System or type(System.GetEntity)~='function' or not PE_RIGID then return false end
 local e=System.GetEntity(id)
 if not e or not sceneryCurrentR94(e,id,cls) or type(e.GetSlotCount)~='function' or
    type(e.IsSlotValid)~='function' or type(e.AwakePhysics)~='function' then return false end
 local ok,count=pcall(e.GetSlotCount,e)
 if not ok or count~=1 or not sceneryCurrentR94(e,id,cls) then return false end
 local valid,slot=pcall(e.IsSlotValid,e,0)
 if not valid or slot~=true or not sceneryCurrentR94(e,id,cls) then return false end
 local before=massR94(e)
 if before==nil or not sceneryCurrentR94(e,id,cls) then return false end
 if promote then
  -- Never recreate an existing rigid body or an authored fracture body.
  if before~=0 or not PROMOTE_R94[cls] or type(e.Physicalize)~='function' then return false end
  local physics=type(e.Properties)=='table' and e.Properties.Physics or nil
  if type(physics)=='table' then
   if physics.bPhysicalize==false or physics.bPhysicalize==0 then return false end
   local authored=num(physics.Mass,nil)
   if authored and authored>0 then
    if authored>3 then return false end
    mass=math.max(.05,authored)
   end
  end
  if not sceneryCurrentR94(e,id,cls) then return false end
  local called,result=pcall(e.Physicalize,e,0,PE_RIGID,{mass=mass,density=-1})
  if not called or result==false then return false end
 else
  if before<.01 or before>3 then return false end
 end
 local after=massR94(e)
 if not sceneryCurrentR94(e,id,cls) or not after or after<.01 or after>3 then return false end
 local called,result=pcall(e.AwakePhysics,e,1)
 if not called or result==false or not sceneryCurrentR94(e,id,cls) then return false end
 local final=massR94(e)
 if not final or final<.01 or final>3 or not sceneryCurrentR94(e,id,cls) then return false end
 if promote then G.woken[id]=true end -- remains native rigid on release
 return true -- ASI must still prove CRigidEntity, mass, unchanged geometry/pose
end
local function exactVectorR94(v)
 return type(v)=='table' and num(v.x,nil)~=nil and num(v.y,nil)~=nil and num(v.z,nil)~=nil
end
local function pushCurrentR94(e,id,cls,item,user)
 if cls~='PickableItem' then return sceneryCurrentR94(e,id,cls) end
 return System.GetEntity(id)==e and sameIdR100(e,id) and e.class==cls and e.item==item and
  not e.npcOnly and not flagged(e) and e.actor==nil and rawget(_G,'g_localActor')==user and
  user and System.GetEntity(user.id)==user
end
function G.PushPropR94(id,cls,point,direction,impulse)
 if type(id)~='number' or id<=0 or type(cls)~='string' or
    (not SCENERY_R94[cls] and cls~='PickableItem') or not exactVectorR94(point) or
    not exactVectorR94(direction) or num(impulse,nil)==nil or impulse<=0 or impulse>2 or
    not System or type(System.GetEntity)~='function' then return false end
 local e=System.GetEntity(id)
 if not e then return false end
 local item,user=e.item,rawget(_G,'g_localActor')
 local function current() return pushCurrentR94(e,id,cls,item,user) end
 if not current() or type(e.GetWorldPos)~='function' or type(e.AwakePhysics)~='function' or
    type(e.AddImpulse)~='function' then return false end
 if cls=='PickableItem' then
  if not item or not user or type(item.CanUse)~='function' or type(item.CanSteal)~='function' or
     type(item.BelongsToDeadBody)~='function' then return false end
  if item:CanUse(user.id)~=true or not current() or item:CanSteal(user.id)~=false or
     not current() or item:BelongsToDeadBody()~=false or not current() then return false end
 end
 local mass=massR94(e)
 if not mass or mass<.01 or mass>3 or not current() then return false end
 local got,pos=pcall(e.GetWorldPos,e)
 if not got or not exactVectorR94(pos) or not current() then return false end
 local dist=(point.x-pos.x)^2+(point.y-pos.y)^2+(point.z-pos.z)^2
 local length=math.sqrt(direction.x^2+direction.y^2+direction.z^2)
 if dist>.65*.65 or length<.99 or length>1.01 then return false end
 local awake,result=pcall(e.AwakePhysics,e,1)
 if not awake or result==false or not current() then return false end
 mass=massR94(e)
 if not mass or mass<.01 or mass>3 or not current() then return false end
 local called,submitted=pcall(e.AddImpulse,e,-1,point,direction,impulse,1)
 return called and submitted~=false -- submission, not a measured motion receipt
end

-- R100 loose scenery props (swap). The ASI has already hidden the brush and suspended its
-- physics; this spawns the loose copy: same .cgf, NOT SAVED (no duplicate after a save load,
-- the level brings the brush back), native rigid body with the ASI's mass. The new id goes
-- back through the private getter note; without the ASI answer the copy is removed again.
G.swapR100 = G.swapR100 or {}
local function vecR100(x,y,z) if num(x,nil)==nil or num(y,nil)==nil or num(z,nil)==nil then return nil end return {x=x,y=y,z=z} end
function G.SwapSpawnR100(serial,path,x,y,z,sx,sy,sz,mass)
 local pos,scale=vecR100(x,y,z),vecR100(sx,sy,sz)
 if type(serial)~='number' or serial<1 or serial>16777215 or serial~=math.floor(serial) or
    type(path)~='string' or #path<5 or #path>199 or not string.match(path,'^[%w_%-%./]+$') or
    string.lower(string.sub(path,-4))~='.cgf' or string.find(path,'..',1,true) or not pos or not scale or
    scale.x<.05 or scale.y<.05 or scale.z<.05 or scale.x>20 or scale.y>20 or scale.z>20 or
    num(mass,nil)==nil or mass<.05 or mass>3 then return false end
 if not System or type(System.SpawnEntity)~='function' or type(System.GetEntity)~='function' or
    type(System.RemoveEntity)~='function' or type(System.GetCVar)~='function' or
    type(ENTITY_FLAG_NO_SAVE)~='number' then return false end
 local params={class='BasicEntity',name='kcd2vr_swap_r100_'..serial,position=pos,scale=scale,flags=ENTITY_FLAG_NO_SAVE,
  properties={object_Model=path,Physics={bPhysicalize=true,bRigidBody=true,bPushableByPlayers=true,bResting=0,Density=-1,Mass=mass}}}
 local ok,e=pcall(System.SpawnEntity,params)
 if not ok or type(e)~='table' then return false end
 local function drop() pcall(System.RemoveEntity,e.id) return false end
 local n=numIdR100(e)
 if not n then return drop() end
 local m=massR94(e)
 if not m or m<.01 or m>3 then return drop() end
 local noted,ack=pcall(System.GetCVar,'kcd2vr_swap_note_r100:'..string.format('%d',serial)..':'..string.format('%d',n))
 if not noted or ack~=1 then return drop() end
 G.swapR100[n]=e
 log(string.format('SWAP_SPAWN serial=%d id=%d model=%s mass=%.2f',serial,n,path,m))
 return true
end
-- remove one loose copy (rollback); true when it is gone
function G.SwapRemoveR100(n)
 if type(n)~='number' or not System or type(System.GetEntity)~='function' then return false end
 local e=G.swapR100[n] or System.GetEntity(n)
 if e and System.GetEntity(n)==e and (G.swapR100[n]==e or (type(e.GetName)=='function' and string.find(tostring(e:GetName()),'kcd2vr_swap_r100_',1,true)==1)) then
  pcall(System.RemoveEntity,n)
 end
 G.swapR100[n]=nil
 return System.GetEntity(n)~=e
end
-- after a load: remove every loose copy that still exists exactly
function G.SwapClearR100()
 local removed=0
 for n,e in pairs(G.swapR100) do
  if System and type(System.GetEntity)=='function' and System.GetEntity(n)==e then pcall(System.RemoveEntity,n) removed=removed+1 end
  G.swapR100[n]=nil
 end
 log('SWAP_CLEAR removed='..removed)
 return true
end

function G.Stats()
  return { calls = G.stats.calls, wakes = G.stats.wakes, physicalized = G.stats.physicalized, refused = G.stats.refused, errors = G.stats.errors }
end

log('loaded version ' .. G.version .. ' protocol ' .. G.protocol)

-- R24: this is called ONLY by the original normal PickableItem.Use invocation.
-- Native GetActions, OnUsed theft suppression and OnUsedHold remain untouched.
-- D85 interact: the small-item mass is the native item definition Weight
-- (S_PickableItemClass+0x58, WHGame ctor 754C3C), returned by the KCD2VR ASI
-- for this exact item only when its native class/weight/bounds/hand admission
-- passes. The ASI is the single authority; this script keeps no second list.
-- Valid answers are whole grams 1..3000; anything else leaves the item static.
local function pickupGramsD85(value)
  if type(value) ~= 'number' or value ~= value or value < 1 or value > 3000 or value ~= math.floor(value) then return nil end
  return value
end
function G.TryPreparePickupR24(e, user)
  if not e or not user or user ~= rawget(_G, 'g_localActor') or e.class ~= 'PickableItem' or
     e.npcOnly or flagged(e) or e.user ~= user or not e.item or not System or not System.GetCVar or
     not System.GetEntity or not Framework or not Framework.WUIDToString or
     not ItemManager or not ItemManager.GetItem or not PE_RIGID then return false,4 end
  local item, actorId, entityId = e.item, user.id, e.id
  local function current()
    return rawget(_G,'g_localActor') == user and user.id == actorId and e.id == entityId and
      e.item == item and e.user == user and not e.npcOnly and System.GetEntity(entityId) == e and
      System.GetEntity(actorId) == user
  end
  if not current() then return false,5 end
  if item:CanUse(actorId) ~= true then return false,6 end
  if not current() then return false,5 end
  if item:CanSteal(actorId) ~= false then return false,7 end
  if not current() then return false,5 end
  if item:BelongsToDeadBody() ~= false then return false,8 end
  if not current() then return false,5 end
  local key = item:GetId()
  local token = Framework.WUIDToString(key)
  if not current() or type(token) ~= 'string' or not token:match('^%[Item%-%d+%]$') then return false,9 end
  local definition = ItemManager.GetItem(key)
  if not current() or type(definition) ~= 'table' or type(definition.class) ~= 'string' or
     Framework.WUIDToString(definition.id) ~= token then return false,10 end
  if not e.GetSlotCount or not e.IsSlotValid or not e.GetMass or not e.Physicalize then return false,11 end
  if e:GetSlotCount() ~= 1 or not current() or e:IsSlotValid(0) ~= true or not current() then return false,12 end
  local mass = e:GetMass()
  if type(mass) ~= 'number' or mass ~= 0 or not current() then return false,13 end
  -- No lease: this same synchronous call rechecks ordinary instant pickup
  -- readiness (or the explicit F9 lab edge), actor,
  -- item-definition/entity/world/local-hand admission immediately before creation.
  if Framework.WUIDToString(item:GetId()) ~= token or not current() then return false,14 end
  -- No native/script leaf between the final live admission and Physicalize.
  -- Only retained Lua identity fields are compared after the native callback.
  local grams = pickupGramsD85(System.GetCVar('kcd2vr_small_pickup_v1:' .. token))
  if not grams then return false,15 end
  if
     rawget(_G,'g_localActor') ~= user or user.id ~= actorId or e.id ~= entityId or
     e.item ~= item or e.user ~= user or e.npcOnly or flagged(e) then return false,16 end
  e:Physicalize(0, PE_RIGID, {mass=grams/1000, density=-1})
  -- Native Physicalize has no success Boolean. This readback is only a script
  -- observation; the existing staged native owner still requires CRigidEntity
  -- and a successful exact dynamics read before admitting any hold.
  if not current() then return false,17 end
  local after = e:GetMass()
  if current() and type(after) == 'number' and after > 0 and after < math.huge then return true,0 end
  return false,18
end

-- Current-contact preparation uses the same audited native item weight as
-- normal pickup. No inventory action, ownership change or automatic Use.
function G.TryPrepareContactR24(id)
  if not System or not System.GetEntity or not System.GetCVar or not Framework or
     not Framework.WUIDToString or not ItemManager or not ItemManager.GetItem or not PE_RIGID then return false end
  local e, user = System.GetEntity(id), rawget(_G, 'g_localActor')
  if not e or not user or e.class ~= 'PickableItem' or e.npcOnly or flagged(e) or not e.item then return false end
  local item, actorId = e.item, user.id
  local function current()
    return rawget(_G,'g_localActor') == user and user.id == actorId and e.id == id and e.item == item and
      not e.npcOnly and not flagged(e) and System.GetEntity(id) == e and System.GetEntity(actorId) == user
  end
  if not current() or item:CanUse(actorId) ~= true or not current() or item:CanSteal(actorId) ~= false or
     not current() or item:BelongsToDeadBody() ~= false or not current() then return false end
  local key = item:GetId()
  local token = Framework.WUIDToString(key)
  if not current() or type(token) ~= 'string' or not token:match('^%[Item%-%d+%]$') then return false end
  local definition = ItemManager.GetItem(key)
  if not current() or type(definition) ~= 'table' or type(definition.class) ~= 'string' or
     Framework.WUIDToString(definition.id) ~= token then return false end
  if not e.GetSlotCount or not e.IsSlotValid or not e.GetMass or not e.Physicalize then return false end
  if e:GetSlotCount() ~= 1 or not current() or e:IsSlotValid(0) ~= true or not current() or e:GetMass() ~= 0 or not current() then return false end
  if Framework.WUIDToString(item:GetId()) ~= token or not current() then return false end
  -- The native reader accepts ONLY the same-stack nominated current contact.
  -- Direct/out-of-scope calls return zero. No native leaf follows
  -- admission before creation; no impulse, OnUsed, or inventory operation here.
  local grams=pickupGramsD85(System.GetCVar('kcd2vr_small_contact_v1:' .. token))
  if not grams or
     rawget(_G,'g_localActor') ~= user or user.id ~= actorId or e.id ~= id or
     e.item ~= item or e.npcOnly or flagged(e) then return false end
  e:Physicalize(0, PE_RIGID, {mass=grams/1000, density=-1})
  if not current() then return false end
  local after = e:GetMass()
  return current() and type(after) == 'number' and after > 0 and after < math.huge
end
