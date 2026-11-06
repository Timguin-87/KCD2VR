"""Actual owned AVM1 callbacks and Lua observer; image/native/GPU leaves are explicit mocks."""
from pathlib import Path
import argparse, hashlib, json, zipfile
import build_map_navigation_t2 as builder
from test_map_incremental_d56 import Scene, DRAW_NAMES
from test_map_renderer_t2 import VM, Loader

ROOT=Path(__file__).resolve().parents[1]

class ImageScene(Scene):
    def get_variable(self,element,instance,name):
        if name=='ReceiptD56':return super().get_variable(element,instance,name)
        assert element=='KCD2VRMapT2' and instance==0
        self.calls.append(('GetVariable',(name,)))
        if self.movie_missing or self.get_mode=='nil':return None
        if self.get_mode=='false':return False
        if self.get_mode=='raise':raise RuntimeError('synthetic image variable read failure')
        value=self.vm.env.get(self.variables.get(name))
        return str(value) if self.get_mode=='string' else value
    def diagnostics(self,packet='D57|1'):
        self.calls=[]
        return self.lua.globals().kcd2vr_map_d57_diagnostics(packet)

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--pak',type=Path,required=True,help='Native catalog/image inventory; generated current UI is executed.')
    ap.add_argument('--out',type=Path,required=True)
    args=ap.parse_args();args.out.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(args.pak) as z:
        catalog=z.read('Scripts/KCD2VR/map_catalog_m39.lua').decode();assets={n.lower() for n in z.namelist()}
    raw=(ROOT/'mods/KCD2VRMap/KCD2VRMapT2.gfx').read_bytes()
    xml=(ROOT/'mods/KCD2VRMap/KCD2VRMapT2.xml').read_bytes()
    source=(ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_text()
    s=ImageScene(source,raw,catalog,assets,xml);checks=[]
    def check(value,label):
        if not value:raise AssertionError(label)
        checks.append(label)
    old=VM(builder.gfx(tile_count=20,wide=True,marker_count=257,receipt_d56=True))
    check(all(old.functions[n]==s.vm.functions[n] for n in DRAW_NAMES.values()),'all four draw functions unchanged byte for byte')
    check(old.functions['fc_receipt_d56']==s.vm.functions['fc_receipt_d56'],'movie nonce method unchanged')
    check(s.vm.created==old.created and s.vm.draws==old.draws,'observer creates no extra clips or vector shapes')
    add_listener=Loader.addListener
    try:
        Loader.addListener=None
        no_listener=VM(raw)
        check(no_listener.created==old.created and no_listener.draws==old.draws,'missing optional decoder listener preserves movie initialization')
    finally:Loader.addListener=add_listener
    s.feed('T2|1|1||')
    before=s.snapshot();loads=len(s.vm.loads)
    check(s.diagnostics()=='D57|OFF' and not s.calls,'OFF produces zero extra UI reads')
    s.execute("captureF5=1;System.GetCVar=function(k)if k=='sv_map'then return level end;if k=='kcd2vr_door_capture_f5' then return captureF5 or 0 end;if k=='kcd2vr_door_capture_f2' then return captureF2 or 0 end;return 0 end")
    reply=s.diagnostics()
    check('IMAGE_LOAD_EVENTS' in reply and 'initEvents=0|errorEvents=0' in reply,'initial zero is an event count, not readiness')
    check(len(s.calls)==2,'ON without error reads exactly two variables')
    holder=s.vm.env['map']['t0'];holder['loader'].notify('onLoadInit',holder['image'])
    reply=s.diagnostics()
    check('initEvents=1|errorEvents=0' in reply,'actual bytecode listener receives loader init')
    holder['loader'].notify('onLoadError',holder['image'],'URLNotFound|injected\nvalue',404)
    reply=s.diagnostics()
    check('errorEvents=1|lastError=URLNotFound_injected_value' in reply,'actual listener error is bounded sanitized diagnostic text')
    check(len(s.calls)==3,'ON after error reads exactly three variables')
    check(s.snapshot()==before and len(s.vm.loads)==loads,'observer has no draw, reload, marker or visibility mutation')
    for name in ('d57LoadInitEvents','d57LoadErrorEvents'):s.vm.env[name]=16777215
    holder['loader'].notify('onLoadInit',holder['image'])
    holder['loader'].notify('onLoadError',holder['image'],'LoadNeverCompleted',0)
    check(s.vm.env['d57LoadInitEvents']==16777215 and s.vm.env['d57LoadErrorEvents']==16777215,'event counters saturate without overflow')
    for packet in ('D57|2','D57|-1','D57|NaN','D57|9007199254740992','wrong'):
        reply=s.diagnostics(packet)
        check(not s.calls and ('STALE' in reply or 'INVALID' in reply),'bad/stale activation has zero UI reads: '+packet)
    for mode in ('nil','false','raise','string'):
        s.get_mode=mode;reply=s.diagnostics()
        check('IMAGE_LOAD_PENDING_OR_UNSUPPORTED' in reply and len(s.calls)==2,'unavailable or invalid variable remains unproven: '+mode)
    s.get_mode=None;s.movie_missing=True;reply=s.diagnostics()
    check('IMAGE_LOAD_PENDING_OR_UNSUPPORTED' in reply,'absent movie remains unknown')
    s.movie_missing=False
    s.execute('UIAction.GetVariable=nil')
    check(s.diagnostics()=='D57|UNSUPPORTED' and not s.calls,'missing getter explicitly unsupported without UI reads')
    s.execute('UIAction.GetVariable=function(...)return ui_get(...)end;captureF5=0;captureF2=1')
    check('IMAGE_LOAD_EVENTS' in s.diagnostics(),'F2 independently admits the observer')
    s.execute('captureF2=0')
    check(s.diagnostics()=='D57|OFF' and not s.calls,'stopping capture immediately removes extra reads')
    s.execute('captureF5=1;paperHidden=true');s.tick()
    check('NOT_VISIBLE' in s.diagnostics() and not s.calls,'hidden map never probes movie variables')
    s.execute('paperHidden=false');s.feed('T2|1|1||')
    s.vm=VM(raw)
    reply=s.diagnostics()
    check('initEvents=0|errorEvents=0' in reply,'movie recreation resets event history')
    # Old companion has no observer variables: it stays drawable and the new
    # optional query reports unknown. It never blocks or fabricates readiness.
    s.variables={'ReceiptD56':'d56Receipt'};s.vm=old
    reply=s.diagnostics()
    check('IMAGE_LOAD_PENDING_OR_UNSUPPORTED' in reply,'old movie fallback remains explicit and harmless')
    record={'passed':True,'checks':len(checks),'cases':checks,'source_sha256':hashlib.sha256((ROOT/'mods/KCD2VRMap/map_navigation_t2.lua').read_bytes()).hexdigest(),
        'gfx_sha256':hashlib.sha256(raw).hexdigest(),'xml_sha256':hashlib.sha256(xml).hexdigest(),
        'ui_reads_off':0,'ui_reads_on_no_error':2,'ui_reads_on_error':3,
        'actual_lua':'Lua5.1','actual_owned_avm1':True,'image_decoder_callbacks':'explicit mocked leaf events',
        'game_or_headset_run':False,'gpu_pixels':'NOT_OBSERVED'}
    (args.out/'MAP_IMAGE_D57_RESULTS.json').write_text(json.dumps(record,indent=2)+'\n')
    print(json.dumps(record,indent=2))

if __name__=='__main__':main()
