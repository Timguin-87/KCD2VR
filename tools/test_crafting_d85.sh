#!/usr/bin/env bash
# D85 craft: build + run the D85 crafting tests and their negative controls.
# Usage: tools/test_crafting_d85.sh [build_dir]   (g++ >= 11, Linux host)
set -u
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="${1:-${TMPDIR:-/tmp}/kcd2vr_d85_craft}"
mkdir -p "$OUT"
SHIM=(-D__fastcall= -D__cdecl= -D__stdcall= -D__thiscall=)
fail=0
run(){ # name expect(0=pass,1=fail) srcroot file extra...
  local name=$1 expect=$2 src=$3 file=$4; shift 4
  if ! g++ -std=c++23 -w "${SHIM[@]}" "$@" -I "$src/src" -I "$src/src/vrik" -I "$src/tests" -I "$src/tests/subsystems/alchemy" -I "$src/tests/syntax_shims" -I "$src/tests/openxr_sdk" -o "$OUT/$name" "$src/tests/$file" -lpthread 2>"$OUT/$name.build.txt"; then
    echo "$name: BUILD_FAILED (see $OUT/$name.build.txt)"; fail=1; return; fi
  "$OUT/$name" >"$OUT/$name.txt" 2>&1; local rc=$?
  if { [ "$expect" = 0 ] && [ $rc = 0 ]; } || { [ "$expect" = 1 ] && [ $rc != 0 ]; }; then echo "$name: OK ($(tail -1 "$OUT/$name.txt"))"
  else echo "$name: UNEXPECTED rc=$rc"; cat "$OUT/$name.txt" | tail -5; fail=1; fi
}
run carafe_pour_d85 0 "$ROOT" carafe_pour_d85_test.cpp -O1
run carafe_pour_d85_asan 0 "$ROOT" carafe_pour_d85_test.cpp -O1 -fsanitize=address,undefined -fno-omit-frame-pointer
run crafting_clock_d81 0 "$ROOT" crafting_clock_d81_test.cpp -O1
run carafe_intent_d85 0 "$ROOT" subsystems/alchemy/carafe_intent_d85_test.cpp -O1
run smith_rt_d85 0 "$ROOT" smith_rt_unlock_d85_test.cpp -O1
run mortar_wait_d85 0 "$ROOT" mortar_long_grind_d85_test.cpp -O1
run mortar_withdraw_d78r2 0 "$ROOT" alchemy_mortar_withdraw_native_d78r2_test.cpp -O1
python3 "$ROOT/tools/test_lt_default_d85.py" "$ROOT" || fail=1
# Negative controls: the same tests against the unpatched D84R1 production code.
if git -C "$ROOT" rev-parse --verify -q D84R1_BASE >/dev/null 2>&1; then BASE=D84R1_BASE; else BASE=$(git -C "$ROOT" rev-list --max-parents=0 HEAD | tail -1); fi
NEG="$OUT/baseline"; rm -rf "$NEG"; mkdir -p "$NEG"
git -C "$ROOT" archive "$BASE" src tests | tar -x -C "$NEG"
# Only the new pure header, the new bridge field and the new tests are added;
# every production behaviour stays the baseline one.
cp "$ROOT/src/subsystems/alchemy/carafe_pour_d85.h" "$NEG/src/subsystems/alchemy/"
python3 - "$NEG/src/subsystems/alchemy/bridge67.h" <<'PY'
import sys;p=sys.argv[1];s=open(p,newline='').read()
k="    bool clockAdmissionD47=false; // exact admitted bellows parent may be paused before its hand-type grant\r\n};"
s=s.replace(k,k[:-3]+"\r\n    unsigned carafeD85=0;\r\n};");open(p,'w',newline='').write(s)
PY
cp "$ROOT/tests/subsystems/alchemy/carafe_intent_d85_test.cpp" "$NEG/tests/subsystems/alchemy/"
cp "$ROOT/tests/carafe_pour_d85_test.cpp" "$ROOT/tests/smith_rt_unlock_d85_test.cpp" "$ROOT/tests/mortar_long_grind_d85_test.cpp" "$NEG/tests/"
run neg_carafe_pour_d85 1 "$NEG" carafe_pour_d85_test.cpp -O1 -DKCD2VR_D85_NEGATIVE_BASELINE
run neg_carafe_intent_d85 1 "$NEG" subsystems/alchemy/carafe_intent_d85_test.cpp -O1
run neg_smith_rt_d85 1 "$NEG" smith_rt_unlock_d85_test.cpp -O1
run neg_mortar_wait_d85 1 "$NEG" mortar_long_grind_d85_test.cpp -O1
python3 "$ROOT/tools/test_lt_default_d85.py" "$NEG" --expect-fail || fail=1
[ $fail = 0 ] && echo "D85 crafting: ALL OK" || echo "D85 crafting: FAILURES"
exit $fail
