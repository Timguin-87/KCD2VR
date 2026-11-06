import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from analyze_motion_d68 import analyze

def row(stage,ns,**kw):
 d=dict(session=1,stage=stage,ns=ns,endNs=ns,sample=1,sourceNs=10,trackingEpoch=1,flags='0',count=0,v='');d.update(kw)
 v=d.pop('v');return 'D68_MOTION '+' '.join(str(k)+'='+str(v) for k,v in d.items())+' schema=descriptive ns=not_numeric;sample=IMAGE_SERIAL v='+v
lines=[row('INPUT_PUBLISHED',20,endNs=30,sample=2,sourceNs=19,flags='1'),
       row('TRACKING_READ',25,endNs=40,flags='3'), # publish overlaps read: NOT stale proof
       row('TRACKING_READ',31,flags='3'), # actual newer available
       row('TRACKING_READ',35,flags='5'), # HJ1 not ordinary family
       row('TRACKING_READ',36,flags='3',trackingEpoch=2), # different epoch
       row('INPUT_PUBLISHED',20,endNs=21,sample=9,sourceNs=50,flags='10001'), # permission packet never latest-family proof
       row('CORE_BEFORE',40,flags='18015F',count=22,v='0,0,0,1,.2,.3,.4,0,0,0,1,.2,.3,.4,0,0,0,1,0,0,0,15'),
       row('CORE_BEFORE',41,flags='5F',count=22,v='0,0,0,1,.21,.3,.4,0,0,0,1,.2,.3,.4,0,0,0,1,0,0,0,15'),
       row('CORE_BEFORE',42,flags='0',count=22,v=','.join(['0']*22)),
       row('MAPPING',50,cycle=1,side=0),row('MAPPING',61,cycle=2,side=0),
       row('CORE_BEFORE',43,flags='1',count=22,v=','.join(['0']*22)),
       row('CORE_AFTER',62),row('CORE_AFTER',63,count=2,v='0')]
r=analyze(lines)
assert len(r['newerPublishedBeforeRead'])==1 and r['newerPublishedBeforeRead'][0]['readLine']==3
assert r['actualB1VsStaged']['observed']==2 and len(r['actualB1VsStaged']['mismatches'])==1
assert abs(r['actualB1VsStaged']['maxPositionErrorMm']-10)<1e-9
assert r['actualB1Cohorts']['free_fresh_clear']==1
assert r['nativeReferenceOrUncommittedB1']==1
assert r['actualB1ReadFailures']==1 and len(r['repeatedSourceAcrossCoreCycles'])==1
assert r['malformedRows']==1
print('PASS D68 analyzer: publication-vs-source clocks, overlapping read, epoch/HJ1 exclusion, packet separation, actual B1 mismatch and malformed row detection')
