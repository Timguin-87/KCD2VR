-- Finite native Ammo carrier. No DrawFromInventory/equipment/belt changes.
if rawget(_G,'KCD2VRAmmoSupplyVersion18')==18 then return end
local OLD={'43286e47-cde0-50bd-ba4c-9747b777b16d','c0760688-f426-5341-95b9-482b87dbcb9b'}
local AMMO={'36ac8977-c630-58ae-95a1-21307111d330','ac7fa1ea-0341-5135-baf9-efda53cde18c'}
local LAUNCHER='dc877f56-16f3-5889-b606-5617658717cc'
local S={actor=nil,body=0,request=0,slots={},launcher=nil,busy=false}
local function token(key)
 local ok,t=pcall(Framework.WUIDToString,key)
 return ok and type(t)=='string' and t:match('^%[Item%-%d+%]$') and t or nil
end
local function item(key)
 local ok,t=pcall(ItemManager.GetItem,key)
 return ok and type(t)=='table' and token(t.id)==token(key) and type(t.class)=='string' and
  type(t.amount)=='number' and t.amount>=1 and t.amount==math.floor(t.amount) and t or nil
end
local function role(key,guid)
 local q=item(key);return q and q.class:lower()==guid and q or nil
end
local function inventory(v)
 local list=v.inventory:GetInventoryTable();if type(list)~='table' then return nil end
 local out,seen,n={},{},0
 for _,key in pairs(list) do
  n=n+1;if n>4096 then return nil end
  local t=token(key);if not t then return nil end
  if not seen[t] then seen[t]=true;out[#out+1]=key end
 end
 return out
end
local function inlist(list,key)
 for _,k in ipairs(list) do if token(k)==token(key) then return true end end
 return false
end
local function count(list,guid)
 local n=0;for _,key in ipairs(list) do local q=role(key,guid);if q then n=n+q.amount end end
 return n
end
local function reserved(key)
 local n=0;for _,r in pairs(S.slots) do if token(r.converted or r.key)==token(key) then n=n+1 end end;return n
end
local function output(serial,status,key,legacy,launcher,d,a)
 return table.concat({serial,status,key and token(key) or '-',legacy or 0,launcher and token(launcher) or '-',d or 0,a or 0},'|')
end
local function run(p)
 local serial,action,id,body,request,side,kind=tonumber(p[1]),p[2],tonumber(p[3]),tonumber(p[4]),tonumber(p[5]),tonumber(p[6]),tonumber(p[7])
 if not serial or serial<1 or serial>=9007199254740992 or serial%1~=0 or
    (side~=0 and side~=1) or (kind~=0 and kind~=1) or not id or id<1 or
    not body or body<1 or not request or request<1 then return output(0,'BAD') end
 -- Terminal bookkeeping may outlive the old actor or a loading screen.
 -- It neither inspects inventory nor rewinds the current body/request context.
 if action=='clear' then
  local r=S.slots[side]
  if r and r.serial==serial then
   if p[8]~='-' and (not Framework or not Framework.WUIDToString or token(r.key)~=p[8]) then return output(serial,'REFUSED') end
   S.slots[side]=nil
  end
  return output(serial,'CLEARED')
 end
 local v=rawget(_G,'g_localActor')
 if not v or not v.inventory or not System or System.GetEntity(id)~=v or v.id~=System.GetEntity(id).id or
    not ItemManager or not Framework or not Framework.WUIDToString then return output(serial,'WAIT') end
 if S.actor~=v or S.body~=body or S.request~=request then
  if S.actor~=v then S.launcher=nil end
  S.actor=v;S.body=body;S.request=request;S.slots={}
 end
 local function current() return rawget(_G,'g_localActor')==v and System.GetEntity(id)==v and S.actor==v and S.body==body and S.request==request end
 local totals={}
 for k=1,2 do
  local old=v.inventory:GetCountOfClass(OLD[k]);local ammo=v.inventory:GetCountOfClass(AMMO[k])
  if type(old)~='number' or type(ammo)~='number' or old<0 or ammo<0 or old+ammo>100000 then return output(serial,'WAIT') end
  totals[k]=old+ammo
 end
 local d,a=totals[1],totals[2]
 if action=='refresh' then return output(serial,'READY',nil,0,nil,d,a) end
 local list=inventory(v);if not list or not current() then return output(serial,'WAIT') end
 if action=='prepare' then
  if S.slots[side] then return output(serial,'BUSY',nil,0,nil,d,a) end
  local key,legacy=nil,0
  for _,candidate in ipairs(list) do local q=role(candidate,AMMO[kind+1]);if q and q.amount>reserved(candidate) then key=candidate;break end end
  if not key then for _,candidate in ipairs(list) do local q=role(candidate,OLD[kind+1]);if q and q.amount==1 and reserved(candidate)==0 then key=candidate;legacy=1;break end end end
  if not key then return output(serial,'EMPTY',nil,0,nil,d,a) end
  local r={key=key,kind=kind,serial=serial,legacy=legacy};S.slots[side]=r -- reserve before any native callback
  if not S.launcher or not role(S.launcher,LAUNCHER) then
   for _,k in ipairs(list) do if role(k,LAUNCHER) then S.launcher=k;break end end
   if not S.launcher then
    -- Private source metadata only. C++ immediately moves this exact unit to
    -- the native world inventory; it is never equipped or exposed as stock.
    local before=count(list,LAUNCHER)
    local ok,result=pcall(v.inventory.CreateItem,v.inventory,LAUNCHER,1,1)
    if not current() then return output(serial,'UNCERTAIN') end
    list=inventory(v)
    if not list or count(list,LAUNCHER)~=before+1 then return output(serial,'SOURCE_UNAVAILABLE') end
    for _,k in ipairs(list) do if role(k,LAUNCHER) then S.launcher=k;break end end
    if not S.launcher then return output(serial,'SOURCE_UNAVAILABLE') end
   end
  end
  return output(serial,'PREPARED',key,legacy,S.launcher,d,a)
 end
 local r=S.slots[side]
 if not r or r.kind~=kind or token(r.key)~=p[8] then return output(serial,'REFUSED') end
 if action=='convert' then
  if r.converted then return output(serial,'CONVERTED',r.converted,0,S.launcher,d,a) end
  if r.attempted then return output(serial,r.status or 'UNCERTAIN') end
  -- C++ has already moved exactly this original unit to native escrow and
  -- verified its world owner, absent equipment and absent world representation.
  -- This callback never infers that permission from a missing inventory count.
  if r.legacy~=1 or not role(r.key,OLD[kind+1]) or inlist(list,r.key) then return output(serial,'REFUSED') end
  local before={};local total=0
  for _,k in ipairs(list) do local q=role(k,AMMO[kind+1]);if q then before[token(k)]=q.amount;total=total+q.amount end end
  r.attempted=true;r.status='UNCERTAIN' -- spend before CreateItem, including errors/reentry
  local ok,result=pcall(v.inventory.CreateItem,v.inventory,AMMO[kind+1],1,1)
  if not current() then return output(serial,'UNCERTAIN') end
  list=inventory(v);if not list then return output(serial,'UNCERTAIN') end
  local after=count(list,AMMO[kind+1]);local changed=nil;local census={}
  for _,k in ipairs(list) do local q=role(k,AMMO[kind+1]);if q then
   census[token(k)]=q.amount
   if q.amount==(before[token(k)] or 0)+1 then
    if changed then return output(serial,'UNCERTAIN') end;changed=k
   end
  end end
  -- Net counts alone do not prove a conversion. Another native callback can
  -- remove a prior unit while the new unit is inserted. Require every other
  -- exact WUID quantity to remain identical to its pre-call census.
  local exact=changed~=nil;local ct=changed and token(changed) or nil
  for t,n in pairs(before) do if census[t]~=n+(t==ct and 1 or 0) then exact=false end end
  for t,n in pairs(census) do if n~=(before[t] or 0)+(t==ct and 1 or 0) then exact=false end end
  if after==total+1 and exact then
   r.converted=changed;r.status='CONVERTED'
   -- Best-effort disposal of the inaccessible old escrow receipt, once. The
   -- new native Ammo unit is the only accessible stock after conversion.
   if ItemManager.RemoveItem then pcall(ItemManager.RemoveItem,r.key) end
   return output(serial,'CONVERTED',changed,0,S.launcher,d,a)
  end
  -- Retail Inventory.CreateItem returns true or no results; neither nil nor
  -- an exception proves that no item was inserted/moved during callbacks.
  -- No Lua census can authorize returning the old escrow after such a call.
  return output(serial,'UNCERTAIN')
 end
 return output(serial,'BAD')
end
function kcd2vr_ammo_supply_t18(input)
 if S.busy or type(input)~='string' or #input>256 then return output(0,'BAD') end
 local p={};for field in (input..'|'):gmatch('([^|]*)|') do p[#p+1]=field end
 if #p~=8 then return output(0,'BAD') end
 S.busy=true;local ok,result=pcall(run,p);S.busy=false
 return ok and result or output(tonumber(p[1]) or 0,'UNCERTAIN')
end
KCD2VRAmmoSupplyVersion18=18
