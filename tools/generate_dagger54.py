"""Embed only exact retail class0 Model rows and their resolved ItemAlias rows."""
from pathlib import Path
from xml.etree import ElementTree as E
import hashlib,json,sys
base=Path(sys.argv[1])
repo=Path(__file__).resolve().parents[1]
rows={};sources={}
for p in sorted(base.glob('item*.xml')):
 try:r=E.parse(p).getroot()
 except E.ParseError:continue
 for x in r.iter():
  if x.tag=='MeleeWeapon' and x.get('Class')=='0' and x.get('Model'):
   rows[x.get('Id').lower()]='objects/'+x.get('Model').replace('\\','/').lower()
   sources[p.name]=hashlib.sha256(p.read_bytes()).hexdigest()
# Aliases inherit the actual retail source Model; never infer by a name containing dagger.
for p in sorted(base.glob('item*.xml')):
 try:r=E.parse(p).getroot()
 except E.ParseError:continue
 for x in r.iter('ItemAlias'):
  src=x.get('SourceItemId','').lower()
  if src in rows:
   rows[x.get('Id').lower()]=rows[src]
   sources[p.name]=hashlib.sha256(p.read_bytes()).hexdigest()
lua=(repo/'src/vrik/dagger_chest54.lua').read_text()
model='{'+','.join("['%s']='%s'"%(k,v) for k,v in sorted(rows.items()))+'}'
lua=lua.replace('__MODELS54__',model)
(repo/'src/vrik/dagger_chest54_lua.inc').write_text('R"DAGGER54(\n'+lua+')DAGGER54"\n',encoding='utf-8')
(repo/'tests/dagger_chest54_provenance.json').write_text(json.dumps({'retail_tables':sources,'models':rows,'lua_sha256':hashlib.sha256(lua.encode()).hexdigest()},indent=2)+'\n')
print('embedded_dagger_models',len(rows))
