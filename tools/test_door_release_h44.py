"""H44 door release + reacquisition against actual pinned retail Lua and DBA keys.
Only native leaf/scheduler bindings are doubles. No engine/headset proof claimed.
"""
from pathlib import Path
import hashlib,json,sys,zipfile
BASE=Path(r"D:/KCD2VR-review-builds/H41_STEP30_F9_INDEPENDENT_GRIP_20261024/delivery_8a8c547_CANONICAL_HOST_QA2/KCD2VR_SOURCE")
LANE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(BASE/'tools'))
import test_hand_only_doors_h40 as old
from verify_door_clip_angle_h40 import decoded_curves,DBA_MEMBER,DBA_SHA
H40=LANE/'mods/KCD2VRDoors/hand_only_h40.lua'
SCENARIOS={
'moving_release':r"""
d=makeDoor(101);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
packet('feed',.9,.5,-.2,-1);frame(d,.04)
local phase=d.time;local starts=objectStarts;local manip=manipulationStarts
-- Extra native frames without a new controller feed must not zero velocity.
frame(d,.01);frame(d,.01)
packet('feed',0,.5,-.2,-1);frame(d,.001)
check(d.rate==1 and d.nDirection==1 and d.curAnim=='door_l_b_o_player','normal-speed Grip release continues acquired native opening')
check(d.time==phase and objectStarts==starts and manipulationStarts==manip,'same clip/phase: no second animation or manipulation start')
check(KCD2VRHandDoorsH40.hands[0].releaseMovingH44,'actual-angle movement qualified')
finish(d)
check(mask()==0 and doorReports==1 and nativeActions==0 and physicalCalls==0,'native completion/report once; no Henry action or flag')
check(crimeSounds==1 and crimeVolumes==1 and manipulationStarts==1,'original semantic costs exactly once')
""",
'partial_rest_reacquire':r"""
d=makeDoor(102);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
local x,y=.5*math.cos(-1.2),.5*math.sin(-1.2)
packet('feed',.9,x,y,-1);frame(d,.04)
packet('feed',.9,x,y,-1);frame(d,.04)
local phase=d.time
packet('feed',0,x,y,-1);frame(d,.001)
check(mask()==0 and KCD2VRHandDoorsH40.restsH43[1] and d.rate==0 and d.time==phase,'stationary release leaves exact partial native operation and frees hand')
check(-y-.05>.35,'moved grab anchor lies beyond original/bind bbox acquisition radius')
local starts,reports,cost=objectStarts,doorReports,manipulationStarts
claim0=claim0+1;packet('feed',.9,x,y,-1)
check(mask()==1 and not KCD2VRHandDoorsH40.restsH43[1],'partly-open door reacquired at current carried palm anchor')
check(objectStarts==starts and doorReports==reports and manipulationStarts==cost and d.time==phase and d.rate==0,'reacquisition joins original operation without native restart/cost/seek')
packet('feed',.9,.5*math.cos(-1),.5*math.sin(-1),-1);frame(d,.04)
checkLeaf(d,-1,'reacquisition rebases without drift; reverse held motion remains controller-driven')
packet('releaseall',-1,nil,nil,-1);finish(d)
check(mask()==0 and doorReports==reports+1 and manipulationStarts==cost,'existing explicit native handback still completes once')
""",
'slow_moving_rest':r"""
d=makeDoor(103);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
packet('feed',.9,.5,-.0005,-1);frame(d,.04)
local st=KCD2VRHandDoorsH40.hands[0];local speed=math.abs(st.motionRateH44)
packet('feed',0,.5,-.0005,-1);frame(d,.001)
check(speed>0 and speed<.08 and mask()==0 and d.rate==0 and KCD2VRHandDoorsH40.restsH43[1],'only very-slow deliberate motion can remain partway')
packet('releaseall',-1,nil,nil,-1)
for n=1,110 do frame(d,.02)end
check(doorReports==1 and not KCD2VRHandDoorsH40.restsH43[1],'rest drains through original native endpoint on explicit cleanup')
""",
'reverse_limitation':r"""
d=makeDoor(104);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
packet('feed',.9,.5,-.4,-1);frame(d,.04)
packet('feed',.9,.5,-.2,-1);frame(d,.04)
local phase=d.time;local st=KCD2VRHandDoorsH40.hands[0]
check(st.motionRateH44>0,'reverse controller angle measured')
packet('feed',0,.5,-.2,-1);frame(d,.001)
check(d.rate==1 and d.nDirection==1 and d.time==phase and objectStarts==1 and manipulationStarts==1,'reverse release retains original native direction; never opposite duplicate semantic start')
finish(d);check(doorReports==1,'original endpoint reported once after reverse handback')
""",
'all_selected_curves':r"""
for _,alias in ipairs(nativeReferenceAliases)do
 entities={};local d=makeDoor(201);d.selectedAlias=alias
 local closing=alias:match('_c_player$')~=nil;if closing then d.nDirection=1 end
 local sign=nativeReferenceDirection(alias);local a=sign*.4
 local starts,reports,manips=objectStarts,doorReports,manipulationStarts
 claim0=claim0+1;packet('feed',.9,.5,0,-1);frame(d,.04)
 packet('feed',.9,.5*math.cos(a),.5*math.sin(a),-1);frame(d,.04)
 local phase=d.time
 packet('feed',0,.5*math.cos(a),.5*math.sin(a),-1);frame(d,.001)
 check(d.rate==1 and d.curAnim==alias and d.time==phase,'selected curve actual-angle movement resumes original without phase jump: '..alias)
 finish(d)
 check(mask()==0 and doorReports==reports+1 and objectStarts==starts+1 and manipulationStarts==manips+1,'single original operation/report for selected native alias: '..alias)
end
""",
'no_distant_anchor_capture':r"""
d=makeDoor(105);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
local x,y=.5*math.cos(-1.2),.5*math.sin(-1.2)
packet('feed',.9,x,y,-1);frame(d,.04)
packet('feed',.9,x,y,-1);frame(d,.04)
packet('feed',0,x,y,-1);frame(d,.001)
local count=objectStarts
claim0=claim0+1;packet('feed',.9,x,y-.5,-1)
check(mask()==0 and objectStarts==count and KCD2VRHandDoorsH40.restsH43[1],'moved anchor does not nominate outside existing 35cm palm radius')
-- Replacing the entity incarnation also cannot nominate the retained anchor.
entities={};claim0=claim0+1;packet('feed',.9,x,y,-1)
check(mask()==0 and objectStarts==count,'invalid current door identity cannot be reacquired')
""",
'stale_motion_not_momentum':r"""
d=makeDoor(106);claim0=claim0+1
packet('feed',.9,.5,0,-1);frame(d,.04)
packet('feed',.9,.5,-.2,-1);frame(d,.04)
now=now+.081 -- no native frame/timer: the arriving physical release itself checks age
packet('feed',0,.5,-.2,-1);frame(d,.001)
check(mask()==0 and d.rate==0 and KCD2VRHandDoorsH40.restsH43[1],'old movement does not continue door after stationary/stale interval')
""",
}
def main():
 out=LANE/'verification';out.mkdir(exist_ok=True)
 with zipfile.ZipFile(old.PAK)as z:retail=z.read(old.MEMBER)
 assert hashlib.sha256(retail).hexdigest()==old.EXPECTED_SCRIPT_SHA
 with zipfile.ZipFile(old.PAK.parent/'Animations.pak')as z:curves=decoded_curves(z.read(DBA_MEMBER))
 ref={alias:(end,angle,min(values),max(values))for alias,end,keys,values,angle in curves}
 results=[]
 for order in ('r4_first','h40_first'):
  for no_debug in (False,True):
   for name,code in SCENARIOS.items():
    L=old.LuaRuntime(unpack_returned_tuples=True)
    L.globals().nativeReferenceAngle=lambda alias,p:ref[alias][1](ref[alias][0]*p)
    L.globals().nativeReferenceClamp=lambda alias,t:min(ref[alias][3],max(ref[alias][2],t))
    L.globals().nativeReferenceAliases=L.table_from(list(ref))
    L.globals().nativeReferenceDirection=lambda a:1 if ref[a][3]>abs(ref[a][2])else -1
    old.chunk(L,old.LEAVES,'test/native_leaves.lua');old.chunk(L,retail.decode('utf-8-sig'),old.MEMBER)
    old.chunk(L,old.DOUBLES,'test/native_object_leaves.lua');old.chunk(L,old.EXTERNAL,'test/h40_external_leaves.lua')
    if no_debug:old.chunk(L,'debug=nil','test/no_debug.lua')
    for p in ([old.R4,H40]if order=='r4_first'else[H40,old.R4]):old.chunk(L,p.read_text(),'mods/KCD2VRDoors/hand_only_h40.lua'if p==H40 else 'mods/KCD2VRDoors/door_runtime.lua')
    old.chunk(L,code,'test/'+name+'.lua')
    results.append(dict(order=order,no_debug=no_debug,scenario=name,checks=L.globals().checks,status='PASS'))
 receipt=dict(status='PASS',results=results,assertions=sum(r['checks']for r in results),lua='Actual pinned retail Lua 5.1 via lupa.lua51 with controlled native leaves',retail_sha256=old.EXPECTED_SCRIPT_SHA,dba_sha256=DBA_SHA,source_sha256=hashlib.sha256(H40.read_bytes()).hexdigest(),test_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),limitations=['No native engine or headset execution.','Normal release continues original native clip direction, even following a held reverse excursion.','Reacquisition uses moved prior grab anchor and original bbox, not a new native mesh surface/contact model.','Continuation uses native animation speed 1, not physical inertia.'])
 (out/'H44_RELEASE_RECEIPT.json').write_text(json.dumps(receipt,indent=2)+'\n')
 print(json.dumps(receipt,indent=2))
if __name__=='__main__':main()
