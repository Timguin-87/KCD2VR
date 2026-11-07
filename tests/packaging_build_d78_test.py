"""Focused packaging integration tests; writes only a caller-selected evidence folder.

Usage: python tests/packaging_build_d78_test.py --out <new-test-directory>
No game files are read or changed. No native build or network access is needed.
"""
from pathlib import Path
import argparse, hashlib, importlib, json, os, shutil, subprocess, sys, tempfile, unittest, zipfile

SOURCE=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(SOURCE/'tools'))
import prepare_dmk_dependency as dependency
from optional_gore_d74 import stage_external_optional, validate_optional

HASH=lambda b:hashlib.sha256(b).hexdigest()
ROOT=None

def write(path,data):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_bytes(data if isinstance(data,bytes) else data.encode())

def js(path,value):write(path,json.dumps(value,indent=2)+'\n')

def optional(root):
    pak=b'optional sealed fixture archive';manifest=b'<Mod/>\n'
    write(root/'Mods/kcd2vr_gore/Data/kcd2vr_gore.pak',pak)
    write(root/'Mods/kcd2vr_gore/mod.manifest',manifest)
    js(root/'Mods/kcd2vr_gore/KCD2VR_GORE_D74.json',{'pak_sha256':HASH(pak),'inputs':{'manifest_sha256':HASH(manifest)}})
    write(root/'README_OPTIONAL_GORE.txt','separate optional content')
    return root

def make_zip(root,path):
    with zipfile.ZipFile(path,'w',zipfile.ZIP_DEFLATED) as z:
        for f in sorted(root.rglob('*')):
            if f.is_file():z.write(f,f.relative_to(root).as_posix())
    return path

def fixture_companion(root):
    mod=root/'Mods/KCD2VR';pak=b'gameplay fixture archive';manifest=b'<Mod/>\n'
    write(mod/'Data/KCD2VR.pak',pak);write(mod/'mod.manifest',manifest)
    js(mod/'KCD2VR_UNIFIED63.json',{'version':63,'pak_sha256':HASH(pak),'distribution_files':{'mod.manifest':HASH(manifest)}})
    js(root/'COMPANION_SELECTION.json',{'schema':1,'optional_gore':False});js(root/'patch2_companion_inputs.json',{})
    return root

