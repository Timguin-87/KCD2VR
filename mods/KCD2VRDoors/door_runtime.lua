-- Own additive startup module. Never substitutes native OnUsed/eligibility or end events.
-- Transport implementation/registration belongs to the exact-retail ASI bridge.
KCD2VRDoors = KCD2VRDoors or {}
local M=KCD2VRDoors
if M.installed then return end
local PREFIX="kcd2vr_door_"
local VERSION=1966
local REVISION=66
local sessions={}
local nativePlay
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(message)
  if not captureD48("f5") then return end
  local write=System and (System.LogAlways or System.Log)
  if type(write)=="function" then pcall(write,"KCD2VR C66 DOOR "..message) end
end
local function number(v) return type(v)=="number" and v==v and v~=math.huge and v~=-math.huge end
local function integer(v,lo,hi) return number(v) and v>=lo and v<=hi and math.floor(v)==v end
local function get(n) return System and System.GetCVar and System.GetCVar(PREFIX..n) end
local function input()
  local seq=get("seq")
  if not integer(seq,2,8388606) or seq%2~=0 then return nil end
  local s={seq=seq,version=get("version"),request=get("request"),hi=get("target_hi"),lo=get("target_lo"),phase=get("phase"),fraction=get("fraction")}
  s.mode69=get("mode69")
  if get("seq")~=seq or s.version~=VERSION or not integer(s.request,0,8388607) or
    not integer(s.hi,0,65535) or not integer(s.lo,0,65535) or
    (s.phase~=0 and s.phase~=1 and s.phase~=2 and s.phase~=4 and s.phase~=5) or
    not number(s.fraction) or s.fraction<0 or s.fraction>999500 then return nil end
  if s.mode69==nil or s.mode69==0 then s.mode69=2 end -- old ASI remains spatial/native
  if s.mode69~=2 and s.mode69~=3 then return nil end
  s.fraction=s.fraction/1000000
  s.target=s.hi*65536+s.lo
  if s.target>16777215 then return nil end
  return s
end
local function output(s,state,fraction)
  -- A single bounded integer-only receipt copied by the ASI's private getter
  -- branch; no SetCVar/registration/foreign-thread Lua entry is involved.
  if not System or not System.GetCVar then return end
  local q=math.floor(math.max(0,math.min(0.9995,fraction or 0))*1000000+0.5)
  return System.GetCVar(PREFIX.."ack_"..s.seq.."_"..s.request.."_"..state.."_"..q)==1
end
-- This callback executes only on the Door's own native Lua update thread.
-- It publishes a copied alias/time request; the ASI verifies/binds its own
-- exact rigid handle curve. No Lua setter is used to discover that curve.
local function curve(s,c,t)
  if type(s.clip)~="string" or #s.clip<1 or #s.clip>80 or
     not string.match(s.clip,"^[A-Za-z0-9_-]+$") or not number(t) or t<0 or t>0.9995 then return 0 end
  local q=math.floor(t*1000000+0.5)
  local key=PREFIX.."curve_"..c.seq.."_"..c.request.."_"..s.clip.."_"..q
  if #key>=128 then return 0 end
  local status=System.GetCVar(key)
  return (status==1 or status==2) and status or 0
end
local function same(self,s)
  return self.curAnim==s.clip and self.nDirection==s.direction and self.lastUsedBy==s.user and
    self.inUse==1 and self.bNeedUpdate==1
end
local function progress(s) return s.mode69==3 and s.rate==-1 and 1-s.last or s.last end
local function release(self,s,reason)
  -- Cleanup-only once requested: a failed read/restore cannot renew hand control.
  s.cancel=true
  if s.owned and same(self,s) then
    local rate=self:GetAnimationSpeed(0,0)
    if not number(rate) then return false end
    if rate==0 then
      self:SetAnimationSpeed(0,0,s.rate)
      if self:GetAnimationSpeed(0,0)~=s.rate then return false end
    end -- Any known nonzero rate belongs to the current native writer.
  end -- A replaced clip/owner cannot receive our restore.
  if s.command and not output(s.command,0,progress(s) or 0) then return false end
  if s.owned then log("CLEANUP target="..(s.command and s.command.target or 0).." reason="..reason.." restore=NATIVE_FINISH") end
  sessions[self]=nil
  return true
end
local function completion(self,s)
  -- The native original has already run and performed its own completion
  -- reporting. Receipt retry owns no animation writer and must survive Stop.
  if not s.completed then
    if not s.finishPending or self.curAnim~="" or self.inUse~=0 or self.bNeedUpdate~=0 or
       self.nDirection~=s.direction or self.lastUsedBy~=s.user then return false end
    s.completed=true
  end
  if not s.command or not output(s.command,2,progress(s)) then return false end
  log("COMPLETED target="..s.command.target.." request="..s.command.request.." nativeUpdate=FINISHED")
  sessions[self]=nil
  return true
