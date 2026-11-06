-- T26 LEFT chest knives only. New explicit one-time target5; no per-load refill.
-- Native inventory delivery is independent of right-hip visuals and VR hand modes.
-- GameplayStarted owns the bounded startup attempt. NativeReady is the existing
-- exact-current-body fallback. No boot inventory polling or replenishment loop.
KCD2VRThrowingAssets1 = KCD2VRThrowingAssets1 or {}
local M=KCD2VRThrowingAssets1
local ITEMS={
  {name="kcd2vr_throwing_dagger1",item="43286e47-cde0-50bd-ba4c-9747b777b16d",receipt="969eb6ac-4837-58a0-98d1-2fc9ac06c5d5",count=5},
}
local RECOVERY_ITEMS={
  {name="kcd2vr_throwing_dagger1",item=ITEMS[1].item,receipt="b033c7eb-4276-5921-b910-04490304152a",count=5},
}
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s)
  if (captureD48("f5") or captureD48("f11")) and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,"T6 INVENTORY "..s) end
end
local function integer(v,max)
  return type(v)=="number" and v==v and v>0 and v<=max and v==math.floor(v)
end
local function player()
  local p=rawget(_G,"g_localActor")
  -- EntityId may be the game's opaque script handle. Preserve it for the exact
  -- native lookup; do not coerce it to a Lua number or compare its text.
  if not p or not p.id or not System or type(System.GetEntity)~="function" then return nil end
  local ok,current=pcall(System.GetEntity,p.id)
  return ok and current==p and p or nil
end
M.generation=(M.generation or 0)+1
M.suspended=true;M.worldActive=false;M.activeActor=nil;M.permitActor=nil
M.attempted={};M.results={};M.tries=0;M.nativeDoneBody=nil;M.nativeDoneStatus=nil
M.nativeTries=0;M.eventAwaitBody=false;M.busy=false
M.deliveryOwnerT8=nil;M.deliveryLoadT8=M.deliveryLoadT8
local function owner(p,generation)
  return generation==M.generation and not M.suspended and player()==p and
    M.activeActor==p and (M.worldActive or M.permitActor==p)
end
local function count(p,a)
  local ok,n=pcall(function() return p.inventory:GetCountOfClass(a.item) end)
  return ok and type(n)=="number" and n==n and n>=0 and n<=100000 and n==math.floor(n) and n or nil
end
local function waiting(p,items)
  items=items or ITEMS
  local status="WAIT"
  for _,a in ipairs(items) do
    local r=M.results[tostring(p.id)..":"..a.receipt]
    if r and r.status=="PARTIAL_UNCERTAIN" then return "PARTIAL_UNCERTAIN" end
    if r and r.status=="CLAIM_REFUSED" then status="CLAIM_REFUSED" end
  end
  return status
end
local function grant(p,generation,items)
  items=items or ITEMS
  for _,a in ipairs(items) do
    if not owner(p,generation) then return "REFUSED" end
    local key=tostring(p.id)..":"..a.receipt
    if not M.attempted[key] then
      local ok,claimed=pcall(function() return p.soul:HasPerk(a.receipt,false) end)
      if not owner(p,generation) then return "REFUSED" end
      if not ok or type(claimed)~="boolean" then return waiting(p,items) end
      local before=count(p,a)
      if not owner(p,generation) then return "REFUSED" end
      if before==nil then return waiting(p,items) end
      local r={status=claimed and "ALREADY_CLAIMED" or "PENDING",count=before,created=0}
      M.results[key]=r;M.attempted[key]=true
      if not claimed then
        -- The old save-owned receipt remains authoritative. Claim and ACK it
        -- BEFORE any item creation; a native uncertainty cannot replay a family.
        local claimOk=pcall(function() p.soul:AddPerk(a.receipt) end)
        if not owner(p,generation) then return "REFUSED" end
        local ack,has=pcall(function() return p.soul:HasPerk(a.receipt,false) end)
        if not owner(p,generation) then return "REFUSED" end
        if not claimOk or not ack or has~=true then
          r.status="CLAIM_REFUSED"
        else
          r.status="PRESENT"
          for unit=1,math.max(0,a.count-before) do
            if not owner(p,generation) then return "REFUSED" end
            local made,result=pcall(function() return p.inventory:CreateItem(a.item,1,1) end)
            if not owner(p,generation) then return "REFUSED" end
            local after=count(p,a)
            if not owner(p,generation) then return "REFUSED" end
            if not made or result~=true or after==nil or after~=r.count+1 then
              r.count=after~=nil and after or -1
              r.status="PARTIAL_UNCERTAIN"
              break
            end
            r.count=after;r.created=r.created+1;r.status="DELIVERED"
          end
        end
      end
      log("item="..a.name.." status="..r.status.." created="..r.created.." count="..r.count..
          " target="..a.count.." receipt=SAVE_OWNED retry=NONE hipRequired=0")
    end
  end
  local status="DONE"
  for _,a in ipairs(items) do
    local r=M.results[tostring(p.id)..":"..a.receipt]
    if not r then return "WAIT" end
    if r.status=="PARTIAL_UNCERTAIN" then return "PARTIAL_UNCERTAIN" end
    if r.status=="CLAIM_REFUSED" then status="CLAIM_REFUSED" end
    if status=="DONE" and r.status=="ALREADY_CLAIMED" and r.count<a.count then status="ALREADY_CLAIMED" end
  end
  return status
