#!/usr/bin/env python3
"""Match native detailed tile pixels to the native cropped overview.

Offline retail-artwork proof only; no reference minimap DLL/artwork is used.
"""
import argparse,hashlib,json,math
from pathlib import Path
from build_native_held_map32 import Retail,streamed_dds,image_dds

def centred(image):
    pixels=list(image.resize((32,32)).getdata());means=[sum(p[c] for p in pixels)/len(pixels) for c in range(3)]
    values=[p[c]-means[c] for p in pixels for c in range(3)]
    return values,math.sqrt(sum(v*v for v in values))
def verify(data):
    retail=Retail(data);checks=[]
    for region,cols,rows,ids in [('trosecko',3,3,[1,5,9]),('kutnohorsko',6,5,[2,3,8,15,22,28])]:
        prefix='Libs/UI/Textures/Maps/global_map_'+region+'_'
        overview=image_dds(streamed_dds(retail,prefix+'0.dds')).resize((cols*100,rows*100))
        candidates=[]
        for row in range(rows):
            for col in range(cols):candidates.append((col,row,*centred(overview.crop((col*100,row*100,(col+1)*100,(row+1)*100)))))
        for ident in ids:
            a,an=centred(image_dds(streamed_dds(retail,prefix+str(ident)+'.dds')));scores=[]
            for col,row,b,bn in candidates:scores.append((sum(x*y for x,y in zip(a,b))/(an*bn),col,row))
            scores.sort(reverse=True);best=scores[0];expected=[(ident-1)%cols,(ident-1)//cols]
            checks.append(dict(region=region,tile=ident,expected_cropped_cell=expected,best_cropped_cell=list(best[1:]),correlation=best[0],next_best=scores[1][0],
                passed=list(best[1:])==expected and best[0]>.97 and best[0]-scores[1][0]>.1))
    retail.close()
    return dict(schema=1,native_execution=False,headset_test=False,projection='level.xml rotation/full tile fit THEN MapTilesCrop; detail suffix IDs index the resulting cropped page',
        checks=checks,retail_members=retail.used,all_passed=all(c['passed'] for c in checks),passed=sum(c['passed'] for c in checks),total=len(checks))
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--data',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);a=ap.parse_args()
    r=verify(a.data);a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_bytes((json.dumps(r,indent=2)+'\n').encode())
    print(json.dumps({k:v for k,v in r.items() if k not in ['checks','retail_members']}))
    for c in r['checks']:
        if not c['passed']:print('FAIL',c)
    return 0 if r['all_passed'] else 1
if __name__=='__main__':raise SystemExit(main())
