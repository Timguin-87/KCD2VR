"""Build an OWN AVM1 paper renderer and retail detailed-map images.

No donor SWF/GFx, minimap code/artwork or external renderer. The existing native
%SNDUVS material samples a private .ui texture, using the engine's own UI XML
registration and MovieClipLoader DDS path. Four persistent image holders are
panned under one fixed mask; 97 persistent vector glyphs change properties only.
This produces an overlay for the CURRENT main companion; it never installs it.
"""
from pathlib import Path
import argparse, hashlib, io, json, math, struct, sys, zlib
import xml.etree.ElementTree as ET
from PIL import Image
import build_native_held_map32 as native
import build_companion32 as archive

ROOT = Path(__file__).resolve().parents[1]
ELEMENT = "KCD2VRMapT2"
PREFIX = "Libs/UI/Textures/KCD2VRMapT2/"
MARKERS = 97

def op(code, data=b""):
    return bytes([code]) + (struct.pack("<H", len(data)) + data if code >= 128 else b"")

def push(x):
    if isinstance(x, str): p = b"\0" + x.encode("ascii") + b"\0"
    elif isinstance(x, bool): p = b"\5" + bytes([x])
    elif isinstance(x, int): p = b"\7" + struct.pack("<i", x)
    else: p = b"\1" + struct.pack("<f", x)
    return op(0x96, p)

def var(name): return push(name) + op(0x1C)
def member(obj, key): return obj + (push(key) if isinstance(key, str) else key) + op(0x4E)
def assign(name, value): return push(name) + value + op(0x1D)
def local(name, value): return push(name) + value + op(0x3C)
def setm(obj, key, value): return obj + push(key) + value + op(0x4F)
def call(obj, name, args=()):
    return b"".join(reversed(args)) + push(len(args)) + obj + push(name) + op(0x52)
def stmt(obj, name, args=()): return call(obj, name, args) + op(0x17)
def new(name, args=()): return b"".join(reversed(args)) + push(len(args)) + push(name) + op(0x40)
def iff(condition, body):
    # Jump over body when condition is false. ActionIf branches on stack bool.
    if len(body) > 32767: raise ValueError("AVM1 branch too long")
    return condition + op(0x12) + op(0x9D, struct.pack("<h", len(body))) + body
def fn(name, args, body):
    body += op(0)
    head = name.encode() + b"\0" + struct.pack("<H", len(args))
    head += b"".join(s.encode() + b"\0" for s in args) + struct.pack("<H", len(body))
    # SWF19: DefineFunction's action Length excludes its codeSize sub-block.
    # Ruffle's primary swf/src/avm1/read.rs:219-225 confirms this boundary.
    return op(0x9B, head) + body

def create(parent, name, depth): return call(parent, "createEmptyMovieClip", [push(name), push(depth)])
def polygon(clip, points, color):
    b = stmt(clip, "beginFill", [push(color), push(100)])
    b += stmt(clip, "moveTo", [push(points[0][0]), push(points[0][1])])
    for x, y in points[1:] + points[:1]: b += stmt(clip, "lineTo", [push(x), push(y)])
    return b + stmt(clip, "endFill")

