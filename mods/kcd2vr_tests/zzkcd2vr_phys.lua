-- H41 bounded developer-console evidence only. NOT controller/hand physics.
-- No automatic mutation, synthetic hand point, loop, constraint, fall/ragdoll,
-- forced NPC reaction, or collision/damage claim. Push/Pull is ONE original
-- Entity:AddImpulse command; normal return does not prove movement/acceptance.
KCD2VR_Phys=KCD2VR_Phys or {}
local P=KCD2VR_Phys
P.version='0.3-H41-ONE-SHOT-TEST'
P.radius=4
local function finite(v)return type(v)=='number'and v==v and v>-math.huge and v<math.huge end
local function vector(p)return type(p)=='table'and finite(p.x)and finite(p.y)and finite(p.z)end
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s)if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,'KCD2VR_PHYS TEST_ONLY '..tostring(s))end end
local function actualPlayer()
 local a=rawget(_G,'player')or rawget(_G,'g_localActor')
 if not a or a.id==nil or System.GetEntity(a.id)~=a then return nil end
 return a
end
local function nearest(a)
 local p=a:GetWorldPos();if not vector(p)then return nil end
 local best,dist
 for _,e in pairs(System.GetEntitiesInSphere(p,4)or{})do
  if e~=a and type(e)=='table'and e.id~=nil and(e.actor or e.soul)and type(e.GetWorldPos)=='function'then
   local q=e:GetWorldPos()
   if vector(q)then local x,y,z=q.x-p.x,q.y-p.y,q.z-p.z;local d=x*x+y*y+z*z
    if d<=16 and(not dist or d<dist)then best,dist=e,d end
   end
  end
 end
 return best,p
end
local function invoke(toward,scale)
 local magnitude=scale==nil and 4 or tonumber(scale)
 if not finite(magnitude)or magnitude<=0 or magnitude>20 then return false,'magnitude must be finite in (0,20]'end
 local a=actualPlayer();if not a then return false,'current player unavailable'end
 local e,p=nearest(a);if not e then return false,'no current character within4m'end
 local id,actor,soul=e.id,e.actor,e.soul
 local q=e:GetWorldPos();if not vector(q)then return false,'target point unavailable'end
 local x,y,z=q.x-p.x,q.y-p.y,q.z-p.z;local n=math.sqrt(x*x+y*y+z*z)
 if n<.0001 then return false,'direction unavailable'end
 local sign=toward and -1 or 1;local d={x=x/n*sign,y=y/n*sign,z=z/n*sign}
 if actualPlayer()~=a or System.GetEntity(id)~=e or e.actor~=actor or e.soul~=soul then return false,'original character replaced'end
 -- Official existing Entity binding: part,worldPoint,direction,linearImpulse,scale.
 -- Entity/body eligibility remains inside the ORIGINAL native method.
 local ok,why=pcall(e.AddImpulse,e,-1,q,d,magnitude,1)
 local result=ok and 'COMMAND_RETURNED movement=UNVERIFIED'or'COMMAND_UNKNOWN noRetry=1'
 log((toward and'pull'or'push')..' '..result..' target='..tostring(id))
 return ok,result
end
function P.Push(scale)local ok,r,s=pcall(invoke,false,scale);return ok and r or false,ok and s or'query unknown'end
function P.Pull(scale)local ok,r,s=pcall(invoke,true,scale);return ok and r or false,ok and s or'query unknown'end
function P.Stats()
 local ok,result=pcall(function()
  local a=actualPlayer();if not a then return false end;local e=nearest(a)
  if not e or System.GetEntity(e.id)~=e then return false end
  local okv,v=pcall(e.GetVelocity,e);local oks,s=pcall(e.GetPhysicalStats,e)
  log('stats target='..tostring(e.id)..' velocityRead='..tostring(okv and vector(v))..' statsRead='..tostring(oks and type(s)=='table'))
  return true
 end)
 return ok and result or false
end
local function unavailable()
 log('UNAVAILABLE: real completed-hand+original current body/part owner and confirmed native release provider required; no developer loop/constraint/ragdoll fallback')
 return false,'not implemented as a hand feature'
end
P.Drag=unavailable;P.Grab=unavailable;P.Constraint=unavailable
P.Fall=unavailable;P.Ragdoll=unavailable;P.StandUp=unavailable
function P.Stop()return false,'this module owns no loops/constraints; old untracked tests require save reload'end
log('loaded '..P.version..' no automatic action; console only')
