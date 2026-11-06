import copy
import importlib.util
import math
import unittest
from pathlib import Path
spec=importlib.util.spec_from_file_location('cmp',Path(__file__).resolve().parents[1]/'tools/compare_finger_references_v6697.py')
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
class ReferenceReport(unittest.TestCase):
    def setUp(self):
        self.audit={'schema':3,'joints':[{'side':'R','joint':1,'parent':0,'offset':[0,0,.02],'relaxed':[0,0,0,1],'fist':[0,0,1,0]}]}
        self.native={'R_0':{'side':'R','category':0,'joints':[{'joint':1,'parent':0,'offset':[0,0,.02],'q_xyzw':[0,0,0,-1]}]}}
    def test_sign_equivalent(self):
        r=m.compare(self.audit,self.native)['groups'][0]['per_joint'][0]
        self.assertEqual(r['relaxed_angle_deg'],0)
        self.assertEqual(r['fist_angle_deg'],180)
    def test_missing_joint(self):
        self.native['R_0']['joints'][0]['joint']=2
        self.assertEqual(m.compare(self.audit,self.native)['groups'][0]['missing_joints'],[2])
    def test_parent_mismatch(self):
        self.native['R_0']['joints'][0]['parent']=3
        self.assertEqual(m.compare(self.audit,self.native)['groups'][0]['parent_mismatches'],[1])
    def test_nonfinite(self):
        self.audit['joints'][0]['fist'][0]=math.nan
        with self.assertRaises(ValueError):m.compare(self.audit,self.native)
    def test_duplicate(self):
        self.audit['joints'].append(copy.deepcopy(self.audit['joints'][0]))
        with self.assertRaises(ValueError):m.compare(self.audit,self.native)
if __name__=='__main__':unittest.main()