end
function M.TryStarter()
  if M.deliveryOwnerT8~=player() or not M.deliveryOwnerT8 then return M.deliveryStatusT8 or "WAIT_NATIVE_LOAD_OWNER" end
  if M.suspended or (not M.worldActive and not M.permitActor) then return "WAIT_WORLD" end
  if M.busy then return "WAIT" end
  local p=player()
  if not p or not p.inventory or not p.soul or type(p.inventory.GetCountOfClass)~="function" or
     type(p.inventory.CreateItem)~="function" or type(p.soul.HasPerk)~="function" or
     type(p.soul.AddPerk)~="function" then return "WAIT" end
  if M.activeActor and M.activeActor~=p then return "REFUSED" end
  M.activeActor=p
  local generation=M.generation
  M.busy=true
  local ok,status=pcall(grant,p,generation,M.deliveryRecoveryT8 and RECOVERY_ITEMS or ITEMS)
  M.busy=false
  if not owner(p,generation) then return "REFUSED" end
  if not ok then log("status=PARTIAL_UNCERTAIN fault="..tostring(status).." retry=NONE");return "PARTIAL_UNCERTAIN" end
  return status
end
local function terminal(status)
  return status=="DONE" or status=="ALREADY_CLAIMED" or status=="CLAIM_REFUSED" or status=="PARTIAL_UNCERTAIN"
end
function M.NativeReady(p,body,request)
  if not integer(body,9007199254740991) or not integer(request,9007199254740991) or player()~=p then return "REFUSED" end
  if M.nativeEndPending then
    if not M.endedBody and M.endedActor==p then M.endedBody=body;return "ENDED_ACTOR" end
    if body==M.endedBody then return "ENDED_BODY" end
    M.nativeEndPending=false;M.suspended=false
    M.generation=M.generation+1;M.attempted={};M.results={};M.activeActor=nil;M.tries=0
  elseif M.endedBody==body then return "ENDED_BODY" end
  if M.nativeBody~=body or M.nativeActor~=p then
    -- Join the first native publication to an event-owned attempt already made
    -- for this actor. Do not erase an uncertain creation merely because the
    -- native fallback is observing the same world for the first time.
    local eventOutcome=M.worldActive and M.eventAwaitBody and M.activeActor==p
    M.nativeBody=body;M.nativeActor=p;M.nativeDoneBody=nil;M.nativeDoneStatus=nil;M.nativeTries=0
    if not eventOutcome then M.attempted={};M.results={};M.activeActor=nil end
  end
  M.eventAwaitBody=false
  M.nativeRequest=request
  if M.nativeDoneBody==body then return M.nativeDoneStatus end
  M.permitActor=p;M.suspended=false
  M.nativeTries=(M.nativeTries or 0)+1
  if M.nativeTries>300 then return "WAIT_LIMIT" end
  local status=M.TryStarter()
  if terminal(status) then M.nativeDoneBody=body;M.nativeDoneStatus=status end
  return status
end
-- An exact native local-player callback can catch a gameplay-start event missed
-- while loading. It grants no fake body epoch and never changes durable receipts.
function M.NativeActorReady(p)
  if player()~=p or (M.nativeEndPending and not M.worldActive) then return "WAIT_WORLD" end
  if M.activeActor and M.activeActor~=p then return "REFUSED" end
  M.permitActor=p;M.suspended=false
  return M.TryStarter()
end
function kcd2vr_inventory_native_t7(input)
  if type(input)~="string" or #input>96 then return "0|BAD" end
  local serial,id=string.match(input,"^(%d+)|(%d+)$")
  serial=tonumber(serial);id=tonumber(id)
  if not integer(serial,9007199254740991) or not integer(id,4294967295) then return "0|BAD" end
  local p=player()
  if not p or not System or type(System.GetEntity)~="function" then return serial.."|WAIT" end
  local ok,current=pcall(System.GetEntity,id)
  if not ok or current~=p then return serial.."|REFUSED" end
  local status=M.NativeActorReady(p)
  if player()~=p then return serial.."|REFUSED" end
  return serial.."|"..status
