from pathlib import Path
import hashlib,json
S=Path(__file__).resolve().parents[1]
p=S/'src/vrik/physical_native.inl';s=p.read_text()
a=s.index('    ph::Command c{};int budget=8;\n    while(budget--&&g_phys_commands6698.peek(c)&&c.sample<=frame.input.sample){',s.index('static void PhysNativeTickImpl6698()'))
b=s.index('#include "../subsystems/throw_drop/native_equipment_commands.inl"',a)
chunk=s[a:b]
assert chunk.count('PhysLeftEquipmentH36(frame,e,c)')==1
# All prefix branches are verbatim. The sole fixture exit is at the existing
# native equipment include boundary; the actual native request is not executed.
text='// Generated from the actual native consumer prefix; no native leaf substituted.\nstatic int ActualEquipmentPrefixH36(const PhysFrame6698& frame,const PhysEquipment6698& e,std::uint64_t now) {\n    std::lock_guard<std::recursive_mutex> lock(g_phys_native_mutex6698);\n    auto& n=g_phys_native6698;const auto nativeFrame=++n.dispatchFrame;\n'+chunk+'    return 1; // next production instruction is native_equipment_commands.inl\n    }\n    return 0;\n}\n'
q=S/'tests/generated_holster_side_retirement_h36.inc';q.write_text(text,newline='\n')
(S/'tests/generated_holster_side_retirement_h36.provenance.json').write_text(json.dumps({'source':str(p.relative_to(S)).replace('\\','/'),'sourceSHA256':hashlib.sha256(p.read_bytes()).hexdigest(),'prefixSHA256':hashlib.sha256(chunk.encode()).hexdigest(),'lineFirst':s[:a].count('\n')+1,'lineLast':s[:b].count('\n'),'generatedSHA256':hashlib.sha256(q.read_bytes()).hexdigest(),'limitation':'Fixture stops before native equipment call; no native request, animation or ACK fabricated.'},indent=2),newline='\n')
