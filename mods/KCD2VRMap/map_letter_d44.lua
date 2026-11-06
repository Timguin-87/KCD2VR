-- D44: the owned retail parchment is a static open letter. Prepare the native
-- open pose while HIDDEN, freeze at its end, then acknowledge that exact intent.
-- C++ alone places/shows it after this receipt. No visible unfold, close clip,
-- second-hand folding, native Map menu, or Henry reading action is started here.
local M=KCD2VRMap
if not M then return end
local intent=false
local serial=-1
local function clock()
 local ok,t=pcall(function()return System.GetCurrTime()end)
 return ok and type(t)=="number" and t==t and t>=0 and t or nil
end
local function current()
 local p=rawget(_G,"g_localActor") or rawget(_G,"player")
 return p and p.id==M.world and System.GetEntity(p.id)==p and not M.awaitStart
end
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s)
 if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,"KCD2VR_MAP D44_LETTER "..s)end
end
local function hidden(e)
 e:Hide(1);e:Activate(0);e:ForceCharacterUpdate(0,false)
end
local function timeValid(v)return type(v)=="number" and v==v and v>=.98 and v<=1.01 end
function M.AdvancePaper31(s,e,visible)
 local t=clock()
 if not t or not current() then s.paper31=nil;s.letterStartD44=nil;hidden(e);return end
 local p=s.paper31
 if p and t<p.last then p=nil;s.paper31=nil;s.letterStartD44=nil end
 local ok,why=pcall(function()
  if not intent then
   hidden(e)
   s.paper31={phase="CLOSED",last=t,prepared=p and p.prepared or false}
   s.letterStartD44=nil
   return
  end
  if p and p.phase=="FAULT"then hidden(e);return end
  if p and p.prepared and timeValid(e:GetAnimationTime(0,0)) then
   e:SetAnimationSpeed(0,0,0);e:Activate(1);e:ForceCharacterUpdate(0,true)
   p.phase="STATIC_READY";p.last=t;return
  end
  -- A restored/replaced FIFO cannot reuse a formerly ready paper pose.
  e:Hide(1)
  local q=s.letterStartD44
  if not q then
   e:StopAnimation(0,-1);e:ResetAnimation(0,-1)
   q={untilTime=t+3,attempts=1}
   q.accepted=e:StartAnimation(0,"open",0,0,0,false,1)~=false
   s.letterStartD44=q
  end
  if t>=q.untilTime then error("OPEN_POSE_UNAVAILABLE")end
  e:Activate(1);e:ForceCharacterUpdate(0,true)
  local length=e:GetAnimationLength(0,"open")
  if length==0 then s.paper31={phase="PREPARING",last=t};return end
  if type(length)~="number" or length~=length or length<=0 or length>5 then error("OPEN_POSE_LENGTH")end
  if not q.accepted then
   if q.attempts>=2 then error("OPEN_POSE_REFUSED")end
   q.attempts=2;e:ResetAnimation(0,-1)
   q.accepted=e:StartAnimation(0,"open",0,0,0,false,1)~=false
   if not q.accepted then error("OPEN_POSE_REFUSED")end
  end
  e:SetAnimationSpeed(0,0,0)
  if not q.posedAt then
   e:SetAnimationTime(0,0,1);q.posedAt=t
   s.paper31={phase="PREPARING",last=t};return
  end
  -- Require a later native update and an actual normalized-time readback.
  -- The timer/elapsed duration alone never proves the pose.
  if t<=q.posedAt or not timeValid(e:GetAnimationTime(0,0)) then
   s.paper31={phase="PREPARING",last=t};return
  end
  s.paper31={phase="STATIC_READY",last=t,prepared=true};s.letterStartD44=nil
  log("stage=STATIC_READY serial="..serial.." nativePose=OPEN_FROZEN visibility=NATIVE_RECEIPT_REQUIRED")
 end)
 if not ok then
  s.paper31={phase="FAULT",last=t};s.letterStartD44=nil;hidden(e)
  log("stage=FAULT serial="..serial.." reason="..tostring(why))
 end
end
function M.Paper31Animating()
 for _,s in pairs(M.entities or {})do
  if s.paper31 and s.paper31.phase=="PREPARING"then return true end
 end
 return false
end
function kcd2vr_map_fold_m39()return "F39|RETIRED_D44"end
function kcd2vr_map_m31_intent(packet)
 if type(packet)~="string" or #packet>96 then return "I31|REFUSED"end
 local a,b=packet:match("^I31|(%d+)|([01])$");a=tonumber(a)
 if not a or a<serial or a>9007199254740991 then return "I31|REFUSED"end
 if not current()then return "I31|NO_CURRENT_PAPER"end
 if a==serial and intent~=(b=="1")then return "I31|REFUSED"end
 if a~=serial then
  for _,s in pairs(M.entities or {})do
   if s.paper31 and s.paper31.phase=="FAULT"then s.paper31=nil;s.letterStartD44=nil end
  end
 end
 serial=a;intent=b=="1"
 if M.SyncPresentation then M.SyncPresentation(true)end
 local phase=intent and "STATIC_READY" or "CLOSED"
 local count=0
 for _,s in pairs(M.entities or {})do
  count=count+1
  if s.paper31 and s.paper31.phase=="FAULT"then phase="FAULT";break end
  if intent and (not s.paper31 or s.paper31.phase~="STATIC_READY")then phase="PREPARING"end
 end
 if count~=2 and phase~="FAULT"then phase="PREPARING"end
 return "I31|"..string.format("%.0f",serial).."|"..phase
end
local previousEnd=M.WorldEnded
function M.WorldEnded(...)
 intent=false;M.awaitStart=true
 for _,s in pairs(M.entities or {})do
  pcall(function()hidden(s.entity)end);s.paper31=nil;s.letterStartD44=nil
 end
 if previousEnd then return previousEnd(...)end
end
