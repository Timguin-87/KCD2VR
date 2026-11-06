-- D33 (Claude) chest dagger sheath: a small sheath with a dagger in it, worn under Henry's left breast, visible while
-- he carries a dagger or the throwing daggers. Henry only (g_localActor), never NPCs. Pure Lua, game API only:
-- CreateBoneAttachment(slot,bone,name) / SetAttachmentPos / SetAttachmentAngles / SetAttachmentCGF / HideAttachment
-- (all registered in WHGame.dll; the gore mod uses the same calls on NPC bones - the neck stump CGF showed in game).
-- The model is a scaled copy of the game's own sword scabbard (tools/cgf_scale_d33.py), shipped in this pak.
-- tune live in the console (# = lua):
--   #KCD2VR_ChestSheath.pos={x=0,y=0.10,z=0.07} KCD2VR_ChestSheath.Refresh()
--   #KCD2VR_ChestSheath.ang={x=0,y=0,z=0} KCD2VR_ChestSheath.Refresh()      (radians, bone space)
--   #KCD2VR_ChestSheath.cgf='objects/manmade/food/food/meat_piece.cgf' KCD2VR_ChestSheath.Refresh()   (any CGF = placement test)
--   #KCD2VR_ChestSheath.enabled=false KCD2VR_ChestSheath.Refresh()
KCD2VR_ChestSheath = KCD2VR_ChestSheath or {}
local M = KCD2VR_ChestSheath
-- D48: query the existing read-only native channel; absent/failed queries stay quiet.
local function captureD48(key)
 local get=System and System.GetCVar
 if type(get)~="function" then return false end
 local ok,value=pcall(get,"kcd2vr_door_capture_"..key)
 return ok and value==1
end
local function log(s) if captureD48("f5") and System and type(System.LogAlways) == 'function' then pcall(System.LogAlways, 'KCD2VR_SHEATH ' .. tostring(s)) end end
if M.enabled == nil then M.enabled = true end
M.bone = M.bone or 'Spine3'                 -- upper chest joint (the DLL resolves the same name for VRIK)
M.tag  = 'kcd2vr_chest_sheath_d33'
-- the baked model; empty = not baked yet (the sheath stays off and says so once)
M.cgf  = M.cgf or 'objects/kcd2vr/chest_sheath_d33/kcd2vr_dagger_sheath_d33.cgf' -- game shortsword01 scabbard (drop model) scaled 0.9/0.8/0.35 = dagger length
-- bone-space offset/rotation (metres/radians): under the left breast, slanted for a right-hand cross draw
M.pos  = M.pos or { x = -0.12, y = 0.09, z = 0.06 }
M.ang  = M.ang or { x = 0.0, y = 0.35, z = 1.20 }
-- item classes that make it visible: the two KCD2VR throwing dagger families; add the game's dagger ids here
M.items = M.items or { '43286e47-cde0-50bd-ba4c-9747b777b16d', '36ac8977-c630-58ae-95a1-21307111d330',
  '3a640e5d-d8bd-4e8b-b61d-8cd5180e79e7', '09b9021d-fdb4-4aee-8d53-78bdeb073c1d', '2ab2abbc-1620-4ee0-924f-cbe1c90adcc4',
  '39826a87-69aa-45cc-94e1-170a4520e2e9', '36099b07-dae2-4b7b-bcb2-5b580e872ec3', '718ba16c-2c4f-4faa-bff5-140b26928b57' } -- throwing daggers + common/ancient/bruncvik/reward daggers (Tables.pak item.xml)
-- the dagger sitting in the sheath: the game's own dagger CGF, unscaled, on its own attachment ('' = none yet)
M.knifeTag = 'kcd2vr_chest_sheath_d33_knife'
M.knifeCgf = M.knifeCgf or ''
M.knifePos = M.knifePos or { x = -0.12, y = 0.09, z = 0.09 }
M.knifeAng = M.knifeAng or { x = 0.0, y = 0.35, z = 1.20 }
M.periodMs = M.periodMs or 1000
M.state = M.state or { made = false, shown = nil, actor = nil, warned = false, gen = 0 }

local function player() return rawget(_G, 'g_localActor') end
local function hasDagger(p)
  for _, id in ipairs(M.items) do
    local ok, n = pcall(function() return p.inventory:GetCountOfClass(id) end)
    if ok and type(n) == 'number' and n > 0 then return true end
  end
  return false
end
local function apply(p, want)
  local st = M.state
  if st.actor ~= p then st.made = false; st.shown = nil; st.actor = p end
  if want and not st.made then
    local ok, err = pcall(function()
      p:CreateBoneAttachment(0, M.bone, M.tag)
      p:SetAttachmentPos(0, M.tag, M.pos)
      p:SetAttachmentAngles(0, M.tag, M.ang)
      p:SetAttachmentCGF(0, M.tag, M.cgf)
      if M.knifeCgf ~= '' then
        p:CreateBoneAttachment(0, M.bone, M.knifeTag)
        p:SetAttachmentPos(0, M.knifeTag, M.knifePos)
        p:SetAttachmentAngles(0, M.knifeTag, M.knifeAng)
        p:SetAttachmentCGF(0, M.knifeTag, M.knifeCgf)
      end
    end)
    st.made = ok
    log('attach bone=' .. M.bone .. ' cgf=' .. M.cgf .. ' ok=' .. tostring(ok) .. (ok and '' or ' err=' .. tostring(err)))
    if not ok then return end
  end
  if st.made and st.shown ~= want then
    local ok = pcall(function() p:HideAttachment(0, M.tag, not want, true) end)
    if M.knifeCgf ~= '' then pcall(function() p:HideAttachment(0, M.knifeTag, not want, true) end) end
    if ok then st.shown = want end
  end
end
function M.Refresh()
  local p = player(); if not p then return end
  if M.state.made then pcall(function() p:SetAttachmentPos(0, M.tag, M.pos); p:SetAttachmentAngles(0, M.tag, M.ang); p:SetAttachmentCGF(0, M.tag, M.cgf) end) end
  apply(p, M.enabled and M.cgf ~= '' and hasDagger(p))
end
local function tick(gen)
  if gen ~= M.state.gen then return end
  local p = player()
  if p then
    if M.cgf == '' then
      if not M.state.warned then M.state.warned = true; log('model not baked yet - sheath off') end
    else apply(p, M.enabled and hasDagger(p)) end
  end
  if Script and type(Script.SetTimer) == 'function' then Script.SetTimer(M.periodMs, function() tick(gen) end) end
end
function M.WorldStarted() M.state.gen = M.state.gen + 1; M.state.made = false; M.state.shown = nil; M.state.actor = nil; tick(M.state.gen) end
function M.WorldEnded() M.state.gen = M.state.gen + 1 end
local tries = 0
local function register()
  if UIAction and type(UIAction.RegisterEventSystemListener) == 'function' then
    local ok = pcall(function()
      UIAction.RegisterEventSystemListener(M, 'System', 'OnGameplayStarted', 'WorldStarted')
      UIAction.RegisterEventSystemListener(M, 'System', 'OnGameplayEnded', 'WorldEnded')
    end)
    if ok then log('listener=REGISTERED') return end
  end
  tries = tries + 1
  if tries < 10 and Script and type(Script.SetTimer) == 'function' then Script.SetTimer(1000, register) end
end
register()
