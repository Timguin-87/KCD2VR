-- Single native animator owner for OUR parchment character. Henry's reading
-- actions, root, equipment and camera are never started or altered here.
local M=KCD2VRMap
if not M then return end
local intent=false
local serial=-1
local manual=nil
local FOLD_CURVE={{0.0,0.0},{0.05,0.1347},{0.1,0.1956},{0.15,0.2441},{0.2,0.287},{0.25,0.3264},{0.3,0.363},{0.35,0.3986},{0.4,0.4327},{0.45,0.4663},{0.5,0.5},{0.55,0.5337},{0.6,0.5673},{0.65,0.6014},{0.7,0.637},{0.75,0.6736},{0.8,0.713},{0.85,0.7559},{0.9,0.8044},{0.95,0.8653},{1.0,1.0}} -- decoded native open.caf fraction -> normalized time
local function clock()
 local ok,t=pcall(function()return System.GetCurrTime()end)
 return ok and type(t)=="number" and t==t and t>=0 and t or nil
end
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s)if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,"KCD2VR_MAP M39 "..s)end end
local function current()
 local p=rawget(_G,"g_localActor") or rawget(_G,"player")
 return p and p.id==M.world and System.GetEntity(p.id)==p and not M.awaitStart
end
local function foldTime(u)
 for i=2,#FOLD_CURVE do
  local a,b=FOLD_CURVE[i-1],FOLD_CURVE[i]
  if u<=b[1]then return a[2]+(b[2]-a[2])*(u-a[1])/(b[1]-a[1])end
 end
 return 1
end
local function openFraction(p)
 local t=p.clip=="close" and 1-p.progress or p.progress
 -- Inverse of the same decoded curve, used only for continuous reversal.
 for i=2,#FOLD_CURVE do
  local a,b=FOLD_CURVE[i-1],FOLD_CURVE[i]
  if t<=b[2]then return a[1]+(b[1]-a[1])*(t-a[2])/(b[2]-a[2])end
 end
 return 1
end
local function start(s,e,opening,t,manualMode)
 local clip=opening and "open" or "close"
 local q=s.start39
 if not q or q.clip~=clip then
  local old=s.paper31
  local u=old and old.clip and openFraction(old) or 0
  if old and old.phase=="OPEN"then u=1 elseif old and old.phase=="CLOSED"then u=0 end
  local from=opening and foldTime(u) or 1-foldTime(u)
  -- Follow retail AnimObject.DoStartAnimation ordering: StartAnimation
  -- requests the clip before querying its length. A length-first gate can
  -- otherwise wait without ever requesting an initially unstreamed CAF.
  e:StopAnimation(0,-1);e:ResetAnimation(0,-1)
  q={clip=clip,from=from,untilTime=t+3,attempts=1}
  q.accepted=e:StartAnimation(0,clip,0,0,manualMode and 0 or 1,false,1)~=false
  s.start39=q;e:Activate(1);e:ForceCharacterUpdate(0,true)
 end
 local length=e:GetAnimationLength(0,clip)
 if length==0 then
  if t>=q.untilTime then error("NATIVE_CLIP_STREAM_EXHAUSTED")end
  s.paper31={phase=opening and "WAIT_OPEN" or "WAIT_CLOSE",progress=q.from,clip=clip,last=t}
  e:ForceCharacterUpdate(0,true);return
 end
 if type(length)~="number" or length~=length or length<=0 or length>5 then error("NATIVE_CLIP_UNAVAILABLE_"..clip)end
 if not q.accepted then
  -- At most ONE retry after a previously missing duration becomes available.
  -- Never restart an accepted/playing clip on monitor or visibility changes.
  if q.attempts>=2 then error("NATIVE_CLIP_REFUSED")end
  q.attempts=2;e:ResetAnimation(0,-1)
  q.accepted=e:StartAnimation(0,clip,0,0,manualMode and 0 or 1,false,1)~=false
  if not q.accepted then error("NATIVE_CLIP_REFUSED")end
 end
 e:SetAnimationSpeed(0,0,manualMode and 0 or 1);e:SetAnimationTime(0,0,q.from)
 e:Activate(1);e:ForceCharacterUpdate(0,true)
 s.paper31={phase=manualMode and "MANUAL" or opening and "OPENING" or "CLOSING",
  progress=q.from,clip=clip,length=length,last=t,start=t,confirmed=false}
 s.start39=nil
 log("stage="..s.paper31.phase.." clip="..clip.." serial="..serial.." nativeClock=1 inventory=NONE")
end
local function advance(p,e,t,visible)
 if p.phase=="OPENING" or p.phase=="CLOSING" then
  -- Read the GAME animator, not a scripted elapsed-time substitute.
  local actual=e:GetAnimationTime(0,0)
  if type(actual)~="number" or actual~=actual or actual<0 or actual>1.01 then error("NATIVE_TIME_INVALID")end
  p.progress=math.max(0,math.min(1,actual))
  e:ForceCharacterUpdate(0,true)
  if actual>=.98 then
   e:SetAnimationSpeed(0,0,0);e:SetAnimationTime(0,0,1)
   p.progress=1;p.phase=p.phase=="OPENING" and "OPEN" or "CLOSED";p.confirmed=true
   log("stage="..p.phase.." serial="..serial.." actualNormalized="..actual.." rendered=UNVERIFIED")
   if p.phase=="CLOSED" then e:Activate(0);e:ForceCharacterUpdate(0,false)end
  elseif t-p.start>p.length+2 then error("NATIVE_CLIP_STALLED")end
 elseif p.phase=="OPEN" or p.phase=="CLOSED" then
  e:SetAnimationSpeed(0,0,0)
  e:ForceCharacterUpdate(0,visible and p.phase=="OPEN")
 end
