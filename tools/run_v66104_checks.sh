#!/usr/bin/env bash
# Host-only checks: no Windows SDK/link, no game calls, no headset claim.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-$ROOT/delivery/v66104/host}"
SAN="${2:-}"
mkdir -p "$OUT"
CXX="${CXX:-clang++}"
FLAGS="-O0 -g0"
if [[ "$SAN" == "--sanitizers" ]]; then FLAGS="$FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer"; fi
cmake -S "$ROOT/tests" -B "$OUT/cmake" -G Ninja -DCMAKE_CXX_COMPILER="$CXX" -DCMAKE_CXX_FLAGS="$FLAGS" >"$OUT/configure.log" 2>&1
cmake --build "$OUT/cmake" -j2 >"$OUT/build.log" 2>&1
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 ctest --test-dir "$OUT/cmake" --output-on-failure -V >"$OUT/ctest.log" 2>&1
for test in fist calibration physical native_contract native104; do
 "$CXX" -std=c++17 $FLAGS -fms-extensions -D_ReturnAddress=HostReturnAddress -ffunction-sections -fdata-sections -I"$ROOT/tests/syntax_shims" -I"$ROOT/src" "$ROOT/tests/${test}_bridge_test.cpp" -Wl,--gc-sections -pthread -o "$OUT/${test}_bridge" >"$OUT/${test}_bridge_compile.log" 2>&1
 ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 "$OUT/${test}_bridge" >"$OUT/${test}_bridge.log" 2>&1
done
"$CXX" -std=c++17 -fsyntax-only -fms-extensions -D_ReturnAddress=HostReturnAddress -I"$ROOT/tests/syntax_shims" -I"$ROOT/src" "$ROOT/src/animation_filter.cpp" >"$OUT/animation_syntax.log" 2>&1
python -m unittest discover -s "$ROOT/tests" -p 'test_*.py' >"$OUT/python.log" 2>&1
printf 'PASS host-only suites, five complete-frik fixtures, actual xr_input consumer, animation syntax and Python tests\n'
