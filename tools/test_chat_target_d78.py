#!/usr/bin/env python3
"""Replay real chat descriptor + eligibility code on Lua 5.1 via lupa.lua51."""
import argparse, hashlib, json
from pathlib import Path
from lupa.lua51 import LuaRuntime

ROOT=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--out',type=Path,required=True)
args=parser.parse_args()
lua=LuaRuntime(unpack_returned_tuples=True)
lua.globals().KCD2VR_SOURCE_ROOT_D78=str(ROOT)
fixture=ROOT/'tests/quick_talk_target_d78_test.lua'
checks=lua.execute(fixture.read_text())
sources=[fixture,ROOT/'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua',
         ROOT/'companion/source/Scripts/Startup/zzkcd2vr_chat_target_d78.lua']
report={'runtime':lua.eval('_VERSION'),'pass':True,'checks':checks,
        'scope':'Actual current GetChatActions and eligibility bridge on Lua 5.1. Native Action builder and actor methods are explicit fixture leaves. No native action, engine or headset execution.',
        'sources':{str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources}}
args.out.parent.mkdir(parents=True,exist_ok=True)
args.out.write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
