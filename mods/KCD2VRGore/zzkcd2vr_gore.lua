-- KCD2VR Gore (optional add-on for the KCD2VR VR mod). Own script, protocol 1.
-- v62.9 (Claude, 22 Oct 2026): the BODY side gets the game's own neck stump (objects/characters/humans/male/head/m_head_decapitated/
--   m_head_decapitated.skin, Warhorse's decapitated-corpse mesh: Neck..Neck1 with its own wound textures) - from Lua, no C++: a helper
--   entity wears the stump in a wrapper character and is bound to the victim's 'Neck' bone attachment with Entity.SetAttachmentObject
--   (official bind: characterSlot, attachmentName, entityId, slot, flags). The attachment offset is the inverse of the Neck bind pose read
--   from the skin file (W2B: pos -1.3697 0 0.4887, angles xyz 108.8 0 90 deg), so the model-space stump sits exactly on the neck.
--   Tunable without rebuild: KCD2VR_Gore.neckStumpPos / neckStumpAngles / neckStump=false. Console: KCD2VR_Gore.TestNeckStump().
-- v62.8 (Claude, 22 Oct 2026): the flying head is the HEAD ONLY (cut at the Neck1/Head bone weights, the C3 line of the game's own
--   m_head_decapitated stump): wrappers <head>_kcdghead.fly[_variant].cdf from kcd2vr_gore_heads.pak, full head as fallback. G.c3Head=false
--   forces the old full head. The body side neck stump (objects/.../m_head_decapitated/m_head_decapitated.skin, the game's own) is attached
--   natively (C++), not from here - CreateSkinAttachment from Lua ignores the path (v60 finding).
-- v62.7 (Claude, 22 Oct 2026): the flying head gets the victim's REAL face: the C++ event may now carry the material names
--   (e.headMtl, e.att[i].m); Lua picks <skin>.fly_<variant>.cdf from the real material (the pak ships one wrapper per shipped
--   material) and only falls back to the default wrapper when no such file loads. Stump blood moved from the 'Neck' joint (neck base,
--   Miki: 'looks like a collarbone cut') to 'Neck1' (mid neck, C3) with Neck as fallback. The flying head bleeds from its own cut face
--   (a second blood effect on the carrier at the Neck1 end), not only a trail. The limb experiment stays OFF by default (62.2 rule).
--   Base: 62.6d (Codex-reviewed). The GORE65 delta (DeepSeek) was on 62.2 and lost 62.4..62.6d; it is NOT merged here.
-- v62.1: leases per victim entity + cut, complete-set-or-nothing hide, carrier cleanup on any failure, Clear restores (Codex review 18:27).
-- v62.0 (Claude, 20 Oct 2026): LIMB CUT for hands and feet. The hand/foot skin of the victim is hidden (both sides, the game cannot swap a skin from Lua),
--   the cut side flies off as a rigid piece (own wrapper character, own mass), blood effect on the wrist/ankle bone. Needs the limb piece PAKs (kcd2vr_gore_limbs_*.pak)
--   and the piece wrappers inside the main pak. Full-body / one-mesh victims (female, dressed one-piece) are refused with one KCD2VR_GORE line that says why.
-- v61.0-T3 (Claude, 20 Oct 2026): neck bleeding on the victim (blood spray bone-attached, NO mesh), stump test command, limb events log what they would cut. Heads/helmets/beards unchanged from T2.
-- v60.1-T2 (Claude, 19 Oct 2026): TEST build. T2 adds hair/beard/helmet parts (own slots, one wrapper character each) and a blood trail on the flying head.
-- v60.0-T1 (Claude, 19 Oct 2026): TEST build. Does nothing by itself: the KCD2VR ASI calls KCD2VR_Gore.Cut{...} once per kill move.
-- What changed against v59.1 (see CLAUDE_GORE_ROOT_CAUSES_AND_FIX_PLAN_2026-10-19.md):
--  * The game's Lua entity:CreateSkinAttachment takes (slot, name) ONLY - the DLL registers it as "characterSlot, attachmentName".
--    The mesh path argument was ignored, so every skin attachment of v50..v59.1 was EMPTY (flying head invisible, cut body skins never loaded).
--  * The flying head now loads through a tiny wrapper character file (kcd2vr_fly.cdf next to each head skin: male.chr + ONE CA_SKIN head).
--  * Limb events no longer touch the victim (the old code hid the whole body/hands/feet attachment and put an empty one in its place).
--  * KCD2VR_Gore.Test('head' | 'generic' | 'carrier') spawns a flying head in front of Henry without killing anyone.
-- Everything is pcall-protected; every step writes one 'KCD2VR_GORE' line into kcd.log.
KCD2VR_Gore = KCD2VR_Gore or {}
local G = KCD2VR_Gore
G.protocol = 1
G.version = '62.9b-H41-CARRIER-ROLLBACK'
G.done = G.done or {}      -- [target] = { [cut] = true }: a limb is cut once
G.V = G.V or {}            -- cut meshes, filled by zzkcd2vr_gore_meshes.lua (not used by v60: needs a C++ skin route, see report)
G.debugCarrier = G.debugCarrier or false   -- true: the rigid meat carrier stays visible (proves the physics body flies)
-- H15: delayed work belongs to the exact Lua entity, never a reused numeric id.
-- Tokens also prevent an older stump timer removing a newer same-entity effect.
G.stumpLeases61 = G.stumpLeases61 or setmetatable({}, { __mode = 'k' })
local stumpLeases61 = G.stumpLeases61
local function currentEntity61(original)
  local current = original and System.GetEntity(original.id)
  if current == original then return current end
  return nil
end

local MEAT = 'objects/manmade/food/food/meat.cgf'
local SKEL = { male = 'objects/characters/humans/male/skeleton/male.cdf', female = 'objects/characters/humans/female/skeleton/female.cdf' }
local MASS = { head = 4 }
local PUSH = { head = 28 }                                     -- impulse at full energy
local LIFE_MS = 120000                                         -- flying parts are removed after 2 minutes
local DEFAULT_HEAD = 'objects/characters/humans/male/head/m_head_034/m_head_034.skin'

local function log(s) System.LogAlways('KCD2VR_GORE ' .. tostring(s)) end
-- one short in-game line when a cut cannot run (Miki: say why where he sees it, no log-only refusals)
local function say(s) pcall(function() Game.SendInfoText(s, true, nil, 2) end) log('SAY ' .. s) end
local function try(what, f) local ok, e = pcall(f) if not ok then log(what .. ' failed: ' .. tostring(e)) end return ok end
local function num(v, d) if type(v) == 'number' and v == v and v ~= math.huge and v ~= -math.huge then return v end return d end
local function vec(t) t = t or {} return { x = num(t.x, 0), y = num(t.y, 0), z = num(t.z, 0) } end

-- T3 stump: G.stumpMode = 'effect' (default): the game's own blood effect on a bone attachment of the victim, NO mesh (a bone-attached cgf floated
-- away from a corpse neck in C59). 'meat': the old 50.4 meat cap (test only). 'off': nothing.
-- Every step is logged; STUMP_PROBE compares the neck bone world position with the entity so a floating attachment shows up in the log.
G.stumpMode = G.stumpMode or 'effect'
local BONE = { head = 'Neck1', lh = 'LeftHand', rh = 'RightHand', lf = 'LeftFoot', rf = 'RightFoot' }   -- v62.7: Neck1 = mid neck (C3), retail joint
local BONE_FALLBACK = { head = 'Neck' }
-- v62.7: the first bone that the victim really has (Neck1 on every retail human; Neck as fallback for odd skeletons)
local function stumpBone(v, cut)
  local b = BONE[cut]
  if not b then return nil end
  local ok, p = pcall(function() return v:GetBonePos(b) end)
  if ok and p then return b end
  local fb = BONE_FALLBACK[cut]
  if fb then log('STUMP ' .. cut .. ' bone ' .. b .. ' not readable, using ' .. fb) return fb end
  return b
