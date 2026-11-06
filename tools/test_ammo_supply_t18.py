"""Execute production T18 Lua with a finite inventory fixture, never the game."""
import json
import sys
from pathlib import Path

sys.path.insert(0, 'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = r'''
local old={'43286e47-cde0-50bd-ba4c-9747b777b16d','c0760688-f426-5341-95b9-482b87dbcb9b'}
local ammo={'36ac8977-c630-58ae-95a1-21307111d330','ac7fa1ea-0341-5135-baf9-efda53cde18c'}
local launcher='dc877f56-16f3-5889-b606-5617658717cc'
F={items={},held={},next=10,scans=0,creates=0,counts=0,removed=0,mode='ok'}
local function wuid(n) return {value=n} end -- each native return may be a new opaque wrapper
local function id(k) return type(k)=='table' and k.value or k end
Framework={WUIDToString=function(k) return '[Item-'..id(k)..']' end}
ItemManager={
 GetItem=function(k) local q=F.items[id(k)];if not q then return nil end
  return {id=wuid(id(k)),class=q.class,amount=q.amount} end,
 RemoveItem=function(k) F.items[id(k)]=nil;F.removed=F.removed+1 end
}
g_localActor={id=7,inventory={}}
System={GetEntity=function(n) return n==7 and g_localActor or nil end}
function F.add(guid,n)
 F.next=F.next+1;F.items[F.next]={class=guid,amount=n};F.held[F.next]=true;return F.next
end
function g_localActor.inventory:GetInventoryTable()
 F.scans=F.scans+1;local r={};for k in pairs(F.held) do r[#r+1]=wuid(k) end;return r
end
function g_localActor.inventory:GetCountOfClass(guid)
 F.counts=F.counts+1;local n=0;for k in pairs(F.held) do local q=F.items[k];if q.class==guid then n=n+q.amount end end;return n
end
function g_localActor.inventory:CreateItem(guid,amount,quality)
 F.creates=F.creates+1
 if F.mode=='no_create' and guid~=launcher then error('refused before insertion') end
 if F.mode=='reentry' and guid~=launcher then F.reentry=kcd2vr_ammo_supply_t18(F.reentryPacket) end
 local key=nil
 for k in pairs(F.held) do if F.items[k].class==guid then key=k;F.items[k].amount=F.items[k].amount+amount;break end end
 key=key or F.add(guid,amount)
 if F.mode=='after_create' and guid~=launcher then error('native inserted then callback failed') end
 return wuid(key)
end
function F.call(serial,action,side,kind,key,body,request)
 local packet=table.concat({serial,action,7,body or 1,request or 1,side or 0,kind or 0,key and Framework.WUIDToString(key) or '-'},'|')
 return kcd2vr_ammo_supply_t18(packet)
end
function F.seed(which,kind,n) return F.add(which=='old' and old[kind+1] or ammo[kind+1],n or 1) end
'''


def fresh():
    lua = LuaRuntime(unpack_returned_tuples=True)
    lua.execute(FIXTURE)
    lua.execute((ROOT / 'src/subsystems/throw_drop/resources/native_ammo_supply_t18.lua').read_text())
    return lua, lua.globals().F


def main():
    checks = []

    def check(name, condition):
        assert condition, name
        checks.append(name)

    for kind in (0, 1):
        lua, f = fresh()
        key = f.seed('old', kind, 1)
        check(f'legacy {kind} prepared with opaque WUIDs', f.call(1, 'prepare', 0, kind, None).split('|')[1] == 'PREPARED')
        check(f'legacy {kind} conversion without escrow refused', f.call(1, 'convert', 0, kind, key).split('|')[1] == 'REFUSED')
        f.held[key] = None  # exact native escrow movement is C++ responsibility
        converted = f.call(1, 'convert', 0, kind, key).split('|')
        check(f'legacy {kind} one Ammo inserted', converted[1] == 'CONVERTED' and f.creates == 2 and f.removed == 1)
        check(f'legacy {kind} repeated conversion returns receipt', f.call(1, 'convert', 0, kind, key).split('|')[2] == converted[2] and f.creates == 2)
        check(f'legacy {kind} converted unit remains reserved from opposite hand', f.call(2, 'prepare', 1, kind, None).split('|')[1] == 'EMPTY')
        f.call(1, 'clear', 0, kind, key)
        check(f'legacy {kind} next held Ammo reuse', f.call(2, 'prepare', 0, kind, None).split('|')[1] == 'PREPARED' and f.creates == 2)

    lua, f = fresh()
    key = f.seed('ammo', 0, 2)
    check('stack reserves right unit', f.call(1, 'prepare', 0, 0, None).split('|')[1] == 'PREPARED')
    check('stack reserves left unit independently', f.call(2, 'prepare', 1, 0, None).split('|')[1] == 'PREPARED')
    check('same side cannot reacquire held unit', f.call(3, 'prepare', 0, 0, None).split('|')[1] == 'BUSY')
    f.call(99, 'clear', 0, 0, key)
    check('stale clear cannot retire current reservation', f.call(4, 'prepare', 0, 0, None).split('|')[1] == 'BUSY')
    f.call(1, 'clear', 0, 0, key)
    check('exact clear permits reacquisition', f.call(5, 'prepare', 0, 0, None).split('|')[1] == 'PREPARED')
    f['items'][key].amount = 1  # native flight has moved one of the two real units
    f.call(5, 'clear', 0, 0, key)
    check('remaining held unit cannot refill just-released hand', f.call(6, 'prepare', 0, 0, None).split('|')[1] == 'EMPTY')
    f.call(2, 'clear', 1, 0, key)
    check('actual last retirement makes remaining stock available', f.call(7, 'prepare', 0, 0, None).split('|')[1] == 'PREPARED')

    for kind in (0, 1):
        lua, f = fresh()
        old_key = f.seed('old', kind, 1)
        f.call(1, 'prepare', 0, kind, None)
        f.held[old_key] = None
        stack = f.seed('ammo', kind, 1)  # separate real unit arrives before conversion
        result = f.call(1, 'convert', 0, kind, old_key).split('|')
        check(f'converted {kind} joins actual two-unit stack', result[1] == 'CONVERTED' and result[2] == f'[Item-{stack}]' and f['items'][stack].amount == 2)
        check(f'converted {kind} two actual units permit two claims', f.call(2, 'prepare', 1, kind, None).split('|')[1] == 'PREPARED')
        f['items'][stack].amount = 1
        f.call(1, 'clear', 0, kind, old_key)
        check(f'converted {kind} old-key receipt clears only retired hand', f.call(3, 'prepare', 0, kind, None).split('|')[1] == 'EMPTY')

    lua, f = fresh()
    key = f.seed('ammo', 0, 1)
    f.call(1, 'prepare', 0, 0, None)
    check('one Ammo cannot be reserved by both hands', f.call(2, 'prepare', 1, 0, None).split('|')[1] == 'EMPTY')
    scans, creates, counts = f.scans, f.creates, f.counts
    check('refresh counts current finite stock', f.call(3, 'refresh', 0, 0, None).split('|')[5:] == ['1', '0'])
    check('refresh has zero inventory scans or grants', f.scans == scans and f.creates == creates and f.counts == counts + 4)

    for mode in ('no_create', 'after_create', 'reentry'):
        lua, f = fresh()
        key = f.seed('old', 1, 1)
        f.call(1, 'prepare', 0, 1, None)
        f.held[key] = None
        f.mode = mode
        f.reentryPacket = f'1|convert|7|1|1|0|1|[Item-{key}]'
        result = f.call(1, 'convert', 0, 1, key).split('|')[1]
        check(mode + ' outcome uses actual census', result == ('UNCERTAIN' if mode == 'no_create' else 'CONVERTED'))
        f.call(1, 'convert', 0, 1, key)
        check(mode + ' never creates twice', f.creates == 2)
        if mode == 'reentry':
            check('reentrant CreateItem blocked', f.reentry.split('|')[1] == 'BAD')

    for kind in (0, 1):
        for mode in ('net_zero', 'offset_churn'):
            lua, f = fresh()
            key = f.seed('old', kind, 1)
            f.call(1, 'prepare', 0, kind, None)
            f.held[key] = None
            prior = f.seed('ammo', kind, 1 if mode == 'net_zero' else 2)
            other = f.seed('ammo', kind, 3) if mode == 'offset_churn' else 0
            lua.globals().removedPrior = prior
            lua.globals().increasedOther = other
            lua.execute("""
function g_localActor.inventory:CreateItem(guid,amount,quality)
 F.creates=F.creates+1
 F.held[removedPrior]=nil
 if increasedOther~=0 then F.items[increasedOther].amount=F.items[increasedOther].amount+2 end
 F.newAmmo=F.add(guid,amount)
 return true
end
""")
            response=f.call(1, 'convert', 0, kind, key).split('|')[1]
            check(f'{mode} {kind} cannot authorize old return or conversion', response == 'UNCERTAIN' and f.removed == 0 and f.held[f.newAmmo])
            f.call(1, 'convert', 0, kind, key)
            check(f'{mode} {kind} mutation is never retried', f.creates == 2)

    lua, f = fresh()
    f.seed('ammo', 1, 1)
    f.call(1, 'prepare', 0, 1, None)
    check('new body invalidates old reservation', f.call(2, 'prepare', 0, 1, None, 2, 2).split('|')[1] == 'PREPARED')
    check('body change reuses private source', f.creates == 1)
    f.call(1, 'clear', 0, 1, None, 1, 1)
    check('old-body clear cannot roll current reservations back', f.call(3, 'prepare', 0, 1, None, 2, 2).split('|')[1] == 'BUSY')
    check('wrong original receipt cannot acknowledge clear', f.call(2, 'clear', 0, 1, 999, 2, 2).split('|')[1] == 'REFUSED')
    lua.execute('savedActor=g_localActor;g_localActor=nil')
    check('terminal clear survives missing old actor', f.call(2, 'clear', 0, 1, None, 2, 2).split('|')[1] == 'CLEARED')
    lua.execute('g_localActor=savedActor')
    check('old actor clear actually released reservation', f.call(4, 'prepare', 0, 1, None, 2, 2).split('|')[1] == 'PREPARED')
    lua.execute('Framework=nil')
    check('exact serial cleanup needs no inventory or WUID API', f.call(4, 'clear', 0, 1, None, 2, 2).split('|')[1] == 'CLEARED')
    source = (ROOT / 'src/subsystems/throw_drop/resources/native_ammo_supply_t18.lua').read_text()
    check('no equipment or native animation commands', all(s not in source for s in (':DrawFromInventory(', ':Equip(', ':Unequip(', 'SetHealth(', 'ServerHit(')))
    print(json.dumps({'suite': 'T18 production Lua', 'checks': len(checks), 'passed': checks, 'game_executed': False}, indent=2))


if __name__ == '__main__':
    main()
