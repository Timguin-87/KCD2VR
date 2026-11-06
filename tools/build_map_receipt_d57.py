"""Regenerate owned map image-event diagnostics, replacing shared files atomically."""
from pathlib import Path
import argparse, hashlib, json, os
import build_map_navigation_t2 as renderer

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out',type=Path,default=Path(__file__).resolve().parents[1]/'mods/KCD2VRMap')
    args=parser.parse_args()
    old=renderer.gfx(tile_count=20,wide=True,marker_count=257,receipt_d56=True)
    assert hashlib.sha256(old).hexdigest()=='fc9d6a01c12c91196ffd908fb879b00766ee743d6b60fdad252264f3755d4880'
    data={'KCD2VRMapT2.gfx':renderer.gfx(tile_count=20,wide=True,marker_count=257,receipt_d56=True,image_receipts_d57=True),
          'KCD2VRMapT2.xml':renderer.ui_xml(receipt_d56=True,image_receipts_d57=True)}
    args.out.mkdir(parents=True,exist_ok=True)
    for name,blob in data.items():
        target=args.out/name;temp=target.with_name(target.name+'.d57.tmp')
        temp.write_bytes(blob);os.replace(temp,target)
    print(json.dumps({n:{'bytes':len(b),'sha256':hashlib.sha256(b).hexdigest()} for n,b in data.items()},indent=2))
if __name__=='__main__':main()