end
local BLOOD_STUMP = 'collisions.combat.blood_head'
-- v62.3: true only when the attachment was really removed (pcall ok AND the native call did not return false). A failure keeps the lease for a retry.
local function destroyStump(en, tag)
  local ok, ret = pcall(function() return en:DestroyAttachment(0, tag) end)
  if not ok then log('stump destroy ' .. tag .. ' failed: ' .. tostring(ret)) return false end
  if ret == false then log('stump destroy ' .. tag .. ' refused by the engine (kept for retry)') return false end
  return true
end
-- v62.4: the attached blood effect (SetAttachmentEffect) was accepted by the game but showed nothing in the headset (Miki, 20 Oct).
-- Same effect, the way the flying head's blood trail already works (LoadParticleEffect on an entity), but on a small helper entity that follows
-- the victim's bone for ~4 s. The helper is removed when done or when the victim is gone. G.worldBlood=false turns it off.
G.worldBlood = (G.worldBlood ~= false)
local function worldBlood(v, cut)
  local bone = stumpBone(v, cut)
  if not G.worldBlood or not bone then return end
  local left, helper = 8, nil
  local function stop() if helper then local hh = helper helper = nil pcall(function() System.RemoveEntity(hh.id) end) end end
  local function tick()
    local w = currentEntity61(v)
    local p = nil
    if w then pcall(function() p = w:GetBonePos(bone) end) end
    if not p then stop() log('WORLDBLOOD ' .. cut .. ' victim gone') return end
    if not helper then
      pcall(function() helper = System.SpawnEntity({ class = 'BasicEntity', name = 'kcd2vr_bloodfx_' .. tostring(math.random(1, 999999)), position = vec(p), properties = {} }) end)
      if not helper then log('WORLDBLOOD ' .. cut .. ' helper spawn FAILED') return end
    else
      pcall(function() helper:SetWorldPos(vec(p)) end)
    end
    pcall(function() helper:LoadParticleEffect(0, BLOOD_STUMP, { Scale = 1.0, CountScale = 1.0 }) end)
    left = left - 1
    if left > 0 then Script.SetTimer(500, tick) else stop() end
  end
  tick()
  log('WORLDBLOOD ' .. cut .. ' started')
end

local function stump(v, cut)
  local mode = G.stumpMode
  local bone = stumpBone(v, cut)
  if mode == 'off' or not bone then
    log('STUMP ' .. cut .. ' off') return false
  end
  local tag = 'kcd2vr_stump_' .. cut
  local ok = try('stump ' .. cut, function()
    v:CreateBoneAttachment(0, bone, tag)
    if mode == 'meat' then v:SetAttachmentCGF(0, tag, 'objects/manmade/food/food/meat.cgf') end
    v:SetAttachmentEffect(0, tag, BLOOD_STUMP, { x = 0, y = 0, z = 0 }, { x = 0, y = 0, z = 1 }, 1, 0)
  end)
  if ok then
    local lease = {}
    local leases = stumpLeases61[v] or {}
    stumpLeases61[v] = leases
    leases[cut] = lease
    local function currentStump61()
      if leases[cut] == lease then return currentEntity61(v) end
      return nil
    end
    local function probe(ms)
      Script.SetTimer(ms, function()
        local w = currentStump61()
        if not w then return end
        local bp, ep = nil, w:GetWorldPos()
        pcall(function() bp = w:GetBonePos(bone) end)
        if bp then
          log('STUMP_PROBE +' .. ms .. 'ms bone=' .. string.format('%.2f %.2f %.2f', bp.x, bp.y, bp.z) .. ' entity=' .. string.format('%.2f %.2f %.2f', ep.x, ep.y, ep.z))
        else log('STUMP_PROBE +' .. ms .. 'ms bonePos=n/a') end
      end)
    end
    probe(300) probe(3000)
    -- the effect is removed after 20 s so it cannot outlive the corpse
    Script.SetTimer(20000, function()
      local w = currentStump61()
      if w and destroyStump(w, tag) then leases[cut] = nil end
    end)
  end
  if mode ~= 'off' and bone then worldBlood(v, cut) end
  log('STUMP ' .. cut .. ' mode=' .. mode .. ' ok=' .. tostring(ok) .. ' mesh=' .. tostring(mode == 'meat'))
  return ok
end

-- head skin path -> its wrapper character (generated into the pak for every head skin the game ships)
local function wrapperFor(skin)
  if type(skin) ~= 'string' or skin == '' then return nil end
  local p = skin:lower():gsub('\\', '/')
  local w = p:gsub('[^/]+%.skin$', 'kcd2vr_fly.cdf')
  if w == p then return nil end
  return w
