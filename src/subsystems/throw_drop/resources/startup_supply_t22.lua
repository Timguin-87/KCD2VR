-- Use the existing startup member; no second scheduler or inventory delivery.
local ok,err=pcall(function()
 if not Script or type(Script.ReloadScript)~='function' then error('ReloadScript unavailable') end
 Script.ReloadScript('Scripts/KCD2VR/native_supply_t2.lua')
 if KCD2VRThrowBootstrapVersion22~=22 or KCD2VRThrowReservationVersion20~=20 or
    type(kcd2vr_throw_reservation_t20)~='function' then error('finite reservation callback not loaded') end
end)
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
if (captureD48("f5") or captureD48("f11")) and System and type(System.LogAlways)=='function' then
 pcall(System.LogAlways,'T22 FINITE_SUPPLY boot='..(ok and 'READY' or 'FAILED')..' detail='..tostring(err or 'native carrier only'))
end
