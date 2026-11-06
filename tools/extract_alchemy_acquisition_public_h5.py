#!/usr/bin/env python3
"""Public mode-to-input fixture from exact current Mod producer statements."""
from pathlib import Path
import argparse,hashlib,json
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--check',action='store_true');args=p.parse_args()
r=Path(__file__).resolve().parents[1];source=r/'src/mod.cpp';text=source.read_text()
start=text.index('        const bool alchemyPalmExperiment63=')
end=text.index('subsystems::alchemy::bridge67::enabled.store(alchemyGesture67,std::memory_order_release);',start)+len('subsystems::alchemy::bridge67::enabled.store(alchemyGesture67,std::memory_order_release);')
selection=text[start:end]
start=text.index('                const bool oneCraft67=')
end=text.index('combat_rules178r10::workstationEnabled.load(std::memory_order_acquire);',start)+len('combat_rules178r10::workstationEnabled.load(std::memory_order_acquire);')
context=text[start:end]
generated='// Generated exact Mod statements. Native activity/context and copied world input are fixture leaves.\nstruct PublicAlchemyGatesH5 {bool gesture=false,head=false,legacyPointer=false;};\nstatic PublicAlchemyGatesH5 PublicAlchemySelectionH5(bool alchemyAvailable63) {\nusing namespace kcd2vr;\n'+selection+'\nreturn {alchemyGesture67,alchemyHeadSelection63,alchemyPointer59c2};\n}\nstatic void PublicAlchemyContextH5(kcd2vr::semi_workstations46::Context& c,std::uint64_t now46) {\nusing namespace kcd2vr;\n'+context+'\n}\n'
out=r/'tests/subsystems/alchemy/generated_acquisition_public_h5.inc';manifest=r/'tests/subsystems/alchemy/acquisition_public_h5_provenance.json'
data={'source':'src/mod.cpp','sourceSha256':hashlib.sha256(source.read_bytes()).hexdigest(),'generated':out.relative_to(r).as_posix(),'generatedSha256':hashlib.sha256(generated.encode()).hexdigest(),'scope':'Exact public mode/gesture/head fallback selection and Context derivation statements; native activity availability and current copied world source remain explicit leaves. No native ABI/state/action substitute.'}
if args.check:
 if out.read_text()!=generated or json.loads(manifest.read_text())!=data:raise SystemExit('Alchemy public acquisition fixture stale; regenerate from current producer source.')
 print('PASS exact public alchemy producer statements; no files written')
else:out.write_text(generated);manifest.write_text(json.dumps(data,indent=2)+'\n')
