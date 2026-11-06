-- KCD2VR kick01 (Claude, 23 Oct 2026): does the game's own kick clip play on Henry when we ask for it directly, with no combat
-- action? the retail has an ADDITIVE kick (combat_rg_sz5_az5_natk_kick_nwpn_add) next to the full one
-- (1d-combat_rg_sz5_az5_natk_kick_nwpn). if the additive one layers on top of whatever Henry does (VRIK arms, walking),
-- that is the armed VR kick: the game's leg, our foot contact for the hit. console only, dev build or normal game:
--   #KCD2VR_Kick.Test()          additive kick on layer 10 (default)
--   #KCD2VR_Kick.Test(5)         additive kick on layer 5
--   #KCD2VR_Kick.Test(10,'full') full kick clip on layer 10 (will fight the mannequin, just to see)
--   #KCD2VR_Kick.Test(0,'full')  full clip on layer 0 (replaces the base pose for 1.17 s if the engine lets it)
--   #KCD2VR_Kick.List()          which of the clip names the loaded character knows (GetAnimationLength > 0)
-- every call logs KCD2VR_KICK ... with the length the engine reports and whether IsAnimationRunning went true.
KCD2VR_Kick = KCD2VR_Kick or {}
local K = KCD2VR_Kick
K.version = '0.1-TEST'
K.clips = {
  add  = 'combat_rg_sz5_az5_natk_kick_nwpn_add',
  full = '1d-combat_rg_sz5_az5_natk_kick_nwpn',
  dog  = '1d-combat_rg_z5_natksw_dog_kick_nwpn',
}
local function log(s) System.LogAlways('KCD2VR_KICK ' .. tostring(s)) end
local function say(s) pcall(function() Game.SendInfoText(s, true, nil, 2) end) log(s) end

local function lengthOf(name)
  local ok, len = pcall(function() return player:GetAnimationLength(0, name) end)
  if ok and type(len) == 'number' then return len end
  return -1
end

function K.List()
  for k, n in pairs(K.clips) do log(string.format('clip %-4s %-45s length=%.3f', k, n, lengthOf(n))) end
end

function K.Test(layer, which)
  layer = tonumber(layer) or 10
  local name = K.clips[which or 'add'] or which or K.clips.add
  local len = lengthOf(name)
  if len <= 0 then say(string.format('kick test: %s not known to Henry (length %.2f)', name, len)) return end
  local ok, err = pcall(function()
    player:StartAnimation(0, name, layer, 0.1, 1.0, false, true)
  end)
  local running = false
  pcall(function() running = player:IsAnimationRunning(0, layer) end)
  say(string.format('kick test: %s layer=%d len=%.2f start=%s running=%s %s', name, layer, len, tostring(ok), tostring(running), ok and '' or tostring(err)))
  if ok then
    Script.SetTimer(math.floor(len * 1000) + 100, function()
      local r2 = false pcall(function() r2 = player:IsAnimationRunning(0, layer) end)
      log(string.format('kick test end: %s layer=%d stillRunning=%s', name, layer, tostring(r2)))
    end)
  end
end

function K.Stop(layer)
  layer = tonumber(layer) or 10
  pcall(function() player:StopAnimation(0, layer) end)
  log('stopped layer ' .. layer)
end
log('loaded ' .. K.version)
