"""Hash the supplied reference snapshots; preserve provenance without copying sources."""
import argparse
import hashlib
import json
from pathlib import Path
import zipfile


def sha(data):
    return hashlib.sha256(data).hexdigest()


SOURCES = (
    ('activeragdoll-master.zip', 'https://github.com/adamhynek/activeragdoll', ('src/main.cpp',)),
    ('higgs-master.zip', 'https://github.com/adamhynek/higgs', ('src/hand.cpp', 'src/constraint.cpp')),
    ('DI2_UEVR-main.zip', 'https://github.com/jbusfield/DI2_UEVR', ('scripts/helpers/melee.lua', 'scripts/libs/gestures.lua')),
    ('DeadIsland2VR-main.zip', 'https://github.com/vinionsinho/DeadIsland2VR', ('README.md', 'scripts/libs/gestures.lua')),
    ('SH2_UEVR-main.zip', None, ('scripts/melee.lua', 'scripts/libs/gestures.lua')),
    ('crysis_vrmod-main.zip', 'https://github.com/fholger/crysis_vrmod', ('Code/VR/OpenXRInput.cpp',)),
    ('REFramework-master.zip', 'https://github.com/praydog/REFramework', ('scripts/re2_vr_melee.lua',)),
    ('Re9vr1.2.4-1017-1-2-4-1780522393.zip', None,
     ('autorun/re9_vr_motion.lua', 'autorun/re9_vr_weapons.lua',
      'autorun/re9_vr_holster.lua', 'autorun/re9_vr_interactive.lua')),
)


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--inspirations', type=Path, required=True)
    p.add_argument('--cry-references', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    archives = []
    for archive, upstream, requested in SOURCES:
        path = a.inspirations/archive
        entry = dict(path=str(path.resolve()), bytes=path.stat().st_size,
                     sha256=sha(path.read_bytes()), upstream=upstream, members=[])
        with zipfile.ZipFile(path) as z:
            entry['zip_entries'] = len(z.namelist())
            for suffix in requested:
                matches = [n for n in z.namelist() if '/' in n and n.split('/', 1)[1] == suffix]
                assert len(matches) == 1, (archive, suffix, matches)
                member = matches[0]; raw = z.read(member)
                entry['members'].append(dict(name=member, bytes=len(raw), sha256=sha(raw)))
            if not requested:
                entry['scope'] = 'Archive inventory only; no corresponding melee source located or implementation inferred.'
                entry['source_members'] = [n for n in z.namelist() if Path(n).suffix.lower() in
                                          {'.cpp', '.c', '.h', '.hpp', '.lua', '.cs'}]
        archives.append(entry)
    paths = {
        'LoaderDBA.cpp': 'CryAnimation/LoaderDBA.cpp',
        'ControllerPQ.h': 'CryAnimation/ControllerPQ.h',
        'ControllerOpt.h': 'CryAnimation/ControllerOpt.h',
        'QuatQuantization.h': 'CryAnimation/QuatQuantization.h',
        'CryHeaders.h': 'CryCommon/Cry3DEngine/CGF/CryHeaders.h',
        'CGFContent.h': 'CryCommon/Cry3DEngine/CGF/CGFContent.h',
    }
    references = []
    for name, upstream_path in paths.items():
        raw = (a.cry_references/name).read_bytes()
        references.append(dict(name=name, bytes=len(raw), sha256=sha(raw),
            url='https://raw.githubusercontent.com/ValtoGameEngines/CryEngine/release/Code/CryEngine/'+upstream_path))
    report = dict(scope='Selected source snapshots inspected for design/file-format comparison; no copied runtime ABI.',
                  supplied_archives=archives, public_file_format_references=references,
                  sdk_indices_transplanted=False, third_party_source_in_delivery=False)
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(f'Recorded {len(archives)} supplied archives and {len(references)} public file-format references')
