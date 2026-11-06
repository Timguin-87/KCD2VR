"""Verify canonical source bytes against the embedded distribution and PAK.
No newline normalization, hash exceptions or game installation are permitted.
"""
from pathlib import Path
import argparse, hashlib, io, json, os, shutil, subprocess, tempfile, zipfile
ROOT=Path(__file__).resolve().parents[1]
PACKAGE='companion/C63_UNIFIED/'
from companion_sources_d39 import SOURCE_PAIRS
PAIRS=SOURCE_PAIRS

def sha(data):return hashlib.sha256(data).hexdigest()
def verify_source(entries):
    base=PACKAGE+'Mods/KCD2VR/'
    meta=json.loads(entries[base+'KCD2VR_UNIFIED63.json'])
    if meta.get('version')!=63 or not meta.get('distribution_files'):raise ValueError('Missing C63 distribution inventory')
    allowed={'Data/KCD2VR.pak':meta['pak_sha256'],'KCD2VR_UNIFIED63.json':None}
    for name,digest in meta['distribution_files'].items():
        if name.startswith('/') or '..' in name.split('/') or chr(92) in name or ':' in name or name in allowed:raise ValueError('Unsafe/duplicate inventory path '+name)
        allowed[name]=digest
    actual={n[len(base):] for n in entries if n.startswith(base)}
    if actual!=set(allowed):raise ValueError('Incomplete/extra source distribution members: '+str(actual.symmetric_difference(allowed)))
    if len({n.casefold() for n in actual})!=len(actual):raise ValueError('Case-colliding distribution paths')
    for name,digest in allowed.items():
        if digest and sha(entries[base+name])!=digest:raise ValueError('Distribution member hash mismatch: '+name)
    pak=entries[base+'Data/KCD2VR.pak']
    with zipfile.ZipFile(io.BytesIO(pak)) as z:
        names=[i.filename for i in z.infolist() if not i.is_dir()]
        if z.testzip() or len({n.casefold()for n in names})!=len(names):raise ValueError('Invalid/duplicate embedded PAK')
        if len(names)!=meta['members']:raise ValueError('Embedded PAK count mismatch')
        provenance={row['path']:row['sha256']for row in meta['members_provenance']}
        if set(names)!=set(provenance):raise ValueError('PAK provenance members differ')
        for name in names:
            if sha(z.read(name))!=provenance[name]:raise ValueError('PAK member hash mismatch: '+name)
        for source,member in PAIRS.items():
            if member not in names:raise ValueError('Current source member missing; prepare the donor package first: '+member)
            if entries[source]!=z.read(member):raise ValueError('Source/PAK byte mismatch: '+source)
    if entries['tools/install_unified_companion63.ps1']!=entries[PACKAGE+'install_unified_companion63.ps1']:raise ValueError('Source/delivered installer bytes differ')
    return {'distribution_members':len(actual),'pak_members':len(names),'source_pak_pairs':len(PAIRS),'byte_exact':True,'game_installed':False}
def read_source_zip(path):
    with zipfile.ZipFile(path)as z:
        prefix='KCD2VR_C63/'
        return {n[len(prefix):]:z.read(n)for n in z.namelist()if n.startswith(prefix) and not n.endswith('/') and
          (n[len(prefix):].startswith(PACKAGE) or n[len(prefix):]in PAIRS or n[len(prefix):]=='tools/install_unified_companion63.ps1')}
def actual_windows_checks(entries,cache,stage_script):
    shell=shutil.which('pwsh')or shutil.which('powershell')
    if not shell:raise RuntimeError('PowerShell required for actual installer validation')
    cmake=shutil.which('cmake')or r'C:/Program Files/CMake/bin/cmake.exe'
    cache=Path(cache).resolve();cache.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='source63-',dir=cache)as child:
        temp=Path(child).resolve()
        if temp.parent!=cache:raise ValueError('Unexpected disposable verification path')
        package=temp/'source'
        for name,data in entries.items():
            if name.startswith(PACKAGE):
                p=package/name[len(PACKAGE):];p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(data)
        def leaf(where):
            p=subprocess.run([shell,'-NoProfile','-File',str(ROOT/'tools/verify_companion_installer63.ps1'),'-PackageRoot',str(where)],capture_output=True,text=True)
            if p.returncode:raise ValueError(p.stdout+p.stderr)
            return p.stdout.strip()
        first=leaf(package)
        target=temp/'staged'
        p=subprocess.run([cmake,'-DSOURCE_ROOT='+str(package),'-DTARGET_ROOT='+str(target),'-P',str(stage_script)],capture_output=True,text=True)
        if p.returncode:raise ValueError(p.stdout+p.stderr)
        second=leaf(target)
        files=[p for p in package.rglob('*')if p.is_file()]
        if not all((target/p.relative_to(package)).read_bytes()==p.read_bytes()for p in files):raise ValueError('Actual CMake staged bytes differ')
        return {'actual_source_validator':first,'actual_staged_validator':second,'cmake_stage':'PASS','staged_files':len(files),'installer_main_executed':False,'game_writes':0}
if __name__=='__main__':
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--zip',required=True,type=Path);ap.add_argument('--cache',type=Path);ap.add_argument('--out',type=Path);a=ap.parse_args()
    entries=read_source_zip(a.zip);result=verify_source(entries)
    if a.cache:
        with zipfile.ZipFile(a.zip)as z:
            a.cache.mkdir(parents=True,exist_ok=True);script=a.cache/'stage_companion63_from_source.cmake'
            script.write_bytes(z.read('KCD2VR_C63/cmake/stage_companion63.cmake'))
        result['actual']=actual_windows_checks(entries,a.cache,script)
    if a.out:a.out.write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
