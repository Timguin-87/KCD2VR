"""Copy actual render decision for coherent-pose->render publication replay."""
from pathlib import Path
import hashlib,json,sys
root=Path(sys.argv[2]) if len(sys.argv)>2 else Path(__file__).resolve().parents[1]
out=Path(sys.argv[1]);out.mkdir(parents=True,exist_ok=True)
s=(root/'src/vrik/render_bounds63.inl').read_text(encoding='utf-8-sig');lo=s.index('struct BoundsNote63 ');hi=s.index('static void TraceHenryBounds63(',lo)
(out/'hand_render_owner_h5_actual.inc').write_text(s[lo:hi],encoding='utf-8')
inputs=[root/'src/vrik/coherent_pose.inl',root/'src/vrik/render_bounds63.inl',root/'src/vrik/render_bounds63.h',root/'src/vrik/tracking_grace111.h',root/'tests/pose_adapter_test.cpp',Path(__file__).resolve()]
(out/'hand_render_owner_h5_provenance.json').write_text(json.dumps({'source_sha256':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in inputs},'scope':'Actual coherent pose/in-memory native leaves plus unchanged real tracking producer grace helper and verbatim renderer decision. No GPU/native execution.'},indent=2)+'\n')
