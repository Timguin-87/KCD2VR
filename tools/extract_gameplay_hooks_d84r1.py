from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
s=(root/'src/diagnostics/interaction_lifecycle165.inl').read_text()
a=s.index('static bool ArmLifecycle165(');b=s.index('{',a)+1;depth=1
while depth:
 depth+=(s[b]=='{')-(s[b]=='}');b+=1
out=Path(sys.argv[1]);out.parent.mkdir(parents=True,exist_ok=True);out.write_text(s[a:b])
