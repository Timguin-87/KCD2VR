"""R153: the hold to look / watch prompt (infinite hold interaction) on X (PS Square) as well as LB / L1 in the companion
profile (R155 tried LB first / X second; R156: X first, LB second again - that order showed the prompt). every other profile action is unchanged (kick, tabs, pages, map, chat follow; R157: Follow is the retail LB-only row again)."""
from pathlib import Path
import subprocess,unittest,xml.etree.ElementTree as E
ROOT=Path(__file__).resolve().parents[1]
PROFILE=ROOT/'companion/source/Libs/Config/defaultProfile.xml'
def inputs(a,dev):
    return ([a.get(dev)] if a.get(dev) else [])+[i.get('input') for i in a.findall('./%s/inputdata'%dev)]
def actions(root):
    return {(m.get('name'),a.get('name')):a for m in root.iter('actionmap') for a in m.findall('action')}
HOLD={('infinite_hold_interaction_init','infinite_hold_interaction_init_press'):{'onPress':'1'},
      ('infinite_hold_interaction_inprogress','infinite_hold_interaction_inprogress_release'):{'onPress':'1','onRelease':'1'}}
class LbPromptX153(unittest.TestCase):
    def setUp(self):
        self.acts=actions(E.fromstring(PROFILE.read_bytes()))
    def test_hold_prompt_on_x_and_lb(self):
        for k,flags in HOLD.items():
            a=self.acts[k]
            self.assertEqual(inputs(a,'xboxpad'),['xi_x','xi_shoulderl'],k)   # R156: X first, LB second again (see the profile comment)
            self.assertEqual(inputs(a,'pspad'),['pad_square','pad_l1'],k)
            self.assertEqual(a.get('keyboard'),'_keybinds_ref_',k)
            for f,v in flags.items():self.assertEqual(a.get(f),v,k)
            self.assertIsNone(a.get('onHold'),k)
    def test_in_progress_map_has_no_other_x(self):
        # exclusive map: X must not mean anything else while the interaction runs
        m=[m for m in E.fromstring(PROFILE.read_bytes()).iter('actionmap') if m.get('name')=='infinite_hold_interaction_inprogress'][0]
        self.assertEqual([a.get('name') for a in m.findall('action')],['infinite_hold_interaction_inprogress_release'])
    def test_other_actions_unchanged(self):
        # R157: compared with the R150 (pre-R151) profile - the hold rows are the ONLY change left in the file
        base=subprocess.run(['git','-C',str(ROOT),'show','f252355:companion/source/Libs/Config/defaultProfile.xml'],capture_output=True)
        if base.returncode:self.skipTest('base commit not in this checkout')
        old=actions(E.fromstring(base.stdout));new=self.acts
        self.assertEqual(set(new),set(old))
        for k,a in old.items():
            if k not in HOLD:self.assertEqual(E.tostring(new[k]),E.tostring(a),k)
        # left alone on purpose: kick/special LB, menu tabs, reading pages, map zoom, chat follow (X = 3rd chat choice there)
        self.assertEqual(inputs(new[('combat','attack_special')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('reading_base','read_prev')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('chat_focus_follow_init','chat_focus_follow_init_press')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('chat_interaction','chat_third_controller')],'xboxpad'),['xi_x'])
        self.assertEqual(inputs(new[('focus_follow_init_interactor','focus_follow_init_interactor_press')],'xboxpad'),['xi_shoulderl'])   # R157: retail Follow row (X tap -> LB is mod-side)
if __name__=='__main__':unittest.main()
