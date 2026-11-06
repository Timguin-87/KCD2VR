"""Extract exact current F9 held-source capture/body guards for explicit-leaf tests."""
import sys as _m2_source_sys
from pathlib import Path as _M2SourcePath
_m2_source_sys.path.insert(0, str(_M2SourcePath(__file__).resolve().parents[1] / "tools"))
from source_text import expand_source
from pathlib import Path
import argparse,hashlib,json
parser=argparse.ArgumentParser();parser.add_argument('--check',action='store_true');args=parser.parse_args()
root=Path(__file__).resolve().parents[1]
path=root/'src/subsystems/physics/native_held_owner_t4.inl'
raw=path.read_bytes();source=raw.decode().replace('\r\n','\n')
wrapper=expand_source(root/'src/vrik/native_held_owner_t4.inl')
assert wrapper.text==source, 'M2 wrapper must still resolve exactly to the actual producer'
def scope(name,structure=False):
    a=source.index(name);i=source.index('{',a)+1;depth=1
    while depth:
        depth+=(source[i]=='{')-(source[i]=='}');i+=1
    if structure:
        assert source[i]==';';i+=1
    return source[a:i]+'\n'
types=scope('struct NativeHeldSourceT4 {',True)+scope('struct NativeHeldProfile62 {',True)+scope('struct NativeHeldProfileReceipt62 {',True)
body=scope('static bool NativeHeldBodyT4(')+scope('static const char* NativeHeldProfileReason62(')+scope('static void NativeHeldCaptureT4(')
outputs={name:'// Generated verbatim from production; do not edit.\n'+text for name,text in [('generated_held_capture63_types.inc',types),('generated_held_capture63.inc',body)]}
data={'source':path.relative_to(root).as_posix(),
 'sourceRawSha256':hashlib.sha256(raw).hexdigest(),'source_sha256':hashlib.sha256(source.encode()).hexdigest(),'types_sha256':hashlib.sha256(types.encode()).hexdigest(),
 'body_sha256':hashlib.sha256(body.encode()).hexdigest(),'production_decisions_rewritten':False,
 'wrapperExpansion':wrapper.provenance(),
 'generatedCanonicalUtf8LfSha256':{name:hashlib.sha256(text.encode()).hexdigest() for name,text in outputs.items()},
 'scope':'Actual NativeHeldCaptureT4, NativeHeldBodyT4, profile/receipt structs and guard; explicit native leaves/readable memory providers; no WHGame execution. Prior wrapper extraction already resolved this same M2-owned producer.'}
meta=root/'tests/held_capture63_provenance.json'
if args.check:
    for name,text in outputs.items():assert (root/'tests'/name).read_text()==text, name
    assert json.loads(meta.read_text())==data
    print('PASS exact current held_capture63 source/functions and M2 provenance; no files written')
else:
    for name,text in outputs.items():(root/'tests'/name).write_text(text,encoding='utf-8',newline='\n')
    meta.write_text(json.dumps(data,indent=2)+'\n',encoding='utf-8',newline='\n')
    print('held_capture63: extracted actual source-capture/body/profile guards')
