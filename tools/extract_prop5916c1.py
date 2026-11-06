#!/usr/bin/env python3
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import read_source
from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1]
s=read_source(r/'src/vrik/staged_pickup596c2.inl')
a=s.index('struct StagedProp596');b=s.index('static bool __fastcall DeferredPickup596',a)
# Preserve actual types and lock. Copy full production consumer without edits.
preamble=s[a:b]
a=s.index('static bool PropInstantContextD78(');b=s.index('static bool BuildStagedPickup596(',a);context=s[a:b]
a=s.index('static void StagedPropTick596()');b=s.index('#include "instant_pickup_native_d72.inl"',a);consumer=s[a:b]
a=s.index('static bool PropPickupHandD81(');b=s.index('static void PropPublish5916C1(',a);pickup_hand=s[a:b]
(r/'tests/prop_adapter5916c1.inc').write_text((preamble+'\n'+context+'\n'+pickup_hand+'\n'+consumer).rstrip()+'\n')
a=s.index('static bool BuildStagedPickup596(');b=s.index('static bool TryStagedPickup596(',a)
(r/'tests/pickup_build_d78.inc').write_text(s[a:b].rstrip()+'\n')
a=s.index('static bool SmallInstantPickupAllowedD81(')
(r/'tests/small_pickup_d81.inc').write_text(s[a:].rstrip()+'\n')
(r/'tests/prop_adapter5916c1_provenance.json').write_text(json.dumps({'source':'src/vrik/staged_pickup596c2.inl','source_sha256':hashlib.sha256((r/'src/vrik/staged_pickup596c2.inl').read_bytes()).hexdigest(),'consumer_sha256':hashlib.sha256(consumer.encode()).hexdigest(),'generated_sha256':{name:hashlib.sha256((r/'tests'/name).read_bytes()).hexdigest() for name in ('prop_adapter5916c1.inc','pickup_build_d78.inc','small_pickup_d81.inc')},'native_leaves':'explicit host spies; no native physics execution'},indent=2))
