"""Compare the narrowed gate policy against the supplied immutable D71 causal index.

This replays recorded order/identity only. It does not simulate how a modified
scheduler would affect later frames, GPU completion or headset latency.
"""
import argparse,bisect,collections,hashlib,json
from pathlib import Path
p=argparse.ArgumentParser();p.add_argument('--index',type=Path,required=True);p.add_argument('--report',type=Path,required=True);a=p.parse_args()
raw=a.index.read_bytes();events=json.loads(raw)['causal'];enqueues=collections.defaultdict(list);pickups={}
for r in events:
 k=r['kv']
 if r['stage']==13:enqueues[k['session']].append((int(k['generation']),r))
 elif r['stage']==10:pickups[k['generation']]=int(k['cpuNs'])
for rows in enqueues.values():rows.sort(key=lambda x:x[0])
clocks={s:[r[0] for r in rows] for s,rows in enqueues.items()};gates=[]
for r in events:
 if r['stage']!=15:continue
 k=r['kv'];start=int(k['sourceNs']);end=int(k['generation']);old=int(k['item'],16);session=k['session']
 rows=enqueues[session];i=bisect.bisect_right(clocks[session],start)-1
 if i<0:continue
 ns,e=rows[i];ek=e['kv'];frame=int(ek['sample']);pickup=pickups.get(ek['generation'])
 if start-ns>500000000:continue
 gates.append(dict(native_frame=int(k['publication']),available_frame=frame,line=r['line'],success=k['code']=='1',newer=frame>old,pending=pickup is not None and pickup>start,request_age_ns=start-ns,wait_ns=end-start,pickup_remaining_ns=pickup-start if pickup else None))
qualified=[r for r in gates if r['newer'] and r['pending'] and max(0,6000000-r['request_age_ns'])<min(r['pickup_remaining_ns'],r['wait_ns'])]
report={'kind':'RECORDED_ORDER_POLICY_COMPARISON_NOT_RUNTIME_PERFORMANCE','index_sha256':hashlib.sha256(raw).hexdigest(),'original_capture_sha256':'643f47377ae54387434304d41f9c43e0d61e05b4b97f1d3b462fdcce6e055389','indexed_gates':len(gates),'successful_already_new':sum(r['success'] and r['newer'] for r in gates),'successful_already_new_pending':sum(r['success'] and r['newer'] and r['pending'] for r in gates),'request_age_threshold_ns':6000000,'affected_successful_gates':sum(r['success'] for r in qualified),'covered_timeout_gates':sum(not r['success'] for r in qualified),'qualified_rows':qualified,'limits':['Exact same-session request joins and original steady clocks; missing publications remain unobserved.','Normal fresh-after-arrival remains; an unconsumed token alone grants no exception.','Native current family, epoch and age validation must also pass at runtime.','Counterfactual ordering comparison only: no claimed FPS or motion-to-photon improvement.']}
a.report.parent.mkdir(parents=True,exist_ok=True);a.report.write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:v for k,v in report.items() if k not in ('qualified_rows','limits')},indent=2))
assert report['indexed_gates']==4984 and report['successful_already_new']==539
assert report['affected_successful_gates']==0 and report['covered_timeout_gates']==4
