-- H41: native inventory transfer, never a replacement recipe/item engine.
-- PREPARE captures the original item while C++ pins its exact physical owner.
-- COMMIT is allowed only after that owner's native constraint/reference retirement.
local pending, high, terminal = nil, 0, nil
local function reply(n,a,s) return 'OK|'..n..'|'..a..'|'..s end
local function current(p)
  return player == p.player and player and System.GetEntity(p.playerId) == p.player and player.inventory == p.inventory
end
local function itemCurrent(p)
  local e=System.GetEntity(p.entityId)
  if e~=p.entity or e.item~=p.item then return false end
  local ok,id=pcall(p.item.GetId,p.item)
  return ok and id==p.wuid
end
function kcd2vr_grip_stow_cb1(input)
  local n,a,pl,en,side=string.match(tostring(input or ''),'^(%d+)|([A-Z]+)|(%d+)|(%d+)|([01])$')
  n,pl,en=tonumber(n),tonumber(pl),tonumber(en)
  if not n or n<1 or n>9007199254740991 or not pl or pl<1 or not en or en<1 then return 'BAD|0' end
  if a~='PREPARE' and a~='COMMIT' and a~='CANCEL' then return 'BAD|0' end
  if terminal and terminal.serial==n and terminal.action==a then return terminal.reply end
  if n<high then return reply(n,a,'STALE') end
  -- A newer original C++ request may replace an uncommitted read-only snapshot;
  -- no native transfer was entered for that abandoned PREPARE.
  if pending and a=='PREPARE' and n>high then pending=nil end
  if pending and (pending.serial~=n or pending.playerId~=pl or pending.entityId~=en or pending.side~=side) then return reply(n,a,'BUSY') end
  if a=='PREPARE' then
    if pending then return reply(n,a,'PREPARED') end
    if n<=high then return reply(n,a,'STALE') end
    high=n
    if not player or System.GetEntity(pl)~=player or not player.inventory then return reply(n,a,'NO_PLAYER') end
    local e=System.GetEntity(en)
    if not e or not e.item or type(e.item.GetId)~='function' then return reply(n,a,'NOT_ITEM') end
    local ok,id=pcall(e.item.GetId,e.item)
    if not ok or id==nil then return reply(n,a,'NO_ID') end
    pending={serial=n,side=side,playerId=pl,player=player,inventory=player.inventory,entityId=en,entity=e,item=e.item,wuid=id}
    return reply(n,a,'PREPARED')
  end
  if not pending then return reply(n,a,'NO_PREPARE') end
  local p=pending;pending=nil
  -- Mark spent BEFORE calling native code; even a side effect followed by an
  -- exception has no retry. Retain the exact reply for duplicate delivery.
  terminal={serial=n,action=a,reply=reply(n,a,'UNKNOWN')}
  if a=='CANCEL' then terminal.reply=reply(n,a,'CANCELLED');return terminal.reply end
  if not current(p) or not itemCurrent(p) then terminal.reply=reply(n,a,'IDENTITY_CHANGED');return terminal.reply end
  local ok,handle=pcall(p.inventory.AddItem,p.inventory,p.wuid)
  terminal.reply=reply(n,a,not ok and 'NATIVE_UNKNOWN' or handle==nil and 'NATIVE_REFUSED' or 'NATIVE_RETURNED')
  -- NATIVE_RETURNED is the actual AddItem result, not fabricated membership,
  -- stack merge, item disappearance or a promised successful transfer.
  return terminal.reply
end
