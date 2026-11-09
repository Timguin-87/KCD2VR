#!/usr/bin/env python3
"""R154: one setting, five places, same key and same default.

checks, without arguments, from the repo root:
  1. KCD2VR.ini (shipped starter INI) key -> value
  2. src/subsystems/experimental/live_settings_d39.h: Settings struct defaults, Id enum, every page's rows,
     advance()/label() cases
  3. src/subsystems/experimental/live_runtime_d40.inl: READ (field <- owner), LOAD (owner <- INI key, default),
     SAVE (INI key <- owner)
  4. every ReadIni*Explicit(...) in src/ (mod.cpp, subsystem host.inl / config.inl ...): key spelling + default
  5. docs/*.md + README.md: every `VrKey` mentioned must exist in the INI or be read by the code

mismatches that are real contradictions exit 1. a difference that is a tuned choice is listed in ALLOW with the
reason, so the script stays quiet about it (edit ALLOW when the choice changes on purpose).
no files are written.
"""
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'src'
D39 = SRC / 'subsystems' / 'experimental' / 'live_settings_d39.h'
D40 = SRC / 'subsystems' / 'experimental' / 'live_runtime_d40.inl'
INI = ROOT / 'KCD2VR.ini'

# key -> reason. a key listed here may differ between the shipped INI and the loader default on purpose.
ALLOW = {
    # the shipped INI is Miki's own INI (R124); the loader default only applies when the key is missing
    'VrEngineClarity': 'his tuned image value (0.24), loader fallback 0.18',
    'VrHudStatusIconsVisible': 'his INI shows the status icons; loader fallback false',
    'VrHudWalkoffD70': 'his INI has HUD event-only updates ON; loader fallback false',
    'VrPitchCorrectionDeg': 'his headset pitch correction (5.5 deg), loader fallback 0',
    'VrSourceWidth': 'his headset source width, the setup window sets it per headset',
    'VrUiTopBoundary': 'migrated by VrHudLayoutVersion 1202 (mod.cpp), loader fallback is the pre-migration value',
    'VrUiBottomBoundary': 'migrated by VrHudLayoutVersion 1202 (mod.cpp), loader fallback is the pre-migration value',
    'StartupUpscaler': '-1 = keep the game upscaler; -99 in the code = key missing',
    'EnableVrikSemanticOwnershipTest': '[Experimental] switch kept ON in the shipped INI, code fallback false',
}
# Ids that have no row on purpose (retired or status-only), see the comments in live_settings_d39.h
IDS_WITHOUT_ROW = {'None', 'MapRegion', 'NpcTouchD40', 'LivingTouchD40', 'CombatTurnStatusD40', 'KnifeRightHipD66'}
# INI keys mentioned in docs that the code no longer reads (the docs say so)
DOC_KEYS_RETIRED = {'VrCrossbowHandOnlyR89'}

errors = []
notes = []


def err(msg):
    errors.append(msg)


def note(msg):
    notes.append(msg)


def read(path):
    return path.read_bytes().decode('utf-8', 'replace').replace('\r\n', '\n')


def norm(value):
    """one spelling for a default: true/false/1/0 -> 'true'/'false', numbers -> float text, rest as is."""
    v = value.strip().rstrip(';').strip()
    v = re.sub(r'^\(?\s*(.*?)\s*\)?$', r'\1', v) if v.startswith('(') and v.endswith(')') else v
    low = v.lower()
    if low in ('true', 'false'):
        return low
    m = re.fullmatch(r'[-+]?(\d+\.?\d*|\.\d+)(e[-+]?\d+)?f?', low)
    if m:
        return repr(float(low.rstrip('f')))
    return v


def balanced_args(text, start):
    """text[start] is '('; return the argument list split at depth-0 commas and the index after ')'."""
    depth = 0
    args = []
    cur = []
    i = start
    in_str = False
    while i < len(text):
        c = text[i]
        if in_str:
            cur.append(c)
            if c == '\\':
                cur.append(text[i + 1])
                i += 1
            elif c == '"':
                in_str = False
        elif c == '"':
            in_str = True
            cur.append(c)
        elif c in '([{':
            depth += 1
            if depth > 1:
                cur.append(c)
        elif c in ')]}':
            depth -= 1
            if depth == 0:
                args.append(''.join(cur).strip())
                return args, i + 1
            cur.append(c)
        elif c == ',' and depth == 1:
            args.append(''.join(cur).strip())
            cur = []
        else:
            cur.append(c)
        i += 1
    raise ValueError('unbalanced call at %d' % start)


