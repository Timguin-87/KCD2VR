#!/usr/bin/env python3
"""Compare 400 outputs of two real body adapters using their native-memory test fixtures.
Usage: python tools/check_normal_pose_parity.py --baseline /path/to/V66.94/tree --out parity.json
This does not load a game, asset curves, rendering or the retail 718-joint character.
"""
from pathlib import Path
import argparse,hashlib,json,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
TEMPLATE=r'''#define main historical_test_main
#include "FIXTURE"
#undef main
#include <fstream>
int main(int argc,char**argv){if(argc!=2)return 2;std::ofstream out(argv[1],std::ios::binary);
 for(int mode:{2,3}){Init();auto t=Tracking();for(int i=0;i<200;++i){float z=float(i)/199.f;
 t.right={.25f+std::sin(z*6.f)*.2f,-.05f-z*.8f,-.03f-.1f*std::cos(z*6.f)};
 t.left={-.25f-std::sin(z*6.f)*.2f,-.05f-z*.8f,-.03f-.1f*std::cos(z*6.f)};
 t.hq={std::sin(z*.4f),0,0,std::cos(z*.4f)};t.tick=30000+i*12;t.sequence=100+i;
 if(!Run(t,mode))return 1;out.write(reinterpret_cast<const char*>(g_pose6689.work.data()),sizeof(QuatT28)*Count);}}
 return out?0:3;}
'''
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--baseline',required=True,type=Path);ap.add_argument('--out',type=Path,default=Path('parity.json'));a=ap.parse_args()
 try:
  with tempfile.TemporaryDirectory() as td:
   t=Path(td);outputs={}
   for label,src in [('baseline',a.baseline.resolve()),('current',ROOT)]:
    fixture=src/'tests/pose_adapter_test.cpp'
    if not fixture.is_file():raise FileNotFoundError(fixture)
    cpp=t/(label+'.cpp');exe=t/label;raw=t/(label+'.bin')
    cpp.write_text(TEMPLATE.replace('FIXTURE',fixture.as_posix()))
    subprocess.run(['g++','-std=c++17','-O0','-pthread',str(cpp),'-o',str(exe)],check=True)
    subprocess.run([str(exe),str(raw)],check=True);outputs[label]=raw.read_bytes()
   equal=outputs['baseline']==outputs['current']
   result={'poses':400,'fixture_joints':48,'modes':[2,3],'native_curve_data_enabled':False,'byte_identical':equal,'hashes':{k:hashlib.sha256(v).hexdigest() for k,v in outputs.items()},'runtime_headset_proof':False}
   a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n');print(json.dumps(result,indent=2));return 0 if equal else 1
 except (OSError,subprocess.CalledProcessError) as exc:print(f'Parity check failed: {exc}');return 2
if __name__=='__main__':raise SystemExit(main())
