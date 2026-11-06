"""Extract the actual View observer/accessor for the isolated native-head fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import argparse

def function(source: str, signature: str) -> str:
    begin = source.index(signature)
    brace = source.index('{', begin)
    depth = 0
    for at in range(brace, len(source)):
        if source[at] == '{': depth += 1
        elif source[at] == '}':
            depth -= 1
            if not depth: return source[begin:at + 1]
    raise ValueError(f"unclosed function: {signature}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    text = read_source(root / 'src/frik_pose_probe.cpp', encoding='utf-8-sig')
    bodies = [function(text, 'void ObserveNativeDialogueHead28('),
              function(text, 'bool NativeDialogueViewHead28(')]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text('// Generated from current src/frik_pose_probe.cpp; do not edit.\n' + '\n\n'.join(bodies) + '\n', encoding='utf-8')
    print(f'Extracted {len(bodies)} actual production bridge functions -> {args.output}')

if __name__ == '__main__': main()
