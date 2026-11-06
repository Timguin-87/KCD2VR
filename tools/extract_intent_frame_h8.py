#!/usr/bin/env python3
"""Generate the focused intent-frame fixture from current production source.

The complete native skeleton/auto-palm adapter is outside this host fixture;
its calibrated input goals are synthetic. The ordinary frame advance block and
frame metadata publication are copied verbatim, not reimplemented in the test.
"""
from pathlib import Path
import hashlib
import json
import sys
from source_text import expand_source


def extract(root, output):
    root, output = Path(root).resolve(), Path(output).resolve()
    output.mkdir(parents=True, exist_ok=True)
    foundation = expand_source(root / 'src/subsystems/vrik/pose_frame_foundation.inl', root=root)
    begin = foundation.text.index('    if(kcd2vr::tracked_frame51::advance_station_d53(')
    end = foundation.text.index('\n    // C54:', begin)
    block = foundation.text[begin:end]
    adapter = expand_source(root / 'src/subsystems/base/pose_physical_executor.inl', root=root)
    metadata = [line for line in adapter.text.splitlines() if 'pi.modelIntentFrameH8=' in line]
    if len(metadata) != 1:
        raise ValueError('expected exactly one production frame-provenance publication')
    (output / 'generated_frame_foundation_h8.inc').write_text(block + '\n', encoding='utf-8')
    (output / 'generated_frame_metadata_h8.inc').write_text(metadata[0] + '\n', encoding='utf-8')
    fist=(root/'src/vrik/fist_native.inl').read_text()
    start=fist.index('f.sample.allowed[i]=')
    finish=fist.index(';',start)+1
    fist_allowed=fist[start:finish]
    (output/'generated_fist_allowed_r93.inc').write_text(fist_allowed+'\n',encoding='utf-8')

    receipt = {'foundation': foundation.provenance(), 'adapter': adapter.provenance(),
               'extracted_foundation_sha256': hashlib.sha256(block.encode()).hexdigest(),
               'metadata_publication': metadata[0], 'fist_publication':fist_allowed, 'fist_source_sha256':hashlib.sha256(fist.encode()).hexdigest(),
               'scope': 'Actual tracked frame and complete physical kernel; synthetic calibrated upstream poses; no native NPC/hit/damage execution.'}
    (output / 'intent_frame_h8_extract.json').write_text(json.dumps(receipt, indent=2) + '\n', encoding='utf-8')
    return receipt


if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: extract_intent_frame_h8.py OUTPUT_DIRECTORY')
    extract(Path(__file__).resolve().parents[1], sys.argv[1])
