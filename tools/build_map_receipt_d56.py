#!/usr/bin/env python3
"""Generate the two owned D56 map receipt assets; no retail assets required."""
import argparse
import hashlib
import json
from pathlib import Path
import build_map_navigation_t2 as renderer

ROOT = Path(__file__).resolve().parents[1]
BASELINE_GFX_SHA256 = '92477e4d330f1d6cea577e0eb170f6e0c355f244bd18de5d898acce5e4f28b08'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=ROOT / 'mods/KCD2VRMap')
    args = parser.parse_args()
    baseline = renderer.gfx(tile_count=20, wide=True, marker_count=257)
    assert hashlib.sha256(baseline).hexdigest() == BASELINE_GFX_SHA256, 'The authored D55 base changed; review before regenerating.'
    assets = {
        'KCD2VRMapT2.gfx': renderer.gfx(tile_count=20, wide=True, marker_count=257, receipt_d56=True),
        'KCD2VRMapT2.xml': renderer.ui_xml(receipt_d56=True),
    }
    args.out.mkdir(parents=True, exist_ok=True)
    for name, data in assets.items():
        (args.out / name).write_bytes(data)
    print(json.dumps({name: {'bytes': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
                      for name, data in assets.items()}, indent=2))


if __name__ == '__main__':
    main()
