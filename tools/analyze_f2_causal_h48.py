"""Summarize F2 causal records. Usage: python tools/analyze_f2_causal_h48.py KCD2VR-diagnostics.log --out f2_review.json"""
import argparse,collections,json,math,re
from pathlib import Path

def analyze(lines):
 craft=[];pacing=[];counts=collections.Counter();restores=collections.Counter();body=collections.Counter();last={};spikes=[];cursor=[];camera=[];drops=[]
 for line in lines:
  if 'C71 F5_CRAFT_' in line or 'C70 F5_HELD_POSE' in line or 'V66.90-PLAYBACK CLIP' in line:
   pairs=dict(re.findall(r'(\w+)=([^ ]+)',line));pairs['_family']=line.split('F5_CRAFT_',1)[1].split()[0] if 'F5_CRAFT_' in line else 'HELD_POSE' if 'F5_HELD_POSE' in line else 'CLIP';craft.append(pairs)
  if 'F2_CAUSAL ' not in line:continue
  if 'QUEUE_DROPPED' in line:drops.append(line.strip());continue
  pairs=dict(re.findall(r'(\w+)=([^ ]+)',line));
  if 'stage' not in pairs:continue
  stage=int(pairs['stage']);counts[str(stage)]+=1
  if 9<=stage<=14:pacing.append(pairs)
  try:data=[float(x) for x in pairs.get('data','').split(',')]
  except ValueError:continue
  reason=pairs.get('reason','');flags=int(pairs.get('flags','0'));code=int(pairs.get('code','0'))
  if stage==1:body[reason]+=1
  if stage==3 and flags&1:restores[str(code)]+=1
  if stage==8 and code in (3,4):cursor.append({k:pairs.get(k) for k in ['order','tick','side','flags','code','reason','data']})
  if stage==5:camera.append({k:pairs.get(k) for k in ['order','tick','sample','object','publication','epoch','flags','reason']})
  if stage!=7 or len(data)!=32:continue
  identity=tuple(pairs.get(k,'0') for k in ['session','actor','side','item','generation','epoch','sourceEpoch'])
  prev=last.get(identity);last[identity]=(pairs,data)
  if not prev:continue
  old,vec=prev
  # All three motor positions are in the SAME model domain. XR-vs-model subtraction is invalid.
  dist=lambda i:math.sqrt(sum((data[i+j]-vec[i+j])**2 for j in range(3)))*1000
  request,allowed,weapon=dist(0),dist(7),dist(14)
  if request<2 and max(allowed,weapon)>20:
   spikes.append({'order':pairs.get('order'),'sample':pairs.get('sample'),'publication':pairs.get('publication'),'side':pairs.get('side'),'requested_step_mm':request,'allowed_step_mm':allowed,'weapon_step_mm':weapon,'flags':flags,'note':'Candidate added motor motion; inspect acquisition, contacts, root and mode transition. Not headset proof.'})
 return {'stage_counts':dict(counts),'native_attachment_restores_by_reason':dict(restores),'body_reasons':dict(body),'motor_added_motion_candidates':spikes,'cursor_click_and_admission':cursor,'camera_source_identity':camera,'queue_drop_receipts':drops,'pacing':analyze_pacing(pacing),'crafting':analyze_craft(craft),'limits':'Actual runtime paths only where recorded. Zero fields mean unknown. Drops/gaps make absence inconclusive. No damage/hit/combo success inferred.'}

def analyze_pacing(records):
 # Queue order can differ from source execution across threads: join by exact IDs and CPU clock.
 records=sorted(records,key=lambda p:int(p.get('cpuNs','0')))
 published={};requests={};rows=[];end=[]
 for p in records:
  stage=int(p['stage']);session=p.get('session');source=p.get('item');request=p.get('generation');ns=int(p.get('cpuNs','0'))
  if stage==14:published[session,source]=p
  elif stage==13:requests[session,request]=p
  elif stage==10:
   origin=published.get((session,source));enqueue=requests.get((session,request))
   row={'session':session,'source_serial':source,'source_camera':p.get('publication'),'request_ns':request,'fresh':bool(int(p.get('flags','0'))&2),'paired':bool(int(p.get('flags','0'))&1)}
   if origin:row['pair_queue_to_pickup_ms']=(ns-int(origin['cpuNs']))/1e6
   if enqueue:
    row['enqueue_to_pickup_ms']=(ns-int(enqueue['cpuNs']))/1e6
    row['xr_frame']=enqueue.get('sample');row['predicted_display_xr_time']=enqueue.get('poseTime')
   rows.append(row)
  elif stage==11:
   enqueue=requests.get((session,request))
   if enqueue:enqueue['copy_return_cpu_ns']=ns
  elif stage==12:end.append({k:p.get(k) for k in ('session','sample','poseTime','cpuNs','code','flags')})
 for row in rows:
  enqueue=requests.get((row['session'],row['request_ns']))
  if enqueue and 'copy_return_cpu_ns' in enqueue:row['enqueue_to_copy_return_ms']=(enqueue['copy_return_cpu_ns']-int(enqueue['cpuNs']))/1e6
 return {'pickups':rows,'end_frame_returns':end,'limits':'CPU steady-clock durations only. poseTime uses the separate OpenXR clock: never subtract it from cpuNs. Pair publication means queue submission, not GPU completion. EndFrame success is not scanout. Missing source records/drops mean unknown age. Desktop source publication is not instrumented by stage14.'}

