-- Run only from the qualified native script scheduler, at a requested display
-- family or UI lifetime change. -1 touches existing instances, creating no UI.
local elements={"HUD","Menu","ItemSelection","ItemTransfer","ApseMap",
 "ApseModalDialog","ApsePlayerInfo","GameOver"}
function kcd2vr_prompt_d56(packet)
 if type(packet)~="string" or #packet>64 then return "P56|REFUSED" end
 local serial,style=packet:match("^P56|(%d+)|([24])$")
 local id=tonumber(serial);style=tonumber(style)
 if not id or id<1 or id>9007199254740991 or id~=math.floor(id) then return "P56|REFUSED" end
 if not UIAction or type(UIAction.CallFunction)~="function" then return "P56|UNAVAILABLE" end
 local calls=0
 for _,name in ipairs(elements)do
  local ok,result=pcall(UIAction.CallFunction,name,-1,"SetKcd2vrGlyphStyleD68",style)
  if ok and result~=false then calls=calls+1
  else
   -- Older/missing UI movie: keep valid Xbox hints, never send invalid family4.
   pcall(UIAction.CallFunction,name,-1,"SetInputId",2)
  end
 end
 -- A dispatch receipt is not an achieved-pixel/visible-glyph receipt.
 return "P56|"..serial.."|"..style.."|"..calls
end

-- Station presentation only. Native actions and the authored children remain
-- alive; the movie changes local size and, optionally, alpha of four prompts.
function kcd2vr_prompt_style_d80(packet)
 if type(packet)~="string" or #packet>96 then return "H80|REFUSED" end
 local serial,active,percent,visible=packet:match("^H80|(%d+)|([01])|(%d+)|([01])$")
 local id=tonumber(serial);local scale=tonumber(percent)
 if not id or id<1 or id>9007199254740991 or id~=math.floor(id) or
    not scale or scale<25 or scale>100 or scale~=math.floor(scale) then return "H80|REFUSED" end
 if not UIAction or type(UIAction.CallFunction)~="function" then return "H80|UNAVAILABLE" end
 local ok,result=pcall(UIAction.CallFunction,"HUD",-1,"SetKcd2vrPromptStyleD80",active=="1",scale,visible=="1")
 if not ok or result==false then return "H80|"..serial.."|UNAVAILABLE" end
 -- Dispatch is not evidence of rendered pixels; the native F5 receipt says so.
 return "H80|"..serial.."|DISPATCHED"
end
