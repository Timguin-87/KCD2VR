from pathlib import Path
import sys,json,hashlib,argparse,zipfile
sys.path.insert(0,r'C:/Users/turcs/.codex/worktrees/8fd6/KCD2VR/held-map-c7fm1/map-test-deps')
from lupa.lua51 import LuaRuntime
from test_hand_only_doors_h40 import LEAVES,DOUBLES,EXTERNAL,PAK,MEMBER,EXPECTED_SCRIPT_SHA,H40,R4,chunk
P=argparse.ArgumentParser();P.add_argument('--output',type=Path,required=True);a=P.parse_args()
common=r'''
function finishTest(d)
 for n=1,110 do frame(d,.02);if not KCD2VRHandDoorsH40.testsH41[d]then return end end
 error('original console episode did not finish')
end
local rawSpeed
function count(t)local n=0;for _ in pairs(t or{})do n=n+1 end;return n end
'''
cases={
'normal':r'''
d=makeDoor(101);local before=objectStarts;check(KCD2VRHandDoorsH40.Test(.5),'queued console request');check(objectStarts==before and d.inUse==0,'no direct clip in console callback');check(KCD2VRHandDoorsH40.testsH41[d]~=nil,'wake-only original owner published');nativeActorFlags=1
frame(d,.01);check(objectStarts==before+1 and nativeActions==0 and math.abs(d.time-.5)<.0001 and d.rate==1,'one exact object-only native start; positive rate no pause');check(d.inUse==1 and d.bNeedUpdate==1,'native episode genuinely running');check(not KCD2VRHandDoorsH40.ReleaseAll(),'entered release awaits original completion');finishTest(d);check(d.inUse==0 and d.bNeedUpdate==0 and d.curAnim=='' and doorReports==1,'actual native report closes episode');check(nativeActorFlags==1 and physicalCalls==0,'unrelated actor flags retained');check(next(KCD2VRHandDoorsH40.testsH41)==nil,'exact own console lease retired');check(KCD2VRHandDoorsH40.TestSwing(2),'fresh native swing queued after completion');frame(d,.01);finishTest(d);check(doorReports==2 and d.nDirection==-1,'second native close completes with next native interaction available')
''',
'locks':r'''
d=makeDoor(101);d.bLocked=true;d.Properties.Lock.esLockTypes='key/key';check(not KCD2VRHandDoorsH40.Test(.5),'locked key-side refuses without key');check(objectStarts==0 and unlockReports==0 and next(KCD2VRHandDoorsH40.testsH41)==nil,'lock refusal no mutation');hasKey=true;check(KCD2VRHandDoorsH40.Test(.5),'native key admission queues');frame(d,.01);check(unlockReports==1 and not d.bLocked,'only original native Unlock acts');finishTest(d);check(doorReports==1,'native keyed completion')
''',
'foreign':r'''
d=makeDoor(101);d.inUse=1;d.bNeedUpdate=1;d.curAnim='foreign';d.rate=0;check(not KCD2VRHandDoorsH40.Test(.5),'untracked paused original refuses adoption');check(KCD2VRHandDoorsH40.ReleaseAll(),'no new owner to release');check(d.inUse==1 and d.curAnim=='foreign' and d.rate==0 and objectStarts==0,'foreign state never cleared or resumed by guess')
''',
'physical':r'''
d=makeDoor(101);claim0=claim0+1;packet('feed',.9,.5,0,-1);check(not KCD2VRHandDoorsH40.Test(.5),'physical pending owner prevents test');frame(d,.01);packet('clear',-1,nil,nil,-1);finish(d);check(mask()==0 and next(KCD2VRHandDoorsH40.testsH41)==nil,'physical cycle preserved')
''',
'cancel_before_start':r'''
d=makeDoor(101);check(KCD2VRHandDoorsH40.Test(.5),'test pending');check(KCD2VRHandDoorsH40.ReleaseAll(),'wake-only cancel synchronous');frame(d,.01);check(objectStarts==0 and d.inUse==0,'cancelled wake creates no clip')
''',
'seek_failure':r'''
d=makeDoor(101);local seek=d.SetAnimationTime;local seeks=0;function d:SetAnimationTime(...)seeks=seeks+1;if seeks==2 then error('preview seek failed')end;return seek(self,...)end;check(KCD2VRHandDoorsH40.Test(.5),'seek-fault request queued');frame(d,.01);check(d.rate==1 and objectStarts==1,'failed preview seek cannot strand pause');finishTest(d);check(doorReports==1 and d.inUse==0,'original positive-rate clip completes after preview seek failure')
''',
'start_unknown':r'''
d=makeDoor(101);check(KCD2VRHandDoorsH40.Test(.5),'start-fault request queued');failStart=true;frame(d,.01);failStart=false;local st=KCD2VRHandDoorsH40.testsH41[d];check(st and st.entered and st.uncertain,'partial native start uncertainty retained');KCD2VRHandDoorsH40.ReleaseAll();frame(d,.01);check(KCD2VRHandDoorsH40.testsH41[d]==st,'ReleaseAll cannot erase entered unknown original');check(objectStarts==0,'unknown native start never retried')
''',
'opaque':r'''
player.id={opaque=true};d=makeDoor(101);check(KCD2VRHandDoorsH40.Test(.5),'opaque original actor joins');frame(d,.01);currentPlayerOverride={id=player.id};finishTest(d);check(doorReports==1 and nativeActions==0,'actor replacement after start does not invent actor cleanup')
''',
'pending_race':r'''
d=makeDoor(101);check(KCD2VRHandDoorsH40.Test(.5),'test queued');check(not KCD2VRHandDoorsH40.Test(.6),'duplicate test cannot overwrite pending');claim0=claim0+1;packet('feed',.9,.5,0,-1);check(mask()==0,'physical feed never adopts pending test');frame(d,.01);finishTest(d);check(doorReports==1,'one native original despite competing requests')
'''}
retail=zipfile.ZipFile(PAK).read(MEMBER);assert hashlib.sha256(retail).hexdigest()==EXPECTED_SCRIPT_SHA
results=[]
for order in ['r4_first','h40_first']:
 for name,code in cases.items():
  L=LuaRuntime(unpack_returned_tuples=True);chunk(L,LEAVES,'test/native_leaves.lua');chunk(L,retail.decode('utf-8-sig'),MEMBER);chunk(L,DOUBLES,'test/native_object_leaves.lua');chunk(L,EXTERNAL,'test/h40_external_leaves.lua')
  for p in ([R4,H40]if order=='r4_first'else[H40,R4]):chunk(L,p.read_text(),str(p))
  chunk(L,common,'test/console_helpers.lua');chunk(L,code,'test/console_'+name+'.lua');results.append({'order':order,'scenario':name,'checks':L.globals().checks,'status':'PASS'})
a.output.mkdir(parents=True,exist_ok=True);r={'results':results,'checks':sum(x['checks']for x in results),'sourceSHA256':hashlib.sha256(H40.read_bytes()).hexdigest(),'retailSHA256':EXPECTED_SCRIPT_SHA,'script':str(H40),'native_engine':False,'headset':'NOT_RUN','limits':'Actual retail AnimDoor+H40+R4 Lua; native scheduler, getters, animation and component leaves modeled. No foreign orphan adoption or fabricated terminal.'};(a.output/'RECEIPT.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
