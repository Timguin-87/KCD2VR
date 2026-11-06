#!/usr/bin/env python3
"""Extract the live collision adapter; mock the native call and monotonic clock."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib
import os
def atomic_text(path,text):
    temporary=path.with_name(path.name+".extract-tmp")
    temporary.write_text(text)
    os.replace(temporary,path)

root=Path(__file__).resolve().parents[1]
s=read_source(root/'src/frik_pose_probe.cpp')
a=s.index('// V66.92R1: corrected world pointer and fault containment.')
b=s.index('\nnamespace {',a)
body=s[a:b]
needle='auto fn=reinterpret_cast<RayWorldIntersectionFn6692>(base+wc::kHelperRva);'
assert body.count(needle)==1
body=body.replace(needle,'auto fn=&MockRay; // native API replacement only')
# H18: retain the production once-per-sample budget initializer, while the
# unrelated native Door selection/reservation leaf is explicitly absent here.
# Door credit reservation/return has its own connected fixture. Importing its
# entire host closure made this focused world adapter unit unbuildable on MSVC.
budget=read_source(root/'src/subsystems/doors/door_surface_budget65.inl')
begin=budget[budget.index('static void BeginContactBudget65('):]
assert begin.count('ReserveDoorSurfaceCredits65(sample,now)')==1
begin=begin.replace('if(reserveDoor)ReserveDoorSurfaceCredits65(sample,now);',
                   '(void)reserveDoor;(void)now; // HOST LEAF: no Door reservation in this unit')
budget_host=('struct DoorSurfaceCredits65 {std::uint64_t sample=0;unsigned count=0;};\n'
             'static thread_local DoorSurfaceCredits65 g_door_surface_credits65;\n'+begin)
assert body.count('#include "vrik/door_surface_budget65.inl"')==1
body=body.replace('#include "vrik/door_surface_budget65.inl"',budget_host)
assert body.count('BeginContactBudget65(key,GetTickCount64())')==1
body=body.replace('BeginContactBudget65(key,GetTickCount64())','BeginContactBudget65(key,tick)') # HOST clock from supplied pass
clock_start=body.index('static std::uint64_t ContactMicros6695() noexcept {')
clock_end=body.index('\n}',clock_start)+2
body=body[:clock_start]+'static std::uint64_t ContactMicros6695() noexcept { return mockMicros59c1; }'+body[clock_end:]
# H24: exercise the actual final free-hand caller order and owner filters. The
# test supplies palm geometry/current eligibility as explicit leaves, while the
# existing ContactPass/ContactHand/native query adapter above run unchanged.
caller=read_source(root/'src/subsystems/physics/pose_free_hand_contact.inl')
caller=caller[caller.index('    if(collisionMode&&!perfContactModeDs2){\n'):]
assert caller.count('ContactHand6695(')==1 and caller.count('CollisionStatus6692(')==1
import re
caller_code=re.sub(r'(?m)^\s*//[^\n]*(?:\n|$)', '', caller)
assert '//' not in caller_code and '\\' not in caller_code
caller_macro='\n// Free-hand caller source SHA-256: '+hashlib.sha256(caller.encode()).hexdigest()+'\n'
caller_macro+='#define WORLD_ADAPTER_FREE_HAND_CALLER_H24 \\\n'+' \\\n'.join(caller_code.splitlines())+'\n'
atomic_text(root/'tests/generated_free_hand_caller.inc',
    '// Auto-extracted current caller; host fixtures provide explicit service leaves.\n'+caller_macro)
atomic_text(root/'tests/generated_world_adapter.inc',
    '// Auto-extracted from active frik_pose_probe.cpp; native call replaced by MockRay.\n'
    '// Unmodified source slice SHA-256: '+hashlib.sha256(s[a:b].encode()).hexdigest()+'\n'+body+caller_macro)
