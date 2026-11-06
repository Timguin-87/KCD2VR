"""Offline stock-map -> native animated parchment resources. No runtime asset scanning.

Requires Pillow >= 11 (BC1 encoder). Inputs are pinned retail PAK members. This
does not import the retired minimap renderer, artwork, metadata or marker feed.
The generated ZIP is an overlay to MERGE into the one current companion, not a
second installable mod. Nothing is written to the game directory.
"""
from __future__ import annotations
import argparse, hashlib, io, json, math, struct, zipfile
from pathlib import Path
import xml.etree.ElementTree as ET
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "review/C32/Map32_SOURCE_PROVENANCE.json"
CHR = "Objects/characters/assets/parchment_folded/parchment_folded.chr"
MTL = "Objects/characters/assets/parchment_folded/parchment_folded.mtl"
SHADER = "Shaders/HWScripts/CryFX/Illum.cfx"
REGIONS = {"trosky": ("trosecko", 3, 3, tuple(range(1, 10))),
           "kuttenberg": ("kutnohorsko", 6, 5,
             (2,3,4,5,7,8,9,10,11,12,13,14,15,16,17,18,20,21,22,23,27,28))}
ATLAS = 4096  # BC1 + complete native-style mip chain down to 4; ~10.7 MiB/region.

def sha(data): return hashlib.sha256(data).hexdigest()

class Retail:
    def __init__(self, data: Path, record=False):
        self.data=data; self.record=record; self.used={}; self.zips={}
        self.expected={} if record else json.loads(MANIFEST.read_text())["members"]
    def read(self, pak, member):
        key=pak+":"+member
        if pak not in self.zips: self.zips[pak]=zipfile.ZipFile(self.data/pak)
        raw=self.zips[pak].read(member); digest=sha(raw)
        if not self.record and self.expected.get(key,{}).get("sha256")!=digest:
            raise ValueError("Retail source member changed/unpinned: "+key)
        self.used[key]={"size":len(raw),"sha256":digest}
        return raw
    def close(self):
        for z in self.zips.values(): z.close()

def streamed_dds(retail: Retail, member: str):
    """Consume the REAL Cry DDS base + numbered large mips, largest first.

    Retail UI maps use DXT1, an 808-byte base holding mips32..4 and separate
    64..N streams. A missing/short/reordered stream is refused, not padded.
    """
    b=retail.read("IPL_GameData.pak",member)
    if len(b)!=808 or b[:4]!=b"DDS " or b[84:88]!=b"DXT1":
        raise ValueError("Unexpected native map DDS base")
    height,width=struct.unpack_from("<2I",b,12)
    mips=struct.unpack_from("<I",b,28)[0]
    if width!=height or width not in (512,2048) or mips!=int(math.log2(width))-1:
        raise ValueError("Unexpected native map dimensions/mips")
    parts=[]
    for stream in range(int(math.log2(width))-5,0,-1):
        part=retail.read("IPL_GameData.pak",member+"."+str(stream))
        side=32<<stream
        if len(part)!=side*side//2: raise ValueError("Native DDS stream length mismatch")
        parts.append(part)
    return b[:128]+b"".join(parts)+b[128:]

def image_dds(raw):
    # Pillow's BC1 CPU decoder handles linear DXGI71 but some releases omit72.
    # Only the LOCAL decode header is changed; encoded sRGB resources stay72.
    if raw[84:88]==b"DX10" and struct.unpack_from("<I",raw,128)[0]==72:
        raw=raw[:128]+struct.pack("<I",71)+raw[132:]
    return Image.open(io.BytesIO(raw)).convert("RGB")

def socket_uv_metadata(chr_bytes):
    magic,version,count,table=struct.unpack_from("<4sIII",chr_bytes)
    if magic!=b"CrCh" or version!=0x746 or count!=27 or table!=16:
        raise ValueError("Unqualified native parchment chunk table")
    found=None
    for i in range(count):
        kind,ver,ident,size,off=struct.unpack_from("<HHIII",chr_bytes,table+16*i)
        if off+size>len(chr_bytes): raise ValueError("Invalid parchment chunk range")
        if (kind,ver)==(0x1016,0x800):
            flags,stream,n,stride=struct.unpack_from("<4I",chr_bytes,off)
            if stream==8:
                if found or flags!=0 or n!=94 or stride!=8 or size!=24+n*stride:
                    raise ValueError("Unqualified native second UV stream")
                uv=[struct.unpack_from("<2f",chr_bytes,off+24+j*stride) for j in range(n)]
                if not all(math.isfinite(u) and math.isfinite(v) and 0<=u<=1 and 0<=v<=.5 for u,v in uv):
                    raise ValueError("Native paper no longer uses top-half second UV atlas")
                found={"vertices":n,"stream":8,"u":[min(u for u,v in uv),max(u for u,v in uv)],
                       "v":[min(v for u,v in uv),max(v for u,v in uv)],
                       "texture_origin":"north/top at lowV; retained native UVs; no geometry changes"}
    if not found: raise ValueError("Native second UV stream absent")
    return found