# ---------------------------------------------------------------- 1. INI
def parse_ini(path):
    keys = {}
    section = ''
    for n, line in enumerate(read(path).split('\n'), 1):
        s = line.strip()
        if not s or s.startswith(';') or s.startswith('#'):
            continue
        if s.startswith('['):
            section = s.strip('[]').strip()
            continue
        if '=' not in s:
            continue
        k, v = s.split('=', 1)
        k = k.strip()
        v = v.split(';')[0].strip()
        if k in keys:
            err('INI: %s written twice (lines %d and %d)' % (k, keys[k][2], n))
        keys[k] = (section, v, n)
    return keys


# ---------------------------------------------------------------- 4. source reads / saves
def collect_constants():
    consts = {}
    for p in SRC.rglob('*'):
        if p.suffix not in ('.h', '.inl', '.cpp', '.hpp'):
            continue
        for m in re.finditer(r'constexpr\s+const\s+char\s*\*\s*(\w+)\s*=\s*"([^"]+)"', read(p)):
            consts.setdefault(m.group(1), set()).add(m.group(2))
    return consts


def resolve_key(expr, consts):
    expr = expr.strip()
    if expr.startswith('"') and expr.endswith('"'):
        return expr[1:-1]
    name = expr.split('::')[-1]
    if name in consts and len(consts[name]) == 1:
        return next(iter(consts[name]))
    if name in consts:
        # several namespaces use the same constant name (kKey): pick by namespace prefix
        for ns in reversed(expr.split('::')[:-1]):
            for p in SRC.rglob('*.h'):
                t = read(p)
                if re.search(r'namespace\s+[\w:]*\b' + re.escape(ns) + r'\s*\{', t):
                    m = re.search(r'constexpr\s+const\s+char\s*\*\s*' + name + r'\s*=\s*"([^"]+)"', t)
                    if m:
                        return m.group(1)
    return None


def collect_reads(consts):
    """returns list of (key, section, type, default_text, file:line, raw_default)"""
    reads = []
    for p in sorted(SRC.rglob('*')):
        if p.suffix not in ('.h', '.inl', '.cpp', '.hpp'):
            continue
        text = read(p)
        for m in re.finditer(r'ReadIni(Bool|Int|Float)Explicit\s*\(', text):
            try:
                args, _ = balanced_args(text, m.end() - 1)
            except ValueError:
                continue
            if len(args) != 4:
                continue
            line = text.count('\n', 0, m.start()) + 1
            where = '%s:%d' % (p.relative_to(ROOT).as_posix(), line)
            section = resolve_key(args[1], consts) or args[1]
            key = resolve_key(args[2], consts)
            if key is None:
                continue  # key from a variable (a table walk, the helper itself): not checked here
            reads.append((key, section, m.group(1), args[3], where))
        # DetourModKit binder: <section>.bind("Key", atomic, default) (mod.cpp / subsystem host.inl),
        # and the small load("Key", atomic, default) lambdas
        for m in re.finditer(r'\b(settings|future|experimental|load)\.?\s*(?:bind)?\s*\(\s*"', text):
            try:
                args, _ = balanced_args(text, text.index('(', m.start()))
            except ValueError:
                continue
            if len(args) != 3 or not args[0].startswith('"'):
                continue
            section = {'future': 'FutureFeatures', 'experimental': 'Experimental'}.get(m.group(1), 'Settings')
            line = text.count('\n', 0, m.start()) + 1
            where = '%s:%d' % (p.relative_to(ROOT).as_posix(), line)
            default = args[2].strip()
            vm = re.match(r'(?:kcd2vr::)?view_defaults_b2::Get\("(\w+)"\s*,\s*([^)]+)\)', default)
            if vm:
                default = view_defaults().get(vm.group(1), vm.group(2))
                if vm.group(1) != args[0].strip('"'):
                    err('view default read under another key: %s uses the table entry %s at %s' % (args[0], vm.group(1), where))
            typ = 'Bool' if default in ('true', 'false') else ('Float' if re.search(r'f$|\.', default) else 'Int')
            reads.append((args[0].strip('"'), section, typ, default, where))
        for m in re.finditer(r'GetPrivateProfile(String|Int)[AW]\s*\(\s*L?"(\w+)"\s*,\s*L?"(\w+)"\s*,\s*L?("[^"]*"|[-\d.]+)', text):
            line = text.count('\n', 0, m.start()) + 1
            where = '%s:%d' % (p.relative_to(ROOT).as_posix(), line)
            default = m.group(4).strip('"')
            if re.fullmatch(r'[-+]?\d+(\.\d+)?|true|false', default):
                typ = 'Bool' if default in ('true', 'false') else ('Float' if '.' in default else 'Int')
                reads.append((m.group(3), m.group(2), typ, default, where))
            else:
                reads.append((m.group(3), m.group(2), 'String', default, where))
    return reads


