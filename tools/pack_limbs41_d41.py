# packs the gore_limb_d41.py output into kcd2vr_gore_limbs41_N.pak (zip, deflate) + the piece-centre Lua table,
# and builds kcd2vr_gore.pak v62.32 from the v62.31 staging pak (script replaced, table added). Nothing installed.
import zipfile, os, sys, json, glob
here = os.path.dirname(os.path.abspath(__file__)); out = os.path.join(here, 'out'); dst = os.path.join(here, 'PAKS'); os.makedirs(dst, exist_ok=True)
step = sys.argv[1]
if step == 'table':
    t = {}
    for f in glob.glob(os.path.join(out, 'limbs_d41_table_*.json')): t.update(json.load(open(f)))
    L = ['-- KCD2VR Gore v62.32: elbow/knee piece centres (model space) per original skin, from tools/gore_limb_d41.py. Data only.',
         'KCD2VR_Gore = KCD2VR_Gore or {}', 'KCD2VR_Gore.L41 = KCD2VR_Gore.L41 or {}', 'local T = KCD2VR_Gore.L41']
    for k in sorted(t):
        parts = []
        for kk in sorted(t[k]):
            v = t[k][kk]
            if kk.startswith('c_'): parts.append('%s={%.4f,%.4f,%.4f}' % (kk, *v))
            else: parts.append('%s=true' % kk)
        L.append("T['%s']={%s}" % (k, ','.join(parts)))
    open(os.path.join(here, 'zzkcd2vr_gore_limbs41.lua'), 'w', newline='\n').write('\n'.join(L) + '\n')
    print('table skins', len(t))
elif step.startswith('pak'):
    n = int(step[3:]); N = 4
    files = sorted(os.path.relpath(p, out).replace('\\', '/') for p in glob.glob(os.path.join(out, 'objects', '**', '*'), recursive=True) if os.path.isfile(p))
    stems = sorted({f.split('_kcdg')[0] for f in files}); mine = set(stems[n - 1::N])
    name = os.path.join(dst, 'kcd2vr_gore_limbs41_%d.pak' % n); c = 0
    with zipfile.ZipFile(name, 'w', zipfile.ZIP_DEFLATED, compresslevel=6) as z:
        for f in files:
            if f.split('_kcdg')[0] in mine: z.write(os.path.join(out, f), f); c += 1
    print(name, c, os.path.getsize(name))
elif step == 'gore':
    src = os.path.join(here, '..', 'STUMP_D41', 'kcd2vr_gore.pak'); name = os.path.join(dst, 'kcd2vr_gore.pak')
    zi = zipfile.ZipFile(src); c = 0
    with zipfile.ZipFile(name, 'w', zipfile.ZIP_DEFLATED) as z:
        for i in zi.infolist():
            if i.filename.lower() == 'scripts/startup/zzkcd2vr_gore.lua':
                z.writestr(i, open(os.path.join(here, 'zzkcd2vr_gore_v6232.lua'), 'rb').read()); c += 1
            else: z.writestr(i, zi.read(i.filename))
        z.write(os.path.join(here, 'zzkcd2vr_gore_limbs41.lua'), 'Scripts/Startup/zzkcd2vr_gore_limbs41.lua')
    print(name, 'script replaced', c, 'members', len(zipfile.ZipFile(name).namelist()))