end
local function tick(generation)
  if generation~=M.generation or M.suspended or not M.worldActive then return end
  local status=M.TryStarter()
  if status=="WAIT" or status=="WAIT_WORLD" then
    M.tries=M.tries+1
    if M.tries<300 and Script and type(Script.SetTimer)=="function" then
      Script.SetTimer(1000,function() tick(generation) end)
    else log("status=WAIT_LIMIT attempts="..M.tries.." fallback=CURRENT_NATIVE_READY") end
  else
    log("status="..status.." source=NATIVE_GAMEPLAY_STARTED hipRequired=0 steadyPolling=0")
  end
end
function M.WorldStarted()
  M.deliveryStatusT8=nil
  log("event=OnGameplayStarted actorCurrent="..tostring(player()~=nil).." source=System")
  M.generation=M.generation+1;M.suspended=false;M.worldActive=true
  M.activeActor=nil;M.permitActor=nil;M.attempted={};M.results={};M.tries=0;M.eventAwaitBody=true
  M.nativeDoneBody=nil;M.nativeDoneStatus=nil
  M.nativeTries=0
  local generation=M.generation
  if Script and type(Script.SetTimer)=="function" then Script.SetTimer(1000,function() tick(generation) end)
  else tick(generation) end
end
function M.WorldEnded()
  M.endedReplenishLoad34=M.replenish34 and M.replenish34.load or nil
  M.endedLoadT8=M.deliveryLoadT8;M.deliveryStatusT8=nil
  M.endedActor=player();M.endedBody=M.nativeActor==M.endedActor and M.nativeBody or nil
  M.generation=M.generation+1;M.suspended=true;M.worldActive=false
  M.activeActor=nil;M.permitActor=nil;M.attempted={};M.results={}
  M.eventAwaitBody=false
  M.nativeEndPending=true;M.nativeDoneBody=nil;M.nativeDoneStatus=nil
end
local registerTries=0
local function register()
  if UIAction and type(UIAction.RegisterEventSystemListener)=="function" then
    local ok,started,ended=pcall(function()
      local a=UIAction.RegisterEventSystemListener(M,"System","OnGameplayStarted","WorldStarted")
      local b=UIAction.RegisterEventSystemListener(M,"System","OnGameplayEnded","WorldEnded")
      return a,b
    end)
    if ok and started~=false and ended~=false then
      log("listener=REGISTERED namespace=System started="..tostring(started).." ended="..tostring(ended))
      return
    end
    log("listener=REFUSED namespace=System attempt="..tostring(registerTries+1).." call="..tostring(ok).." started="..tostring(started).." ended="..tostring(ended))
  end
  registerTries=registerTries+1
  if registerTries<10 and Script and type(Script.SetTimer)=="function" then Script.SetTimer(1000,register)
  else log("status=LISTENER_UNAVAILABLE fallback=CURRENT_NATIVE_READY") end
end
-- Inventory-only knife draw: do not register starter-grant listeners/timers.
-- Existing explicit APIs remain for compatibility; this build never calls them.

-- T8: only the original-first native callback after an observed load close can
-- scope inventory writes. Events and hip refreshes never grant on their own.
function kcd2vr_inventory_native_t8(input)
  if type(input)~="string" or #input>128 then return "0|BAD|0|0" end
  local serial,id,load,recovery=string.match(input,"^(%d+)|(%d+)|(%d+)|([01])$")
  serial=tonumber(serial);id=tonumber(id);load=tonumber(load)
  if not integer(serial,9007199254740991) or not integer(id,4294967295) or
     not integer(load,9007199254740991) or not recovery then return "0|BAD|0|0" end
  local p=player()
  if not p then return serial.."|WAIT|0|0" end
  local ok,current=pcall(System.GetEntity,id)
  if not ok or current~=p then return serial.."|REFUSED|0|0" end
  if M.nativeEndPending and M.endedLoadT8==load then return serial.."|WAIT_WORLD|0|0" end
  if M.deliveryLoadT8 and load<M.deliveryLoadT8 then return serial.."|REFUSED|0|0" end
  if M.deliveryLoadT8~=load then
    -- The save owns claims. Only transient acknowledgements belong to a load.
    M.generation=M.generation+1;M.attempted={};M.results={};M.activeActor=nil
    M.deliveryLoadT8=load;M.nativeEndPending=false;M.deliveryStatusT8=nil
  end
  M.permitActor=p;M.suspended=false;M.deliveryOwnerT8=p;M.deliveryRecoveryT8=recovery=="1"
  local called,status=pcall(M.TryStarter)
  M.deliveryOwnerT8=nil;M.deliveryRecoveryT8=false
  if not called or player()~=p then return serial.."|REFUSED|0|0" end
  local d=count(p,ITEMS[1]);local a=count(p,{item="c0760688-f426-5341-95b9-482b87dbcb9b"})
  if player()~=p or d==nil or a==nil then return serial.."|WAIT|0|0" end
  M.deliveryStatusT8=status
  return serial.."|"..status.."|"..d.."|"..a
