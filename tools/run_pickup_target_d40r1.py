#!/usr/bin/env python3
"""Build/run five connected Windows pickup checks without historical extractors."""
import argparse
import json
from pathlib import Path
import subprocess
import sys

CMAKE = r'''
cmake_minimum_required(VERSION 3.20)
project(D40R1_Pickup_Connected LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)
enable_testing()
foreach(NAME pickup_target_d40r1 pickup_handoff_h5 door_producer46 door_direct54r1)
  add_executable(${NAME} "${REPO}/tests/${NAME}_test.cpp")
  target_include_directories(${NAME} PRIVATE "${REPO}/tests/syntax_shims" "${REPO}/src")
  target_compile_options(${NAME} PRIVATE /UNDEBUG /Gy /Gw /bigobj /FI"${REPO}/tests/host_test_runtime.h")
  target_link_options(${NAME} PRIVATE /OPT:REF /STACK:8388608)
  target_link_libraries(${NAME} PRIVATE Threads::Threads)
  add_test(NAME ${NAME} COMMAND ${NAME})
  set_tests_properties(${NAME} PROPERTIES TIMEOUT 30)
endforeach()
foreach(NAME pickup_target_d40r1 pickup_handoff_h5)
  target_sources(${NAME} PRIVATE "${REPO}/tests/pickup_target_d40r1_stubs.cpp")
endforeach()
target_compile_options(pickup_handoff_h5 PRIVATE /FI"${CMAKE_CURRENT_SOURCE_DIR}/native_seh.h")
target_compile_options(door_direct54r1 PRIVATE /FI"${REPO}/src/subsystems/doors/hand_only_h40.h")
# Actual Windows SDK header-first regression: no syntax_shims include path.
add_executable(pickup_windows_headers_d40r1 "${REPO}/tests/pickup_windows_headers_d40r1_test.cpp")
target_include_directories(pickup_windows_headers_d40r1 PRIVATE "${REPO}/src")
target_compile_options(pickup_windows_headers_d40r1 PRIVATE /UNDEBUG /FI"${REPO}/tests/host_test_runtime.h")
add_test(NAME pickup_windows_headers_d40r1 COMMAND pickup_windows_headers_d40r1)
'''

NATIVE_SEH = '''#pragma once
// Keep explicit fixture OS leaves, but use actual MSVC SEH keywords.
#include <windows.h>
#undef __try
#undef __except
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', required=True, type=Path)
    parser.add_argument('--config', default='Release', choices=['Release', 'RelWithDebInfo'])
    args = parser.parse_args()
    repo = Path(__file__).resolve().parents[1]
    output = args.out.resolve()
    harness = output / 'harness'
    build = output / 'build'
    harness.mkdir(parents=True, exist_ok=True)
    (harness / 'CMakeLists.txt').write_text(CMAKE, encoding='utf-8')
    (harness / 'native_seh.h').write_text(NATIVE_SEH, encoding='utf-8')
    commands = [
        ('configure', ['cmake', '-S', str(harness), '-B', str(build), '-G',
                       'Visual Studio 17 2022', '-A', 'x64', '-DREPO=' + repo.as_posix()]),
        ('build', ['cmake', '--build', str(build), '--config', args.config, '--parallel', '3']),
        ('ctest', ['ctest', '--test-dir', str(build), '-C', args.config, '--output-on-failure']),
    ]
    results = {'source': str(repo), 'configuration': args.config, 'assertions': '/UNDEBUG',
               'scope': 'five focused host fixtures; no retail execution/headset claim', 'steps': []}
    exit_code = 0
    for name, command in commands:
        with (output / (name + '.log')).open('wb') as log:
            process = subprocess.run(command, cwd=repo, stdout=log, stderr=subprocess.STDOUT)
        results['steps'].append({'name': name, 'command': command, 'exit': process.returncode})
        print(f'{name}: exit {process.returncode}; log: {output / (name + ".log")}')
        if process.returncode:
            exit_code = process.returncode
            break
    results['passed'] = exit_code == 0 and len(results['steps']) == len(commands)
    (output / 'RESULT.json').write_text(json.dumps(results, indent=2), encoding='utf-8')
    if results['passed']:
        print((output / 'ctest.log').read_text(encoding='utf-8', errors='replace'))
    return exit_code


if __name__ == '__main__':
    sys.exit(main())
