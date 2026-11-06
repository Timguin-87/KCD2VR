-- Event-qualified native book surface. No GetElement/GetArray/instance0 probing.
local M=KCD2VRMap
if not M or not M.CreateNavigationH28 then return end
local S=nil
local registered=false
local actor=nil
local registrationRetries=0
local function clock()
 local ok,t=pcall(function()return System.GetCurrTime()end)
 return ok and type(t)=="number" and t==t and t>=0 and t or nil
end
local function player()
 local p=rawget(_G,"g_localActor") or rawget(_G,"player")
 return p and p.id and System.GetEntity(p.id)==p and p or nil
end
local function ownPayload(payload)
 if type(payload)~="string" or #payload>2048 then return nil end
 local rev,images=payload:match("^(%d+)|(.+)$");rev=tonumber(rev)
 if not rev or rev<1 or rev>2147483647 then return nil end
 -- Native ShowMap receives flat page/image IDs. Never match a book title or
 -- metadata from another item. Only our exact two DocumentContent image IDs.
 local own=false;local n=0
 for value in (images.."|"):gmatch("(.-)|") do
  n=n+1;if n>32 then return nil end
  value=value:lower():gsub("\\","/")
  for _,region in ipairs({"trosky","kuttenberg"}) do
   local id="kcd2vr_map_"..region
   if value==id or value==id.."_ui.dds" or value=="textures/books/maps/"..id.."_ui.dds" or value=="libs/ui/textures/books/maps/"..id.."_ui.dds" then own=true end
  end
 end
 return own and rev or nil
end
function M.OnNativeMapContentH28(a,b,c,d,e)
 if a==M then a,b,c,d=b,c,d,e end
 if a~="GeneralBook" or type(b)~="number" or b<0 or b%1~=0 or b>2147483647 or c~="OnKCD2VRMapContent" or type(d)~="table" then return end
 local p=player();if not p or p~=actor then return end
 local rev=ownPayload(d[1] or d.Payload)
 if not rev then if S and S.instance==b then S=nil end;return end
 if S and S.instance==b and S.revision==rev and S.player==p then return end
 local t=clock();if not t then return end
 S={instance=b,revision=rev,player=p,received=t,lastFeed=-100,owner=nil}
end
function M.NativeMapArmH28()
 local p=player();if not p or not UIAction or not UIAction.RegisterElementListener then return false end
 if actor~=p then registered=false;actor=p end
 S=nil;registrationRetries=0
 local ok,result=pcall(function()return UIAction.RegisterElementListener(M,"GeneralBook",-1,"OnKCD2VRMapContent","OnNativeMapContentH28")end)
 registered=ok and result~=false
 return registered
end
local function current()
 local t=clock()
 return S and t and t>=S.lastFeed and t-S.lastFeed<=1.5 and player()==S.player and actor==S.player
end
local backend={}
function backend.Visible()return current() and true or false end
function backend.Player()return current() and S.player or nil end
function backend.Call(name,...)
 if not current() then return false end
 local id=S.instance;local args={...};local n=select("#",...)
 local ok,result=pcall(function()return UIAction.CallFunction("GeneralBook",id,"KCD2VRMap"..name,unpack(args,1,n))end)
 return ok and result~=false
end
local nav=M.CreateNavigationH28(backend)
M.nativeNavigationH28=nav.State
function kcd2vr_map_native_live_h28(packet)
 -- OFF is a logical suspension, not a UI call that could recreate a destroyed
 -- instance. The own clip watchdog hides old glyphs if service stops entirely.
 if packet=="OFF" then if S then S.lastFeed=-100 end;return "L8|OFF" end
 local serial=type(packet)=="string" and tonumber(packet:match("^T2|(%d+)|"))
 local t=clock();local p=player()
 if not S and p and p==actor and registrationRetries<3 then
  -- Some native managers attach -1 to existing instances only. A CURRENT
  -- native-reading callback may subscribe again after that instance exists.
  -- Three attempts, no probing during startup/loading and none after success.
  registrationRetries=registrationRetries+1
  pcall(function()UIAction.RegisterElementListener(M,"GeneralBook",-1,"OnKCD2VRMapContent","OnNativeMapContentH28")end)
 end
 if not serial or serial<1 or not S or not t or p~=S.player or p~=actor then return "L8|NO_CURRENT_SURFACE_EVENT" end
 if S.owner and S.owner~=serial then S=nil;return "L8|READING_OWNER_CHANGED" end
 if not S.owner then if t<S.received or t-S.received>5 then S=nil;return "L8|CONTENT_EVENT_EXPIRED" end;S.owner=serial end
 S.lastFeed=t
 if not backend.Call("Begin") then return "L8|NATIVE_PAGE_METHOD_REFUSED" end
 local reply=nav.Feed(packet)
 if not reply:match("^T2|[^|]+|trosecko|") and not reply:match("^T2|[^|]+|kutnohorsko|") then backend.Call("End") end
 return reply -- existing strict player/world metadata packet, no native pointers
end
local previousEnd=M.WorldEnded
function M.WorldEnded(...)
 S=nil;actor=nil;registered=false
 if previousEnd then return previousEnd(...) end
end
