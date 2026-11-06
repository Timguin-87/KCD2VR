"""Read-only exact-retail scene-helper proof; no minimap/addon dependencies."""
import argparse, hashlib, json, struct
from pathlib import Path
import pefile

def verify(path):
    digest=hashlib.sha256(path.read_bytes()).hexdigest()
    assert digest=="bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7"
    pe=pefile.PE(str(path),fast_load=True)
    assert pe.FILE_HEADER.TimeDateStamp==0x6A350E20
    pins={0xA6D6F4:"48895c240848896c2418488954241056",0x87963C:"48895c24185556574881ecb000000048",
          0x509810:"48895c24084889742410574883ec2045",0x423C30:"0f10020f1181080a00000f104a100f11",
          0x6411AC:"40534883ec208bda33d2e8095ff9ff4c",
          # Actual Hide body: read+8, bool<<4, clear10, OR, write+8.
          0x879663:"8b4908448bc541c1e0048bc183e0efc1e904440bc044894308"}
    for rva,h in pins.items():assert pe.get_data(rva,len(bytes.fromhex(h)))==bytes.fromhex(h),hex(rva)
    for v,slot,fn in [(0x4077048,0x80,0xA6D6F4),(0x4072C78,0x168,0x509810),(0x4072C78,0x328,0x6411AC)]:
        assert struct.unpack("<Q",pe.get_data(v+slot,8))[0]==pe.OPTIONAL_HEADER.ImageBase+fn
    return {"retail_sha256":digest,"timestamp":"6A350E20","pins":len(pins),"vtable_slots":3,
            "own_CEntity_hidden":"entity+8 bit0x10; native Hide87963C reads/writes exact same field",
            "result":"PASS","native_scene_render":"NOT_RUN","headset":"NOT_RUN"}

if __name__=="__main__":
    p=argparse.ArgumentParser();p.add_argument("dll",type=Path);a=p.parse_args()
    print(json.dumps(verify(a.dll),indent=2))
