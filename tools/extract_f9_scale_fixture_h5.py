"""Compose existing explicit native providers around ACTUAL capture and backend."""
from pathlib import Path
import argparse,hashlib,json
p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');args=p.parse_args()
r=Path(__file__).resolve().parents[1]
b=(r/'tests/native_weapon_body_t4_test.cpp').read_text();b=b[:b.index('int main(')]
old='return fixture::suspended&&id==11&&body==reinterpret_cast<void*>(0x700)&&entity==reinterpret_cast<void*>(0x400);'
assert b.count(old)==1;b=b.replace(old,'return fixture::suspended&&CapturedItemSourceH5(id,body,entity);')
old='return fixture::living&&id==12&&body==reinterpret_cast<void*>(0x900)&&entity==reinterpret_cast<void*>(0xA00);'
assert b.count(old)==1;b=b.replace(old,'return fixture::living&&CapturedPlayerSourceH5(id,body,entity);')
t=(r/'tests/native_held_capture63_test.cpp').read_text();t=t[:t.index('static unsigned checks=')]
a=t.index('namespace kcd2vr::native_weapon_body_t4 {');e=t.index('#include "generated_held_capture63_types.inc"',a)
t=t[:a]+t[e:] # actual backend supplies Enabled/SetSourcePredicate
a=t.index('static bool GetEntityWorldTmV6552(');e=t.index('#include "generated_held_capture63.inc"',a)
t=t[:a]+'''static Matrix34V6550 sourceWorldH5{};
static bool GetEntityWorldTmV6552(void* p,std::uint32_t id,Matrix34V6550& tm){
 if(p!=entity.data()||id!=42)return false;tm=sourceWorldH5;return true;
}
'''+t[e:]
text='''// Generated provider composition only; production capture/backend decisions unchanged.
static bool CapturedItemSourceH5(int,void*,const void*) noexcept;
static bool CapturedPlayerSourceH5(int,void*,const void*) noexcept;
'''+b+t
out=r/'tests/generated_f9_scale_fixture_h5.inc';meta=r/'tests/f9_scale_fixture_h5_provenance.json'
data={'sourceInputs':{p:hashlib.sha256((r/p).read_bytes()).hexdigest() for p in ['tests/native_weapon_body_t4_test.cpp','tests/native_held_capture63_test.cpp','src/subsystems/physics/native_held_owner_t4.inl','src/subsystems/physics/runtime/native_weapon_body_t4.cpp']},'generatedCanonicalUtf8LfSha256':hashlib.sha256(text.encode()).hexdigest(),'scope':'Reuse exact actual backend .cpp, generated actual Capture functions and original native-provider/setup code. Only providers changed: use supplied Matrix34 instead of identity; phase native source-ID leaves compare actual Capture output instead of hardcoded sentinel pointers; remove duplicated backend public API stubs. Original fixture mains not executed; production decisions not rewritten.'}
if args.check:
 assert out.read_text()==text and json.loads(meta.read_text())==data
 print('PASS exact current F9 provider composition; no files written')
else:
 out.write_text(text,encoding='utf-8',newline='\n');meta.write_text(json.dumps(data,indent=2)+'\n');print('Extracted current F9 production-connected explicit providers')
