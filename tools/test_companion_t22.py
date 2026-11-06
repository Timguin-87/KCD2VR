"""Actual archive/Lua composition checks; no PAK output or retail execution."""
import argparse
import copy
import hashlib
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'src/subsystems/throw_drop/resources'))
sys.path.insert(0, 'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
import companion_patch_t22 as p
from lupa.lua51 import LuaRuntime
from test_ammo_supply_t18 import FIXTURE


def run(parent):
    checks = []
    def ck(value, label):
        assert value, label
        checks.append(label)
    def refuse(members, label):
        try: p.plan_members(members, native)
        except p.base.PackageError:
            checks.append(label)
            return
        raise AssertionError(label)
    native = p.prior.Native(Path('C:/Steam/steamapps/common/KingdomComeDeliverance2'))
    raw = parent.read_bytes()
    members = p.base.read_pak(raw)
    frozen = {n: hashlib.sha256(data).hexdigest() for n, (_, data) in members.items()}
    after, changed, removed = p.plan_members(members, native)
    ck(changed == {p.base.SUPPLY_MEMBER, p.prior.TABLE, p.RESERVATION, p.STARTUP_MEMBER}, 'only four declared source/table members changed')
    ck(not removed and p.prior.LUA not in after, 'retired conversion Lua never exported')
    ck(len(after) == len(members)+2, 'only role table and reservation added')
    ck(all(after[n][1] == data for n, (_, data) in members.items() if n not in changed), 'every unrelated parent payload byte-identical')
    ck(frozen == {n:hashlib.sha256(data).hexdigest() for n, (_, data) in members.items()}, 'input archive dictionary unchanged')
    ck(p.plan_members(after, native)[1:] == (set(), set()), 'exact composition idempotent')
    # The current combined tree can omit both retired executor sources and the
    # historical PAK. Fail if composition tries to read any of them.
    original_read = Path.read_bytes
    def current_only(path):
        if path in p.prior.SOURCES.values() or path.name == 'KCD2VR.pak':
            raise AssertionError('historical executor/archive dependency: '+str(path))
        return original_read(path)
    Path.read_bytes = current_only
    try:
        source_independent, _, _ = p.plan_members(members, native)
    finally:
        Path.read_bytes = original_read
    ck({n:d for n,(_,d) in source_independent.items()} == {n:d for n,(_,d) in after.items()},
       'current composition never reads retired executor sources or historical PAK')
    # Legacy source is a frozen TEST INPUT, never a live-resource dependency.
    # Exercise migration without manufacturing an executor through T18's CLI.
    legacy = ROOT/'tests/subsystems/throw_drop/fixtures/legacy_bootstrap_t22'
    old, _ = p.prior.plan_members(members, native, include_carrier=False)
    for name, fixture, expected in [
        (p.base.SUPPLY_MEMBER, 'legacy_loader.lua', 'e7046553bba171cf4c0858dece5d447cfb0324f797c300f8a1ca25550b1e1d8e'),
        (p.prior.LUA, 'legacy_carrier.lua', p.RETIRED_CARRIER_SHA256),
    ]:
        data=(legacy/fixture).read_bytes()
        assert hashlib.sha256(data).hexdigest()==expected
        info=copy.copy(old[p.base.SUPPLY_MEMBER][0]);info.filename=name
        old[name]=(info,data)
    upgraded, _, old_removed = p.plan_members(old, native)
    ck(old_removed == {p.prior.LUA}, 'known retired carrier positively removed')
    ck({n:d for n,(_,d) in upgraded.items()} == {n:d for n,(_,d) in after.items()}, 'old T18 composition migrates to same safe output')
    for path in (p.base.SUPPLY_MEMBER, p.STARTUP_MEMBER, p.RESERVATION, p.prior.LUA):
        bad = copy.deepcopy(after)
        if path not in bad: bad[path] = copy.deepcopy(after[p.RESERVATION])
        bad[path] = (bad[path][0], b'-- unknown edited source\n')
        refuse(bad, 'unknown source refused: '+path)
    bad=copy.deepcopy(members)
    name='Scripts/KCD2VR/other.lua'
    bad[name]=(copy.copy(members[p.base.SUPPLY_MEMBER][0]), b"Script.ReloadScript('Scripts/KCD2VR/native_ammo_supply_t18.lua')")
    refuse(bad, 'second old carrier loader refused')
    bad=copy.deepcopy(members)
    bad[p.RESERVATION.lower()]=(copy.copy(members[p.base.SUPPLY_MEMBER][0]), b'alias')
    refuse(bad, 'owned path case alias refused')

    # Execute the actual existing startup -> new bootstrap -> reservation chain.
    # Only Script.ReloadScript/System/native inventory leaves are substituted.
    lua=LuaRuntime(unpack_returned_tuples=True)
    lua.execute(FIXTURE)
    lua.execute('calls22=0; old22=0; Script={}; function kcd2vr_ammo_supply_t18() old22=old22+1 end; function kcd2vr_supply_native_t2() old22=old22+1 end')
    loaded=[]
    def reload_script(name):
        loaded.append(name)
        assert name in after, name
        lua.execute(after[name][1].decode('utf-8-sig'))
    lua.globals().py_reload22=reload_script
    lua.execute('function Script.ReloadScript(name) return py_reload22(name) end')
    lua.execute(after[p.STARTUP_MEMBER][1].decode('utf-8-sig'))
    ck(loaded == [p.base.SUPPLY_MEMBER,p.RESERVATION], 'actual startup loads exactly bootstrap then finite reservation')
    ck(lua.globals().KCD2VRThrowBootstrapVersion22==22 and lua.globals().KCD2VRThrowReservationVersion20==20, 'actual startup version/API readiness succeeds')
    ck(lua.globals().kcd2vr_ammo_supply_t18 is None and lua.globals().kcd2vr_supply_native_t2 is None, 'retired Lua executors disabled')
    f=lua.globals().F
    key=f.seed('old',1,3)
    call=lua.globals().kcd2vr_throw_reservation_t20
    first=call('100|prepare|7|1|1|0|1|-')
    ck(first.split('|')[1]=='RESERVED', 'loaded actual finite three-axe stock reserves')
    lua.execute(after[p.STARTUP_MEMBER][1].decode('utf-8-sig'))
    ck(loaded == [p.base.SUPPLY_MEMBER,p.RESERVATION,p.base.SUPPLY_MEMBER], 'second startup never reloads reservation state')
    ck(call('100|prepare|7|1|1|0|1|-')==first, 'second startup retains original finite reservation')
    ck(f['items'][key].amount==3 and f.creates==0 and f.removed==0 and lua.globals().old22==0, 'bootstrap/reservation neither equips nor creates/converts/debits')
    for failure in ('missing','no_api','throws'):
        badlua=LuaRuntime(unpack_returned_tuples=True)
        badlua.execute("System={}; Script={}; function kcd2vr_ammo_supply_t18() error('OLD') end; function kcd2vr_supply_native_t2() error('OLD') end")
        def fail_reload(name):
            if failure=='throws': raise RuntimeError('loader failed')
            if failure=='no_api': badlua.globals().KCD2VRThrowReservationVersion20=20
        if failure=='missing':badlua.globals().Script=None
        else:
            badlua.globals().py_reload22=fail_reload
            badlua.execute('function Script.ReloadScript(name) return py_reload22(name) end')
        badlua.execute(after[p.base.SUPPLY_MEMBER][1].decode('utf-8-sig'))
        ck(badlua.globals().KCD2VRThrowBootstrapVersion22 is None and badlua.globals().kcd2vr_ammo_supply_t18 is None and badlua.globals().kcd2vr_supply_native_t2 is None, 'failed bootstrap has no old executor fallback: '+failure)
    return {'parent':str(parent),'parent_sha256':hashlib.sha256(raw).hexdigest(),'checks':checks,'count':len(checks),
            'members_before':len(members),'members_after':len(after),'changed':sorted(changed),'removed':sorted(removed),
            'unrelated_payloads_preserved':len(members)-len(changed & members.keys()),
            'native_or_headset_executed':False,'archive_written':False,'fresh_process_required':True}


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--parent',type=Path,default=ROOT/'companion/C63_UNIFIED/Mods/KCD2VR/Data/KCD2VR.pak')
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    result=run(args.parent)
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(result,indent=2))
