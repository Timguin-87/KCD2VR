-- v62.32 (Claude, 28 Oct 2026, Miki: "finally working dismemberment of one chosen arm or leg"): ELBOW / KNEE cuts with the D41 VR mod.
--   The VR mod binds the limbs41 pak's cut copies (<skin>_kcdg<side>.skin: forearm/shin folded into a closed cap at the joint) as its own
--   skin attachments kcd2vr_cut_<layer> on the DEAD victim and hides the original layers, natively. This script only adds what is left:
--   the severed forearm / shin flies off (<skin>_kcdg<side>p.skin pieces in .fly.cdf wrappers) and the elbow / knee bleeds.
--   Event e.lvl == 1 (sides la ra ll rl from e.cut lh rh lf rf). KCD2VR_Gore.limbs41=false = no flying piece / blood (the cut stays native).
--   The old wrist/ankle path (G.limbs) is unchanged and still off.
-- v62.31 (Claude, 28 Oct): Miki's own head stump (made in Blender, fitted to the head) replaces the meat piece on the flying male head.
--   D70G: its measured model-space centre is above the neck; apply the reviewed D71G neck placement below.
--   KCD2VR_Gore.headStumpCgf='' = old meat piece; female heads keep the meat piece (no female fit yet).
-- v62.30 (Claude): flying head flesh cap centred on the real neck hole and turned to match it (meat cgf is twisted 25 deg), size 1.0.
-- v62.28 (Claude): V3 Neck-local stump CGF shipped (correct bytes), opt-in; default body stump = game skinned helper.
-- v62.26 (Claude): limb (hand/foot) cuts off unless KCD2VR_Gore.limbs=true; decap, neck cap, belly unchanged.
-- v62.25 (Claude): flesh cap on the flying head open neck end (vanilla meat_piece.cgf, slot 12). v62.29 centred+tilted from mesh data.
-- v62.24 (Claude, lane ST): CGF stump OFF (wrong bake); default = the game's skinned stump wrapper; one NECKSTUMP route= line per decap.
-- v62.23 (Claude): default neck stump = static m_head_decapitated CGF on the victim Neck bone (DeepSeek conversion); no helper, no follow.
-- KCD2VR Gore (optional add-on for the KCD2VR VR mod). Own script, protocol 1.
-- v62.22 (Claude, lane GS): optional rigid CGF neck stump on the victim's own Neck bone attachment (KCD2VR_Gore.neckStumpCgf, off).

