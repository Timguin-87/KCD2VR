"""Read-only exact-retail byte proof for the existing combo observer/timer.
No ASI/DLL is loaded, and no new native callable is introduced.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(root / 'tools'))
from native_research112 import PE

p = argparse.ArgumentParser()
p.add_argument('--dll', type=Path, required=True)
p.add_argument('--report', type=Path, required=True)
args = p.parse_args()
pe = PE(args.dll)
expected_sha = 'bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7'
checks = [
    (0x911176, '4183f910743a', 'native phase16 branch'),
    (0x9111B6, 'e8d588ffff84c074d6', 'native eligibility909A90 false returns without opening'),
    (0x9111C3, '4c8b4338', 'native optional target at trigger+38; smallest contiguous snapshot0x40'),
    (0x9111C7, 'c6431801', 'eligible native sets request+18'),
    (0x911209, '488d4b28', 'native TimerStart receiver is trigger+28'),
    (0x91122B, 'f30f114b1ce8ff570c00c6431b01', 'duration+1C then original timer then timed+1B'),
    (0x9D6AD3, '488903', 'native TimerStart stores absolute64-bit CTimeValue at receiver+0'),
    (0x9111AA, '33d2488bcbe89c370000', 'phase4 retains original close914950(false)'),
    (0x91123E, 'ba01000000e808370000', 'phase1/64 original close914950(true)'),
    (0x90D1B4, '4c8b4940448b4138895138', 'native phase property stored before observer notifications'),
]
results = []
for rva, hexbytes, meaning in checks:
    expected = bytes.fromhex(hexbytes)
    actual = pe.read(rva, len(expected))
    results.append({'rva': hex(rva), 'expected': hexbytes, 'actual': actual.hex(), 'pass': actual == expected, 'meaning': meaning})
ranges = []
for rva in [0x91115C, 0x911250, 0x909A90, 0x9D6A34]:
    a, b = pe.function(rva)
    ranges.append({'start': hex(a), 'end_exclusive': hex(b), 'sha256': hashlib.sha256(pe.read(a, b-a)).hexdigest()})
actual_sha = hashlib.sha256(pe.data).hexdigest()
report = {'schema': 1, 'pass': actual_sha == expected_sha and pe.stamp == 0x6A350E20 and all(x['pass'] for x in results),
    'dll_path': str(args.dll), 'dll_sha256': actual_sha, 'expected_sha256': expected_sha, 'timestamp': hex(pe.stamp),
    'checks': results, 'bounded_function_ranges': ranges, 'native_runtime_executed': False,
    'limits': 'Native-open/timed is timing authority only. No accepted damage, recipe cursor, finisher or headset acceptance inferred.'}
args.report.parent.mkdir(parents=True, exist_ok=True)
args.report.write_text(json.dumps(report, indent=2) + '\n')
print('PASS' if report['pass'] else 'FAIL', f'{sum(x["pass"] for x in results)}/{len(results)} byte pins', args.report)
raise SystemExit(0 if report['pass'] else 1)
