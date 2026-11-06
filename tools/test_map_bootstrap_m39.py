"""Actual startup + presentation composition, current native animator leaves."""
from pathlib import Path
import runpy,sys,json
sys.argv=['bootstrap-fixture']
m=runpy.run_path(str(Path(__file__).with_name('test_standalone_map19c3.py')),run_name='fixture_only')
l=m['fixture']()
l.execute('''
g_localActor=actor(42,true)
assert(string.find(ready(42),'HELPERS_READY',1,true))
for _,e in pairs(papers)do
 e.ResetAnimation=function(self,slot,layer)self.progress=0 end
 e.Activate=function(self,v)self.active=v==1 end
 e.SetAnimationSpeed=function(self,slot,layer,speed)assert(speed==0 or speed==1);self.speed=speed end
end
papers[names[1]].hidden=false
assert(kcd2vr_map_m31_intent('I31|1|1')=='I31|1|OPENING')
assert(papers[names[1]].Properties.Animation.bAlwaysUpdate==true)
papers[names[1]].progress=1;now=now+.1;tick()
assert(KCD2VRMap.entities[names[1]].paper31.phase=='OPEN')
assert(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSING')
assert(papers[names[1]].active)
papers[names[1]].progress=1;now=now+.1;tick()
assert(kcd2vr_map_m31_intent('I31|2|0')=='I31|2|CLOSED')
assert(not papers[names[1]].active)
assert(kcd2vr_map_m31_intent('I31|3|1')=='I31|3|OPENING')
assert(spawned==2 and created==1) -- no Inventory UI owner or extra actor
KCD2VRMap.WorldEnded();assert(KCD2VRMap.awaitStart and not KCD2VRMap.world)
assert(papers[names[1]].hidden and papers[names[2]].hidden)
''')
print(json.dumps({'actual_startup_native_clock_presentation':'PASS','helper_and_inventory_supply_counts_preserved':'PASS','native_render':'NOT_RUN'},indent=2))
