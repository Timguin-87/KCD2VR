"""Verbatim F7 annotations at the real natural skinning callback; no pose policy copy."""
from pathlib import Path
import hashlib, json, sys, re
from source_text import read_source_bytes
root=Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]) if len(sys.argv)>1 else root/'tests'
out.mkdir(parents=True,exist_ok=True)
source=root/'src/frik_pose_probe.cpp'
raw=read_source_bytes(source)
text=raw.decode().replace('\r\n','\n')
def between(start,end):
    assert text.count(start)==1,start
    lo=text.index(start);hi=text.index(end,lo)
    return text[lo:hi]
# Locate the one actual decision immediately surrounding BeginCoherent, rather
# than copying a historical predicate. XR34 adds a specifically owned working
# hand exception; the fixture must compile the CURRENT complete predicate.
starts=list(re.finditer(r'^    if\([^\r\n]+\)\n        \{PerfScope101R2 timing\(0\);committed=BeginCoherent6689\(',text,re.MULTILINE))
assert len(starts)==1,'unique actual BeginCoherent eligibility boundary'
decision=between(text[starts[0].start():text.index('\n',starts[0].start())],
                 '    if(!committed)g_committed_pose_valid6689.store(false,')
native=between('    {PerfScope101R2 timing(1);if(g_skinning_core_original){','    InvalidateReadCache101R2(); // native code may change mappings')
kickBegin=between('    auto kickOutput23=BeginKickOutput23(',text[starts[0].start():text.index('\n',starts[0].start())])
kick=between('    if(kickOutput23){KickOutputJoints23(*kickOutput23,1,b1fn);KickOutputPalette23(*kickOutput23,0,desc);',
             '    transaction.Restore();')
fingerBegin=between('    auto fingerOutput28=BeginFingerOutput28(', '    auto kickOutput23=BeginKickOutput23(')
fingerFinish=between('    if(fingerOutput28)FinishFingerOutput28(*fingerOutput28,transaction);','    if(rigOk&&rawOk&&output&&')
handAttachment=between('static void HandAttachmentReceipt28(void* self,void* attachment,std::uint64_t now) noexcept {','static void TraceToolItem28(const CommittedFrame6695& body')
items={'hand_attachment_observer28.inc':handAttachment,'finger_output_begin28.inc':fingerBegin,'finger_output_finish28.inc':fingerFinish,'vrik_callback_decision_h29r4.inc':decision,'vrik_callback_original_h29r4.inc':native,
       'kick_output_phases23.inc':kick,'kick_output_begin23.inc':kickBegin}
for name,body in items.items():
    (out/name).write_text('// Generated verbatim current production; do not edit.\n'+body,encoding='utf-8')
(out/'vrik_callback_h29r4_provenance.json').write_text(json.dumps({
 'source':str(source.relative_to(root)), 'source_sha256':hashlib.sha256(raw).hexdigest(),
 'extracted':{name:hashlib.sha256(body.encode()).hexdigest() for name,body in items.items()},
 'scope':'Actual Begin eligibility, copied F7 skip annotation, original call/return annotation. Fixture explicitly supplies callback-local copied values and native original; no retail execution.'
},indent=2)+'\n',encoding='utf-8')
print('F7: extracted actual callback eligibility and original-call annotations')
