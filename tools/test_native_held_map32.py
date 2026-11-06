"""Actual retail streamed DDS + production atlas/material policy fixture.

Uses real game pixels, CHR UV and Illum shader. The independent low-resolution
stock overview is the oracle for tile ordering, rather than reproducing the
builder's tile arithmetic in the test. Native rendering is not simulated.
"""
import argparse, io, json, struct, tempfile, zipfile, re
from pathlib import Path
from PIL import Image, ImageChops, ImageStat
import build_native_held_map32 as prod

def mse(a,b):
    return sum(x*x for x in ImageStat.Stat(ImageChops.difference(a,b)).rms)/3

def main(data,overlay):
    checks=[];r=prod.Retail(data)
    def ok(name,condition):
        assert condition,name;checks.append(name)
    try:
        for region,(texture,cols,rows,ids) in prod.REGIONS.items():
            page,overview=prod.stock_page(r,region,512)
            # Overview is a separately filtered low-detail asset. Compare macro geography,
            # not high-frequency grain/ink that exists only in detailed native tiles.
            error=mse(page.resize((128,128)),overview.resize((128,128)))
            ok(region+" actual detailed tile order matches independent stock overview",error<10)
        # Same production decoder with a truncated native large mip: fail closed.
        class Short:
            def read(self,p,m):
                b=r.read(p,m);return b[:-1] if m.endswith(".dds.6") else b
        try:prod.streamed_dds(Short(),"Libs/UI/Textures/Maps/global_map_trosecko_1.dds")
        except ValueError:checks.append("truncated actual native mip refused")
        else:raise AssertionError("Short stream accepted")
        # Deliberate wrong physical tiles, same producer, independent oracle.
        class Wrong:
            def read(self,p,m):
                match=re.search(r"global_map_trosecko_([1-9])\.dds",m)
                if match:m=m[:match.start(1)]+str(10-int(match.group(1)))+m[match.end(1):]
                return r.read(p,m)
        wrong,overview=prod.stock_page(Wrong(),"trosky",512)
        ok("wrong native tile regression rejected by independent overview",mse(wrong.resize((128,128)),overview.resize((128,128)))>100)
        native=r.read("IPL_Characters-part0.pak",prod.CHR)
        metadata=prod.socket_uv_metadata(native)
        ok("actual native parchment UV",metadata["vertices"]==94 and metadata["v"][1]<.5)
        damaged=bytearray(native)
        # Locate actual stream independently through the native chunk table.
        _,_,count,table=struct.unpack_from("<4sIII",damaged)
        for i in range(count):
            kind,ver,_,size,off=struct.unpack_from("<HHIII",damaged,table+16*i)
            if (kind,ver)==(0x1016,0x800) and struct.unpack_from("<I",damaged,off+4)[0]==8:
                struct.pack_into("<f",damaged,off+24,2.)
        try:prod.socket_uv_metadata(damaged)
        except ValueError:checks.append("out-of-atlas native UV refused")
        else:raise AssertionError("Invalid UV accepted")
        with zipfile.ZipFile(overlay) as z:
            ok("eight authored overlay members",len(z.namelist())==8 and z.testzip() is None)
            for region in prod.REGIONS:
                p=f"Objects/kcd2vr/map32/{region}"
                b=z.read(p+".dds")
                ok(region+" real DDS format",b[:4]==b"DDS " and b[84:88]==b"DX10" and struct.unpack_from("<I",b,128)[0]==72)
                ok(region+" resource budget",struct.unpack_from("<2I",b,12)==(4096,4096) and struct.unpack_from("<I",b,28)[0]==11 and len(b)==11184956)
                im=prod.image_dds(b)
                # Compression roundtrip contains genuine detailed image, not blank resource.
                center=im.crop((1024,0,3072,2048))
                ok(region+" decoded rendered-resource pixels nonblank",max(ImageStat.Stat(center).var)>50)
                m=prod.ET.fromstring(z.read(p+".mtl"))
                native_m=prod.ET.fromstring(r.read("IPL_Characters-part0.pak",prod.MTL))
                opacity=m.find(".//Texture[@Map='Opacity']")
                ok(region+" material uses actual opacity secondUV slot",opacity.get("File")==p.lower()+".dds")
                opacity.set("File",native_m.find(".//Texture[@Map='Opacity']").get("File"))
                ok(region+" only native texture reference changed",prod.ET.tostring(m)==prod.ET.tostring(native_m))
                c=prod.ET.fromstring(z.read(p+".cdf"))
                ok(region+" native animated geometry retained",c.find("Model").get("File")==prod.CHR.lower())
            ok("no native copied geometry or UI/minimap renderer",not any(n.endswith((".chr",".gfx",".cgf")) for n in z.namelist()))
        localization=prod.ET.fromstring(overlay.with_suffix(".english.xml").read_bytes())
        labels={row.find("Cell").text:row.findall("Cell")[1].text for row in localization.findall("Row")}
        ok("actual English localization two existing keys",set(labels)=={"ui_nm_kcd2vr_map","ui_in_kcd2vr_map"})
        ok("actual localization describes printed map and arm/open distinction",
           "blank" not in labels["ui_in_kcd2vr_map"] and "F2 rung 3 arms" in labels["ui_in_kcd2vr_map"])
        print(json.dumps({"checks":checks,"count":len(checks),"result":"PASS",
            "native_DDS_renderer":"NOT_RUN","headset":"NOT_RUN"},indent=2))
    finally:r.close()

if __name__=="__main__":
    p=argparse.ArgumentParser();p.add_argument("--data",type=Path,required=True);p.add_argument("--overlay",type=Path,required=True)
    a=p.parse_args();main(a.data,a.overlay)
