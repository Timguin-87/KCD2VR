"""Exercise T6 readiness labels through the actual packed native-supply Lua.

Reuses the existing supply fixture and its 51 checks without modifying it.
Native leaves remain substitutions; this is not retail inventory acceptance.
"""
import argparse
import contextlib
import io
import json
from pathlib import Path
import runpy
import sys

ROOT = Path(__file__).resolve().parents[3]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--pak", type=Path, required=True)
    parser.add_argument("--report", type=Path, required=True)
    args = parser.parse_args()
    original = sys.argv
    historical_report = ROOT / "review/THROW2_SUPPLY/LUA51_TEST.json"
    original_report = historical_report.read_bytes() if historical_report.exists() else None
    try:
        sys.argv = ["test_native_supply_t2.py", "--pak", str(args.pak)]
        with contextlib.redirect_stdout(io.StringIO()):
            fixture = runpy.run_path(str(ROOT / "tools/test_native_supply_t2.py"), run_name="__main__")
    finally:
        sys.argv = original
        # The historical runner writes to a fixed report path. Preserve its
        # existing evidence; this test owns only the explicitly selected report.
        if original_report is None:
            historical_report.unlink(missing_ok=True)
        else:
            historical_report.write_bytes(original_report)
    checks = []
    for status, expected in (
        ("ALREADY_CLAIMED", "STARTER_ALREADY_CLAIMED"),
        ("CLAIM_REFUSED", "STARTER_CLAIM_REFUSED"),
        ("PARTIAL_UNCERTAIN", "STARTER_UNCERTAIN"),
    ):
        lua = fixture["make"]()
        lua.execute("KCD2VRThrowingAssets1.NativeReady=function() return '" + status + "' end")
        native = lua.globals()
        assert native.invoke(1, "refresh", 0) == expected + "|1|5|3", status
        assert native.liveCount() == 8 and native.itemCreates == 0, status
        checks.append(status + " reports " + expected + "; owned supply stays usable; no item creation")
    report = {"result": "PASS", "new_status_checks": len(checks),
              "existing_supply_checks": len(fixture["checks"]), "details": checks,
              "pak": str(args.pak), "native_leaves": "existing documented substitutions",
              "retail_inventory": "NOT_EXECUTED", "headset": "NOT_EXECUTED"}
    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print("PASS: 3 T6 readiness mappings and", len(fixture["checks"]), "existing supply checks")


if __name__ == "__main__":
    main()
