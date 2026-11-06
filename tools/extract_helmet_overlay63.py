"""Extract actual event-bounded native wrapper; only its native leaves are doubled."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
import hashlib
import json
from pathlib import Path
import sys

source = Path(__file__).resolve().parents[1]/'src/flashui_probe.cpp'
text = read_source(source)
begin = text.index('std::uint64_t HelmetGeneration63() noexcept')
end = text.index('std::uint64_t WaitGenerationB2()', begin) if 'std::uint64_t WaitGenerationB2()' in text else text.index('std::uint64_t PauseGeneration()', begin)
out = Path(sys.argv[1]); out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(text[begin:end], encoding='utf-8', newline='\n')
Path(str(out)+'.json').write_text(json.dumps({'source': 'src/flashui_probe.cpp',
    'source_sha256': hashlib.sha256(read_source_bytes(source)).hexdigest(),
    'extracted_sha256': hashlib.sha256(text[begin:end].encode()).hexdigest(),
    'boundary': 'actual HelmetGeneration63 and ApplyHelmetOverlay63; native Flash leaves doubled, guards not replaced'}, indent=2)+'\n')
