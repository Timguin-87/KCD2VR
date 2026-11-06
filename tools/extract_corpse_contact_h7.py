from pathlib import Path
import sys, hashlib, json
sys.path.insert(0,str(Path(__file__).resolve().parent))
from source_text import read_source
root=Path(__file__).resolve().parents[1]
source=read_source(root/'src/subsystems/gore/native_part_anchor.inl')
a=source.rfind('\n',0,source.index(' g_corpse_contact_fault_h7;'))+1
b=source.index('// Legacy read-only observation helper',a)
text=source[a:b]
out=Path(sys.argv[1]);out.write_text(text,encoding='utf-8')
out.with_suffix('.json').write_text(json.dumps(dict(source='src/subsystems/gore/native_part_anchor.inl',sha256=hashlib.sha256(text.encode()).hexdigest(),scope='Actual current corpse-body read helper; native status and memory leaves mocked'),indent=2))
