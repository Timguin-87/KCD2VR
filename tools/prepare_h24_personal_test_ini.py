"""Prepare a PERSONAL test INI without touching the installed INI.

Keep every saved line/value, add missing Settings defaults, and apply explicit
test overrides only in Settings. Never move calibration or finger sections.
"""
import argparse, hashlib, json, re
from pathlib import Path

def sha(data):
    return hashlib.sha256(data).hexdigest()

def settings(lines):
    section = ''
    result = {}
    for line in lines:
        h = re.match(r'\s*\[([^]]+)\]', line)
        if h:
            section = h[1].casefold()
        elif section == 'settings':
            m = re.match(r'\s*([^;#=][^=]*?)\s*=\s*(.*)', line)
            if m:
                result.setdefault(m[1].strip().casefold(), (m[1].strip(), m[2]))
    return result

def main():
    p = argparse.ArgumentParser()
    p.add_argument('--saved', type=Path, required=True)
    p.add_argument('--defaults', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    assert a.out.resolve() not in (a.saved.resolve(), a.defaults.resolve())
    saved = a.saved.read_bytes()
    lines = saved.decode('utf-8-sig').splitlines()
    defaults = settings(a.defaults.read_text(encoding='utf-8-sig').splitlines())
    original = settings(lines)
    overrides = {
        'VrGameFov': '95',
        'VrHorizontalFov': '95',
        'VrSourceVerticalFovDegrees': '95',
        'VrCorrectedCenterScaleFovDegrees': '95',
        'VrNativeHandPhysics': 'false',
        'VrShowBlockIndicator': 'true',
        'VrMasterStrikeVeto': '0',
        'VrMasterStrikeStabOnly': '0',
        'VrRoomscaleDodge': 'true',
        'VrNoBlockCooldownF4': 'true',
        'VrHybridDefault59': 'true',
        'VrGuardAutoWindupRefresh': 'false',
        'VrRagdollF9Default': 'true',
        'VrGuardEnterMaxSpeed': '3',
        'VrGuardExitSpeed': '4',
        'VrMasterHandoffForwardSpeed': '3.5',
        'VrPlayerCombatWindowScaleH26': '1.5',
        'VrKickStyle': '0',
        'VrKickFallbackLeg': 'false',
    }
    override_keys = {k.casefold(): (k, v) for k, v in overrides.items()}
    section = ''
    for i, line in enumerate(lines):
        h = re.match(r'\s*\[([^]]+)\]', line)
        if h:
            section = h[1].casefold()
            continue
        m = re.match(r'\s*([^;#=][^=]*?)\s*=', line)
        if section == 'settings' and m and m[1].strip().casefold() in override_keys:
            k, v = override_keys[m[1].strip().casefold()]
            lines[i] = f'{k} = {v}'
    missing = {k: v for k, v in defaults.items() if k not in original}
    for k, v in override_keys.items():
        if k not in original:
            missing[k] = v
    start = next((i for i, x in enumerate(lines) if x.strip().casefold() == '[settings]'), None)
    if start is None:
        lines.extend(['', '[Settings]'])
        end = len(lines)
    else:
        end = next((i for i in range(start + 1, len(lines)) if re.match(r'\s*\[', lines[i])), len(lines))
    additions = ['', '; Matching candidate options; existing calibration remains below.']
    additions.extend(f'{k} = {v}' for k, v in missing.values())
    lines[end:end] = additions
    # This comment described the retired zero timer that could retain block.
    lines = [x.replace('wh_cs_BlockCooldown = 0 while F4 is on', 'wh_cs_BlockCooldown = 0.001 s while F4 is on (native timer retained)') for x in lines]
    header = ['; H28-C54-COMBAT-RECOVERY-CANDIDATE1 personal TEST configuration.',
              '; Prepared from your saved INI; see H28_START_HERE.txt and the manifest for overrides.',
              '; Native hand physics OFF: restore tracked wrist authority after H25 locomotion regression; NPC touch remains unverified.', '']
    output = ('\r\n'.join(header + lines) + '\r\n').encode('utf-8')
    final = settings(lines)
    for k, (_, v) in original.items():
        if k not in override_keys:
            assert final[k][1] == v, k
    for k, (_, v) in override_keys.items():
        assert final[k][1] == v, k
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_bytes(output)
    manifest = {'saved_sha256': sha(saved), 'defaults_sha256': sha(a.defaults.read_bytes()),
                'output_sha256': sha(output), 'overrides': overrides,
                'added_settings': [x[0] for x in missing.values()],
                'preserved': 'All other saved settings and all non-Settings lines/sections; no live file changed.',
                'status': 'Personal candidate configuration; feature acceptance requires headset tests.'}
    a.out.with_suffix('.manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
    print(json.dumps({'out': str(a.out), 'sha256': sha(output), 'added': len(missing)}))

if __name__ == '__main__':
    main()
