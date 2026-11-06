"""Portable paths and native compiler support for maintained M41 host fixtures."""
from pathlib import Path
import argparse
import hashlib
import json
import os
import shutil
import subprocess
import sys


def options(suite):
    parser = argparse.ArgumentParser(description='Run actual M41 source with explicit native engine test leaves.')
    parser.add_argument('--source-root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True, help='Private results/build directory; no game installation')
    parser.add_argument('--vcvars', type=Path, help='Optional vcvars64.bat; otherwise discover Visual Studio')
    parser.add_argument('--lua-path', type=Path, help='Optional directory containing an installed lupa package')
    parser.add_argument('--pak', type=Path, help='Current gameplay companion PAK for native-book preservation checks')
    parser.add_argument('--control-no-model-gate', action='store_true', help='Presenter negative control; expected failure')
    args = parser.parse_args()
    args.source_root = args.source_root.resolve()
    if not (args.source_root / 'src/subsystems/vrmap/static_paper_m41.h').is_file():
        parser.error('--source-root must name the current M41-capable source tree')
    args.out = args.out.resolve() / suite
    if suite == 'presenter' and args.control_no_model_gate:
        args.out = args.out.with_name('presenter_control_no_model_gate')
    args.out.mkdir(parents=True, exist_ok=True)
    return args


def lua_runtime(args):
    extra = args.lua_path or os.environ.get('KCD2VR_LUA_TEST_PATH')
    if extra:
        sys.path.insert(0, str(Path(extra).resolve()))
    try:
        from lupa.lua51 import LuaRuntime
    except ImportError as exc:
        raise SystemExit('Lua 5.1 fixture needs lupa: python -m pip install lupa, or pass --lua-path to its package directory') from exc
    return LuaRuntime


def compile_and_run(args, cpp, name, include_dirs=()):
    """Compile current generated fixture with MSVC/ASan, leaving engine calls mocked."""
    if os.name != 'nt':
        raise SystemExit('This native-leaf fixture requires Windows MSVC with AddressSanitizer')
    env = {key.upper(): value for key, value in os.environ.items()}
    vcvars = args.vcvars
    if vcvars is None and not shutil.which('cl'):
        finder = Path(os.environ.get('ProgramFiles(x86)', 'C:/Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        if finder.is_file():
            found = subprocess.run([str(finder), '-latest', '-products', '*', '-requires',
                                    'Microsoft.VisualStudio.Component.VC.Tools.x86.x64', '-property',
                                    'installationPath'], capture_output=True, text=True, check=True).stdout.strip()
            if found:
                vcvars = Path(found) / 'VC/Auxiliary/Build/vcvars64.bat'
    if vcvars is not None:
        vcvars = Path(vcvars).resolve()
        if not vcvars.is_file():
            raise SystemExit('vcvars64.bat does not exist: ' + str(vcvars))
        # Path is passed to a batch call only; reject shell expansion/metacharacters.
        if any(c in str(vcvars) for c in '%!&|<>^\r\n"'):
            raise SystemExit('Unsupported command metacharacter in --vcvars path')
        setup = args.out / 'compiler_environment.cmd'
        setup.write_text('@echo off\ncall "' + str(vcvars) + '" >nul\nif errorlevel 1 exit /b 1\nset\n', encoding='utf-8')
        probe = subprocess.run(['cmd', '/d', '/c', str(setup)], capture_output=True, text=True, check=True)
        for line in probe.stdout.splitlines():
            key, sep, value = line.partition('=')
            if sep and key:
                env[key.upper()] = value
    compiler = shutil.which('cl', path=env.get('PATH'))
    if not compiler:
        raise SystemExit('MSVC cl.exe unavailable; use an x64 developer prompt or --vcvars')
    binary = args.out / (name + '.exe')
    cmd = [compiler, '/nologo', '/std:c++latest', '/EHsc', '/UNDEBUG', '/Od', '/Zi',
           '/fsanitize=address', *['/I' + str(Path(d).resolve()) for d in include_dirs],
           str(cpp), '/Fe:' + str(binary), '/Fo:' + str(args.out / (name + '.obj')),
           '/Fd:' + str(args.out / (name + '.pdb')), '/link', '/INCREMENTAL:NO']
    (args.out / 'COMMAND.json').write_text(json.dumps(cmd, indent=2) + '\n', encoding='utf-8')
    built = subprocess.run(cmd, cwd=args.out, env=env, capture_output=True, text=True)
    (args.out / 'build.log').write_text(built.stdout + built.stderr, encoding='utf-8')
    if built.returncode:
        raise RuntimeError('MSVC build failed:\n' + built.stdout + built.stderr)
    run = subprocess.run([str(binary)], cwd=args.out, env=env, capture_output=True, text=True)
    log = run.stdout + run.stderr
    (args.out / 'run.log').write_text(log, encoding='utf-8')
    return run, log


def companion_pak(args):
    if args.pak:
        path, origin = args.pak.resolve(), 'explicit'
    else:
        suffix = Path('companion/Mods/KCD2VR/Data/KCD2VR.pak')
        candidates = [(args.source_root / 'dist/prerelease/Mods/KCD2VR/Data/KCD2VR.pak', 'current-dist')]
        # Custom or shortened build directories remain available via --pak.
        for build in (args.source_root / 'build-prerelease', args.source_root / 'build', args.source_root.parent / 'build'):
            for config in ('RelWithDebInfo', 'Release'):
                candidates.append((build / 'release' / config / suffix, 'current-build'))
            candidates.append((build / 'release' / suffix, 'current-build'))
        selected = next(((p, origin) for p, origin in candidates if p.is_file()), (None, None))
        path, origin = selected
    if path is None or not path.is_file():
        raise SystemExit('Supply --pak with the current gameplay companion package (or build it first)')
    return path, dict(path=str(path), selection=origin, sha256=hashlib.sha256(path.read_bytes()).hexdigest())
