"""Execute exact retail IsKind code + actual preview/paid contribution bodies.

Retail is read as data, never loaded as a module. Exact type predicates execute
on a private allocated image; the TLS definition getter alone is substituted.
Engine creation, VM, payment, Shoot and contact remain explicit host substitutes.
"""
import argparse
import hashlib
import json
import re
import struct
import subprocess
from pathlib import Path
from native_research112 import PE

ROOT = Path(__file__).resolve().parents[1]
SHA = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


def function(text, name):
    start = text.index(name)
    i = text.index('{', start) + 1
    depth = 1
    while depth:
        depth += (text[i] == '{') - (text[i] == '}')
        i += 1
    return text[start:i] + '\n'


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll', type=Path, required=True)
    ap.add_argument('--out', type=Path, required=True)
    ap.add_argument('--old-kind-control', action='store_true')
    a = ap.parse_args()
    out = a.out.resolve()
    assert out != ROOT and ROOT not in out.parents
    out.mkdir(parents=True, exist_ok=True)
    pe = PE(a.dll)
    assert hashlib.sha256(pe.data).hexdigest() == SHA and pe.stamp == 0x6A350E20
    runtime = ROOT / 'src/subsystems/throw_drop/runtime'
    launch = (runtime / 'native_ammo_launch_t17.inl').read_text()
    actual = function(launch, 'static bool AmmoKindsT17(')
    assert 'kind(ammo,3)' in actual
    old = actual.replace('AmmoKindsT17(', 'OldAmmoKindsK40(', 1).replace('kind(ammo,3)', 'kind(ammo,0x1B)')
    selected = actual.replace('kind(ammo,3)', 'kind(ammo,0x1B)') if a.old_kind_control else actual
    (out / 'actual_ammo_kinds_k40.inc').write_text(selected)
    (out / 'old_ammo_kinds_k40.inc').write_text(old)
    # All inherited predicates, native dispatch and the three RTTI-qualified rows.
    code_rvas = [0x469590, 0x96F66C, 0x474954, 0x469654, 0x469674, 0x469694, 0x4696B4, 0x4696D4]
    spans = []
    proof = []
    for address in code_rvas:
        bounds = pe.function(address)
        if bounds is None:
            assert address == 0x4696D4  # leaf has no unwind record; both RETs end at E9
            bounds = (address, address + 0x15)
        start, end = bounds
        assert start == address
        data = pe.read(start, end - start)
        spans.append((start, data))
        proof.append(dict(rva=hex(start), end=hex(end), sha256=hashlib.sha256(data).hexdigest(), bytes=data.hex()))
    (out / 'native_kind_code_k40.inc').write_text('\n'.join(
        '{0x%X,{%s}},' % (v, ','.join('0x%02X' % b for b in data)) for v, data in spans))
    types = []
    for vft, target, typename in [
        (0x3BA6EE8, 0x96F66C, b'.?AUS_AmmoItemClass@entitymodule@wh@@'),
        (0x3A4D890, 0x474954, b'.?AUS_MissileWeaponItemClass@entitymodule@wh@@'),
        (0x3A4E390, 0x469654, b'.?AUS_MeleeWeaponItemClass@entitymodule@wh@@')]:
        assert struct.unpack('<Q', pe.read(vft + 0x18, 8))[0] == pe.base + target
        col = struct.unpack('<Q', pe.read(vft - 8, 8))[0] - pe.base
        descriptor = struct.unpack('<6I', pe.read(col, 24))[3]
        assert pe.read(descriptor + 16, len(typename) + 1) == typename + b'\0'
        types.append(dict(vft=hex(vft), isKind=hex(target), rtti=typename.decode()))
    (out / 'RETAIL_PROOF.json').write_text(json.dumps(dict(retail_sha256=SHA, instructions=proof, rows=types,
        getter_substitution='4695B4 -> mov rax,[rcx+48];ret; no game TLS touched'), indent=2))
    loader = '''static void LoadKindCodeK40(){
 struct Span{std::uintptr_t rva;std::vector<unsigned char>bytes;};const Span spans[]{
#include "native_kind_code_k40.inc"
 };for(const auto& p:spans)std::memcpy(image+p.rva,p.bytes.data(),p.bytes.size());
 const unsigned char getter[]{0x48,0x8B,0x41,0x48,0xC3};std::memcpy(image+0x4695B4,getter,sizeof(getter));
 Put(image,0x3BA6EE8+0x18,WhBase()+0x96F66C);Put(image,0x3A4D890+0x18,WhBase()+0x474954);Put(image,0x3A4E390+0x18,WhBase()+0x469654);
 FlushInstructionCache(GetCurrentProcess(),image,0x5500000);
}
'''
    (out / 'native_kind_loader_k40.inc').write_text(loader)
    transient = (runtime / 'native_transient_ammo_t20.inl').read_text()
    paid = (runtime / 'native_paid_ammo_release_t20.inl').read_text()
    pins = [(int(m[1], 16), bytes(int(x.strip(), 0) for x in m[2].split(',')))
            for m in re.finditer(r'\{(0x[0-9a-fA-F]+),\{([^}]+)\}\}', transient + '\n' + paid)]
    assert all(pe.read(v, len(b)) == b for v, b in pins)
    for n in ['pins_t20.inc', 'paid_pins_t20.inc']:
        (out / n).write_text('\n'.join('{0x%X,{%s}},' % (v, ','.join('0x%02X' % b for b in data)) for v, data in pins))
    (out / 'factory_capture_paid_t20.inc').write_text(launch[launch.index('using CreateAmmoFnT17='):launch.index('static bool InstallAmmoLaunchT17()')])
    fixtures = [('kind', ROOT / 'tests/subsystems/throw_drop/knife_native_kind_k40_test.cpp')]
    for mode, stub in [('preview', 'static bool AmmoKindsT17(void* l,void* a){return kind&&l==&launcher&&a==&source;}'),
                       ('paid', 'static bool AmmoKindsT17(void* l,void* p){return kind&&l==&launcher&&p==&payload;}')]:
        original = ROOT / ('tests/subsystems/throw_drop/native_transient_ammo_t20_test.cpp' if mode == 'preview' else
                           'tests/subsystems/throw_drop/native_paid_ammo_release_t20_test.cpp')
        body = original.read_text()
        assert body.count(stub) == 1
        body = body.replace(stub, 'static bool AmmoKindsT17(void*,void*) noexcept;')
        body = body.replace('#include <optional>', '#include <optional>\n#include <vector>')
        body = body.replace('static unsigned char* image=nullptr;', 'static unsigned char* image=nullptr;\nstruct Memory;')
        # Add a real launcher definition to the existing native-identity fixture.
        marker = 'struct alignas(16) Memory {unsigned char b[0x400]{};};'
        assert body.count(marker) == 1
        body = body.replace(marker, marker + '\nstatic Memory launcherDefinitionK40;')
        needle = 'switch(rva){'
        assert body.count(needle) == 1
        body = body.replace(needle, needle + 'case 0x469590:return reinterpret_cast<' + ('F' if mode == 'paid' else 'F') + '>(WhBase()+rva);')
        include = '#include "subsystems/throw_drop/runtime/native_transient_ammo_t20.inl"'
        body = body.replace(include, '#include "actual_ammo_kinds_k40.inc"\n#include "native_kind_loader_k40.inc"\n' + include)
        body = body.replace('LoadPins();', '''LoadPins();LoadKindCodeK40();
 Put(&launcherDefinitionK40,0,WhBase()+0x3A4D890);Put(&launcher,0,WhBase()+0x3A2DA88);Put(&launcher,0x48,static_cast<void*>(&launcherDefinitionK40));''')
        body = body.replace('PAGE_READWRITE)', 'PAGE_EXECUTE_READWRITE)')
        if mode == 'preview':
            needle = 'static t20::Status Run(){const auto v='
            assert body.count(needle) == 1
            body = body.replace(needle, 'static t20::Status Run(){if(!kind)Put(&launcherDefinitionK40,0,WhBase()+0x3BA6EE8);const auto v=')
            if a.old_kind_control:
                body = body.replace('LegacyUntouched();return v;}', 'LegacyUntouched();std::fprintf(stderr,"K40_OLD_PREVIEW status=%u boundary=%s cloneCalls=%d sourceRetired=%d\\n",unsigned(v),receipt.boundaryD66,cloneCalls,int(receipt.sourceAbsent));return v;}')
        elif a.old_kind_control:
            body = body.replace('return AmmoPaidReleaseT20(ctx,prepared,v,receipt);}', 'const auto status=AmmoPaidReleaseT20(ctx,prepared,v,receipt);std::fprintf(stderr,"K40_OLD_PAID status=%u cost=%u shoot=%u\\n",unsigned(status),costCalls,shootCalls);return status;}')
        f = out / (mode + '_real_kind_k40.cpp')
        f.write_text(body)
        fixtures.append((mode, f))
    results = []
    for mode, fixture in fixtures:
        for variant, options in [('debug', '/Od'), ('asan', '/O2 /fsanitize=address')]:
            folder = out / (mode + '_' + variant)
            folder.mkdir(exist_ok=True)
            cmd = folder / 'run.cmd'
            cmd.write_text('@echo off\ncall "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat" >nul\n'
                'cl /nologo /std:c++20 /EHsc /Zi /UNDEBUG ' + options + ' /I "' + str(ROOT/'src') + '" /I "' + str(out) +
                '" "' + str(fixture) + '" /Fe:test.exe >build.log 2>&1\nif errorlevel 1 exit /b %errorlevel%\ntest.exe >run.log 2>&1\nexit /b %errorlevel%\n')
            p = subprocess.run(['cmd.exe', '/d', '/c', str(cmd)], cwd=folder)
            build = (folder/'build.log').read_text(errors='replace')
            log = (folder/'run.log').read_text(errors='replace') if (folder/'run.log').exists() else ''
            print(mode, variant, p.returncode, build[-1200:], log[-1800:])
            if a.old_kind_control:
                expected = ('FAILED check=122' if mode == 'kind' else
                    'K40_OLD_PREVIEW status=4 boundary=CLONE_NATIVE_KINDS cloneCalls=0 sourceRetired=1' if mode == 'preview' else
                    'K40_OLD_PAID status=1 cost=0 shoot=0')
                ok = p.returncode != 0 and 'error C' not in build and expected in log
            else:
                ok = p.returncode == 0 and any(x.startswith('{"checks":') for x in log.splitlines())
            row = dict(mode=mode, variant=variant, passed=ok, exit=p.returncode, negative=a.old_kind_control)
            if p.returncode == 0:
                row['fixture'] = json.loads(next(x for x in log.splitlines() if x.startswith('{"checks":')))
            results.append(row)
    report = dict(passed=all(x['passed'] for x in results), results=results, old_kind_control=a.old_kind_control,
        game_run=False, exact_retail_predicates_execute=True, native_definition_getter='SUBSTITUTED',
        native_creation_payment_shoot_vm_device='SUBSTITUTED', retail_sha256=SHA,
        source_sha256={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [
            runtime/'native_ammo_launch_t17.inl', runtime/'native_transient_ammo_t20.inl', runtime/'native_paid_ammo_release_t20.inl',
            ROOT/'tests/subsystems/throw_drop/native_transient_ammo_t20_test.cpp', ROOT/'tests/subsystems/throw_drop/native_paid_ammo_release_t20_test.cpp',
            ROOT/'tests/subsystems/throw_drop/knife_native_kind_k40_test.cpp', Path(__file__)]})
    (out / 'RESULTS.json').write_text(json.dumps(report, indent=2))
    return 0 if report['passed'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
