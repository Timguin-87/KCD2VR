#!/usr/bin/env python3
"""Offline audit against the supplied archive/DLL. Never run inside KCD2.
Checks bytes/metadata only, not live hook safety or menu visibility semantics.
Usage: python tools/audit_ui89_supplied_assets.py --archive ... --dll ... --output ...
The original assets are NOT bundled with this source package.
"""
from __future__ import annotations
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import struct
import sys
import xml.etree.ElementTree as ET
from zipfile import ZipFile
import zlib

ROOT = Path(__file__).resolve().parents[1]

def sha(b: bytes) -> str:
    return hashlib.sha256(b).hexdigest()

def stage(raw: bytes) -> dict:
    signature = raw[:3].decode('ascii')
    if signature in ('CFX', 'CWS'):
        body = zlib.decompress(raw[8:])
    elif signature in ('GFX', 'FWS'):
        body = raw[8:]
    else:
        raise ValueError(f'Unsupported GFX signature {signature!r}')
    pos = 0
    def bits(n: int, signed: bool = False) -> int:
        nonlocal pos
        v = 0
        for _ in range(n):
            v = (v << 1) | ((body[pos // 8] >> (7 - pos % 8)) & 1)
            pos += 1
        return v - (1 << n) if signed and v & (1 << (n - 1)) else v
    width = bits(5)
    r = [bits(width, True) for _ in range(4)]
    return {'signature': signature, 'rect_twips': r,
            'width': (r[1]-r[0])/20, 'height': (r[3]-r[2])/20}

class PE:
    def __init__(self, raw: bytes):
        self.raw = raw
        if raw[:2] != b'MZ': raise ValueError('Not a PE file')
        nt = struct.unpack_from('<I', raw, 0x3c)[0]
        if raw[nt:nt+4] != b'PE\0\0': raise ValueError('Invalid PE header')
        sections, self.timestamp = struct.unpack_from('<HI', raw, nt+6)
        opt_size = struct.unpack_from('<H', raw, nt+20)[0]
        opt = nt+24
        if struct.unpack_from('<H', raw, opt)[0] != 0x20b: raise ValueError('Expected PE32+')
        self.base = struct.unpack_from('<Q', raw, opt+24)[0]
        self.import_rva = struct.unpack_from('<I', raw, opt+120)[0]
        self.sections = []
        for i in range(sections):
            _, vs, va, size, offset = struct.unpack_from('<8sIIII', raw, opt+opt_size+40*i)
            self.sections.append((va, max(vs,size), offset, size))
    def off(self, rva: int) -> int:
        for va, extent, offset, size in self.sections:
            if va <= rva < va+extent:
                if rva-va >= size: raise ValueError('RVA is not backed by file bytes')
                return offset+rva-va
        raise ValueError(f'Unmapped RVA {rva:#x}')
    def read(self, rva: int, n: int) -> bytes:
        p = self.off(rva)
        return self.raw[p:p+n]
    def qword(self, rva: int) -> int:
        return struct.unpack('<Q', self.read(rva,8))[0]
    def cstr(self, rva: int) -> str:
        p = self.off(rva)
        return self.raw[p:self.raw.index(0,p)].decode('ascii')
    def imports(self) -> dict:
        out = {}
        for i in range(4096):
            desc = struct.unpack('<IIIII', self.read(self.import_rva+i*20,20))
            oft, _, _, name, iat = desc
            if not any(desc): return out
            dll = self.cstr(name).lower()
            for j in range(65536):
                thunk = self.qword((oft or iat)+j*8)
                if not thunk: break
                if thunk & (1 << 63): symbol = f'ordinal:{thunk & 0xffff}'
                else: symbol = self.cstr(thunk+2)
                out[(dll,symbol)] = iat+j*8
        raise ValueError('Unterminated import descriptors')

def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--archive', type=Path, required=True)
    ap.add_argument('--dll', type=Path, required=True)
    ap.add_argument('--output', type=Path, required=True)
    args = ap.parse_args()
    checks = []
    def check(name: str, passed: bool, details=None):
        checks.append({'check': name, 'passed': bool(passed), 'details': details})
    cat = json.loads((ROOT/'data/native_ui_catalogue.json').read_text())
    archive_raw = args.archive.read_bytes()
    check('archive SHA-256', sha(archive_raw)==cat['source_sha256'], sha(archive_raw))
    with ZipFile(args.archive) as z:
        xml_paths = [p for p in z.namelist() if '/UIElements/' in p and p.lower().endswith('.xml')]
        check('exact 28 XML definitions', len(xml_paths)==28, len(xml_paths))
        entries = {}
        for p in xml_paths:
            b = z.read(p); el = ET.fromstring(b).find('UIElement')
            if el is None: raise ValueError(f'No UIElement: {p}')
            entries[el.attrib['name']] = (p,b,el)
        check('no omitted or extra definitions', set(entries)=={e['element'] for e in cat['elements']})
        for e in cat['elements']:
            p, raw, el = entries[e['element']]
            gf = el.find('GFx'); constraints = el.find('GFx/Constraints')

            if gf is None or constraints is None: raise ValueError(f'Missing GFx/Constraints: {p}')
            align = constraints.find('Align'); position = constraints.find('Position')
            gfx_path = str(PurePosixPath(p).parents[1] / gf.attrib['file'])
            gfx = z.read(gfx_path)
            checks_for_element = {
                'xml_hash': sha(raw)==e['xml_sha256'],
                'gfx_hash': sha(gfx)==e['gfx_sha256'],
                'native_attributes': el.attrib==e['native_attributes'],
                'gfx_attributes': gf.attrib==e['gfx'],
                'alignment': (align.attrib if align is not None else {})==e['align'],
                'position_constraints': (position.attrib if position is not None else {})==e['position_constraints'],
                'logical_stage': stage(gfx)==e['logical_stage'],
            }
            check('native metadata '+e['element'], all(checks_for_element.values()), checks_for_element)
        profile = ET.fromstring(z.read('IPL gamedata.pak/Libs/Config/defaultProfile.xml'))
        loading = next(e for e in profile.iter('actionmap') if e.attrib.get('name')=='loading')
        actions = {e.attrib['name']:e.attrib for e in loading.findall('action')}
        check('loading is exclusive overlays actionmap', loading.attrib.get('exclusivity')=='1', loading.attrib)
        check('native loading skip uses xi_b', actions['loading_video_skip'].get('xboxpad')=='xi_b',actions['loading_video_skip'])
        supers = ET.fromstring(z.read('IPL gamedata.pak/Libs/Config/keybindSuperactions.xml'))
        skip = next((e for e in supers.iter('superaction')
                     if any(c.attrib.get('name')=='loading_video_skip' for c in e.findall('action'))),None)
        if skip is None: raise ValueError('No loading_video_skip superaction')
        serialized = ET.tostring(skip, encoding='unicode')
        controls = [c.attrib for c in skip.findall('control')]
        check('fallback loading skip matches Space',
              any(c.get('controller')=='keyboard' and c.get('input')=='space' for c in controls),
              {'superaction':skip.attrib.get('name'),'controls':controls,'xml':serialized})
    raw=args.dll.read_bytes(); pe=PE(raw)
    check('retail DLL SHA-256',sha(raw)=='bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7',sha(raw))
    check('retail timestamp',pe.timestamp==0x6a350e20,hex(pe.timestamp))
    old=json.loads((ROOT/'validation/ui88/static_audit.json').read_text())
    for c in old['checks']:
        d=c.get('details')
        if isinstance(d,dict) and 'rva' in d and 'entry' in d:
            rva=int(d['rva'],16); expected=bytes.fromhex(d['entry']); actual=pe.read(rva,len(expected))
            check('unchanged native target '+c['check'],actual==expected,{'rva':hex(rva),'actual':actual.hex(' ')})
    for slot,target in ((0,0x16e94b0),(0x50,0x72dd20),(0xe0,0x568510),(0xe8,0x1a7d900)):
        check(f'Flash vtable slot {slot:#x}',pe.qword(0x4049bd0+slot)==pe.base+target,hex(target))
    imports=pe.imports()
    for name,rva in [('XInputGetCapabilities',0x3a046d8),('XInputGetState',0x3a046e0),('XInputSetState',0x3a046e8)]:
        actual=imports.get(('xinput9_1_0.dll',name))
        check('unchanged XInput import '+name,actual==rva,hex(actual) if actual is not None else None)
    report={'status':'STATIC FILE CHECKS ONLY: not a Windows SDK build, live ABI validation or headset test',
            'checks':checks,'passed':sum(c['passed'] for c in checks),'total':len(checks)}
    args.output.parent.mkdir(parents=True,exist_ok=True)
    args.output.write_text(json.dumps(report,indent=2)+'\n')
    print(f"{report['passed']}/{report['total']} supplied-asset checks passed")
    for c in checks:
        if not c['passed']: print('FAIL:',c['check'])
    return 0 if report['passed']==report['total'] else 1

if __name__=='__main__':
    try: sys.exit(main())
    except (OSError,ValueError,KeyError,StopIteration,ET.ParseError) as e:
        print(f'Audit failed: {e}',file=sys.stderr);sys.exit(2)
