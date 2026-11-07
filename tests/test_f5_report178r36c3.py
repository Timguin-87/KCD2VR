import importlib.util
from pathlib import Path
import unittest
spec=importlib.util.spec_from_file_location('f5report',Path(__file__).resolve().parents[1]/'tools/report_f5_178r36c3.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)

class F5Report36C3(unittest.TestCase):
    def begin(self):
        return ['F5_CAPTURE session=1 ready=1']+['F5_GROUP session=1 group='+g+' ready=1' for g in ('PICKUP','HAND_CHANGE','ALCHEMY','REMOVAL','CONSTRAINT','PHYSIC_PARAMS')]
    def event(self,kind,tick,extra=''):
        return f' F8_LIFECYCLE session=1 event={kind} tick={tick} {extra}'
    def report(self,lines):
        return m.summarize([' '+line+' ' for line in lines])['sessions'][0]
    def test_grip_release_is_not_physical_release(self):
        s=self.report(self.begin()+[self.event('HAND_ENTER',100,'side=0 wuid=77'),self.event('GRIP_SAMPLE',150,'side=0 reason=2'),self.event('HAND_LEAVE',350,'side=0 wuid=77'),'F8_LIFECYCLE_END session=1 dropped=0'])
        self.assertTrue(s['transport_complete'])
        self.assertEqual(s['items'][0]['native_hand_intervals'][0]['duration_ms'],250)
        self.assertEqual(s['items'][0]['physical_hold'],'UNPROVEN')
    def test_physics_return_separate_from_script_wrapper(self):
        identity='call=9 body=ABC entity=DEF target=7 actionRaw=11 constraintId=45'
        s=self.report(self.begin()+[self.event('PHYSICS_SUBMIT',100,identity+' valid=1'),self.event('PHYSICS_ACTION_RETURN',101,identity+' resultRaw=0'),self.event('SET_PHYSIC_PARAMS_RETURN',102,identity+' resultRaw=1'),'F8_LIFECYCLE_END session=1'])
        self.assertEqual(s['physics_calls'][0]['native_return'],'0')
        self.assertEqual(s['physics_calls'][0]['wrapper_returns'],['1'])
        self.assertNotEqual(s['physics_calls'][0]['cleanup'],'SUCCESS')
    def test_mismatched_identity_does_not_join_return(self):
        s=self.report(self.begin()+[self.event('PHYSICS_SUBMIT',100,'call=9 body=A entity=X target=1 actionRaw=11'),self.event('PHYSICS_ACTION_RETURN',101,'call=9 body=B entity=X target=1 actionRaw=11 resultRaw=1')])
        self.assertIsNone(s['physics_calls'][0]['native_return'])
        self.assertFalse(s['transport_complete'])
    def test_loss_or_missing_group_invalidates_completeness(self):
        for ending in ['F8_LIFECYCLE_END session=1 dropped=1','F8_LIFECYCLE_END session=1 scopeLost=1','F8_LIFECYCLE_END session=1 budgetLost=1']:
            self.assertFalse(self.report(self.begin()+[ending])['transport_complete'])
        self.assertFalse(self.report(self.begin()[:-1]+['F8_LIFECYCLE_END session=1'])['transport_complete'])
    def test_two_entities_same_key_are_not_silently_one_lifetime(self):
        s=self.report(self.begin()+[self.event('HAND_SAMPLE',100,'wuid=77 entity=A target=1'),self.event('HAND_SAMPLE',200,'wuid=77 entity=B target=2')])
        self.assertTrue(s['items'][0]['identity_ambiguous'])
    def test_sessions_and_open_hold_not_completed(self):
        s=self.report(self.begin()+[self.event('HAND_ENTER',100,'side=0 wuid=77'),'F8_LIFECYCLE session=2 event=HAND_LEAVE tick=200 side=0 wuid=77','F8_LIFECYCLE_END session=1'])
        self.assertIsNone(s['items'][0]['native_hand_intervals'][0]['end'])

if __name__=='__main__':unittest.main()
