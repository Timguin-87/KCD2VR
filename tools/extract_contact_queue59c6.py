#!/usr/bin/env python3
"""Exact queue admission, cancellation and active-consumer booking block. No native hit emulation."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1];p=r/'src/diagnostics/geometry_hit178r27.inl';s=read_source(p);meta=[]
def span(start,end):
 a=s.index(start);b=s.index(end,a);t=s[a:b];meta.append({'source':str(p.relative_to(r)),'start_line':s[:a].count('\n')+1,'sha256':hashlib.sha256(t.encode()).hexdigest()});return t
header=span('struct Pending {','struct ActorSeen')
submit=span('bool Submit(const Pending& p)','static bool CallCtor')
booking=span('void Drain(void* henryActor) noexcept {','    const auto drainRecovery65h=')
booking=booking.replace('void Drain(void* henryActor) noexcept {','template<class Action> void DrainBooking59c6(Action action) noexcept {',1)+'    action(p,pendingLease);\n}\n'
(r/'tests/generated_contact_queue59c6.inc').write_text(header+'\n'+submit+'\n'+booking)
(r/'tests/contact_queue59c6_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')

# Actual actor-map observation and target arrival gate. Native memory/identity
# leaves are explicit fixture doubles; retained-body validation is extracted.
meta=[]
target=span('struct ActorSeen {','namespace decap59c7e')
target+=span('std::array<ActorSeen,32> g_actors{};','\n',)+ '\n'
target+=span('void ActorSeen27(const void* actor)','// C2: phase-1 diagnostics only.')
(r/'tests/generated_target_arrival19c3.inc').write_text(target)
(r/'tests/target_arrival19c3_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')

# Actual HYBRID call site and recovery refusal edge, not a parallel model.
meta=[]
stage=span('    // C65H1 TARGET_STAGE_BEGIN','    // C65H1 TARGET_STAGE_END')
(r/'tests/generated_hybrid_target65h.inc').write_text(stage)
entry=span('    const auto drainRecovery65h=','    const auto now=std::uint64_t(GetTickCount64());')
(r/'tests/generated_hybrid_entry65h.inc').write_text(entry)
claim=span('    // C65H1 TARGET_STAGE_END','        if(noRelease54){').split('\n',1)[1]+'    }\n'
(r/'tests/generated_hybrid_claim65h.inc').write_text(claim)
live=span('            const auto liveRecovery65h=','            bool survived=false,serialEntered59=false;')
(r/'tests/generated_hybrid_live65h.inc').write_text(live)
(r/'tests/hybrid_target65h_extraction.json').write_text(json.dumps(meta,indent=2)+'\n')

# Complete diagnostic-only ray adapter: supplied native query and memory leaves
# are replaced explicitly in the host fixture, not in production.
p=r/'src/vrik/feature_probe59c6.inl';s=read_source(p);a=s.index('struct Result {');b=s.index('static bool Exclusions',a);t=s[a:b]
(r/'tests/generated_feature_ray59c6.inc').write_text(t)
(r/'tests/feature_ray59c6_extraction.json').write_text(json.dumps({'source':str(p.relative_to(r)),'start_line':s[:a].count('\n')+1,'sha256':hashlib.sha256(t.encode()).hexdigest()},indent=2)+'\n')
