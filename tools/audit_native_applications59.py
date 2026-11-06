"""Read-only C59 native callback / physical receipt distinction. No game mutation.
Usage: python tools/audit_native_applications59.py headset-folder output.json
Log line counts are not unique swings; native application is not a physical join.
"""
from pathlib import Path
import bisect,collections,hashlib,json,re,sys
folder=Path(sys.argv[1]);main=folder/'KCD2VR.log';diag=folder/'KCD2VR-diagnostics.log'
modes=[]
for number,line in enumerate(main.read_text(errors='replace').splitlines(),1):
    if ' F4_PRESS' in line:
        mode='HYBRID' if 'PRESS1_' in line else 'DIRECT' if 'PRESS2_' in line else 'OFF'
        modes.append({'tick':line[1:24],'mode':mode,'mainLine':number})
mode_ticks=[x['tick'] for x in modes]
applications=collections.defaultdict(collections.Counter)
physical=collections.defaultdict(collections.Counter)
answers=collections.Counter();examples={}
for number,line in enumerate(diag.open(errors='replace'),1):
    index=bisect.bisect_right(mode_ticks,line[1:24])-1
    mode=modes[index]['mode'] if index>=0 else 'BEFORE_F4'
    if 'SUBMIT_HIT producer=' in line and all(x in line for x in ('effect=APPLICATION','applyNative=1','returned=1')):
        role='HenryAttacker' if re.search(r'attacker=\d+\(PLAYER\) target=',line) else 'HenryVictim' if re.search(r'target=\d+\(PLAYER\)',line) else 'unknown'
        c=applications[mode];c[role+' applicationCallbackLines']+=1
        for flag in ('blocked','perfectBlock','masterStrike'):
            if re.search(r'\b'+flag+r'=1\b',line):c[role+' '+flag]+=1
        examples.setdefault('application '+mode+' '+role,{'line':number,'text':line.strip()})
        if 'perfectBlock=1' in line:examples.setdefault('perfect '+mode+' '+role,{'line':number,'text':line.strip()})
    if ' CONTACT_RECEIPT ' in line:
        c=physical[mode];c['receiptLines']+=1
        for flag in ('submitted','applied','fields','blocked','perfect'):
            if re.search(r'\b'+flag+r'=1\b',line):c[flag]+=1
    if ' VR_HIT_ANSWERED_BY_NPC ' in line:
        answer=re.search(r'\banswer=([^ ]+)',line);lineage=re.search(r'\blineage=([^ ]+)',line)
        answers[mode+' '+(answer[1] if answer else 'UNKNOWN')+' '+(lineage[1] if lineage else 'UNPRINTED')]+=1
        examples.setdefault('answer '+str(len(answers)),{'line':number,'text':line.strip()})
report={
    'sourceHashes':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (main,diag)},
    'modeEdges':modes,'nativeApplicationCallbackLinesByModeAndRole':applications,
    'physicalContactReceiptsByMode':physical,'answeredContactLines':answers,'examples':examples,
    'limits':[
      'Counts are existing budgeted native original callback lines, not total attempts, unique swings or probabilities.',
      'Application filter is SUBMIT_HIT producer + returned=1 + applyNative=1 + effect=APPLICATION. Evaluations excluded.',
      'Roles use explicit native attacker/target PLAYER marker; modes are timestamp join to main F4 press edges.',
      'Native application lines have no physical gesture/ticket. They do NOT prove our tracked blade contact or one-per-gesture attribution.',
      'CONTACT_RECEIPT and geometry/native application lines overlap; never add their totals.',
      'UNKNOWN_CONSERVATIVE answer is no-redelivery safety, not accepted native block/perfect proof.',
      'No causal excessive-perfect-block conclusion, native-window change or bypass follows from these samples.'
    ]}
Path(sys.argv[2]).write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({'applications':applications,'physical':physical,'answers':answers},indent=2))
