from pathlib import Path
import importlib.util,json,subprocess,sys
from m41_test_support import options,lua_runtime,companion_pak
args=options('bootstrap');LANE=Path(__file__).parent;BASE=args.source_root;OUT=args.out
lua_runtime(args)
spec=importlib.util.spec_from_file_location('m41_fixture',LANE/'test_m41_lua.py');module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
lua=module.lua
lua.execute('uiCalls=0;uiReads=0;UIAction.CallFunction=function(...)uiCalls=uiCalls+1;return true end;UIAction.GetVariable=function(...)uiReads=uiReads+1;return nil end')
for name in ['map_navigation_t2.lua','map_native_live_h28.lua','map_native_read_h25.lua']:
 lua.execute((BASE/'mods/KCD2VRMap'/name).read_text())
lua.execute('''
check(type(KCD2VRMap.CreateNavigationH28)=='function')
check(type(kcd2vr_map_native_live_h28)=='function')
check(type(kcd2vr_map_native_read_h25)=='function')
check(uiCalls==0 and uiReads==0)
KCD2VRMap.WorldEnded()
check(uiCalls==0 and uiReads==0)
local reply=requestReadyM41('R3|31|21|42')
check(reply:find('|HELPER_READY|',1,true)~=nil)
check(uiCalls==0 and uiReads==0)
check(kcd2vr_map_m31_intent('I31|32|0')=='I31|32|CLOSED')
check(uiCalls==0 and uiReads==0)
''')
# Independent native-book modules/bytecode are unchanged. Exercise their real
# existing fixture against the selected current companion, no game process.
pak,pak_receipt=companion_pak(args)
# Import the existing native-book fixture from this same current source tree.
# Native leaves remain its explicit substitutes; all Lua/AVM and identity checks
# execute unchanged. No reliance on the old donor source/build directory.
import io,contextlib
sys.path.insert(0,str(BASE/'tools'))
book_spec=importlib.util.spec_from_file_location('m41_native_book_fixture',BASE/'tools/test_native_map_live_h28.py')
book=importlib.util.module_from_spec(book_spec);book_spec.loader.exec_module(book)
stream=io.StringIO()
with contextlib.redirect_stdout(stream):book.main(pak)
log=stream.getvalue();(OUT/'NATIVE_BOOK_RUN.log').write_text(log)
assert '"result": "PASS"'in log,log[-2000:]
report=dict(actual_static_bootstrap_with_preserved_native_modules=9,static_start_open_close_UI_calls=lua.globals().uiCalls,
 static_start_open_close_UI_reads=lua.globals().uiReads,native_book_existing_fixture_exit=0,
 native_book_pak=pak_receipt,native_book_native_leaves='SUBSTITUTED',headset='NOT_RUN')
(OUT/'RESULT.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
