#!/usr/bin/env python3
"""Compare the generated v3 reference audit with retained evaluated native poses.
No packages required. Distances are comparisons, NOT a natural-fist/mesh score.
Example:
  python tools/compare_finger_references_v6697.py GAME-reference.json \
    --native continuity/v6697/finger_native_samples.json --output comparison.json
"""
from __future__ import annotations
import argparse
import json
import math
import statistics
from pathlib import Path
from typing import Any


def quaternion(value: Any) -> list[float]:
    if not isinstance(value, list) or len(value) != 4:
        raise ValueError('Expected four quaternion components, xyzw')
    q = [float(x) for x in value]
    n = math.sqrt(sum(x*x for x in q))
    if not math.isfinite(n) or not .8 <= n <= 1.2:
        raise ValueError('Quaternion is not finite and approximately normalized')
    return [x/n for x in q]


def angle_degrees(a: Any, b: Any) -> float:
    qa, qb = quaternion(a), quaternion(b)
    return math.degrees(2 * math.acos(min(1.0, abs(sum(x*y for x, y in zip(qa, qb))))))


def compare(audit: dict[str, Any], native: dict[str, Any]) -> dict[str, Any]:
    if audit.get('schema') != 3 or not isinstance(audit.get('joints'), list):
        raise ValueError('Expected KCD2VR V66.97 schema-3 reference JSON')
    by_id: dict[tuple[str, int], dict[str, Any]] = {}
    for row in audit['joints']:
        key = (str(row['side']), int(row['joint']))
        if key[0] not in ('L', 'R') or key in by_id:
            raise ValueError(f'Invalid or duplicate side/joint: {key}')
        for endpoint in ('relaxed', 'fist'):
            quaternion(row[endpoint])
        for candidate in row.get('clench_candidates', []):
            quaternion(candidate)
        offsets = row['offset']
        if len(offsets) != 3 or not all(math.isfinite(float(x)) for x in offsets):
            raise ValueError(f'Invalid anatomical offset: {key}')
        by_id[key] = row
    groups = []
    for name, group in sorted(native.items()):
        if not isinstance(group, dict) or not isinstance(group.get('joints'), list):
            continue
        side = str(group['side'])
        rows, missing, mismatched = [], [], []
        for original in group['joints']:
            key = (side, int(original['joint']))
            row = by_id.get(key)
            if row is None:
                missing.append(key[1]); continue
            if int(original['parent']) != int(row['parent']):
                mismatched.append(key[1]); continue
            difference = math.dist([float(x) for x in original['offset']], [float(x) for x in row['offset']])
            if not math.isfinite(difference):
                raise ValueError('Nonfinite native anatomical offset')
            rows.append({'joint': key[1], 'parent': row['parent'],
                         'relaxed_angle_deg': angle_degrees(row['relaxed'], original['q_xyzw']),
                         'fist_angle_deg': angle_degrees(row['fist'], original['q_xyzw']),
                         'offset_difference_m': difference,
                         'recorded_native_line': original.get('source_line')})
        summary = {}
        for endpoint in ('relaxed', 'fist'):
            values = [r[f'{endpoint}_angle_deg'] for r in rows]
            summary[endpoint] = {'median_angle_deg': statistics.median(values) if values else None,
                                 'max_angle_deg': max(values) if values else None}
        groups.append({'recording': name, 'side': side, 'category': group.get('category'),
                       'native_sample': group.get('sample'), 'matched_joints': len(rows),
                       'missing_joints': missing, 'parent_mismatches': mismatched,
                       'summary': summary, 'per_joint': rows})
    return {'schema': 1, 'rig': audit.get('rig'), 'source': 'GENERATED_REFERENCE_VS_NATIVE_EVALUATED_SNAPSHOTS',
            'interpretation': 'Button categories are not authored extrema. Smaller angular difference does not prove mesh clearance, naturalness, or maximum fist. No poses are changed by this tool.',
            'generated_joints': len(by_id), 'have_authored_candidates': audit.get('have_phases', False),
            'groups': groups}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('audit', type=Path)
    parser.add_argument('--native', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    try:
        result = compare(json.loads(args.audit.read_text(encoding='utf-8')),
                         json.loads(args.native.read_text(encoding='utf-8')))
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(json.dumps(result, indent=2, allow_nan=False) + '\n', encoding='utf-8')
    except (OSError, ValueError, TypeError, KeyError) as exc:
        parser.error(str(exc))
    print(f'Wrote {len(result["groups"])} recording comparisons to {args.output}')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
