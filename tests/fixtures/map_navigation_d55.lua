-- Own material-bound paper navigation. No minimap module, screen UI or input map.
-- Geometry/animation stays on the existing left-hand parchment. The private GFx
-- owns persistent tile holders and a bounded persistent vector glyph pool.
local M=KCD2VRMap
if not M then return end
local function createNavigation(backend)
local UI="KCD2VRMapT2"
local PREFIX="libs/ui/textures/kcd2vrmapt2/"
local MAX_MARKERS=96
local SPAN=800 -- closest local view, full width in world metres
-- Same linear-light neutral128 pigment modulation as the detailed map images.
-- Native parchment SNDUVS multiplies paper albedo by4*sampled ink color.
local INK={player=0x510E0B,waypoint=0x0B2E53,quest=0x6F4D0C,poi=0x322519}
M.feedGeneration=tonumber(M.feedGeneration) or 1
local N={activation=-1,lastFeed=-100,lastRender=-100,markers={},waypoint=nil,status=0,open=false}

local function finite(x) return type(x)=="number" and x==x and x>-20000 and x<20000 end
local function clock()
 local ok,t=pcall(function() return System.GetCurrTime() end)
 if ok and type(t)=="number" and t==t and t>=0 then return t end
 local t=rawget(_G,"_time")
 return type(t)=="number" and t==t and t>=0 and t or nil