end


-- K34: one deliberate menu request tops up finite knives to five. It does NOT
-- remove/change either saved starter/recovery receipt and never runs from a
-- timer, GameplayStarted, inventory refresh or holster draw. Native callback
-- serials are monotonic for this process; keep the high water mark on reload.
function kcd2vr_inventory_replenish_k34(input)
  if type(input)~="string" or #input>128 then return "0|BAD|0|0" end
  local serial,id,load=string.match(input,"^(%d+)|(%d+)|(%d+)$")
  serial=tonumber(serial);id=tonumber(id);load=tonumber(load)
  if not integer(serial,4294967295) or not integer(id,4294967295) or
     not integer(load,2147483647) then return "0|BAD|0|0" end
  local p=player()
  if not p then return serial.."|WAIT_OWNER|0|0" end
  local ok,current=pcall(System.GetEntity,id)
  if not ok or current~=p then return serial.."|REFUSED|0|0" end
  if M.nativeEndPending and (M.endedLoadT8==load or M.endedReplenishLoad34==load) then return serial.."|WAIT_WORLD|0|0" end
  if M.deliveryLoadT8 and load<M.deliveryLoadT8 then return serial.."|REFUSED|0|0" end
  local prior=M.replenish34
  if prior and serial<=prior.serial then
    if serial==prior.serial and prior.actor==p and prior.load==load and not prior.busy then
      return serial.."|"..prior.status.."|"..prior.daggers.."|"..prior.axes
    end
    return serial.."|REFUSED|0|0"
  end
  if M.busy then return serial.."|WAIT_BUSY|0|0" end
  -- No inventory call or intent has happened yet; native may keep this menu
  -- click queued while the finite holder/clear token finishes normally.
  local idle=rawget(_G,"kcd2vr_throw_inventory_idle_k34")
  if type(idle)~="function" then return serial.."|WAIT_BUSY|0|0" end
  local idleOk,isIdle=pcall(idle)
  if not idleOk or isIdle~=true then return serial.."|WAIT_BUSY|0|0" end
  -- This exact native packet owns only the explicit top-up. Do not advance
  -- deliveryLoadT8 or reset starter transients: the ordinary T8 delivery must
  -- still inspect the next save's own durable receipts after this request.
  local generation=M.generation
  local function current34()
    return generation==M.generation and player()==p and
      not (M.nativeEndPending and (M.endedLoadT8==load or M.endedReplenishLoad34==load)) and
      (not M.deliveryLoadT8 or load>=M.deliveryLoadT8)
  end
  -- Persist intent before the FIRST inventory API. If a native call partially
  -- succeeds then errors/reenters, this serial remains terminal and unreplayable.
  local r={serial=serial,actor=p,load=load,status="PARTIAL_UNCERTAIN",daggers=-1,axes=-1,busy=true}
  M.replenish34=r;M.busy=true
  local function finish(status)
    r.status=status;r.busy=false;M.busy=false
    log("request=MENU_REPLENISH serial="..serial.." status="..status.." count="..r.daggers..
        " target=5 retry=NONE receipts=UNCHANGED")
    return serial.."|"..status.."|"..r.daggers.."|"..r.axes
  end
  local called,status=pcall(function()
    if not p.inventory or type(p.inventory.GetCountOfClass)~="function" or
       type(p.inventory.CreateItem)~="function" then return "WAIT" end
    local function familyCount(first,recovered)
      local n=count(p,{item=first})
      if not current34() then return nil end
      local q=count(p,{item=recovered})
      if not current34() or n==nil or q==nil or n+q>100000 then return nil end
      return n+q
    end
    local function knives()
      return familyCount(ITEMS[1].item,"36ac8977-c630-58ae-95a1-21307111d330")
    end
    local n=knives()
    if not current34() then return "REFUSED" end
    local a=familyCount("c0760688-f426-5341-95b9-482b87dbcb9b","ac7fa1ea-0341-5135-baf9-efda53cde18c")
    if not current34() then return "REFUSED" end
    if n==nil or a==nil then return "WAIT" end
    r.daggers=n;r.axes=a
    for unit=1,math.max(0,5-n) do
      if not current34() then return "REFUSED" end
      local made,result=pcall(function() return p.inventory:CreateItem(ITEMS[1].item,1,1) end)
      if not current34() then return "REFUSED" end
      local after=knives()
      if not current34() then return "REFUSED" end
      if after~=nil then r.daggers=after end
      if not made or result~=true or after==nil or after~=n+1 then return "PARTIAL_UNCERTAIN" end
      n=after
    end
    return "DONE"
  end)
  if not called then status="PARTIAL_UNCERTAIN" end
  if not current34() then status="REFUSED" end
  return finish(status)
end
