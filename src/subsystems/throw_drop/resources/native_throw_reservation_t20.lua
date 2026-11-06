-- Source candidate only. No inventory mutation, grant, conversion or native
-- equipment command. The existing native owner must debit one exact stock
-- unit only after its current hand/release receipt is qualified.
if rawget(_G,'KCD2VRThrowReservationVersion20')==20 then return end
local CLASSES={'43286e47-cde0-50bd-ba4c-9747b777b16d','c0760688-f426-5341-95b9-482b87dbcb9b'}
-- Exact retail common dagger; draw does not equip or debit it.
local COMMON_DAGGER='3a640e5d-d8bd-4e8b-b61d-8cd5180e79e7'
local RECOVERED={'36ac8977-c630-58ae-95a1-21307111d330','ac7fa1ea-0341-5135-baf9-efda53cde18c'}
local S={actor=nil,id=0,body=0,request=0,slots={},ended={},busy=false}
local function integer(x,minimum)
 return type(x)=='number' and x>=minimum and x<9007199254740992 and x==math.floor(x)
end
-- The retail Lua VM stores numbers as float32 and stringifies them with %g.
-- Keep native decimal serials as strings for replies AND reservation identity.
local function serialToken(value)
 if type(value)~='string' or not value:match('^[1-9]%d*$') then return nil end
 if #value<16 or (#value==16 and value<'9007199254740992') then return value end
 return nil
end
local function serialAfter(a,b)
 return not b or #a>#b or (#a==#b and a>b)
end
local function reply(serial,status,key,daggers,axes)
 return table.concat({serial,status,key or '-',daggers or 0,axes or 0},'|')
end
local function token(key)
 local ok,value=pcall(Framework.WUIDToString,key)
 return ok and type(value)=='string' and value:match('^%[Item%-%d+%]$') and value or nil
end
local function same(r,id,body,request,serial,kind)
 return r and r.id==id and r.body==body and r.request==request and r.serial==serial and r.kind==kind
end
local function run(p)
 local serial,action,id,body,request,side,kind=serialToken(p[1]),p[2],tonumber(p[3]),tonumber(p[4]),tonumber(p[5]),tonumber(p[6]),tonumber(p[7])
 if not serial or not integer(id,1) or not integer(body,1) or not integer(request,1) or
    (side~=0 and side~=1) or (kind~=0 and kind~=1) or not p[8] then return reply(serial or '0','BAD') end
 -- Only exact native-owner retirement may clear a reservation. This terminal
 -- bookkeeping survives absence/replacement of the old actor and uses no game
 -- calls; sending CLEAR alone is not a native entity-retirement ACK.
 if action=='clear' then
  local r=S.slots[side]
  if r and (not same(r,id,body,request,serial,kind) or p[8]~=r.token) then return reply(serial,'REFUSED') end
  if r and serialAfter(serial,S.ended[side]) then S.ended[side]=serial end
  S.slots[side]=nil
  return reply(serial,'CLEARED')
 end
 if action~='refresh' and action~='prepare' then return reply(serial,'BAD') end
 if not System or not System.GetEntity or not ItemManager or not ItemManager.GetItem or
    not Framework or not Framework.WUIDToString then return reply(serial,'WAIT') end
 local actor=rawget(_G,'g_localActor')
 -- Native EntityId may be an opaque handle; the packet id is numeric.
 -- Join through the native registry, retaining the original handle across queries.
 if not actor or not actor.inventory or not actor.id or System.GetEntity(id)~=actor then return reply(serial,'WAIT') end
 local actorHandle=actor.id
 local function current()
  return rawget(_G,'g_localActor')==actor and actor.id==actorHandle and System.GetEntity(id)==actor and
   S.actor==actor and S.id==id and S.body==body and S.request==request
 end
 if S.actor~=actor or S.id~=id or S.body~=body or S.request~=request then
  -- Never erase a still-owned old hand merely because a new context arrived.
  if next(S.slots) then return reply(serial,'BUSY') end
  S.actor=actor;S.id=id;S.body=body;S.request=request;S.ended={}
 end
 local counts={}
 for k=1,2 do
  local n=actor.inventory:GetCountOfClass(CLASSES[k])
  if not current() or not integer(n,0) or n>100000 then return reply(serial,'WAIT') end
  local recovered=actor.inventory:GetCountOfClass(RECOVERED[k])
  if not current() or not integer(recovered,0) or n+recovered>100000 then return reply(serial,'WAIT') end
  counts[k]=n+recovered
  if k==1 then
   local ordinary=actor.inventory:GetCountOfClass(COMMON_DAGGER)
   if not current() or not integer(ordinary,0) or counts[k]+ordinary>100000 then return reply(serial,'WAIT') end
   counts[k]=counts[k]+ordinary
  end
 end
 if action=='refresh' then return reply(serial,'READY',nil,counts[1],counts[2]) end
 if not serialAfter(serial,S.ended[side]) then return reply(serial,'STALE') end
 local existing=S.slots[side]
 if existing and not same(existing,id,body,request,serial,kind) then return reply(serial,'BUSY') end
 local keys=actor.inventory:GetInventoryTable()
 if not current() or type(keys)~='table' then return reply(serial,'WAIT') end
 local seen,n={},0
 for _,key in pairs(keys) do
  n=n+1;if n>4096 then return reply(serial,'WAIT') end
  local t=token(key)
  if not current() or not t then return reply(serial,'WAIT') end
  if not seen[t] then
   seen[t]=true
   local item=ItemManager.GetItem(key)
   if not current() then return reply(serial,'WAIT') end
   if type(item)~='table' or token(item.id)~=t or not current() or
      type(item.class)~='string' or not integer(item.amount,1) then return reply(serial,'WAIT') end
   local class=item.class:lower()
   if class==CLASSES[kind+1] or class==RECOVERED[kind+1] or (kind==0 and class==COMMON_DAGGER) then
    local reserved=0
    for s,r in pairs(S.slots) do if s~=side and r.token==t then reserved=reserved+1 end end
    if (not existing or (existing.token==t and existing.class==class)) and item.amount>reserved then
     if not current() then return reply(serial,'WAIT') end
     S.slots[side]={id=id,body=body,request=request,serial=serial,kind=kind,token=t,class=class}
     return reply(serial,'RESERVED',t,counts[1],counts[2])
    end
   end
  end
 end
 return reply(serial,'EMPTY',nil,counts[1],counts[2])
end
-- Read-only inventory admission: a pending/held/retiring finite reservation
-- must finish before menu replenishment. A launched flight has no slot here.
function kcd2vr_throw_inventory_idle_k34()
 return not S.busy and next(S.slots)==nil
end
function kcd2vr_throw_reservation_t20(text)
 if type(text)~='string' or #text>256 then return reply(0,'BAD') end
 local p={};for part in (text..'|'):gmatch('(.-)|') do p[#p+1]=part end
 local serial=serialToken(p[1]) or '0'
 if #p~=8 then return reply(serial,'BAD') end
 if S.busy then return reply(serial,'BUSY') end
 S.busy=true
 local ok,value=pcall(run,p)
 S.busy=false
 return ok and value or reply(serial,'UNKNOWN')
end
KCD2VRThrowReservationVersion20=20
