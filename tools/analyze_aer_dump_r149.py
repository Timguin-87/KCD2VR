#!/usr/bin/env python3
"""Analyse a KCD2VR R149 alternate-eye (AER) DLSS frame dump.

Usage:  python tools/analyze_aer_dump_r149.py <dump folder> [--no-png]

Reads dump.json and the DDS textures written by Ctrl+F12 / VR menu DUMP and
writes <dump>/analysis/ with PNG previews and report.txt:

* eye identity per live frame (render-side match vs main-thread eye),
* jitter subsequence per eye (AFR halves the game's sequence),
* camera/depth model check: the static-camera N->N-1 prediction from depth and
  the CRenderView CURRENT/PREVIOUS banks against the game's own vectors, for all
  four depth conventions (this is the runtime model check, recomputed offline),
* rebuilt-vector check: mv_dlss against the static N->N-2 same-eye prediction,
* temporal check: each eye's previous DLSS output warped with the vectors DLSS
  received, compared with the current output (where that differs, DLSS sees a
  history mismatch -> shimmer). The cross-eye game vectors are shown as control.

Needs numpy. Nothing here touches the game.
"""
import argparse
import json
import math
import struct
import sys
import zlib
from pathlib import Path

try:
    import numpy as np
except ImportError:  # pragma: no cover
    sys.exit('numpy is required: python -m pip install numpy')

DXGI = {
    2: ('f32', 4), 10: ('f16', 4), 16: ('f32', 2), 34: ('f16', 2), 41: ('f32', 1), 26: ('r11g11b10', 3),
    24: ('r10g10b10a2', 4), 28: ('u8', 4), 29: ('u8srgb', 4), 87: ('bgra8', 4), 91: ('bgra8srgb', 4), 88: ('bgrx8', 4),
}
DEPTH_NAMES = ['REVERSED_Z', 'STANDARD_Z', 'LINEAR_OVER_FAR', 'LINEAR_METRES']


def read_dds(path):
    data = path.read_bytes()
    if data[:4] != b'DDS ':
        raise ValueError(f'{path.name}: not a DDS file')
    height, width = struct.unpack_from('<II', data, 12)
    pitch = struct.unpack_from('<I', data, 20)[0]
    fourcc = data[84:88]
    if fourcc != b'DX10':
        raise ValueError(f'{path.name}: no DX10 header')
    fmt = struct.unpack_from('<I', data, 128)[0]
    raw = np.frombuffer(data, dtype=np.uint8, offset=148)
    if fmt not in DXGI:
        raise ValueError(f'{path.name}: unsupported DXGI format {fmt}')
    kind, channels = DXGI[fmt]
    rows = raw[:pitch * height].reshape(height, pitch)
    if kind in ('f32', 'f16'):
        dtype = np.float32 if kind == 'f32' else np.float16
        size = 4 if kind == 'f32' else 2
        img = rows[:, :width * channels * size].copy().view(dtype).reshape(height, width, channels).astype(np.float32)
    elif kind == 'r11g11b10':
        v = rows[:, :width * 4].copy().view(np.uint32).reshape(height, width)
        img = np.stack([_small_float(v & 0x7FF, 6), _small_float((v >> 11) & 0x7FF, 6), _small_float((v >> 22) & 0x3FF, 5)], -1)
    elif kind == 'r10g10b10a2':
        v = rows[:, :width * 4].copy().view(np.uint32).reshape(height, width)
        img = np.stack([(v & 0x3FF) / 1023.0, ((v >> 10) & 0x3FF) / 1023.0, ((v >> 20) & 0x3FF) / 1023.0, (v >> 30) / 3.0], -1).astype(np.float32)
    else:
        img = rows[:, :width * 4].reshape(height, width, 4).astype(np.float32) / 255.0
        if kind.startswith('bgr'):
            img = img[..., [2, 1, 0, 3]]
        if kind.endswith('srgb'):
            img[..., :3] = np.where(img[..., :3] <= 0.04045, img[..., :3] / 12.92, ((img[..., :3] + 0.055) / 1.055) ** 2.4)
    return img, fmt


