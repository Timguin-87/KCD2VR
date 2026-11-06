-- D72 save-corpse hand drag (D85: numeric id from the native entity handle). Called synchronously by the exact-retail
-- native_script_t2 owner. No timers, living ragdoll, constraints, inventory or carry.
-- PHYS02 Drag established Entity:SetVelocity; this retains a selected corpse and
-- targets a tracked HAND, not player:GetDirectionVector/the camera.
local S={session='',next=0,slots={},scanAfter={}}
-- The current KCD2 skeleton names are documented by gorepak50.h. Keep the old
-- aliases after them for compatible characters; a returned point must be finite.
local bones={'RightHand','LeftHand','RightForeArm','LeftForeArm','RightFoot','LeftFoot','RightLeg','LeftLeg',
 'RHand','LHand','RForeArm','LForeArm','RFoot','LFoot','RCalf','LCalf','Head','Spine2','Pelvis','Neck',
 'Bip01 R Hand','Bip01 L Hand','Bip01 R Forearm','Bip01 L Forearm','Bip01 R Foot','Bip01 L Foot','Bip01 R Calf','Bip01 L Calf','Bip01 Head','Bip01 Spine2','Bip01 Pelvis'}
-- A near nomination authorizes only a short pending input occurrence. Native
-- acquisition still requires 25 cm and does not write position or velocity.
local acquireRadius,nominationRadius=0.25,0.45
local boundsMargin=0.65 -- nomination plus 20 cm tolerance; read-only broad phase
local function finite(n) return type(n)=='number' and n==n and n~=math.huge and n~=-math.huge end
local function vec(p) return type(p)=='table' and finite(p.x) and finite(p.y) and finite(p.z) end
local function distance(a,b) local x,y,z=a.x-b.x,a.y-b.y,a.z-b.z return math.sqrt(x*x+y*y+z*z) end
local function dead(e,audit)
 if not e or not System or type(System.GetEntity)~='function' or System.GetEntity(e.id)~=e then return false end
 local actor,soul=e.actor,e.soul
 if audit then
  audit.current=audit.current+1 -- identity already queried by the existing death gate
  if not (actor and type(actor.IsDead)=='function') and not (soul and type(soul.IsDead)=='function') then audit.deathMissing=audit.deathMissing+1 end
 end
 local ok,d=pcall(function()
  if actor and type(actor.IsDead)=='function' then return actor:IsDead() end
  if soul and type(soul.IsDead)=='function' then return soul:IsDead() end
 end)
 local result=ok and (d==true or d==1) and e.actor==actor and e.soul==soul and System.GetEntity(e.id)==e
 if audit then
  if not ok then audit.deathErrors=audit.deathErrors+1 end
  if result then audit.dead=audit.dead+1 end
 end
 return result
end
-- D85 interact: KCD2 entity ids are script handles (light userdata), not Lua
-- numbers. WHGame registers NULL_ENTITY as ScriptAnyValue type 3 (handle,
-- 14512B4 -> 144D3B4) while plain numbers use type 4 (144D660), and native
-- readers of an entity table's "id" accept only type 3 (2F336E9, 39ACC06).
-- The former number-only filter therefore rejected every entity in the sphere
-- and every query answered NO_NEAR_DEAD_BONE (D80 22:14:15-22:14:30, 16 rows).
-- The handle's numeric value is read once and proven by the same GetEntity
-- lookup the proven petting/washing scripts use with numeric ids.
local function entityId(e)
 if type(e)~='table' or not System or type(System.GetEntity)~='function' then return nil end
 local h,n=e.id,nil
 if type(h)=='number' then n=h
 elseif type(h)=='userdata' then
  local ok,text=pcall(tostring,h)
  local hex=ok and type(text)=='string' and string.match(text,'(%x+)$') or nil
  if hex and #hex<=16 then n=tonumber(hex,16) end
 end
 if not finite(n) or n<1 or n>4294967295 or n~=math.floor(n) then return nil end
 if System.GetEntity(n)~=e then return nil end
 return n
