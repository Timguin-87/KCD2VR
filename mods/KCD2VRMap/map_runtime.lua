-- M41 static paper ONLY. Native C++ presenter owns fresh pose/visibility.
-- No Inventory, ItemUse, native reading action, animation, UI surface or timer.
-- Loading this Startup must preserve the independent finite knife bootstrap.
for _,path in ipairs({"Scripts/KCD2VR/native_supply_t2.lua","Scripts/KCD2VR/map_catalog_static_m41.lua"})do
 pcall(function()Script.ReloadScript(path)end)
end
local C=KCD2VRMapCatalogM39
-- Same-VM reload reuses exact original entity records and listener object.
-- An unknown older module remains its owner until a normal game/VM restart;
-- never replace it and then adopt its world objects merely by name.
local previous=rawget(_G,"KCD2VRMap")
if previous and (type(previous)~="table"or previous.protocolM44~="static-paper-m44.1")then return end
local M=previous or{entities={},world=nil,serial=-1,protocolM44="static-paper-m44.1"}
KCD2VRMap=M
local names={"KCD2VR_StaticPaper_M41"}
local function current(actor,epoch)
 local p=rawget(_G,"g_localActor")or rawget(_G,"player")
 if not p or not p.id or System.GetEntity(p.id)~=p then return nil end
 if actor and System.GetEntity(actor)~=p then return nil end
 if M.endedActor==p and (not epoch or not M.endedEpoch or epoch<=M.endedEpoch)then return nil end
 return p
end
local function region()
 local s=System.GetCVar("sv_map")
 if type(s)~="string"then return nil end
 for _,r in ipairs({"trosecko","kutnohorsko","klaster"})do if s:lower():find(r,1,true)then return r end end
end
local function pixels(r,x,y)
 local l=C and C.levels[r];if not l then return nil end
 local a=-l.rotation*math.pi/180;local c,s=math.cos(a),math.sin(a)
 local xs,ys={0,c,c-s,-s},{0,s,s+c,c}
 local minx,maxx=math.min(unpack(xs)),math.max(unpack(xs));local miny,maxy=math.min(unpack(ys)),math.max(unpack(ys))
 local sx,sy=l.tiles[1]*2048/(maxx-minx),l.tiles[2]*2048/(maxy-miny)
 local nx,ny=(x-l.world[1])/l.world[3],(l.level[2]-y-l.world[2])/l.world[4]
 return sx*c*nx-sy*s*ny-minx*sx-l.crop[1]*2048,sx*s*nx+sy*c*ny-miny*sy-l.crop[2]*2048
end
local function polygon(x,y,xy)
 local inside=false;local j=#xy-1
 for i=1,#xy,2 do
  local ax,ay,bx,by=xy[i],xy[i+1],xy[j],xy[j+1]
  if(ay>y)~=(by>y)and x<(bx-ax)*(y-ay)/(by-ay)+ax then inside=not inside end;j=i
 end
 return inside
end
local function choose(p)
 local r=region();local pos=p:GetWorldPos();if not r or not pos then return nil end
 local x,y=pixels(r,pos.x,pos.y);if not x then return nil end
 local best=nil
 for _,m in ipairs(C.maps)do if m.region==r then
  local a=m.active;local dx,dy=(x-a[1])/a[3],(y-a[2])/a[4]
  local inside=m.ellipse and((dx-.5)^2+(dy-.5)^2<=.25)or(not m.ellipse and dx>=0 and dx<=1 and dy>=0 and dy<=1)
  if inside and m.inside then
   inside=false
   for _,area in ipairs((C.inside[r]or{})[m.location]or{})do
    if(area.height<=0 or(pos.z>=area.z and pos.z<=area.z+area.height))and polygon(pos.x,pos.y,area.xy)then inside=true;break end
   end
  end
  if inside and(not best or m.priority>best.priority or(m.priority==best.priority and a[3]*a[4]<best.active[3]*best.active[4]))then best=m end
 end end
 if not best then for _,m in ipairs(C.maps)do if m.region==r and m.default then best=m;break end end end
 return best and best.name or r