end


-- T2: every other part the victim wore on the head (hair, beard, helmet, coif...) is its own wrapper character in its own slot.
-- Wrapper naming (generated into the pak for every human skin): <skin without .skin>.fly.cdf = default material,
-- <skin>.fly_<variant>.cdf = that material variant (m01, m04, ...). The variant comes from the attachment name (e.g. kettlehat07_m01_...).
-- v62.7: the material VARIANT comes first from the real material name the C++ side sends (e.g. 'm_head_034_b00.mtl' -> 'b00',
-- 'kettlehat07_m01.mtl' -> 'm01'); the attachment-name guess stays as the second source; nil = default wrapper.
local function variantOf(mtl, attName, skin)
  if type(mtl) == 'string' and mtl ~= '' then
    local m = mtl:lower():gsub('\\', '/'):gsub('%.mtl$', '')
    local stem = type(skin) == 'string' and skin:lower():gsub('\\', '/'):gsub('%.skin$', ''):match('([^/]+)$') or nil
    if stem and m:match('([^/]+)$') == stem then return nil end            -- the skin's own default material
    local v = m:match('_([a-z]%d%d)$') or m:match('_([a-z]%d%d)_[^/]*$')
    if v then return v end
  end
  return type(attName) == 'string' and attName:lower():match('_([mb]%d%d)_') or nil
end
local function partWrapper(skin, attName, mtl)
  local base = skin:lower():gsub('\\', '/'):gsub('%.skin$', '')
  return base, variantOf(mtl, attName, skin)
