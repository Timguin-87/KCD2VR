"""Package relative-path build verifier beside the freshly built DLL.

Called by the release integrator AFTER its own build, with its current source.
Never installs or edits game files/INI. Rejects a DLL with an older marker.
"""
import argparse,hashlib,json,re,shutil,zipfile,os
from verify_single_binary_b1 import Binary
from pathlib import Path
from companion_sources_d39 import SOURCE_PAIRS, ALLOWED_ADDITIONS
from optional_gore_d74 import stage_external_optional, validate_optional
from optional_gore_r93 import prepare_current_optional


def prepare_companion(source,out,optional_gore_source=None):
    base=source/'companion/BASE_GAMEPLAY/Mods/KCD2VR';manifest=base/'KCD2VR_UNIFIED63.json';pak=base/'Data/KCD2VR.pak'
    source=source.resolve();out=out.resolve()
    if out==source or source.is_relative_to(out) or out.is_relative_to(base.resolve()):
        raise SystemExit('Companion output must be a separate build directory')
    sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
    meta=json.loads(manifest.read_text(encoding='utf-8-sig'));parent=sha(pak)
    if parent!=meta['pak_sha256']:raise SystemExit('Source companion PAK does not match its inventory')
    for name,expected in meta['distribution_files'].items():
        if sha(base/name)!=expected:raise SystemExit('Source companion distribution mismatch: '+name)
    gore_names={r['path'] for r in meta['members_provenance'] if r['source'].lower().startswith('kcd2vr_gore')}
    if gore_names:raise SystemExit('Gameplay source seed unexpectedly contains adult content')
    replacements={member:source/path for path,member in SOURCE_PAIRS.items() if member not in ALLOWED_ADDITIONS}
    additions={member:source/path for path,member in SOURCE_PAIRS.items() if member in ALLOWED_ADDITIONS}
    # D33: the baked chest sheath model joins the pak only once it exists (tools/cgf_scale_d33.py output); no model = Lua says so
    sheath=source/'companion/source/Objects/kcd2vr/chest_sheath_d33'
    if sheath.is_dir():
        for f in sorted(sheath.iterdir()):
            if f.suffix.lower() in ('.cgf','.cgfm','.mtl'):additions['Objects/kcd2vr/chest_sheath_d33/'+f.name]=f
    # M41: native static paper + baked first-UV artwork, current source assets.
    # No separate map PAK; the source closure covers every added resource.
    paper=source/'companion/source/Objects/kcd2vr/staticmap40'
    if not paper.is_dir():raise SystemExit('M41 static paper assets missing from source')
    static_files=sorted(paper.iterdir())
    if len(static_files)!=104:raise SystemExit('M41 static paper resource inventory incomplete')
    for f in static_files:
        if f.suffix.lower() not in ('.cgf','.cgfm','.mtl','.dds'):raise SystemExit('Unexpected static paper resource: '+f.name)
        additions['Objects/kcd2vr/staticmap40/'+f.name]=f
    # D64 bounded static POI batches and player glyph use the same utility slots.
    markers=source/'companion/source/Objects/kcd2vr/mapmarkers64'
    if markers.is_dir():
        for f in sorted(markers.iterdir()):
            if f.suffix.lower() not in ('.cgf','.cgfm','.mtl','.dds'):raise SystemExit('Unexpected D64 marker asset')
            additions['Objects/kcd2vr/mapmarkers64/'+f.name]=f
    quests=source/'companion/source/Objects/kcd2vr/mapquests75'
    if not quests.is_dir():raise SystemExit('D75 native quest presentation assets missing')
    quest_files=sorted(quests.iterdir())
    if len(quest_files)!=54:raise SystemExit('D75 quest presentation inventory incomplete')
    for f in quest_files:
        if f.suffix.lower() not in ('.cgf','.cgfm','.mtl'):raise SystemExit('Unexpected D75 quest asset')
        additions['Objects/kcd2vr/mapquests75/'+f.name]=f
    sources={**replacements,**additions}
    payload={name:path.read_bytes() for name,path in sources.items()}
    marker_match=re.search(r'#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"',(source/'src/build_identity.h').read_text())
    if not marker_match:raise SystemExit('Current source build identity is missing')
    marker=marker_match.group(1)
    optional_receipt=prepare_current_optional(source,out,optional_gore_source,marker) if optional_gore_source else None
    selection={'schema':1,'optional_gore':optional_receipt is not None}
    out.mkdir(parents=True,exist_ok=True)
    selection_path=out/'COMPANION_SELECTION.json';selection_tmp=selection_path.with_suffix('.json.tmp')
    selection_tmp.write_text(json.dumps(selection,indent=2)+'\n');os.replace(selection_tmp,selection_path)
    closure={'build_marker':marker,'parent_pak_sha256':parent,
             'replacements':{n:hashlib.sha256(payload[n]).hexdigest() for n in replacements},
             'additions':{n:hashlib.sha256(payload[n]).hexdigest() for n in additions},
             'source_paths':{n:p.relative_to(source).as_posix() for n,p in sources.items()}}
    with zipfile.ZipFile(pak) as zin:
        names=zin.namelist();folded={n.casefold():n for n in names}
        if len(folded)!=len(names):raise SystemExit('Case-colliding/duplicate companion PAK members')
        if not set(replacements)<=set(names):raise SystemExit('Required existing companion script entry missing')
        for name in payload:
            if name.casefold() in folded and folded[name.casefold()]!=name:
                raise SystemExit('Case-colliding companion source override: '+name)
        provenance={row['path']:row for row in meta['members_provenance']}
        if len(provenance)!=len(meta['members_provenance']) or set(provenance)!=set(names) or meta['members']!=len(names):
            raise SystemExit('Source companion member inventory mismatch')
    # D74: main and optional gore are disjoint; preserve donor provenance for rebuilds.
    meta['members_provenance']=[r for r in meta['members_provenance'] if r['path'] not in gore_names]
    closure['optional_gore_members']=0 # Gameplay PAK never receives optional adult members.
    expected_member_hashes={name:row['sha256'] for name,row in provenance.items() if name not in gore_names}
    expected_member_hashes.update({name:hashlib.sha256(data).hexdigest() for name,data in payload.items()})
    record=out/'patch2_companion_inputs.json';dest=out/'Mods/KCD2VR';target=dest/'Data/KCD2VR.pak'
    if record.is_file() and target.is_file():
        # A matching PAK alone cannot validate stale/corrupt staged metadata or
        # distribution files. Rebuild the owned package if any cached member
        # differs; the later staging gate still refuses unrelated extra files.
        try:
            previous=json.loads(record.read_text())
            cached=json.loads((dest/'KCD2VR_UNIFIED63.json').read_text())
            cached_distribution=cached.get('distribution_files',{})
            cache_ok=(previous.get('inputs')==closure and
                      sha(target)==previous.get('pak_sha256')==cached.get('pak_sha256') and
                      cached.get('patch2_source_overrides')==closure and
                      cached.get('version')==63 and cached.get('candidate')==marker and
                      cached.get('members')==len(expected_member_hashes) and
                      len(cached.get('members_provenance',[]))==len(expected_member_hashes) and
                      {row['path']:row['sha256'] for row in cached.get('members_provenance',[])}==expected_member_hashes and
                      cached_distribution==meta['distribution_files'] and
                      all((dest/name).is_file() and sha(dest/name)==digest
                          for name,digest in cached_distribution.items()))
        except (OSError, ValueError, KeyError, TypeError):
            cache_ok=False
        if cache_ok:
            print('Matching current-source companion already prepared: '+previous['pak_sha256']);return
    if out.resolve()==source.resolve() or out.resolve()==base.resolve():raise SystemExit('Companion output must be a separate build directory')
    dest.mkdir(parents=True,exist_ok=True)
    for name in meta['distribution_files']:
        p=dest/name;p.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(base/name,p)
    target.parent.mkdir(parents=True,exist_ok=True);temp=target.with_suffix('.pak.tmp')
    with zipfile.ZipFile(pak) as zin,zipfile.ZipFile(temp,'w') as zout:
        names=set(zin.namelist())-gore_names
        for entry in zin.infolist():
            if entry.filename in gore_names:continue
            data=zin.read(entry)
            if hashlib.sha256(data).hexdigest()!=provenance[entry.filename]['sha256']:raise SystemExit('Main donor member hash mismatch: '+entry.filename)
            zout.writestr(entry,payload.get(entry.filename,data))
        for name in additions:
            if name not in names:
                entry=zipfile.ZipInfo(name,(1980,1,1,0,0,0));entry.compress_type=zipfile.ZIP_DEFLATED
                zout.writestr(entry,payload[name])
    with zipfile.ZipFile(temp) as z:
        if z.testzip() is not None:raise SystemExit('Generated companion CRC failed')
        for name,data in payload.items():
            if z.read(name)!=data:raise SystemExit('Generated companion source closure failed: '+name)
    os.replace(temp,target);digest=sha(target)
    for member in meta['members_provenance']:
        if member['path'] in payload:
            member['sha256']=hashlib.sha256(payload[member['path']]).hexdigest()
            member['source']='Patch2 exact source override: '+closure['source_paths'][member['path']]
    for name in additions:
        if name not in names:
            meta['members_provenance'].append({'path':name,'sha256':closure['additions'][name],
                'source':'Patch2 exact source addition: '+closure['source_paths'][name]})
    meta['members']=len(names|set(additions))
    meta['pak_sha256']=digest;meta['patch2_source_overrides']=closure;meta['headset']='NOT_RUN';meta['candidate']=marker
    (dest/'KCD2VR_UNIFIED63.json').write_text(json.dumps(meta,indent=2)+'\n')
    record.write_text(json.dumps({'inputs':closure,'pak_sha256':digest,'headset':'NOT_RUN'},indent=2)+'\n')
    print('Prepared main companion for '+marker+' (optional gore excluded): '+digest)

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1])
    ap.add_argument('--binary',type=Path)
    ap.add_argument('--prepare-companion',action='store_true')
    ap.add_argument('--optional-gore-source',type=Path,help='Explicit standalone optional ZIP or package root; omitted for main-only builds')
    ap.add_argument('--out',type=Path,required=True)
    args=ap.parse_args();source=args.source.resolve();out=args.out.resolve()
    if args.prepare_companion:prepare_companion(source,out,args.optional_gore_source);return
    if args.binary is None:raise SystemExit('--binary is required for DLL packaging')
    marker=re.search(r'#define\s+KCD2VR_BUILD_MARKER\s+"([^"]+)"',(source/'src/build_identity.h').read_text()).group(1)
    if marker.encode('ascii') not in args.binary.read_bytes():raise SystemExit('DLL does not match the current source marker')
    out.mkdir(parents=True,exist_ok=True)
    binary_name=args.binary.name
    launcher=args.binary.parent/'Start KCD2VR.cmd'
    if 'KCD2VRSetupW' not in Binary(args.binary).exports():raise SystemExit('Actual binary has no desktop setup export')
    if not launcher.is_file() or ('%~dp0'+binary_name).encode() not in launcher.read_bytes():raise SystemExit('Matching staged setup launcher missing or targets another binary')
    if args.binary.resolve()!=(out/binary_name).resolve():shutil.copyfile(args.binary,out/binary_name)
    if launcher.resolve()!=(out/launcher.name).resolve():shutil.copyfile(launcher,out/launcher.name)
    files=[binary_name,'Start KCD2VR.cmd','VERIFY_BUILD.cmd','tools/verify_release_payload.ps1']
    for name in files[2:]:
        target=out/name;target.parent.mkdir(parents=True,exist_ok=True)
        if (source/name).resolve()!=target.resolve():shutil.copyfile(source/name,target)
    staged=args.binary.parent/'companion/Mods/KCD2VR';staged_meta=json.loads((staged/'KCD2VR_UNIFIED63.json').read_text())
    if hashlib.sha256((staged/'Data/KCD2VR.pak').read_bytes()).hexdigest()!=staged_meta['pak_sha256']:raise SystemExit('Staged DLL companion identity mismatch')
    if (out/'Mods/KCD2VR').resolve()!=staged.resolve():shutil.copytree(staged,out/'Mods/KCD2VR',dirs_exist_ok=True)
    selection=json.loads((args.binary.parent/'companion/COMPANION_SELECTION.json').read_text())
    if selection.get('schema')!=1 or not isinstance(selection.get('optional_gore'),bool):raise SystemExit('Invalid companion selection receipt')
    optional=args.binary.parent/'companion/OptionalGore'
    if selection['optional_gore']:
        validate_optional(optional/'Mods/kcd2vr_gore')
        if (out/'OptionalGore').resolve()!=optional.resolve():stage_external_optional(optional,out)
        validate_optional(out/'OptionalGore/Mods/kcd2vr_gore')
    elif (out/'OptionalGore').exists():
        raise SystemExit('Old optional content remains in release output; use a clean output folder for a main-only package')
    files += ['Mods/KCD2VR/Data/KCD2VR.pak','Mods/KCD2VR/KCD2VR_UNIFIED63.json']
    manifest={'build_marker':marker,'binary':binary_name,'startup_launcher':'Start KCD2VR.cmd','artifacts':[{'path':n,'sha256':hashlib.sha256((out/n).read_bytes()).hexdigest()} for n in files]}
    manifest['optional_artifacts']=[{'path':'OptionalGore/'+p.relative_to(out/'OptionalGore').as_posix(),'sha256':hashlib.sha256(p.read_bytes()).hexdigest()} for p in sorted((out/'OptionalGore').rglob('*')) if p.is_file()] if selection['optional_gore'] else []
    (out/'BUILD_MANIFEST.json').write_text(json.dumps(manifest,indent=2))
    print('Packaged matching portable VERIFY_BUILD.cmd:',marker)

if __name__=='__main__':main()
