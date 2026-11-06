import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from analyze_motion_d68 import analyze
identity='session=7 cycle=10 actor=0xabc poseEpoch=3 trackingEpoch=8 sample=22 sourceNs=100'
core='D68_MOTION '+identity+' stage=CORE_AFTER ns=200 count=0 v='
lineage='D68_MOTION '+identity+' stage=PALETTE_LINEAGE ns=190 endNs=195 flags=0xfff output=0xdef previousOutput=0x123 pools=9/8 data=0x456 currentR=0x0000a/0x0000b meshSubmission=UNOBSERVED'
view='D68_MOTION session=7 stage=PALETTE_VIEW ns=199 epoch=4 frame=33 eye=1 status=5 renderer=0x222 crv=0x333 pipeline=0x444 uploadPool=9 rendererPool=10 bulkOwner=0x555 meshSubmission=UNOBSERVED'
r=analyze([core,lineage,lineage.replace('cycle=10','cycle=11'),lineage.replace('actor=0xabc','actor=0xabd'),view,
 'D68_MOTION session=7 stage=PALETTE_HEALTH copied=4 dropped=0',lineage.replace('flags=0xfff','flags=broken')])
assert r['malformedRows']==1
assert len(r['nativePaletteCoreJoins'])==3
assert r['nativePaletteCoreJoins'][0]['exactCoreLines']==[1]
assert r['nativePaletteCoreJoins'][1]['exactCoreLines']==[]
assert r['nativePaletteCoreJoins'][2]['exactCoreLines']==[]
assert r['nativePaletteCoreJoins'][0]['qualifiedPaletteIdentity']
assert r['nativePaletteCoreJoins'][0]['stablePaletteRing']
assert r['nativePaletteLineage'][0]['currentR']=='0x0000a/0x0000b'
assert r['nativePaletteViews'][0]['uploadPool']==9
assert r['nativePaletteViews'][0]['bulkOwner']==0x555
assert len(r['nativePaletteHealth'])==1
assert 'meshSubmission' not in r['nativePaletteCoreJoins'][0]
print('PASS D69 analyzer: exact CPU cycle/actor joins, separate native pool schema, hash preservation, malformed rows and no guessed mesh/image join')

# Explicit trial rows retain targets and raw floats; same-cycle write never
# becomes thread-safety/display proof from time proximity.
trial="D68_MOTION session=1 stage=HAND_PREDICTION ns=110 sourceNs=80 trackingEpoch=2 targetXrNs=1000 leftTargetD69=11001000 rightTargetD69=11001000 handPolicyD69=44000003 leftLeadD69=1 rightLeadD69=2 count=4 v=1,2,3,4"
placement="D68_MOTION session=1 stage=ATTACHMENT ns=120 sourceNs=80 cycle=0 flags=10015 sample=2 count=0 v="
a=analyze([trial,placement])
assert a['handPredictionTrials'][0]['leftTargetD69']==11001000
assert a['handPredictionTrials'][0]['rightLeadD69']==2
assert a['handPredictionTrials'][0]['v']==[1,2,3,4]
p=a['sameCycleWeaponPlacements'][0]
assert p['placementReturned'] and p['readbackAttempted'] and p['readbackMatched']
assert p['exactCoreLines']==[] and p['threadSafety']=='UNPROVEN' and p['meshSubmission']=='UNOBSERVED'
print('PASS D69 trial analyzer: explicit extra target/raw data, same-cycle CPU receipt, no invented thread/display proof')
