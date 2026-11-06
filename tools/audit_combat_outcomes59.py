"""Read a supplied headset capture; never treat event samples as all swings."""
from pathlib import Path
import argparse, collections, hashlib, json, re

def audit(path):
    path=Path(path); raw=path.read_bytes(); text=raw.decode('utf-8',errors='replace')
    counts=collections.Counter(); fields={name:collections.Counter() for name in ('blocked','perfect','reactionDispatch','death','applied','combo')}
    impales=[]; outcomes=[]
    for line in text.splitlines():
        if ' CONTACT_RECEIPT ' in line:
            values=dict(re.findall(r'\b(\w+)=([^ ]+)',line))
            counts['sampled_contact_receipts']+=1
            for name, bank in fields.items():
                bank[values.get(name,'UNOBSERVED')]+=1
            outcomes.append({key:values.get(key) for key in ('serial','gesture','release','target','kind','sample','impactAgeMs','bodyContact','nativeExecutedZone','rawCollisionPart','nativeHitPart','material','submitted','applied','fields','damage','stamina','blocked','perfect','death','reactionDispatch','combo')})
        if 'C47 IMPALE stage=' in line:impales.append(line)
        if ' MELEE_INPUT ' in line and 'NATIVE_STATE_READ' in line:counts['printed_native_state_read_refusals']+=1
        if ' MELEE_INPUT ' in line and 'OUT_OF_REACH' in line:counts['printed_out_of_reach_refusals']+=1
    return {'input':str(path),'bytes':len(raw),'sha256':hashlib.sha256(raw).hexdigest(),
        'scope':'Capture-gated sampled result events. Not all physical swings; not an acceptance-rate denominator.',
        'unknown_reaction':'-1 or missing means unobserved, never proof no reaction occurred.',
        'counts':dict(counts),'sampled_result_fields':{k:dict(v) for k,v in fields.items()},
        'outcomes':outcomes,'impale_edges':impales}

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('log');parser.add_argument('--out',required=True);a=parser.parse_args()
    output=Path(a.out);output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(audit(a.log),indent=2)+'\n',encoding='utf-8')
