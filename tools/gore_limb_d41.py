# KCD2VR gore D41: elbow / knee cut copies of the game's own body and clothing .skin files.
# Claude, 28 Oct 2026. Successor of gore_split.py (hands/feet, triangles only).
#
# What it writes, per source skin and per side (la ra = left/right forearm, ll rl = left/right shin):
#   <stem>_kcdg<side>.skin   CUT   : the limb below the elbow/knee is folded into a closed cap at the joint
#                                    (vertices -> cap point, skinned 100% to the upper arm / thigh), triangles
#                                    fully inside the limb are made degenerate. The ring at the joint loses its
#                                    forearm/shin weights, so the cap rides the upper arm rigidly.
#   <stem>_kcdg<side>p.skin  PIECE : only the severed limb; the joint ring is folded into the same cap point,
#                                    so the flying piece is closed at the cut end too.
#   <stem>_kcdg<side>p.fly[_mNN].cdf  wrapper character for the piece (male/female skeleton, one CA_SKIN part).
# Nothing else in a file changes: same chunk table, same sizes, same subsets, materials, LOD naming.
# Streams touched: 0 position (f32x3), 9 bone mapping (u16x4 + u8x4), 5 indices (u16), chunk 0x2004 internal
# faces (u16x3), chunk 0x2005 internal skin vertices (32 byte header, 64 byte entries: pos @12, ids u16x4 @36,
# weights f32x4 @44). Formats verified on male_body_arms.skin (chunk 0x2000 ver 0x800, 584 byte bone rows:
# W2B Matrix34 @248, B2W @296, name @344).
#
# usage: python3 gore_limb_d41.py --pak <game pak> [--pak ...] --out <folder> [--stats] [--filter substr]
import struct, sys, os, re, zipfile, argparse, json

SIDES = {
    #       set predicate on bone name                      keep bone      joint        distal
    'la': (lambda n: n.startswith('LeftForeArm') or n.startswith('LeftHand') or n.startswith('LeftInHand'),  'LeftArm',  'LeftForeArm',  'LeftHand'),
    'ra': (lambda n: n.startswith('RightForeArm') or n.startswith('RightHand') or n.startswith('RightInHand'), 'RightArm', 'RightForeArm', 'RightHand'),
    'll': (lambda n: n in ('LeftLeg', 'LeftLegRoll', 'LeftFoot', 'LeftToeBase', 'LeftHeel', 'LeftFoot_IKBlend', 'LeftToeBaseEnd'),       'LeftUpLeg',  'LeftLeg',  'LeftFoot'),
    'rl': (lambda n: n in ('RightLeg', 'RightLegRoll', 'RightFoot', 'RightToeBase', 'RightHeel', 'RightFoot_IKBlend', 'RightToeBaseEnd'), 'RightUpLeg', 'RightLeg', 'RightFoot'),
}
CAP_OUT = 0.025      # cap point: 2.5 cm past the joint, towards the hand / foot
MIN_VERTS = 12       # a skin needs at least this many limb vertices to get a variant

def parse(d):
    if d[:4] != b'CrCh' or struct.unpack_from('<I', d, 4)[0] != 0x746: raise ValueError('not CrCh 746')
    n, to = struct.unpack_from('<II', d, 8)
    st, c, bones = {}, {}, []
    for i in range(n):
        t, v, cid, sz, off = struct.unpack_from('<HHIII', d, to + 16 * i)
        if t == 0x1016:
            fl, ty, cnt, esz, _ = struct.unpack_from('<IIIHH', d, off); st[ty] = (off + 24, cnt, esz)
        elif t in (0x2000, 0x2004, 0x2005, 0x2006) and t not in c: c[t] = (off, sz, v)
    if 0x2000 in c:
        off, sz, v = c[0x2000]; k = 0
        while 344 + 584 * k < sz:
            o = off + 584 * k
            name = bytes(d[o + 344:o + 408]).split(b'\0')[0].decode('latin1')
            b2w = struct.unpack_from('<12f', d, o + 296)
            bones.append((name, (b2w[3], b2w[7], b2w[11])))
            k += 1
    return st, c, bones

