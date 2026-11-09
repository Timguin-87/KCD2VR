#!/usr/bin/env python3
"""Compile the actual NGX routing with substituted OS/vendor leaves.

The optional native image check validates export identities and prologues; it
does not execute Windows code or establish GPU/headset image quality.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import render_host_compiler as host
import re
import struct
import subprocess


def run(command, log, env=None, timeout=30):
    result = subprocess.run(command, text=True, capture_output=True, env=env, timeout=timeout)
    log.write_text(result.stdout + result.stderr)
    return result.returncode


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def native_contract(path, cpp):
    data = path.read_bytes()
    pe = struct.unpack_from('<I', data, 0x3c)[0]
    opt = pe + 24
    table = opt + struct.unpack_from('<H', data, pe + 20)[0]
    sections = [struct.unpack_from('<IIII', data, table + i*40 + 8)
                for i in range(struct.unpack_from('<H', data, pe + 6)[0])]

    def offset(rva):
        for virtual_size, virtual_address, raw_size, raw_pointer in sections:
            if virtual_address <= rva < virtual_address + max(virtual_size, raw_size):
                return raw_pointer + rva - virtual_address
        raise ValueError(hex(rva))

    def string(rva):
        p = offset(rva)
        return data[p:data.index(b'\0', p)].decode()

    export_rva = struct.unpack_from('<I', data, opt + 112)[0]
    fields = struct.unpack_from('<IIHHIIIIIII', data, offset(export_rva))
    count, functions, names, ordinals = fields[7], fields[8], fields[9], fields[10]
    exports = {}
    for i in range(count):
        name = string(struct.unpack_from('<I', data, offset(names)+4*i)[0])
        ordinal = struct.unpack_from('<H', data, offset(ordinals)+2*i)[0]
        exports[name] = struct.unpack_from('<I', data, offset(functions)+4*ordinal)[0]
    pins = []
    for name, rva_text, byte_text in re.findall(r'\{"(NVSDK_[^"]+)",(0x[0-9A-Fa-f]+),\{([^}]+)\}\}', cpp):
        rva = int(rva_text, 16)
        expected = bytes(int(v.strip(), 16) for v in byte_text.split(','))
        actual = data[offset(rva):offset(rva)+len(expected)]
        pins.append({'name': name, 'rva': hex(rva), 'passed': exports.get(name) == rva and actual == expected})
    # R149: +SetD3d12Resource and +GetUI (alternate-eye vector substitution/subrect reads).
    assert len(pins) == 11 and all(p['passed'] for p in pins)
    assert struct.unpack_from('<I', data, pe + 8)[0] == 0x6A350E20
    assert struct.unpack_from('<I', data, opt + 56)[0] == 0x5B2D000
    return {'sha256': hashlib.sha256(data).hexdigest(), 'pins': pins, 'executed': False}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--native', type=Path)
    parser.add_argument('--skip-sanitizers', action='store_true')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=True)
    cpp_path = root/'src/native_dlss_stereo_d82.cpp'
    runtime_path = root/'src/native_dlss_stereo_d82_runtime.inl'
    view_path = root/'src/native_stereo_preflight_v6602.cpp'
    cpp, runtime, view = cpp_path.read_text(), runtime_path.read_text(), view_path.read_text()
    state = cpp[cpp.index('bool Installed() noexcept {'):cpp.index('void LogStatus(')]
    accessor = view[view.index('bool InUpscalerView() noexcept {'):view.index('bool Busy() noexcept {')]
    positive = out/'positive'
    positive.mkdir(exist_ok=True)

    def prepare(folder, r=runtime, s=state, a=accessor):
        folder.mkdir(parents=True, exist_ok=True)
        (folder/'native_dlss_stereo_d82_runtime.inl').write_text(r)
        (folder/'native_dlss_state_d82_actual.inc').write_text(s)
        (folder/'native_upscaler_view_d82_actual.inc').write_text(a)

    def compile_case(folder, fixture, name, flags):
        binary = host.binary(folder/name)
        command = ['g++', '-std=c++20', '-pthread', '-Wall', '-Wextra', *flags,
                   '-I'+str(folder), '-I'+str(root/'src'), str(root/'tests'/fixture), '-o', str(binary)]
        code = run(host.command(command), folder/(name+'_compile.log'))
        if code:
            raise RuntimeError(f'compile failed: {folder/(name+"_compile.log")}')
        return binary

    prepare(positive)
    results = {'kind': 'actual production routing; OS/NGX leaves substituted', 'windows_build': False,
               'headset_test': False, 'production_files': {str(p.relative_to(root)): sha(p) for p in (cpp_path, runtime_path, view_path)},
               'runs': [], 'negative_controls': []}
    for fixture, name in [('native_dlss_stereo_d82_test.cpp', 'routing'), ('native_upscaler_view_d82_test.cpp', 'view')]:
        binary = compile_case(positive, fixture, name, ['-O2'])
        code = run([str(binary)], positive/(name+'_run.log'))
        results['runs'].append({'name': name, 'passed': code == 0, 'output': (positive/(name+'_run.log')).read_text()})
        assert code == 0, results['runs'][-1]
    if not args.skip_sanitizers:
        binary = compile_case(positive, 'native_dlss_stereo_d82_test.cpp', 'routing_sanitized',
                              ['-O1', '-g', '-fsanitize=address,undefined', '-fno-omit-frame-pointer'])
        env = dict(os.environ, ASAN_OPTIONS='detect_leaks=0')
        code = run([str(binary)], positive/'routing_sanitized_run.log', env=env)
        results['runs'].append({'name': 'routing_asan_ubsan', 'passed': code == 0, 'output': (positive/'routing_sanitized_run.log').read_text()})
        assert code == 0, results['runs'][-1]

    mutations = [
        ('shared_eye_history', runtime.replace('const void* selected=eye?p->right:p->native;', 'const void* selected=p->native;'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('missing_reset', runtime.replace('const bool overrideReset=p->reset[eye] && nativeReset==0;', 'const bool overrideReset=false;'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('pipeline_identity_ignored', runtime.replace('if(view.eye==1 && p->pipeline && p->pipeline!=view.pipeline)', 'if(false)').replace(' || p->pipeline!=view.pipeline)', ')'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('asymmetric_reset_accepted', runtime.replace('if(eye==1 && p->effectiveReset[0]!=effectiveReset)', 'if(false)'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('reset_cut_permanent_fault', runtime.replace('const auto cuts=++g_stats.resetCuts;', 'g_fault.store(Fault::ResetMismatch); const auto cuts=++g_stats.resetCuts;'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('missing_right_continuity_ignored', runtime.replace('if(eye==0 && pairGap)', 'if(false)'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('latest_receipt_only', runtime.replace('auto& receipt=g_receipts[g_receipt_next++%g_receipts.size()];', 'for(auto& old:g_receipts)ClearReceipt(old);\n    auto& receipt=g_receipts[g_receipt_next++%g_receipts.size()];'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('wrong_c_entry', runtime.replace('return Evaluate(g_evaluate_c,c,h,p,callback);', 'return Evaluate(g_evaluate,c,h,p,callback);'), state, accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('queue_sdk_lock_cycle', runtime, state.replace('bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {', 'bool PairReady(std::uint64_t epoch,std::uint32_t frame) noexcept {\n    std::lock_guard<std::recursive_mutex> queueForbidden(g_mutex);'), accessor, 'native_dlss_stereo_d82_test.cpp'),
        ('stale_native_frame', runtime, state, accessor.replace('frame!=f->frame', 'false'), 'native_upscaler_view_d82_test.cpp'),
    ]
    for name, r, s, a, fixture in mutations:
        assert (r, s, a) != (runtime, state, accessor)
        folder = out/name
        prepare(folder, r, s, a)
        binary = compile_case(folder, fixture, 'negative', ['-O2'])
        code = run([str(binary)], folder/'run.log', timeout=15)
        results['negative_controls'].append({'name': name, 'rejected': code != 0, 'exit_code': code})
        assert code != 0, name
    if args.native:
        results['native_contract'] = native_contract(args.native, cpp)
    (out/'RESULT.json').write_text(json.dumps(results, indent=2)+'\n')
    print(json.dumps(results, indent=2))


if __name__ == '__main__':
    main()