end
local function preupdate(self,dt)
  local s=sessions[self];if not s then return end
  if s.completed then completion(self,s);return end
  if s.cancel then release(self,s,"RESTORE_RETRY");return end
  if s.finishPending then return end -- only original native completion owns the endpoint now
  if not number(dt) or dt<0 or dt>1 then release(self,s,"BAD_DT");return end
  s.age=s.age+dt;s.quiet=s.quiet+dt
  local c=input()
  if not same(self,s) then release(self,s,"NATIVE_OWNER");return end
  if not c or c.target~=self:GetRawId() or c.request==0 or c.request==s.before or (s.command and c.request~=s.command.request) or c.phase==0 then
    if s.owned then release(self,s,"CONTEXT_OR_CLEANUP")
    elseif s.age>0.25 then sessions[self]=nil end
    return
  end
  if c.seq~=s.sequence then s.sequence=c.seq;s.quiet=0 end
  if s.quiet>0.08 then release(self,s,"STALE");return end
  if not s.owned then
    if s.age>0.25 or c.phase~=1 then sessions[self]=nil;return end
    local t=self:GetAnimationTime(0,0)
    local rate=self:GetAnimationSpeed(0,0)
    local initial=rate==-1 and 1-t or t
    if not number(t) or t<0 or t>1 or (rate~=1 and rate~=-1) or initial>=0.9995 or not self:IsAnimationRunning(0,0) then sessions[self]=nil;return end
    -- Claim cleanup before a possibly applied pause. The native bridge's Stop
    -- disables new claims first and retains its getter until ACK0/ACK2 retires it.
    if System.GetCVar(PREFIX.."claim_"..c.seq.."_"..c.request)~=1 then return end
    s.command=c;s.last=t;s.rate=rate;s.owned=true;s.paused=true;s.originalDirection=s.direction;s.mode69=c.mode69
    self:SetAnimationSpeed(0,0,0)
    if self:GetAnimationSpeed(0,0)~=0 then release(self,s,"PAUSE_NOT_CONFIRMED");return end
    log("CLAIMED target="..c.target.." request="..c.request.." pauseReadback=0")
    local status=c.mode69==3 and 1 or curve(s,c,t)
    if status==0 then release(self,s,"CURVE_BIND_REFUSED");return end
    s.curveReady=status==1
    output(c,1,progress(s))
    return
  end
  local t=self:GetAnimationTime(0,0)
  local rate=self:GetAnimationSpeed(0,0)
  if not number(t) or t<0 or t>1 or rate~=0 or math.abs(t-s.last)>0.00002 or not self:IsAnimationRunning(0,0) then
    release(self,s,"NATIVE_WRITER");return
  end
  s.command=c
  s.last=t
  if s.mode69~=c.mode69 then release(self,s,"DOOR_MODE_CHANGED");return end
  local curveStatus=c.mode69==3 and 1 or curve(s,c,t)
  if curveStatus==0 or (s.curveReady and curveStatus~=1) then
    release(self,s,"CURVE_OWNER_LOST");return
  end
  s.curveReady=s.curveReady or curveStatus==1
  local curveReady=curveStatus==1
  if c.phase==5 then
    -- C++ qualifies physical release at this exact native curve's endpoint.
    -- This is only a request: original Server.OnUpdate below owns completion.
    if c.fraction>=0.999 then
      s.finishPending=true
      self:SetAnimationTime(0,0,1)
      if self:GetAnimationTime(0,0)~=1 then release(self,s,"ENDPOINT_TIME_NOT_CONFIRMED");return end
      s.last=1
      self:SetAnimationSpeed(0,0,s.rate)
      if self:GetAnimationSpeed(0,0)~=s.rate then release(self,s,"ENDPOINT_RATE_NOT_CONFIRMED");return end
    elseif c.fraction<=0.0005 then
      -- Reaching the start again must report the original opposite state. Use
      -- native object-only DoPlay, never a new player action or invented event.
      local wanted=-s.originalDirection
      -- Retail ignores forceTime when reusing a running bUseSameAnim clip.
      -- Stop only our exact reserved Door clip so original DoPlay selects its
      -- own new opposite object clip and honors its normal forceTime branch.
      if self.bUseSameAnim then self:StopAnimation(0,0) end
      nativePlay(self,wanted,1,false,nil,false,s.user)
      if type(self.curAnim)~="string" or self.curAnim=="" or self.nDirection~=wanted or
         self.lastUsedBy~=s.user or self.inUse~=1 or self.bNeedUpdate~=1 then
        release(self,s,"OPPOSITE_NATIVE_START_REFUSED");return
      end
      s.clip=self.curAnim;s.direction=self.nDirection
      local endpoint=self:GetAnimationTime(0,0)
      local nativeRate=self:GetAnimationSpeed(0,0)
      -- DoPlay's real resulting clip/rate/time, not a guessed inverse pose,
      -- becomes the cleanup/completion identity of this same native operation.
      if number(nativeRate) and nativeRate~=0 then s.rate=nativeRate end
      s.last=endpoint
      if endpoint~=1 or (nativeRate~=1 and nativeRate~=-1) then
        release(self,s,"OPPOSITE_ENDPOINT_NOT_CONFIRMED");return
      end
      s.finishPending=true
    else
      -- A malformed non-endpoint finalization cannot silently complete a door.
      release(self,s,"FINALIZE_OUTSIDE_ENDPOINT");return
    end
    log("FINALIZE_REQUEST target="..c.target.." direction="..s.direction.." nativeUpdate=PENDING")
    return
  end
  -- Grip attaches/releases; only controller-owned phase2 time can move the
  -- accepted native object. Still hand, repeated command, phase1 or release4
  -- never resumes an autonomous native PLAY or starts a player animation.
  if c.phase==2 then
    if not curveReady then output(c,1,progress(s));return end
    local wanted=c.mode69==3 and s.rate==-1 and 1-c.fraction or c.fraction
    if math.abs(wanted-t)>0.0000005 then
      self:SetAnimationTime(0,0,wanted)
      local got=self:GetAnimationTime(0,0)
      if not number(got) or math.abs(got-wanted)>0.00001 or self:GetAnimationSpeed(0,0)~=0 then
        release(self,s,"CONTROLLER_TIME_NOT_CONFIRMED");return
      end
      s.last=got
    end
  elseif c.phase==4 and not s.paused then
    log("RELEASE_PAUSE target="..c.target.." nativeRate=0")
  end
  s.paused=c.phase~=2
  output(c,1,progress(s))