_view_defaults = None


def view_defaults():
    global _view_defaults
    if _view_defaults is None:
        _view_defaults = {}
        text = read(SRC / 'subsystems' / 'base' / 'view_defaults_b2.h')
        for m in re.finditer(r'\{"(\w+)",([-\d.]+)\}', text):
            _view_defaults[m.group(1)] = m.group(2)
    return _view_defaults


def collect_saves(consts):
    saves = []
    for p in sorted(SRC.rglob('*')):
        if p.suffix not in ('.h', '.inl', '.cpp', '.hpp'):
            continue
        text = read(p)
        for m in re.finditer(r'\bw([bif])\s*\(', text):
            try:
                args, _ = balanced_args(text, m.end() - 1)
            except ValueError:
                continue
            if len(args) != 2:
                continue
            key = resolve_key(args[0], consts)
            line = text.count('\n', 0, m.start()) + 1
            where = '%s:%d' % (p.relative_to(ROOT).as_posix(), line)
            if key is None:
                continue
            saves.append((key, m.group(1), args[1], where))
        for m in re.finditer(r'WritePrivateProfileStringA\s*\(\s*"Settings"\s*,\s*"(\w+)"', text):
            line = text.count('\n', 0, m.start()) + 1
            saves.append((m.group(1), 's', '', '%s:%d' % (p.relative_to(ROOT).as_posix(), line)))
    return saves


# ---------------------------------------------------------------- 2. live_settings_d39.h
def parse_struct_defaults(text):
    start = text.index('struct Settings {')
    end = text.index('\n};', start)
    body = text[start:end]
    defaults = {}
    for line in body.split('\n')[1:]:
        line = line.split('//')[0].strip()
        if not line:
            continue
        for stmt in line.split(';'):
            stmt = stmt.strip()
            if not stmt:
                continue
            m = re.match(r'(bool|int|float|unsigned)\s+(.*)$', stmt)
            if not m:
                continue
            for decl in m.group(2).split(','):
                decl = decl.strip()
                if '[' in decl:
                    continue
                if '=' in decl:
                    name, val = decl.split('=', 1)
                    defaults[name.strip()] = (m.group(1), norm(val))
                else:
                    defaults[decl] = (m.group(1), None)
    return defaults


def parse_ids(text):
    m = re.search(r'enum class Id:unsigned \{(.*?)\n\};', text, re.S)
    return [i.strip() for i in m.group(1).replace('\n', '').split(',') if i.strip()]


def parse_rows(text):
    rows = []
    for m in re.finditer(r'\{Id::(\w+),L"((?:[^"\\]|\\.)*)",L"((?:[^"\\]|\\.)*)"(?:,(true|false))?(?:,(true|false))?\}', text):
        line = text.count('\n', 0, m.start()) + 1
        rows.append((m.group(1), m.group(2), line, m.group(5) == 'true'))
    return rows


def parse_cases(text, func):
    start = text.index(func)
    end = text.index('\n}', start)
    body = text[start:end]
    ids = set()
    for m in re.finditer(r'case Id::(\w+)', body):
        ids.add(m.group(1))
    return ids, body


