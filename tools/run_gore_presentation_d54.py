"""Run actual shipped Lua presentation fixtures (requires lupa.lua51)."""
from pathlib import Path
import argparse,hashlib,io,json,sys,unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args()
    from tests.test_gore_presentation_d54 import GorePresentationD54
    from tests.test_gore_d52_lua import GoreD52
    suite=unittest.TestSuite([unittest.defaultTestLoader.loadTestsFromTestCase(GorePresentationD54),unittest.defaultTestLoader.loadTestsFromTestCase(GoreD52)])
    buf=io.StringIO();result=unittest.TextTestRunner(stream=buf,verbosity=2).run(suite)
    args.out.mkdir(parents=True,exist_ok=True)
    (args.out/'GORE_PRESENTATION_LUA_TESTS.txt').write_text(buf.getvalue())
    files=['mods/KCD2VRGore/limbs41/zzkcd2vr_gore_v6232.lua','mods/KCD2VRGore/d54/kcd2vr_gore_d54_material_wrappers.pak','tests/test_gore_presentation_d54.py','tests/test_gore_d52_lua.py']
    report={'tests_run':result.testsRun,'failures':len(result.failures),'errors':len(result.errors),'runtime':'Lupa Lua5.1','headset':'NOT_RUN',
      'engine_entities_and_render_leaves':'MOCKED','source_sha256':{p:hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in files},
      'evidence':'Full shipped Lua module executed, not a copied state machine. Generated PAK material tables and CA_SKIN CDFs read for five integration cases. Native loading/parenting/render leaves mocked; no headset visibility proof.'}
    (args.out/'GORE_PRESENTATION_LUA_TESTS.json').write_text(json.dumps(report,indent=2)+'\n')
    print(buf.getvalue());print(json.dumps(report,indent=2))
    return 0 if result.wasSuccessful() else 1

if __name__=='__main__':raise SystemExit(main())