class Packaging(unittest.TestCase):
    def setUp(self):self.work=Path(tempfile.mkdtemp(prefix=self._testMethodName+'-',dir=ROOT))
    @classmethod
    def setUpClass(cls):
        cls.cmake=shutil.which('cmake')
        if not cls.cmake:
            matches=list(Path('C:/Program Files/Microsoft Visual Studio').glob('*/**/CMake/bin/cmake.exe'))
            if matches:cls.cmake=str(matches[0])
        if not cls.cmake:raise RuntimeError('CMake is required to test the actual staging adapter')
    def stage(self,source,out,ok=True):
        p=subprocess.run([self.cmake,'-DSOURCE_ROOT='+str(source),'-DTARGET_ROOT='+str(out),'-P',str(SOURCE/'cmake/stage_companion63.cmake')],capture_output=True,text=True)
        write(self.work/('stage-'+out.name+'.txt'),p.stdout+p.stderr)
        self.assertEqual(p.returncode==0,ok,p.stdout+p.stderr)
        return p
    def test_main_only_stages_without_optional_input(self):
        src=fixture_companion(self.work/'prepared');dst=self.work/'staged'
        self.stage(src,dst)
        self.assertFalse((dst/'OptionalGore').exists())
        self.assertEqual((dst/'Mods/KCD2VR/Data/KCD2VR.pak').read_bytes(),b'gameplay fixture archive')
        self.assertEqual(len([p for p in dst.rglob('*') if p.is_file()]),5)
    def test_unselected_leftover_optional_is_not_forwarded(self):
        src=fixture_companion(self.work/'prepared');optional(src/'OptionalGore');dst=self.work/'staged'
        self.stage(src,dst)
        self.assertFalse((dst/'OptionalGore').exists())
    def test_explicit_optional_zip_stages_separately(self):
        supplied=optional(self.work/'supplied');archive=make_zip(supplied,self.work/'optional.zip')
        src=fixture_companion(self.work/'prepared');stage_external_optional(archive,src)
        js(src/'COMPANION_SELECTION.json',{'schema':1,'optional_gore':True})
        dst=self.work/'staged';self.stage(src,dst)
        self.assertEqual(validate_optional(dst/'OptionalGore/Mods/kcd2vr_gore')['pak_sha256'],HASH(b'optional sealed fixture archive'))
        self.assertEqual((dst/'Mods/KCD2VR/Data/KCD2VR.pak').read_bytes(),b'gameplay fixture archive')
    def test_optional_folder_and_corrupt_replacement_fail_closed(self):
        supplied=optional(self.work/'supplied');dst=self.work/'prepared';stage_external_optional(supplied,dst)
        before=(dst/'OptionalGore/Mods/kcd2vr_gore/Data/kcd2vr_gore.pak').read_bytes()
        write(supplied/'Mods/kcd2vr_gore/Data/kcd2vr_gore.pak',b'corrupt')
        archive=make_zip(supplied,self.work/'corrupt.zip')
        with self.assertRaisesRegex(ValueError,'hash mismatch'):stage_external_optional(archive,dst)
        self.assertEqual((dst/'OptionalGore/Mods/kcd2vr_gore/Data/kcd2vr_gore.pak').read_bytes(),before)
    def test_optional_zip_cannot_introduce_extra_or_traversal_path(self):
        supplied=optional(self.work/'supplied');archive=make_zip(supplied,self.work/'extra.zip')
        with zipfile.ZipFile(archive,'a') as z:z.writestr('../escape',b'no')
        with self.assertRaisesRegex(ValueError,'layout'):stage_external_optional(archive,self.work/'prepared')
        self.assertFalse((self.work/'escape').exists())
    def test_stale_target_is_refused_without_deletion(self):
        src=fixture_companion(self.work/'prepared');dst=self.work/'staged';optional(dst/'OptionalGore')
        self.stage(src,dst,ok=False)
        self.assertTrue((dst/'OptionalGore/Mods/kcd2vr_gore/Data/kcd2vr_gore.pak').exists())
    def test_missing_selected_optional_or_corrupt_main_fail(self):
        src=fixture_companion(self.work/'prepared');js(src/'COMPANION_SELECTION.json',{'schema':1,'optional_gore':True})
        self.stage(src,self.work/'missing',ok=False)
        js(src/'COMPANION_SELECTION.json',{'schema':1,'optional_gore':False});write(src/'Mods/KCD2VR/Data/KCD2VR.pak',b'corrupt')
        self.stage(src,self.work/'corrupt',ok=False)
    def test_bad_selection_type_is_refused(self):
        src=fixture_companion(self.work/'prepared');js(src/'COMPANION_SELECTION.json',{'schema':1,'optional_gore':'false'})
        self.stage(src,self.work/'staged',ok=False)
    def test_dependency_actual_offline_restore_matches_every_pin(self):
        package=SOURCE/'deps/DetourModKit';out=self.work/'restored'
        dependency.prepare(package,out)
        manifest=json.loads((package/'DEPENDENCY_MANIFEST.json').read_text())
        for row in manifest['files']:
            self.assertEqual(dependency.sha(out/row['path']),row['sha256'],row['path'])
        for name in dependency.EXTRA:self.assertEqual((out/name).read_bytes(),(package/name).read_bytes())
        self.assertEqual(dependency.sha(out/dependency.LIBRARY),dependency.EXPECTED_LIBRARY_SHA)
        self.assertFalse((out/'lib/libDetourModKit.lib.zip').exists())
        self.assertFalse(any('.lib.' in p.name for p in out.rglob('*') if p.is_file()))
        # Cached restore also checks the actual linked library, repairing only owned output.
        write(out/dependency.LIBRARY,b'bad output')
        dependency.prepare(package,out)
        self.assertEqual(dependency.sha(out/dependency.LIBRARY),dependency.EXPECTED_LIBRARY_SHA)
    def test_dependency_rejects_bad_receipt_before_output(self):
        package=SOURCE/'deps/DetourModKit';fixture=self.work/'bad-input';fixture.mkdir()
        shutil.copyfile(package/'DEPENDENCY_MANIFEST.json',fixture/'DEPENDENCY_MANIFEST.json')
        receipt=json.loads((package/'COMPRESSED_LIBRARY.json').read_text());receipt['sha256']='0'*64
        js(fixture/'COMPRESSED_LIBRARY.json',receipt)
        with self.assertRaisesRegex(ValueError,'receipt'):dependency.prepare(fixture,self.work/'out')
        self.assertFalse((self.work/'out').exists())
    def test_dependency_rejects_unsafe_members_and_source_output_alias(self):
        for path in ('../lib','a/../lib','C:/lib','/lib','a\\b','a//b','./lib'):
            with self.assertRaises(ValueError,msg=path):dependency.relative(path)
        with self.assertRaises(ValueError):dependency.prepare(SOURCE/'deps/DetourModKit',SOURCE/'deps/DetourModKit/lib')

if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args();ROOT=args.out.resolve();ROOT.mkdir(parents=True,exist_ok=True)
    result=unittest.TextTestRunner(verbosity=2).run(unittest.defaultTestLoader.loadTestsFromTestCase(Packaging))
    js(ROOT/'RESULTS.json',{'tests':result.testsRun,'failures':len(result.failures),'errors':len(result.errors),'source':str(SOURCE),'headset':'NOT_RUN'})
    raise SystemExit(not result.wasSuccessful())
