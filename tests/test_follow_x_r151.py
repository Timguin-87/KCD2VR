"""R151: Follow prompt on X (PS Square) in the companion profile, LB kept second; X Quick talk prompt hidden while
Follow is offered; reply case keeps the old LB-only Follow. Every other profile action is unchanged."""
from pathlib import Path
import re,subprocess,unittest,xml.etree.ElementTree as E
ROOT=Path(__file__).resolve().parents[1]
PROFILE=ROOT/'companion/source/Libs/Config/defaultProfile.xml'
LUA=ROOT/'companion/source/Scripts/Entities/AI/Shared/BasicAIActions.lua'
def inputs(a,dev):
    return ([a.get(dev)] if a.get(dev) else [])+[i.get('input') for i in a.findall('./%s/inputdata'%dev)]
def actions(root):
    return {(m.get('name'),a.get('name')):a for m in root.iter('actionmap') for a in m.findall('action')}
class FollowX151(unittest.TestCase):
    def setUp(self):
        self.root=E.fromstring(PROFILE.read_bytes());self.acts=actions(self.root)
    def test_follow_binding(self):
        f=self.acts[('focus_follow_init_interactor','focus_follow_init_interactor_press')]
        self.assertEqual(inputs(f,'xboxpad'),['xi_x','xi_shoulderl'])   # X first = shown glyph
        self.assertEqual(inputs(f,'pspad'),['pad_square','pad_l1'])
        lb=self.acts[('focus_follow_init_interactor','kcd2vr_follow_lb_press')]
        self.assertEqual(inputs(lb,'xboxpad'),['xi_shoulderl']);self.assertEqual(inputs(lb,'pspad'),['pad_l1'])
    def test_no_shared_button_between_listed_npc_prompts(self):
        # the rows GetChatActions can list together (BasicAIActions.lua), per case
        cases={'init+follow':[('focus_follow_init_interactor','focus_follow_init_interactor_press'),('chat_init_interactor','kcd2vr_chat_init_gesture')],
               'reply+follow':[('focus_follow_init_interactor','kcd2vr_follow_lb_press'),('chat_init_interactor','chat_init_interactor_press')],
               'init only':[('chat_init_interactor','chat_init_with_focus'),('chat_init_interactor','kcd2vr_chat_init_gesture')]}
        for name,rows in cases.items():
            seen=[set(inputs(self.acts[r],'xboxpad')) for r in rows]
            self.assertFalse(seen[0]&seen[1],name)
    def test_other_actions_unchanged(self):
        base=subprocess.run(['git','-C',str(ROOT),'show','f252355:companion/source/Libs/Config/defaultProfile.xml'],capture_output=True)
        if base.returncode:self.skipTest('base commit not in this checkout')
        old=actions(E.fromstring(base.stdout));new=self.acts
        changed={('focus_follow_init_interactor','focus_follow_init_interactor_press')}
        self.assertEqual(set(new)-set(old),{('focus_follow_init_interactor','kcd2vr_follow_lb_press')})
        for k,a in old.items():
            if k not in changed:self.assertEqual(E.tostring(new[k]),E.tostring(a),k)
        # the proven ones stay: quick talk X, kick LB, reading pages, menu tabs
        self.assertEqual(inputs(new[('chat_init_interactor','chat_init_with_focus')],'xboxpad'),['xi_x'])
        self.assertEqual(inputs(new[('combat','attack_special')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('reading_base','read_prev')],'xboxpad'),['xi_shoulderl'])
        self.assertEqual(inputs(new[('apse_tabs','tab_next')],'xboxpad'),['xi_shoulderr'])
    def test_lua_guards(self):
        s=LUA.read_text()
        self.assertIn('if showReplyHint then followAction = "kcd2vr_follow_lb_press" end',s)
        self.assertRegex(s,r'if not canFollow then\n\t\tAddInteractorAction\(output, false, Action\(\):hint\(chatHint\):action\("chat_init_with_focus"\)')
        self.assertEqual(s.count('action("kcd2vr_chat_init_gesture")'),1)   # pointing quick talk always offered
        body=s[s.index('function BasicAIActions:GetChatActions'):s.index('function BasicAIActions:OnTalk')]
        self.assertEqual(len(re.findall(r'\b(if|function)\b',body)),len(re.findall(r'\bend\b',body)))  # blocks closed
if __name__=='__main__':unittest.main()
