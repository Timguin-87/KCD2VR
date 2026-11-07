-- T18 source is loaded once by the same established companion bootstrap.
if not rawget(_G,'KCD2VRAmmoSupplyVersion18') and Script and Script.ReloadScript then
 pcall(Script.ReloadScript,'Scripts/KCD2VR/native_ammo_supply_t18.lua')
end
-- T15 invisible inventory holsters. Native DrawFromInventory only; no belt CGFs,
-- bone attachments, slot mutations, cloned weapons, timers or item creation.
if rawget(_G,'KCD2VRGhostSupplyT15')==15 and type(kcd2vr_supply_native_t2)=='function' then return end
local S={id=0,body=0,request=0,serial=0,reserved=nil,keys={{},{}},totals={-1,-1},census=false}
local guid={'43286e47-cde0-50bd-ba4c-9747b777b16d','c0760688-f426-5341-95b9-482b87dbcb9b'}
local cap={5,3}
local function same(v) return v and v==(rawget(_G,'g_localActor') or rawget(_G,'player')) and System.GetEntity(v.id)==v end
local function inspect(key,k)
 local ok,q=pcall(ItemManager.GetItem,key)
 return ok and type(q)=='table' and q.id==key and type(q.class)=='string' and q.class:lower()==guid[k] and q.amount==1 and q or nil
end
local function scan(v)
 local list=v.inventory:GetInventoryTable();if type(list)~='table' then return false end
 local keys={{},{}};local n=0;local seen={}
 for _,key in pairs(list) do
  n=n+1;if n>4096 then return false end
  if not seen[key] then seen[key]=true
   for k=1,2 do if inspect(key,k) then keys[k][#keys[k]+1]=key end end
  end
 end
 S.keys=keys;S.census=true;return true
end
local function token(key)
 if not Framework or type(Framework.WUIDToString)~='function' then return nil end
 local ok,s=pcall(Framework.WUIDToString,key);return ok and s or nil
end
function kcd2vr_supply_native_t2(input)
 if type(input)~='string' or #input>512 then return 'BAD|0|0|0' end
 local p={};for f in (input..'|'):gmatch('([^|]*)|') do p[#p+1]=f end
 if #p~=15 and #p~=24 then return 'BAD|0|0|0' end
 local serial=tonumber(p[1]);local action=p[2];local kind=tonumber(p[3]);local id=tonumber(p[4]);local body=tonumber(p[5]);local request=tonumber(p[6])
 if not serial or serial<1 or serial>=9007199254740992 or serial%1~=0 or (kind~=0 and kind~=1) then return 'BAD|0|0|0' end
 for j=7,15 do local v=tonumber(p[j]);if not v or v~=v or math.abs(v)>6.4 then return 'BAD|0|0|0' end end
 local excluded='-'
 if #p==24 then
  excluded=p[16];if excluded~='-' and not excluded:match('^%[Item%-%d+%]$') then return 'BAD|0|0|0' end
  for j=17,24 do local v=tonumber(p[j]);if not v or v~=v or math.abs(v)>6.4 then return 'BAD|0|0|0' end end
 end
 if action=='clear' then return 'CLEARED|'..serial..'|0|0' end -- never alters any equipment/unknown native reservation
 if action~='refresh' and action~='release' and action~='draw' then return 'BAD|0|0|0' end
 if not id or id<=0 or id%1~=0 or not body or body<=0 or not request or request<=0 then return 'BAD|0|0|0' end
 local v=System and System.GetEntity and System.GetEntity(id)
 if not same(v) or not v.inventory or not v.human or not v.inventory.GetInventoryTable or not v.inventory.GetCountOfClass or
    not v.human.DrawFromInventory or not ItemManager or type(ItemManager.GetItem)~='function' then return 'UNAVAILABLE|'..serial..'|0|0' end
 local changed=S.id~=id or S.body~=body or S.request~=request
 if changed then S.id=id;S.body=body;S.request=request;S.reserved=nil;S.keys={{},{}};S.totals={-1,-1};S.census=false end
 if action=='release' then S.reserved=nil end -- C++ exact transfer/stow acknowledgement owns this edge
 local dirty=changed or action=='release' or action=='draw' or not S.census
 local totals={}
 for k=1,2 do
  local n=v.inventory:GetCountOfClass(guid[k]);if type(n)~='number' or n~=n or n<0 then return 'UNAVAILABLE|'..serial..'|0|0' end
  totals[k]=n;if S.totals[k]~=n then dirty=true end
  -- Same-count inventory replacement can invalidate every cached WUID. Do not
  -- let its zero usable count disable the hip draw that would otherwise rescan.
  -- Check raw cached units, before reservation/left-hand exclusion filtering.
  if not dirty then for _,key in ipairs(S.keys[k]) do if not inspect(key,k) then dirty=true;break end end end
 end
 if dirty then
  S.census=false
  if not scan(v) then return 'INVENTORY_CAP|'..serial..'|0|0' end
  S.totals=totals -- failed census never records totals as successfully observed
 end
 if excluded~='-' and (not Framework or type(Framework.WUIDToString)~='function') then return 'UNAVAILABLE|'..serial..'|0|0' end
 local function usable(key,k) return key~=S.reserved and (excluded=='-' or token(key)~=excluded) and inspect(key,k) end
 local function count(k) local n=0;for _,key in ipairs(S.keys[k]) do if usable(key,k) then n=n+1 end end;return math.min(n,cap[k]) end
 local status='READY'
 if action=='draw' then
  if serial<=S.serial or S.reserved then return 'RESERVED|'..serial..'|'..count(1)..'|'..count(2) end
  S.serial=serial
  local key=nil;for _,candidate in ipairs(S.keys[kind+1]) do if usable(candidate,kind+1) then key=candidate;break end end
  if not key then status='EMPTY' else
   S.reserved=key -- BEFORE native dispatch, including false returns/errors/reentry
   v.human:DrawFromInventory(key,0,true)
   status='REQUESTED'
  end
 end
 if status=='READY' and count(1)==0 and count(2)==0 then status='EMPTY_STOCK' end
 return status..'|'..serial..'|'..count(1)..'|'..count(2)
end
KCD2VRGhostSupplyT15=15
