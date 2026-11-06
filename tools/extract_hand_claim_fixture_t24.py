"""Extract the exact production claim helper for standalone owner fixtures.
Native vft/backlink reads are explicit typed fixture leaves; policy is real.
"""
from pathlib import Path
import argparse,hashlib,json
ap=argparse.ArgumentParser();ap.add_argument('--source',type=Path,default=Path(__file__).resolve().parents[1]);ap.add_argument('--output',type=Path);a=ap.parse_args();root=a.source;out=a.output or root/'tests';out.mkdir(exist_ok=True)
p=root/'src/frik_pose_probe.cpp';s=p.read_text();anchor='static bool HandClaimBlockedT24(int side,std::uintptr_t character,std::uint64_t body,std::uint64_t request) noexcept {'
assert s.count(anchor)==1
start=s.index(anchor);begin=s.index('{',start);depth=1;end=begin+1
while depth:depth+=(s[end]=='{')-(s[end]=='}');end+=1
body=s[start:end]+'\n'
assert 'hand_claim_t24::competing(side,owner)' in body
(out/'generated_hand_claim_t24.inc').write_text(body)
(out/'generated_hand_claim_t24.provenance.json').write_text(json.dumps({'source':str(p.relative_to(root)),'sourceSHA256':hashlib.sha256(p.read_bytes()).hexdigest(),'helperSHA256':hashlib.sha256(body.encode()).hexdigest(),'scope':'EXACT private production helper; copied claim mailbox/classifier remain real. Native current-player/vft/backlink memory reads are explicit fixture leaves. No native caller, cleanup, hand-key write or game execution.'},indent=2)+'\n')
