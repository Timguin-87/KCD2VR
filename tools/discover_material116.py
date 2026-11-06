"""Join retail material effects to audio events and particle emitters, offline.

Asset assignments are not a record of what played in a headset session, and XML
row numbers / FMOD editor IDs are not native material IDs or callable handles.
"""
import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET
import zipfile


def particle_tree(node):
    params = node.find('Params')
    return dict(name=node.get('Name'),
                params=dict(params.attrib) if params is not None else {},
                children=[particle_tree(c) for c in node.findall('Childs/Particles')])


def discover(game, impact_report, output):
    assets = json.loads(impact_report.read_text(encoding='utf-8'))
    provenance = list(assets['provenance'])
    # Refuse to join a stale extraction against different source assets.
    for source in provenance:
        with zipfile.ZipFile(game / source['archive']) as archive:
            data = archive.read(source['member'])
        if hashlib.sha256(data).hexdigest() != source['sha256']:
            raise ValueError('Stale impact extraction: ' + source['member'])
    audio, particles = {}, {}
    archive_name = 'IPL gamedata.pak.zip'
    with zipfile.ZipFile(game / archive_name) as archive:
        members = [n for n in archive.namelist()
                   if n.startswith('IPL gamedata.pak/Libs/GameAudio/') and n.endswith('.xml')]
        members.append('IPL gamedata.pak/Libs/Particles/collisions.xml')
        for member in sorted(members):
            data = archive.read(member)
            root = ET.fromstring(data)
            records = list(root.iter('ATLTrigger'))
            relevant = bool(records) or root.tag == 'ParticleLibrary'
            if relevant:
                provenance.append(dict(archive=archive_name, member=member,
                                       bytes=len(data), sha256=hashlib.sha256(data).hexdigest()))
            for trigger in records:
                audio.setdefault(trigger.get('atl_name'), []).append(dict(
                    member=member, attributes=dict(trigger.attrib),
                    implementations=[dict(tag=e.tag, attributes=dict(e.attrib)) for e in trigger]))
            if root.tag == 'ParticleLibrary':
                for particle in root.findall('Particles'):
                    particles[root.get('Name') + '.' + particle.get('Name')] = particle_tree(particle)

    libraries = dict(collisions=assets['collision_effects'], destructibles=assets['destructible_effects'])
    effects = {}
    for library, entries in libraries.items():
        for name, raw in entries.items():
            node = ET.fromstring(raw)
            sounds = [e.get('trigger') for e in node.iter('Audio') if e.get('trigger')]
            names = [(e.text or '').strip() for e in node.findall('.//Particle/Name')]
            effects[library + ':' + name] = dict(
                audio=[dict(trigger=t, definitions=audio.get(t, [])) for t in sounds],
                particles=[dict(name=n, definition=particles.get(n)) for n in names],
                decals=[(e.text or '').strip() for e in node.findall('.//Decal/Material')],
                flowgraphs=[dict(e.attrib) for e in node.iter('FlowGraph')],
                source_xml=raw)
    rows = [dict(sheet=m['sheet'], **r) for m in assets['material_effect_matrices']
            for r in m['rows'] if r['label'].startswith(('hit_', 'kick_', 'punch_'))]
    reverse = {}
    for row in rows:
        for material, effect in row['effects'].items():
            reverse.setdefault(effect, []).append(dict(row=row['label'], surface=material))
    report = dict(provenance=provenance, surfaces=assets['surfaces'], impact_rows=rows,
                  effects=effects, reverse_effect_assignments=reverse,
                  limits=[
                      'Assignments, not observed playback. Current F3 logs contain neither an effect ID nor an audio trigger.',
                      'Effects are many-to-one: do not infer a unique material from a sound or particle name.',
                      'Player/NPC, weapon class, stab/slash and passed/stopped rows remain distinct.',
                      'EmitterStrength curves exist but the native impact-speed-to-strength input is not established.',
                      'No material-ID lookup, FX call ABI, damage or native trajectory write is established by this extraction.',
                      'Particle collision/friction/elasticity settings govern debris, not Henry\'s weapon.',
                  ])
    output.mkdir(parents=True, exist_ok=True)
    (output / 'material_effect_chains.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    lines = ['# Retail impact sound and particle discovery — V66.116', '',
             'Reproducible offline extraction from the supplied retail assets. These are configured assignments, not proof of playback in a particular logged hit.', '',
             '| Player sword contact (passed slash row) | Effect | Audio trigger → FMOD event | Particle |',
             '|---|---|---|---|']
    sword = next(r for r in rows if r['label'] == 'hit_passed_sword_slash_player')
    for material in ['mat_wood', 'mat_rock', 'mat_metal', 'mat_door_metal', 'mat_shield_wood']:
        ref = sword['effects'][material]
        effect = effects[ref]
        sounds = [a['trigger'] + ' → ' + ', '.join(
            x['attributes'].get('fmod_name', x['tag']) for d in a['definitions'] for x in d['implementations'])
                  for a in effect['audio']]
        lines.append(f"| {material} | `{ref}` | {'; '.join(sounds)} | {', '.join(p['name'] for p in effect['particles'])} |")
    lines += ['', '## Findings and interpretation', '',
              '- Wood: `wood_chips_diff.tif` plus a `hit_dust_diff.tif` child emitter and a wood decal. The wood chip emitter has SimpleCollision enabled.',
              '- Stone: `stone_debris_diff.tif`, a dust child and sparks. Debris has SimpleCollision enabled.',
              '- Metal in the player sword row uses `collisions:lngsw_lngsw_clash_player`: the sword-clash sound and spark emitters. A separate `destructibles:sword_metal` asset exists, but is not that row\'s selected effect.',
              '- A metal-door cell uses a plate-impact effect. The passed-plate particle definition includes blood as well as sparks. Do not blindly reuse it for a harmless VR touch against a static door.',
              '- Sword stab and slash player rows share these wood/rock/metal effects; these effects alone cannot distinguish a thrust from a cut.',
              '- Stone/plaster share the sword-stone audio trigger; wood/wicker/shield effects can share particle emitters. Preserve the material and event context instead of reverse-classifying from audio alone.',
              '- Some particle parameters contain EmitterStrength curves. This supports authored scaling, but does not establish the native parameter API or a speed-to-strength formula.',
              '', '## Runtime boundary', '',
              'The supplied F3 logs record raw pre-filter contacts and native actions, without the dispatched material-effect row or a verified runtime surface-ID/name join. V66.116 adds a read-only observer for the statically recovered combat FX dispatcher, row/surface lookup and execution; see ../../native/MATERIAL_ABI.md. The raw 0x88-byte collision payload must not be relabelled using XML row numbers. Static/world weapon sweeps currently return distance/point/normal, not a qualified surface identity.', '',
              'The desired future response is local to Henry: retain wood contact without an automatic bounce, apply a small bounded deflection for hard surfaces, and dispatch a qualified native surface effect once per impact. Live collision/material identity, effect-parameter lifetime and duplicate-event suppression remain unproven. Native attack animation playback is not a safe substitute for a controller-owned weapon response.', '',
              'The JSON preserves complete relevant emitter trees, audio implementation attributes, effect XML, reverse assignments and source-member hashes. It contains no old raw log corpus.', '']
    (output / 'material_effect_chains.md').write_text('\n'.join(lines), encoding='utf-8')
    print(json.dumps(dict(effects=len(effects), rows=len(rows), audio_triggers=len(audio),
                          particle_roots=len(particles), provenance=len(provenance))))


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--game', type=Path, required=True)
    parser.add_argument('--impact-report', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    discover(args.game, args.impact_report, args.out)
