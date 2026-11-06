from pathlib import Path
import hashlib,json,sys
r=Path(__file__).resolve().parents[1]
p=r/'src/subsystems/hybrid/drain_cost_claim.inl'
s=p.read_text(); start=s.index('        if(noRelease54){');end=s.index('\n        }\n    }',start)+10
body=s[start:end]
(r/'tests/generated_hybrid_quiet_guard_h20.inc').write_text(body+'\n')
(r/'tests/hybrid_quiet_guard_h20_provenance.json').write_text(json.dumps({'source':str(p.relative_to(r)),'source_sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'body_sha256':hashlib.sha256(body.encode()).hexdigest(),'scope':'Exact production no-release phase/ownership/cost decision; native owner witness supplied as an explicit leaf; real cost and motion ledgers.'},indent=2)+'\n')
