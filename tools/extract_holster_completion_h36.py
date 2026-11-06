from pathlib import Path
import hashlib,json,argparse
ap=argparse.ArgumentParser();ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);ap.add_argument('--output',type=Path);a=ap.parse_args();S=a.source;O=a.output or S/'tests';O.mkdir(exist_ok=True)
leaf=S/'src/subsystems/throw_drop/native_equipment_leaves.inl';native=S/'src/vrik/physical_native.inl';commands=S/'src/subsystems/throw_drop/native_equipment_commands.inl'
s=leaf.read_text();start=s.index('static bool PhysEquipmentCommand6698(');end=s.index('// These small policies',start);chunk=s[start:end].replace('PhysEquipmentCommand6698(', 'ActualEquipmentCommandH36(',1)
replacements={
'reinterpret_cast<bool(__fastcall*)(void*,std::uint64_t)>(WhBase()+0x8CF158)':'HostStowH36',
'reinterpret_cast<void(__fastcall*)(void*,std::uint64_t,int)>(WhBase()+0x8CEA48)':'HostDrawH36',
'reinterpret_cast<void(__fastcall*)(void*,int)>(WhBase()+0x8EEA04)':'HostSelectH36',
'reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x2AA24A0)':'HostLegacyDrawH36',
'reinterpret_cast<void(__fastcall*)(void*)>(WhBase()+0x8EE994)':'HostLegacyStowH36'}
for old,new in replacements.items():assert old in chunk;chunk=chunk.replace(old,new)
s=native.read_text();start=s.index('    ph::Command c{};int budget=8;\n    while(budget--&&g_phys_commands6698.peek(c)&&c.sample<=frame.input.sample){',s.index('static void PhysNativeTickImpl6698()'));end=s.index('#include "../subsystems/throw_drop/native_equipment_commands.inl"',start)
pre=s[start:end];body=commands.read_text().replace('PhysEquipmentCommand6698(', 'ActualEquipmentCommandH36(')
text='// Generated actual native leaf and consumer prefix; ONLY native primitive leaves have explicit host providers.\n'+chunk+'\nstatic void ActualCompletionDrainH36(const PhysFrame6698& frame,const PhysEquipment6698& e,std::uint64_t now){\n std::lock_guard<std::recursive_mutex> lock(g_phys_native_mutex6698);auto& n=g_phys_native6698;const auto nativeFrame=++n.dispatchFrame;\n'+pre+body+'\n return; // remaining attack-only consumer is outside this equipment fixture\n }\n}\n'
q=O/'generated_holster_completion_h36.inc';q.write_text(text,newline='\n')
(O/'generated_holster_completion_h36.provenance.json').write_text(json.dumps({'sources':{str(p.relative_to(S)):hashlib.sha256(p.read_bytes()).hexdigest() for p in [leaf,native,commands]},'nativeLeafReplacements':replacements,'generatedSHA256':hashlib.sha256(q.read_bytes()).hexdigest(),'baselineOrCandidate':str(S),'limitations':'Native primitives explicit host services. No retail native machine code, native attachment queue, animation or game outcome executed.'},indent=2),newline='\n')
