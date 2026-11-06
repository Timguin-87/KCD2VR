from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parent))
from source_text import read_source
r=Path(__file__).resolve().parents[1];t=read_source(r/'src/flashui_probe.cpp')
a=t.index('std::uint64_t WaitGenerationB2()');b=t.index('std::uint64_t PauseGeneration()',a)
Path(sys.argv[1]).write_text(t[a:b],encoding='utf-8')
