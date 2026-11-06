"""Validate generated native mesh chunks, aspect/UV layout and finite POIs."""
import json, struct
from pathlib import Path
from build_map_geometry_d64 import chunks
ROOT=Path(__file__).resolve().parents[1]
data=json.loads((ROOT/'tests/data/map/GEOMETRY_D64.json').read_text());checks=0
for row in data['sheets']:
    assert row['pois']>0;assert abs(row['inkWidth']/row['inkHeight']-row['rect'][2]/row['rect'][3])<1e-7;checks+=2
    assert abs(row['width']-row['inkWidth']-.03)<1e-7 and abs(row['height']-row['inkHeight']-.03)<1e-7;checks+=1
    for folder in ('staticmap40','mapmarkers64'):
        root=ROOT/'companion/source/Objects/kcd2vr'/folder
        mesh=chunks((root/(row['name']+'.cgfm')).read_bytes());head=chunks((root/(row['name']+'.cgf')).read_bytes())
        verts=inds=None
        for typ,ver,cid,b in mesh:
            if typ==0x1016:
                stype,count,esize=struct.unpack_from('<3I',b,4);assert len(b)==24+count*esize;checks+=1
                if stype==15:verts=[struct.unpack_from('<3e',b,24+i*16)for i in range(count)]
                if stype==5:inds=struct.unpack_from('<'+'H'*count,b,24)
        assert verts and inds and max(inds)<len(verts) and len(inds)%3==0;checks+=1
        for typ,ver,cid,b in mesh+head:
            if typ==0x1000:
                nv,ni=struct.unpack_from('<2I',b,8);assert nv==len(verts) and ni==len(inds);checks+=1
                bb=struct.unpack_from('<6f',b,108)
                assert all(bb[k]-.00001<=v[k]<=bb[k+3]+.00001 for v in verts for k in range(3));checks+=1
            if typ==0x1014:
                assert len(b)==153 and struct.unpack_from('<I',b,128)[0]==2;checks+=1
                expected=('Objects/kcd2vr/'+folder+'/'+row['name']).encode()
                assert b[:128].split(b'\0',1)[0]==expected;checks+=1
print(json.dumps({'result':'PASS','checks':checks,'sheets':len(data['sheets']),'native_rendering':'NOT_RUN'}))
