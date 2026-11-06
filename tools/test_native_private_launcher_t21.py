"""Qualify T21 retail bytes as data, then run its actual inl with native leaves stubbed.

No game DLL is loaded, no ASI/PAK is built and no installation is performed.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import struct
import subprocess
import uuid
from native_research112 import PE

EXPECTED_SHA='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--dll',type=Path,required=True)
    ap.add_argument('--build-dir',type=Path,required=True)
    ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--resume',action='store_true',help='Run the additional K37 phase-resumption fixture')
    args=ap.parse_args()
    root=Path(__file__).resolve().parents[1]
    build=args.build_dir.resolve()
    if build==root or root in build.parents:raise SystemExit('External build directory required')
    build.mkdir(parents=True,exist_ok=True)
    source=root/'src/subsystems/throw_drop/runtime/native_private_launcher_t21.inl'
    header=root/'src/subsystems/throw_drop/native_private_launcher_t21.h'
    shared=root/'src/subsystems/throw_drop/runtime/native_transient_ammo_t20.inl'
    fixture=root/'tests/subsystems/throw_drop'/('native_private_launcher_k37_resume_test.cpp' if args.resume else 'native_private_launcher_t21_test.cpp')
    ownpins=[(int(m[1],16),bytes(int(x.strip(),0) for x in m[2].split(',')))
             for m in re.finditer(r'\{(0x[0-9A-Fa-f]+),\{([^}]+)\}\}',source.read_text())]
    sharedpins=[(int(m[1],16),bytes(int(x.strip(),0) for x in m[2].split(',')))
                for m in re.finditer(r'\{(0x[0-9A-Fa-f]+),\{([^}]+)\}\}',shared.read_text())]
    assert len(ownpins)==6 and len(sharedpins)>=10
    p=PE(args.dll);checks=[]
    def ck(name,actual,expected):checks.append(dict(name=name,actual=actual,expected=expected,passed=actual==expected))
    ck('exact retail SHA256',hashlib.sha256(p.data).hexdigest(),EXPECTED_SHA)
    ck('exact retail PE timestamp',p.stamp,0x6A350E20)
    for rva,value in ownpins+sharedpins:ck(f'production pin {rva:X}',p.read(rva,len(value)).hex(),value.hex())
    for vft,off,fn in [(0x3A5C838,8,0x1A78ED0),(0x3A5C838,0x80,0x838AE0),
                       (0x3A4D890,0x108,0x5F5DA0)]:
        ck(f'actual native vft {vft:X}+{off:X}',struct.unpack('<Q',p.read(vft+off,8))[0]-p.base,fn)
    extra=[
        (0x8D3DDE,'418d4818'),
        (0x8D3E0A,'488d05278a180348c7410800000000488901c6411000'),
        (0x28F068B,'e83437fefd'),
        (0x28F069D,'48894308'),
        (0x28F06B9,'e8262db6fd'),
        (0x28F06BE,'488b4b08488d5424704533c9458d411ce8ed58b7fd'),
        (0x28F0740,'488d4d48e897d654ff'),
        (0x28F0749,'4c8b4508488d5508488d4d084d8b4008e85627b6fd'),
        (0x28F075E,'488b4d08ba30000000e8244d1cff'),
        (0x468387,'488d0440488d0cc2'),
        (0x468409,'488b4310'),
    ]
    for rva,value in extra:ck(f'exact native caller/schema {rva:X}',p.read(rva,len(bytes.fromhex(value))).hex(),value)
    text=header.read_text();m=re.search(r'launcherGuid\[16\]\{([^}]+)\}',text)
    actual=bytes(int(x.strip(),0) for x in m[1].split(','))
    ck('own launcher native GUID layout',actual.hex(),uuid.UUID('dc877f56-16f3-5889-b606-5617658717cc').bytes_le.hex())
    report=dict(schema=1,native_execution=False,enabled=False,installed=False,native_checks=checks,fixture=None,
                limits=['Native leaves are substituted in the ASAN host fixture.',
                        'No cross-save persistence or physical/native damage acceptance claim.',
                        'Original manager/world address-pair reuse refuses; no invented generation proof.',
                        'Quarantined wrapper allocation is 0x18=24 bytes; unknown record construction may retain native auxiliary allocations.'],
                input_sha256={str(f.relative_to(root)):hashlib.sha256(f.read_bytes()).hexdigest()
                              for f in [source,header,fixture,shared,root/'src/native_script_t2.h']})
    args.out.parent.mkdir(parents=True,exist_ok=True)
    if not all(c['passed'] for c in checks):
        report['all_passed']=False;args.out.write_text(json.dumps(report,indent=2)+'\n')
        print(json.dumps([c for c in checks if not c['passed']],indent=2));return 1
    (build/'pins_t21.inc').write_text('\n'.join('{0x%X,{%s}},'%(rva,','.join('0x%02x'%b for b in value)) for rva,value in ownpins)+'\n')
    vcvars=Path('C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvars64.bat')
    cmd=build/'run.cmd'
    cmd.write_text('@echo off\ncall "'+str(vcvars)+'" >nul\nif errorlevel 1 exit /b %errorlevel%\n'
                   'cd /d "'+str(build)+'"\ncl /nologo /std:c++20 /EHsc /Od /Zi /UNDEBUG /fsanitize=address '
                   '/I "'+str(root/'src')+'" /I "'+str(build)+'" "'+str(fixture)+'" /Fe:test.exe\n'
                   'if errorlevel 1 exit /b %errorlevel%\ntest.exe\nexit /b %errorlevel%\n')
    result=subprocess.run(['cmd.exe','/d','/c',str(cmd)],text=True,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
    (build/'RUN.log').write_text(result.stdout)
    outputs=[s for s in result.stdout.splitlines() if s.startswith('{"checks":')]
    report['fixture']=json.loads(outputs[-1]) if outputs else None
    report['fixture_exit_code']=result.returncode
    report['all_passed']=result.returncode==0 and bool(report['fixture'])
    report['run_log']=str(build/'RUN.log')
    args.out.write_text(json.dumps(report,indent=2)+'\n')
    print(result.stdout);print('Native static checks: %d/%d'%(sum(c['passed'] for c in checks),len(checks)))
    return 0 if report['all_passed'] else 1


if __name__=='__main__':raise SystemExit(main())