end
local function addParts(h, e, centre)
  local n, slot, seen = 0, 2, {}
  seen[(e.headSkin or ''):lower()] = true
  for _, a in ipairs(e.att or {}) do
    local s = a.s
    if type(s) == 'string' and s:find('%.skin$') and slot <= 9 and not seen[s:lower()] then
      seen[s:lower()] = true
      local base, variant = partWrapper(s, a.n, a.m)
      local tries = {}
      if variant then tries[#tries + 1] = base .. '.fly_' .. variant .. '.cdf' end
      tries[#tries + 1] = base .. '.fly.cdf'
      for _, cdf in ipairs(tries) do
        local ok, ret = pcall(function() return h:LoadCharacter(slot, cdf) end)
        local valid = false
        pcall(function() valid = h:IsSlotValid(slot) and h:IsSlotCharacter(slot) end)
        log('FLY part ' .. tostring(a.n) .. ' wrapper=' .. cdf .. ' mtl=' .. tostring(a.m) .. ' ok=' .. tostring(ok) .. ' ret=' .. tostring(ret) .. ' valid=' .. tostring(valid))
        if valid then
          pcall(function() h:SetSlotPos(slot, { x = -centre.x, y = -centre.y, z = -centre.z }) end)
          slot = slot + 1 n = n + 1
          break
        end
      end
    end
  end
  return n
end
-- H41: each flying-head carrier keeps an exact incarnation lease through partial failure.
-- Removal is acknowledged only by GetEntity readback; failed cleanup stays retryable.
G.headCarriersH41 = G.headCarriersH41 or {}
local headCarriersH41 = G.headCarriersH41
local function currentHeadH41(h)
  local lease = headCarriersH41[h]
  return lease and lease.active and currentEntity61(h) or nil
end
local function releaseHeadH41(h)
  local lease = headCarriersH41[h]
  if not lease then return true end
  lease.active = false
  local ok, same = pcall(function() return currentEntity61(h) end)
  if not ok then return false end
  if not same then headCarriersH41[h] = nil return true end
  pcall(function() System.RemoveEntity(h.id) end)
  ok, same = pcall(function() return currentEntity61(h) end)
  if ok and not same then headCarriersH41[h] = nil return true end
  return false
end
local function releaseHeadsH41()
  local list = {}
  for h in pairs(headCarriersH41) do list[#list + 1] = h end
  for _, h in ipairs(list) do releaseHeadH41(h) end
end
local BLOOD = 'collisions.combat.blood_head'
local function bloodTrail(h)
  -- the game's own head-hit blood effect rides the rigid body for a few seconds
  local left = 6
  local function tick()
    local w = currentHeadH41(h)
    if not w then return end
    pcall(function() w:LoadParticleEffect(10, BLOOD, { Scale = 1.0, CountScale = 1.0 }) end)
    left = left - 1
    if left > 0 then Script.SetTimer(500, tick) end
  end
  pcall(tick)
end

-- a rigid carrier (invisible meat, own mass) with a wrapper character in slot 1 that wears ONLY the cut-off head
local function flyHead(e, forceGeneric)
  local centre = { x = 0, y = 0, z = e.female and 1.55 or 1.65 }
  local name = 'kcd2vr_gore_head_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999))
  local h = nil
  local ok = try('flying head', function()
    h = System.SpawnEntity({ class = 'BasicEntity', name = name, position = vec(e.p),
      properties = { object_Model = MEAT, Physics = { bPhysicalize = true, bRigidBody = true, bPushableByPlayers = true, bResting = 0, Density = -1, Mass = MASS.head } } })
    if not h then error('SpawnEntity returned nil') end
    headCarriersH41[h] = { active = true }
    local how, cdf = 'none', wrapperFor(e.headSkin)
    local ret = nil
    local valid = false
    if cdf and not forceGeneric then
      -- v62.7: real material first. <head>.fly_<variant>.cdf exists in the pak for every shipped material of that head.
      local tries = {}
      local variant = variantOf(e.headMtl, nil, e.headSkin)
      local base = e.headSkin:lower():gsub('\\', '/'):gsub('%.skin$', '')
      -- v62.8: the head-only piece (cut at the Neck1/Head weights = C3) from kcd2vr_gore_heads.pak comes first, same material rule;
      -- the full head (with the neck) stays as the fallback when the heads pak is not installed.
      if G.c3Head ~= false then
        if variant then tries[#tries + 1] = base .. '_kcdghead.fly_' .. variant .. '.cdf' end
        tries[#tries + 1] = base .. '_kcdghead.fly.cdf'
      end
      if variant then tries[#tries + 1] = base .. '.fly_' .. variant .. '.cdf' end
      tries[#tries + 1] = cdf
      for _, c in ipairs(tries) do
        ret = h:LoadCharacter(1, c)
        valid = false
        local read = pcall(function() valid = h:IsSlotValid(1) and h:IsSlotCharacter(1) end)
        valid = read and valid
        log('FLY loadCharacter wrapper=' .. c .. ' headMtl=' .. tostring(e.headMtl) .. ' ret=' .. tostring(ret) .. ' slotCharacter=' .. tostring(valid))
        if valid then cdf = c break end
      end
      how = valid and ((cdf:find('_kcdghead', 1, true) and 'c3head' or 'wrapper') .. (variant and '_' .. variant or '')) or 'wrapper_FAILED'
    end
    if not valid then
      -- fallback: the game's default male/female character shows its own default head (body attachment hidden)
      local sk = e.female and SKEL.female or SKEL.male
      ret = h:LoadCharacter(1, sk)
      valid = false
      local read = pcall(function() valid = h:IsSlotValid(1) and h:IsSlotCharacter(1) end)
      valid = read and valid
      pcall(function() h:HideAttachment(1, 'body', true, true) end)
      how = valid and 'generic_default_head' or 'generic_FAILED'
      log('FLY loadCharacter generic=' .. sk .. ' ret=' .. tostring(ret) .. ' slotCharacter=' .. tostring(valid))
    end
    if not valid then error('no loaded head character slot; carrier rollback required') end
    if not G.debugCarrier then h:DrawSlot(0, 0) end
    h:SetSlotPos(1, { x = -centre.x, y = -centre.y, z = -centre.z })
    local extra = addParts(h, e, centre)
    bloodTrail(h)
    -- v62.7: the head bleeds from its cut face too: the same blood effect in slot 11 at the neck end of the head (local -centre + 0.08 m).
    pcall(function()
      h:LoadParticleEffect(11, BLOOD, { Scale = 0.8, CountScale = 1.2 })
      h:SetSlotPos(11, { x = 0, y = 0, z = -0.12 })
    end)
    local d = vec(e.d)
    local k = PUSH.head * (0.5 + 0.5 * math.max(0, math.min(1, num(e.energy, 1))))
    h:AddImpulse(-1, h:GetWorldPos(), d, k, 1)
    -- v62.4: the wrapper spawn path of a corpse head moved only 0.2 m in 2 s (impulse lost, Miki's 20 Oct log). Check once; push again if it did not move.
    do
      local pStart = h:GetWorldPos()
      Script.SetTimer(250, function()
        local w = currentHeadH41(h)
        if not w then return end
        local q = w:GetWorldPos()
        local dx, dy, dz = q.x - pStart.x, q.y - pStart.y, q.z - pStart.z
        if (dx * dx + dy * dy + dz * dz) < 0.09 then
          pcall(function() w:AddImpulse(-1, q, d, k * 1.5, 1) end)
          log('FLY head did not move, impulse repeated x1.5')
        end
      end)
    end
    local id = h.id
    local p0 = h:GetWorldPos()
    log('FLY head spawned how=' .. how .. ' id=' .. tostring(id) .. ' headSkin=' .. tostring(e.headSkin) .. ' carrierVisible=' .. tostring(G.debugCarrier) ..
        ' at=' .. string.format('%.2f %.2f %.2f', p0.x, p0.y, p0.z) .. ' push=' .. string.format('%.1f', k))
    local function probe(ms)
      Script.SetTimer(ms, function()
        local w = currentHeadH41(h)
        if not w then log('FLY probe +' .. ms .. 'ms: entity gone') return end
        local p = w:GetWorldPos()
        local sv, sc = 'n/a', 'n/a'
        pcall(function() sv = tostring(w:IsSlotValid(1)) end)
        pcall(function() sc = tostring(w:IsSlotCharacter(1)) end)
        log('FLY probe +' .. ms .. 'ms: at=' .. string.format('%.2f %.2f %.2f', p.x, p.y, p.z) .. ' moved=' ..
            string.format('%.2f', math.sqrt((p.x - p0.x) ^ 2 + (p.y - p0.y) ^ 2 + (p.z - p0.z) ^ 2)) .. ' slot1valid=' .. sv .. ' slot1character=' .. sc)
      end)
    end
    probe(400) probe(2000)
    Script.SetTimer(LIFE_MS, function() releaseHeadH41(h) end)
  end)
  if not ok and h then releaseHeadH41(h) end
  return ok
end

-- v62.9: the game's own neck stump on the body side (male only - the mesh is on the male skeleton)
G.neckStump = (G.neckStump ~= false)
G.neckStumpCdf = G.neckStumpCdf or 'objects/characters/humans/male/head/m_head_decapitated/kcd2vr_neckstump.cdf'
G.neckStumpBone = G.neckStumpBone or 'Neck'
G.neckStumpPos = G.neckStumpPos or { x = -1.3697, y = 0, z = 0.4887 }      -- inverse Neck bind pose (W2B) from the skin's bone table
G.neckStumpAngles = G.neckStumpAngles or { x = 108.8, y = 0, z = 90 }      -- same, as xyz euler degrees (Rz*Ry*Rx)
G.neckStumpLeases = G.neckStumpLeases or setmetatable({}, { __mode = 'k' })
local function neckStump(v, e)
  if not G.neckStump then return false end
  if e.female then log('NECKSTUMP skipped: female (no female stump mesh yet)') return false end
  if G.neckStumpLeases[v] then log('NECKSTUMP already on this victim') return true end
  local tag = 'kcd2vr_neckstump'
  local helper = nil
  local ok = try('neck stump', function()
    local pos = v:GetWorldPos()
    helper = System.SpawnEntity({ class = 'BasicEntity', name = 'kcd2vr_neckstump_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999)),
      position = vec(pos), properties = {} })
    if not helper then error('helper spawn returned nil') end
    local ret = helper:LoadCharacter(0, G.neckStumpCdf)
    local valid = false
    pcall(function() valid = helper:IsSlotValid(0) and helper:IsSlotCharacter(0) end)
    if not valid then error('stump wrapper did not load: ' .. tostring(G.neckStumpCdf) .. ' ret=' .. tostring(ret)) end
    v:CreateBoneAttachment(0, G.neckStumpBone, tag)
    v:SetAttachmentPos(0, tag, vec(G.neckStumpPos))
    v:SetAttachmentAngles(0, tag, vec(G.neckStumpAngles))
    v:SetAttachmentObject(0, tag, helper.id, 0, 0)
  end)
  if not ok then
    if helper then pcall(function() System.RemoveEntity(helper.id) end) end
    pcall(function() v:DestroyAttachment(0, tag) end)
    return false
  end
  local lease = { helper = helper, tag = tag }
  G.neckStumpLeases[v] = lease
  Script.SetTimer(LIFE_MS, function()
    if G.neckStumpLeases[v] ~= lease then return end
    G.neckStumpLeases[v] = nil
    local w = currentEntity61(v)
    if w then pcall(function() w:DestroyAttachment(0, tag) end) end
    local h = currentEntity61(helper)
    if h then pcall(function() System.RemoveEntity(h.id) end) end
  end)
  log('NECKSTUMP target=' .. tostring(e.target) .. ' helper=' .. tostring(helper.id) .. ' bone=' .. G.neckStumpBone .. ' cdf=' .. G.neckStumpCdf)
  return true
end
local function releaseNeckStumps()
  for v, lease in pairs(G.neckStumpLeases) do
    G.neckStumpLeases[v] = nil
    local w = currentEntity61(v)
    if w then pcall(function() w:DestroyAttachment(0, lease.tag) end) end
    local h = currentEntity61(lease.helper)
    if h then pcall(function() System.RemoveEntity(h.id) end) end
  end
end
-- console: put the stump on the nearest npc for 15 s (hides his head from Lua for the test only; the real decap hides it natively)
function G.TestNeckStump()
  local p = player:GetWorldPos()
  local best, bd = nil, 4
  for _, en in ipairs(System.GetEntitiesInSphere(p, 4) or {}) do
    if en ~= player and en.GetBonePos then
      local okb, bp = pcall(function() return en:GetBonePos('Neck') end)
      if okb and bp then local d = math.sqrt((bp.x - p.x) ^ 2 + (bp.y - p.y) ^ 2) if d < bd then best, bd = en, d end end
    end
  end
  if not best then log('TESTNECKSTUMP no character with a Neck bone within 4 m') return end
  pcall(function() best:HideAttachment(0, 'head', true, true) end)
  local ok = neckStump(best, { target = best.id, female = false })
  log('TESTNECKSTUMP npc=' .. tostring(best:GetName()) .. ' ok=' .. tostring(ok) .. ' (restores in 15 s)')
  Script.SetTimer(15000, function()
    local w = currentEntity61(best) if not w then return end
    pcall(function() w:HideAttachment(0, 'head', false, true) end)
    local lease = G.neckStumpLeases[w]
    if lease then G.neckStumpLeases[w] = nil pcall(function() w:DestroyAttachment(0, lease.tag) end)
      local h = currentEntity61(lease.helper) if h then pcall(function() System.RemoveEntity(h.id) end) end end
  end)
end

local function cutHead(v, e)
  local okStump = stump(v, 'head')
  local okNeck = neckStump(v, e)
  local okHead = false
  if type(e.headSkin) == 'string' and e.headSkin ~= '' then okHead = flyHead(e, false) else log('HEAD no headSkin: nothing to fly') end
  -- Head visibility (hiding/restoring the victim's head parts) has exactly one owner: the native side (C59 leases).
  log('HEAD target=' .. tostring(e.target) .. ' stump=' .. tostring(okStump) .. ' neckStump=' .. tostring(okNeck) .. ' flyingHead=' .. tostring(okHead) .. ' headSkin=' .. tostring(e.headSkin) .. ' headMtl=' .. tostring(e.headMtl))
end

-- ===== v62.1 limb cut (hands / feet) - EXPERIMENT =====
-- The game's Lua cannot put a new skin on the victim (CreateSkinAttachment has no mesh argument), so:
--   * only SPLIT parts are used: a hands / feet skin of the victim (gloves, gauntlets, male_body_hands, shoes, boots, male_body_feet...). Those are hidden whole,
--     which means BOTH hands (or both feet) of that skin are gone from the victim, only the cut side flies off. This is an opt-out visual experiment, not finished dismemberment.
--   * the cut side flies off as <piece>.fly.cdf (piece = that hand/foot only, listed in zzkcd2vr_gore_meshes.lua).
-- Every delayed step belongs to the exact victim entity object AND to one lease token of that cut (same pattern as the stump): a reused numeric id,
-- a load, or Clear()/ReleaseAll() makes the timers stop and shows again only what this lease hid (no load callback is proven: the same object across a load needs a native ReleaseAll call).
G.limbExperiment = (G.limbExperiment == true)   -- v62.2: OWN default-off gate. Heads work without it. console: KCD2VR_Gore.limbExperiment=true
G.limbHideVictim = (G.limbHideVictim ~= false)   -- console: KCD2VR_Gore.limbHideVictim=false  keeps the victim parts visible (test only)
G.limbLeases = G.limbLeases or setmetatable({}, { __mode = 'k' })   -- [entity object] = { cuts = { [cut] = leaseToken }, hidden = { [attName] = true } }
G.limbToken = G.limbToken or 0
local limbLeases = G.limbLeases
local MASS_LIMB = { lh = 1, rh = 1, lf = 1.5, rf = 1.5 }
local PUSH_LIMB = { lh = 9, rh = 9, lf = 11, rf = 11 }
local FLY_LIMB_MAX_SLOTS = 8

local function splitPart(cut, skin)
  local p = skin:lower():gsub('\\', '/')
  if cut == 'lh' or cut == 'rh' then return p:find('/hands/', 1, true) ~= nil or p:find('_hands', 1, true) ~= nil end
  return p:find('/feet/', 1, true) ~= nil or p:find('_feet', 1, true) ~= nil
end

local function pieceWrapper(pieceSkin, attName)
  local base = pieceSkin:lower():gsub('\\', '/'):gsub('%.skin$', '')
  local variant = type(attName) == 'string' and attName:lower():match('_([mb]%d%d)_') or nil
  local tries = {}
  if variant then tries[#tries + 1] = base .. '.fly_' .. variant .. '.cdf' end
  tries[#tries + 1] = base .. '.fly.cdf'
  return tries
end

-- Spawns one rigid carrier wearing every piece wrapper (one slot each, same offset so the layers stay together).
-- Returns ok, loadedCount. On ANY failure after SpawnEntity the carrier is removed again (no orphan), nothing else is touched.
-- With 'complete' the call also fails when not every piece loaded (the caller then hides nothing).
local function flyLimb(e, pieces, centre, pos, dirv, complete)
  local name = 'kcd2vr_gore_' .. e.cut .. '_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999))
  local h, count = nil, 0
  local ok, err = pcall(function()
    local params = { class = 'BasicEntity', name = name, position = pos,
      properties = { object_Model = MEAT, Physics = { bPhysicalize = true, bRigidBody = true, bPushableByPlayers = true, bResting = 0, Density = -1, Mass = MASS_LIMB[e.cut] } } }
    if dirv then params.orientation = dirv end
    h = System.SpawnEntity(params)
    if not h then error('SpawnEntity returned nil') end
    local slot = 1
    for _, pc in ipairs(pieces) do
      if slot > FLY_LIMB_MAX_SLOTS then break end
      for _, cdf in ipairs(pieceWrapper(pc.skin, pc.att)) do
        local okLoad = pcall(function() h:LoadCharacter(slot, cdf) end)
        local valid = false
        pcall(function() valid = h:IsSlotValid(slot) and h:IsSlotCharacter(slot) end)
        if okLoad and valid then
          pcall(function() h:SetSlotPos(slot, { x = -centre.x, y = -centre.y, z = -centre.z }) end)
          slot = slot + 1 count = count + 1
          break
        end
      end
    end
    if count == 0 then error('no piece wrapper loaded (limb piece PAK missing?)') end
    if complete and count ~= #pieces then error('only ' .. count .. ' of ' .. #pieces .. ' piece layers loaded') end
    if not G.debugCarrier then h:DrawSlot(0, 0) end
    local d = vec(e.d)
    local k = PUSH_LIMB[e.cut] * (0.5 + 0.5 * math.max(0, math.min(1, num(e.energy, 1))))
    h:AddImpulse(-1, h:GetWorldPos(), d, k, 1)
    local id = h.id
    local p0 = h:GetWorldPos()
    log('FLY ' .. e.cut .. ' spawned id=' .. tostring(id) .. ' pieces=' .. count .. '/' .. #pieces .. ' at=' .. string.format('%.2f %.2f %.2f', p0.x, p0.y, p0.z) .. ' push=' .. string.format('%.1f', k))
    local function probe(ms)
      Script.SetTimer(ms, function()
        local w = currentEntity61(h)
        if not w then log('FLY probe +' .. ms .. 'ms: entity gone') return end
        local p = w:GetWorldPos()
        log('FLY probe +' .. ms .. 'ms: moved=' .. string.format('%.2f', math.sqrt((p.x - p0.x) ^ 2 + (p.y - p0.y) ^ 2 + (p.z - p0.z) ^ 2)))
      end)
    end
    probe(400) probe(2000)
    Script.SetTimer(LIFE_MS, function() if currentEntity61(h) then System.RemoveEntity(id) end end)
  end)
  if not ok then
    if h then pcall(function() System.RemoveEntity(h.id) end) end
    log('flying ' .. e.cut .. ' failed: ' .. tostring(err))
    return false, count
  end
  return true, count, h
end

-- hide again a few times (equipment refresh may show the part): only while the victim is the SAME entity object and this cut's lease is still the active one
local function keepHiddenLimb(v, lease, cut, token, names)
  local left = 8
  local function tick()
    if currentEntity61(v) ~= v then return end
    if lease.cuts[cut] ~= token then return end
    for _, n in ipairs(names) do pcall(function() v:HideAttachment(0, n, true, true) end) end
    left = left - 1
    if left > 0 then Script.SetTimer(1000, tick) end
  end
  Script.SetTimer(250, tick)
end

local function cutLimb(v, e, paired)
  -- v62.6: `paired` is decided LOCALLY by G.Cut (kill cut 'hands'/'feet'), never taken from the event table.
  -- paired: BOTH sides (lh+rh or lf+rf): every twin is created first, the victim is hidden only when ALL exist, any failure removes every carrier.
  local cut = e.cut
  if not (cut == 'lh' or cut == 'rh' or cut == 'lf' or cut == 'rf') then say('gore: no cut piece for ' .. tostring(cut) .. ' yet (hands/feet/head only)') return end
  if not (G.limbExperiment or paired) then log('LIMB cut=' .. cut .. ' OFF: limb experiment is disabled (KCD2VR_Gore.limbExperiment=true to try it; hides both hands/feet of the victim)') return end
  local sides = { cut }
  if paired then sides = (cut == 'lh' or cut == 'rh') and { 'lh', 'rh' } or { 'lf', 'rf' } end
  local lease = limbLeases[v]
  if not lease then lease = { cuts = {}, hidden = {} } limbLeases[v] = lease end
  for _, sd in ipairs(sides) do
    if lease.cuts[sd] then log('LIMB target=' .. tostring(e.target) .. ' cut=' .. sd .. ' already cut: nothing more to do') return end
  end
  local per, hide, seen, sawBody = {}, {}, {}, false
  for _, sd in ipairs(sides) do per[sd] = { pieces = {}, centre = nil } end
  for _, a in ipairs(e.att or {}) do
    local key = type(a.s) == 'string' and a.s:lower():gsub('\\', '/') or ''
    local var = G.V[key]
    if key ~= '' then
      -- v62.6b: validate EVERY selected split layer (not aggregate per-side counts) before any spawn or hide
      if paired and (splitPart(sides[1], key) or splitPart(sides[2], key)) then
        for _, sd in ipairs(sides) do
          if not (var and type(var[sd .. 'p']) == 'string' and var[sd .. 'p'] ~= '') then
            say('gore: cut not available for ' .. sd .. ' (' .. tostring(a.n) .. ' has no piece)')
            return
          end
        end
      end
      local split = false
      for _, sd in ipairs(sides) do
        if splitPart(sd, key) then
          split = true
          if var and var[sd .. 'p'] then
            local pp = per[sd]
            pp.pieces[#pp.pieces + 1] = { skin = var[sd .. 'p'], att = a.n }
            pp.centre = pp.centre or var['c_' .. sd .. 'p']
            if not seen[a.n] then seen[a.n] = true hide[#hide + 1] = a.n end
          end
        end
      end
      if not split then sawBody = true end
    end
  end
  for _, sd in ipairs(sides) do
    if #per[sd].pieces == 0 then
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. sd .. ' REFUSED: ' .. (paired and 'paired cut needs BOTH sides, ' or '') .. 'victim has no usable separate ' .. ((sd == 'lh' or sd == 'rh') and 'hands' or 'feet') ..
          ' piece (' .. (sawBody and 'one-piece body/legs mesh cannot be split by script' or 'no usable skin in the event') .. '); victim untouched')
      return
    end
  end
  local dirv = nil
  pcall(function() local d = v:GetDirectionVector() if d and num(d.x, nil) and num(d.y, nil) then dirv = { x = d.x, y = d.y, z = 0 } end end)
  -- complete set or nothing: all flying twins first
  local carriers, nfly = {}, 0
  local function dropCarriers() for _, c in ipairs(carriers) do pcall(function() System.RemoveEntity(c.id) end) end carriers = {} end
  for _, sd in ipairs(sides) do
    local pp = per[sd]
    local pos = nil
    pcall(function() pos = v:GetBonePos(BONE[sd]) end)
    if not pos then pos = vec(e.p) end
    local es = {}
    for k, val in pairs(e) do es[k] = val end
    es.cut = sd
    local okFly, n, carrier = flyLimb(es, pp.pieces, pp.centre or { x = 0, y = 0, z = 0 }, pos, dirv, G.limbHideVictim)
    if not okFly then
      dropCarriers()
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. sd .. ' REFUSED: flying pieces could not be created completely' .. (paired and ' (paired: all carriers removed)' or '') .. '; victim hands/feet left visible')
      return
    end
    if carrier then carriers[#carriers + 1] = carrier end
    nfly = nfly + n
  end
  G.limbToken = G.limbToken + 1
  local token = G.limbToken
  for _, sd in ipairs(sides) do lease.cuts[sd] = token end
  local hidden = 0
  if G.limbHideVictim then
    -- commit / rollback: if any hide fails, undo exactly the hides THIS call made, drop the token and remove every flying twin (no duplicate hand)
    local done, failed = {}, nil
    for _, nm in ipairs(hide) do
      if try('hide ' .. nm, function() v:HideAttachment(0, nm, true, true) end) then
        done[#done + 1] = nm
      else failed = nm break end
    end
    if failed then
      for _, nm in ipairs(done) do
        if not pcall(function() v:HideAttachment(0, nm, false, false) end) then
          lease.hidden[nm] = true   -- v62.6c: undo failed, keep the name so ReleaseAll retries it
          log('LIMB rollback: could not show ' .. tostring(nm) .. ' again; kept in lease for retry')
        end
      end
      for _, sd in ipairs(sides) do lease.cuts[sd] = nil end
      dropCarriers()
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. cut .. ' ROLLED BACK: hide of ' .. tostring(failed) .. ' failed; ' .. #done .. ' hides undone, flying piece(s) removed')
      return
    end
    for _, nm in ipairs(done) do lease.hidden[nm] = true end
    hidden = #done
    keepHiddenLimb(v, lease, sides[1], token, hide)
  end
  local okStump = true
  for _, sd in ipairs(sides) do okStump = stump(v, sd) and okStump end
  log('LIMB target=' .. tostring(e.target) .. ' cut=' .. table.concat(sides, '+') .. ' paired=' .. tostring(paired == true) .. ' stump=' .. tostring(okStump) .. ' flyingPieces=' .. tostring(nfly) .. ' carriers=' .. #carriers .. ' victimPartsHidden=' .. hidden .. '/' .. #hide .. ' (both sides of that hands/feet skin are hidden: engine limit)')
end

-- Release one lease: only when the CURRENT entity for that object is the same object and the lease is still the registered one.
-- A replaced/dead entity is discarded without touching anything (its replacement is not ours).
local function releaseLimbLease(en, lease)
  if limbLeases[en] ~= lease then return false end
  lease.cuts = {}   -- every pending delayed hide of this victim stops
  local restored, failed = false, 0
  if currentEntity61(en) == en then
    local names = {}
    for nm in pairs(lease.hidden) do names[#names + 1] = nm end
    for _, nm in ipairs(names) do
      -- v62.6c: a restore that throws keeps its name in the lease so the next ReleaseAll retries it (ownership is not dropped on failure)
      if pcall(function() en:HideAttachment(0, nm, false, false) end) then lease.hidden[nm] = nil restored = true
      else failed = failed + 1 end
    end
    if failed > 0 then
      log('RELEASE victim restore failed for ' .. failed .. ' attachment(s); lease kept for retry')
      return restored, true   -- second value: lease RETAINED for retry
    end
  end
  lease.hidden = {}
  limbLeases[en] = nil
  return restored, false
end
-- All tracked leases, any distance, no player needed. For mode off / world teardown the native side (or the console) calls KCD2VR_Gore.ReleaseAll().
function G.ReleaseAll()
  releaseHeadsH41() -- includes failed partial carriers; no victim visibility write
  releaseNeckStumps()   -- v62.9
  local list = {}
  for en, lease in pairs(limbLeases) do list[#list + 1] = { en, lease } end
  local n = 0
  local retained, dropped = 0, 0
  for _, p in ipairs(list) do
    local ok, kept = releaseLimbLease(p[1], p[2])
    if ok then n = n + 1 end
    if kept then retained = retained + 1 else dropped = dropped + 1 end
  end
  -- stump blood attachments: remove each owned one from its exact still-current entity BEFORE dropping the lease; replaced entity -> drop only; failed removal -> keep
  local sl = {}
  for en, leases in pairs(stumpLeases61) do sl[#sl + 1] = { en, leases } end
  local stumpsGone, stumpsKept = 0, 0
  for _, p in ipairs(sl) do
    local en, leases = p[1], p[2]
    local cuts = {}
    for cut in pairs(leases) do cuts[#cuts + 1] = cut end
    for _, cut in ipairs(cuts) do
      if currentEntity61(en) == en then
        if destroyStump(en, 'kcd2vr_stump_' .. cut) then leases[cut] = nil stumpsGone = stumpsGone + 1 else stumpsKept = stumpsKept + 1 end
      else leases[cut] = nil end
    end
    if next(leases) == nil then stumpLeases61[en] = nil end
  end
  log('RELEASEALL limb leases attempted=' .. #list .. ' dropped=' .. dropped .. ' retained=' .. retained .. ' victims restored=' .. n .. ' stumps removed=' .. stumpsGone .. ' kept for retry=' .. stumpsKept)
  return #list
end

function G.Cut(e)
  if type(e) ~= 'table' or type(e.target) ~= 'number' or type(e.cut) ~= 'string' then log('bad event') return end
  local v = System.GetEntity(e.target)
  if not v then log('target ' .. tostring(e.target) .. ' not found') return end
  -- v62.5: 'hands' / 'feet' = the kill cut for BOTH sides (the engine hides both sides of that skin anyway, so this is the honest event for it)
  local paired = false
  if e.cut == 'hands' then e.cut = 'lh' paired = true elseif e.cut == 'feet' then e.cut = 'lf' paired = true end
  if e.cut == 'head' then cutHead(v, e) else cutLimb(v, e, paired) end
end

-- Console test, no kill needed:  #KCD2VR_Gore.Test('head')   (wrapper character, Mod's own cdf)
--                                 #KCD2VR_Gore.Test('generic') (game default male head, no wrapper)
--                                 #KCD2VR_Gore.Test('carrier') (visible meat only: proves the physics body flies)
--                                 #KCD2VR_Gore.Test('hat')     (head + kettle hat + blood trail: proves the extra-part slots)
function G.Test(kind, skin)
  kind = kind or 'head'
  local parts = nil
  if kind == 'hat' then
    kind = 'head'
    parts = { { n = 'clothing_head_m01_test', s = 'objects/characters/humans/male/clothing/head/helmet/kettle_hat/kettlehat07/kettlehat07.skin' } }
  end
  local pos, dir = { x = 0, y = 0, z = 0 }, { x = 0, y = 1, z = 0 }
  local okp = pcall(function() pos = player:GetWorldPos() dir = player:GetDirectionVector() end)
  if not okp then log('TEST cannot read the player entity (global "player")') return end
  local e = { target = 0, cut = 'head', female = false, energy = 1, headSkin = skin or DEFAULT_HEAD,
    att = parts, p = { x = pos.x + dir.x * 1.5, y = pos.y + dir.y * 1.5, z = pos.z + 1.3 }, d = { x = dir.x * 0.3, y = dir.y * 0.3, z = 0.6 } }
  local old = G.debugCarrier
  if kind == 'carrier' then G.debugCarrier = true end
  log('TEST ' .. kind .. ' headSkin=' .. tostring(e.headSkin))
  flyHead(e, kind == 'generic')
  G.debugCarrier = old
end

-- Console test: #KCD2VR_Gore.TestStump()  -> puts the stump (blood on the Neck bone) on the nearest NPC within 4 m and logs STUMP_PROBE.
-- #KCD2VR_Gore.TestStump('meat') uses the old meat cap instead. Remove with #KCD2VR_Gore.Clear() (destroys the attachment on all NPCs near you).
function G.TestStump(mode)
  local old = G.stumpMode
  if mode then G.stumpMode = mode end
  local pos = nil
  pcall(function() pos = player:GetWorldPos() end)
  if not pos then log('TESTSTUMP cannot read player') G.stumpMode = old return end
  local list = System.GetEntitiesInSphere(pos, 4) or {}
  local best, bd = nil, 1e9
  for _, en in ipairs(list) do
    if en.id ~= player.id and en.GetBonePos then
      local okb, bp = pcall(function() return en:GetBonePos('Neck') end)
      if okb and bp then
        local p = en:GetWorldPos()
        local d = (p.x - pos.x) ^ 2 + (p.y - pos.y) ^ 2
        if d < bd then best, bd = en, d end
      end
    end
  end
  if not best then log('TESTSTUMP no character with a Neck bone within 4 m') G.stumpMode = old return end
  log('TESTSTUMP target=' .. tostring(best.id) .. ' mode=' .. tostring(G.stumpMode))
  stump(best, 'head')
  G.stumpMode = old
end
-- Console test, no kill needed:  #KCD2VR_Gore.TestLimb('lh')  ('lh' | 'rh' | 'lf' | 'rf') -> a flying hand/foot (male_body_hands / male_body_feet) 1.5 m in front of you.
function G.TestLimb(cut)
  cut = cut or 'lh'
  local pos, dir = nil, nil
  pcall(function() pos = player:GetWorldPos() dir = player:GetDirectionVector() end)
  if not pos then log('TESTLIMB cannot read the player entity (global "player")') return end
  local hand = (cut == 'lh' or cut == 'rh')
  local key = hand and 'objects/characters/humans/male/body/male_body_hands.skin' or 'objects/characters/humans/male/body/male_body_feet.skin'
  local var = G.V[key]
  if not (var and var[cut .. 'p']) then log('TESTLIMB no piece for ' .. cut .. ' (is zzkcd2vr_gore_meshes.lua in the pak?)') return end
  local e = { target = 0, cut = cut, energy = 1, d = { x = dir.x * 0.3, y = dir.y * 0.3, z = 0.6 } }
  local ok, n = flyLimb(e, { { skin = var[cut .. 'p'], att = 'test' } }, var['c_' .. cut .. 'p'] or { x = 0, y = 0, z = 0 },
    { x = pos.x + dir.x * 1.5, y = pos.y + dir.y * 1.5, z = pos.z + 1.3 }, { x = dir.x, y = dir.y, z = 0 }, true)
  log('TESTLIMB ' .. cut .. ' ok=' .. tostring(ok) .. ' pieces=' .. tostring(n))
end
function G.Clear()
  G.ReleaseAll()   -- v62.2: first, independent of distance and of the player entity
  local pos = nil
  pcall(function() pos = player:GetWorldPos() end)
  if pos then
    for _, en in ipairs(System.GetEntitiesInSphere(pos, 8) or {}) do
      for _, c in ipairs({ 'head', 'lh', 'rh', 'lf', 'rf' }) do
        pcall(function() en:DestroyAttachment(0, 'kcd2vr_stump_' .. c) end)
      end
    end
  end
  log('CLEAR leases released; stump attachments near the player destroyed')
end

log('loaded version ' .. G.version .. ' protocol ' .. G.protocol)
