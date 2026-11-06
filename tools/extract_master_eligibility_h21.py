from pathlib import Path
import sys,json,hashlib
root=Path(__file__).resolve().parents[1]
p=root/"src/vrik/master_eligibility_h21.inl"
s=p.read_text()
out=Path(sys.argv[1]);out.parent.mkdir(parents=True,exist_ok=True)
body=s[:s.index('#include "master_eligibility_pins_h21.h"')].replace('#include "../diagnostics/master_eligibility_h21.h"',"")
out.write_text(body)
out.with_suffix(".provenance.json").write_text(json.dumps({"source":str(p),"sourceSha256":hashlib.sha256(p.read_bytes()).hexdigest(),"generatedSha256":hashlib.sha256(out.read_bytes()).hexdigest(),"method":"Verbatim prefix containing actual outer, mid, context and teardown functions; native/hook leaves explicit in fixture."},indent=2))
