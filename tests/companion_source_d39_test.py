"""Production release preparation regressions: current scripts and cache validity."""
from contextlib import redirect_stdout
import hashlib,io,json,shutil,sys,tempfile,unittest,zipfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from companion_sources_d39 import SOURCE_PAIRS
from package_release_helpers_r19 import prepare_companion

class CurrentCompanionD39(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp=tempfile.TemporaryDirectory(prefix='kcd2vr-d39-package-')
        cls.addClassCleanup(cls.temp.cleanup)
        cls.source=Path(cls.temp.name)/'source';cls.out=Path(cls.temp.name)/'prepared'
        for source in [*SOURCE_PAIRS,'src/build_identity.h']:
            p=cls.source/source;p.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(ROOT/source,p)
        donor=Path('companion/C63_UNIFIED')
        shutil.copytree(ROOT/donor,cls.source/donor,dirs_exist_ok=True)
        cls.prepare()
    @classmethod
    def prepare(cls):
        with redirect_stdout(io.StringIO()):prepare_companion(cls.source,cls.out)
    def test_01_all_current_members(self):
        with zipfile.ZipFile(self.out/'Mods/KCD2VR/Data/KCD2VR.pak') as z:
            for source,member in SOURCE_PAIRS.items():
                with self.subTest(member=member):self.assertEqual(z.read(member),(self.source/source).read_bytes())
    def test_02_map_edit_invalidates_package(self):
        # This map runtime was outside the old helper's replacement list.
        p=self.source/'mods/KCD2VRMap/map_runtime.lua';original=p.read_bytes()
        try:
            p.write_bytes(original+b'\n-- D39 packaging regression fixture\n');self.prepare()
            with zipfile.ZipFile(self.out/'Mods/KCD2VR/Data/KCD2VR.pak') as z:
                self.assertEqual(z.read('Scripts/Startup/zzkcd2vr_map.lua'),p.read_bytes())
        finally:p.write_bytes(original);self.prepare()
    def test_03_corrupt_distribution_is_repaired(self):
        path=self.out/'Mods/KCD2VR/mod.manifest';original=path.read_bytes();path.write_bytes(b'bad manifest')
        self.prepare();self.assertEqual(path.read_bytes(),original)
    def test_04_corrupt_metadata_is_repaired(self):
        path=self.out/'Mods/KCD2VR/KCD2VR_UNIFIED63.json';stale=json.loads(path.read_text());stale['members_provenance'][0]['sha256']='0'*64;path.write_text(json.dumps(stale))
        self.prepare();meta=json.loads(path.read_text())
        self.assertEqual(meta['pak_sha256'],hashlib.sha256((path.parent/'Data/KCD2VR.pak').read_bytes()).hexdigest())
        with zipfile.ZipFile(path.parent/'Data/KCD2VR.pak') as package:
            self.assertEqual({row['path']:row['sha256'] for row in meta['members_provenance']},
                             {name:hashlib.sha256(package.read(name)).hexdigest() for name in package.namelist()})
    def test_05_identical_reprepare_preserves_pak(self):
        path=self.out/'Mods/KCD2VR/Data/KCD2VR.pak';before=path.stat().st_mtime_ns;digest=hashlib.sha256(path.read_bytes()).hexdigest()
        self.prepare();self.assertEqual(before,path.stat().st_mtime_ns);self.assertEqual(digest,hashlib.sha256(path.read_bytes()).hexdigest())
if __name__=='__main__':unittest.main()
