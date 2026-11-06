"""Read-only asset provenance for the F12=7 protected alias seen in V125R1."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile


def inspect(archive):
    uses = []
    with zipfile.ZipFile(archive) as z:
        definition = 'Mannequin/ADB/kcd_male_controllerdefs.xml'
        raw = z.read(definition)
        root = ET.fromstring(raw)
        scopes = [dict(name=s.tag, **s.attrib) for s in root.find('ScopeDefs')
                  if s.tag in ('UpperBody', 'FacialExpression')]
        definition_hash = hashlib.sha256(raw).hexdigest()
        for name in z.namelist():
            if not name.lower().endswith('.adb'):
                continue
            raw = z.read(name)
            if b'fa_disgust_02' not in raw:
                continue
            fragments = ET.fromstring(raw).find('FragmentList')
            if fragments is None:
                continue
            for entry in fragments:
                for fragment in entry:
                    for index, layer in enumerate(fragment.findall('AnimLayer')):
                        for animation in layer.iter('Animation'):
                            if animation.get('name') == 'fa_disgust_02':
                                uses.append(dict(path=name, sha256=hashlib.sha256(raw).hexdigest(),
                                    fragment=entry.tag, tags=fragment.get('Tags'), guid=fragment.get('GUID'),
                                    layer_index=index, animation=animation.attrib))
    return dict(archive=str(archive.resolve()), alias='fa_disgust_02', uses=uses,
        controller_definition=definition, controller_definition_sha256=definition_hash, scopes=scopes,
        qualification='Shared ordinary/mounted/dialogue alias; name alone does not qualify ownership. '
                      'Layer 7 is not the male FacialExpression scope. This ADB context alone is insufficient. '
                      'The separate research_facial126.py DBA/skeleton track proof qualifies the exact layer-7 exception.')


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('archive', type=Path)
    p.add_argument('output', type=Path)
    a = p.parse_args()
    report = inspect(a.archive)
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(f"Extracted {len(report['uses'])} alias uses; no native ownership inference")
