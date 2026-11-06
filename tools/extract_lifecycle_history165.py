"""Small, reproducible historical evidence extract; never copies the raw corpus."""
import argparse
import json
import re
import sqlite3
from collections import Counter
from pathlib import Path
from query_log_discovery113 import rows

def extract(path):
    db=sqlite3.connect(path.resolve().as_uri()+'?mode=ro',uri=True)
    result={}
    for family,pattern in [('V66.80-F5-CORRELATE','.'),
                           ('V66.89-ANIM-START',r"3d_pick_rhand\.bspace"),
                           ('V66.90-PLAYBACK CLIP',r"3d_pick_rhand\.bspace"),
                           ('V65.77c-F5-INTERACT ACQUIRE',r'ACQUIRE')]:
        count=0;examples=[];successful=[];joins=Counter()
        for log,line,text in rows(db,family):
            if not re.search(pattern,text,re.I):continue
            count+=1
            m=re.search(r'exactJoin=(\d+)',text)
            if m:joins[m[1]]+=1
            if len(examples)<4 or (m and m[1]=='1'):
                sha=db.execute('SELECT sha256 FROM logs WHERE path=?',(log,)).fetchone()
                entry=dict(path=log,line=line,sha256=sha[0] if sha else None,text=text)
                if len(examples)<4:examples.append(entry)
                if m and m[1]=='1':successful.append(entry)
        result[family]=dict(count=count,exactJoin=dict(joins),examples=examples,successfulJoinExamples=successful)
    db.close()
    return dict(source_database=str(path),read_only=True,findings=result,
                interpretation='Pickup animation occurred. Legacy last-item correlation can pair different WUIDs; animation target attribution is explicitly UNVERIFIED. Input ACQUIRE is not a grasp/constraint success.')

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--db',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();result=extract(a.db);a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:dict(count=v['count'],exactJoin=v['exactJoin']) for k,v in result['findings'].items()}))
