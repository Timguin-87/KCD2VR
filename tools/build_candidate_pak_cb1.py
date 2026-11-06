"""CB1 (Claude, 23 Oct 2026): one combined Data PAK for the candidate build.
Input = the reviewed H40 PAK (H39 84 members + zzkcd2vr_hand_only_doors_h40.lua), byte-identical, plus ONLY these added members:
  Scripts/Startup/zzkcd2vr_grip_stow_cb1.lua   grip -> hold -> stow (mods/KCD2VRStow)
  Scripts/Startup/zzkcd2vr_kick.lua            #KCD2VR_Kick.List/Test/Stop  (kick clip test commands)
  Scripts/Startup/zzkcd2vr_phys.lua            #KCD2VR_Phys.Push/Pull/Fall/Ragdoll/...  (retail physics binds test commands)
  Scripts/Startup/zzkcd2vr_pick.lua            #KCD2VR_Pick.List/Stow/Native/Draw  (AddItem vs OnUsed comparison)
H41 review: console kick/physics/pick probes are retained as source-only research, not startup gameplay.
Nothing is replaced or removed. Writes the PAK next to the parent (companion/C63_UNIFIED/...) so the normal post-build stage ships it.
usage: python tools/build_candidate_pak_cb1.py --parent <H40 KCD2VR.pak> [--out companion/C63_UNIFIED/Mods/KCD2VR/Data/KCD2VR.pak]
"""
from pathlib import Path
import argparse, hashlib, json, zipfile
ROOT=Path(__file__).resolve().parents[1]
ADDED={
 'Scripts/Startup/zzkcd2vr_grip_stow_cb1.lua': ROOT/'mods/KCD2VRStow/zzkcd2vr_grip_stow_cb1.lua',
}
def sha(b): return hashlib.sha256(b).hexdigest()
def main():
    ap=argparse.ArgumentParser(); ap.add_argument('--parent',required=True); ap.add_argument('--out',default=str(ROOT/'companion/C63_UNIFIED/Mods/KCD2VR/Data/KCD2VR.pak'))
    a=ap.parse_args(); parent=Path(a.parent); out=Path(a.out)
    pb=parent.read_bytes(); members=[]
    with zipfile.ZipFile(parent) as z:
        if z.testzip(): raise SystemExit('parent PAK CRC error')
        for info in z.infolist():
            if info.is_dir(): continue
            if info.filename.casefold() in {k.casefold() for k in ADDED}: raise SystemExit('parent already has '+info.filename)
            members.append((info,z.read(info)))
    tmp=out.with_suffix('.pak.tmp'); out.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(tmp,'w') as z:
        for info,data in members: z.writestr(info,data,compress_type=info.compress_type)  # metadata preserved
        for name,path in ADDED.items():
            zi=zipfile.ZipInfo(name,(2026,10,23,0,0,0)); zi.compress_type=zipfile.ZIP_DEFLATED; z.writestr(zi,path.read_bytes())
    with zipfile.ZipFile(tmp) as z:
        assert z.testzip() is None
        for info,data in members: assert z.read(info.filename)==data
    tmp.replace(out)
    receipt={'parent_pak_sha256':sha(pb),'pak_sha256':sha(out.read_bytes()),'parent_members':len(members),
             'added':{n:sha(p.read_bytes()) for n,p in ADDED.items()},'changed':[],'removed':[]}
    (out.parent/'CB1_PAK_RECEIPT.json').write_text(json.dumps(receipt,indent=2)); print(json.dumps(receipt,indent=2))
if __name__=='__main__': main()
