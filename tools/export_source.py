#!/usr/bin/env python3
"""Export buildable public source without private history or optional gore payloads."""
from pathlib import Path
import argparse, hashlib, json, os, re, shutil, subprocess, zipfile

ROOT_FILES = {'.gitattributes', '.gitignore', 'README.md', 'CONTRIBUTING.md',
              'CMakeLists.txt', 'KCD2VR.ini', 'subsystems.json', 'build_release.bat',
              'Start KCD2VR.cmd.in', 'VERIFY_BUILD.cmd'}
TREES = {'src','tests','tools','cmake','data','assets','deps','mods','companion'}
DOCS = {'BUILDING.md','DEPENDENCIES.md','DEVELOPMENT.md','DIAGNOSTICS.md',
        'KNOWN_ISSUES.md','SETTINGS.md','VALIDATION.md','TESTING.md','CHANGES_D79.md','CHANGES_D80.md','CHANGES_D81.md','CHANGES_D81R2.md','CHANGES_D82.md','TEMPORAL_STEREO_D82.md','TEMPORAL_COMPOSITION_D82.md','CROSSBOW_D82_REPAIR.md','SMITH_COMBAT_D82.md','VALIDATION_D82.md','STEREO_UPSCALER_D82_UI_TESTS.md','VALIDATION_D80.md','LAUNCHER_OPTIONS.md'}
PRIVATE_PREFIXES = ('companion/D73_UNIFIED_BASE/','companion/C63_UNIFIED/',
                    'companion/OptionalGore','prepared_', 'repair_evidence/', 'review/')
DOCS.update({'CHANGES_D83R1.md','VALIDATION_D83R1.md','UPSCALER_D83_REVIEW.md',
             'CROSSBOW_D83_REVIEW.md','SMITH_D83R1_REVIEW.md',
             'FINISHER_D83R1_REVIEW.md','COMBAT_D83_LOG_AUDIT.md'})
DOCS.add('D84R1_MASTERPLAN_STATUS.md')
DOCS.update({'R93_REVIEW.md','R93R3_REVIEW.md','R93R4_BUILD.md','R93R4_REPAIR_R1.md','CROSSBOW_CURRENT_BEHAVIOR.txt'})
DOCS.add('R94R1_REVIEW.md')
DOCS.update({'R95_REPAIRS.md','R95R1_CROSSBOW_BOLT.md','R96_REPAIR_REVIEW.md','R96_VALIDATION.json'})
DOCS.update({'R96R1_REPAIR_REVIEW.md','R96R1_VALIDATION.json'})
DOCS.update({'R105_REPAIR_REVIEW.md','R105_VALIDATION.json','R105_RESTORED_TESTS.json'})
DOCS.update({'D85R5R3_FRESH_POSE_UNCAPPED.md','D85R5R2_CORRECTIVE_REVIEW.md','D85R5R1_REPAIR_STATUS.md'})
DOCS.update({'CHANGES_D84.md','VALIDATION_D84.md','REGRESSION_LEDGER_D84.md',
             'INTERACTIONS_D84_REVIEW.md','COMBAT_SMITH_KICKS_D84_REVIEW.md',
             'MOTION_PHYSICS_D84_REVIEW.md'})
OMIT_FILES = {'deps/DetourModKit/lib/libDetourModKit.lib',
              'tests/build_dependency_selection.ps1', 'tests/test_source_archive63.py',
              'src/ACTIVE_OWNERSHIP_MAP.md', 'tools/prepare_gore_d56.py',
              'tools/compare_frik_syntax_d41.py', 'tools/summarize_d60_validation.py',
              'tools/verify_d59_closure.py','tools/verify_d60_closure.py','tools/verify_d61_closure.py'}
KEEP_PACKAGE_TOOLS = {'package_release_helpers_r19.py'}

def eligible(name):
    p=Path(name);parts=p.parts
    if name in ROOT_FILES:return True
    if parts[0]=='docs':return len(parts)==2 and p.name in DOCS
    if parts[0] not in TREES:return False
    if name in OMIT_FILES or name.startswith(PRIVATE_PREFIXES):return False
    if name.startswith('mods/KCD2VRGore/') and p.suffix.lower()=='.pak':return False # optional generated asset payload
    if any(x in {'.git','__pycache__','.pytest_cache','CMakeFiles','dist','delivery'} or x.startswith('build-') for x in parts):return False
    if any(x=='build' for x in parts):return False
    if p.name.startswith('.KCD2VR.pak.') or p.name.endswith('.d79-tmp'):return False
    if p.suffix.lower() in {'.obj','.pdb','.ilk','.pch','.exe','.dll','.pyc','.log','.partial'}:return False
    if p.name in {'CMakeCache.txt','cmake_install.cmake'} or p.name.startswith('.libDetourModKit.lib.'):return False
    if parts[0]=='tools' and (p.name.startswith('seal_') or
       (p.name.startswith('package_') and p.name not in KEEP_PACKAGE_TOOLS)):return False
    return True