def gfx(tile_count=4, wide=False, marker_count=MARKERS, receipt_d56=False, image_receipts_d57=False):
    root = var("_root")
    init = setm(var("Stage"), "scaleMode", push("noScale")) + setm(var("Stage"), "align", push("TL"))
    init += assign("back", create(root, "back", 1))
    init += polygon(var("back"), [(0,0),(1024,0),(1024,1024),(0,1024)], 0x808080)
    init += assign("map", create(root, "map", 10))
    init += assign("mask", create(root, "mask", 11))
    bounds = [(64,32),(960,32),(960,480),(64,480)] if wide else [(256,0),(768,0),(768,512),(256,512)]
    init += polygon(var("mask"), bounds, 0xFFFFFF)
    init += stmt(var("map"), "setMask", [var("mask")])
    if image_receipts_d57:
        # Owned movie only: passive, saturated image-loader event counters.
        # They are not per-tile readiness or proof of GPU/material output.
        init += assign("d57LoadInitEvents", push(0)) + assign("d57LoadErrorEvents", push(0))
        init += assign("d57LastLoadError", push(""))
        init += setm(root, "onLoadInit", var("fc_load_init_d57"))
        init += setm(root, "onLoadError", var("fc_load_error_d57"))
    for i in range(tile_count):
        init += assign("h", create(var("map"), "t"+str(i), i+1))
        init += setm(var("h"), "image", create(var("h"), "image", 1))
        init += setm(var("h"), "loader", new("MovieClipLoader"))
        if image_receipts_d57:
            # An older/custom decoder without listeners still draws normally.
            init += iff(member(member(var("h"), "loader"), "addListener"),
                stmt(member(var("h"), "loader"), "addListener", [root]))
        init += setm(var("h"), "source", push(""))
        init += setm(var("h"), "_visible", push(False))
    for i in range(marker_count):
        init += assign("m", create(var("map"), "m"+str(i), i+(100 if wide else 20)))
        init += assign("d", create(var("m"), "dot", 1))
        init += polygon(var("d"), [(0,-8),(7,0),(0,8),(-7,0)], 0x231B13)
        init += assign("ink", create(var("d"), "ink", 1))
        init += polygon(var("ink"), [(0,-5),(4,0),(0,5),(-4,0)], 0xDE9F24)
        init += setm(var("m"), "tintD", new("Color", [var("ink")]))
        init += assign("d", create(var("m"), "dir", 2))
        init += polygon(var("d"), [(0,-12),(8,8),(0,4),(-8,8)], 0x231B13)
        init += assign("ink", create(var("d"), "ink", 1))
        init += polygon(var("ink"), [(0,-8),(5,5),(0,2),(-5,5)], 0xDE9F24)
        init += setm(var("m"), "tintA", new("Color", [var("ink")]))
        init += setm(var("m"), "_visible", push(False))
    # A holder keeps its transform while its child image is asynchronously loaded.
    def tile_body(scale_x, scale_y):
        tile = local("h", member(var("map"), push("t")+var("index")+op(0x47)))
        tile += setm(var("h"), "_visible", var("visible"))
        tile += iff(var("visible"),
            setm(var("h"), "_x", var("x")) + setm(var("h"), "_y", var("y")) +
            setm(var("h"), "_xscale", var(scale_x)) + setm(var("h"), "_yscale", var(scale_y)) +
            iff(member(var("h"), "source") + var("file") + op(0x49) + op(0x12),
                setm(var("h"), "source", var("file")) +
                stmt(member(var("h"), "loader"), "loadClip", [push("img://")+var("file")+op(0x47), member(var("h"), "image")])))
        return tile
    tile = tile_body("scale", "scale")
    marker = local("m", member(var("map"), push("m")+var("index")+op(0x47)))
    marker += setm(var("m"), "_x", var("x")) + setm(var("m"), "_y", var("y"))
    marker += setm(var("m"), "_rotation", var("angle")) + setm(var("m"), "_visible", push(True))
    marker += setm(member(var("m"), "dot"), "_visible", var("arrow")+op(0x12))
    marker += setm(member(var("m"), "dir"), "_visible", var("arrow"))
    marker += stmt(member(var("m"), "tintD"), "setRGB", [var("color")])
    marker += stmt(member(var("m"), "tintA"), "setRGB", [var("color")])
    clear = b"".join(setm(member(var("map"), "m"+str(i)), "_visible", push(False)) for i in range(marker_count))
    code = fn("fc_tile", ["index","file","x","y","scale","visible"], tile)
    # Assemble fresh so each ActionIf offset includes its actual operands.
    rectangular = tile_body("scaleX", "scaleY")
    code += fn("fc_tile_rect", ["index","file","x","y","scaleX","scaleY","visible"], rectangular)
    code += fn("fc_marker", ["index","x","y","angle","color","arrow"], marker)
    code += fn("fc_clear", [], clear)
    if receipt_d56:
        # Existing draw method bytecode/ABI stays exact. Only our private M39
        # movie gets a synchronous dispatch receipt, reset by movie recreation.
        code += fn("fc_receipt_d56", ["nonce"], assign("d56Receipt", var("nonce")))
        init += assign("d56Receipt", push(0))
    if image_receipts_d57:
        def count_event(name):
            return iff(var(name)+push(16777215)+op(0x48), assign(name,var(name)+push(1)+op(0x47)))
        code += fn("fc_load_init_d57", ["target"], count_event("d57LoadInitEvents"))
        code += fn("fc_load_error_d57", ["target","errorCode","httpStatus"],
            count_event("d57LoadErrorEvents") + assign("d57LastLoadError",var("errorCode")))
    code += init + op(0x07) + op(0) # stop own timeline; never recreate clips per frame
    bits = format(16,"05b") + "".join(format(v,"016b") for v in (0,20480,0,20480))
    bits += "0"*((-len(bits))%8)
    rect = int(bits,2).to_bytes(len(bits)//8,"big")
    def tag(t, b): return struct.pack("<H",t<<6|63)+struct.pack("<I",len(b))+b
    body = rect + struct.pack("<HH",24<<8,1)
    body += tag(69,struct.pack("<I",0)) + tag(9,bytes([128,128,128]))
    body += tag(12,code) + tag(1,b"") + tag(0,b"")
    # This is authored SWF bytecode, not GFxExport output (no ExporterInfo).
    # Scaleform accepts standard SWF directly; do not impersonate stripped GFx.
    return b"CWS\x08"+struct.pack("<I",len(body)+8)+zlib.compress(body,9)

def ui_xml(receipt_d56=False, image_receipts_d57=False):
    root=ET.Element("UIElements",name=ELEMENT)
    e=ET.SubElement(root,"UIElement",name=ELEMENT,lazy_update="0",mouseevents="0",keyevents="0",controller_input="0",cursor="0")
    g=ET.SubElement(e,"GFx",file=ELEMENT+".gfx",layer="0")
    c=ET.SubElement(g,"Constraints")
    ET.SubElement(c,"Align",mode="fixdyntexsize",valign="center",halign="center")
    ET.SubElement(c,"Position",width="1024",height="1024")
    functions=ET.SubElement(e,"functions")
    for name, func, params in [
        ("Tile","fc_tile",[("index","int"),("file","string"),("x","float"),("y","float"),("scale","float"),("visible","bool")]),
        ("TileRect","fc_tile_rect",[("index","int"),("file","string"),("x","float"),("y","float"),("scaleX","float"),("scaleY","float"),("visible","bool")]),
        ("Marker","fc_marker",[("index","int"),("x","float"),("y","float"),("angle","float"),("color","int"),("arrow","bool")]),
        ("Clear","fc_clear",[])]:
        f=ET.SubElement(functions,"function",name=name,funcname=func)
        for n,t in params: ET.SubElement(f,"param",name=n,type=t)
    if receipt_d56:
        f=ET.SubElement(functions,"function",name="ReceiptD56",funcname="fc_receipt_d56")
        ET.SubElement(f,"param",name="nonce",type="int")
        variables=ET.SubElement(e,"variables")
        ET.SubElement(variables,"variable",name="ReceiptD56",varname="d56Receipt")
    if image_receipts_d57:
        variables=e.find("variables")
        if variables is None: variables=ET.SubElement(e,"variables")
        for public,private in (("LoadInitEventsD57","d57LoadInitEvents"),
                               ("LoadErrorEventsD57","d57LoadErrorEvents"),
                               ("LastLoadErrorD57","d57LastLoadError")):
            ET.SubElement(variables,"variable",name=public,varname=private)
    return ET.tostring(root,encoding="utf-8",xml_declaration=True)

def legacy_pigment(image):
    # Same qualified linear-light neutral modulation as C32; legacy BC1 is the
    # native Flash image decoder path, avoiding its unsupported DXGI72 header.
    def lin(v):
        v=v/255.;return v/12.92 if v<=.04045 else ((v+.055)/1.055)**2.4
    def srgb(v):return round(255*(12.92*v if v<=.0031308 else 1.055*v**(1/2.4)-.055))
    lut=[srgb(lin(v)*lin(128)) for v in range(256)]
    image=image.convert("RGB").point(lut*3)
    out=io.BytesIO();image.save(out,format="DDS",pixel_format="DXT1")
    raw=out.getvalue()
    if raw[84:88]!=b"DXT1" or len(raw)!=128+2048*2048//2:raise ValueError("Pillow BC1 encoding contract")
    return raw

def matte_paper(mat):
    scroll=mat.find("SubMaterials")[1]
    if scroll.get("Name")!="scroll" or scroll.get("Shader")!="Illum":
        raise ValueError("Unexpected map paper material")
    # Own paper only: retain native albedo, normals, second UV and SSS.
    scroll.set("Shininess","10");scroll.set("Specular","0.005,0.005,0.005")
    scroll.find("PublicParams").set("DetailGlossScale","0")

def build(data, output, report):
    retail=native.Retail(Path(data));entries={};tiles={}
    entries["Libs/UI/"+ELEMENT+".gfx"]=gfx()
    entries["Libs/UI/UIElements/"+ELEMENT+".xml"]=ui_xml()
    try:
        for region,(texture,cols,rows,ids) in native.REGIONS.items():
            overview=native.image_dds(native.streamed_dds(retail,f"Libs/UI/Textures/Maps/global_map_{texture}_0.dds"))
            for i in range(1,cols*rows+1):
                path=f"Libs/UI/Textures/Maps/global_map_{texture}_{i}.dds"
                if i in ids: image=native.image_dds(native.streamed_dds(retail,path));source="NATIVE_DETAIL"
                else:
                    x,y=(i-1)%cols,(i-1)//cols
                    image=overview.crop((round(x*overview.width/cols),round(y*overview.height/rows),round((x+1)*overview.width/cols),round((y+1)*overview.height/rows))).resize((2048,2048),Image.Resampling.LANCZOS)
                    source="NATIVE_OVERVIEW_FALLBACK"
                name=PREFIX+region+"_"+str(i)+".dds"
                entries[name]=legacy_pigment(image)
                tiles[name]={"source":source,"sha256":archive.sha(entries[name])}
            # Only existing owned material opacity maps change; geometry/native
            # paper diffuse/normal/animation remain the original C32 resources.
            main=archive.unpack((ROOT/"companion/Mods/KCD2VR/Data/KCD2VR_KICK_D_fulltable_plus_masterstrikes.pak").read_bytes())
            mat=ET.fromstring(main[f"Objects/kcd2vr/map32/{region}.mtl"])
            opacity=mat.findall(".//SubMaterials/Material/Textures/Texture[@Map='Opacity']")
            if len(opacity)!=1:raise ValueError("Unique owned paper opacity texture required")
            opacity[0].set("File",ELEMENT+"@0.ui");opacity[0].set("TexType","5")
            matte_paper(mat)
            entries[f"Objects/kcd2vr/map32/{region}.mtl"]=ET.tostring(mat,encoding="utf-8",xml_declaration=True)
        entries["Scripts/Startup/zzkcd2vr_map.lua"]=(ROOT/"mods/KCD2VRMap/map_runtime.lua").read_bytes()
        entries["Scripts/KCD2VR/map_navigation_t2.lua"]=(ROOT/"mods/KCD2VRMap/map_navigation_t2.lua").read_bytes()
    finally:retail.close()
    result=archive.packed(entries);archive.atomic(Path(output),result)
    record={"overlay_sha256":archive.sha(result),"members":len(entries),"tiles":tiles,
            "own_gfx_no_donor":True,"private_material_ui":ELEMENT+"@0.ui","screen_overlay":False,
            "gfX_build":"OWN_AVM1_GENERATOR","renderer":"4 persistent image holders + 97 vector glyphs",
            "navigation_update_hz_max":2,"closest_local_view_metres":800,
            "retail_inputs":retail.used,"headset_acceptance":"NOT_RUN"}
    archive.atomic(Path(report),(json.dumps(record,indent=2)+"\n").encode())
    return record

if __name__=="__main__":
    p=argparse.ArgumentParser(description=__doc__);p.add_argument("--data",required=True,type=Path)
    p.add_argument("--output",required=True,type=Path);p.add_argument("--report",required=True,type=Path)
    a=p.parse_args();r=build(a.data,a.output,a.report)
    print(json.dumps({k:v for k,v in r.items()if k not in("tiles","retail_inputs")},indent=2))