end
-- D64: ordinary POIs are one cached native mesh in slot1, not one actor per
-- marker. Slot2 is a small current-player glyph; all slots share paper visibility.
-- Public script methods are checked on this exact owned no-save utility only.
local function sheet(art)
 for _,m in ipairs(C.maps)do if m.name==art then return m end end
 local l=C.levels[art]
 if l then return {name=art,region=art,rect={0,0,l.crop[3]*2048,l.crop[4]*2048}}end
end
local function finiteD64(v)return type(v)=="number"and v==v and math.abs(v)<100000 end
local function hideMarkerD64(s)
 -- Hidden geometry must remain within the paper bounds, not a kilometre away.
 s.playerPoseD81=nil
 pcall(function()s.entity:DrawSlot(2,0)end)
 pcall(function()s.entity:SetSlotPos(2,{x=0,y=0,z=0})end)
end
local function markerPose(s,p)
 if not s.markersD64 then return false end
 local m=sheet(s.art);if not m then return false end
 local pos=p:GetWorldPos();if not pos or not finiteD64(pos.x)or not finiteD64(pos.y)then return false end
 local x,y=pixels(m.region,pos.x,pos.y)
 if not finiteD64(x)or not finiteD64(y)then return false end
 local r=m.rect;local u,v=(x-r[1])/r[3],(y-r[2])/r[4]
 -- Off-map positions do not clamp to a misleading map edge.
 if u<0 or u>1 or v<0 or v>1 then return false end
 local a=r[3]/r[4];local w=.55*math.min(1,a);local h=.55*math.min(1,1/a)
 local at={x=(u-.5)*w,y=0,z=(.5-v)*h}
 local old=s.playerPoseD81
 if old and old.x==at.x and old.y==at.y and old.z==at.z then return true end
 s.playerPoseD81=nil -- do not cache a desired pose before the engine accepts it
 s.entity:SetSlotPos(2,at)
 local read=s.entity:GetSlotPos(2)
 local accepted=read and finiteD64(read.x)and finiteD64(read.y)and finiteD64(read.z)and math.abs(read.x-at.x)<.0005 and math.abs(read.y-at.y)<.0005 and math.abs(read.z-at.z)<.0005
 if not accepted then return false end
 s.entity:DrawSlot(2,1);s.playerPoseD81=at
 return true
end
local function markers(s,p)
 local e=s.entity
 if type(e.LoadObject)~="function"or type(e.SetSlotPos)~="function"or type(e.GetSlotPos)~="function"or type(e.DrawSlot)~="function"then return end
 local ok=pcall(function()
  if s.markerArtD64~=s.art then
   s.playerPoseD81=nil
   e:LoadObject(1,"Objects/kcd2vr/mapmarkers64/"..s.art..".cgf")
   e:LoadObject(2,"Objects/kcd2vr/mapmarkers64/player.cgf")
   s.markerArtD64=s.art;s.markersD64=true
  end
  if not markerPose(s,p)then hideMarkerD64(s)end
 end)
 if not ok then hideMarkerD64(s);s.markersD64=false end -- paper's existing owner remains independent
end
local function originalState(s)
 if not s or not s.entity or not s.entity.id then return nil end
 local ok,e=pcall(System.GetEntity,s.entity.id)
 if not ok then return nil end
 return e==s.entity -- false is a POSITIVE retired/replaced entity, nil is unknown
end
local function hiddenState(s)
 local ok,h=pcall(function()return s.entity:IsHidden()end)
 if not ok then return nil end
 return h and true or false
end
local function retire()
 M.retiringM44=true
 local complete=true
 for name,s in pairs(M.entities)do
  local state=originalState(s)
  if state==false then M.entities[name]=nil
  elseif state==nil then complete=false
  else
   pcall(function()s.entity:Hide(1)end)
   if hiddenState(s)~=true then complete=false
   else
    -- RemoveEntity may be deferred or may apply and then fault. Only native
    -- registry readback retires this exact original table/opaque entity ID.
    pcall(System.RemoveEntity,s.entity.id)
    if originalState(s)==false then M.entities[name]=nil else complete=false end
   end
  end
 end
 if complete then M.world=nil;M.epoch=nil;M.retiringM44=false end
 return complete