def cap_point(bones, joint, distal):
    idx = {n: i for i, (n, _) in enumerate(bones)}
    if joint not in idx or distal not in idx: return None
    j = bones[idx[joint]][1]; e = bones[idx[distal]][1]
    dx, dy, dz = e[0] - j[0], e[1] - j[1], e[2] - j[2]; l = (dx * dx + dy * dy + dz * dz) ** .5
    if l < 1e-4: return j
    return (j[0] + dx / l * CAP_OUT, j[1] + dy / l * CAP_OUT, j[2] + dz / l * CAP_OUT)

def build(d0, side):
    d = bytes(d0); st, c, bones = parse(d)
    if 0 not in st or 5 not in st or 9 not in st: return None, 'no pos/index/bonemap'
    pred, keepName, joint, distal = SIDES[side]
    names = [n for n, _ in bones]
    if keepName not in names: return None, 'no ' + keepName
    keep = names.index(keepName); cap = cap_point(bones, joint, distal)
    if cap is None: return None, 'no joint'
    inB = [pred(n) for n in names]
    po, nv, pes = st[0]; bo, nb, bes = st[9]; io, ni, ies = st[5]
    if pes != 12 or bes != 12 or ies != 2 or nb != nv: return None, 'stream layout %d/%d/%d' % (pes, bes, ies)
    ext_in = []; ext_w = []
    for i in range(nv):
        ids = struct.unpack_from('<4H', d, bo + 12 * i); w = d[bo + 12 * i + 8:bo + 12 * i + 12]
        tot = sum(w) or 1; sw = sum(w[k] for k in range(4) if ids[k] < len(inB) and inB[ids[k]] and w[k])
        ext_in.append(sw * 2 > tot); ext_w.append((ids, w))
    nin = sum(ext_in)
    if nin < MIN_VERTS: return None, 'limb verts %d' % nin
    idx = list(struct.unpack_from('<%dH' % ni, d, io))
    out = {}
    # ---------------- CUT ----------------
    cut = bytearray(d); removed = 0
    for i in range(nv):
        ids, w = ext_w[i]
        if ext_in[i]:
            struct.pack_into('<3f', cut, po + 12 * i, *cap)
            struct.pack_into('<4H4B', cut, bo + 12 * i, keep, 0, 0, 0, 255, 0, 0, 0)
        else:
            ww = [0 if (ids[k] < len(inB) and inB[ids[k]]) else w[k] for k in range(4)]
            if ww != list(w):
                s = sum(ww)
                if s == 0: ids, ww = (keep, 0, 0, 0), [255, 0, 0, 0]
                else:
                    ww = [int(round(x * 255.0 / s)) for x in ww]; ww[ww.index(max(ww))] += 255 - sum(ww)
                struct.pack_into('<4H4B', cut, bo + 12 * i, *ids, *ww)
    for t in range(ni // 3):
        a, b, cc = idx[3 * t:3 * t + 3]
        if ext_in[a] and ext_in[b] and ext_in[cc]: struct.pack_into('<3H', cut, io + 6 * t, a, a, a); removed += 1
    internal(cut, d, c, inB, keep, cap, 'cut')
    out[side] = (bytes(cut), removed, ni // 3, None)
    # ---------------- PIECE ----------------
    pc = bytearray(d); used = set(); dropped = 0
    for t in range(ni // 3):
        tri = idx[3 * t:3 * t + 3]
        if any(ext_in[v] for v in tri): used.update(tri)
        else: struct.pack_into('<3H', pc, io + 6 * t, tri[0], tri[0], tri[0]); dropped += 1
    sx = sy = sz = 0.0; m = 0
    for i in range(nv):
        if ext_in[i]:
            x, y, z = struct.unpack_from('<3f', d, po + 12 * i); sx += x; sy += y; sz += z; m += 1
        elif i in used:
            struct.pack_into('<3f', pc, po + 12 * i, *cap)
    internal(pc, d, c, inB, keep, cap, 'piece')
    out[side + 'p'] = (bytes(pc), dropped, ni // 3, (sx / m, sy / m, sz / m))
    return out, ''

def internal(out, d, c, inB, keep, cap, mode):
    # chunk 0x2005: internal skin vertices; chunk 0x2004: internal faces (indices into 0x2005)
    if 0x2005 not in c: return
    off, sz, v = c[0x2005]
    if (sz - 32) % 64: return
    n = (sz - 32) // 64; base = off + 32; vin = []
    for k in range(n):
        e = base + 64 * k
        ids = struct.unpack_from('<4H', d, e + 36); w = struct.unpack_from('<4f', d, e + 44)
        tot = sum(w) or 1.0; sw = sum(w[j] for j in range(4) if ids[j] < len(inB) and inB[ids[j]])
        isin = sw * 2 > tot; vin.append(isin)
        if mode == 'cut':
            if isin:
                struct.pack_into('<3f', out, e + 12, *cap); struct.pack_into('<4H4f', out, e + 36, keep, 0, 0, 0, 1.0, 0, 0, 0)
            elif sw > 0:
                ww = [0.0 if (ids[j] < len(inB) and inB[ids[j]]) else w[j] for j in range(4)]; s = sum(ww)
                if s <= 0: struct.pack_into('<4H4f', out, e + 36, keep, 0, 0, 0, 1.0, 0, 0, 0)
                else: struct.pack_into('<4f', out, e + 44, *[x / s for x in ww])
    if mode == 'piece':
        if 0x2004 in c:
            fo, fsz, _ = c[0x2004]
            for t in range(fsz // 6):
                a, b, cc = struct.unpack_from('<3H', d, fo + 6 * t)
                if a < n and b < n and cc < n and not (vin[a] or vin[b] or vin[cc]): struct.pack_into('<3H', out, fo + 6 * t, a, a, a)
                else:
                    for q in (a, b, cc):
                        if q < n and not vin[q]: struct.pack_into('<3f', out, base + 64 * q + 12, *cap)
    elif 0x2004 in c:
        fo, fsz, _ = c[0x2004]
        for t in range(fsz // 6):
            a, b, cc = struct.unpack_from('<3H', d, fo + 6 * t)
            if a < n and b < n and cc < n and vin[a] and vin[b] and vin[cc]: struct.pack_into('<3H', out, fo + 6 * t, a, a, a)

RELEVANT = re.compile(r'objects/characters/humans/(male|female)/(body/|clothing/)')
SKIP = re.compile(r'(_lod\d|_sim\.skin|/head/|/hair/|/beard|/scabbard/|/decoration/|f_head/|_kcdg)')

def embedded_material_d52(data):
    """Read only the qualified retail material-name chunk; never scan strings."""
    if len(data) < 16 or data[:4] != b'CrCh' or struct.unpack_from('<I', data, 4)[0] != 0x746:
        return '', 'unsupported-header'
    count, table = struct.unpack_from('<II', data, 8)
    if table < 16 or table + count * 16 > len(data):
        return '', 'invalid-chunk-table'
    material = []
    for i in range(count):
        kind, version, _, size, offset = struct.unpack_from('<HHIII', data, table + i * 16)
        if kind != 0x1014:
            continue
        if version != 0x802 or size < 128 or offset < table + count * 16 or offset + size > len(data):
            return '', 'unsupported-material-chunk'
        name = data[offset:offset + 128]
        if b'\0' not in name:
            return '', 'unterminated-material-name'
        try:
            name = name.split(b'\0', 1)[0].decode('ascii').replace('\\', '/').lower()
        except UnicodeDecodeError:
            return '', 'invalid-material-name'
        # No source-drive paths, traversal or XML markup in a wrapper override.
        if not name or not re.fullmatch(r'[a-z0-9_./ -]+', name) or name.startswith('/') or any(p in ('', '.', '..') for p in name.split('/')):
            return '', 'invalid-material-name'
        material.append(name)
    if len(material) != 1:
        return '', 'ambiguous-or-missing-material-chunk'
    return material[0], 'embedded-material'

def source_material_d52(data, skin, available_materials):
    """Resolve the embedded default only. Empty means retain native skin material.

    A qualified root path or a bare name in the skin's own folder must exist in
    the supplied retail archive index. Exporter aliases and per-NPC material
    overrides are not guessed. Named fly_mNN/fly_bNN variants stay separate.
    """
    name, reason = embedded_material_d52(data)
    if not name:
        return '', reason
    if '/' in name and not name.startswith('objects/'):
        return '', 'unresolved-relative-material'
    path = name if '/' in name else skin.replace('\\', '/').lower().rsplit('/', 1)[0] + '/' + name
    if not path.endswith('.mtl'):
        path += '.mtl'
    if path not in available_materials:
        return '', 'embedded-material-not-found'
    return path, 'exact-embedded-material'

def wrapper(skel, piece, mtl):
    m = (' Material="%s"' % mtl) if mtl else ''
    return ('<?xml version="1.0" encoding="us-ascii"?>\n<CharacterDefinition>\n\t<Model File="%s" Material="Objects/characters/skeleton_nodraw" />\n'
            '\t<AttachmentList>\n\t\t<Attachment Type="CA_SKIN" AName="part" Binding="%s"%s Flags="32" />\n\t</AttachmentList>\n</CharacterDefinition>\n') % (skel, piece, m)

if __name__ == '__main__':
    ap = argparse.ArgumentParser()
    ap.add_argument('--pak', action='append', required=True); ap.add_argument('--out', required=True)
    ap.add_argument('--stats', action='store_true'); ap.add_argument('--filter', default='')
    ap.add_argument('--shard', default='0/1')
    a = ap.parse_args()
    si, sn = (int(x) for x in a.shard.split('/'))
    os.makedirs(a.out, exist_ok=True)
    table = {}; total = 0; files = 0; k = -1
    for pak in a.pak:
        z = zipfile.ZipFile(pak); allnames = z.namelist(); low = {n.lower(): n for n in allnames}
        for name in allnames:
            ln = name.lower().replace('\\', '/')
            if not ln.endswith('.skin') or not RELEVANT.search(ln) or SKIP.search(ln) or a.filter not in ln: continue
            k += 1
            if k % sn != si: continue
            try: d = z.read(name)
            except Exception as e: print('ERR read', ln, e); continue
            stem = ln[:-5]; folder = os.path.dirname(ln)
            mtls = sorted(n for n in low if n.startswith(folder + '/') and n.endswith('.mtl') and n.count('/') == ln.count('/'))
            default, material_reason = source_material_d52(d, ln, low)
            if not default:
                print('MATERIAL native-skin-default', ln, material_reason, flush=True)
            skel = 'objects/characters/humans/female/skeleton/female.chr' if '/female/' in ln else 'objects/characters/humans/male/skeleton/male.chr'
            for side in SIDES:
                try: res, why = build(d, side)
                except Exception as e: res, why = None, 'ERR %s' % e
                if not res: continue
                for key, (blob, cnt, nt, cen) in res.items():
                    q = stem + '_kcdg' + key + '.skin'; total += len(blob); files += 1
                    if not a.stats:
                        p = os.path.join(a.out, q); os.makedirs(os.path.dirname(p), exist_ok=True); open(p, 'wb').write(blob)
                        if key.endswith('p'):
                            open(os.path.join(a.out, stem + '_kcdg' + key + '.fly.cdf'), 'w', newline='\n').write(wrapper(skel, q, default))
                            for mt in mtls:
                                mm = re.search(r'_([mb]\d\d)\.mtl$', mt)
                                if mm: open(os.path.join(a.out, stem + '_kcdg' + key + '.fly_' + mm.group(1) + '.cdf'), 'w', newline='\n').write(wrapper(skel, q, mt))
                    e = table.setdefault(ln, {}); e[key] = 1
                    if cen: e['c_' + key] = [round(x, 4) for x in cen]
                print('OK', ln, side, why or '', flush=True)
    json.dump(table, open(os.path.join(a.out, 'limbs_d41_table_%d.json' % si), 'w'), indent=0, sort_keys=True)
    print('variants', files, 'skins', len(table), 'bytes', total)
