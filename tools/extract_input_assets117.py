"""Extract the retail table rows used to review physical input selection."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--tables', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    assets = []
    with zipfile.ZipFile(a.tables) as z:
        for name in ('combat_input_class', 'combat_zone', 'combat_attack_config', 'combat_zone_mapping'):
            member = 'Libs/Tables/combat/' + name + '.xml'
            data = z.read(member)
            assets.append(dict(member=member, sha256=hashlib.sha256(data).hexdigest(),
                               rows=[dict(e.attrib) for e in ET.fromstring(data).iter(name)]))
    result = dict(archive=str(a.tables.resolve()), archive_sha256=hashlib.sha256(a.tables.read_bytes()).hexdigest(),
                  assets=assets, limitation='Authored configuration, not live accepted attacks or physical contacts.')
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(assets=len(assets), rows=sum(len(x['rows']) for x in assets))))


if __name__ == '__main__': main()
