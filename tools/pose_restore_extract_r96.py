"""Literal source slices for the narrow R96 native restoration regression.

No solver logic is rewritten. Native memory/identity leaves are supplied by the
fixture; the current restoration, qualification and outer-entry statements are
compiled exactly as written in the production source.
"""
from pathlib import Path
import hashlib
import re


def sha(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def block(text, prefix):
    at = text.index(prefix)
    start = text.index('{', at)
    depth, end = 1, start + 1
    while depth:
        depth += (text[end] == '{') - (text[end] == '}')
        end += 1
    if text[end:end + 1] == ';':
        end += 1
    return text[at:end]


def shared(root, dest):
    source = root / 'src/vrik/coherent_pose.inl'
    text = source.read_text()
    start = text.find('namespace restoreR96=')
    if start < 0:
        code = '#define R96_RESTORE_DEBT_PRESENT 0\n'
    else:
        end = text.index('\nstruct CoherentTransaction6689;', start)
        code = ('#define R96_RESTORE_DEBT_PRESENT 1\n'
                '#include "vrik/pose_restore_debt_r96.h"\n'
                '#include "core/observed_load_recovery_r96.h"\n' + text[start:end] + '\n')
    dest.write_text(code)
    return {'source': str(source), 'source_sha256': sha(source), 'slice_sha256': sha(dest)}


def extract(root, out):
    out.mkdir(parents=True, exist_ok=True)
    cpp = (root / 'src/frik_pose_probe.cpp').read_text()
    coh = (root / 'src/vrik/coherent_pose.inl').read_text()
    anatomy = (root / 'src/subsystems/vrik/pose_anatomy.inl').read_text()
    parts = [block(cpp, 'struct QuatT28 ')]
    parts += [re.search(r'using ' + n + r' = .*?;', cpp).group()
              for n in ('PoseGetFn', 'PoseSetFn')]
    (out / 'pose_restore_types_r96.inc').write_text('\n'.join(parts) + '\n')
    (out / 'pose_restore_read_r96.inc').write_text(
        block(coh, 'static bool ValidPose6689(') + '\n' +
        block(anatomy, 'static bool ReadNativeBuffer6689(') + '\n')
    shared(root, out / 'pose_restore_shared_r96.inc')
    (out / 'pose_restore_method_r96.inc').write_text(block(coh, 'void Restore() noexcept') + '\n')
    start = cpp.find('    if(rigOk&&rawOk&&g_pose_restore_fault6689.load(std::memory_order_acquire))')
    outer = '' if start < 0 else cpp[start:cpp.index('\n    if(rigOk&&rawOk) {', start)]
    (out / 'pose_restore_outer_r96.inc').write_text(outer + '\n')
    # This is the unchanged real BeginCoherent admission gate, executed after
    # the outer callback. Only InitCoherent/Reject are controlled fixture leaves.
    start = coh.index('    if(g_pose_restore_fault6689.load()||!InitCoherent6689(')
    gate = coh[start:coh.index('\n', start)]
    (out / 'pose_restore_gate_r96.inc').write_text(gate + '\n')
    inputs = ['src/frik_pose_probe.cpp', 'src/vrik/coherent_pose.inl',
              'src/subsystems/vrik/pose_anatomy.inl', 'src/vrik/pose_restore_debt_r96.h']
    return {'source_sha256': {p: sha(root / p) for p in inputs if (root / p).exists()},
            'slices': {p.name: sha(p) for p in sorted(out.glob('pose_restore_*.inc'))},
            'outer_recovery_present': bool(outer),
            'scope': 'Literal Restore, recovery helper, native read/validation, outer qualified callback statement, and BeginCoherent fault gate. The complete solver is not compiled by this focused fixture.'}
