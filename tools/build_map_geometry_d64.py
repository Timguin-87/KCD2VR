"""D64: crop first UV to the native artwork, resize paper, bake ordinary POIs.

Uses the sealed M41 CGF/CGFM pair as a format template. No texture is stretched,
no engine ABI or actor is changed. Pass --baseline-zip with the sealed D63R1 ZIP.
The output assets are delivered; normal source builds do not need to regenerate.
"""
import argparse, hashlib, json, math, os, struct, sys, zipfile
from pathlib import Path
import xml.etree.ElementTree as ET

ROOT=Path(__file__).resolve().parents[1]
def atomic(p,b):
    p.parent.mkdir(parents=True,exist_ok=True);t=p.with_name(p.name+'.tmp');t.write_bytes(b);os.replace(t,p)
def chunks(d):
    sig,ver,n,off=struct.unpack_from('<4sIII',d)
    assert sig==b'CrCh' and ver==0x746
    out=[]
    for j in range(n):
        typ,v,cid,size,start=struct.unpack_from('<HHIII',d,off+j*16)
        assert start+size<=len(d)
        out.append([typ,v,cid,bytearray(d[start:start+size])])
    return out
def pack(cs):
    out=bytearray(16+16*len(cs));struct.pack_into('<4sIII',out,0,b'CrCh',0x746,len(cs),16)
    for j,(typ,v,cid,b) in enumerate(cs):
        out.extend(b'\0'*((-len(out))%4));offset=len(out);out.extend(b)
        struct.pack_into('<HHIII',out,16+16*j,typ,v,cid,len(b),offset)
    return bytes(out)
def bounds(cs,pts,vertices,indices):
    lo=[min(v[k] for v in pts) for k in range(3)];hi=[max(v[k] for v in pts) for k in range(3)]
    for typ,ver,cid,b in cs:
        if typ==0x1000:
            assert ver==0x801 and len(b)==264
            struct.pack_into('<II',b,8,vertices,indices);struct.pack_into('<6f',b,108,*(lo+hi))
        elif typ==0x1017:
            assert len(b)==56 and struct.unpack_from('<I',b,4)[0]==1
            struct.pack_into('<I',b,20,indices);struct.pack_into('<I',b,28,vertices)
            radius=math.sqrt(sum((hi[k]-lo[k])**2 for k in range(3)))/2
            struct.pack_into('<4f',b,36,radius,*[(lo[k]+hi[k])/2 for k in range(3)])
def pixels(l,x,y):
    a=-l['rotation']*math.pi/180;c,s=math.cos(a),math.sin(a)
    xs=[0,c,c-s,-s];ys=[0,s,s+c,c];sx=l['tiles'][0]*2048/(max(xs)-min(xs));sy=l['tiles'][1]*2048/(max(ys)-min(ys))
    nx=(x-l['world'][0])/l['world'][2];ny=(l['level'][1]-y-l['world'][1])/l['world'][3]
    return sx*c*nx-sy*s*ny-min(xs)*sx-l['crop'][0]*2048,sx*s*nx+sy*c*ny-min(ys)*sy-l['crop'][1]*2048
def plain(v):
    if not hasattr(v,'items'):return v
    keys=list(v.keys())
    if keys and all(isinstance(k,(int,float)) for k in keys):return [plain(v[i]) for i in range(1,len(v)+1)]
    return {k:plain(x) for k,x in v.items()}
def material(name,color=None):
    m=ET.Element('Material',MtlFlags='524544',vertModifType='0');subs=ET.SubElement(m,'SubMaterials')
    ET.SubElement(subs,'Material',Name='proxy',Shader='NoDraw',MtlFlags='525440')
    a=dict(Name='paper' if color is None else 'ink',Shader='Illum',MtlFlags='524416',Diffuse=color or '1,1,1',Specular='0,0,0',Opacity='1',Shininess='1',GenMask='0',StringGenMask='')
    s=ET.SubElement(subs,'Material',**a);t=ET.SubElement(s,'Textures')
    if color is None:ET.SubElement(t,'Texture',File='Objects/kcd2vr/staticmap40/'+name+'.dds',Map='Diffuse')
    ET.SubElement(s,'PublicParams');return ET.tostring(m,encoding='utf-8',xml_declaration=True)
def rename(cs,old,new):
    for row in cs:
        if row[0]==0x1014:
            b=row[3];assert row[1]==0x802 and len(b)>=140
            name=bytes(b[:128]).split(b'\0',1)[0].decode().replace(old,new).encode();assert len(name)<128
            b[:128]=name+b'\0'*(128-len(name))
