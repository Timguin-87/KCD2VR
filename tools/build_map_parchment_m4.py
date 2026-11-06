"""Repack ONLY the four M4 map resources into a frozen combined companion.
No game writes; native inventory/combat/HUD and all tile/marker inputs retained.
"""
import argparse, hashlib, json, zipfile
from pathlib import Path
import xml.etree.ElementTree as ET
import build_map_navigation_t2 as renderer
import build_companion32 as archive
ROOT=Path(__file__).resolve().parents[1]
def build(base,output,report):
    original=base.read_bytes();before=archive.unpack(original);after=dict(before)
    after["Libs/UI/KCD2VRMapT2.gfx"]=renderer.gfx()
    after["Scripts/Startup/zzkcd2vr_map.lua"]=(ROOT/"mods/KCD2VRMap/map_runtime.lua").read_bytes()
    for region in ("trosky","kuttenberg"):
        key=f"Objects/kcd2vr/map32/{region}.mtl"
        mat=ET.fromstring(before[key]);tex=mat.find(".//Texture[@Map='Opacity']")
        if tex.get("File")!="KCD2VRMapT2@0.ui" or tex.get("TexType")!="5":raise ValueError("Wrong current map material")
        renderer.matte_paper(mat)
        after[key]=ET.tostring(mat,encoding="utf-8",xml_declaration=True)
    changed=sorted(k for k in after if after[k]!=before.get(k))
    expected={"Libs/UI/KCD2VRMapT2.gfx","Scripts/Startup/zzkcd2vr_map.lua",*(f"Objects/kcd2vr/map32/{r}.mtl" for r in ("trosky","kuttenberg"))}
    if set(changed)!=expected or set(before)!=set(after):raise ValueError("Unexpected map patch footprint")
    raw=archive.packed(after);archive.atomic(output,raw)
    with zipfile.ZipFile(output) as z:
        assert z.testzip() is None
        for k,b in after.items():assert z.read(k)==b
    record={"base_sha256":archive.sha(original),"output_sha256":archive.sha(raw),"changed":changed,
        "preserved_members":len(before)-len(changed),"native_tiles_and_live_marker_feed":"BYTE_IDENTICAL",
        "native_rendering":"NOT_RUN","headset":"NOT_RUN"}
    archive.atomic(report,(json.dumps(record,indent=2)+"\n").encode());return record
if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__);p.add_argument("--base",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True);p.add_argument("--report",type=Path,required=True)
    a=p.parse_args();print(json.dumps(build(a.base,a.output,a.report),indent=2))