end
local clearQuestsD75
function kcd2vr_map_t3_ready(packet)
 local protocol=type(packet)=="string"and packet:sub(1,6)=="R3N44|"and"R3N44"or"R3"
 local native=protocol=="R3N44"
 if type(packet)~="string"or#packet>128 then return protocol.."|REFUSED|MALFORMED"end
 local serial,epoch,actor,helper=packet:match("^"..protocol.."|(%d+)|(%d+)|(%d+)|(%d+)$")
 serial,epoch,actor,helper=tonumber(serial),tonumber(epoch),tonumber(actor),tonumber(helper)
 if not serial or serial<1 or serial>9007199254740991 or not epoch or epoch<1 or not actor or actor<1 or actor>4294967295 or not helper or helper<0 or helper>4294967295 then return protocol.."|REFUSED|MALFORMED"end
 if serial==M.serial and M.backendNativeM44~=nil and M.backendNativeM44~=native then return protocol.."|REFUSED|BACKEND_MISMATCH"end
 if serial<M.serial then return protocol.."|REFUSED|STALE_INTENT"end
 local p=current(actor,epoch);if not p then return protocol.."|REFUSED|CURRENT_ACTOR_MISMATCH"end
 if M.world==p.id and M.epoch and epoch<M.epoch then return protocol.."|REFUSED|STALE_BODY"end
 local ok,why=pcall(function()
  if M.retiringM44 or M.world~=p.id or M.epoch~=epoch then
   if not retire()then error("HELPER_RETIRE_PENDING")end;M.world=p.id;M.epoch=epoch
  end
  local art=choose(p);if not art then error("NO_CURRENT_MAP")end
  for _,name in ipairs(names)do
   local state=M.entities[name];local e=System.GetEntityByName(name)
   if state and e~=state.entity then error("HELPER_IDENTITY_CHANGED")end
   if not state then
    if e then error("FOREIGN_NAME_COLLISION")end
    -- Retail registers ENTITY_FLAG_NO_SAVE (WHGame1451AC7, value0x8000).
    -- The utility is never a durable inventory/world object saved into the game.
    local noSave=rawget(_G,"ENTITY_FLAG_NO_SAVE")
    local client=rawget(_G,"ENTITY_FLAG_CLIENT_ONLY")
    if type(noSave)~="number"or noSave<=0 then error("NATIVE_NO_SAVE_FLAG_UNAVAILABLE")end
    if type(client)~="number"or client<0 then client=0 end
    -- INVESTIGATE_D67R1 MAP_HELPER_RENDER: the supplied Scripts.pak BasicEntity
    -- SetupModel physicalizes only when Physics.bPhysicalize == true; the
    -- explicit false below already disables that path. POIs are one cached
    -- slot1 mesh, not one entity per marker. Trace this helper's slot render
    -- nodes/material passes before blaming physics, marker count or a guessed
    -- no-shadow material bit for the recorded map-open performance loss.
    -- See repair_evidence/d67r1/map/MAP_STATIC_INVESTIGATION.txt.
    e=System.SpawnEntity({class="BasicEntity",name=name,flags=noSave+client,position={x=0,y=0,z=-1000},
     properties={object_Model="Objects/kcd2vr/staticmap40/"..art..".cgf",bSaved_by_game=false,bCanTriggerAreas=false,
      Physics={bPhysicalize=false,bRigidBody=false,bPushableByPlayers=false},MultiplayerOptions={bNetworked=false}}})
    if not e then error("STATIC_HELPER_CREATE_FAILED")end
    state={name=name,entity=e,art=art};M.entities[name]=state -- record receipt before any fallible call
    e:Hide(1)
   elseif state.art~=art then
    e:Hide(1);clearQuestsD75(state);e:LoadObject(0,"Objects/kcd2vr/staticmap40/"..art..".cgf");state.art=art
   end
  end
  if serial~=M.serial or native~=M.backendNativeM44 then
   for _,s in pairs(M.entities)do clearQuestsD75(s)end
  end
  M.serial=serial;M.backendNativeM44=native
  for _,s in pairs(M.entities)do markers(s,p)end
 end)
 if not ok then if not M.retiringM44 then pcall(retire)end;return protocol.."|REFUSED|"..tostring(why):sub(1,120)end
 local owned=M.entities[names[1]]
 if helper==0 then return string.format(protocol.."|41.0-static-paper|%u|%.0f|%.0f|1|1|HELPER_PENDING_ID|%s",actor,epoch,serial,owned.art)end
 -- Echo the native integer supplied by the current presenter only after the
 -- engine resolves it to OUR original Lua entity. Never coerce an opaque e.id.
 if System.GetEntity(helper)~=owned.entity or System.GetEntityByName(owned.name)~=owned.entity then return protocol.."|REFUSED|HELPER_IDENTITY_MISMATCH"end
 if System.LogAlways then System.LogAlways("KCD2VR_MAP M41 helperOwned id="..helper.." art="..owned.art.." nativeSlotReadback=REQUIRED rendered=UNVERIFIED")end
 return string.format(protocol.."|41.0-static-paper|%u|%.0f|%.0f|1|1|HELPER_READY|%s|%u",actor,epoch,serial,owned.art,helper)
