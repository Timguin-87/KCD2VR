#!/usr/bin/env python3
"""Actual pinned retail AnimDoor + both production VR door scripts, host leaves.
Tests lock denial and continued unlocked hand operation; no engine/headset claim.
"""
import argparse
import ast
import hashlib
import json
from pathlib import Path
import zipfile
from lua_host_r94 import LuaHost

ROOT = Path(__file__).resolve().parents[1]


def literal(path, name):
    for node in ast.parse(path.read_text()).body:
        if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == name for t in node.targets):
            return ast.literal_eval(node.value)
    raise ValueError(name)


def main():
    p = argparse.ArgumentParser(); p.add_argument('--scripts-pak', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True); a = p.parse_args()
    with zipfile.ZipFile(a.scripts_pak) as z: retail = z.read('Scripts/Entities/Doors/AnimDoor.lua')
    expected = '62aad1a999e37babf447b39583ad251e4a7facbdd297f92029915da4759068e4'
    if hashlib.sha256(retail).hexdigest() != expected: raise ValueError('Pinned retail script differs')
    base = ROOT/'tools/test_door_retail64.py'; full = ROOT/'tools/test_hand_only_doors_h40.py'
    modules = [ROOT/'mods/KCD2VRDoors/door_runtime.lua', ROOT/'mods/KCD2VRDoors/hand_only_h40.lua']
    scenarios = {
        'locked_no_key': 'd.bLocked=true;hasKey=false',
        'locked_with_key': 'd.bLocked=true;hasKey=true',
        'locked_manual_side': "d.bLocked=true;d.Properties.Lock.esLockTypes='none/key'",
        'numeric_locked': 'd.bLocked=1;hasKey=true',
        'unknown_lock': "d.bLocked='unknown';hasKey=true",
        'unlocked_right': 'd.bLocked=false',
        'unlocked_left': 'd.bLocked=false',
        'relocked_right': 'd.bLocked=false',
        'relocked_left': 'd.bLocked=false',
    }
    results = []
    for reverse in (False, True):
        for name, setup in scenarios.items():
            with LuaHost() as lua:
                lua.run(literal(base, 'LEAVES'), 'test/native_leaves.lua')
                lua.run(retail, 'Scripts/Entities/Doors/AnimDoor.lua')
                lua.run(literal(base, 'DOUBLES'), 'test/native_object_leaves.lua')
                lua.run(literal(full, 'EXTERNAL'), 'test/h40_external_leaves.lua')
                for m in (list(reversed(modules)) if reverse else modules): lua.run(m.read_text(), m.relative_to(ROOT).as_posix())
                lua.run("d=makeDoor(101);d.Properties.Lock.esLockTypes='key/key';"+setup)
                left = name.endswith('_left')
                feed = "claim1=claim1+1;packet('feed',-1,nil,nil,.9,.5,0)" if left else "claim0=claim0+1;packet('feed',.9,.5,0,-1)"
                lua.run(feed)
                if name.startswith('relocked'):
                    lua.run("check(mask()~=0 and d.rate==0,'unlocked door initially held');local before=writes;d.bLocked=true;frame(d,.01);check(writes==before and d.rate==1,'new native lock stops controller seeks and returns only original clock');finish(d);check(mask()==0 and unlockReports==0 and doorReports==1 and objectStarts==1,'native relock retires original operation without unlock or repeat')")
                elif name.startswith('unlocked'):
                    lua.run("check(mask()~=0 and objectStarts==1 and d.rate==0,'unlocked Grip retains the object-only door owner');check(nativeActions==0 and unlockReports==0,'hand path creates no Henry action or lock mutation');packet('releaseall',-1,nil,nil,-1);finish(d);check(mask()==0 and doorReports==1,'native hand release completes exactly once')")
                elif name in ('locked_with_key','locked_manual_side'):
                    lua.run("check(mask()~=0 and objectStarts==1 and d.rate==0 and d.bLocked==false,'native key/manual unlock retains hand operation');check(unlockReports==1 and nativeActions==0,'retail unlock runs exactly once');packet('releaseall',-1,nil,nil,-1);finish(d);check(mask()==0 and doorReports==1 and unlockReports==1,'native completion without repeated unlock')")
                else:
                    lua.run("check(mask()==0 and objectStarts==0,'locked or unknown door never admits a hand');check(unlockReports==0 and speedWrites==0 and writes==0,'no unlock, clock or seek side effect');d.bLocked=false;"+feed+";check(mask()~=0 and objectStarts==1,'same door works after native lock state clears');packet('releaseall',-1,nil,nil,-1);finish(d);check(mask()==0 and doorReports==1,'unlocked native operation completes')")
                results.append({'order':'h40_first' if reverse else 'r4_first', 'case':name, 'checks':int(lua.number('checks')), 'status':'PASS'})
    receipt = {'checks':sum(r['checks'] for r in results), 'results':results, 'retail_sha256':expected,
        'runtime':'Lua 5.1 via lupa on Windows; Lua 5.4 compatibility host elsewhere', 'game':'NOT_RUN', 'headset':'NOT_RUN',
        'source_sha256':{m.relative_to(ROOT).as_posix():hashlib.sha256(m.read_bytes()).hexdigest() for m in modules}}
    a.out.mkdir(parents=True, exist_ok=True); (a.out/'RESULT.json').write_text(json.dumps(receipt, indent=2)+'\n')
    print('door_lock_r94:',receipt['checks'],'checks PASS; pinned retail + production scripts; game/headset NOT_RUN')


if __name__ == '__main__': main()