end
function M.AdvancePaper31(s,e,visible)
 local t=clock()
 if not t then s.paper31={phase="FAULT",last=0,progress=0};e:Hide(1);e:Activate(0);return end
 local p=s.paper31
 if p and t<p.last then s.paper31=nil;s.clipDue31=nil;s.clipUntil31=nil;s.start39=nil;p=nil;manual=nil end
 local ok,why=pcall(function()
  local hand=intent and manual and manual.serial==serial and t>=manual.time and t-manual.time<=.12 and manual.u
  if hand then
   if not p or p.clip~="open" or p.phase~="MANUAL" then start(s,e,true,t,true);p=s.paper31 end
   if p and p.phase=="MANUAL" then
    p.progress=foldTime(hand);p.last=t;e:SetAnimationTime(0,0,p.progress);e:ForceCharacterUpdate(0,true)
   end
   return
  end
  if p and p.phase=="MANUAL" then
   p.phase="OPENING";p.start=t;e:SetAnimationSpeed(0,0,1)
  end
  if not intent and p and p.phase=="WAIT_OPEN" then
   p.phase="CLOSED";p.progress=1;s.start39=nil;s.clipDue31=nil;s.clipUntil31=nil;e:ResetAnimation(0,-1);e:Activate(0);return
  end
  -- A reopen while the close clip is still unstreamed (WAIT_CLOSE) reverses like CLOSING; it used to wait there forever.
  if intent and ((visible and (not p or p.phase=="CLOSED" or p.phase=="WAIT_OPEN")) or (p and (p.phase=="CLOSING" or p.phase=="WAIT_CLOSE"))) then
   start(s,e,true,t,false);p=s.paper31
  elseif not intent and p and (p.phase=="OPEN" or p.phase=="OPENING" or p.phase=="WAIT_CLOSE") then
   start(s,e,false,t,false);p=s.paper31
  end
  if not p then e:ForceCharacterUpdate(0,false);return end
  p.last=t;advance(p,e,t,visible)
 end)
 if not ok then
  if not p or p.phase~="FAULT"then log("stage=FAULT serial="..serial.." reason="..tostring(why))end
  s.paper31={phase="FAULT",last=t,progress=0};e:Hide(1);e:Activate(0);e:ForceCharacterUpdate(0,false)
 end
end
function M.Paper31Animating()
 for _,s in pairs(M.entities or {})do local p=s.paper31
  if p and (p.phase=="OPENING" or p.phase=="CLOSING" or p.phase=="MANUAL" or p.phase=="WAIT_OPEN" or p.phase=="WAIT_CLOSE")then return true end
 end
 return false
end
function kcd2vr_map_fold_m39(packet)
 if type(packet)~="string" or #packet>128 or not current()then return "F39|REFUSED"end
 local a,u=packet:match("^F39|(%d+)|([%d%.%-]+)$");a,u=tonumber(a),tonumber(u)
 local t=clock()
 if a~=serial or not intent or not u or u~=u or not t or (u~=-1 and (u<0 or u>1))then return "F39|REFUSED"end
 manual=u>=0 and {serial=a,u=u,time=t} or nil
 if M.SyncPresentation then M.SyncPresentation(true)end
 return "F39|OK"
end
function kcd2vr_map_m31_intent(packet)
 if type(packet)~="string"then return "I31|REFUSED"end
 local a,b=packet:match("^I31|(%d+)|([01])$");a=tonumber(a)
 if not a or a<serial or a>9007199254740991 then return "I31|REFUSED"end
 if not current()then return "I31|NO_CURRENT_PAPER"end
 if a~=serial then
  manual=nil
  for _,s in pairs(M.entities or {})do if s.paper31 and s.paper31.phase=="FAULT"then s.paper31=nil;s.start39=nil;s.clipDue31=nil;s.clipUntil31=nil end end
 end
 serial=a;intent=b=="1"
 if M.SyncPresentation then M.SyncPresentation(true)end
 local phase="CLOSED"
 for _,s in pairs(M.entities or {})do
  if s.paper31 then
   if s.paper31.phase=="FAULT"then phase="FAULT";break end
   if s.paper31.phase~="CLOSED"then phase=s.paper31.phase end
  end
 end
 return "I31|"..string.format("%.0f",serial).."|"..phase
end
local previousEnd=M.WorldEnded
function M.WorldEnded(...)
 intent=false;manual=nil;M.awaitStart=true
 for _,s in pairs(M.entities or {})do pcall(function()s.entity:Hide(1);s.entity:Activate(0);s.entity:ForceCharacterUpdate(0,false)end);s.paper31=nil;s.start39=nil end
 if previousEnd then return previousEnd(...)end
end
