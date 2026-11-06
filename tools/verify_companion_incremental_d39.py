#!/usr/bin/env python3
"""Replay the real preparation/staging rules without building the game DLL.

Uses the production helper, source mapping, CMake preparation include, exact
companion staging block and stage script. A tiny synthetic donor and trivial
native target isolate incremental-build ordering; no game content is executed.
Use --stage-source with the immutable D38 tree for the POST_BUILD negative
control while retaining the repaired current-source preparation route.
"""
import argparse
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import zipfile

ROOT = Path(__file__).resolve().parents[1]
MAP_SOURCE = 'mods/KCD2VRMap/map_runtime.lua'
MAP_MEMBER = 'Scripts/Startup/zzkcd2vr_map.lua'


def sha(data):
    return hashlib.sha256(data).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=ROOT)
    parser.add_argument('--stage-source', type=Path)
    parser.add_argument('--cmake', default='cmake')
    parser.add_argument('--generator', default='Unix Makefiles')
    parser.add_argument('--compiler', default='c++')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    source = args.source.resolve()
    stage_source = (args.stage_source or source).resolve()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    fixture = Path(tempfile.mkdtemp(prefix='fixture-', dir=out))
    src = fixture / 'source'
    spec = importlib.util.spec_from_file_location('companion_sources_fixture_d39',
                                                source / 'tools/companion_sources_d39.py')
    mapping = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mapping)
    for relative in ['tools/package_release_helpers_r19.py',
                     'tools/companion_sources_d39.py',
                     'tools/verify_single_binary_b1.py',
                     'cmake/prepare_companion_d39.cmake',
                     'cmake/stage_companion63.cmake']:
        dest = src / relative
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source / relative, dest)
    for relative in mapping.SOURCE_PAIRS:
        path = src / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(('fixture current ' + relative + '\n').encode())
    identity = src / 'src/build_identity.h'
    identity.parent.mkdir(parents=True, exist_ok=True)
    identity.write_text('#define KCD2VR_BUILD_MARKER "D39_INCREMENTAL_FIXTURE_A"\n')
    donor = src / 'companion/C63_UNIFIED/Mods/KCD2VR'
    (donor / 'Data').mkdir(parents=True, exist_ok=True)
    rows = []
    with zipfile.ZipFile(donor / 'Data/KCD2VR.pak', 'w') as archive:
        for name in mapping.SOURCE_PAIRS.values():
            if name not in mapping.ALLOWED_ADDITIONS:
                payload = ('fixture donor ' + name + '\n').encode()
                archive.writestr(name, payload)
                rows.append({'path': name, 'sha256': sha(payload), 'source': 'synthetic fixture'})
    (donor / 'mod.manifest').write_bytes(b'<fixture/>\n')
    (donor / 'KCD2VR_UNIFIED63.json').write_text(json.dumps({
        'version': 63, 'pak_sha256': sha((donor / 'Data/KCD2VR.pak').read_bytes()),
        'members': len(rows), 'members_provenance': rows,
        'distribution_files': {'mod.manifest': sha((donor / 'mod.manifest').read_bytes())},
    }))
    production = (stage_source / 'CMakeLists.txt').read_text()
    begin = production.index('# C63: stage the same reviewed unified package')
    end = production.index('# Build as KCD2VR.asi', begin)
    stage_block = production[begin:end]
    (src / 'CMakeLists.txt').write_text(
        'cmake_minimum_required(VERSION 3.20)\n'
        'project(D39Incremental LANGUAGES CXX)\n'
        'find_package(Python3 REQUIRED COMPONENTS Interpreter)\n'
        'add_executable(KCD2VR fixture.cpp)\n'
        'set_target_properties(KCD2VR PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/release")\n'
        'include(cmake/prepare_companion_d39.cmake)\n' + stage_block)
    (src / 'fixture.cpp').write_text('int main() { return 0; }\n')
    checks, runs = [], []

    def check(name, passed, **details):
        checks.append({'name': name, 'passed': bool(passed), **details})

    def run(command, expected=0):
        proc = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        runs.append({'command': command, 'exit': proc.returncode, 'output': proc.stdout})
        if expected == 0 and proc.returncode != 0:
            raise RuntimeError(proc.stdout)
        return proc

    def configure(build, prepared=None):
        command = [args.cmake, '-S', str(src), '-B', str(build), '-G', args.generator,
                   '-DCMAKE_CXX_COMPILER=' + args.compiler, '-DPython3_EXECUTABLE=' + sys.executable]
        if prepared is not None:
            command.append('-DKCD2VR_COMPANION_SOURCE_ROOT=' + str(prepared))
        run(command)

    def build_target(build, expected=0):
        return run([args.cmake, '--build', str(build), '--target', 'KCD2VR'], expected)

    def member_matches(pak):
        if not pak.is_file():
            return False
        with zipfile.ZipFile(pak) as archive:
            return archive.read(MAP_MEMBER) == (src / MAP_SOURCE).read_bytes()

    def prepare(prepared):
        run([sys.executable, str(src / 'tools/package_release_helpers_r19.py'),
             '--source', str(src), '--prepare-companion', '--out', str(prepared)])

    try:
        auto = fixture / 'automatic'
        configure(auto)
        build_target(auto)
        staged = auto / 'release/companion/Mods/KCD2VR'
        pak = staged / 'Data/KCD2VR.pak'
        prepared_pak = auto / 'd39_companion/Mods/KCD2VR/Data/KCD2VR.pak'
        binary = auto / 'release/KCD2VR'
        initial_binary_time = binary.stat().st_mtime_ns
        check('automatic initial stage contains current source', member_matches(pak))
        edit = src / MAP_SOURCE
        edit.write_bytes(edit.read_bytes() + b'-- automatic incremental edit\n')
        build_target(auto)
        check('Lua-only edit regenerates prepared package', member_matches(prepared_pak))
        check('Lua-only edit reaches actual binary-adjacent package', member_matches(pak))
        check('Lua-only edit does not force native relink', binary.stat().st_mtime_ns == initial_binary_time)
        pak_time = pak.stat().st_mtime_ns
        build_target(auto)
        check('no-op build preserves package and native timestamps',
              pak.stat().st_mtime_ns == pak_time and binary.stat().st_mtime_ns == initial_binary_time)
        identity.write_text('#define KCD2VR_BUILD_MARKER "D39_INCREMENTAL_FIXTURE_B"\n')
        build_target(auto)
        check('identity-only edit reaches staged metadata',
              json.loads((staged / 'KCD2VR_UNIFIED63.json').read_text())['candidate'] == 'D39_INCREMENTAL_FIXTURE_B')
        pak.unlink()
        build_target(auto)
        check('missing staged package is restored without relink',
              member_matches(pak) and binary.stat().st_mtime_ns == initial_binary_time)

        # Mirrors build_release.ps1: it prepares an explicit source root first,
        # then builds KCD2VR specifically rather than an arbitrary ALL target.
        explicit = fixture / 'explicit'
        prepared = fixture / 'explicit_prepared'
        prepare(prepared)
        configure(explicit, prepared)
        build_target(explicit)
        staged = explicit / 'release/companion/Mods/KCD2VR'
        pak = staged / 'Data/KCD2VR.pak'
        binary = explicit / 'release/KCD2VR'
        initial_binary_time = binary.stat().st_mtime_ns
        check('explicit prepared initial stage contains current source', member_matches(pak))
        edit.write_bytes(edit.read_bytes() + b'-- explicit incremental edit\n')
        prepare(prepared)
        configure(explicit, prepared)
        build_target(explicit)
        check('explicit prepared edit reaches actual binary-adjacent package', member_matches(pak))
        check('explicit prepared edit does not force native relink',
              binary.stat().st_mtime_ns == initial_binary_time)
        manifest = staged / 'mod.manifest'
        manifest.write_bytes(b'corrupt staged distribution\n')
        build_target(explicit)
        check('no-op native build repairs staged distribution corruption',
              manifest.read_bytes() == (donor / 'mod.manifest').read_bytes())
        (prepared / 'Mods/KCD2VR/mod.manifest').write_bytes(b'corrupt prepared input\n')
        rejected = build_target(explicit, expected=1)
        check('corrupt prepared input fails build even with current native target',
              rejected.returncode != 0 and 'source distribution member hash mismatch' in rejected.stdout)
    except Exception as error:
        check('fixture execution completed', False, error=str(error))
    result = {
        'scope': 'Actual production preparation and stage rules; synthetic tiny donor and native fixture only. No game DLL/headset execution.',
        'source': str(source), 'stage_source': str(stage_source),
        'fixture': str(fixture), 'stage_block_sha256': sha(stage_block.encode()),
        'checks': checks, 'passed': sum(c['passed'] for c in checks),
        'failed': sum(not c['passed'] for c in checks), 'runs': runs,
    }
    (out / 'results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: value for key, value in result.items() if key != 'runs'}, indent=2))
    return 0 if checks and all(c['passed'] for c in checks) else 1


if __name__ == '__main__':
    raise SystemExit(main())
