"""Map-only native Document overlay. Retail Book/animations/mesh/UI remain native.
Run against the CURRENT combined companion; no game writes or old-PAK replacement.
"""
import argparse,io,json,struct,zipfile,xml.etree.ElementTree as ET
from pathlib import Path
from PIL import Image
import build_native_held_map32 as native
import build_companion32 as archive
ROOT=Path(__file__).resolve().parents[1]

def image_dds(image):
    # Native book/Scaleform images use legacy DXT1 DDS, not a DX10 header.
    # Keep that exact codec path with a complete >=4px mip chain.
    size=image.width
    if image.size!=(size,size) or size!=1024:raise ValueError('native book image must be 1024 square')
    blocks=[];header=None
    for level in range(9):
        width=size>>level;level_image=image if level==0 else image.resize((width,width),Image.Resampling.LANCZOS)
        data=io.BytesIO();level_image.save(data,format='DDS',pixel_format='DXT1');raw=data.getvalue()
        if raw[84:88]!=b'DXT1' or len(raw)!=128+width*width//2:raise ValueError('BC1 encoder unavailable')
        if header is None:header=raw[:128]
        blocks.append(raw[128:])
    h=bytearray(header)
    for offset,value in [(8,0xA1007),(20,size*size//2),(28,len(blocks)),(108,0x401008)]:struct.pack_into('<I',h,offset,value)
    if h[84:88]!=b'DXT1':raise ValueError('native book codec changed')
    return bytes(h)+b''.join(blocks)

def build(data,base,output,report):
    if output.resolve()==base.resolve():raise ValueError('refusing to overwrite input evidence')
    before=archive.unpack(base.read_bytes());after=dict(before)
    key='Libs/Tables/item/item__kcdvr_map.xml'
    if key not in before:raise ValueError('current companion has no own Map item')
    table=ET.fromstring(before[key]);rows=table.findall('ItemClasses/Document')
    if len(rows)!=1 or rows[0].get('Id')!='5f3c2a71-8e4d-4b6a-9c1e-2d7f0b9a4e63':raise ValueError('unexpected own Map row')
    document=rows[0];document.set('Model','characters/assets/parchment_folded/parchment_folded.cdf')
    content=document.find('DocumentContent')
    if content is not None:document.remove(content)
    content=ET.SubElement(document,'DocumentContent')
    for page,region in enumerate(['trosky','kuttenberg']):ET.SubElement(content,'Image',Page=str(page),Image='kcd2vr_map_'+region)
    after[key]=ET.tostring(table,encoding='utf-8',xml_declaration=True)
    retail=native.Retail(data)
    try:
        # Verify actual native maps take precisely this Document/Book/CDF route.
        with zipfile.ZipFile(data/'Tables.pak') as tables:stock_bytes=tables.read('Libs/Tables/item/item.xml')
        if archive.sha(stock_bytes)!='1267b3531c46a36b5eee1897a81227b51bbb2822d0b8de2a69bc8983f67a4b7a':raise ValueError('native Document table changed')
        stock=ET.fromstring(stock_bytes)
        maps=[x for x in stock.iter('Document') if x.get('Type')=='4' and x.get('Model')==document.get('Model') and x.get('EntityScript')=='Book' and x.find('DocumentContent/Image') is not None]
        if not maps:raise ValueError('native map document contract changed')
        for region in ['trosky','kuttenberg']:
            page,_=native.stock_page(retail,region,1024)
            canvas=Image.new('RGB',(1024,1024),(229,218,190));canvas.paste(page,(0,(1024-page.height)//2))
            after[f'Libs/UI/Textures/Books/Maps/kcd2vr_map_{region}_ui.dds']=image_dds(canvas)
    finally:retail.close()
    after['Scripts/Startup/zzkcd2vr_map.lua']=(ROOT/'mods/KCD2VRMap/map_runtime.lua').read_bytes()
    after['Scripts/KCD2VR/map_native_read_h25.lua']=(ROOT/'mods/KCD2VRMap/map_native_read_h25.lua').read_bytes()
    changed=sorted(k for k in after if after[k]!=before.get(k))
    permitted={key,'Scripts/Startup/zzkcd2vr_map.lua','Scripts/KCD2VR/map_native_read_h25.lua',*(f'Libs/UI/Textures/Books/Maps/kcd2vr_map_{r}_ui.dds' for r in ['trosky','kuttenberg'])}
    if not set(changed)<=permitted or not set(before)<=set(after):raise ValueError('unexpected payload mutation')
    raw=archive.packed(after);archive.atomic(output,raw)
    with zipfile.ZipFile(output) as z:
        if z.testzip() is not None:raise ValueError('output CRC failure')
        for k,v in after.items():assert z.read(k)==v
    result={'base_sha256':archive.sha(base.read_bytes()),'output_sha256':archive.sha(raw),'changed':changed,
            'preserved_members':len(before)-len(set(changed)&set(before)),'native_Book_Lua':'UNMODIFIED','native_mesh_and_animation':'UNMODIFIED',
            'native_reading_request':'FocusItem exact event -> native LeftDoubleClick','headset':'NOT_RUN',
            'printed_pages':['Trosky','Kuttenberg'],'native_reading_live_markers':'NOT_CONNECTED',
            'existing_helper_live_marker_resources':'PRESERVED_BYTE_IDENTICAL'}
    archive.atomic(report,(json.dumps(result,indent=2)+'\n').encode());return result
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--data',type=Path,required=True);p.add_argument('--base',type=Path,required=True);p.add_argument('--output',type=Path,required=True);p.add_argument('--report',type=Path,required=True)
    a=p.parse_args();print(json.dumps(build(a.data,a.base,a.output,a.report),indent=2))
