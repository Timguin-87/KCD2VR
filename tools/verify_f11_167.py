"""Static F11 retail input/zone evidence. Reads files; never loads native code."""
import argparse, hashlib, json, zipfile
from pathlib import Path
from xml.etree import ElementTree as ET
from verify_input117 import verify

def main():
 p=argparse.ArgumentParser();p.add_argument('--game',type=Path,required=True);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
 native=verify(a.game/'Bin/Win64MasterMasterSteamPGO/WHGame.dll')
 archive=a.game/'Data/Tables.pak';tables={}
 with zipfile.ZipFile(archive) as z:
  for leaf in ['combat_zone.xml','combat_attack_config.xml','combat_input_class.xml']:
   name=next(n for n in z.namelist() if n.endswith('/'+leaf));data=z.read(name)
   tables[leaf]=dict(member=name,sha256=hashlib.sha256(data).hexdigest(),rows=[e.attrib for e in ET.fromstring(data).iter() if e.attrib])
 zones={int(r['combat_zone_id']):r for r in tables['combat_zone.xml']['rows'] if 'combat_zone_id' in r}
 for z,opposite in {0:5,5:0,1:2,2:1}.items():assert int(zones[z]['master_strike_combat_zone_id'])==opposite
 assert all(float(zones[z]['radius_to'])==0 for z in [3,4])
 light={int(r['combat_zone_id']) for r in tables['combat_attack_config.xml']['rows'] if r.get('combat_input_class_id')=='0'}
 assert light=={0,5},light
 result=dict(native=native,tables_path=str(archive),tables=tables,light_input_configured_zones=sorted(light),
  scope='Static data and exact retail input contracts; no runtime hit or master-strike acceptance proof.',
  consequence='F11=2 supplies chest-origin guard 0/1/2/5 through 90F824 while preserving light input at both D4AAC4/D4A99C edges. Native executed zone/type may normalize. No lateral-thrust recipe or forced master strike is added.')
 a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8');print('PASS retail input pins, four guard zones/opposites and ordinary light-input table limits')
if __name__=='__main__':main()
