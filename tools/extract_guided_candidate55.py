#!/usr/bin/env python3
"""Actual admitted OnScreen offer -> generic publisher -> public getter fixture."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source, read_source_bytes
from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
p=root/'src/interaction_alignment.cpp'
s=read_source(p, encoding='utf-8').replace('\r\n','\n')
def function(signature):
    begin=s.index(signature);i=s.index('{',begin)+1;depth=1
    while depth:
        depth+=(s[i]=='{')-(s[i]=='}');i+=1
    return s[begin:i]
parts=[function('static void PublishHandCandidate178r16('),function('bool LastHandCandidate('),function('bool HandRayActive() noexcept {')]
call='if(offered)PublishHandCandidate178r16(now,point,ray,split&&known&&!head,candidate.id,candidate.cls,doorGeneration38);'
assert s.count(call)==1
parts.append('static void ProductionOffer55(bool offered,std::uint64_t now,const float* point,const RaySample& ray, bool split,bool known,bool head,const Candidate57c1& candidate,std::uint64_t doorGeneration38) noexcept {\n    '+call+'\n}')
value='// Generated exact production functions/call; native surface leaves are fixture providers.\n'+'\n\n'.join(parts)+'\n'
(root/'tests/generated_guided_candidate55.inc').write_text(value,encoding='utf-8',newline='\n')
(root/'tests/guided_candidate55_provenance.json').write_text(json.dumps({'source':str(p.relative_to(root)),
 'source_sha256':hashlib.sha256(read_source_bytes(p)).hexdigest(),'excerpt_sha256':hashlib.sha256(value.encode()).hexdigest(),
 'production_decisions_rewritten':False,'covered':'actual admitted OnScreen offer call, complete generic publisher, complete getter and current hand-ray gate',
 'not_executed':'SEH projection and retail candidate enumeration; actual emitted metadata and downstream native selected/hint/Guided adapter ARE executed'},indent=2)+'\n',encoding='utf-8',newline='\n')
print('Extracted actual generic palm offer/publication/current-getter chain for Guided F5')

mod=read_source(root/'src/mod.cpp', encoding='utf-8')
a=mod.index('        const auto& alchemyWitness59c2=')
b=mod.index('        const bool interactionBackendEnabled=',a)
pointer=mod[a:b]
(root/'tests/generated_guided_pointer55.inc').write_text('// Generated actual Mod semantic alchemy pointer permission.\n'+pointer,encoding='utf-8',newline='\n')

(root/'tests/guided_pointer55_provenance.json').write_text(json.dumps({'source':'src/mod.cpp',
 'source_sha256':hashlib.sha256(read_source_bytes(root/'src/mod.cpp')).hexdigest(),'excerpt_sha256':hashlib.sha256(pointer.encode()).hexdigest(),
 'production_decisions_rewritten':False,'covered':'actual semantic alchemy ownership: Native/Semi gaze, Free one qualified close-table nominee, legacy palm ray retired'},indent=2)+'\n',encoding='utf-8',newline='\n')
