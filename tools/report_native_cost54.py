"""Extract numeric native-cost stages; no time/latest-key ancestry is inferred."""
import argparse,collections,hashlib,json,re
from pathlib import Path
ap=argparse.ArgumentParser();ap.add_argument('--log',required=True,type=Path);ap.add_argument('--out',required=True,type=Path);a=ap.parse_args()
raw=a.log.read_bytes();text=raw.decode('utf-8',errors='replace');stages=collections.Counter();effects=collections.Counter();spans={}
for line in text.splitlines():
    if 'R59C5 NATIVE_SERVICE seq=' not in line:continue
    d=dict(re.findall(r'(\w+)=([^ ]+)',line));stage=d['stage'];stages[stage]+=1
    if stage=='4':effects[f"flag={d['flags']},type={d['type']}"]+=1
    if stage in ('0','1','2','3','4'):spans.setdefault(d['span'],[]).append(stage)
out={'sha256':hashlib.sha256(raw).hexdigest(),'source':a.log.name,'nativeServiceRows':sum(stages.values()),
     'numericStages':dict(stages),'effectsByActualNativeFlagAndType':dict(effects),
     'wrapperSequences':dict(collections.Counter('/'.join(s) for s in spans.values())),
     'exactOwnPairRows':text.count('route=EXACT_OWN_PAIR'),
     'interpretation':'Native wrapper spans prove their own stack only; old logs do not carry the full VR physical gesture key. No nearest-time input join is made.'}
a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(out,indent=2)+'\n')
print(json.dumps(out,indent=2))
