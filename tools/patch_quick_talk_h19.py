"""Replace only BasicAIActions in an explicitly supplied candidate companion.

No installed game path is selected. All other members are verified byte-identical.
The input may be the output: replacement happens only after a verified temp ZIP.
"""
from pathlib import Path
import argparse
import hashlib
import json
import os
import zipfile

ROOT = Path(__file__).resolve().parents[1]
MEMBER = 'Scripts/Entities/AI/Shared/BasicAIActions.lua'
ORIGINAL_SHA = '8faba8d08f3d1019c9a71c7bf198aaa5a16bf15ab2df82936d3aec8c5daaec0d'

def sha(data):
    return hashlib.sha256(data).hexdigest()

def build(base, out, manifest=None):
    replacement = (ROOT / 'companion/source' / MEMBER).read_bytes()
    with zipfile.ZipFile(base) as z:
        infos = z.infolist()
        entries = {i.filename: z.read(i.filename) for i in infos}
    assert len(entries) == len(infos), 'duplicate ZIP entries refused'
    assert MEMBER in entries and sha(entries[MEMBER]) in (ORIGINAL_SHA, sha(replacement)), 'unreviewed BasicAIActions base'
    assert b':action("chat_init_with_focus")' in replacement and b':uiOrder(6)' in replacement
    before_sha = sha(base.read_bytes())
    pending = out.with_name(out.name + '.h19-pending')
    assert not pending.exists(), 'existing temp file refused'
    out.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(pending, 'w') as z:
        for info in infos:
            z.writestr(info, replacement if info.filename == MEMBER else entries[info.filename])
    with zipfile.ZipFile(pending) as z:
        assert z.testzip() is None
        assert z.namelist() == [i.filename for i in infos]
        assert all(z.read(n) == (replacement if n == MEMBER else old) for n, old in entries.items())
    result = {'base_sha256': before_sha, 'pak_sha256': sha(pending.read_bytes()), 'member': MEMBER,
              'before_member_sha256': sha(entries[MEMBER]), 'after_member_sha256': sha(replacement),
              'unchanged_members': len(entries) - 1, 'installed': False}
    os.replace(pending, out)
    if manifest:
        data = json.loads(manifest.read_text(encoding='utf-8-sig'))
        data['pak_sha256'] = result['pak_sha256']
        data['h19_quick_talk_prompt'] = result
        for entry in data.get('members_provenance', []):
            if entry.get('path') == MEMBER:
                entry['h19_previous'] = dict(entry)
                entry['sha256'] = result['after_member_sha256']
                entry['sources'] = ['companion/source/' + MEMBER]
        manifest.write_text(json.dumps(data, indent=2) + '\n', encoding='utf8')
    return result

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--base', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--manifest', type=Path)
    args = parser.parse_args()
    print(json.dumps(build(args.base, args.out, args.manifest), indent=2))
