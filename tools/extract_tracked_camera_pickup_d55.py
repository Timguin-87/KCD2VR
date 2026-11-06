"""Extract current production camera reader, with all gated diagnostics retained."""
import argparse
import hashlib
import json
from pathlib import Path
from source_text import expand_source, read_source

ROOT = Path(__file__).resolve().parents[1]


def section(text, start, end):
    if text.count(start) != 1:
        raise ValueError('camera extraction boundary changed: ' + start)
    begin = text.index(start)
    if text[begin:].count(end) != 1:
        raise ValueError('camera extraction end changed: ' + end)
    return text[begin:text.index(end, begin)]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--out', type=Path, required=True)
    args = p.parse_args()
    source = expand_source(ROOT/'src/frik_pose_probe.cpp')
    context = read_source(ROOT/'src/vrik/tool_attachment59c10.inl')
    parts = [section(context, 'static const char* WorkstationContextRefusal19c4r2(',
                     'static const char* WorkstationToolReason19c4r2(')]
    for start, end in [('static bool ReadCommittedPacketFor6695(', 'static bool ReadF12HandDeltaV6557('),
                       ('void SetSmoothBody51(', 'void SetHeadRaiseMax59c9('),
                       ('static kcd2vr::core::SnapshotMailbox<kcd2vr::camera_hold_e1::Sample> g_camera_hold_e1;',
                        'bool MotionOwnedCamera47(')]:
        parts.append(section(source.text, start, end))
    body = '// Current production functions; generated, never hand-edited.\n'+'\n'.join(parts)
    args.out.mkdir(parents=True, exist_ok=True)
    (args.out/'generated_tracked_camera_pickup_d55.inc').write_text(body, encoding='utf-8', newline='\n')
    report = source.provenance()
    report.update(extracted_sha256=hashlib.sha256(body.encode()).hexdigest(),
                  native_leaves='Synthetic packet, tracking/context and validated root reads; no engine execution',
                  diagnostics='Actual F5 interaction recorder and core/refusal/provenance paths retained')
    (args.out/'CAMERA_EXTRACTION_D55.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')


if __name__ == '__main__':
    main()
