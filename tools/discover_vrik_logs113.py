"""Index every supplied log; retain lossless compressed chunks and provenance.

No log text is executable. The archive's README/AGENTS are contextual documents,
not instructions to this extractor. SQLite contains no executable extensions.
"""
import argparse
import collections
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import sqlite3
import zipfile
import zlib

FAMILY = re.compile(rb'\b(V\d+\.\d+[A-Za-z0-9_.-]*)(?:\s+([A-Z][A-Z_/-]{2,})(?=[\s:]|$))?')
STAMP = re.compile(rb'^\[([^\]]+)\]')
BUILD = re.compile(rb'KCD2VR[- ]BUILD\s+(\S+)|\b(KCD2VR-V\d+\.[A-Za-z0-9_.-]+)')


def family(line):
    m = FAMILY.search(line)
    return (m[1] + (b' '+m[2] if m[2] else b'')).decode('ascii') if m else 'UNVERSIONED'


def build(archives, output):
    if not archives:
        raise ValueError('No log archives found')
    output.mkdir(parents=True, exist_ok=True)
    dbpath = output/'logs.sqlite'
    if dbpath.exists():
        raise ValueError('Use a fresh output directory; do not overwrite evidence.')
    db = sqlite3.connect(dbpath)
    try:
        db.executescript('''
            PRAGMA journal_mode=OFF;
            CREATE TABLE archives(name TEXT PRIMARY KEY,sha256 TEXT,bytes INTEGER);
            CREATE TABLE logs(id INTEGER PRIMARY KEY,archive TEXT,path TEXT,sha256 TEXT,
              bytes INTEGER,lines INTEGER,first_stamp TEXT,last_stamp TEXT,build_markers TEXT,verified INTEGER);
            CREATE TABLE chunks(id INTEGER PRIMARY KEY,log_id INTEGER,first_line INTEGER,last_line INTEGER,data BLOB);
            CREATE TABLE chunk_families(chunk_id INTEGER,family TEXT,count INTEGER);
            CREATE TABLE families(log_id INTEGER,family TEXT,count INTEGER,first_line INTEGER,last_line INTEGER,
              first_text TEXT,last_text TEXT);
        ''')
        inventory = []
        signatures = collections.Counter()
        seen = set()
        declared = {}
        present_names = set()
        for archive in archives:
            archive_hash = hashlib.sha256(archive.read_bytes()).hexdigest()
            db.execute('INSERT INTO archives VALUES(?,?,?)',(archive.name,archive_hash,archive.stat().st_size))
            with zipfile.ZipFile(archive) as z:
                index = list(csv.DictReader(io.StringIO(z.read('INDEX/LOG_INDEX.csv').decode('utf-8-sig'))))
                expected = {row['filename']: row for row in index}
                present=[Path(e.filename).name for e in z.infolist() if not e.is_dir() and e.filename.startswith('LOGS/')]
                if len(expected)!=len(index) or len(present)!=len(set(present)) or not set(present)<=set(expected):
                    raise ValueError('Archive log membership differs from supplied index')
                for name,row in expected.items():
                    signature=(int(row['size_bytes']),row['sha256'].lower())
                    if name in declared and declared[name]!=signature:
                        raise ValueError('Conflicting shared log index: '+name)
                    declared[name]=signature
                present_names.update(present)
                for entry in z.infolist():
                    if entry.is_dir() or not entry.filename.startswith('LOGS/'):
                        continue
                    data = z.read(entry)  # ZipFile validates CRC while decompressing.
                    digest = hashlib.sha256(data).hexdigest()
                    wanted = expected.get(Path(entry.filename).name)
                    if wanted is None or wanted['sha256'].lower()!=digest or int(wanted['size_bytes'])!=len(data):
                        raise ValueError('Log does not match supplied index: '+entry.filename)
                    if digest in seen:
                        raise ValueError('Unexpected duplicate content: '+entry.filename)
                    seen.add(digest)
                    cursor = db.execute('INSERT INTO logs(archive,path,sha256,bytes,verified) VALUES(?,?,?,?,1)',
                                        (archive.name,entry.filename,digest,len(data)))
                    log_id = cursor.lastrowid
                    groups = {}
                    markers = set()
                    first = last = ''
                    number = 0
                    block = []
                    block_groups = collections.Counter()
                    def flush():
                        if not block:
                            return
                        blob = zlib.compress(b''.join(block),6)
                        c = db.execute('INSERT INTO chunks(log_id,first_line,last_line,data) VALUES(?,?,?,?)',
                                       (log_id,number-len(block)+1,number,blob))
                        db.executemany('INSERT INTO chunk_families VALUES(?,?,?)',
                                       [(c.lastrowid,key,count) for key,count in block_groups.items()])
                        block.clear()
                        block_groups.clear()
                    for line in io.BytesIO(data):
                        number += 1
                        key = family(line)
                        stamp = STAMP.match(line)
                        if stamp:
                            last = stamp[1].decode('utf-8',errors='replace')
                            if not first:
                                first = last
                        if b'KCD2VR' in line:
                            for m in BUILD.finditer(line):
                                markers.add((m[1] or m[2]).decode('utf-8',errors='replace'))
                        text = line.decode('utf-8',errors='replace').rstrip('\r\n')
                        if key not in groups:
                            groups[key] = [0,number,number,text,text]
                        g = groups[key]
                        g[0] += 1
                        g[2],g[4] = number,text
                        block.append(line)
                        block_groups[key] += 1
                        if len(block)>=512:
                            flush()
                    flush()
                    db.execute('UPDATE logs SET lines=?,first_stamp=?,last_stamp=?,build_markers=? WHERE id=?',
                               (number,first,last,json.dumps(sorted(markers)),log_id))
                    db.executemany('INSERT INTO families VALUES(?,?,?,?,?,?,?)',
                                   [(log_id,key,*g) for key,g in groups.items()])
                    signatures.update({key:g[0] for key,g in groups.items()})
                    inventory.append({'id':log_id,'archive':archive.name,'path':entry.filename,'sha256':digest,
                                      'bytes':len(data),'lines':number,'first_stamp':first,'last_stamp':last,
                                      'build_markers':sorted(markers),'families':len(groups),'verified':True})
                    db.commit()
                    if len(inventory)%20==0:
                        print(f'Indexed {len(inventory)} logs, {sum(x["bytes"] for x in inventory):,} original bytes',flush=True)
        if present_names!=set(declared):
            raise ValueError('Archive log membership differs from supplied shared index')
        db.executescript('''CREATE INDEX family_lookup ON chunk_families(family,chunk_id);
            CREATE INDEX chunk_log ON chunks(log_id,first_line);
            CREATE INDEX summary_family ON families(family,log_id);''')
        assert db.execute('PRAGMA integrity_check').fetchone()[0]=='ok'
        db.commit()
        # Independently reconstruct every original byte stream from stored chunks.
        for item in inventory:
            digest=hashlib.sha256()
            for (blob,) in db.execute('SELECT data FROM chunks WHERE log_id=? ORDER BY first_line',(item['id'],)):
                digest.update(zlib.decompress(blob))
            assert digest.hexdigest()==item['sha256'],item['path']
    finally:
        db.close()
    (output/'inventory.json').write_text(json.dumps(inventory,indent=2)+'\n',encoding='utf-8')
    (output/'families.json').write_text(json.dumps(dict(signatures.most_common()),indent=2)+'\n',encoding='utf-8')
    summary={'logs':len(inventory),'original_bytes':sum(x['bytes'] for x in inventory),
             'lines':sum(x['lines'] for x in inventory),'families':len(signatures),
             'corpus_bytes':dbpath.stat().st_size,'corpus_sha256':hashlib.sha256(dbpath.read_bytes()).hexdigest(),
             'supplied_index_hashes':'ALL_MATCH','zip_crc':'ALL_PASS','lossless_reconstruction':'ALL_MATCH',
             'time_warning':'Archive/filename dates and embedded timestamps can differ. Embedded markers are not all current build markers.'}
    (output/'summary.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(summary,indent=2),flush=True)


if __name__=='__main__':
    p=argparse.ArgumentParser()
    p.add_argument('--archives',required=True,type=Path)
    p.add_argument('--out',required=True,type=Path)
    args=p.parse_args()
    build(sorted(args.archives.glob('KCD2VR_CODEX_VRIK_LOGS_P*.zip')),args.out)
