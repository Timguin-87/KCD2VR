"""Execute source-only reservation Lua against opaque finite inventory leaves."""
import hashlib
import json
from pathlib import Path
import sys
sys.path.insert(0, 'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
from test_ammo_supply_t18 import FIXTURE

ROOT = Path(__file__).resolve().parents[1]
LUA = ROOT/'src/subsystems/throw_drop/resources/native_throw_reservation_t20.lua'
EXTENSION = r'''
function F.call20(serial,action,side,kind,key,body,request)
 return kcd2vr_throw_reservation_t20(table.concat({serial,action,7,body or 1,request or 1,side or 0,kind or 0,key and Framework.WUIDToString(key) or '-'},'|'))
end
function g_localActor.inventory:CreateItem() error('MUTATION FORBIDDEN') end
function ItemManager.RemoveItem() error('MUTATION FORBIDDEN') end
'''


def fresh():
    lua = LuaRuntime(unpack_returned_tuples=True)
    lua.execute(FIXTURE)
    lua.execute(EXTENSION)
    lua.execute(LUA.read_text())
    return lua, lua.globals().F


def main():
    checks = []

    def check(name, result):
        assert result, name
        checks.append(name)

    def status(value):
        return value.split('|')[1]

    lua, f = fresh()
    key=f.seed('old',0,1)
    f['items'][key]['class']='3a640e5d-d8bd-4e8b-b61d-8cd5180e79e7'
    result=f.call20(1,'prepare',1,0,None).split('|')
    check('ordinary unequipped dagger uses exact original WUID', result[1]=='RESERVED' and result[2]==f'[Item-{key}]')
    check('ordinary dagger is counted without grants', result[3]=='1' and f.creates==0 and f.removed==0)
    check('ordinary dagger stow keeps original unit', status(f.call20(1,'clear',1,0,key))=='CLEARED' and f['items'][key].amount==1)
    for excluded in ('80591e34-92ce-44d7-a4ed-b2fcd073eb6e','36099b07-dae2-4b7b-bcb2-5b580e872ec3'):
        lua,f=fresh();key=f.seed('old',0,1);f['items'][key]['class']=excluded
        check('quest/unique not converted '+excluded, status(f.call20(1,'prepare',1,0,None))=='EMPTY' and f.removed==0)

    for kind, which in ((0,'old'), (1,'old'), (0,'ammo'), (1,'ammo')):
        for amount in (1, 3, 5):
            lua, f = fresh()
            key = f.seed(which, kind, amount)
            first = f.call20(1, 'prepare', 0, kind, None)
            check(f'{kind}/{amount} finite original reserved', status(first) == 'RESERVED' and first.split('|')[2] == f'[Item-{key}]')
            check(f'{kind}/{amount} same serial is idempotent', f.call20(1, 'prepare', 0, kind, None) == first)
            check(f'{kind}/{amount} another right serial busy', status(f.call20(99, 'prepare', 0, kind, None)) == 'BUSY')
            check(f'{kind}/{amount} finite second hand', status(f.call20(2, 'prepare', 1, kind, None)) == ('EMPTY' if amount == 1 else 'RESERVED'))
            check(f'{kind}/{amount} draw never debits or migrates', f['items'][key].amount == amount and f.creates == 0 and f.removed == 0 and f.held[key])
            check(f'{kind}/{amount} stale clear refused', status(f.call20(9, 'clear', 0, kind, key)) == 'REFUSED')
            check(f'{kind}/{amount} foreign key clear refused', status(f.call20(1, 'clear', 0, kind, 999)) == 'REFUSED')
            check(f'{kind}/{amount} exact stow bookkeeping clears', status(f.call20(1, 'clear', 0, kind, key)) == 'CLEARED')
            check(f'{kind}/{amount} retired prepare cannot replay', status(f.call20(1, 'prepare', 0, kind, None)) == 'STALE')
            check(f'{kind}/{amount} stow retains original stock', f['items'][key].amount == amount and f.creates == 0 and f.removed == 0)

    lua, f = fresh()
    key = f.seed('old', 0, 2)
    f.call20(1, 'prepare', 0, 0, None)
    f.call20(2, 'prepare', 1, 0, None)
    f['items'][key].amount = 1  # native cost happens outside this Lua API
    f.call20(1, 'clear', 0, 0, key)
    check('remaining reserved unit cannot replenish opposite hand', status(f.call20(3, 'prepare', 0, 0, None)) == 'EMPTY')
    f.call20(2, 'clear', 1, 0, key)
    check('last original available after exact other-hand retirement', status(f.call20(3, 'prepare', 0, 0, None)) == 'RESERVED')
    scans, counts = f.scans, f.counts
    check('refresh actual finite stock', f.call20(4, 'refresh', 0, 0, None).split('|')[3:] == ['1', '0'])
    check('refresh performs only five class counts and no inventory scan', f.scans == scans and f.counts == counts+5)
    check('new body cannot erase current original reservation', status(f.call20(4, 'prepare', 0, 0, None, 2, 2)) == 'BUSY')
    lua.execute(LUA.read_text())
    check('reloading module retains original reservation', status(f.call20(5, 'prepare', 0, 0, None)) == 'BUSY')
    f.call20(3, 'clear', 0, 0, key)
    check('new body can bind only after exact old clear', status(f.call20(4, 'prepare', 0, 0, None, 2, 2)) == 'RESERVED')
    check('old clear cannot erase new body', status(f.call20(3, 'clear', 0, 0, key)) == 'REFUSED')
    check('conversion API unavailable', status(f.call20(4, 'convert', 0, 0, key, 2, 2)) == 'BAD')

    for kind in (0, 1):
        lua, f = fresh()
        first_key=f.seed('old',kind,1)
        second_key=f.seed('ammo',kind,1)
        right=f.call20(1,'prepare',0,kind,None).split('|')
        left=f.call20(2,'prepare',1,kind,None).split('|')
        check(f'{kind} mixed native stock uses two distinct actual units', right[1]==left[1]=='RESERVED' and right[2]!=left[2])
        check(f'{kind} mixed native stock counts actual total', right[3+kind]=='2' and left[3+kind]=='2')
        selected=int(right[2][6:-1]);f.held[selected]=None
        f.call20(1,'clear',0,kind,selected)
        check(f'{kind} mixed remaining other-hand unit is not cloned', status(f.call20(3,'prepare',0,kind,None))=='EMPTY' and f.creates==0 and f.removed==0)

    lua,f=fresh()
    key=f.seed('old',0,1)
    f.call20(1,'prepare',0,0,None)
    lua.globals().replaceKey=key
    lua.execute("F.items[replaceKey].class='36ac8977-c630-58ae-95a1-21307111d330'")
    check('in-place stock class replacement cannot rewrite old reservation', status(f.call20(1,'prepare',0,0,None))=='EMPTY')

    lua, f = fresh()
    recovered=f.seed('ammo', 0, 10)
    response=f.call20(1, 'prepare', 0, 0, None)
    check('recovered native Ammo retains its actual finite key and quantity', status(response) == 'RESERVED' and response.split('|')[2] == f'[Item-{recovered}]' and response.split('|')[3]=='10' and f['items'][recovered].amount==10 and f.creates==0 and f.removed==0)
    f.call20(1, 'clear', 0, 0, recovered)
    lua, f = fresh()
    key = f.seed('old', 1, 3)
    lua.execute('''local old=ItemManager.GetItem
ItemManager.GetItem=function(k) F.reentry=kcd2vr_throw_reservation_t20('99|prepare|7|1|1|1|1|-');return old(k) end''')
    check('actual reentrant inventory callback cannot reserve second hand', status(f.call20(1, 'prepare', 0, 1, None)) == 'RESERVED' and status(f.reentry) == 'BUSY')
    lua.execute('''savedGetter=ItemManager.GetItem;ItemManager.GetItem=function() error('native query failed') end''')
    check('query exception is unknown without changing original reservation', status(f.call20(1, 'prepare', 0, 1, None)) == 'UNKNOWN')
    lua.execute('ItemManager.GetItem=savedGetter')
    check('read-only query retry reuses same reservation', status(f.call20(1, 'prepare', 0, 1, None)) == 'RESERVED')
    lua.execute('g_localActor=nil')
    check('original clear works after actor disappears', status(f.call20(1, 'clear', 0, 1, key)) == 'CLEARED')

    for callback in ('GetCountOfClass', 'GetInventoryTable'):
        lua, f = fresh()
        f.seed('old', 0, 5)
        lua.execute(f'''local old=g_localActor.inventory.{callback}
g_localActor.inventory.{callback}=function(self,...) local result=old(self,...);g_localActor={{id=7,inventory=self}};return result end''')
        check(callback+' replacement actor cannot publish old read', status(f.call20(1, 'prepare', 0, 0, None)) == 'WAIT')
    lua, f = fresh()
    key = f.seed('old', 0, 1)
    lua.execute('''local old=ItemManager.GetItem
ItemManager.GetItem=function(k) local q=old(k);g_localActor={id=7,inventory=g_localActor.inventory};return q end''')
    check('GetItem replacement actor cannot publish old read', status(f.call20(1, 'prepare', 0, 0, None)) == 'WAIT')
    check('read-side failure never mutates inventory', f['items'][key].amount == 1 and f.creates == 0 and f.removed == 0)
    for value in ('', '1|prepare', 'nan|prepare|7|1|1|0|0|-', '1|prepare|7|1|1|2|0|-', '1|prepare|7|1|1|0|0|-|extra'):
        check('malformed packet '+value, status(lua.globals().kcd2vr_throw_reservation_t20(value)) == 'BAD')
    report = dict(passed=True, checks=len(checks), cases=checks, actual_lua='Lua 5.1',
        source_sha256=hashlib.sha256(LUA.read_bytes()).hexdigest(), native_execution=False, registered=False,
        limits=['No native item debit/projectile launch/save verification; inventory query leaves substituted.'])
    out = ROOT/'review/T20/RESERVATION_LUA.json'
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps(dict(passed=True, checks=len(checks), actual_lua='Lua 5.1', native_execution=False)))


if __name__ == '__main__':
    main()