def _small_float(bits, mantissa_bits):
    e = (bits >> mantissa_bits).astype(np.int32)
    m = (bits & ((1 << mantissa_bits) - 1)).astype(np.float32) / (1 << mantissa_bits)
    return np.where(e == 0, m * 2.0 ** -14, (1.0 + m) * np.exp2(e.astype(np.float32) - 15.0)).astype(np.float32)


def write_png(path, rgb):
    rgb = np.clip(rgb, 0.0, 1.0)
    img = (rgb * 255.0 + 0.5).astype(np.uint8)
    h, w = img.shape[:2]
    if img.ndim == 2:
        img = np.repeat(img[..., None], 3, -1)
    raw = b''.join(b'\0' + img[y, :, :3].tobytes() for y in range(h))

    def chunk(tag, payload):
        return struct.pack('>I', len(payload)) + tag + payload + struct.pack('>I', zlib.crc32(tag + payload) & 0xFFFFFFFF)
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0))
    png += chunk(b'IDAT', zlib.compress(raw, 6)) + chunk(b'IEND', b'')
    path.write_bytes(png)


def tonemap(c):
    c = np.maximum(c[..., :3], 0.0)
    c = c / (1.0 + c)
    return np.where(c <= 0.0031308, c * 12.92, 1.055 * np.power(c, 1 / 2.4) - 0.055)


def srgb(c):
    c = np.clip(c[..., :3], 0.0, 1.0)
    return np.where(c <= 0.0031308, c * 12.92, 1.055 * np.power(c, 1 / 2.4) - 0.055)


def flow_rgb(mv, scale_px=None):
    mag = np.hypot(mv[..., 0], mv[..., 1])
    top = scale_px or max(float(np.percentile(mag, 99)), 1e-3)
    hue = (np.arctan2(mv[..., 1], mv[..., 0]) / (2 * math.pi)) % 1.0
    val = np.clip(mag / top, 0.0, 1.0)
    i = np.floor(hue * 6).astype(int) % 6
    f = hue * 6 - np.floor(hue * 6)
    p, q, t = np.zeros_like(val), val * (1 - f), val * f
    r = np.choose(i, [val, q, p, p, t, val]); g = np.choose(i, [t, val, val, q, p, p]); b = np.choose(i, [p, p, t, val, val, q])
    return np.stack([r, g, b], -1)


def heat(x, top):
    v = np.clip(x / max(top, 1e-6), 0.0, 1.0)
    return np.stack([np.clip(v * 3, 0, 1), np.clip(v * 3 - 1, 0, 1), np.clip(v * 3 - 2, 0, 1)], -1)


def linear_z(d, mode, near, far):
    if mode == 3:
        return np.maximum(d, near)
    if mode == 2:
        return np.maximum(d * far, near)
    dr = d if mode == 0 else 1.0 - d
    return near * far / np.maximum(near + dr * (far - near), 1e-7)


def camera(c):
    r = np.array(c['rotation'], dtype=np.float64).reshape(3, 3)
    return r, np.array(c['position'], dtype=np.float64), float(c['tanX']), float(c['tanY'])


def project_uv(rot_n, pos_n, tan_n, rot_k, pos_k, tan_k, uv, z):
    """Current-camera pixel (uv, view depth z) -> uv in camera K (shader math)."""
    ndc_x, ndc_y = uv[..., 0] * 2 - 1, 1 - uv[..., 1] * 2
    local = np.stack([ndc_x * tan_n[0] * z, z, ndc_y * tan_n[1] * z], -1)
    a = rot_k.T @ rot_n
    b = rot_k.T @ (pos_n - pos_k)
    lk = local @ a.T + b
    y = np.maximum(lk[..., 1], 1e-4)
    return np.stack([0.5 + 0.5 * lk[..., 0] / (y * tan_k[0]), 0.5 - 0.5 * lk[..., 2] / (y * tan_k[1])], -1), lk[..., 1]


