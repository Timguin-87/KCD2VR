from pathlib import Path
import hashlib,json
root=Path(__file__).resolve().parents[1]
p=root/'src/interaction_native_selection68.inl';s=p.read_text()
a=s.index('static bool SelectionCopy68(')
# This fixture needs only bounded readers and the current-policy graph reader.
# Do not use the next unrelated function as an end marker: new collector
# instrumentation may be inserted between these actual production helpers.
policy=s.index('static bool SelectionPolicy68(',a)
opening=s.index('{',policy);depth=1;b=opening+1
while depth and b<len(s):
    if s[b]=='{':depth+=1
    elif s[b]=='}':depth-=1
    b+=1
assert depth==0,'SelectionPolicy68 body is incomplete'
part=s[a:b]+'\n'
assert 'SelectionPolicy68(' in part and 'DoorCollectorBeginC54(' not in part
(root/'tests/generated_door_origin_reads_h12.inc').write_text('// Generated exact bounded production reader/policy helpers.\n'+part)
(root/'tests/door_origin_h12_extraction.json').write_text(json.dumps({'source':str(p.relative_to(root)),'sha256':hashlib.sha256(part.encode()).hexdigest(),'production_decisions_rewritten':False},indent=2)+'\n')
