"""M39 regression: reopen while the CLOSE clip is still unstreamed (WAIT_CLOSE).
Reuses the exact engine-leaf prelude of test_native_paper_m39.py and the production
map_presentation_m31.lua. Before the fix the paper stayed in WAIT_CLOSE forever.
Does not prove rendering."""
from pathlib import Path
import sys,re
root=Path(__file__).resolve().parents[1]
m39=(root/'tools/test_native_paper_m39.py').read_text()
prelude=m39[m39.index("l.execute('''")+len("l.execute('''"):]
prelude=prelude[:prelude.index("''')")]
head=m39[:m39.index("root=Path(")]
exec(head)  # same lupa import path as the M39 test
l=LuaRuntime(unpack_returned_tuples=True)
l.execute(prelude)
l.execute((root/'mods/KCD2VRMap/map_presentation_m31.lua').read_text())
l.execute('''
-- reproduce: OPEN, close requested while the close clip is not streamed (WAIT_CLOSE), then reopened
assert(intent(1,1)=='I31|1|WAIT_OPEN');header=true;tick(.1,false)
for i=1,40 do tick(.033,true)end;assert(state.paper31.phase=='OPEN')
header=false;assert(intent(2,0)=='I31|2|WAIT_CLOSE')
local r=intent(3,1)                        -- reopen while the close clip is still unstreamed
header=true;for i=1,10 do tick(.033,true)end
assert(state.paper31.phase=='OPEN' or state.paper31.phase=='OPENING','reopen stuck in '..tostring(state.paper31.phase)..' (first answer '..r..')')
for i=1,40 do tick(.033,true)end;assert(state.paper31.phase=='OPEN')
assert(intent(4,0)=='I31|4|CLOSING');for i=1,40 do tick(.033,true)end;assert(intent(4,0)=='I31|4|CLOSED')
print('reopen from WAIT_CLOSE - PASS')
''')
print('PASS M39 reopen from WAIT_CLOSE (production Lua, engine leaves simulated)')
