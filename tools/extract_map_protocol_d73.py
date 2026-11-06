"""Extract the current production map protocol bodies for substituted-VM tests."""
from pathlib import Path
import argparse, os
p=argparse.ArgumentParser();p.add_argument('out',type=Path);a=p.parse_args()
root=Path(__file__).resolve().parents[1]
s=(root/'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp').read_text()
def fn(marker):
 start=s.index(marker);end=s.index('{',start)+1;depth=1
 while depth:
  if s[end]=='{':depth+=1
  elif s[end]=='}':depth-=1
  end+=1
 return s[start:end]+'\n'
decl=s[s.index('struct ReadinessRequest'):s.index('std::uintptr_t base=0;')]
text=decl+'\n'.join(fn(m) for m in ['void ServiceReadiness(', 'void ServiceMarkersD64(', 'void RequestReadiness(', 'bool ReportHelperMissing(', 'bool StaticModelM41('])
a.out.parent.mkdir(parents=True,exist_ok=True)
t=a.out.with_suffix('.tmp');t.write_text(text);os.replace(t,a.out)
print('Extracted actual D73 map readiness, markers, absent recovery and model ACK')
