"""Extract complete production D56 publication/read paths for host native-leaf fixtures."""
import argparse
import hashlib
import json
from pathlib import Path
from source_text import read_source
ROOT=Path(__file__).resolve().parents[1]
def span(source,start,end):
    if source.count(start)!=1:raise ValueError('ambiguous production start: '+start)
    a=source.index(start);b=source.index(end,a)
    return source[a:b]
def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--out',type=Path,required=True);args=p.parse_args()
    path=ROOT/'src/action_time_owner59c11.cpp';source=read_source(path);args.out.mkdir(parents=True,exist_ok=True)
    parts={
        'generated_acquisition_owner_d56.inc':span(source,'enum State : int {Idle=0,Armed=1,Playing=2,Held=3,Released=4,Cancelling=5,PendingAttach=6};','static bool CurrentOwnerInput55('),
        'generated_acquisition_functions_d56.inc':'\n'.join([
            span(source,'static void RetireUse55(','struct PendingPhysicalPickup63 '),
            span(source,'static void PublishAcquisition54() noexcept {','void Publish() noexcept {'),
            span(source,'bool ReadAcquisition54(grasp13::Acquisition54& out) noexcept {','bool ReadGraspBinding(')])}
    for name,body in parts.items():(args.out/name).write_text('// Current complete production functions; generated, never hand-edited.\n'+body,encoding='utf-8')
    (args.out/'ACQUISITION_EXTRACTION_D56.json').write_text(json.dumps({'production_file':str(path.relative_to(ROOT)),
        'source_sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'extracted_sha256':{n:hashlib.sha256(b.encode()).hexdigest() for n,b in parts.items()},
        'limits':'Actual publication and both readers; native capability, owner-thread/current-root, and attachment-publication leaves are mocked; not a native queue/Attach/inventory test.'},indent=2)+'\n')
if __name__=='__main__':main()
