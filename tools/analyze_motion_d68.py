"""Join D68 Shift+F2 copied motion evidence. No inferred mesh/scanout association."""
import argparse, collections, json, math, re
from pathlib import Path

def analyze(lines):
    counts=collections.Counter(); rows=[]; health=[]; malformed=0
    lineage=[]; views=[]; palette_health=[]
    for number,line in enumerate(lines,1):
        if 'D68_MOTION session=' not in line: continue
        text=line.split('D68_MOTION ',1)[1]
        header,_,values=text.partition(' v=')
        # Schema is descriptive text, never an identity or timing source.
        fields=dict(re.findall(r'(\w+)=([^\s]+)',header.split(' schema=',1)[0]))
        fields['v']=values.split()[0] if values.strip() else ''
        if fields.get('stage')=='HEALTH':health.append(fields);continue
        if fields.get('stage') in ('PALETTE_HEALTH','PALETTE_VIEW_HEALTH'):
            palette_health.append(fields);continue
        if fields.get('stage') in ('PALETTE_LINEAGE','PALETTE_VIEW'):
            # Separate wire schema: preserve native ring/pool/hash fields rather
            # than pretending these are the float-array controller records.
            try:
                r=dict(fields);r.pop('v',None);r['line']=number
                numeric=('session','cycle','serial','tick','ns','endNs','finishNs','readNs','thread','sample','publication','poseEpoch','trackingEpoch','sourceNs','targetXrNs','epoch','frame','eye','status','uploadPool','rendererPool')
                pointer=('actor','pose','skeleton','descriptor','output','previousOutput','renderer','crv','pipeline','bulkOwner')
                for k in numeric:
                    if k in r:r[k]=int(r[k])
                for k in pointer:
                    if k in r:r[k]=int(r[k],16)
                if 'flags' in r:r['flags']=int(r['flags'],16)
                if not r.get('session') or not r.get('ns'):raise ValueError('missing identity/clock')
                if r['stage']=='PALETTE_LINEAGE':
                    if not r.get('cycle') or not r.get('actor'):raise ValueError('missing body cycle')
                    lineage.append(r)
                else:views.append(r)
                counts[r['stage']]+=1
            except (ValueError,KeyError):malformed+=1
            continue
        try:
            row={key:int(fields.get(key,'0'),16 if key in ('flags','actor','object','descriptor','pose','output') else 10) for key in
                ('session','order','ns','endNs','sourceNs','sourceTick','sample','trackingEpoch','poseEpoch','publication','frame','cycle','inputTick','priorSample','priorSourceNs','request','thread','side','flags','count','actor','object','descriptor','pose','output','targetXrNs','leftTargetD69','rightTargetD69','handPolicyD69','leftLeadD69','rightLeadD69')}
            row['stage']=fields['stage'];row['v']=[float(x) for x in fields.get('v','').split(',') if x];row['line']=number
            if len(row['v'])!=row['count']:raise ValueError('truncated values')
            rows.append(row);counts[row['stage']]+=1
        except (ValueError,KeyError):malformed+=1
    families=collections.defaultdict(list)
    for r in rows:
        if r['stage']=='INPUT_PUBLISHED' and r['flags']&1 and not r['flags']&0x10000 and r['endNs']:
            families[r['session'],r['trackingEpoch']].append(r)
    for group in families.values():group.sort(key=lambda r:r['endNs'])
    older=[];reads=0
    for r in rows:
        # Only successful ordinary-current-family substitution. HJ1/fallback/invalid excluded.
        if r['stage']!='TRACKING_READ' or (r['flags']&255)!=3:continue
        reads+=1
        prior=[p for p in families[r['session'],r['trackingEpoch']] if p['endNs']<=r['ns']]
        if not prior:continue
        latest=max(prior,key=lambda p:p['sourceNs'])
        if latest['sourceNs']>r['sourceNs']:
            older.append({'readLine':r['line'],'readSample':r['sample'],'availableLine':latest['line'],
                'availableSample':latest['sample'],'publishedBeforeReadUs':(r['ns']-latest['endNs'])/1000})
    b1=[];b1unread=0;b1reference=0;palette=[];ages=[];repeated=[];priorByHand={}
    cohorts=collections.Counter()
    # CORE flags only. Never infer free air from zero query results or an absent row.
    ownerMask=0x200|0x400|0x800|0x1000|0x2000|0x4000|0x8000|0x10000|0x20000|0x4000000
    contactMask=0x200000|0x400000|0x800000|0x1000000|0x10000000|0x40000000|0x80000000
    unknownMask=0x40000|0x2000000|0x8000000|0x20000000
    def cohort(flags):
        if not flags&0x100:return 'owner_state_unobserved'
        if flags&ownerMask:return 'held_or_other_owner'
        if flags&unknownMask:return 'unknown_or_handover'
        if flags&contactMask:return 'contact_or_release'
        if flags&0x180000==0x180000:return 'free_fresh_clear'
        return 'contact_state_not_fresh_or_ineligible'

    for r in sorted(rows,key=lambda x:x['ns']):
        v=r['v']
        if r['stage']=='CORE_BEFORE' and len(v)>=21:
            cohorts[cohort(r['flags'])]+=1
            if not r['flags']&1 or not all(math.isfinite(x) for x in v[:21]):b1unread+=1;continue
            if r['flags']&0x5f!=0x5f:b1reference+=1;continue
            d=math.sqrt(sum((v[4+i]-v[11+i])**2 for i in range(3)))
            q=min(math.sqrt(sum((v[i]-sign*v[7+i])**2 for i in range(4))) for sign in (1,-1))
            b1.append({'line':r['line'],'cycle':r['cycle'],'side':r['side'],'positionErrorMm':1000*d,'quaternionComponentError':q})
        if r['stage']=='CORE_AFTER':palette.append(r)
        if r['stage']=='MAPPING':
            if r['sourceNs'] and r['ns']>=r['sourceNs']:ages.append((r['ns']-r['sourceNs'])/1e6)
            key=(r['session'],r['trackingEpoch'],r['actor'],r['poseEpoch'],r['side']);p=priorByHand.get(key)
            if p and p['sample']==r['sample'] and p['sourceNs']==r['sourceNs'] and p['cycle']!=r['cycle']:
                repeated.append({'line':r['line'],'priorLine':p['line'],'sample':r['sample'],'side':r['side'],'gapMs':(r['ns']-p['ns'])/1e6})
            priorByHand[key]=r
    # Exact CPU transaction identity only; never join a mesh draw from proximity
    # in time or matching pool counters. A view receipt has no Henry draw owner.
    core=collections.defaultdict(list)
    for r in rows:
        if r['stage'] in ('CORE_BEFORE','CORE_AFTER','POSE_PUBLISHED'):
            key=tuple(r[k] for k in ('session','cycle','actor','poseEpoch','trackingEpoch','sample','sourceNs'))
            core[key].append(r)
    lineage_joins=[]
    for r in lineage:
        key=tuple(r.get(k,0) for k in ('session','cycle','actor','poseEpoch','trackingEpoch','sample','sourceNs'))
        matches=core.get(key,[])
        lineage_joins.append({'line':r['line'],'cycle':r['cycle'],'exactCoreLines':[x['line'] for x in matches],
            'exactCoreStages':sorted(set(x['stage'] for x in matches)),
            'qualifiedPaletteIdentity':(r.get('flags',0)&0x137)==0x137,
            'stablePaletteRing':(r.get('flags',0)&0x1ff)==0x1ff})
    prediction_trials=[r for r in rows if r['stage']=='HAND_PREDICTION']
    same_cycle=[]
    for r in rows:
        if r['stage']!='ATTACHMENT' or not r['flags']&0x10000:continue
        key=tuple(r[k] for k in ('session','cycle','actor','poseEpoch','trackingEpoch','sample','sourceNs'))
        matches=core.get(key,[]) if r['cycle'] else []
        same_cycle.append({'record':r,'exactCoreLines':[x['line'] for x in matches],
            'placementReturned':bool(r['flags']&1),'readbackAttempted':bool(r['flags']&4),
            'readbackMatched':bool(r['flags']&16),'threadSafety':'UNPROVEN','meshSubmission':'UNOBSERVED'})
    return {'recordCounts':dict(counts),'malformedRows':malformed,'healthReceipts':health,
        'handPredictionTrials':prediction_trials,'sameCycleWeaponPlacements':same_cycle,
        'nativePaletteLineage':lineage,'nativePaletteViews':views,'nativePaletteHealth':palette_health,'nativePaletteCoreJoins':lineage_joins,
        'ordinaryReadsObserved':reads,'newerPublishedBeforeRead':older,
        'actualB1Cohorts':dict(cohorts),'actualB1ReadFailures':b1unread,'nativeReferenceOrUncommittedB1':b1reference,'actualB1VsStaged':{'observed':len(b1),'maxPositionErrorMm':max((r['positionErrorMm'] for r in b1),default=None),'maxQuaternionComponentError':max((r['quaternionComponentError'] for r in b1),default=None),'mismatches':[r for r in b1 if r['positionErrorMm']>.1 or r['quaternionComponentError']>1e-4]},
        'mappingSourceAgeMs':{'count':len(ages),'mean':sum(ages)/len(ages) if ages else None,'max':max(ages,default=None)},
        'repeatedSourceAcrossCoreCycles':repeated,'nativePaletteRawRecords':len(palette),
        'limits':['Partial synchronous bursts: absence of a matching row is not proof of a missing stage; inspect HEALTH omissions.',
                  'Older-source finding requires same epoch and an actual publication completed before read began; review branches and validity.',
                  'CPU callback reuse is not proof of repeated displayed frames.',
                  'HAND_PREDICTION is an explicit extra target offset beyond OpenXR; it is not measured display latency.',
                  'Same-cycle native placement/readback is a CPU receipt, not engine thread-safety or mesh/frame proof.',
                  'Palette entries are raw native dual quaternions. No guessed conversion or native mesh association.',
                  'Native upload-view/pool receipts do not identify Henry mesh draws. Only exact CPU cycle identities are joined.',
                  'XR submission is not scanout. Native mesh/palette/world-to-submitted-image association remains UNOBSERVED.']}

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('log',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 with a.log.open(encoding='utf-8-sig',errors='replace') as f:result=analyze(f)
 a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(json.dumps({k:v for k,v in result.items() if k in ('recordCounts','malformedRows','actualB1VsStaged','mappingSourceAgeMs')},indent=2))
if __name__=='__main__':main()
