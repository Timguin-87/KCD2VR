-- T22 native carrier bootstrap. Fresh-process companion replacement only.
-- The retired Lua carrier must never equip, convert or launch a weapon.
kcd2vr_ammo_supply_t18=nil
kcd2vr_supply_native_t2=nil
if rawget(_G,'KCD2VRThrowReservationVersion20')==20 and
   type(kcd2vr_throw_reservation_t20)=='function' then
 KCD2VRThrowBootstrapVersion22=22
 return
end
local ok,err=pcall(function()
 if not Script or type(Script.ReloadScript)~='function' then error('ReloadScript unavailable') end
 Script.ReloadScript('Scripts/KCD2VR/native_throw_reservation_t20.lua')
 if rawget(_G,'KCD2VRThrowReservationVersion20')~=20 or
    type(kcd2vr_throw_reservation_t20)~='function' then error('reservation API unavailable') end
end)
if ok then KCD2VRThrowBootstrapVersion22=22 end
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
if (captureD48("f5") or captureD48("f11")) and System and type(System.LogAlways)=='function' then
 pcall(System.LogAlways,'T22 FINITE_RESERVATION boot='..(ok and 'READY' or 'FAILED')..' detail='..tostring(err or 'native owner only'))
end