def analyze_craft(records):
 counts=collections.Counter(p['_family'] for p in records);attempts={};bodies=[];roots=[];animations=[]
 def ident(p):return (p.get('session',''),p.get('instance',''),p.get('life',''))
 for p in records:
  family=p['_family']
  if family=='BODY':bodies.append(p)
  if family=='NATIVE_ROOT':roots.append(p)
  if family=='ANIMATION':animations.append(p)
  if family!='INPUT':continue
  bits=int(p.get('bits','0'));kind=p.get('kind');grips=p.get('grip','0,0').split(',')
  for h,side in enumerate(('R','L')):
   attempt=p.get('attempt'+side,'0')
   if attempt=='0' or len(grips)!=2 or float(grips[h])<.35:continue
   key=(*ident(p),side,attempt);row=attempts.setdefault(key,{'session':key[0],'instance':key[1],'life':key[2],'side':side,'attempt':attempt,'kind':kind,'observed_gaps':[],'first_tick':p.get('tick')})
   if kind=='ALCHEMY':
    palm=p.get('palmValid','0,0').split(',');quarantine=p.get('quarantined','0,0').split(',')
    reason=('ADAPTER_DISABLED' if not bits&1 else 'CONTEXT_UNQUALIFIED' if not bits&2 else 'TRACKING_FRAME_STALE_OR_INVALID' if not bits&8 else 'WORLD_PALM_INVALID' if len(palm)==2 and palm[h]=='0' else 'NATIVE_TABLE_STALE_OR_MISSING' if not bits&16 else 'WAITING_GRIP_RELEASE_QUARANTINE' if len(quarantine)==2 and quarantine[h]=='1' else 'NO_HAND_NOMINATION' if not bits&64 else 'NATIVE_SELECTION_UNOBSERVED' if not bits&32 else 'NATIVE_SELECTION_DIFFERS' if p.get('nativeSelected')!=p.get('nominated') else 'ADMISSION_NOT_READY' if p.get('admissionReady')=='0' else 'NOT_ADMITTED' if not bits&128 else 'NATIVE_USE_INPUT_REQUESTED' if bits&256 else 'ADMITTED_WITHOUT_USE_THIS_SAMPLE')
    row['last_nominated_entity']=p.get('nominated');row['last_selected_entity']=p.get('nativeSelected');row['last_action']=p.get('action');row['last_work_parent']=p.get('workParent')
   else:
    reason=('ADAPTER_DISABLED' if not bits&1 else 'CONTEXT_UNQUALIFIED' if not bits&2 else 'SOURCE_TABLE_STALE_OR_MISSING' if not bits&8 else 'WORLD_PALM_INVALID' if not bits&(16<<h) else 'CONTEXTUAL_A_REQUESTED' if bits&64 else 'NO_CONTEXTUAL_A_REQUEST_THIS_SAMPLE')
    row['last_requested_entity']=p.get('requestEntity')
   if reason not in row['observed_gaps']:row['observed_gaps'].append(reason)
   row['last_tick']=p.get('tick')
 return {'family_counts':dict(counts),'outer_context':[p for p in records if p['_family']=='CONTEXT'],'attempts':list(attempts.values()),'native_root_snapshots':roots,'committed_body_snapshots':bodies,'animation_joins':animations,'source_snapshots':[p for p in records if p['_family']=='SOURCE'],'tool_snapshots':[p for p in records if p['_family']=='TOOL'],'native_action_snapshots':[p for p in records if p['_family']=='NATIVE'],'outgoing_pad':[p for p in records if p['_family']=='PAD_PUBLISHED'],'held_item_poses':[p for p in records if p['_family']=='HELD_POSE'],
         'limits':'Observed pipeline conditions, not invented engine refusal reasons. Attempts are raw squeeze edges, not accepted actions. Native action/attachment success needs matching entity, station lifetime and timestamps in original C71/C70 records. No completion or recipe result is inferred from published A/use, FIFO presence, or missing callbacks. CLIP alias lines join animation_joins by playbackSeq=seq. Source distances are WORLD selection bounds, not mesh/finger collision.'}

def main():
 p=argparse.ArgumentParser();p.add_argument('log');p.add_argument('--out',default='f2_review.json');a=p.parse_args()
 with Path(a.log).open(errors='replace') as f:result=analyze(f)
 Path(a.out).write_text(json.dumps(result,indent=2));print(json.dumps({'output':a.out,'stages':result['stage_counts'],'restores':result['native_attachment_restores_by_reason'],'motor_candidates':len(result['motor_added_motion_candidates']),'drop_receipts':len(result['queue_drop_receipts'])}))
if __name__=='__main__':main()
