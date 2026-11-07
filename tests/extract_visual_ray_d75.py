"""Extract unchanged production visual-ray publication/getter for the host fixture."""
from pathlib import Path
import argparse, hashlib, json

p = argparse.ArgumentParser()
p.add_argument('--out', required=True, type=Path)
args = p.parse_args()
source = Path(__file__).resolve().parents[1] / 'src/camera_hook.cpp'
text = source.read_text()
items = []
for signature in ('static void PublishVisualRayD75(', 'bool CameraHook::GetLatestVisualRayD75('):
    begin = text.index(signature)
    brace = text.index('{', begin)
    depth = 1
    end = brace + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    body = text[begin:end]
    items.append((signature, body))
args.out.parent.mkdir(parents=True, exist_ok=True)
args.out.write_text('\n\n'.join(body for _, body in items) + '\n')
args.out.with_suffix('.json').write_text(json.dumps({
    'source': str(source), 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
    'extracted': [{'signature': name, 'sha256': hashlib.sha256(body.encode()).hexdigest()} for name, body in items],
    'substitutions': [],
}, indent=2) + '\n')