def stock_page(retail, region, width=2048):
    texture,cols,rows,ids=REGIONS[region]
    prefix=f"Libs/UI/Textures/Maps/global_map_{texture}_"
    overview=image_dds(streamed_dds(retail,prefix+"0.dds"))
    height=round(width*rows/cols)
    page=overview.resize((width,height),Image.Resampling.LANCZOS)
    for i in ids:
        x=(i-1)%cols; y=(i-1)//cols
        box=(round(x*width/cols),round(y*height/rows),round((x+1)*width/cols),round((y+1)*height/rows))
        tile=image_dds(streamed_dds(retail,prefix+str(i)+".dds"))
        page.paste(tile.resize((box[2]-box[0],box[3]-box[1]),Image.Resampling.LANCZOS),box[:2])
    return page,overview

def paper_atlas(page):
    # Native %SNDUVS shader multiplies original paper albedo by4*TM_Opacity.
    # Its empty sRGB overlay is approximately128. Preserve that neutral paper
    # outside the print, and modulate print pigment in LINEAR light, not byte/4.
    def linear(v):
        v=v/255.; return v/12.92 if v<=.04045 else ((v+.055)/1.055)**2.4
    def srgb(v): return round(255*(12.92*v if v<=.0031308 else 1.055*v**(1/2.4)-.055))
    neutral=linear(128); lut=[srgb(linear(v)*neutral) for v in range(256)]
    atlas=Image.new("RGB",(ATLAS,ATLAS),(128,128,128))
    # Actual native paper secondary UV is2:1 (top half of square atlas). A
    # square map is centred within it, not stretched over the long parchment.
    atlas.paste(page.point(lut*3),((ATLAS-page.width)//2,(ATLAS//2-page.height)//2))
    return atlas

def bc1_srgb_mips(image):
    if image.size!=(ATLAS,ATLAS): raise ValueError("Atlas dimensions exceed fixed resource policy")
    blocks=[]; size=ATLAS
    while size>=4:
        mip=image if size==ATLAS else image.resize((size,size),Image.Resampling.LANCZOS)
        out=io.BytesIO();mip.save(out,format="DDS",pixel_format="DXT1")
        b=out.getvalue()
        if b[84:88]!=b"DXT1" or len(b)!=128+size*size//2:
            raise RuntimeError("Pillow>=11 BC1 encoder required; refusing uncompressed fallback")
        blocks.append(b[128:]);size//=2
    h=bytearray(148);h[:4]=b"DDS "
    for off,val in [(4,124),(8,0xA1007),(12,ATLAS),(16,ATLAS),(20,ATLAS*ATLAS//2),
                    (28,len(blocks)),(76,32),(80,4),(108,0x401008),
                    (128,72),(132,3),(140,1)]:struct.pack_into("<I",h,off,val)
    h[84:88]=b"DX10"
    return bytes(h)+b"".join(blocks)

def material(native, region):
    root=ET.fromstring(native);sub=root.find("SubMaterials")
    if sub is None or len(sub)!=2 or sub[0].get("Shader")!="NoDraw" or sub[1].get("Name")!="scroll":
        raise ValueError("Native parchment material changed")
    scroll=sub[1]
    if "%SNDUVS" not in scroll.get("StringGenMask",""):raise ValueError("Second UV shader not enabled")
    opacity=[x for x in scroll.find("Textures") if x.get("Map")=="Opacity"]
    if len(opacity)!=1: raise ValueError("Missing unique native opacity/second UV map")
    opacity[0].set("File",f"objects/kcd2vr/map32/{region}.dds")
    return ET.tostring(root,encoding="utf-8",xml_declaration=True)

def build(data, output, report, preview=None, record=False):
    retail=Retail(data,record);entries={};detail={}
    try:
        chr_b=retail.read("IPL_Characters-part0.pak",CHR)
        uv=socket_uv_metadata(chr_b)
        empty=retail.read("IPL_Characters-part0.pak","Objects/characters/assets/book/2nduvs_empty_diff.dds")
        if empty[84:88]!=b"DX10" or struct.unpack_from("<I",empty,128)[0]!=72:
            raise ValueError("Native parchment sRGB second-UV format changed")
        neutral=image_dds(empty).getpixel((0,0))
        if not all(120<=channel<=140 for channel in neutral):
            raise ValueError("Native neutral second-UV pigment changed")
        mtl=retail.read("IPL_Characters-part0.pak",MTL)
        shader=retail.read("../Engine/Shaders.pak",SHADER).decode()
        if not all(x in shader for x in ["SNDUVSDiffuseTex","> = TM_Opacity;",
                                         "GetSNDUVSDiffuseTex(secondTC.xy)","(4.0 * attribs.Albedo.xyz * detColor.xyz)"]):
            raise ValueError("Native Illum overlay contract changed")
        for region in REGIONS:
            page,overview=stock_page(retail,region)
            atlas=paper_atlas(page);dds=bc1_srgb_mips(atlas)
            prefix=f"Objects/kcd2vr/map32/{region}"
            entries[prefix+".dds"]=dds;entries[prefix+".mtl"]=material(mtl,region)
            entries[prefix+".cdf"]=(f'<?xml version="1.0"?><CharacterDefinition><Model File="{CHR.lower()}" '
                f'Material="{prefix.lower()}.mtl"/><AttachmentList/></CharacterDefinition>').encode()
            texture,cols,rows,ids=REGIONS[region]
            detail[region]={"stock_texture":texture,"tile_grid":[cols,rows],"detailed_tiles":list(ids),
                "stock_overview_fallback_tiles":sorted(set(range(1,cols*rows+1))-set(ids)),
                "printed_image_dimensions":list(page.size),"atlas_dimensions":[ATLAS,ATLAS],
                "dds_format":"BC1_UNORM_SRGB/DXGI72","mips":11,"bytes":len(dds),"sha256":sha(dds)}
            if preview:
                preview.mkdir(parents=True,exist_ok=True);page.save(preview/(region+"_printed.png"))
        entries["Scripts/Startup/zzkcd2vr_map.lua"]=(ROOT/"mods/KCD2VRMap/map_runtime.lua").read_bytes()
        item=ET.parse(ROOT/"mods/KCD2VRMap/item__kcdvr_map.xml")
        item.find(".//Document").set("Model","characters/assets/parchment_folded/parchment_folded.cdf")
        entries["Libs/Tables/item/item__kcdvr_map.xml"]=ET.tostring(item.getroot(),encoding="utf-8",xml_declaration=True)
        output.parent.mkdir(parents=True,exist_ok=True)
        with zipfile.ZipFile(output,"w",zipfile.ZIP_DEFLATED,compresslevel=9) as z:
            for name,raw in sorted(entries.items()):
                info=zipfile.ZipInfo(name,(2026,10,16,0,0,0));info.compress_type=zipfile.ZIP_DEFLATED;z.writestr(info,raw)
        localization=(ROOT/"mods/KCD2VRMap/text_ui__kcdvr_map.xml").read_bytes()
        ET.fromstring(localization) # separate localization archive, never a Data table
        localization_output=output.with_suffix(".english.xml")
        localization_output.write_bytes(localization)
        result={"english_localization_sidecar":localization_output.name,
            "english_localization_sha256":sha(localization),"overlay_sha256":sha(output.read_bytes()),"regions":detail,"native_second_uv":uv,
            "members":{n:{"size":len(b),"sha256":sha(b)} for n,b in sorted(entries.items())},
            "retail_inputs":retail.used,"retail_inputs_count":len(retail.used),"external_assets":False,
            "runtime_uploads":"native renderer loads ordinary DDS resources; no ASI texture uploads",
            "live_player_quest_markers":False,"current_region":"EXPLICIT_SELECTION_ONLY",
            "headset_acceptance":"NOT_RUN","engine_DDS_decode_render":"NOT_RUN"}
        if record:
            MANIFEST.parent.mkdir(parents=True,exist_ok=True)
            MANIFEST.write_text(json.dumps({"members":retail.used},indent=2)+"\n")
        report.parent.mkdir(parents=True,exist_ok=True);report.write_text(json.dumps(result,indent=2)+"\n")
        return result
    finally:retail.close()

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__);p.add_argument("--data",type=Path,required=True)
    p.add_argument("--output",type=Path,required=True);p.add_argument("--report",type=Path,required=True)
    p.add_argument("--preview",type=Path);p.add_argument("--record-provenance",action="store_true")
    a=p.parse_args();r=build(a.data,a.output,a.report,a.preview,a.record_provenance)
    print(json.dumps({"output":str(a.output),"sha256":r["overlay_sha256"],"regions":r["regions"],
                      "headset_acceptance":"NOT_RUN"},indent=2))
