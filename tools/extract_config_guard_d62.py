#!/usr/bin/env python3
"""Extract unchanged startup/save prefixes and calibration writers for I/O tests.

These fixtures execute the source guard ordering, not all of Mod::LoadConfig or
the proprietary DetourModKit parser. Native OS profile and parser calls are the
explicit test leaves. Generated output is a build artifact, never hand-edited.
"""
from pathlib import Path
import argparse
import hashlib
import json


def function_body(text, signature):
    start = text.index(signature)
    begin = text.index('{', start)
    # Calibration functions contain no brace-bearing string literals. Keep this
    # small extractor scoped to those signatures instead of parsing arbitrary C++.
    depth = 0
    for pos in range(begin, len(text)):
        depth += (text[pos] == '{') - (text[pos] == '}')
        if depth == 0:
            return text[start:pos + 1]
    raise ValueError(f'unclosed function {signature}')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    mod_path = args.root / 'src/mod.cpp'
    calibration_path = args.root / 'src/subsystems/vrik/calibration_runtime.inl'
    mod, calibration = mod_path.read_text(), calibration_path.read_text()
    load_start = mod.index('bool Mod::LoadConfig()')
    load_end = mod.index('    auto settings = ini.section("Settings");', load_start)
    load = mod[load_start:load_end].replace('bool Mod::LoadConfig()', 'static bool GuardedLoadD62()', 1)
    assert 'Lease iniLeaseD62' in load and 'ini.load(configPathD62.c_str())' in load
    save_start = mod.index('bool Mod::SaveMenuSettings() const')
    save_end = mod.index('    bool ok = true;', save_start)
    save = mod[save_start:save_end].replace('bool Mod::SaveMenuSettings() const', 'static bool GuardedMenuSaveD62()', 1)
    assert 'Lease iniLeaseD62' in save and 'auto wb =' in save
    pieces = [
        '// GENERATED from current production source. Do not edit.\n',
        load + '    ++bindingPhaseEnteredD62;\n    return FixtureMigrationD62(configPathD62.c_str());\n}\n',
        save + '    bool ok=wb("D62TestMenuBool",true);\n    ok=wi("D62TestMenuInt",5)&&ok;\n    ok=wf("D62TestMenuFloat",1.75f)&&ok;\n    return ok;\n}\n',
        function_body(calibration, 'template<class T>static bool SaveProfile66100('),
        function_body(calibration, 'static bool SaveHeadKeyD62('),
    ]
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text('\n'.join(pieces) + '\n')
    pins = {str(path.relative_to(args.root)): hashlib.sha256(path.read_bytes()).hexdigest()
            for path in (mod_path, calibration_path, args.root / 'src/core/config_file_guard_d62.h')}
    args.out.with_suffix('.pins.json').write_text(json.dumps(pins, indent=2) + '\n')


if __name__ == '__main__':
    main()
