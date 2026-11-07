"""Restore the pinned bundled MSVC dependency into build output, without downloads.

The archive is transport only. Every restored input keeps the original dependency
manifest hash. Unknown files, caches and private partial libraries are not copied.
"""
from pathlib import Path, PurePosixPath
import argparse, hashlib, json, os, shutil, zipfile

EXPECTED_LIBRARY_SHA = 'c01dfd67d2a72bfb7fc5a8666f6bcd8dad93d530dafe5afcf7fdbcf9f02f6e54'
LIBRARY = 'lib/libDetourModKit.lib'
EXTRA = ('LICENSE', 'ACKNOWLEDGEMENTS.txt', 'CAPACITY128.patch', 'KCD2VR_SOURCE.json')

def sha(path):
    h=hashlib.sha256()
    with Path(path).open('rb') as f:
        for chunk in iter(lambda:f.read(1024*1024),b''):h.update(chunk)
    return h.hexdigest()

def member_bytes(path,row):
    """R133: the reviewed bytes of a text member. a GitHub "Download ZIP" or a clone with other line-ending
    settings gives the same file with LF instead of CRLF (or the other way round). the content is the same, so
    accept it when the CRLF or LF form has the manifest hash, and restore exactly those reviewed bytes."""
    data=Path(path).read_bytes()
    if len(data)==row['bytes'] and hashlib.sha256(data).hexdigest()==row['sha256']:return data
    if b'\0' not in data:
        lf=data.replace(b'\r\n',b'\n')
        for form in (lf.replace(b'\n',b'\r\n'),lf):
            if len(form)==row['bytes'] and hashlib.sha256(form).hexdigest()==row['sha256']:return form
    return None

def atomic_write(data,dst):
    dst.parent.mkdir(parents=True,exist_ok=True)
    temp=dst.with_name(dst.name+'.restore.tmp')
    try:
        temp.write_bytes(data);os.replace(temp,dst)
    finally:
        if temp.exists():temp.unlink()

def relative(name):
    p=PurePosixPath(name)
    if not name or p.is_absolute() or '\\' in name or ':' in name or any(v in ('','..','.') for v in name.split('/')):
        raise ValueError('Unsafe dependency member: '+name)
    return p.as_posix()

def atomic_copy(src,dst):
    dst.parent.mkdir(parents=True,exist_ok=True)
    temp=dst.with_name(dst.name+'.restore.tmp')
    try:
        with Path(src).open('rb') as f,temp.open('wb') as g:shutil.copyfileobj(f,g,1024*1024)
        os.replace(temp,dst)
    finally:
        if temp.exists():temp.unlink()

def prepare(package,out):
    package=Path(package).resolve();out=Path(out).resolve()
    if out==package or out.is_relative_to(package) or package.is_relative_to(out):
        raise ValueError('Dependency output must be separate from the source package')
    manifest_path=package/'DEPENDENCY_MANIFEST.json'
    manifest=json.loads(manifest_path.read_text(encoding='utf-8-sig'))
    if manifest.get('library_sha256')!=EXPECTED_LIBRARY_SHA:
        raise ValueError('Dependency is not the reviewed capacity128 binary')
    rows=manifest['files'];names=[relative(row['path']) for row in rows]
    if len(names)!=len(set(n.casefold() for n in names)) or LIBRARY not in names:
        raise ValueError('Duplicate dependency member or missing library')
    lib=next(row for row in rows if row['path']==LIBRARY)
    if lib['sha256']!=EXPECTED_LIBRARY_SHA:raise ValueError('Library pin mismatch')
    compressed=json.loads((package/'COMPRESSED_LIBRARY.json').read_text(encoding='utf-8-sig'))
    if compressed.get('schema')!=1 or compressed.get('member')!=LIBRARY or compressed.get('sha256')!=EXPECTED_LIBRARY_SHA or compressed.get('bytes')!=lib['bytes']:
        raise ValueError('Compressed dependency receipt differs from original inputs')
    archive=package/relative(compressed['archive'])
    if sha(archive)!=compressed['archive_sha256']:raise ValueError('Compressed dependency archive hash mismatch')
    # Verify every source member before changing output. Preserve attribution.
    members={}
    for row in rows:
        if row['path']==LIBRARY:continue
        data=member_bytes(package/row['path'],row)
        if data is None:
            raise ValueError('Dependency source member hash mismatch: '+row['path'])
        members[row['path']]=data
    for name in EXTRA:
        if not (package/name).is_file():raise ValueError('Missing dependency provenance: '+name)
    with zipfile.ZipFile(archive) as z:
        info=z.infolist()
        if len(info)!=1 or info[0].filename!=LIBRARY or info[0].file_size!=lib['bytes']:
            raise ValueError('Unexpected compressed dependency member')
        target=out/LIBRARY
        if not target.is_file() or target.stat().st_size!=lib['bytes'] or sha(target)!=EXPECTED_LIBRARY_SHA:
            target.parent.mkdir(parents=True,exist_ok=True);temp=target.with_name(target.name+'.restore.tmp')
            try:
                with z.open(info[0]) as src,temp.open('wb') as dst:shutil.copyfileobj(src,dst,1024*1024)
                if sha(temp)!=EXPECTED_LIBRARY_SHA:raise ValueError('Restored dependency library hash mismatch')
                os.replace(temp,target)
            finally:
                if temp.exists():temp.unlink()
    for row in rows:
        if row['path']==LIBRARY:continue
        dst=out/row['path']
        if not dst.is_file() or sha(dst)!=row['sha256']:atomic_write(members[row['path']],dst)
    for name in (*EXTRA,'DEPENDENCY_MANIFEST.json','COMPRESSED_LIBRARY.json'):
        src=package/name;dst=out/name
        if not dst.is_file() or sha(dst)!=sha(src):atomic_copy(src,dst)
    # Recheck the actual restored input closure, never infer it from timestamps.
    for row in rows:
        if sha(out/row['path'])!=row['sha256']:raise ValueError('Restored dependency verification failed: '+row['path'])
    return out

if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--package',type=Path,required=True);p.add_argument('--out',type=Path,required=True)
    a=p.parse_args();result=prepare(a.package,a.out)
    print('Verified offline dependency restored: '+str(result))
