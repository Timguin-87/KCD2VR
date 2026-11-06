from pathlib import Path
import hashlib, json, sys
from source_text import read_source
root=Path(__file__).resolve().parents[1]
source_path='src/subsystems/gore/native_part_anchor.inl'
source=read_source(root/source_path)
if len(sys.argv)>1 and sys.argv[1]=='--impale-options':
 source_path='src/subsystems/base/pose_physical_executor.inl'
 source=read_source(root/source_path)
 start=source.index('        pi.impaleHang32=')
 body=source[start:source.index('        if(pi.impaleEnabled178r15)',start)]
 owner=read_source(root/'src/frik_pose_probe.cpp')
 start=owner.index('int F4Rung178r10() noexcept {')
 rung=owner[start:owner.index('\n',start)]
 actual='#if defined(KCD2VR_IMPALE_OPTIONS_BODY_H41)\n'+body+'#else\n'+rung+'\n#endif\n'
 out=Path(sys.argv[2]);scope='Verbatim physical input option assignments and actual F4 rung accessor; accepted mode configuration/native leaves controlled.'
elif len(sys.argv)>1 and sys.argv[1]=='--living-c40':
 start=source.index('struct LivingImpaleC40 {')
 actual=source[start:source.index('// C47: the death ragdoll.',start)]
 out=Path(sys.argv[2]);scope='Verbatim evaluated-joint display attachment and current native namespace capture; only native/memory leaves mocked. No physics-part mapping.'
else:
 actual=source[source.index('static bool ReadImpaleCurrentCorpseH18('):]
 out=Path(sys.argv[1]);scope='Verbatim successor NEW contact/current typed owner/GetStatus adapter; only memory/native leaves mocked'
out.write_text(actual,encoding='utf-8')
out.with_suffix('.json').write_text(json.dumps(dict(source=source_path,sha256=hashlib.sha256(actual.encode()).hexdigest(),scope=scope),indent=2))
