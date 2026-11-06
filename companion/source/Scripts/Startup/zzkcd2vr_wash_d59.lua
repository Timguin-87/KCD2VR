-- D59 experimental washing: current native WaterTubeActionTrigger only.
-- Called exclusively through the existing exact-retail native_script_t2 client.
-- No timers, actor physics, equipment writes, custom reward or custom fader.
-- The normal trigger handler and its original WashFace/3300ms/cleanup run once.
local lease = nil
local lastSpent = 0
local function integer(s)
  local n=tonumber(s)
  if not n or n~=math.floor(n) or n<1 or n>9007199254740991 then return nil end
  return n
end
local function finite(n) return type(n)=='number' and n==n and n>-1000000 and n<1000000 end
local function reply(serial,status) return 'OK|'..string.format('%.0f',serial)..'|'..status end
local function refused(reason) return nil,nil,nil,nil,reason end
local function eligible(pid,tid)
  if not System or not System.GetEntity or not player or not TriggerBase or not WaterTubeActionTrigger then return refused('ENVIRONMENT') end
  local p=System.GetEntity(pid)
  local e=System.GetEntity(tid)
  if p~=player or not p.actor or not e or not e.GetClass or e:GetClass()~='WaterTubeActionTrigger' then return refused('IDENTITY') end
  -- Exact inherited retail methods are used; another trigger with a copied name
  -- or an overridden action dispatcher cannot borrow this qualification.
  if e.IsEnabled~=WaterTubeActionTrigger.IsEnabled or e.CanWash~=WaterTubeActionTrigger.CanWash or
     e.GetActions~=TriggerBase.GetActions or not e.GetLinkedSmartObject then return refused('METHOD') end
  local click=e.Properties and e.Properties.Click
  if not click or click.esActionType~='Animation' or
     string.lower(tostring(click.sAction))~='washface' then return refused('ACTION') end
  local so=e:GetLinkedSmartObject()
  -- Retail ActionTrigger:IsUsable/IsEnabled explicitly accept an unlinked
  -- Animation and ReportUse passes INVALID_WUID in that case. Preserve that
  -- native branch, while retaining exact identity for any link that exists.
  if so and (not integer(so.id) or not so.smartObject or not so.smartObject.__this) then return refused('LINK') end
  if not e:IsUsable(p) then return refused('IS_USABLE') end
  local enabled=e:IsEnabled(p)
  if not enabled then return refused('IS_ENABLED') end
  -- The actual completed script descriptor is the permission, including its
  -- quest/item/approach/dirt checks. No hard-coded generic "use" replacement.
  local prior,priorInst=output,inst
  local ok,rows=pcall(e.GetActions,e,p,false)
  output,inst=prior,priorInst
  if not ok or type(rows)~='table' then return refused('DESCRIPTOR_QUERY') end
  for _,row in pairs(rows) do
    if type(row)=='table' and row.action=='trigger_use' and row.enabled==true and
       row.func==TriggerBase.OnUsed and row.interactionName==inr_scriptTrigger then
      if player~=p or System.GetEntity(pid)~=p or System.GetEntity(tid)~=e or
         e:GetLinkedSmartObject()~=so or e.GetActions~=TriggerBase.GetActions then return refused('IDENTITY_CHANGED') end
      return p,e,so,row
    end
  end
  return refused('TRIGGER_USE_DESCRIPTOR')
end
local function bounds(e)
  if not e.GetWorldBBox then return nil end
  local lo,hi=e:GetWorldBBox()
  if type(lo)~='table' or type(hi)~='table' then return nil end
  for _,a in ipairs({'x','y','z'}) do
    if not finite(lo[a]) or not finite(hi[a]) or hi[a]-lo[a]<.03 or hi[a]-lo[a]>1.6 then return nil end
  end
  return lo,hi
end
local function sameBox(a,b)
  for _,k in ipairs({'x','y','z'}) do if math.abs(a[k]-b[k])>.03 then return false end end
  return true
end
function kcd2vr_wash_d59(packet)
  if type(packet)~='string' or #packet>128 then return 'ERR|PACKET' end
  local op,s,p,t=string.match(packet,'^(%u+)|(%d+)|(%d+)|(%d+)$')
  local serial,pid,tid=integer(s),integer(p),integer(t)
  if not serial or not pid or not tid then return 'ERR|IDENTITY' end
  if op=='CANCEL' then
    if lease and lease.serial==serial and lease.pid==pid and lease.tid==tid then lease=nil end
    return reply(serial,'CANCELLED')
  end
  if op~='PROBE' and op~='COMMIT' then return reply(serial,'OP_REFUSED') end
  if serial<=lastSpent then return reply(serial,'SPENT') end
  local p,e,so,row,reason=eligible(pid,tid)
  if not p then return reply(serial,'NATIVE_REFUSED_'..tostring(reason)) end
  local lo,hi=bounds(e)
  if not lo then return reply(serial,'VOLUME_UNAVAILABLE') end
  if op=='PROBE' then
    if lease and lease.serial==serial and
       (lease.player~=p or lease.entity~=e or lease.smartObject~=so or not sameBox(lease.lo,lo) or not sameBox(lease.hi,hi)) then
      lease=nil
      return reply(serial,'IDENTITY_CHANGED')
    end
    lease={serial=serial,pid=pid,tid=tid,player=p,entity=e,smartObject=so,
      lo={x=lo.x,y=lo.y,z=lo.z},hi={x=hi.x,y=hi.y,z=hi.z}}
    return reply(serial,string.format('READY|%.0f|%.0f|%.6f|%.6f|%.6f|%.6f|%.6f|%.6f',
      tid,so and so.id or 0,lo.x,lo.y,lo.z,hi.x,hi.y,hi.z))
  end
  if not lease or lease.serial~=serial or lease.pid~=pid or lease.tid~=tid or
     lease.player~=p or lease.entity~=e or lease.smartObject~=so or
     not sameBox(lease.lo,lo) or not sameBox(lease.hi,hi) then return reply(serial,'LEASE_REFUSED') end
  if player~=p or System.GetEntity(pid)~=p or System.GetEntity(tid)~=e or
     e:GetLinkedSmartObject()~=so or e.GetActions~=TriggerBase.GetActions or
     e.IsEnabled~=WaterTubeActionTrigger.IsEnabled or row.func~=TriggerBase.OnUsed then
    lease=nil
    return reply(serial,'IDENTITY_CHANGED')
  end
  -- Revalidate above immediately before the original contextual callback.
  -- Consume BEFORE pcall: an exception after a native side effect cannot repeat.
  lastSpent=serial
  lease=nil
  local ok=pcall(row.func,e,p,row.slotId)
  return reply(serial,ok and 'DISPATCHED' or 'NATIVE_UNKNOWN')
end
