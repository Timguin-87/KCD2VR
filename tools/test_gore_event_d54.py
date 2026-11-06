"""Execute the C++-emitted maximum D54 event with old/new optional PAK capability.

Generate the input with gore_material_d54_test <emitted.lua>. Requires lupa;
--lua-deps may point to an existing local dependency directory.
"""
import argparse
import hashlib
import json
from pathlib import Path
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--emitted', type=Path, required=True)
    parser.add_argument('--lua-deps', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    if args.lua_deps:
        sys.path.insert(0, str(args.lua_deps))
    from lupa.lua51 import LuaRuntime

    raw = args.emitted.read_bytes()
    code = raw.decode('utf-8')
    results = []
    for enabled in (False, True):
        lua = LuaRuntime(unpack_returned_tuples=True)
        lua.execute("System={LogAlways=function(s) error(s) end}; "
                    "KCD2VR_Gore={protocol=1,Cut=function(e) captured=e end}")
        if enabled:
            lua.execute('KCD2VR_Gore.nativeMaterialD54=true')
        lua.execute(code)
        event = lua.globals().captured
        assert event.target == 123 and event.cut == 'head'
        assert len(event.att) == 24 and len(event.sw) == 16
        assert event.headSkin == 's' * 199
        for i in range(1, 25):
            assert event.att[i].n == 'n' * 63
            assert event.att[i].s == 's' * 199
        if enabled:
            assert event.materialTokenD54 == 'fedcba9876543210'
            assert event.headMtl == 'm' * 259
            for i in range(1, 25):
                assert event.att[i].m == 'm' * 259 and event.att[i].iD54 == i
        else:
            assert event.materialTokenD54 is None and event.headMtl is None
            for i in range(1, 25):
                assert event.att[i].m is None and event.att[i].iD54 is None
        results.append(dict(native_material_capability=enabled, passed=True,
                            old_event_shape_retained=not enabled))
    report = dict(emitted_bytes=len(raw), emitted_sha256=hashlib.sha256(raw).hexdigest(),
                  lua_runtime=lua.eval('_VERSION'), native_execution=False,
                  passed=True, results=results)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