def marker_mesh(template,points,name,old='kutna_hora',player=False):
    """Own unphysicalized glyph mesh, ordinary four-vertex diamonds or arrow."""
    cs=chunks(template);verts=[];idx=[]
    for x,z,y in points:
        r=.0028 if not player else .006
        for dx,dz in ((-r,0),(0,r),(r,0),(0,-r)):
            verts.append((x+dx,y,z+dz))
        n=len(verts)-4;idx.extend((n,n+1,n+2,n,n+2,n+3,n+2,n+1,n,n+3,n+2,n))
    assert verts and len(verts)<65536
    for typ,ver,cid,b in cs:
        if typ!=0x1016:continue
        stype=struct.unpack_from('<I',b,4)[0];head=bytearray(b[:24])
        if stype==15:
            struct.pack_into('<I',head,8,len(verts));b[:]=head+b''.join(struct.pack('<4e4B2e',*p,1,128,128,0,127,0,0) for p in verts)
        elif stype==5:
            struct.pack_into('<I',head,8,len(idx));b[:]=head+struct.pack('<'+'H'*len(idx),*idx)
        elif stype==6:
            tangent=bytes(b[24:40]);struct.pack_into('<I',head,8,len(verts));b[:]=head+tangent*len(verts)
    verts=[struct.unpack("<3e",struct.pack("<3e",*p))for p in verts]
    bounds(cs,verts,len(verts),len(idx));rename(cs,old,name);return pack(cs),verts,len(idx)
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--baseline-zip',type=Path,required=True);ap.add_argument('--lua-path');a=ap.parse_args()
    if a.lua_path:sys.path.insert(0,a.lua_path)
    from lupa.lua51 import LuaRuntime
    lua=LuaRuntime();lua.execute((ROOT/'mods/KCD2VRMap/map_catalog_static_m41.lua').read_text());cat=plain(lua.globals().KCD2VRMapCatalogM39)
    maps={m['name']:m for m in cat['maps']}
    for region in ('trosecko','kutnohorsko'):
        l=cat['levels'][region];maps[region]={'name':region,'region':region,'rect':[0,0,l['crop'][2]*2048,l['crop'][3]*2048]}
    target=ROOT/'companion/source/Objects/kcd2vr/staticmap40';glyph=ROOT/'companion/source/Objects/kcd2vr/mapmarkers64';rows=[]
    with zipfile.ZipFile(a.baseline_zip) as z:
        prefix='KCD2VR_D63R1_SOURCE/companion/source/Objects/kcd2vr/staticmap40/'
        template=z.read(prefix+'kutna_hora.cgfm');template_cgf=z.read(prefix+'kutna_hora.cgf')
        for name,m in sorted(maps.items()):
            rw,rh=m['rect'][2:];aspect=rw/rh;inkw=.55*min(1,aspect);inkh=.55*min(1,1/aspect);w,h=inkw+.03,inkh+.03
            oldw=min(.46,.26*aspect);oldh=oldw/aspect
            # Source M41 texture contains centered art in the 50x30cm sheet.
            # Crop first UV; the remaining 15mm edge is PAPER, never stretched art.
            uw=(oldw/.5)*(w/inkw);vh=(oldh/.3)*(h/inkh);u0=(1-uw)/2;v0=(1-vh)/2
            mesh=chunks(z.read(prefix+name+'.cgfm'));positions=[]
            for typ,ver,cid,b in mesh:
                if typ==0x1016 and struct.unpack_from('<I',b,4)[0]==15:
                    count=struct.unpack_from('<I',b,8)[0]
                    for k in range(count):
                        off=24+16*k;x,y,zz=struct.unpack_from('<3e',b,off);u,v=struct.unpack_from('<2e',b,off+12)
                        # Keep the native paper's small curve; use quantized output for all bounds.
                        p=(x*w/.5,y,zz*h/.3);struct.pack_into('<3e',b,off,*p);positions.append(struct.unpack_from('<3e',b,off))
                        struct.pack_into('<2e',b,off+12,u0+u*uw,v0+v*vh)
            count=len(positions);bounds(mesh,positions,count,264)
            triangles=[]
            for typ,ver,cid,b in mesh:
                if typ==0x1016 and struct.unpack_from('<I',b,4)[0]==5:
                    ix=struct.unpack_from('<264H',b,24);triangles=[ix[k:k+3] for k in range(0,len(ix),3)]
            def surface(x,z):
                heights=[]
                for ia,ib,ic in triangles:
                    a,b,c=positions[ia],positions[ib],positions[ic]
                    det=(b[2]-c[2])*(a[0]-c[0])+(c[0]-b[0])*(a[2]-c[2])
                    if abs(det)<1e-10:continue
                    u=((b[2]-c[2])*(x-c[0])+(c[0]-b[0])*(z-c[2]))/det
                    v=((c[2]-a[2])*(x-c[0])+(a[0]-c[0])*(z-c[2]))/det
                    if u>=-1e-6 and v>=-1e-6 and u+v<=1.000001:heights.append(u*a[1]+v*b[1]+(1-u-v)*c[1])
                assert heights, 'POI not over paper geometry'
                return min(heights)

            main=chunks(z.read(prefix+name+'.cgf'));bounds(main,positions,count,264)
            atomic(target/(name+'.cgfm'),pack(mesh));atomic(target/(name+'.cgf'),pack(main))
            # Ordinary POIs only. Static geometry adds no per-frame actor scans/VM calls.
            points=[]
            for poi in cat['pois'][m['region']]:
                px,py=pixels(cat['levels'][m['region']],poi[0],poi[1]);u=(px-m['rect'][0])/rw;v=(py-m['rect'][1])/rh
                if 0<=u<=1 and 0<=v<=1:
                    x=(u-.5)*inkw;zz=(.5-v)*inkh
                    # Native paper is curved. Sample nearest source vertex height; glyph front offset 1.5mm.
                    points.append((x,zz,surface(x,zz)-.002))
            assert points, 'No ordinary POIs for '+name
            path='Objects/kcd2vr/mapmarkers64/'+name
            gm,pts,ni=marker_mesh(template,points,name)
            cm=chunks(template_cgf);bounds(cm,pts,len(pts),ni);rename(cm,'Objects/kcd2vr/staticmap40/kutna_hora',path)
            gc=chunks(gm);rename(gc,'Objects/kcd2vr/staticmap40/'+name,path)
            atomic(glyph/(name+'.cgf'),pack(cm));atomic(glyph/(name+'.cgfm'),pack(gc));atomic(glyph/(name+'.mtl'),material(name,'0.13,0.07,0.025'))
            rows.append(dict(name=name,width=w,height=h,inkWidth=inkw,inkHeight=inkh,pois=len(points),uv=[u0,v0,u0+uw,v0+vh],rect=m['rect'],region=m['region']))
        gm,pts,ni=marker_mesh(template,[(0,0,-.018)],'player',player=True);gc=chunks(gm);rename(gc,'Objects/kcd2vr/staticmap40/player','Objects/kcd2vr/mapmarkers64/player')
        cm=chunks(template_cgf);bounds(cm,pts,len(pts),ni);rename(cm,'Objects/kcd2vr/staticmap40/kutna_hora','Objects/kcd2vr/mapmarkers64/player')
        atomic(glyph/'player.cgfm',pack(gc));atomic(glyph/'player.cgf',pack(cm));atomic(glyph/'player.mtl',material('player','0.55,0.015,0.01'))
    header='#pragma once\n#include <cstring>\nnamespace kcd2vr::map_geometry_d64 {\nstruct Sheet {const char* name;float width,height,inkWidth,inkHeight;};\ninline constexpr Sheet sheets[]{\n'
    for r in rows:header+=' {"%s",%.9ff,%.9ff,%.9ff,%.9ff},\n'%(r['name'],r['width'],r['height'],r['inkWidth'],r['inkHeight'])
    header+='};\ninline const Sheet* find(const char* path) noexcept {if(!path)return nullptr;const char* p=std::strrchr(path,\'/\');p=p?p+1:path;for(const auto& s:sheets){const auto n=std::strlen(s.name);if(!std::strncmp(p,s.name,n)&&!std::strcmp(p+n,".cgf"))return &s;}return nullptr;}\n}\n'
    atomic(ROOT/'src/subsystems/vrmap/map_geometry_d64.h',header.encode())
    atomic(ROOT/'tests/data/map/GEOMETRY_D64.json',(json.dumps({'baseline_sha256':hashlib.sha256(a.baseline_zip.read_bytes()).hexdigest(),'sheets':rows},indent=2)+'\n').encode())
    print('D64',len(rows),'aspect-correct paper meshes; static ordinary POI meshes and player glyph generated.')
if __name__=='__main__':main()