end
function M.Install()
  if M.installed then return true end
  local D=rawget(_G,"AnimDoor")
  if not D or not D.Server or type(D.DoPlayAnimation)~="function" or type(D.Server.OnUpdate)~="function" then
    log("INSTALL_REFUSED nativeClass=UNAVAILABLE revision="..REVISION);return false
  end
  local play,update=D.DoPlayAnimation,D.Server.OnUpdate
  nativePlay=play
  D.DoPlayAnimation=function(self,direction,forceTime,useSound,customAnim,usePlayerAnim,userId)
    local ok,before=pcall(input);if not ok then before=nil end;local old=sessions[self]
    if before and not M.bridge69 then
      local seen,revision=pcall(get,"companion_69")
      if seen and revision==69 then M.bridge69=true;log("LEGACY_DOOR_MODE_SUPPORTED revision=69") end
    end
    if before and not M.bridge66 then
      local seen,revision=pcall(get,"companion_66")
      if seen and revision==REVISION then M.bridge66=true;log("BRIDGE_MATCHED revision="..REVISION.." protocol="..VERSION) end
    end
    if old then pcall(release,self,old,"REPLACED") end
    local result=play(self,direction,forceTime,useSound,customAnim,usePlayerAnim,userId)
    -- Native replacement still runs once even while cleanup is pending. It may
    -- itself establish a new native rate/clip; retry the OLD receipt afterwards,
    -- but never overwrite the old cleanup session with a new acquisition.
    if old and sessions[self]==old then
      pcall(release,self,old,"NATIVE_REPLACED")
      if sessions[self]==old then return result end
    end
    local p=rawget(_G,"g_localActor") or rawget(_G,"player")
    if before and p and p.id==userId and usePlayerAnim==true and forceTime==nil and customAnim==nil and
       (direction==1 or direction==-1) and self.inUse==1 and self.bNeedUpdate==1 and
       type(self.curAnim)=="string" and self.curAnim~="" and (self.bNoAnims==nil or self.bNoAnims==false or self.bNoAnims==0) then
      sessions[self]={before=before.request,clip=self.curAnim,direction=self.nDirection,user=userId,age=0,quiet=0}
    end
    return result
  end
  D.Server.OnUpdate=function(self,dt)
    -- No getters/tables/protected-call on ordinary inactive doors.
    local active=sessions[self]
    if active then
      local ok=pcall(preupdate,self,dt)
      if not ok then local s=sessions[self];if s then pcall(release,self,s,"SCRIPT_ERROR") end end
    end
    local result=update(self,dt)
    -- ACK2 means the ORIGINAL native update actually completed this operation,
    -- rather than that our command merely requested its final frame.
    if active and sessions[self]==active and active.finishPending and not active.cancel then
      pcall(completion,self,active)
    end
    return result
  end
  for _,name in ipairs({"OnLoad","OnReset","OnSave","OnShutDown"}) do
    local original=D[name]
    if type(original)=="function" then D[name]=function(self,...)
      local s=sessions[self];if s then pcall(release,self,s,name) end
      return original(self,...)
    end end
  end
  M.installed=true;M.revision=REVISION;log("INSTALLED revision="..REVISION.." protocol="..VERSION);return true
end
-- No scene scan or continuous bootstrap. Native class is loaded once through the
-- game's script loader, then the additive method wrappers install once.
if not rawget(_G,"AnimDoor") and Script and Script.ReloadScript then Script.ReloadScript("Scripts/Entities/Doors/AnimDoor.lua") end
M.Install()
