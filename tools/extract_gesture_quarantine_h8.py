#!/usr/bin/env python3
"""Copy the actual Drain cost/attempt block into its full-producer host fixture.
The surrounding native geometry/actor readbacks and irreversible native entry
are fixture leaves; this does not execute retail instructions or prove damage.
"""
from pathlib import Path
import hashlib
import json
import sys
import os
import subprocess
from source_text import expand_source

def extract(root, output):
    root, output = Path(root).resolve(), Path(output).resolve()
    output.mkdir(parents=True, exist_ok=True)
    source = expand_source(root / 'src/subsystems/hybrid/drain_cost_claim.inl', root=root)
    block, tail = source.text.split('    if(liveRoute39&&!vrDirect596&&p.lease47c2.serial', 1)
    assert block.count('hybridAttempts54.claim(') == 1 and block.count('costProof54(') == 1
    (output/'generated_quarantine_drain_h8.inc').write_bytes(block.encode())
    geometry = expand_source(root/'src/diagnostics/geometry_hit178r27.inl', root=root)
    baseline = os.environ.get('KCD2VR_QUARANTINE_OUTER_BASELINE')
    text = subprocess.check_output(['git','-C',str(root),'show',baseline+':src/diagnostics/geometry_hit178r27.inl']).decode() if baseline else geometry.text
    begin = text.index('static bool OriginalContactOwned19c3(')
    end = text.index('static bool RetainedContactBody19c3(', begin)
    owner = text[begin:end]
    begin = text.index('    const bool originalOwnedH8=') if not baseline else text.index('    if(!OriginalContactOwned19c3(p,now)){')
    end = text.index('    const void* gameRules5910c1=nullptr;',begin)
    outer = text[begin:end]
    (output/'generated_quarantine_owner_h8.inc').write_bytes(owner.encode())
    (output/'generated_quarantine_outer_h8.inc').write_bytes(outer.encode())
    begin = geometry.text.index('static bool RetainedContactBody19c3(')
    end = geometry.text.index('\n}',begin)+2
    (output/'generated_quarantine_body_h8.inc').write_bytes(geometry.text[begin:end].encode())
    target = expand_source(root/'src/subsystems/hybrid/drain_target_resolution.inl',root=root).text
    begin = target.index('    if(target&&(target==henryActor')
    end = target.index('target=nullptr;',begin)+len('target=nullptr;')
    (output/'generated_quarantine_target_h8.inc').write_bytes(target[begin:end].encode())


    receipt = {'source': source.provenance(), 'block_sha256': hashlib.sha256(block.encode()).hexdigest(),
               'outer_baseline':baseline, 'geometry':geometry.provenance(),
               'outer_prefix_sha256':hashlib.sha256(outer.encode()).hexdigest(),
               'owner_sha256':hashlib.sha256(owner.encode()).hexdigest(),
               'scope': 'Whole physical producer and actual native command gate plus verbatim Drain claim/cost block; synthetic geometry and native-completion/entry leaves, no retail execution.'}
    (output/'gesture_quarantine_h8_extract.json').write_bytes((json.dumps(receipt, indent=2)+'\n').encode())
    return receipt

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: extract_gesture_quarantine_h8.py OUTPUT_DIRECTORY')
    extract(Path(__file__).resolve().parents[1], sys.argv[1])
