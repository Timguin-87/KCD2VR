"""Current source contracts after intentional C3 identity/input restructuring.

Historical test originals and their 18 initial structural failures are retained
under review/V66.178R59.16C3. This follows actual producers/consumers; it does not
append fake marker/input strings to production code to satisfy old tests.
"""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
import re

def assert_current_identity(root):
    header=read_source(root/'src/build_identity.h')
    match=re.search(r'^#define KCD2VR_BUILD_MARKER "(KCD2VR-[^"\r\n]+)"$',header,re.M)
    assert match, 'one concrete current marker must exist'
    source=read_source(root/'src/mod.cpp')
    assert '#include "build_identity.h"' in source
    for call in ('debug_trace::Write(KCD2VR_BUILD_MARKER)',
                 'KCD2VR_DIAG_LOG(Bug,warning,"KCD2VR BUILD " KCD2VR_BUILD_MARKER)',
                 'diagnostic_trace::Write("KCD2VR BUILD " KCD2VR_BUILD_MARKER)'):
        assert call in source, call
    bat=(root/'build_release.bat').read_text()
    assert 'tools\\build_release.ps1' in bat
    builder=(root/'tools/build_release.ps1').read_text()
    assert "'src/build_identity.h'" in builder
    assert '$marker=$Matches[1]' in builder
    assert "'--marker',$marker" in builder
    assert 'verify_single_binary_b1.py' in builder
    return match.group(1)

def assert_filtered_pad_route(source):
    copy=source.index('auto padLeft5916c3=routeL,padRight5916c3=routeR;')
    build=source.index('native_vr_pad_premap::Build(padLeft5916c3,padRight5916c3,padCtx)',copy)
    touch=source.index('touch_bindings_.Update(routeL,routeR',build)
    for side in (0,1):
        token=f'FistTransportTrigger101({side},'
        assert source.count(token)==1
        assert source.index(token)<copy<build<touch
    assert source.index('if(physical_rt_owned) routeR.trigger=0.0f;')<copy
    overlay=source[copy:build]
    assert '.ownsGrip(0,' in overlay and '.ownsGrip(1,' in overlay
    assert 'padLeft5916c3.grip=0' in overlay and 'padRight5916c3.grip=0' in overlay
    assert not re.search(r'pad(?:Left|Right)5916c3\.trigger\s*=',overlay)
