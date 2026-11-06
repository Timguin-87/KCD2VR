"""Run zero-argument function tests which unittest discovery does not collect."""
from pathlib import Path
import inspect
import runpy
import traceback

root = Path(__file__).resolve().parents[1]
passed = failed = skipped = 0
for path in sorted((root / 'tests').glob('test_*.py')):
    scope = runpy.run_path(str(path))
    for name, function in scope.items():
        if not name.startswith('test_') or not inspect.isfunction(function):
            continue
        if inspect.signature(function).parameters:
            print('REQUIRES FIXTURE', path.name, name)
            skipped += 1
            continue
        try:
            function()
            passed += 1
            print('PASS', path.name, name)
        except Exception:
            failed += 1
            traceback.print_exc()
print(f'Function tests: {passed} passed, {failed} failed, {skipped} require fixtures')
raise SystemExit(1 if failed or skipped else 0)