end
-- Share only a copied discovery list between nearby hands in the same short
-- game-time interval. Every use still repeats entity/death/point validation;
-- no cached position, death answer, token or drive permission is shared.
-- A 2.4 m first query contains the original 1.6 m sphere around a second hand
-- at most 0.8 m away. Same-hand reuse is forbidden, so a paused clock cannot
-- sustain a stale cache. Missing clock uses the original independent queries.
local function discovery(ehand,side,audit)
 local at=nil
 if type(System.GetCurrTime)=='function' then
  local ok,t=pcall(System.GetCurrTime);if ok and finite(t) then at=t end
 end
 local previous=S.discovery;S.discovery=nil
 if at and previous and previous.system==System and previous.side~=side and
    at>=previous.at and at-previous.at<=0.03 and distance(ehand,previous.center)<=0.8 then
  if audit then audit.queryReuse=audit.queryReuse+1 end
  return previous.list,true
 end
 if audit then audit.queryCalls=audit.queryCalls+1 end
 local list=System.GetEntitiesInSphere(ehand,at and 2.4 or 1.6) or {}
 if at then
  local count=0
  for _ in pairs(list)do count=count+1;if count>4096 then break end end
  if count<=4096 then
   S.discovery={system=System,side=side,at=at,center={x=ehand.x,y=ehand.y,z=ehand.z},list=list}
  else
   -- Expansion must not turn an original admissible query into QUERY_TOO_DENSE.
   -- This exceptional fallback spends one extra scan and retains the old cap.
   if audit then audit.queryCalls=audit.queryCalls+1 end
   return System.GetEntitiesInSphere(ehand,1.6) or {},false
  end
 end
 return list,at~=nil
