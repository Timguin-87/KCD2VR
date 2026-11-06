import contextlib
import hashlib
import io
import json
from pathlib import Path
import sqlite3
import sys
import tempfile
import unittest
import zipfile
import zlib

sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from discover_vrik_logs113 import build
from query_log_discovery113 import rows

class LogDiscovery113(unittest.TestCase):
    def archive(self,root,wrong_hash=False,missing=False):
        data=b'[t] V66.104 NATIVE_COMBAT first\r\n[t] V66.109 PLAYER-ACTION second\vstill same line\nlast\xff'
        sha='0'*64 if wrong_hash else hashlib.sha256(data).hexdigest()
        path=root/'example.zip'
        with zipfile.ZipFile(path,'w') as z:
            z.writestr('INDEX/LOG_INDEX.csv',f'filename,size_bytes,sha256\nx.log,{len(data)},{sha}\n')
            if not missing:z.writestr('LOGS/x.log',data)
        return path,data

    def test_round_trip_provenance_and_family_filter(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);archive,data=self.archive(root);out=root/'out'
            with contextlib.redirect_stdout(io.StringIO()):build([archive],out)
            c=sqlite3.connect(out/'logs.sqlite')
            self.assertEqual(b''.join(zlib.decompress(r[0]) for r in c.execute('SELECT data FROM chunks ORDER BY id')),data)
            result=list(rows(c,'V66.104 NATIVE_COMBAT'))
            self.assertEqual([(r[0],r[1]) for r in result],[('LOGS/x.log',1)])
            self.assertEqual(len(list(rows(c,'V66.104 NATIVE_COMBAT',context=True))),3)
            self.assertEqual(list(rows(c,path_like='%missing%')),[])
            c.close()
            summary=json.loads((out/'summary.json').read_text())
            self.assertEqual(summary['logs'],1)
            self.assertEqual(summary['lossless_reconstruction'],'ALL_MATCH')
            self.assertEqual(summary['corpus_sha256'],hashlib.sha256((out/'logs.sqlite').read_bytes()).hexdigest())

    def test_bad_sha_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);archive,_=self.archive(root,wrong_hash=True)
            with self.assertRaisesRegex(ValueError,'does not match'):build([archive],root/'out')

    def test_missing_log_is_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);archive,_=self.archive(root,missing=True)
            with self.assertRaisesRegex(ValueError,'membership'):build([archive],root/'out')

    def test_empty_input_and_existing_output_are_rejected(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            with self.assertRaises(ValueError):build([],root/'empty')
            archive,_=self.archive(root);out=root/'out';out.mkdir();(out/'logs.sqlite').write_bytes(b'keep')
            with self.assertRaises(ValueError):build([archive],out)
            self.assertEqual((out/'logs.sqlite').read_bytes(),b'keep')

    def test_split_archives_may_repeat_global_index(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);data=[b'first\n',b'second\n']
            index='filename,size_bytes,sha256\n'+''.join(f'{i}.log,{len(d)},{hashlib.sha256(d).hexdigest()}\n' for i,d in enumerate(data))
            paths=[]
            for i,d in enumerate(data):
                p=root/f'{i}.zip';paths.append(p)
                with zipfile.ZipFile(p,'w') as z:
                    z.writestr('INDEX/LOG_INDEX.csv',index);z.writestr(f'LOGS/{i}.log',d)
            with contextlib.redirect_stdout(io.StringIO()):build(paths,root/'out')
            self.assertEqual(json.loads((root/'out/summary.json').read_text())['logs'],2)

if __name__=='__main__':unittest.main()
