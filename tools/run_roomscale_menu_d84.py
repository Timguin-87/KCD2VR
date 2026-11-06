"""Exercise literal menu/Mod/apply paths and existing finite movement tests."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess


def block(text, marker):
    start = text.index(marker)
    end = text.index('{', start) + 1
    depth = 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    return text[start:end]


def statement(text, marker):
    start = text.index(marker)
    return text[start:text.index(';', start) + 1]


def extract(source, fixture_root, dest):
    paths = ['src/mod.cpp', 'src/subsystems/experimental/live_runtime_d40.inl',
             'src/subsystems/vrik/host.inl', 'src/subsystems/vrik/menu.inl',
             'src/subsystems/experimental/menu.inl', 'src/subsystems/hud_ui_controls/menu.inl',
             'src/subsystems/experimental/live_settings_d39.h']
    values = {p: (source / p).read_text(encoding='utf-8-sig') for p in paths}
    mod, runtime = values[paths[0]], values[paths[1]]
    state_fixture = (fixture_root / 'tests/live_settings_d39_test.cpp').read_text()
    parts = [block(state_fixture, 'struct MenuState') + ';']
    parts.append('static MenuState Fill(){MenuState out{};\n' +
                 statement(mod, 'out.roomscale_mode =') + '\n' +
                 '#define KCD2VR_M2_HOST_SLOT 28\n#include "subsystems/vrik/host.inl"\n#undef KCD2VR_M2_HOST_SLOT\n' +
                 statement(runtime, 'd40.nativeRoomscaleD76=').replace('d40.', 'out.live_d39.') + '\nreturn out;}')
    prior = statement(mod, 'const bool nativeRoomscaleWasEnabledD84=') if 'const bool nativeRoomscaleWasEnabledD84=' in mod else ''
    parts.append('static void Apply(const MenuState& state){\n' + prior + '\nconst auto& d40=state.live_d39;\n' +
                 statement(runtime, 'roomscale_d76::SetEnabled(d40.nativeRoomscaleD76)') + '\n' +
                 '\n'.join(f'#define KCD2VR_M2_HOST_SLOT {n}\n#include "subsystems/vrik/host.inl"\n#undef KCD2VR_M2_HOST_SLOT' for n in (75, 84)) + '\n}')
    parts.append('static void ApplyState(const MenuState& s){Apply(s);}')
    f1 = block(mod, '{static bool f1_prev=false;')
    parts.append('static void Hotkey(bool matrix_active,bool f1_down)\n{' + f1 + '\n}')
    for name, path, marker in [('ClickWalking', paths[3], 'if(PtIn(kIxRoomscale,x,y))'),
                               ('ClickCycle', paths[5], 'if(PtIn(kRoomscale59,x,y))'),
                               ('ClickDodge', paths[4], 'if(PtIn(kHeadDodge54,x,y))')]:
        click = block(values[path], marker)
        parts.append(f'static void {name}(MenuState& s){{const int x=0,y=0,kIxRoomscale=0,kRoomscale59=0,kHeadDodge54=0;auto done=[&](const char*){{Apply(s);}};\n{click}\n}}')
    dest.mkdir(parents=True, exist_ok=True)
    literal = '\n\n'.join(parts) + '\n'
    (dest / 'roomscale_menu_source.inc').write_text(literal)
    (dest / 'source.json').write_text(json.dumps({
        'sources': [{'path': str(source / p), 'sha256': hashlib.sha256((source / p).read_bytes()).hexdigest()} for p in paths],
        'extraction_sha256': hashlib.sha256(literal.encode()).hexdigest(),
        'scope': 'Literal getter, prior-D40 snapshot, D40 native apply, F1 and all classic button bodies; included real host75/84 and live reducer; portable data fixture only.'}, indent=2) + '\n')


def native_fixture(source, dest):
    # The existing test is Windows-host-only: Linux excludes every callback
    # and intentionally returns false from its native memory leaves. Enable
    # callback compilation in this copied artifact and replace only clock and
    # guarded memcpy leaves with access to the test's allocated byte buffers.
    native = source / 'src/subsystems/roomscale/native_roomscale_d76.cpp'
    text = native.read_text(encoding='utf-8-sig')
    for marker, replacement in [
        ('bool copy(', 'bool copy(std::uintptr_t address,void*out,std::size_t n) noexcept {if(!address||!out)return false;std::memcpy(out,reinterpret_cast<const void*>(address),n);return true;}'),
        ('bool write(', 'bool write(std::uintptr_t address,const void*value,std::size_t n) noexcept {if(!address||!value)return false;std::memcpy(reinterpret_cast<void*>(address),value,n);return true;}'),
        ('std::uint64_t nowMs(', 'std::uint64_t nowMs() noexcept {return 1000;}')]:
        old = block(text, marker)
        assert text.count(old) == 1
        text = text.replace(old, replacement)
    old = '#if defined(_WIN32)\nusing namespace DetourModKit::hook;'
    assert text.count(old) == 1
    text = text.replace(old, '#if 1 // Host fixture enables actual native callbacks\nusing namespace DetourModKit::hook;')
    native_copy = dest / 'native_roomscale_host.inc'
    native_copy.write_text(text)
    fixture = source / 'tests/native_roomscale_d76_test.cpp'
    adapted = fixture.read_text(encoding='utf-8-sig').replace('#include "../src/subsystems/roomscale/native_roomscale_d76.cpp"', '#include "native_roomscale_host.inc"')
    target = dest / 'native_roomscale_host_test.cpp'
    target.write_text(adapted)
    (dest / 'native_adapter_source.json').write_text(json.dumps({
        'source': str(native), 'sha256': hashlib.sha256(native.read_bytes()).hexdigest(),
        'host_sha256': hashlib.sha256(text.encode()).hexdigest(),
        'adaptations': ['Enable actual callback compile block on Linux', 'copy/write use fixture-owned byte buffers instead of Windows SEH', 'nowMs leaf fixed at 1000', 'existing test include points at copied artifact'],
        'scope': 'All identity checks, callback bodies, step/queue/adoption ledger and tests otherwise unchanged. Not a Windows hook-install or runtime-game test.'}, indent=2) + '\n')
    return target


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--output', type=Path, required=True)
    ap.add_argument('--baseline', type=Path)
    args = ap.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.output.resolve()
    rows = []
    for label, source in [('current', root)] + ([('baseline', args.baseline.resolve())] if args.baseline else []):
        if out == source or source in out.parents:
            ap.error('Evidence output must be outside source trees')
        dest = out / label
        extract(source, root, dest)
        native_test = native_fixture(source, dest) if label == 'current' else None
        names = ['roomscale_menu_d84', 'roomscale_producer_d76', 'native_roomscale_d76', 'roomscale58', 'roomscale_dodge_d75'] if label == 'current' else ['roomscale_menu_d84']
        for name in names:
            configs = [('debug', ['-O0'])]
            if label == 'current' and name == 'roomscale_menu_d84':
                configs += [('release', ['-O2']), ('sanitized', ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer', '-no-pie'])]
            for mode, flags in configs:
                binary = dest / (name + '_' + mode)
                test = native_test if name == 'native_roomscale_d76' else root / 'tests' / (name + '_test.cpp')
                cmd = ['g++', '-std=c++20', '-UNDEBUG', '-pthread', *flags, '-I', str(source / 'src'), '-I', str(source / 'src/subsystems/roomscale'), '-I', str(dest), str(test), '-o', str(binary)]
                build = subprocess.run(cmd, capture_output=True, text=True)
                binary.with_suffix('.build.log').write_text(build.stdout + build.stderr)
                row = dict(label=label, name=name, mode=mode, command=cmd, build_rc=build.returncode, passed=False)
                if build.returncode == 0:
                    env = dict(os.environ)
                    if mode == 'sanitized':
                        env['ASAN_OPTIONS'] = 'detect_leaks=0'
                    run = subprocess.run([str(binary)], capture_output=True, text=True, env=env)
                    binary.with_suffix('.run.log').write_text(run.stdout + run.stderr)
                    row.update(run_rc=run.returncode, stdout=run.stdout, stderr=run.stderr)
                    row['passed'] = run.returncode == 0 if label == 'current' else run.returncode == 1 and 'FAIL D84 walking ON selects finite D76' in run.stdout and not run.stderr
                rows.append(row)
                print(f'{label}/{name}/{mode}: {"PASS" if row["passed"] else "FAIL"}', flush=True)
    (out / 'results.json').write_text(json.dumps(rows, indent=2) + '\n')
    return 0 if all(r['passed'] for r in rows) else 1


if __name__ == '__main__':
    raise SystemExit(main())
