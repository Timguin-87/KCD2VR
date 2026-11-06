#!/usr/bin/env python3
"""Extract the current actor view, native-map adapter and callback for host tests.

Only explicitly marked M2 forwarding includes are expanded. Native memory,
the exact ABI call's implementation and unrelated callback effects are test
leaves; the production view, lookup adapter/cache and callback order are not.
No source-tree generated files or archived runtime implementations are used.
"""
from pathlib import Path
import hashlib
import json
import sys
from source_text import expand_source


def extract(root, output):
    root, output = Path(root).resolve(), Path(output).resolve()
    output.mkdir(parents=True, exist_ok=True)
    geometry = expand_source(root / 'src/diagnostics/geometry_hit178r27.inl', root=root)
    native = expand_source(root / 'src/diagnostics/native_hit_probe134.inl', root=root)
    slices = []

    def take(source, start, end, filename, close=''):
        if source.text.count(start) != 1:
            raise ValueError('expected exactly one production start: ' + start)
        begin = source.text.index(start)
        finish = source.text.index(end, begin)
        block = source.text[begin:finish] + close
        (output / filename).write_text(block, encoding='utf-8')
        slices.append({'requested': source.requested, 'start': start, 'end': end,
                       'filename': filename, 'synthetic_closing_suffix': close,
                       'sha256': hashlib.sha256(block.encode()).hexdigest()})

    take(native, 'std::uint32_t EntityId(const void* entity)', 'int I(const void* st',
         'generated_actor_native_view_h8.inc')
    take(geometry, 'using ActorLookupScopeH7=', '// C19C3: an Update/map ordering gap',
         'generated_actor_lookup_h8.inc')
    take(native, 'void __fastcall ActorUpdateDetour(void* actor,float frameTime)',
         '    if(!Active())return;', 'generated_actor_callback_h8.inc', '}\n')
    header = root / 'src/diagnostics/actor_lookup_h7.h'
    receipt = {'geometry': geometry.provenance(), 'native': native.provenance(),
               'compiled_helper': {'path': header.relative_to(root).as_posix(),
                                   'raw_sha256': hashlib.sha256(header.read_bytes()).hexdigest()},
               'slices': slices,
               'scope': 'Actual current production actor view/adapter/cache/original-first callback; synthetic native memory/ABI method and unrelated effects; no native game hit execution.'}
    (output / 'actor_lookup_h8_extract.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    return receipt


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: extract_actor_lookup_h8.py OUTPUT_DIRECTORY')
    extract(Path(__file__).resolve().parents[1], sys.argv[1])
