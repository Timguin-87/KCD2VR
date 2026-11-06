"""Read-only, bounded extraction. Does not install mods or execute game scripts."""
import argparse, hashlib, json, re, zipfile
from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument('game_root', type=Path)
p.add_argument('--ragdoll-pak', type=Path)
p.add_argument('--ragdoll-cfg', type=Path)
p.add_argument('--out', type=Path, required=True)
a = p.parse_args()
sha = lambda b: hashlib.sha256(b).hexdigest()
report = {'runtime_execution': False, 'sources': [], 'notes': [
    'Script declarations and CVar string presence do not prove a live movable object or an enabled engine setting.',
    'KCD1 mod settings are comparison evidence, not a KCD2 installation or ABI specification.',
    'Free corpse-part grabbing is distinct from native carry; shoulder carry is a later user gesture.',
]}
script_names = {'carryableitem.lua', 'carryitempile.lua', 'pickableitem.lua', 'entityutils.lua'}
pattern = re.compile(r'CanPickUp|CarryableItemBind|CIPileBind|Physicalize|SetPhysicParams|bRigidBody|Physics|Mass|Density|AwakePhysics|COLLISION_CLASS', re.I)
with zipfile.ZipFile(a.game_root / 'Data/Scripts.pak') as z:
    for name in z.namelist():
        if name.replace('\\', '/').rsplit('/', 1)[-1].lower() not in script_names:
            continue
        raw = z.read(name)
        assert len(raw) < 2_000_000
        lines = raw.decode('utf-8-sig', errors='replace').splitlines()
        report['sources'].append({'archive': 'Data/Scripts.pak', 'member': name, 'sha256': sha(raw),
            'matches': [{'line': i+1, 'text': s[:250]} for i, s in enumerate(lines) if pattern.search(s)][:80]})
if a.ragdoll_pak:
    with zipfile.ZipFile(a.ragdoll_pak) as z:
        report['ragdoll_mod'] = {'path': str(a.ragdoll_pak), 'sha256': sha(a.ragdoll_pak.read_bytes()), 'members': z.namelist(), 'selected': []}
        for name in ('scripts/PickableItem__zzzxdddbrdp.lua', 'scripts/BasicEntity__zzzxdddbrdp.lua', 'scripts/EntityUtils__zzzxdddbrdp.lua'):
            raw = z.read(name)
            lines = raw.decode('utf-8-sig', errors='replace').splitlines()
            report['ragdoll_mod']['selected'].append({'member': name, 'sha256': sha(raw),
                'matches': [{'line': i+1, 'text': s[:250]} for i, s in enumerate(lines) if pattern.search(s)][:80]})
if a.ragdoll_cfg:
    raw = a.ragdoll_cfg.read_bytes()
    report['ragdoll_cfg'] = {'path': str(a.ragdoll_cfg), 'sha256': sha(raw),
        'settings': [s.strip() for s in raw.decode(errors='replace').splitlines()
                     if re.match(r'\s*(?:p_|ca_|es_|wh_|e_).*=', s)][:100]}
    dll = (a.game_root / 'Bin/Win64MasterMasterSteamPGO/WHGame.dll').read_bytes()
    report['dll_sha256'] = sha(dll)
    report['string_presence_NOT_registration_proof'] = {
        key: key.encode()+b'\0' in dll for key in ('wh_cs_PhysSim', 'wh_cs_PhysHitReaction_ImpactCoef',
        'wh_HitReaction_CollisionsEnabled', 'wh_e_PernamentPhysicalization', 'es_ImpulseScale')}
a.out.parent.mkdir(parents=True, exist_ok=True)
a.out.write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
print(f'Wrote bounded evidence: {a.out}; native calls/writes: NONE')
