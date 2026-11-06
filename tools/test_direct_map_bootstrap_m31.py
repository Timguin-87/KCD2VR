"""Run actual startup/readiness and M31 together, using explicit entity leaves."""
from pathlib import Path
import runpy,sys,json
sys.argv=['bootstrap-fixture']
m=runpy.run_path(str(Path(__file__).with_name('test_standalone_map19c3.py')),run_name='fixture_only')
lua=m['fixture']()
lua.execute('''
g_localActor=actor(42,true)
assert(string.find(ready(42),'HELPERS_READY',1,true))
assert(type(kcd2vr_map_m31_intent)=='function')
papers[names[1]].hidden=false
assert(kcd2vr_map_m31_intent('I31|1|1')=='I31|1|OPENING')
now=now+2.1;tick();assert(KCD2VRMap.entities[names[1]].paper31.phase=='OPEN')
assert(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSING')
now=now+2.1;tick();assert(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSED')
assert(kcd2vr_map_m31_intent('I31|3|1')=='I31|3|OPENING')
assert(spawned==2 and created==1)
KCD2VRMap.WorldEnded();assert(KCD2VRMap.awaitStart and not KCD2VRMap.world)
''')
print(json.dumps({'result':'PASS','actual_startup_loads_M31':True,'native_request_readiness_fixture':'PASS','repeated_native_clip_open_close':'PASS','native_leaves':'SUBSTITUTED','headset':'NOT_RUN'},indent=2))
