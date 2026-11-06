from pathlib import Path
import zipfile,xml.etree.ElementTree as ET,hashlib,json,copy
def patch_dialogue(data):
 root=ET.fromstring(data)
 bt=next(x for x in root.findall('BehaviorTree') if x.get('name')=='speech_getParametersFromPreset')
 variables=bt.find('Variables');assert variables is not None
 ET.SubElement(variables,'Variable',{'name':'kcd2vrNoCameras','type':'_bool','values':'false','isPersistent':'0','form':'single'})
 behavior=bt.find('Root/Behavior');assert behavior is not None and len(behavior)==1
 old=behavior[0];behavior.remove(old)
 sequence=ET.SubElement(behavior,'Sequence');sequence.append(old)
 # Retail ExecuteLua maps declared AI variables through data; stock so_door.xml uses the same CVar API/boolean assignment.
 ET.SubElement(sequence,'ExecuteLua',{'code':"data.kcd2vrNoCameras = (System.GetCVar('wh_dlg_NoCameras') == 1)"})
 gate=ET.SubElement(sequence,'IfCondition',{'failOnCondition':'false','condition':'$kcd2vrNoCameras and $preset==$enum:dialogPreset.fader'})
 ET.SubElement(gate,'Expression',{'expressions':'$dialogParameters.useTwinsAsInitiator = false\n$dialogParameters.movePlayer = false\n$dialogParameters.rotatePlayer = false'})
 # Editor tree mirrors the native runtime node hierarchy; editor-only metadata does not execute.
 ed=bt.find('EditorData')
 if ed is not None:
  ev=ed.find('Variables')
  if ev is not None:ET.SubElement(ev,'Variable',{'name':'kcd2vrNoCameras','comment':'NoCameras uses stock original-actor dialogue path.'})
  eb=ed.find('Root/Behavior')
  if eb is not None and len(eb)==1:
   previous=eb[0];eb.remove(previous);sq=ET.SubElement(eb,'Sequence');sq.append(previous)
   ET.SubElement(sq,'ExecuteLua');cond=ET.SubElement(sq,'IfCondition');ET.SubElement(cond,'Expression')
 return ET.tostring(root,encoding='us-ascii',xml_declaration=True)
def build(installed,scripts,out):
 key='AI/speech/getparametersfrompreset.xml'
 with zipfile.ZipFile(scripts) as z:before=z.read(key)
 patched=patch_dialogue(before)
 out.parent.mkdir(parents=True,exist_ok=True)
 with zipfile.ZipFile(installed) as old,zipfile.ZipFile(out,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
  copied={}
  for name in old.namelist():
   if name==key:continue
   data=old.read(name);z.writestr(name,data);copied[name]=hashlib.sha256(data).hexdigest()
  z.writestr(key,patched)
 report={'base_pak':str(installed),'base_sha256':hashlib.sha256(Path(installed).read_bytes()).hexdigest(),
  'preserved_entries':copied,'changed':{key:{'input_sha256':hashlib.sha256(before).hexdigest(),'output_sha256':hashlib.sha256(patched).hexdigest()}},
  'condition':'wh_dlg_NoCameras == 1 AND preset=fader',
  'scope':'stock RequestDialog UseTwins=false; movePlayer=false; rotatePlayer=false. No quest/cutscene/loading/death asset edits.',
  'native_bt_compile_and_headset_acceptance':'NOT_RUN'}
 out.with_suffix('.json').write_text(json.dumps(report,indent=2)+'\n')
 return report
if __name__=='__main__':
 import argparse
 ap=argparse.ArgumentParser();ap.add_argument('installed',type=Path);ap.add_argument('scripts',type=Path);ap.add_argument('out',type=Path);a=ap.parse_args()
 print(json.dumps(build(a.installed,a.scripts,a.out),indent=2))