end
-- Preserve the original 1.6 m recipient set before spending the 32-corpse
-- budget. Retail GetEntitiesInSphere uses CEntity vtable +170 -> 648030;
-- Entity:GetWorldPos (4D131C) reads the SAME world-translation fields
-- +64/+74/+84. The wider shared list must not crowd a nearer high-ID corpse
-- out of the per-query budget. Missing/current-invalid position uses a fresh
-- original query rather than inventing a location or denying acquisition.
local function originalSphere(list,h,wide,audit)
 if not wide then return list end
 local filtered={}
 for _,e in pairs(list)do
  local include=true
  if type(e)=='table' and type(e.GetBonePos)=='function' and type(e.SetVelocity)=='function' then
   local ok,p=false,nil
   if type(e.GetWorldPos)=='function' then ok,p=pcall(e.GetWorldPos,e) end
   if not ok or not vec(p) then
    S.discovery=nil
    if audit then audit.queryCalls=audit.queryCalls+1;audit.queryReuse=0 end
    return System.GetEntitiesInSphere(h,1.6) or {}
   end
   include=distance(h,p)<=1.6
  end
  if include then filtered[#filtered+1]=e end
 end
 return filtered
end
-- D85R5R2: GetWorldBBox is the retail entity binding at WHGame+4D2188,
-- registered at 144F80B. It reads the render proxy's WORLD box, not an entity
-- origin or a guessed physics part. Native AlchemyTable/AnimDoor use the same
-- no-argument, two-vector return. Do not transform it a second time.
-- A missing/degenerate box cannot veto a grab; only a valid, separated box can
-- avoid the expensive 31-alias search (and native missing-joint log calls).
local function nearBounds(e,h,audit)
 if type(e.GetWorldBBox)~='function' then
  if audit then audit.boundsUnknown=audit.boundsUnknown+1 end
  return true
 end
 if audit then audit.boundsCalls=audit.boundsCalls+1 end
 local ok,lo,hi=pcall(e.GetWorldBBox,e)
 local extent=ok and vec(lo) and vec(hi) and {hi.x-lo.x,hi.y-lo.y,hi.z-lo.z} or nil
 if not extent or extent[1]<0 or extent[2]<0 or extent[3]<0 or
    extent[1]>20 or extent[2]>20 or extent[3]>20 or extent[1]+extent[2]+extent[3]<0.05 then
  if audit then audit.boundsUnknown=audit.boundsUnknown+1 end
  return true
 end
 local dx=math.max(lo.x-h.x,0,h.x-hi.x)
 local dy=math.max(lo.y-h.y,0,h.y-hi.y)
 local dz=math.max(lo.z-h.z,0,h.z-hi.z)
 if dx*dx+dy*dy+dz*dz>boundsMargin*boundsMargin then
  if audit then audit.boundsRejected=audit.boundsRejected+1 end
  return false
 end
 return true
end
local function point(e,b,audit)
 if not e or type(e.GetBonePos)~='function' then return nil end
 if audit then audit.boneCalls=audit.boneCalls+1 end
 local ok,p=pcall(e.GetBonePos,e,bones[b])
 if audit then
  if not ok then audit.boneErrors=audit.boneErrors+1 end
  if ok and vec(p) then audit.finitePoints=audit.finitePoints+1 end
 end
 if ok and vec(p) then return p end
 return nil
end
local function current(s)
 return s and System.GetEntity(s.id)==s.entity and s.entity.actor==s.actor and s.entity.soul==s.soul and dead(s.entity)
end
local function reply(seq,s,p,why,audit)
 local text=string.format('D72|%d|%d|%d|%d|%.9g|%.9g|%.9g|%s',seq,s and s.id or 0,s and s.token or 0,s and s.bone or 0,p and p.x or 0,p and p.y or 0,p and p.z or 0,why)
 -- The lowercase query is requested only by the bounded native F9 observer.
 -- Counters reuse this search; no timer, second sphere scan or physics write.
 if audit then
  return text..string.format('|F9|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%.9g|%d|%d',
   audit.entries,audit.tables,audit.boneMethods,audit.velocityMethods,audit.current,audit.deathMissing,audit.deathErrors,
   audit.dead,audit.idUnreadable,audit.eligible,audit.scanned,audit.boneCalls,audit.boneErrors,audit.finitePoints,
   audit.nearest,audit.nearestId,audit.nearestBone)..string.format('|BBOX|%d|%d|%d|%d|%d',
    audit.boundsCalls,audit.boundsRejected,audit.boundsUnknown,audit.queryCalls,audit.queryReuse)
 end
 return text
end
-- R95: nominate a native-dead ENTITY for C++ part enumeration. Bounds and
-- world position are broad phase only; neither supplies a physics part ID.
local function nativeNomination(e,h)
 if type(e.GetWorldBBox)=='function' then
  local ok,lo,hi=pcall(e.GetWorldBBox,e)
  if ok and vec(lo) and vec(hi) and hi.x>=lo.x and hi.y>=lo.y and hi.z>=lo.z and
     hi.x-lo.x<=20 and hi.y-lo.y<=20 and hi.z-lo.z<=20 then
   return {x=math.max(lo.x,math.min(hi.x,h.x)),y=math.max(lo.y,math.min(hi.y,h.y)),z=math.max(lo.z,math.min(hi.z,h.z))}
  end
 end
 if type(e.GetWorldPos)=='function' then local ok,p=pcall(e.GetWorldPos,e);if ok and vec(p) then return p end end
 return nil
end
local function apply(packet)
 if type(packet)~='string' then return 'D72|BAD_PACKET' end
 local op,session,side,seq,id,token,x,y,z=string.match(packet,'^D72|([NnQqVARDC])|([%d_]+)|([01])|(%d+)|(%d+)|(%d+)|([^|]+)|([^|]+)|([^|]+)$')
 side,seq,id,token,x,y,z=tonumber(side),tonumber(seq),tonumber(id),tonumber(token),tonumber(x),tonumber(y),tonumber(z)
 if not op or not seq or not finite(x) or not finite(y) or not finite(z) then return 'D72|BAD_PACKET' end
 local nativeParts=op=='N' or op=='n'
 local audit=(op=='q' or op=='n') and {entries=0,tables=0,boneMethods=0,velocityMethods=0,current=0,deathMissing=0,deathErrors=0,
  dead=0,idUnreadable=0,eligible=0,scanned=0,boneCalls=0,boneErrors=0,finitePoints=0,nearest=-1,nearestId=0,nearestBone=0,
  boundsCalls=0,boundsRejected=0,boundsUnknown=0,queryCalls=0,queryReuse=0} or nil
 if op=='q' or nativeParts then op='Q' end
 if session~=S.session then S.session=session;S.slots={};S.scanAfter={};S.discovery=nil end -- private retirement, no physics write
 local h={x=x,y=y,z=z};local s=S.slots[side]
 if op=='R' or op=='C' then S.slots[side]=nil;return reply(seq,nil,nil,'RELEASED') end
 if op=='Q' then
  S.slots[side]=nil
  if not System or type(System.GetEntitiesInSphere)~='function' or type(System.GetEntity)~='function' then return reply(seq,nil,nil,'NO_QUERY',audit) end
  local list,wide=discovery(h,side,audit);list=originalSphere(list,h,wide,audit)
  local candidates,seen={},{};local count,unreadable=0,0
  local other=S.slots[1-side]
  for _,e in pairs(list) do
   count=count+1
   if audit then
    audit.entries=count
    if type(e)=='table' then
     audit.tables=audit.tables+1
     if type(e.GetBonePos)=='function' then audit.boneMethods=audit.boneMethods+1 end
     if type(e.SetVelocity)=='function' then audit.velocityMethods=audit.velocityMethods+1 end
    end
   end
   -- An explicit whole-query refusal is deterministic; never accept the first
   -- arbitrary prefix of an overflowing table. Clutter does not spend the
   -- separate, expensive 32-corpse bone-query budget.
   if count>4096 then return reply(seq,nil,nil,'QUERY_TOO_DENSE',audit) end
   if type(e)=='table' and e~=player and e~=g_localActor and (nativeParts or type(e.GetBonePos)=='function') and
      (nativeParts or type(e.SetVelocity)=='function') and dead(e,audit) then
    local n=entityId(e)
    if not n then unreadable=unreadable+1
    -- R96: native-part mode lets both hands hold ONE body (C++ gives the second hand another part); the old
    -- whole-entity SetVelocity mode keeps one owner per body.
    elseif not seen[n] and not (not nativeParts and other and other.held and other.id==n) then
     seen[n]=true;candidates[#candidates+1]={entity=e,id=n}
    end
   end
  end
  if audit then audit.idUnreadable=unreadable;audit.eligible=#candidates end
  table.sort(candidates,function(a,b)return a.id<b.id end)
  -- Rotate through the sorted eligible set when there are more than 32 corpses.
  -- A high-ID corpse is eventually examined; pairs() order never selects it.
  local start=1;local after=S.scanAfter[side] or 0
  for i=1,#candidates do if candidates[i].id>after then start=i;break end end
  local best,pick,bp,bd=nil,nil,nil,nativeParts and 1.6 or nominationRadius
  for offset=0,math.min(32,#candidates)-1 do
   local c=candidates[(start+offset-1)%#candidates+1];S.scanAfter[side]=c.id
   if audit then audit.scanned=audit.scanned+1 end
   if nativeParts then
    local p=nativeNomination(c.entity,h);local d=p and distance(h,p) or math.huge
    if d<bd or (best and d==bd and c.id<best.id) then best,pick,bp,bd=c,0,p,d end
   elseif nearBounds(c.entity,h,audit) then
   for b=1,#bones do local p=point(c.entity,b,audit);if p then local d=distance(h,p)
    if audit and (audit.nearest<0 or d<audit.nearest) then audit.nearest=d;audit.nearestId=c.id;audit.nearestBone=b end
    if d<bd or (best and d==bd and (c.id<best.id or (c.id==best.id and b<pick))) then best,pick,bp,bd=c,b,p,d end
   end end
   end
  end
  if not best and #candidates==0 and unreadable>0 then return reply(seq,nil,nil,'DEAD_ENTITY_ID_UNREADABLE',audit) end
  if not best or not dead(best.entity) or entityId(best.entity)~=best.id then return reply(seq,nil,nil,'NO_NEAR_DEAD_BONE',audit) end
  if S.next>=9007199254740991 then return reply(seq,nil,nil,'TOKEN_EXHAUSTED',audit) end
  S.next=S.next+1
  s={entity=best.entity,id=best.id,actor=best.entity.actor,soul=best.entity.soul,token=S.next,bone=pick,held=false,nativeParts=nativeParts};S.slots[side]=s
  return reply(seq,s,bp,bd<=acquireRadius and 'CANDIDATE' or 'NEAR',audit)
 end
 if not s or s.id~=id or s.token~=token or not current(s) then S.slots[side]=nil;return reply(seq,nil,nil,'OWNER_CHANGED') end
 -- Native part mode never calls GetBonePos or Entity:SetVelocity. Its token
 -- only revalidates native IsDead/current entity while C++ owns reach/part/motor.
 if s.nativeParts then
  if op=='V' then return reply(seq,s,h,s.held and 'HELD' or 'CANDIDATE') end
  if op=='A' and not s.held then s.held=true;return reply(seq,s,h,'ACQUIRED') end
  return reply(seq,nil,nil,'NATIVE_PART_ONLY')
 end
 local p=point(s.entity,s.bone)
 if not p then S.slots[side]=nil;return reply(seq,nil,nil,'BONE_UNAVAILABLE') end
 if op=='V' then
  local d=distance(h,p)
  if not s.held and d>nominationRadius then S.slots[side]=nil;return reply(seq,nil,nil,'OUT_OF_REACH') end
  return reply(seq,s,p,s.held and 'HELD' or (d<=acquireRadius and 'CANDIDATE' or 'NEAR'))
 end
 if op=='A' then
  if s.held or distance(h,p)>acquireRadius then return reply(seq,nil,nil,'ACQUIRE_REFUSED') end
  s.held=true;return reply(seq,s,p,'ACQUIRED') -- no first-frame position/velocity snap
 end
 if op~='D' or not s.held then return reply(seq,nil,nil,'NOT_HELD') end
 local dist=distance(h,p)
 if dist>1.5 then S.slots[side]=nil;return reply(seq,nil,nil,'LEASH') end
 local k=dist>0 and math.min(3,2.5/dist) or 0
 local velocity={x=(h.x-p.x)*k,y=(h.y-p.y)*k,z=(h.z-p.z)*k}
 -- Recheck after getters, directly before the original native entity operation.
 if not current(s) then S.slots[side]=nil;return reply(seq,nil,nil,'OWNER_CHANGED') end
 local ok,result=pcall(s.entity.SetVelocity,s.entity,velocity)
 if not ok or result==false or not current(s) then S.slots[side]=nil;return reply(seq,nil,nil,'VELOCITY_REFUSED') end
 return reply(seq,s,p,'VELOCITY_RETURNED') -- return is not a claim of observed motion
end
function kcd2vr_corpse_d72(packet)
 local ok,r=pcall(apply,packet)
 if ok then return r end
 S.slots={};return 'D72|LUA_ERROR'
end
