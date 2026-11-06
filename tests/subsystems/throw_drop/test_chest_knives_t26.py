"""Actual T26 starter under real Lua5.1; native inventory/soul leaves are spies."""
from pathlib import Path
import sys,json,hashlib,argparse
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'tests/subsystems/throw_drop'))
from test_inventory_delivery_t6 import HARNESS,load_lua,DEFAULT_DEPS
RECEIPT='969eb6ac-4837-58a0-98d1-2fc9ac06c5d5'
def main():
 p=argparse.ArgumentParser();p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 raw=(ROOT/'src/subsystems/throw_drop/resources/inventory_delivery_t26.lua').read_bytes();source=raw.decode();checks=[]
 Lua=load_lua(DEFAULT_DEPS)
 def fixture(config=''):
  l=Lua(unpack_returned_tuples=True);l.execute(HARNESS.replace("dr='70ceb681-04b8-5c5d-bb49-f80f03c9dc40'", "dr='"+RECEIPT+"'"));l.execute(config);l.execute(source);return l
 def ck(ok,name):
  assert ok,name
  checks.append(name)
 def call(l,load=2,serial=1):return l.globals().kcd2vr_inventory_native_t8(f'{serial}|1|{load}|0')
 l=fixture();g=l.globals();ck(call(l)=='1|DONE|5|0' and len(g.state.calls)==5,'exactly five knives, no axes')
 ck(g.state.perks[RECEIPT] is True,'durable new one-time receipt')
 ck(all(g.state.calls[i].guid==g.gd for i in range(1,6)),'all real unit requests are knife GUID')
 ck(call(l,2,2)=='2|DONE|5|0' and len(g.state.calls)==5,'same load no duplicate')
 l.execute('state.counts={}');ck('ALREADY_CLAIMED' in call(l,4,3) and len(g.state.calls)==5,'depletion and reload do not refill')
 l=fixture("state.perks['70ceb681-04b8-5c5d-bb49-f80f03c9dc40']=true")
 ck(call(l)=='1|DONE|5|0' and len(l.globals().state.calls)==5,'explicit new user grant independent of old exhausted receipt')
 l=fixture('state.counts[gd]=3;state.counts[ga]=2');ck(call(l)=='1|DONE|5|2' and len(l.globals().state.calls)==2,'existing stock preserved; top up to five only, existing axes untouched')
 l=fixture('state.counts[gd]=5');ck(call(l)=='1|DONE|5|0' and len(l.globals().state.calls)==0,'existing five never duplicated')
 for mode in ('false','nil','throw','false_commit','throw_commit'):
  l=fixture("state.failCreateAt=3;state.createMode='"+mode+"'");ck('PARTIAL_UNCERTAIN' in call(l),'partial creation is uncertain '+mode)
  before=len(l.globals().state.calls);call(l,2,2);ck(len(l.globals().state.calls)==before,'uncertain creation not replayed '+mode)
 l=fixture('UIAction=nil;Script.SetTimer=nil');ck(call(l)=='1|DONE|5|0','native post-load delivery independent of timers')
 l=fixture();ck('DONE' not in l.globals().kcd2vr_inventory_native_t8('1|1|0|0') and len(l.globals().state.calls)==0,'invalid native load cannot grant')
 report={'passed':True,'checks':len(checks),'details':checks,'source_sha256':hashlib.sha256(raw).hexdigest(),'native_game':False,'save_serialization':False,'lua':str(Lua)}
 a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
if __name__=='__main__':main()
