-- Exact native authored sounds after a successful instant action.
-- No timers, actor search, animation/recipe/attachment calls or particles.
KCD2VR_AlchemySoundD80=KCD2VR_AlchemySoundD80 or {tokens={},next=1}
local S=KCD2VR_AlchemySoundD80
local sounds={
  'f_ge_herbs_from_sack', -- alchemy_pick_herb_1/2/3
  'f_he_herbs_put',       -- alchemy_return_herb_1/2/3
  'f_he_herbs_put_bowl',  -- alchemy_herb_to_bowl
  'f_he_herbs_to_mortar', -- alchemy_herb_to_mortar
  'f_he_herbs_put_water', -- alchemy_herb_to_pot and _pot_up
  'f_he_bowl_take',      -- alchemy_pick_bowl
  'f_he_bowl_put',       -- alchemy_return_bowl
  'f_he_glassflask_take', -- alchemy_pick_potion_flask
  'f_he_glassflask_put',  -- alchemy_return_potion_flask
  'f_he_special2_pick',  -- alchemy_get_spec_ingredience_v01..03
  'f_he_special_return', -- alchemy_return_spec_ingredience_v01..03
  'f_he_special_to_bowl',-- alchemy_spec_ingredience_to_bowl
  'f_he_special_to_mortar', -- alchemy_spec_ingredience_to_mortar
  'f_he_special_use',    -- alchemy_spec_ingredience_to_pot and _pot_up
  'f_he_hourglass_take', -- alchemy_use_sandglass
  'f_he_hourglass_put'   -- alchemy_use_sandglass
}
local function integer(text,max)
  local n=tonumber(text)
  return n and n>=1 and n<=max and n==math.floor(n) and n or nil
end
function kcd2vr_alchemy_sound_d80(packet)
  if type(packet)~='string' or #packet>160 then return 'A80|REFUSED' end
  local vm,life,serial,id,cue=packet:match('^A80|(%d+)|(%d+)|(%d+)|(%d+)|(%d+)$')
  if not vm or not integer(vm,9007199254740991) or not integer(life,9007199254740991) or
     not integer(serial,9007199254740991) then return 'A80|REFUSED' end
  id=integer(id,16777215);cue=integer(cue,#sounds)
  if not id or not cue then return 'A80|REFUSED' end
  local token=vm..':'..life..':'..serial
  for _,old in ipairs(S.tokens)do if old==token then return 'A80|'..serial..'|SPENT' end end
  if not player or player.id~=id or not player.actor or not System or not System.GetEntity or
     not Sound or not Sound.GetAudioTriggerID or
     not player.ExecuteAudioTrigger or not player.GetDefaultAuxAudioProxyID then return 'A80|'..serial..'|UNAVAILABLE' end
  local ok,entity=pcall(System.GetEntity,id)
  if not ok or entity~=player then return 'A80|'..serial..'|WRONG_PLAYER' end
  local name=sounds[cue]
  local found,trigger=pcall(Sound.GetAudioTriggerID,name)
  if not found or trigger==nil or trigger==false or trigger==0 then return 'A80|'..serial..'|NO_TRIGGER' end
  -- Consume before the engine boundary; reload retains this bounded ledger.
  S.tokens[S.next]=token;S.next=S.next%16+1
  -- These are the same two native leaves used by AudioUtils. Do not require
  -- that optional Lua helper table to have been loaded in this script context.
  local returned,result=pcall(function()
    local proxy=player:GetDefaultAuxAudioProxyID()
    return player:ExecuteAudioTrigger(trigger,proxy)
  end)
  return 'A80|'..serial..'|'..(returned and result~=false and 'DISPATCHED' or 'UNCERTAIN')
end
