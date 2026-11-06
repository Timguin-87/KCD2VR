-- Optional KCD2VR asset starter. No native equip/throw/drop or continuous inventory polling.
KCD2VRThrowingAssets1 = KCD2VRThrowingAssets1 or {}
local M = KCD2VRThrowingAssets1
local ITEMS = {
  {name="kcd2vr_throwing_dagger1", item="43286e47-cde0-50bd-ba4c-9747b777b16d", receipt="70ceb681-04b8-5c5d-bb49-f80f03c9dc40", count=5},
  {name="kcd2vr_throwing_axe1", item="c0760688-f426-5341-95b9-482b87dbcb9b", receipt="86c6952f-0382-5a68-9435-aebcd31a4a39", count=3}
}
local function log(s)
  if System and System.LogAlways then System.LogAlways("KCD2VR_THROW_ASSETS "..s) end
end
M.generation=(M.generation or 0)+1
M.suspended=false;M.attempted={};M.tries=0
local function player()
  local p=rawget(_G,"g_localActor") or rawget(_G,"player")
  return p and p.id and p.inventory and p.soul and p or nil
end
function M.TryStarter()
  if M.suspended then return "DONE" end
  local p=player()
  if not p or type(p.inventory.GetCountOfClass)~="function" or
     type(p.inventory.CreateItem)~="function" or type(p.soul.HasPerk)~="function" or
     type(p.soul.AddPerk)~="function" then return "WAIT" end
  for _,a in ipairs(ITEMS) do
    local attempt=tostring(p.id)..":"..a.receipt
    if not M.attempted[attempt] then
      -- A save-owned soul perk is the durable receipt, not a transient Lua boolean
      -- or an empty-inventory check. Consumed/thrown items are never replenished.
      local ok,done=pcall(function() return p.soul:HasPerk(a.receipt,false) end)
      if not ok or type(done)~="boolean" then return "WAIT" end
      if not done then
        local got,count=pcall(function() return p.inventory:GetCountOfClass(a.item) end)
        if not got or type(count)~="number" or count~=count or count<0 or count>100000 then return "WAIT" end
        count=math.floor(count)
        M.attempted[attempt]=true
        -- Claim before CreateItem: an uncertain native operation must not replay.
        local claimed=pcall(function() p.soul:AddPerk(a.receipt) end)
        local ack,has=pcall(function() return p.soul:HasPerk(a.receipt,false) end)
        if not claimed or not ack or has~=true then
          log("starter refused receipt="..a.name.."; no item created")
        elseif count<a.count then
          local amount=a.count-count
          local completed=0
          -- Never request a multi-unit native weapon stack. Each create asks for
          -- one item; actual WUID/quantity1 admission is still the runtime's job.
          for unit=1,amount do
            local made,result=pcall(function() return p.inventory:CreateItem(a.item,1,1) end)
            local read,after=pcall(function() return p.inventory:GetCountOfClass(a.item) end)
            if not made or result~=true or not read or after~=count+1 then
              log("starter uncertain item="..a.name.." unit="..tostring(unit).."; stopped noAutomaticRetry=1")
              break
            end
            count=after;completed=completed+1
          end
          log("starter="..a.name.." requested="..tostring(amount).." createdUnits="..tostring(completed)..
              " count="..tostring(count).." receipt=CLAIMED noAutomaticRetry=1")
        else
          log("starter="..a.name.." already owned="..tostring(count).." receipt=CLAIMED")
        end
      else
        M.attempted[attempt]=true
      end
    end
  end
  return "DONE"
end
-- Called only by the main companion's already qualified native script callback.
-- Losing every startup timer cannot lose the one-time grant. No current actor,
-- body/request integer or save-owned receipt is inferred from an empty pouch.
function M.NativeReady(p,body,request)
  local function integer(v)return type(v)=="number" and v==math.floor(v) and v>0 and v<9007199254740992 end
  if not integer(body) or not integer(request) or p~=rawget(_G,"g_localActor") or not p or not p.id or
     not System or type(System.GetEntity)~="function" then return "REFUSED" end
  local joined,current=pcall(System.GetEntity,p.id)
  if not joined or current~=p then return "REFUSED" end
  local function ended(reason)
    local key=tostring(body)..":"..reason
    if M.nativeRefusal~=key then
      M.nativeRefusal=key
      log("native starter refused="..reason.." body="..tostring(body)..
          " nextAction=FRESH_CURRENT_NATIVE_BODY noItemCreated=1")
    end
    return reason
  end
  if M.nativeEndPending then
    -- A known outgoing body cannot reopen a world. With no body join, a NEW
    -- exact actor (or no outgoing actor at Ended) plus the fresh native Allowed
    -- request is positive readiness. Only the SAME known outgoing actor needs
    -- one body latch; its distinct refusal is returned to the native receipt.
    if not M.endedBody and M.endedActor==p then M.endedBody=body;return ended("ENDED_ACTOR") end
    if body==M.endedBody then return ended("ENDED_BODY") end
    M.nativeEndPending=false;M.suspended=false
    M.generation=M.generation+1;M.attempted={};M.tries=0
  elseif M.endedBody==body then return ended("ENDED_BODY") end
  if M.nativeBody~=body or M.nativeActor~=p then
    M.nativeBody=body;M.nativeActor=p;M.nativeDoneBody=nil;M.nativeTries=0
  end
  M.nativeRequest=request
  if M.nativeDoneBody==body then return "DONE" end
  M.nativeTries=(M.nativeTries or 0)+1
  if M.nativeTries>300 then return "WAIT_LIMIT" end
  local ok,status=pcall(M.TryStarter)
  if not ok then
    M.nativeDoneBody=body
    log("native starter fault; stopped="..tostring(status).." noAutomaticRetry=1")
    return "UNCERTAIN"
  end
  if status=="DONE" then M.nativeDoneBody=body end
  return status
end
local function tick(generation)
  if generation~=M.generation or M.suspended then return end
  local ok,status=pcall(M.TryStarter)
  if not ok then log("starter fault; stopped="..tostring(status));return end
  if status=="WAIT" then
    M.tries=M.tries+1
    if M.tries<300 and Script and Script.SetTimer then
      Script.SetTimer(1000,function() tick(generation) end)
    else log("starter readiness timed out; native GameplayStarted may rearm") end
  end
end
function M.WorldStarted()
  M.generation=M.generation+1;M.suspended=false;M.attempted={};M.tries=0
  -- C63: only explicit native supply request may call TryStarter; no load-time grant.
end
function M.WorldEnded()
  M.generation=M.generation+1;M.suspended=true;M.attempted={}
  M.endedActor=player()
  M.endedBody=M.nativeActor==M.endedActor and M.nativeBody or nil
  M.nativeEndPending=true;M.nativeDoneBody=nil
end
local registerTries=0
local function register()
  if UIAction and UIAction.RegisterEventSystemListener then
    local ok=pcall(function()
      UIAction.RegisterEventSystemListener(M,"","OnGameplayStarted","WorldStarted")
      UIAction.RegisterEventSystemListener(M,"","OnGameplayEnded","WorldEnded")
    end)
    if ok then return end
  end
  registerTries=registerTries+1
  if registerTries<10 and Script and Script.SetTimer then Script.SetTimer(1000,register) end
end
register()
-- C63: NativeReady from the explicit RIGHT HIP supply toggle is the sole grant entry.