def digest(path):
    h=hashlib.sha256()
    with path.open('rb') as f:
        for b in iter(lambda:f.read(1024*1024),b''):h.update(b)
    return h.hexdigest()

def source_files(root):
    result=[];folded=set()
    for p in sorted(root.rglob('*')):
        if not p.is_file():continue
        name=p.relative_to(root).as_posix()
        if not eligible(name):continue
        if p.is_symlink():raise ValueError('Source symlink requires explicit review: '+name)
        if name.casefold() in folded:raise ValueError('Case-colliding archive path: '+name)
        if p.stat().st_size>=100*1024*1024:raise ValueError('Oversized public source input: '+name)
        folded.add(name.casefold());result.append((name,p))
    required=ROOT_FILES|{'tools/prepare_dmk_dependency.py','tools/package_release_helpers_r19.py',
       'deps/DetourModKit/lib/libDetourModKit.lib.zip',
       'companion/BASE_GAMEPLAY/Mods/KCD2VR/Data/KCD2VR.pak'}
    missing=required-{n for n,_ in result}
    if missing:raise ValueError('Missing required inputs: '+str(sorted(missing)))
    return result

def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
    ap.add_argument('--out',type=Path,help='New empty independent source directory')
    ap.add_argument('--zip',type=Path,help='New source ZIP')
    ap.add_argument('--commit',default='',help='Exact private source commit, recorded as provenance')
    args=ap.parse_args();root=args.source.resolve()
    if not(args.out or args.zip):ap.error('Choose --out or --zip')
    files=source_files(root)
    rows=[{'path':n,'bytes':p.stat().st_size,'sha256':digest(p)} for n,p in files]
    closure=hashlib.sha256(''.join(r['path']+'\0'+r['sha256']+'\n' for r in rows).encode()).hexdigest()
    marker=re.search(r'#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"',(root/'src/build_identity.h').read_text()).group(1)
    commit=args.commit
    if not commit:
        try:commit=subprocess.check_output(['git','-C',str(root),'rev-parse','HEAD'],stderr=subprocess.DEVNULL,text=True).strip()
        except (OSError,subprocess.CalledProcessError):commit='unavailable'
    manifest={'schema':1,'build_marker':marker,'source_commit':commit,
      'source_files_closure_sha256':closure,'files':rows,
      'excluded':'Private history, old deliveries, raw logs, intermediate binaries and optional gore payloads. Required dependency notices remain.',
      'self_hash_excluded':True}
    raw=(json.dumps(manifest,indent=2)+'\n').encode()
    if args.out:
        out=args.out.resolve()
        if out==root or root in out.parents:ap.error('--out must be outside source')
        if out.exists() and any(out.iterdir()):ap.error('--out must be empty; existing files are never removed')
        out.mkdir(parents=True,exist_ok=True)
        for name,p in files:
            target=out/name;target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,target)
            if digest(target)!=next(r['sha256'] for r in rows if r['path']==name):raise ValueError('Copy mismatch: '+name)
        (out/'EXPORT_MANIFEST.json').write_bytes(raw)
    if args.zip:
        zpath=args.zip.resolve()
        if zpath.exists():ap.error('ZIP already exists; choose a new output')
        zpath.parent.mkdir(parents=True,exist_ok=True)
        prefix='KCD2VR_SOURCE/'
        with zipfile.ZipFile(zpath,'x',zipfile.ZIP_DEFLATED,compresslevel=6) as z:
            for name,p in files:z.write(p,prefix+name)
            z.writestr(prefix+'EXPORT_MANIFEST.json',raw)
        with zipfile.ZipFile(zpath) as z:
            if z.testzip():raise ValueError('Archive CRC failure')
            for row in rows:
                if hashlib.sha256(z.read(prefix+row['path'])).hexdigest()!=row['sha256']:raise ValueError('ZIP mismatch: '+row['path'])
        print(json.dumps({'zip':str(zpath),'bytes':zpath.stat().st_size,'sha256':digest(zpath)}))
    print(json.dumps({'files':len(files),'uncompressed_bytes':sum(r['bytes'] for r in rows),'source_files_closure_sha256':closure,'source_commit':commit}))

if __name__=='__main__':main()