end
local function split(s,delimiter,cap)
 local out={};local p=1
 while true do
  local a,b=string.find(s,delimiter,p,true)
  out[#out+1]=string.sub(s,p,a and a-1 or -1)
  if #out>cap then return nil end
  if not a then return out end
  p=b+1
 end
end
local function ui(name,...)
 if backend then return backend.Call(name,...) end
 if not UIAction or not UIAction.CallFunction then return false end
 local args={...};local count=select("#",...)
 local ok,result=pcall(function() return UIAction.CallFunction(UI,0,name,unpack(args,1,count)) end)
 if not ok or result==false then N.surfaceRefused=true;return false end
 return true
end
local function visible()
 if backend then return backend.Visible() end
 if M.awaitStart or not M.world or not M.entities then return false end
 for _,s in pairs(M.entities) do
  local ok,h=pcall(function() return s.entity:IsHidden() end)
  if ok and not h then return true end
 end
 return false
end
local function player()
 local p=rawget(_G,"g_localActor") or rawget(_G,"player")
 return p and p.id and p.inventory and p or nil
end
local function region()
 -- sv_map is the qualified native string CVar (WH E40496/B8F01C). Its
 -- requested map is admitted only with current live actor/world/hand ownership.
 -- mapname is NOT a qualified CVar; previous-level tokens are never a fallback.
 local ok,s=pcall(function() return System.GetCVar("sv_map") end)
 if ok and type(s)=="string" then
  s=string.lower(s)
  if string.find(s,"trosecko",1,true) then return "trosecko" end
  if string.find(s,"kutnohorsko",1,true) then return "kutnohorsko" end
  if string.find(s,"klaster",1,true) then return "klaster" end
 end
 return nil
end
local function metadata()
 local p
 if backend then p=backend.Player and backend.Player() else p=not M.awaitStart and player() or nil end
 if not p or (not backend and M.world~=p.id) then return nil end
 local ok,pos,ang=pcall(function() return p:GetWorldPos(),p:GetWorldAngles() end)
 local r=region()
 if not ok or not pos or not ang or not r or not finite(pos.x) or not finite(pos.y) or not finite(pos.z) or not finite(ang.z) then return nil end
 local token=tostring(p.id):gsub("[^%w_%.:%-]",""):sub(1,64)
 if token=="" then return nil end
 return {token=token,region=r,x=pos.x,y=pos.y,z=pos.z,yaw=ang.z}
end
local function project(r,x,y)
 if not finite(x) or not finite(y) then return nil end
 if r=="trosecko" then return (x-496)/2652,(4096-y-704)/2652 end
 if r=="kutnohorsko" then
  local nx,ny=x/4096,(4096-y)/4096
  return (nx+ny)*.5,(3*(-nx+ny+1)-1)/5
 end
 return nil
end

-- M39: exact native level transform + unpadded LocalMap table rectangles.
-- Native ApseMap.gfx GlobalMapData.SetData/MEnum.GetMapFromWorldCoord and
-- LocalMap.SetData/OnStreamed supply this projection and independent tile scales.
local C39=not backend and rawget(_G,"KCD2VRMapCatalogM39") or nil
if C39 then MAX_MARKERS=256 end
local TILES39=20
local function pixels39(r,x,y)
 local l=C39 and C39.levels[r];if not l or not finite(x) or not finite(y)then return nil end
 local rot=-l.rotation*math.pi/180;local c,s=math.cos(rot),math.sin(rot)
 local xs={0,c,c-s,-s};local ys={0,s,s+c,c}
 local minx,maxx=math.min(unpack(xs)),math.max(unpack(xs))
 local miny,maxy=math.min(unpack(ys)),math.max(unpack(ys))
 local sx=l.tiles[1]*2048/(maxx-minx);local sy=l.tiles[2]*2048/(maxy-miny)
 local nx=(x-l.world[1])/l.world[3];local ny=(l.level[2]-y-l.world[2])/l.world[4]
 return sx*c*nx-sy*s*ny-minx*sx-l.crop[1]*2048,
        sx*s*nx+sy*c*ny-miny*sy-l.crop[2]*2048
end
local function polygon39(x,y,xy)
 local inside=false;local j=#xy-1
 for i=1,#xy,2 do
  local ax,ay,bx,by=xy[i],xy[i+1],xy[j],xy[j+1]
  if (ay>y)~=(by>y) and x<(bx-ax)*(y-ay)/(by-ay)+ax then inside=not inside end
  j=i
 end
 return inside
end
local function inside39(m,p)
 if not m.inside then return true end
 for _,a in ipairs((C39.inside[m.region] or {})[m.location] or {}) do
  if (a.height<=0 or (p.z>=a.z and p.z<=a.z+a.height)) and polygon39(p.x,p.y,a.xy) then return true end
 end
 return false
end
local function view39(p)
 if not C39 then return nil end
 local px,py=pixels39(p.region,p.x,p.y);if not px then return nil end
 local best=nil
 for _,m in ipairs(C39.maps) do if m.region==p.region then
  local a=m.active;local dx,dy=(px-a[1])/a[3],(py-a[2])/a[4]
  local inArea=m.ellipse and ((dx-.5)^2+(dy-.5)^2<=.25) or
      (not m.ellipse and dx>=0 and dx<=1 and dy>=0 and dy<=1)
  if inArea and inside39(m,p) and (not best or m.priority>best.priority or
     (m.priority==best.priority and a[3]*a[4]<best.active[3]*best.active[4]))then best=m end
 end end
 if not best then for _,m in ipairs(C39.maps)do if m.region==p.region and m.default then best=m;break end end end
 if best then
  local r=best.rect;return {kind="local",id=best.name,m=best,px=px,py=py,
   cx=r[1]+r[3]*.5,cy=r[2]+r[4]*.5,scale=math.min(864/r[3],416/r[4])}
 end
 local l=C39.levels[p.region]
 return {kind="region",id=p.region,px=px,py=py,cx=px,cy=py,
  scale=3*math.min(896/(l.crop[3]*2048),448/(l.crop[4]*2048))}
end
local function draw39(p,t)
 local v=view39(p);if not v then return false end
 local slot=0
 if v.m then
  local r=v.m.rect
  ui("TileRect",0,PREFIX.."local_"..v.id..".dds",512+(r[1]-v.cx)*v.scale,256+(r[2]-v.cy)*v.scale,
    r[3]*v.scale/2048*100,r[4]*v.scale/2048*100,true);slot=1
 else
  local l=C39.levels[p.region];local cols,rows=l.crop[3],l.crop[4]
  local short=p.region=="trosecko" and "trosky" or "kuttenberg"
  local x0=math.max(0,math.floor((v.cx-448/v.scale)/2048))
  local x1=math.min(cols-1,math.floor((v.cx+448/v.scale)/2048))
  local y0=math.max(0,math.floor((v.cy-224/v.scale)/2048))
  local y1=math.min(rows-1,math.floor((v.cy+224/v.scale)/2048))
  for y=y0,y1 do for x=x0,x1 do
   if slot<TILES39 then
    ui("TileRect",slot,PREFIX..short.."_"..tostring(y*cols+x+1)..".dds",
      512+(x*2048-v.cx)*v.scale,256+(y*2048-v.cy)*v.scale,v.scale*100,v.scale*100,true);slot=slot+1
   end
  end end
 end
 for i=slot,TILES39-1 do ui("TileRect",i,"",0,0,100,100,false)end
 ui("Clear")
 local count=0
 local function place39(x,y,color,direction,index)
  local mx,my=pixels39(p.region,x,y);if not mx then return false end
  local dx,dy=(mx-v.cx)*v.scale,(my-v.cy)*v.scale
  local edge=math.abs(dx)>434 or math.abs(dy)>210
  if edge and not direction then return false end
  if edge then local k=math.min(434/math.max(math.abs(dx),.0001),210/math.max(math.abs(dy),.0001));dx=dx*k;dy=dy*k end
  if not index then if count>=MAX_MARKERS then return false end;count=count+1;index=count end
  ui("Marker",index,512+dx,256+dy,edge and math.deg(math.atan2(dx,-dy)) or 0,color,edge)
  return true
 end
 local fx,fy=pixels39(p.region,p.x-math.sin(p.yaw),p.y+math.cos(p.yaw))
 local heading=math.deg(math.atan2(fx-v.px,-(fy-v.py)))
 ui("Marker",0,512+(v.px-v.cx)*v.scale,256+(v.py-v.cy)*v.scale,heading,INK.player,true)
 if N.waypoint then place39(N.waypoint.x,N.waypoint.y,INK.waypoint,true)end
 for _,m in ipairs(N.markers)do if m.quest then place39(m.x,m.y,INK.quest,true)end end
 for _,m in ipairs(N.markers)do if not m.quest then place39(m.x,m.y,INK.poi,false)end end
 -- Baked native ordinary POIs provide coverage when the compass is disabled.
 -- No entity scan, inventory read, periodic UI rebuild or external DLL.
 local candidates={}
 for _,m in ipairs(C39.pois[p.region] or {})do
  local mx,my=pixels39(p.region,m[1],m[2])
  if mx and math.abs((mx-v.cx)*v.scale)<=434 and math.abs((my-v.cy)*v.scale)<=210 then
   local duplicate=false
   for _,live in ipairs(N.markers)do
    if not live.quest and (live.x-m[1])^2+(live.y-m[2])^2<.64 then duplicate=true;break end
   end
   if not duplicate then candidates[#candidates+1]=m end
  end
 end
 table.sort(candidates,function(a,b)return (a[1]-p.x)^2+(a[2]-p.y)^2<(b[1]-p.x)^2+(b[2]-p.y)^2 end)
 for _,m in ipairs(candidates)do if count>=MAX_MARKERS then break end;place39(m[1],m[2],INK.poi,false)end
 N.drawn=count;N.view39=v.id;N.viewKind39=v.kind;N.tileCount39=slot;N.lastRender=t or N.lastRender
 N.open=not N.surfaceRefused;N.region=p.region
 return not N.surfaceRefused
end
M.MapPixelsM39=pixels39;M.MapViewM39=view39

local function marker(index,dx,dy,color,arrow)
 local edge=math.abs(dx)>238 or math.abs(dy)>238
 if edge then
  local factor=238/math.max(math.abs(dx),math.abs(dy))
  dx=dx*factor;dy=dy*factor;arrow=true
 end
 local angle=arrow and math.deg(math.atan2(dx,-dy)) or 0
 ui("Marker",index,512+dx,256+dy,angle,color,arrow and true or false)
end
local function clear()
 ui("Clear")
 N.markers={};N.waypoint=nil;N.status=0
end
local function hideTiles()
 if C39 then for i=0,TILES39-1 do ui("TileRect",i,"",0,0,100,100,false)end
 else for i=0,3 do ui("Tile",i,"",0,0,100,false)end end
end
-- Game time may rewind on save/load without replacing this Lua module or
-- actor handle. Retire this own movie/data edge before using the new clock;
-- otherwise its old lastRender can throttle every fresh feed for minutes.
local function sampleClock()
 local t=clock()
 if t then
  if N.lastClock and t<N.lastClock then
   clear();hideTiles();N.open=false
   N.lastFeed=-100;N.lastRender=-100
   M.feedGeneration=M.feedGeneration+1
  end
  N.lastClock=t -- missing/invalid clocks cannot erase a previous valid sample
 end
 return t
end
local function render(p,t)
 if not p or not visible() then return false end
 if t and t-N.lastRender<.48 then return false end
 N.surfaceRefused=false
 if C39 then return draw39(p,t)end
 local r=p.region
 local short=r=="trosecko" and "trosky" or "kuttenberg"
 local cols,rows=r=="trosecko" and 3 or 6,r=="trosecko" and 3 or 5
 local u,v=project(r,p.x,p.y)
 if not u then return false end
 local width,height=cols*2048,rows*2048
 -- Native detail IDs address the CROPPED image. Kuttenberg's world rotation
 -- and row crop already happened in project(); do not shift detail rows again.
 local pixelsPerMetre=r=="trosecko" and width/2652 or math.sqrt(2)*1.5
 local scale=512/(pixelsPerMetre*SPAN)
 local px,py=u*width,v*height
 local x0,x1=math.floor((px-256/scale)/2048),math.floor((px+256/scale)/2048)
 local y0,y1=math.floor((py-256/scale)/2048),math.floor((py+256/scale)/2048)
 local slot=0
 for y=math.max(0,y0),math.min(rows-1,y1) do
  for x=math.max(0,x0),math.min(cols-1,x1) do
   if slot<4 then
    ui("Tile",slot,PREFIX..short.."_"..tostring(y*cols+x+1)..".dds",
     512+(x*2048-px)*scale,256+(y*2048-py)*scale,scale*100,true)
    slot=slot+1
   end
  end
 end
 for i=slot,3 do ui("Tile",i,"",0,0,100,false) end
 ui("Clear") -- visibility properties only; persistent glyphs are never rebuilt
 local fu,fv=project(r,p.x-math.sin(p.yaw),p.y+math.cos(p.yaw))
 local heading=math.deg(math.atan2((fu-u)*width,-(fv-v)*height))
 ui("Marker",0,512,256,heading,INK.player,true)
 local count=0
 local function place(m,color,direction)
  if count>=MAX_MARKERS then return end
  local mu,mv=project(r,m.x,m.y);if not mu then return end
  local dx,dy=(mu*width-px)*scale,(mv*height-py)*scale
  if not direction and (math.abs(dx)>238 or math.abs(dy)>238) then return end
  count=count+1;marker(count,dx,dy,color,false)
 end
 if N.waypoint then place(N.waypoint,INK.waypoint,true) end
 for _,m in ipairs(N.markers) do if m.quest then place(m,INK.quest,true) end end
 for _,m in ipairs(N.markers) do if not m.quest then place(m,INK.poi,false) end end
 N.drawn=count;N.lastRender=t or N.lastRender;N.open=not N.surfaceRefused;N.region=r
 if N.surfaceRefused then return false end
 return true
end
local function packet(s,t)
 if type(s)~="string" or #s>24000 then return false end
 local f=split(s,"|",5)
 if not f or #f~=5 or f[1]~="T2" then return false end
 local serial,status=tonumber(f[2]),tonumber(f[3])
 if not serial or serial~=math.floor(serial) or serial<0 or serial>9007199254740991 or serial<N.activation or
    (status~=0 and status~=1 and status~=3) then return false end
 local waypoint=nil;local markers={}
 if status==0 and (f[4]~="" or f[5]~="") then return false end
 if status~=0 and f[4]~="" then
  local w=split(f[4],",",2)
  if not w or #w~=2 then return false end
  local x,y=tonumber(w[1]),tonumber(w[2])
  if not finite(x) or not finite(y) then return false end
  waypoint={x=x,y=y}
 end
 if status==3 then
  local list=split(f[5],";",97)
  if not list then return false end
  for _,row in ipairs(list) do if row~="" then
   local a=split(row,",",5)
   if not a or #a~=5 then return false end
   local kind,x,y,z=tonumber(a[1]),tonumber(a[2]),tonumber(a[3]),tonumber(a[4])
   if not kind or kind~=math.floor(kind) or not finite(x) or not finite(y) or not finite(z) or
      #a[5]>32 or not string.match(a[5],"^[%x]+$") then return false end
   if #markers>=96 then return false end
   markers[#markers+1]={x=x,y=y,quest=kind>=1 and kind<=3,nativeType=kind}
  end end
 elseif f[5]~="" then return false end
 -- Commit only after every field is qualified. A malformed high activation
 -- cannot poison the next real producer packet or leave partially parsed marks.
 N.activation=serial;N.status=t and status or 0;N.lastFeed=t or -100
 N.waypoint=t and waypoint or nil;N.markers=t and markers or {}
 return true
end
local function feed(s)
 if not backend and M.SyncPresentation then M.SyncPresentation(true) end
 local p=metadata();local t=sampleClock()
 if not packet(s,t) then clear() end
 if visible() and p and t then render(p,t) else clear();hideTiles();N.open=false end
 if not p then return "T2||unknown|0|0|0|0|"..tostring(M.feedGeneration) end
 return string.format("T2|%s|%s|%.6f|%.6f|%.6f|%.6f|%d",p.token,p.region,p.x,p.y,p.z,p.yaw,M.feedGeneration)
end
local function tick()
 local t=sampleClock()
 if not visible() then if N.open then clear();hideTiles();N.open=false end;return end
 if not t then
  if N.open then clear();hideTiles();N.open=false end
  return -- no clock means no freshness permission; clear the actual movie
 end
 if t-N.lastFeed>1.5 and N.status~=0 then clear() end
 -- Native feed normally drives rendering. The fallback only keeps native Lua
 -- player position/artwork usable during a failed objective capture/scheduler.
 if t-N.lastRender>=.55 then
  local p=metadata()
  if p then render(p,t) else clear();hideTiles();N.open=false end
 end
end
local function retire()
 clear();hideTiles();N.open=false;N.activation=-1;N.lastFeed=-100;N.lastRender=-100;N.lastClock=nil
 M.feedGeneration=M.feedGeneration+1
end
return {Feed=feed,Tick=tick,Retire=retire,State=N,Project=project}
end
M.CreateNavigationH28=createNavigation
local default=createNavigation(nil)
M.navigation=default.State;M.NavigationProject=default.Project
M.NavigationTick=default.Tick;M.NavigationRetire=default.Retire
function kcd2vr_map_t2_feed(packet) return default.Feed(packet) end
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,"KCD2VR_MAP T2 navigation ready renderer=OWN_PRIVATE_MATERIAL_UI tiles=NATIVE_DETAIL updateHzMax=2 view="..(rawget(_G,"KCD2VRMapCatalogM39") and "M39_NATIVE_LOCAL_OR_REGION_3X" or "LEGACY_800_METRES").." externalDLL=NONE") end