-- v62.21 (Claude, 26 Oct 2026, Codex N1 review): (1) the neck stump had TWO owners when follow+calibration succeeded (a bone attachment
--   AND the script follow moving the same helper) - only follow=false binds an attachment now. (2) the kept hand follows by a frame
--   from bone positions too (Hand->Middle1 + Index1-Pinky1), feet keep the old path. KCD2VR_Gore.keepPosFrame=false = old.
--   (3) hair/beard tint stays ON as an approximation (the victim's own colour is not readable yet): KCD2VR_Gore.hairTint=false off.
-- v62.20 (Claude, 26 Oct 2026, Miki's R14 run + screenshot): (1) grey beards/hair on the flying head = the game tints hair per NPC at
--   runtime (Hair shader Diffuse 1,1,1 in every vanilla hair/beard mtl); tinted copies ship in the pak (kcd2vr_tint, 4 colours,
--   picked from the victim id). (2) neck stump stood upright on corpses lying down (follow angles -0 0 1): GetBoneLocal gave no
--   ragdoll tilt, so the stump frame now comes from bone positions (Neck->Neck1 + shoulders) on helper and victim alike.
--   (3) /f_head/ head parts kept (Codex M12). console: KCD2VR_Gore.hairTint / hairTintIndex / stumpPosFrame.
-- v62.12 (Claude, 23 Oct 2026, Miki's H46 run 16:35-16:38): (1) hand/foot cuts are KILL MOVES ONLY: a cut on a living npc is checked
--   again for 0.8 s and only happens when the game has him dead, else one line and nothing. (2) NEVER two hands/feet: a paired event cuts
--   only the side nearest the hit, and if the other side can not be kept on the body the whole cut is refused (was: both fly).
--   (3) the kept hand/foot and the neck stump follow the body by script when the engine gives the bone rotation (GetBoneLocal),
--   because on a dead body (ragdoll) an entity attachment stays where he died. (4) hair/beard/hat material from the game's own
--   CharacterComponent table when the ASI does not send one (H46 sends no material: headMtl=nil, mtl=nil in kcd.log) -> no grey beard.
-- v62.11 (Claude, 23 Oct 2026): two fixes from Miki's screenshot. (1) the neck stump stayed flickering where the man was killed instead
--   of riding the body: Entity.SetAttachmentObject on a dead (ragdoll) character does not follow the bones. The stump helper is now
--   placed by script every 33 ms on the victim's Neck bone (position = GetBonePos, orientation = Neck->Neck1 axis + the shoulder line,
--   times the same inverse bind pose W2B as before). No attachment on the victim any more. KCD2VR_Gore.neckStumpFollow=false = old way.
--   (2) hair and beard on the flying head were grey (no material): the part wrappers in the pak carry no material and the real one
--   the ASI sends (att[i].m) was only used to pick a wrapper name. Every hair/beard/hat part is now its own child entity of the head
--   carrier and gets that real material with Entity.SetMaterial. KCD2VR_Gore.partMaterial=false = old slots.
-- v62.10 (Claude, 23 Oct 2026): SINGLE hand / foot cuts, ON by default. The engine can only hide a whole hands/feet skin (both sides),
--   so the cut side flies as before and the KEPT side is put back: a helper entity wears the kept side's own piece (the same generated
--   <skin>_kcdg<side>p piece from the limb paks) and is bound to the victim's wrist/ankle bone with Entity.SetAttachmentObject, offset =
--   the inverse bind pose of that bone (W2B from the skin bone table, same method as the 62.9 neck stump). The kept hand/foot rides the
--   bone rigidly (fingers in bind pose). One helper per skin layer (skin + glove). If the kept side cannot be rebuilt the cut falls back to
--   the 62.6 paired rule (both fly) instead of leaving a hole. Console: KCD2VR_Gore.singleLimb=false (paired again), KCD2VR_Gore.limbKeepPos /
--   limbKeepAngles[side] tune the bind offsets live. Stump blood at the wrist/ankle as before.
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
-- v62.13 (Claude, 23 Oct 2026, Miki's screenshot + kcd.log 45101..45105): (1) the NECK STUMP sits ON the neck: the helper measures its
--   OWN Neck bone (same wrapper skeleton) once and is placed so that bone lands exactly on the victim's Neck bone (position AND rotation),
--   no hand-typed bind offsets any more (the old W2B constants put the stump 1.4 m off - log 'follow at' vs 'STUMP_PROBE bone').
--   The stump helper has NO physics (it was a BasicEntity with default physics teleported into the ragdoll every 33 ms) and gets the
--   game's own m_head_decapitated_m01.mtl explicitly. (2) the flying head spawned INSIDE the neck (carrier at the Neck1 point, log: 'did
--   not move, impulse repeated'): it pushed the ragdoll (NPC jumping) and lay there like a brick. It now starts 25 cm out along the cut
--   direction, gets a spin so it ROLLS, damping + a sleep speed so it settles and stops costing physics. (3) the textureless egg head =
--   the generated head-only piece (_kcdghead): default back to the game's own full head skin (real face texture); G.c3Head=true = piece.
--   The head also gets its own material from the game's CharacterComponent table. (4) stump and head live 10 minutes (was 2), the native
--   head hide lease too (was 30 s - the head came back on the corpse after 30 s).
KCD2VR_Gore = KCD2VR_Gore or {}
local G = KCD2VR_Gore
G.protocol = 1
G.version = '62.35-D70G'
G.nativeMaterialD54 = true -- explicit serializer capability; old PAKs retain old events
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
-- v62.15 (Miki 24 Oct: 'the head should have physics as before, there it worked, it was broken after the hands came'):
--   the HEAD flies as a rigid body again (62.13 throw: spawned 25 cm outside the neck, spin, damping - kcd.log 24 Oct: 5 heads flew
--   1.8-2.5 m and settled). cut HANDS/FEET get NO physics: the 24 Oct rh piece shot 8.75 m in 2 s from inside the ragdoll, so the
--   piece is laid on the ground beside the wrist instead. console: KCD2VR_Gore.headPhysics=false / limbPhysics=true.
G.headPhysics = (G.headPhysics ~= false)
-- v62.17 (Miki 24 Oct: 'limbs flying too, if it does not explode or move with the npc'): hands/feet fly again, but SAFE:
--   (1) spawn OUTSIDE the body: from the wrist/ankle 22 cm away from the victim's centre line (blended with the cut direction), +5 cm up,
--   never inside the ragdoll (the 24 Oct 8.75 m shot started inside it). (2) a throw SPEED, not a raw impulse: mass x 3 m/s at full
--   energy (was 9 Ns on 1 kg = 9 m/s) with a small upward arc. (3) spin so it tumbles, damping + sleep speed so it settles and stops
--   simulating. (4) watchdog: if the piece gets more than limbMaxTravel (3.5 m) from its start or still moves after 3 s, its physics is
--   switched off and it is laid on the ground where it is - it can never fly across the map. (5) own entity, never attached to the
--   npc: it does not move with him. console: KCD2VR_Gore.limbPhysics=false = 62.16 lying pieces, limbThrowSpeed, limbSpawnOut, limbMaxTravel.
G.limbPhysics = (G.limbPhysics ~= false)
-- v62.14: head/stump belong to the corpse: they go when the corpse entity goes, at a load (ReleaseAll), or after this safety cap.
G.partLifeMs = G.partLifeMs or 3600000
local LIFE_MS = (type(G.partLifeMs) == 'number' and G.partLifeMs >= 60000) and G.partLifeMs or 3600000
local DEFAULT_HEAD = 'objects/characters/humans/male/head/m_head_034/m_head_034.skin'
-- v62.14: ground height under a point (static world + terrain only, never an actor). falls back to the victim's own position.
local function groundZ(p, victimId)
  local z = nil
  pcall(function()
    local hits = {}
    local flags = (type(ent_static) == 'number' and ent_static or 0) + (type(ent_terrain) == 'number' and ent_terrain or 0)
    if flags == 0 then return end
    local n = Physics.RayWorldIntersection({ x = p.x, y = p.y, z = p.z + 0.3 }, { x = 0, y = 0, z = -4 }, 1, flags, nil, nil, hits)
    if n and n > 0 and hits[1] and hits[1].pos and type(hits[1].pos.z) == 'number' then z = hits[1].pos.z end
  end)
  if not z and victimId then pcall(function() local v = System.GetEntity(victimId) if v then z = v:GetWorldPos().z end end) end
  return z
end
-- v62.19 (Claude, 25 Oct, Miki R11 run): a BasicEntity spawned with an EMPTY properties table keeps the retail defaults
-- (Scripts/Entities/Physics/BasicEntity.lua: object_Model = primitive_pyramid.cgf, Physics.bPhysicalize/bRigidBody = true), i.e. every
-- blood/part/stump/kept-hand/belly helper started life as a rigid pyramid INSIDE the ragdoll = exploding corpses, heads that "did not move".
-- Presentation helpers now spawn with no model (SetFromProperties returns before SetupModel) and nested physics off.
-- Only the flying head carrier and the severed-limb carriers stay rigid bodies (physicsFor below).
local function helperProps() return { object_Model = '', Physics = { bPhysicalize = false, bRigidBody = false, bPushableByPlayers = false, Density = 0, Mass = 0 } } end
local function physicsFor(mass, on)
  if on then return { bPhysicalize = true, bRigidBody = true, bPushableByPlayers = true, bResting = 0, Density = -1, Mass = mass } end
  return { bPhysicalize = false, bRigidBody = false, bPushableByPlayers = false }
end

local function log(s) System.LogAlways('KCD2VR_GORE ' .. tostring(s)) end
-- D56: a positive current male-human witness for the native pre-write gate.
local function maleHumanD56(v)
  if not v or not v.id or not v.human or not v.soul or type(enum_humanGender) ~= 'table' or
     type(enum_humanGender.male) ~= 'number' or enum_humanGender.male ~= 1 then return false end
  local human, soul = v.human, v.soul
  if type(soul.GetGender) ~= 'function' then return false end
  local ok, gender = pcall(function() return soul:GetGender() end)
  return ok and gender == enum_humanGender.male and System.GetEntity(v.id) == v and v.human == human and v.soul == soul
end
function kcd2vr_gore_eligibility_d56(packet)
  if type(packet) ~= 'string' then return 'D56|BAD_PACKET' end
  local serial, id = string.match(packet, '^D56|(%d+)|(%d+)$')
  local target = tonumber(id)
  if not serial or not target or target <= 0 then return 'D56|BAD_PACKET' end
  local v = System.GetEntity(target)
  return 'D56|' .. serial .. '|' .. id .. (maleHumanD56(v) and '|MALE_HUMAN' or '|REFUSED')
end
G.EligibleMaleHumanD56 = maleHumanD56
G.nativeEligibilityD56 = true

-- v62.13: every gore entity is NOT saved into the savegame and is registered, so a load (native ReleaseAll at the loading screen)
-- removes all of them: heads, hair/hat parts, stumps, kept hands/feet, blood helpers, flying limbs.
G.spawned = G.spawned or setmetatable({}, { __mode = 'k' })
local function spawnGore(params)
  params.flags = (type(params.flags) == 'number' and params.flags or 0) + (type(ENTITY_FLAG_NO_SAVE) == 'number' and ENTITY_FLAG_NO_SAVE or 0)
  local en = System.SpawnEntity(params)
  if en then G.spawned[en] = true end
  return en
end
-- Keep removal debt in the existing spawned registry until absence is observed.
-- false retires activity; the exact entity key remains retryable by ReleaseAll.
local function retireSpawned(en)
  if not en then return true, false end
  G.spawned[en] = false
  local ok, cur = pcall(function() return en.id and System.GetEntity(en.id) end)
  if not ok then return false, false end
  if cur ~= en then G.spawned[en] = nil return true, false end
  pcall(function() System.RemoveEntity(en.id) end)
  ok, cur = pcall(function() return en.id and System.GetEntity(en.id) end)
  if ok and cur ~= en then G.spawned[en] = nil return true, true end
  return false, false
end
local function currentSpawned(en)
  return en and G.spawned[en] == true and currentEntity61(en) or nil
end
local function removeAllSpawned()
  local n = 0
  for en in pairs(G.spawned) do
    local _, removed = retireSpawned(en)
    if removed then n = n + 1 end
  end
  return n
end
-- one short in-game line when a cut cannot run (Miki: say why where he sees it, no log-only refusals)
local function say(s) pcall(function() Game.SendInfoText(s, true, nil, 2) end) log('SAY ' .. s) end
local function try(what, f) local ok, e = pcall(f) if not ok then log(what .. ' failed: ' .. tostring(e)) end return ok end
local function num(v, d) if type(v) == 'number' and v == v and v ~= math.huge and v ~= -math.huge then return v end return d end
local function vec(t) t = t or {} return { x = num(t.x, 0), y = num(t.y, 0), z = num(t.z, 0) } end

-- v62.11 small vector / 3x3 matrix kit (matrices as three COLUMN vectors, CryEngine xyz euler = Rz*Ry*Rx, degrees)
local function vsub(a, b) return { x = a.x - b.x, y = a.y - b.y, z = a.z - b.z } end
local function vadd(a, b) return { x = a.x + b.x, y = a.y + b.y, z = a.z + b.z } end
local function vscale(a, k) return { x = a.x * k, y = a.y * k, z = a.z * k } end
local function vdot(a, b) return a.x * b.x + a.y * b.y + a.z * b.z end
local function vcross(a, b) return { x = a.y * b.z - a.z * b.y, y = a.z * b.x - a.x * b.z, z = a.x * b.y - a.y * b.x } end
local function vlen(a) return math.sqrt(vdot(a, a)) end
local function vnorm(a) local l = vlen(a) if l < 1e-6 then return nil end return vscale(a, 1 / l) end
local function mapply(m, v) return { x = m[1].x * v.x + m[2].x * v.y + m[3].x * v.z, y = m[1].y * v.x + m[2].y * v.y + m[3].y * v.z, z = m[1].z * v.x + m[2].z * v.y + m[3].z * v.z } end
local function mmul(a, b) return { mapply(a, b[1]), mapply(a, b[2]), mapply(a, b[3]) } end
local function rotXYZ(deg)
  local x, y, z = math.rad(num(deg.x, 0)), math.rad(num(deg.y, 0)), math.rad(num(deg.z, 0))
  local cx, sx, cy, sy, cz, sz = math.cos(x), math.sin(x), math.cos(y), math.sin(y), math.cos(z), math.sin(z)
  local Rx = { { x = 1, y = 0, z = 0 }, { x = 0, y = cx, z = sx }, { x = 0, y = -sx, z = cx } }
  local Ry = { { x = cy, y = 0, z = -sy }, { x = 0, y = 1, z = 0 }, { x = sy, y = 0, z = cy } }
  local Rz = { { x = cz, y = sz, z = 0 }, { x = -sz, y = cz, z = 0 }, { x = 0, y = 0, z = 1 } }
  return mmul(Rz, mmul(Ry, Rx))
end
local atan2 = math.atan2 or math.atan   -- the game's Lua is 5.1 (atan2), the mock runs 5.4 (atan with two arguments)
local function anglesXYZ(m)   -- inverse of rotXYZ, degrees
  local sy = math.max(-1, math.min(1, -m[1].z))
  return { x = math.deg(atan2(m[2].z, m[3].z)), y = math.deg(math.asin(sy)), z = math.deg(atan2(m[1].y, m[1].x)) }
end
-- v62.12: exact world rotation of a bone from the engine. Entity.GetBoneLocal(bone, dir) returns dir in the bone's frame, so the three
-- world axes give the rows of the bone rotation. Validated (orthonormal, det +1, matches GetBoneDir = bone Y) once per call; nil when not.
local function boneRot(w, bone)
  local rows = {}
  for i, d in ipairs({ { x = 1, y = 0, z = 0 }, { x = 0, y = 1, z = 0 }, { x = 0, y = 0, z = 1 } }) do
    local ok, r = pcall(function() return w:GetBoneLocal(bone, d) end)
    if not ok or type(r) ~= 'table' or not num(r.x, nil) then return nil end
    rows[i] = r
  end
  -- columns of R = bone axes in world: column k = (rows[1][k], rows[2][k], rows[3][k])
  local cx = { x = rows[1].x, y = rows[2].x, z = rows[3].x }
  local cy = { x = rows[1].y, y = rows[2].y, z = rows[3].y }
  local cz = { x = rows[1].z, y = rows[2].z, z = rows[3].z }
  local function unitish(a) local l = vlen(a) return l > .95 and l < 1.05 end
  if not (unitish(cx) and unitish(cy) and unitish(cz)) then return nil end
  if math.abs(vdot(cx, cy)) > .05 or math.abs(vdot(cy, cz)) > .05 or math.abs(vdot(cx, cz)) > .05 then return nil end
  if vdot(vcross(cx, cy), cz) < .9 then return nil end
  local okd, bd = pcall(function() return w:GetBoneDir(bone) end)
  if okd and type(bd) == 'table' and num(bd.x, nil) then
    local n = vnorm(bd)
    if n and vdot(n, cy) < .9 then return nil end   -- GetBoneDir disagrees: do not trust the rows
  end
  return { cx, cy, cz }
end

-- T3 stump: G.stumpMode = 'effect' (default): the game's own blood effect on a bone attachment of the victim, NO mesh (a bone-attached cgf floated
-- away from a corpse neck in C59). 'meat': the old 50.4 meat cap (test only). 'off': nothing.
-- Every step is logged; STUMP_PROBE compares the neck bone world position with the entity so a floating attachment shows up in the log.
G.stumpMode = G.stumpMode or 'effect'
local BONE = { head = 'Neck1', lh = 'LeftHand', rh = 'RightHand', lf = 'LeftFoot', rf = 'RightFoot' }
BONE.la, BONE.ra, BONE.ll, BONE.rl = 'LeftForeArm', 'RightForeArm', 'LeftLeg', 'RightLeg'   -- v62.32: elbow / knee   -- v62.7: Neck1 = mid neck (C3), retail joint
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
-- v62.13 (Miki: 'add blood like 2x or 3x'): one multiplier for all decap blood - particle count x mul, size +30% per extra x,
-- spray lasts mul times longer. console: KCD2VR_Gore.bloodMul = 1 (old) .. 4
G.bloodMul = G.bloodMul or 2.5
local function bloodK() return math.max(1, math.min(4, num(G.bloodMul, 2.5))) end
local function bloodFx(scale, count) local k = bloodK() return { Scale = scale * (1 + 0.3 * (k - 1)), CountScale = count * k } end
local function bloodTicks(n) return math.floor(n * bloodK() + 0.5) end
local function worldBlood(v, cut)
  local bone = stumpBone(v, cut)
  if not G.worldBlood or not bone then return end
  local left, helper = bloodTicks(8), nil
  local function stop() if helper then local hh = helper helper = nil pcall(function() System.RemoveEntity(hh.id) end) end end
  local function tick()
    local w = currentEntity61(v)
    local p = nil
    if w then pcall(function() p = w:GetBonePos(bone) end) end
    if not p then stop() log('WORLDBLOOD ' .. cut .. ' victim gone') return end
    if not helper then
      pcall(function() helper = spawnGore({ class = 'BasicEntity', name = 'kcd2vr_bloodfx_' .. tostring(math.random(1, 999999)), position = vec(p), properties = helperProps() }) end)
      if not helper then log('WORLDBLOOD ' .. cut .. ' helper spawn FAILED') return end
    else
      pcall(function() helper:SetWorldPos(vec(p)) end)
    end
    pcall(function() helper:LoadParticleEffect(0, BLOOD_STUMP, bloodFx(1.0, 1.0)) end)
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
G.partMaterial = (G.partMaterial ~= false)   -- v62.11: hair/beard/hat parts as child entities with the victim's real material (false = old slots)
-- v62.19: only head things fly with the head (hair, beard, head clothing/helmets/hoods, eyes, lashes). The ASI read a coat skin for the
-- eye_left bone socket (C++ fixed too); this is the Lua backstop. KCD2VR_Gore.partFilter = false turns it off.
G.partFilter = (G.partFilter ~= false)
-- D73: mandatory equipment boundary also protects old/external events. The
-- optional legacy partFilter cannot permit coif/hood/neck gear or other hats.
local function detachedHeadPartD73(s, name)
  local p = s:lower():gsub('\\', '/')
  local n = type(name) == 'string' and name:lower() or ''
  for _, word in ipairs({'hood','coif','cowl','neck','collar','gorget','cape'}) do
    if p:find(word, 1, true) or n:find(word, 1, true) then return false end
  end
  if not (p:find('objects/characters/humans/', 1, true) == 1 or p:find('characters/humans/', 1, true) == 1) or not p:find('%.skin$') then return false end
  if p:find('/clothing/', 1, true) then return p:find('/clothing/head/helmet/', 1, true) ~= nil end
  return p:find('/head/', 1, true) or p:find('/hair/', 1, true) or p:find('/beard', 1, true) or
    p:find('/eye', 1, true) or p:find('/teeth/', 1, true) or p:find('/tongue/', 1, true) or false
end
local function headPartPath(s, name)
  if not detachedHeadPartD73(s, name) then return false end
  if not G.partFilter then return true end
  local p = s:lower():gsub('\\', '/')
  if p:find('/torso/', 1, true) or p:find('/legs/', 1, true) or p:find('/body/', 1, true) or p:find('/arms/', 1, true) or p:find('/hands/', 1, true) or p:find('/feet/', 1, true) then return false end
  return p:find('/head/', 1, true) or p:find('/f_head/', 1, true) or p:find('/hair/', 1, true) or p:find('/beard', 1, true) or p:find('hood', 1, true) or
    p:find('coif', 1, true) or p:find('/eye', 1, true) or p:find('lash', 1, true) or false
end
-- v62.20: hair/beard materials have Diffuse 1,1,1 - the game tints them per NPC at runtime (hair colour), a loose copy
-- renders grey ("no textures"). the pak ships tinted copies (Diffuse set on the Hair submaterials, absolute texture paths)
-- in objects/characters/humans/kcd2vr_tint/<name>_t<0..3>.mtl: 0 dark brown 1 brown 2 black 3 ash. KCD2VR_Gore.hairTint=false off,
-- KCD2VR_Gore.hairTintIndex=0..3 forces one colour (else picked from the victim id).
G.hairTint = (G.hairTint ~= false)
local function tintedHairMtl(mtl, target)
  if not G.hairTint or type(mtl) ~= 'string' then return mtl end
  local p = mtl:lower():gsub('\\', '/')
  if not (p:find('/hair/', 1, true) or p:find('/beard', 1, true)) then return mtl end
  local name = p:match('([^/]+)%.mtl$')
  if not name or not (G.TintMtl and G.TintMtl[name]) then return mtl end
  local k = tonumber(G.hairTintIndex)
  if not k then k = (tonumber(target) or 0) % 4 end
  k = math.max(0, math.min(3, math.floor(k)))
  return 'objects/characters/humans/kcd2vr_tint/' .. name .. '_t' .. k .. '.mtl'
end
-- D54 names are a receipt for the synchronous native material handoff. No
-- addresses cross Lua: only an event token and the original attachment index.
local function materialTokenD54(e)
  local t = e.materialTokenD54
  if type(t) == 'string' and #t == 16 and t:match('^[0-9a-f]+$') and t ~= '0000000000000000' then return t end
  return nil
end
local function normalPathD54(p)
  return type(p) == 'string' and p:lower():gsub('\\', '/') or nil
end
local function addParts(h, e, centre, kids)
  local n, slot, seen = 0, 2, {}
  local token = materialTokenD54(e)
  seen[normalPathD54(e.headSkin) or ''] = true
  for _, a in ipairs(e.att or {}) do
    local s = normalPathD54(a.s)
    if type(s) == 'string' and s:find('%.skin$') and not headPartPath(s, a.n) then
      log('FLY part ' .. tostring(a.n) .. ' skipped: not a head part (' .. s .. ')')
      s = nil
    end
    if type(s) == 'string' and s:find('%.skin$') and slot <= 9 and not seen[s] then
      seen[s] = true
      local base, variant = partWrapper(s, a.n, a.m)
      local nativeMtl = normalPathD54(a.m)
      if nativeMtl == '' then nativeMtl = nil end
      -- Full skin identity matters: m_beard_08 under different head folders is
      -- not a globally unique table key. D54 includes inherited retail material.
      local mtl = nativeMtl or (G.PartMaterialD54 and G.PartMaterialD54[s]) or
        (G.PartMtl and G.PartMtl[s:match('([^/]+)$')]) or nil
      local tries, tried, tintedTries = {}, {}, {}
      local function addTry(cdf)
        if cdf and not tried[cdf] then tries[#tries + 1] = cdf tried[cdf] = true end
      end
      local materialWrappers = G.MaterialWrapperD54 and G.MaterialWrapperD54[s]
      local nativeWrapper = nativeMtl and materialWrappers and materialWrappers[nativeMtl]
      local function addTintTry(cdf)
        if cdf and G.hairTint and G.TintWrapperD54 and G.TintWrapperD54[cdf] then
          local k = tonumber(G.hairTintIndex)
          if not k then k = (tonumber(e.target) or 0) % 4 end
          k = math.max(0, math.min(3, math.floor(k)))
          local tinted = G.TintWrapperD54[cdf][k + 1]
          addTry(tinted)
          if tinted then tintedTries[tinted] = true end
        end
      end
      -- A native filename can still name the uncoloured base of a runtime
      -- tinted material. Keep an authored tint fallback until pointer transfer.
      if token then addTintTry(nativeWrapper) end
      addTry(nativeWrapper)
      local ordinary = {}
      if variant then ordinary[#ordinary + 1] = base .. '.fly_' .. variant .. '.cdf' end
      ordinary[#ordinary + 1] = base .. '.fly.cdf'
      for _, cdf in ipairs(ordinary) do
        -- A tint in the CA_SKIN wrapper reaches the skin's material owner;
        -- Entity.SetMaterial alone does not establish that attachment owner.
        if not nativeMtl then
          addTintTry(cdf)
          -- Inherited table material also needs a CA_SKIN owner when tint is
          -- disabled/missing; do not replace an earlier named variant choice.
          if cdf == base .. '.fly.cdf' and materialWrappers then addTry(materialWrappers[mtl]) end
        end
        addTry(cdf)
      end
      local index = a.iD54
      local named = token and type(index) == 'number' and index >= 1 and index <= 24 and index == math.floor(index)
      -- Native exact material copy needs one skin per named child, including
      -- skins whose original fallback table had no material (e.g. beard 08).
      local asChild = G.partMaterial and kids ~= nil and (named or (type(mtl) == 'string' and mtl ~= ''))
      if asChild then
        local kid = nil
        local name = named and ('kcd2vr_gore_d54_' .. token .. '_part_' .. index) or
          ('kcd2vr_gore_part_' .. tostring(e.target) .. '_' .. slot .. '_' .. tostring(math.random(1, 999999)))
        pcall(function() kid = spawnGore({ class = 'BasicEntity', name = name,
          position = vec(h:GetWorldPos()), properties = helperProps() }) end)
        if not kid then log('FLY part ' .. tostring(a.n) .. ' child spawn FAILED, falling back to a slot') end
        local loaded = nil
        if kid then
          for _, cdf in ipairs(tries) do
            local ok, ret = pcall(function() return kid:LoadCharacter(0, cdf) end)
            local valid = false
            pcall(function() valid = ok and ret ~= false and kid:IsSlotValid(0) and kid:IsSlotCharacter(0) end)
            log('FLY part ' .. tostring(a.n) .. ' child wrapper=' .. cdf .. ' mtl=' .. tostring(a.m) .. ' ok=' .. tostring(ok) .. ' ret=' .. tostring(ret) .. ' valid=' .. tostring(valid))
            if valid then loaded = cdf break end
          end
          if loaded then
            local materialSource = nativeMtl and 'NATIVE_EVENT_NAME' or 'TABLE_DEFAULT'
            local ownedWrapperMtl = G.WrapperMtlD54 and G.WrapperMtlD54[loaded]
            local wrapperMtl = ownedWrapperMtl or (G.WrapperMtlD52 and G.WrapperMtlD52[loaded])
            -- A default wrapper map is not evidence for an unknown native
            -- material. Preserve that explicit old-event compatibility hint;
            -- only an actual token tint candidate may stand in pending copy.
            if nativeMtl and loaded ~= nativeWrapper and not (token and tintedTries[loaded]) then wrapperMtl = nil end
            if wrapperMtl then mtl = wrapperMtl; materialSource = 'WRAPPER' end
            local okm, rm = true, nil
            -- Generated wrappers already bind the material at CA_SKIN. Keep
            -- SetMaterial only as compatibility for older incomplete asset packs.
            if not wrapperMtl and type(mtl) == 'string' and mtl ~= '' then
              if not nativeMtl then
                local tinted = tintedHairMtl(mtl, e.target)
                if tinted ~= mtl then materialSource = materialSource .. '_TINT_APPROXIMATION' end
                mtl = tinted
              end
              okm, rm = pcall(function() return kid:SetMaterial(mtl) end)
            end
            -- D52 counted every loaded child even if parenting/placement failed.
            -- A failed bind must use the existing common carrier-slot route.
            local bound, bindError = pcall(function()
              if h:AttachChild(kid.id, 0) == false then error('AttachChild refused') end
              if kid:SetLocalPos({ x = -centre.x, y = -centre.y, z = -centre.z }) == false then error('SetLocalPos refused') end
              if kid:SetLocalAngles({ x = 0, y = 0, z = 0 }) == false then error('SetLocalAngles refused') end
              if kid:DrawSlot(0, 1) == false then error('DrawSlot refused') end
            end)
            if bound then
              kids[#kids + 1] = kid
              slot = slot + 1 n = n + 1
              log('FLY part ' .. tostring(a.n) .. ' child=' .. tostring(kid.id) .. ' material=' .. tostring(mtl) .. ' source=' .. materialSource ..
                ' setMaterial=' .. tostring(okm and rm ~= false) .. ' bound=true nativeToken=' .. tostring(named and token or nil))
            else
              log('FLY part ' .. tostring(a.n) .. ' child bind FAILED: ' .. tostring(bindError) .. '; using carrier slot')
              loaded = nil
            end
          end
          if not loaded then
            -- Retain removal debt, and stop drawing a failed detached helper.
            pcall(function() kid:DrawSlot(0, 0) end)
            retireSpawned(kid)
            kid = nil
          end
        end
        if not loaded then asChild = false end
      end
      if not asChild then
        for _, cdf in ipairs(tries) do
          local ok, ret = pcall(function() return h:LoadCharacter(slot, cdf) end)
          local valid = false
          pcall(function() valid = ok and ret ~= false and h:IsSlotValid(slot) and h:IsSlotCharacter(slot) end)
          log('FLY part ' .. tostring(a.n) .. ' wrapper=' .. cdf .. ' mtl=' .. tostring(a.m) .. ' ok=' .. tostring(ok) .. ' ret=' .. tostring(ret) .. ' valid=' .. tostring(valid))
          if valid then
            local positioned = pcall(function()
              if h:SetSlotPos(slot, { x = -centre.x, y = -centre.y, z = -centre.z }) == false then error('SetSlotPos refused') end
              if h:DrawSlot(slot, 1) == false then error('DrawSlot refused') end
            end)
            if positioned then slot = slot + 1 n = n + 1
            else pcall(function() h:DrawSlot(slot, 0) end); log('FLY part ' .. tostring(a.n) .. ' carrier placement FAILED') end
            break
          end
        end
      end
    end
  end
  return n
end
local BLOOD = 'collisions.combat.blood_head'
local function bloodTrail(h)
  -- the game's own head-hit blood effect rides the rigid body for a few seconds
  local left = bloodTicks(6)
  local function tick()
    local w = currentSpawned(h)
    if not w then return end
    pcall(function() w:LoadParticleEffect(10, BLOOD, bloodFx(1.0, 1.0)) end)
    left = left - 1
    if left > 0 then Script.SetTimer(500, tick) end
  end
  pcall(tick)
end

-- a rigid carrier (invisible meat, own mass) with a wrapper character in slot 1 that wears ONLY the cut-off head
local function flyHead(e, forceGeneric)
  local centre = { x = 0, y = 0, z = e.female and 1.55 or 1.65 }
  local token = materialTokenD54(e)
  local name = token and ('kcd2vr_gore_d54_' .. token .. '_head') or
    ('kcd2vr_gore_head_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999)))
  -- v62.13: start OUTSIDE the neck (25 cm along the horizontal cut direction, 5 cm up) so the carrier never spawns inside the ragdoll.
  local dirH = vnorm({ x = num((e.d or {}).x, 0), y = num((e.d or {}).y, 0), z = 0 }) or { x = 1, y = 0, z = 0 }
  local start = vadd(vec(e.p), vadd(vscale(dirH, num(G.headSpawnOut, 0.25)), { x = 0, y = 0, z = 0.05 }))
  local h, kids = nil, {}
  local okHead = try('flying head', function()
    if not G.headPhysics then
      -- v62.14: lie on the ground beside the neck (head centre ~11 cm above the floor), no physics
      local gz = groundZ(start, e.target)
      if gz then start = { x = start.x, y = start.y, z = gz + num(G.headRestHeight, 0.11) } end
    end
    h = spawnGore({ class = 'BasicEntity', name = name, position = start,
      properties = { object_Model = MEAT, Physics = physicsFor(MASS.head, G.headPhysics) } })
    if not h then error('SpawnEntity returned nil') end
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
      if G.c3Head == true then   -- v62.13: the generated head-only piece renders without its face texture; opt-in only
        if variant then tries[#tries + 1] = base .. '_kcdghead.fly_' .. variant .. '.cdf' end
        tries[#tries + 1] = base .. '_kcdghead.fly.cdf'
      end
      local byMaterial = G.MaterialWrapperD54 and G.MaterialWrapperD54[normalPathD54(e.headSkin)]
      local exactWrapper = byMaterial and byMaterial[normalPathD54(e.headMtl)]
      if exactWrapper then tries[#tries + 1] = exactWrapper end
      if variant then tries[#tries + 1] = base .. '.fly_' .. variant .. '.cdf' end
      tries[#tries + 1] = cdf
      for _, c in ipairs(tries) do
        ret = h:LoadCharacter(1, c)
        pcall(function() valid = h:IsSlotValid(1) and h:IsSlotCharacter(1) end)
        log('FLY loadCharacter wrapper=' .. c .. ' headMtl=' .. tostring(e.headMtl) .. ' ret=' .. tostring(ret) .. ' slotCharacter=' .. tostring(valid))
        if valid then cdf = c break end
      end
      how = valid and ((cdf:find('_kcdghead', 1, true) and 'c3head' or 'wrapper') .. (variant and '_' .. variant or '')) or 'wrapper_FAILED'
    end
    if not valid then
      -- fallback: the game's default male/female character shows its own default head (body attachment hidden)
      local sk = e.female and SKEL.female or SKEL.male
      ret = h:LoadCharacter(1, sk)
      pcall(function() valid = h:IsSlotValid(1) and h:IsSlotCharacter(1) end)
      pcall(function() h:HideAttachment(1, 'body', true, true) end)
      how = valid and 'generic_default_head' or 'generic_FAILED'
      log('FLY loadCharacter generic=' .. sk .. ' ret=' .. tostring(ret) .. ' slotCharacter=' .. tostring(valid))
    end
    if not valid then error('no loaded head character slot; carrier rollback required') end
    if not (G.debugCarrier or how == 'wrapper_FAILED' or how == 'generic_FAILED') then h:DrawSlot(0, 0) end
    h:SetSlotPos(1, { x = -centre.x, y = -centre.y, z = -centre.z })
    local extra = addParts(h, e, centre, kids)
    bloodTrail(h)
    -- v62.29: cap placed from the real meshes. head skin neck hole (all male heads same topology): centre (0,-0.0047,1.4294)
    -- model space, 28x17 cm, faces down and 8.3 deg back. meat_piece.cgf is a 17x14x5.6 cm slab, pivot at its bottom face.
    -- v62.30: the meat piece itself is twisted 25 deg and tilted 8 deg in its own cgf. rotation now lines its flat side up
    -- with the hole and its long side with the wide side of the neck. size 1.0 (Miki: size was good). ~3 cm out, ~3 cm in.
    -- v62.28 had it 5 cm up inside the neck with no tilt = looked off centre and rotated.
    local CAP_M = { x = 0.004, y = 0.0166, z = -0.2494 }
    local CAP_F = { x = 0.004, y = 0.0166, z = -0.2494 } -- no female head in the extract, same offset from the head centre
    -- v62.25 (Claude): a flesh cap on the open neck end of the flying head (the vanilla head mesh is open there = "empty mesh").
    -- the game's own raw meat piece in slot 12 at the neck end. tunables: KCD2VR_Gore.neckCapCgf ('' = off), neckCapPos,
    -- neckCapAngles (radians), neckCapScale.
    -- D70G: merge Claude D71G's centre-to-neck placement, retaining D70 load
    -- validation. Only this newly spawned head's custom cap changes. Defaults
    -- are a cosmetic fit, not proof of final rendered coverage for every head.
    local mikiCap = (not e.female) and type(G.headStumpCgf) == 'string' and G.headStumpCgf ~= ''
    if mikiCap then
      local capRet, capPos, capScale, capTarget
      local okCap = pcall(function()
        capRet = h:LoadObject(12, G.headStumpCgf)
        if capRet == false or h:IsSlotValid(12) ~= true then error('custom cap not loaded') end
        local off = vec(G.headStumpPos)
        local ca = vec(G.headStumpAngles) -- existing slot-angle convention: radians
        local C = G.headStumpCenter or {}
        C = { x = num(C.x, -0.031076), y = num(C.y, 0.043906), z = num(C.z, 1.576277) }
        local T = G.headStumpTarget or {}
        T = { x = num(T.x, 0), y = num(T.y, 0.0025), z = num(T.z, 1.382) }
        local sc = num(G.headStumpScale, 1.3)
        if sc <= 0 then error('custom cap scale is not positive') end
        -- Keep the measured centre on the target even with a user angle/scale:
        -- slotPos + R*(scale*C) = target - headCentre + userOffset.
        local R = rotXYZ({ x = math.deg(ca.x), y = math.deg(ca.y), z = math.deg(ca.z) })
        local pos = vadd(vsub(vsub(T, centre), mapply(R, vscale(C, sc))), off)
        if not num(pos.x, nil) or not num(pos.y, nil) or not num(pos.z, nil) then error('custom cap transform overflow') end
        if h:SetSlotAngles(12, ca) == false then error('custom cap angle refused') end
        if h:SetSlotScale(12, sc) == false then error('custom cap scale refused') end
        if h:SetSlotPos(12, pos) == false then error('custom cap position refused') end
        if h:DrawSlot(12, 1) == false then error('custom cap draw refused') end
        capPos, capScale, capTarget = pos, sc, T
      end)
      -- pcall alone does not prove a loaded/renderable CGF. Failed custom
      -- transforms also take the existing flesh-cap fallback; no victim write.
      local slotOk, slotValid = pcall(function() return h:IsSlotValid(12) end)
      okCap = okCap and capRet ~= false and slotOk and slotValid == true
      log('FLY head stump (Miki) cgf=' .. G.headStumpCgf .. ' ok=' .. tostring(okCap) .. ' loadRet=' .. tostring(capRet) .. ' slotValid=' .. tostring(slotValid) .. ' rendered=UNOBSERVED')
      if okCap then
        log(string.format('FLY head stump placed slotPos=%.3f %.3f %.3f scale=%.2f target=%.3f %.3f %.3f rendered=UNOBSERVED',
          capPos.x, capPos.y, capPos.z, capScale, capTarget.x, capTarget.y, capTarget.z))
      else
        pcall(function() h:DrawSlot(12, 0) end)
        mikiCap = false
      end
    end
    if not mikiCap and type(G.neckCapCgf) == 'string' and G.neckCapCgf ~= '' then
      local okCap = pcall(function()
        h:LoadObject(12, G.neckCapCgf)
        local cp = G.neckCapPos or (e.female and CAP_F or CAP_M)
        h:SetSlotPos(12, { x = num(cp.x, 0), y = num(cp.y, 0), z = num(cp.z, -0.25) })
        local ca = G.neckCapAngles or { x = -0.1018, y = -0.0716, z = 0.4412 }
        pcall(function() h:SetSlotAngles(12, { x = num(ca.x, 0), y = num(ca.y, 0), z = num(ca.z, 0) }) end)
        pcall(function() h:SetSlotScale(12, num(G.neckCapScale, 1.0)) end)
        h:DrawSlot(12, 1)
      end)
      log('FLY neck cap cgf=' .. G.neckCapCgf .. ' ok=' .. tostring(okCap))
    end
    -- v62.7: the head bleeds from its cut face too: the same blood effect in slot 11 at the neck end of the head (local -centre + 0.08 m).
    pcall(function()
      h:LoadParticleEffect(11, BLOOD, bloodFx(0.8, 1.2))
      h:SetSlotPos(11, { x = 0, y = 0, z = -0.12 })
    end)
    -- v62.13: the head's own material (the ASI sends none: headMtl=nil) from the game's CharacterComponent table
    do
      local hm = (type(e.headMtl) == 'string' and e.headMtl ~= '') and e.headMtl or
        (G.PartMtl and G.PartMtl[(e.headSkin or ''):lower():gsub('\\', '/'):match('([^/]+)$') or ''])
      -- v62.19: the wrapper cdf already wears the head's own material (<head>_m01 / the fly_<variant> one). An entity-wide SetMaterial on top
      -- overrides the whole carrier's material - candidate for the untextured heads. Now only when the ASI sent the real material
      -- (e.headMtl) or KCD2VR_Gore.headMaterial = true forces the old behaviour; false = never.
      local need = not token and ((G.headMaterial == true) or (G.headMaterial ~= false and type(e.headMtl) == 'string' and e.headMtl ~= ''))
      -- D54 exact transfer targets the head CA_SKIN after Cut returns; an
      -- entity-wide override would also recolour the carrier's flesh cap.
      if not need then log('FLY head material kept from the wrapper (' .. tostring(hm) .. ' not forced)') end
      if need and type(hm) == 'string' and hm ~= '' then
        local okm = pcall(function() h:SetMaterial(hm) end)
        log('FLY head material=' .. hm .. ' setMaterial=' .. tostring(okm))
      end
    end
    local k = 0
    if G.headPhysics then
    -- v62.13: settle like a real object - some damping, a sleep speed so it stops simulating once it lies still
    pcall(function() h:SetPhysicParams(PHYSICPARAM_SIMULATION, { damping = num(G.headDamping, 0.15), sleep_speed = 0.04, max_time_step = 0.02 }) end)
    local d = vec(e.d)
    k = PUSH.head * (0.5 + 0.5 * math.max(0, math.min(1, num(e.energy, 1))))
    h:AddImpulse(-1, h:GetWorldPos(), d, k, 1)
    -- v62.13: spin around the axis across the throw (up x dir) so it ROLLS on landing instead of sliding like a brick
    do
      local axis = vnorm(vcross({ x = 0, y = 0, z = 1 }, dirH))
      if axis and num(G.headSpin, 1) > 0 then
        local ang = vscale(axis, num(G.headSpin, 1) * 0.6 * (0.5 + 0.5 * math.max(0, math.min(1, num(e.energy, 1)))))
        local okSpin = pcall(function() h:AddImpulse(-1, h:GetWorldPos(), d, 0, 1, ang) end)
        log('FLY head spin ' .. string.format('%.2f %.2f %.2f', ang.x, ang.y, ang.z) .. ' ok=' .. tostring(okSpin))
      end
    end
    -- v62.4: the wrapper spawn path of a corpse head moved only 0.2 m in 2 s (impulse lost, Miki's 20 Oct log). Check once; push again if it did not move.
    do
      local pStart = h:GetWorldPos()
      Script.SetTimer(250, function()
        local w = currentSpawned(h)
        if not w then return end
        local q = w:GetWorldPos()
        local dx, dy, dz = q.x - pStart.x, q.y - pStart.y, q.z - pStart.z
        if (dx * dx + dy * dy + dz * dz) < 0.09 then
          pcall(function() w:AddImpulse(-1, q, d, k * num(G.headRepushMul, 1.0), 1) end)   -- v62.19: was x1.5 (it pushed against our own pyramids)
          log('FLY head did not move, impulse repeated x' .. tostring(num(G.headRepushMul, 1.0)))
        end
      end)
    end
    else
      pcall(function() h:Physicalize(0, PE_NONE, {}) end)
    end
    local id = h.id
    local p0 = h:GetWorldPos()
    log('FLY head spawned how=' .. how .. ' id=' .. tostring(id) .. ' headSkin=' .. tostring(e.headSkin) .. ' carrierVisible=' .. tostring(G.debugCarrier) ..
        ' at=' .. string.format('%.2f %.2f %.2f', p0.x, p0.y, p0.z) .. ' push=' .. string.format('%.1f', k))
    local function probe(ms)
      Script.SetTimer(ms, function()
        local w = currentSpawned(h)
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
    local function removeHead()
      local complete = true
      for _, k in ipairs(kids) do if not retireSpawned(k) then complete = false end end
      return retireSpawned(h) and complete
    end
    Script.SetTimer(LIFE_MS, removeHead)
    -- v62.14: the head goes with ITS corpse (the native side keeps the corpse's own head hidden as long as that corpse exists)
    local victim0 = System.GetEntity(e.target)
    if victim0 then
      local function watch()
        if not currentSpawned(h) then return end
        if currentEntity61(victim0) ~= victim0 then
          local complete = removeHead()
          log('FLY head cleanup ' .. (complete and 'complete' or 'pending') .. ': corpse ' .. tostring(e.target) .. ' gone')
          return
        end
        Script.SetTimer(2000, watch)
      end
      Script.SetTimer(2000, watch)
    end
  end)
  -- v62.14: a head that failed half way is removed again (no orphan carrier in the world)
  if not okHead and h then
    for _, k in ipairs(kids) do retireSpawned(k) end
    retireSpawned(h)
  end
  return okHead
end

-- v62.9: the game's own neck stump on the body side (male only - the mesh is on the male skeleton)
G.neckStump = (G.neckStump ~= false)
G.neckStumpCdf = G.neckStumpCdf or 'objects/characters/humans/male/head/m_head_decapitated/kcd2vr_neckstump.cdf'
G.neckStumpBone = G.neckStumpBone or 'Neck'
if G.neckCapCgf == nil then G.neckCapCgf = 'objects/manmade/food/food/meat_piece.cgf' end -- v62.25 flesh cap on the flying head
if G.headStumpCgf == nil then G.headStumpCgf = 'objects/characters/humans/male/head/m_head_decapitated/kcd2vr_headstump_miki_d41.cgf' end -- v62.31
-- v62.23: the real stump (vanilla m_head_decapitated converted to a static CGF, pivot at Neck) rides the victim's own Neck bone.
-- KCD2VR_Gore.neckStumpCgf='' (or false) = old helper stump.
-- v62.24: the static CGF bake is wrong (pivot translated only, no Neck bind rotation) -> OFF by default again. The default stump is the
-- game's own SKINNED m_head_decapitated.skin in the kcd2vr_neckstump.cdf wrapper (same skin + skeleton logic as the head), placed by script.
-- KCD2VR_Gore.neckStumpCgf='objects/characters/humans/male/head/m_head_decapitated/kcd2vr_neckstump_static.cgf' = CGF test.
if G.neckStumpCgf == nil then G.neckStumpCgf = '' end -- v62.28: V3 static stump is OPT-IN (Codex: runtime attach/cleanup not yet qualified); try: KCD2VR_Gore.neckStumpCgf='objects/characters/humans/male/head/m_head_decapitated/kcd2vr_neckstump_static.cgf'
G.neckStumpPos = G.neckStumpPos or { x = -1.3697, y = 0, z = 0.4887 }      -- inverse Neck bind pose (W2B) from the skin's bone table
G.neckStumpAngles = G.neckStumpAngles or { x = 108.8, y = 0, z = 90 }      -- same, as xyz euler degrees (Rz*Ry*Rx)
G.neckStumpLeases = G.neckStumpLeases or setmetatable({}, { __mode = 'k' })
G.neckStumpFollow = (G.neckStumpFollow ~= false)   -- v62.11: script-driven placement on the Neck bone (true) / old entity attachment (false)
G.neckStumpFlip = (G.neckStumpFlip == true)         -- v62.11: mirror the stump around the neck axis if the shoulder line reads the other way
G.neckStumpShoulders = G.neckStumpShoulders or { { 'RightShoulder', 'LeftShoulder' }, { 'RightArm', 'LeftArm' } }
local FOLLOW_MS = 33
-- v62.13c (Miki: stump position right, but it FLICKERS): follow every frame (timer 0) while the body moves, write the transform
-- only when it really changed, slow to 200 ms once the corpse lies still; the stump is 2% larger than the body's neck so its
-- base does not z-fight with the collar/neck skin. console: neckStumpFollowMs (0), neckStumpScale (1.02)
G.neckStumpFollowMs = G.neckStumpFollowMs or 0
G.neckStumpScale = G.neckStumpScale or 1.02
-- world pose of the stump helper for the victim's current Neck bone: entity = T(neck) * W2B (the same offset the attachment used)
local function neckStumpPose(w, lastLat)
  local okn, neck = pcall(function() return w:GetBonePos(G.neckStumpBone) end)
  if not okn or type(neck) ~= 'table' then return nil end
  if G.boneRotation ~= false then
    local Rb = boneRot(w, G.neckStumpBone)
    if Rb then
      if G.neckStumpFlip then Rb = { Rb[1], vscale(Rb[2], -1), vscale(Rb[3], -1) } end
      return vadd(neck, mapply(Rb, vec(G.neckStumpPos))), anglesXYZ(mmul(Rb, rotXYZ(G.neckStumpAngles))), lastLat
    end
  end
  local okn1, neck1 = pcall(function() return w:GetBonePos('Neck1') end)
  local ax = (okn1 and type(neck1) == 'table') and vnorm(vsub(neck1, neck)) or nil   -- bone local X = along the neck (from the skin bone table)
  if not ax then return nil end
  local lat = nil
  for _, pair in ipairs(G.neckStumpShoulders) do
    local okr, r = pcall(function() return w:GetBonePos(pair[1]) end)
    local okl, l = pcall(function() return w:GetBonePos(pair[2]) end)
    if okr and okl and type(r) == 'table' and type(l) == 'table' then lat = vsub(r, l) break end
  end
  lat = lat or lastLat or { x = 1, y = 0, z = 0 }
  if G.neckStumpFlip then lat = vscale(lat, -1) end
  local ay = vnorm(vsub(lat, vscale(ax, vdot(lat, ax))))   -- bone local Y = model +X (the right side), squared against the neck axis
  if not ay then return nil end
  local az = vcross(ax, ay)
  local Rb = { ax, ay, az }
  local R = mmul(Rb, rotXYZ(G.neckStumpAngles))
  local pos = vadd(neck, mapply(Rb, vec(G.neckStumpPos)))
  return pos, anglesXYZ(R), lat
end
-- v62.13: exact placement without hand-typed offsets. The stump wrapper wears the SAME male skeleton as the victim, so its own Neck
-- bone is measured once (helper at zero angles) and the helper is then placed every tick so that bone coincides with the victim's
-- Neck bone (position and rotation). The angle unit of SetWorldAngles is checked once against the measured bone (radians vs degrees).
-- v62.20 (Miki: stump not on the body; R14 log: follow angles=-0 0 1 on corpses lying on the ground = upright stump):
-- GetBoneLocal does not give the ragdoll's bone rotation (only the yaw came through), GetBonePos does follow the ragdoll
-- (STUMP_PROBE bone z = ground). So the stump frame is built from bone POSITIONS only: Neck->Neck1 axis + the shoulder line,
-- the same way on the helper (calibration) and on the victim, so whatever the bone axes are, helper Neck lands on victim Neck.
-- KCD2VR_Gore.stumpPosFrame=false = the old GetBoneLocal rotation.
G.stumpPosFrame = (G.stumpPosFrame ~= false)
local function posFrame(w)
  local okn, neck = pcall(function() return w:GetBonePos(G.neckStumpBone) end)
  local okn1, neck1 = pcall(function() return w:GetBonePos('Neck1') end)
  if not (okn and okn1 and type(neck) == 'table' and type(neck1) == 'table') then return nil end
  local ax = vnorm(vsub(neck1, neck))
  if not ax then return nil end
  local lat = nil
  for _, pair in ipairs(G.neckStumpShoulders) do
    local okr, r = pcall(function() return w:GetBonePos(pair[1]) end)
    local okl, l = pcall(function() return w:GetBonePos(pair[2]) end)
    if okr and okl and type(r) == 'table' and type(l) == 'table' then lat = vsub(r, l) break end
  end
  if not lat then return nil end
  local ay = vnorm(vsub(lat, vscale(ax, vdot(lat, ax))))
  if not ay then return nil end
  return { ax, ay, vcross(ax, ay) }
end
local function stumpFrame(w) if G.stumpPosFrame then return posFrame(w) end return boneRot(w, G.neckStumpBone) end
local function mT(m) return { { x = m[1].x, y = m[2].x, z = m[3].x }, { x = m[1].y, y = m[2].y, z = m[3].y }, { x = m[1].z, y = m[2].z, z = m[3].z } } end
local function calibrateStump(h)
  local okp, hp = pcall(function() return h:GetWorldPos() end)
  local okn, np = pcall(function() return h:GetBonePos(G.neckStumpBone) end)
  local Rn = stumpFrame(h)
  if not (okp and okn and type(hp) == 'table' and type(np) == 'table' and Rn) then return nil end
  return { p = vsub(np, hp), R = Rn }          -- helper angles are zero here, so world == helper frame
end
local function anglesFor(R) local a = anglesXYZ(R) if G.angleUnit == 'rad' then return { x = math.rad(a.x), y = math.rad(a.y), z = math.rad(a.z) } end return a end
local function calibratedPose(w, cal)
  local okn, pv = pcall(function() return w:GetBonePos(G.neckStumpBone) end)
  local Rv = stumpFrame(w)
  if not (okn and type(pv) == 'table' and Rv) then return nil end
  local Rh = mmul(Rv, mT(cal.R))
  local s = num(G.neckStumpScale, 1)
  return vsub(pv, mapply(Rh, vscale(cal.p, s))), Rh
end
-- one-time check which unit SetWorldAngles takes: rotate the helper by a known matrix and see where its own Neck bone goes
local function detectAngleUnit(h, cal)
  if G.angleUnit then return G.angleUnit end
  local R = rotXYZ({ x = 0, y = 0, z = 90 })
  local function score(unit)
    G.angleUnit = unit
    pcall(function() h:SetWorldAngles(anglesFor(R)) end)
    local Rb = stumpFrame(h)
    pcall(function() h:SetWorldAngles({ x = 0, y = 0, z = 0 }) end)
    if not Rb then return -9 end
    local want = mmul(R, cal.R)
    return vdot(Rb[1], want[1]) + vdot(Rb[2], want[2]) + vdot(Rb[3], want[3])
  end
  local sr, sd = score('rad'), score('deg')
  G.angleUnit = (sr >= sd) and 'rad' or 'deg'
  log('NECKSTUMP angle unit=' .. G.angleUnit .. ' score rad=' .. string.format('%.2f', sr) .. ' deg=' .. string.format('%.2f', sd))
  return G.angleUnit
end
local function neckStump(v, e)
  if not G.neckStump then return false end
  if e.female then log('NECKSTUMP skipped: female (no female stump mesh yet)') return false end
  if G.neckStumpLeases[v] then log('NECKSTUMP already on this victim') return true end
  -- v62.22 (Claude, lane GS): a rigid stump as a CGF on the victim's OWN Neck bone attachment (engine SetAttachmentCGF, the same call
  -- the 'meat' limb stump uses). no helper entity, no script follow: the character draws it from its current (ragdoll) bone pose.
  -- off unless KCD2VR_Gore.neckStumpCgf is a .cgf path. decisive test: KCD2VR_Gore.neckStumpCgf='objects/manmade/food/food/meat.cgf'
  -- then decapitate: if the meat rides the neck of the falling body, ship a stump .cgf (m_head_decapitated converted) and use it.
  if type(G.neckStumpCgf) == 'string' and G.neckStumpCgf ~= '' then -- v62.23 default: the real static stump
    local ctag = 'kcd2vr_neckstump'
    local okc = try('neck stump cgf', function()
      v:CreateBoneAttachment(0, G.neckStumpBone, ctag)
      if G.neckStumpCgfPos then v:SetAttachmentPos(0, ctag, vec(G.neckStumpCgfPos)) end
      if G.neckStumpCgfAngles then v:SetAttachmentAngles(0, ctag, vec(G.neckStumpCgfAngles)) end
      v:SetAttachmentCGF(0, ctag, G.neckStumpCgf)
    end)
    if okc then
      local got = nil
      pcall(function() got = v:GetAttachmentCGF(0, ctag) end)
      local clease = { tag = ctag, follow = false, cgf = true }
      G.neckStumpLeases[v] = clease
      Script.SetTimer(LIFE_MS, function()
        if G.neckStumpLeases[v] ~= clease then return end
        G.neckStumpLeases[v] = nil
        local w = currentEntity61(v)
        if w then pcall(function() w:DestroyAttachment(0, ctag) end) end
      end)
      log('NECKSTUMP route=CGF_BONE target=' .. tostring(e.target) .. ' bone=' .. G.neckStumpBone .. ' mode=CGF_BONE_ATTACHMENT cgf=' .. G.neckStumpCgf .. ' got=' .. tostring(got))
      return true
    end
    pcall(function() v:DestroyAttachment(0, ctag) end)
    log('NECKSTUMP cgf mode failed, helper stump used')
  end
  local tag = 'kcd2vr_neckstump'
  local follow = G.neckStumpFollow
  local helper = nil
  local lat0 = nil
  local cal0 = nil
  local ok = try('neck stump', function()
    local pos = v:GetWorldPos()
    local ang = nil
    if follow then
      local fp, fa, fl = neckStumpPose(v, nil)
      if fp then pos, ang, lat0 = fp, fa, fl else log('NECKSTUMP bones not readable at spawn (' .. G.neckStumpBone .. '/Neck1), starts at the entity origin') end
    end
    helper = spawnGore({ class = 'BasicEntity', name = 'kcd2vr_neckstump_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999)),
      position = vec(pos), properties = helperProps() })
    if not helper then error('helper spawn returned nil') end
    local ret = helper:LoadCharacter(0, G.neckStumpCdf)
    local valid = false
    pcall(function() valid = helper:IsSlotValid(0) and helper:IsSlotCharacter(0) end)
    if not valid then error('stump wrapper did not load: ' .. tostring(G.neckStumpCdf) .. ' ret=' .. tostring(ret)) end
    -- v62.13: a pure visual - no physics body that the 33 ms teleport would push into the ragdoll / into Henry
    pcall(function() helper:DestroyPhysics() end)
    pcall(function() helper:Physicalize(0, PE_NONE, {}) end)
    pcall(function() helper:SetMaterial(G.neckStumpMtl or 'objects/characters/humans/male/head/m_head_decapitated/m_head_decapitated_m01.mtl') end)
    if follow and G.neckStumpCalibrated ~= false then
      pcall(function() helper:SetWorldAngles({ x = 0, y = 0, z = 0 }) end)
      cal0 = calibrateStump(helper)   -- measured at scale 1; the scale is applied after
      pcall(function() helper:SetScale(num(G.neckStumpScale, 1)) end)
      if cal0 then
        detectAngleUnit(helper, cal0)
        local cp, cR = calibratedPose(v, cal0)
        if cp then pcall(function() helper:SetWorldPos(cp) end) pcall(function() helper:SetWorldAngles(anglesFor(cR)) end) end
        log('NECKSTUMP calibrated neck-in-helper=' .. string.format('%.3f %.3f %.3f', cal0.p.x, cal0.p.y, cal0.p.z) .. ' unit=' .. tostring(G.angleUnit))
      else log('NECKSTUMP helper Neck bone not readable - old bind offsets used') end
    end
    if follow and not cal0 then
      if ang then helper:SetWorldAngles(vec(G.angleUnit == 'rad' and { x = math.rad(ang.x), y = math.rad(ang.y), z = math.rad(ang.z) } or ang)) end
    elseif not follow then   -- v62.21 (Codex N1 review): only the explicit attachment mode binds; script-follow keeps the helper independent
      v:CreateBoneAttachment(0, G.neckStumpBone, tag)
      v:SetAttachmentPos(0, tag, vec(G.neckStumpPos))
      v:SetAttachmentAngles(0, tag, vec(G.neckStumpAngles))
      v:SetAttachmentObject(0, tag, helper.id, 0, 0)
    end
  end)
  if not ok then
    if helper then pcall(function() System.RemoveEntity(helper.id) end) end
    if not follow then pcall(function() v:DestroyAttachment(0, tag) end) end
    return false
  end
  local lease = { helper = helper, tag = tag, follow = follow, lat = lat0, ticks = 0, cal = cal0 }
  G.neckStumpLeases[v] = lease
  local function finish()
    if G.neckStumpLeases[v] ~= lease then return end
    G.neckStumpLeases[v] = nil
    local w = currentEntity61(v)
    if w and not follow then pcall(function() w:DestroyAttachment(0, tag) end) end
    local h = currentEntity61(helper)
    if h then pcall(function() System.RemoveEntity(h.id) end) end
  end
  if follow then
    -- v62.11: the body is a ragdoll now; its bones are read every tick and the helper is moved onto the Neck. Ends with the lease,
    -- when the victim entity is gone, or when the bones stop being readable.
    local function tick()
      if G.neckStumpLeases[v] ~= lease then return end
      local w = currentEntity61(v)
      local h = currentEntity61(helper)
      if not w or not h then finish() log('NECKSTUMP follow ended: ' .. (w and 'helper' or 'victim') .. ' gone') return end
      local pos, ang, lat
      if lease.cal then
        local R
        pos, R = calibratedPose(w, lease.cal)
        if pos then ang = anglesFor(R) lat = lease.lat end
      else
        pos, ang, lat = neckStumpPose(w, lease.lat)
        if pos and G.angleUnit == 'rad' then ang = { x = math.rad(ang.x), y = math.rad(ang.y), z = math.rad(ang.z) } end
      end
      local nextMs = num(G.neckStumpFollowMs, 0)
      if pos then
        lease.lat = lat
        local moved = not lease.lastPos or vlen(vsub(pos, lease.lastPos)) > 0.0005 or
          math.abs(ang.x - lease.lastAng.x) + math.abs(ang.y - lease.lastAng.y) + math.abs(ang.z - lease.lastAng.z) > 0.001
        if moved then
          pcall(function() h:SetWorldPos(vec(pos)) end)
          pcall(function() h:SetWorldAngles(vec(ang)) end)
          lease.lastPos, lease.lastAng, lease.stillSince = vec(pos), vec(ang), nil
        else
          lease.stillSince = lease.stillSince or (lease.ticks or 0)
          if (lease.ticks or 0) - lease.stillSince > 120 then nextMs = 200 end   -- lying still: check 5x a second only
        end
        lease.ticks = lease.ticks + 1
        if lease.ticks == 1 or lease.ticks == 60 then
          log('NECKSTUMP follow tick=' .. lease.ticks .. ' at=' .. string.format('%.2f %.2f %.2f', pos.x, pos.y, pos.z) .. ' angles=' .. string.format('%.0f %.0f %.0f', ang.x, ang.y, ang.z))
        end
      else
        lease.misses = (lease.misses or 0) + 1
        if lease.misses > 300 then finish() log('NECKSTUMP follow ended: neck bones unreadable') return end
      end
      Script.SetTimer(nextMs, tick)
    end
    Script.SetTimer(0, tick)
  end
  Script.SetTimer(LIFE_MS, finish)
  log('NECKSTUMP route=SKIN_HELPER target=' .. tostring(e.target) .. ' helper=' .. tostring(helper.id) .. ' bone=' .. G.neckStumpBone .. ' mode=' .. (follow and 'FOLLOW' or 'ATTACHMENT') .. ' cdf=' .. G.neckStumpCdf)
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
local MASS_LIMB = { lh = 1, rh = 1, lf = 1.5, rf = 1.5, la = 2, ra = 2, ll = 4, rl = 4 }   -- v62.32: forearm+hand / shin+foot
local PUSH_LIMB = { lh = 9, rh = 9, lf = 11, rf = 11, la = 12, ra = 12, ll = 16, rl = 16 }   -- 62.16 raw impulses, only used for the old path (limbThrowSpeed<=0)
local FLY_LIMB_MAX_SLOTS = 16 -- D52: matches the native event's sixteen clothing layers

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
-- v62.17: where a flying hand/foot starts and which way it goes: away from the victim's centre line, blended with the cut.
local function limbLaunch(e, pos)
  local cutH = vnorm({ x = num((e.d or {}).x, 0), y = num((e.d or {}).y, 0), z = 0 })
  local away = nil
  pcall(function()
    local v = System.GetEntity(e.target)
    if v then local c = v:GetWorldPos() away = vnorm({ x = pos.x - c.x, y = pos.y - c.y, z = 0 }) end
  end)
  local dir = vnorm(vadd(away or { x = 0, y = 0, z = 0 }, cutH or { x = 0, y = 0, z = 0 })) or away or cutH or { x = 1, y = 0, z = 0 }
  local out = math.max(0.10, math.min(0.40, num(G.limbSpawnOut, 0.22)))
  return vadd(pos, vadd(vscale(dir, out), { x = 0, y = 0, z = 0.05 })), dir
end
local function layOnGround(w, target)
  pcall(function() w:Physicalize(0, PE_NONE, {}) end)
  pcall(function()
    local p = w:GetWorldPos()
    local gz = groundZ(p, target)
    if gz then w:SetWorldPos({ x = p.x, y = p.y, z = gz + num(G.limbRestHeight, 0.05) }) end
  end)
end
local function flyLimb(e, pieces, centre, pos, dirv, complete)
  local name = 'kcd2vr_gore_' .. e.cut .. '_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999))
  local h, count = nil, 0
  local throwDir = nil
  if G.limbPhysics then pos, throwDir = limbLaunch(e, pos) end
  local ok, err = pcall(function()
    if not G.limbPhysics then
      local gz = groundZ(pos, e.target)
      if gz then pos = { x = pos.x, y = pos.y, z = gz + num(G.limbRestHeight, 0.05) } end
    end
    local params = { class = 'BasicEntity', name = name, position = pos,
      properties = { object_Model = MEAT, Physics = physicsFor(MASS_LIMB[e.cut], G.limbPhysics) } }
    if dirv then params.orientation = dirv end
    h = spawnGore(params)
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
    local k = 0
    if G.limbPhysics then
      local en = 0.5 + 0.5 * math.max(0, math.min(1, num(e.energy, 1)))
      pcall(function() h:SetPhysicParams(PHYSICPARAM_SIMULATION, { damping = num(G.limbDamping, 0.25), sleep_speed = 0.05, max_time_step = 0.02 }) end)
      local speed = num(G.limbThrowSpeed, 3.0)
      if speed > 0 then
        speed = math.min(speed, 6.0)
        d = vnorm(vadd(throwDir or vec(e.d), { x = 0, y = 0, z = 0.35 })) or { x = 0, y = 0, z = 1 }
        k = (MASS_LIMB[e.cut] or 1) * speed * en
      else
        k = PUSH_LIMB[e.cut] * en
      end
      h:AddImpulse(-1, h:GetWorldPos(), d, k, 1)
      local axis = vnorm(vcross({ x = 0, y = 0, z = 1 }, throwDir or { x = 1, y = 0, z = 0 }))
      if axis and num(G.limbSpin, 1) > 0 then
        local ang = vscale(axis, num(G.limbSpin, 1) * 0.15 * en)
        pcall(function() h:AddImpulse(-1, h:GetWorldPos(), d, 0, 1, ang) end)
      end
      -- watchdog: never across the map, never sliding forever
      local start, maxTravel = h:GetWorldPos(), math.max(1.0, num(G.limbMaxTravel, 3.5))
      local function guard(ms, last)
        Script.SetTimer(ms, function()
          local w = currentSpawned(h)
          if not w then return end
          local q = w:GetWorldPos()
          local far = math.sqrt((q.x - start.x) ^ 2 + (q.y - start.y) ^ 2 + (q.z - start.z) ^ 2)
          if far > maxTravel then
            layOnGround(w, e.target)
            log('FLY ' .. e.cut .. ' watchdog: ' .. string.format('%.2f', far) .. ' m from start > ' .. string.format('%.1f', maxTravel) .. ' -> physics off, laid on the ground')
          elseif last then
            local okv, vel = pcall(function() return w:GetVelocity() end)
            local sp = (okv and type(vel) == 'table') and vlen(vec(vel)) or 0
            if sp > 0.3 then layOnGround(w, e.target) log('FLY ' .. e.cut .. ' watchdog: still moving ' .. string.format('%.2f', sp) .. ' m/s after 3 s -> laid on the ground') end
          end
        end)
      end
      guard(150) guard(400) guard(1000) guard(2000) guard(3000, true)
    else
      pcall(function() h:Physicalize(0, PE_NONE, {}) end)
    end
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
    Script.SetTimer(LIFE_MS, function() retireSpawned(h) end)
  end)
  if not ok then
    if h then retireSpawned(h) end
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

-- v62.10: single-side cuts. KEPT[cut] = the side that stays; the kept piece is bound to its bone with the inverse bind pose (W2B) of the
-- male skeleton (read from the game's own skin bone table: LeftHand/RightHand/LeftFoot/RightFoot rows). Female skeleton untested (same names).
G.singleLimb = (G.singleLimb ~= false)
local KEPT = { lh = 'rh', rh = 'lh', lf = 'rf', rf = 'lf' }
G.limbKeepPos = G.limbKeepPos or {
  lh = { x = 0.5141, y = -1.0044, z = 0.1295 }, rh = { x = -0.5141, y = 1.0044, z = -0.1295 },
  lf = { x = 0.0484, y = -0.1567, z = -0.0644 }, rf = { x = -0.0484, y = 0.1567, z = 0.0644 } }
G.limbKeepAngles = G.limbKeepAngles or {
  lh = { x = 90, y = -15.14, z = -125.37 }, rh = { x = -90, y = -15.14, z = -125.37 },
  lf = { x = 23.94, y = -2.33, z = -93.42 }, rf = { x = -156.06, y = -2.33, z = -93.42 } }
-- binds every piece layer of the kept side to its bone; returns ok, list of { helper = entity, tag = attachment name }
-- v62.15: the kept hand/foot got DEGREES from anglesXYZ, the engine wants radians (same bug the 62.13 stump fix found) -> the kept
-- side sat at a garbage rotation next to the wrist = "two hands". radians unless the stump calibration measured degrees.
local function keepAngles(R) local a = anglesXYZ(R) if G.angleUnit == 'deg' then return a end return { x = math.rad(a.x), y = math.rad(a.y), z = math.rad(a.z) } end
-- v62.21: the kept hand/foot had the stump's GetBoneLocal weakness (rotation does not follow the ragdoll). Same fix as the stump:
-- a frame from bone POSITIONS (hand: Hand->Middle1 + Index1-Pinky1; feet keep the old path), measured once on the helper
-- (its wrapper wears the same male skeleton) and every tick on the victim, so helper bone lands on victim bone. No offsets needed.
-- KCD2VR_Gore.keepPosFrame=false = the old GetBoneLocal + limbKeepPos/Angles path.
G.keepPosFrame = (G.keepPosFrame ~= false)
local KEEP_FRAME = {
  lh = { 'LeftHand', 'LeftHandMiddle1', 'LeftHandIndex1', 'LeftHandPinky1' },
  rh = { 'RightHand', 'RightHandMiddle1', 'RightHandIndex1', 'RightHandPinky1' },
  -- feet: no second rigid bone pair inside the foot (Leg->Foot moves with the ankle) - feet keep the old path
}
local function keepFrame(w, sd)
  local f = KEEP_FRAME[sd]
  if not f then return nil end
  local p = {}
  for i = 1, 4 do
    local ok, q = pcall(function() return w:GetBonePos(f[i]) end)
    if not ok or type(q) ~= 'table' or not num(q.x, nil) then return nil end
    p[i] = q
  end
  local ax = vnorm(vsub(p[2], p[1]))
  local side = vsub(p[4], p[3])
  if not ax then return nil end
  local ay = vnorm(vsub(side, vscale(ax, vdot(side, ax))))
  if not ay then return nil end
  return { ax, ay, vcross(ax, ay) }, p[1]
end
local function keepCalibrate(h, sd)
  pcall(function() h:SetWorldAngles({ x = 0, y = 0, z = 0 }) end)
  local okp, hp = pcall(function() return h:GetWorldPos() end)
  local R, bp = keepFrame(h, sd)
  if not (okp and type(hp) == 'table' and R and bp) then return nil end
  return { p = vsub(bp, hp), R = R }
end
local function keepPose(w, sd, cal)
  local Rv, pv = keepFrame(w, sd)
  if not (Rv and pv) then return nil end
  local Rh = mmul(Rv, mT(cal.R))
  return vsub(pv, mapply(Rh, cal.p)), Rh
end
local function keepSide(v, e, sd, pieces)
  local made = {}
  local function undo()
    for _, k in ipairs(made) do
      pcall(function() v:DestroyAttachment(0, k.tag) end)
      local h = currentEntity61(k.helper) if h then pcall(function() System.RemoveEntity(h.id) end) end
    end
    made = {}
  end
  local bone = BONE[sd]
  local okBone = false
  pcall(function() okBone = v:GetBonePos(bone) ~= nil end)
  if not okBone then log('KEEP ' .. sd .. ' bone ' .. tostring(bone) .. ' not readable') return false, {} end
  for i, pc in ipairs(pieces) do
    local tag = 'kcd2vr_keep_' .. sd .. '_' .. i
    local helper = nil
    local kcal = nil   -- v62.21 position-frame calibration of this layer (nil = old bone-rotation path)
    local ok = try('keep ' .. sd .. ' layer ' .. i, function()
      helper = spawnGore({ class = 'BasicEntity', name = 'kcd2vr_keep_' .. sd .. '_' .. tostring(e.target) .. '_' .. tostring(math.random(1, 999999)),
        position = vec(v:GetWorldPos()), properties = helperProps() })
      if not helper then error('helper spawn returned nil') end
      local loaded = nil
      for _, cdf in ipairs(pieceWrapper(pc.skin, pc.att)) do
        local okLoad = pcall(function() helper:LoadCharacter(0, cdf) end)
        local valid = false
        pcall(function() valid = helper:IsSlotValid(0) and helper:IsSlotCharacter(0) end)
        if okLoad and valid then loaded = cdf break end
      end
      if not loaded then error('no piece wrapper loaded for the kept side (limb piece PAK missing?)') end
      -- v62.12: follow by script when the engine gives the bone rotation (a dying victim ragdolls; attachments stay behind)
      local kc = G.keepPosFrame and keepCalibrate(helper, sd) or nil
      local kp, kR = nil, nil
      if kc then kp, kR = keepPose(v, sd, kc) end
      if kp then kcal = kc end
      local Rb = (not kp) and (G.boneRotation ~= false) and boneRot(v, bone) or nil
      if kp then
        helper:SetWorldPos(vec(kp))
        helper:SetWorldAngles(vec(keepAngles(kR)))
        tag = nil
        log('KEEP ' .. sd .. ' layer ' .. i .. ' FOLLOW(position frame): ' .. loaded .. ' on ' .. bone)
      elseif Rb then
        local bp = v:GetBonePos(bone)
        helper:SetWorldPos(vec(vadd(bp, mapply(Rb, vec(G.limbKeepPos[sd])))))
        helper:SetWorldAngles(vec(keepAngles(mmul(Rb, rotXYZ(G.limbKeepAngles[sd])))))
        tag = nil
        log('KEEP ' .. sd .. ' layer ' .. i .. ' FOLLOW: ' .. loaded .. ' on ' .. bone)
      else
        v:CreateBoneAttachment(0, bone, tag)
        v:SetAttachmentPos(0, tag, vec(G.limbKeepPos[sd]))
        v:SetAttachmentAngles(0, tag, vec(G.limbKeepAngles[sd]))
        v:SetAttachmentObject(0, tag, helper.id, 0, 0)
        log('KEEP ' .. sd .. ' layer ' .. i .. ' bound (attachment, no bone rotation from the engine): ' .. loaded .. ' on ' .. bone)
      end
    end)
    if not ok then
      if helper then pcall(function() System.RemoveEntity(helper.id) end) end
      pcall(function() v:DestroyAttachment(0, tag) end)
      undo()
      return false, {}
    end
    made[#made + 1] = { helper = helper, tag = tag, bone = bone, sd = sd, cal = kcal }
  end
  -- v62.12: one follow timer for every script-placed layer of this kept side
  local follow = {}
  for _, k in ipairs(made) do if not k.tag then follow[#follow + 1] = k end end
  if #follow > 0 then
    local misses = 0
    local function tick()
      local w = currentEntity61(v)
      if not w then return end
      local live = false
      for _, k in ipairs(follow) do
        local h = currentEntity61(k.helper)
        if h then
          live = true
          if k.cal then
            local kp, kR = keepPose(w, k.sd, k.cal)
            if kp then
              pcall(function() h:SetWorldPos(vec(kp)) end)
              pcall(function() h:SetWorldAngles(vec(keepAngles(kR))) end)
              misses = 0
            else misses = misses + 1 end
          else
          local Rb = boneRot(w, k.bone)
          local okp, bp = pcall(function() return w:GetBonePos(k.bone) end)
          if Rb and okp and bp then
            pcall(function() h:SetWorldPos(vec(vadd(bp, mapply(Rb, vec(G.limbKeepPos[k.sd]))))) end)
            pcall(function() h:SetWorldAngles(vec(keepAngles(mmul(Rb, rotXYZ(G.limbKeepAngles[k.sd]))))) end)
            misses = 0
          else misses = misses + 1 end
          end
        end
      end
      if live and misses < 90 then Script.SetTimer(num(G.limbKeepFollowMs, 0), tick) end
    end
    Script.SetTimer(num(G.limbKeepFollowMs, 0), tick)
  end
  return true, made
end
local function releaseKept(v, lease)
  for _, k in ipairs(lease.kept or {}) do
    local w = currentEntity61(v) if w and k.tag then pcall(function() w:DestroyAttachment(0, k.tag) end) end
    local h = currentEntity61(k.helper) if h then pcall(function() System.RemoveEntity(h.id) end) end
  end
  lease.kept = {}
end

local function cutLimb(v, e, paired)
  -- v62.6: `paired` is decided LOCALLY by G.Cut (kill cut 'hands'/'feet'), never taken from the event table.
  -- paired: BOTH sides (lh+rh or lf+rf): every twin is created first, the victim is hidden only when ALL exist, any failure removes every carrier.
  local cut = e.cut
  if not (cut == 'lh' or cut == 'rh' or cut == 'lf' or cut == 'rf') then say('gore: no cut piece for ' .. tostring(cut) .. ' yet (hands/feet/head only)') return end
  if paired and G.neverPaired ~= false then paired = false end   -- v62.12 Miki: NEVER two (G.Cut already picked the hit side)
  local single = (not paired) and G.singleLimb
  if not (G.limbExperiment or paired or single) then log('LIMB cut=' .. cut .. ' OFF: limb experiment is disabled (KCD2VR_Gore.limbExperiment=true to try it; hides both hands/feet of the victim)') return end
  local sides = { cut }
  if paired then sides = (cut == 'lh' or cut == 'rh') and { 'lh', 'rh' } or { 'lf', 'rf' } end
  -- v62.10: single cut gathers the pieces of BOTH sides (the kept side is rebuilt from its own piece), flies only the cut side
  local gather = sides
  if single then gather = { cut, KEPT[cut] } end
  local lease = limbLeases[v]
  if not lease then lease = { cuts = {}, hidden = {} } limbLeases[v] = lease end
  for _, sd in ipairs(sides) do
    if lease.cuts[sd] then log('LIMB target=' .. tostring(e.target) .. ' cut=' .. sd .. ' already cut: nothing more to do') return end
  end
  local per, hide, seen, sawBody = {}, {}, {}, false
  for _, sd in ipairs(gather) do per[sd] = { pieces = {}, centre = nil } end
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
      for _, sd in ipairs(gather) do
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
  for _, sd in ipairs(gather) do
    if #per[sd].pieces == 0 then
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. sd .. ' REFUSED: ' .. (paired and 'paired cut needs BOTH sides, ' or '') .. 'victim has no usable separate ' .. ((sd == 'lh' or sd == 'rh') and 'hands' or 'feet') ..
          ' piece (' .. (sawBody and 'one-piece body/legs mesh cannot be split by script' or 'no usable skin in the event') .. '); victim untouched')
      return
    end
  end
  local dirv = nil
  pcall(function() local d = v:GetDirectionVector() if d and num(d.x, nil) and num(d.y, nil) then dirv = { x = d.x, y = d.y, z = 0 } end end)
  -- v62.12: the kept side first. if it can not stay on the body, NOTHING is cut (never both fly).
  local keptEarly, keptInfo = nil, ''
  if single then
    local ks = KEPT[cut]
    local okKeep, made = keepSide(v, e, ks, per[ks].pieces)
    if not okKeep then
      say('gore: ' .. ((cut == 'lh' or cut == 'rh') and 'hand' or 'foot') .. ' cut refused - the other side can not stay on the body here')
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. cut .. ' REFUSED: kept side ' .. ks .. ' could not be rebuilt; victim untouched')
      return
    end
    keptEarly = made
    lease.kept = made
    keptInfo = ' kept=' .. ks .. '(' .. #made .. ' layer(s))'
  end
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
      if keptEarly then releaseKept(v, lease) end
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
      if keptEarly then releaseKept(v, lease) end
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. cut .. ' ROLLED BACK: hide of ' .. tostring(failed) .. ' failed; ' .. #done .. ' hides undone, flying piece(s) removed')
      return
    end
    for _, nm in ipairs(done) do lease.hidden[nm] = true end
    hidden = #done
    keepHiddenLimb(v, lease, sides[1], token, hide)
  end
  local okStump = true
  for _, sd in ipairs(sides) do okStump = stump(v, sd) and okStump end
  log('LIMB target=' .. tostring(e.target) .. ' cut=' .. table.concat(sides, '+') .. ' paired=' .. tostring(paired == true) .. ' stump=' .. tostring(okStump) .. ' flyingPieces=' .. tostring(nfly) .. ' carriers=' .. #carriers .. ' victimPartsHidden=' .. hidden .. '/' .. #hide .. keptInfo .. ' (both sides of that hands/feet skin are hidden: engine limit; v62.10 rebinds the kept side)')
end

-- Release one lease: only when the CURRENT entity for that object is the same object and the lease is still the registered one.
-- A replaced/dead entity is discarded without touching anything (its replacement is not ours).
local function releaseLimbLease(en, lease)
  if limbLeases[en] ~= lease then return false end
  lease.cuts = {}   -- every pending delayed hide of this victim stops
  releaseKept(en, lease)   -- v62.10: the rebound kept side goes with the lease
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

-- ===== v62.32 elbow / knee (D41) =====
G.limbs41 = (G.limbs41 ~= false)
G.L41 = G.L41 or {}   -- filled by zzkcd2vr_gore_limbs41.lua (piece centres in model space, from tools/gore_limb_d41.py)
G.limb41Leases = G.limb41Leases or setmetatable({}, { __mode = 'k' })
local limb41Leases = G.limb41Leases
local SIDE41 = { lh = 'la', rh = 'ra', lf = 'll', rf = 'rl' }
local DISTAL41 = { la = 'LeftHand', ra = 'RightHand', ll = 'LeftFoot', rl = 'RightFoot' }
-- bind-pose centres of a male forearm / shin piece when the table has none (male_body_arms / male_body_legs)
local CENTRE41 = { la = { x = -0.45, y = -0.02, z = 1.10 }, ra = { x = 0.45, y = -0.02, z = 1.10 }, ll = { x = -0.14, y = 0.0, z = 0.28 }, rl = { x = 0.14, y = 0.0, z = 0.28 } }
local function cutLimb41(v, e)
  local side = SIDE41[e.cut]
  if not side then log('LIMB41 bad side ' .. tostring(e.cut)) return end
  local lease = limb41Leases[v] or { made = {}, orig = {} }
  limb41Leases[v] = lease
  local pieces, centre, names = {}, nil, {}
  for _, s in ipairs(e.sw or {}) do
    if type(s) == 'table' and type(s.s) == 'string' and type(s.c) == 'string' then
      lease.made[#lease.made + 1] = s.c
      if type(s.n) == 'string' then lease.orig[#lease.orig + 1] = s.n end
      names[#names + 1] = tostring(s.n) .. '->' .. s.c
      local stem = s.s:lower():gsub('\\', '/'):gsub('%.skin$', '')
      pieces[#pieces + 1] = { skin = stem .. '_kcdg' .. side .. 'p.skin', att = s.n }
      local t = G.L41[s.s:lower()]
      local c = t and t['c_' .. side .. 'p']
      if not centre and type(c) == 'table' then centre = { x = num(c[1] or c.x, 0), y = num(c[2] or c.y, 0), z = num(c[3] or c.z, 0) } end
    end
  end
  log('LIMB41 target=' .. tostring(e.target) .. ' side=' .. side .. ' cut natively: ' .. table.concat(names, ' '))
  if not G.limbs41 then log('LIMB41 flying piece / blood off (KCD2VR_Gore.limbs41=false)') return end
  centre = centre or CENTRE41[side]
  local pos = nil
  pcall(function()
    local a, b = v:GetBonePos(BONE[side]), v:GetBonePos(DISTAL41[side])
    if a and b then pos = vscale(vadd(a, b), 0.5) elseif a then pos = a end
  end)
  pos = pos or vec(e.p)
  if #pieces > 0 then
    local es = {}
    for k, x in pairs(e) do es[k] = x end
    es.cut = side
    local okFly, n = flyLimb(es, pieces, centre, pos, nil, false)
    log('LIMB41 flying ' .. side .. ' ok=' .. tostring(okFly) .. ' layers=' .. tostring(n) .. '/' .. #pieces)
  end
  stump(v, side)   -- the game's blood effect on the elbow / knee bone + the world blood helper (G.stumpMode, G.bloodMul)
end
local function releaseLimb41(en, lease)
  if currentEntity61(en) ~= en then
    if limb41Leases[en] == lease then limb41Leases[en] = nil end
    return 0
  end
  local n, pending = 0, { made = {}, orig = {} }
  for i = 1, math.max(#lease.made, #lease.orig) do
    local made, original = lease.made[i], lease.orig[i]
    local removed = not made
    if made then
      local ok, result = pcall(function() return en:DestroyAttachment(0, made) end)
      removed = ok and result ~= false -- void success is nil; false is a refusal
      if removed then n = n + 1 end
    end
    local restored = not original
    if removed and original then
      local ok, result = pcall(function() return en:HideAttachment(0, original, false, false) end)
      restored = ok and result ~= false
    end
    if not removed or not restored then
      if removed then made = false end
      if restored then original = false end
      pending.made[#pending.made + 1] = made or false
      pending.orig[#pending.orig + 1] = original or false
    end
  end
  -- Retain failed work for a later ReleaseAll/Clear; never forget a failed hide.
  if #pending.made > 0 then limb41Leases[en] = pending
  elseif limb41Leases[en] == lease then limb41Leases[en] = nil end
  return n
end
function G.ReleaseAll()
  releaseNeckStumps()   -- v62.9
  do local l41 = {} for en, lease in pairs(limb41Leases) do l41[#l41 + 1] = { en, lease } end
    local undone = 0 for _, p in ipairs(l41) do undone = undone + releaseLimb41(p[1], p[2]) end
    if #l41 > 0 then log('RELEASEALL elbow/knee cuts undone: victims=' .. #l41 .. ' attachments=' .. undone) end end   -- v62.32
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
  local gone = removeAllSpawned()   -- v62.13: heads, parts, helpers too (reload must reset everything)
  log('RELEASEALL gore entities removed=' .. gone)
  return #list
end

-- v62.12: the game's own death state (retail Lua: self.actor:IsDead() / self.actor:GetHealth() <= 0 / entity:IsDead())
local function victimDead(w)
  local dead = false
  pcall(function() if w.actor and w.actor.IsDead and w.actor:IsDead() then dead = true end end)
  if not dead then pcall(function() if w.actor and w.actor.GetHealth and w.actor:GetHealth() <= 0 then dead = true end end) end
  if not dead then pcall(function() if w.IsDead and w:IsDead() then dead = true end end) end
  return dead
end
-- v62.18 (Miki 24 Oct): killing swing to the belly = a gore kill with a LOT of blood, no guts mesh (no organ asset; Miki: "we
--   need only a lot of blood"). a helper follows the victim's Spine1 (belly) bone and re-fires the game's own combat blood
--   effect every 400 ms for ~8 s at a bigger scale/count than the neck stump, plus a short first gush. kill moves only (same
--   0.8 s dead check as hands/feet). goes when the corpse goes / ReleaseAll. console: KCD2VR_Gore.evisceration=false,
--   bellyBloodTicks (20), bellyBloodScale (1.6).
G.evisceration = (G.evisceration ~= false)
local function cutViscera(v, e)
  if not G.evisceration then log('BELLY target=' .. tostring(e.target) .. ' OFF') return false end
  G.done[e.target] = G.done[e.target] or {}
  if G.done[e.target].abdomen then log('BELLY target=' .. tostring(e.target) .. ' already done') return false end
  local bone = nil
  for _, b in ipairs({ 'Spine1', 'Spine', 'Spine2' }) do
    local p = nil
    pcall(function() p = v:GetBonePos(b) end)
    if p and num(p.x, nil) then bone = b break end
  end
  if not bone then log('BELLY target=' .. tostring(e.target) .. ' REFUSED: no Spine bone readable') return false end
  G.done[e.target].abdomen = true
  local left = math.max(1, math.min(60, math.floor(num(G.bellyBloodTicks, 20))))
  local scale = math.max(0.5, math.min(3, num(G.bellyBloodScale, 1.6)))
  local helper, first = nil, true
  local function stop() if helper then local hh = helper helper = nil retireSpawned(hh) end end
  local function tick()
    local w = currentEntity61(v)
    local p = nil
    if w then pcall(function() p = w:GetBonePos(bone) end) end
    if not p then stop() log('BELLY target=' .. tostring(e.target) .. ' corpse gone, bleeding stopped') return end
    if not helper then
      pcall(function() helper = spawnGore({ class = 'BasicEntity', name = 'kcd2vr_bellyblood_' .. tostring(math.random(1, 999999)), position = vec(p), properties = helperProps() }) end)
      if not helper then log('BELLY target=' .. tostring(e.target) .. ' helper spawn FAILED') return end
    else
      pcall(function() helper:SetWorldPos(vec(p)) end)
    end
    pcall(function() helper:LoadParticleEffect(0, BLOOD_STUMP, bloodFx(scale, 2.0)) end)
    if first then first = false pcall(function() helper:LoadParticleEffect(1, BLOOD, bloodFx(scale * 1.3, 3.0)) end) end
    left = left - 1
    if left > 0 then Script.SetTimer(400, tick) else stop() end
  end
  tick()
  log('BELLY target=' .. tostring(e.target) .. ' bone=' .. bone .. ' bleeding ticks=' .. left + 1 .. ' scale=' .. string.format('%.1f', scale))
  return true
end
function G.Cut(e)
  if type(e) ~= 'table' or type(e.target) ~= 'number' or type(e.cut) ~= 'string' then log('bad event') return end
  local v = System.GetEntity(e.target)
  if not v then log('target ' .. tostring(e.target) .. ' not found') return end
  if not maleHumanD56(v) then log('ELIGIBILITY target=' .. tostring(e.target) .. ' REFUSED: no current male human proof') return end
  -- v62.5: 'hands' / 'feet' = the kill cut for BOTH sides (the engine hides both sides of that skin anyway, so this is the honest event for it)
  local paired = false
  if e.cut == 'hands' then e.cut = 'lh' paired = true elseif e.cut == 'feet' then e.cut = 'lf' paired = true end
  if e.cut == 'head' then cutHead(v, e) return end
  -- v62.32: D41 elbow / knee - the VR mod already cut the layers natively (dead victim proven there)
  if e.lvl == 1 and type(e.sw) == 'table' and SIDE41[e.cut] then cutLimb41(v, e) return end
  -- v62.26 (Miki 26 Oct): hand/foot dismemberment scrapped for now (never worked). KCD2VR_Gore.limbs=true brings it back.
  if G.limbs ~= true and (e.cut == 'lh' or e.cut == 'rh' or e.cut == 'lf' or e.cut == 'rf') then log('LIMB ' .. e.cut .. ' target=' .. tostring(e.target) .. ' ignored (dismemberment off, KCD2VR_Gore.limbs=true to test)') return end
  if e.cut == 'abdomen' then
    if e.corpse or victimDead(v) then cutViscera(v, e) return end
    local tries = { 150, 400, 800 }
    local function check(k)
      local w = currentEntity61(v)
      if not w then return end
      if victimDead(w) then cutViscera(w, e) return end
      if k > #tries then log('BELLY target=' .. tostring(e.target) .. ' REFUSED: victim alive after 0.8 s (kill moves only)') return end
      Script.SetTimer(tries[k] - (k > 1 and tries[k - 1] or 0), function() check(k + 1) end)
    end
    check(1)
    return
  end
  -- v62.12 Miki: NEVER two. a paired event becomes ONE cut on the side nearest the hit point.
  if paired and G.neverPaired ~= false then
    local a, b = (e.cut == 'lh') and 'lh' or 'lf', (e.cut == 'lh') and 'rh' or 'rf'
    local pa, pb = nil, nil
    pcall(function() pa = v:GetBonePos(BONE[a]) end) pcall(function() pb = v:GetBonePos(BONE[b]) end)
    local hp = vec(e.p)
    if pa and pb then e.cut = (vlen(vsub(pa, hp)) <= vlen(vsub(pb, hp))) and a or b end
    paired = false
  end
  -- v62.12 Miki: hand/foot cuts are KILL MOVES ONLY. a corpse is fine; a living npc must be dead by the game within 0.8 s.
  if e.corpse or G.limbOnLiving == true then cutLimb(v, e, paired) return end
  local tries = { 150, 400, 800 }
  local function check(k)
    local w = currentEntity61(v)
    if not w then return end
    if victimDead(w) then cutLimb(w, e, paired) return end
    if k > #tries then
      say('gore: no ' .. ((e.cut == 'lh' or e.cut == 'rh') and 'hand' or 'foot') .. ' cut - he is still alive (kill moves only)')
      log('LIMB target=' .. tostring(e.target) .. ' cut=' .. tostring(e.cut) .. ' REFUSED: victim alive after 0.8 s (kill moves only)')
      return
    end
    Script.SetTimer(tries[k] - (k > 1 and tries[k - 1] or 0), function() check(k + 1) end)
  end
  if victimDead(v) then cutLimb(v, e, paired) else check(1) end
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
      for _, c in ipairs({ 'head', 'lh', 'rh', 'lf', 'rf', 'la', 'ra', 'll', 'rl' }) do
        pcall(function() en:DestroyAttachment(0, 'kcd2vr_stump_' .. c) end)
      end
    end
  end
  log('CLEAR leases released; stump attachments near the player destroyed')
end

log('loaded version ' .. G.version .. ' protocol ' .. G.protocol)