def bilinear(img, x, y):
    h, w = img.shape[:2]
    x = np.clip(x, 0, w - 1.001); y = np.clip(y, 0, h - 1.001)
    x0 = np.floor(x).astype(int); y0 = np.floor(y).astype(int)
    fx = (x - x0)[..., None]; fy = (y - y0)[..., None]
    a = img[y0, x0]; b = img[y0, x0 + 1]; c = img[y0 + 1, x0]; d = img[y0 + 1, x0 + 1]
    return (a * (1 - fx) + b * fx) * (1 - fy) + (c * (1 - fx) + d * fx) * fy


def luminance(c):
    return 0.2126 * c[..., 0] + 0.7152 * c[..., 1] + 0.0722 * c[..., 2]


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('dump', type=Path)
    ap.add_argument('--no-png', action='store_true')
    args = ap.parse_args()
    root = args.dump
    meta = json.loads((root / 'dump.json').read_text(encoding='utf-8', errors='replace'))
    out = root / 'analysis'; out.mkdir(exist_ok=True)
    lines = []

    def say(text=''):
        print(text); lines.append(text)

    g = meta.get('global', {})
    say(f"dump {root.name}  build={meta.get('build')}  frames={len(meta.get('dlss', []))}  xrBlits={len(meta.get('xr', []))}")
    model = g.get('model', {})
    say(f"runtime model: valid={model.get('valid')} depth={model.get('depthModeName')} scores(<2px)={model.get('scores')} switches={model.get('switches')}")
    say(f"stats: {json.dumps(g.get('stats', {}))}")
    say(f"identity: {json.dumps(g.get('identityStats', {}))}")
    for side in ('Left', 'Right'):
        seq = g.get('jitterHistory' + side, [])
        if seq:
            xs, ys = np.array(seq[0::2]), np.array(seq[1::2])
            say(f"jitter {side}: n={len(xs)} x[{xs.min():+.3f},{xs.max():+.3f}] mean {xs.mean():+.3f}  y[{ys.min():+.3f},{ys.max():+.3f}] mean {ys.mean():+.3f}")
    say()

    frames = {}
    for f in meta.get('dlss', []):
        tex = {}
        for t in f.get('textures', []):
            p = root / t['file']
            if t.get('written') and p.exists():
                try:
                    tex[t['name']] = read_dds(p)[0]
                except ValueError as e:
                    say(f'  skip {p.name}: {e}')
        frames[f['index']] = (f, tex)
        ident = f.get('identity', {})
        say(f"live frame {f['index']}: eye={'LR'[f['eye']]} frame={f['frame']} decision={f.get('decision')} reset={f.get('effectiveReset')} "
            f"identity={ident.get('sourceName')} mainEye={ident.get('mainEye')} {'EYE-RACE' if ident.get('mainEye') not in (None, f['eye']) else ''}")
        p = f.get('params', {})
        say(f"   jitter={p.get('jitter')} mvScale={p.get('mvScale')} render={p.get('render')} mvActive={p.get('mvActive')} createFlags={f.get('createFlags')} "
            f"historyJump={f.get('historyJumpM', 0):.4f}m/{f.get('historyJumpDeg', 0):.3f}deg")
        cur, prev = f.get('cameraCurrent', {}), f.get('cameraPrevious', {})
        if cur.get('valid') and prev.get('valid'):
            rn, pn, *_ = camera(cur); rp, pp, *_ = camera(prev)
            delta = rn.T @ (pp - pn)
            say(f"   cam fov={math.degrees(cur['fov']):.3f} near={cur['near']:.4f} far={cur['far']:.1f} tan={cur['tanX']:.4f}/{cur['tanY']:.4f} "
                f"PREVIOUS-in-current [right,fwd,up]=[{delta[0]:+.4f},{delta[1]:+.4f},{delta[2]:+.4f}]m (AFR expects about +-IPD on right)")
        else:
            say(f"   cameras unavailable: {f.get('cameraFailure')}")

        if not args.no_png:
            name = f"dlss{f['index']}_{'LR'[f['eye']]}"
            if 'color_in' in tex: write_png(out / f'{name}_color_in.png', tonemap(tex['color_in']))
            if 'dlss_output' in tex: write_png(out / f'{name}_dlss_output.png', tonemap(tex['dlss_output']))
            if 'depth' in tex:
                d = tex['depth'][..., 0]; v = d[np.isfinite(d)]
                lo, hi = (np.percentile(v, 1), np.percentile(v, 99)) if v.size else (0, 1)
                write_png(out / f'{name}_depth.png', np.clip((d - lo) / max(hi - lo, 1e-9), 0, 1))
            for key in ('mv_game', 'mv_dlss'):
                if key in tex:
                    sc = np.array(p.get('mvScale', [1, 1]), dtype=np.float32)
                    write_png(out / f'{name}_{key}.png', flow_rgb(tex[key][..., :2] * sc))

        # --- camera/depth model check (all conventions) -------------------
        if cur.get('valid') and prev.get('valid') and 'depth' in tex and 'mv_game' in tex:
            rw, rh = p.get('render', [0, 0]); mw, mh = p.get('mvActive', [rw, rh])
            mbx, mby = p.get('mvBase', [0, 0]); dbx, dby = p.get('depthBase', [0, 0])
            sc = np.array(p.get('mvScale', [1, 1]), dtype=np.float32)
            step = max(1, int(max(mw, mh) // 640))
            ys, xs = np.mgrid[0:mh:step, 0:mw:step]
            uv = np.stack([(xs + 0.5) / mw, (ys + 0.5) / mh], -1)
            dx = np.clip((dbx + (xs + 0.5) * rw / mw).astype(int), 0, tex['depth'].shape[1] - 1)
            dy = np.clip((dby + (ys + 0.5) * rh / mh).astype(int), 0, tex['depth'].shape[0] - 1)
            d = tex['depth'][dy, dx, 0].astype(np.float64)
            game = tex['mv_game'][mby + ys, mbx + xs, :2] * sc
            rn, pn, txn, tyn = camera(cur); rp, pp, txp, typ = camera(prev)
            results = []
            for mode in range(4):
                z = linear_z(d, mode, cur['near'], cur['far'])
                uvp, _ = project_uv(rn, pn, (txn, tyn), rp, pp, (txp, typ), uv, z)
                res = np.hypot(*(game - (uvp - uv) * np.array([rw, rh])).transpose(2, 0, 1))
                good = float(np.mean(res < 2.0)); med = float(np.median(res))
                results.append((good, med, mode, res))
            best = max(results, key=lambda r: r[0])
            say('   static-model agreement (<2px / median px): ' + '  '.join(
                f"{DEPTH_NAMES[m]} {g_:.3f}/{md:.2f}" for g_, md, m, _ in results) + f"  -> best {DEPTH_NAMES[best[2]]}")
            if not args.no_png:
                write_png(out / f"dlss{f['index']}_{'LR'[f['eye']]}_model_residual.png", heat(best[3], 8.0))
            hist = f.get('cameraHistory', {})
            if 'mv_dlss' in tex and hist.get('valid'):
                rh_ = np.array(hist['rotation'], dtype=np.float64).reshape(3, 3); ph_ = np.array(hist['position'], dtype=np.float64)
                z = linear_z(d, best[2], cur['near'], cur['far'])
                uvh, _ = project_uv(rn, pn, (txn, tyn), rh_, ph_, (hist['tanX'], hist['tanY']), uv, z)
                rebuilt = tex['mv_dlss'][mby + ys, mbx + xs, :2] * sc
                res = np.hypot(*(rebuilt - (uvh - uv) * np.array([rw, rh])).transpose(2, 0, 1))
                say(f"   mv_dlss vs static same-eye N->N-2 prediction: <1px {np.mean(res < 1):.3f}  <2px {np.mean(res < 2):.3f}  median {np.median(res):.2f}px")

    # --- temporal check: previous same-eye output warped into the current one
    say()
    for idx, (f, tex) in sorted(frames.items()):
        prev_idx = next((j for j in sorted(frames, reverse=True) if j < idx and frames[j][0]['eye'] == f['eye']), None)
        if prev_idx is None or 'dlss_output' not in tex or 'dlss_output' not in frames[prev_idx][1]:
            continue
        cur_out, prev_out = tex['dlss_output'], frames[prev_idx][1]['dlss_output']
        p = f.get('params', {}); sc = np.array(p.get('mvScale', [1, 1]), dtype=np.float32)
        rw, rh = p.get('render', [cur_out.shape[1], cur_out.shape[0]]); mw, mh = p.get('mvActive', [rw, rh])
        mbx, mby = p.get('mvBase', [0, 0])
        oh, ow = cur_out.shape[:2]
        step = max(1, int(max(ow, oh) // 1024))
        ys, xs = np.mgrid[0:oh:step, 0:ow:step]
        mx = np.clip(((xs + 0.5) * mw / ow).astype(int), 0, mw - 1) + mbx
        my = np.clip(((ys + 0.5) * mh / oh).astype(int), 0, mh - 1) + mby
        cl = luminance(tonemap(cur_out[ys, xs]))
        for key in ('mv_dlss', 'mv_game'):
            if key not in tex:
                continue
            mv = tex[key][my, mx, :2] * sc * np.array([ow / rw, oh / rh])
            warped = bilinear(prev_out, xs + mv[..., 0], ys + mv[..., 1])
            err = np.abs(luminance(tonemap(warped)) - cl)
            say(f"temporal {'LR'[f['eye']]} {prev_idx}->{idx} via {key}: mean|dL| {err.mean():.4f}  p95 {np.percentile(err, 95):.4f}  >0.1: {np.mean(err > 0.1):.3%}"
                + ('' if key == 'mv_dlss' else '   (control: cross-eye vectors)'))
            if not args.no_png:
                write_png(out / f"temporal_{'LR'[f['eye']]}_{prev_idx}_to_{idx}_{key}.png", heat(err, 0.2))
        if 'mv_dlss' not in tex:
            say(f"temporal {'LR'[f['eye']]} {prev_idx}->{idx}: DLSS was reset this frame ({f.get('decision')}), no rebuilt vectors")

    # --- XR composition --------------------------------------------------
    say()
    for x in meta.get('xr', []):
        eyes = x.get('eyes', [])
        say(f"xr blit {x['index']}: sourceEye={x.get('sourceEye')} newFrame={x.get('newFrame')} renderFrameIndex={x.get('renderFrameIndex')} "
            f"cacheCameraFrame={x.get('cacheCameraFrame')} warp={[e.get('warp') for e in eyes]} cameraWarp={[e.get('cameraWarp') for e in eyes]}")
        if not args.no_png:
            for t in x.get('textures', []):
                p = root / t['file']
                if t.get('written') and p.exists():
                    try:
                        img, fmt = read_dds(p)
                        write_png(out / (p.stem + '.png'), srgb(img) if fmt in (28, 29, 87, 91, 88, 24) else tonemap(img))
                    except ValueError as e:
                        say(f'  skip {p.name}: {e}')
    (out / 'report.txt').write_text('\n'.join(lines) + '\n', encoding='utf-8')
    print(f'\nwrote {out}')


if __name__ == '__main__':
    main()
