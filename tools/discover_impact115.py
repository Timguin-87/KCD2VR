"""Extract native impact assets and kick table rows; no engine ABI assumptions."""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile


def discover(game, output):
    provenance = []

    def read(archive, member):
        with zipfile.ZipFile(game / archive) as z:
            data = z.read(member)
        provenance.append(dict(archive=archive, member=member, bytes=len(data),
                               sha256=hashlib.sha256(data).hexdigest()))
        return ET.fromstring(data)

    prefix = 'IPL gamedata.pak/Libs/MaterialEffects/'
    surface = read('IPL gamedata.pak.zip', prefix + 'SurfaceTypes.xml')
    effects = read('IPL gamedata.pak.zip', prefix + 'FXLibs/collisions.xml')
    destructibles = read('IPL gamedata.pak.zip', prefix + 'FXLibs/destructibles.xml')
    workbook = read('IPL gamedata.pak.zip', prefix + 'MaterialEffects.xml')
    ss = '{urn:schemas-microsoft-com:office:spreadsheet}'
    matrices = []
    for sheet in workbook.findall(ss + 'Worksheet'):
        cells, row_index = {}, 1
        for row in sheet.findall(ss + 'Table/' + ss + 'Row'):
            row_index = int(row.get(ss + 'Index', row_index))
            values, column = {}, 1
            for cell in row.findall(ss + 'Cell'):
                column = int(cell.get(ss + 'Index', column))
                data = cell.find(ss + 'Data')
                values[column] = ''.join(data.itertext()).strip() if data is not None else ''
                column += 1
            cells[row_index] = values
            row_index += 1
        header = cells[min(cells)]
        assert header.get(2) == 'mat_flesh' and header.get(9) == 'mat_metal'
        rows = []
        for index, row in cells.items():
            if not row.get(1):
                continue
            rows.append(dict(row=index, label=row[1],
                             effects={header[c]: value for c, value in row.items()
                                      if c > 1 and header.get(c) and value}))
        matrices.append(dict(sheet=sheet.get(ss + 'Name'), rows=rows))

    attacks = read('Data/Tables.pak', 'Libs/Tables/combat/combat_action_attack.xml')
    kicks = [dict(row.attrib) for row in attacks.iter('combat_action_attack')
             if 'kick' in row.get('mn_tags', '').split('+')]
    report = dict(
        provenance=provenance,
        surfaces=[dict(name=s.get('name'), type=s.get('type'),
                       physics=dict(s.find('Physics').attrib) if s.find('Physics') is not None else {})
                  for s in surface.findall('SurfaceType')],
        collision_effects={e.get('name'): ET.tostring(e, encoding='unicode') for e in effects.findall('Effect')},
        destructible_effects={e.get('name'): ET.tostring(e, encoding='unicode') for e in destructibles.findall('Effect')},
        material_effect_matrices=matrices, kick_attack_rows=kicks,
        limits=[
            'Asset names/row numbers are not runtime material IDs or callable ABI.',
            'minscale/maxscale/maxscaledist are asset attributes, not proof of impact-speed scaling.',
            'Kick row existence does not prove player input eligibility outside combat.',
            'A pre-filter collision candidate or particle effect is not accepted damage.',
        ])
    libraries = {'collisions': report['collision_effects'], 'destructibles': report['destructible_effects']}
    references = {v for m in matrices for row in m['rows'] for v in row['effects'].values()}
    report['unresolved_asset_references_in_inspected_libraries'] = sorted(
        v for v in references if ':' in v and v.split(':')[0] in libraries
        and v.split(':', 1)[1] not in libraries[v.split(':')[0]])
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(dict(surfaces=len(report['surfaces']), effects=len(report['collision_effects']),
                          destructible_effects=len(report['destructible_effects']),
                          unresolved_references=len(report['unresolved_asset_references_in_inspected_libraries']),
                          matrix_rows=sum(len(m['rows']) for m in matrices), kick_rows=len(kicks),
                          player_kick_rows=sum(r.get('player') == 'true' for r in kicks)), indent=2))


if __name__ == '__main__':
    p = argparse.ArgumentParser()
    p.add_argument('--game', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    discover(args.game, args.out)
