"""Reuse actual held capture with explicit native providers, not a policy clone."""
from pathlib import Path
import argparse,hashlib,json,re
p=argparse.ArgumentParser();p.add_argument('--check',action='store_true');args=p.parse_args()
r=Path(__file__).resolve().parents[1]
source=r/'tests/native_held_capture63_test.cpp'
t=source.read_text();t=t[:t.index('static unsigned checks=')]
a=t.index('namespace kcd2vr::native_weapon_body_t4 {');e=t.index('#include "generated_held_capture63_types.inc"',a)
t=t[:a]+t[e:] # use actual backend Enabled/SetSourcePredicate, not duplicate stubs
t='\n'.join(line for line in t.splitlines() if not line.startswith('#include ') or 'generated_held_capture63' in line)+'\n'
old='static std::array<unsigned char,0xB00> player{},item{},entity{},link{},model{},attachment{},object{},manager{},character{},weaponProxy{},weaponBody{},playerEntity{},playerProxy{},playerBody{};'
assert t.count(old)==1
t=t.replace(old,'''static std::array<unsigned char,0xB00> player{},item{},entity{},link{},model{},attachment{},object{},manager{},character{},weaponProxy{},playerEntity{},playerProxy{};
static auto& weaponBody=::fixture::nativeItem;
static auto& playerBody=::fixture::henry;''')
out=r/'tests/generated_weapon_self_pair_capture_h5.inc'
text='// Generated provider composition; actual capture decisions remain verbatim.\n'+t
meta=r/'tests/weapon_self_pair_capture_h5_provenance.json'
data={'sourceInputs':{str(x.relative_to(r)).replace('\\','/'):hashlib.sha256(x.read_bytes()).hexdigest() for x in [source,r/'src/subsystems/physics/native_held_owner_t4.inl',r/'tests/generated_held_capture63.inc',r/'tests/generated_held_capture63_types.inc']},
 'generatedCanonicalUtf8LfSha256':hashlib.sha256(text.encode()).hexdigest(),
 'scope':'NativeHeldCaptureT4 and NativeHeldBodyT4 are verbatim existing extraction. Reuse existing explicit native-memory providers; alias only suspended item/Henry body storage into actual typed phase providers. Remove duplicate backend public API stubs and redundant includes. Native leaves/OS/hook dispatch are substituted; production capture, session stamp, weapon owner and typed pair APIs are not rewritten.'}
if args.check:
 assert out.read_text()==text and json.loads(meta.read_text())==data
 print('PASS actual capture/provider composition; no files written')
else:
 out.write_text(text,encoding='utf-8',newline='\n');meta.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8',newline='\n')
 print('Extracted actual capture/native body providers for full typed weapon self-pair fixture')
