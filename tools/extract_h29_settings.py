from pathlib import Path
import hashlib,json
r=Path(__file__).resolve().parents[1];p=r/'src/mod.cpp';lines=p.read_text().splitlines()
def capture(start,end):
 first=next(i for i,s in enumerate(lines) if start in s)
 last=next(i for i in range(first,len(lines)) if end in lines[i])
 return '\n'.join(lines[first:last+1]),[first+1,last+1]
fill,f=capture('out.melee_types_c54[c]=melee_tuning_c54::read','H31_FINGER_FILL')
apply,a=capture('set(int(c),state.melee_types_c54[c])','H31_FINGER_APPLY')
save,s=capture('const auto v=melee_tuning_c54::read(int(c));char key[64]','H31_FINGER_SAVE')
load,l=capture('for(unsigned c=0;c<17;++c){auto v=melee_tuning_c54::read(int(c));char key[64]','H31_FINGER_LOAD')
first=next(i for i,s in enumerate(lines) if s.startswith('float ReadIniFloatExplicit('))
last=next(i for i in range(first+1,len(lines)) if lines[i]=='}')
reader='\n'.join(lines[first:last+1])
text='// Generated from ACTUAL current mod.cpp settings blocks. Regenerate; do not edit.\n'
text+=reader+'\n'
text+='void H29Fill(State& out){\n'+fill+'\n}\nvoid H29Apply(const State& state){\n'+apply+'\n}\nbool H29Save(){bool ok=true;\n'+save+'\nreturn ok;}\nvoid H29Load(){const char* explicit_ini="fixture";\n'+load+'\n}\n'
out=r/'tests/generated/h29_settings.inl';out.parent.mkdir(parents=True,exist_ok=True);out.write_text(text,newline='\n')
(r/'tests/generated/h29_settings.provenance.json').write_text(json.dumps({'source':'src/mod.cpp','sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'blocks':{'fill':f,'apply':a,'save':s,'load':l},'extracted_sha256':hashlib.sha256(out.read_bytes()).hexdigest()},indent=2)+'\n')
print('Extracted actual H29 per-class cut/stab, punch, block and finger settings')
