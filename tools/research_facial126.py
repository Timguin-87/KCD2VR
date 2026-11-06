"""Derive exact body-ownership exceptions from actual DBA tracks and skeleton IDs.

File format reference: CryEngine LoaderDBA::ReadController905, CryHeaders.h and
CGFContent.h. No SDK runtime/vtable contract is used. Every mapped bone CRC is
independently checked against its stored case-sensitive name.
"""
import argparse, hashlib, json, re, struct, zipfile, zlib
from pathlib import Path

DBA='Animations/humans/facials/facials.dba'
CHR='Objects/characters/humans/male/skeleton/male.chr'
ALIAS='animations/humans/facials/gestures/fa_disgust_02.caf'
# Layer1 is observed in the 2026-10-14 C10R3 runtime BODY_GATE receipt.
# D50 neutral idle: D46 layer1 receipts plus the supplied exact-retail
# RIDING_NEUTRAL_FACIAL_TRACKS.json prove 107 facial/blend leaf controls only.
EXCEPTIONS=((ALIAS,7),('animations/humans/facials/gestures/fa_open_mouth.caf',1),
            ('animations/humans/facials/idle/fa_idle_neutral_03.caf',1))

def chunks(data):
    magic,version,count,table=struct.unpack_from('<4sIII',data)
    assert magic==b'CrCh' and version==0x746 and count<10000
    result=[]
    for i in range(count):
        kind,version,ident,size,offset=struct.unpack_from('<HHIII',data,table+16*i)
        assert offset+size<=len(data)
        result.append((kind,version,ident,offset,size))
    return result

def derive(dba,skeleton,alias=ALIAS,layer=7):
    bone=next(x for x in chunks(skeleton) if x[:2]==(0x2000,0x800))
    _,_,_,off,size=bone
    assert (size-32)%584==0
    bones={}
    for p in range(off+32,off+size,584):
        crc=struct.unpack_from('<I',skeleton,p)[0]
        name=skeleton[p+312:p+568].split(b'\0',1)[0].decode('ascii')
        assert name and zlib.crc32(name.encode())==crc and crc not in bones
        limb,parent,children,child=struct.unpack_from('<4i',skeleton,p+568)
        bones[crc]=dict(name=name,children=children,parent_offset=parent)
    chunk=next(x for x in chunks(dba) if x[:2]==(0x100D,0x905))
    _,_,_,off,size=chunk;kp,kr,kt,animations=struct.unpack_from('<4I',dba,off)
    assert 0<animations<10000 and max(kp,kr,kt)<1000000
    p=off+16;table_start=p
    def skip(fmt,n):
        nonlocal p
        result=struct.unpack_from('<'+str(n)+fmt,dba,p);p+=struct.calcsize(fmt)*n;return result
    times=skip('H',kt);tf=skip('I',7)
    positions=skip('H',kp);pf=skip('I',9)
    rotations=skip('H',kr);rf=skip('I',9)
    assert sum(tf)==kt and sum(pf)==kp and sum(rf)==kr
    to=skip('i',kt);po=skip('i',kp);ro=skip('i',kr+1)
    assert ro[-1]<0 # supplied retail DBA is stream prepared
    padding=skip('I',1)[0]
    p+=(-(p-table_start))%4;p+=padding;animbase=p
    tracks=None
    for _ in range(animations):
        length=struct.unpack_from('<H',dba,p)[0];p+=2;assert 0<length<=1024
        name=dba[p:p+length].decode('ascii');p+=length
        assert name.startswith('animations/') and name.endswith('.caf')
        p+=132 # MotionParams905: 11 scalars, two QuatT, 8 heel/toe scalars
        feet=struct.unpack_from('<H',dba,p)[0];p+=2+feet
        count,relative=struct.unpack_from('<Hi',dba,p);p+=6
        assert 0<=relative and animbase+relative+count*20<=off+size
        if name!=alias:continue
        assert tracks is None
        tracks=[]
        for i in range(count):
            crc,pt,pv,rt,rv=struct.unpack_from('<5I',dba,animbase+relative+i*20)
            assert crc in bones and all(v==0xffffffff or v<limit for v,limit in [(pt,kt),(pv,kp),(rt,kt),(rv,kr)])
            b=bones[crc]
            assert re.fullmatch(r'FA_POSE_.+|blend_control_0[1-6]',b['name']) and b['children']==0
            tracks.append(dict(crc=f'{crc:08X}',**b,position_time=pt,position_track=pv,rotation_time=rt,rotation_track=rv))
    assert tracks and len(tracks)==107
    return dict(alias=Path(alias).stem,caf_path=alias,exception_layer=layer,animations_parsed=animations,bones_crc_verified=len(bones),
                tracks=tracks,facial_pose_tracks=sum(t['name'].startswith('FA_POSE_') for t in tracks),
                blend_control_tracks=sum(t['name'].startswith('blend_control_') for t in tracks),
                dba_sha256=hashlib.sha256(dba).hexdigest(),skeleton_sha256=hashlib.sha256(skeleton).hexdigest(),
                dba_member=DBA,skeleton_member=CHR,
                conclusion=f'No root, spine, arm, weapon, alignment, or blend_physics bone tracks. All 107 targets are leaves. Keep this native clip running; its presence alone on observed layer {layer} does not require native body motion.')

def main():
    ap=argparse.ArgumentParser();ap.add_argument('--data',type=Path,required=True);ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--header',type=Path);a=ap.parse_args()
    with zipfile.ZipFile(a.data/'Facials/Facials_english-part2.pak') as z:dba=z.read(DBA)
    with zipfile.ZipFile(a.data/'IPL_Characters-part3.pak') as z:skeleton=z.read(CHR)
    reports=[derive(dba,skeleton,alias,layer) for alias,layer in EXCEPTIONS]
    # Preserve the original primary report shape for existing research consumers.
    report=reports[0].copy();report['additional_exceptions']=reports[1:]
    a.out.parent.mkdir(parents=True,exist_ok=True)
    a.out.write_text(json.dumps(report,indent=2)+'\n')
    if a.header:
        clauses=['(layer=='+str(r['exception_layer'])+'&&std::strcmp(name,"'+r['alias']+'") == 0)' for r in reports]
        a.header.write_text('// Generated by tools/research_facial126.py from retail DBA + skeleton.\n#pragma once\n#include <cstring>\nnamespace kcd2vr::vr_combat111 {\ninline bool BodylessFacial126(const char* name,int layer) noexcept {\n    return name&&('+'||\n                  '.join(clauses)+');\n}\n}\n')
    for r in reports:
        print(json.dumps({k:r[k] for k in ('alias','exception_layer','animations_parsed','bones_crc_verified','facial_pose_tracks','blend_control_tracks','conclusion')}))

if __name__=='__main__':main()
