"""Focused source provenance; never treats a README success claim as a live test."""
import argparse, hashlib, json, re, zipfile
from pathlib import Path

def run(roots,out):
    versions={'115','123R1','126','130','135R1','136R3','146','146R1','147','161','162R1','163','164R2','164R4','165'}
    files={'src/vrik/physical_weapon.h':r'manualAcquire|manualRelease|gap>|gap<|supportMaxAngle|supportConstraint130',
           'src/vrik/physical_native.inl':r'supportDistance|candidate=primary|separation>|PhysCancelNative6698\(',
           'src/vrik/coherent_pose.inl':r'supportMaxAngle|nativeBowTrigger126|manualBow146|supportConstraint130',
           'src/mod.cpp':r'KCD2VR BUILD|grip_weapon_comparison114\s*=|VrPoseSourceMode|SetRightGripFreeLookEnabled|VrContextualHud'}
    result=[];seen=set()
    for root in roots:
        if not root.exists():continue
        for p in root.rglob('*.zip'):
            if 'SOURCE' not in p.name.upper():continue
            match=re.search(r'V66[._](\d+(?:R\d+)?)',p.name,re.I)
            if not match or match[1].upper() not in versions:continue
            with p.open('rb') as f:digest=hashlib.file_digest(f,'sha256').hexdigest()
            if digest in seen:continue
            seen.add(digest)
            try:z=zipfile.ZipFile(p)
            except zipfile.BadZipFile:continue
            entry=dict(version=match[1].upper(),archive=str(p),sha256=digest,members=[])
            for suffix,pattern in files.items():
                choices=[n for n in z.namelist() if n.replace('\\','/').lower().endswith(suffix.lower())]
                if not choices:continue
                name=min(choices,key=lambda n:(n.count('/'),len(n)));data=z.read(name)
                lines=data.decode('utf-8-sig',errors='replace').splitlines()
                entry['members'].append(dict(member=name,sha256=hashlib.sha256(data).hexdigest(),alternate_matches=len(choices)-1,
                    selected_lines=[dict(line=i+1,text=l.strip()) for i,l in enumerate(lines) if re.search(pattern,l)]))
            result.append(entry)
    out.parent.mkdir(parents=True,exist_ok=True)
    out.write_text(json.dumps(dict(scope='Selected production files from available source archives; no headset equivalence claim. Shortest member path avoids older nested trees.',archives=result),indent=2)+'\n')
    print(json.dumps({'archives':len(result),'versions':sorted({e['version'] for e in result})}))
if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('--root',type=Path,action='append',required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args();run(a.root,a.out)