# ---------------------------------------------------------------- 3. live_runtime_d40.inl
def owner_of_getter(expr):
    e = expr.strip().rstrip(';')
    e = re.sub(r'^(int|unsigned|bool|float)\((.*)\)$', r'\2', e)
    e = re.sub(r'\.load\([^)]*\)$', '', e)
    e = re.sub(r'\(\)$', '', e)
    e = e.replace('kcd2vr::', '')
    return e


def owner_of_setter(expr):
    e = expr.strip().replace('kcd2vr::', '')
    m = re.match(r'(.*)\.store$', e)
    if m:
        return m.group(1)
    m = re.match(r'(.*::)[Ss]et([A-Z]\w*)$', e)
    if m:
        name = m.group(2)
        return [m.group(1) + name, m.group(1) + name[0].lower() + name[1:]]
    m = re.match(r'(.*::)Set$', e)
    if m:
        return m.group(1) + 'Get'
    return e


def parse_d40(text, consts):
    blocks = {}
    for name in ('READ', 'APPLY', 'SAVE', 'LOAD'):
        s = text.index('KCD2VR_D40_LIVE_' + name)
        e = text.find('#elif', s)
        if e < 0:
            e = text.find('#else', s)
        blocks[name] = text[s:e]
    field_getter = {}
    for m in re.finditer(r'd40\.(\w+)=([^;]+);', blocks['READ']):
        field_getter[m.group(1)] = owner_of_getter(m.group(2))
    setter_key = {}  # owner string -> (key, default, type)
    load_keys = {}
    for m in re.finditer(r'([\w:]+(?:\.store)?)\(\s*ReadIni(Bool|Int|Float)Explicit\s*\(', blocks['LOAD']):
        args, _ = balanced_args(blocks['LOAD'], m.end() - 1)
        key = resolve_key(args[2], consts)
        owners = owner_of_setter(m.group(1))
        if isinstance(owners, str):
            owners = [owners]
        for o in owners:
            setter_key[o] = (key, args[3], m.group(2))
        load_keys[key] = (args[3], m.group(2))
    # APPLY: the owner each field is stored to must be the owner READ takes it from
    apply_owner = {}
    for m in re.finditer(r'([\w:]+(?:\.store)?)\(\s*(?:unsigned|int|float|bool)?\(?\s*(?:std::clamp\()?d40\.(\w+)', blocks['APPLY']):
        owners = owner_of_setter(m.group(1))
        apply_owner.setdefault(m.group(2), set()).update([owners] if isinstance(owners, str) else owners)
    def stem(name):
        return re.sub(r'setting|selected|enabled|\(.*', '', name.split('::')[-1].lower())
    for field, getter in field_getter.items():
        if field not in apply_owner or getter in apply_owner[field]:
            continue
        g = stem(getter)
        # same owner namespace and the same name stem (rightHipSetting <-> setRightHip, crankTravelSetting(crankTravel) <-> crankTravelSetting)
        if any(o.split('::')[0] == getter.split('::')[0] and (stem(o) in g or g in stem(o) or stem(o) in ('d77', 'mode', 'get', '')) for o in apply_owner[field]):
            continue
        if any(stem(o).startswith('lab58') for o in apply_owner[field]):
            continue  # lab58 flags are applied through frik_pose_probe::SetLab58*
        err('live_runtime: %s is read from %s but applied to %s' % (field, getter, sorted(apply_owner[field])))
    save_keys = set()
    for m in re.finditer(r'\bw[bif]\(\s*([^,]+),', blocks['SAVE']):
        k = resolve_key(m.group(1), consts)
        if k:
            save_keys.add(k)
    return field_getter, setter_key, load_keys, save_keys


# ---------------------------------------------------------------- 5. docs
def doc_files():
    # the history doc describes old states on purpose (keys that existed once): not checked
    return [ROOT / 'README.md'] + [p for p in sorted((ROOT / 'docs').glob('*.md')) if 'HISTORY' not in p.name]


def collect_doc_keys():
    found = {}
    for p in doc_files():
        text = read(p)
        for m in re.finditer(r'`(Vr[A-Za-z0-9_]+)', text):
            found.setdefault(m.group(1), []).append('%s:%d' % (p.relative_to(ROOT).as_posix(), text.count('\n', 0, m.start()) + 1))
    return found


