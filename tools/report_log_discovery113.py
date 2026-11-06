"""Derive reproducible counts, extrema and cited observations from the lossless corpus."""
import argparse
import collections
import csv
import json
from pathlib import Path
import re
import sqlite3
from query_log_discovery113 import rows
from discover_vrik_logs113 import family

HIGH = re.compile(r'V66\.(?:104 NATIVE_COMBAT|109 PLAYER-ACTION|101R1-(?:COMBAT_CAPTURE|WEAPON)|101R2 (?:GATES|PERF)|92-VRIK-HAND-INVARIANT|90-LAB|94-ALIAS|105-FINGER-HARDCODE|104 WEAPON_POSE)')
NUMBER = re.compile(r'\b([A-Za-z][A-Za-z0-9]*)=(-?\d+(?:\.\d+)?)(?=[\s,;]|$)')


def generate(database, output):
    output.mkdir(parents=True, exist_ok=True)
    db=sqlite3.connect(database.resolve().as_uri()+'?mode=ro',uri=True)
    summary={'archives':[dict(zip(('name','sha256','bytes'),r)) for r in db.execute('SELECT * FROM archives')],
             'logs':db.execute('SELECT count(*) FROM logs').fetchone()[0],
             'lines':db.execute('SELECT sum(lines) FROM logs').fetchone()[0]}
    status=collections.Counter(); damage=collections.Counter(); clip_counts=collections.Counter()
    builds=[]; boundaries=[]; clips={}; inputs={}; metrics={}; selected=0
    with (output/'observations.jsonl').open('w',encoding='utf-8') as evidence:
        for path,line,text in rows(db):
            record={'path':path,'line':line,'text':text}
            if 'KCD2VR-V' in text:
                builds.append(record)
            if re.search(r'\bA7A124\b|NATIVE-WEAPON-SWEEP|REFERENCE-SWEEP',text,re.I):
                boundaries.append(record)
            high=HIGH.search(text)
            if high:
                selected+=1;evidence.write(json.dumps(record)+'\n')
                f=family(text.encode())
                for match in NUMBER.finditer(text):
                    name=match[1]
                    if name.lower() in ('tick','sample','epoch','actor','target','key','keys','session','ptr','sequence'):continue
                    value=float(match[2]);k=f+' / '+name
                    if k not in metrics:metrics[k]={'count':0,'min':value,'max':value,'min_at':[path,line],'max_at':[path,line]}
                    m=metrics[k];m['count']+=1
                    if value<m['min']:m['min']=value;m['min_at']=[path,line]
                    if value>m['max']:m['max']=value;m['max_at']=[path,line]
            if 'V66.104 NATIVE_COMBAT' in text:
                m=re.search(r'status=(\w+)',text)
                if m:status[m[1]]+=1
                if m and m[1]=='NATIVE_DAMAGE_OBSERVED':
                    for key in ('flags','healthDamage','staminaDamage'):
                        v=re.search(r'\b'+key+r'=(\S+)',text)
                        if v:damage[key+'='+v[1]]+=1
            clip=('V66.101R1-COMBAT_CAPTURE' in text and 'kind=1 ' in text) or 'V66.89-ANIM-START' in text or 'V66.90-PLAYBACK CLIP' in text
            if clip:
                m=re.search(r'name=[\"\']([^\"\']*)',text)
                name=m[1] if m else '<NO_NAME_FIELD>'
                lay=re.search(r'\blayer=(-?\d+)',text)
                source='capture' if 'COMBAT_CAPTURE' in text else 'start' if 'ANIM-START' in text else 'fifo'
                key=(source,name,lay[1] if lay else '')
                clip_counts[source]+=1;clip_counts[source+':unresolved']+=int('UNRESOLVED' in name or name=='<NO_NAME_FIELD>')
                if key not in clips:clips[key]=[0,path,line,path,line]
                c=clips[key];c[0]+=1;c[3:]=[path,line]
            if 'V66.101R1-COMBAT_CAPTURE' in text and 'kind=0 ' in text:
                m=re.search(r'name="([^"]*)"',text);mode=re.search(r'\bmode=(-?\d+)',text)
                key=(m[1] if m else '',mode[1] if mode else '')
                if key not in inputs:inputs[key]=[0,path,line]
                inputs[key][0]+=1
    def table(name,header,data):
        with (output/name).open('w',encoding='utf-8',newline='') as f:
            w=csv.writer(f);w.writerow(header);w.writerows(data)
    table('animation_observations.csv',['source','name','layer','count','first_path','first_line','last_path','last_line'],
          [(*k,*v) for k,v in sorted(clips.items())])
    table('input_observations.csv',['action','mode','count','first_path','first_line'],
          [(*k,*v) for k,v in sorted(inputs.items())])
    summary.update(native_status=dict(status),damage_fields=dict(damage),clip_counts=dict(clip_counts),
                   selected_observations=selected,build_marker_lines=builds,boundary_mentions=boundaries)
    (output/'report.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    (output/'numeric_ranges.json').write_text(json.dumps(metrics,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in summary.items() if k not in ('build_marker_lines','archives','boundary_mentions')},indent=2))
    print('build marker lines:',len(builds),'new native boundary mentions:',len(boundaries),flush=True)


if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--db',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();generate(a.db,a.out)
