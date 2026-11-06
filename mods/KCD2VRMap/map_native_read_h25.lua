-- Use the game's existing inventory document action. No animation/actor pose,
-- world helper, camera, equipment or material writes. Inventory visibility and
-- one-shot admission come from the original-first native script scheduler.
local M=KCD2VRMap
if not M then return end
local ITEM="5f3c2a71-8e4d-4b6a-9c1e-2d7f0b9a4e63"
local lastSerial=0
local request=nil
local listening=false
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s) if captureD48("f5") and System and type(System.LogAlways)=="function" then pcall(System.LogAlways,"KCD2VR_MAP H25 "..s) end end
local function currentPlayer()
 local p=rawget(_G,"g_localActor") or rawget(_G,"player")
 return p and p.id and p.inventory and System.GetEntity(p.id)==p and p or nil
end
function M.OnNativeMapFocus(a,b,c,d,e)
 -- Native ScriptBind UI callback is (element, instance, event, argTable).
 -- Also accept the ordinary table-method calling convention without guessing
 -- a different argument order or accepting an unqualified focus notification.
 if a==M then a,b,c,d=b,c,d,e end
 if not request or not request.focusIssued or request.spent or a~="ApseInventoryList" or b~=0 or c~="OnFocusChanged" or type(d)~="table" then return end
 local ids=d[1] or d.Ids
 local client=d[4];if client==nil then client=d.ClientId end
 request.focused=ids==request.id and client==0 and currentPlayer()==request.player
end
function kcd2vr_map_native_read_h25(packet)
 local serial=type(packet)=="string" and tonumber(packet:match("^N5|(%d+)$"))
 if not serial or serial<=0 or serial%1~=0 then return "N5|0|REFUSED" end
 local prefix="N5|"..string.format("%.0f",serial).."|"
 if serial<lastSerial then return prefix.."STALE" end
 if request and serial==lastSerial and request.spent then return prefix.."SPENT_NO_RETRY" end
 local p=currentPlayer()
 if not p or not UIAction or not ItemManager or type(ItemManager.GetItem)~="function" then return prefix.."UNAVAILABLE" end
 if serial~=lastSerial then lastSerial=serial;request={player=p,spent=false,attempts=0} end
 if request.player~=p or request.attempts>=3 then request.spent=true;return prefix.."CONTEXT_CHANGED" end
 request.attempts=request.attempts+1
 local ok,result=pcall(function()
  local count=p.inventory:GetCountOfClass(ITEM)
  if count==0 and not request.grantAttempted then
   request.grantAttempted=true -- irreversible native create is never retried on ambiguity
   p.inventory:CreateItem(ITEM,1,1)
   count=p.inventory:GetCountOfClass(ITEM)
  end
  if type(count)~="number" or count<1 then return "ITEM_MISSING" end
  local id=p.inventory:FindItem(ITEM)
  if id==nil then return "ITEM_MISSING" end
  -- Retail GetItem(WUID) returns copied metadata, NOT a world script table.
  -- Do not guess .Use/.OnUsed or turn it into a manually spawned Book entity.
  local info=ItemManager.GetItem(id)
  if type(info)~="table" or type(info.class)~="string" or info.class:lower()~=ITEM then return "ITEM_IDENTITY_REFUSED" end
  local text=tostring(info.id)
  if not text:match("^%[Item%-%d+%]$") or text~=tostring(id) then return "ITEM_TOKEN_REFUSED" end
  if not listening then
   local registered=UIAction.RegisterElementListener(M,"ApseInventoryList",0,"OnFocusChanged","OnNativeMapFocus")
   if registered==false then return "LISTENER_REFUSED" end
   listening=true
  end
  request.id=text
  -- The actual event may arrive on a later UI tick. Retain only the latest
  -- exact item/client focus while the C++ inventory/serial lease is current.
  -- Event callbacks NEVER dispatch item use. The next qualified poll may do so.
  if not request.focused then
   if not request.focusIssued then UIAction.CallFunction("ApseInventoryList",0,"FocusNext") end
   request.focusIssued=true
   local focused=UIAction.CallFunction("ApseInventoryList",0,"FocusItem",text)
   if focused==false or not request.focused then return "WAIT_FOCUS" end
  end
  if M.NativeMapArmH28 then M.NativeMapArmH28() end -- subscribe before native content can publish its actual UI instance
  request.spent=true -- spend BEFORE native dispatch / synchronous callbacks
  local used=UIAction.CallFunction("ApseInventoryList",0,"LeftDoubleClick",0)
  if used==false then return "NATIVE_READ_REFUSED" end
  log("serial="..serial.." item="..text.." stage=NATIVE_ITEM_USE_REQUESTED animation=GAME_READING result=NOT_YET_OBSERVED")
  return "NATIVE_READ_REQUESTED"
 end)
 if not ok then request.spent=true;log("serial="..serial.." stage=FAULT noRetry=1 reason="..tostring(result));return prefix.."FAULT_NO_RETRY" end
 if result~="WAIT_FOCUS" then request.spent=true end
 return prefix..result
end

local previousWorldEnded=M.WorldEnded
function M.WorldEnded(...)
 if request then request.spent=true;request.focusIssued=false end
 if previousWorldEnded then return previousWorldEnded(...) end
end
