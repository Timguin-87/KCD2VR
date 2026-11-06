"""Extract current XR boolean read and exact B qualification; no runtime execution."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib,json,sys
r=Path(__file__).resolve().parents[1]
path=r/'src/xr_input.cpp';text=read_source(path)
begin=text.index('bool XrInput::GetBoolState(')
end=text.index('bool XrInput::TryLocateGripPair(',begin)
reader=text[begin:end]
anchor='{ // Recovery cannot use an inactive action\'s false level as a neutral witness.'
begin=text.index(anchor);end=text.index('\n    }',begin)+len('\n    }')
block=text[begin:end]
out=Path(sys.argv[1]);out.parent.mkdir(parents=True,exist_ok=True)
body=reader+'\nvoid XrInput::ReadCurrentBForTest()\n'+block+'\n'
out.write_text(body)
out.with_suffix('.json').write_text(json.dumps({'source':'src/xr_input.cpp','sourceSHA256':hashlib.sha256(read_source_bytes(path)).hexdigest(),'extractedSHA256':hashlib.sha256(body.encode()).hexdigest(),'nativeExecution':False},indent=2)+'\n')
