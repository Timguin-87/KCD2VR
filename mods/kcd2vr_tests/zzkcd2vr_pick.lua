-- KCD2VR pick01 (Claude, 23 Oct 2026): can a world item go into Henry's inventory WITHOUT the game's pickup animation
-- (the 1 s "Picking" fragment with the lean)? the game's own pickup is PickableItem:Use -> self.item:OnUsed(userId) (C++:
-- plays Picking, then adds). the inventory script bind has AddItem(itemId) (listed next to RemoveItem/DeleteItem/CreateItem in
-- the retail bind table). if AddItem on the world item's WUID works, the VR flow is: grip = physical hold (the mod's T4 body),
-- stow = AddItem (instant), drop/throw = T4 release. console, dev build or normal game, stand within 1.5 m of a loose item:
--   #KCD2VR_Pick.List()        nearest 5 pickable items: name, class, distance, item:GetId, CanPickUp
--   #KCD2VR_Pick.Stow()        nearest item: player.inventory:AddItem(item:GetId()) - NO animation expected; logs whether the
--                              world entity disappeared 300 ms later and whether the inventory count of its class went up
--   #KCD2VR_Pick.Native()      nearest item: the game's own self.item:OnUsed(player.id) for comparison (the lean)
--   #KCD2VR_Pick.Draw()        the last stowed item back to the hand: player.human:DrawFromInventory(wuid, 0, true)
-- every call says one line on screen and logs KCD2VR_PICK.
KCD2VR_Pick = KCD2VR_Pick or {}
local P = KCD2VR_Pick
P.version = '0.1-TEST'
P.radius = 1.5
P.last = nil
local function log(s) System.LogAlways('KCD2VR_PICK ' .. tostring(s)) end
local function say(s) pcall(function() Game.SendInfoText(s, true, nil, 2) end) log(s) end
local function dist2(a, b) return (a.x - b.x) ^ 2 + (a.y - b.y) ^ 2 + (a.z - b.z) ^ 2 end
local function isPickable(e)
  return e and e.item ~= nil and type(e.item) == 'table' and e.id ~= player.id
end
local function items()
  local pos = player:GetWorldPos()
  local list = System.GetEntitiesInSphere(pos, P.radius + 1.0) or {}
  local out = {}
  for _, e in ipairs(list) do
    if isPickable(e) then
      local okp, p = pcall(function() return e:GetWorldPos() end)
      if okp and p then out[#out + 1] = { e = e, d = math.sqrt(dist2(p, pos)) } end
    end
  end
  table.sort(out, function(a, b) return a.d < b.d end)
  return out
end
local function wuid(e)
  local ok, id = pcall(function() return e.item:GetId() end)
  if ok then return id end
  return nil
end
local function canPick(e)
  local ok, v = pcall(function() return e.item:CanPickUp(player.id) end)
  if ok then return v end
  return 'n/a'
end
local function classOf(e)
  local ok, c = pcall(function() return e.item:GetUIName() end)
  return (ok and c) or tostring(e.class)
end
function P.List()
  local l = items()
  if #l == 0 then say('pick: no item with an item bind within ' .. P.radius .. ' m') return end
  for i = 1, math.min(5, #l) do
    local it = l[i]
    log(string.format('%d %s class=%s d=%.2f wuid=%s canPickUp=%s', i, tostring(it.e:GetName()), classOf(it.e), it.d, tostring(wuid(it.e)), tostring(canPick(it.e))))
  end
  say(string.format('pick: %d items, nearest %s (%.2f m) - see log', #l, tostring(l[1].e:GetName()), l[1].d))
end
local function countOfClass(e)
  local ok, n = pcall(function() return player.inventory:GetCountOfClass(e.item:GetClassId()) end)
  if ok then return n end
  return nil
end
function P.Stow()
  local l = items()
  if #l == 0 or l[1].d > P.radius then say('pick: nothing within ' .. P.radius .. ' m') return end
  local e = l[1].e
  local id, eid = wuid(e), e.id
  if not id then say('pick: ' .. tostring(e:GetName()) .. ' has no item:GetId') return end
  local before = countOfClass(e)
  local ok, r = pcall(function() return player.inventory:AddItem(id) end)
  say(string.format('pick stow %s AddItem(wuid) ok=%s ret=%s', tostring(e:GetName()), tostring(ok), tostring(r)))
  if ok then P.last = id end
  Script.SetTimer(300, function()
    local gone = System.GetEntity(eid) == nil
    local after = countOfClass(e)
    log(string.format('pick stow after 300 ms: worldEntityGone=%s countBefore=%s countAfter=%s', tostring(gone), tostring(before), tostring(after)))
  end)
end
function P.Native()
  local l = items()
  if #l == 0 or l[1].d > P.radius then say('pick: nothing within ' .. P.radius .. ' m') return end
  local e = l[1].e
  local ok, r = pcall(function() return e.item:OnUsed(player.id) end)
  say(string.format('pick native %s item:OnUsed ok=%s ret=%s (expect the Picking animation)', tostring(e:GetName()), tostring(ok), tostring(r)))
end
function P.Draw()
  if not P.last then say('pick: nothing stowed yet') return end
  local ok, r = pcall(function() return player.human:DrawFromInventory(P.last, 0, true) end)
  say(string.format('pick draw last wuid ok=%s ret=%s', tostring(ok), tostring(r)))
end
log('loaded ' .. P.version)
