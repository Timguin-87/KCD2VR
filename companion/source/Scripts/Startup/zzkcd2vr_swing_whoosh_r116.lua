-- R116: the weapon whoosh at the moment of your physical swing.
-- Same two native leaves as zzkcd2vr_alchemy_sound_d80.lua (player audio proxy + ExecuteAudioTrigger).
-- Only authored weapon flick triggers (c_w_..flick..) are accepted; no timers, no actor search.
KCD2VR_SwingWhooshR116=KCD2VR_SwingWhooshR116 or {last=0}
local S=KCD2VR_SwingWhooshR116
function kcd2vr_swing_whoosh_r116(packet)
  if type(packet)~='string' or #packet>90 then return 'W116|REFUSED' end
  local serial,name=packet:match('^W116|(%d+)|(c_w_[a-z0-9_]+)$')
  serial=serial and tonumber(serial)
  if not serial or serial<1 or not name or not name:find('flick',1,true) or #name>39 then return 'W116|REFUSED' end
  if serial<=S.last then return 'W116|'..serial..'|SPENT' end
  if not player or not Sound or not Sound.GetAudioTriggerID or
     not player.ExecuteAudioTrigger or not player.GetDefaultAuxAudioProxyID then return 'W116|'..serial..'|UNAVAILABLE' end
  local found,trigger=pcall(Sound.GetAudioTriggerID,name)
  if not found or trigger==nil or trigger==false or trigger==0 then return 'W116|'..serial..'|NO_TRIGGER' end
  S.last=serial -- spent before the engine call; a lost reply never replays a whoosh
  local returned,result=pcall(function()
    local proxy=player:GetDefaultAuxAudioProxyID()
    return player:ExecuteAudioTrigger(trigger,proxy)
  end)
  return 'W116|'..serial..'|'..(returned and result~=false and 'DISPATCHED' or 'UNCERTAIN')
end