end
function kcd2vr_map_markers_d64(packet)
 local protocol=type(packet)=="string"and packet:sub(1,7)=="D64N44|"and"D64N44"or"D64"
 local native=protocol=="D64N44"
 if type(packet)~="string"or#packet>128 then return protocol.."|REFUSED"end
 local serial,epoch,actor,helper=packet:match("^"..protocol.."|(%d+)|(%d+)|(%d+)|(%d+)$")
 serial,epoch,actor,helper=tonumber(serial),tonumber(epoch),tonumber(actor),tonumber(helper)
 if M.backendNativeM44~=native then return protocol.."|BACKEND"end
 if not serial or serial~=M.serial or epoch~=M.epoch then return protocol.."|STALE"end
 local p=current(actor,epoch);local s=M.entities[names[1]]
 if not p or not s or System.GetEntity(helper)~=s.entity or System.GetEntityByName(s.name)~=s.entity then return protocol.."|OWNER"end
 local ok,shown=pcall(function()return native or not s.entity:IsHidden()end)
 if not ok or not shown then return protocol.."|HIDDEN"end
 local updated=false;ok=pcall(function()updated=markerPose(s,p)end)
 if not ok or not updated then hideMarkerD64(s)end
 return ok and updated and protocol.."|PLAYER"or protocol.."|UNAVAILABLE"
end
-- Exact retail C4D664 + HUD MEnum.GetTrackingIconNameByTypeAndTrackingId.
-- Added to the existing paper helper; no per-marker entities/physics/Flash UI.
local colour={[-1]="none",[0]="red",[1]="yellow",[2]="blue",[3]="orange",[4]="pink",[5]="violet"}
local family={[1]="main",[2]="side",[3]="activity",[86]="racing"}
local available={main={none=true,red=true,yellow=true,blue=true},side={none=true,red=true,yellow=true,blue=true},
 activity={none=true,red=true,yellow=true,blue=true,orange=true,pink=true,violet=true},racing={none=true,red=true}}
local function questArt(kind,tracking)
 local f,c=family[kind],colour[tracking]
 if not f or not c or not available[f][c]then return nil end
 return "quest_"..f.."_"..c
end
clearQuestsD75=function(s)
 if not s.questSlotsD75 or originalState(s)~=true then return end
 for slot in pairs(s.questSlotsD75)do
  local prior=s.questStateD80 and s.questStateD80[slot]
  if not prior or prior.shown~=false then
   local ok=pcall(function()s.entity:DrawSlot(slot,0);s.entity:SetSlotPos(slot,{x=0,y=0,z=0})end)
   if s.questStateD80 then
    s.questStateD80[slot]=ok and {key=prior and prior.key,shown=false,pos={x=0,y=0,z=0}} or nil
   end
  end
 end
