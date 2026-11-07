"""Execute the station packet callback in Lua5.1, with an explicit UIAction leaf."""
from pathlib import Path
from lupa.lua51 import LuaRuntime
lua=LuaRuntime(unpack_returned_tuples=True)
lua.execute((Path(__file__).resolve().parents[1]/'companion/source/Scripts/Startup/zzkcd2vr_prompt_d56.lua').read_text())
lua.execute(r"""
checks=0
function ck(v)checks=checks+1;assert(v)end
ck(kcd2vr_prompt_style_d80('H80|1|1|30|1')=='H80|UNAVAILABLE')
local calls={}
UIAction={CallFunction=function(...)calls[#calls+1]={...};return true end}
for _,s in ipairs({{1,30,1},{1,30,0},{0,100,1},{1,25,1},{1,100,1}})do
 local n=#calls;ck(kcd2vr_prompt_style_d80('H80|17|'..s[1]..'|'..s[2]..'|'..s[3])=='H80|17|DISPATCHED')
 ck(#calls==n+1)
 local c=calls[#calls];ck(c[1]=='HUD' and c[2]==-1 and c[3]=='SetKcd2vrPromptStyleD80')
 ck(c[4]==(s[1]==1) and c[5]==s[2] and c[6]==(s[3]==1) and #c==6)
end
for _,packet in ipairs({'','H80|0|1|30|1','H80|9007199254740992|1|30|1','H80|1|2|30|1','H80|1|1|24|1','H80|1|1|101|1','H80|1|1|30.5|1','H80|1|1|30|2','H80|1|1|30|1\n',string.rep('x',97)})do
 local n=#calls;ck(kcd2vr_prompt_style_d80(packet)=='H80|REFUSED');ck(#calls==n)
end
UIAction.CallFunction=function()return false end
ck(kcd2vr_prompt_style_d80('H80|18|1|30|1')=='H80|18|UNAVAILABLE')
UIAction.CallFunction=function()error('HUD gone')end
ck(kcd2vr_prompt_style_d80('H80|19|0|100|1')=='H80|19|UNAVAILABLE')
""")
print('D80 actual Lua5.1 station packet: %d checks PASS; UIAction is a substituted leaf, no pixels observed'%lua.globals().checks)
