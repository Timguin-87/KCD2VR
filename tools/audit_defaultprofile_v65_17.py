#!/usr/bin/env python3
from pathlib import Path
import argparse,zipfile,re,hashlib,sys
ap=argparse.ArgumentParser(); ap.add_argument('pak',type=Path); a=ap.parse_args()
raw=a.pak.read_bytes(); print('controller_rebind SHA256:',hashlib.sha256(raw).hexdigest())
with zipfile.ZipFile(a.pak) as z:
    data=z.read('Libs/Config/defaultProfile.xml').decode('utf-8','ignore')
checks={
 'sprint xi_thumbl': r'name="sprint"[^>]*xboxpad="xi_thumbl"',
 'jump xi_b': r'name="jump"[^>]*xboxpad="xi_b"',
 'talk xi_a': r'name="talk"[^>]*xboxpad="xi_a"',
 'use/hold xi_a': r'name="use"[^>]*onHold="1"[^>]*xboxpad="xi_a"',
 'call xi_y': r'name="call"[^>]*xboxpad="xi_y"',
 'horse/hold xi_y': r'name="use_horse"[^>]*onHold="1"[^>]*xboxpad="xi_y"',
 'block xi_triggerl_btn': r'name="block"[^>]*xboxpad="xi_triggerl_btn"',
 'inventory xi_back': r'name="open_apse_inventory_controller"[^>]*xboxpad="xi_back"',
 'map xi_dpad_down': r'name="open_apse_map_controller"[^>]*xboxpad="xi_dpad_down"',
 'skip time xi_dpad_up': r'name="open_skiptime"[^>]*xboxpad="xi_dpad_up"',
 'crouch xi_thumbr': r'name="toggle_crouch"[^>]*xboxpad="xi_thumbr"',
 'helmet hold xi_dpad_up 0.5s': r'name="xi_toggle_helmet"[^>]*onHold="1"[^>]*holdTriggerDelay="0.5"[^>]*xboxpad="xi_dpad_up"',
 'torch hold xi_dpad_down 0.5s': r'name="xi_toggle_torch"[^>]*onHold="1"[^>]*holdTriggerDelay="0.5"[^>]*xboxpad="xi_dpad_down"',
 'pause xi_start': r'name="open_pause_menu"[^>]*xboxpad="xi_start"',
}
bad=[]
for n,p in checks.items():
    ok=re.search(p,data,re.I|re.S) is not None
    print(('PASS' if ok else 'FAIL')+': '+n)
    if not ok: bad.append(n)
print(f'RESULT: {len(checks)-len(bad)}/{len(checks)} passed')
sys.exit(1 if bad else 0)