end
local function questPositionD75(s,x,y)
 local m=sheet(s.art);if not m or region()~=m.region then return nil end
 local px,py=pixels(m.region,x,y);if not finiteD64(px)or not finiteD64(py)then return nil end
 local r=m.rect;local u,v=(px-r[1])/r[3],(py-r[2])/r[4]
 if u<0 or u>1 or v<0 or v>1 then return nil end
 local a=r[3]/r[4];return {x=(u-.5)*.55*math.min(1,a),y=0,z=(.5-v)*.55*math.min(1,1/a)}
end
function kcd2vr_map_quests_d75(packet)
 if type(packet)~="string"or#packet>4096 then return "Q75|MALFORMED"end
 local serial,epoch,actor,helper,known,pin,rows=packet:match("^Q75|(%d+)|(%d+)|(%d+)|(%d+)|([01])|([^|]*)|([^|]*)$")
 serial,epoch,actor,helper=tonumber(serial),tonumber(epoch),tonumber(actor),tonumber(helper)
 if not serial or serial~=M.serial or epoch~=M.epoch or M.backendNativeM44~=false then return "Q75|STALE"end
 local p=current(actor,epoch);local s=M.entities[names[1]]
 if not p or not s or System.GetEntity(helper)~=s.entity or System.GetEntityByName(s.name)~=s.entity then return "Q75|OWNER"end
 local ok,shown=pcall(function()return not s.entity:IsHidden()end)
 if not ok or not shown then return "Q75|HIDDEN"end
 local list={};local count=0
 -- Parse and validate all rows before touching owned presentation. Never accept
 -- asset paths from the packet; only the exact native enum+colour lookup above.
 if known=="0"and rows~=""then clearQuestsD75(s);return "Q75|MALFORMED"end
 if rows:find(";;",1,true)or rows:sub(1,1)==";"then clearQuestsD75(s);return "Q75|MALFORMED"end
 if known=="1"then
  for row in rows:gmatch("([^;]+);")do
   count=count+1;if count>16 then clearQuestsD75(s);return "Q75|LIMIT"end
   local k,c,o,x,y=row:match("^(-?%d+),(-?%d+),(-?%d+),(-?[%d%.]+),(-?[%d%.]+)$")
   k,c,o,x,y=tonumber(k),tonumber(c),tonumber(o),tonumber(x),tonumber(y)
   if not k or not c or not o or not finiteD64(x)or not finiteD64(y)then clearQuestsD75(s);return "Q75|MALFORMED"end
   local art=questArt(k,c);local pos=questPositionD75(s,x,y)
   if art and pos then list[#list+1]={art=art,pos=pos,objective=o,key=k..":"..c..":"..o}end
  end
  if rows~=""and rows:sub(-1)~=";"then clearQuestsD75(s);return "Q75|MALFORMED"end
 end
 if pin~=""then
  local x,y=pin:match("^(-?[%d%.]+),(-?[%d%.]+)$");x,y=tonumber(x),tonumber(y)
  if not finiteD64(x)or not finiteD64(y)then clearQuestsD75(s);return "Q75|MALFORMED"end
  local pos=questPositionD75(s,x,y);if pos then list[#list+1]={art="checkpoint",pos=pos,key="pin"}end
 end
 -- Stable semantic keys survive compass enumeration reordering. Several
 -- locations can share an objective; disambiguate those by copied coordinates.
 local counts={};for _,q in ipairs(list)do counts[q.key]=(counts[q.key]or 0)+1 end
 local seen={};for _,q in ipairs(list)do
  if counts[q.key]>1 then q.key=q.key..":"..string.format("%.9g:%.9g",q.pos.x,q.pos.z)end
  seen[q.key]=(seen[q.key]or 0)+1;q.key=q.key..":"..seen[q.key]
 end
 ok=pcall(function()
  s.questSlotsD75=s.questSlotsD75 or{};s.questStateD80=s.questStateD80 or{}
  local used={}
  -- Reserve all matching slots before assigning new objectives. A new first
  -- row must not steal a later row's existing slot.
  for _,q in ipairs(list)do
   for slot=3,19 do local old=s.questStateD80[slot]
    if not used[slot]and old and old.key==q.key then q.slot=slot;used[slot]=true;break end
   end
  end
  for _,q in ipairs(list)do
   if not q.slot then for slot=3,19 do if not used[slot]then q.slot=slot;used[slot]=true;break end end end
   local slot=q.slot;local old=s.questStateD80[slot]
   if s.questSlotsD75[slot]~=q.art then
    -- Invalidate the receipt BEFORE any fallible native operation.
    s.questStateD80[slot]=nil;s.questSlotsD75[slot]=false
    s.entity:DrawSlot(slot,0);s.entity:SetSlotPos(slot,{x=0,y=0,z=0})
    s.entity:LoadObject(slot,"Objects/kcd2vr/mapquests75/"..q.art..".cgf")
    s.entity:DrawSlot(slot,0);s.questSlotsD75[slot]=q.art;old=nil
   end
   local samePosition=old and old.pos and old.pos.x==q.pos.x and old.pos.y==q.pos.y and old.pos.z==q.pos.z
   if not samePosition or not old or not old.shown then
    if M.serial~=serial or M.epoch~=epoch or M.backendNativeM44~=false or
       M.entities[names[1]]~=s or current(actor,epoch)~=p or originalState(s)~=true then error("QUEST_OWNER_CHANGED")end
    s.questStateD80[slot]=nil
    if not samePosition then s.entity:SetSlotPos(slot,q.pos)end
    if not old or not old.shown then s.entity:DrawSlot(slot,1)end
   end
   s.questStateD80[slot]={key=q.key,pos=q.pos,shown=true}
  end
  for slot in pairs(s.questSlotsD75)do
   local old=s.questStateD80[slot]
   if not used[slot]and(not old or old.shown~=false)then
    s.questStateD80[slot]=nil
    s.entity:DrawSlot(slot,0);s.entity:SetSlotPos(slot,{x=0,y=0,z=0})
    s.questStateD80[slot]={key=old and old.key,shown=false,pos={x=0,y=0,z=0}}
   end
  end
 end)
 if not ok then clearQuestsD75(s);return "Q75|FAULT"end
 return "Q75|LIVE|"..#list
end

function kcd2vr_map_m31_intent(packet)
 if type(packet)~="string"or#packet>128 then return "I31|REFUSED"end
 local serial,on=packet:match("^I31|(%d+)|([01])$");serial=tonumber(serial)
 if not serial or serial<M.serial or serial>9007199254740991 then return "I31|REFUSED"end
 if on=="1"then return "I31|REFUSED"end -- C++ alone grants visibility
 local closed=true
 for _,s in pairs(M.entities)do
  local state=originalState(s)
  if state==nil then closed=false
  elseif state==true then pcall(function()s.entity:Hide(1)end);if hiddenState(s)~=true then closed=false end end
 end
 M.serial=serial;return string.format("I31|%.0f|%s",serial,closed and"CLOSED"or"PENDING")
end
function M.WorldEnded()
 local p=rawget(_G,"g_localActor")or rawget(_G,"player")
 if p then M.endedActor=p;M.endedEpoch=M.epoch or M.endedEpoch end
 pcall(retire)
end
function M.WorldStarted()M.endedActor=nil;M.endedEpoch=nil end
if UIAction and UIAction.RegisterEventSystemListener then
 -- Retain each successful registration independently if the second one faults.
 if not M.endedListenerM44 then M.endedListenerM44=pcall(function()UIAction.RegisterEventSystemListener(M,"","OnGameplayEnded","WorldEnded")end)end
 if not M.startedListenerM44 then M.startedListenerM44=pcall(function()UIAction.RegisterEventSystemListener(M,"","OnGameplayStarted","WorldStarted")end)end
end

-- Preserve independent native inventory-document/book forwarding. Loading these
-- modules defines their event-qualified backends; static utility never calls
-- the default private movie Feed/Tick or any UI surface renderer.
for _,path in ipairs({"Scripts/KCD2VR/map_navigation_t2.lua","Scripts/KCD2VR/map_native_live_h28.lua","Scripts/KCD2VR/map_native_read_h25.lua"})do
 pcall(function()Script.ReloadScript(path)end)
end
