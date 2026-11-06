#!/usr/bin/env python3
"""Revalidate every compiled T4 phase pin against PE bytes; never load the DLL."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import argparse,ast,hashlib,json,re,struct
from verify_native_grasp13 import PE,EXPECTED_SHA,EXPECTED_TS

def verify(dll):
    pe=PE(Path(dll));assert hashlib.sha256(pe.data).hexdigest()==EXPECTED_SHA and pe.timestamp==EXPECTED_TS
    source=Path(__file__).resolve().parents[1]/'src/native_physics_phase_t4.cpp'
    text=read_source(source);checks=[]
    for rva,literal,count in re.findall(r'\{0x([0-9A-Fa-f]+),"((?:\\.|[^"\\])+)",(\d+)\}',text):
        rva=int(rva,16);wanted=ast.literal_eval('b"'+literal+'"');assert len(wanted)==int(count)
        actual=pe.at(rva,len(wanted));checks.append(dict(name='compiled pin',rva=hex(rva),expected=wanted.hex(),actual=actual.hex(),passed=actual==wanted))
    assert len(checks)>=29,'missing compiled phase/suspension pins'
    slots=[(0x407FC80,0xD8,0x6EFD50),(0x407FC80,0x10,0x38DC0B4),(0x407FC80,0xB0,0x667418),(0x407FC80,0xA8,0x52950C),
           (0x3A48DC0,0x10,0x5311C4),(0x3A48DC0,0x18,0x531F84),(0x3A48DC0,0x30,0x8A6C90),(0x3A48DC0,0x38,0xAC0D18),
           (0x3A578C0,8,0x1A72470),(0x3A578C0,0x50,0x5D6F44),(0x3A578C0,0x58,0x82BED0),
           (0x3A82570,8,0x1A72500),(0x3A82570,0x28,0x3D5384)]
    for table,slot,target in slots:
        actual=pe.q(table+slot)-pe.base;checks.append(dict(name='compiled table contract',rva=hex(table+slot),expected=hex(target),actual=hex(actual),passed=actual==target))
    # Fresh body ownership / suspension schema and queue-consumer evidence.
    for rva,wanted,name in [(0x8471BE,'ff90a0000000','destroy retires physical ID before ref-governed memory disposal'),
                            (0x6F026D,'41b801000000','RemoveGeometry native consumer already-safe arg1'),
                            (0x6F027A,'ff5048','RemoveGeometry native consumer slot48'),
                            (0x6F0293,'c744242001000000','AddGeometry native consumer already-safe arg1'),
                            (0x6F029B,'ff5040','AddGeometry native consumer slot40'),
                            (0x8A6DC8,'498b4c0048','status26 buddy from live source constraint+48'),
                            (0xABF66E,'4c897e48','native create stores buddy at constraint+48'),
                            (0xABF771,'4c89640750','status26 output40 user pointer initialized null'),
                            (0x6F0150,'83f901','queued destroy opcode branch has ECX1'),
                            (0x6F015C,'448bc9','queued destroy sets already-safe R9D1'),
                            (0x6F0168,'ff9098000000','queued destroy invokes world98'),
                            (0x847120,'85db750f','already-safe fourth argument skips native guard acquisition'),
                            (0x847133,'458bc6','native guard acquire flag is zero in already-safe path'),
                            (0x50B657,'4585c07417','native guard constructor skips lock when flag zero'),
                            (0x6F14A0,'8b4254','deferred body deletion reads independent reference54'),
                            (0x6F14A3,'85c0','test independent reference count'),
                            (0x6F14A5,'7f36','positive strong reference skips native body deletion'),
                            (0x6F14D7,'ff90f8000000','native deletion occurs only beyond positive-ref guard')]:
        actual=pe.at(rva,len(bytes.fromhex(wanted))).hex();checks.append(dict(name=name,rva=hex(rva),expected=wanted,actual=actual,passed=actual==wanted))
    assert all(c['passed'] for c in checks),[c for c in checks if not c['passed']]
    return dict(sha256=EXPECTED_SHA,timestamp=hex(pe.timestamp),native_execution=False,
                source=str(source),source_sha256=hashlib.sha256(read_source_bytes(source)).hexdigest(),
                passed=len(checks),total=len(checks),all_passed=True,checks=checks)

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('dll');p.add_argument('--out');a=p.parse_args();r=verify(a.dll)
    if a.out:Path(a.out).parent.mkdir(parents=True,exist_ok=True);Path(a.out).write_text(json.dumps(r,indent=2)+'\n')
    print(f"T4 production native phase pins {r['passed']}/{r['total']} PASS; native execution=NONE")
