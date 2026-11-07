import importlib.util, json, subprocess, tempfile, unittest, zipfile
from pathlib import Path
spec=importlib.util.spec_from_file_location("subsystem_patch",Path(__file__).resolve().parents[1]/"tools/subsystem_patch.py")
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class PatchTests(unittest.TestCase):
 def setUp(self):
  self.tmp=tempfile.TemporaryDirectory();self.addCleanup(self.tmp.cleanup);self.root=Path(self.tmp.name)/"repo";self.root.mkdir()
  self.manifest={"schema":1,"base_contract":"test1","shared_host_bridges":["src/mod.cpp"],"modules":{k:{"implementation_prefix":"src/subsystems/"+k+"/","owned_files":["src/"+k+".h"]} for k in ["a","b"]}}
  (self.root/"subsystems.json").write_text(json.dumps(self.manifest));(self.root/"src").mkdir()
  for k in ["a","b"]:(self.root/"src"/(k+".h")).write_bytes((k+" old").encode())
  (self.root/"src/mod.cpp").write_text("protected")
  m.git(self.root,"init");m.git(self.root,"add",".")
  m.git(self.root,"-c","user.name=Fixture","-c","user.email=fixture@example.invalid","commit","-m","base")
  self.base=m.git(self.root,"rev-parse","HEAD").decode().strip();self.archive=Path(self.tmp.name)/"a.zip"
 def build(self):
  (self.root/"src/a.h").write_bytes(b"a new");m.create(self.root,"a",self.base,self.archive)
  (self.root/"src/a.h").write_bytes(b"a old")
 def tamper(self,mutate):
  with zipfile.ZipFile(self.archive) as z:data={n:z.read(n) for n in z.namelist()}
  mutate(data)
  with zipfile.ZipFile(self.archive,"w") as z:
   for n,v in data.items():z.writestr(n,v)
 def test_independent_change_survives(self):
  self.build();(self.root/"src/b.h").write_bytes(b"b separate work")
  meta,n=m.apply(self.root,self.archive);self.assertEqual(n,1);self.assertEqual((self.root/"src/b.h").read_bytes(),b"b separate work")
  self.assertEqual(m.apply(self.root,self.archive)[1],0)
 def test_same_file_conflict_no_write(self):
  self.build();(self.root/"src/a.h").write_bytes(b"concurrent")
  with self.assertRaises(m.PatchError):m.apply(self.root,self.archive)
  self.assertEqual((self.root/"src/a.h").read_bytes(),b"concurrent")
 def test_payload_corruption(self):
  self.build();self.tamper(lambda d:d.update({"source/src/a.h":b"wrong"}))
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_shared_file_refused(self):
  self.build()
  def change(d):
   meta=json.loads(d["patch.json"]);meta["files"][0]["path"]="src/mod.cpp";d["patch.json"]=json.dumps(meta).encode();d["source/src/mod.cpp"]=d.pop("source/src/a.h")
  self.tamper(change)
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_escape_refused(self):
  self.build()
  def change(d):
   meta=json.loads(d["patch.json"]);meta["files"][0]["path"]="../outside";d["patch.json"]=json.dumps(meta).encode()
  self.tamper(change)
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_wrong_base_refused(self):
  self.build();self.manifest["base_contract"]="other";(self.root/"subsystems.json").write_text(json.dumps(self.manifest))
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_extra_member_refused(self):
  self.build();self.tamper(lambda d:d.update({"run.cmd":b"must never execute"}))
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_add_and_delete(self):
  folder=self.root/"src/subsystems/a";folder.mkdir(parents=True);(folder/"new.cpp").write_bytes(b"new source")
  (self.root/"src/a.h").unlink();m.create(self.root,"a",self.base,self.archive)
  (folder/"new.cpp").unlink();(self.root/"src/a.h").write_bytes(b"a old");self.assertEqual(m.apply(self.root,self.archive)[1],2)
  self.assertFalse((self.root/"src/a.h").exists());self.assertEqual((folder/"new.cpp").read_bytes(),b"new source")
 def test_frozen_base_change_refused(self):
  self.manifest["base_files"]={"src/mod.cpp":m.content_sha((self.root/"src/mod.cpp").read_bytes())}
  (self.root/"subsystems.json").write_text(json.dumps(self.manifest));m.git(self.root,"add","subsystems.json");m.git(self.root,"-c","user.name=Fixture","-c","user.email=fixture@example.invalid","commit","-m","freeze base")
  self.base=m.git(self.root,"rev-parse","HEAD").decode().strip();self.build();(self.root/"src/mod.cpp").write_text("different ABI")
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
 def test_git_lf_checkout_crlf(self):
  (self.root/"src/a.h").write_bytes(b"old\n");m.git(self.root,"add","src/a.h");m.git(self.root,"-c","user.name=Fixture","-c","user.email=fixture@example.invalid","commit","-m","newline base")
  self.base=m.git(self.root,"rev-parse","HEAD").decode().strip()
  (self.root/"src/a.h").write_bytes(b"new\r\n");m.create(self.root,"a",self.base,self.archive);(self.root/"src/a.h").write_bytes(b"old\r\n")
  self.assertEqual(m.apply(self.root,self.archive)[1],1);self.assertEqual((self.root/"src/a.h").read_bytes(),b"new\r\n")
 def test_manifest_newline_compatible(self):
  raw=json.dumps(self.manifest,indent=2)+"\n";(self.root/"subsystems.json").write_bytes(raw.encode())
  self.build();(self.root/"subsystems.json").write_bytes(raw.replace("\n","\r\n").encode())
  self.assertEqual(m.apply(self.root,self.archive)[1],1)
 def test_case_duplicate_archive_refused(self):
  self.build();self.tamper(lambda d:d.update({"SOURCE/src/a.h":b"duplicate"}))
  with self.assertRaises(m.PatchError):m.verify(self.root,self.archive)
if __name__=="__main__":unittest.main()
