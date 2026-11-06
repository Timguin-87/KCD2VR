#!/usr/bin/env python3
"""Extract the complete pending-setup file flow; native OS/profile calls are leaves."""
from pathlib import Path
import argparse
import hashlib
import json
from extract_config_guard_d62 import function_body


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    path = args.root / 'src/subsystems/base/startup_setup_s15.inl'
    text = path.read_text()
    declarations = ('struct Field ', 'static constexpr Field fields[]=', 'struct Headset ',
                    'static constexpr Headset headsets[]=', 'struct State ')
    functions = ('static int HeadsetIndex(', 'static int ReadHeadset(', 'static bool Parse(',
                 'static bool CommitIni(', 'static fs::path PendingPath(',
                 'static std::wstring Canonical(', 'static bool ApplyPending(',
                 'bool ApplyPendingBeforeConfig()')
    parts = ['// GENERATED complete production functions, not hand-copied policy.']
    parts += [function_body(text, signature) + ';' for signature in declarations]
    parts += [function_body(text, signature) for signature in functions]
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text('\n\n'.join(parts) + '\n')
    args.out.with_suffix('.pins.json').write_text(json.dumps({
        str(path.relative_to(args.root)): hashlib.sha256(path.read_bytes()).hexdigest(),
        'extracted_sha256': hashlib.sha256(args.out.read_bytes()).hexdigest(),
        'native_OS_profile_leaves': 'explicit fixture replacements',
    }, indent=2) + '\n')


if __name__ == '__main__':
    main()
