"""Source-only current H36 holster claim proposal; never applies shared files."""
import argparse
import difflib
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]


def one(text, before, after):
    assert text.count(before) == 1, before
    return text.replace(before, after)


def proposal(current_root, revision):
    def read(name):
        return subprocess.check_output(['git', 'show', revision+':'+name], cwd=current_root).decode().replace('\r\n', '\n')
    changes = {}
    name = 'src/subsystems/throw_drop/holster_gestures.inl'
    old = read(name)
    new = one(old, '    const bool rightFenceH36=sharedFenceH36||rightInhibitH36;',
        '    const bool rightClaimT24=HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),in.bodyEpoch,in.epoch);\n'
        '    const bool leftClaimT24=HandClaimBlockedT24(1,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),in.bodyEpoch,in.epoch);\n'
        '    const bool rightFenceH36=sharedFenceH36||rightInhibitH36||rightClaimT24;')
    new = one(new, 'const bool leftFenceH36=sharedFenceH36||(rightInhibitH36&&!PhysLeftHolsterRetiredH36(in,e));',
        'const bool leftFenceH36=sharedFenceH36||(rightInhibitH36&&!PhysLeftHolsterRetiredH36(in,e))||leftClaimT24;')
    new = one(new, 'const bool boxingHands=equipment114::BoxingHands59c7f(e.valid&&e.handKnown[0]&&e.handKnown[1]&&',
        'const bool boxingHands=equipment114::BoxingHands59c7f(!rightClaimT24&&!leftClaimT24&&e.valid&&e.handKnown[0]&&e.handKnown[1]&&')
    changes[name] = (old, new)
    name = 'src/vrik/physical_native.inl'
    old = read(name)
    before = '        if(c.side==0&&(c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&'
    new = one(old, before,
        '        // T24: a native-empty utility preview still owns this hand.\n'
        '        // Recheck queued equipment at its existing native consumer too.\n'
        '        if((c.kind==ph::CommandKind::Holster||c.kind==ph::CommandKind::Draw)&&\n'
        '           HandClaimBlockedT24(c.side,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),e.epoch,e.request)){g_phys_commands6698.pop(c);continue;}\n'+before)
    changes[name] = (old, new)
    return changes


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--current-root', type=Path, required=True)
    parser.add_argument('--revision', required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    out = args.out.resolve()
    if out == ROOT or ROOT in out.parents:
        raise ValueError('external output required')
    out.mkdir(parents=True, exist_ok=True)
    changes = proposal(args.current_root, args.revision)
    patch = ''.join(''.join(difflib.unified_diff(old.splitlines(True), new.splitlines(True), 'a/'+name, 'b/'+name))
                    for name, (old, new) in changes.items())
    (out/'H36_CURRENT_CLAIM_PROPOSED.diff').write_text(patch)
    for name, (_, new) in changes.items():
        path = out/'source'/name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(new)
    report = {'root_revision': args.revision, 'applied': False, 'native_execution': False,
        'files': {name: {'before_normalized_sha256': hashlib.sha256(old.encode()).hexdigest(),
                         'after_normalized_sha256': hashlib.sha256(new.encode()).hexdigest()}
                  for name, (old, new) in changes.items()},
        'limits': ['Proposal only: coordinator preserves its newer T24 contact/guard changes.',
                   'Native hand WUIDs, H36 retirement, collider bits and native completion paths are unchanged.',
                   'Only current same-side claim blocks normal holsters; opposite hand remains independent.']}
    (out/'RESULT.json').write_text(json.dumps(report, indent=2)+'\n')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
