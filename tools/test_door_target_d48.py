#!/usr/bin/env python3
"""Execute actual Lua 5.1 parser/target-selection prefix, with explicit entity leaves.

This does not execute retail AnimDoor, animation admission, collision, or the game.
The packet input is produced by the separately compiled actual C++ serializer.
Requires lupa with its Lua 5.1 module (tested with lupa 2.6).
"""
import argparse
import hashlib
import json
from pathlib import Path
from lupa.lua51 import LuaRuntime

p=argparse.ArgumentParser(description=__doc__)
p.add_argument('--packet-output',type=Path,required=True)
p.add_argument('--out',type=Path,required=True)
a=p.parse_args();root=Path(__file__).resolve().parents[1];a.out.mkdir(parents=True,exist_ok=True)
path=root/'mods/KCD2VRDoors/hand_only_h40.lua';source=path.read_text(encoding='utf-8-sig')
lua=LuaRuntime(unpack_returned_tuples=True)
syntax=lua.eval('function(code,name) local fn,err=loadstring(code,name); assert(fn,err); return true end')
syntax(source,'@hand_only_h40.lua')
door=root/'mods/KCD2VRDoors/door_runtime.lua';syntax(door.read_text(encoding='utf-8-sig'),'@door_runtime.lua')
helpers='\n'.join(line for line in source.splitlines() if line.startswith(('local function finite(', 'local function integer(', 'local function vector(')))+'\n'
distance=source[source.index('local function distance('):source.index('local retainedCandidateH44 --')]
parser=source[source.index('local function parse(input)'):source.index('local function sameIdentity(p)')]
selection=source[source.index('local function reserve('):source.index('  if not e or D.testsH41[e] then return end')]
program=helpers+distance+parser+selection+'  return e\nend\nreturn {parse=parse,select=reserve}\n'
(a.out/'actual_target_functions.lua').write_text(program)
api=lua.execute(program)
checks=0
def check(ok,label):
    global checks
    checks+=1
    if not ok:raise AssertionError(label)

packets={line.split(' ',1)[0]:line.split(' ',1)[1] for line in a.packet_output.read_text().splitlines() if line.startswith(('OFFERED ', 'UNSPECIFIED ', 'CLEAR '))}
offered=api['parse'](packets['OFFERED']);check(offered is not None,'actual C++ offered packet parses')
check(offered['hand'][0]['offered']==101 and offered['hand'][1]['offered']==202,'right/left offered identity survives serialization')
check(api['parse'](packets['UNSPECIFIED'])['hand'][0]['offered']==0,'no target retains nearest policy')
check(api['parse'](packets['CLEAR'])['hand'][0]['g']==-1,'release wire parses')
legacy='|'.join(packets['OFFERED'].split('|')[:16]);check(api['parse'](legacy)['hand'][1]['offered']==0,'legacy sixteen fields remain compatible')
for suffix in ('-1|202','4294967296|202','nan|202','101|202|303'):
    check(api['parse'](legacy+'|'+suffix) is None,'malformed offered identity rejected: '+suffix)

lua.execute('''
entities={}; nearestCalls=0; retained=nil; retainedDistance=nil
System={GetEntity=function(id) return entities[id] end}
nearest=function(p) nearestCalls=nearestCalls+1;return entities[101] end
retainedCandidateH44=function(p) return retained,retainedDistance end
function makeTarget(id,x,cls)
 local e={id=id,class=cls or 'AnimDoor'}
 function e:GetWorldBBox() return {x=x,y=-.02,z=-.02},{x=x+.04,y=.02,z=.02} end
 entities[id]=e;return e
end
makeTarget(101,.01);makeTarget(202,.12);makeTarget(303,3);makeTarget(404,.01,'Other')
''')
origin=lua.table_from({'x':0,'y':0,'z':0})
def select(target):return api['select'](0,None,19,origin,.9,1000,41,target)
check(select(202)['id']==202 and lua.globals().nearestCalls==0,'explicit offered target wins over a closer different door')
check(select(0)['id']==101 and lua.globals().nearestCalls==1,'no offered target uses existing nearest path')
for target in (999,303,404):check(select(target) is None,'missing/far/wrong-class offered target rejected')
check(lua.globals().nearestCalls==1,'failed explicit identity never substitutes nearest')
lua.execute('retained=entities[303];retainedDistance=.2')
check(select(303)['id']==303,'existing retained target can supply its validated near distance')
lua.execute('retained=entities[101];retainedDistance=.01')
check(select(303) is None,'different retained door cannot qualify offered identity')
receipt={'status':'PASS','checks':checks,'lua':lua.eval('_VERSION'),'scope':'actual C++ packet output; actual Lua parser and target-selection prefix; explicit entity, bounds and retained-distance leaves',
         'retail_AnimDoor':'NOT_RUN','native_game':'NOT_RUN','whole_Lua_script_syntax':'PASS',
         'inputs':{str(q.relative_to(root)):hashlib.sha256(q.read_bytes()).hexdigest() for q in (path,door,root/'tests/d48_door_packet_test.cpp',Path(__file__))},
         'extracted_sha256':hashlib.sha256(program.encode()).hexdigest()}
(a.out/'RESULT.json').write_text(json.dumps(receipt,indent=2)+'\n')
print(f'PASS D48 offered door identity: {checks} checks; Lua 5.1; retail/native game NOT RUN')
