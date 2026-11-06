"""Extract the actual mod control plane; no parallel policy implementation."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib
import json
import sys

root = Path(__file__).resolve().parents[1]
source = root / 'src/mod.cpp'
text = read_source(source)
start = text.index('static std::atomic<int> g_physical_held_f9_t4')
end = text.index('static std::atomic<bool> g_inventory_early_exit25', start)
result = text[start:end]
dest = Path(sys.argv[1])
dest.write_text(result)
metadata = {
    'source': 'src/mod.cpp', 'source_sha256': hashlib.sha256(read_source_bytes(source)).hexdigest(),
    'extracted_sha256': hashlib.sha256(result.encode()).hexdigest(),
    'scope': 'Actual F9 control-plane only; no native physics acceptance'
}
if len(sys.argv) > 2:
    # Optional literal input-wiring fixture; production stays in its existing TU.
    begin = '// CG1_CORPSE_INPUT_BEGIN\n'
    finish = '// CG1_CORPSE_INPUT_END'
    first = text.index(begin) + len(begin)
    corpse = text[first:text.index(finish, first)]
    corpse_dest = Path(sys.argv[2])
    corpse_dest.write_text(corpse)
    metadata['corpse_input_fixture'] = {
        'path': str(corpse_dest),
        'sha256': hashlib.sha256(corpse.encode()).hexdigest(),
        'scope': 'Actual ordinary pulse/held input boundary; native leaves remain controlled'
    }
dest.with_suffix('.json').write_text(json.dumps(metadata, indent=2) + '\n')
