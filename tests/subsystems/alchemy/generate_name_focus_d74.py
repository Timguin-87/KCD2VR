"""Extract current production name callback; native memory reads are explicit spies."""
from pathlib import Path
import argparse,hashlib,json
p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
r=Path(__file__).resolve().parents[3];src=r/'src/interaction_alignment.cpp';raw=src.read_bytes();s=raw.decode().replace("\r\n","\n")
start=s.index('bool ReadAlchemyHandNameFocusD72(');beg=s.index('{',start);end=beg+1;depth=1
while depth:
 depth+=(s[end]=='{')-(s[end]=='}');end+=1
a.out.mkdir(parents=True,exist_ok=True);(a.out/'actual_name_focus_d74.inc').write_text(s[start:end]+'\n')
(a.out/'NAME_FOCUS_PROVENANCE.json').write_text(json.dumps({'source':str(src),'sha256':hashlib.sha256(raw).hexdigest(),'native_reads':'SPY','runtime':'NOT_RUN'},indent=2)+'\n')
