"""Execute the shipped glyph callback in Lua5.1; UIAction itself is a test leaf.
Requires the optional test dependency: pip install lupa. No game process is used.
"""
from pathlib import Path
from lupa.lua51 import LuaRuntime
lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute((Path(__file__).resolve().parents[1]/'companion/source/Scripts/Startup/zzkcd2vr_prompt_d56.lua').read_text())
lua.execute(r"""
checks=0
function ck(x)checks=checks+1;assert(x)end
ck(kcd2vr_prompt_d56('P56|1|4')=='P56|UNAVAILABLE')
local calls={}
UIAction={CallFunction=function(...)calls[#calls+1]={...};return true end}
for _,style in ipairs({2,4})do
 calls={};ck(kcd2vr_prompt_d56('P56|12|'..style)=='P56|12|'..style..'|8')
 for _,c in ipairs(calls)do ck(c[2]==-1 and c[3]=='SetKcd2vrGlyphStyleD68' and c[4]==style and #c==4)end
end
for _,p in ipairs({'','P56|0|2','P56|1|16','P56|1|4junk','P56|9007199254740992|2','P56|1|2\n','P56|x|2',string.rep('a',65)})do ck(kcd2vr_prompt_d56(p)=='P56|REFUSED')end
calls={};UIAction.CallFunction=function(...)local a={...};calls[#calls+1]=a;return false end
ck(kcd2vr_prompt_d56('P56|13|4')=='P56|13|4|0');ck(#calls==16)
for i=2,#calls,2 do ck(calls[i][3]=='SetInputId' and calls[i][4]==2)end
UIAction.CallFunction=function()error('missing function')end
ck(kcd2vr_prompt_d56('P56|14|4')=='P56|14|4|0')
""")
print('D68 actual Lua5.1 callback %d/%d PASS; native UIAction leaf substituted'%(lua.globals().checks,lua.globals().checks))
