#!/usr/bin/env bash
# Linux/Clang host checks only. Does not build or test a Windows ASI.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="${BUILD_DIR:-$(mktemp -d -t kcd2vr-v6698.XXXXXX)}"
LOG="$ROOT/delivery/v6698"
mkdir -p "$BUILD" "$LOG"
cmake -S "$ROOT/tests" -B "$BUILD/cmake" -G Ninja > "$LOG/cmake_configure.log" 2>&1
cmake --build "$BUILD/cmake" -j4 > "$LOG/cmake_build.log" 2>&1
ctest --test-dir "$BUILD/cmake" --output-on-failure -V > "$LOG/ctest_verbose.log" 2>&1
python3 -m unittest discover -s "$ROOT/tests" -p 'test_*.py' > "$LOG/python_tests.log" 2>&1
clang++ -std=c++17 -fms-extensions -D_ReturnAddress=HostReturnAddress -I"$ROOT/tests/syntax_shims" -I"$ROOT/src" -c "$ROOT/src/frik_pose_probe.cpp" -o "$BUILD/frik.o" > "$LOG/frik_host.log" 2>&1
clang++ -std=c++17 -fms-extensions -D_ReturnAddress=HostReturnAddress -ffunction-sections -fdata-sections -I"$ROOT/tests/syntax_shims" -I"$ROOT/src" "$ROOT/tests/physical_bridge_test.cpp" -Wl,--gc-sections -o "$BUILD/physical_bridge_test" > "$LOG/physical_bridge_compile.log" 2>&1
"$BUILD/physical_bridge_test" > "$LOG/physical_bridge_tests.log" 2>&1
"$BUILD/cmake/physical_weapon_test" > "$LOG/physical_core_tests.log" 2>&1
if [[ "${1:-}" == "--sanitizers" ]]; then
 mkdir -p "$BUILD/san"; : > "$LOG/sanitizers.log"
 for name in pose_math pose_adapter controller_adapter state_queue playback_lab combo_snapshot world_adapter snapshot_mailbox elbow_continuity trigger_ownership physical_weapon physical_frame physical_input; do
  clang++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -pthread "$ROOT/tests/${name}_test.cpp" -o "$BUILD/san/${name}_test" >> "$LOG/sanitizers.log" 2>&1
  (cd "$BUILD/san" && ASAN_OPTIONS=detect_leaks=1 "./${name}_test") >> "$LOG/sanitizers.log" 2>&1
  echo "SANITIZER PASS $name" >> "$LOG/sanitizers.log"
 done
 clang++ -std=c++17 -O1 -g -fms-extensions -D_ReturnAddress=HostReturnAddress -fsanitize=address,undefined -fno-omit-frame-pointer -ffunction-sections -fdata-sections -I"$ROOT/tests/syntax_shims" -I"$ROOT/src" "$ROOT/tests/physical_bridge_test.cpp" -Wl,--gc-sections -o "$BUILD/san/physical_bridge_test" > "$LOG/physical_bridge_sanitizer_compile.log" 2>&1
 (cd "$BUILD/san" && ASAN_OPTIONS=detect_leaks=1 ./physical_bridge_test) > "$LOG/physical_bridge_sanitizer.log" 2>&1
fi
printf 'Host checks passed. Logs: %s\nFull Windows/MSVC, native ABI execution and headset: NOT TESTED.\n' "$LOG"