def main():
    ini = parse_ini(INI)
    consts = collect_constants()
    reads = collect_reads(consts)
    saves = collect_saves(consts)
    d39 = read(D39)
    d40 = read(D40)

    # --- key spellings: same key with different case anywhere = two spellings
    all_keys = {}
    for k, *_ in reads:
        all_keys.setdefault(k.lower(), set()).add(k)
    for k, *_ in saves:
        all_keys.setdefault(k.lower(), set()).add(k)
    for k in ini:
        all_keys.setdefault(k.lower(), set()).add(k)
    for low, spellings in sorted(all_keys.items()):
        if len(spellings) > 1:
            err('key spelled two ways: %s' % ' / '.join(sorted(spellings)))

    # --- same key read with two different default literals
    ini_negative_ok = {k for k, (s, v, l) in ini.items() if v.strip().startswith('-')}
    by_key = {}
    for key, section, typ, default, where in reads:
        by_key.setdefault(key, []).append((norm(default), default, where, typ, section))
    for key, lst in sorted(by_key.items()):
        defaults = {}
        for n, raw, where, typ, section in lst:
            # a default that is an expression (another setting, a function) is not a literal: skip it
            if typ == 'String':
                continue
            if n == 'true':
                n = '1.0'
            elif n == 'false':
                n = '0.0'
            if not re.fullmatch(r"[-+]?\d+(\.\d+)?", n):
                continue
            if float(n) < 0 and float(n) == int(float(n)) and key not in ini_negative_ok:
                continue  # -1 / -99: "not set" probe, not a default
            defaults.setdefault(n, []).append(where)
        if len(defaults) > 1:
            # a key read twice with true and then false in the same place is a presence test (R133 VrFreeTargeting,
            # VrPreciseSurfaceContacts): both reads sit within two lines of each other
            places = sorted((w for ws in defaults.values() for w in ws), key=lambda s: (s.rsplit(':', 1)[0], int(s.rsplit(':', 1)[1])))
            if set(defaults) == {'1.0', '0.0'} and len(places) == 2 and places[0].rsplit(':', 1)[0] == places[1].rsplit(':', 1)[0] and abs(int(places[0].rsplit(':', 1)[1]) - int(places[1].rsplit(':', 1)[1])) <= 2:
                continue
            err('read with two defaults: %s: %s' % (key, '; '.join('%s at %s' % (d, ','.join(w)) for d, w in sorted(defaults.items()))))
        types = {typ for _, _, _, typ, _ in lst} - {'String'}
        if 'Float' in types and 'Bool' in types:
            err('read as Float and as Bool: %s: %s' % (key, ', '.join('%s at %s' % (t, w) for _, _, w, t, _ in lst)))

    read_keys = set(by_key)
    saved_keys = {k for k, *_ in saves}

    # --- saved but never read (a save under a different name than the read)
    for key, kind, expr, where in sorted(saves):
        if key not in read_keys and not key.endswith('Adopted') and not key.endswith('Migrated') and 'Defaults' not in key:
            note('saved but never read (retired key still written): %s at %s' % (key, where))

    # --- shipped INI value vs loader default
    for key, lst in sorted(by_key.items()):
        if key not in ini:
            continue
        section, value, line = ini[key]
        if re.search(r'Version|Migrated|Adopted|Defaults|Preference\d|Default[A-Z]\d', key):
            continue  # one-shot migration markers, the INI carries the applied state
        boolish = {'true': '1.0', 'false': '0.0'}
        inorm = norm(value)
        inorm = boolish.get(inorm, inorm)
        if all(typ == 'String' for _, _, _, typ, _ in lst):
            continue
        lits = {boolish.get(n, n) for n, raw, where, typ, sec in lst if typ != 'String' and (n in boolish or re.fullmatch(r"[-+]?\d+(\.\d+)?", n))}
        if len(lits) != 1:
            continue
        code = next(iter(lits))
        if code.startswith('-') and key not in ini_negative_ok:
            continue  # sentinel (-1 / -99): "not set", the owner decides
        if inorm != code:
            if key in ALLOW:
                continue
            note('INI value differs from loader default: %s INI=%s (line %d) code=%s (%s)' % (key, value, line, code, lst[0][2]))

    # --- INI keys in [Settings] nobody reads
    for key, (section, value, line) in sorted(ini.items()):
        if key not in read_keys and key not in saved_keys:
            note('INI key not read by the code: %s (line %d, [%s])' % (key, line, section))

    # --- live_settings_d39.h
    struct_defaults = parse_struct_defaults(d39)
    ids = parse_ids(d39)
    rows = parse_rows(d39)
    row_ids = {}
    for rid, title, line, ro in rows:
        row_ids.setdefault(rid, []).append((title, line, ro))
    adv_ids, adv_body = parse_cases(d39, 'template<class S> bool advance(')
    lab_ids, lab_body = parse_cases(d39, 'template<class S> void label(')
    for rid in ids:
        if rid not in row_ids and rid not in IDS_WITHOUT_ROW:
            err('Id::%s has no row on any page' % rid)
    for rid in row_ids:
        if rid not in ids:
            err('row uses unknown Id::%s' % rid)
        if rid not in lab_ids:
            err('Id::%s has a row but no label() case' % rid)
        if rid not in adv_ids and not all(ro for _, _, ro in row_ids[rid]):
            err('Id::%s has a clickable row but no advance() case' % rid)
    for rid in adv_ids | lab_ids:
        if rid not in ids:
            err('advance/label case for unknown Id::%s' % rid)
    # advance() cases that always refuse the click on a row that is not read-only
    for m in re.finditer(r'case Id::(\w+):return false;', adv_body):
        rid = m.group(1)
        if rid in row_ids and not all(ro for _, _, ro in row_ids[rid]):
            err('Id::%s row is clickable but advance() always refuses it (user rule: a switch is never refused)' % rid)
    # the R141 comma fix: every row line ends with "}," or "}}}," (a missing comma joins two rows)
    for m in re.finditer(r'L"[^"\n]*"\}\s*//[^\n]*\n\s*\{Id::', d39):
        line = d39.count('\n', 0, m.start()) + 1
        err('row without a trailing comma before the next row at live_settings_d39.h:%d' % line)
    # one Id on two rows of the same page
    for m in re.finditer(r'inline constexpr std::array<Row,8> (\w+)\{\{(.*?)\}\};', d39, re.S):
        page_ids = re.findall(r'\{Id::(\w+),', m.group(2))
        for rid in set(page_ids):
            if page_ids.count(rid) > 1:
                err('page %s lists Id::%s twice' % (m.group(1), rid))
        if len(page_ids) != 8:
            err('page %s has %d rows (8 expected)' % (m.group(1), len(page_ids)))
    krows = re.search(r'kRows\{\{(.*?)\n\}\};', d39, re.S).group(1)
    for n, page in enumerate(re.findall(r'\{\{(.*?)\}\}\}', krows, re.S)):
        page_ids = re.findall(r'\{Id::(\w+),', page)
        if len(page_ids) != 8:
            err('kRows page %d has %d rows (8 expected)' % (n, len(page_ids)))

    # --- live_runtime_d40.inl: field <- owner <- INI key: struct default must equal the loader default
    field_getter, setter_key, load_keys, save_keys = parse_d40(d40, consts)
    field_key = {}
    for field, getter in field_getter.items():
        hit = setter_key.get(getter)
        if hit is None:
            # getter 'x::Foo' may be set through 'x::SetFoo' -> owner_of_setter gave both spellings
            continue
        key, default, typ = hit
        field_key[field] = key
        if field in struct_defaults and struct_defaults[field][1] is not None:
            if norm(default) != struct_defaults[field][1]:
                err('Settings::%s default %s but %s loads %s with default %s' % (field, struct_defaults[field][1], key, D40.name, norm(default)))
    # every LOAD key is saved and every SAVE key is loaded (the live owner wiring is symmetric)
    for key in sorted(load_keys):
        if key not in save_keys and key not in saved_keys:
            err('live_runtime LOAD reads %s but no SAVE writes it (a menu change would be lost on exit)' % key)
    for key in sorted(save_keys):
        if key not in load_keys and key not in read_keys and not key.endswith('Adopted'):
            note('live_runtime SAVE writes %s but nothing reads it (retired)' % key)
    # every LOAD key with a Settings field must have a row
    # (the user rule: every INI key = a switch in Experiments). mechanical part: field -> Id by name
    id_fields = {}
    adv_clean = re.sub(r'//[^\n]*', '', adv_body)
    for seg in re.split(r'(?=case Id::)', adv_clean)[1:]:
        cid = re.match(r'case Id::(\w+)', seg).group(1)
        for m in re.finditer(r'\bd\.(\w+)\s*=[^=]', seg):
            id_fields.setdefault(m.group(1), set()).add(cid)
    for field, key in sorted(field_key.items()):
        if field in struct_defaults and field not in id_fields and not field.endswith('Ready') and 'Status' not in field:
            if field not in ('elbowF2OverrideD48', 'freeTargetingHoldR133', 'freeTargetingContextD42', 'freeTargetingLeaseD42', 'elbowPreferenceR92', 'frikElbowsD48', 'elbowSolverR89', 'knifeRightHipD66', 'instantStateD72', 'diceHandsStatusD85'):
                err('Settings::%s (%s) is never changed by advance(): no working switch' % (field, key))
    # a row whose click changes one field but whose label shows another = the row is wired to the wrong variable
    lab_fields = {}
    lab_clean = re.sub(r'//[^\n]*', '', lab_body)
    for seg in re.split(r'(?=case Id::)', lab_clean)[1:]:
        cid = re.match(r'case Id::(\w+)', seg).group(1)
        reads = set(re.findall(r'\b[ds]\.(\w+)', seg))
        if reads:  # a grouped "case A:case B:{...}" leaves A without its own reads: not judged
            lab_fields[cid] = reads
    adv_fields = {}
    for seg in re.split(r'(?=case Id::)', adv_clean)[1:]:
        cid = re.match(r'case Id::(\w+)', seg).group(1)
        adv_fields[cid] = set(re.findall(r'\b[ds]\.(\w+)\s*=[^=]', seg)) | set(re.findall(r'\b[ds]\.(\w+)\s*\[', seg))
    for cid, written in sorted(adv_fields.items()):
        shown = lab_fields.get(cid)
        if written and shown is not None and not (written & shown):
            err('Id::%s: advance() writes %s but label() shows %s (row wired to the wrong variable)' % (cid, sorted(written), sorted(shown)))
    # R14x/R15x keys: every one has a row
    for key in sorted(read_keys):
        if re.search(r'R1[45]\d', key):
            field = [f for f, k in field_key.items() if k == key]
            if not field:
                err('%s has no live_runtime field (no Experiments row)' % key)
            elif not any(f in id_fields for f in field):
                err('%s: field %s has no advance() case' % (key, field))
            if key not in ini:
                err('%s is read by the code but missing from the shipped KCD2VR.ini' % key)

    # --- docs
    doc_keys = collect_doc_keys()
    for key, places in sorted(doc_keys.items()):
        if key in DOC_KEYS_RETIRED:
            continue
        if key not in ini and key not in read_keys and key.lower() not in all_keys:
            err('docs name %s but neither the INI nor the code has it (%s)' % (key, places[0]))
        elif key not in read_keys and key.lower() in all_keys and key not in ini:
            err('docs name %s with a spelling the code does not use (%s)' % (key, places[0]))
    # docs "shipped X" claims for backticked keys: `VrKey` ... shipped VALUE / (shipped VALUE)
    for p in doc_files():
        text = read(p)
        for m in re.finditer(r'`(Vr\w+)`[^`\n.]*?(?:shipped|default)\s+(ON|OFF|true|false|[-+]?\d+(?:\.\d+)?)', text):
            key, claim = m.group(1), m.group(2)
            if key not in ini:
                continue
            c = {'ON': 'true', 'OFF': 'false'}.get(claim, claim)
            if norm(c) != norm(ini[key][1]):
                err('%s:%d says %s shipped %s but KCD2VR.ini has %s' % (p.relative_to(ROOT).as_posix(), text.count('\n', 0, m.start()) + 1, key, claim, ini[key][1]))

    for n in notes:
        print('note: ' + n)
    for e in errors:
        print('MISMATCH: ' + e)
    print('%d keys read, %d saved, %d in the INI, %d rows, %d Ids; %d mismatches, %d notes' % (
        len(read_keys), len(saved_keys), len(ini), len(rows), len(ids), len(errors), len(notes)))
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
