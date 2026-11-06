"""Restore the D64 dark diamond POIs from the current per-sheet marker geometry.

Uses the existing checked-in CGF format producer, preserves each marker centre
and the paper/player models, and introduces no runtime texture generation.
Run once when preparing source; normal builds use the checked-in result.
"""
from pathlib import Path
import struct
from build_map_geometry_d64 import atomic, bounds, chunks, marker_mesh, material, pack

ROOT = Path(__file__).resolve().parents[1]

def restore():
    directory = ROOT / "companion/source/Objects/kcd2vr/mapmarkers64"
    count = 0
    for mesh in sorted(directory.glob("*.cgfm")):
        name = mesh.stem
        if name == "player":
            continue
        data = mesh.read_bytes()
        points = []
        for kind, _, _, block in chunks(data):
            if kind == 0x1016 and struct.unpack_from("<I", block, 4)[0] == 15:
                n = struct.unpack_from("<I", block, 8)[0]
                assert n and n % 4 == 0
                vertices = [struct.unpack_from("<3e", block, 24 + 16*i) for i in range(n)]
                for i in range(0, n, 4):
                    x, y, z = (sum(v[k] for v in vertices[i:i+4])/4 for k in range(3))
                    points.append((x, z, y))
        assert points, name
        generated, vertices, indices = marker_mesh(data, points, name, old=name)
        main = chunks(mesh.with_suffix(".cgf").read_bytes())
        bounds(main, vertices, len(vertices), indices)
        atomic(mesh, generated)
        atomic(mesh.with_suffix(".cgf"), pack(main))
        atomic(mesh.with_suffix(".mtl"), material(name, "0.13,0.07,0.025"))
        count += 1
    print(f"Restored simple dark POIs on {count} map sheets; player and paper unchanged.")

if __name__ == "__main__":
    restore()
