-- Dedicated supply bootstrap: independent of the held-map startup/renderer.
local ok,err=pcall(function()
 if not Script or type(Script.ReloadScript)~='function' then error('ReloadScript unavailable') end
 Script.ReloadScript('Scripts/KCD2VR/native_supply_t2.lua')
 if type(kcd2vr_supply_native_t2)~='function' or KCD2VRGhostSupplyT15~=15 then error('ghost supply callback not loaded') end
end)
if System and System.LogAlways then System.LogAlways('T15 GHOST_SUPPLY boot='..(ok and 'READY' or 'FAILED')..' detail='..tostring(err or 'native inventory only, no visible holsters')) end
