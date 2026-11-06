#!/usr/bin/env python3
"""Read-only current-release identity and HUD wiring checks (not Windows execution)."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import json, re, sys

def main() -> int:
    root=Path(__file__).resolve().parents[1]
    try:
        header=read_source(root/'src/build_identity.h')
        marker=re.search(r'^#define KCD2VR_BUILD_MARKER "([^"]+)"',header,re.M)
        if not marker:raise ValueError('Missing current marker')
        batch=(root/'build_release.bat').read_text()
        mod=read_source(root/'src/mod.cpp')
        checks={
          'marker':marker.group(1)=='KCD2VR-V66.178R59.16C7-HUD-VITALS-REUSE',
          'shared_header':'src\\build_identity.h' in batch and '#define KCD2VR_BUILD_MARKER ' in batch,
          'startup_macro':'KCD2VR_BUILD_MARKER' in mod and '#include "build_identity.h"' in mod,
          'mandatory_binary_marker':'findstr /m /c:"%MARKER%" "%ASI%"' in batch,
          'common_ini_restore':':finish' in batch and 'copy /y "%PRESERVED_INI%" "%RESTORE_DIR%\\KCD2VR.ini"' in batch,
          'capture_only_branch':'SHIFT_F2_CAPTURE_ONLY' in mod,
          'current_child_offsets':'vr_ui_stats_x_.load(),vr_ui_stats_y_.load()' in mod,
        }
        print(json.dumps({'marker':marker.group(1),'checks':checks,'passed':all(checks.values()),'windows_executed':False},indent=2))
        return 0 if all(checks.values()) else 1
    except (OSError,ValueError) as e:
        print(str(e),file=sys.stderr);return 2
if __name__=='__main__':raise SystemExit(main())
