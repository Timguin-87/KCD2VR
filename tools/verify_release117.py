"""Inspect a linked V66.117 PE and build transcript without loading the ASI."""
import argparse
import hashlib
import json
import re
import struct
from pathlib import Path
from native_research112 import PE

MARKER = 'KCD2VR-V66.117-NATURAL-GUARD-NATIVE-INPUT'


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--asi', type=Path, required=True)
    p.add_argument('--log', type=Path, action='append', required=True)
    p.add_argument('--exit-file', type=Path, required=True)
    p.add_argument('--out', type=Path, required=True)
    a = p.parse_args()
    pe = PE(a.asi)
    nt = struct.unpack_from('<I', pe.data, 0x3c)[0]
    assert struct.unpack_from('<H', pe.data, nt+24)[0] == 0x20b, 'Expected PE32+'
    imports = []
    for directory, stride, name_offset in ((1, 20, 12), (13, 32, 4)):
        rva, size = struct.unpack_from('<II', pe.data, nt+24+112+8*directory)
        if not rva: continue
        for off in range(0, size, stride):
            descriptor = pe.read(rva+off, stride)
            if not any(descriptor): break
            name_rva = struct.unpack_from('<I', descriptor, name_offset)[0]
            imports.append(pe.read(name_rva, 128).split(b'\0')[0].decode('ascii'))
    logs = '\n'.join(x.read_text(encoding='utf-8-sig') for x in a.log)
    warnings = [s for s in logs.splitlines() if re.search(r'\bwarning\s+[A-Z]*\d+:', s, re.I)]
    errors = [s for s in logs.splitlines() if re.search(r'\berror\s+[A-Z]*\d+:', s, re.I)]
    result = dict(asi_path=str(a.asi.resolve()), bytes=len(pe.data), sha256=hashlib.sha256(pe.data).hexdigest(),
                  marker=MARKER, marker_found=MARKER.encode() in pe.data,
                  imports=imports, asan_import=any('asan' in n.lower() for n in imports),
                  compiler_linker_exit=int(a.exit_file.read_text(encoding='utf-8-sig').strip()),
                  compiler_warnings=warnings, compiler_errors=errors, game_installed=False, headset_tested=False)
    assert result['marker_found'] and not result['asan_import'] and not errors and result['compiler_linker_exit'] == 0
    a.out.parent.mkdir(parents=True, exist_ok=True)
    a.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf-8')
    print(json.dumps({k: result[k] for k in ('sha256', 'bytes', 'marker_found', 'asan_import', 'compiler_linker_exit')}))


if __name__ == '__main__': main()
